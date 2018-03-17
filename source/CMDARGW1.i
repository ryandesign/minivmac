/*
	CMDARGW1.i
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
	CoMmandD ARGument Window part 1

	inspired by Apple's TESample that came with MPW 3
*/


LOCALPROC ResumeSystemCursor(void)
{
	SetCursor(&(qd.arrow));
}

/* -- error reporting -- */

#define EmptyStrConst ((StringPtr)"\000")

#define MouseOrKeyMask \
	(mDownMask | mUpMask | keyDownMask | keyUpMask | autoKeyMask)

#define rAboutAlert 128 /* about alert */
#define rTextAlert  129 /* user error alert */

LOCALPROC MyAlertFromIdPStr(SInt16 alertID, StringPtr s)
{
	ResumeSystemCursor();
	ParamText(s, EmptyStrConst, EmptyStrConst, EmptyStrConst);
	FlushEvents(MouseOrKeyMask, 0);
	ResetAlertStage();
	(void) Alert(alertID, NULL);
}

LOCALPROC MyAlertFromPStr(ps3p message)
{
	MyAlertFromIdPStr(rTextAlert, message);
}

GLOBALPROC MyAlertFromCStr(char *s)
{
	MyPStr t;

	PStrFromCStr(t, s);
	MyAlertFromPStr(t);
}

GLOBALPROC ReportUnhandledErr(tMyErr err)
{
	if ((kMyErr_noErr == err)
		|| (kMyErrReported == err)
		|| (kMyErrUsrCancel == err)
		|| (kMyErrUsrAbort == err))
	{
		/* nothing more needed */
	} else if (noErr != err) {
		MyAlertFromCStr(GetTextForSavedSysErr_v2(err));
	}
}

LOCALPROC GetVersLongStr(StringPtr s)
{
	ui3p p;
	Handle h = GetResource('vers', 1);
	if (NULL == h) {
		PStrClear(s);
	} else {
		HLock(h);
		p = (ui3p)*h;
		p += 6;
		p += *p + 1;
		PStrFromPtr((MyPtr)p + 1, *p, s);
		HUnlock(h);
	}
}

LOCALPROC ShowAboutMessage(void)
{
	MyPStr s;

	GetVersLongStr(s);
	MyAlertFromIdPStr(rAboutAlert, s);
}

#define eWrongMachine 1
#define eSmallSize    2
#define eNoMemory     3
#define eNoSpaceCut   4
#define eNoCut        5
#define eNoCopy       6
#define eExceedPaste  7
#define eNoSpacePaste 8
#define eNoWindow     9
#define eExceedChar   10
#define eNoPaste      11

#define kErrStrings 128 /* error string list */

LOCALPROC AlertUser(short error)
{
	Str255 message;

	GetIndString(message, kErrStrings, error);
	MyAlertFromPStr(message);
}

LOCALFUNC tMyErr DeskScrapFromHandle(Handle h)
{
	tMyErr err;

	if (noErr != (err = ZeroScrap())) {
		goto l_exit;
	}

	if (noErr != (err = MyMemoryCheckSpare())) {
		goto l_exit;
	}

	HLock(h);

	err = PutScrap(
			GetHandleSize(h),
			FOUR_CHAR_CODE('TEXT'),
			*h);

	HUnlock(h);

l_exit:
	return err;
}

LOCALFUNC tMyErr DeskScrapToHand(Handle *h)
{
	tMyErr err;
	long L;
	SInt32 offset;
	Handle h0 = nullpr;

	L = GetScrap(NULL,
		FOUR_CHAR_CODE('TEXT'),
		&offset);
	if (L < 0) {
		err = MyHandleNew_v2(0, &h0);
	} else {
		if (noErr == (err = MyHandleNew_v2(L, &h0))) {
			L = GetScrap(h0,
				FOUR_CHAR_CODE('TEXT'),
				&offset);
			if (L < 0) {
				err = kMyErrSysUnknown;
			} else {
				err = noErr;
			}
		}
	}

	if (noErr == err) {
		*h = h0;
	} else {
		if (nullpr != h0) {
			DisposeHandle(h0);
		}
	}

	return err;
}

LOCALFUNC tMyErr HTCtoHand(Handle *h)
{
	tMyErr err;
	tPbuf Pbuf_No;
	uimr L;
	Handle h0 = nullpr;

	if (noErr == (err = HTCImport_v2(&Pbuf_No))) {
		if (noErr == (err = PBufGetSize_v2(Pbuf_No, &L))) {
			if (noErr == (err = MyHandleNew_v2(L, &h0))) {
				HLock(h0);
				err = PbufTransfer_v2(*h0,
					Pbuf_No, 0, L, falseblnr);
				HUnlock(h0);
			}
		}

		err = ErrCombine(err, PbufDispose_v2(Pbuf_No));
	}

	if (noErr == err) {
		*h = h0;
	} else {
		if (nullpr != h0) {
			DisposeHandle(h0);
		}
	}

	return err;
}

LOCALFUNC tMyErr HTCfromHandRange(Handle h, uimr offset, uimr L)
{
	tMyErr err;
	tPbuf Pbuf_No;

	if (noErr != (err = PbufNew_v2(L, &Pbuf_No))) {
		/* fail */
	} else {
		HLock(h);

		err = PbufTransfer_v2(*h + offset,
			Pbuf_No, 0, L, trueblnr);

		HUnlock(h);

		if (noErr == err) {
			err = HTCExport_v2(Pbuf_No);
		}

		if (noErr != PbufDispose_v2(Pbuf_No)) {
			/* oops */
		}
	}

	return err;
}

LOCALFUNC tMyErr SaferTENew(const Rect *destRect, const Rect *viewRect,
	TEHandle *hTE)
{
	tMyErr err;
	TEHandle hTE0;

	if (noErr == (err = MyMemoryCheckSpare())) {
		hTE0 = TENew(destRect, viewRect);
		if (NULL == hTE0) {
			err = kMyErrSysUnknown;
		} else {
			if (noErr == (err = MyMemoryCheckSpare())) {
				*hTE = hTE0;
			} else {
				TEDispose(hTE0);
			}
		}
	}

	return err;
}

LOCALFUNC tMyErr SaferNewControl(WindowPtr theWindow,
	const Rect *boundsRect, ConstStr255Param title, Boolean visible,
	short value, short min, short max,
	short procID, long refCon, ControlRef *theControl)
{
	tMyErr err;
	ControlRef theControl0;

	if (noErr == (err = MyMemoryCheckSpare())) {
		theControl0 = NewControl(theWindow, boundsRect,
			title, visible, value, min, max, procID, refCon);
		if (NULL == theControl0) {
			err = kMyErrSysUnknown;
		} else {
			if (noErr == (err = MyMemoryCheckSpare())) {
				*theControl = theControl0;
			} else {
				DisposeControl(theControl0);
			}
		}
	}

	return err;
}


LOCALVAR TEHandle MyDocTE = NULL;
LOCALVAR ControlRef MyDocVScroll = NULL;

#define kMaxTELength 32000

#define kCrChar 13
#define kDelChar 8

static pascal void MyDocTrackAction(ControlRef control,
	ControlPartCode part)
{
	short amount;
	short NewV;
	short OldV = GetControlValue(control);
	short max = GetControlMaximum(control);

	if ((part != 0) && (control == MyDocVScroll)) {
		/* if it was actually in the control */

		switch (part) {
			case kControlUpButtonPart:
			case kControlDownButtonPart: /* one line */
				amount = (*MyDocTE)->lineHeight;
				break;
			case kControlPageUpPart: /* one page */
			case kControlPageDownPart:
				amount = ((*MyDocTE)->viewRect.bottom
					- (*MyDocTE)->viewRect.top);
				break;
		}

		if ((part == kControlDownButtonPart)
			|| (part == kControlPageDownPart))
		{
			NewV = OldV + amount;
		} else {
			NewV = OldV - amount;
		}

		if (NewV < 0) {
			NewV = 0;
		} else if (NewV > max) {
			NewV = max;
		}

		if (NewV != OldV) {
			SetControlValue(control, NewV);
			TEScroll(0, OldV - NewV, MyDocTE);
		}
	}
}

LOCALPROC MyDocAdjustTE(void)
/*
	Adjust MyDocTE to match state of MyDocVScroll
*/
{
	Rect rView = (*MyDocTE)->viewRect;
	short curpix = rView.top - (*MyDocTE)->destRect.top;
	short OldV = GetControlValue(MyDocVScroll);

	if (OldV != curpix) {
		TEScroll(0, curpix - OldV, MyDocTE);
	}
}

LOCALPROC MyDocCheckScrollPosMax(void)
/*
	Adjust MyDocVScroll to match state of MyDocTE
*/
{
	short n;
	short VMax;
	Rect rView = (*MyDocTE)->viewRect;
	/* short sheight = rView.bottom - rView.top; */
	short curpix = rView.top - (*MyDocTE)->destRect.top;

	n = (*MyDocTE)->nLines;
#if 0
	if ((*MyDocTE)->teLength > 0) {
		if (((char *)(*((*MyDocTE)->hText)))[(*MyDocTE)->teLength - 1]
			== chrCR)
		{
			n += 1;
		}
	}
#endif
	VMax = n * (*MyDocTE)->lineHeight;
	VMax -= /* sheight */ (*MyDocTE)->lineHeight;
	if (VMax < 0) {
		VMax = 0;
	}

	if (VMax != GetControlMaximum(MyDocVScroll)) {
		SetControlMaximum(MyDocVScroll, VMax);
	}
	if (curpix != GetControlValue(MyDocVScroll)) {
		SetControlValue(MyDocVScroll, curpix);
	}
}

LOCALVAR long LastLoopTick = 0;

