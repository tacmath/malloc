#ifndef MALLOC_H
# define MALLOC_H
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>

#define _ANONY_ 0x20
#define TINY 128
#define SMALL 1024
#define TINY_PAGE 2
#define SMALL_PAGE 16
#define LARGE_PAGE 64


struct s_alloc {
    struct s_alloc  *next;
    size_t          size;
};
typedef struct s_alloc t_alloc;

struct s_header {
    struct s_header *nextPage;
    t_alloc         *first;
    size_t          memLeft;
    size_t          memSize;
};

typedef struct s_header t_header;

struct s_malloc {
    size_t      pageSize;
    t_header    *tHeader;
    t_header    *sHeader;
    t_header    *lHeader;
};

typedef struct s_malloc t_malloc;

extern t_malloc data;

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
void    alineSize(size_t *size);
int addNewPage(t_header *header, size_t size);
int initHeader(t_header **header, size_t page);
void show_alloc_mem(void);


#endif