/*
	SNDEMDEV.h
	
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

#ifdef SNDEMDEV_H
#error "header already included"
#else
#define SNDEMDEV_H
#endif

typedef struct
{
  UBYTE Enable;
  UBYTE Volume;
  UBYTE Buffer;
} Sound_Ty;

#define kSnd_Main_Offset 	0x0300
#define kSnd_Alt_Offset 	0x5F00

#define kSnd_Main_Buffer (kRAM_Size - kSnd_Main_Offset)
#define kSnd_Alt_Buffer (kRAM_Size - kSnd_Alt_Offset)

// VIA Interface Headers

extern UBYTE VIA_GORA3 (void); // Main/Alternate Sound Buffer
extern void  VIA_PORA3 (UBYTE Data);

extern UBYTE VIA_GORA2 (void); // Sound Volume Bit 2
extern void  VIA_PORA2 (UBYTE Data);

extern UBYTE VIA_GORA1 (void); // Sound Volume Bit 1
extern void  VIA_PORA1 (UBYTE Data);

extern UBYTE VIA_GORA0 (void); // Sound Volume Bit 0
extern void  VIA_PORA0 (UBYTE Data);

extern UBYTE VIA_GORB7 (void); // Sound Enable
extern void  VIA_PORB7 (UBYTE Data);
