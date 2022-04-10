#pragma once
#include <ntdef.h>

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI 0x20

static const char *exception_messages[ 32 ] = {
	"Division by zero",
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Detected overflow",
	"Out-of-bounds",
	"Invalid opcode",
	"No coprocessor",
	"Double fault",
	"Coprocessor segment overrun",
	"Bad TSS",
	"Segment not present",
	"Stack fault",
	"General protection fault",
	"Page fault",
	"Unknown interrupt",
	"Coprocessor fault",
	"Alignment check",
	"Machine check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
};
void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();
void isr127();

void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47


struct HAL_IDT_DESCRIPTOR
{
	USHORT Offset1;
	USHORT Selector;
	UCHAR IST;
	UCHAR TypeAttribute;
	USHORT Offset2;
	UINT Offset3;
	UINT Zero;
};

struct HAL_IDT_ENTRY
{
	USHORT ISR_Low;
	USHORT Kernel_cs;
	UCHAR IST;
	UCHAR Attributes;
	USHORT ISR_Mid;
	UINT ISR_High;
	UINT Reserved;
} __attribute__( ( packed ) );

struct HAL_IDT_GENERAL
{
	USHORT Limit;
	ULONG64 Base;
} __attribute__( ( packed ) );

struct InterruptRegisters 
{
	ULONG64 r15, r14, r13, r12,
	r11, r10, r9, r8, rbp, rdi,
	rsi, rdx, rcx, rbx, rax,
	int_no, error_code, rip,
	cs, rflags, rsp, ss;
}__attribute__( ( packed ) );

static struct HAL_IDT_ENTRY IDT[ 256 ] __attribute__( ( aligned( 0x10 ) ) );

static struct HAL_IDT_GENERAL IDTGeneral;

VOID
HalInitIDT(

);

typedef void( *HalInterruptHandler )( struct InterruptRegisters *registers );

VOID
HalRegisterInterrupt(
	UCHAR n,
	HalInterruptHandler Handler
);




VOID HalHaltSystem();