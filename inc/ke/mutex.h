#pragma once
#include <ntdef.h>

typedef volatile BOOLEAN KMUTEX_OBJECT;
#define DEFINE_LOCK(name) static KMUTEX_OBJECT name;

VOID
KeTryAcquireMutex(
	KMUTEX_OBJECT Mutex
);

VOID
KeReleaseMutex(
	KMUTEX_OBJECT Mutex
);