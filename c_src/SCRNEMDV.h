/*
	SCRNEMDV.h

	Copyright (C) 2002 Philip Cummins, Richard F. Bannister, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef SCRNEMDV_H
#error "header already included"
#else
#define SCRNEMDV_H
#endif

#define kMain_Offset      0x5900
#define kAlternate_Offset 0xD900
#define kMain_Buffer      (kRAM_Size - kMain_Offset)
#define kAlternate_Buffer (kRAM_Size - kAlternate_Offset)

EXPORTPROC Screen_Draw(void);

// VIA Interface Headers

EXPORTFUNC ui3b VIA_GORA6 (void); // Main/Alternate Screen Buffer
EXPORTPROC VIA_PORA6(ui3b Data);

EXPORTFUNC ui3b VIA_GORB6 (void); // Video Beam in Display
EXPORTPROC VIA_PORB6(ui3b Data);
