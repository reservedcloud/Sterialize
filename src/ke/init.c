#include <stdint.h>
#include <stddef.h>
#include <stivale2.h>

#include <ntdef.h>

#include <hal/gdt.h>
#include <hal/idt.h>
#include <hal/vid.h>

#include <ke/bugcheck.h>
#include <ke/clock.h>

#include <rtl/printf.h>

#include <mm/mm.h>

#include <ps/ps.h>

#include <kd/com.h>

#include <i8042prt/mouse.h>
#include <i8042prt/keyboard.h>

#include <ntuser/wm.h>

VOID KiKernelThread()
{

    DbgPrintFmt("sl!KiKernelThread: Hello!");
    while (1)
    {WmUpdateScreen();
         DbgPrintFmt("sl!KiKernelThread: Cursor at: %d, %d", KiSystemCursor.X, KiSystemCursor.Y);
    }
}

VOID KiSystemStartup(struct stivale2_struct *LoaderBlock)
{
    UCHAR ClockText[512];
    KSYSTEM_TIME CurrentTime;

    VidInitialize(KeTryAcquireTag(LoaderBlock, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID));
    VidSetTextColor(0xFFFFFFFF);
    VidSolidColorFill(0, 0, VidScreenWidth, VidScreenHeight, 0x00000000);
    VidDisplayString("Vid: Video Initialized\n\r");

    HalInitializeGDT();
    VidDisplayString("Hal: Global Descriptor Table Initialized\n\r");

    HalInitializeIDT();
    VidDisplayString("Hal: Interrupt Descriptor Table Initialized\n\r");

    HalInitializePIT();
    VidDisplayString("Hal: Programmable Interval Timer Initialized\n\r");

    // MmInitializePmm(KeTryAcquireTag(LoaderBlock, STIVALE2_STRUCT_TAG_MEMMAP_ID));
    // VidDisplayString("Mm: Physical Memory Manager Initialized\n\r");

    KeInitializeMouse();
    VidDisplayString("Ke: (i8042) PS/2 Mouse Driver Initialized\n\r");

    KeInitializeKeyboard();
    VidDisplayString("Ke: (i8042) PS/2 Keyboard Driver Initialized\n\r");

    KeInitializeKernelClock();
    VidDisplayString("Ke: Kernel Clock Initialized\n\r");
    KeQuerySystemTime(&CurrentTime);
    DbgPrintFmt(ClockText, "Ke: Current time is: %d:%d:%d\n\r", CurrentTime.Hours, CurrentTime.Minutes, CurrentTime.Seconds);
    VidDisplayString(ClockText);

    PsThreadsInit();
    VidDisplayString("Ps: Initialized Threads\n\r");
    PspCreateThread((LPTHREAD_START_ROUTINE)KiKernelThread);

    WmInitialize();
    VidDisplayString("Wm: Initialized Window Manager\n\r");
    
    DbgPrintFmt("sl!KiSystemStartup: All done!");
    for (;;)
        ;
}
