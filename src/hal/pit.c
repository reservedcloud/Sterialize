#include <hal/pit.h>
#include <hal/idt.h>
#include <io/io.h>

#include <ps/ps.h>

#include <kd/com.h>

ULONG64 ulTick = 0;

VOID
Sleep(
	LONG Milliseconds
)
{
	ULONG64 lTick;
	lTick = ulTick + ( Milliseconds );
	
	
	while ( ulTick < lTick )
		ulTick++ ;
}

ULONG64
KeQueryTickCount(

)
{
	return ulTick;
}

STATIC VOID
KiUpdateTick(
	struct InterruptRegisters *reg
)
{
	ulTick++;
	PsScheduleThreads(reg);
}

INT BaseFrequency = 1193180;
INT div = 50;

VOID
HalInitializePIT(

)
{
	HalRegisterInterrupt(
		IRQ0,
		KiUpdateTick
	);
	

	ULONG64 ulDivisor = ( ULONG64 )( BaseFrequency / div );

	UCHAR ucLow = ( UCHAR )( ulDivisor & 0xFF );
	UCHAR ucHigh = ( UCHAR )( ( ulDivisor >> 8 ) & 0xFF );

	IoOutputByte( 0x43, 0x36 );

	IoOutputByte( 0x40, ucLow );
	IoOutputByte( 0x40, ucHigh );

}