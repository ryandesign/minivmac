/*
	CMDARGW1.i
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
	CoMmandD ARGument Window part 1

	inspired by Apple's TESample that came with MPW 3
*/

#define My_GetWindowBounds(window, r) GetPortBounds(GetWindowPort(window), (r))


#define kControlInvisible 0
#define My_SetControlVisibilityFalse(inControl) (*inControl)->contrlVis = kControlInvisible

#define kControlVisible 0xFF
#define My_SetControlVisibilityTrue(inControl) (*inControl)->contrlVis = kControlVisible

GLOBALPROC MyGetGrayRgnBounds(Rect *r)
{
	GetRegionBounds(GetGrayRgn(), r);
}

LOCALPROC ResumeSystemCursor(void)
{
	SetCursor(&(qd.arrow));
}

LOCALVAR WindowRef gMyMainWindow = NULL;
LOCALVAR blnr ProgramDone = falseblnr;

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
#define kFileQuitItem 3

#define kEditUndoItem 1
#define kEditCutItem 3
#define kEditCopyItem 4
#define kEditPasteItem 5
#define kEditClearItem 6
#define kEditSelectAllItem 8

#define kTextMargin 2

#define TEWidthFromWindow 1

#if ! TEWidthFromWindow
#define kMaxDocWidth 576
#endif

#define kMinDocDim 64

#define kScrollbarWidth 16
#define kScrollbarAdjust (kScrollbarWidth - 1)

#define kScrollTweek 2

#define kCrChar 13
#define kDelChar 8

#define kButtonScroll 4

#define kMaxTELength 32000

#define kSysEnvironsVersion 1

#define kOSEvent app4Evt /* event used by MultiFinder */
#define kSuspendResumeMessage 1 /* high byte of suspend/resume event message */
#define kResumeMask 1 /* bit of message field for resume vs. suspend */
#define kMouseMovedMessage 0xFA /* high byte of mouse-moved event message */

#define kExtremeNeg -32768
#define kExtremePos (32767 - 1) /* required to address an old region bug */

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

#define rAboutAlert 128 /* about alert */
#define rUserAlert  129 /* user error alert */
#define kErrStrings 128 /* error string list */


LOCALVAR TEHandle MyDocTE = NULL;
LOCALVAR ControlRef MyDocVScroll = NULL;
#if ! TEWidthFromWindow
LOCALVAR ControlRef MyDocHScroll = NULL;
#endif
LOCALVAR TEClickLoopUPP MyDocOldClik = NULL;


LOCALVAR Boolean gInBackground = falseblnr;


#define TopLeft(aRect)  (* (Point *) &(aRect).top)
#define BotRight(aRect) (* (Point *) &(aRect).bottom)



extern pascal void AsmClikLoop(void);

LOCALPROC GetWindowPortGlobalOffset(WindowRef w, Point *p)
{
	p->v = - w->portBits.bounds.top;
	p->h = - w->portBits.bounds.left;
}

LOCALPROC GetLocalUpdateRgn(RgnHandle localRgn)
{
	Point p;

	CopyRgn(((WindowPeek) gMyMainWindow)->updateRgn, localRgn); /* save old update region */
	GetWindowPortGlobalOffset(gMyMainWindow, &p);
	OffsetRgn(localRgn, - p.h, - p.v);
}

