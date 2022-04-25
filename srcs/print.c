#include "malloc.h"

#include <stdio.h>

static void show_alloc_type(t_header *header, char *typeName) {
    t_alloc     *alloc;
    t_header *page;

    if (!header)
        return ;
    page = header;
    while (page) {
        printf("%s : %X - %X\n", typeName, page, (void*)page + page->memSize);
        alloc = page->first;
        while (alloc) {
            printf("%X - %X : %lu octets\n", (void*)alloc + sizeof(t_alloc), (void*)alloc + sizeof(t_alloc) + alloc->size, alloc->size);
            alloc = alloc->next;
        }
        page = page->nextPage;
    }
}

void show_alloc_mem(void) {
    show_alloc_type(data.tHeader, "TINY ");
    show_alloc_type(data.sHeader, "SMALL");
    show_alloc_type(data.lHeader, "LARGE");
}