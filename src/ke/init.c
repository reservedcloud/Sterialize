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

#include <hal/pit.h>

#include <io/io.h>

VOID Shutdown(){
    IoOutputWord(0x604, 0x2000);
}

VOID StartMenu(){
    INT WinIDStartMenu = WmCreateWindow("Start Menu", 1, VidScreenHeight - 26 /*taskbar pops*/ - 345, 200, 345, 1, -1, (ULONG64 *)-1);
    INT WinIDButtonShutdown = WmCreateWindow("Shutdown", 4, 60, 120, 50, 4, WinIDStartMenu, (ULONG64 *)Shutdown);
}


VOID KiKernelThread()
{
    

    UCHAR ClockText[512];
    KSYSTEM_TIME CurrentTime;

    INT WinIDTaskbar = WmCreateWindow("taskbar_1221s", 0, VidScreenHeight - 26, VidScreenWidth, VidScreenHeight, 2, -1, (ULONG64 *)-1);
    INT WinIDClockTaskbar = WmCreateWindow("clock_taskbar", VidScreenWidth - 63 - 10, 5, NULL, NULL, 3, WinIDTaskbar, (ULONG64 *)-1); // label
    INT WinIDButtonTaskbar = WmCreateWindow("Start", 4, 4, 44, 20, 4, WinIDTaskbar, (ULONG64 *)StartMenu);
    while (1)
    {
        KeQuerySystemTime(&CurrentTime); // continously update
        sprintf(ClockText, "%d:%d:%d", CurrentTime.Hours, CurrentTime.Minutes, CurrentTime.Seconds);
        WmEditWindowText(WinIDClockTaskbar, ClockText);

        // DbgPrintFmt("sl!KiKernelThread: Cursor at: %d, %d with state: %d", KiSystemCursor.X, KiSystemCursor.Y, KiSystemCursor.State);
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
