#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/mman.h>

#define MAX_ORDER 10
#define INITIAL_BLOCKS_AMOUNT 32
#define MAX_BLOCK_SIZE 1024*128

class MallocMetadata {
public:
    int cookie;
    size_t size;
    MallocMetadata* freeListNext;
    MallocMetadata* freeListPrev;
};

unsigned int getMatchingIndex(size_t size);

unsigned int getMatchingFirstIndex(size_t size);

bool isAdjacent(const MallocMetadata *secondBlock, const MallocMetadata *firstBlock);

bool isMergedBlockSizeOk(const MallocMetadata *block);

MallocMetadata* freeListsArray[MAX_ORDER + 1] = {NULL};
bool didWeAllocate = false;
size_t num_allocated_blocks = 0;
size_t num_allocated_bytes = 0;

size_t _size_meta_data() {
    return sizeof(MallocMetadata);
}

void _num_free_blocks_unique_action(MallocMetadata* temp, size_t& num_free_blocks) {
    num_free_blocks++;
}

void _num_free_bytes_unique_action(MallocMetadata* temp, size_t& num_free_bytes) {
    num_free_bytes+=temp->size;
}

void _num_allocated_blocks_unique_action(MallocMetadata* temp, size_t& num_allocated_blocks) {
    num_allocated_blocks++;
}

void _num_allocated_bytes_unique_action(MallocMetadata* temp, size_t& num_allocated_bytes) {
    num_allocated_bytes+=temp->size;
}

//void _num_meta_data_bytes_unique_action(MallocMetadata* temp, size_t& num_meta_data_bytes) {
//    num_meta_data_bytes += _size_meta_data();
//}

size_t _generic_function_for_all_methods(void (*unique_action)(MallocMetadata*, size_t&)) {
    size_t num = 0;
    for (unsigned int i = 0; i <= MAX_ORDER; i++) {
        MallocMetadata* temp = freeListsArray[i];
        while (temp) {
            unique_action(temp, num);
            temp = temp->freeListNext;
        }
    }

    return num;
}

size_t _num_free_blocks() {
    return _generic_function_for_all_methods(_num_free_blocks_unique_action);
}

size_t _num_free_bytes() {
    return _generic_function_for_all_methods(_num_free_bytes_unique_action);
}

size_t _num_allocated_blocks () {
    return num_allocated_blocks;
}

size_t _num_allocated_bytes () {
    return num_allocated_bytes;
}

size_t _num_meta_data_bytes() {
//    return _generic_function_for_all_methods(_num_meta_data_bytes_unique_action);
    return 0;
}

bool initialAllocation() {
    int cookiesForAll = rand();
    if (sbrk(0) && (unsigned long)sbrk(0) % (INITIAL_BLOCKS_AMOUNT * MAX_BLOCK_SIZE) > 0) {
        sbrk((unsigned long)sbrk(0) % (INITIAL_BLOCKS_AMOUNT * MAX_BLOCK_SIZE));
    }
    for (unsigned int i = 0; i < INITIAL_BLOCKS_AMOUNT; i++) {
        void* sbrk_result = sbrk(MAX_BLOCK_SIZE);
        if (!sbrk_result) {
            return false;
        }
        MallocMetadata* temp = freeListsArray[MAX_ORDER];
        MallocMetadata* new_node = (MallocMetadata*)sbrk_result;
        new_node->size = MAX_BLOCK_SIZE - _size_meta_data();
        new_node->cookie = cookiesForAll;
        new_node->freeListNext = NULL;
        while (temp && temp->freeListNext) {
            temp = temp->freeListNext;
        }
        if (!temp) {
            freeListsArray[MAX_ORDER] = new_node;
            new_node->freeListPrev = NULL;
        }
        else {
            temp->freeListNext = new_node;
            new_node->freeListPrev = temp;
        }
    }
    return true;
}

