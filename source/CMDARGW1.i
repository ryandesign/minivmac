/*
	CMDARGW1.i
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

LOCALVAR blnr AbortRequested = falseblnr;

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

	if (noErr == (err = PStr2Hand_v2(s, &Name))) {
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

		(void) CheckSysErr(err);
	}
}
#endif

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
		err = CombineErr(err, AEDisposeDesc(&docList));
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


#define My_GetWindowBounds(window, r) \
	GetPortBounds(GetWindowPort(window), (r))

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
#define kFileImportItem 3
#define kFileExportItem 4
#define kFileQuitItem 6

#define kEditUndoItem 1
#define kEditCutItem 3
#define kEditCopyItem 4
#define kEditPasteItem 5
#define kEditClearItem 6
#define kEditSelectAllItem 8

#define kTextMargin 2


#define ScrollWidth 14
#define BorderWidth 1

#define kMinDocDim 64

#define kCrChar 13
#define kDelChar 8

#define kButtonScroll 4

#define kMaxTELength 32000

#define kSysEnvironsVersion 1

#define kSuspendResumeMessage 1
	/* high byte of suspend/resume event message */
#define kMouseMovedMessage 0xFA
	/* high byte of mouse-moved event message */

#define kExtremeNeg -32768
#define kExtremePos (32767 - 1)
	/* required to address an old region bug */

#define kTESlop 1024

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


LOCALVAR TEHandle MyDocTE = NULL;
LOCALVAR ControlRef MyDocVScroll = NULL;


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

LOCALPROC CommonAction(ControlRef control, short *amount)
{
	short value;
	short max;

	value = GetControlValue(control); /* get current value */
	max = GetControlMaximum(control); /* and maximum value */
	*amount = value - *amount;
	if (*amount < 0) {
		*amount = 0;
	} else if (*amount > max) {
		*amount = max;
	}
	SetControlValue(control, *amount);
	*amount = value - *amount; /* calculate the real change */
}

