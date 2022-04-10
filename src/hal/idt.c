#include <hal/idt.h>
#include <stdint.h>
#include <io/io.h>

#include <ke/bugcheck.h>

HalInterruptHandler HalIntHandlers[ 256 ];

VOID
HalInsertIDTDescriptor(
	UCHAR Vector,
	PVOID ISR,
	UCHAR TypeAttribute
)
{
	struct HAL_IDT_DESCRIPTOR *Descriptor = ( struct HAL_IDT_DESCRIPTOR * )&IDT[ Vector ];

	Descriptor->Offset1 = ( ULONG64 )ISR & 0xFFFF;
	Descriptor->Selector = 0x28;
	Descriptor->IST = 0;
	Descriptor->TypeAttribute = TypeAttribute;
	Descriptor->Offset2 = ( ( ULONG64 )ISR >> 16 ) & 0xFFFF;
	Descriptor->Offset3 = ( ( ULONG64 )ISR >> 32 ) & 0xFFFFFFFF;
	Descriptor->Zero = 0;
}

STATIC VOID
HalIrqNotImplemented(
	struct InterruptRegisters *
);

VOID
HalInstallIRS(

);

VOID
HalInitializeIDT(

)
{
	IDTGeneral.Base = ( uintptr_t )&IDT[ 0 ];
	IDTGeneral.Limit = ( USHORT )sizeof( struct HAL_IDT_DESCRIPTOR ) * 256 - 1;

	for ( INT i = 32; i < 48; i++ )
	{
		HalRegisterInterrupt( i, HalIrqNotImplemented );
	}

	HalInstallIRS();

	__asm__ volatile( "lidt %0"
					  :
	: "memory"( IDTGeneral ) );
	KeEnableInterrupts( );

	
}

VOID
HalRegisterInterrupt(
	UCHAR n,
	HalInterruptHandler Handler
)
{
	HalIntHandlers[ n ] = Handler;
}



VOID
HalISRHandler(
	struct InterruptRegisters *regs
)
{
	


	//RtlRaiseException( regs );
	KeBugCheck2( regs->int_no, regs->error_code, regs->rip, regs->cs, regs->ss, NULL );

	
	
}

VOID HalHaltSystem(){
	for(;;)
		asm("hlt");
}

VOID
HalIRQHandler(
	struct InterruptRegisters *Register 
)
{
	if ( HalIntHandlers[ Register->int_no ] != 0 )
	{
		HalInterruptHandler Handler = HalIntHandlers[ Register->int_no ];
		Handler( Register );
	}

	if ( Register->int_no >= IRQ8 )
	{
		IoOutputByte( PIC2_COMMAND, PIC_EOI );
	}
	IoOutputByte( PIC1_COMMAND, PIC_EOI );
}

STATIC VOID
HalIrqNotImplemented(
	struct InterruptRegisters *testing
)
{
	//nothing

}

