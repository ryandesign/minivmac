/*
	SNDEMDEV.h

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

#ifdef SNDEMDEV_H
#error "header already included"
#else
#define SNDEMDEV_H
#endif

typedef struct {
	ui3b Enable;
	ui3b Volume;
	ui3b Buffer;
} Sound_Ty;

#define kSnd_Main_Offset   0x0300
#define kSnd_Alt_Offset    0x5F00

#define kSnd_Main_Buffer (kRAM_Size - kSnd_Main_Offset)
#define kSnd_Alt_Buffer (kRAM_Size - kSnd_Alt_Offset)

// VIA Interface Headers

EXPORTFUNC ui3b VIA_GORA3(void); // Main/Alternate Sound Buffer
EXPORTPROC VIA_PORA3(ui3b Data);

EXPORTFUNC ui3b VIA_GORA2(void); // Sound Volume Bit 2
EXPORTPROC VIA_PORA2(ui3b Data);

EXPORTFUNC ui3b VIA_GORA1(void); // Sound Volume Bit 1
EXPORTPROC VIA_PORA1(ui3b Data);

EXPORTFUNC ui3b VIA_GORA0(void); // Sound Volume Bit 0
EXPORTPROC VIA_PORA0(ui3b Data);

EXPORTFUNC ui3b VIA_GORB7(void); // Sound Enable
EXPORTPROC VIA_PORB7(ui3b Data);
