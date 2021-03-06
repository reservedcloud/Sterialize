#pragma once
#include <ntdef.h>

int k_tolower( int c );
int k_toupper( int c );
PCHAR strcpy( PCHAR destination, const char* source );
PCHAR strcat( PCHAR destination, const char* source );
int strcmp( const char* a, const char* b );
void *memcpy( void *dest, const void *src, INT n );
void *memset( void *s, int c, INT n );
void memmove( void* dest, void* src, INT n );
int strncmp( const char *s1, const char *s2, INT n );
int memcmp( const void *s1, const void *s2, INT n );


ULONG64
strlen(
	PCSTR String
);

void str_cpy( UCHAR* src, UCHAR* dest, UCHAR count );

char *strtok_r( char *str, const char *delim, char **saveptr );

#define RtlCopyMemory memcpy
#define RtlSetMemory memset
#define RtlZeroMemory(A, B) RtlSetMemory(A, 0, B)
#define RtlCompareMemory memcmp
#define RtlStringLength strlen