#include "malloc.h"

int initHeader(t_header **header, size_t size) {
    t_header *firstPage;

    if (!(*header = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | _ANONY_, -1, 0)))
        return (0);
    firstPage = *header;
    firstPage->nextPage = 0;
    firstPage->memSize = size;
    firstPage->memLeft = firstPage->memSize - sizeof(t_header);
    firstPage->first = 0;
    return (1);
}

int addNewPage(t_header *header, size_t size) {
    t_header    *nextPage;
    size_t      pageMemSize;
    
    pageMemSize = header->memSize;
    if (pageMemSize > LARGE_PAGE)
        pageMemSize = LARGE_PAGE;
    if (size > pageMemSize - sizeof(t_header) - sizeof(t_alloc)) {
        size += sizeof(t_header) + sizeof(t_alloc);
        pageMemSize = size - (size % PAGE_SIZE) + PAGE_SIZE;
    } 
    if (!(nextPage = mmap((void*)header + pageMemSize, pageMemSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | _ANONY_, -1, 0)) || nextPage == (void*)0xffffffffffffffff)
        return (0);
    nextPage->memSize = pageMemSize;
    nextPage->nextPage = 0;
    nextPage->first = 0;
    nextPage->memLeft = nextPage->memSize - sizeof(t_header);
    header->nextPage = nextPage;
    return (1);
}

void    alineSize(size_t *size) {
    if (*size & 15)
        *size = (((*size) >> 4) << 4) + 16;
}

void *calloc(size_t nmemb, size_t size) {
    char *ptr;
    size_t n;

    size *= nmemb;
    if ((ptr = malloc(size))) {
        n = -1;
        while (++n < size)
            ptr[n] = 0;
    }
    return (ptr);
}