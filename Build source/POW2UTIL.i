/*
	POW2UTIL.i
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
	POWer of 2 UTILities

	Some very basic math macros dealing
	with powers of two.
*/


#define PowOf2(p) ((uimr)1 << (p))
#define Pow2Mask(p) (PowOf2(p) - 1)
#define ModPow2(i, p) ((i) & Pow2Mask(p))
#define FloorDivPow2(i, p) ((i) >> (p))
#define CeilDivPow2(i, p) (((i) + Pow2Mask(p)) >> (p))
	/* warning - CeilDivPow2 evaluates p twice */
#define NotEvenPow2Mult(i, p) (ModPow2((i), (p)) != 0)
#define FloorPow2Mult(i, p) ((i) & (~ Pow2Mask(p)))
#define CeilPow2Mult(i, p) FloorPow2Mult((i) + Pow2Mask(p), (p))
	/* warning - CeilPow2Mult evaluates p twice */

#define IsOdd(i) NotEvenPow2Mult(i, 1)
#define TestBit(i, p) (((uimr)(i) & PowOf2(p)) != 0)
#define SetBit(i, p) (i = ((uimr)(i) | PowOf2(p)))
#define ClearBit(i, p) (i = ((uimr)(i) & (~ PowOf2(p))))
#define ExtractBitField(i, p, n) (ModPow2((uimr)(i) >> (p), n))
#define ClearBitField(i, p, n) (((uimr)(i)) & (~ ((PowOf2(n) - 1) << (p))))
#define InsertBitField(i, p, n, v) (ClearBitField(i, p, n) | (ModPow2(v, n) << (p)))
	/* warning - InsertBitField evaluates p and n twice */
#define AndBits(x, y) ((uimr)(x) & (uimr)(y))

LOCALFUNC uimr FloorLn2Long(uimr n)
{
	uimr r = 0;
	uimr x = n;

	if (x >= 0x00010000) {
		x >>= 16;
		r += 16;
	}
	if (x >= 0x00000100) {
		x >>= 8;
		r += 8;
	}
	if (x >= 0x00000010) {
		x >>= 4;
		r += 4;
	}
	if (x >= 0x00000004) {
		x >>= 2;
		r += 2;
	}
	if (x >= 0x00000002) {
		x >>= 1;
		r += 1;
	}

	return r;
}

LOCALFUNC uimr CeilLn2Long(uimr n)
{
	uimr r = FloorLn2Long(n);

	if (n > PowOf2(r)) {
		r++;
	}

	return r;
}

LOCALFUNC uimr CeilPowerOf2(uimr i)
{
	if (i <= 0) {
		return 0;
	} else {
		return PowOf2(CeilLn2Long(i));
	}
}
