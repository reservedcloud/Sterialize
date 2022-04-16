#pragma once
#include <ntdef.h>

VOID
WmUpdateScreen();

VOID WmInitialize();

typedef struct {
    LONG64 Bitmap[16 * 21];
} KiCursorBitmap;


VOID WmUpdateCursor();
BOOLEAN NeedsUpdate;