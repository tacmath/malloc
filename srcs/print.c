#include "malloc.h"

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

void        ft_hexdump(void *data, size_t size) {
    char    hexdumpForma[] = "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |                |\n";
    char *str;
    int len;
    int n;

    str = data;
    while (size > 0) {
        if (size > 15)
            len = 16;
        else
            len = size % 16;
        n = -1;
        while (++n < len) {
            if (str[n])
                ft_puthex(str[n], &hexdumpForma[n * 3], 2);
            else {
                hexdumpForma[n * 3] = '0';
                hexdumpForma[n * 3 + 1] = '0';
            }
            if (str[n] >= 32 && str[n] <= 126)
                hexdumpForma[49 + n] = str[n];
            else
                hexdumpForma[49 + n] = '.';
        }
        write(1, hexdumpForma, 67);
        str += len;
        size -= len;
    }
}

static void show_alloc_type(t_header *header, char *typeName, char option) {
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
            if (option)
                ft_hexdump((void*)alloc + sizeof(t_alloc), alloc->size);
            alloc = alloc->next;
        }
        page = page->nextPage;
    }
}

void show_alloc_mem_hex(void) {
    show_alloc_type(global_malloc.tHeader, "TINY ", 1);
    show_alloc_type(global_malloc.sHeader, "SMALL", 1);
    show_alloc_type(global_malloc.lHeader, "LARGE", 1);
}

void show_alloc_mem(void) {
    show_alloc_type(global_malloc.tHeader, "TINY ", 0);
    show_alloc_type(global_malloc.sHeader, "SMALL", 0);
    show_alloc_type(global_malloc.lHeader, "LARGE", 0);
}