#pragma once
#include <ntdef.h>

VOID
KeInitializeKeyboard();

typedef struct {
    CHAR KbdLastCharacter;
}KSYSTEM_KEYBOARD, *PKSYSTEM_KEYBOARD;

KSYSTEM_KEYBOARD KbdSystemKeyboard;