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

int addNewPage(t_header *header) {
    t_header *nextPage;


    if (!(nextPage = mmap(header, header->memSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | _ANONY_, -1, 0)))
        return (0);
    nextPage->memSize = header->memSize;
    nextPage->nextPage = 0;
    nextPage->first = 0;
    nextPage->memLeft = nextPage->memSize - sizeof(t_header);
    header->nextPage = nextPage;
}

int initData(void) {
    data.pageSize = getpagesize();
    initHeader(&data.tHeader, TINY_PAGE);
    
  //  initHeader(&data.sHeader, SMALL_PAGE);
 /*   initHeader(&data.lHeader, LARGE_PAGE);*/
    return (1);
}

void show_alloc_mem(void) {
    t_alloc     *alloc;
    t_header *page;

    page = data.tHeader;
    while (page) {
        printf("TINY : %X\n", page);
        alloc = page->first;
        while (alloc) {
            printf("%X - %X : %d octets\n", alloc + sizeof(t_alloc), alloc + sizeof(t_alloc) + alloc->size, alloc->size);
            alloc = alloc->next;
        }
        page = page->nextPage;
    }
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
t_alloc *firstAlloc(size_t size, t_header *header) {
    t_alloc *newAlloc;

    if (!header->first || header + sizeof(t_header) + size + sizeof(t_alloc) <= header->first) {
        newAlloc = header + sizeof(t_header);
        newAlloc->next = 0;
        newAlloc->size = size;
        if (header->first) {
            newAlloc->next = header->first;
            header->first = newAlloc;
        }
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
        if (alloc + sizeof(t_alloc) + alloc->size + sizeof(t_alloc) + size <= next) {
            newAlloc = alloc + alloc->size + sizeof(t_alloc);
            newAlloc->next = next;
            newAlloc->size = size;
            alloc->next = newAlloc;
            return (newAlloc);
        }
        alloc = next;
        next = alloc->next;
    }
    if (alloc + sizeof(t_alloc) + alloc->size + sizeof(t_alloc) + size <= header + header->memSize) {
        newAlloc = alloc + alloc->size + sizeof(t_alloc);
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
                addNewPage(page);
            page = page->nextPage;
            continue;
        }
        if ((newAlloc = getPtr(size, page))) {
            page->memLeft -= newAlloc->size + sizeof(t_alloc);
            return (newAlloc);
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
    t_alloc *alloc;
    t_alloc *next;
    t_header *page;

    if (!ptr)
        return ;
    page = data.tHeader;
    while (page) {
        if (ptr >= page->first && ptr < page + page->memSize) {
            alloc = page->first;
            next = alloc->next;
            if (ptr == alloc + sizeof(t_alloc)) {
                page->memLeft += sizeof(t_alloc) + alloc->size;
                page->first = next;
                return ;
            }
            while (alloc) {
                if (ptr == next + sizeof(t_alloc)) {
                    page->memLeft += sizeof(t_alloc) + next->size;
                    alloc->next = next->next;
                    return ;
                }
                alloc = next;
                next = alloc->next;
            }
        }
        page = page->nextPage;
    }
}
/*
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
*/
#include <string.h>
int main(void) {
    char *test;
    void *t1;
    void *t2;
    void *t3;
    int n;

    t1 = fmalloc(10);
  /*  t2 = fmalloc(52);
    t3 = fmalloc(30);
    strcpy(t3, "it works");
//    show_alloc_mem();
    ffree(t2);
    ffree(t1);
//    ffree(t3);
    fmalloc(5);
    n = -1;
    while (++n < 100)
    t1 =    fmalloc(128);
  //  show_alloc_mem();
    
    printf("%s\n", t3);*/
    return (0);
}