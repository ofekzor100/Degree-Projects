#include <unistd.h>
#include <cstring>

class MallocMetadata {
public:
    size_t size;
    bool is_free;
    MallocMetadata* next;
    MallocMetadata* prev;
};


MallocMetadata* mallocMetaDataListHead= NULL;

size_t _size_meta_data() {
    return sizeof(MallocMetadata);
}

void _num_free_blocks_unique_action(MallocMetadata* temp, size_t& num_free_blocks) {
    if (temp->is_free) {
        num_free_blocks++;
    }
}

void _num_free_bytes_unique_action(MallocMetadata* temp, size_t& num_free_bytes) {
    if (temp->is_free) {
        num_free_bytes+=temp->size;
    }
}

void _num_allocated_blocks_unique_action(MallocMetadata* temp, size_t& num_allocated_blocks) {
    num_allocated_blocks++;
}

void _num_allocated_bytes_unique_action(MallocMetadata* temp, size_t& num_allocated_bytes) {
    num_allocated_bytes+=temp->size;
}

void _num_meta_data_bytes_unique_action(MallocMetadata* temp, size_t& num_meta_data_bytes) {
    num_meta_data_bytes += _size_meta_data();
}

size_t _generic_function_for_all_methods(void (*unique_action)(MallocMetadata*, size_t&)) {
    MallocMetadata* temp = mallocMetaDataListHead;
    size_t num = 0;
    while (temp) {
        unique_action(temp, num);
        temp = temp->next;
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
    return _generic_function_for_all_methods(_num_allocated_blocks_unique_action);
}

size_t _num_allocated_bytes () {
    return _generic_function_for_all_methods(_num_allocated_bytes_unique_action);
}

size_t _num_meta_data_bytes() {
    return _generic_function_for_all_methods(_num_meta_data_bytes_unique_action);
}

MallocMetadata* smalloc_reUse(size_t size) {
    MallocMetadata* temp = mallocMetaDataListHead;
    while (temp) {
        if(temp->is_free && temp->size>=size) {
            temp->is_free = false;
            break;
        }
        temp = temp->next;
    }
    return temp;
}

void* smalloc_newUse(size_t size)
{
    void* sbrk_result = sbrk(size+_size_meta_data());
    if (sbrk_result == (void *)-1) {
        return NULL;
    }
    MallocMetadata *new_node = (MallocMetadata*)sbrk_result, *temp = mallocMetaDataListHead, *prevTemp=NULL;
    new_node->is_free = false;
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->size = size;
    if (!mallocMetaDataListHead) {
        mallocMetaDataListHead = new_node;
        return (void*)((unsigned long)sbrk_result + _size_meta_data());
    }
    while(temp) {
        if(temp->size <= size) {
            prevTemp = temp;
            temp = temp->next;
            continue;
        }
        break;
    }
    if (!temp) {
        prevTemp->next = new_node;
        new_node->prev = prevTemp;
    }
    else {
        new_node->prev = prevTemp;
        new_node->next = temp;
        temp->prev = new_node;
        if(prevTemp) {
            prevTemp->next = new_node;
        } else {
            mallocMetaDataListHead = new_node;
        }
    }
    return (void*)((unsigned long)sbrk_result + _size_meta_data());
}

void* smalloc(size_t size){
    if (size == 0 || size > 100000000) {
        return NULL;
    }
    void* to_reuse= smalloc_reUse(size);
    if(to_reuse){
        return (void*)((unsigned long)to_reuse + _size_meta_data());
    }
    void* new_use = smalloc_newUse(size);
    if (new_use) {
        return new_use;
    }
    return NULL;
}



void* scalloc(size_t num, size_t size) {
    void* tozeroall= smalloc(num*size);
    if (tozeroall) {
        std::memset(tozeroall,0,num*size);
    }
    return tozeroall;
}

void sfree(void* p){
    if(!p || ((MallocMetadata *) (MallocMetadata*)((unsigned long)p-_size_meta_data()))->is_free)
    {
        return;
    }
    ((MallocMetadata*)((unsigned long)p-_size_meta_data()))->is_free=true;
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