static pascal void MyDocClikLoop(void)
{
	RgnHandle region;
	Point curP;
	short d;
	long MinD;
	Rect View = (*MyDocTE)->viewRect;
	ControlPartCode part = 0;

	GetMouse(&curP);

	if (curP.v > View.bottom) {
		part = kControlDownButtonPart;
		d = curP.v - View.bottom;
	} else if (curP.v < View.top) {
		part = kControlUpButtonPart;
		d = View.top - curP.v;
	}

	if (0 != part) {
		long CurrentTick = LMGetTicks();
		unsigned long TickDelta = CurrentTick - LastLoopTick;

		if (d < 4) {
			MinD = 16;
		} else if (d < 16) {
			MinD = 8;
		} else  if (d < 32) {
			MinD = 4;
		} else {
			MinD = 2;
		}

		if (TickDelta >= MinD) {
			LastLoopTick = CurrentTick;

			region = NewRgn();
			if (NULL != region) {
				GetClip(region); /* save clip */
				/* My_GetWindowBounds(MyMainWind, &r); */
				ClipRect(&qd.thePort->portRect);

				MyDocTrackAction(MyDocVScroll, part);

				SetClip(region); /* restore clip */
				DisposeRgn(region);
			}
		}
	}
}

LOCALPROC MyDocScrollToSel(void)
{
	short curlinetop;
	short curlinebot;
	short ScrollPos;
	short curpix;
	short sheight;
	Rect rView;
	short i;
	long dV;
	short VMax;
	short s = (*MyDocTE)->selStart;

	i = (*MyDocTE)->nLines - 1;
	if (i >= 0) {
		while ((*MyDocTE)->lineStarts[i] > s) {
			--i;
		}
	} else {
		i = 0;
	}

	curlinetop = i * (*MyDocTE)->lineHeight;
	curlinebot = curlinetop + (*MyDocTE)->lineHeight;
	rView = (*MyDocTE)->viewRect;
	curpix = rView.top - (*MyDocTE)->destRect.top;
	sheight = rView.bottom - rView.top;

	VMax = GetControlMaximum(MyDocVScroll);

	if (curlinetop < curpix || curlinebot > curpix + sheight) {
		ScrollPos = (curlinetop + curlinebot) / 2 - (sheight / 2);
		if (ScrollPos < 0) {
			ScrollPos = 0;
		} else if (ScrollPos > VMax) {
			ScrollPos = VMax;
		}
		dV = curpix - ScrollPos;
		if (0 != dV) {
			TEScroll(/*dh*/ 0, dV, MyDocTE);
			SetControlValue(MyDocVScroll, ScrollPos);
		}
	}
}

LOCALPROC MyDocAfterEditScroll(void)
{
	MyDocCheckScrollPosMax();
	MyDocScrollToSel();
}

LOCALVAR short MyDocUndoSelStart = 0;
LOCALVAR short MyDocUndoSelEnd = 0;
LOCALVAR Handle MyDocUndoH = nullpr;

LOCALFUNC tMyErr MyDocSelToUndoH(void)
{
	tMyErr err;
	Handle texth = (Handle) TEGetText(MyDocTE);
	short curselstart = (*MyDocTE)->selStart;
	short curselend = (*MyDocTE)->selEnd;
	uimr L = curselend - curselstart;

	err = NHandleFromHandPart(&MyDocUndoH,
		texth, curselstart, L);

	return err;
}

LOCALFUNC tMyErr MyDocUndoHPrePendSel(void)
{
	tMyErr err;
	Handle texth = (Handle) TEGetText(MyDocTE);
	short curselstart = (*MyDocTE)->selStart;
	short curselend = (*MyDocTE)->selEnd;
	uimr OldL = GetHandleSize(MyDocUndoH);
	uimr L = curselend - curselstart;

	if (noErr == (err = NHandleSetSize_v2(&MyDocUndoH, OldL + L))) {
		MyMoveBytes((MyPtr)*MyDocUndoH, ((MyPtr)*MyDocUndoH + L), OldL);
		MyMoveBytes(((MyPtr)*texth + curselstart),
			(MyPtr)*MyDocUndoH, L);
	}

	return err;
}

LOCALFUNC tMyErr MyDocPasteHand(Handle h)
{
	tMyErr err;
	blnr UndoSpclCase = falseblnr;
	short curselstart = (*MyDocTE)->selStart;
	short curselend = (*MyDocTE)->selEnd;
	uimr OldL = curselend - curselstart;
	uimr NewL = (nullpr == h) ? 0 : GetHandleSize(h);

	if (((*MyDocTE)->teLength - OldL + NewL)
		> kMaxTELength)
	{
		AlertUser(eExceedPaste);
		err = kMyErrReported;
		goto l_exit;
	}

	if (noErr != (err = MyMemoryCheckSpare())) {
		/* fail */
		AlertUser(eNoSpacePaste);
		err = kMyErrReported;
		goto l_exit;
	}

	if (curselend == MyDocUndoSelEnd) {
		if (0 != NewL) {
			if ((1 == NewL) && (0 == OldL)) {
				/* insert at end of new text */
				MyDocUndoSelEnd = curselstart + NewL;
				UndoSpclCase = trueblnr;
			}
		} else {
			if (1 == OldL) {
				MyDocUndoSelEnd = curselstart;
				if ((curselstart >= MyDocUndoSelStart))
				{
					/* delete from end of new text */
				} else {
					/* delete more of old text */
					MyDocUndoHPrePendSel();
					MyDocUndoSelStart = MyDocUndoSelEnd;
				}
				UndoSpclCase = trueblnr;
			}
		}
	}

	if (! UndoSpclCase) {
		(void) MyDocSelToUndoH();
		MyDocUndoSelStart = curselstart;
		MyDocUndoSelEnd = curselstart + NewL;
	}

	if (0 == NewL) {
		TEDelete(MyDocTE);
	} else
	{
#if 0
		/*
			would rather not have locked handle
			while memory may be allocated
		*/

		TEDelete(MyDocTE);
		HLock(h);
		TEInsert(*h, NewL, MyDocTE);
		HUnlock(h);
#endif

		UInt16 SaveTEScrpLength = LMGetTEScrpLength();
		Handle SaveTEScrpHandle = LMGetTEScrpHandle();

		LMSetTEScrpHandle(h);
		LMSetTEScrpLength(NewL);

		TEPaste(MyDocTE);

		LMSetTEScrpHandle(SaveTEScrpHandle);
		LMSetTEScrpLength(SaveTEScrpLength);
	}

	MyDocAfterEditScroll();

l_exit:
	return err;
}

LOCALFUNC tMyErr MyDocEditClear(void)
{
	return MyDocPasteHand(nullpr);
}

LOCALPROC MyDocBackSpace(void)
{
	short curselstart = (*MyDocTE)->selStart;
	short curselend = (*MyDocTE)->selEnd;

	if (curselstart != curselend) {
		(void) MyDocEditClear();
	} else
	if (0 == curselstart)
	{
		/* ignore */
	} else
	{
		TESetSelect(curselstart - 1, curselend, MyDocTE);
		(void) MyDocEditClear();
	}
}

LOCALVAR Handle MyDocInsertBuff = nullpr;

LOCALPROC MyDocPasteChar(char key)
{
	if (noErr == NHandleSetSize_v2(&MyDocInsertBuff, 1)) {
		(*MyDocInsertBuff)[0] = key;
		(void) MyDocPasteHand(MyDocInsertBuff);
	}
}

LOCALPROC MyDocControlChar(char key)
{
	TEKey(key, MyDocTE);
	MyDocAfterEditScroll();
}

LOCALFUNC blnr CharIsTypeable(char key)
{
	blnr v;

	if (((ui3r)key) >= 0x20) {
		v = (0x7F != key);
	} else {
		v = (0x09 == key) || (0x0D == key);
	}

	return v;
}

LOCALVAR blnr AbortRequested = falseblnr;
LOCALVAR blnr WaitingForInput = falseblnr;
LOCALVAR blnr MyDocEditable = falseblnr;

LOCALPROC MyDocKeyDown(char key)
{
	if (CharIsTypeable(key)) {
		if (MyDocEditable) {
			MyDocPasteChar(key);
		}
	} else {
		switch (key) {
			case 0x08: /* backspace */
				if (MyDocEditable) {
					MyDocBackSpace();
				}
				break;
			case 0x1B: /* escape */
				AbortRequested = trueblnr;
				break;
			case 0x1C: /* left arrow */
			case 0x1D: /* right arrow */
			case 0x1E: /* up arrow */
			case 0x1F: /* down arrow */
				MyDocControlChar(key);
				break;
		}
	}
}

LOCALPROC MyDocEditUndo(void)
{
	Handle h;

	TESetSelect(MyDocUndoSelStart, MyDocUndoSelEnd, MyDocTE);

	h = MyDocUndoH;
	MyDocUndoH = nullpr;
	MyDocUndoSelStart = 0;
	MyDocUndoSelEnd = 0;

	(void) MyDocPasteHand(h);

	if (h != nullpr) {
		DisposeHandle(h);
	}

	TESetSelect(MyDocUndoSelStart, MyDocUndoSelEnd, MyDocTE);
}

LOCALPROC MyDocSelectAll(void)
{
	TESetSelect(0, 32767, MyDocTE);
}

LOCALPROC MyDocSelectEnd(void)
{
	TESetSelect(32767, 32767, MyDocTE);
}

LOCALPROC MyDocReplaceEntireWithHand(Handle h)
{
	MyDocSelectAll();
	(void) MyDocPasteHand(h);
}

LOCALFUNC tMyErr MyDocSelToHand(Handle *h)
{
	tMyErr err;
	Handle texth = (Handle) TEGetText(MyDocTE);
	uimr L = (*MyDocTE)->selEnd - (*MyDocTE)->selStart;

	err = MyHandleNewFromHandPart(h, texth,
		(*MyDocTE)->selStart, L);

	return err;
}

LOCALFUNC tMyErr MyDocEditCopy(void)
{
	tMyErr err;
	Handle h;

	if (noErr == (err = MyDocSelToHand(&h))) {
		err = DeskScrapFromHandle(h);
		DisposeHandle(h);
	}

	return err;
}

LOCALFUNC tMyErr MyDocEditPaste(void)
{
	tMyErr err;
	Handle h;

	if (noErr == (err = DeskScrapToHand(&h))) {
		err = MyDocPasteHand(h);
		DisposeHandle(h);
	}

	return err;
}

LOCALFUNC tMyErr MyDocEditHostCopy(void)
{
	uimr L = (*MyDocTE)->selEnd - (*MyDocTE)->selStart;

	return HTCfromHandRange(TEGetText(MyDocTE),
		(*MyDocTE)->selStart, L);
}

