/*
	COREDEFS.i
	Copyright (C) 2007 Paul C. Pratt

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
	CORE DEFinitionS
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

typedef ui5p *ui5h;
typedef ui4p *ui4h;
typedef ui3p *ui3h;

/*
	define the largest supported
	representation types.
*/
typedef si5r simr;
typedef ui5r uimr;

#define simr signed long
#define uimr unsigned long

typedef ui3b blnb;
typedef ui3r blnr;
#define trueblnr 1
#define falseblnr 0

typedef unsigned char MyPStr[256];
typedef unsigned char *ps3p;
#define MyCharR unsigned char
#define MyCharPtr MyCharR *

typedef unsigned char MyByte;
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

#ifndef WantOptMoveBytes
#define WantOptMoveBytes 0
#endif

#if WantOptMoveBytes
/*
	avoid trap dispatch overhead for small copies.
	Worthwhile for programs that use zillions
	of such.
*/
LOCALPROC MyMoveBytes(MyPtr src, MyPtr dst, uimr n)
{
	switch (n) {
		case 15: *dst++ = *src++; /* fall through */
		case 14: *dst++ = *src++; /* fall through */
		case 13: *dst++ = *src++; /* fall through */
		case 12: *dst++ = *src++; /* fall through */
		case 11: *dst++ = *src++; /* fall through */
		case 10: *dst++ = *src++; /* fall through */
		case 9: *dst++ = *src++; /* fall through */
		case 8: *dst++ = *src++; /* fall through */
		case 7: *dst++ = *src++; /* fall through */
		case 6: *dst++ = *src++; /* fall through */
		case 5: *dst++ = *src++; /* fall through */
		case 4: *dst++ = *src++; /* fall through */
		case 3: *dst++ = *src++; /* fall through */
		case 2: *dst++ = *src++; /* fall through */
		case 1: *dst   = *src  ; /* fall through */
		case 0:
			break;
		default:
			BlockMove((void *)(src), (void *)(dst), n);
			break;
	}
}
#else
#define MyMoveBytes(src, dst, n) \
	BlockMove((void *)(src), (void *)(dst), n)
#endif

#define NuimrForVar uimr
#define for_NuimrTimes(v, n) for ((v) = (n) + 1; --(v) != 0; )
