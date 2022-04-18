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

VOID DestroyWindowTest(){
    WmDestroyWindow(WmGetWindowIDByName("Start Menu"));
}

VOID StartMenu(){
    if(WmIsWindowOpenedByName("Start Menu")){
        WmDestroyWindow(WmGetWindowIDByName("Start Menu"));
        return;
    }
        

    
    INT WinIDStartMenu = WmCreateWindow("Start Menu", WIN_TYPE_BORDERLESS_WINDOW, VidScreenHeight - (27-4) /*taskbar pops*/ - 278, 177, 278, 2, -1, 0x0, (ULONG64 *)-1);
    
    INT WinIDButtonShutdown = WmCreateWindow("Shut Down...", 26, 240, 148, 34, WIN_TYPE_BUTTON, WinIDStartMenu, 0x0, (ULONG64 *)DestroyWindowTest);
    
    WmCreateWindow("panel1", 4, 4, 21, 272 - 2, WIN_TYPE_PANEL, WinIDStartMenu, 0x0000A4, (ULONG64 *)-1);
    WmCreateWindow("\nS\nt\ne\nr\ni\na\nl\ni\nz\ne", 3, 272 - 2 - /*calculate text*/(11 * 16), 0, 0, WIN_TYPE_LABEL, WinIDStartMenu, 0xFFFFFFFF, (ULONG64 *)-1);
}

INT initThread = FALSE;
VOID KiKernelThread2()
{
    initThread++;
    while(1){

    }
}

VOID KiKernelThread()
{
    
    INT WinIDHelloWorld = 
        WmCreateWindow("Hello World", 60, 60, 400, 250, WIN_TYPE_WINDOW, -1, 0x0, (ULONG64 *)-1);

        WmCreateWindow("Hello World Label", 30, 30, NULL, NULL, WIN_TYPE_LABEL, WinIDHelloWorld, 0xff000000, (ULONG64 *)-1);
        WmCreateWindow("Hello World Button", 50, 40, 165, 50, WIN_TYPE_BUTTON, WinIDHelloWorld, NULL, (ULONG64 *)-1);
        WmCreateWindow("Hello World Panel", 90, 120, 20, 30, WIN_TYPE_PANEL, WinIDHelloWorld, 0x0000ff, (ULONG64 *)-1);

    UCHAR ClockText[512];
    KSYSTEM_TIME CurrentTime;

    INT WinIDTaskbar = WmCreateWindow("taskbar_1221s", -2, VidScreenHeight - 28, VidScreenWidth + 4, 30, WIN_TYPE_BORDERLESS_WINDOW, -1, 0x0, (ULONG64 *)-1);
    INT WinIDClockTaskbar = WmCreateWindow("clock_taskbar", VidScreenWidth - 63 - 10, 5, NULL, NULL, WIN_TYPE_LABEL, WinIDTaskbar, 0x0, (ULONG64 *)-1); // label
    INT WinIDButtonTaskbar = WmCreateWindow("Start", 4, 4, 55, 22, WIN_TYPE_BUTTON, WinIDTaskbar, 0x0, (ULONG64 *)StartMenu);
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