LOCALFUNC tMyErr MyDocEditHostPaste(void)
{
	tMyErr err;
	Handle h;

	if (noErr == (err = HTCtoHand(&h))) {
		err = MyDocPasteHand(h);
		DisposeHandle(h);
	}

	return err;
}

LOCALPROC MyDocClearAll(void)
{
	if (0 != (*MyDocTE)->teLength) {
		MyDocSelectAll();
		(void) MyDocEditClear();
	}
}

#define My_GetWindowBounds(window, r) \
	GetPortBounds(GetWindowPort(window), (r))

LOCALVAR WindowRef MyMainWind = NULL;

LOCALVAR blnr ProgramDone = falseblnr;

#ifndef WantRealInputFile
#define WantRealInputFile 0
#endif

#ifndef AutoQuitIfStartUpFile
#define AutoQuitIfStartUpFile WantRealInputFile
#endif

#if AutoQuitIfStartUpFile
LOCALVAR blnr PastStartUpFiles = falseblnr;
LOCALVAR blnr SawStartUpFile = falseblnr;
#endif

#if WantRealInputFile
struct input_r { /* 16 bytes */
	MyDir_R d;
	ui4b filler1;
	Handle Name;
	ui5b filler2;
};
typedef struct input_r input_r;
#endif

#if WantRealInputFile
LOCALPROC DisposeInputR(input_r *r)
{
	DisposeHandle(r->Name);
}
#endif

#if WantRealInputFile
LOCALFUNC tMyErr Create_input_r_v2(MyDir_R *d, ps3p s, input_r *r)
{
	tMyErr err;
	Handle Name;

	if (noErr == (err = MyHandleNewFromPStr(&Name, s))) {
		r->d = *d;
		r->Name = Name;
	}

	return err;
}
#endif

#if WantRealInputFile
LOCALVAR xbh_r InputA = xbh_ZapVal;
#endif

#if WantRealInputFile
LOCALFUNC tMyErr InitInputA(void)
{
	return xbh_Init_v2(0, &InputA);
}
#endif

#if WantRealInputFile
LOCALFUNC blnr PopFromInputA(input_r *r)
{
	return (noErr == xbh_PopToPtr_v2(&InputA,
		(MyPtr)r, sizeof(input_r)));
}
#endif

#if WantRealInputFile
LOCALPROC ClearInputA(void)
{
	input_r r;

	while (PopFromInputA(&r)) {
		DisposeInputR(&r);
	}
}
#endif

#if WantRealInputFile
LOCALPROC UnInitInputA(void)
{
	if (xbh_Initted(&InputA)) {
		ClearInputA();
		xbh_UnInit(&InputA);
	}
}
#endif

#if WantRealInputFile
LOCALFUNC tMyErr ProcessInputFile_v2(MyDir_R *d, ps3p s)
{
	tMyErr err;
	input_r r;

#if AutoQuitIfStartUpFile
	if (! PastStartUpFiles) {
		SawStartUpFile = trueblnr;
	}
#endif

	if (noErr == (err = Create_input_r_v2(d, s, &r))) {
		if (noErr != (err = xbh_AppendPtr_v2(&InputA,
			(MyPtr)&r, sizeof(input_r))))
		{
			DisposeInputR(&r);
		} else {
			/* ok */
		}
	}

	return err;
}
#endif

#if WantRealInputFile
LOCALFUNC tMyErr ProcessInputFileFSSpec_v2(FSSpec *spec)
{
	MyDir_R d;

	d.VRefNum = spec->vRefNum;
	d.DirId = spec->parID;

	return ProcessInputFile_v2(&d, spec->name);
}
#endif

#if WantRealInputFile
LOCALFUNC tMyErr ProcessInputFileNameNamevRef_v2(
	ConstStr255Param fileName, short vRefNum)
{
	tMyErr err;
	MyDir_R d;

	if (noErr == (err = MyDirFromWD_v2(vRefNum, &d)))
	if (noErr == (err = ProcessInputFile_v2(&d, (ps3p)fileName)))
	{
		/* ok */
	}

	return err;
}
#endif

#ifndef InputMayBeFolder
#define InputMayBeFolder 0
#endif

#ifndef InputMayBeFile
#define InputMayBeFile 1
#endif

#ifndef InputFileIsGeneric
#define InputFileIsGeneric 1
#endif

#if WantRealInputFile
LOCALPROC DoGetOldFile(void)
{
	tMyErr err;
	MyDir_R d;
	Str255 s;
#if ! InputMayBeFolder
#if InputFileIsGeneric
#define pfInputType NULL
#define nInputTypes (-1)
#else
	long fInputType = InputFileType;
#define pfInputType ((ConstSFTypeListPtr)&fInputType)
#define nInputTypes 1
#endif
#endif

#if AutoQuitIfStartUpFile
	PastStartUpFiles = trueblnr;
#endif

#if InputMayBeFolder
	err = MySelectFolder(&d, s);
#else
	err = MyFileGetOld_v2(nInputTypes, pfInputType,
		&d, s);
#endif
	if (kMyErrUsrAbort == err) {
	} else {
		if (noErr == err) {
			err = ProcessInputFile_v2(&d, s);
		}

		ReportUnhandledErr(err);
	}
}
#endif

GLOBALVAR OSErr SavedSysErr = noErr;

GLOBALPROC vCheckSysErr(OSErr theErr)
{
	if (noErr != theErr) {
		if (noErr == SavedSysErr) {
#if 0
			Debugger();
#endif
			SavedSysErr = theErr;
		}
	}
}

#if WantRealInputFile
LOCALFUNC tMyErr MyAppFilesInit_v2(void)
{
	tMyErr err = noErr;
	short i;
	short message;
	short count;
	AppFile theFile;

	CountAppFiles(&message, &count);
	for (i = 1; i <= count; ++i) {
		GetAppFiles(i, &theFile);

		if (noErr != (err = ProcessInputFileNameNamevRef_v2(
			theFile.fName, theFile.vRefNum)))
		{
			goto error_exit;
		}

		ClrAppFiles(i);
	}

error_exit:
	return err;
}
#endif

#if WantRealInputFile
LOCALFUNC tMyErr ProcessInputFileFSSpecOrAlias_v2(FSSpec *spec)
{
	tMyErr err;
	Boolean isFolder;
	Boolean isAlias;

	if (noErr == (err = ResolveAliasFile(spec, true,
		&isFolder, &isAlias)))
	{
#if ! InputMayBeFolder
		if (isFolder) {
			err = paramErr;
		} else
#endif
#if ! InputMayBeFile
		if (! isFolder) {
			err = paramErr;
		} else
#endif
		{
			err = ProcessInputFileFSSpec_v2(spec);
		}
	}

	return err;
}
#endif

LOCALFUNC tMyErr GotRequiredParams_v2(const AppleEvent *theAppleEvent)
{
	tMyErr err;
	DescType typeCode;
	Size actualSize;

	/* Make sure there are no additional "required" parameters. */

	err = AEGetAttributePtr(theAppleEvent, keyMissedKeywordAttr,
		typeWildCard, &typeCode, NULL, 0, &actualSize);
	if (errAEDescNotFound == err) {
		/* no more required params */
		err = noErr;
	} else if (noErr == err) {
		/* missed required param */
		err = errAEEventNotHandled;
	} else {
		/* other error */
	}

	return err;
}

#if WantRealInputFile
#define openOnly 1
#define openPrint 2
#endif

#if WantRealInputFile
static pascal OSErr OpenOrPrintFiles(const AppleEvent *theAppleEvent,
	AppleEvent *reply, long aRefCon)
{
#pragma unused(reply, aRefCon)

	/*
		Adapted from IM VI: AppleEvent Manager:
			Handling Required AppleEvents
	*/
	tMyErr err;
	AEDescList docList;
	simr itemsInList;
	simr index;
	AEKeyword keywd;
	DescType typeCode;
	FSSpec myFSS;
	Size actualSize;

	/* put the direct parameter (a list of descriptors) into docList */
	if (noErr == (err = AEGetParamDesc(theAppleEvent, keyDirectObject,
		typeAEList, &docList)))
	{
		if (noErr == (err = GotRequiredParams_v2(theAppleEvent)))
			/* Check for missing required parameters */
		{
			if (noErr == (err = AECountItems(&docList, &itemsInList)))
			{
				for (index = 1; index <= itemsInList; ++index) {
					/*
						Get each descriptor from the list,
						get the alias record, open the file,
						maybe print it.
					*/
					if (noErr == (err = AEGetNthPtr(&docList, index,
						typeFSS, &keywd, &typeCode,
						(Ptr)&myFSS, sizeof(myFSS), &actualSize)))
					if (noErr == (err = ProcessInputFileFSSpec_v2(
						&myFSS /* , aRefCon == openPrint */)))
					{
						/* ok */
					}
					if (noErr != err) {
						goto label_fail;
					}
				}
label_fail:
				;
			}
		}
		err = ErrCombine(err, AEDisposeDesc(&docList));
	}

	vCheckSysErr(err);

	return noErr;
}
#endif

static pascal OSErr DoOpenEvent(const AppleEvent *theAppleEvent,
	AppleEvent *reply, long aRefCon)
/*
	This is the alternative to getting an open document event
	on startup.
*/
{
#pragma unused(reply, aRefCon)
	tMyErr err;

#if AutoQuitIfStartUpFile
	PastStartUpFiles = trueblnr;
#endif
	err = GotRequiredParams_v2(theAppleEvent);

	vCheckSysErr(err);

	return noErr;
}

static pascal OSErr DoQuitEvent(const AppleEvent *theAppleEvent,
	AppleEvent *reply, long aRefCon)
{
#pragma unused(reply, aRefCon)
	tMyErr err;

	if (noErr == (err = GotRequiredParams_v2(theAppleEvent)))
	{
		ProgramDone = trueblnr;
	}

	vCheckSysErr(err);

	return noErr;
}

LOCALFUNC tMyErr MyInstallEventHandler(AEEventClass theAEEventClass,
	AEEventID theAEEventID, AEEventHandlerProcPtr p,
	long handlerRefcon, blnr isSysHandler)
{
	return AEInstallEventHandler(theAEEventClass,
		theAEEventID, p, handlerRefcon, isSysHandler);
}

