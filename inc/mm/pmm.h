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

enum memmap_types {
    USABLE                      = 0x01,
    RESERVED                    = 0x02,    
    ACPI_RECLAIMABLE            = 0x03,
    ACPI_NVS                    = 0x04,
    BAD_MEM                     = 0x05,
    BOOTLOADER_RECLAIMABLE      = 0x1000,
    KERNEL                      = 0x1001,
    FRAMEBUFFER                 = 0x1002
};

typedef struct MemoryBlock {
    UINT        *b_address;
    UINT        *e_address;
    UINT         naked_baddr;
    UINT         naked_eaddr;  
    BOOLEAN      is_used;
    UINT         block_id;
} _MemoryBlock;

static _MemoryBlock *mem_map;
static UINT current_block_id = 0;
static UINT last_addr = 0;

#define SET_BLOCK(block, faddr, eaddr)              \
    block.b_address    = (UINT *)faddr;            \
    block.e_address    = (UINT *)eaddr;            \
    block.naked_baddr  = faddr;                    \
    block.naked_eaddr  = eaddr;                    \
    block.is_used = FALSE;                         \
    block.block_id = current_block_id;             \
    current_block_id++;                                               

void init_blocks(struct stivale2_struct *LB, UINT start_addr);
_MemoryBlock access_block(UINT block);
UINT *PMM_Allocate(UINT blocks);