/*
	XBUFHAND.i
	Copyright (C) 2009 Paul C. Pratt

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

#define xbh_ZapVal {NULL, 0, 0}

#define xbh_Initted(r) (NULL != (r)->h)
	/* works assuming r initialized to xbh_ZapVal */

GLOBALFUNC tMyErr xbh_Init_v2(uimr L, xbh_r *r)
{
	r->L = L;
	r->hSize = L;
	return MyHandleNew_v2(L, &(r->h));
}

GLOBALPROC xbh_UnInit(xbh_r *r)
{
	if (NULL != r->h) {
		DisposeHandle(r->h);
	}
}

#define xbh_GetH(r) ((r)->h)

#define xbh_GetLen(r) ((r)->L)

GLOBALFUNC tMyErr xbh_SetSize_v2(xbh_r *r, uimr L)
{
	tMyErr err = noErr;

	if (L > r->hSize) {
		err = MyHandleSetSize_v2(r->h, L);
		if (noErr != err) {
			goto Label_err;
		}
		r->hSize = L;
	}

	r->L = L;

Label_err:
	return err;
}

#define xbh_SetEmpty(r) (r)->L = 0

GLOBALFUNC tMyErr xbh_AppendPtr_v2(xbh_r *r, MyPtr p, uimr L)
{
	tMyErr err;
	uimr OldLen = r->L;

	err = xbh_SetSize_v2(r, OldLen + L);
	if (noErr == err) {
		MyMoveBytes(p, (MyPtr)(*r->h) + OldLen, L);
	}

	return err;
}

GLOBALFUNC tMyErr xbh_PopToPtr_v2(xbh_r *r, MyPtr p, uimr L)
{
	tMyErr err;
	uimr OldLen = xbh_GetLen(r);
	if (OldLen < L) {
		err = kMyErrParamErr;
	} else {
		uimr NewLen = OldLen - L;
		MyMoveBytes((MyPtr)(*r->h) + NewLen, p, L);
		r->L = NewLen;
			/* (void) xbh_SetSize_v2(r, NewLen); */
		err = noErr;
	}

	return err;
}

GLOBALFUNC tMyErr xbh_AppendHandRange_v2(xbh_r *r, Handle h,
	uimr offset, uimr L)
{
	tMyErr err;
	uimr oldL = r->L;

	err = xbh_SetSize_v2(r, oldL + L);
	if (noErr == err) {
		MyMoveBytes((MyPtr)(*h) + offset, (MyPtr)(*r->h) + oldL, L);
	}

	return err;
}

GLOBALFUNC tMyErr xbh_AppendPStr_v2(xbh_r *r, ps3p s)
{
	return xbh_AppendPtr_v2(r, PStrToPtr(s), PStrToSize(s));
}

GLOBALFUNC tMyErr xbh_AppendCStr_v2(xbh_r *r, char *s)
{
	return xbh_AppendPtr_v2(r, (MyPtr)s, CStrLength(s));
}

GLOBALFUNC tMyErr xbh_Append_xbh_v2(xbh_r *r, xbh_r *x)
{
	return xbh_AppendHandRange_v2(r, xbh_GetH(x), 0, xbh_GetLen(x));
}

GLOBALFUNC tMyErr xbh_SetFromPtr(xbh_r *r, MyPtr p, uimr L)
{
	tMyErr err;

	err = xbh_SetSize_v2(r, L);
	if (noErr == err) {
		MyMoveBytes(p, (MyPtr)*r->h, L);
	}

	return err;
}

GLOBALFUNC tMyErr xbh_SetFromCStr(xbh_r *r, char *s)
{
	return xbh_SetFromPtr(r, (MyPtr)s, CStrLength(s));
}

GLOBALFUNC tMyErr xbh_SetFromPStr(xbh_r *r, ps3p s)
{
	return xbh_SetFromPtr(r, PStrToPtr(s), PStrToSize(s));
}