LOCALFUNC tMyErr InstallAppleEventHandlers(void)
{
	tMyErr err;

	if (! HaveAppleEvtMgrAvail()) {
		err = noErr;
	} else {
		if (noErr == (err = AESetInteractionAllowed(
			kAEInteractWithLocal)))
		if (noErr == (err = MyInstallEventHandler(
			kCoreEventClass, kAEOpenApplication,
			DoOpenEvent, 0, false)))
#if WantRealInputFile
		if (noErr == (err = MyInstallEventHandler(
			kCoreEventClass, kAEOpenDocuments,
			OpenOrPrintFiles, openOnly, false)))
		if (noErr == (err = MyInstallEventHandler(
			kCoreEventClass, kAEPrintDocuments,
			OpenOrPrintFiles, openPrint, false)))
#endif
		if (noErr == (err = MyInstallEventHandler(
			kCoreEventClass, kAEQuitApplication,
			DoQuitEvent, 0, false)))
		{
		}
	}

	return err;
}


#if WantRealInputFile
static pascal OSErr GlobalTrackingHandler(short message,
	WindowRef pWindow, void *handlerRefCon, DragRef theDragRef)
{
#pragma unused(pWindow, handlerRefCon)
	RgnHandle hilightRgn;
	Rect Bounds;

	switch(message) {
		case kDragTrackingEnterWindow:
			hilightRgn = NewRgn();
			if (hilightRgn != NULL) {
				My_GetWindowBounds(MyMainWind, &Bounds);
				RectRgn(hilightRgn, &Bounds);
				ShowDragHilite(theDragRef, hilightRgn, true);
				DisposeRgn(hilightRgn);
			}
			break;
		case kDragTrackingLeaveWindow:
			HideDragHilite(theDragRef);
			break;
	}

	return noErr;

}
#endif

#if WantRealInputFile
static pascal OSErr GlobalReceiveHandler(WindowRef pWindow,
	void *handlerRefCon, DragRef theDragRef)
{
#pragma unused(pWindow, handlerRefCon)
	tMyErr err;
	unsigned short items;
	unsigned short index;
	DragItemRef theItem;
	Size SentSize;
	HFSFlavor r;

	if (noErr == (err = CountDragItems(theDragRef, &items))) {
		for (index = 1; index <= items; index++) {
			if (noErr == (err = GetDragItemReferenceNumber(theDragRef,
				index, &theItem)))
			if (noErr == (err = GetFlavorDataSize(theDragRef,
				theItem, kDragFlavorTypeHFS, &SentSize)))
			/*
				SentSize may be only big enough to hold the actual
				file name. So use '<=' instead of '=='.
			*/
			if (SentSize <= sizeof(HFSFlavor))
			if (noErr == (err = GetFlavorData(theDragRef, theItem,
				kDragFlavorTypeHFS, (Ptr)&r, &SentSize, 0)))
			if (noErr == (err = ProcessInputFileFSSpecOrAlias_v2(
				&r.fileSpec)))
			{
				/* ok */
			}
			if (noErr != err) {
				goto label_fail;
			}
		}

		{
			ProcessSerialNumber currentProcess = {0, kCurrentProcess};

			err = SetFrontProcess(&currentProcess);
		}
label_fail:
		;
	}

	vCheckSysErr(err);

	return noErr;
}
#endif

#if WantRealInputFile
LOCALVAR blnr gHaveDragMgr = falseblnr;
#endif

#if WantRealInputFile
LOCALPROC PrepareForDragging(void)
{
	if (HaveDragMgrAvail()) {
		if (noErr == InstallTrackingHandler(
			GlobalTrackingHandler, MyMainWind, nil))
		{
			if (noErr == InstallReceiveHandler(
				GlobalReceiveHandler, MyMainWind, nil))
			{
				gHaveDragMgr = trueblnr;
				return;
				/*
					RemoveReceiveHandler(
						GlobalReceiveHandler, MyMainWind);
				*/
			}
			RemoveTrackingHandler(GlobalTrackingHandler, MyMainWind);
		}
	}
}
#endif

#if WantRealInputFile
LOCALPROC UnPrepareForDragging(void)
{
	if (gHaveDragMgr) {
		RemoveReceiveHandler(GlobalReceiveHandler, MyMainWind);
		RemoveTrackingHandler(GlobalTrackingHandler, MyMainWind);
	}
}
#endif


#define kControlInvisible 0
#define My_SetControlVisibilityFalse(inControl) \
	(*inControl)->contrlVis = kControlInvisible

#define kControlVisible 0xFF
#define My_SetControlVisibilityTrue(inControl) \
	(*inControl)->contrlVis = kControlVisible

GLOBALPROC MyGetGrayRgnBounds(Rect *r)
{
	GetRegionBounds(GetGrayRgn(), r);
}

LOCALVAR blnr GoRequested = falseblnr;
LOCALVAR Handle ParseHandle;
LOCALVAR uimr ParseRangeStart;
LOCALVAR uimr ParseRangeStop;

LOCALVAR EventRecord curevent;

#define kMyMenuBar 128

#define kAppleMenu 128
#define kFileMenu 129
#define kEditMenu 130

#define kAppleAboutItem 1

#define kFileGoItem 1
#define kFileAbortItem 2
#define kFileImportItem 4
#define kFileExportItem 5
#define kFileQuitItem 7

#define kEditUndoItem 1
#define kEditCutItem 3
#define kEditCopyItem 4
#define kEditPasteItem 5
#define kEditClearItem 6
#define kEditSelectAllItem 7
#define kEditHostCutItem 9
#define kEditHostCopyItem 10
#define kEditHostPasteItem 11

#define kTextMargin 2


#define ScrollWidth 14
#define BorderWidth 1

#define kMinDocDim 64

#define kSysEnvironsVersion 1

#define kSuspendResumeMessage 1
	/* high byte of suspend/resume event message */
#define kMouseMovedMessage 0xFA
	/* high byte of mouse-moved event message */

#define kExtremeNeg -32768
#define kExtremePos (32767 - 1)
	/* required to address an old region bug */



LOCALVAR Boolean gBackgroundFlag = falseblnr;


#define TopLeft(aRect)  (* (Point *) &(aRect).top)
#define BotRight(aRect) (* (Point *) &(aRect).bottom)



LOCALPROC GetWindowPortGlobalOffset(WindowRef w, Point *p)
{
	p->v = - w->portBits.bounds.top;
	p->h = - w->portBits.bounds.left;
}

LOCALPROC GetLocalUpdateRgn(RgnHandle localRgn)
{
	Point p;

	CopyRgn(((WindowPeek) MyMainWind)->updateRgn, localRgn);
		/* save old update region */
	GetWindowPortGlobalOffset(MyMainWind, &p);
	OffsetRgn(localRgn, - p.h, - p.v);
}

LOCALFUNC unsigned long GetSleep(void)
{
	long sleep;
	WindowRef window;
	TEHandle te;

	sleep = LONG_MAX; /* default value for sleep */
	if (! gBackgroundFlag) {
		window = FrontWindow(); /* and the front window is ours... */
		if (window == MyMainWind) {
			te = MyDocTE;
				/* and the selection is an insertion point... */
			if ((*te)->selStart == (*te)->selEnd) {
				sleep = GetCaretTime();
					/* blink time for the insertion point */
			}
		}
	}
	return sleep;
}


LOCALPROC MyDrawGrowIcon(WindowRef w, Rect *r)
{
	/* Assuming w is the current port */
	RgnHandle SaveRgn;
	RgnHandle growr;

	SaveRgn = NewRgn();
	if (NULL != SaveRgn) {
		GetClip(SaveRgn);
		growr = NewRgn();
		if (NULL != growr) {
			RectRgn(growr, r);
			SectRgn(SaveRgn, growr, growr);
			SetClip(growr);
			DrawGrowIcon(w);
			DisposeRgn(growr);
		}
		SetClip(SaveRgn);
		DisposeRgn(SaveRgn);
	}
}

LOCALPROC DrawTheLines(void)
{
	Rect r;

	My_GetWindowBounds(MyMainWind, &r);
	r.left = r.right - (ScrollWidth + BorderWidth);
	r.right = r.left + 1;
	PaintRect(&r);

	My_GetWindowBounds(MyMainWind, &r);
	r.top = r.bottom - (ScrollWidth + BorderWidth);
	r.bottom = r.top + 1;
	PaintRect(&r);
}

struct MyWindRectsR {
	Rect rW;
	Rect rContent;
	Rect rVScroll;
	Rect rProgBar;
	Rect rGrow;
};
typedef struct MyWindRectsR MyWindRectsR;

GLOBALPROC GetMyWindRects(MyWindRectsR *r)
{
	My_GetWindowBounds(MyMainWind, &r->rW);

	SetRect(&r->rVScroll,
		r->rW.right - ScrollWidth - BorderWidth,
		r->rW.top - BorderWidth,
		r->rW.right + BorderWidth,
		r->rW.bottom - ScrollWidth);
	SetRect(&r->rProgBar,
		r->rW.left,
		r->rW.bottom - ScrollWidth,
		r->rW.right - ScrollWidth - BorderWidth,
		r->rW.bottom);

	r->rContent = r->rW;
	r->rContent.bottom -= (ScrollWidth + BorderWidth);
	r->rContent.right -= (ScrollWidth + BorderWidth);
	InsetRect(&r->rContent, kTextMargin, kTextMargin);
		/* adjust for margin */

	SetRect(&r->rGrow,
		r->rW.right - ScrollWidth, r->rW.bottom - ScrollWidth,
		r->rW.right, r->rW.bottom);
}

LOCALPROC DrawWindow(void)
{
	MyWindRectsR r;

	GetMyWindRects(&r);

	DrawTheLines();
	DrawControls(MyMainWind);
	MyDrawGrowIcon(MyMainWind, &r.rGrow);
	TEUpdate(&r.rContent, MyDocTE);
	ProgressBar_Draw();
}

LOCALFUNC Boolean IsDAWindow(WindowRef window)
{
	if (window == nil) {
		return false;
	} else { /* DA windows have negative windowKinds */
		return GetWindowKind(window) < 0;
	}
}

