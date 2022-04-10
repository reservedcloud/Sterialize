#include <hal/gdt.h>

struct HAL_GDT_GENERAL HalGDTGeneral = {0};

VOID HalInsertGDTEntry(
    INT iIndex,
    USHORT Limit0,
    USHORT Base0,
    UCHAR Base1,
    UCHAR AccessByte,
    UCHAR Limit1_Flags,
    UCHAR Base2)
{
    HalGDTGeneral.Entries[iIndex].Limit0 = Limit0;
    HalGDTGeneral.Entries[iIndex].Base0 = Base0;
    HalGDTGeneral.Entries[iIndex].Base1 = Base1;
    HalGDTGeneral.Entries[iIndex].AccessByte = AccessByte;
    HalGDTGeneral.Entries[iIndex].Limit1_Flags = Limit1_Flags;
    HalGDTGeneral.Entries[iIndex].Base2 = Base2;
}

VOID HalInitializeGDT(){

    HalInsertGDTEntry(0, 0, 0, 0, 0x00, 0x00, 0);//Null

    HalInsertGDTEntry(1, 0xffff, 0, 0, 0x9a, 0x80, 0);//KernelCode
    HalInsertGDTEntry(2, 0xffff, 0, 0, 0x92, 0x80, 0);//KernelData
    HalInsertGDTEntry(3, 0xffff, 0, 0, 0x9a, 0xcf, 0);//UserNull
    HalInsertGDTEntry(4, 0xffff, 0, 0, 0x92, 0xcf, 0);//UserCode
    HalInsertGDTEntry(5, 0, 0, 0, 0x9a, 0xa2, 0);//UserData

    HalInsertGDTEntry(6, 0, 0, 0, 0x92, 0xa0, 0);//Reserved

    //do the drill
    struct HAL_GDT_DESCRIPTOR Descriptor;
    Descriptor.Size = sizeof(struct HAL_GDT_GENERAL) - 1;
    Descriptor.Offset = ( ULONG64 )&HalGDTGeneral;
    HalLoadGDT( &Descriptor );

}