#ifndef MALLOC_H
# define MALLOC_H
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>

#define _ANONY_ 0x20
#define TINY 64
#define SMALL 512
#define TINY_PAGE 1
#define SMALL_PAGE 8
#define LARGE_PAGE 64


struct s_alloc {
    void            *ptr;
    size_t          size;
};
typedef struct s_alloc t_alloc;

struct s_header {
    void        *origin;
    t_alloc     *alloc;
    size_t      nb_alloc;
    size_t      headerSize;
    size_t      memSize;
};

typedef struct s_header t_header;

struct s_malloc {
    size_t      pageSize;
    t_header    tHeader;
    t_header    sHeader;
    t_header    lHeader;
};

typedef struct s_malloc t_malloc;



#endif