LOCALPROC MyDeskAccClose(WindowRef w)
{
	SelectWindow(w);
	SetPortWindowPort(w);
	CloseDeskAcc(GetWindowKind(w));
}

LOCALPROC CloseAWindow(WindowRef w)
/*
	Close a window which isn't ours.
*/
{
	if (GetWindowKind(w) < 0) {
		MyDeskAccClose(w);
	} else {
		DisposeWindow(w); /* get rid of it, whatever it is */
	}
}

LOCALPROC CloseAllExtraWindows(void)
/*
	assuming our window is gone, get
	rid of the rest of the windows,
	such as DAs.
*/
{
	while (FrontWindow() != NULL) {
		CloseAWindow(FrontWindow());
	}
}

LOCALPROC MyMainWindowUnInit(void)
{
	if (MyMainWind != NULL) {
		if (MyDocTE != nil) {
			TEDispose(MyDocTE);
				/*
					dispose the TEHandle if we got
					far enough to make one
				*/
		}
		DisposeWindow(MyMainWind);
	}
}

GLOBALPROC AppReportAnySavedError(void)
{
	if ((SavedSysErr != noErr) || AbortRequested) {
		if (AbortRequested) {
			MyAlertFromCStr("Aborted");
		} else {
			ReportUnhandledErr(SavedSysErr);
		}
		SavedSysErr = noErr;
		AbortRequested = falseblnr;
#if AutoQuitIfStartUpFile
		SawStartUpFile = falseblnr;
#endif
#if WantRealInputFile
		ClearInputA();
#endif
	}
}

LOCALPROC AdjustCursor(Point mouse, RgnHandle region)
{
	WindowRef window;
	RgnHandle arrowRgn;
	RgnHandle iBeamRgn;
	RgnHandle GlobalVisRgn;
	Rect iBeamRect;
	Point p;

	window = FrontWindow();
		/* we only adjust the cursor when we are in front */
	if ((! gBackgroundFlag) && (! IsDAWindow(window))) {
		/* calculate regions for different cursor shapes */
		arrowRgn = NewRgn();
		iBeamRgn = NewRgn();
		GlobalVisRgn = NewRgn();

		/* start arrowRgn wide open */
		SetRectRgn(arrowRgn,
			kExtremeNeg, kExtremeNeg, kExtremePos, kExtremePos);

		/* calculate iBeamRgn */
		if (window == MyMainWind) {
			iBeamRect = (*MyDocTE)->viewRect;
			SetPortWindowPort(window);
				/* make a global version of the viewRect */
			LocalToGlobal(&TopLeft(iBeamRect));
			LocalToGlobal(&BotRight(iBeamRect));
			RectRgn(iBeamRgn, &iBeamRect);
			GetWindowPortGlobalOffset(window, &p);
			CopyRgn(window->visRgn, GlobalVisRgn);
			OffsetRgn(GlobalVisRgn, p.h, p.v);
			SectRgn(iBeamRgn, GlobalVisRgn, iBeamRgn);
		}

		/* subtract other regions from arrowRgn */
		DiffRgn(arrowRgn, iBeamRgn, arrowRgn);

		/* change the cursor and the region parameter */
		if (PtInRgn(mouse, iBeamRgn)) {
			SetCursor(*GetCursor(iBeamCursor));
			CopyRgn(iBeamRgn, region);
		} else {
			ResumeSystemCursor();
			CopyRgn(arrowRgn, region);
		}

		DisposeRgn(GlobalVisRgn);
		DisposeRgn(arrowRgn);
		DisposeRgn(iBeamRgn);
	}
}

LOCALPROC DoActivate(Boolean becomingActive)
{
	MyWindRectsR r;

	GetMyWindRects(&r);

	if (becomingActive) {
#if 1
		TEActivate(MyDocTE);
#else
		/*
			This doesn't seem to work on Mac 128K with old system.

			since we don't want TEActivate to draw a selection in
			an area where we're going to erase and redraw, we'll clip
			out the update region before calling it.
		*/
		RgnHandle tempRgn;
		RgnHandle clipRgn;

		tempRgn = NewRgn();
		if (NULL != tempRgn) {
			clipRgn = NewRgn();
			if (NULL != clipRgn) {
				GetLocalUpdateRgn(tempRgn);
					/* get localized update region */
				GetClip(clipRgn);
				DiffRgn(clipRgn, tempRgn, tempRgn);
					/* subtract updateRgn from clipRgn */
				SetClip(tempRgn);

				TEActivate(MyDocTE);

				SetClip(clipRgn); /* restore the full-blown clipRgn */
				DisposeRgn(clipRgn);
			}
			DisposeRgn(tempRgn);
		}
#endif

		/* the controls must be redrawn on activation: */
		My_SetControlVisibilityTrue(MyDocVScroll);
		InvalRect(&r.rVScroll);

		InvalRect(&r.rGrow);
	} else {
		TEDeactivate(MyDocTE);
		/* the controls must be hidden on deactivation: */
		HideControl(MyDocVScroll);
		/* the growbox should be changed immediately on deactivation: */
		MyDrawGrowIcon(MyMainWind, &r.rGrow);
	}
}

LOCALVAR short MyResFile;
LOCALVAR blnr MyWindActive = falseblnr;

LOCALPROC FixUpMyState(void)
/*
	Repair state after system has
	done unknown things
*/
{
	blnr b = gBackgroundFlag ? falseblnr
		: (MyMainWind == FrontWindow());

	if (MyResFile != LMGetCurMap()) {
		UseResFile(MyResFile);
	}
	if (GetWindowPort(MyMainWind) != qd.thePort) {
		SetPortWindowPort(MyMainWind);
	}
	if (b != MyWindActive) {
		MyWindActive = b;
		DoActivate(b);
	}
}

LOCALPROC DoIdle(void)
{
	FixUpMyState();

	if (MyWindActive) {
		TEIdle(MyDocTE);
	}
}

LOCALVAR blnr MyDocHaveHTC = falseblnr;


LOCALPROC EnableDisableMenuItem(
	MenuRef   theMenu,
	short     item,
	blnr v)
{
	if (v) {
		EnableMenuItem(theMenu, item);
	} else {
		DisableMenuItem(theMenu, item);
	}
}

LOCALPROC DoUpdateMenus(void)
{
	MenuRef menu;
	TEHandle te;
	Boolean cutCopyClear;
	Boolean paste;
	long offset;

	menu = GetMenuHandle(kFileMenu);
	EnableDisableMenuItem(menu, kFileGoItem,
		MyWindActive && WaitingForInput);
	EnableDisableMenuItem(menu, kFileImportItem,
		MyWindActive && MyDocEditable);

	if (MyWindActive) {
		te = MyDocTE;
		cutCopyClear = ((*te)->selStart < (*te)->selEnd);
		/*
			Cut, Copy, and Clear is enabled for app.
			windows with selections
		*/

		paste = (GetScrap(nil, 'TEXT', &offset) > 0);
			/*
				if there's any text in the clipboard,
				paste is enabled
			*/
	}

	menu = GetMenuHandle(kEditMenu);
	EnableDisableMenuItem(menu, kEditUndoItem,
		(! MyWindActive) || MyDocEditable);
	EnableDisableMenuItem(menu, kEditCopyItem,
		(! MyWindActive) || cutCopyClear);
	if ((! MyWindActive) || (cutCopyClear && MyDocEditable)) {
		EnableMenuItem(menu, kEditCutItem);
		EnableMenuItem(menu, kEditClearItem);
	} else {
		DisableMenuItem(menu, kEditCutItem);
		DisableMenuItem(menu, kEditClearItem);
	}
	EnableDisableMenuItem(menu, kEditPasteItem,
		(! MyWindActive) || (paste && MyDocEditable));

	EnableMenuItem(menu, kEditSelectAllItem);

	EnableDisableMenuItem(menu, kEditHostCutItem,
		MyWindActive && cutCopyClear && MyDocHaveHTC && MyDocEditable);
	EnableDisableMenuItem(menu, kEditHostCopyItem,
		MyWindActive && cutCopyClear && MyDocHaveHTC);
	EnableDisableMenuItem(menu, kEditHostPasteItem,
		MyWindActive && MyDocHaveHTC && MyDocEditable);
}

LOCALPROC DoQuit(void)
{
	/*
		Quit requested.
		For this program can always quit immediatedly.
	*/
	ProgramDone = trueblnr;
}

#ifndef NewTextCreator
#define NewTextCreator 'R*ch'
#endif

LOCALPROC DoSaveNewFile(void)
{
	tMyErr err;
	MyPStr s;
	MyDir_R d;
	short refNum;
	Handle h = (Handle) TEGetText(MyDocTE);
	uimr L = (*MyDocTE)->teLength;

	if (noErr == (err = ProgressBar_SetStage_v2(
		"Select destination\311", 0)))
	{
		UpdateProgressBar();
		err = CreateOpenNewFile_v2((StringPtr)"\pOutput File",
			(StringPtr)"\p",
			NewTextCreator,
			'TEXT' /* type */,
			&d, s, &refNum);
		if (noErr == err) {
			if (noErr == (err = ProgressBar_SetStage_v2(
				"Writing\311", 0)))
			if (noErr == (err = MyOpenFileSetEOF_v2(refNum, L)))
			{
				HLock(h);
				err = MyWriteBytes_v2(refNum, *h, L);
				HUnlock(h);
			}
			err = MyCloseNewFile_v3(refNum, &d, s, err);
		}
	}

	err = ErrCombine(err, ProgressBar_SetStage_v2((noErr == err)
		? "File written."
			: "Failed to write file.",
		0));
	UpdateProgressBar();
	ReportUnhandledErr(err);
}

