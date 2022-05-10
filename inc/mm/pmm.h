#pragma once
#include <stivale2.h>
#include <ntdef.h>
#include <mm/mm.h>

// Might not need.
#define set_entry(b, l, t, entry, counter) \
    entry = (struct stivale2_mmap_entry) { \
        .base = b, \
        .length = l, \
        .type = t, \
        .unused = 0\
    }; \
    counter++;

#define BLOCK_SIZE      4096

enum memmap_types
{
    USABLE = 0x01,
    RESERVED = 0x02,
    ACPI_RECLAIMABLE = 0x03,
    ACPI_NVS = 0x04,
    BAD_MEM = 0x05,
    BOOTLOADER_RECLAIMABLE = 0x1000,
    KERNEL = 0x1001,
    FRAMEBUFFER = 0x1002
};

typedef struct MemoryBlock
{
    UINT* BAddress;
    UINT* EAddress;
    UINT         NakedBAddress;
    UINT         NakedEAddress;
    BOOLEAN      IsUsed;
    UINT         BlockID;
} KSYSTEM_MEMORY;

static KSYSTEM_MEMORY* MiMemoryMap;
static UINT MiCurrentBlockID = 0;
static UINT MiLastAddress = 0;

#define SET_BLOCK(block, faddr, eaddr)              \
    block.BAddress    = (UINT *)faddr;            \
    block.EAddress    = (UINT *)eaddr;            \
    block.NakedBAddress  = faddr;                    \
    block.NakedEAddress  = eaddr;                    \
    block.IsUsed = FALSE;                         \
    block.BlockID = MiCurrentBlockID;             \
    MiCurrentBlockID++;                                               

VOID MiInitializeBlocks( struct stivale2_struct* LB, UINT start_addr );
KSYSTEM_MEMORY MiAccessBlock( UINT block );
UINT* MmAllocPhyisicalMemory( UINT blocks );
