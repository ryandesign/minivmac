/*
	STRUTILS.i
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
	STRing UTILitieS

	Some very basic string macros and routines.
*/


#define PStrLength(s) (*(s))
#define PStrToMyCharPtr(s) ((s) + 1)

#define PStrMaxLength 255

#define SizeOfListMyChar(n) (n)
#define PStrToPtr(s) ((MyPtr)PStrToMyCharPtr(s))
#define PStrToSize(s) (SizeOfListMyChar(PStrLength(s)))
#define PStrToTotSize(s) (SizeOfListMyChar(PStrLength(s) + 1))
	/* + 1 for length byte */

#define PStrClear(s) PStrLength(s) = 0

GLOBALPROC PStrCopy(ps3p r, ps3p s)
{
	MyMoveBytes((MyPtr)s, (MyPtr)r, PStrToTotSize(s));
}

GLOBALPROC PStrApndChar(ps3p s, MyCharR x)
{
	si4r n = s[0] + 1;

	if (n <= PStrMaxLength) {
		s[0] = (MyCharR)n;
		s[n] = x;
	}
}

GLOBALPROC PStrAppend(ps3p r, ps3p s)
{
	ps3p p = s;
	ps3p q = r;
	si4r m = *p++;
	si4r n = *q++;

	if (n + m > PStrMaxLength) {
		m = PStrMaxLength - n;
	}

	*r = (MyCharR)(n + m);
	MyMoveBytes((MyPtr)p, (MyPtr)(q + n), m);
}

GLOBALFUNC uimr CStrLength(char *s)
{
	char *p = s;

	while (*p++ != 0) {
	}
	return p - s - 1;
}

GLOBALPROC PStrFromCStr(ps3p r, /* CONST */ char *s)
{
	uimr L;

	L = CStrLength(s);
	if (L > PStrMaxLength) {
		L = PStrMaxLength;
	}
	*r++ = (MyCharR)L;
	MyMoveBytes((MyPtr)s, (MyPtr)r, L);
}

GLOBALPROC PStrFromChar(ps3p r, char x)
{
	r[0] = 1;
	r[1] = x;
}

GLOBALPROC PStrFromPtr(MyPtr p, uimr L, ps3p s)
{
	uimr tL;

	if (L <= PStrMaxLength) {
		tL = L;
	} else {
		tL = PStrMaxLength;
	}
	s[0] = (MyCharR)tL;
	MyMoveBytes(p, PStrToPtr(s), SizeOfListMyChar(tL));
}

GLOBALPROC PStrApndCStr(ps3p r, /* CONST */ char *s)
{
	MyPStr t;

	PStrFromCStr(t, s);
	PStrAppend(r, t);
}

GLOBALFUNC blnr PStrEq(ps3p s1, ps3p s2)
{
	register si4r i;
	MyCharPtr p1 = s1;
	MyCharPtr p2 = s2;
	MyCharR n = *p1++;
	MyCharR m = *p2++;

	if (n != m) {
		return falseblnr;
	} else {
		for (i = n; --i >= 0; ) {
			if (*p1++ != *p2++) {
				return falseblnr;
			}
		}
		return trueblnr;
	}
}

GLOBALFUNC blnr CStrEq(char *s1, char *s2)
{
	MyCharR c1;

	while ((c1 = *s1++) == *s2++) {
		if (0 == c1) {
			return trueblnr;
		}
	}
	return falseblnr;
}

GLOBALPROC CStrFromPtr(MyPtr p, uimr L, char *s)
{
	MyMoveBytes(p, (MyPtr)s, SizeOfListMyChar(L));
	s[L] = (MyCharR)0;
}

GLOBALPROC CStrCopy(char *r, char *s)
{
	while (0 != (*r++ = *s++)) {
	}
}

LOCALPROC ReversePStr(ps3p s)
{
	MyCharR c;
	MyCharR *p1 = PStrToMyCharPtr(s);
	MyCharR *p2 = p1 + PStrLength(s) - 1;

	while (p1 < p2) {
		c = *p1;
		*p1 = *p2;
		*p2 = c;
		++p1;
		--p2;
	}
}

#define Have_STRUTILS 1
