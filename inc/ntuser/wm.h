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

typedef struct window_t {
    int id;
    char *title;
    int x;
    int y;
    int x_size;
    int y_size;
    uint32_t *framebuffer;
    struct window_t *next;
} window_t;