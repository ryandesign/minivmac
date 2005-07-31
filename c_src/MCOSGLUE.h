/*
	MCOSGLUE.h

	Copyright (C) 2005 Philip Cummins, Richard F. Bannister, Paul C. Pratt

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
	MaCintosh Operating System GLUE.

	All operating system dependent code for the
	Macintosh platform should go here.

	This code is descended from Richard F. Bannister's Macintosh
	port of vMac, by Philip Cummins.

	The main entry point 'main' is at the end of this file.
*/

#ifndef NavigationAvail
#define NavigationAvail 1
#endif

#ifndef AppearanceAvail
#define AppearanceAvail 1
#endif

#ifndef NewNamesAvail
#define NewNamesAvail 1
#endif

#ifndef CALL_NOT_IN_CARBON
#define CALL_NOT_IN_CARBON 1
#endif /* !defined(CALL_NOT_IN_CARBON) */

/*--- initial initialization ---*/

#if CALL_NOT_IN_CARBON
#if defined(__SC__) || ((defined(powerc) || defined(__powerc)) && ! defined(__MWERKS__))
/* GLOBALVAR */ QDGlobals qd;
#endif
#endif

LOCALFUNC blnr InitMacManagers(void)
{
#if CALL_NOT_IN_CARBON
	MaxApplZone();
#endif

	{
		int i;

		for (i = 7; --i >= 0; ) {
			MoreMasters();
		}
	}

#if CALL_NOT_IN_CARBON
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(NULL);
#endif
	InitCursor();
	return trueblnr;
}

/*--- some simple utilities ---*/

#define PowOf2(p) ((unsigned long)1 << (p))
#define TestBit(i, p) (((unsigned long)(i) & PowOf2(p)) != 0)

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
	BlockMove((Ptr)srcPtr, (Ptr)destPtr, byteCount);
}

/* don't want to include c libraries, so: */
LOCALFUNC si5b CStrLength(char *src)
{
	char *p = src;
	while (*p++ != 0) {
	}
	return ((si5b)p) - ((si5b)src) - 1;
}

#define PStrMaxLength 255

LOCALPROC PStrFromCStr(ps3p r, /*CONST*/ char *s)
{
	unsigned short L;

	L = CStrLength(s);
	if (L > PStrMaxLength) {
		L = PStrMaxLength;
	}
	*r++ = L;
	MyMoveBytes((anyp)s, (anyp)r, L);
}

LOCALPROC PStrFromChar(ps3p r, char x)
{
	r[0] = 1;
	r[1] = (char)x;
}

/*--- information about the environment ---*/

#define UseCarbonEvents (! CALL_NOT_IN_CARBON)

#ifndef HaveCPUfamM68K
#define HaveCPUfamM68K 0
#endif

#if CALL_NOT_IN_CARBON
LOCALFUNC TrapType GetTrapType(short theTrap)
{
	if ((theTrap & 0x0800) != 0) {
		return ToolTrap;
	} else {
		return OSTrap;
	}
}
#endif

#if CALL_NOT_IN_CARBON
LOCALFUNC short NumToolboxTraps(void)
{
	if (NGetTrapAddress(_InitGraf, ToolTrap)
		== NGetTrapAddress(0xAA6E, ToolTrap))
	{
		return 0x200;
	} else {
		return 0x400;
	}
}
#endif

#if CALL_NOT_IN_CARBON
LOCALFUNC blnr TrapAvailable(short trap_num)
{
	TrapType type;

	type = GetTrapType(trap_num);
	if (type == ToolTrap) {
		trap_num &= 0x07ff;
	}
	if (trap_num > NumToolboxTraps()) {
		return falseblnr;
	} else
	{
		return NGetTrapAddress(trap_num, type) != NGetTrapAddress(_Unimplemented, ToolTrap);
	}

}
#endif

LOCALVAR blnr MyEnvrAttrAppleEvtMgrAvail;
LOCALVAR blnr gWeHaveHideShowMenu;
LOCALVAR blnr gWeHaveNewWndMgr;
#if AppearanceAvail
LOCALVAR blnr gWeHaveAppearance;
#endif
#if NavigationAvail
LOCALVAR blnr gNavServicesExists;
#endif
#if EnableDragDrop
LOCALVAR blnr gHaveDragMgr;
#endif
#if HaveCPUfamM68K
LOCALVAR blnr MyEnvrAttrFSSpecCallsAvail;
#endif
#if HaveCPUfamM68K
LOCALVAR blnr gHaveSndMngr;
#endif
#if HaveCPUfamM68K
LOCALVAR blnr MyEnvrAttrWaitNextEventAvail;
#endif

#if ! CALL_NOT_IN_CARBON
LOCALVAR CFBundleRef AppServBunRef;
#endif

#if ! CALL_NOT_IN_CARBON
/* SetSystemUIModeProcPtr API always not available */

typedef UInt32                          MySystemUIMode;
typedef OptionBits                      MySystemUIOptions;

enum {
	MykUIModeNormal                 = 0,
	MykUIModeAllHidden              = 3
};

enum {
	MykUIOptionAutoShowMenuBar      = 1 << 0,
	MykUIOptionDisableAppleMenu     = 1 << 2,
	MykUIOptionDisableProcessSwitch = 1 << 3,
	MykUIOptionDisableForceQuit     = 1 << 4,
	MykUIOptionDisableSessionTerminate = 1 << 5,
	MykUIOptionDisableHide          = 1 << 6
};

typedef OSStatus (*SetSystemUIModeProcPtr)
	(MySystemUIMode inMode, MySystemUIOptions inOptions);

LOCALVAR SetSystemUIModeProcPtr MySetSystemUIModeProc = NULL;
#endif

LOCALFUNC blnr InitCheckMyEnvrn(void)
{
	long result;

	MyEnvrAttrAppleEvtMgrAvail = falseblnr;
	gWeHaveHideShowMenu = falseblnr;
	gWeHaveNewWndMgr = falseblnr;
#if AppearanceAvail
	gWeHaveAppearance = falseblnr;
#endif
#if NavigationAvail
	gNavServicesExists = falseblnr;
#endif
#if EnableDragDrop
	gHaveDragMgr = falseblnr;
#endif
#if HaveCPUfamM68K
	MyEnvrAttrFSSpecCallsAvail = falseblnr;
#endif
#if HaveCPUfamM68K
	gHaveSndMngr = falseblnr;
#endif

#if HaveCPUfamM68K
	MyEnvrAttrWaitNextEventAvail = TrapAvailable(_WaitNextEvent);
#endif

#if HaveCPUfamM68K
	if (TrapAvailable(_Gestalt))
#endif
	{
#if HaveCPUfamM68K
		if (Gestalt(gestaltFSAttr, &result) == noErr)
		if (TestBit(result, gestaltHasFSSpecCalls))
		{
			MyEnvrAttrFSSpecCallsAvail = trueblnr;
		}
#endif

		if (Gestalt(gestaltMenuMgrAttr, &result) == 0)
		{
			gWeHaveHideShowMenu = trueblnr;
		}

		if (Gestalt(gestaltWindowMgrAttr, &result) == 0)
		{
			gWeHaveNewWndMgr = trueblnr;
		}

#if AppearanceAvail
		/* Appearance Manager */
		if (Gestalt(gestaltAppearanceAttr, &result) == 0)
		{
			gWeHaveAppearance = trueblnr;
		}
#endif

	/* Navigation */
#if NavigationAvail
		gNavServicesExists = NavServicesAvailable();
#endif

#if EnableDragDrop
		if (Gestalt(gestaltDragMgrAttr, &result) == 0)
		if (TestBit(result, gestaltDragMgrPresent))
		{
			gHaveDragMgr = trueblnr;
		}
#endif

		if (Gestalt(gestaltAppleEventsAttr, &result) == 0)
		if (TestBit(result, gestaltAppleEventsPresent))
		{
			MyEnvrAttrAppleEvtMgrAvail = trueblnr;
		}

#if HaveCPUfamM68K
		gHaveSndMngr = TrapAvailable(_SndNewChannel);
#endif
	}

#if ! CALL_NOT_IN_CARBON
	AppServBunRef = CFBundleGetBundleWithIdentifier(
		CFSTR("com.apple.ApplicationServices"));

	{
		CFBundleRef HIToolboxBunRef = CFBundleGetBundleWithIdentifier(
			CFSTR("com.apple.HIToolbox"));
		if (HIToolboxBunRef != NULL) {
			MySetSystemUIModeProc =
				(SetSystemUIModeProcPtr)
				CFBundleGetFunctionPointerForName(
					HIToolboxBunRef, CFSTR("SetSystemUIMode"));
		}
	}
#endif

	return trueblnr;
}

/*--- utilities for adapting to the environment ---*/

#ifndef Windows85APIAvail
#define Windows85APIAvail 1
#endif

#ifndef MightNotHaveAppearanceMgrAvail
#define MightNotHaveAppearanceMgrAvail CALL_NOT_IN_CARBON
#endif

#ifndef MightNotHaveWindows85Avail
#define MightNotHaveWindows85Avail MightNotHaveAppearanceMgrAvail
#endif

#ifndef LowMemAPIAvail
#define LowMemAPIAvail 1
#endif

#if CALL_NOT_IN_CARBON
#ifndef My_LMGetTime
#if LowMemAPIAvail
#define My_LMGetTime LMGetTime
#else
#define My_LMGetTime() (*(SInt32 *)(0x020C))
#endif
#endif
#endif

#ifndef My_LMGetMBarHeight
#if CALL_NOT_IN_CARBON
#if LowMemAPIAvail
#define My_LMGetMBarHeight LMGetMBarHeight
#else
#define My_LMGetMBarHeight() (*(short *)(0x0BAA))
#endif
#else
#define My_LMGetMBarHeight GetMBarHeight
#endif
#endif

#ifndef My_GetGrayRgn
#if CALL_NOT_IN_CARBON
#if /* LowMemAPIAvail */ 0
#define My_GetGrayRgn LMGetGrayRgn
#else
#define My_GetGrayRgn() (*(RgnHandle *)(0x9EE))
#endif
#else
#define My_GetGrayRgn GetGrayRgn
#endif
#endif

#ifndef My_LMGetCurApName
#if LowMemAPIAvail
#define My_LMGetCurApName LMGetCurApName
#else
#define My_LMGetCurApName() ((StringPtr) 0x0910)
#endif
#endif

#if CALL_NOT_IN_CARBON
#define MyGetScreenBitsBounds(r) (*r) = qd.screenBits.bounds
#else
LOCALPROC MyGetScreenBitsBounds(Rect *r)
{
	BitMap screenBits;

	GetQDGlobalsScreenBits(&screenBits);
	*r = screenBits.bounds;
}
#endif

#if CALL_NOT_IN_CARBON
#define My_GetRegionBounds(region, bounds) *(bounds) = (**(region)).rgnBBox
#else
#define My_GetRegionBounds GetRegionBounds
#endif

#if CALL_NOT_IN_CARBON
#define My_GetPortPixMap(p) ((p)->portPixMap)
#else
#define My_GetPortPixMap GetPortPixMap
#endif

#ifndef My_WindowRef
#if NewNamesAvail
#define My_WindowRef WindowRef
#else
#define My_WindowRef WindowPtr
#endif
#endif

#if CALL_NOT_IN_CARBON
#define My_SetPortWindowPort(w) SetPort(w)
#else
#define My_SetPortWindowPort SetPortWindowPort
#endif

#if CALL_NOT_IN_CARBON
LOCALPROC My_InvalWindowRect(My_WindowRef mw, Rect *r)
{
	GrafPtr SavePort;

	GetPort(&SavePort);
	My_SetPortWindowPort(mw);
	InvalRect(r);
	SetPort(SavePort);
}
#else
#define My_InvalWindowRect InvalWindowRect
#endif

#if CALL_NOT_IN_CARBON
#define My_GetWindowPortBounds(w, r) *(r) = ((w)->portRect)
#else
#define My_GetWindowPortBounds GetWindowPortBounds
#endif

LOCALPROC InvalWholeWindow(My_WindowRef mw)
{
	Rect bounds;

	My_GetWindowPortBounds(mw, &bounds);
	My_InvalWindowRect(mw, &bounds);
}

LOCALPROC MySetMacWindContRect(My_WindowRef mw, Rect *r)
{
#if Windows85APIAvail
	if (gWeHaveNewWndMgr) {
		(void) SetWindowBounds (mw, kWindowContentRgn, r);
	} else
#endif
	{
#if MightNotHaveWindows85Avail
		MoveWindow(mw, r->left, r->top, falseblnr);
		SizeWindow(mw, r->right - r->left, r->bottom - r->top, trueblnr);
#endif
	}
	InvalWholeWindow(mw);
}

LOCALFUNC blnr MyGetWindowTitleBounds(My_WindowRef mw, Rect *r)
{
#if Windows85APIAvail
	if (gWeHaveNewWndMgr) {
		return (noErr == GetWindowBounds(mw,
				kWindowTitleBarRgn, r));
	} else
#endif
	{
#if MightNotHaveWindows85Avail
		My_GetRegionBounds(((WindowPeek)mw)->strucRgn, r);
		r->bottom = r->top + 15;
		r->left += 4;
		r->right -= 4;
#endif
		return trueblnr;
	}
}

#define topLeft(r) (((Point *) &(r))[0])
#define botRight(r) (((Point *) &(r))[1])

LOCALFUNC blnr MyGetWindowContBounds(My_WindowRef mw, Rect *r)
{
#if Windows85APIAvail
	if (gWeHaveNewWndMgr) {
		return (noErr == GetWindowBounds(mw,
				kWindowContentRgn, r));
	} else
#endif
	{
#if MightNotHaveWindows85Avail
		GrafPtr oldPort;
		GetPort(&oldPort);
		My_SetPortWindowPort(mw);
		My_GetWindowPortBounds(mw, r);
		LocalToGlobal(&topLeft(*r));
		LocalToGlobal(&botRight(*r));
		SetPort(oldPort);
#endif
		return trueblnr;
	}
}

LOCALPROC MyGetGrayRgnBounds(Rect *r)
{
	My_GetRegionBounds(My_GetGrayRgn(), (Rect *)r);
}

/*--- sending debugging info to file ---*/

#define MakeDumpFile 0

#if MakeDumpFile

#include <stdio.h>

EXPORTVAR(FILE, *DumpFile)
GLOBALVAR FILE *DumpFile = NULL;

LOCALFUNC blnr StartDump(void)
{
	DumpFile = fopen("DumpFile", "w");
	if (DumpFile == NULL) {
		return falseblnr;
	} else {
		fprintf(DumpFile, "vMac Dump File\n");
		return trueblnr;
	}
}

LOCALPROC EndDump(void)
{
	if (DumpFile != NULL) {
		fclose(DumpFile);
	}
}

EXPORTPROC DumpAJump(CPTR fromaddr, CPTR toaddr);

GLOBALPROC DumpAJump(CPTR fromaddr, CPTR toaddr)
{
	fprintf(DumpFile, "%d,%d\n", fromaddr, toaddr);
}

EXPORTPROC DumpANote(char *s);

GLOBALPROC DumpANote(char *s)
{
	fprintf(DumpFile, s);
	/* fprintf(DumpFile, "at %d\n", m68k_getpc1() - 0x00400000); */
}

#endif

/*--- main window data ---*/

LOCALVAR WindowPtr gMyMainWindow = NULL;

#if EnableFullScreen
LOCALVAR short hOffset;
LOCALVAR short vOffset;
#endif

#if EnableFullScreen
LOCALVAR blnr GrabMachine = falseblnr;
#endif

#if EnableFullScreen
LOCALVAR blnr UseFullScreen = falseblnr;
#endif

#if EnableMagnify
LOCALVAR blnr UseMagnify = falseblnr;
#endif

#define MyWindowScale 2

#if EnableMagnify
LOCALPROC MyScaleRect(Rect *r)
{
	r->left *= MyWindowScale;
	r->right *= MyWindowScale;
	r->top *= MyWindowScale;
	r->bottom *= MyWindowScale;
}
#endif

LOCALPROC SetScrnRectFromCoords(Rect *r, si4b top, si4b left, si4b bottom, si4b right)
{
	r->left = left;
	r->right = right;
	r->top = top;
	r->bottom = bottom;
#if EnableMagnify
	if (UseMagnify) {
		MyScaleRect(r);
	}
#endif
#if EnableFullScreen
	if (UseFullScreen) {
		OffsetRect(r, hOffset, vOffset);
	}
#endif
}

