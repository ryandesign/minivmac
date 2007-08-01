/*
	MOUSEMDV.c

	Copyright (C) 2006 Philip Cummins, Paul C. Pratt

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
#include "ADDRSPAC.h"
#include "SCCEMDEV.h"
#endif

#include "MOUSEMDV.h"

GLOBALPROC Mouse_Update(void)
{
#ifdef MouseBtnUp
	MouseBtnUp = CurMouseButton ? 0 : 1;
#endif

	/* if start doing this too soon after boot, will mess up memory check */
	if (Mouse_Enabled()) {
#if EnableMouseMotion
		if (HaveMouseMotion) {
			/* tell platform specific code where the mouse went */
			CurMouseV = get_ram_word(0x082C);
			CurMouseH = get_ram_word(0x082E);

			if ((MouseMotionH != 0) || (MouseMotionV != 0)) {
				put_ram_word(0x0828, get_ram_word(0x0828) + MouseMotionV);
				put_ram_word(0x082A, get_ram_word(0x082A) + MouseMotionH);
				put_ram_byte(0x08CE, get_ram_byte(0x08CF));
					/* Tell MacOS to redraw the Mouse */

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
#if CurEmu <= kEmuPlus
				put_ram_byte(0x08CE, get_ram_byte(0x08CF));
					/* Tell MacOS to redraw the Mouse */
#else
				put_ram_long(0x0830, NewMouse);
				put_ram_byte(0x08CE, 0xFF);
					/* Tell MacOS to redraw the Mouse */
#endif
			}
		}
	}
}
