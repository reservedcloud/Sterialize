#include <ke/clock.h>
#include <io/io.h>

#include <hal/idt.h>

#include <kd/com.h>

#include <ke/i386.h>

KSYSTEM_TIME KiGlobalTime = { 0 };

UCHAR
KiConvertBcd(
	UCHAR ucValue
)
{
	return ( ucValue >> 4 ) * 10 + ( ucValue & 15 );
}

UCHAR
KiSetRTCData(
	INT iValue,
	INT iValue2
)
{
	IoOutputByte( CMOS_IO, iValue );
	IoOutputByte( CMOS_DATA, iValue2 );
}


UCHAR
KiGetRTCData(
	INT iValue
)
{
	IoOutputByte( CMOS_IO, iValue );
	return KiConvertBcd( IoInputByte( CMOS_DATA ) );
}


STATIC VOID
KiUpdateClock( 
	struct InterruptRegisters* regs
)
{
	KiGlobalTime.Seconds = KiGetRTCData( RTC_SECONDS );
	KiGlobalTime.Minutes = KiGetRTCData( RTC_MINUTES );
	KiGlobalTime.Hours	 = KiGetRTCData( RTC_HOURS );
	KiGlobalTime.Days	 = KiGetRTCData( RTC_DAYS );
	KiGlobalTime.Months  = KiGetRTCData( RTC_MONTHS );
	KiGlobalTime.Years	 = KiGetRTCData( RTC_YEARS );

	//"Yes.. we got the interrupt, please send more"
	IoOutputByte( 0x70, 0x0C );
	IoInputByte( 0x71 );

	IoOutputByte( PIC1, PIC_EOI );
}

VOID
KeQuerySystemTime(
	PKSYSTEM_TIME ClockTime
)
{
	*ClockTime = KiGlobalTime;
}

VOID
KeInitializeKernelClock(

)
{
	HalRegisterInterrupt(
	  IRQ8,
	  KiUpdateClock );

	//Turn on our irq
	KeDisableInterrupts( );

	IoOutputByte( 0x70, 0x8B );
	CHAR prev = IoInputByte( 0x71 );
	IoOutputByte( 0x70, 0x8B );
	IoOutputByte( 0x71, prev | 0x40 );

	KeEnableInterrupts( );
}