#if EnableMagnify
#define MyScaledHeight (MyWindowScale * vMacScreenHeight)
#define MyScaledWidth (MyWindowScale * vMacScreenWidth)
#endif

#ifndef UseOpenGL
#define UseOpenGL 0 /* (EnableFullScreen && (! CALL_NOT_IN_CARBON)) */
#endif

#define EnableScalingBuff ((1 && EnableMagnify && (MyWindowScale == 2)) || UseOpenGL)

#if EnableScalingBuff
LOCALVAR char *ScalingBuff = nullpr;
#endif

LOCALPROC DefaultDrawScreenBuff(si4b top, si4b left, si4b bottom, si4b right)
{
	BitMap src;
	Rect SrcRect;
	Rect DstRect;

	SrcRect.left = left;
	SrcRect.right = right;
	SrcRect.top = top;
	SrcRect.bottom = bottom;

	src.baseAddr = screencomparebuff;
	src.rowBytes = vMacScreenByteWidth;
	SetRect(&src.bounds, 0, 0, vMacScreenWidth, vMacScreenHeight);
#if EnableScalingBuff && EnableMagnify
	if (UseMagnify) {
		int i;
		int j;
		int k;
		ui5b *p1 = (ui5b *)screencomparebuff + vMacScreenWidth / 32 * top;
		ui5b *p2 = (ui5b *)ScalingBuff + MyWindowScale * MyWindowScale * vMacScreenWidth / 32 * top;
		ui5b *p3;
		ui5b t0;
		ui5b t1;
		ui5b t2;
		ui5b m;

		for (i = bottom - top; --i >= 0; ) {
			p3 = p2;
			for (j = vMacScreenWidth / 32; --j >= 0; ) {
				t0 = *p1++;
				t1 = t0;
				m = 0x80000000;
				t2 = 0;
				for (k = 16; --k >= 0; ) {
					t2 |= t1 & m;
					t1 >>= 1;
					m >>= 2;
				}
				*p2++ = t2 | (t2 >> 1);

				t1 = t0 << 16;
				m = 0x80000000;
				t2 = 0;
				for (k = 16; --k >= 0; ) {
					t2 |= t1 & m;
					t1 >>= 1;
					m >>= 2;
				}
				*p2++ = t2 | (t2 >> 1);
			}
			for (j = MyScaledWidth / 32; --j >= 0; ) {
				*p2++ = *p3++;
			}
		}

		MyScaleRect(&SrcRect);
		MyScaleRect(&src.bounds);

		src.baseAddr = ScalingBuff;
		src.rowBytes *= MyWindowScale;
	}
#endif
	SetScrnRectFromCoords(&DstRect, top, left, bottom, right);
	CopyBits(&src,
#if CALL_NOT_IN_CARBON
		&gMyMainWindow->portBits,
#else
		GetPortBitMapForCopyBits(GetWindowPort(gMyMainWindow)),
#endif
		&SrcRect, &DstRect, srcCopy, NULL);
	/* FrameRect(&SrcRect); for testing */
}

LOCALPROC Update_Screen(void)
{
	GrafPtr savePort;

	GetPort(&savePort);
	My_SetPortWindowPort(gMyMainWindow);
#if EnableFullScreen
	if (UseFullScreen) {
#if CALL_NOT_IN_CARBON
		PaintRect(&gMyMainWindow->portRect);
#else
		{
			Rect pr;
			GetPortBounds(GetWindowPort(gMyMainWindow), &pr);
			PaintRect(&pr);
		}
#endif
	}
#endif
	DefaultDrawScreenBuff(0, 0, vMacScreenHeight, vMacScreenWidth);
	SetPort(savePort);
}

#if 0 /* some experiments */
LOCALVAR CGrafPtr GrabbedPort = NULL;
#endif

#if 0
LOCALPROC AdjustMainScreenGrab(void)
{
	if (GrabMachine) {
		if (GrabbedPort == NULL) {
			/* CGDisplayCapture(CGMainDisplayID()); */
			CGCaptureAllDisplays();
			/* CGDisplayHideCursor( CGMainDisplayID() ); */
			GrabbedPort = CreateNewPortForCGDisplayID((UInt32)kCGDirectMainDisplay);
			LockPortBits (GrabbedPort);
		}
	} else {
		if (GrabbedPort != NULL) {
			UnlockPortBits (GrabbedPort);
			/* CGDisplayShowCursor( CGMainDisplayID() ); */
			/* CGDisplayRelease(CGMainDisplayID()); */
			CGReleaseAllDisplays();
			GrabbedPort = NULL;
		}
	}
}
#endif

#if 0
typedef CGDisplayErr (*CGReleaseAllDisplaysProcPtr)
	(void);

LOCALPROC MyReleaseAllDisplays(void)
{
	if (AppServBunRef != NULL) {
		CGReleaseAllDisplaysProcPtr ReleaseAllDisplaysProc =
			(CGReleaseAllDisplaysProcPtr)
			CFBundleGetFunctionPointerForName(
				AppServBunRef, CFSTR("CGReleaseAllDisplays"));
		if (ReleaseAllDisplaysProc != NULL) {
			ReleaseAllDisplaysProc();
		}
	}
}

typedef CGDisplayErr (*CGCaptureAllDisplaysProcPtr)
	(void);

LOCALPROC MyCaptureAllDisplays(void)
{
	if (AppServBunRef != NULL) {
		CGCaptureAllDisplaysProcPtr CaptureAllDisplaysProc =
			(CGCaptureAllDisplaysProcPtr)
			CFBundleGetFunctionPointerForName(
				AppServBunRef, CFSTR("CGCaptureAllDisplays"));
		if (CaptureAllDisplaysProc != NULL) {
			CaptureAllDisplaysProc();
		}
	}
}
#endif

#if UseOpenGL
LOCALVAR blnr HaveGLsetup = falseblnr;
LOCALVAR AGLPixelFormat fmt;
LOCALVAR AGLContext ctx;
#endif

#if UseOpenGL
LOCALPROC UpdateLuminanceCopy(si4b top, si4b left, si4b bottom, si4b right)
{
	int i;
	int j;
	int k;
	ui3b *p1 = (ui3b *)screencomparebuff + vMacScreenWidth / 8 * top;
	ui3b *p2 = (ui3b *)ScalingBuff + vMacScreenWidth * top;
	ui5b t0;

	UnusedParam(left);
	UnusedParam(right);
	for (i = bottom - top; --i >= 0; ) {
		for (j = vMacScreenWidth / 8; --j >= 0; ) {
			t0 = *p1++;
			for (k = 8; --k >= 0; ) {
				*p2++ = ((t0 >> k) & 0x01) - 1;
			}
		}
	}
}
#endif

#if UseOpenGL
LOCALVAR short GLhOffset;
LOCALVAR short GLvOffset;
#endif

#if UseOpenGL
LOCALPROC MyDrawWithOpenGL(si4b top, si4b left, si4b bottom, si4b right)
{
	if (GL_TRUE != aglSetCurrentContext(ctx)) {
		/* err = aglReportError() */
	} else {
#if 0
		glClear(GL_COLOR_BUFFER_BIT);
		glBitmap(vMacScreenWidth,
			vMacScreenHeight,
			0,
			0,
			0,
			0,
			(const GLubyte *)screencomparebuff);
#endif
#if 1
		UpdateLuminanceCopy(top, left, bottom, right);
		{
			si4b top2 = top;
			si4b left2 = left;

#if EnableMagnify
			if (UseMagnify) {
				top2 *= MyWindowScale;
				left2 *= MyWindowScale;
			}
#endif
			glRasterPos2i(GLhOffset + left2, GLvOffset - top2);
			glDrawPixels(vMacScreenWidth,
				bottom - top,
				GL_LUMINANCE,
				GL_UNSIGNED_BYTE,
				ScalingBuff + top * vMacScreenWidth);
		}
#endif
		glFlush();
	}
}
#endif

#if UseOpenGL
LOCALPROC MyAdjustGLforSize(void)
{
	if (GL_TRUE != aglSetCurrentContext(ctx)) {
		/* err = aglReportError() */
	} else {
		Rect AllScrnBounds = (*GetMainDevice())->gdRect;
		int h = AllScrnBounds.right - AllScrnBounds.left;
		int v = AllScrnBounds.bottom - AllScrnBounds.top;
		short NewWindowHeight = vMacScreenHeight;
		short NewWindowWidth = vMacScreenWidth;

#if EnableMagnify
		if (WantMagnify) {
			NewWindowHeight *= MyWindowScale;
			NewWindowWidth *= MyWindowScale;
		}
#endif

		glClearColor (0.0, 0.0, 0.0, 1.0);

#if 1
		glViewport(0, 0, h, v);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, h, 0, v, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
#endif

		glColor3f(0.0, 0.0, 0.0);
#if EnableMagnify
		if (UseMagnify) {
			glPixelZoom(MyWindowScale, - MyWindowScale);
		} else
#endif
		{
			glPixelZoom(1, -1);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		if (GL_TRUE != aglSetCurrentContext(NULL)) {
			/* err = aglReportError() */
		}

		GLhOffset = h < NewWindowWidth ? 0 :
			(h - NewWindowWidth) / 2;
		GLvOffset = v < NewWindowHeight ? v :
			v - ((v - NewWindowHeight) / 2);

		MyDrawWithOpenGL(0, 0, vMacScreenHeight, vMacScreenWidth);
	}
}
#endif

#if UseOpenGL
LOCALPROC MyUnsetupGL(void)
{
	if (HaveGLsetup) {
		if (gMyMainWindow != NULL) {
			Update_Screen();
#if ! CALL_NOT_IN_CARBON
			QDFlushPortBuffer(GetWindowPort(gMyMainWindow), NULL);
#endif
				/* otherwise, will see image from last draw without OpenGL */
		}

		if (GL_TRUE != aglSetCurrentContext(NULL)) {
			/* err = aglReportError() */
		}
		/* MyReleaseAllDisplays(); */
		if (GL_TRUE != aglDestroyContext(ctx)) {
			/* err = aglReportError() */
		}
		aglDestroyPixelFormat (fmt);
		HaveGLsetup = falseblnr;
	}
}
#endif

#if UseOpenGL
LOCALPROC MySetupGL(void)
{
	if (! HaveGLsetup) {
		static const GLint attrib[] = {AGL_RGBA,
			AGL_NO_RECOVERY,
			AGL_FULLSCREEN,
			AGL_SINGLE_RENDERER, AGL_ACCELERATED,
			/* AGL_DOUBLEBUFFER, */ AGL_NONE};
		AGLDevice theDevice = GetMainDevice();

		fmt = aglChoosePixelFormat(&theDevice, 1, attrib);
		if (NULL == fmt) {
			/* err = aglReportError() */
		} else {
			ctx = aglCreateContext(fmt, NULL);
			if (NULL == ctx) {
				/* err = aglReportError() */
			} else {
				/* MyCaptureAllDisplays(); */
				if (GL_TRUE != aglSetFullScreen(ctx, 0, 0, 0, 0))
				{
					/* err = aglReportError() */
				} else {
					HaveGLsetup = trueblnr;
					return;
				}
				/* MyReleaseAllDisplays(); */

				if (GL_TRUE != aglDestroyContext(ctx)) {
					/* err = aglReportError() */
				}
			}

			aglDestroyPixelFormat (fmt);
		}
	}
}
#endif

#if UseOpenGL
LOCALPROC AdjustOpenGLGrab(void)
{
	if (GrabMachine) {
		MySetupGL();
	} else {
		MyUnsetupGL();
	}
	if (HaveGLsetup) {
		MyAdjustGLforSize();
	}
}
#endif

GLOBALPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right)
{
#if UseOpenGL
	if (HaveGLsetup) {
		MyDrawWithOpenGL(top, left, bottom, right);
	} else
#endif
#if 0 /* experimental code in progress */
#if CALL_NOT_IN_CARBON
	if (UseFullScreen)
#else
	if (QDIsPortBuffered(GetWindowPort(gMyMainWindow)))
#endif
	{
#if ! CALL_NOT_IN_CARBON
		CGrafPtr p = GetWindowPort(gMyMainWindow);
		/* LockPortBits((GrafPtr)p); */
#endif
		{
#if CALL_NOT_IN_CARBON
			PixMapHandle pm= (**GetMainDevice()).gdPMap;
#else
			PixMapHandle pm = My_GetPortPixMap(p);
#endif

			/* LockPixels(pm); */
#if EnableMagnify
			if (! UseMagnify) {
#define PixelT ui5b
				PixelT *p1 = (PixelT *)GetPixBaseAddr(pm);
				int i;
				int j;
				int k;
				ui5b *p0 = (ui5b *)screencomparebuff;
				ui5b SkipBytes = GetPixRowBytes(pm) - sizeof(PixelT) * vMacScreenWidth;
				ui5b t0;
				PixelT a[2];

				((Ptr)p1) += (long)GetPixRowBytes(pm) * (top + vOffset);
				p1 += hOffset;
				p0 += (long)top * vMacScreenWidth / 32;

				a[0] = (PixelT) -1;
				a[1] = 0;

#if 1
				for (i = bottom - top; --i >= 0; ) {
					for (j = vMacScreenWidth / 32; --j >= 0; ) {
						t0 = *p0++;

						for (k = 32; --k >= 0; ) {
							PixelT v = a[(t0 >> k) & 1];
							*p1++ = v;
						}
					}
					((Ptr)p1) += SkipBytes;
				}
#endif
			} else {
#define PixelT ui5b
				PixelT *p1 = (PixelT *)GetPixBaseAddr(pm);
				int i;
				int j;
				int k;
				ui5b *p0 = (ui5b *)screencomparebuff;
				PixelT *p2;
				ui5b t0;
				PixelT a[2];

				p1 += vOffset * MyScaledWidth;
				p1 += (long)MyWindowScale * (long)MyScaledWidth * top;
				p0 += (long)top * vMacScreenWidth / 32;

				a[0] = (PixelT) -1;
				a[1] = 0;

#if 1
				for (i = bottom - top; --i >= 0; ) {
					p2 = p1;
					for (j = vMacScreenWidth / 32; --j >= 0; ) {
						t0 = *p0++;

						for (k = 32; --k >= 0; ) {
							PixelT v = a[(t0 >> k) & 1] /* ((t0 >> k) & 1) - 1 */;
							*p1++ = v;
							*p1++ = v;
						}
					}
					for (j = MyScaledWidth; --j >= 0; ) {
						*p1++ = *p2++;
					}
				}
#endif
			}
#endif
			/* UnlockPixels(pm); */
		}
#if ! CALL_NOT_IN_CARBON
		/* UnlockPortBits((GrafPtr)p); */
#if 1
		{
			static RgnHandle Rc = NULL;

			if (Rc == NULL) {
				Rc = NewRgn();
			}
			if (Rc != NULL) {
				Rect DstRect;
				SetScrnRectFromCoords(&DstRect, top, left, bottom, right);
				RectRgn(Rc, &DstRect);
				QDFlushPortBuffer(p, Rc);
				/* QDAddRectToDirtyRegion(p, &DstRect); */
			}
		}
#endif
#endif
	} else
#endif
	{
		GrafPtr savePort;

		GetPort(&savePort);
		My_SetPortWindowPort(gMyMainWindow);
		DefaultDrawScreenBuff(top, left, bottom, right);
		SetPort(savePort);
	}
}

/*--- keyboard ---*/

#if ! UseCarbonEvents
LOCALFUNC blnr Keyboard_TestKeyMap(int key)
{
	if ((key >= 0) && (key < 128)) {
		ui3b *kp = (ui3b *)theKeys;
		return (kp[key / 8] & (1 << (key & 7))) != 0;
	} else {
		return falseblnr;
	}
}
#endif

#if ! UseCarbonEvents
#define ClearOneKey(key) ((ui3b *)theKeys)[(key) / 8] &= ~ (1 << ((key) & 7))
#define SetOneKey(key) ((ui3b *)theKeys)[(key) / 8] |= (1 << ((key) & 7))
#endif

#if UseCarbonEvents
LOCALPROC Keyboard_UpdateKeyMap(int key, blnr down)
{
	ui3b *kp = (ui3b *)theKeys;

	if (key >= 0 && key < 128) {
		int bit = 1 << (key & 7);
		if (down) {
			kp[key / 8] |= bit;
		} else {
			kp[key / 8] &= ~ bit;
		}
	}
}
#endif

#if UseCarbonEvents
LOCALVAR UInt32 SavedModifiers = 0;
#endif

