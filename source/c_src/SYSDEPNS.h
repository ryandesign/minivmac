/*
	SYSDEPNS.h

	Copyright (C) 2006 Bernd Schmidt, Philip Cummins, Paul C. Pratt

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
	SYStem DEPeNdencies.
*/

#ifdef SYSDEPNS_H
#error "header already included"
#else
#define SYSDEPNS_H
#endif

#include "CNFGGLOB.h"

#ifndef MayInline
#define MayInline
#endif

#ifndef MayNotInline
#define MayNotInline
#endif

#ifndef UnusedParam
#define UnusedParam(p) (void) p
#endif

#ifndef BigEndianUnaligned
#define BigEndianUnaligned 0
#endif

/* If char has more then 8 bits, good night. */
typedef unsigned char ui3b;
typedef signed char si3b;

/* ui4b should be two byte unsigned integer */
typedef unsigned short ui4b;

/* si4b should be two byte signed integer */
typedef short si4b;

/* ui5b should be four byte unsigned integer */
typedef unsigned long ui5b;

/* si5b should be four byte signed integer */
typedef long si5b;

typedef ui5b CPTR;

#define blnr int
#define trueblnr 1
#define falseblnr 0

#define nullpr ((void *) 0)

typedef ui3b *ui3p;
#define anyp ui3p

/* pascal string, single byte characters */
#define ps3p ui3p

#define LOCALVAR static
#ifdef AllFiles
#define GLOBALVAR LOCALVAR
#define EXPORTVAR(t, v)
#else
#define GLOBALVAR
#define EXPORTVAR(t, v) extern t v;
#endif

#define LOCALFUNC static
#define FORWARDFUNC LOCALFUNC
#ifdef AllFiles
#define GLOBALFUNC LOCALFUNC
#define EXPORTFUNC LOCALFUNC
#else
#define GLOBALFUNC
#define EXPORTFUNC extern
#endif
#define IMPORTFUNC EXPORTFUNC
#define TYPEDEFFUNC typedef

#define LOCALPROC LOCALFUNC void
#define GLOBALPROC GLOBALFUNC void
#define EXPORTPROC EXPORTFUNC void
#define IMPORTPROC IMPORTFUNC void
#define FORWARDPROC FORWARDFUNC void
#define TYPEDEFPROC TYPEDEFFUNC void
