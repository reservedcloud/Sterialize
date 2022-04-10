#include <io/io.h>

VOID IoOutputByte( USHORT port, UCHAR data )
{
	asm volatile( "outb %0, %1"
				  :
	: "a"( data ), "Nd"( port ) );
}

VOID IoOutputWord( USHORT port, USHORT data )
{
	asm volatile( "outw %0, %1"
				  :
	: "a"( data ), "Nd"( port ) );
}

VOID IoOutputDword( USHORT port, UINT data )
{
	asm volatile( "outl %0, %1"
				  :
	: "a"( data ), "Nd"( port ) );
}


UCHAR IoInputByte( USHORT port )
{
	UCHAR ret;
	asm volatile( "inb %1, %0"
				  : "=a"( ret )
				  : "Nd"( port ) );
	return ret;
}

USHORT IoInputWord( USHORT port )
{
	USHORT ret;
	asm volatile( "inw %1, %0"
				  : "=a"( ret )
				  : "Nd"( port ) );
	return ret;
}

UINT IoInputDword( USHORT port )
{
	UINT ret;
	asm volatile( "inl %1, %0"
				  : "=a"( ret )
				  : "Nd"( port ) );
	return ret;
}
