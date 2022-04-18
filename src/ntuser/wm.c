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
    CHAR *Title;
    INT X;
    INT Y;
    INT XSize;
    INT YSize;
    ULONG64 Framebuffer;
    BOOLEAN Focused;
    INT FormType;
    INT ParentID;
    UINT HexColor;
    ULONG64* ActionAddress;
} KSYSTEM_WINDOW;

KSYSTEM_WINDOW Windows[32];

VOID WmDisplayChar(
    CHAR cChararcter,
    UINT uiX,
    UINT uiY,
    UINT uiColor)
{
    ULONG64 ulOffset;
    UCHAR ucDisplay;

    for (INT iy = uiY; uiY + 16 > iy; iy++)
    {
        for (INT ix = uiX; uiX + 8 > ix; ix++)
        {
            ucDisplay = KiDisplayFont[(INT)cChararcter * 16 + iy - uiY];

            if (ucDisplay & (1 << (7 - (ix - uiX))) &&
                ix >= 0 && iy >= 0 && ix < VidScreenWidth && iy < VidScreenHeight)
            {

                WmSetPixelAntibuffer(ix, iy, uiColor);
            }
        }
    }
}

VOID WmDisplayStringXY(PUCHAR String, ULONG Left, ULONG Top, UINT Color)
{
    ULONG BackColor;
    ULONG OrigLeft = Left;
    /*
     * If the caller wanted transparent, then send the special value (16),
     * else use our default and call the helper routine.
     */
    

    /* Loop every character and adjust the position */
    for (; *String; ++String, Left += 8)
    {
        /* Display a character */
        if(*String == '\n'){
            Left = OrigLeft;
            Top += 16;
        }
        WmDisplayChar(*String, Left, Top, Color);
    }
}

static void *memewm_alloc(size_t size)
{
    uint8_t *ptr = malloc(size);

    if (!ptr)
        return (void *)0;

    for (size_t i = 0; i < size; i++)
        ptr[i] = 0;

    return (void *)ptr;
}

#define INVERTED_BLT 0x1337

VOID WmSetPixelAntibuffer(INT X, INT Y, INT Color)
{
    if (X >= ScreenWidth || Y >= ScreenHeight || X < 0 || Y < 0)
        return;

    UINT fb_i = X + (ScreenPitch / sizeof(UINT)) * Y;

    if (Color == INVERTED_BLT)
        KiAntiFramebuffer[fb_i] = ~KiAntiFramebuffer[fb_i];
    else
        KiAntiFramebuffer[fb_i] = Color;
}

VOID WmUpdateCursor()
{
    for (INT x = 0; x < 16; x++)
        for (INT y = 0; y < 21; y++)
            if (CursorBitmap.Bitmap[y * 16 + x] != -1)
                WmSetPixelAntibuffer(KiCursorPosX + x, KiCursorPosY + y, CursorBitmap.Bitmap[y * 16 + x]);
}

VOID WmPixelFillColor(INT X, INT Y, INT EndX, INT EndY, UINT COLOR)
{
    for (int i = X; i < EndX; i++)
    {
        for (int j = Y; j < EndY; j++)
        {
            WmSetPixelAntibuffer(i, j, COLOR);
        }
    }
}

INT WmCreateWindow(CHAR *Title, INT X, INT Y, INT XSize, INT YSize, INT FormType, INT ParentID, UINT HexColor, ULONG64* Action)
{
    if (memewm_current_window == MAX_WINDOWS)
        return;

    INT backup_memewm_current_window = memewm_current_window;
    for(int i = 0; i< memewm_current_window; i++){ //check for any available window
        if(Windows[i].FormType == 0){
            memewm_current_window = i;
            break;
        }

    }
    /////////
    Windows[memewm_current_window].X = X;
    Windows[memewm_current_window].Y = Y;
    Windows[memewm_current_window].XSize = XSize;
    Windows[memewm_current_window].YSize = YSize;
    Windows[memewm_current_window].HexColor = HexColor;

    Windows[memewm_current_window].FormType = FormType;
    Windows[memewm_current_window].ParentID = ParentID;

    Windows[memewm_current_window].ActionAddress = Action;

    // char *wtitle = (char*)malloc(RtlStringLength(Title) + 1);
    // strcpy(wtitle, Title);

    // strcpy(Windows[memewm_current_window].Title, wtitle);

    // DbgPrintFmt("wtitle %p, Title: %p, result wtitle: %s\n",wtitle, Title, wtitle );

    // DbgPrintFmt("Windows[memewm_current_window].Title: s\n", Windows[memewm_current_window].Title);

    Windows[memewm_current_window].Title = Title;

    // UINT* fb = memewm_alloc(XSize * YSize * sizeof(UINT));
    // RtlZeroMemory(fb, XSize * YSize * sizeof(UINT));
    // Windows[memewm_current_window].Framebuffer = (ULONG64)fb;
    for (int i = 0; i < memewm_current_window; i++)
        Windows[i].Focused = 0;

    Windows[memewm_current_window].Focused = 1;

    if(backup_memewm_current_window == memewm_current_window){
        memewm_current_window++;

        DbgPrintFmt("Generated window : (id)%d\n", memewm_current_window - 1);
        return memewm_current_window - 1;
    }
    else{
        INT returnVar = memewm_current_window;
        memewm_current_window = backup_memewm_current_window;//revert

        DbgPrintFmt("Found old slot : (id)%d\n", returnVar);
        return returnVar;
    }
    
}