static pascal void VActionProc(ControlRef control, ControlPartCode part)
{
	short amount;
	TEPtr te;

	if (part != 0) { /* if it was actually in the control */
		te = *MyDocTE;
		switch (part) {
			case kControlUpButtonPart:
			case kControlDownButtonPart: /* one line */
				amount = 1;
				break;
			case kControlPageUpPart: /* one page */
			case kControlPageDownPart:
				amount = (te->viewRect.bottom - te->viewRect.top)
					/ te->lineHeight;
				break;
		}
		if ((part == kControlDownButtonPart)
			|| (part == kControlPageDownPart))
		{
			amount = - amount; /* reverse direction for a downer */
		}
		CommonAction(control, &amount);
		if (amount != 0) {
			TEScroll(0, amount * te->lineHeight, MyDocTE);
		}
	}
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

LOCALPROC AlertUser(short error)
{
	Str255 message;

	GetIndString(message, kErrStrings, error);
	MyAlertFromPStr(message);
}

GLOBALPROC AppReportAnySavedError(void)
{
	if ((SavedSysErr != noErr) || AbortRequested) {
		if (AbortRequested) {
			MyAlertFromCStr("Aborted!");
		} else if ((kMyErrReported == SavedSysErr)
			|| (kMyErrUsrCancel == SavedSysErr))
		{
			/* nothing more needed */
		} else if (SavedSysErr != noErr) {
			MyAlertFromCStr(GetTextForSavedSysErr_v2(SavedSysErr));
		}
		SavedSysErr = noErr;
		AbortRequested = falseblnr;
		SawStartUpFile = falseblnr;
#if WantRealInputFile
		ClearInputA();
#endif
	}
}

LOCALPROC AdjustTE(void)
{
	TEPtr te;

	te = *MyDocTE;
	TEScroll(
		0,
		(te->viewRect.top - te->destRect.top) -
			(GetControlValue(MyDocVScroll) *
			te->lineHeight),
		MyDocTE);
}

LOCALPROC AdjustV(Boolean canRedraw)
{
	short value;
	short lines;
	short max;
	short oldValue, oldMax;
	TEPtr te;

	oldValue = GetControlValue(MyDocVScroll);
	oldMax = GetControlMaximum(MyDocVScroll);
	te = *MyDocTE; /* point to TERec for convenience */

	lines = te->nLines;
	/*
		since nLines is not right if the last character is a return,
		check for that case
	*/
	if (*(*te->hText + te->teLength - 1) == kCrChar) {
		lines += 1;
	}
	max = lines - ((te->viewRect.bottom - te->viewRect.top) /
		te->lineHeight);

	if (max < 0) {
		max = 0;
	}
	SetControlMaximum(MyDocVScroll, max);

	/*
		Must deref. after SetControlMaximum since, technically, it
		could draw and therefore move memory. This is why we do not
		just do it once at the beginning.
	*/
	te = *MyDocTE;
	value = (te->viewRect.top - te->destRect.top) / te->lineHeight;

	if (value < 0) {
		value = 0;
	} else if (value > max) {
		value = max;
	}

	SetControlValue(MyDocVScroll, value);

	if (canRedraw || (max != oldMax) || (value != oldValue)) {
		ShowControl(MyDocVScroll);
	}
}

LOCALPROC AdjustScrollValues(Boolean canRedraw)
{
	AdjustV(canRedraw);
}

LOCALPROC AdjustScrollbars(void)
{
	/*
		First, turn visibility of scrollbars off so we won't get
		unwanted redrawing
	*/
	My_SetControlVisibilityFalse(MyDocVScroll); /* turn them off */
	AdjustScrollValues(falseblnr);
		/* fool with max and current value */
	/*
		Now, restore visibility in case we never had to ShowControl
		during adjustment
	*/
	My_SetControlVisibilityTrue(MyDocVScroll); /* turn them on */
}

extern pascal void PascalClikLoop(void); /* export to ASM code */

pascal void PascalClikLoop(void)
{
	RgnHandle region;
	Rect r;

	region = NewRgn();
	if (NULL != region) {
		GetClip(region); /* save clip */
		My_GetWindowBounds(MyMainWind, &r);
		ClipRect(&r);
		AdjustScrollValues(true); /* pass true for canRedraw */
		SetClip(region); /* restore clip */
		DisposeRgn(region);
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

LOCALPROC DoIdle(void)
{
	WindowRef window;

	window = FrontWindow();
	if (window == MyMainWind) {
		TEIdle(MyDocTE);
	}
}

LOCALPROC DoUpdateMenus(void)
{
	WindowRef window;
	MenuRef menu;
	long offset;
	Boolean undo;
	Boolean cutCopyClear;
	Boolean paste;
	TEHandle te;

	window = FrontWindow();

	menu = GetMenuHandle(kEditMenu);
	undo = false;
	cutCopyClear = false;
	paste = false;
	if (IsDAWindow(window)) {
		undo = true; /* all editing is enabled for DA windows */
		cutCopyClear = true;
		paste = true;
	} else if (window == MyMainWind) {
		te = MyDocTE;
		if ((*te)->selStart < (*te)->selEnd) {
			cutCopyClear = true;
			/*
				Cut, Copy, and Clear is enabled for app.
				windows with selections
			*/
		}
		if (GetScrap(nil, 'TEXT', &offset)) {
			paste = true;
				/*
					if there's any text in the clipboard,
					paste is enabled
				*/
		}
	}
	if (undo) {
		EnableMenuItem(menu, kEditUndoItem);
	} else {
		DisableMenuItem(menu, kEditUndoItem);
	}
	if (cutCopyClear) {
		EnableMenuItem(menu, kEditCutItem);
		EnableMenuItem(menu, kEditCopyItem);
		EnableMenuItem(menu, kEditClearItem);
	} else {
		DisableMenuItem(menu, kEditCutItem);
		DisableMenuItem(menu, kEditCopyItem);
		DisableMenuItem(menu, kEditClearItem);
	}
	if (paste) {
		EnableMenuItem(menu, kEditPasteItem);
	} else {
		DisableMenuItem(menu, kEditPasteItem);
	}
	EnableMenuItem(menu, kEditSelectAllItem);
}

LOCALPROC DoQuit(void)
{
	/*
		Quit requested.
		For this program can always quit immediatedly.
	*/
	ProgramDone = trueblnr;
}

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
		err = CreateOpenNewFile_v2((StringPtr)"\pOutput File", s,
			'GrBl' /* creator (ExportFl) */,
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

	err = CombineErr(err, ProgressBar_SetStage_v2((noErr == err)
		? "File written."
			: "Failed to write file.",
		0));
	if (kMyErrUsrCancel != err) {
		(void) CheckSysErr(err);
	}
}

LOCALPROC DecodeSysMenus(long theMenuItem)
{
	short menuID;
	short menuItem;
	short daRefNum;
	Str255 daName;
	OSErr saveErr;
	TEHandle te;
	WindowRef window;
	Handle aHandle;
	long oldSize;
	long newSize;

	window = FrontWindow();
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
					break;
			}
			break;
		case kFileMenu:
			switch (menuItem) {
				case kFileGoItem:
					ParseHandle = (Handle) TEGetText(MyDocTE);
					ParseRangeStart = 0;
					ParseRangeStop = (*MyDocTE)->teLength;
					GoRequested = trueblnr;
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
				te = MyDocTE;
				switch (menuItem) {
					case kEditCutItem:
						if (ZeroScrap() == noErr) {
#if 0
	/*
		not on 64k ROM, our memory managment scheme should take
		care of this anyway
	*/
							long total;
							long contig;
							PurgeSpace(&total, &contig);
							if ((*MyDocTE)->selEnd
								- (*MyDocTE)->selStart + kTESlop
								> contig)
							{
								AlertUser(eNoSpaceCut);
							} else
#endif
							{
								TECut(MyDocTE);
								if (TEToScrap() != noErr) {
									AlertUser(eNoCut);
									ZeroScrap();
								}
							}
						}
						break;
					case kEditCopyItem:
						if (ZeroScrap() == noErr) {
							TECopy(MyDocTE);
								/* after copying, export the TE scrap */
							if (TEToScrap() != noErr) {
								AlertUser(eNoCopy);
								ZeroScrap();
							}
						}
						break;
					case kEditPasteItem:
						/* import the TE scrap before pasting */
						if (TEFromScrap() == noErr) {
							if (TEGetScrapLength()
								+ ((*MyDocTE)->teLength
								- ((*MyDocTE)->selEnd
								- (*MyDocTE)->selStart))
								> kMaxTELength)
							{
								AlertUser(eExceedPaste);
							} else {
								aHandle = (Handle) TEGetText(MyDocTE);
								oldSize = GetHandleSize(aHandle);
								newSize = oldSize + TEGetScrapLength()
									+ kTESlop;
								SetHandleSize(aHandle, newSize);
								saveErr = MemError();
								SetHandleSize(aHandle, oldSize);
								if (saveErr != noErr) {
									AlertUser(eNoSpacePaste);
								} else {
									TEPaste(MyDocTE);
								}
							}
						} else {
							AlertUser(eNoPaste);
						}
						break;
					case kEditClearItem:
						TEDelete(MyDocTE);
						break;
					case kEditSelectAllItem:
						TESetSelect(0, 32767, MyDocTE);
						break;
				}
				AdjustScrollbars();
				AdjustTE();
			}
			break;
	}
	HiliteMenu(0);
		/* unhighlight what MenuSelect (or MenuKey) hilited */
}

static pascal void MyTrackActionProc(ControlRef control,
	ControlPartCode part)
{
	VActionProc(control, part);
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
#if WantRealInputFile
		ProgressBar_InvertAll();
		while (Button()) {
		}
		ProgressBar_InvertAll();
		ParseHandle = (Handle) TEGetText(MyDocTE);
		ParseRangeStart = 0;
		ParseRangeStop = (*MyDocTE)->teLength;
		GoRequested = trueblnr;
#endif
	} else {
		part = FindControl(mouse, MyMainWind, &control);
		switch (part) {
			case 0: /* do nothing for viewRect case */
				break;
			case kControlIndicatorPart:
				value = GetControlValue(control);
				part = TrackControl(control, mouse, nil);
				if (part != 0) {
					value -= GetControlValue(control);
					/*
						value now has CHANGE in value;
						if value changed, scroll
					*/
					if (value != 0) {
						TEScroll(0, value * (*MyDocTE)->lineHeight,
							MyDocTE);
					}
				}
				break;
			default: /* they clicked in an arrow, so track & scroll */
				(void) TrackControl(control, mouse, MyTrackActionProc);
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

	AdjustScrollValues(false);
	/* fool with max and current value */

	AdjustTE();
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

LOCALPROC DoKeyDown(void)
{
	char key;

	key = curevent.message & charCodeMask;
	/*
		we have a char. for our window; see if we are still below
		TextEdit's limit for the number of characters (but deletes
		are always rad)
	*/
	if (key == kDelChar ||
		(*MyDocTE)->teLength
			- ((*MyDocTE)->selEnd - (*MyDocTE)->selStart) + 1
		< kMaxTELength)
	{
		TEKey(key, MyDocTE);
		AdjustScrollbars();
		AdjustTE();
	} else {
		AlertUser(eExceedChar);
	}
}

LOCALPROC DoActivate(Boolean becomingActive)
{
	RgnHandle tempRgn;
	RgnHandle clipRgn;
	MyWindRectsR r;

	GetMyWindRects(&r);

	if (becomingActive) {
		/*
			since we don't want TEActivate to draw a selection in
			an area where we're going to erase and redraw, we'll clip
			out the update region before calling it.
		*/
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

LOCALVAR WindowRef MacEventWind;

LOCALPROC inMenuBarAction(void)
{
	DoUpdateMenus();
	DecodeSysMenus(MenuSelect(curevent.where));
}

LOCALPROC inSysWindowAction(void)
{
	SystemClick(&curevent, MacEventWind);
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
}

LOCALPROC inGoAwayAction(void)
{
	if (TrackGoAway(MacEventWind, curevent.where)) {
		if (MacEventWind == MyMainWind) {
			DoQuit();
		} else {
			CloseAWindow(MacEventWind);
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
	char key;
	key = curevent.message & charCodeMask;
	if (curevent.modifiers & cmdKey) { /* Command key down */
		if (curevent.what == keyDown) {
			DoUpdateMenus();
				/* enable/disable/check menu items properly */
			DecodeSysMenus(MenuKey(key));
		}
	} else {
		WindowRef window = FrontWindow();
		if (window == MyMainWind) {
			DoKeyDown();
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
	WindowRef window = (WindowRef) curevent.message;
	if (window == MyMainWind) {
		DoActivate((curevent.modifiers & activeFlag) != 0);
	}
}

LOCALPROC ProcessDiskEvt(void)
{
	Point where;

	if (HiWord(curevent.message) != noErr) {
		SetPt(&where, 70, 50);
		ResumeSystemCursor();
		(void) DIBadMount(where, curevent.message);
	}
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
			vCheckSysErr(err);
		}
	} else {
		vCheckSysErr(errAENotAppleEvent);
	}
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
					{
						WindowRef window = FrontWindow();
						if (window == MyMainWind) {
							DoActivate(! gBackgroundFlag);
						}
					}
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

	if (noErr == (err = MyMemoryCheckSpare())) {
		w0 = NewWindow(wStorage, boundsRect, title,
			visible, theProc, behind, goAwayFlag, refCon);
		if (NULL == w0) {
			err = kMyErrSysUnknown;
		} else {
			if (noErr == (err = MyMemoryCheckSpare())) {
				*w = w0;
			} else {
				if (NULL == wStorage) {
					DisposeWindow(w0);
				} else {
					CloseAWindow(w0);
				}
			}
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
#if Support64kROM
			if (! Have64kROM())
#endif
			{
				TEAutoView(true, MyDocTE);
			}
			p = (ui5b *)&AsmClikLoop[1];
			*p++ = (ui5b)(*MyDocTE)->clickLoop;
			*p = (ui5b)PascalClikLoop;
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
			/*
				false to AdjustScrollValues means musn't redraw;
				technically, of course, the window is hidden so it
				wouldn't matter whether we called ShowControl or not.
			*/
			AdjustScrollValues(false);
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


LOCALFUNC tMyErr MyFileRead2Hand(MyDir_R *d, StringPtr s, Handle *r)
{
	tMyErr err;
	short refNum;
	uimr n;
	Handle h = nullpr;

	if (noErr == (err = MyOpenOldFileRead_v2(d, s, &refNum))) {
		if (noErr == (err = MyOpenFileGetEOF_v2(refNum, &n))) {
			if (noErr == (err = MyHandleNew_v2(n, &h)))  {
				HLock(h);
				err = MyReadBytes_v2(refNum, *h, n);
				HUnlock(h);

				if (noErr == err) {
					*r = h;
					h = nullpr;
				}
			}
		}
		err = CombineErr(err, MyCloseFile_v2(refNum));
	}

	if (nullpr != h) {
		DisposeHandle(h);
	}

	return err;
}

LOCALPROC MyDoNextEvents(blnr busy)
{
	RgnHandle cursorRgn;
	Boolean gotEvent;

	cursorRgn = NewRgn();
		/* we'll pass WNE an empty region the 1st time thru */
	UpdateProgressBar();
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

LOCALPROC WaitForInput(void)
{
	input_r InputCur;

	do {
		AppReportAnySavedError();
		if (PopFromInputA(&InputCur)) {
			tMyErr err;
			Str255 s;
			Handle h = nullpr;

			HandToPStr(InputCur.Name, s);
			if (noErr == (err = MyFileRead2Hand(&InputCur.d, s, &h))) {
				TESetSelect(0, 32767, MyDocTE);
				TEDelete(MyDocTE);
				HLock(h);
				TEInsert(*h, GetHandleSize(h), MyDocTE);
				HUnlock(h);
				AdjustScrollbars();
				AdjustTE();
				ParseHandle = (Handle) TEGetText(MyDocTE);
				ParseRangeStart = 0;
				ParseRangeStop = (*MyDocTE)->teLength;
				GoRequested = trueblnr;
			}

			if (nullpr != h) {
				DisposeHandle(h);
			}

			DisposeInputR(&InputCur);

			(void) CheckSysErr(err);
		} else
#if AutoQuitIfStartUpFile
		if (SawStartUpFile) {
			ProgramDone = trueblnr;
		} else
#endif
		{
			MyDoNextEvents(falseblnr);
		}
	} while ((! ProgramDone) && (! GoRequested));
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

LOCALVAR long NextCheckTick;
LOCALVAR long NextCheckEventTick;

LOCALPROC CheckAbort0_v2(void)
{
	blnr ShouldGetEvent;
	long CurrentTick = LMGetTicks();
	NextCheckTick = CurrentTick + 6;

	if (gBackgroundFlag) {
		ShouldGetEvent = trueblnr;
		/*
			while in background, relinquish processor
			ten times a second
		*/
	} else if (CurrentTick > NextCheckEventTick) {
		ShouldGetEvent = trueblnr;
		/*
			while in foreground, relinquish processor
			once a second at minimum
		*/
	} else {
		ShouldGetEvent = EventPendingQuickCheck();
	}
	if (ShouldGetEvent) {
		NextCheckEventTick = CurrentTick + 60;
		MyDoNextEvents(trueblnr);
	} else {
		UpdateProgressBar();
	}
}

GLOBALFUNC tMyErr CheckAbortRequested(void)
{
	if (LMGetTicks() >= NextCheckTick) {
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

	NextCheckTick = LMGetTicks();
	NextCheckEventTick = NextCheckTick;

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
			(void) CheckSysErr(MyAppFilesInit_v2());
			PrepareForDragging();
#endif
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
	HandRangeToCStr(ParseHandle, The_arg_range_start,
		L, s);
}

LOCALPROC GetCurArgAsPStr(ps3p s)
{
	/* warning : assumes (! The_arg_end) */
	HandRangeToPStr(ParseHandle, The_arg_range_start,
		The_arg_range_size, s);
}

LOCALFUNC tMyErr GetCurArgAsHandle_v2(Handle *r)
{
	/* warning : assumes (! The_arg_end) */
	return HandRangeToHandle_v2(ParseHandle, The_arg_range_start,
		The_arg_range_size, r);
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
		TESetSelect(0, 32767, MyDocTE);
	}
}
