#include "malloc.h"

static void freePage(t_header *header) {
    t_header *page;
    t_header *nextPage;
    char n;

    n = 0;
    page = header;
    nextPage = page->nextPage;
    if (!page->first)
        n++;
    while (nextPage) {
        if (!nextPage->first && ++n == 2) {
            page->nextPage = nextPage->nextPage;
            munmap(nextPage, nextPage->memSize);
            return ;
        }
        page = nextPage;
        nextPage = page->nextPage;
    }
}

static int freePtr(void *ptr, t_header *header) {
    t_alloc *alloc;
    t_alloc *next;
    t_header *page;

    if (!header)
        return (0);
    page = header;
    while (page) {
        if (page->first && (void*)ptr >= (void*)page->first && (void*)ptr < (void*)page + page->memSize) {
            alloc = page->first;
            next = alloc->next;
            if (ptr == (void*)alloc + sizeof(t_alloc)) {
                page->memLeft += sizeof(t_alloc) + alloc->size;
                page->first = next;
                if (!next)
                    freePage(header);
                return (1);
            }
            while (next) {
                if (ptr == (void*)next + sizeof(t_alloc)) {
                    page->memLeft += sizeof(t_alloc) + next->size;
                    alloc->next = next->next;
                    return (1);
                }
                alloc = next;
                next = alloc->next;
            }
        }
        page = page->nextPage;
    }
    return (0);
}


void free(void *ptr) {
    if (!ptr)
        return ;
    if (freePtr(ptr, global_malloc.tHeader) || freePtr(ptr, global_malloc.sHeader) || freePtr(ptr, global_malloc.lHeader))
        return ;
}