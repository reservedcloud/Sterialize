#pragma once
#include <ntdef.h>
#include <stivale2.h>

BOOLEAN VidInitialize(struct stivale2_struct_tag_framebuffer* VidFramebuffer);
VOID VidResetDisplay(BOOLEAN HalReset);
ULONG VidSetTextColor(ULONG Color);
VOID VidDisplayStringXY(PUCHAR String, ULONG Left, ULONG Top, BOOLEAN Transparent);
VOID VidSolidColorFill(ULONG Left, ULONG Top, ULONG Right, ULONG Bottom, UINT Color);
VOID VidDisplayString(PUCHAR String);

UINT VidScreenWidth;
UINT VidScreenHeight;
UINT VidScreenBpp;
UINT VidScreenPitch;

ULONG64 VidFramebufferAddress;


UCHAR
KiDisplayFont[ 4096 ];

VOID
VidDisplayChar(
	CHAR  cChararcter,
	UINT  uiX,
	UINT  uiY,
	UINT  uiColor
);