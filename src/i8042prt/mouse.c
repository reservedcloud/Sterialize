#include <i8042prt/mouse.h>
#include <io/io.h>
#include <hal/vid.h>
#include <hal/idt.h>
#include <kd/com.h>
#include <ntuser/wm.h>

KSYSTEM_CURSOR KiSystemCursor = {0};
UCHAR i8042MouPacket[4];

UCHAR i8042MouCycle = 0;
BOOLEAN i8042MouPacketReady = FALSE;

VOID i8042MouWait(

)
{
	ULONG64 ulTimeout = 100000;
	while (ulTimeout--)
		if ((IoInputByte(0x64) & 0b10) == 0)
			return;
}

VOID i8042MouInput(

)
{
	ULONG64 ulTimeout = 100000;
	while (ulTimeout--)
		if (IoInputByte(0x64) & 0b1)
			return;
}

VOID i8042MouWrite(
	UCHAR ucValue)
{
	i8042MouWait();
	IoOutputByte(0x64, 0xD4);
	i8042MouWait();
	IoOutputByte(0x60, ucValue);
}

UCHAR
i8042MouRead(

)
{
	i8042MouWait();
	return IoInputByte(0x60);
}

VOID i8042MouHandle(
	UCHAR ucData)
{
	i8042MouProcess();
	static BOOLEAN bSkip = TRUE;

	if (bSkip)
	{
		bSkip = FALSE;
		return;
	}

	switch (i8042MouCycle)
	{
	case 0:
		if (i8042MouPacketReady)
			break;
		if ((ucData & 0b00001000) == 0)
			break;

		i8042MouPacket[0] = ucData;
		i8042MouCycle++;
		break;
	case 1:
		if (i8042MouPacketReady)
			break;

		i8042MouPacket[1] = ucData;
		i8042MouCycle++;
		break;
	case 2:
		if (i8042MouPacketReady)
			break;

		i8042MouPacket[2] = ucData;
		i8042MouPacketReady = TRUE;
		i8042MouCycle = 0;
		break;
	}
}

VOID i8042MouProcess(

)
{
	if (!i8042MouPacketReady)
		return;

	BOOLEAN xNegative, yNegative, xOverflow, yOverflow;

	// hardcode this a little
	if (i8042MouPacket[0] & 0b00010000)
		xNegative = 1;
	else
		xNegative = 0;

	if (i8042MouPacket[0] & 0b00100000)
		yNegative = 1;
	else
		yNegative = 0;

	if (i8042MouPacket[0] & 0b01000000)
		xOverflow = 1;
	else
		xOverflow = 0;

	if (i8042MouPacket[0] & 0b10000000)
		yOverflow = 1;
	else
		yOverflow = 0;

	if (!xNegative)
	{
		KiSystemCursor.X += i8042MouPacket[1];
		if (xOverflow)
			KiSystemCursor.X += 255;
	}
	else
	{
		i8042MouPacket[1] = 256 - i8042MouPacket[1];
		KiSystemCursor.X -= i8042MouPacket[1];
		if (xOverflow)
			KiSystemCursor.X -= 255;
	}

	if (!yNegative)
	{
		KiSystemCursor.Y -= i8042MouPacket[2];
		if (yOverflow)
			KiSystemCursor.Y -= 255;
	}
	else
	{
		i8042MouPacket[2] = 256 - i8042MouPacket[2];
		KiSystemCursor.Y += i8042MouPacket[2];
		if (yOverflow)
			KiSystemCursor.Y += 255;
	}

	if (KiSystemCursor.X < 1)
		KiSystemCursor.X = 1;

	if (KiSystemCursor.X > VidScreenWidth - 1)
		KiSystemCursor.X = VidScreenWidth - 1;

	if (KiSystemCursor.Y < 1)
		KiSystemCursor.Y = 1;

	if (KiSystemCursor.Y > VidScreenHeight - 1)
		KiSystemCursor.Y = VidScreenHeight - 1;

	
	////////

	if (i8042MouPacket[0] & 0b00000001)
		KiSystemCursor.State = MOUSE_LEFT_CLICK;

	else
	if (i8042MouPacket[0] & 0b00000100)
		KiSystemCursor.State = MOUSE_MIDDLE_CLICK;

	else 
	if (i8042MouPacket[0] & 0b00000010)
		KiSystemCursor.State = MOUSE_RIGHT_CLICK;

	else KiSystemCursor.State = NULL;

	
	WmHandleWindows();
	i8042MouPacketReady = FALSE;

	
	NtSetCursorPos(KiSystemCursor.X, KiSystemCursor.Y);
	
	WmNeedsUpdate = 1;
}

VOID i8042MouUpdate(
	struct InterruptRegisters *reg)
{
	// DbgPrintFmt( "dawgging" );

	UCHAR ucData = IoInputByte(0x60);
	i8042MouHandle(ucData);
}

VOID KeInitializeMouse()
{

	IoOutputByte(0x64, 0xA8); // enabling the auxiliary device - mouse

	i8042MouWait();
	IoOutputByte(0x64, 0x20); // tells the keyboard controller that we want to send a command to the mouse
	i8042MouInput();

	UCHAR ucStatus = IoInputByte(0x60);
	ucStatus |= 0b10;
	i8042MouWait();
	IoOutputByte(0x64, 0x60);
	i8042MouWait();
	IoOutputByte(0x60, ucStatus); // setting the correct bit is the "compaq" status byte

	i8042MouWrite(0xF6);
	i8042MouRead();

	i8042MouWrite(0xF4);
	i8042MouRead();

	HalRegisterInterrupt(
		IRQ12,
		i8042MouUpdate); // init
}