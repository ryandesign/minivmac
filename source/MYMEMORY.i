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
		err = kMyErr_noErr;
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

#if HaveTempBuffH
GLOBALFUNC tMyErr MyHandleNew_v2(uimr L, Handle *h)
{
	/* *h must be set to NULL if fail */

	tMyErr err;

	if (noErr == (err = MyHandleNew0_v2(L, h))) {
		err = GetTempBuffH_v2();
		if (noErr != err) {
			DisposeHandle(*h);
			*h = NULL;
		}
	}

	return err;
}
#else
#define MyHandleNew_v2 MyHandleNew0_v2
#endif

#define MyHandle Handle

GLOBALFUNC tMyErr MyHandleDispose_v2(MyHandle h)
{
	DisposeHandle(h);

	return kMyErr_noErr;
}

GLOBALFUNC tMyErr MyHandleSetSize_v2(MyHandle h, uimr L)
{
	tMyErr err;
	uimr oldL = GetHandleSize(h);

	SetHandleSize(h, L);
	if (GetHandleSize(h) != L) {
		err = ReportMemErr();
	} else {
#if HaveTempBuffH
		err = GetTempBuffH_v2();
		if (noErr != err) {
			if (oldL < L) {
				SetHandleSize(h, oldL);
			}
		}
#else
		err = kMyErr_noErr;
#endif
	}

	return ErrReportStack(err, "MyHandleSetSize_v2");
}

LOCALFUNC tMyErr MyHandleGetSize_v2(MyHandle h, uimr *L)
{
	*L = GetHandleSize(h);

	return kMyErr_noErr;
}

LOCALPROC MyHandleLock(MyHandle h)
{
	HLock(h);
}

LOCALPROC MyHandleUnlock(MyHandle h)
{
	HUnlock(h);
}

LOCALFUNC MyPtr MyHandleP(MyHandle h)
{
	return ((MyPtr)*h);
}

LOCALFUNC MyPtr MyHandleOffsetToPtr(MyHandle h, uimr offset)
{
	return ((MyPtr)*h) + offset;
}

GLOBALFUNC tMyErr MyMemoryCheckSpare(void)
{
	return GetTempBuffH_v2();
}