VOID
HalInstallIRS(

)
{
	HalInsertIDTDescriptor( 0, ( PVOID )isr0, 0x8E );
	HalInsertIDTDescriptor( 1, ( PVOID )isr1, 0x8E );
	HalInsertIDTDescriptor( 2, ( PVOID )isr2, 0x8E );
	HalInsertIDTDescriptor( 3, ( PVOID )isr3, 0x8E );
	HalInsertIDTDescriptor( 4, ( PVOID )isr4, 0x8E );
	HalInsertIDTDescriptor( 5, ( PVOID )isr5, 0x8E );
	HalInsertIDTDescriptor( 6, ( PVOID )isr6, 0x8E );
	HalInsertIDTDescriptor( 7, ( PVOID )isr7, 0x8E );
	HalInsertIDTDescriptor( 8, ( PVOID )isr8, 0x8E );
	HalInsertIDTDescriptor( 9, ( PVOID )isr9, 0x8E );
	HalInsertIDTDescriptor( 10, ( PVOID )isr10, 0x8E );
	HalInsertIDTDescriptor( 11, ( PVOID )isr11, 0x8E );
	HalInsertIDTDescriptor( 12, ( PVOID )isr12, 0x8E );
	HalInsertIDTDescriptor( 13, ( PVOID )isr13, 0x8E );
	HalInsertIDTDescriptor( 14, ( PVOID )isr14, 0x8E );
	HalInsertIDTDescriptor( 15, ( PVOID )isr15, 0x8E );
	HalInsertIDTDescriptor( 16, ( PVOID )isr16, 0x8E );
	HalInsertIDTDescriptor( 17, ( PVOID )isr17, 0x8E );
	HalInsertIDTDescriptor( 18, ( PVOID )isr18, 0x8E );
	HalInsertIDTDescriptor( 19, ( PVOID )isr19, 0x8E );
	HalInsertIDTDescriptor( 20, ( PVOID )isr20, 0x8E );
	HalInsertIDTDescriptor( 21, ( PVOID )isr21, 0x8E );
	HalInsertIDTDescriptor( 22, ( PVOID )isr22, 0x8E );
	HalInsertIDTDescriptor( 23, ( PVOID )isr23, 0x8E );
	HalInsertIDTDescriptor( 24, ( PVOID )isr24, 0x8E );
	HalInsertIDTDescriptor( 25, ( PVOID )isr25, 0x8E );
	HalInsertIDTDescriptor( 26, ( PVOID )isr26, 0x8E );
	HalInsertIDTDescriptor( 27, ( PVOID )isr27, 0x8E );
	HalInsertIDTDescriptor( 28, ( PVOID )isr28, 0x8E );
	HalInsertIDTDescriptor( 29, ( PVOID )isr29, 0x8E );
	HalInsertIDTDescriptor( 30, ( PVOID )isr30, 0x8E );
	HalInsertIDTDescriptor( 31, ( PVOID )isr31, 0x8E );

	IoOutputByte( 0x20, 0x11 );
	IoOutputByte( 0xA0, 0x11 );
	IoOutputByte( 0x21, 0x20 );
	IoOutputByte( 0xA1, 0x28 );
	IoOutputByte( 0x21, 0x04 );
	IoOutputByte( 0xA1, 0x02 );
	IoOutputByte( 0x21, 0x01 );
	IoOutputByte( 0xA1, 0x01 );
	IoOutputByte( 0x21, 0x0 );
	IoOutputByte( 0xA1, 0x0 );


	

	HalInsertIDTDescriptor( IRQ0, ( PVOID )irq0, 0x8E );
	HalInsertIDTDescriptor( IRQ1, ( PVOID )irq1, 0x8E );
	HalInsertIDTDescriptor( IRQ2, ( PVOID )irq2, 0x8E );
	HalInsertIDTDescriptor( IRQ3, ( PVOID )irq3, 0x8E );
	HalInsertIDTDescriptor( IRQ4, ( PVOID )irq4, 0x8E );
	HalInsertIDTDescriptor( IRQ5, ( PVOID )irq5, 0x8E );
	HalInsertIDTDescriptor( IRQ6, ( PVOID )irq6, 0x8E );
	HalInsertIDTDescriptor( IRQ7, ( PVOID )irq7, 0x8E );
	HalInsertIDTDescriptor( IRQ8, ( PVOID )irq8, 0x8E );
	HalInsertIDTDescriptor( IRQ9, ( PVOID )irq9, 0x8E );
	HalInsertIDTDescriptor( IRQ10, ( PVOID )irq10, 0x8E );
	HalInsertIDTDescriptor( IRQ11, ( PVOID )irq11, 0x8E );
	HalInsertIDTDescriptor( IRQ12, ( PVOID )irq12, 0x8E );
	HalInsertIDTDescriptor( IRQ13, ( PVOID )irq13, 0x8E );
	HalInsertIDTDescriptor( IRQ14, ( PVOID )irq14, 0x8E );
	HalInsertIDTDescriptor( IRQ15, ( PVOID )irq15, 0x8E );
}