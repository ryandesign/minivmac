/*
	HANDUTIL.i
	Copyright (C) 2018 Paul C. Pratt

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
	HANDle UTILities
*/


#if DebugCheck
LOCALPROC MyHandleWriteToDbgLog(MyHandle h, char *s)
{
	tMyErr err;
	uimr L;

	if (kMyErr_noErr != (err =
		MyHandleGetSize_v2(h, &L)))
	{
		/* fail */
	} else
	{
		MyHandleLock(h);
		debug_dump_buffer(s, MyHandleP(h), L);
		MyHandleUnlock(h);
	}
}
#endif

LOCALFUNC tMyErr MyHandleNewFromPtr_v2(MyHandle *r, MyPtr p, uimr L)
{
	tMyErr err;
	MyHandle h;

	if (kMyErr_noErr == (err = MyHandleNew_v2(L, &h))) {
		MyMoveBytes(p, (MyPtr)*h, L);
		*r = h;
	}

	return ErrReportStack(err, "MyHandleNewFromPtr_v2");
}

LOCALFUNC tMyErr MyHandleNewFromHandPart(MyHandle *r,
	MyHandle x, uimr offset, uimr L)
{
	tMyErr err;
	MyHandle h;

	if (kMyErr_noErr == (err = MyHandleNew_v2(L, &h))) {
		MyMoveBytes(((MyPtr)*x + offset), (MyPtr)*h, L);
		*r = h;
	}

	return ErrReportStack(err, "MyHandleNewFromHandPart");
}

#ifdef Have_STRUTILS
LOCALFUNC tMyErr MyHandleNewFromCStr(MyHandle *r, char *s)
{
	/* on failure *r either set to nullpr or left unchanged */
	return MyHandleNewFromPtr_v2(r, (MyPtr)s, CStrLength(s));
}
#endif

#ifdef Have_STRUTILS
LOCALFUNC tMyErr MyHandleNewFromPStr(MyHandle *r, ps3p s)
{
	/* on failure *r either set to nullpr or left unchanged */
	return MyHandleNewFromPtr_v2(r, PStrToPtr(s), PStrToSize(s));
}
#endif

#ifdef Have_STRUTILS
LOCALPROC MyHandlePartToPStr(MyHandle x,
	uimr offset, uimr L, ps3p s)
{
	PStrFromPtr(((MyPtr)*x + offset), L, s);
}
#endif

#ifdef Have_STRUTILS
LOCALPROC MyHandlePartToCStr(MyHandle x,
	uimr offset, uimr L, char *s)
{
	CStrFromPtr(((MyPtr)*x + offset), L, s);
}
#endif

#ifdef Have_STRUTILS
LOCALFUNC tMyErr MyHandleToPStr_v2(MyHandle x, ps3p s)
{
	tMyErr err;
	uimr L;

	if (kMyErr_noErr != (err =
		MyHandleGetSize_v2(x, &L)))
	{
		/* fail */
	} else
	{
		MyHandlePartToPStr(x, 0, L, s);
		err = kMyErr_noErr;
	}

	return ErrReportStack(err, "MyHandleToPStr_v2");
}
#endif

#ifdef Have_STRUTILS
LOCALFUNC tMyErr MyHandleToCStr(MyHandle x, char *s)
{
	tMyErr err;
	uimr L;

	if (kMyErr_noErr != (err =
		MyHandleGetSize_v2(x, &L)))
	{
		/* fail */
	} else
	{
		MyHandlePartToCStr(x, 0, L, s);
		err = kMyErr_noErr;
	}

	return ErrReportStack(err, "MyHandleToCStr");
}
#endif

LOCALFUNC tMyErr NHandleClear_v2(MyHandle *h)
{
	tMyErr err;
	MyHandle x;

	if (nullpr == (x = *h)) {
		err = kMyErr_noErr;
	} else {
		*h = nullpr;
		err = MyHandleDispose_v2(x);
	}

	return ErrReportStack(err, "NHandleClear_v2");
}

LOCALFUNC tMyErr NHandleSetSize_v2(MyHandle *h, uimr L)
{
	tMyErr err;

	if (L == 0) {
		err = NHandleClear_v2(h);
	} else {
		if (*h == nullpr) {
			err = MyHandleNew_v2(L, h);
		} else {
			err = MyHandleSetSize_v2(*h, L);
		}
	}

	return ErrReportStack(err, "NHandleSetSize_v2");
}

LOCALFUNC tMyErr NHandleFromHandPart(MyHandle *r,
	MyHandle x, uimr offset, uimr L)
{
	tMyErr err;

	if (kMyErr_noErr == (err = NHandleSetSize_v2(r, L))) {
		MyHandle h = *r;

		MyMoveBytes(((MyPtr)*x + offset), (MyPtr)*h, L);
	}

	return err;
}

LOCALFUNC tMyErr NHandleFromPtr(MyHandle *r, MyPtr p, uimr L)
{
	tMyErr err;

	if (kMyErr_noErr == (err = NHandleSetSize_v2(r, L))) {
		MyMoveBytes(p, (MyPtr)*(*r), L);
	}

	return ErrReportStack(err, "NHandleFromPtr");
}

#ifdef Have_STRUTILS
LOCALFUNC tMyErr NHandleFromCStr(MyHandle *r, char *s)
{
	return NHandleFromPtr(r, (MyPtr)s, CStrLength(s));
}
#endif
