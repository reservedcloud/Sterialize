#include <ntuser/wm.h>
#include <mm/mm.h>
#include <hal/vid.h>
#include <i8042prt/mouse.h>

UINT *KiFramebufferAddress;
INT ScreenWidth = 0;
INT ScreenHeight = 0;
INT ScreenPitch = 0;
INT KiFramebufferSize = 0;

UINT *KiAntiFramebuffer;

BOOLEAN NeedsUpdate = 0;


static KiCursorBitmap CursorBitmap;

VOID
WmSetPixelAntibuffer( INT X, INT Y, INT Color){
    if (X >= ScreenWidth || Y >= ScreenHeight || X < 0 || Y < 0)
        return;

    UINT fb_i = X + (ScreenPitch / sizeof(UINT)) * Y;

    KiAntiFramebuffer[fb_i] = Color;
}

VOID WmUpdateCursor(){
    for (INT x = 0; x < 16; x++)
        for (INT y = 0; y < 21; y++)
            if (CursorBitmap.Bitmap[x * 16 + y] != -1)
                WmSetPixelAntibuffer(KiSystemCursor.X + x,KiSystemCursor.Y + y , CursorBitmap.Bitmap[x * 16 + y]);
}

VOID WmInitialize()
{
    KiFramebufferAddress = (UINT *)VidFramebufferAddress;
    ScreenWidth = VidScreenWidth;
    ScreenHeight = VidScreenHeight;
    ScreenPitch = VidScreenPitch;

    KiFramebufferSize = (ScreenPitch / sizeof(UINT)) * ScreenHeight * sizeof(UINT);

    KiAntiFramebuffer = malloc(KiFramebufferSize);

    NeedsUpdate = 1;
}



VOID WmUpdateScreen()
{
    
    NeedsUpdate = 0;

    for (int i = 0; i < KiFramebufferSize; i++)
        KiAntiFramebuffer[i] = 0x00008080;


    //windows

    //cursor update
    WmUpdateCursor();

    for (int i = 0; i < KiFramebufferSize; i++)
        KiFramebufferAddress[i] = KiAntiFramebuffer[i];
}

#define X 0x00ffffff
#define B 0x00000000
#define o (-1)

static KiCursorBitmap CursorBitmap = {
   	{
    B,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,
	B,B,o,o,o,o,o,o,o,o,o,o,o,o,o,o,
	B,X,B,o,o,o,o,o,o,o,o,o,o,o,o,o,
	B,X,X,B,o,o,o,o,o,o,o,o,o,o,o,o,
	B,X,X,X,B,o,o,o,o,o,o,o,o,o,o,o,
	B,X,X,X,X,B,o,o,o,o,o,o,o,o,o,o,
	B,X,X,X,X,X,B,o,o,o,o,o,o,o,o,o,
	B,X,X,X,X,X,X,B,o,o,o,o,o,o,o,o,
	B,X,X,X,X,X,X,X,B,o,o,o,o,o,o,o,
	B,X,X,X,X,X,X,X,X,B,o,o,o,o,o,o,
	B,X,X,X,X,X,X,X,X,X,B,o,o,o,o,o,
	B,X,X,X,X,X,X,B,B,B,B,B,o,o,o,o,
	B,X,X,X,B,X,X,B,o,o,o,o,o,o,o,o,
	B,X,X,B,B,X,X,B,o,o,o,o,o,o,o,o,
	B,X,B,o,o,B,X,X,B,o,o,o,o,o,o,o,
	B,B,o,o,o,B,X,X,B,o,o,o,o,o,o,o,
	B,o,o,o,o,o,B,X,X,B,o,o,o,o,o,o,
	o,o,o,o,o,o,B,X,X,B,o,o,o,o,o,o,
	o,o,o,o,o,o,o,B,X,X,B,o,o,o,o,o,
	o,o,o,o,o,o,o,B,X,X,B,o,o,o,o,o,
	o,o,o,o,o,o,o,o,B,B,o,o,o,o,o,o,
    }
};

#undef X
#undef B
#undef o

