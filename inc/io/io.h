#pragma once
#include <ntdef.h>

VOID IoOutputByte( USHORT port, UCHAR data );
VOID IoOutputWord( USHORT port, USHORT data );
VOID IoOutputDword( USHORT port, UINT data );
UCHAR IoInputByte( USHORT port );
USHORT IoInputWord( USHORT port );
UINT IoInputDword( USHORT port );