#if UseCarbonEvents
LOCALPROC InitKeyCodes(void)
{
	theKeys[0] = 0;
	theKeys[1] = 0;
	theKeys[2] = 0;
	theKeys[3] = 0;

	SavedModifiers = GetCurrentKeyModifiers();
		/*
			Perhaps should use GetCurrentEventKeyModifiers,
			but not available in old os x versions.
		*/
}
#endif

#if UseCarbonEvents
LOCALPROC Keyboard_UpdateKeyMap2(int key, blnr down)
{
	if (MKC_F1 == key) {
		key = MKC_Option;
	} else if (MKC_F2 == key) {
		key = MKC_Command;
	}
	Keyboard_UpdateKeyMap(key, down);
}
#endif

#if UseCarbonEvents
LOCALPROC MyUpdateKeyboardModifiers(UInt32 theModifiers)
{
	UInt32 ChangedModifiers = theModifiers ^ SavedModifiers;

	if (ChangedModifiers & shiftKey) {
		Keyboard_UpdateKeyMap(MKC_Shift, (shiftKey & theModifiers) != 0);
	}
	if (ChangedModifiers & cmdKey) {
		Keyboard_UpdateKeyMap(MKC_Command, (cmdKey & theModifiers) != 0);
	}
	if (ChangedModifiers & alphaLock) {
		Keyboard_UpdateKeyMap(MKC_CapsLock, (alphaLock & theModifiers) != 0);
	}
	if (ChangedModifiers & optionKey) {
		Keyboard_UpdateKeyMap(MKC_Option, (optionKey & theModifiers) != 0);
	}
	if (ChangedModifiers & controlKey) {
		Keyboard_UpdateKeyMap(MKC_Control, (controlKey & theModifiers) != 0);
	}
	SavedModifiers = theModifiers;
}
#endif

#if ! UseCarbonEvents
LOCALPROC CheckKeyBoardState(void)
{
	GetKeys(*(KeyMap *)theKeys);
	if (Keyboard_TestKeyMap(MKC_F1)) {
		ClearOneKey(MKC_F1);
		SetOneKey(MKC_Option);
	}
	if (Keyboard_TestKeyMap(MKC_F2)) {
		ClearOneKey(MKC_F2);
		SetOneKey(MKC_Command);
	}
}
#endif

/*--- cursor hiding ---*/

LOCALVAR blnr HaveCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		ShowCursor();
	}
}

/*--- cursor moving ---*/

LOCALPROC SetCursorArror(void)
{
#if CALL_NOT_IN_CARBON
	SetCursor(&qd.arrow);
#else
	Cursor c;

	GetQDGlobalsArrow(&c);
	SetCursor(&c);
#endif
}

/*
	mouse moving code (non OS X) adapted from
	MoveMouse.c by Dan Sears, which says that
	"Based on code from Jon Wtte, Denis Pelli,
	Apple, and a timely suggestion from Bo Lindbergh."
	Also says 'For documentation of the CDM, see Apple
	Tech Note "HW 01 - ADB (The Untold Story: Space Aliens
	ate my mouse)"'
*/

#ifndef TARGET_CPU_PPC
#error "TARGET_CPU_PPC undefined"
#endif

#if CALL_NOT_IN_CARBON
#if TARGET_CPU_PPC
enum {
	glueUppCursorDeviceMoveToProcInfo =
		kD0DispatchedPascalStackBased |
		DISPATCHED_STACK_ROUTINE_SELECTOR_SIZE(kTwoByteCode) |
		RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
		DISPATCHED_STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(CursorDevicePtr))) |
		DISPATCHED_STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(long))) |
		DISPATCHED_STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(long))),
	glueUppCursorDeviceNextDeviceProcInfo =
		kD0DispatchedPascalStackBased |
		DISPATCHED_STACK_ROUTINE_SELECTOR_SIZE(kTwoByteCode) |
		RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
		DISPATCHED_STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(CursorDevicePtr *)))
};
#endif
#endif

#if CALL_NOT_IN_CARBON
#if TARGET_CPU_PPC
LOCALFUNC OSErr
CallCursorDeviceMoveTo(
	CursorDevicePtr ourDevice,
	long absX,
	long absY)
{
	return CallUniversalProc(
		GetToolboxTrapAddress(_CursorDeviceDispatch),
		glueUppCursorDeviceMoveToProcInfo,
		1, ourDevice, absX, absY);
}
#else
#define CallCursorDeviceMoveTo CursorDeviceMoveTo
#endif
#endif

#if CALL_NOT_IN_CARBON
#if TARGET_CPU_PPC
LOCALFUNC OSErr
CallCursorDeviceNextDevice(
	CursorDevicePtr *ourDevice)
{
	return CallUniversalProc(
		GetToolboxTrapAddress(_CursorDeviceDispatch),
		glueUppCursorDeviceNextDeviceProcInfo,
		0xB, ourDevice);
}
#else
#define CallCursorDeviceNextDevice CursorDeviceNextDevice
#endif
#endif

#if CALL_NOT_IN_CARBON
#if ! TARGET_CPU_PPC
pascal void CallCursorTask(void) =
{
	0x2078, 0x08EE,  /* MOVE.L jCrsrTask,A0 */
	0x4E90           /* JSR (A0) */
};
#endif
#endif

/*
	Low memory globals for the mouse
*/

#define MyRawMouse   0x082C  /* low memory global that has current mouse loc */
#define MyMTemp      0x0828  /* low memory global that has current mouse loc */
#define MyCrsrNew    0x08CE  /* set after you change mtemp and rawmouse */
#define MyCrsrCouple 0x08CF  /* true if the cursor is tied to the mouse */

#if 0 && (! CALL_NOT_IN_CARBON)
typedef CGDisplayErr (*CGDisplayMoveCursorToPointProcPtr)
	(CGDirectDisplayID display, CGPoint point);
LOCALVAR CGDisplayMoveCursorToPointProcPtr MyMoveCursorToPointProc = NULL;
#endif

#if ! CALL_NOT_IN_CARBON
typedef CGEventErr
(*CGSetLocalEventsSuppressionIntervalProcPtr) (CFTimeInterval seconds);
LOCALVAR CGSetLocalEventsSuppressionIntervalProcPtr MySetLocalEventsSuppressionIntervalProc = NULL;

typedef CGEventErr
(*CGWarpMouseCursorPositionProcPtr) (CGPoint newCursorPosition);
LOCALVAR CGWarpMouseCursorPositionProcPtr MyWarpMouseCursorPositionProc = NULL;
#endif

#if EnableMouseMotion
LOCALFUNC blnr MyMoveMouse(si4b h, si4b v)
{
	GrafPtr oldPort;
	Point CurMousePos;
	Point NewMousePos;
	ui5b difftime;
	blnr IsOk;
	long int StartTime = TickCount();

#if EnableMagnify
	if (UseMagnify) {
		h *= MyWindowScale;
		v *= MyWindowScale;
	}
#endif
#if EnableFullScreen
	if (UseFullScreen) {
		h += hOffset;
		v += vOffset;
	}
#endif
	CurMousePos.h = h;
	CurMousePos.v = v;

	GetPort(&oldPort);
	My_SetPortWindowPort(gMyMainWindow);
	LocalToGlobal(&CurMousePos);

	do {

#if CALL_NOT_IN_CARBON
		if (TrapAvailable(_CursorDeviceDispatch)) {
			CursorDevice *firstMouse = NULL;
			CallCursorDeviceNextDevice(&firstMouse);
			if (firstMouse != NULL) {
				CallCursorDeviceMoveTo(firstMouse,
					(long) CurMousePos.h,
					(long) CurMousePos.v);
			}
		} else {
			*(Point *)MyRawMouse = CurMousePos;
			*(Point *)MyMTemp = CurMousePos;
			*(Ptr)MyCrsrNew = *(Ptr)MyCrsrCouple;
#if ! TARGET_CPU_PPC
			CallCursorTask();
#endif
		}
#else
		{
			if (MyWarpMouseCursorPositionProc == NULL) {
				if (AppServBunRef != NULL) {
#if 0
					MyMoveCursorToPointProc =
						(CGDisplayMoveCursorToPointProcPtr)
						CFBundleGetFunctionPointerForName(
							AppServBunRef, CFSTR("CGDisplayMoveCursorToPoint"));
#endif
					MySetLocalEventsSuppressionIntervalProc =
						(CGSetLocalEventsSuppressionIntervalProcPtr)
						CFBundleGetFunctionPointerForName(
							AppServBunRef, CFSTR("CGSetLocalEventsSuppressionInterval"));
					MyWarpMouseCursorPositionProc =
						(CGWarpMouseCursorPositionProcPtr)
						CFBundleGetFunctionPointerForName(
							AppServBunRef, CFSTR("CGWarpMouseCursorPosition"));
				}
			}
			/* This method from SDL_QuartzWM.m, "Simple DirectMedia Layer", Copyright (C) 1997-2003 Sam Lantinga */
			if (MySetLocalEventsSuppressionIntervalProc == 0) {
				/* don't use MacMsg which can call MyMoveMouse */
			} else {
				if (MySetLocalEventsSuppressionIntervalProc(0.0) != 0) {
					/* don't use MacMsg which can call MyMoveMouse */
				}
			}
			if (MyWarpMouseCursorPositionProc == 0) {
				/* don't use MacMsg which can call MyMoveMouse */
			} else {
				CGPoint pt;
				pt.x = CurMousePos.h;
				pt.y = CurMousePos.v;
				if (MyWarpMouseCursorPositionProc(pt) != 0) {
					/* don't use MacMsg which can call MyMoveMouse */
				}
			}
#if 0
			if (MyMoveCursorToPointProc != NULL) {
				CGPoint pt;
				pt.x = CurMousePos.h;
				pt.y = CurMousePos.v;
				/* MyMoveCursorToPointProc(NULL, pt); */
				if (MyMoveCursorToPointProc(kCGDirectMainDisplay, pt) != 0) {
					/* don't use MacMsg which can call MyMoveMouse */
				}
#if 0
				if (CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, pt) != 0) {
					/* don't use MacMsg which can call MyMoveMouse */
				}
#endif
			}
#endif
		}
#endif

		GetMouse(&NewMousePos);
		IsOk = (h == NewMousePos.h) && (v == NewMousePos.v);
		difftime = (ui5b)(TickCount() - StartTime);
	} while ((! IsOk) && (difftime < 5));

	SetPort(oldPort);
	return IsOk;
}
#endif

#if EnableMouseMotion
LOCALVAR si4b SavedMouseH;
LOCALVAR si4b SavedMouseV;
#endif

#if EnableMouseMotion
LOCALPROC AdjustMouseMotionGrab(void)
{
	if (gMyMainWindow != NULL) {
		if (GrabMachine) {
			/*
				if magnification changes, need to reset,
				even if HaveMouseMotion already true
			*/
			if (MyMoveMouse(vMacScreenWidth / 2, vMacScreenHeight / 2)) {
				SavedMouseH = vMacScreenWidth / 2;
				SavedMouseV = vMacScreenHeight / 2;
				HaveMouseMotion = trueblnr;
			}
		} else {
			if (HaveMouseMotion) {
				(void) MyMoveMouse(CurMouseH, CurMouseV);
				HaveMouseMotion = falseblnr;
			}
		}
	}
}
#endif

#if ! UseCarbonEvents
LOCALVAR blnr CurTrueMouseButton = falseblnr;
#endif

LOCALPROC CheckMouseState(void)
{
	blnr ShouldHaveCursorHidden;
#if ! UseCarbonEvents
	ui3b NewMouseButton;
#endif
	Point NewMousePos;
	GrafPtr oldPort;

	ShouldHaveCursorHidden = trueblnr;

	GetPort(&oldPort);
	My_SetPortWindowPort(gMyMainWindow);
	GetMouse(&NewMousePos);

#if EnableFullScreen
	if (UseFullScreen) {
		NewMousePos.h -= hOffset;
		NewMousePos.v -= vOffset;
	} else
#endif
	{
#if CALL_NOT_IN_CARBON
		if (! PtInRgn(NewMousePos, gMyMainWindow->visRgn)) {
			ShouldHaveCursorHidden = falseblnr;
		}
#else
		if (IsWindowCollapsed(gMyMainWindow)) {
			ShouldHaveCursorHidden = falseblnr;
		}
#if 0 /* this works, but too slow */
		WindowPtr whichWindow;
		Point GlobalMousePos = NewMousePos;
		LocalToGlobal(&GlobalMousePos);
		if ((FindWindow(GlobalMousePos, &whichWindow) != inContent)
			|| (whichWindow != gMyMainWindow))
		{
			ShouldHaveCursorHidden = falseblnr;
		}
#endif
#if 0 /* none of this helps in os x */
		RgnHandle TestRgn = NewRgn();
		if (TestRgn != NULL) {
			/* GetPortVisibleRegion(GetWindowPort(gMyMainWindow), TestRgn); */
			Point GlobalMousePos = NewMousePos;
			LocalToGlobal(&GlobalMousePos);
			(void) GetWindowRegion(gMyMainWindow, /* kWindowContentRgn */ /* kWindowStructureRgn */ kWindowOpaqueRgn, TestRgn);
			if (! PtInRgn(GlobalMousePos, TestRgn)) {
				ShouldHaveCursorHidden = falseblnr;
			}
			DisposeRgn(TestRgn);
		}
#endif
#endif
	}

	SetPort(oldPort);

#if EnableMagnify
	if (UseMagnify) {
		NewMousePos.h /= MyWindowScale;
		NewMousePos.v /= MyWindowScale;
	}
#endif

#if EnableMouseMotion
	if (HaveMouseMotion) {
		si4b shiftdh;
		si4b shiftdv;

		MouseMotionH += NewMousePos.h - SavedMouseH;
		MouseMotionV += NewMousePos.v - SavedMouseV;
		if (NewMousePos.h < vMacScreenWidth / 4) {
			shiftdh = vMacScreenWidth / 2;
		} else if (NewMousePos.h > vMacScreenWidth - vMacScreenWidth / 4) {
			shiftdh = - vMacScreenWidth / 2;
		} else {
			shiftdh = 0;
		}
		if (NewMousePos.v < vMacScreenHeight / 4) {
			shiftdv = vMacScreenHeight / 2;
		} else if (NewMousePos.v > vMacScreenHeight - vMacScreenHeight / 4) {
			shiftdv = - vMacScreenHeight / 2;
		} else {
			shiftdv = 0;
		}
		if ((shiftdh != 0) || (shiftdv != 0)) {
			NewMousePos.h += shiftdh;
			NewMousePos.v += shiftdv;
			if (! MyMoveMouse(NewMousePos.h, NewMousePos.v)) {
				HaveMouseMotion = falseblnr;
			}
		}
		SavedMouseH = NewMousePos.h;
		SavedMouseV = NewMousePos.v;
	} else
#endif
	{
		if (NewMousePos.h < 0) {
			NewMousePos.h = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePos.h >= vMacScreenWidth) {
			NewMousePos.h = vMacScreenWidth - 1;
			ShouldHaveCursorHidden = falseblnr;
		}
		if (NewMousePos.v < 0) {
			NewMousePos.v = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePos.v >= vMacScreenHeight) {
			NewMousePos.v = vMacScreenHeight - 1;
			ShouldHaveCursorHidden = falseblnr;
		}

#if EnableFullScreen
		if (UseFullScreen) {
			ShouldHaveCursorHidden = trueblnr;
		}
#endif

		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/* for a game like arkanoid, would like mouse to still
		move even when outside window in one direction */
		{
			CurMouseV = NewMousePos.v;
			CurMouseH = NewMousePos.h;
		}
	}

#if ! UseCarbonEvents
	NewMouseButton = Button();

	if (CurTrueMouseButton != NewMouseButton) {
		CurTrueMouseButton = NewMouseButton;
		CurMouseButton = CurTrueMouseButton && ShouldHaveCursorHidden;
		/*
			CurMouseButton changes only when the button state changes.
			So if have mouse down outside our window, CurMouseButton will
			stay false even if mouse dragged back over our window.
			and if mouse down inside our window, CurMouseButton will
			stay true even if mouse dragged outside our window.
		*/
	}
#endif

	if (HaveCursorHidden != ShouldHaveCursorHidden) {
		HaveCursorHidden = ShouldHaveCursorHidden;
		if (HaveCursorHidden) {
			HideCursor();
		} else {
#if ! CALL_NOT_IN_CARBON
			/*
				kludge for OS X, where mouse over Dock devider
				changes cursor, and never sets it back.
			*/
			SetCursorArror();
#endif
			ShowCursor();
		}
	}
}


