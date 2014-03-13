/*
	MYFPMATH.c
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
	MY Floating Point MATH
*/

struct fp_extended {
	si4b w[5];
};
typedef struct fp_extended fp_extended;

pascal void
fp_add(
	fp_extended *x,
	fp_extended *r)
	= {
		0x4267, /* CLR.W      -(A7) */
		0xA9EB  /* _FP68K */
	};

pascal void
fp_sub(
	fp_extended *x,
	fp_extended *r)
	= {
		0x3F3C, 0x0002, /* MOVE.W     #$0002,-(A7) */
		0xA9EB  /* _FP68K */
	};

pascal void
fp_mul(
	fp_extended *x,
	fp_extended *r)
	= {
		0x3F3C, 0x0004, /* MOVE.W     #$0004,-(A7) */
		0xA9EB  /* _FP68K */
	};

pascal void
fp_div(
	fp_extended *x,
	fp_extended *r)
	= {
		0x3F3C, 0x0006, /* MOVE.W     #$0006,-(A7) */
		0xA9EB  /* _FP68K */
	};

pascal void
fp_sqrt(fp_extended *r)
	= {
		0x3F3C, 0x0012, /* MOVE.W     #$0012,-(A7) */
		0xA9EB  /* _FP68K */
	};

pascal void
fp_neg(fp_extended *r)
	= {
		0x3F3C, 0x000D, /* MOVE.W     #$000D,-(A7) */
		0xA9EB  /* _FP68K */
	};

pascal void
fp_fromSi5b(
	si5b *x,
	fp_extended *r)
	= {
		0x3F3C, 0x280E, /* MOVE.W     #$280E,-(A7) */
		0xA9EB  /* _FP68K */
	};

pascal void
fp_toSi5b(
	fp_extended *x,
	si5b *r)
	= {
		0x3F3C, 0x2810, /* MOVE.W     #$2810,-(A7) */
		0xA9EB  /* _FP68K */
	};

typedef si4b fp_environment;

pascal void
fp_getenvironment(
	fp_environment *r)
	= {
		0x3F3C, 0x0003, /* MOVE.W     #$0003,-(A7) */
		0xA9EB  /* _FP68K */
	};

pascal void
fp_setenvironment(
	fp_environment *r)
	= {
		0x3F3C, 0x0001, /* MOVE.W     #$0001,-(A7) */
		0xA9EB  /* _FP68K */
	};

#define fp_TONEAREST 0
#define fp_UPWARD 1
#define fp_DOWNWARD 2
#define fp_TOWARDZERO 3

typedef si4r fp_rounddir;

#define fp_getround(e) (((e) >> 13) & 3)
#define fp_setround(e, r) (((e) & ~ (3 << 13)) | ((r) << 13))

static void fp_fromSi5r(si5r x, fp_extended *r)
{
	si5b i = x;

	fp_fromSi5b(&i, r);
}

static si5r fp_toSi5rDown(fp_extended *x)
{
	fp_environment e;
	fp_environment e2;
	si5b i;

	fp_getenvironment(&e);
	e2 = fp_setround(e, fp_DOWNWARD);
	fp_setenvironment(&e2);
	fp_toSi5b(x, &i);
	fp_setenvironment(&e);

	return i;
}
