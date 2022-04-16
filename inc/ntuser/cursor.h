#pragma once
#include <ntdef.h>

UINT KiCursorPosX;
UINT KiCursorPosY;

VOID
NtSetCursorPos(
	INT X,
	INT Y
);