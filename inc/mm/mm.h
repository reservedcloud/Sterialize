#pragma once
#include <ntdef.h>

#include <stdint.h>
#include <rtl/rtl.h>
#include <stivale2.h>
#include <mm/liballoc.h>

//vmm.c


#define KERNEL_MEM_OFFSET 0xffffffff80000000


//pmm.c

#define PAGE_SIZE 0x1000
#define PHYS_MEM_OFFSET 0xffff800000000000

void *pmalloc( size_t pages );
void *pcalloc( size_t pages );
void pmm_free_pages( void *adr, size_t page_count );
int MmInitializePmm( struct stivale2_struct_tag_memmap *memory_info );

//heap.c

void *malloc( size_t size );
void free( void *ptr );
void *krealloc( void *ptr, size_t size );
void *kcalloc( size_t size );
int kheap_init( );



void *liballoc_alloc( int size );