#include "malloc.h"

static void    *reallocPtr(t_header *page, t_alloc *alloc, size_t size) {
    size_t *new;
    size_t *old;
    size_t n;

    if (alloc->size >= size || (alloc->next && (void*)alloc + sizeof(t_alloc) + size <= (void*)alloc->next) 
        || (!alloc->next && (void*)alloc + sizeof(t_alloc) + size <= (void*)page + page->memSize)) {
        page->memLeft += size - alloc->size;
        alloc->size = size;
        return ((void*)alloc + sizeof(t_alloc));
    }
    if ((new = malloc(size))) {
        size = alloc->size / sizeof(size_t);
        old = (void*)alloc + sizeof(t_alloc);
        n = 0;
        while (n < size)
            new[n] = old[n++];
    /*    size = alloc->size;
        n = (n - 1) * sizeof(size_t);
        while (n < size)
            (char*)new[n] = (char*)old[n++];*/
        free(old);
        return (new);
    }
    return (0);
}

static void *getReallocPtr(void *ptr, t_header *firstPage, size_t size) {
    t_alloc *alloc;
    t_header *page;

    if (!firstPage)
        return (0);
    page = firstPage;
    while (page) {
        if ((void*)ptr >= (void*)page->first && (void*)ptr < (void*)page + page->memSize) {
            alloc = page->first;
            while (alloc) {
                if ((void*)ptr == (void*)alloc + sizeof(t_alloc))
                    return (reallocPtr(page, alloc, size));
                alloc = alloc->next;
            }
        }
        page = page->nextPage;
    }
    return (0);
}

void *realloc(void *ptr, size_t size) {
    int n;
    void *new;

    if (!ptr)
        return (malloc(size));
    if (!size) {
        free(ptr);
        return (0);
    }
    alineSize(&size);
    if ((new = getReallocPtr(ptr, data.tHeader, size)) || (new = getReallocPtr(ptr, data.sHeader, size))
        || (new = getReallocPtr(ptr, data.lHeader, size)))
        return (new);
    return (0);
}