/*--- time, date, location ---*/

/*
	be sure to avoid getting confused if TickCount
	overflows and wraps.
*/

LOCALVAR ui5b TrueEmulatedTime = 0;
LOCALVAR ui5b CurEmulatedTime = 0;

#if ! UseCarbonEvents
LOCALVAR long int LastTime;
#endif

#if UseCarbonEvents
LOCALVAR EventTime NextTickChangeTime;
#endif

#if UseCarbonEvents
#define MyTickDuration (kEventDurationSecond / 60.14742)
#endif

LOCALFUNC blnr InitLocationDat(void)
{
	MachineLocation loc;

	ReadLocation(&loc);
	CurMacLatitude = (ui5b)loc.latitude;
	CurMacLongitude = (ui5b)loc.longitude;
	CurMacDelta = (ui5b)loc.u.gmtDelta;

	return trueblnr;
}

LOCALPROC StartUpTimeAdjust(void)
{
#if UseCarbonEvents
	NextTickChangeTime = GetCurrentEventTime() + MyTickDuration;
#else
	LastTime = TickCount();
#endif
}

LOCALPROC UpdateTrueEmulatedTime(void)
{
#if UseCarbonEvents
	EventTime LatestTime = GetCurrentEventTime();
	EventTime TimeDiff = LatestTime - NextTickChangeTime;

	if (TimeDiff >= 0.0) {
		if (TimeDiff > 3 * MyTickDuration) {
			/* emulation interrupted, forget it */
			++TrueEmulatedTime;
			NextTickChangeTime = LatestTime + MyTickDuration;
		} else {
			do {
				++TrueEmulatedTime;
				TimeDiff -= MyTickDuration;
				NextTickChangeTime += MyTickDuration;
			} while (TimeDiff >= 0.0);
		}
	}
#else
	long int LatestTime = TickCount();
	ui5b TimeDiff = LatestTime - LastTime;

	if (TimeDiff != 0) {
		LastTime = LatestTime;

		if (TimeDiff > 4) {
			/* emulation interrupted, forget it */
			++TrueEmulatedTime;
		} else {
			TrueEmulatedTime += TimeDiff;
		}
	}
#endif
}

GLOBALFUNC blnr ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	TimeAdjust = TrueEmulatedTime - CurEmulatedTime;
	return TimeAdjust < 0;
}

LOCALFUNC blnr CheckDateTime(void)
{
	ui5b NewMacDateInSecond;

#if CALL_NOT_IN_CARBON
	NewMacDateInSecond = My_LMGetTime();
#else
	unsigned long secs;

	GetDateTime(&secs);
	NewMacDateInSecond = secs;
#endif
	if (CurMacDateInSeconds != NewMacDateInSecond) {
		CurMacDateInSeconds = NewMacDateInSecond;
		return trueblnr;
	} else {
		return falseblnr;
	}
}

/*--- sound ---*/

/*
	Some of this code descended from CarbonSndPlayDB, an
	example from Apple, as found being used in vMac for Mac OS.
*/

#if MySoundEnabled

#define SOUND_SAMPLERATE rate22khz /* = 0x56EE8BA3 = (7833600 * 2 / 704) << 16 */

#define kLn2SoundBuffers 4 /* kSoundBuffers must be a power of two */
#define kSoundBuffers (1 << kLn2SoundBuffers)
#define kSoundBuffMask (kSoundBuffers - 1)

#define DesiredMinFilledSoundBuffs 3
	/*
		if too big then sound lags behind emulation.
		if too small then sound will have pauses.
	*/

#define dbhBufferSize (kSoundBuffers * SOUND_LEN)

#define PrintSoundBuffStats (0 && MakeDumpFile)

/* Structs */
struct PerChanInfo {
	volatile ui4b  CurPlayBuffer;
	volatile ui4b  CurFillBuffer;
	volatile ui4b  MinFilledSoundBuffs;
#if PrintSoundBuffStats
	volatile ui4b  MaxFilledSoundBuffs;
#endif
	volatile blnr  stopping;
	Ptr            dbhBufferPtr;
	CmpSoundHeader soundHeader;
};
typedef struct PerChanInfo   PerChanInfo;
typedef struct PerChanInfo * PerChanInfoPtr;

LOCALPROC InsertSndDoCommand (SndChannelPtr chan, SndCommand * newCmd)
{
	if (-1 == chan->qHead) {
		chan->qHead = chan->qTail;
	}

	if (1 <= chan->qHead) {
		chan->qHead--;
	} else {
		chan->qHead = chan->qTail;
	}

	chan->queue[chan->qHead] = *newCmd;
}

LOCALPROC FillWithSilence(ui3p p, int n)
{
	int i;

	for (i = n; --i >= 0; ) {
		*p++ = 0x80 /* 0 */;
	}
}

#if 0
LOCALPROC RampSound(ui3p p, int n, ui3b BeginVal, ui3b EndVal)
{
	int i;

	for (i = n; --i >= 0; ) {
		*p++ = EndVal + (i * ((si5b)BeginVal - (si5b)EndVal)) / n;
	}
}
#endif

/* call back */ static pascal void MySound_CallBack (SndChannelPtr theChannel, SndCommand * theCallBackCmd)
{
	PerChanInfoPtr perChanInfoPtr;
	SndCommand     playCmd;

	perChanInfoPtr = (PerChanInfoPtr)(theCallBackCmd->param2);
	if (! perChanInfoPtr->stopping) {
		ui5b NextPlayOffset;
		ui4b CurPlayBuff = perChanInfoPtr->CurPlayBuffer;
		ui4b NextPlayBuffer = CurPlayBuff + 1;
		ui4b FilledSoundBuffs = perChanInfoPtr->CurFillBuffer - NextPlayBuffer;

		if (FilledSoundBuffs < perChanInfoPtr->MinFilledSoundBuffs) {
			perChanInfoPtr->MinFilledSoundBuffs = FilledSoundBuffs;
		}
		if (FilledSoundBuffs == 0) {
			/* out of sound to play. play a bit of silence */
			NextPlayOffset = (CurPlayBuff & kSoundBuffMask) * SOUND_LEN;
			perChanInfoPtr->soundHeader.samplePtr = (Ptr)perChanInfoPtr->dbhBufferPtr + NextPlayOffset;
			FillWithSilence((ui3p)perChanInfoPtr->soundHeader.samplePtr, SOUND_LEN);
		} else {
			NextPlayOffset = (NextPlayBuffer & kSoundBuffMask) * SOUND_LEN;
			perChanInfoPtr->CurPlayBuffer = NextPlayBuffer;
			perChanInfoPtr->soundHeader.samplePtr = (Ptr)perChanInfoPtr->dbhBufferPtr + NextPlayOffset;
		}

		perChanInfoPtr->soundHeader.numFrames = (unsigned long)SOUND_LEN;

		/* Insert our callback command */
		InsertSndDoCommand (theChannel, theCallBackCmd);

		/* Play the next buffer */
		playCmd.cmd = bufferCmd;
		playCmd.param1 = 0;
		playCmd.param2 = (long)&(perChanInfoPtr->soundHeader);
		InsertSndDoCommand (theChannel, &playCmd);
	}
}

LOCALVAR PerChanInfoPtr TheperChanInfoPtr = NULL;

LOCALVAR SndCallBackUPP gCarbonSndPlayDoubleBufferCallBackUPP = NULL;

LOCALVAR SndChannelPtr sndChannel = NULL; /* our sound channel */

LOCALPROC MySound_Start(void)
{
	if (sndChannel == NULL) {
#if HaveCPUfamM68K
		if (gHaveSndMngr)
#endif
		{
			SndChannelPtr  chan = NULL;
			SndCommand     callBack;

			SndNewChannel(&chan, sampledSynth, initMono, nil);
			if (chan != NULL) {
				sndChannel = chan;

				TheperChanInfoPtr->stopping = falseblnr;

				callBack.cmd = callBackCmd;
				callBack.param1 = 0; /* unused */
				callBack.param2 = (long)TheperChanInfoPtr;

				chan->callBack = gCarbonSndPlayDoubleBufferCallBackUPP;

				(void) SndDoCommand (chan, &callBack, true);
			}
		}
	}
}

#define IgnorableEventMask (mUpMask | keyDownMask | keyUpMask | autoKeyMask)

LOCALPROC MySound_Stop(void)
{
	if (sndChannel != NULL) {
#if 1
/*
		this may not be necessary, but try to clean up
		in case it might help prevent problems.
*/
		if (TheperChanInfoPtr != NULL) {
			SCStatus r;
			blnr busy = falseblnr;

			TheperChanInfoPtr->stopping = trueblnr;
			r.scChannelBusy = falseblnr;
			do {
				if (0 == SndChannelStatus(sndChannel,
					sizeof(SCStatus), &r))
				{
					busy = r.scChannelBusy;
				}
				if (busy) {
					/*
						give time back, particularly important
						if got here on a suspend event.
					*/
					EventRecord theEvent;

#if HaveCPUfamM68K
					if (! MyEnvrAttrWaitNextEventAvail) {
						(void) GetNextEvent(IgnorableEventMask, &theEvent);
					} else
#endif
					{
						(void) WaitNextEvent(IgnorableEventMask, &theEvent, 1, NULL);
					}
				}
			} while (busy);
		}
#endif
		SndDisposeChannel(sndChannel, true);
		sndChannel = NULL;
	}
}

LOCALFUNC blnr MySound_Init(void)
{
	PerChanInfoPtr perChanInfoPtr;
	Ptr buffer;

	gCarbonSndPlayDoubleBufferCallBackUPP = NewSndCallBackUPP(MySound_CallBack);
	if (gCarbonSndPlayDoubleBufferCallBackUPP != NULL) {
		perChanInfoPtr = (PerChanInfoPtr) NewPtr(sizeof(PerChanInfo));
		if (perChanInfoPtr != NULL) {
			buffer = (Ptr) NewPtr(dbhBufferSize);
			if (buffer != NULL) {
				FillWithSilence((ui3p)buffer, dbhBufferSize);
				/* RampSound((ui3p)buffer, dbhBufferSize, 0x80, 0x00); */
				perChanInfoPtr->dbhBufferPtr = buffer;

				perChanInfoPtr->CurPlayBuffer = 0;
				perChanInfoPtr->CurFillBuffer = kSoundBuffers - 1;
				perChanInfoPtr->MinFilledSoundBuffs = kSoundBuffers;
#if PrintSoundBuffStats
				perChanInfoPtr->MaxFilledSoundBuffs = 0;
#endif

				/* Init basic per channel information */
				perChanInfoPtr->soundHeader.sampleRate = SOUND_SAMPLERATE; /* sample rate */
				perChanInfoPtr->soundHeader.numChannels = 1; /* one channel */
				perChanInfoPtr->soundHeader.loopStart = 0;
				perChanInfoPtr->soundHeader.loopEnd = 0;
				perChanInfoPtr->soundHeader.encode = cmpSH;
				perChanInfoPtr->soundHeader.baseFrequency = kMiddleC;
				perChanInfoPtr->soundHeader.numFrames = (unsigned long)SOUND_LEN;
				/* perChanInfoPtr->soundHeader.AIFFSampleRate = 0; */ /* unused */
				perChanInfoPtr->soundHeader.markerChunk = nil;
				perChanInfoPtr->soundHeader.futureUse2 = 0;
				perChanInfoPtr->soundHeader.stateVars = nil;
				perChanInfoPtr->soundHeader.leftOverSamples = nil;
				perChanInfoPtr->soundHeader.compressionID = 0; /* no compression */
				perChanInfoPtr->soundHeader.packetSize = 0; /* no compression */
				perChanInfoPtr->soundHeader.snthID = 0;
				perChanInfoPtr->soundHeader.sampleSize = 8; /* 8 bits per sample */
				perChanInfoPtr->soundHeader.sampleArea[0] = 0;
				perChanInfoPtr->soundHeader.format = kSoundNotCompressed;
				perChanInfoPtr->soundHeader.samplePtr = (Ptr)(perChanInfoPtr->dbhBufferPtr);

				TheperChanInfoPtr = perChanInfoPtr;

				return trueblnr;
				/* DisposePtr(buffer); */
			}
			DisposePtr((Ptr)perChanInfoPtr);
		}
	}
	return falseblnr;
}

LOCALPROC MySound_UnInit(void)
{
	if (TheperChanInfoPtr != NULL) {
		if (TheperChanInfoPtr->dbhBufferPtr != NULL) {
			DisposePtr(TheperChanInfoPtr->dbhBufferPtr);
		}
		DisposePtr ((Ptr)TheperChanInfoPtr);
		TheperChanInfoPtr = NULL;
	}
}

GLOBALFUNC ui3p GetCurSoundOutBuff(void)
{
	if (sndChannel == NULL) {
		return nullpr;
	} else {
		ui4b CurFillBuff = TheperChanInfoPtr->CurFillBuffer;
		ui4b NextFillBuffer = CurFillBuff + 1;
		ui4b FilledSoundBuffs = NextFillBuffer - TheperChanInfoPtr->CurPlayBuffer;

#if PrintSoundBuffStats
		if (FilledSoundBuffs > TheperChanInfoPtr->MaxFilledSoundBuffs) {
			TheperChanInfoPtr->MaxFilledSoundBuffs = FilledSoundBuffs;
		}
#endif
		if (FilledSoundBuffs < kSoundBuffers) {
			CurFillBuff = NextFillBuffer;
			TheperChanInfoPtr->CurFillBuffer = NextFillBuffer;
		}

		return (ui3p)TheperChanInfoPtr->dbhBufferPtr + (CurFillBuff & kSoundBuffMask) * SOUND_LEN;
	}
}

LOCALPROC MySound_SecondNotify(void)
{
	if (sndChannel != NULL) {
		if (TheperChanInfoPtr->MinFilledSoundBuffs > DesiredMinFilledSoundBuffs) {
			++CurEmulatedTime;
		} else if (TheperChanInfoPtr->MinFilledSoundBuffs < DesiredMinFilledSoundBuffs) {
			--CurEmulatedTime;
		}
#if PrintSoundBuffStats
		fprintf(DumpFile, "MinFilledSoundBuffs = %d\n", TheperChanInfoPtr->MinFilledSoundBuffs);
		fprintf(DumpFile, "MaxFilledSoundBuffs = %d\n", TheperChanInfoPtr->MaxFilledSoundBuffs);
		TheperChanInfoPtr->MaxFilledSoundBuffs = 0;
#endif
		TheperChanInfoPtr->MinFilledSoundBuffs = kSoundBuffers;
	}
}

#endif

#if MySoundEnabled && MySoundFullScreenOnly
LOCALPROC AdjustSoundGrab(void)
{
	if (GrabMachine) {
		MySound_Start();
	} else {
		MySound_Stop();
	}
}
#endif

#if EnableFullScreen
LOCALPROC AdjustMachineGrab(void)
{
#if EnableMouseMotion
	AdjustMouseMotionGrab();
#endif
#if UseOpenGL
	AdjustOpenGLGrab();
#endif
#if 0
	AdjustMainScreenGrab();
#endif
#if MySoundEnabled && MySoundFullScreenOnly
	AdjustSoundGrab();
#endif
}
#endif

/*--- basic dialogs ---*/

#define HogCPU CALL_NOT_IN_CARBON

#if HogCPU
LOCALVAR long NoEventsCounter = 0;
#endif

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

LOCALVAR blnr ADialogIsUp = falseblnr;

LOCALPROC MyBeginDialog(void)
{
	ADialogIsUp = trueblnr;
#if EnableFullScreen
	GrabMachine = falseblnr;
	AdjustMachineGrab();
#endif
	ForceShowCursor();
}

LOCALPROC MyEndDialog(void)
{
#if UseCarbonEvents
	InitKeyCodes();
#else
	do {
		GetKeys(*(KeyMap *)theKeys);
	} while ( /* wait for key ups */
		Keyboard_TestKeyMap(MKC_Return)
		|| Keyboard_TestKeyMap(MKC_Enter)
		|| Keyboard_TestKeyMap(MKC_Escape)
		|| Keyboard_TestKeyMap(MKC_Command) /* such as Command-O in open dialog */
		);
#endif
#if HogCPU
	NoEventsCounter = 0;
#endif
	ADialogIsUp = falseblnr;
#if EnableFullScreen
	GrabMachine = UseFullScreen;
	AdjustMachineGrab();
#endif
}

#define kMyStandardAlert 128

