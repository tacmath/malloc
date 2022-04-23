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

int addMemory(t_header *header, int pages) {
    void *test;
    printf("ptr : %X\n", header->origin);
    printf("size : %d\n", header->memSize);
    header->memSize += data.pageSize * pages;
    test = mmap(header->origin, header->memSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | _ANONY_ | MAP_FIXED, -1, 0);
    printf("size : %d\n", header->memSize);
    printf("ptr : %X\n", test);
    printf("ptr max : %X\n", header->origin + header->memSize);
}

int initData(void) {
    data.pageSize = getpagesize();
    initHeader(&data.tHeader, TINY_PAGE);
    initHeader(&data.sHeader, SMALL_PAGE);
 /*   initHeader(&data.lHeader, LARGE_PAGE);*/
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

void *ft_memcpy(void *s1, void *s2, size_t size) {
    char modulo;
    int     *st1;
    int     *st2;
    int n;
    
    st1 = (int*)s1;
    st2 = (int*)s2;
    modulo = size % sizeof(int);
    size /= sizeof(int);
    n  = -1;
    while (++n < size)
        st1[n] = st2[n];
    n = -1;
    while (++n < modulo)
        ((char*)st1)[n] = ((char*)st2)[n];
    return (s1);
}

void *ft_memrcpy(void *s1, void *s2, size_t size) {
    int     *st1;
    int     *st2;
    char modulo;
    int n;
    
    st1 = (int*)s1;
    st2 = (int*)s2;
    modulo = size % sizeof(int);
    size -= modulo;
    n = modulo;
    while (n)
        ((char*)st1)[size + n] = ((char*)st2)[size + n--];
    n  = size / sizeof(int);
    while (n)
        st1[n] = st2[n--];
    return (s1);
}

t_alloc *checkSpaceLeft(size_t size, t_header *header) {
    int n;

    n = -1;
    while (++n < header->nb_alloc - 1) {
        if (header->alloc[n + 1].ptr - (header->alloc[n].ptr + header->alloc[n].size) >= size) {
            ft_memrcpy(&header->alloc[n + 2], &header->alloc[n + 1], (header->nb_alloc - n) * sizeof(t_alloc));
            header->alloc[n + 1].ptr = header->alloc[n].ptr + header->alloc[n].size;
            return (&header->alloc[n + 1]);
        }
    }
    addMemory(header, TINY_PAGE);     //ajouer une variable pour les pages par realloc
    header->alloc[n + 1].ptr = header->alloc[n].ptr + header->alloc[n].size;
    return (&header->alloc[n + 1]);
}

void *createPtr(size_t size, t_header *header) {
    t_alloc *lastAlloc;
    t_alloc *newAlloc;
    
    newAlloc = &header->alloc[header->nb_alloc];
    if (header->nb_alloc) {
        lastAlloc = &header->alloc[header->nb_alloc - 1];
        newAlloc->ptr = lastAlloc->ptr + lastAlloc->size;
        if (newAlloc->ptr + size - header->origin > header->memSize)
            newAlloc = checkSpaceLeft(size, header);
        newAlloc->size = size;
    }
    else {
        newAlloc->ptr = header->origin;
        newAlloc->size = size;
    }
    header->nb_alloc++;
    return (newAlloc->ptr);
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
    int n;

    if (!ptr)
        return ;
    n = -1;
    while (++n < data.tHeader.nb_alloc) {
        if (data.tHeader.alloc[n].ptr == ptr && --data.tHeader.nb_alloc > n)
            ft_memcpy(&data.tHeader.alloc[n], &data.tHeader.alloc[n + 1], (data.tHeader.nb_alloc - n) * sizeof(t_alloc));
    }
}

void *fcalloc(size_t nmemb, size_t size) {
    char *ptr;
    size_t n;

    size *= nmemb;
    if ((ptr = fmalloc(size))) {
        n = -1;
        while (++n < size)
            ptr[n] = 0;
    }
    return (ptr);
}

void *frealloc(void *ptr, size_t size) {
    int n;

    if (!ptr)
        return (fmalloc(size));
    if (!size) {
        ffree(ptr);
        return (0);
    }
    alineSize(&size);
    if (size <= TINY) {
        n = -1;
        while (++n < data.tHeader.nb_alloc) {
            if (data.tHeader.alloc[n].ptr == ptr && (n + 1 == data.tHeader.nb_alloc
                || data.tHeader.alloc[n].ptr + size <= data.tHeader.alloc[n + 1].ptr)) {
                data.tHeader.alloc[n].size = size;
                return (ptr);
            }    
        }
    }
    ffree(ptr);
    return (fmalloc(size));
}

#include <string.h>
int main(void) {
    char *test;
    void *t1;
    void *t2;
    void *t3;
    int n;

    t1 = fmalloc(10);
    t2 = fmalloc(52);
    t3 = fmalloc(30);
    show_alloc_mem();
    ffree(t2);
    ffree(t1);
//    ffree(t3);
    fmalloc(5);
    n = -1;
    while (++n < 200)
    t1 =    fmalloc(50);
    show_alloc_mem();
    strcpy(t1, "it works");
    printf("%s\n", t1);
    return (0);
}