#include "malloc.h"

#include <stdio.h>

char         hexConvert(char nb) {
    if (nb >= 0 && nb <= 9)
        return (nb + '0');
    if (nb >= 10 && nb <= 16)
        return (nb - 10 + 'A');
    return ('0');
}

void		ft_puthex(size_t nb, char* str, int len) {
	if (nb && len) {
		ft_puthex(nb / 16, str, --len);
        str[len] = hexConvert((char)(nb % 16));
	}
}

static void	put_char(char c)
{
	write(1, &c, 1);
}

void		ft_putnbr(size_t nb)
{
	if (nb)
	{
		ft_putnbr(nb / 10);
		put_char((nb % 10) + '0');
	}
}

static void show_alloc_type(t_header *header, char *typeName) {
    t_alloc     *alloc;
    t_header *page;
    char    pageForma[] = " : XXXXXXXX - XXXXXXXX\n";
    char    allocForma[] = "XXXXXXXX - XXXXXXXX : ";

    if (!header)
        return ;
    page = header;
    while (page) {
        ft_puthex((size_t)page & 0xFFFFFFFF, &pageForma[3], 8);
        ft_puthex((((size_t)page + page->memSize) & 0xFFFFFFFF), &pageForma[14], 8);
        write(1, typeName, 5);
        write(1, pageForma, 23);
        alloc = page->first;
        while (alloc) {
            ft_puthex(((size_t)alloc + sizeof(t_alloc)) & 0xFFFFFFFF, allocForma, 8);
            ft_puthex(((size_t)alloc + sizeof(t_alloc) + alloc->size) & 0xFFFFFFFF, &allocForma[11], 8);
            write(1, allocForma, 22);
            ft_putnbr(alloc->size);
            write(1, " octets\n", 8);
           // printf("%X - %X : %lu octets\n", (void*)alloc + sizeof(t_alloc), (void*)alloc + sizeof(t_alloc) + alloc->size, alloc->size);
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