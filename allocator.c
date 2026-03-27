#include "allocator.h"
#include <stdio.h>
#include <unistd.h>
#define MAGIC 0xBAAAAAAAAADA110CL
static HEADER *free_list = NULL;

static void write_end_magic(void *user_ptr, size_t bloc_size)
{
    long magic_num = MAGIC;
    unsigned char *magic_num_bytes = (unsigned char *)&magic_num;
    unsigned char *end_ptr = (unsigned char *)user_ptr + bloc_size;
    for (size_t i = 0; i < sizeof(long); i++)
    {
        end_ptr[i] = magic_num_bytes[i];
    }
}

static int check_end_magic(void *user_ptr, size_t bloc_size)
{
    long magic_num = MAGIC;
    unsigned char *magic_num_bytes = (unsigned char *)&magic_num;
    unsigned char *end_ptr = (unsigned char *)user_ptr + bloc_size;
    for (size_t i = 0; i < sizeof(long); i++)
    {
        if (end_ptr[i] != magic_num_bytes[i])
            return 0;
    }
    return 1;
}

void *malloc_fame(size_t bloc_size)
{

    if (bloc_size == 0)
        return NULL;

    HEADER *prev = NULL;
    HEADER *cur = free_list;

    while (cur != NULL)
    {

        if (cur->bloc_size >= bloc_size)
        {

            size_t leftover = cur->bloc_size - bloc_size;
            if (leftover > sizeof(HEADER) + sizeof(long))
            {

                HEADER *new_free = (HEADER *)((char *)(cur + 1) + bloc_size + sizeof(long));
                new_free->bloc_size = leftover - sizeof(HEADER) - sizeof(long);
                new_free->magic_number = MAGIC;
                new_free->ptr_next = cur->ptr_next;

                write_end_magic(new_free + 1, new_free->bloc_size);

                cur->bloc_size = bloc_size;

                if (prev)
                {
                    prev->ptr_next = new_free;
                }
                else
                {
                    free_list = new_free;
                }
            }
            else
            {
                if (prev)
                {
                    prev->ptr_next = cur->ptr_next;
                }
                else
                {
                    free_list = cur->ptr_next;
                }
            }

            cur->ptr_next = NULL;
            cur->magic_number = MAGIC;

            void *user_ptr = (void *)(cur + 1);
            write_end_magic(user_ptr, cur->bloc_size);
            return user_ptr;
        }
        prev = cur;
        cur = cur->ptr_next;
    }

    size_t total = sizeof(HEADER) + bloc_size + sizeof(long);
    void *raw = sbrk((intptr_t)total);
    if (raw == (void *)-1)
    {
        return NULL;
    }

    HEADER *bloc = (HEADER *)raw;
    bloc->ptr_next = NULL;
    bloc->bloc_size = bloc_size;
    bloc->magic_number = MAGIC;

    void *user_ptr = (void *)(bloc + 1);
    write_end_magic(user_ptr, bloc_size);
    return user_ptr;
}

void free_fame(void *ptr)
{

    if (ptr == NULL)
    {
        return;
    }

    HEADER *bloc = (HEADER *)ptr - 1;

    if (bloc->magic_number != MAGIC)
    {
        fprintf(stderr, "Buffer overflow detected: (start of the block).\n");
        return;
    }

    if (!check_end_magic(ptr, bloc->bloc_size))
    {
        fprintf(stderr, "Buffer overflow detected: (end of the block).\n");
        return;
    }

    printf("Memory freed successfully\n");

    HEADER *prev = NULL;
    HEADER *cur = free_list;

    while (cur != NULL && cur < bloc)
    {
        prev = cur;
        cur = cur->ptr_next;
    }

    bloc->ptr_next = cur;
    if (prev)
    {
        prev->ptr_next = bloc;
    }
    else
    {
        free_list = bloc;
    }
    char *bloc_end = (char *)(bloc + 1) + bloc->bloc_size + sizeof(long);

    if (bloc->ptr_next != NULL && (char *)bloc->ptr_next == bloc_end)
    {
        HEADER *next = bloc->ptr_next;
        bloc->bloc_size += sizeof(HEADER) + sizeof(long) + next->bloc_size;
        bloc->ptr_next = next->ptr_next;
        write_end_magic(bloc + 1, bloc->bloc_size);
    }

    if (prev != NULL)
    {
        char *prev_end = (char *)(prev + 1) + prev->bloc_size + sizeof(long);
        if (prev_end == (char *)bloc)
        {
            prev->bloc_size += sizeof(HEADER) + sizeof(long) + bloc->bloc_size;
            prev->ptr_next = bloc->ptr_next;
            write_end_magic(prev + 1, prev->bloc_size);
        }
    }
}