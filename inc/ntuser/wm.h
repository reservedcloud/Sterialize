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

INT WmCreateWindow(CHAR *Title, INT X, INT Y, INT XSize, INT YSize, INT FormType, INT ParentID, ULONG64* Action);

VOID WmEditWindowText( INT WinID, CHAR* NewText);

VOID WmEditWindowText( INT WinID, CHAR* NewText);
INT WmGetWindowIDByName( CHAR* Title );