/*
	MOUSEMDV.c
	
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
	MOUSe EMulated DeVice
	
	Emulation of the mouse in the Mac Plus.

	This code descended from "Mouse-MacOS.c" in Richard F. Bannister's
	macintosh port of vMac, by Philip Cummins.
*/

#include "SYSDEPNS.h"

#include "ADDRSPAC.h"

#include "MOUSEMDV.h"
#include "SCCEMDEV.h"

#include "OSCOMVAR.h"

UBYTE X2 = 0;
UBYTE Y2 = 0;

void Mouse_Update2 (void)
{
	ULONG NewMouse;
  
	if (Mouse_Enabled()) { // SCC activated yet
 		NewMouse = (CurMouseV << 16) | CurMouseH;

		if (get_long(0x0828) != NewMouse) {
			put_long(0x0828, NewMouse); // Set Mouse Position
			put_long(0x082C, NewMouse);
			put_long(0x0830, NewMouse);
			put_byte(0x08CE, 0xFF); // Tell MacOS to redraw the Mouse
		}
	}
}

// VIA Interface Functions

UBYTE VIA_GORB5 (void) // Mouse Y2
{
  return Y2;
}

UBYTE VIA_GORB4 (void) // Mouse X2
{
  return X2;
}

UBYTE VIA_GORB3 (void) // Mouse Button
{
  return !CurMouseButton;
}

void  VIA_PORB5 (UBYTE Data)
{
	UnusedParam(Data);
#	ifdef _VIA_Interface_Debug
  	printf("VIA ORB5 attempts to be an output\n");
#	endif
}

void  VIA_PORB4 (UBYTE Data)
{
	UnusedParam(Data);
#	ifdef _VIA_Interface_Debug
  	printf("VIA ORB4 attempts to be an output\n");
#	endif
}

void  VIA_PORB3 (UBYTE Data)
{
	UnusedParam(Data);
#	ifdef _VIA_Interface_Debug
  	printf("VIA ORB3 attempts to be an output\n");
#	endif
}
