#include <i8042prt/keyboard.h>
#include <io/io.h>
#include <hal/idt.h>

#include <hal/vid.h>

#include <kd/com.h>


CONST UCHAR KbdAlphabetOne[] = "qwertzuiop"; // 0x10 - 0x1C
CONST UCHAR KbdAlphabetTwo[] = "asdfghjkl"; // 0x1E - 0x26
CONST UCHAR KbdAlphabetThree[] = "yxcvbnm"; // 0x2C - 0x32
CONST UCHAR KbdNumbers[] = "123456789"; // 0x2 - 0xA

KSYSTEM_KEYBOARD KbdSystemKeyboard = {0};

UCHAR
i8042KbdRead();

UCHAR
i8042KbdTranslate(
    UCHAR ucKey
);
///////////////////////


VOID
i8042KbdHandle(
	struct InterruptRegisters *reg
)
{
    UCHAR Character = i8042KbdTranslate(IoInputByte(0x60));
    if(Character == 0)
        return;
        
    KbdSystemKeyboard.KbdLastCharacter = Character;
}

UCHAR
i8042KbdTranslate(
    UCHAR ucKey
)
{
    if(ucKey == 0x1C)
        return '\n';

    if(ucKey == 0x39)
    return ' ';

	if(ucKey == 0xE)
    return '\r';

	if(ucKey == 0xB4)
    return '.';

	if(ucKey == 0xB5)
    return '/';

	if(ucKey == 0x29)
    return '0';

	if(ucKey >= 0x2 && ucKey <= 0xA)
		return KbdNumbers[ucKey - 0x2];

	if(ucKey >= 0x10 && ucKey <= 0x1C)
	{
		return KbdAlphabetOne[ucKey - 0x10];
	}
    else if(ucKey >= 0x1E && ucKey <= 0x26)
	{
		return KbdAlphabetTwo[ucKey - 0x1E];
	}
    else if(ucKey >= 0x2C && ucKey <= 0x32)
	{
		return KbdAlphabetThree[ucKey - 0x2C];
	}
    
    return 0;
}

UCHAR
i8042KbdRead()
{
    if(IoInputByte(0x64) & 1)
        return IoInputByte(0x60);

    return -1;
}

VOID
KeInitializeKeyboard()
{
    HalRegisterInterrupt(IRQ1, i8042KbdHandle);
}