LOCALPROC DecodeSysMenus(long theMenuItem)
{
	short menuID;
	short menuItem;
	short daRefNum;
	Str255 daName;

	menuID = HiWord(theMenuItem); /* use macros for efficiency to... */
	menuItem = LoWord(theMenuItem);
		/* get menu item number and menu number */
	switch (menuID) {
		case kAppleMenu:
			switch (menuItem) {
				case kAppleAboutItem: /* bring up alert for About */
					ShowAboutMessage();
					break;
				default:
					/* all non-About items in this menu are DAs et al */
					/* type Str255 is an array in MPW 3 */
					GetMenuItemText(GetMenuHandle(kAppleMenu),
						menuItem, daName);
					daRefNum = OpenDeskAcc(daName);
					FixUpMyState();
					break;
			}
			break;
		case kFileMenu:
			switch (menuItem) {
				case kFileGoItem:
					if (WaitingForInput && ! GoRequested) {
						GoRequested = trueblnr;
					}
					break;
				case kFileAbortItem:
					AbortRequested = trueblnr;
					break;
				case kFileImportItem:
#if WantRealInputFile
					DoGetOldFile();
#endif
					break;
				case kFileExportItem:
					DoSaveNewFile();
					break;
				case kFileQuitItem:
					DoQuit();
					break;
			}
			break;
		case kEditMenu:
			/* call SystemEdit for DA editing & MultiFinder */
			if (! SystemEdit(menuItem - 1)) {
				switch (menuItem) {
					case kEditUndoItem:
						MyDocEditUndo();
						break;
					case kEditCutItem:
						(void) MyDocEditCopy();
						(void) MyDocEditClear();
						break;
					case kEditCopyItem:
						(void) MyDocEditCopy();
						MyDocScrollToSel();
						break;
					case kEditPasteItem:
						(void) MyDocEditPaste();
						break;
					case kEditClearItem:
						(void) MyDocEditClear();
						break;
					case kEditSelectAllItem:
						MyDocSelectAll();
						break;
					case kEditHostCutItem:
						if (MyDocHaveHTC) {
							(void) MyDocEditHostCopy();
							(void) MyDocEditClear();
						}
						break;
					case kEditHostCopyItem:
						if (MyDocHaveHTC) {
							(void) MyDocEditHostCopy();
							MyDocScrollToSel();
						}
						break;
					case kEditHostPasteItem:
						if (MyDocHaveHTC) {
							(void) MyDocEditHostPaste();
						}
						break;
				}
			}
			break;
	}
	HiliteMenu(0);
		/* unhighlight what MenuSelect (or MenuKey) hilited */
}

LOCALPROC DoContentClick(void)
{
	Point mouse;
	ControlRef control;
	ControlPartCode part;
	short value;
	Boolean shiftDown;
	MyWindRectsR r;

	GetMyWindRects(&r);

	mouse = curevent.where; /* get the click position */
	GlobalToLocal(&mouse);
		/*
			see if we are in the viewRect.
			if so, we won't check the controls
		*/
	if (PtInRect(mouse, &r.rContent)) {
		/* see if we need to extend the selection */
		shiftDown = (curevent.modifiers & shiftKey) != 0;
			/* extend if Shift is down */
		TEClick(mouse, shiftDown, MyDocTE);
	} else if (PtInRect(mouse, &r.rProgBar)) {
		if (WaitingForInput && ! GoRequested) {
			ProgressBar_InvertAll();
			GoRequested = trueblnr;
			while (Button()) {
				GetMouse(&mouse);
				if (GoRequested != PtInRect(mouse, &r.rProgBar)) {
					GoRequested = ! GoRequested;
					ProgressBar_InvertAll();
				}
			}
			if (GoRequested) {
				ProgressBar_InvertAll();
			}
		}
	} else {
		part = FindControl(mouse, MyMainWind, &control);
		switch (part) {
			case 0: /* do nothing for viewRect case */
				break;
			case kControlIndicatorPart:
				value = GetControlValue(control);
				part = TrackControl(control, mouse, NULL);
				if (part != 0) {
					value -= GetControlValue(control);
					/*
						value now has CHANGE in value;
						if value changed, scroll
					*/
					if (value != 0) {
						TEScroll(0, value, MyDocTE);
					}
				}
				break;
			default: /* they clicked in an arrow, so track & scroll */
				(void) TrackControl(control, mouse, MyDocTrackAction);
				break;
		}
	}
}

LOCALPROC ResizeMyWindow(void)
{
	MyWindRectsR r;

	GetMyWindRects(&r);
	/*
		First, turn visibility of scrollbars off so we won't get
		unwanted redrawing
	*/
	My_SetControlVisibilityFalse(MyDocVScroll); /* turn them off */

	MoveControl(MyDocVScroll, r.rVScroll.left, r.rVScroll.top);
	SizeControl(MyDocVScroll, r.rVScroll.right - r.rVScroll.left,
		r.rVScroll.bottom - r.rVScroll.top);

	(*MyDocTE)->viewRect = r.rContent;
	(*MyDocTE)->destRect.right = r.rContent.right;
	TECalText(MyDocTE);

	MyDocCheckScrollPosMax();

	ProgressBar_Move(&r.rProgBar);

	/*
		Now, restore visibility in case we never had to ShowControl
		during adjustment
	*/
	My_SetControlVisibilityTrue(MyDocVScroll); /* turn them on */

	InvalRect(&r.rW);
}

LOCALPROC DoGrowWindow(void)
{
	long growResult;
	Rect tempRect;
	RgnHandle tempRgn;

	MyGetGrayRgnBounds(&tempRect);
	tempRect.bottom -= tempRect.top;
	tempRect.right -= tempRect.left;
	tempRect.top = kMinDocDim;
	tempRect.left = kMinDocDim;
	growResult = GrowWindow(MyMainWind, curevent.where, &tempRect);
	/* see if it really changed size */
	if (growResult != 0) {
		tempRect = (*MyDocTE)->viewRect; /* save old text box */
		tempRgn = NewRgn();
		if (NULL != tempRgn) {
			GetLocalUpdateRgn(tempRgn);
				/* get localized update region */
			SizeWindow(MyMainWind,
				LoWord(growResult), HiWord(growResult), true);
			ResizeMyWindow();
			/*
				calculate & validate the region that hasn't changed so
				it won't get redrawn
			*/
			SectRect(&tempRect, &(*MyDocTE)->viewRect, &tempRect);
			InvalRgn(tempRgn); /* put back any prior update */
			DisposeRgn(tempRgn);
		}
	}
}

LOCALPROC DoZoomWindow(short which_part)
{
	Rect r;

	My_GetWindowBounds(MyMainWind, &r);
	EraseRect(&r);
	ZoomWindow(MyMainWind, which_part,
		MyMainWind == FrontWindow());
	ResizeMyWindow();
}

LOCALVAR WindowRef MacEventWind;

LOCALPROC inMenuBarAction(void)
{
	DoUpdateMenus();
	DecodeSysMenus(MenuSelect(curevent.where));
}

LOCALPROC inSysWindowAction(void)
{
	SystemClick(&curevent, MacEventWind);
	FixUpMyState();
}

LOCALPROC inContentAction(void)
{
	if (MacEventWind == MyMainWind) {
		if (MacEventWind != FrontWindow()) {
			SelectWindow(MacEventWind);
			/* ProcessMacEvent(); */
				/* use this line for "do first click" */
		} else {
			SetPortWindowPort(MyMainWind);
			DoContentClick();
		}
	}
}

LOCALPROC inDragAction(void)
{
	Rect r;

	MyGetGrayRgnBounds(&r);
	InsetRect(&r, 4, 4);

	DragWindow(MacEventWind, curevent.where, &r);
	FixUpMyState();
}

LOCALPROC inGoAwayAction(void)
{
	if (TrackGoAway(MacEventWind, curevent.where)) {
		if (MacEventWind == MyMainWind) {
			DoQuit();
		} else {
			CloseAWindow(MacEventWind);
			FixUpMyState();
		}
	}
}

LOCALPROC inGrowAction(void)
{
	if (MacEventWind == MyMainWind) {
		DoGrowWindow();
	}
}

LOCALPROC inZoomAction(short which_part)
{
	if (MacEventWind == MyMainWind) {
		if (TrackBox(MyMainWind, curevent.where, which_part)) {
			DoZoomWindow(which_part);
		}
	}
}

LOCALPROC ProcessMouseDown(void)
{
	short which_part;

	which_part = FindWindow(curevent.where, &MacEventWind);
	switch (which_part) {
		case inDesk:
			/* nothing */
			break;
		case inMenuBar:
			inMenuBarAction();
			break;
		case inSysWindow:
			inSysWindowAction();
			break;
		case inContent:
			inContentAction();
			break;
		case inDrag:
			inDragAction();
			break;
		case inGoAway:
			inGoAwayAction();
			break;
		case inGrow:
			inGrowAction();
			break;
		case inZoomIn:
		case inZoomOut:
			inZoomAction(which_part);
			break;
	}
}

LOCALPROC ProcessKeyDown(void)
{
	/* check for menukey equivalents */
	ui3r key = curevent.message & charCodeMask;

	if (curevent.modifiers & cmdKey) { /* Command key down */
		if (curevent.what == keyDown) {
			DoUpdateMenus();
				/* enable/disable/check menu items properly */

			if ((0x8D == key) || (0x82 == key)) {
				/* not working as desired before System 7 */
				DecodeSysMenus(((ui5r)kEditMenu << 16)
					| kEditHostCopyItem);
			} else {
				DecodeSysMenus(MenuKey(key));
			}
		}
	} else {
		WindowRef window = FrontWindow();

		if (window == MyMainWind) {
			MyDocKeyDown(key);
		}
	}
}

LOCALPROC ProcessUpdateEvt(void)
{
	GrafPtr saveport;
	Rect r;

	MacEventWind = (WindowRef)curevent.message;
	GetPort(&saveport);
	SetPortWindowPort(MacEventWind);

	BeginUpdate(MacEventWind);
	My_GetWindowBounds(MacEventWind, &r);
	EraseRect(&r);
	if (MacEventWind == MyMainWind) {
		DrawWindow();
	} else {
		/* Unexpected update for unknown window */
	}
	EndUpdate(MacEventWind);

	SetPort(saveport);
}

LOCALPROC ProcessActivateEvt(void)
{
#if 0
	WindowRef window = (WindowRef) curevent.message;
	if (window == MyMainWind) {
		DoActivate((curevent.modifiers & activeFlag) != 0);
	}
#endif
	FixUpMyState();
}

