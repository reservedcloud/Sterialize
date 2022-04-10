#include <hal/vid.h>
#include <rtl/rtl.h>


ULONG VidTextColor = NULL;
ULONG VidDisplayX = 0;
ULONG VidDisplayY = 0;

ULONG64 VidFramebufferAddress = 0x0;
UINT VidScreenWidth = 0;
UINT VidScreenHeight = 0;
UINT VidScreenBpp = 0;
UINT VidScreenPitch = 0;

BOOLEAN VidInitialize(struct stivale2_struct_tag_framebuffer* VidFramebuffer){
    VidTextColor = 0x00000000;
    VidDisplayX = 0;
    VidDisplayY = 0;

    VidFramebufferAddress = VidFramebuffer->framebuffer_addr;
    VidScreenWidth = VidFramebuffer->framebuffer_width;
    VidScreenHeight = VidFramebuffer->framebuffer_height;
    VidScreenBpp = VidFramebuffer->framebuffer_bpp;
    VidScreenPitch = VidFramebuffer->framebuffer_pitch;

    return TRUE;
}

VOID VidResetDisplay(BOOLEAN HalReset){
    VidTextColor = 0x00000000;
    VidDisplayX = 0;
    VidDisplayY = 0;
}

ULONG VidSetTextColor(ULONG Color){
    ULONG OldColor;
 
     /* Save the old color and set the new one */
     OldColor = VidTextColor;
     VidTextColor = Color;
     return OldColor;
}

VOID VidDisplayString(PUCHAR String){
    INT iX = VidDisplayX;
	for ( INT i = 0; RtlStringLength( String ) > i; i++ )
	{
		if ( String[ i ] == '\n' )
			VidDisplayY += 16;
		else if ( String[ i ] == '\r' )
			iX = VidDisplayX;
		else if ( iX < VidScreenWidth )
		{
			KiDisplayChar(
				String[ i ],
				iX,
				VidDisplayY,
				VidTextColor
			);

			iX += 8;
		}
	}
}

VOID VidDisplayStringXY(PUCHAR String, ULONG Left, ULONG Top, BOOLEAN Transparent)
{
    ULONG BackColor;
 
     /*
      * If the caller wanted transparent, then send the special value (16),
      * else use our default and call the helper routine.
      */
     BackColor = Transparent ? 0x00000000 : 0xffffffff;
 
     /* Loop every character and adjust the position */
      for (; *String; ++String, Left += 8)
      {
          /* Display a character */
          KiDisplayChar(*String, Left, Top, VidTextColor);
      }
}

VOID SetPixel(INT x, INT y, UINT Color){
    ULONG64 ulOffset;

	ulOffset = ( ULONG64 )( ( x * 4 ) + ( y * VidScreenPitch ) );
	*( INT* )( VidFramebufferAddress + ulOffset ) = Color;
}

VOID VidSolidColorFill(ULONG Left, ULONG Top, ULONG Right, ULONG Bottom, UINT Color){
    int y, x;
 
     //
     // Loop along the Y-axis
     //
     for (y = Top; y <= Bottom; y++)
     {
         //
         // Loop along the X-axis
         //
         for (x = Left; x <= Right; x++)
         {
             //
             // Draw the pixel
             //
             SetPixel(x, y, Color);
         }
     }
}

VOID
KiDisplayChar(
	CHAR  cChararcter,
	UINT  uiX,
	UINT  uiY,
	UINT  uiColor
)
{
	ULONG64 ulOffset;
	UCHAR ucDisplay;


	for ( INT iy = uiY; uiY + 16 > iy; iy++ )
	{
		for ( INT ix = uiX; uiX + 8 > ix; ix++ )
		{
			ucDisplay = KiDisplayFont[ ( INT )cChararcter * 16 + iy - uiY ];

			if ( ucDisplay & ( 1 << ( 7 - ( ix - uiX ) ) ) &&
				   ix >= 0 && iy >= 0 && ix < VidScreenWidth && iy < VidScreenHeight )
			{

				ulOffset = ( ULONG64 )( ( ix * 4 ) + ( iy * VidScreenPitch ) );
				*( INT* )( VidFramebufferAddress + ulOffset ) = uiColor;
			
			}

		}
			

	}
		

	
}

