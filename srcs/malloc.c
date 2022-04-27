#include "malloc.h"

t_malloc global_malloc = {0, 0, 0};

static t_alloc *firstAlloc(size_t size, t_header *header) {
    t_alloc *newAlloc;

    if (!header->first || (void*)header + sizeof(t_header) + size + sizeof(t_alloc) <= (void*)header->first) {
        newAlloc = (void*)header + sizeof(t_header);
        newAlloc->next = 0;
        newAlloc->size = size;
        if (header->first)
            newAlloc->next = header->first;
        header->first = newAlloc;
        return (newAlloc);
    }
    return (0);
}


static t_alloc *getPtr(size_t size, t_header *header) {
    t_alloc *newAlloc;
    t_alloc *alloc;
    t_alloc *next;

    if ((newAlloc = firstAlloc(size, header)))
        return (newAlloc);
    alloc = header->first;
    next = alloc->next;
    while (next) {
        if ((void*)alloc + sizeof(t_alloc) + alloc->size + sizeof(t_alloc) + size <= (void*)next) {
            newAlloc = (void*)alloc + alloc->size + sizeof(t_alloc);
            newAlloc->next = next;
            newAlloc->size = size;
            alloc->next = newAlloc;
            return (newAlloc);
        }
        alloc = next;
        next = alloc->next;
    }
    if ((void*)alloc + sizeof(t_alloc) + alloc->size + sizeof(t_alloc) + size <= (void*)header + header->memSize) {
        newAlloc = (void*)alloc + alloc->size + sizeof(t_alloc);
        newAlloc->next = 0;
        newAlloc->size = size;
        alloc->next = newAlloc;
        return (newAlloc);
    }
    return (0);
}

static void *createPtr(size_t size, t_header *header) {
    t_alloc *newAlloc;
    t_header *page;
    
    page = header;
    while (page) {
        if (page->memLeft < size + sizeof(t_alloc)) {
            if (!page->nextPage)
                addNewPage(page, size);
            page = page->nextPage;
            continue;
        }
        if ((newAlloc = getPtr(size, page))) {
            page->memLeft -= newAlloc->size + sizeof(t_alloc);
            return ((void*)newAlloc + sizeof(t_alloc));
        }
        page = page->nextPage;
    }
    return (0);
}

void *malloc(size_t size) {
    if (!size)
        return (0);
    alineSize(&size);
    if (size <= TINY && (global_malloc.tHeader || initHeader(&global_malloc.tHeader, TINY_PAGE)))
        return(createPtr(size, global_malloc.tHeader));
    else if (size <= SMALL && (global_malloc.sHeader || initHeader(&global_malloc.sHeader, SMALL_PAGE)))
        return(createPtr(size, global_malloc.sHeader));
    else if (global_malloc.lHeader || initHeader(&global_malloc.lHeader, LARGE_PAGE)) //augmenter la taille si elle est trop petite
        return(createPtr(size, global_malloc.lHeader));
    return (0);
}
