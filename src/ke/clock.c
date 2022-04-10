#include <ke/clock.h>
#include <io/io.h>

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

)
{
	KiGlobalTime.Seconds = KiGetRTCData( RTC_SECONDS );
	KiGlobalTime.Minutes = KiGetRTCData( RTC_MINUTES );
	KiGlobalTime.Hours	 = KiGetRTCData( RTC_HOURS );
	KiGlobalTime.Days	 = KiGetRTCData( RTC_DAYS );
	KiGlobalTime.Months  = KiGetRTCData( RTC_MONTHS );
	KiGlobalTime.Years	 = KiGetRTCData( RTC_YEARS );
}

VOID
KeQuerySystemTime(
	PKSYSTEM_TIME ClockTime
)
{
	KiUpdateClock( );
	*ClockTime = KiGlobalTime;
}

VOID
KeInitializeKernelClock(

)
{
	KiUpdateClock( );
}