/*
	MOUSEMDV.c

	Copyright (C) 2003 Philip Cummins, Paul Pratt

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
	MOUSe EMulated DeVice

	Emulation of the mouse in the Mac Plus.

	This code descended from "Mouse-MacOS.c" in Richard F. Bannister's
	Macintosh port of vMac, by Philip Cummins.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#include "MYOSGLUE.h"
#include "ENDIANAC.h"
#include "MINEM68K.h"
#include "ADDRSPAC.h"
#include "SCCEMDEV.h"
#endif

#include "MOUSEMDV.h"

LOCALVAR ui3b X2 = 0;
LOCALVAR ui3b Y2 = 0;

GLOBALPROC Mouse_Update(void)
{
	if (Mouse_Enabled()) { /* SCC activated yet */
#if EnableMouseMotion
		if (HaveMouseMotion) {
			if ((MouseMotionH != 0) || (MouseMotionV != 0)) {
				CurMouseV = get_ram_word(0x082C);
				CurMouseH = get_ram_word(0x082E);

				put_ram_word(0x0828, get_ram_word(0x0828) + MouseMotionV);
				put_ram_word(0x082A, get_ram_word(0x082A) + MouseMotionH);
				put_ram_byte(0x08CE, get_ram_byte(0x08CF)); /* Tell MacOS to redraw the Mouse */
				MouseMotionV = 0;
				MouseMotionH = 0;
			}
		} else
#endif
		{
			ui5b NewMouse = (CurMouseV << 16) | CurMouseH;

			if (get_ram_long(0x0828) != NewMouse) {
				put_ram_long(0x0828, NewMouse); /* Set Mouse Position */
				put_ram_long(0x082C, NewMouse);
				put_ram_byte(0x08CE, get_ram_byte(0x08CF)); /* Tell MacOS to redraw the Mouse */
#if 0
				put_ram_long(0x0830, NewMouse);
				put_ram_byte(0x08CE, 0xFF); /* Tell MacOS to redraw the Mouse */
#endif
			}
		}
	}
}

/* VIA Interface Functions */

GLOBALFUNC ui3b VIA_GORB5(void) /* Mouse Y2 */
{
	return Y2;
}

GLOBALFUNC ui3b VIA_GORB4(void) /* Mouse X2 */
{
	return X2;
}

GLOBALFUNC ui3b VIA_GORB3(void) /* Mouse Button */
{
	return ! CurMouseButton;
}

GLOBALPROC VIA_PORB5(ui3b Data)
{
	UnusedParam(Data);
#ifdef _VIA_Interface_Debug
	printf("VIA ORB5 attempts to be an output\n");
#endif
}

GLOBALPROC VIA_PORB4(ui3b Data)
{
	UnusedParam(Data);
#ifdef _VIA_Interface_Debug
	printf("VIA ORB4 attempts to be an output\n");
#endif
}

GLOBALPROC VIA_PORB3(ui3b Data)
{
	UnusedParam(Data);
#ifdef _VIA_Interface_Debug
	printf("VIA ORB3 attempts to be an output\n");
#endif
}
