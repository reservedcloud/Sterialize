#include <ke/bugcheck.h>
#include <hal/vid.h>
#include <rtl/printf.h>
#include <ke/i386.h>
#include <ke/mutex.h>
#include <kd/com.h>
#include <hal/idt.h>

DEFINE_LOCK(BugCheckMutex);

VOID
KeBugCheck(ULONG BugCheckCode)
{
    KeBugCheck2(BugCheckCode, 0, 0, 0, 0, NULL);
}

VOID KeBugCheck2(
    ULONG BugCheckCode,
    ULONG BugCheckParameter1,
    ULONG BugCheckParameter2,
    ULONG BugCheckParameter3,
    ULONG BugCheckParameter4,
    ULONG TrapFrame
){

    KeTryAcquireMutex(BugCheckMutex);

    UCHAR StopCode[ 1024 ];
    VidSolidColorFill(0, 0, VidScreenWidth, VidScreenHeight, 0x0000FF);

    sprintf(
		StopCode,
		"\n*** Fatal System Error: 0x%08lx\n\r"
        "                       (0x%p,0x%p,0x%p,0x%p)\n\n",
		BugCheckCode,
		BugCheckParameter1,
		BugCheckParameter2,
		BugCheckParameter3,
		BugCheckParameter4 );
    
    DbgPrintFmt(StopCode);
    DbgPrintFmt("Translated: %s at %08lx",exception_messages[ BugCheckCode ], BugCheckParameter2);
    VidSetTextColor(0xFFFFFFFF);
    VidDisplayString(StopCode);

    KeDisableInterrupts();  
    HalHaltSystem();

    KeReleaseMutex(BugCheckMutex); //kinda useless as the os will just hang..
}