/*
	KBRDEMDV.h
	
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

#ifdef KBRDEMDV_H
#error "header already included"
#else
#define KBRDEMDV_H
#endif

extern UBYTE Keyboard_Enabled;

extern int  Keyboard_Init (void);
extern void Keyboard_Reset (void);

extern void  Keyboard_Put (UBYTE in);
extern UBYTE Keyboard_Get (void);

extern void Keyboard_Down (ULONG Key);
extern void Keyboard_Up (ULONG Key);
extern void Keyboard_Auto (ULONG Key);

extern void KeyBoard_Update(void);
