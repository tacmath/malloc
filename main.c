#include "malloc.h"
#include <stdio.h>

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
 /*   while (++n < 10000)
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
    t2 = realloc(t2, 8000);
    t2 = realloc(0, 1200);
    
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
