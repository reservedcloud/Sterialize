#pragma once
#include <ntdef.h>

VOID
HalInitializePIT(

);

ULONG64
KeQueryTickCount(

);

VOID
Sleep(
	LONG Milliseconds
);