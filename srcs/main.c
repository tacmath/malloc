#include <malloc.h>
#include <stdio.h>

t_malloc data = {0, 0, 0, 0};

int initHeader(t_header **header, size_t page) {
    t_header *firstPage;

    if (!(*header = mmap(0, data.pageSize * page, PROT_READ | PROT_WRITE, MAP_PRIVATE | _ANONY_, -1, 0)))
        return (0);
    firstPage = *header;
    firstPage->nextPage = 0;
    firstPage->memSize = data.pageSize * page;
    firstPage->memLeft = firstPage->memSize - sizeof(t_header);
    firstPage->first = 0;
    return (1);
}

int addNewPage(t_header *header, size_t size) {
    t_header    *nextPage;
    size_t      pageMemSize;
    
    pageMemSize = header->memSize;
    if (pageMemSize > data.pageSize * LARGE_PAGE)
        pageMemSize = data.pageSize * LARGE_PAGE;
    if (size > pageMemSize - sizeof(t_header))
        pageMemSize = size - (size % data.pageSize) + data.pageSize; 
    if (!(nextPage = mmap(header, pageMemSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | _ANONY_, -1, 0)) || nextPage == (void*)0xffffffffffffffff)
        return (0);
    nextPage->memSize = pageMemSize;
    nextPage->nextPage = 0;
    nextPage->first = 0;
    nextPage->memLeft = nextPage->memSize - sizeof(t_header);
    header->nextPage = nextPage;
}

void show_alloc_type(t_header *header, char *typeName) {
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

t_alloc *firstAlloc(size_t size, t_header *header) {
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


t_alloc *getPtr(size_t size, t_header *header) {
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

void *createPtr(size_t size, t_header *header) {
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

void    alineSize(size_t *size) {
    char modulo;

    modulo = *size % 16;
    if (modulo)
        *size = *size - modulo + 16;
}

void *malloc(size_t size) {
    if (!size)
        return (0);
    alineSize(&size);
    if (!data.pageSize)
        data.pageSize = getpagesize();
    if (size <= TINY && (data.tHeader || initHeader(&data.tHeader, TINY_PAGE)))
        return(createPtr(size, data.tHeader));
    else if (size <= SMALL && (data.sHeader || initHeader(&data.sHeader, SMALL_PAGE)))
        return(createPtr(size, data.sHeader));
    else if (data.lHeader || initHeader(&data.lHeader, LARGE_PAGE)) //augmenter la taille si elle est trop petite
        return(createPtr(size, data.lHeader));
    return (0);
}

int freePtr(void *ptr, t_header *header) {              //free des pages si il y a deux pages vides d'afilée
    t_alloc *alloc;
    t_alloc *next;
    t_header *page;

    if (!header)
        return (0);
    page = header;
    while (page) {
        if ((void*)ptr >= (void*)page->first && (void*)ptr < (void*)page + page->memSize) {
            alloc = page->first;
            next = alloc->next;
            if (ptr == (void*)alloc + sizeof(t_alloc)) {
                page->memLeft += sizeof(t_alloc) + alloc->size;
                page->first = next;
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
    if (freePtr(ptr, data.tHeader) || freePtr(ptr, data.sHeader) || freePtr(ptr, data.lHeader))
        return ;
    dprintf(2, "free pointer : %p not found", ptr);
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

void    *reallocPtr(t_header *page, t_alloc *alloc, size_t size) {
    size_t *new;
    size_t *old;
    size_t n;

    if (alloc->size >= size || (alloc->next && size <= (void*)alloc->next -((void*)alloc + sizeof(t_alloc))) 
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
        free(old);
        return (new);
    }
    return (0);
}

void *getReallocPtr(void *ptr, t_header *firstPage, size_t size) {
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

#include <string.h>
int main(void) {
    char *test;
    void *t1;
    void *t2;
    void *t3;
    int n;

//    t1 = malloc(10);

    t2 = malloc(100);
 //   t3 = malloc(40);

    
     t2 = malloc(20);
 //    show_alloc_mem();
//    show_alloc_mem();
   // free(t3);
//    printf("%s\n", t1);
  //  show_alloc_mem();
  t3 = malloc(600);
    n = -1;
  /*  while (++n < 10000)
        t2 = malloc(10000);*/
    strcpy(t2, "it works");
    printf("%s\n", t2);
//     t2 = calloc(10, 10);
//    memcpy(t2, "123212123132212312123212123132212312123212123132212312123212123132212312123212123132212312999", 93);
    t1 = malloc(600);
    t3 = malloc(600);
    t3 = malloc(600);
    t3 = malloc(6000);
   
    free(t3);
    free(t1);
    realloc(t2, 8000);
    realloc(0, 1200);
    
    printf("%s\n", t2);
        t2 = malloc(1024);
    show_alloc_mem();
  /*  t2 = malloc(52);
    t3 = malloc(30);
    strcpy(t3, "it works");
//    show_alloc_mem();
    free(t2);
    free(t1);
//    free(t3);
    malloc(5);
    n = -1;
    while (++n < 100)
    t1 =    malloc(128);
  //  show_alloc_mem();
    
    printf("%s\n", t3);*/
    return (0);
}

/*
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
}*/
