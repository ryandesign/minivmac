/*
	IMATHOPT.i
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
	Integer MATH utilities (OPTimize)
*/


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


GLOBALPROC pUi5VecFromConst(ui5p p, uimr n, ui5r x)
{
	uimr i;

	for (i = n + 1; 0 != --i; ) {
		*p++ = x;
	}
}

GLOBALPROC pUi4VecFromConst(ui4p p, uimr n, ui4r x)
{
	ui5b L;

	if (AndBits((uimr)p, 2) != 0) {
		if (n == 0) {
			return;
		}
		*p = x;
		p++;
		n--;
	}
	if (IsOdd(n)) {
		n--;
		p[n] = (ui4b)x;
	}
	L = (ui5b)((((ui5b)((ui4b)(x))) << 16) | ((ui4b)(x)));
	pUi5VecFromConst((ui5p)p, n / 2, L);
}

GLOBALPROC pUi3VecFromConst(ui3p p, uimr n, ui3r x)
{
	ui4b w;

	if (IsOdd((uimr)p)) {
		if (n == 0) {
			return;
		}
		*p = (ui3b)x;
		p++;
		n--;
	}
	if (IsOdd(n)) {
		n--;
		p[n] = (ui3b)x;
	}
	w = (ui4b)((((ui4b)((ui3b)(x))) << 8) | ((ui3b)(x)));
	pUi4VecFromConst((ui4p)p, n >> 1, w);
}

GLOBALPROC pBytesClear(MyPtr p, uimr n)
{
	pUi3VecFromConst((ui3p)p, n, 0);
}