GLOBALPROC MacMsg(char *briefMsg, char *longMsg, blnr fatal)
{
	Str255 briefMsgp;
	Str255 longMsgp;

	if (! gTrueBackgroundFlag) {
		/* dialog during drag and drop hangs if in background */
		MyBeginDialog();
		PStrFromCStr(briefMsgp, briefMsg);
		PStrFromCStr(longMsgp, longMsg);
#if AppearanceAvail
		if (gWeHaveAppearance) {
			AlertStdAlertParamRec param;
			short itemHit;

			param.movable = 0;
			param.filterProc = nil;
			param.defaultText = "\pOK";
			param.cancelText = nil;
			param.otherText = nil;
			param.helpButton = false;
			param.defaultButton = kAlertStdAlertOKButton;
			param.cancelButton = 0;
			param.position = kWindowDefaultPosition;

			StandardAlert((fatal)? kAlertStopAlert : kAlertCautionAlert, briefMsgp, longMsgp, &param, &itemHit);
		} else
#endif
		{
#if CALL_NOT_IN_CARBON /* dialog resource not present in Carbon version */
			ParamText(briefMsgp, longMsgp, "\p", "\p");
			if (fatal) {
				while (StopAlert(kMyStandardAlert, NULL) != 1) {
				}
			} else {
				while (CautionAlert(kMyStandardAlert, NULL) != 1) {
				}
			}
			/* Alert (kMyStandardAlert, 0L); */
#endif
		}
		MyEndDialog();
	}
}

/*--- hide/show menubar ---*/

#if EnableFullScreen
#if MightNotHaveWindows85Avail
LOCALVAR RgnHandle GrayRgnSave = NULL;
LOCALVAR short mBarHeightSave;
#endif
#endif

#if EnableFullScreen
LOCALPROC My_HideMenuBar(void)
{
#if ! CALL_NOT_IN_CARBON
	if (MySetSystemUIModeProc != NULL) {
		(void) MySetSystemUIModeProc(MykUIModeAllHidden,
			MykUIOptionDisableAppleMenu
			| MykUIOptionDisableProcessSwitch
			| MykUIOptionDisableForceQuit
			| MykUIOptionDisableSessionTerminate);
	} else
#endif
#if Windows85APIAvail
	if (gWeHaveHideShowMenu) {
		if (IsMenuBarVisible()) {
			HideMenuBar();
		}
	} else
#endif
	{
#if MightNotHaveWindows85Avail
		if (GrayRgnSave == NULL) {
			RgnHandle mBarRgn = NewRgn();
			if (mBarRgn != NULL) {
				GrayRgnSave = NewRgn();
				if (GrayRgnSave != NULL) {
					CopyRgn(My_GetGrayRgn(), GrayRgnSave);
					RectRgn(mBarRgn, &qd.screenBits.bounds);
					DiffRgn(mBarRgn, My_GetGrayRgn(), mBarRgn);
						/* menu bar rgn, plus corner areas of main screen */
					mBarHeightSave = My_LMGetMBarHeight();
					LMSetMBarHeight(0);
					UnionRgn(My_GetGrayRgn(), mBarRgn, My_GetGrayRgn());
					PaintBehind(LMGetWindowList(), mBarRgn);
					CalcVisBehind(LMGetWindowList(), mBarRgn);
#if 0
					controlStripHidden = false;
					if (Gestalt(gestaltControlStripVersion, &result) == noErr) {
						if (SBIsControlStripVisible()) {
							controlStripHidden = true;
							SBShowHideControlStrip(false);
						}
					}
#endif
				}
				DisposeRgn(mBarRgn);
			}
		}
#endif
	}
}
#endif

#if EnableFullScreen
LOCALPROC My_ShowMenuBar(void)
{
#if ! CALL_NOT_IN_CARBON
	if (MySetSystemUIModeProc != NULL) {
		(void) MySetSystemUIModeProc(MykUIModeNormal,
			0);
	} else
#endif
#if Windows85APIAvail
	if (gWeHaveHideShowMenu) {
		if (! IsMenuBarVisible()) {
			ShowMenuBar();
		}
	} else
#endif
	{
#if MightNotHaveWindows85Avail
		if (GrayRgnSave != NULL) {
			LMSetMBarHeight(mBarHeightSave);
			CopyRgn(GrayRgnSave, My_GetGrayRgn());
			/* PaintBehind(LMGetWindowList(), GrayRgnSave);
			CalcVisBehind(LMGetWindowList(), GrayRgnSave); */
			DisposeRgn(GrayRgnSave);
			DrawMenuBar();
			GrayRgnSave = NULL;
#if 0
			if (controlStripHidden) {
				controlStripHidden = falseblnr;
				if (Gestalt(gestaltControlStripVersion, &result) == noErr) {
					SBShowHideControlStrip(true);
				}
			}
#endif
		}
#endif
	}
}
#endif

/*--- drives ---*/

#define NotAfileRef (-1)

LOCALVAR short Drives[NumDrives]; /* open disk image files */

LOCALPROC InitDrives(void)
{
	si4b i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
	}
}

GLOBALFUNC si4b vSonyRead(void *Buffer, ui4b Drive_No, ui5b Sony_Start, ui5b *Sony_Count)
{
	si4b result;

	result = SetFPos(Drives[Drive_No], fsFromStart, Sony_Start);
	if (result == 0) {
		result = FSRead(Drives[Drive_No], (long *)Sony_Count, Buffer);
	}
	return result;
}

GLOBALFUNC si4b vSonyWrite(void *Buffer, ui4b Drive_No, ui5b Sony_Start, ui5b *Sony_Count)
{
	si4b result;

	result = SetFPos(Drives[Drive_No], fsFromStart, Sony_Start);
	if (result == 0) {
		result = FSWrite(Drives[Drive_No], (long *)Sony_Count, Buffer);
	}
	return result;
}

GLOBALFUNC si4b vSonyGetSize(ui4b Drive_No, ui5b *Sony_Count)
{
	return GetEOF(Drives[Drive_No], (long *)Sony_Count);
}

GLOBALFUNC si4b vSonyEject(ui4b Drive_No)
{
	si4b result;
	short vRefNum;
	short refnum = Drives[Drive_No];
	Drives[Drive_No] = NotAfileRef;

	result = GetVRefNum(refnum, &vRefNum);
	if (result == 0) {
		result = FlushVol(NULL, vRefNum);
	}
	/* should report result if nonzero, but still close in any case */
	result = FSClose(refnum);
	return 0x0000;
}

LOCALFUNC blnr Sony_Insert0(short refnum, blnr locked)
{
	ui4b Drive_No;

	if (! FirstFreeDisk(&Drive_No)) {
		(void) FSClose(refnum);
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage, falseblnr);
		return falseblnr;
	} else {
		Drives[Drive_No] = refnum;
		vSonyInsertedMask |= ((ui5b)1 << Drive_No);
		if (! locked) {
			vSonyWritableMask |= ((ui5b)1 << Drive_No);
		}
		return trueblnr;
	}
}

LOCALFUNC blnr InsertADiskFromFileRef(FSSpec *spec)
{
	short refnum;
	OSErr err;
	blnr locked = falseblnr;

	err = FSpOpenDF(spec, fsRdWrPerm, &refnum);
	if ((permErr == err) || (afpAccessDenied == err)) {
		locked = trueblnr;
		err = FSpOpenDF(spec, fsRdPerm, &refnum);
	}
	if (err != 0) {
		/* report this */
#if 1
		if (opWrErr == err) {
			MacMsg(kStrImageInUseTitle, kStrImageInUseMessage, falseblnr);
		}
#endif
		return falseblnr;
	} else {
		return Sony_Insert0(refnum, locked);
	}
}

#if HaveCPUfamM68K
LOCALFUNC blnr InsertADiskFromNamevRef(ConstStr255Param fileName, short vRefNum)
{
	ParamBlockRec R;
	OSErr err;
	blnr locked = falseblnr;

	R.ioParam.ioCompletion = NULL;
	R.ioParam.ioNamePtr = (StringPtr)fileName;
	R.ioParam.ioVRefNum = vRefNum;
	R.ioParam.ioVersNum = 0;
	R.ioParam.ioPermssn = fsRdWrPerm;
	R.ioParam.ioMisc = NULL;
	err = PBOpen(&R, false);
	if (permErr == err) {
		locked = trueblnr;
		R.ioParam.ioPermssn = fsRdPerm;
		err = PBOpen(&R, false);
	}
	if (err != 0) {
		/* report this */
#if 1
		if (opWrErr == err) {
			MacMsg(kStrImageInUseTitle, kStrImageInUseMessage, falseblnr);
		}
#endif
		return falseblnr;
	} else {
		return Sony_Insert0(R.ioParam.ioRefNum, locked);
	}
}
#endif

LOCALPROC InsertDisksFromDocList(AEDescList *docList)
{
	long itemsInList;
	long index;
	AEKeyword keyword;
	DescType typeCode;
	FSSpec spec;
	Size actualSize;

	if (noErr == AECountItems(docList, &itemsInList)) {
		for (index = 1; index <= itemsInList; ++index) {
			if (noErr == AEGetNthPtr(docList, index, typeFSS, &keyword, &typeCode,
								(Ptr)&spec, sizeof(FSSpec), &actualSize))
			{
				if (! InsertADiskFromFileRef(&spec)) {
					break;
				}
			}
		}
	}
}

