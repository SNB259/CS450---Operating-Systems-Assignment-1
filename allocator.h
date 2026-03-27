#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <stddef.h>

void *malloc_fame(size_t size);
void free_fame(void *ptr);

typedef struct HEADER_TAG
{
    struct HEADER_TAG *ptr_next; /* points to the next free block */
    size_t bloc_size;            /* size of the memory block in bytes */
    long magic_number;           /* 0xBAAAAAAAAADA110CL */
} HEADER;

#endif