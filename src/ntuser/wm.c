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
    for(int i = X; i< EndX; i++){
        for(int j = Y; j< EndY; j++){
            WmSetPixelAntibuffer(i, j, COLOR);
        }
    }
}

VOID WmCreateWindow(CHAR* Title, INT X, INT Y, INT XSize, INT YSize){
    if(memewm_current_window == MAX_WINDOWS)
        return;    

    Windows[memewm_current_window].X = X;
    Windows[memewm_current_window].Y = Y;
    Windows[memewm_current_window].XSize = XSize;
    Windows[memewm_current_window].YSize = YSize;

    char *wtitle = malloc(RtlStringLength(Title) + 1);
    Windows[memewm_current_window].Title = wtitle;
    free(wtitle);

    // UINT* fb = memewm_alloc(XSize * YSize * sizeof(UINT));
    // RtlZeroMemory(fb, XSize * YSize * sizeof(UINT));
    // Windows[memewm_current_window].Framebuffer = (ULONG64)fb;
    for(int i =0; i < memewm_current_window; i++ )
        Windows[i].Focused = 0;

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

    WmCreateWindow("test2", 70,70,100,100);
    WmCreateWindow("test3", 100,70,100,100);
    WmCreateWindow("test4", 10,420,100,100);
    WmCreateWindow("test5", 70,320,100,100);
    WmCreateWindow("test6", 70,700,100,100);
    WmCreateWindow("test7", 10,20,100,100);
    
    WmNeedsUpdate = 1;
}

#define TITLE_BAR_THICKNESS 22

VOID WmSetWindowPos(INT X, INT Y, INT ID){

    Windows[ID].X += X;
    Windows[ID].Y += Y;

    if(!Windows[ID].Focused){
         for(int i =0; i < memewm_current_window; i++ )
            Windows[i].Focused = 0;
        Windows[ID].Focused = 1;
    }
    
}


VOID WmHandleWindows(){
    if(KiSystemCursor.State == 1)//left click
    {
         for(int i =0; i < memewm_current_window; i++ ){
            INT WindowX = Windows[i].X;
            INT WindowY = Windows[i].Y;

            INT WindowXSize = Windows[i].XSize;
            INT WindowYSize = Windows[i].YSize;

            INT WindowWidth = WindowX + WindowXSize;
            INT WindowHeight = WindowY + WindowYSize;

            if((KiCursorPosX > WindowX && KiCursorPosX < WindowWidth) &&
            (KiCursorPosY > WindowY && KiCursorPosY < WindowY + TITLE_BAR_THICKNESS))
            {
                WmSetWindowPos(KiSystemCursor.X - KiCursorPosX ,KiSystemCursor.Y -  KiCursorPosY , i);
            }
         }
    }
}

VOID WmUpdateScreen()
{
    if(!WmNeedsUpdate)
        return;

    WmNeedsUpdate = 0;

    for (int i = 0; i < KiFramebufferSize; i++)
        KiAntiFramebuffer[i] = 0x3A6EA5;


    //windows
    for(int i =0; i < memewm_current_window; i++ ){
        
        UINT* FramebufferWindow = (UINT*)Windows[i].Framebuffer;
        INT WindowX = Windows[i].X;
        INT WindowY = Windows[i].Y;

        INT WindowXSize = Windows[i].XSize;
        INT WindowYSize = Windows[i].YSize;

        INT WindowWidth = WindowX + WindowXSize;
        INT WindowHeight = WindowY + WindowYSize;

        INT WindowEndX = WindowX + WindowWidth;
        INT WindowEndY = WindowY + WindowHeight;

        BOOLEAN Focused = Windows[i].Focused;

        DbgPrintFmt("FramebufferWindow: %p, KiFramebufferAddress: %p, KiAntiFramebuffer: %p\n",FramebufferWindow,KiFramebufferAddress, KiAntiFramebuffer );

        DbgPrintFmt("WindowX: %d, WindowY: %d, WindowXSize: %d, WindowYSize: %d, WindowWidth: %d, WindowHeight: %d, WindowEndX: %d, WindowEndY: %d\n",WindowX,WindowY, WindowXSize, WindowYSize, WindowWidth, WindowHeight, WindowEndX, WindowEndY );

        
        WmPixelFillColor( WindowX, WindowY, WindowWidth, WindowHeight, 0x000000 );
		WmPixelFillColor( WindowX, WindowY, WindowWidth - 1, WindowHeight - 1, 0xDFDFDF );
		WmPixelFillColor( WindowX + 1, WindowY + 1, WindowWidth - 1, WindowHeight - 1, 0x808080 );
		WmPixelFillColor( WindowX + 1, WindowY + 1, WindowWidth - 2, WindowHeight - 2, 0xFFFFFF );
		WmPixelFillColor( WindowX + 2, WindowY + 2, WindowWidth - 2, WindowHeight - 2, 0xC0C0C0 );
		//TitlebAr
		WmPixelFillColor( WindowX + 3, WindowY + 3, WindowWidth - 3, WindowY + TITLE_BAR_THICKNESS - 1, Focused ? 0x000080 : 0x818181 );




        //INT in_x = 1;
        //INT in_y = 10;

        // for (int i = 0; i < WindowXSize * WindowYSize; i++) {
        //     WmSetPixelAntibuffer(in_x++ + WindowX, in_y + WindowY, FramebufferWindow[i]);
        //     if (in_x - 1 == WindowXSize) {
        //         in_y++;
        //         in_x = 1;
        //     }
        // }
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

