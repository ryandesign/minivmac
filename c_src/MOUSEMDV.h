/*
	MOUSEMDV.h

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

#ifdef MOUSEMDV_H
#error "header already included"
#else
#define MOUSEMDV_H
#endif

void Mouse_Update2 (void);

// VIA Interface Headers

extern UBYTE VIA_GORB5 (void); // Mouse Y2
extern void  VIA_PORB5 (UBYTE Data);

extern UBYTE VIA_GORB4 (void); // Mouse X2
extern void  VIA_PORB4 (UBYTE Data);

extern UBYTE VIA_GORB3 (void); // Mouse Button
extern void  VIA_PORB3 (UBYTE Data);