MallocMetadata* insertToFreeListAt(unsigned int index, void *beginning_address, int cookie) {
    MallocMetadata* temp = freeListsArray[index];
    MallocMetadata* new_node = (MallocMetadata*)beginning_address;
    new_node->size = (128 << index) - _size_meta_data();
    new_node->cookie = cookie;
    if (!temp) {
        new_node->freeListPrev=NULL;
        new_node->freeListNext=NULL;
        freeListsArray[index] = new_node;
        return new_node;
    }
    MallocMetadata* prev = temp->freeListPrev;
    while(temp && (unsigned long)beginning_address > (unsigned long)temp) {
        prev = temp;
        temp = temp->freeListNext;
    }
    if (prev) {
        prev->freeListNext = new_node;
    }
    else {
        freeListsArray[index] = new_node;
    }
    temp->freeListPrev = new_node;
    new_node->freeListPrev = prev;
    new_node->freeListNext = temp;
    return new_node;
}

void* allocateFromFreeList(size_t size) {
    unsigned int power_of_2 = getMatchingIndex(size);
    if (power_of_2 > MAX_ORDER || !freeListsArray[power_of_2]) {
        return NULL;
    }
    MallocMetadata* availableBlock = freeListsArray[power_of_2];
    freeListsArray[power_of_2]->freeListNext->freeListPrev = NULL;
    freeListsArray[power_of_2] = freeListsArray[power_of_2]->freeListNext;
    while ((unsigned long)128 << (power_of_2-1) > size+_size_meta_data()) {
        availableBlock->size -= (128 << (power_of_2-1));
        insertToFreeListAt(power_of_2 - 1, (void *) ((unsigned long) availableBlock + (128 << (power_of_2 - 1))), availableBlock->cookie);
        power_of_2--;
    }
    return (void*)((unsigned long)availableBlock + _size_meta_data());
}

void* smalloc(size_t size){
    if (size == 0 || size > 100000000) {
        return NULL;
    }
    if (!didWeAllocate) {
        didWeAllocate = true;
        if (!initialAllocation()) {
            return NULL;
        }
    }
    if (size > MAX_BLOCK_SIZE - _size_meta_data()) {
        size_t allocation_size = size/4096;
        allocation_size*=4096;
        if(size % 4096 > 0) {
            allocation_size+= 4096;
        }
        void* mmap_res = mmap(NULL,allocation_size,PROT_READ | PROT_WRITE, MAP_ANONYMOUS,-1,0);
        if (mmap_res == (void *) -1) {
            return NULL;
        }
        num_allocated_blocks++;
        num_allocated_bytes += size;
        return mmap_res;
    }
    void* allocated_address = allocateFromFreeList(size);
    num_allocated_blocks++;
    num_allocated_bytes += size;
    return allocated_address;
}

unsigned int getMatchingIndex(size_t size) {
    unsigned int power_of_2 = getMatchingFirstIndex(size);
    while (!freeListsArray[power_of_2]) {
        power_of_2++;
    }
    return power_of_2;
}

unsigned int getMatchingFirstIndex(size_t size) {
    unsigned int power_of_2 = 0;
    while (power_of_2 < MAX_ORDER && ((unsigned long)128 << power_of_2) < (size + _size_meta_data())) {
        power_of_2++;
    }
    return power_of_2;
}


void* scalloc(size_t num, size_t size) {
    void* tozeroall= smalloc(num*size);
    if (tozeroall) {
        std::memset(tozeroall,0,num*size);
    }
    return tozeroall;
}

MallocMetadata* mergeBlocks (MallocMetadata* firstBlock, MallocMetadata* secondBlock) {
    firstBlock->size += secondBlock->size + _size_meta_data();
    //in case the first block is on the beginning of the list
    if(!firstBlock->freeListPrev) {
        freeListsArray[getMatchingIndex(firstBlock->size)] = secondBlock->freeListNext;
    }
    else {
        firstBlock->freeListPrev->freeListNext = secondBlock->freeListNext;
    }
    //if the second block is not the latest we need to update the next block's prev
    if (secondBlock->freeListNext) {
        secondBlock->freeListNext->freeListPrev = firstBlock->freeListPrev;
    }
    return insertToFreeListAt(getMatchingIndex(firstBlock->size)+1, firstBlock, firstBlock->cookie);
}