LOCALPROC ProcessDiskEvt(void)
{
	Point where;

	if (HiWord(curevent.message) != noErr) {
		SetPt(&where, 70, 50);
		ResumeSystemCursor();
		(void) DIBadMount(where, curevent.message);
	}
	FixUpMyState();
}

LOCALPROC ProcessHighLevelEvt(void)
{
	AEEventClass thisClass;

	thisClass = (AEEventClass)curevent.message;
	if (kCoreEventClass == thisClass) {
		OSErr err = AEProcessAppleEvent(&curevent);
		if (errAEEventNotHandled == err) {
			/* ignore */
		} else {
			ReportUnhandledErr(err);
		}
	} else {
		ReportUnhandledErr(errAENotAppleEvent);
	}
	FixUpMyState();
}

LOCALPROC ProcessMacEvent(void)
{

	switch (curevent.what) {
		case nullEvent:
			DoIdle();
			break;
		case mouseDown:
			ProcessMouseDown();
			break;
		case keyDown:
		case autoKey:
			ProcessKeyDown();
			break;
		case activateEvt:
			ProcessActivateEvt();
			break;
		case updateEvt:
			ProcessUpdateEvt();
			break;
		case diskEvt:
			ProcessDiskEvt();
			break;
		case kHighLevelEvent:
			ProcessHighLevelEvt();
			break;
		case osEvt:
			switch (curevent.message >> 24) { /* high byte of message */
				case kMouseMovedMessage:
					DoIdle(); /* mouse-moved is also an idle event */
					break;
				case kSuspendResumeMessage:
					/* suspend/resume is also an activate/deactivate */
					gBackgroundFlag = ! TestBit(curevent.message, 0);
					FixUpMyState();
					break;
			}
			break;
	}
}

LOCALFUNC tMyErr SaferNewWindow(Ptr wStorage, const Rect *boundsRect,
	const Str255 title, Boolean visible, short theProc,
	WindowPtr behind, Boolean goAwayFlag, long refCon, WindowPtr *w)
{
	tMyErr err;
	WindowPtr w0;

	if (noErr != (err = MyMemoryCheckSpare())) {
		/* fail */
	} else
	if (NULL == (w0 = NewWindow(wStorage, boundsRect, title,
		visible, theProc, behind, goAwayFlag, refCon)))
	{
		err = kMyErrSysUnknown;
	} else
	if (noErr != (err = MyMemoryCheckSpare())) {
		if (NULL == wStorage) {
			DisposeWindow(w0);
		} else {
			CloseWindow(w0);
		}
	} else
	{
		*w = w0;
	}

	return err;
}

LOCALVAR ui4b AsmClikLoop[] = {
	0x6008,         /*     BRA.S      @1 */
	0x0000, 0x0000, /* @2: DC.L    0 */
	0x0000, 0x0000, /* @3: DC.L    0 */
	0x207A, 0xFFF6, /* @1: MOVEA.L    @2,A0 */
	0x4E90,         /*     JSR        (A0) */
	0x48E7, 0x6040, /*     MOVEM.L    D1/D2/A1,-(A7) */
	0x207A, 0xFFF0, /*     MOVEA.L    @3,A0 */
	0x4E90,         /*     JSR        (A0) */
	0x4CDF, 0x0206, /*     MOVEM.L    (A7)+,D1/D2/A1 */
	0x7001,         /*     MOVEQ      #$01,D0 */
	0x4E75,         /*     RTS */
};

LOCALFUNC tMyErr MyMainWindowInit(void)
{
	tMyErr err;
	Rect rBounds;
	Rect destRect;
	MyWindRectsR r;
	ui5b *p;

	SetRect(&rBounds, 60, 64, 460, 314);
	if (noErr == (err = SaferNewWindow(NULL, &rBounds,
		LMGetCurApName() /* "\puntitled" */, false,
#if Support64kROM
		Have64kROM() ? documentProc :
#endif
			zoomDocProc,
		(WindowRef) -1, true, 0, &MyMainWind)))
	{
		SetPortWindowPort(MyMainWind);
		GetMyWindRects(&r);
		destRect = r.rContent;
		if (noErr == (err = SaferTENew(
			&destRect, &r.rContent, &MyDocTE)))
		{
#if 0
#if Support64kROM
			if (! Have64kROM())
#endif
			{
				TEAutoView(true, MyDocTE);
			}
#endif
			p = (ui5b *)&AsmClikLoop[1];
			*p++ = (ui5b)(*MyDocTE)->clickLoop;
			*p = (ui5b)MyDocClikLoop;
			if (NULL == (*MyDocTE)->clickLoop) {
				(*MyDocTE)->clickLoop = (TEClickLoopUPP)&AsmClikLoop[8];
			} else {
				(*MyDocTE)->clickLoop = (TEClickLoopUPP)AsmClikLoop;
			}
		}

		if (noErr == err) { /* good document? - get scrollbars */
			err = SaferNewControl(MyMainWind, &r.rVScroll,
				(ConstStr255Param)"\p", true, 0, 0, 0, scrollBarProc,
				0, &MyDocVScroll);
		}

		if (noErr == (err = ProgressBar_Init_v2(
			MyMainWind, &r.rProgBar)))
		if (noErr == err)
		{
			/* good? - adjust & draw the controls, draw the window */
			MyDocCheckScrollPosMax();
			ShowWindow(MyMainWind);
		}
	}

	if (noErr != err) {
		MyMainWindowUnInit();
	}

	return err;
}

LOCALFUNC tMyErr MyMenusInit(void)
{
	tMyErr err;
	MenuRef theMenu;
	MenuBarHandle menuBar = GetNewMBar(kMyMenuBar);

	if (menuBar == nil) {
		err = kMyErrSysUnknown;
	} else {
		SetMenuBar(menuBar); /* install menus */
		DisposeHandle(menuBar);
		theMenu = GetMenuHandle(kAppleMenu);
		if (theMenu != NULL) {
			AppendResMenu(theMenu, 'DRVR');
				/* add DA names to Apple menu */
		}

		DrawMenuBar();

		err = noErr;
	}

	return err;
}


LOCALFUNC tMyErr MyFileRead2Hand(MyDir_R *d, StringPtr s, MyHandle *r)
{
	tMyErr err;
	short refNum;
	uimr n;
	MyHandle h = nullpr;

	if (noErr == (err = MyOpenOldFileRead_v2(d, s, &refNum))) {
		if (noErr == (err = MyOpenFileGetEOF_v2(refNum, &n))) {
			if (noErr == (err = MyHandleNew_v2(n, &h)))  {
				HLock(h);
				err = MyReadBytes_v2(refNum, *h, n);
				HUnlock(h);
			}
		}
		err = ErrCombine(err, MyCloseFile_v2(refNum));
	}

	if (noErr == err) {
		*r = h;
	} else {
		if (nullpr != h) {
			(void) MyHandleDispose_v2(h);
		}
	}

	return err;
}

LOCALPROC MyDoNextEvents(blnr busy)
{
	RgnHandle cursorRgn;
	Boolean gotEvent;

	cursorRgn = NewRgn();
		/* we'll pass WNE an empty region the 1st time thru */
	do {
		if (HaveWaitNextEventAvail()) {
			gotEvent = WaitNextEvent(everyEvent, &curevent,
				busy ? 0 : GetSleep(), cursorRgn);
		} else {
			SystemTask();
			gotEvent = GetNextEvent(everyEvent, &curevent);
		}
		if (gotEvent) {
			/*
				make sure we have the right cursor before
				handling the event
			*/
			AdjustCursor(curevent.where, cursorRgn);
			ProcessMacEvent();
		} else {
			DoIdle(); /* perform idle tasks when it's not our event */
		}
		/* change the cursor (and region) if necessary */
		AdjustCursor(curevent.where, cursorRgn);
	} while (EventAvail(everyEvent, &curevent) && (! AbortRequested));
	DisposeRgn(cursorRgn);
}

LOCALVAR long LastCheckTick;
LOCALVAR long LastBusyTogTick;
LOCALVAR long LastCheckEventTick;

LOCALFUNC tMyErr WaitForInput(void)
{
	tMyErr err;
	input_r InputCur;

	WaitingForInput = trueblnr;
	ProgressBar_WantBusyMark = falseblnr;

	UpdateProgressBar();

	do {
		AppReportAnySavedError();
		if (MyDocEditable && PopFromInputA(&InputCur)) {
			Str255 s;
			Handle h;

			(void) MyHandleToPStr_v2(InputCur.Name, s);
			if (kMyErr_noErr == (err =
				MyFileRead2Hand(&InputCur.d, s, &h)))
			{
				MyDocReplaceEntireWithHand(h);
				GoRequested = trueblnr;

				DisposeHandle(h);
			}

			DisposeInputR(&InputCur);

			ReportUnhandledErr(err);
		} else
		{
			MyDoNextEvents(falseblnr);
		}
	} while ((! ProgramDone) && (! GoRequested) && (! AbortRequested));

	if (! GoRequested) {
		if (AbortRequested) {
			AbortRequested = falseblnr;
		}
		err = kMyErrUsrCancel;
	} else
	{
		ParseHandle = (Handle) TEGetText(MyDocTE);
		ParseRangeStart = 0;
		ParseRangeStop = (*MyDocTE)->teLength;

#if AutoQuitIfStartUpFile
		if (SawStartUpFile) {
			ProgramDone = trueblnr;
		}
#endif

		GoRequested = falseblnr;
		err = kMyErr_noErr;
	}

	WaitingForInput = falseblnr;

	LastCheckTick = LMGetTicks();
	LastBusyTogTick = LastCheckTick;
	LastCheckEventTick = LastCheckTick;

	return ErrReportStack(err, "WaitForInput");
}

LOCALFUNC tMyErr GetEntireInputAsHandle_v2(Handle *r)
{
	/* warning : assumes (! The_arg_end) */
	return MyHandleNewFromHandPart(r, ParseHandle, ParseRangeStart,
		ParseRangeStop - ParseRangeStart);
}

