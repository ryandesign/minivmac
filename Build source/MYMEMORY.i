/*
	MYMEMORY.i
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
	MY MEMORY management
*/


LOCALPROC ReportMemErr(void)
{
	OSErr iErr = MemError();
	if (noErr != iErr) {
		CheckSysErr(iErr);
	} else {
		CheckSysErr(1024);
	}
}

GLOBALFUNC blnr MyHandleNew0(uimr L, Handle *h)
{
	blnr IsOk = falseblnr;
	Handle h0 = NewHandle(L);
	*h = h0;
	if (NULL == h0) {
		ReportMemErr();
	} else {
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALVAR Handle TempBuffH = NULL;

#define TempBuffSize (32 * (long)1024)
	/* may become variable in future */

GLOBALFUNC blnr MyMemory_Init(void)
{
	blnr IsOk = falseblnr;

	if (MyHandleNew0(TempBuffSize, &TempBuffH)) {
		HPurge(TempBuffH);
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALPROC MyMemory_UnInit(void)
{
	if (NULL != TempBuffH) {
		DisposHandle(TempBuffH);
	}
}

#define HaveTempBuffH 1

GLOBALFUNC blnr ReallocTempBuffH(void)
{
	blnr IsOk = falseblnr;

	ReallocHandle(TempBuffH, TempBuffSize);
	if ((NULL == *TempBuffH) ||
		GetHandleSize(TempBuffH) != TempBuffSize)
	{
		ReportMemErr();
	} else {
		HPurge(TempBuffH);
		IsOk = trueblnr;
	}

	return IsOk;
}

#define GetTempBuffH() ((NULL != *TempBuffH) || ReallocTempBuffH())

GLOBALFUNC blnr TempBuffHBeginUse(MyPtr *p)
{
	blnr IsOk = falseblnr;

	if (GetTempBuffH()) {
		/* HNoPurge(TempBuffH); */
		HLock(TempBuffH);
		*p = (MyPtr)*TempBuffH;
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALPROC TempBuffHEndUse(void)
{
	HUnlock(TempBuffH);
	/* HPurge(TempBuffH); */
}

GLOBALFUNC blnr MyHandleNew(uimr L, Handle *h)
{
	blnr IsOk = falseblnr;

	if (! MyHandleNew0(L, h)) {
	} else if (! GetTempBuffH()) {
		DisposHandle(*h);
	} else {
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALFUNC blnr MyHandleSetLen(Handle h, uimr L)
{
	uimr oldL = GetHandleSize(h);
	blnr IsOk = falseblnr;

	SetHandleSize(h, L);
	if (GetHandleSize(h) != L) {
		ReportMemErr();
	} else if (! GetTempBuffH()) {
		if (oldL < L) {
			SetHandleSize(h, oldL);
		}
	} else {
		IsOk = trueblnr;
	}

	return IsOk;
}

/* utilities */

LOCALFUNC blnr MyPtrToHandle(Ptr p, uimr L, Handle *r)
{
	Handle h;
	blnr IsOk = falseblnr;

	if (MyHandleNew(L, &h)) {
		MyMoveBytes(p, (MyPtr)*h, L);
		*r = h;
		IsOk = trueblnr;
	}

	return IsOk;
}

LOCALFUNC blnr HandRangeToHandle(Handle x,
	uimr offset, uimr L, Handle *r)
{
	Handle h;
	blnr IsOk = falseblnr;

	if (MyHandleNew(L, &h)) {
		MyMoveBytes(((MyPtr)*x + offset), (MyPtr)*h, L);
		*r = h;
		IsOk = trueblnr;
	}

	return IsOk;
}

#ifdef Have_STRUTILS
LOCALFUNC blnr CStr2Hand(char *s, Handle *r)
{
	return MyPtrToHandle((MyPtr)s, CStrLength(s), r);
}
#endif

#ifdef Have_STRUTILS
LOCALFUNC blnr PStr2Hand(ps3p s, Handle *r)
{
	return MyPtrToHandle(PStrToPtr(s), PStrToSize(s), r);
}
#endif

#ifdef Have_STRUTILS
LOCALPROC HandRangeToPStr(Handle x,
	uimr offset, uimr L, ps3p s)
{
	PStrFromPtr(((MyPtr)*x + offset), L, s);
}
#endif

#ifdef Have_STRUTILS
LOCALPROC HandToPStr(Handle x, ps3p s)
{
	HandRangeToPStr(x, 0, GetHandleSize(x), s);
}
#endif
