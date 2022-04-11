#include <ntuser/window.h>
#include <mm/mm.h>
#include <hal/vid.h>
#include <kd/com.h>

INT iLastWindow = 0;

STATIC INT WmScreenWidth;
STATIC INT WmScreenHeight;
INT WmScreenPitch;

STATIC UINT *WmFramebuffer;
STATIC INT WmFramebufferSize;
UINT *WmAntiBuffer;


#define BACKGROUND_COLOR 0x00008080
VOID WmInitialization()
{
    WmScreenWidth = VidScreenWidth;
    WmScreenHeight = VidScreenHeight;
    WmScreenPitch = VidScreenPitch;

    WmFramebuffer = (UINT *)VidFramebufferAddress;
    WmFramebufferSize = WmScreenPitch * WmScreenHeight;
    WmAntiBuffer = malloc(WmFramebufferSize);

    if (!WmAntiBuffer)
        return;

    for (INT i = 0; i < WmFramebufferSize; ++i)
        WmAntiBuffer[i] = BACKGROUND_COLOR;



    // Apply changes
    for (INT i = 0; i < WmFramebufferSize; ++i)
        WmFramebuffer[i] = WmAntiBuffer[i];
}