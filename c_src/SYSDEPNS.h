/*
	SYSDEPNS.h
	
	Copyright (C) 2001 Bernd Schmidt, Philip Cummins, Paul Pratt

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
	SYStem DEPeNdencies.
*/

#ifdef SYSDEPNS_H
#error "header already included"
#else
#define SYSDEPNS_H
#endif

#include <Types.h>

#define _MacOS

/* If char has more then 8 bits, good night. */
typedef unsigned char UBYTE;
typedef signed char BYTE;

/* UWORD should be two byte unsigned integer */
typedef unsigned short UWORD;

/* WORD should be two byte signed integer */
typedef short WORD;

/* ULONG should be four byte unsigned integer */
typedef unsigned long ULONG;

/* LONG should be four byte signed integer */
typedef long LONG;

typedef ULONG CPTR;

#define UnusedParam(p) (void) p