VOID WmInitialize()
{
    KiFramebufferAddress = (UINT *)VidFramebufferAddress;
    ScreenWidth = VidScreenWidth;
    ScreenHeight = VidScreenHeight;
    ScreenPitch = VidScreenPitch;

    KiFramebufferSize = (ScreenPitch / sizeof(UINT)) * ScreenHeight * sizeof(UINT);

    KiAntiFramebuffer = malloc(KiFramebufferSize);

    WmNeedsUpdate = 1;
}

#define TITLE_BAR_THICKNESS 18

VOID WmSetWindowPosCursorBased(INT X, INT Y, INT ID)
{

    Windows[ID].X += X;
    Windows[ID].Y += Y;

    if (!Windows[ID].Focused)
    {
        for (int i = 0; i < memewm_current_window; i++)
            Windows[i].Focused = 0;
        Windows[ID].Focused = 1;
    }
}

BOOLEAN IsWindowDragged = FALSE;
INT WindowDraggedID = 0;
VOID WmHandleWindows()
{
    if (KiSystemCursor.State == 1) // left click
    {
        for (int i = 0; i < memewm_current_window; i++)
        {
            INT WindowX = Windows[i].X;
            INT WindowY = Windows[i].Y;

            INT WindowXSize = Windows[i].XSize;
            INT WindowYSize = Windows[i].YSize;

            INT WindowWidth = WindowX + WindowXSize;
            INT WindowHeight = WindowY + WindowYSize;

            INT ParentID = Windows[i].ParentID;
            /// buttons
            INT MainWindowX = Windows[ParentID].X;
            INT MainWindowY = Windows[ParentID].Y;

            INT ActualX = MainWindowX + WindowX;
            INT ActualY = MainWindowY + WindowY;

            INT ActualWidth = ActualX + WindowXSize;
            INT ActualHeight = ActualY + WindowYSize;

            if (Windows[i].FormType == 4)
            {
                if ((KiCursorPosX > ActualX && KiCursorPosX < ActualWidth) &&
                    (KiCursorPosY > ActualY && KiCursorPosY < ActualHeight))
                {
                    IsWindowDragged = TRUE;
                    WindowDraggedID = i;

                    //Action
                    ULONG64* AddyAddress = (ULONG64*)Windows[i].ActionAddress;
                    typedef VOID func(void);
                    func* f = (func*)AddyAddress;
                    if(AddyAddress != (ULONG64*)-1){//action
                        f();
                    }
                    
                }
            }

            if (Windows[i].FormType == 1)
            {
                if ((KiCursorPosX > WindowX && KiCursorPosX < WindowWidth /*close button calculation*/ - 6 - 16 ) &&
                    (KiCursorPosY > WindowY && KiCursorPosY < WindowY + TITLE_BAR_THICKNESS))
                {
                    if (i != WindowDraggedID && WindowDraggedID != -1)
                        break;

                    IsWindowDragged = TRUE;
                    WindowDraggedID = i;
                    WmSetWindowPosCursorBased(KiSystemCursor.X - KiCursorPosX, KiSystemCursor.Y - KiCursorPosY, i);
                }
                //terminate window
                 if ((KiCursorPosX > WindowWidth /*close button calculation*/ - 6 - 16 &&
                    KiCursorPosX < WindowWidth /*close button calculation*/ - 6 ) &&
                    (KiCursorPosY > WindowY && KiCursorPosY < WindowY + TITLE_BAR_THICKNESS))
                    {
                        WmDestroyWindow(i);
                    }
                // else if((KiCursorPosX > WindowX && KiCursorPosX < WindowWidth) &&
                // (KiCursorPosY > WindowY + TITLE_BAR_THICKNESS && KiCursorPosY < WindowHeight) && !Windows[i].Focused)
                //    break;
            }
        }
    }
    else
    {
        IsWindowDragged = FALSE;
        WindowDraggedID = -1;
    }
}

