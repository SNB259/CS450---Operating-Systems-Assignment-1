#include "allocator.h"
#include <stdio.h>

int main()
{
    printf("Allocation and Freeing: \n");
    char *p1 = malloc_fame(10);
    p1[0] = 'A';
    p1[9] = 'B';
    free_fame(p1);

    printf("\nReusing blocks: \n");
    char *p2 = malloc_fame(10);
    printf("p1=%p\np2=%p\n", (void *)p1, (void *)p2);
    free_fame(p2);

    printf("\nMultiple allocations and merging\n");
    char *a = malloc_fame(20);
    char *b = malloc_fame(20);
    char *c = malloc_fame(20);
    free_fame(a);
    free_fame(b);
    free_fame(c);

    printf("\nBuffer overflow\n");
    char *buf = malloc_fame(10);
    for (int i = 10; i < 18; i++)
        buf[i] = 'C';
    free_fame(buf);

    printf("\nNULL free\n");
    free_fame(NULL);
    printf("NULL free works correctly\n");

    return 0;
}