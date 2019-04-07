#include "Multiboot.h"
#include "console.h"
#include "testsuite.h"
#include "disk.h"
#include "Interrupts.h"

extern volatile unsigned char* framebuffer;

void kmain(struct MultibootInfo* mbi) 
{
    console_init(mbi);
    disk_init();
    interrupt_init();
    sweet(1, (void*)framebuffer, mbi->mbiFramebufferRedPos, mbi->mbiFramebufferRedMask,
    mbi->mbiFramebufferGreenPos, mbi->mbiFramebufferGreenMask, mbi->mbiFramebufferBluePos,
    mbi->mbiFramebufferBlueMask, mbi->mbiFramebufferBpp, mbi->mbiFramebufferPitch);
    //interrupt test 1
    /*int a = 5;
    int b = 0;
    int c = a/b;
    kprintf("%d\n", c);*/

    //interrupt test 2
    //asm volatile("int 3" :::"memory");

    //interrupt test 3
    //asm volatile(".byte 15,255" ::: "memory");

    while(1){}
}

void clearBss(char* bssStart, char* bssEnd) 
{
    while (bssStart != bssEnd) {
        *bssStart = 0;
        bssStart++;
    }
}

int factorial(int i) 
{
    if (i >= 1)
        return i * factorial(i - 1);
    else
        return 1;
}