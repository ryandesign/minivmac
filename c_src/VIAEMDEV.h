/*
	VIAEMDEV.h
	
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

#ifdef VIAEMDEV_H
#error "header already included"
#else
#define VIAEMDEV_H
#endif

// PC - VIA Memory Handlers

extern void VIA_Reset(void);

extern void VIA_Access(CPTR addr);

void VIA_Timer (void);
void VIA_Int_Vertical_Blanking(void);
void VIA_Int_One_Second(void);
