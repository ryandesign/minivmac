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


LOCALFUNC tMyErr ReportMemErr(void)
{
	tMyErr err = MemError();

	if (noErr == err) {
		err = kMyErrSysUnknown;
	}

	return err;
}

GLOBALFUNC tMyErr MyHandleNew0_v2(uimr L, Handle *h)
{
	tMyErr err;
	Handle h0 = NewHandle(L);
	*h = h0;
	if (NULL == h0) {
		err = ReportMemErr();
	} else {
		err = noErr;
	}

	return err;
}

GLOBALVAR Handle TempBuffH = NULL;

#define TempBuffSize (32 * (long)1024)
	/* may become variable in future */

GLOBALFUNC tMyErr MyMemory_Init_v2(void)
{
	tMyErr err;

	err = MyHandleNew0_v2(TempBuffSize, &TempBuffH);
	if (noErr == err) {
		HPurge(TempBuffH);
	}

	return err;
}

GLOBALPROC MyMemory_UnInit(void)
{
	if (NULL != TempBuffH) {
		DisposeHandle(TempBuffH);
	}
}

#define HaveTempBuffH 1

LOCALFUNC tMyErr ReallocTempBuffH_v2(void)
{
	tMyErr err;

	ReallocateHandle(TempBuffH, TempBuffSize);
	if ((NULL == *TempBuffH) ||
		(GetHandleSize(TempBuffH) != TempBuffSize))
	{
		err = ReportMemErr();
	} else {
		HPurge(TempBuffH);
		err = noErr;
	}

	return err;
}

#define GetTempBuffH_v2() ((NULL != *TempBuffH) ? noErr \
	: ReallocTempBuffH_v2())

GLOBALFUNC blnr TempBuffHBeginUse_v2(MyPtr *p)
{
	tMyErr err = GetTempBuffH_v2();

	if (noErr == err) {
		/* HNoPurge(TempBuffH); */
		HLock(TempBuffH);
		*p = (MyPtr)*TempBuffH;
	}

	return err;
}

GLOBALPROC TempBuffHEndUse(void)
{
	HUnlock(TempBuffH);
	/* HPurge(TempBuffH); */
}

GLOBALFUNC tMyErr MyHandleNew_v2(uimr L, Handle *h)
{
	/* *h must be set to NULL if fail */

	tMyErr err = MyHandleNew0_v2(L, h);

	if (noErr == err) {
		err = GetTempBuffH_v2();
		if (noErr != err) {
			DisposeHandle(*h);
			*h = NULL;
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyHandleSetLen_v2(Handle h, uimr L)
{
	tMyErr err;
	uimr oldL = GetHandleSize(h);

	SetHandleSize(h, L);
	if (GetHandleSize(h) != L) {
		err = ReportMemErr();
	} else {
		err = GetTempBuffH_v2();
		if (noErr != err) {
			if (oldL < L) {
				SetHandleSize(h, oldL);
			}
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyMemoryCheckSpare(void)
{
	return GetTempBuffH_v2();
}

/* utilities */

LOCALPROC MyNHandleClear(Handle *h)
{
	if (*h != nullpr) {
		DisposeHandle(*h);
		*h = nullpr;
	}
}

LOCALFUNC tMyErr MyNHandleSetLen_v2(Handle *h, uimr L)
{
	tMyErr err;

	if (L == 0) {
		MyNHandleClear(h);
		err = noErr;
	} else {
		if (*h == nullpr) {
			err = MyHandleNew_v2(L, h);
		} else {
			err = MyHandleSetLen_v2(*h, L);
		}
	}

	return err;
}

LOCALFUNC tMyErr MyPtrToHandle_v2(MyPtr p, uimr L, Handle *r)
{
	tMyErr err;
	Handle h;

	if (noErr == (err = MyHandleNew_v2(L, &h))) {
		MyMoveBytes(p, (MyPtr)*h, L);
		*r = h;
	}

	return err;
}

LOCALFUNC tMyErr HandRangeToHandle_v2(Handle x,
	uimr offset, uimr L, Handle *r)
{
	tMyErr err;
	Handle h;

	if (noErr == (err = MyHandleNew_v2(L, &h))) {
		MyMoveBytes(((MyPtr)*x + offset), (MyPtr)*h, L);
		*r = h;
	}

	return err;
}

#ifdef Have_STRUTILS
LOCALFUNC tMyErr CStr2Hand_v2(char *s, Handle *r)
{
	/* on failure *r either set to nullpr or left unchanged */
	return MyPtrToHandle_v2((MyPtr)s, CStrLength(s), r);
}
#endif

#ifdef Have_STRUTILS
LOCALFUNC tMyErr PStr2Hand_v2(ps3p s, Handle *r)
{
	/* on failure *r either set to nullpr or left unchanged */
	return MyPtrToHandle_v2(PStrToPtr(s), PStrToSize(s), r);
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
LOCALPROC HandRangeToCStr(Handle x,
	uimr offset, uimr L, char *s)
{
	CStrFromPtr(((MyPtr)*x + offset), L, s);
}
#endif

#ifdef Have_STRUTILS
LOCALPROC HandToPStr(Handle x, ps3p s)
{
	HandRangeToPStr(x, 0, GetHandleSize(x), s);
}
#endif