LOCALFUNC blnr InsertADiskOrAliasFromSpec(FSSpec *spec)
{
	Boolean isFolder;
	Boolean isAlias;

	if (noErr == ResolveAliasFile(spec, trueblnr, &isFolder, &isAlias))
	if (InsertADiskFromFileRef(spec))
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALFUNC blnr InsertADiskFromNameEtc(short vRefNum, long dirID, ConstStr255Param fileName)
{
#if HaveCPUfamM68K
	if (! MyEnvrAttrFSSpecCallsAvail) {
		return InsertADiskFromNamevRef(fileName, vRefNum);
	} else
#endif
	{
		FSSpec spec;

		if (0 == FSMakeFSSpec(vRefNum, dirID, fileName, &spec))
		if (InsertADiskOrAliasFromSpec(&spec))
		{
			return trueblnr;
		}
		return falseblnr;
	}
}

LOCALFUNC blnr OpenNamedFileInFolder(short vRefNum,
	long dirID, ps3p fileName, short *refnum, blnr *Exists)
{
	OSErr err;

#if HaveCPUfamM68K
	if (! MyEnvrAttrFSSpecCallsAvail) {
		err = FSOpen(fileName, vRefNum, refnum);
		if (err == fnfErr) {
			*Exists = falseblnr;
			return trueblnr;
		} else if (err == 0) {
			*Exists = trueblnr;
			return trueblnr;
		}
	} else
#endif
	{
		Boolean isFolder;
		Boolean isAlias;
		FSSpec spec;

		err = FSMakeFSSpec(vRefNum, dirID, fileName, &spec);
		if (err == fnfErr) {
			*Exists = falseblnr;
			return trueblnr;
		} else if (err == 0) {
			if (0 == ResolveAliasFile(&spec, trueblnr, &isFolder, &isAlias)) {
				err = FSpOpenDF(&spec, fsRdPerm, refnum);
				if (err == 0) {
					*Exists = trueblnr;
					return trueblnr;
				}
			}
		}
	}
	return falseblnr;
}

#if NavigationAvail
pascal Boolean NavigationFilterProc(AEDesc* theItem, void* info, void* NavCallBackUserData, NavFilterModes theNavFilterModes);
pascal Boolean NavigationFilterProc(AEDesc* theItem, void* info, void* NavCallBackUserData, NavFilterModes theNavFilterModes)
{
	Boolean display = true;
	NavFileOrFolderInfo* theInfo = (NavFileOrFolderInfo*)info;
	UnusedParam(theNavFilterModes);
	UnusedParam(NavCallBackUserData);

	if (theItem->descriptorType == typeFSS) {
		if (! theInfo->isFolder) {
			/*
				use:
					'theInfo->fileAndFolder.fileInfo.finderInfo.fdType'
				to check for the file type you want to filter.
			*/
		}
	}
	return display;
}
#endif


#if NavigationAvail
pascal void NavigationEventProc(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void *NavCallBackUserData);
pascal void NavigationEventProc(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void *NavCallBackUserData)
{
	UnusedParam(NavCallBackUserData);

	if (callBackSelector == kNavCBEvent) {
		switch (callBackParms->eventData.eventDataParms.event->what) {
			case updateEvt:
				{
					WindowPtr which = (WindowPtr)callBackParms->eventData.eventDataParms.event->message;

					BeginUpdate(which);

					if (which == gMyMainWindow) {
						Update_Screen();
					}

					EndUpdate(which);
				}
				break;
		}
	}
}
#endif

#define PStrConstBlank ((ps3p)"\000")

LOCALPROC InsertADisk0(void)
{
#if NavigationAvail
#if CALL_NOT_IN_CARBON
#define MyDisposeNavEventUPP(userUPP) DisposeRoutineDescriptor(userUPP)
#define MyDisposeNavObjectFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
#define MyNewNavObjectFilterUPP NewNavObjectFilterProc
#define MyNewNavEventUPP NewNavEventProc
#else
#define MyDisposeNavEventUPP DisposeNavEventUPP
#define MyDisposeNavObjectFilterUPP DisposeNavObjectFilterUPP
#define MyNewNavObjectFilterUPP NewNavObjectFilterUPP
#define MyNewNavEventUPP NewNavEventUPP
#endif

	if (gNavServicesExists) {
		NavReplyRecord theReply;
		NavDialogOptions dialogOptions;
		OSErr theErr = noErr;
		NavTypeListHandle openList = NULL;
		NavObjectFilterUPP filterUPP = MyNewNavObjectFilterUPP(/* (NavObjectFilterProcPtr) */NavigationFilterProc);
		NavEventUPP eventUPP = MyNewNavEventUPP(/* (NavEventProcPtr) */NavigationEventProc);

		theErr = NavGetDefaultDialogOptions(&dialogOptions);

		/* GetIndString((unsigned char*)&dialogOptions.clientName, 130, 1); */

		dialogOptions.dialogOptionFlags += kNavDontAutoTranslate;
		/* dialogOptions.dialogOptionFlags -= kNavAllowMultipleFiles; */
		dialogOptions.dialogOptionFlags -= kNavAllowPreviews;

		MyBeginDialog();
		theErr = NavGetFile(NULL,
						&theReply,
						&dialogOptions,
						/* NULL */ eventUPP,
						NULL,
						filterUPP,
						(NavTypeListHandle)openList,
						NULL);
		MyEndDialog();

		MyDisposeNavObjectFilterUPP(filterUPP);
		MyDisposeNavEventUPP(eventUPP);


		if (theErr == noErr) {
			if (theReply.validRecord) {
				InsertDisksFromDocList(&theReply.selection);
			}

			NavDisposeReply(&theReply);
		}

	} else
#endif
#if HaveCPUfamM68K
	if (! MyEnvrAttrFSSpecCallsAvail) {
		Point where;
		SFReply reply;

		where.h = 50;
		where.v = 50;
		MyBeginDialog();
		SFGetFile(*(Point *)&where, PStrConstBlank, NULL,
					-1 /* kNumFileTypes */, NULL /* fileTypes */,
					NULL, &reply);
		MyEndDialog();
		if (reply.good) {
			(void) InsertADiskFromNamevRef(reply.fName, reply.vRefNum);
		}
	} else
#endif
	{
#if CALL_NOT_IN_CARBON
		StandardFileReply reply;

		MyBeginDialog();
		StandardGetFile(0L, -1, 0L, &reply);
		MyEndDialog();
		if (reply.sfGood) {
			(void) InsertADiskFromFileRef(&reply.sfFile);
		}
#endif
	}
}

LOCALFUNC blnr AllocateMacROM(void)
{
	ROM = (ui4b *)NewPtr(kROM_Size);
	if (ROM == NULL) {
		MacMsg("Not enough Memory.", "Unable to allocate ROM.", trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

#ifndef MyAppIsBundle
#define MyAppIsBundle 0
#endif

LOCALVAR short MyDatvRefNum;
LOCALVAR long MyDatdirID;

#if MyAppIsBundle
LOCALFUNC blnr DirectorySpec2DirId(FSSpec *spec, long *dirID)
{
	CInfoPBRec b;

	b.hFileInfo.ioCompletion = NULL;
	b.hFileInfo.ioNamePtr = (StringPtr)spec->name;
	b.hFileInfo.ioVRefNum = spec->vRefNum;
	b.dirInfo.ioFDirIndex = 0;
	b.dirInfo.ioDrDirID = spec->parID;
	if (noErr == PBGetCatInfo(&b, false)) {
		*dirID = b.dirInfo.ioDrDirID;
		return trueblnr;
	} else {
		return falseblnr;
	}
}
#endif

#if MyAppIsBundle
LOCALFUNC blnr FindNamedChildDirId(short TrueParentVol, long ParentDirId,
	StringPtr ChildName, short *TrueChildVol, long *ChildDirId)
{

	FSSpec temp_spec;
	Boolean isFolder;
	Boolean isAlias;

	if (noErr == FSMakeFSSpec(TrueParentVol, ParentDirId,
		ChildName, &temp_spec))
	if (noErr == ResolveAliasFile(&temp_spec, true,
		&isFolder, &isAlias))
	if (isFolder)
	if (DirectorySpec2DirId(&temp_spec, ChildDirId))
	{
		*TrueChildVol = temp_spec.vRefNum;
		return trueblnr;
	}
	return falseblnr;
}
#endif

#if ! CALL_NOT_IN_CARBON
LOCALVAR CFStringRef MyAppName = NULL;
#endif

LOCALPROC UnInitMyApplInfo(void)
{
#if ! CALL_NOT_IN_CARBON
	if (MyAppName != NULL) {
		CFRelease(MyAppName);
	}
#endif
}

typedef OSStatus (*LSCopyDisplayNameForRefProcPtr)
	(const FSRef *inRef, CFStringRef *outDisplayName);

LOCALFUNC blnr InitMyApplInfo(void)
{
#if ! CALL_NOT_IN_CARBON
	ProcessSerialNumber currentProcess = {0, kCurrentProcess};
#endif
#if MyAppIsBundle
	FSSpec fsSpec;
	FSRef fsRef;

	if (0 == GetProcessBundleLocation(&currentProcess,
		&fsRef))
	if (0 == FSGetCatalogInfo(&fsRef, kFSCatInfoNone,
		NULL, NULL, &fsSpec, NULL))
	{
		long AppDirId;
		short ContentsvRefNum;
		long ContentsdirID;
		short DatvRefNum;
		long DatdirID;

		MyDatvRefNum = fsSpec.vRefNum;
		MyDatdirID = fsSpec.parID;
		if (DirectorySpec2DirId(&fsSpec, &AppDirId))
		if (FindNamedChildDirId(fsSpec.vRefNum, AppDirId,
			"\pContents", &ContentsvRefNum, &ContentsdirID))
		if (FindNamedChildDirId(ContentsvRefNum, ContentsdirID,
			"\pmnvm_dat", &DatvRefNum, &DatdirID))
		{
			MyDatvRefNum = DatvRefNum;
			MyDatdirID = DatdirID;
		}

		if (AppServBunRef != NULL) {
			LSCopyDisplayNameForRefProcPtr MyCopyDisplayNameForRefProc =
				(LSCopyDisplayNameForRefProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("LSCopyDisplayNameForRef"));
			if (MyCopyDisplayNameForRefProc != NULL) {
				if (noErr == MyCopyDisplayNameForRefProc(&fsRef, &MyAppName)) {
					return trueblnr;
				}
			}

		}

		if (noErr == CopyProcessName(&currentProcess, &MyAppName)) {
			return trueblnr;
		}
	}
#else
#if ! CALL_NOT_IN_CARBON
	if (noErr == CopyProcessName(&currentProcess, &MyAppName))
#endif
	{
#if HaveCPUfamM68K
		if (! MyEnvrAttrFSSpecCallsAvail) {
			if (0 == GetVol(NULL, &MyDatvRefNum)) {
				MyDatdirID = 0;
				return trueblnr;
			}
		} else
#endif
		{
			FCBPBRec pb;
			Str255 fileName;

			pb.ioCompletion = NULL;
			pb.ioNamePtr = fileName;
			pb.ioVRefNum = 0;
			pb.ioRefNum = CurResFile();
			pb.ioFCBIndx = 0;
			if (0 == PBGetFCBInfoSync(&pb)) {
				MyDatvRefNum = pb.ioFCBVRefNum;
				MyDatdirID = pb.ioFCBParID;
				return trueblnr;
			}
		}
	}
#endif
	return falseblnr;
}

LOCALFUNC blnr LoadMacRom(void)
{
	OSErr err;
	short refnum;
	Str255 fileName;
	blnr Exists;
	long count = kTrueROM_Size;

	PStrFromCStr(fileName, RomFileName);
	if (OpenNamedFileInFolder(MyDatvRefNum, MyDatdirID, fileName,
		&refnum, &Exists))
	{
		if (! Exists) {
			MacMsg("Unable to locate ROM image.", "The ROM image file could not be found. Please read the manual for instructions on where to get this file.", trueblnr);
		} else {
			err = FSRead(refnum, &count, ROM);
			(void) FSClose(refnum);
			return (err == 0);
		}
	}
	return falseblnr;
}

LOCALFUNC blnr LoadInitialImages(void)
{
	int n = NumDrives > 9 ? 9 : NumDrives;
	int i;
	Str255 s;

	PStrFromCStr(s, "disk?.dsk");

	for (i = 1; i <= n; ++i) {
		s[5] = '0' + i;
		if (! InsertADiskFromNameEtc(MyDatvRefNum, MyDatdirID, s)) {
			/* stop on first error (including file not found) */
			return trueblnr;
		}
	}

	return trueblnr;
}

LOCALFUNC blnr AllocateMacRAM (void)
{
	RAM = (ui4b *)NewPtr(kRAM_Size + RAMSafetyMarginFudge);
	if (RAM == NULL) {
		MacMsg("Not enough Memory.", "Unable to allocate RAM.", trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

#define openOnly 1
#define openPrint 2

LOCALFUNC blnr GotRequiredParams(AppleEvent *theAppleEvent)
{
	DescType typeCode;
	Size actualSize;
	OSErr theErr;

	theErr = AEGetAttributePtr(theAppleEvent, keyMissedKeywordAttr,
				typeWildCard, &typeCode, NULL, 0, &actualSize);
	if (theErr == errAEDescNotFound) { /* No more required params. */
		return trueblnr;
	} else if (theErr == noErr) { /* More required params! */
		return /* CheckSysCode(errAEEventNotHandled) */ falseblnr;
	} else { /* Unexpected Error! */
		return /* CheckSysCode(theErr) */ falseblnr;
	}
}

LOCALFUNC blnr GotRequiredParams0(AppleEvent *theAppleEvent)
{
	DescType typeCode;
	Size actualSize;
	OSErr theErr;

	theErr = AEGetAttributePtr(theAppleEvent, keyMissedKeywordAttr,
				typeWildCard, &typeCode, NULL, 0, &actualSize);
	if (theErr == errAEDescNotFound) { /* No more required params. */
		return trueblnr;
	} else if (theErr == noErr) { /* More required params! */
		return trueblnr; /* errAEEventNotHandled; */ /*^*/
	} else { /* Unexpected Error! */
		return /* CheckSysCode(theErr) */ falseblnr;
	}
}

/* call back */ static pascal OSErr OpenOrPrintFiles(AppleEvent *theAppleEvent, AppleEvent *reply, long aRefCon)
{
	/*Adapted from IM VI: AppleEvent Manager: Handling Required AppleEvents*/
	AEDescList docList;

	UnusedParam(reply);
	UnusedParam(aRefCon);
	/* put the direct parameter (a list of descriptors) into docList */
	if (noErr == (AEGetParamDesc(theAppleEvent, keyDirectObject, typeAEList, &docList))) {
		if (GotRequiredParams0(theAppleEvent)) { /* Check for missing required parameters */
			/* printIt = (aRefCon == openPrint) */
			InsertDisksFromDocList(&docList);
		}
		/* vCheckSysCode */ (void) (AEDisposeDesc(&docList));
	}
	return /* GetASysResultCode() */ 0;
}

/* call back */ static pascal OSErr DoOpenEvent(AppleEvent *theAppleEvent, AppleEvent *reply, long aRefCon)
/* This is the alternative to getting an open document event on startup. */
{
	UnusedParam(reply);
	UnusedParam(aRefCon);
	if (GotRequiredParams0(theAppleEvent)) {
	}
	return /* GetASysResultCode() */ 0; /* Make sure there are no additional "required" parameters. */
}


/* call back */ static pascal OSErr DoQuitEvent(AppleEvent *theAppleEvent, AppleEvent *reply, long aRefCon)
{
	UnusedParam(reply);
	UnusedParam(aRefCon);
	if (GotRequiredParams(theAppleEvent)) {
		RequestMacOff = trueblnr;
	}
	return /* GetASysResultCode() */ 0;
}

#if CALL_NOT_IN_CARBON
#define MyNewAEEventHandlerUPP NewAEEventHandlerProc
#else
#define MyNewAEEventHandlerUPP NewAEEventHandlerUPP
#endif

LOCALFUNC blnr MyInstallEventHandler(AEEventClass theAEEventClass, AEEventID theAEEventID,
						ProcPtr p, long handlerRefcon, blnr isSysHandler)
{
	return 0 == (AEInstallEventHandler(theAEEventClass, theAEEventID,
#if /* useUPP */ 1
			MyNewAEEventHandlerUPP((AEEventHandlerProcPtr)p),
#else
			(AEEventHandlerUPP)p,
#endif
			handlerRefcon, isSysHandler));
}

LOCALPROC InstallAppleEventHandlers(void)
{
	if (AESetInteractionAllowed(kAEInteractWithLocal) == 0)
	if (MyInstallEventHandler(kCoreEventClass, kAEOpenApplication, (ProcPtr)DoOpenEvent, 0, falseblnr))
	if (MyInstallEventHandler(kCoreEventClass, kAEOpenDocuments, (ProcPtr)OpenOrPrintFiles, openOnly, falseblnr))
	if (MyInstallEventHandler(kCoreEventClass, kAEPrintDocuments, (ProcPtr)OpenOrPrintFiles, openPrint, falseblnr))
	if (MyInstallEventHandler(kCoreEventClass, kAEQuitApplication, (ProcPtr)DoQuitEvent, 0, falseblnr))
	{
	}
}

#if EnableDragDrop
static pascal OSErr GlobalTrackingHandler(short message, WindowRef pWindow, void *handlerRefCon, DragReference theDragRef)
{
	RgnHandle hilightRgn;
	Rect Bounds;

	UnusedParam(pWindow);
	UnusedParam(handlerRefCon);
	switch(message) {
		case kDragTrackingEnterWindow:
			hilightRgn = NewRgn();
			if (hilightRgn != NULL) {
				SetScrnRectFromCoords(&Bounds, 0, 0, vMacScreenHeight, vMacScreenWidth);
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

#if EnableDragDrop
static DragTrackingHandlerUPP gGlobalTrackingHandler = NULL;
#endif

#if EnableDragDrop
static pascal OSErr GlobalReceiveHandler(WindowRef pWindow, void *handlerRefCon, DragReference theDragRef)
{
	unsigned short items;
	unsigned short index;
	ItemReference theItem;
	Size SentSize;
	HFSFlavor r;

	UnusedParam(pWindow);
	UnusedParam(handlerRefCon);

	CountDragItems(theDragRef, &items);
	for (index = 1; index <= items; index++) {
		GetDragItemReferenceNumber(theDragRef, index, &theItem);
		if (GetFlavorDataSize(theDragRef, theItem, flavorTypeHFS, &SentSize) == noErr) {
			if (SentSize == sizeof(HFSFlavor)) {
				GetFlavorData(theDragRef, theItem, flavorTypeHFS, (Ptr)&r, &SentSize, 0);
				if (! InsertADiskOrAliasFromSpec(&r.fileSpec)) {
				}
			}
		}
	}

	{
		ProcessSerialNumber currentProcess = {0, kCurrentProcess};

		(void) SetFrontProcess(&currentProcess);
	}

	return noErr;
}
#endif

#if EnableDragDrop
static DragReceiveHandlerUPP gGlobalReceiveHandler = NULL;
#endif

#if EnableDragDrop
#if CALL_NOT_IN_CARBON
#define MyNewDragTrackingHandlerUPP NewDragTrackingHandlerProc
#define MyNewDragReceiveHandlerUPP NewDragReceiveHandlerProc
#else
#define MyNewDragTrackingHandlerUPP NewDragTrackingHandlerUPP
#define MyNewDragReceiveHandlerUPP NewDragReceiveHandlerUPP
#endif
#if ! OPAQUE_UPP_TYPES
#define MyDisposeDragReceiveHandlerUPP(userUPP) DisposeRoutineDescriptor(userUPP)
#define MyDisposeDragTrackingHandlerUPP(userUPP) DisposeRoutineDescriptor(userUPP)
#else
#define MyDisposeDragReceiveHandlerUPP DisposeDragReceiveHandlerUPP
#define MyDisposeDragTrackingHandlerUPP DisposeDragTrackingHandlerUPP
#endif
#endif

#if EnableDragDrop
LOCALFUNC blnr PrepareForDragging(void)
{
	gGlobalTrackingHandler = MyNewDragTrackingHandlerUPP(GlobalTrackingHandler);
	if (gGlobalTrackingHandler != NULL) {
		gGlobalReceiveHandler = MyNewDragReceiveHandlerUPP(GlobalReceiveHandler);
		if (gGlobalReceiveHandler != NULL) {
			if (InstallTrackingHandler(gGlobalTrackingHandler, nil, nil) == 0) {
				if (InstallReceiveHandler(gGlobalReceiveHandler, nil, nil) == 0) {
					return trueblnr;
					/* RemoveReceiveHandler(gGlobalReceiveHandler, nil); */
				}
				RemoveTrackingHandler(gGlobalTrackingHandler, nil);
			}
			MyDisposeDragReceiveHandlerUPP(gGlobalReceiveHandler);
		}
		MyDisposeDragTrackingHandlerUPP(gGlobalTrackingHandler);
	}
	return falseblnr;
}
#endif

#if UseCarbonEvents
static pascal OSStatus windowEventHandler(EventHandlerCallRef nextHandler,
	EventRef theEvent,
	void* userData)
{
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass = GetEventClass(theEvent);
	UInt32 eventKind = GetEventKind(theEvent);

	UnusedParam(nextHandler);
	UnusedParam(userData);
	switch(eventClass) {
		case kEventClassWindow:
			switch(eventKind) {
				case kEventWindowClose:
					RequestMacOff = trueblnr;
					result = noErr;
					break;
				case kEventWindowDrawContent:
					Update_Screen();
					result = noErr;
					break;
				case kEventWindowClickContentRgn:
					CurMouseButton = trueblnr;
					result = noErr;
					break;
			}
			break;
	}

	return result;
}
#endif

#if EnableScalingBuff
#define ScaleBuffSzMult (MyWindowScale * MyWindowScale)
#endif

LOCALFUNC blnr AllocateScreenCompare(void)
{
	screencomparebuff = NewPtr(vMacScreenNumBytes);
	if (screencomparebuff == NULL) {
		MacMsg("Not enough memory", "There is not enough memory available to allocate the screencomparebuff.", trueblnr);
		return falseblnr;
	}
#if UseControlKeys
	CntrlDisplayBuff = NewPtr(vMacScreenNumBytes);
	if (CntrlDisplayBuff == NULL) {
		MacMsg("Not enough memory", "There is not enough memory available to allocate the CntrlDisplayBuff.", trueblnr);
		return falseblnr;
	}
#endif
#if EnableScalingBuff
	ScalingBuff = NewPtr(vMacScreenNumBytes * (
#if UseOpenGL
		ScaleBuffSzMult < 8 ? 8 :
#endif
		ScaleBuffSzMult
		));
	if (ScalingBuff == NULL) {
		MacMsg("Not enough memory", "There is not enough memory available to allocate the ScalingBuff.", trueblnr);
		return falseblnr;
	}
#endif
	return trueblnr;
}

LOCALFUNC blnr MyCreateNewWindow(Rect *Bounds, WindowPtr *theWindow)
{
	WindowPtr ResultWin;
	blnr IsOk = falseblnr;

#if CALL_NOT_IN_CARBON
	ResultWin = NewWindow(
		0L, Bounds, LMGetCurApName() /* "\pMini vMac" */, false,
		noGrowDocProc, /* Could use kWindowSimpleProc for Full Screen */
		(WindowPtr) -1, true, 0);
	if (ResultWin != NULL)
#else
	if (noErr == CreateNewWindow(
#if EnableFullScreen
		WantFullScreen ? kPlainWindowClass :
#endif
			kDocumentWindowClass,
#if EnableFullScreen
		WantFullScreen ? /* kWindowStandardHandlerAttribute */ 0 :
#endif
			kWindowCloseBoxAttribute
				| kWindowCollapseBoxAttribute,
		Bounds, &ResultWin
	))
#endif
	{
#if ! CALL_NOT_IN_CARBON
#if EnableFullScreen
		if (! WantFullScreen)
#endif
		{
			SetWindowTitleWithCFString(ResultWin,
				MyAppName /* CFSTR("Mini vMac") */);
		}
#endif
#if UseCarbonEvents
		InstallStandardEventHandler(GetWindowEventTarget(ResultWin));
		{
			static const EventTypeSpec windowEvents[] =
				{
					{kEventClassWindow, kEventWindowClose},
					{kEventClassWindow, kEventWindowDrawContent},
					{kEventClassWindow, kEventWindowClickContentRgn}
				};
			InstallWindowEventHandler(ResultWin,
				NewEventHandlerUPP(windowEventHandler),
				GetEventTypeCount(windowEvents),
				windowEvents, 0, NULL);
		}
#endif

		*theWindow = ResultWin;

		IsOk = trueblnr;
	}

	return IsOk;
}

enum {
	kMagStateNormal,
#if EnableMagnify
	kMagStateMagnifgy,
#endif
	kNumMagStates
};

#define kMagStateAuto kNumMagStates

LOCALVAR int CurWinIndx;
LOCALVAR blnr HavePositionWins[kNumMagStates];
LOCALVAR Point WinPositionWins[kNumMagStates];

LOCALFUNC blnr ReCreateMainWindow(void)
{
	int WinIndx;
	WindowPtr NewMainWindow;
	Rect MainScrnBounds;
	Rect AllScrnBounds;
	Rect NewWinRect;
	short leftPos;
	short topPos;
	short NewWindowHeight = vMacScreenHeight;
	short NewWindowWidth = vMacScreenWidth;
	WindowPtr OldMainWindow = gMyMainWindow;

#if EnableFullScreen
	if (! UseFullScreen)
#endif
	{
		/* save old position */
		if (OldMainWindow != NULL) {
			Rect r;

			if (MyGetWindowContBounds(OldMainWindow, &r)) {
				WinPositionWins[CurWinIndx].h = r.left;
				WinPositionWins[CurWinIndx].v = r.top;
			}
		}
	}

#if EnableFullScreen
	if (WantFullScreen) {
		My_HideMenuBar();
	} else {
		My_ShowMenuBar();
	}
#endif

	MyGetGrayRgnBounds(&AllScrnBounds);
	MyGetScreenBitsBounds(&MainScrnBounds);

#if EnableMagnify
	if (WantMagnify) {
		NewWindowHeight *= MyWindowScale;
		NewWindowWidth *= MyWindowScale;
	}
#endif

	leftPos = MainScrnBounds.left + ((MainScrnBounds.right - MainScrnBounds.left) - NewWindowWidth) / 2;
	topPos = MainScrnBounds.top + ((MainScrnBounds.bottom - MainScrnBounds.top) - NewWindowHeight) / 2;
	if (leftPos < MainScrnBounds.left) {
		leftPos = MainScrnBounds.left;
	}
	if (topPos < MainScrnBounds.top) {
		topPos = MainScrnBounds.top;
	}

	/* Create window rectangle and centre it on the screen */
	SetRect(&MainScrnBounds, 0, 0, NewWindowWidth, NewWindowHeight);
	OffsetRect(&MainScrnBounds, leftPos, topPos);

#if EnableFullScreen
	if (WantFullScreen) {
		NewWinRect = AllScrnBounds;
	} else
#endif
	{
#if EnableMagnify
		if (WantMagnify) {
			WinIndx = kMagStateMagnifgy;
		} else
#endif
		{
			WinIndx = kMagStateNormal;
		}

		if (! HavePositionWins[WinIndx]) {
			WinPositionWins[WinIndx].h = leftPos;
			WinPositionWins[WinIndx].v = topPos;
			HavePositionWins[WinIndx] = trueblnr;
			NewWinRect = MainScrnBounds;
		} else {
			SetRect(&NewWinRect, 0, 0, NewWindowWidth, NewWindowHeight);
			OffsetRect(&NewWinRect, WinPositionWins[WinIndx].h, WinPositionWins[WinIndx].v);
		}
	}

	if ((OldMainWindow == NULL)
#if EnableFullScreen
		|| (WantFullScreen != UseFullScreen)
#endif
		)
	{
		if (! MyCreateNewWindow(&NewWinRect, &NewMainWindow)) {
			return falseblnr;
		}
	} else {
		NewMainWindow = OldMainWindow;
		MySetMacWindContRect(NewMainWindow, &NewWinRect);
	}

	CurWinIndx = WinIndx;
#if EnableFullScreen
	UseFullScreen = WantFullScreen;
#endif
#if EnableMagnify
	UseMagnify = WantMagnify;
#endif
	gMyMainWindow = NewMainWindow;

#if EnableFullScreen
	if (WantFullScreen) {
		hOffset = MainScrnBounds.left - AllScrnBounds.left;
		vOffset = MainScrnBounds.top - AllScrnBounds.top;
	}
#endif

	if (NewMainWindow != OldMainWindow) {
		ShowWindow(NewMainWindow);
		if (OldMainWindow != NULL) {
			DisposeWindow(OldMainWindow);
		}
	}

	/* check if window rect valid */
#if EnableFullScreen
	if (! WantFullScreen)
#endif
	{
		Rect tr;

		if (MyGetWindowTitleBounds(NewMainWindow, &tr)) {
			if (! RectInRgn(&tr, My_GetGrayRgn())) {
				MySetMacWindContRect(NewMainWindow,
					&MainScrnBounds);
				if (MyGetWindowTitleBounds(NewMainWindow, &tr)) {
					if (! RectInRgn(&tr, My_GetGrayRgn())) {
						OffsetRect(&MainScrnBounds, 0, AllScrnBounds.top - tr.top);
						MySetMacWindContRect(NewMainWindow,
							&MainScrnBounds);
					}
				}
			}
		}
	}

#if EnableFullScreen
	GrabMachine = UseFullScreen && ! ADialogIsUp;
	AdjustMachineGrab();
#endif
	return trueblnr;
}

LOCALPROC CheckMagnifyAndFullScreen(void)
{
	if (0
#if EnableMagnify
		|| (UseMagnify != WantMagnify)
#endif
#if EnableFullScreen
		|| (UseFullScreen != WantFullScreen)
#endif
		)
	{
		(void) ReCreateMainWindow();
#if HogCPU
		NoEventsCounter = 0;
#endif
	}
}

#if EnableMagnify
GLOBALPROC ToggleWantMagnify(void)
{
	WantMagnify = ! WantMagnify;
}
#endif

#if EnableFullScreen && EnableMagnify
enum {
	kWinStateWindowed,
#if EnableMagnify
	kWinStateFullScreen,
#endif
	kNumWinStates
};
#endif

#if EnableFullScreen && EnableMagnify
LOCALVAR int WinMagStates[kNumWinStates];
#endif

#if EnableFullScreen
GLOBALPROC ToggleWantFullScreen(void)
{
	WantFullScreen = ! WantFullScreen;

#if EnableMagnify
	{
		int OldWinState = UseFullScreen ? kWinStateFullScreen : kWinStateWindowed;
		int OldMagState = UseMagnify ? kMagStateMagnifgy : kMagStateNormal;
		int NewWinState = WantFullScreen ? kWinStateFullScreen : kWinStateWindowed;
		int NewMagState = WinMagStates[NewWinState];
		WinMagStates[OldWinState] = OldMagState;
		if (kMagStateAuto != NewMagState) {
			WantMagnify = (NewMagState == kMagStateMagnifgy);
		} else {
			WantMagnify = falseblnr;
			if (WantFullScreen) {
				Rect r;

				MyGetScreenBitsBounds(&r);
				if (((r.right - r.left) >= vMacScreenWidth * MyWindowScale)
					&& ((r.bottom - r.top) >= vMacScreenHeight * MyWindowScale)
					)
				{
					WantMagnify = trueblnr;
				}
			}
		}
	}
#endif
}
#endif

LOCALPROC DoMoreCommandsMsg(void)
{
	MacMsg("More commands are available in the Mini vMac Control Mode.",
		"To enter the Control Mode, press and hold down the 'control' key (after closing this message). You will remain in the Control Mode until you release the 'control' key. Type 'H' in the Control Mode to list available commands.",
		falseblnr);
}

LOCALPROC DoAboutMsg(void)
{
	MacMsg("About",
		"To display information about this program, use the 'A' command of the Mini vMac Control Mode. To learn about the Control Mode, see the 'More Commands\311' item in the 'Special' menu.",
		falseblnr);
}

#if UseCarbonEvents
LOCALVAR EventLoopTimerRef MyTimerRef;
#endif

LOCALPROC LeaveBackground(void)
{
#if HogCPU
	NoEventsCounter = 0;
#endif

#if UseCarbonEvents
	InitKeyCodes();
#endif

	SetCursorArror();
}

LOCALPROC EnterBackground(void)
{
#if EnableFullScreen
	if (WantFullScreen) {
		ToggleWantFullScreen();
	}
#endif

	ForceShowCursor();
}

LOCALPROC LeaveSpeedStopped(void)
{
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Start();
#endif

	StartUpTimeAdjust();
#if UseCarbonEvents
	(void) SetEventLoopTimerNextFireTime(
		MyTimerRef, MyTickDuration);
#endif
}

LOCALPROC EnterSpeedStopped(void)
{
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
}

LOCALPROC DoMyLowPriorityTasks(void)
{
	if (gTrueBackgroundFlag != gBackgroundFlag) {
		gBackgroundFlag = gTrueBackgroundFlag;
		if (gTrueBackgroundFlag) {
			EnterBackground();
		} else {
			LeaveBackground();
		}
	}

	if (CurSpeedStopped != ( /* SpeedStopped || */
		(gBackgroundFlag && ! RunInBackground)))
	{
		CurSpeedStopped = ! CurSpeedStopped;
		if (CurSpeedStopped) {
			EnterSpeedStopped();
		} else {
			LeaveSpeedStopped();
		}
	}

#if EnableMagnify || EnableFullScreen
	CheckMagnifyAndFullScreen();
#endif

	if (RequestInsertDisk) {
		RequestInsertDisk = falseblnr;
		InsertADisk0();
	}

	if (RequestMacOff) {
		RequestMacOff = falseblnr;
		if (AnyDiskInserted()) {
			MacMsg("Please shut down the emulated machine before quitting.",
				"To force Mini vMac to quit, at the risk of corrupting the mounted disk image files, use the 'Q' command of the Mini vMac Control Mode. To learn about the Control Mode, see the 'More Commands\311' item in the 'Special' menu.",
				falseblnr);
		} else {
			ForceMacOff = trueblnr;
		}
	}

	DoEmulateExtraTime();
}

LOCALPROC RunEmulatedTicksToTrueTime(void)
{
	UpdateTrueEmulatedTime();

	if (CheckDateTime()) {
#if MySoundEnabled
		MySound_SecondNotify();
#endif
	}

	TimeAdjust = TrueEmulatedTime - CurEmulatedTime;
	if (TimeAdjust >= 0) {
		ui5b InitialTime = TrueEmulatedTime;

		if (! (gBackgroundFlag || ADialogIsUp)) {
			CheckMouseState();
#if ! UseCarbonEvents
			CheckKeyBoardState();
#endif
		}

		do {
			DoEmulateOneTick();
			++CurEmulatedTime;

			UpdateTrueEmulatedTime();
			TimeAdjust = TrueEmulatedTime - CurEmulatedTime;
		} while ((TimeAdjust >= 0)
			&& (TrueEmulatedTime == InitialTime));

		if (TimeAdjust > 7) {
			/* emulation not fast enough */
			TimeAdjust = 7;
			CurEmulatedTime = TrueEmulatedTime - TimeAdjust;
		}
	}
}

#if UseCarbonEvents
LOCALVAR EventRef EventForMyLowPriority = nil;
#endif

#if UseCarbonEvents
LOCALFUNC blnr InitMyLowPriorityTasksEvent(void)
{
	if (noErr != CreateEvent(nil, 'MnvM', 'MnvM', GetCurrentEventTime(),
						kEventAttributeNone, &EventForMyLowPriority))
	{
		EventForMyLowPriority = nil;
	} else {
		if (noErr == PostEventToQueue(GetMainEventQueue(),
			EventForMyLowPriority, kEventPriorityLow))
		{
			return trueblnr;
			/* ReleaseEvent(EventForMyLowPriority); */
		}
	}
	return falseblnr;
}
#endif

#if UseCarbonEvents
LOCALPROC PostMyLowPriorityTasksEvent(void)
{
	EventQueueRef mq = GetMainEventQueue();

	if (IsEventInQueue(mq, EventForMyLowPriority)) {
		(void) RemoveEventFromQueue(mq, EventForMyLowPriority);
	}
	(void) SetEventTime(EventForMyLowPriority, GetCurrentEventTime());

	if (noErr == PostEventToQueue(mq,
		EventForMyLowPriority, kEventPriorityLow))
	{
	}
}
#endif

#if ! CALL_NOT_IN_CARBON
#define CheckItem CheckMenuItem
#endif

/* Menu Constants */

#define kAppleMenu   128
#define kFileMenu    129
#define kSpecialMenu 130

/* Apple */

enum {
	kAppleNull,

	kAppleAboutItem,
	kAppleSep1,

	kNumAppleItems
};

/* File */

enum {
	kFileNull,

	kFileOpenDiskImage,
	kFileSep1,
	kFileQuitItem,

	kNumFileItems
};

/* Special */

enum {
	kSpecialNull,

	kSpecialMoreCommandsItem,

	kNumSpecialItems
};

#if CALL_NOT_IN_CARBON
LOCALPROC DoOpenDA(short menuItem)
{
	Str32 name;
	GrafPtr savePort;

	GetPort(&savePort);
	GetMenuItemText(GetMenuHandle(kAppleMenu), menuItem, name);
	OpenDeskAcc(name);
	SystemTask();
	SetPort(savePort);
}
#endif

#if ! UseCarbonEvents
LOCALPROC MacOS_HandleMenu(short menuID, short menuItem)
{
	switch (menuID) {
		case kAppleMenu:
			if (menuItem == kAppleAboutItem) {
				DoAboutMsg();
			} else {
#if CALL_NOT_IN_CARBON
				DoOpenDA(menuItem);
#endif
			}
			break;

		case kFileMenu:
			switch (menuItem) {
				case kFileOpenDiskImage:
					RequestInsertDisk = trueblnr;
					break;

				case kFileQuitItem:
					RequestMacOff = trueblnr;
					break;
			}
			break;

		case kSpecialMenu:
			switch (menuItem) {
				case kSpecialMoreCommandsItem:
					DoMoreCommandsMsg();
					break;
			}
			break;

		default:
			/* if menuID == 0, then no command chosen from menu */
			/* do nothing */
			break;
	}
}
#endif

#if UseCarbonEvents
enum {
	kCmdIdNull,
	kCmdIdMoreCommands,

	kNumCmdIds
};
#endif

#if UseCarbonEvents
LOCALFUNC OSStatus MyProcessCommand(MenuCommand inCommandID)
{
	OSStatus result = noErr;

	switch (inCommandID) {
		case kHICommandAbout:
			DoAboutMsg();
			break;
		case kHICommandQuit:
			RequestMacOff = trueblnr;
			break;
		case kHICommandOpen:
			RequestInsertDisk = trueblnr;
			break;
		case kCmdIdMoreCommands:
			DoMoreCommandsMsg();
			break;
		default:
			result = eventNotHandledErr;
			break;
	}

	return result;
}
#endif

#if UseCarbonEvents
LOCALFUNC OSStatus Keyboard_UpdateKeyMap3(EventRef theEvent, blnr down)
{
	UInt32 uiKeyCode;

	GetEventParameter(theEvent, kEventParamKeyCode, typeUInt32, NULL, sizeof(uiKeyCode), NULL, &uiKeyCode);
	Keyboard_UpdateKeyMap2(uiKeyCode & 0x000000FF, down);
	return noErr;
}
#endif

#if UseCarbonEvents
static pascal OSStatus MyEventHandler(EventHandlerCallRef nextHandler, EventRef theEvent, void * userData)
{
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass = GetEventClass(theEvent);
	UInt32 eventKind = GetEventKind(theEvent);

	UnusedParam(userData);

	switch (eventClass) {
		case kEventClassMouse:
			switch (eventKind) {
				case kEventMouseDown:
#if EnableFullScreen
					if (GrabMachine) {
						CurMouseButton = trueblnr;
						result = noErr;
					} else
#endif
					{
						result = CallNextEventHandler(nextHandler, theEvent);
					}
					break;
				case kEventMouseUp:
					CurMouseButton = falseblnr;
#if EnableFullScreen
					if (GrabMachine) {
						result = noErr;
					} else
#endif
					{
						result = CallNextEventHandler(nextHandler, theEvent);
					}
					break;
			}
			break;
		case kEventClassApplication:
			switch (eventKind) {
				case kEventAppActivated:
					gTrueBackgroundFlag = falseblnr;
					if (CurSpeedStopped) {
						PostMyLowPriorityTasksEvent();
					}
					result = noErr;
					break;
				case kEventAppDeactivated:
					gTrueBackgroundFlag = trueblnr;
					result = noErr;
					break;
			}
			break;
		case kEventClassCommand:
			switch (eventKind) {
				case kEventProcessCommand:
					{
						HICommand hiCommand;

						GetEventParameter(theEvent, kEventParamDirectObject, typeHICommand, NULL,
							sizeof(HICommand), NULL, &hiCommand);
						result = MyProcessCommand(hiCommand.commandID);
					}
					break;
			}
			break;
		case kEventClassKeyboard:
			if (ADialogIsUp) {
				return CallNextEventHandler(nextHandler, theEvent);
			} else {
				switch (eventKind) {
					case kEventRawKeyDown:
						result = Keyboard_UpdateKeyMap3(theEvent, trueblnr);
						break;
					case kEventRawKeyUp:
						result = Keyboard_UpdateKeyMap3(theEvent, falseblnr);
						break;
					case kEventRawKeyModifiersChanged:
						{
							UInt32 theModifiers;

							GetEventParameter(theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(typeUInt32), NULL, &theModifiers);

							MyUpdateKeyboardModifiers(theModifiers);
						}
						result = noErr;
						break;
					default:
						break;
				}
			}
			break;
		case 'MnvM':
			if ('MnvM' == eventKind) {
				DoMyLowPriorityTasks();
			}
			break;
		default:
			break;
	}
	return result;
}
#endif

#if UseCarbonEvents
static pascal void MyTimerProc(EventLoopTimerRef inTimer,
	void *inUserData)
{
	UnusedParam(inUserData);

	if (ForceMacOff) {
		QuitApplicationEventLoop();
	} else if (CurSpeedStopped) {
		(void) SetEventLoopTimerNextFireTime(
			inTimer, kEventDurationForever);
	} else {
		EventTimeout inTimeout = NextTickChangeTime - GetCurrentEventTime();
		if (inTimeout > 0.0) {
			(void) SetEventLoopTimerNextFireTime(
				inTimer, inTimeout);
			/*
				already a periodic timer, this is just to make
				sure of accuracy.
			*/
		}

		RunEmulatedTicksToTrueTime();

		PostMyLowPriorityTasksEvent();
	}
}
#endif

#if ! UseCarbonEvents
LOCALPROC HandleMacEvent(EventRecord *theEvent)
{
	WindowPtr whichWindow;
	GrafPtr savePort;

	switch(theEvent->what) {
		case mouseDown:
			switch (FindWindow(theEvent->where, &whichWindow)) {
				case inSysWindow:
#if CALL_NOT_IN_CARBON
					SystemClick(theEvent, whichWindow);
#endif
					break;
				case inMenuBar:
					ForceShowCursor();
					{
						long menuSelection = MenuSelect(theEvent->where);
						MacOS_HandleMenu(HiWord(menuSelection), LoWord(menuSelection));
					}
					HiliteMenu(0);
					break;

				case inDrag:
					{
						Rect r;

						MyGetScreenBitsBounds(&r);
						DragWindow(whichWindow, theEvent->where, &r);
					}
					break;

				case inContent:
					if (FrontWindow() != whichWindow) {
						SelectWindow(whichWindow);
					}
					break;

				case inGoAway:
					if (TrackGoAway(whichWindow, theEvent->where)) {
						RequestMacOff = trueblnr;
					}
					break;

				case inZoomIn:
				case inZoomOut:
					/* Zoom Boxes */
					break;
			}
			break;

		case updateEvt:
			GetPort(&savePort);
			BeginUpdate((WindowPtr) theEvent->message);

			if ((WindowPtr)theEvent->message == gMyMainWindow) {
				Update_Screen();
			}

			EndUpdate((WindowPtr) theEvent->message);
			SetPort(savePort);
			break;

		case keyDown:
		case autoKey:
		case keyUp:
			/* ignore it */
			break;
		case osEvt:
			if ((theEvent->message >> 24) & suspendResumeMessage) {
				if (theEvent->message & 1) {
					gTrueBackgroundFlag = falseblnr;
				} else {
					gTrueBackgroundFlag = trueblnr;
				}
			}
			break;
		case kHighLevelEvent:
			if ((AEEventClass)theEvent->message == kCoreEventClass) {
				if (/* CheckSysCode */0 == (AEProcessAppleEvent(theEvent))) {
				}
			} else {
				/* vCheckSysCode(errAENotAppleEvent); */
			}
			break;
	}
}
#endif

#if ! UseCarbonEvents
LOCALPROC WaitInBackground(void)
{
	EventRecord theEvent;

	do {
		/* we're not doing anything, let system do as it pleases */
		if (
#if HaveCPUfamM68K
			(! MyEnvrAttrWaitNextEventAvail) ?
			GetNextEvent(everyEvent, &theEvent) :
#endif
			WaitNextEvent(everyEvent, &theEvent, 5*60*60, NULL))
		{
			HandleMacEvent(&theEvent);
		}
	} while (gTrueBackgroundFlag && ! RequestMacOff);
}
#endif

#if ! UseCarbonEvents
LOCALPROC DontWaitForEvent(void)
{
	/* we're busy, but see what system wants */

	EventRecord theEvent;
	int i = 0;

#if 0 /* this seems to cause crashes on some machines */
	if (EventAvail(everyEvent, &theEvent)) {
		NoEventsCounter = 0;
#endif

		while ((
#if HaveCPUfamM68K
			(! MyEnvrAttrWaitNextEventAvail) ?
			GetNextEvent(everyEvent, &theEvent) :
#endif
			WaitNextEvent(everyEvent, &theEvent, 0, NULL))
			&& (i < 10))
		{
			HandleMacEvent(&theEvent);
#if HogCPU
			NoEventsCounter = 0;
#endif
			i++;
		}
#if 0
	}
#endif
	if (gTrueBackgroundFlag && ! RunInBackground) {
		WaitInBackground();
	}
}
#endif

#define PrivateEventMask (mDownMask | mUpMask | keyDownMask | keyUpMask | autoKeyMask)

#define IsPowOf2(x) (((x) & ((x) - 1)) == 0)

#if ! UseCarbonEvents
LOCALPROC CheckForSystemEvents(void)
{
#if HogCPU
	/* can't hog cpu in carbon. OSEventAvail and GetOSEvent not available. */
	/* only hog cpu in full screen mode */
	if (UseFullScreen && ! SpeedLimit && ! CurSpeedStopped) {
		EventRecord theEvent;

		if (! OSEventAvail(everyEvent, &theEvent)) {
			/*
				if no OSEvent now, and not looking for aftermath of
				event, assume there is no event of any kind we need to look at
			*/
			if (NoEventsCounter < 256) {
				NoEventsCounter++;
				if (IsPowOf2(NoEventsCounter)) {
					DontWaitForEvent();
				}
			}
		} else {
			WindowPtr whichWindow;

			blnr PrivateEvent = falseblnr;
			switch (theEvent.what) {
				case keyDown:
				case autoKey:
				case keyUp:
				case mouseUp:
					PrivateEvent = trueblnr;
					break;
				case mouseDown:
					if ((FindWindow(theEvent.where, &whichWindow) == inContent)
						&& (whichWindow == gMyMainWindow)
						&& (FrontWindow() == whichWindow))
					{
						PrivateEvent = trueblnr;
					}
					break;
			}
			if (PrivateEvent) {
				/*
					if event can effect only us, and not looking out for aftermath
					of another event, then hog the cpu
				*/
				(void) GetOSEvent(PrivateEventMask, &theEvent); /* discard it */
			} else {
				NoEventsCounter = 0;
				/*
					Have an Event, so reset NoEventsCounter, no matter what.
					WaitNextEvent can return false, even if it did handle an
					event. Such as a click in the collapse box. In this case
					we need to look out for update events.
				*/
				DontWaitForEvent();
			}
		}
	} else
#endif
	{
		DontWaitForEvent();
	}
}
#endif

#if ! UseCarbonEvents
LOCALPROC WaitForEndOfSixtieth(void)
{
	if (TimeAdjust < 0) {
		do {
#if HaveCPUfamM68K
			if (MyEnvrAttrWaitNextEventAvail)
#endif
			{
				EventRecord theEvent;

				if (WaitNextEvent(everyEvent, &theEvent, 1, NULL)) {
					HandleMacEvent(&theEvent);
#if HogCPU
					NoEventsCounter = 0;
#endif
				}
			}
		} while (ExtraTimeNotOver());
	}
}
#endif

#if ! UseCarbonEvents
LOCALPROC MainEventLoop(void)
{
	do {
		CheckForSystemEvents();
		RunEmulatedTicksToTrueTime();
		DoMyLowPriorityTasks();
		WaitForEndOfSixtieth();
	} while (! ForceMacOff);
}
#endif

LOCALPROC AppendMenuCStr(MenuHandle menu, char *s)
{
	Str255 t;

	PStrFromCStr(t, s);
	AppendMenu(menu, t);
}

#if ! CALL_NOT_IN_CARBON
LOCALPROC RemoveCommandKeysInMenu(MenuRef theMenu)
{
	MenuRef outHierMenu;
	int i;
	UInt16 n = CountMenuItems(theMenu);

	for (i = 1; i <= n; i++) {
		SetItemCmd(theMenu, i, 0);
		if (noErr == GetMenuItemHierarchicalMenu(
			theMenu, i, &outHierMenu)
			&& (NULL != outHierMenu))
		{
			RemoveCommandKeysInMenu(outHierMenu);
		}
	}
}
#endif

LOCALFUNC blnr InstallOurMenus(void)
{
	MenuHandle menu;
	Str255 s;
#if ! CALL_NOT_IN_CARBON
	blnr AquaMenuLayout = falseblnr;

	{
		long response;

		/* see if we should modify quit in accordance with the Aqua HI guidelines */
		if ((Gestalt(gestaltMenuMgrAttr, &response) == noErr) && (response & gestaltMenuMgrAquaLayoutMask))
		{
			AquaMenuLayout = trueblnr;
		}
	}
#endif

	PStrFromChar(s, (char)20);
	menu = NewMenu(kAppleMenu, s);
	if (menu != NULL) {
		AppendMenuCStr(menu, "About Mini vMac\311");
#if UseCarbonEvents
		(void) SetMenuItemCommandID(menu, kAppleAboutItem, kHICommandAbout);
#endif
		AppendMenuCStr(menu, "(-");
#if CALL_NOT_IN_CARBON
		AppendResMenu(menu, 'DRVR');
#endif
		InsertMenu(menu, 0);
	}

	PStrFromCStr(s, "File");
	menu = NewMenu(kFileMenu, s);
	if (menu != NULL) {
		AppendMenuCStr(menu, "Open Disk Image\311");
#if UseCarbonEvents
		(void) SetMenuItemCommandID(menu, kFileOpenDiskImage, kHICommandOpen);
#endif
#if ! CALL_NOT_IN_CARBON
		if (! AquaMenuLayout)
#endif
		{
			AppendMenuCStr(menu, "(-");
			AppendMenuCStr(menu, "Quit");
		}
		InsertMenu(menu, 0);
	}

	PStrFromCStr(s, "Special");
	menu = NewMenu(kSpecialMenu, s);
	if (menu != NULL) {
		AppendMenuCStr(menu, "More Commands\311");
#if UseCarbonEvents
		(void) SetMenuItemCommandID(menu, kSpecialMoreCommandsItem, kCmdIdMoreCommands);
#endif
		InsertMenu(menu, 0);
	}

#if ! CALL_NOT_IN_CARBON
	{
		MenuRef outMenu;
		MenuItemIndex outIndex;

		if (noErr == GetIndMenuItemWithCommandID(
			NULL, kHICommandQuit, 1, &outMenu, &outIndex))
		{
			RemoveCommandKeysInMenu(outMenu);
		}
	}
#endif

	DrawMenuBar();

	return trueblnr;
}

#if AppearanceAvail
LOCALFUNC blnr InstallOurAppearanceClient(void)
{
	if (gWeHaveAppearance) {
		RegisterAppearanceClient();
	}
	return trueblnr;
}
#endif

LOCALFUNC blnr InstallOurEventHandlers(void)
{
#if UseCarbonEvents
	EventTypeSpec eventTypes[] = {
		{kEventClassMouse, kEventMouseDown},
		{kEventClassMouse, kEventMouseUp},
		{kEventClassKeyboard, kEventRawKeyModifiersChanged},
		{kEventClassKeyboard, kEventRawKeyDown},
		{kEventClassKeyboard, kEventRawKeyUp},
		{kEventClassApplication, kEventAppActivated},
		{kEventClassApplication, kEventAppDeactivated},
		{kEventClassCommand, kEventProcessCommand},
		{'MnvM', 'MnvM'}
	};

	InstallApplicationEventHandler(
		NewEventHandlerUPP(MyEventHandler),
		GetEventTypeCount(eventTypes),
		eventTypes, NULL, NULL);

	InitKeyCodes();
#endif

	if (MyEnvrAttrAppleEvtMgrAvail) {
		InstallAppleEventHandlers();
	}
#if EnableDragDrop
	if (gHaveDragMgr) {
		gHaveDragMgr = PrepareForDragging();
	}
#endif
#if UseCarbonEvents
	if (noErr != InstallEventLoopTimer(GetMainEventLoop(),
		kEventDurationMinute, /* will set actual time later */
		MyTickDuration,
		NewEventLoopTimerUPP(MyTimerProc),
		NULL, &MyTimerRef))
	{
		return falseblnr;
	}
#endif
	return trueblnr;
}

LOCALPROC ZapOSGLUVars(void)
{
	InitDrives();
	{
		int i;

		for (i = 0; i < kNumMagStates; i++) {
			HavePositionWins[i] = falseblnr;
		}
#if EnableFullScreen && EnableMagnify
		for (i = 0; i < kNumWinStates; i++) {
			WinMagStates[i] = kMagStateAuto;
		}
#endif
	}
}

LOCALFUNC blnr InitOSGLU(void)
{
	if (InitMacManagers())
	if (InitCheckMyEnvrn())
	if (InitMyApplInfo())
#if AppearanceAvail
	if (InstallOurAppearanceClient())
#endif
	if (InstallOurEventHandlers())
	if (InstallOurMenus())
#if MySoundEnabled
	if (MySound_Init())
#endif
	if (AllocateScreenCompare())
	if (ReCreateMainWindow())
	if (AllocateMacROM())
	if (LoadMacRom())
	if (LoadInitialImages())
#if MakeDumpFile
	if (StartDump())
#endif
	if (AllocateMacRAM())
	if (InitLocationDat())
#if UseCarbonEvents
	if (InitMyLowPriorityTasksEvent())
#endif
	if (InitEmulation())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
#if EnableFullScreen
	GrabMachine = falseblnr;
	AdjustMachineGrab();
#endif
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
#if MySoundEnabled
	MySound_UnInit();
#endif

#if EnableFullScreen
	My_ShowMenuBar();
#endif

#if MakeDumpFile
	EndDump();
#endif

#if EnableDragDrop
	if (gHaveDragMgr) {
		RemoveReceiveHandler(gGlobalReceiveHandler, nil);
		RemoveTrackingHandler(gGlobalTrackingHandler, nil);
	}
#endif
	UnInitMyApplInfo();

	ForceShowCursor();

/*
	we're relying on the operating
	system to take care of disposing
	our window, and any memory allocated
	should disappear with the applications
	heap.
*/
}

#ifndef MainReturnsInt
#define MainReturnsInt 0
#endif

#if MainReturnsInt
int
#else
void
#endif
main(void)
{
	ZapOSGLUVars();
	if (InitOSGLU()) {
#if UseCarbonEvents
		RunApplicationEventLoop();
#else
		MainEventLoop();
#endif
	}
	UnInitOSGLU();

#if MainReturnsInt
	return 0;
#endif
}