VOID WmEditWindowText(INT WinID, CHAR *NewText)
{
    if( Windows[WinID].FormType == 0) //check for invalid window
        return;

    Windows[WinID].Title = NewText;
    WmNeedsUpdate = 1;
}

INT WmGetWindowIDByName(CHAR *Title)
{
    for (int i = 0; i < memewm_current_window; i++)
    {
        if( Windows[i].FormType != 0) //check for invalid window
        {
            if (Windows[i].Title == Title){
                return i;
            }
        }
    }
    return -1;
}

VOID WmDrawLabels(INT WinID, INT X, INT Y, INT ParentID)
{
    if(IsWindowDragged && Windows[WinID].ParentID == WindowDraggedID)
        return;

    INT MainWindowX = Windows[ParentID].X;
    INT MainWindowY = Windows[ParentID].Y;

    INT MainWindowWidth = Windows[ParentID].XSize + MainWindowX;
    INT MainWindowHeight = Windows[ParentID].YSize + MainWindowY;

    INT ItemWindowX = X;
    INT ItemWindowY = Y;
    //////

    if (Windows[WinID].FormType == 3) // label type
    {
        WmDisplayStringXY(Windows[WinID].Title, MainWindowX + ItemWindowX, MainWindowY + ItemWindowY, Windows[WinID].HexColor);
    }
}

//VOID WmDraw

VOID WmDrawButtons(INT WinID, INT X, INT Y, INT Width, INT Height, INT ParentID)
{   
    if(IsWindowDragged && Windows[WinID].ParentID == WindowDraggedID)
        return;

    INT MainWindowX = Windows[ParentID].X;
    INT MainWindowY = Windows[ParentID].Y;

    INT MainWindowWidth = Windows[ParentID].XSize + MainWindowX;
    INT MainWindowHeight = Windows[ParentID].YSize + MainWindowY;

    INT ItemWindowX = X;
    INT ItemWindowY = Y;
    //////////
    INT ActualX = MainWindowX + ItemWindowX;
    INT ActualY = MainWindowY + ItemWindowY;

    INT ActualWidth = ActualX + Width;
    INT ActualHeight = ActualY + Height;

    if (Windows[WinID].FormType == 4)
    { // buttons


        INT iCharWidth = 8;
        INT iCharHeight = 16;

        INT iMiddleX = ActualX / 2 + ( ActualWidth / 2 ) - //cut the screen in half, minus
            ( RtlStringLength( Windows[WinID].Title ) * ( iCharWidth / 2 ) );//letter lenght, multiplied with
                                                            //letter font width devided by 2 (in half)

        INT iMiddleY = ActualY / 2 + ( ActualHeight / 2 ) -
            ( RtlStringLength( Windows[WinID].Title ) / ( iCharHeight / 2 ) );


        if (IsWindowDragged && WindowDraggedID == WinID)
        {
            WmPixelFillColor(ActualX, ActualY, ActualWidth, ActualHeight, 0xFFFFFF);
            WmPixelFillColor(ActualX, ActualY, ActualWidth - 1, ActualHeight - 1, 0x808080);
            WmPixelFillColor(ActualX + 1, ActualY + 1, ActualWidth - 1, ActualHeight - 1, 0xDFDFDF);
            WmPixelFillColor(ActualX + 1, ActualY + 1, ActualWidth - 2, ActualHeight - 2, 0x000000);
            WmPixelFillColor(ActualX + 2, ActualY + 2, ActualWidth - 2, ActualHeight - 2, 0xC0C0C0);
            WmDisplayStringXY(Windows[WinID].Title, iMiddleX + 2, iMiddleY - 10, 0x00000000);
        }
        else
        {
            WmPixelFillColor(ActualX, ActualY, ActualWidth, ActualHeight, 0x000000);
            WmPixelFillColor(ActualX, ActualY, ActualWidth - 1, ActualHeight - 1, 0xDFDFDF);
            WmPixelFillColor(ActualX + 1, ActualY + 1, ActualWidth - 1, ActualHeight - 1, 0x808080);
            WmPixelFillColor(ActualX + 1, ActualY + 1, ActualWidth - 2, ActualHeight - 2, 0xFFFFFF);
            WmPixelFillColor(ActualX + 2, ActualY + 2, ActualWidth - 2, ActualHeight - 2, 0xC0C0C0);
            WmDisplayStringXY(Windows[WinID].Title, iMiddleX, iMiddleY - 10, 0x00000000);


        }
    }
}

