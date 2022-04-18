#pragma once
#include <ntdef.h>
#include <stddef.h>
#include <stdint.h>
#define MAX_WINDOWS 32

VOID
WmUpdateScreen();

VOID WmInitialize();

typedef struct {
    LONG64 Bitmap[16 * 21];
} KiCursorBitmap;


VOID WmUpdateCursor();
extern BOOLEAN WmNeedsUpdate;



VOID WmHandleWindows();

INT WmCreateWindow(CHAR *Title,
    INT X,
    INT Y,
    INT XSize,
    INT YSize,
    INT FormType,
    INT ParentID,
    UINT HexColor,
    ULONG64* Action);

VOID WmEditWindowText( INT WinID, CHAR* NewText);

VOID WmEditWindowText( INT WinID, CHAR* NewText);
INT WmGetWindowIDByName( CHAR* Title );

VOID WmDestroyWindow(INT WinID);

BOOLEAN WmIsWindowOpenedByName(CHAR* Title);

#define WIN_TYPE_WINDOW 1
#define WIN_TYPE_BORDERLESS_WINDOW 2
#define WIN_TYPE_LABEL 3
#define WIN_TYPE_BUTTON 4
#define WIN_TYPE_PANEL 5