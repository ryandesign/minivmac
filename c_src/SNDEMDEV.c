/*
	SNDEMDEV.c

	Copyright (C) 2001 Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with with this file; see the file COPYING.

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

#include "SYSDEPNS.h"
#include "SNDEMDEV.h"

// Local Variables

static Sound_Ty theSound;

// VIA Interface Functions

UBYTE VIA_GORA3 (void) // Main/Alternate Sound Buffer
{
#ifdef _VIA_Interface_Debug
	printf("VIA ORA3 attempts to be an input\n");
#endif
	return 0;
}

void  VIA_PORA3 (UBYTE Data)
{
	theSound.Buffer = Data; // 1 = Main, 0 = Alternate
}

UBYTE VIA_GORA2 (void) // Sound Volume Bit 2
{
#ifdef _VIA_Interface_Debug
	printf("VIA ORA2 attempts to be an input\n");
#endif
	return 0;
}

void  VIA_PORA2 (UBYTE Data)
{
	if (Data == 0) {
		theSound.Volume &= 0x03;
	} else {
		theSound.Volume |= 0x04;
	}
}

UBYTE VIA_GORA1 (void) // Sound Volume Bit 1
{
#ifdef _VIA_Interface_Debug
	printf("VIA ORA1 attempts to be an input\n");
#endif
	return 0;
}

void  VIA_PORA1 (UBYTE Data)
{
	if (Data == 0) {
		theSound.Volume &= 0x05;
	} else {
		theSound.Volume |= 0x02;
	}
}

UBYTE VIA_GORA0 (void) // Sound Volume Bit 0
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

void VIA_PORA0 (UBYTE Data)
{
	if (Data == 0) {
		theSound.Volume &= 0x06;
	} else {
		theSound.Volume |= 0x01;
	}
}

UBYTE VIA_GORB7 (void) // Sound Enable
{
#ifdef _VIA_Interface_Debug
	printf("VIA ORB7 attempts to be an input\n");
#endif
	return 0;
}

void  VIA_PORB7 (UBYTE Data)
{
	theSound.Enable = Data; // 0 = Enabled, 1 = Disabled
}