LOCALFUNC tMyErr WaitForInputText(char *prompt, MyHandle *h)
{
	tMyErr err;

	if (noErr == (err =
		ProgressBar_SetStage_v2(prompt, 0)))
	{
		MyDocEditable = trueblnr;
		err = WaitForInput();
		MyDocEditable = falseblnr;

		if (noErr == err) {
			if (noErr == ProgressBar_SetStage_v2(
				"Running, type command-period to abort\311", 0))
			{
				err = GetEntireInputAsHandle_v2(h);
			}
		}
	}

	return ErrReportStack(err, "WaitForInputText");
}

LOCALFUNC tMyErr ShowOutPutText(char *prompt, MyHandle h)
{
	tMyErr err;

	MyDocReplaceEntireWithHand(h);
	MyDocSelectAll();
	MyDocScrollToSel();

	if (noErr == (err =
		ProgressBar_SetStage_v2(prompt, 0)))
	{
		if (noErr == (err = WaitForInput())) {
		}
	}

	return ErrReportStack(err, "ShowOutPutText");
}

LOCALFUNC blnr EventPendingQuickCheck(void)
{
	EvQElPtr p = (EvQElPtr)GetEvQHdr()->qHead;

	while (p != NULL) {
		switch (p->evtQWhat) {
			case keyDown:
			case autoKey:
			case mouseDown:

			case updateEvt:
			case osEvt:
				/*
					these two aren't actually in this queue,
					and so won't be seen here
				*/
				return trueblnr;
				break;
			default:
				break;
		}
		p = (EvQElPtr)p->qLink;
	}
	return falseblnr;
}

LOCALPROC CheckAbort0_v2(void)
{
	blnr ShouldGetEvent;
	long CurrentTick = LMGetTicks();

	LastCheckTick = CurrentTick;

	if ((CurrentTick - LastBusyTogTick) >= 12) {
		ProgressBar_WantBusyMark = ! ProgressBar_WantBusyMark;
		LastBusyTogTick = CurrentTick;
	}

	UpdateProgressBar();

	if ((CurrentTick - LastCheckEventTick)
		>= (gBackgroundFlag ? 6 : 60))
	{
		/*
			while in background, relinquish processor
			ten times a second. while in foreground,
			once a second at minimum
		*/
		ShouldGetEvent = trueblnr;
	} else {
		ShouldGetEvent = EventPendingQuickCheck();
	}

	if (ShouldGetEvent) {
		LastCheckEventTick = CurrentTick;
		MyDoNextEvents(trueblnr);
	}
}

#define TimeForCheckAbort() ((LMGetTicks() - LastCheckTick) >= 6)

GLOBALFUNC tMyErr CheckAbortRequested(void)
{
	if (TimeForCheckAbort()) {
		CheckAbort0_v2();
	}

	return AbortRequested ? kMyErrUsrAbort : noErr;
}

LOCALPROC OneWindAppPreInit(void)
/*
	Inits that aren't allowed to fail,
	and are needed to display an error
	message if the rest of the startup
	process fails.
*/
{
	MyMacHeapInit();

	MyMacToolBoxInit();
}

GLOBALFUNC tMyErr OneWindAppInit_v2(void)
{
	tMyErr err;

#if 0
	/*
		Now this next bit of code may make you toss your cookies,
		but it is necessary to allow the default button of our
		alert be outlined.
	*/
	/*
		No, don't want to lose initial apple events.
	*/

	EventRecord event;
	short count;

	for (count = 1; count <= 3; count++) {
		GetNextEvent(everyEvent, &event);
	}
#endif

	LastCheckTick = LMGetTicks();
	LastBusyTogTick = LastCheckTick;
	LastCheckEventTick = LastCheckTick;

	MyResFile = LMGetCurMap();

#if ! Support64kROM
	if (Have64kROM()) {
		AlertUser(eWrongMachine);
		err = kMyErrReported;
	} else
#endif
	{
		if (noErr == (err = MyMemory_Init_v2()))
#if WantRealInputFile
		if (noErr == (err = InitInputA()))
#endif
		if (noErr == (err = InstallAppleEventHandlers()))
		if (noErr == (err = MyMenusInit()))
		if (noErr == (err = MyMainWindowInit()))
		{
#if WantRealInputFile
			vCheckSysErr(MyAppFilesInit_v2());
			PrepareForDragging();
#endif

			MyDocHaveHTC = (noErr == HaveHTCExtenstion_v2());
		}
	}

	return err;
}

LOCALPROC OneWindAppUnInit(void)
{
	AppReportAnySavedError();
#if WantRealInputFile
	UnPrepareForDragging();
	UnInitInputA();
#endif
	MyMainWindowUnInit();
	CloseAllExtraWindows();
	MyMemory_UnInit();
}

LOCALVAR MyHandle SyntaxErrH = nullpr;
LOCALVAR uimr SyntaxErrOffsetA;
LOCALVAR uimr SyntaxErrOffsetB;

LOCALFUNC tMyErr SetSyntaxErrCStr(char *s,
	uimr offsetA, uimr offsetB)
{
	tMyErr err;

#if DebugCheck
	dbglog_writeln(s);
#endif
	if (noErr == (err = NHandleFromCStr(&SyntaxErrH, s))) {
		SyntaxErrOffsetA = offsetA;
		SyntaxErrOffsetB = offsetB;
		err = kMyErrSyntaxErr;
	}

	return err;
}

LOCALPROC ShowSyntaxError(void)
{
	MyPStr t;

	TESetSelect(SyntaxErrOffsetA, SyntaxErrOffsetB, MyDocTE);
	(void) MyHandleToPStr_v2(SyntaxErrH, t);
	MyAlertFromPStr(t);
	MyDocScrollToSel();
}

LOCALVAR uimr ParseCharIndex;
LOCALVAR MyCharR ParseCharCur;
LOCALVAR blnr ParseCharDone;

LOCALPROC AdvanceParseChar(void)
{
	++ParseCharIndex;
	if (ParseCharIndex < ParseRangeStop) {
		ParseCharCur = *((MyCharR *)*ParseHandle + ParseCharIndex);
	} else {
		ParseCharDone = trueblnr;
	}
}

LOCALVAR uimr The_arg_range_start;
LOCALVAR uimr The_arg_range_size;
LOCALVAR blnr The_arg_end;
LOCALVAR blnr ParseArgsFailed;

LOCALPROC ReportParseFailure(char *s)
{
	if (! ParseArgsFailed) {
		MyAlertFromCStr(s);

		ParseArgsFailed = trueblnr;
	}
}

LOCALFUNC blnr CurCharIsWhiteSpace(void)
{
	blnr v;

	switch (ParseCharCur) {
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			v = trueblnr;
			break;
		default:
			v = falseblnr;
			break;
	}

	return v;
}

LOCALPROC AdvanceTheArg(void)
{
remove_white:
	if (ParseCharDone) {
		The_arg_range_start = ParseRangeStop;
		The_arg_range_size = 0;
		The_arg_end = trueblnr;
	} else if (CurCharIsWhiteSpace()) {
		AdvanceParseChar();
		goto remove_white;
	} else if ('"' == ParseCharCur) {
		AdvanceParseChar();

		The_arg_range_start = ParseCharIndex;

		while ((! ParseCharDone) && ('"' != ParseCharCur)) {
			AdvanceParseChar();
		}
		The_arg_range_size = ParseCharIndex - The_arg_range_start;
		if (! ParseCharDone) {
			AdvanceParseChar();
		} else {
			ReportParseFailure("missing '\"' to end string");
			The_arg_end = trueblnr;
		}
	} else if ('{' == ParseCharCur) {
		The_arg_range_start = ParseCharIndex;
		AdvanceParseChar();

		while ((! ParseCharDone) && ('}' != ParseCharCur)) {
			AdvanceParseChar();
		}
		if (! ParseCharDone) {
			AdvanceParseChar();
			goto remove_white;
		} else {
			The_arg_range_size = ParseCharIndex - The_arg_range_start;
			ReportParseFailure("missing '}' to end comment");
			The_arg_end = trueblnr;
		}
	} else {
		The_arg_range_start = ParseCharIndex;

		do {
			AdvanceParseChar();
		} while ((! ParseCharDone) && ! CurCharIsWhiteSpace());
		The_arg_range_size = ParseCharIndex - The_arg_range_start;
	}
}

LOCALFUNC blnr CurArgIsCStr_v2(char *s)
{
	/* warning : assumes (! The_arg_end) */
	MyCharR *p = ((MyCharR *)*ParseHandle + The_arg_range_start);
	simr i;

	for (i = The_arg_range_size; --i >= 0; ) {
		if (*s++ != *p++) {
			return falseblnr;
		}
	}
	if (0 != *s) {
		return falseblnr;
	}

	return trueblnr;
}

LOCALFUNC blnr CurArgIsCStrAdvance_v2(char *s)
{
	/* warning : assumes (! The_arg_end) */
	if (! CurArgIsCStr_v2(s)) {
		return falseblnr;
	} else {
		AdvanceTheArg();
		return trueblnr;
	}
}

LOCALPROC GetCurArgAsCStr(char *s, uimr MaxN)
{
	/* warning : assumes (! The_arg_end) */
	uimr L = The_arg_range_size;

	if (L > MaxN) {
		L = MaxN;
	}
	MyHandlePartToCStr(ParseHandle, The_arg_range_start,
		L, s);
}

LOCALPROC GetCurArgAsPStr(ps3p s)
{
	/* warning : assumes (! The_arg_end) */
	MyHandlePartToPStr(ParseHandle, The_arg_range_start,
		The_arg_range_size, s);
}

LOCALFUNC tMyErr GetCurArgAsHandle_v2(Handle *r)
{
	/* warning : assumes (! The_arg_end) */
	return MyHandleNewFromHandPart(r,
		ParseHandle, The_arg_range_start, The_arg_range_size);
}

LOCALPROC BeginParseFromTE(void)
{
	ParseCharIndex = ParseRangeStart - 1;
	ParseCharDone = falseblnr;
	AdvanceParseChar();

	ParseArgsFailed = falseblnr;
	The_arg_end = falseblnr;
	AdvanceTheArg();
}

LOCALPROC EndParseFromTE(void)
{
	if (ParseArgsFailed) {
		TESetSelect(The_arg_range_start,
			The_arg_range_start + The_arg_range_size, MyDocTE);
	} else {
		MyDocSelectAll();
	}
}