LOCALFUNC unsigned long GetSleep(void)
{
	long sleep;
	WindowRef window;
	TEHandle te;

	sleep = LONG_MAX; /* default value for sleep */
	if (! gInBackground) {
		window = FrontWindow(); /* and the front window is ours... */
		if (window == gMyMainWindow) {
			te = MyDocTE; /* and the selection is an insertion point... */
			if ((*te)->selStart == (*te)->selEnd) {
				sleep = GetCaretTime(); /* blink time for the insertion point */
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

static pascal void VActionProc(ControlRef control, short part)
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
				amount = (te->viewRect.bottom - te->viewRect.top) / te->lineHeight;
				break;
		}
		if ((part == kControlDownButtonPart) || (part == kControlPageDownPart)) {
			amount = - amount; /* reverse direction for a downer */
		}
		CommonAction(control, &amount);
		if (amount != 0) {
			TEScroll(0, amount * te->lineHeight, MyDocTE);
		}
	}
}


#if ! TEWidthFromWindow
static pascal void HActionProc(ControlRef control, short part)
{
	short amount;
	TEPtr te;

	if (part != 0) {
		te = *MyDocTE;
		switch (part) {
			case kControlUpButtonPart:
			case kControlDownButtonPart: /* a few pixels */
				amount = kButtonScroll;
				break;
			case kControlPageUpPart: /* a page */
			case kControlPageDownPart:
				amount = te->viewRect.right - te->viewRect.left;
				break;
		}
		if ((part == kControlDownButtonPart) || (part == kControlPageDownPart)) {
			amount = - amount; /* reverse direction */
		}
		CommonAction(control, &amount);
		if (amount != 0) {
			TEScroll(amount, 0, MyDocTE);
		}
	}
}
#endif

LOCALPROC MyDrawGrowIcon(WindowRef w)
{
	/* Assuming w is the current port */
	RgnHandle SaveRgn;
	RgnHandle growr;

	SaveRgn = NewRgn();
	if (NULL != SaveRgn) {
		GetClip(SaveRgn);
		growr = NewRgn();
		if (NULL != growr) {
			Rect r;

			My_GetWindowBounds(w, &r);
			r.top = r.bottom - 14;
			r.left = r.right - 14;
			RectRgn(growr, &r);
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

	My_GetWindowBounds(gMyMainWindow, &r);
	r.left = r.right - 15;
	r.right = r.left + 1;
	PaintRect(&r);

	My_GetWindowBounds(gMyMainWindow, &r);
	r.top = r.bottom - 15;
	r.bottom = r.top + 1;
#if TEWidthFromWindow
	r.left = r.right - 14;
#endif
	PaintRect(&r);
}

LOCALPROC DrawWindow(void)
{
	Rect r;

	DrawTheLines();
	DrawControls(gMyMainWindow);
	MyDrawGrowIcon(gMyMainWindow);
	My_GetWindowBounds(gMyMainWindow, &r);
	TEUpdate(&r, MyDocTE);
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
	if (gMyMainWindow != NULL) {
		if (MyDocTE != nil) {
			TEDispose(MyDocTE); /* dispose the TEHandle if we got far enough to make one */
		}
		DisposeWindow(gMyMainWindow);
	}
}

LOCALPROC AlertUserFromPStr(ps3p message)
{
	short itemHit;

	ResumeSystemCursor();
	ParamText(message, "\p", "\p", "\p");

	itemHit = Alert(rUserAlert, nil);
}

LOCALPROC AlertUser(short error)
{
	Str255 message;

	GetIndString(message, kErrStrings, error);
	AlertUserFromPStr(message);
}

LOCALPROC BigBadError(short error)
{
	AlertUser(error);
	ExitToShell();
}

GLOBALPROC AppReportAnySavedError(void)
{
	if (SavedSysErr != noErr) {
		MyPStr t;

		PStrFromCStr(t, GetTextForSavedSysErr());
		AlertUserFromPStr(t);

		SavedSysErr = noErr;
	}
}

LOCALPROC GetTERect(Rect *teRect)
{
	My_GetWindowBounds(gMyMainWindow, teRect);
	InsetRect(teRect, kTextMargin, kTextMargin); /* adjust for margin */
#if ! TEWidthFromWindow
	teRect->bottom = teRect->bottom - 15; /* and for the scrollbars */
#endif
	teRect->right = teRect->right - 15;
}

LOCALPROC AdjustViewRect(void)
{
#if 0
	TEPtr te;

	te = *MyDocTE;
	te->viewRect.bottom = (((te->viewRect.bottom - te->viewRect.top) / te->lineHeight)
							* te->lineHeight) + te->viewRect.top;
#endif
}

LOCALPROC AdjustTE(void)
{
	TEPtr te;

	te = *MyDocTE;
	TEScroll(
#if TEWidthFromWindow
		0
#else
		(te->viewRect.left - te->destRect.left) -
			GetControlValue(MyDocHScroll)
#endif
			,
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
	/* since nLines is not right if the last character is a return, check for that case */
	if (*(*te->hText + te->teLength - 1) == kCrChar) {
		lines += 1;
	}
	max = lines - ((te->viewRect.bottom - te->viewRect.top) /
		te->lineHeight);

	if (max < 0) {
		max = 0;
	}
	SetControlMaximum(MyDocVScroll, max);

	/* Must deref. after SetControlMaximum since, technically, it could draw and therefore move
		memory. This is why we do not just do it once at the beginning. */
	te = *MyDocTE;
	value = (te->viewRect.top - te->destRect.top) / te->lineHeight;

	if (value < 0) {
		value = 0;
	} else if (value >  max) {
		value = max;
	}

	SetControlValue(MyDocVScroll, value);

	if (canRedraw || (max != oldMax) || (value != oldValue)) {
		ShowControl(MyDocVScroll);
	}
}

#if ! TEWidthFromWindow
LOCALPROC AdjustH(Boolean canRedraw)
{
	short value;
	short lines;
	short max;
	short oldValue, oldMax;
	TEPtr te;

	oldValue = GetControlValue(MyDocHScroll);
	oldMax = GetControlMaximum(MyDocHScroll);
	te = *MyDocTE; /* point to TERec for convenience */
	max = kMaxDocWidth - (te->viewRect.right - te->viewRect.left);

	if (max < 0) {
		max = 0;
	}
	SetControlMaximum(MyDocHScroll, max);

	/* Must deref. after SetControlMaximum since, technically, it could draw and therefore move
		memory. This is why we do not just do it once at the beginning. */
	te = *MyDocTE;
	value = te->viewRect.left - te->destRect.left;

	if (value < 0) {
		value = 0;
	} else if (value >  max) {
		value = max;
	}

	SetControlValue(MyDocHScroll, value);

	if (canRedraw || (max != oldMax) || (value != oldValue)) {
		ShowControl(MyDocHScroll);
	}
}
#endif

LOCALPROC AdjustScrollValues(Boolean canRedraw)
{
	AdjustV(canRedraw);
#if ! TEWidthFromWindow
	AdjustH(canRedraw);
#endif
}

LOCALPROC AdjustScrollSizes(void)
{
	Rect teRect;
	Rect r;

	GetTERect(&teRect); /* start with TERect */
	(*MyDocTE)->viewRect = teRect;

	AdjustViewRect(); /* snap to nearest line */
	My_GetWindowBounds(gMyMainWindow, &r);
	MoveControl(MyDocVScroll, r.right - kScrollbarAdjust, -1);
	SizeControl(MyDocVScroll, kScrollbarWidth, (r.bottom -
		r.top) - (kScrollbarAdjust - kScrollTweek));
#if ! TEWidthFromWindow
	MoveControl(MyDocHScroll, -1, r.bottom - kScrollbarAdjust);
	SizeControl(MyDocHScroll, (r.right -
		r.left) - (kScrollbarAdjust - kScrollTweek),
		kScrollbarWidth);
#endif
#if TEWidthFromWindow
		(*MyDocTE)->destRect.right = teRect.right;
		TECalText(MyDocTE);
#endif
}

LOCALPROC AdjustScrollbars(Boolean needsResize)
{
	/* First, turn visibility of scrollbars off so we won't get unwanted redrawing */
	My_SetControlVisibilityFalse(MyDocVScroll); /* turn them off */
#if ! TEWidthFromWindow
	My_SetControlVisibilityFalse(MyDocHScroll);
#endif
	if (needsResize) { /* move & size as needed */
		AdjustScrollSizes();
	}
	AdjustScrollValues(needsResize); /* fool with max and current value */
	/* Now, restore visibility in case we never had to ShowControl during adjustment */
	My_SetControlVisibilityTrue(MyDocVScroll); /* turn them on */
#if ! TEWidthFromWindow
	My_SetControlVisibilityTrue(MyDocHScroll);
#endif
}

extern pascal void PascalClikLoop(void); /* export to ASM code */

pascal void PascalClikLoop(void)
{
	RgnHandle region;
	Rect r;

	region = NewRgn();
	if (NULL != region) {
		GetClip(region); /* save clip */
		My_GetWindowBounds(gMyMainWindow, &r);
		ClipRect(&r);
		AdjustScrollValues(true); /* pass true for canRedraw */
		SetClip(region); /* restore clip */
		DisposeRgn(region);
	}
}

extern pascal ProcPtr GetOldClikLoop(void); /* export to ASM code */

pascal ProcPtr GetOldClikLoop()
{
	return (ProcPtr)MyDocOldClik;
}

LOCALPROC AdjustCursor(Point mouse, RgnHandle region)
{
	WindowRef window;
	RgnHandle arrowRgn;
	RgnHandle iBeamRgn;
	RgnHandle GlobalVisRgn;
	Rect iBeamRect;
	Point p;

	window = FrontWindow(); /* we only adjust the cursor when we are in front */
	if ((! gInBackground) && (! IsDAWindow(window))) {
		/* calculate regions for different cursor shapes */
		arrowRgn = NewRgn();
		iBeamRgn = NewRgn();
		GlobalVisRgn = NewRgn();

		/* start arrowRgn wide open */
		SetRectRgn(arrowRgn, kExtremeNeg, kExtremeNeg, kExtremePos, kExtremePos);

		/* calculate iBeamRgn */
		if (window == gMyMainWindow) {
			iBeamRect = (*MyDocTE)->viewRect;
			SetPortWindowPort(window); /* make a global version of the viewRect */
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
	if (window == gMyMainWindow) {
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
	} else if (window == gMyMainWindow) {
		te = MyDocTE;
		if ((*te)->selStart < (*te)->selEnd) {
			cutCopyClear = true;
			/* Cut, Copy, and Clear is enabled for app. windows with selections */
		}
		if (GetScrap(nil, 'TEXT', &offset)) {
			paste = true; /* if there's any text in the clipboard, paste is enabled */
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

LOCALPROC DecodeSysMenus(long theMenuItem)
{
	short menuID;
	short menuItem;
	short itemHit;
	short daRefNum;
	Str255 daName;
	OSErr saveErr;
	TEHandle te;
	WindowRef window;
	Handle aHandle;
	long oldSize;
	long newSize;
	long total;
	long contig;

	window = FrontWindow();
	menuID = HiWord(theMenuItem); /* use macros for efficiency to... */
	menuItem = LoWord(theMenuItem); /* get menu item number and menu number */
	switch (menuID) {
		case kAppleMenu:
			switch (menuItem) {
				case kAppleAboutItem: /* bring up alert for About */
					itemHit = Alert(rAboutAlert, nil);
					break;
				default: /* all non-About items in this menu are DAs et al */
					/* type Str255 is an array in MPW 3 */
					GetMenuItemText(GetMenuHandle(kAppleMenu), menuItem, daName);
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
				case kFileQuitItem:
					DoQuit();
					break;
			}
			break;
		case kEditMenu: /* call SystemEdit for DA editing & MultiFinder */
			if (! SystemEdit(menuItem - 1)) {
				te = MyDocTE;
				switch (menuItem) {
					case kEditCutItem:
						if (ZeroScrap() == noErr) {
							PurgeSpace(&total, &contig);
							if ((*MyDocTE)->selEnd - (*MyDocTE)->selStart + kTESlop > contig) {
								AlertUser(eNoSpaceCut);
							} else {
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
							TECopy(MyDocTE); /* after copying, export the TE scrap */
							if (TEToScrap() != noErr) {
								AlertUser(eNoCopy);
								ZeroScrap();
							}
						}
						break;
					case kEditPasteItem: /* import the TE scrap before pasting */
						if (TEFromScrap() == noErr) {
							if (TEGetScrapLength() + ((*MyDocTE)->teLength -
								((*MyDocTE)->selEnd - (*MyDocTE)->selStart)) > kMaxTELength)
							{
								AlertUser(eExceedPaste);
							} else {
								aHandle = (Handle) TEGetText(MyDocTE);
								oldSize = GetHandleSize(aHandle);
								newSize = oldSize + TEGetScrapLength() + kTESlop;
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
				AdjustScrollbars(false);
				AdjustTE();
			}
			break;
	}
	HiliteMenu(0); /* unhighlight what MenuSelect (or MenuKey) hilited */
}

static pascal void MyTrackActionProc(ControlRef control, short part)
{
#if ! TEWidthFromWindow
	if (control == MyDocHScroll) {
		HActionProc(control, part);
	} else
#endif
	{
		VActionProc(control, part);
	}
}

LOCALPROC DoContentClick(void)
{
	Point mouse;
	ControlRef control;
	short part;
	short value;
	Boolean shiftDown;
	Rect teRect;

	mouse = curevent.where; /* get the click position */
	GlobalToLocal(&mouse);
	/* see if we are in the viewRect. if so, we won't check the controls */
	GetTERect(&teRect);
	if (PtInRect(mouse, &teRect)) {
		/* see if we need to extend the selection */
		shiftDown = (curevent.modifiers & shiftKey) != 0; /* extend if Shift is down */
		TEClick(mouse, shiftDown, MyDocTE);
	} else {
		part = FindControl(mouse, gMyMainWindow, &control);
		switch (part) {
			case 0: /* do nothing for viewRect case */
				break;
			case kControlIndicatorPart:
				value = GetControlValue(control);
				part = TrackControl(control, mouse, nil);
				if (part != 0) {
					value -= GetControlValue(control);
					/* value now has CHANGE in value; if value changed, scroll */
					if (value != 0) {
#if ! TEWidthFromWindow
						if (control == MyDocHScroll) {
							TEScroll(value, 0, MyDocTE);
						} else
#endif
						{
							TEScroll(0, value * (*MyDocTE)->lineHeight, MyDocTE);
						}
					}
				}
				break;
			default: /* they clicked in an arrow, so track & scroll */
				value = TrackControl(control, mouse, (ProcPtr)MyTrackActionProc);
				break;
		}
	}
}

LOCALPROC ResizeMyWindow(void)
{
	Rect r;

	AdjustScrollbars(true);
	AdjustTE();
	My_GetWindowBounds(gMyMainWindow, &r);
	InvalRect(&r);
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
	growResult = GrowWindow(gMyMainWindow, curevent.where, &tempRect);
	/* see if it really changed size */
	if (growResult != 0) {
		tempRect = (*MyDocTE)->viewRect; /* save old text box */
		tempRgn = NewRgn();
		if (NULL != tempRgn) {
			GetLocalUpdateRgn(tempRgn); /* get localized update region */
			SizeWindow(gMyMainWindow, LoWord(growResult), HiWord(growResult), true);
			ResizeMyWindow();
			/* calculate & validate the region that hasn't changed so it won't get redrawn */
			SectRect(&tempRect, &(*MyDocTE)->viewRect, &tempRect);
#if ! TEWidthFromWindow
			ValidRect(&tempRect); /* take it out of update */
#endif
			InvalRgn(tempRgn); /* put back any prior update */
			DisposeRgn(tempRgn);
		}
	}
}

LOCALPROC DoZoomWindow(short which_part)
{
	Rect r;

	My_GetWindowBounds(gMyMainWindow, &r);
	EraseRect(&r);
	ZoomWindow(gMyMainWindow, which_part, gMyMainWindow == FrontWindow());
	ResizeMyWindow();
}

LOCALPROC DoKeyDown(void)
{
	char key;

	key = curevent.message & charCodeMask;
	/* we have a char. for our window; see if we are still below TextEdit's
		limit for the number of characters (but deletes are always rad) */
	if (key == kDelChar ||
			(*MyDocTE)->teLength - ((*MyDocTE)->selEnd - (*MyDocTE)->selStart) + 1 <
			kMaxTELength)
	{
		TEKey(key, MyDocTE);
		AdjustScrollbars(false);
		AdjustTE();
	} else {
		AlertUser(eExceedChar);
	}
}

LOCALPROC DoActivate(Boolean becomingActive)
{
	RgnHandle tempRgn;
	RgnHandle clipRgn;
	Rect growRect;
	Rect r;

	if (becomingActive) {
		/*
			since we don't want TEActivate to draw a selection in an area where
			we're going to erase and redraw, we'll clip out the update region
			before calling it.
		*/
		tempRgn = NewRgn();
		clipRgn = NewRgn();
		GetLocalUpdateRgn(tempRgn); /* get localized update region */
		GetClip(clipRgn);
		DiffRgn(clipRgn, tempRgn, tempRgn); /* subtract updateRgn from clipRgn */
		SetClip(tempRgn);
		TEActivate(MyDocTE);
		SetClip(clipRgn); /* restore the full-blown clipRgn */
		DisposeRgn(tempRgn);
		DisposeRgn(clipRgn);

		/* the controls must be redrawn on activation: */
		My_SetControlVisibilityTrue(MyDocVScroll);
		GetControlBounds(MyDocVScroll, &r);
		InvalRect(&r);
#if ! TEWidthFromWindow
		My_SetControlVisibilityTrue(MyDocHScroll);
		GetControlBounds(MyDocHScroll, &r);
		InvalRect(&r);
#endif
		/* the growbox needs to be redrawn on activation: */
		My_GetWindowBounds(gMyMainWindow, &growRect);
		/* adjust for the scrollbars */
		growRect.top = growRect.bottom - kScrollbarAdjust;
		growRect.left = growRect.right - kScrollbarAdjust;
		InvalRect(&growRect);
	} else {
		TEDeactivate(MyDocTE);
		/* the controls must be hidden on deactivation: */
		HideControl(MyDocVScroll);
#if ! TEWidthFromWindow
		HideControl(MyDocHScroll);
#endif
		/* the growbox should be changed immediately on deactivation: */
		MyDrawGrowIcon(gMyMainWindow);
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
	if (MacEventWind == gMyMainWindow) {
		if (MacEventWind != FrontWindow()) {
			SelectWindow(MacEventWind);
			/* ProcessMacEvent(); */ /* use this line for "do first click" */
		} else {
			SetPortWindowPort(gMyMainWindow);
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
		if (MacEventWind == gMyMainWindow) {
			DoQuit();
		} else {
			CloseAWindow(MacEventWind);
		}
	}
}

LOCALPROC inGrowAction(void)
{
	if (MacEventWind == gMyMainWindow) {
		DoGrowWindow();
	}
}

LOCALPROC inZoomAction(short which_part)
{
	if (MacEventWind == gMyMainWindow) {
		if (TrackBox(gMyMainWindow, curevent.where, which_part)) {
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
			DoUpdateMenus(); /* enable/disable/check menu items properly */
			DecodeSysMenus(MenuKey(key));
		}
	} else {
		WindowRef window = FrontWindow();
		if (window == gMyMainWindow) {
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
	if (MacEventWind == gMyMainWindow) {
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
	if (window == gMyMainWindow) {
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
		case kOSEvent:
			switch (curevent.message >> 24) { /* high byte of message */
				case kMouseMovedMessage:
					DoIdle(); /* mouse-moved is also an idle event */
					break;
				case kSuspendResumeMessage: /* suspend/resume is also an activate/deactivate */
					gInBackground = (curevent.message & kResumeMask) == 0;
					{
						WindowRef window = FrontWindow();
						if (window == gMyMainWindow) {
							DoActivate(! gInBackground);
						}
					}
					break;
			}
			break;
	}
}

LOCALFUNC blnr MyMainWindowInit(void)
{
	Boolean good;
	WindowRef window;
	Rect rBounds;
	Rect destRect;
	Rect viewRect;
	blnr IsOk = falseblnr;

	SetRect(&rBounds, 60, 64, 460, 314);
	window = NewWindow(NULL, &rBounds, "\puntitled", false,
		zoomDocProc, (WindowRef) -1, true, 0);
	if (NULL != window) {
		gMyMainWindow = window;
		good = false;
		SetPortWindowPort(window);
		GetTERect(&viewRect);
		destRect = viewRect;
#if ! TEWidthFromWindow
		destRect.right = destRect.left + kMaxDocWidth;
#endif
		MyDocTE = TENew(&destRect, &viewRect);
		AdjustViewRect();
		TEAutoView(true, MyDocTE);
		MyDocOldClik = (*MyDocTE)->clickLoop;
		(*MyDocTE)->clickLoop = (TEClickLoopUPP)AsmClikLoop;

		good = (MyDocTE != nil); /* if TENew succeeded, we have a good document */
		if (good) { /* good document? - get scrollbars */
			SetRect(&rBounds, 385, -1, 401, 236);
			MyDocVScroll = NewControl(window, &rBounds, "\p", true, 0, 0, 0, scrollBarProc, 0);
			good = (MyDocVScroll != nil);
		}
#if ! TEWidthFromWindow
		if (good) {
			SetRect(&rBounds, -1, 235, 386, 251);
			MyDocHScroll = NewControl(window, &rBounds, "\p", true, 0, 0, 0, scrollBarProc, 0);
			good = (MyDocHScroll != nil);
		}
#endif

		if (good) { /* good? - adjust & draw the controls, draw the window */
			/*
				false to AdjustScrollValues means musn't redraw; technically, of course,
				the window is hidden so it wouldn't matter whether we called ShowControl or not.
			*/
			AdjustScrollValues(false);
			ShowWindow(window);
			IsOk = trueblnr;
		} else {
			CloseAWindow(window); /* otherwise regret we ever created it... */
			AlertUser(eNoWindow); /* and tell user */
		}
	}

	return IsOk;
}

LOCALFUNC blnr GotRequiredParams(const AppleEvent *theAppleEvent)
{
	DescType typeCode;
	Size actualSize;
	OSErr theErr;

	theErr = AEGetAttributePtr(theAppleEvent, keyMissedKeywordAttr,
		typeWildCard, &typeCode, NULL, 0, &actualSize);
	if (errAEDescNotFound == theErr) { /* No more required params. */
		return trueblnr;
	} else if (noErr == theErr) { /* More required params! */
		return CheckSysErr(errAEEventNotHandled);
	} else { /* Unexpected Error! */
		return CheckSysErr(theErr);
	}
}

LOCALFUNC blnr GotRequiredParams0(const AppleEvent *theAppleEvent)
{
	DescType typeCode;
	Size actualSize;
	OSErr theErr;

	theErr = AEGetAttributePtr(theAppleEvent, keyMissedKeywordAttr,
				typeWildCard, &typeCode, NULL, 0, &actualSize);
	if (errAEDescNotFound == theErr) { /* No more required params. */
		return trueblnr;
	} else if (noErr == theErr)  { /* More required params! */
		return trueblnr; /* errAEEventNotHandled; */
	} else { /* Unexpected Error! */
		return CheckSysErr(theErr);
	}
}

LOCALFUNC OSErr GetASysResultCode(void)
{
#if 0
	if (noErr == errCode) {
		return noErr;
	} else if (SysCodeAbort == errCode) {
		return SysResultCode;
	} else {
		return -50; /*& paramErr &*/
	}
#endif
	return noErr;
}

#if WantRealInputFile
#define openOnly 1
#define openPrint 2
#endif

#if WantRealInputFile
static pascal OSErr OpenOrPrintFiles(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefcon)
{
#pragma unused(reply, handlerRefcon)

	/*Adapted from IM VI: AppleEvent Manager: Handling Required AppleEvents*/
	AEDescList docList;

#if 0
	FSSpec myFSS;
	simr index;
	simr itemsInList;
	Size actualSize;
	AEKeyword keywd;
	DescType typeCode;
#endif

	/* put the direct parameter (a list of descriptors) into docList */
	if (CheckSysErr(AEGetParamDesc(theAppleEvent, keyDirectObject, typeAEList, &docList))) {
		if (GotRequiredParams0(theAppleEvent)) { /* Check for missing required parameters */
#if 0
			if (CheckSysErr(AECountItems(&docList, &itemsInList))) {
				for (index = 1; index <= itemsInList; ++index) { /* Get each descriptor from the list, get the alias record, open the file, maybe print it. */
					if (CheckSysErr(AEGetNthPtr(&docList, index, typeFSS, &keywd, &typeCode,
										(Ptr)&myFSS, sizeof(myFSS), &actualSize))) {
						ProcessInputFileFSSpec(&myFSS /* , handlerRefcon == openPrint */);
					}
				}
			}
#endif
		}
		vCheckSysErr(AEDisposeDesc(&docList));
	}
	return GetASysResultCode();
}
#endif

static pascal OSErr DoOpenEvent(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefcon)
/*This is the alternative to getting an open document event on startup.*/
{
#pragma unused(reply, handlerRefcon)

	if (GotRequiredParams0(theAppleEvent)) {
	}
	return GetASysResultCode(); /* Make sure there are no additional "required" parameters. */
}

static pascal OSErr DoQuitEvent(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefcon)
{
#pragma unused(reply, handlerRefcon)

	if (GotRequiredParams(theAppleEvent)) {
		ProgramDone = trueblnr;
	}
	return GetASysResultCode();
}

LOCALFUNC blnr MyInstallEventHandler(AEEventClass theAEEventClass, AEEventID theAEEventID,
						AEEventHandlerProcPtr p, long handlerRefcon, blnr isSysHandler)
{
	return CheckSysErr(AEInstallEventHandler(theAEEventClass, theAEEventID,
			p, handlerRefcon, isSysHandler));
}

LOCALFUNC blnr InstallAppleEventHandlers(void)
{
	if (HaveAppleEvtMgrAvail())
	if (CheckSysErr(AESetInteractionAllowed(kAEInteractWithLocal)))
	if (MyInstallEventHandler(kCoreEventClass, kAEOpenApplication, DoOpenEvent, 0, false))
#if WantRealInputFile
	if (MyInstallEventHandler(kCoreEventClass, kAEOpenDocuments, OpenOrPrintFiles, openOnly, false))
	if (MyInstallEventHandler(kCoreEventClass, kAEPrintDocuments, OpenOrPrintFiles, openPrint, false))
#endif
	if (MyInstallEventHandler(kCoreEventClass, kAEQuitApplication, DoQuitEvent, 0, false))
	{
	}

	return trueblnr;
}

LOCALFUNC blnr MyMenusInit(void)
{
	MenuRef theMenu;
	MenuBarHandle menuBar = GetNewMBar(kMyMenuBar);

	if (menuBar == nil) {
		BigBadError(eNoMemory);
	} else {
		SetMenuBar(menuBar); /* install menus */
		DisposeHandle(menuBar);
		theMenu = GetMenuHandle(kAppleMenu);
		if (theMenu != NULL) {
			AppendResMenu(theMenu, 'DRVR'); /* add DA names to Apple menu */
		}

		DrawMenuBar();

		return trueblnr;
	}

	return falseblnr;
}

LOCALPROC WaitForInput(void)
{
	RgnHandle cursorRgn;
	Boolean gotEvent;

	cursorRgn = NewRgn(); /* we'll pass WNE an empty region the 1st time thru */
	do {
		AppReportAnySavedError();
		/* use WNE if it is available */
		if (! HaveWaitNextEventAvail()) {
			SystemTask();
			gotEvent = GetNextEvent(everyEvent, &curevent);
		} else {
			gotEvent = WaitNextEvent(everyEvent, &curevent, GetSleep(), cursorRgn);
		}
		if (gotEvent) {
			/* make sure we have the right cursor before handling the event */
			AdjustCursor(curevent.where, cursorRgn);
			ProcessMacEvent();
		} else {
			DoIdle(); /* perform idle tasks when it's not our event */
		}
		/* change the cursor (and region) if necessary */
		AdjustCursor(curevent.where, cursorRgn);
	} while ((! ProgramDone) && (! GoRequested));
	DisposeRgn(cursorRgn);
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

LOCALFUNC blnr OneWindAppInit(void)
{
	EventRecord event;
	short count;
	blnr IsOk = falseblnr;

	/*
		Now this next bit of code may make you toss your cookies, but it is
		necessary to allow the default button of our alert be outlined.
	*/

	for (count = 1; count <= 3; count++) {
		GetNextEvent(everyEvent, &event);
	}

	/*
		Ignore the error returned from SysEnvirons; even if an error occurred,
		the SysEnvirons glue will fill in the SysEnvRec. You can save a redundant
		call to SysEnvirons by calling it after initializing AppleTalk.
	*/

	{
		SysEnvRec gMac;

		SysEnvirons(kSysEnvironsVersion, &gMac);

		/* Make sure that the machine has at least 128K ROMs. If it doesn't, exit. */

		if (gMac.machineType < 0) {
			BigBadError(eWrongMachine);
		}
	}

	if (MyMemory_Init())
	if (InstallAppleEventHandlers())
	if (MyMenusInit())
	if (MyMainWindowInit())
	{
		IsOk = trueblnr;
	}


	/* do other initialization here */

	return IsOk; /* create a single empty document */
}

LOCALPROC OneWindAppUnInit(void)
{
	MyMainWindowUnInit();
	CloseAllExtraWindows();
	MyMemory_UnInit();
	AppReportAnySavedError();
}

LOCALPROC DisplayRunErr(char *s)
{
	MyPStr t;

	PStrFromCStr(t, s);
	AlertUserFromPStr(t);
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

LOCALVAR blnr The_arg_rangestart;
LOCALVAR blnr The_arg_rangestop;
LOCALVAR blnr The_arg_end;
LOCALVAR blnr ParseArgsFailed;

LOCALPROC ReportParseFailure(char *s)
{
	DisplayRunErr(s);

	ParseArgsFailed = trueblnr;
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
		The_arg_end = trueblnr;
	} else if (CurCharIsWhiteSpace()) {
		AdvanceParseChar();
		goto remove_white;
	} else if ('"' == ParseCharCur) {
		AdvanceParseChar();

		The_arg_rangestart = ParseCharIndex;

		while ((! ParseCharDone) && ('"' != ParseCharCur)) {
			AdvanceParseChar();
		}
		The_arg_rangestop = ParseCharIndex;
		if (! ParseCharDone) {
			AdvanceParseChar();
		} else {
			ReportParseFailure("missing '\"' to end string");
		}
	} else if ('{' == ParseCharCur) {
		The_arg_rangestart = ParseCharIndex;
		AdvanceParseChar();

		while ((! ParseCharDone) && ('}' != ParseCharCur)) {
			AdvanceParseChar();
		}
		if (! ParseCharDone) {
			AdvanceParseChar();
			goto remove_white;
		} else {
			The_arg_rangestop = ParseCharIndex;
			ReportParseFailure("missing '}' to end comment");
		}
	} else {
		The_arg_rangestart = ParseCharIndex;

		do {
			AdvanceParseChar();
		} while ((! ParseCharDone) && ! CurCharIsWhiteSpace());
		The_arg_rangestop = ParseCharIndex;
	}
}

LOCALFUNC blnr CurArgIsCStr(char *s)
{
	if (The_arg_end) {
		return falseblnr;
	} else {
		MyCharR *p = ((MyCharR *)*ParseHandle + The_arg_rangestart);
		uimr n = The_arg_rangestop - The_arg_rangestart;
		simr i;

		for (i = 0; i < n; ++i) {
			if (*s++ != *p++) {
				return falseblnr;
			}
		}
		if (0 != *s) {
			return falseblnr;
		}

		return trueblnr;
	}
}

LOCALFUNC blnr CurArgIsCStrAdvance(char *s)
{
	if (! CurArgIsCStr(s)) {
		return falseblnr;
	} else {
		AdvanceTheArg();
		return trueblnr;
	}
}

LOCALPROC GetCurArgAsPStr(ps3p s)
{
	HandRangeToPStr(ParseHandle, The_arg_rangestart,
		The_arg_rangestop - The_arg_rangestart, s);
}

LOCALFUNC blnr GetCurArgAsHandle(Handle *r)
{
	return HandRangeToHandle(ParseHandle, The_arg_rangestart,
		The_arg_rangestop - The_arg_rangestart, r);
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
		if (The_arg_end) {
			TESetSelect(ParseRangeStop, ParseRangeStop, MyDocTE);
		} else {
			TESetSelect(The_arg_rangestart, The_arg_rangestop, MyDocTE);
		}
	} else {
		TESetSelect(0, 32767, MyDocTE);
	}
}

LOCALVAR blnr AbortRequested = falseblnr;
LOCALVAR uimr ProgressBarVal;

#define CheckAbort()

#define ProgressBar_SetStage(s, n)
#define TimeForCheckAbort() falseblnr
#define CheckAbortProgress(v) falseblnr
