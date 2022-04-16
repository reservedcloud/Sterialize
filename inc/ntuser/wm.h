#pragma once
#include <ntdef.h>
#include <stddef.h>
#include <stdint.h>
#define MAX_WINDOWS 5

VOID
WmUpdateScreen();

VOID WmInitialize();

typedef struct {
    LONG64 Bitmap[16 * 21];
} KiCursorBitmap;


VOID WmUpdateCursor();
extern BOOLEAN WmNeedsUpdate;



VOID WmHandleWindows();