MallocMetadata* checkAndMerge(MallocMetadata* firstBlock, MallocMetadata* secondBlock) {
    MallocMetadata* toReturn = NULL;
    while (secondBlock && isAdjacent(secondBlock, firstBlock) &&
           isMergedBlockSizeOk(firstBlock)) {
        toReturn = mergeBlocks(firstBlock, secondBlock);
    }
    return toReturn;
}

MallocMetadata* checkAndMergeFromBack(MallocMetadata *pMetaData) {
    if (pMetaData && pMetaData->freeListPrev) {
        return checkAndMerge(pMetaData->freeListPrev, pMetaData);
    }
    return NULL;
}

MallocMetadata* checkAndMergeFromFront(MallocMetadata *pMetaData) {
    if (pMetaData && pMetaData->freeListNext) {
        return checkAndMerge(pMetaData, pMetaData->freeListNext);
    }
    return NULL;
}

bool isMergedBlockSizeOk(const MallocMetadata *block) {
    return getMatchingFirstIndex(block->size) < MAX_ORDER;
}

bool isAdjacent(const MallocMetadata *secondBlock, const MallocMetadata *firstBlock) {
    return (unsigned long)firstBlock + _size_meta_data() + firstBlock->size == (unsigned long)secondBlock;
}

bool canBeMerged(MallocMetadata* block) {
    if (block &&isMergedBlockSizeOk(block) &&
    ((block->freeListPrev && isAdjacent(block, block->freeListPrev)) || ((block->freeListNext && isAdjacent(block->freeListNext, block))))) {
        return true;
    }
    return false;
}

bool canBeMergedFromBack(MallocMetadata* block)
{
        if(block && isMergedBlockSizeOk(block) && ((block->freeListPrev && isAdjacent(block, block->freeListPrev)) )) {
            return true;
        }
        return false;
}

bool canBeMergedFromFront(MallocMetadata* block) {
    if (block &&isMergedBlockSizeOk(block) &&((block->freeListNext && isAdjacent(block->freeListNext, block)))) {
        return true;
    }
    return false;
}

void sfree(void* p){
    MallocMetadata* pMetaData = (MallocMetadata*)p;
    pMetaData = insertToFreeListAt(getMatchingIndex(pMetaData->size), p, pMetaData->cookie);
    while(canBeMerged(pMetaData)) {
        MallocMetadata* backMerge = checkAndMergeFromBack(pMetaData);
        if(backMerge) {
            pMetaData = backMerge;
            continue;
        }
        pMetaData = checkAndMergeFromFront(pMetaData);
    }
}

void* srealloc(void* oldp, size_t size){
    if (size == 0 || size > 100000000) {
        return NULL;
    }

    if(!oldp)
    {
        return smalloc(size);
    }

    if(size <= ((MallocMetadata*)((unsigned long)oldp - _size_meta_data()))->size)
    {
        return oldp;
    }

    void* reallocated_space=smalloc(size);
    if(reallocated_space)
    {
        memmove(reallocated_space,oldp,size);
        sfree(oldp);
        return reallocated_space;
    }
    return NULL;
}

int desired_power_for_realloc(unsigned int size)
{
    int sum=0;
    int power_of_2=0;
            while(sum<=size)
            {
                sum=128<<power_of_2;
                power_of_2++;
            }
            return power_of_2;
}

bool backtracking_speculation_for_realloc(MallocMetadata* start_adress, int current_size, int desired_size)
{
    if(current_size==desired_size)
    {
        return true;
    }
    bool leftResult = false, rightResult = false;
    if(canBeMergedFromBack(start_adress))
    {
        leftResult = backtracking_speculation_for_realloc(start_adress-=(128<<current_size),current_size+1,desired_size);
    }
    if(checkAndMergeFromFront(start_adress))
    {
        rightResult = backtracking_speculation_for_realloc(start_adress,current_size+1,desired_size);
    }
    return leftResult || rightResult;
};






