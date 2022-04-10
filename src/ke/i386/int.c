#include <ke/i386.h>

VOID KeEnableInterrupts(){
    __asm__ volatile ("sti");
}

VOID KeDisableInterrupts(){
    __asm__ volatile ("cli");
}
