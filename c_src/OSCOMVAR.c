/*
	OSCOMVAR.c
	
	Copyright (C) 2001 Paul Pratt

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
	Operating System COMmunication VARiables
	
	A place for communication between the
	operating system specific code and the
	rest of the program.
*/

#include "SYSDEPNS.h"

#include "OSCOMVAR.h"

Boolean HogCPU;

unsigned char SpeedLimit;

char *screencomparebuff;

UWORD *RAM = NULL;

ULONG kRAM_Size = 0; 

UWORD *ROM;

UBYTE PARAMRAM[PARAMRAMSize];

UWORD CurMouseV = 0;
UWORD CurMouseH = 0;
UBYTE CurMouseButton = false;

ULONG theKeys[4];
