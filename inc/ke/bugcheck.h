#pragma once
#include <ntdef.h>

VOID KeBugCheck2(
    ULONG BugCheckCode,
    ULONG BugCheckParameter1,
    ULONG BugCheckParameter2,
    ULONG BugCheckParameter3,
    ULONG BugCheckParameter4,
    ULONG TrapFrame
);

VOID
KeBugCheck(ULONG BugCheckCode);