//
// Created by illus on 01/07/2023.
//

#ifndef OS_HW4_MALLOC_2_H
#define OS_HW4_MALLOC_2_H

void* smalloc(size_t size);
void* scalloc(size_t num, size_t size);
void sfree(void* p);
void* srealloc(void* oldp, size_t size);
size_t _size_meta_data();

#endif //OS_HW4_MALLOC_2_H
