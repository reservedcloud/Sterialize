#include <mm/pmm.h>

void init_blocks(struct stivale2_struct *LB, UINT start_addr)
{
    struct stivale2_struct_tag_memmap *a = KeTryAcquireTag(LB, STIVALE2_STRUCT_TAG_MEMMAP_ID);
 
    struct stivale2_struct s = {
        .tags = (unsigned long)&a
    };

    mem_map = (_MemoryBlock *)start_addr;
    SET_BLOCK(mem_map[current_block_id], start_addr, start_addr+BLOCK_SIZE);
    
    // Get total entries available
    UINT entries = 0;
    for(UINT i = 0; i < a->entries; i++)
    {
        switch(a->memmap[i].type)
        {
            case USABLE: entries += a->memmap[i].length;break;
            default: break;
        }
    }

    for(UINT i = 1; i < entries / BLOCK_SIZE; i++)
    {
        SET_BLOCK(
            mem_map[current_block_id],
            mem_map[current_block_id-1].naked_eaddr,
            mem_map[current_block_id-1].naked_eaddr+BLOCK_SIZE);
    }
}
 
_MemoryBlock access_block(UINT block)
{
    UINT index = 0;
    _MemoryBlock current = mem_map[index];
 
 
    while(current.is_used == TRUE && !(current.block_id == block) && !(index >= current_block_id))
        current = mem_map[index++];
   
    // Is the last block used? If this evaluates to false, we'll just return the current block.
    if(current.is_used == TRUE)
    {
        // If the condition is false, go through each block until we find an available block.
        while(current.is_used == TRUE && !(index >= current_block_id))
            current = mem_map[index++];
        if(current.is_used == FALSE) return current;
        return mem_map[current_block_id];
    }
    return current;
}

UINT *PMM_Allocate(UINT blocks)
{
    UINT addr = last_addr == 0 ? mem_map[0].naked_baddr : last_addr;
    UINT index = 0;
    UINT total = 0;
    _MemoryBlock b;

    /*
     * Find an available block, add the value of the end address.
     */
    while(total < blocks)
    {
        if(mem_map[index].is_used == TRUE)
            index++;
        else {
            UINT val = mem_map[index].naked_eaddr - mem_map[index].naked_baddr;
            addr += val;
            mem_map[index].is_used = TRUE;
            total++;
        }
    }

    last_addr = addr;
    DbgPrintFmt("%d", addr);

    return (UINT *)addr;
}