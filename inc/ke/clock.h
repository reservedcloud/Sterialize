#pragma once
#include <ntdef.h>

#define CMOS_IO		0x70
#define CMOS_DATA	0x71

#define RTC_A		0x0A
#define RTC_B		0x0B
#define RTC_C		0x0C
#define RTC_D		0x0D

#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS	0x04
#define RTC_DAYS	0x07
#define RTC_MONTHS	0x08
#define RTC_YEARS	0x09

typedef struct _KSYSTEM_TIME
{
	INT Seconds;
	INT Minutes;
	INT Hours;
	INT Days;
	INT Months;
	INT Years;
} KSYSTEM_TIME, *PKSYSTEM_TIME;



VOID
KeInitializeKernelClock(

);

VOID
KeQuerySystemTime(
	PKSYSTEM_TIME ClockTime
);