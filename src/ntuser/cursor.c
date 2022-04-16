#include <ntuser/cursor.h>

UINT KiCursorPosX = 0;
UINT KiCursorPosY = 0;


VOID
NtSetCursorPos(
	INT X,
	INT Y
)
{
    KiCursorPosX = X;
    KiCursorPosY = Y;
}