UCHAR
KiDisplayFont[ 4096 ] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x12, 0x12, 0x12, 0x7e, 0x24, 0x24, 0x7e, 0x48, 0x48, 0x48, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x3e, 0x49, 0x48, 0x38, 0x0e, 0x09, 0x49, 0x3e, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x31, 0x4a, 0x4a, 0x34, 0x08, 0x08, 0x16, 0x29, 0x29, 0x46, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x14, 0x18, 0x29, 0x45, 0x42, 0x46, 0x39, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x04, 0x08, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x08, 0x08, 0x04, 0x00,
	0x00, 0x00, 0x00, 0x20, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x49, 0x2a, 0x1c, 0x2a, 0x49, 0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x7f, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x08, 0x08, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x40, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x18, 0x24, 0x42, 0x46, 0x4a, 0x52, 0x62, 0x42, 0x24, 0x18, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x18, 0x28, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x02, 0x0c, 0x10, 0x20, 0x40, 0x40, 0x7e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x02, 0x1c, 0x02, 0x02, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x04, 0x0c, 0x14, 0x24, 0x44, 0x44, 0x7e, 0x04, 0x04, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7e, 0x40, 0x40, 0x40, 0x7c, 0x02, 0x02, 0x02, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1c, 0x20, 0x40, 0x40, 0x7c, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7e, 0x02, 0x02, 0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x3e, 0x02, 0x02, 0x02, 0x04, 0x38, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x08, 0x08, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x02, 0x04, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1c, 0x22, 0x4a, 0x56, 0x52, 0x52, 0x52, 0x4e, 0x20, 0x1e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x18, 0x24, 0x24, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7c, 0x42, 0x42, 0x42, 0x7c, 0x42, 0x42, 0x42, 0x42, 0x7c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x40, 0x40, 0x40, 0x40, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x78, 0x44, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x44, 0x78, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7e, 0x40, 0x40, 0x40, 0x7c, 0x40, 0x40, 0x40, 0x40, 0x7e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7e, 0x40, 0x40, 0x40, 0x7c, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x40, 0x40, 0x4e, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x44, 0x44, 0x38, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x42, 0x44, 0x48, 0x50, 0x60, 0x60, 0x50, 0x48, 0x44, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x66, 0x66, 0x5a, 0x5a, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x42, 0x62, 0x62, 0x52, 0x52, 0x4a, 0x4a, 0x46, 0x46, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7c, 0x42, 0x42, 0x42, 0x7c, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x5a, 0x66, 0x3c, 0x03, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7c, 0x42, 0x42, 0x42, 0x7c, 0x48, 0x44, 0x44, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x40, 0x30, 0x0c, 0x02, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7f, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x41, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x5a, 0x5a, 0x66, 0x66, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x24, 0x24, 0x18, 0x18, 0x24, 0x24, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7e, 0x02, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x40, 0x7e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x02, 0x02, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x70, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00,
	0x00, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x5c, 0x62, 0x42, 0x42, 0x42, 0x42, 0x62, 0x5c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x40, 0x40, 0x40, 0x40, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x7e, 0x40, 0x40, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0c, 0x10, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3a, 0x44, 0x44, 0x44, 0x38, 0x20, 0x3c, 0x42, 0x42, 0x3c,
	0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x5c, 0x62, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x04, 0x04, 0x00, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x48, 0x30,
	0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x44, 0x48, 0x50, 0x60, 0x50, 0x48, 0x44, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x49, 0x49, 0x49, 0x49, 0x49, 0x49, 0x49, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x62, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x62, 0x42, 0x42, 0x42, 0x42, 0x62, 0x5c, 0x40, 0x40,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x42, 0x46, 0x3a, 0x02, 0x02,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x62, 0x42, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x40, 0x30, 0x0c, 0x02, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x24, 0x24, 0x24, 0x18, 0x18, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x49, 0x49, 0x49, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x26, 0x1a, 0x02, 0x02, 0x3c,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x7e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0c, 0x10, 0x10, 0x08, 0x08, 0x10, 0x20, 0x10, 0x08, 0x08, 0x10, 0x10, 0x0c,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x00, 0x30, 0x08, 0x08, 0x10, 0x10, 0x08, 0x04, 0x08, 0x10, 0x10, 0x08, 0x08, 0x30,
	0x00, 0x00, 0x00, 0x31, 0x49, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x73, 0xca, 0x4b, 0xca, 0x73, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x71, 0xca, 0x73, 0xc2, 0x42, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x49, 0xca, 0x7a, 0xca, 0x49, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x73, 0xca, 0x73, 0xca, 0x72, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x4b, 0xea, 0x5b, 0xca, 0x4b, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x74, 0xa6, 0x25, 0xa4, 0x74, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x4b, 0xea, 0x5b, 0xca, 0x4b, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x39, 0xc2, 0x31, 0x88, 0x73, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x79, 0xc2, 0x79, 0xc0, 0x7b, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x4b, 0xc9, 0x79, 0xc9, 0x49, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x25, 0xa4, 0x3c, 0xa4, 0x24, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x45, 0xc4, 0x44, 0xa8, 0x10, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x72, 0xca, 0x72, 0xc2, 0x43, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x72, 0xca, 0x72, 0xc2, 0x43, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x0e, 0x89, 0x0e, 0x8a, 0x09, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x39, 0xc2, 0x31, 0x88, 0x73, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x39, 0xc2, 0x31, 0x88, 0x73, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x71, 0xca, 0x4a, 0xca, 0x71, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x72, 0xca, 0x72, 0xc2, 0x41, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x72, 0xca, 0x72, 0xc2, 0x41, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x3b, 0xc1, 0x31, 0x89, 0x71, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x39, 0xc2, 0x42, 0xc2, 0x39, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x22, 0xb6, 0x2a, 0xa2, 0x22, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x3b, 0xc2, 0x33, 0x8a, 0x72, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x7b, 0xc2, 0x7b, 0xc2, 0x7a, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x39, 0xc2, 0x32, 0x8a, 0x71, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x33, 0xc4, 0x25, 0x94, 0x63, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x39, 0xc2, 0x32, 0x8a, 0x71, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x39, 0xc2, 0x41, 0xc0, 0x3b, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x0e, 0x90, 0x0c, 0x82, 0x1c, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x31, 0xca, 0x49, 0xc8, 0x33, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x1c, 0x92, 0x1c, 0x90, 0x10, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0xaa, 0x00, 0x80, 0x00, 0x80, 0x33, 0xca, 0x7b, 0xca, 0x4a, 0x80, 0x00, 0x80, 0x00, 0x80, 0x55,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x3e, 0x49, 0x48, 0x48, 0x49, 0x3e, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0e, 0x10, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x3e, 0x61, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x3c, 0x24, 0x42, 0x42, 0x24, 0x3c, 0x42, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x41, 0x22, 0x14, 0x08, 0x7f, 0x08, 0x7f, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x40, 0x3c, 0x42, 0x42, 0x3c, 0x02, 0x42, 0x3c, 0x00, 0x00,
	0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x99, 0xa5, 0xa1, 0xa1, 0xa5, 0x99, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x1c, 0x02, 0x1e, 0x22, 0x1e, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x12, 0x24, 0x24, 0x48, 0x24, 0x24, 0x12, 0x12, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x02, 0x02, 0x02, 0x00, 0x00,
	0xaa, 0x00, 0x80, 0x3a, 0xc2, 0x33, 0x8a, 0x72, 0x80, 0x00, 0x80, 0x03, 0x80, 0x00, 0x80, 0x55,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0xb9, 0xa5, 0xa5, 0xb9, 0xa9, 0xa5, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x7f, 0x08, 0x08, 0x08, 0x00, 0x7f, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x38, 0x44, 0x04, 0x18, 0x20, 0x40, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x38, 0x44, 0x04, 0x38, 0x04, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x04, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x66, 0x59, 0x40, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x3f, 0x7a, 0x7a, 0x7a, 0x3a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x30,
	0x00, 0x00, 0x00, 0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x48, 0x24, 0x24, 0x12, 0x24, 0x24, 0x48, 0x48, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x22, 0x62, 0x24, 0x28, 0x28, 0x12, 0x16, 0x2a, 0x4e, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x22, 0x62, 0x24, 0x28, 0x28, 0x14, 0x1a, 0x22, 0x44, 0x4e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x62, 0x12, 0x24, 0x18, 0x68, 0x12, 0x16, 0x2a, 0x4e, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x10, 0x10, 0x20, 0x40, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x30, 0x0c, 0x00, 0x00, 0x18, 0x24, 0x24, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x0c, 0x30, 0x00, 0x00, 0x18, 0x24, 0x24, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x18, 0x24, 0x00, 0x00, 0x18, 0x24, 0x24, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x32, 0x4c, 0x00, 0x00, 0x18, 0x24, 0x24, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x24, 0x24, 0x00, 0x00, 0x18, 0x24, 0x24, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x18, 0x24, 0x18, 0x00, 0x18, 0x24, 0x24, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1f, 0x28, 0x48, 0x48, 0x7f, 0x48, 0x48, 0x48, 0x48, 0x4f, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x40, 0x40, 0x40, 0x40, 0x42, 0x42, 0x3c, 0x08, 0x30,
	0x30, 0x0c, 0x00, 0x00, 0x7e, 0x40, 0x40, 0x40, 0x7c, 0x40, 0x40, 0x40, 0x40, 0x7e, 0x00, 0x00,
	0x0c, 0x30, 0x00, 0x00, 0x7e, 0x40, 0x40, 0x40, 0x7c, 0x40, 0x40, 0x40, 0x40, 0x7e, 0x00, 0x00,
	0x18, 0x24, 0x00, 0x00, 0x7e, 0x40, 0x40, 0x40, 0x7c, 0x40, 0x40, 0x40, 0x40, 0x7e, 0x00, 0x00,
	0x24, 0x24, 0x00, 0x00, 0x7e, 0x40, 0x40, 0x40, 0x7c, 0x40, 0x40, 0x40, 0x40, 0x7e, 0x00, 0x00,
	0x18, 0x06, 0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x0c, 0x30, 0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x18, 0x24, 0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x24, 0x24, 0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x78, 0x44, 0x42, 0x42, 0xf2, 0x42, 0x42, 0x42, 0x44, 0x78, 0x00, 0x00,
	0x32, 0x4c, 0x00, 0x00, 0x42, 0x62, 0x62, 0x52, 0x52, 0x4a, 0x4a, 0x46, 0x46, 0x42, 0x00, 0x00,
	0x30, 0x0c, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x0c, 0x30, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x18, 0x24, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x32, 0x4c, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x24, 0x24, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x24, 0x18, 0x24, 0x42, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x3a, 0x44, 0x46, 0x4a, 0x4a, 0x52, 0x52, 0x62, 0x22, 0x5c, 0x40, 0x00,
	0x30, 0x0c, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x0c, 0x30, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x18, 0x24, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x24, 0x24, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x0c, 0x30, 0x00, 0x00, 0x41, 0x41, 0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x40, 0x40, 0x78, 0x44, 0x42, 0x42, 0x44, 0x78, 0x40, 0x40, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x44, 0x44, 0x48, 0x58, 0x44, 0x42, 0x42, 0x52, 0x4c, 0x00, 0x00,
	0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x24, 0x24, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x18, 0x24, 0x18, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x49, 0x09, 0x3f, 0x48, 0x48, 0x49, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x40, 0x40, 0x40, 0x40, 0x42, 0x3c, 0x08, 0x30,
	0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x7e, 0x40, 0x40, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x7e, 0x40, 0x40, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x7e, 0x40, 0x40, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x24, 0x24, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x7e, 0x40, 0x40, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x00, 0x00, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x24, 0x24, 0x00, 0x00, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x32, 0x0c, 0x14, 0x22, 0x02, 0x3e, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x5c, 0x62, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x24, 0x24, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3c, 0x46, 0x4a, 0x4a, 0x52, 0x52, 0x62, 0x3c, 0x40, 0x00,
	0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x24, 0x24, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x46, 0x3a, 0x00, 0x00,
	0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x26, 0x1a, 0x02, 0x02, 0x3c,
	0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x5c, 0x62, 0x42, 0x42, 0x42, 0x42, 0x62, 0x5c, 0x40, 0x40,
	0x00, 0x00, 0x24, 0x24, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x26, 0x1a, 0x02, 0x02, 0x3c,
};