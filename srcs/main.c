#include <malloc.h>
#include <stdio.h>

t_malloc data = {0, 0, 0, 0};

int initHeader(t_header *header, size_t page) {
    header->origin = mmap(0, data.pageSize * page, PROT_READ | PROT_WRITE, MAP_PRIVATE | _ANONY_, -1, 0);
    header->alloc = mmap(0, data.pageSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | _ANONY_, -1, 0);
    header->headerSize = data.pageSize;
    header->memSize = data.pageSize * page;
    header->alloc->ptr = 0;
    header->nb_alloc = 0;
    return (1);
}

int initData(void) {
    data.pageSize = getpagesize();
    initHeader(&data.tHeader, TINY_PAGE);
 /*   initHeader(&data.sHeader, SMALL_PAGE);
    initHeader(&data.lHeader, LARGE_PAGE);*/
    return (1);
}

void show_alloc_mem(void) {
    int n;

    printf("TINY : %X\n", data.tHeader.origin);
    n = -1;
    while (++n < data.tHeader.nb_alloc)
        printf("%X - %X : %d octets\n", data.tHeader.alloc[n].ptr,
            data.tHeader.alloc[n].ptr + data.tHeader.alloc[n].size, data.tHeader.alloc[n].size);
}

void *createPtr(size_t size, t_header *header) {
    t_alloc *lastAlloc;
    t_alloc *newAlloc;
    
    newAlloc = &header->alloc[header->nb_alloc];
    if (header->nb_alloc) {
        lastAlloc = &header->alloc[header->nb_alloc - 1];
        newAlloc->ptr = lastAlloc->ptr + lastAlloc->size;
        newAlloc->size = size; 
    }
    else {
        newAlloc->ptr = header->origin;
        newAlloc->size = size;
    }
    header->nb_alloc++;
    return (newAlloc->ptr);
  /*  while (1) {
        if (header->alloc->ptr) {
            if (!header->alloc->next) {

            }
            if ((size_t)header->alloc->next - (size_t)(header->alloc->ptr + header->alloc->size) >= size) {

            }
        }
        else {
            if (header->alloc->prev)
                header->alloc->ptr = header->origin;
        }
    }*/
}

void    alineSize(size_t *size) {
    char modulo;

    modulo = *size % 16;
    if (*size)
        *size = *size - modulo + 16;
}

void *fmalloc(size_t size) {
    if (!size)
        return (0);
    alineSize(&size);
    if (!data.pageSize)
        initData();
    if (size <= TINY)
        return(createPtr(size, &data.tHeader));
    return (0);
}

void ffree(void *ptr) {

}

void *fcalloc(size_t nmemb, size_t size) {

}

void *frealloc(void *ptr, size_t size) {

}

#include <string.h>
int main(void) {
    char *test;
    printf("pointer = %X\n", fmalloc(10));
    fmalloc(52);
    fmalloc(500);
    test = fmalloc(30);
    strcpy(test, "hello\n");
    printf("%s", test);
    show_alloc_mem();
    return (0);
}