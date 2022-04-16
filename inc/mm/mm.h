#pragma once
#include <ntdef.h>

#include <stdint.h>
#include <rtl/rtl.h>
#include <stivale2.h>

#include <stddef.h>

#define PAGE_SIZE 0x1000

//heap.c

void *malloc( size_t count );
void free( void *ptr );