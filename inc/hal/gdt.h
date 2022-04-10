#pragma once
#include <ntdef.h>

struct HAL_GDT_ENTRY
{
	USHORT Limit0;
	USHORT Base0;
	UCHAR Base1;
	UCHAR AccessByte;
	UCHAR Limit1_Flags;
	UCHAR Base2;
} __attribute__( ( packed ) );

struct HAL_GDT_DESCRIPTOR
{
	USHORT Size;
	ULONG64 Offset;
} __attribute__( ( packed ) );

struct HAL_GDT_GENERAL
{
    struct HAL_GDT_ENTRY Entries[6];
}
__attribute__( ( packed ) )
__attribute( ( aligned( 0x1000 ) ) );

VOID HalLoadGDT(struct HAL_GDT_DESCRIPTOR *HAL_DESCRIPTOR);
VOID HalInitializeGDT();