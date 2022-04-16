#include <ntuser/wm.h>
#include <mm/mm.h>
#include <hal/vid.h>
#include <i8042prt/mouse.h>
#include <ntuser/cursor.h>


UINT *KiFramebufferAddress;
INT ScreenWidth = 0;
INT ScreenHeight = 0;
INT ScreenPitch = 0;
INT KiFramebufferSize = 0;

UINT *KiAntiFramebuffer;

BOOLEAN WmNeedsUpdate = 0;
static int memewm_current_window = 0;

static KiCursorBitmap CursorBitmap;

typedef struct 
{   
    CHAR* Title;
    INT X;
    INT Y;
    INT XSize;
    INT YSize;
    ULONG64 Framebuffer;
    BOOLEAN Focused;
}KSYSTEM_WINDOW;

KSYSTEM_WINDOW Windows[32];


static void *memewm_alloc(size_t size) {
    uint8_t *ptr = malloc(size);

    if (!ptr)
        return (void *)0;

    for (size_t i = 0; i < size; i++)
        ptr[i] = 0;

    return (void *)ptr;
}

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
            if (CursorBitmap.Bitmap[y * 16 + x] != -1)
                WmSetPixelAntibuffer(KiCursorPosX + x, KiCursorPosY + y, CursorBitmap.Bitmap[y * 16 + x]);
}


VOID
WmPixelFillColor(INT X, INT Y, INT EndX, INT EndY, UINT COLOR){
    for(int i = Y; i< EndX; i++){
        for(int j = Y; j< EndY; j++){
            WmSetPixelAntibuffer(i, j, COLOR);
        }
    }
}

VOID WmCreateWindow(CHAR* Title, INT X, INT Y, INT XSize, INT YSize){
    

    Windows[memewm_current_window].X = X;
    Windows[memewm_current_window].Y = Y;
    Windows[memewm_current_window].XSize = XSize;
    Windows[memewm_current_window].YSize = YSize;

    char *wtitle = malloc(RtlStringLength(Title) + 1);
    Windows[memewm_current_window].Title = wtitle;
    free(wtitle);

    UINT* fb = memewm_alloc(XSize * YSize * sizeof(UINT));
    RtlZeroMemory(fb, XSize * YSize * sizeof(UINT));
    Windows[memewm_current_window].Framebuffer = (ULONG64)fb;
    

    Windows[memewm_current_window].Focused = 1;

    memewm_current_window++;
}

VOID WmInitialize()
{
    KiFramebufferAddress = (UINT *)VidFramebufferAddress;
    ScreenWidth = VidScreenWidth;
    ScreenHeight = VidScreenHeight;
    ScreenPitch = VidScreenPitch;

    KiFramebufferSize = (ScreenPitch / sizeof(UINT)) * ScreenHeight * sizeof(UINT);

    KiAntiFramebuffer = malloc(KiFramebufferSize);

    WmCreateWindow("test", 10,10,100,100);

    WmNeedsUpdate = 1;
}


VOID WmUpdateScreen()
{
    if(!WmNeedsUpdate)
        return;

    WmNeedsUpdate = 0;

    for (int i = 0; i < KiFramebufferSize; i++)
        KiAntiFramebuffer[i] = 0x00008080;


    //windows
    for(int i =0; i < memewm_current_window; i++ ){
        
        UINT* FramebufferWindow = (UINT*)Windows[i].Framebuffer;
        INT WindowX = Windows[i].X;
        INT WindowY = Windows[i].Y;

        INT WindowXSize = Windows[i].XSize;
        INT WindowYSize = Windows[i].YSize;

        INT WindowWidth = WindowX + WindowXSize;
        INT WindowHeight = WindowY + WindowYSize;

        

        DbgPrintFmt("FramebufferWindow: %p, KiFramebufferAddress: %p, KiAntiFramebuffer: %p\n",FramebufferWindow,KiFramebufferAddress, KiAntiFramebuffer );

        WmPixelFillColor(WindowX, WindowY, WindowWidth, WindowHeight, 0xFFFFFFFF );

        INT in_x = 1;
        INT in_y = 10;

        for (int i = 0; i < WindowXSize * WindowYSize; i++) {
            WmSetPixelAntibuffer(in_x++ + WindowX, in_y + WindowY, FramebufferWindow[i]);
            if (in_x - 1 == WindowXSize) {
                in_y++;
                in_x = 1;
            }
        }
    }

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