BOOLEAN WmIsWindowOpenedByName(CHAR* Title){
    INT WinID = WmGetWindowIDByName(Title);

    if(WinID == -1)
        return FALSE;

    return TRUE;
}

VOID WmBasicWindows(int WinID, int WindowX, int WindowY, int WindowWidth, int WindowHeight, int Focused){
    WmPixelFillColor(WindowX, WindowY, WindowWidth, WindowHeight, 0x000000);
    WmPixelFillColor(WindowX, WindowY, WindowWidth - 1, WindowHeight - 1, 0xDFDFDF);
    WmPixelFillColor(WindowX + 1, WindowY + 1, WindowWidth - 1, WindowHeight - 1, 0x808080);
    WmPixelFillColor(WindowX + 1, WindowY + 1, WindowWidth - 2, WindowHeight - 2, 0xFFFFFF);
    WmPixelFillColor(WindowX + 2, WindowY + 2, WindowWidth - 2, WindowHeight - 2, 0xC0C0C0);
    // TitlebAr
    WmPixelFillColor(WindowX + 4, WindowY + 4, WindowWidth - 4, WindowY + 4 + TITLE_BAR_THICKNESS, Focused ? 0x0000AA : 0x868A8E);
    WmDisplayStringXY(Windows[WinID].Title, WindowX + 6, WindowY + 4, 0xffffffff);

    //Close Button
    INT DefaultCloseWidthHeightPrefix = 6;

    INT CloseX = WindowWidth - DefaultCloseWidthHeightPrefix - 16;
    INT CloseWidth = WindowWidth - DefaultCloseWidthHeightPrefix;

    INT CloseY = WindowY + DefaultCloseWidthHeightPrefix;
    INT CloseHeight = WindowY + DefaultCloseWidthHeightPrefix + 14;
    WmPixelFillColor(CloseX, CloseY, CloseWidth, CloseHeight, 0x000000);
    WmPixelFillColor(CloseX, CloseY, CloseWidth - 1, CloseHeight - 1, 0xDFDFDF);
    WmPixelFillColor(CloseX + 1, CloseY + 1, CloseWidth - 1, CloseHeight - 1, 0x808080);
    WmPixelFillColor(CloseX + 1, CloseY + 1, CloseWidth - 2, CloseHeight - 2, 0xFFFFFF);
    WmPixelFillColor(CloseX + 2, CloseY + 2, CloseWidth - 2, CloseHeight - 2, 0xC0C0C0);

    WmDisplayStringXY("x", CloseX + 4, CloseY - 3, 0x00000000);
}

VOID WmDrawPanels(INT WinID, INT X, INT Y, INT Width, INT Height, INT ParentID, UINT Color){
    if(IsWindowDragged && Windows[WinID].ParentID == WindowDraggedID)
        return;

    INT MainWindowX = Windows[ParentID].X;
    INT MainWindowY = Windows[ParentID].Y;

    INT MainWindowWidth = Windows[ParentID].XSize + MainWindowX;
    INT MainWindowHeight = Windows[ParentID].YSize + MainWindowY;

    INT ItemWindowX = X;
    INT ItemWindowY = Y;
    //////////
    INT ActualX = MainWindowX + ItemWindowX;
    INT ActualY = MainWindowY + ItemWindowY;

    INT ActualWidth = ActualX + Width;
    INT ActualHeight = ActualY + Height;

    if (Windows[WinID].FormType == 5)
    { // panels

        WmPixelFillColor(ActualX, ActualY, ActualWidth, ActualHeight, Color);

    }
}

