/*
	XBUFHAND.i
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
	eXpanding BUFfer HANDle
*/


struct xbh_r
{
	Handle h;
	uimr hSize;
	uimr L;
};

typedef struct xbh_r xbh_r;

GLOBALFUNC blnr xbh_Init(uimr L, xbh_r *r)
{
	r->L = L;
	r->hSize = L;
	return MyHandleNew(L, &(r->h));
}

GLOBALPROC xbh_UnInit(xbh_r *r)
{
	DisposHandle(r->h);
}

#define xbh_GetH(r) ((r)->h)

#define xbh_GetLen(r) ((r)->L)

GLOBALFUNC blnr xbh_SetLen(xbh_r *r, uimr L)
{
	if (L > r->hSize) {
		if (! MyHandleSetLen(r->h, L)) {
			return falseblnr;
		} else {
			r->hSize = L;
		}
	}
	r->L = L;
	return trueblnr;
}

GLOBALFUNC blnr xbh_AppendPtr(xbh_r *r, MyPtr p, uimr L)
{
	uimr oldL = r->L;
	if (! xbh_SetLen(r, oldL + L)) {
		return falseblnr;
	}
	MyMoveBytes(p, (*r->h) + oldL, L);
	return trueblnr;
}

GLOBALFUNC blnr xbh_AppendHandRange(xbh_r *r, Handle h,
	uimr offset, uimr L)
{
	uimr oldL = r->L;
	if (! xbh_SetLen(r, oldL + L)) {
		return falseblnr;
	}
	MyMoveBytes((MyPtr)(*h) + offset, (*r->h) + oldL, L);
	return trueblnr;
}

GLOBALFUNC blnr xbh_AppendPStr(xbh_r *r, ps3p s)
{
	return xbh_AppendPtr(r, PStrToPtr(s), PStrToSize(s));
}

GLOBALFUNC blnr xbh_AppendCStr(xbh_r *r, char *s)
{
	return xbh_AppendPtr(r, (MyPtr)s, CStrLength(s));
}

GLOBALFUNC blnr xbh_Append_xbh(xbh_r *r, xbh_r *x)
{
	return xbh_AppendHandRange(r, xbh_GetH(x), 0, xbh_GetLen(x));
}
