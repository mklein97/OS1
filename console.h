#pragma once
#include "Multiboot.h"
void console_init(struct MultibootInfo* mbi);
void set_pixel(int x, int y, int r, int g, int b);
void draw_rect(int x1, int y1, int x2, int y2, int r, int g, int b);
void draw_char(int x, int y, char ch);
void draw_string(int x, int y, char* str, int size);
void console_putc(char ch);
void outb(unsigned short port, unsigned char value);
void outw(unsigned short port, unsigned short value);
unsigned char inb(unsigned short port);
unsigned short inw(unsigned short port);
int isBusy();
void printstr(char* str, int size);
void kmemcpy(void* dv, const void* sv, unsigned n);
void kmemset(void *p, char c, int n);
int row;
int col;