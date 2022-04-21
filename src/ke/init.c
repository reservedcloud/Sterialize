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

#include <hal/pit.h>

#include <io/io.h>

#include <ps/ps.h>

VOID KiKernelThread()
{
    DbgPrintFmt("Hello from KiKernelThread()\n");
    for (;;)
        ;
}

VOID KiSystemStartup(struct stivale2_struct *LoaderBlock)
{

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

    KeInitializeMouse();
    VidDisplayString("Ke: (i8042) PS/2 Mouse Driver Initialized\n\r");

    KeInitializeKeyboard();
    VidDisplayString("Ke: (i8042) PS/2 Keyboard Driver Initialized\n\r");

    KeInitializeKernelClock();
    VidDisplayString("Ke: Kernel Clock Initialized\n\r");




    DbgPrintFmt("sl!KiSystemStartup: All done!");

    for (;;)
        ;
}
