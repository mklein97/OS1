#include "console.h"
#include "font.h"

volatile unsigned char* framebuffer;
struct MultibootInfo* multiboot;
void console_init(struct MultibootInfo* mbi) 
{
    framebuffer = (volatile unsigned char*) (unsigned)mbi->mbiFramebufferAddress;
    multiboot = mbi;
}

void set_pixel(int x, int y, int r, int g, int b) 
{
    volatile unsigned char* tmp = framebuffer;
    tmp = framebuffer+(2*x+(y*multiboot->mbiFramebufferPitch));
    r >>= (8 - multiboot->mbiFramebufferRedMask);
    g >>= (8 - multiboot->mbiFramebufferGreenMask);
    b >>= (8 - multiboot->mbiFramebufferBlueMask);
    unsigned short colorValue = (b<<multiboot->mbiFramebufferBluePos) | (g<<multiboot->mbiFramebufferGreenPos) | (r<<multiboot->mbiFramebufferRedPos);
    *(unsigned short*)tmp = colorValue;
}

void draw_rect(int x1, int y1, int x2, int y2, int r, int g, int b) 
{
    set_pixel(x1, y1, r, g, b);
    for (int i = x1; i <= x2; i++)
    {
        for (int j = y1; j <= y2; j++)
        set_pixel(i, j, r, g, b);
    }
}

void draw_char(int x, int y, char ch) 
{
    const int * C = font_data[(int)ch];
    int cy, cx;
    for (cy=0;cy<CHAR_HEIGHT;cy++) {
        for (cx=0;cx<CHAR_WIDTH;cx++) {
            if ((MASK_VALUE>>cx) & C[cy])
                set_pixel(cx+x,cy+y,170,170,170);
            else
                set_pixel(cx+x,cy+y,0,0,0);
        }
    }
}

void draw_string(int x, int y, char* str, int size)
{
    for (int i = 0; i < size; i++)
        console_putc(str[i]);
}

void outb(unsigned short port, unsigned char value) {
    asm volatile( "out dx,al" 
        : 
        : "a"(value), "d"(port) 
        : "memory"
    );
}

void outw(unsigned short port, unsigned short value) {
    asm volatile("out dx,ax"
        :
        : "a"(value), "d"(port)
        : "memory"
    );
}

unsigned char inb(unsigned short port) {
    unsigned value;
    asm volatile("in al,dx" : "=a"(value): "d"(port) );
    return (unsigned char) value;
}

unsigned short inw(unsigned short port) {
    unsigned value;
    asm volatile("in ax,dx" : "=a"(value): "d"(port) );
    return (unsigned short) value;
}

int isBusy() {
    return inb(0x1f7) & 0x80;
}

void printstr(char* str, int size)
{
    for (int i = 0; i < size; i++)
        outb(0x3f8, str[i]);
}

void kmemset(void* p, char c, int n)
{
    char* v = (char*)p;
    int i;
    for(i = 0; i < n; i++)
        v[i] = c;
}

void kmemcpy(void* dv, const void* sv, unsigned n)
{
    char* d = (char*)dv;
    const char* s = (const char*)sv;
    while(n--)
        *d++ = *s++;
}

void console_putc(char ch)
{
    if (col == 80) {
        col = 0;
        row++;
    }
    if (ch == '\f') {
        draw_rect(0, 0, 800, 600, 0, 0, 0);
        row = 0;
        col = 0;
    }
    if (ch == '\n') {
        col = 0;
        row++;
    }
    if (ch == '\t') {
        if (col >= 80) {
            row++;
            col = 8;
        }
        if (col % 8 == 0 && col < 80)
            col += 8;
        if (col == 0)
            col = 8;
        if (col == 80)
        {
            col = 0;
            row++;
        }
        else {
            while (col % 8 != 0)
                col++;
        }
    }
    if (ch == '\x7f') {
        if (col == 0) {
            col = 79;
            row--;
            draw_char(col*CHAR_WIDTH,row*CHAR_HEIGHT,' ');
        }
        col--;
        draw_char(col*CHAR_WIDTH,row*CHAR_HEIGHT,' ');
    }
    if (ch == '\r')
        col = 0;
    if (ch == '\b') {
        if (col == 0) {
            col = 79;
            row--;
            draw_char(col*CHAR_WIDTH,row*CHAR_HEIGHT,' ');
        }
        col--;
        draw_char(col*CHAR_WIDTH,row*CHAR_HEIGHT,' ');
    }
    if (row >= 37)
    {
        kmemcpy((void*)(volatile unsigned char*) framebuffer, (void*)(volatile unsigned char*)framebuffer
         + (multiboot->mbiFramebufferPitch*16), multiboot->mbiFramebufferPitch * (multiboot->mbiFramebufferHeight - 16));
        row--;
        col = 0;
        for (int i = 0; i <= CHAR_HEIGHT; ++i)
        {
            volatile unsigned char* fb = framebuffer + multiboot->mbiFramebufferPitch * (multiboot->mbiFramebufferHeight - i);
            kmemset((void*)(volatile unsigned char*)fb, 0, multiboot->mbiFramebufferWidth * 2);
        }
    }
    if (ch != '\n' && ch != '\t' && ch != '\f' && ch != '\x7f' && ch != '\r' && ch != '\b')
    {
        draw_char(col*CHAR_WIDTH, row*CHAR_HEIGHT, ch);
        col++;
    }
    outb(0x3f8, ch);
}