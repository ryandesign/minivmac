/*
	COREDEFS.i
	Copyright (C) 2007 Paul Pratt

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
	CORE DEFinitions
*/


typedef unsigned long ui5b;
typedef unsigned short ui4b;
typedef unsigned char ui3b;

typedef long si5b;
typedef short si4b;
typedef char si3b;

typedef ui5b ui5r;
typedef ui4b ui4r;
typedef ui3b ui3r;

typedef si5b si5r;
typedef si4b si4r;
typedef si3b si3r;

typedef si3b *si3p;
typedef ui3b *ui3p;
typedef si4b *si4p;
typedef ui4b *ui4p;
typedef si5b *si5p;
typedef ui5b *ui5p;

/*
	define the largest supported
	representation types.
*/
typedef si5r simr;
typedef ui5r uimr;

#define simr signed long
#define uimr unsigned long

typedef ui3b blnb;
#define blnr int
#define trueblnr 1
#define falseblnr 0

#define MyPStr Str255
#define ps3p StringPtr
#define MyCharR char
#define MyCharPtr MyCharR *

typedef char MyByte;
typedef MyByte *MyPtr;


#define LOCALFUNC static
#define GLOBALFUNC static
#define EXPORTFUNC static
#define TYPEDEFFUNC typedef

#define LOCALPROC LOCALFUNC void
#define GLOBALPROC GLOBALFUNC void
#define EXPORTPROC EXPORTFUNC void
#define TYPEDEFPROC TYPEDEFFUNC void

#define LOCALVAR static
#define GLOBALVAR static
#define EXPORTVAR static

#define DISCARDVAL (void)

#define nullpr ((void *) 0)

TYPEDEFFUNC blnr (*ptWriteBytes)(MyPtr p, uimr L);

GLOBALVAR MyPtr pDt;

#define MyMoveBytes(src, dst, n) BlockMove((void *)(src), (void *)(dst), n)

#define NuimrForVar uimr
#define for_NuimrTimes(v, n) for ((v) = (n) + 1; --(v) != 0; )
