/*
	SNDEMDEV.c

	Copyright (C) 2002 Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

/*
	SouND EMulated DEVice

	Emulation of Sound in the Mac Plus could go here.

	This code adapted from "Sound.c" in vMac by Philip Cummins.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#endif

#include "SNDEMDEV.h"

// Local Variables

LOCALVAR Sound_Ty theSound;

// VIA Interface Functions

GLOBALFUNC ui3b VIA_GORA3 (void) // Main/Alternate Sound Buffer
{
#ifdef _VIA_Interface_Debug
	printf("VIA ORA3 attempts to be an input\n");
#endif
	return 0;
}

GLOBALPROC VIA_PORA3(ui3b Data)
{
	theSound.Buffer = Data; // 1 = Main, 0 = Alternate
}

GLOBALFUNC ui3b VIA_GORA2(void) // Sound Volume Bit 2
{
#ifdef _VIA_Interface_Debug
	printf("VIA ORA2 attempts to be an input\n");
#endif
	return 0;
}

GLOBALPROC VIA_PORA2(ui3b Data)
{
	if (Data == 0) {
		theSound.Volume &= 0x03;
	} else {
		theSound.Volume |= 0x04;
	}
}

GLOBALFUNC ui3b VIA_GORA1(void) // Sound Volume Bit 1
{
#ifdef _VIA_Interface_Debug
	printf("VIA ORA1 attempts to be an input\n");
#endif
	return 0;
}

GLOBALPROC VIA_PORA1(ui3b Data)
{
	if (Data == 0) {
		theSound.Volume &= 0x05;
	} else {
		theSound.Volume |= 0x02;
	}
}

GLOBALFUNC ui3b VIA_GORA0(void) // Sound Volume Bit 0
{
#ifdef _VIA_Interface_Debug
	printf("VIA ORA0 attempts to be an input\n");
#endif
//
// bill huey ---
// I'm basically trying to take a guess as to what
// would be neutral return value for the "return"
// statement that I just added to satisfy GCC's
// complaints...

	return 0;
}

GLOBALPROC VIA_PORA0(ui3b Data)
{
	if (Data == 0) {
		theSound.Volume &= 0x06;
	} else {
		theSound.Volume |= 0x01;
	}
}

GLOBALFUNC ui3b VIA_GORB7(void) // Sound Enable
{
#ifdef _VIA_Interface_Debug
	printf("VIA ORB7 attempts to be an input\n");
#endif
	return 0;
}

GLOBALPROC VIA_PORB7(ui3b Data)
{
	theSound.Enable = Data; // 0 = Enabled, 1 = Disabled
}
