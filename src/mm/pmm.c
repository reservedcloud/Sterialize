#include <mm/pmm.h>

VOID MiInitializeBlocks( struct stivale2_struct* LB, UINT start_addr )
{
    struct stivale2_struct_tag_memmap* a = KeTryAcquireTag( LB, STIVALE2_STRUCT_TAG_MEMMAP_ID );

    struct stivale2_struct s = {
        .tags = ( unsigned long )&a
    };

    MiMemoryMap = ( KSYSTEM_MEMORY* )start_addr;
    SET_BLOCK( MiMemoryMap[ MiCurrentBlockID ], start_addr, start_addr + BLOCK_SIZE );

    // Get total entries available
    UINT entries = 0;
    for ( UINT i = 0; i < a->entries; i++ )
    {
        switch ( a->memmap[ i ].type )
        {
            case USABLE: entries += a->memmap[ i ].length; break;
            default: break;
        }
    }

    for ( UINT i = 1; i < entries / BLOCK_SIZE; i++ )
    {
        SET_BLOCK(
            MiMemoryMap[ MiCurrentBlockID ],
            MiMemoryMap[ MiCurrentBlockID - 1 ].NakedEAddress,
            MiMemoryMap[ MiCurrentBlockID - 1 ].NakedEAddress + BLOCK_SIZE );
    }
}

KSYSTEM_MEMORY MiAccessBlock( UINT block )
{
    UINT index = 0;
    KSYSTEM_MEMORY current = MiMemoryMap[ index ];


    while ( current.IsUsed && !( current.BlockID == block ) && !( index >= MiCurrentBlockID ) )
        current = MiMemoryMap[ index++ ];

    // Is the last block used? If this evaluates to false, we'll just return the current block.
    if ( current.IsUsed )
    {
        // If the condition is false, go through each block until we find an available block.
        while ( current.IsUsed && !( index >= MiCurrentBlockID ) )
            current = MiMemoryMap[ index++ ];
        if ( !current.IsUsed ) return current;
        return MiMemoryMap[ MiCurrentBlockID ];
    }
    return current;
}

UINT* MmAllocPhyisicalMemory( UINT blocks )
{
    UINT addr = MiLastAddress == 0 ? MiMemoryMap[ 0 ].NakedBAddress : MiLastAddress;
    UINT index = 0;
    UINT total = 0;
    KSYSTEM_MEMORY b;

    /*
     * Find an available block, add the value of the end address.
     */
    while ( total < blocks )
    {
        if ( MiMemoryMap[ index ].IsUsed )
            index++;
        else
        {
            UINT val = MiMemoryMap[ index ].NakedEAddress - MiMemoryMap[ index ].NakedBAddress;
            addr += val;
            MiMemoryMap[ index ].IsUsed = TRUE;
            total++;
        }
    }

    MiLastAddress = addr;
    DbgPrintFmt( "%d", addr );

    return ( UINT* )addr;
}