VOID WmDrawWindows(int WinID, int WindowX, int WindowY, int WindowWidth, int WindowHeight, int Focused)
{
    if (Windows[WinID].FormType == 1)
    {
        
        if (IsWindowDragged)
        {

            if (WindowDraggedID != WinID)
            {
                WmBasicWindows(WinID, WindowX, WindowY, WindowWidth, WindowHeight, Focused);
            }
            else
            {
                WmPixelFillColor(WindowX + 1, WindowY, WindowWidth - 1, WindowY + 1, INVERTED_BLT);           // top
                WmPixelFillColor(WindowX + 1, WindowHeight - 1, WindowWidth - 1, WindowHeight, INVERTED_BLT); // bottom

                WmPixelFillColor(WindowX, WindowY, WindowX + 1, WindowHeight, INVERTED_BLT);         // left line
                WmPixelFillColor(WindowWidth - 1, WindowY, WindowWidth, WindowHeight, INVERTED_BLT); // right line
            }
        }
        else
        {
            WmBasicWindows(WinID, WindowX, WindowY, WindowWidth, WindowHeight, Focused);
        }
    }
    else if (Windows[WinID].FormType == 2) // formtype 2 - borderless
    {
        WmPixelFillColor(WindowX, WindowY, WindowWidth, WindowHeight, 0x000000);
        WmPixelFillColor(WindowX, WindowY, WindowWidth - 1, WindowHeight - 1, 0xDFDFDF);
        WmPixelFillColor(WindowX + 1, WindowY + 1, WindowWidth - 1, WindowHeight - 1, 0x808080);
        WmPixelFillColor(WindowX + 1, WindowY + 1, WindowWidth - 2, WindowHeight - 2, 0xDFDFDF);
        WmPixelFillColor(WindowX + 2, WindowY + 2, WindowWidth - 2, WindowHeight - 2, 0xC3C7CB);
    }
}

VOID WmDestroyWindow(INT WinID){
 DbgPrintFmt("Destroyed main window : (id)%d\n", WinID);
        //check for existing windows
        if(Windows[WinID].FormType != 0)//if there is no form type (type = 0) == not existing window
        {
            Windows[WinID].FormType = 0;//deintialize
            

            //remove elements inside the window, as like buttons, labels
            for(int i =0; i < memewm_current_window; i++ ){//get all elements
                if(Windows[i].FormType != 1 && Windows[i].FormType != 2){ //check if the shit aint a window or a
                                                                          //borderless window
                    if(Windows[i].ParentID == WinID){
                        DbgPrintFmt("Destroyed element window : (id)%d\n", i);
                        Windows[i].FormType =0; //deintialize
                   
                    }
                }
            }
        }
}

VOID WmUpdateScreen()
{
    if (!WmNeedsUpdate)
        return;

    WmNeedsUpdate = 0;

    for (int i = 0; i < KiFramebufferSize; i++)
        KiAntiFramebuffer[i] = 0x55AAAA;

    // windows
    for (int i = 0; i < memewm_current_window; i++)
    {
        UINT *FramebufferWindow = (UINT *)Windows[i].Framebuffer;
        INT WindowX = Windows[i].X;
        INT WindowY = Windows[i].Y;

        INT WindowXSize = Windows[i].XSize;
        INT WindowYSize = Windows[i].YSize;

        INT WindowWidth = WindowX + WindowXSize;
        INT WindowHeight = WindowY + WindowYSize;

        INT WindowEndX = WindowX + WindowWidth;
        INT WindowEndY = WindowY + WindowHeight;

        BOOLEAN Focused = Windows[i].Focused;

        WmDrawWindows(i, WindowX, WindowY, WindowWidth, WindowHeight, Focused);
        WmDrawLabels(i, WindowX, WindowY, Windows[i].ParentID);
        WmDrawButtons(i, WindowX, WindowY, WindowXSize, WindowYSize, Windows[i].ParentID);
        WmDrawPanels(i,WindowX, WindowY, WindowXSize, WindowYSize, Windows[i].ParentID, Windows[i].HexColor );
        // HOLY SHIT THIS TAKES A LOT OF RAM
        //  DbgPrintFmt("FramebufferWindow: %p, KiFramebufferAddress: %p, KiAntiFramebuffer: %p\n",FramebufferWindow,KiFramebufferAddress, KiAntiFramebuffer );
        //  DbgPrintFmt("WindowX: %d, WindowY: %d, WindowXSize: %d, WindowYSize: %d, WindowWidth: %d, WindowHeight: %d, WindowEndX: %d, WindowEndY: %d\n",WindowX,WindowY, WindowXSize, WindowYSize, WindowWidth, WindowHeight, WindowEndX, WindowEndY );

        // INT in_x = 1;
        // INT in_y = 10;

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
