#include "Interrupts.h"
#include "kprintf.h"

void haltForever(void) {
    while(1)
        asm volatile("hlt" ::: "memory");
}

__attribute__((__interrupt__))
void unknownInterrupt(struct InterruptFrame* fr) {
    kprintf("\nFatal exception at eip=%x\n", fr->eip);
    haltForever();
}

__attribute__((__interrupt__))
void unknownInterruptWithCode(struct InterruptFrame* fr, unsigned code) {
    kprintf("Fatal exception: Code=%x eip=%x\n", code, fr->eip);
    haltForever();
}

__attribute__((__interrupt__))
void divide_by_zero(struct InterruptFrame* fr) {
    kprintf("Divide by 0 error at eip=%x\n", fr->eip);
    haltForever();
}

__attribute__((__interrupt__))
void bad_op_code(struct InterruptFrame* fr) {
    kprintf("Bad Op Code at eip=%x\n", fr->eip);
    haltForever();
}

__attribute__((__interrupt__))
void debug_trap(struct InterruptFrame* fr) {
    kprintf("Debug trap at eip=%x\n", fr->eip);
    haltForever();
}

__attribute__((__interrupt__))
void page_fault(struct InterruptFrame* fr) {
    kprintf("Page fault at eip=%x\n", fr->eip);
    haltForever();
}

__attribute__((__interrupt__))
void protection_fault(struct InterruptFrame* fr) {
    kprintf("Protection fault at eip=%x\n", fr->eip);
    haltForever();
}

struct IDTEntry idt[32];
void table(int i, void* func){
    unsigned x = (unsigned)func;
    idt[i].addrLow = x&0xffff;
    idt[i].selector = 2 << 3;
    idt[i].zero = 0;
    idt[i].flags = 0x8e;
    idt[i].addrHigh = x>>16;
}

void interrupt_init() {
    struct GDTEntry gdt[] = {
        { 0,0,0,0,0,0 }, //zeros
        { 0xffff, 0,0,0, 0xcf92, 0}, //data
        { 0xffff, 0,0,0, 0xcf9a, 0} //code
    };
    struct LGDT lgdt;
    lgdt.size = sizeof(gdt);
    lgdt.addr = &gdt[0];
    asm volatile("lgdt [eax]" : : "a"(&lgdt) : "memory");
    struct LIDT tmp;
    tmp.size = sizeof(idt);
    tmp.addr = &idt[0];
    asm volatile("lidt [eax]" : : "a"(&tmp) : "memory");

    for (int i = 0; i < 32; i++) {
        if (i == 0)
            table(i, divide_by_zero);
        else if (i == 3)
            table(i, debug_trap);
        else if (i == 6)
            table(i, bad_op_code);
        else if (i == 13)
            table(i, protection_fault);
        else if (i == 14)
            table(i, page_fault);
        else if (i != 8 && i != 10 && i != 11 && i != 12 && i != 17)
            table(i, unknownInterrupt);
        else
            table(i, unknownInterruptWithCode);
    }
}