/*
	ASCEMDEV.c

	Copyright (C) 2008 Paul C. Pratt

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
	Apple Sound Chip EMulated DEVice
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#include "MYOSGLUE.h"
#include "EMCONFIG.h"
#include "GLOBGLUE.h"
#include "VIAEMDEV.h"
#endif

#include "ASCEMDEV.h"

LOCALVAR ui3r SoundReg801 = 0;
LOCALVAR ui3r SoundReg802 = 0;
LOCALVAR ui3r SoundReg803 = 0;
LOCALVAR ui3r SoundReg804 = 0;
LOCALVAR ui3r SoundReg805 = 0;
LOCALVAR ui3r SoundReg806 = 0;
LOCALVAR ui3r SoundReg807 = 0;

GLOBALFUNC ui5b ASC_Access(ui5b Data, blnr WriteMem, CPTR addr)
{
	switch (addr) {
		case 0x801: /* ENABLE */
			if (WriteMem) {
				SoundReg801 = Data;
			} else {
				Data = SoundReg801;
			}
			break;
		case 0x802: /* MODE */
			if (WriteMem) {
				SoundReg802 = Data;
			} else {
				Data = SoundReg802;
			}
			break;
		case 0x803:
			if (WriteMem) {
				SoundReg803 = Data;
			} else {
				Data = SoundReg803;
			}
			break;
		case 0x804:
			if (WriteMem) {
				SoundReg804 = Data;
			} else {
				Data = SoundReg804;
			}
			break;
		case 0x805:
			if (WriteMem) {
				SoundReg805 = Data;
			} else {
				Data = SoundReg805;
			}
			break;
		case 0x806: /* VOLUME */
			if (WriteMem) {
				SoundReg806 = Data;
			} else {
				Data = SoundReg806;
			}
			break;
		case 0x807: /* CHAN */
			if (WriteMem) {
				SoundReg807 = Data;
			} else {
				Data = SoundReg807;
			}
			break;
		default:
			if (WriteMem) {
			} else {
				Data = 0;
			}
			break;
	}

	return Data;
}
