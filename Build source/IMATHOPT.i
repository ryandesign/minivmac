/*
	IMATHOPT.i
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
	Integer MATH utilities (OPTimize)
*/


GLOBALPROC pUi5VecFromConst(ui5p p, uimr n, ui5r x)
{
	NuimrForVar i;

	for_NuimrTimes(i, n) {
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
