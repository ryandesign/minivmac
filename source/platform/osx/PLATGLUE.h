/*
	PLATGLUE.h

	Copyright (C) 2007 Philip Cummins, Richard F. Bannister, Paul C. Pratt

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
	PLATform GLUE. (for Macintosh OS X)

	All operating system dependent code for the
	Macintosh platform should go here.

	This code is descended from Richard F. Bannister's Macintosh
	port of vMac, by Philip Cummins.

	The main entry point 'main' is at the end of this file.
*/

/* if UsingCarbonLib, instead of native Macho, then some APIs are missing */
#ifndef UsingCarbonLib
#define UsingCarbonLib 0
#endif

/*--- some simple utilities ---*/

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
	(void) memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

LOCALPROC PStrFromChar(ps3p r, char x)
{
	r[0] = 1;
	r[1] = (char)x;
}

/*--- control mode and internationalization ---*/

#define NeedRequestInsertDisk 1
#define NeedDoMoreCommandsMsg 1
#define NeedDoAboutMsg 1

#define NeedCell2UnicodeMap 1

#include "CONTROLM.h"

/*--- information about the environment ---*/

LOCALVAR CFBundleRef AppServBunRef;

LOCALVAR blnr DidApplicationServicesBun = falseblnr;

LOCALFUNC blnr HaveApplicationServicesBun(void)
{
	if (! DidApplicationServicesBun) {
		AppServBunRef = CFBundleGetBundleWithIdentifier(
			CFSTR("com.apple.ApplicationServices"));
		DidApplicationServicesBun = trueblnr;
	}
	return (AppServBunRef != NULL);
}

LOCALVAR CFBundleRef HIToolboxBunRef;

LOCALVAR blnr DidHIToolboxBunRef = falseblnr;

LOCALFUNC blnr HaveHIToolboxBunRef(void)
{
	if (! DidHIToolboxBunRef) {
		HIToolboxBunRef = CFBundleGetBundleWithIdentifier(
			CFSTR("com.apple.HIToolbox"));
		DidHIToolboxBunRef = trueblnr;
	}
	return (HIToolboxBunRef != NULL);
}


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
LOCALVAR SetSystemUIModeProcPtr MySetSystemUIMode = NULL;
LOCALVAR blnr DidSetSystemUIMode = falseblnr;

LOCALFUNC blnr HaveMySetSystemUIMode(void)
{
	if (! DidSetSystemUIMode) {
		if (HaveHIToolboxBunRef()) {
			MySetSystemUIMode =
				(SetSystemUIModeProcPtr)
				CFBundleGetFunctionPointerForName(
					HIToolboxBunRef, CFSTR("SetSystemUIMode"));
		}
		DidSetSystemUIMode = trueblnr;
	}
	return (MySetSystemUIMode != NULL);
}

/* In 10.1 or later */

typedef OSStatus (*LSCopyDisplayNameForRefProcPtr)
	(const FSRef *inRef, CFStringRef *outDisplayName);
LOCALVAR LSCopyDisplayNameForRefProcPtr MyLSCopyDisplayNameForRef = NULL;
LOCALVAR blnr DidLSCopyDisplayNameForRef = falseblnr;

LOCALFUNC blnr HaveMyLSCopyDisplayNameForRef(void)
{
	if (! DidLSCopyDisplayNameForRef) {
		if (HaveApplicationServicesBun()) {
			MyLSCopyDisplayNameForRef =
				(LSCopyDisplayNameForRefProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("LSCopyDisplayNameForRef"));
		}
		DidLSCopyDisplayNameForRef = trueblnr;
	}
	return (MyLSCopyDisplayNameForRef != NULL);
}

/* routines not in carbon lib */


#if UsingCarbonLib

typedef CGDisplayErr
(*CGGetActiveDisplayListProcPtr) (
	CGDisplayCount       maxDisplays,
	CGDirectDisplayID *  activeDspys,
	CGDisplayCount *     dspyCnt);
LOCALVAR CGGetActiveDisplayListProcPtr MyCGGetActiveDisplayList = NULL;
LOCALVAR blnr DidCGGetActiveDisplayList = falseblnr;

LOCALFUNC blnr HaveMyCGGetActiveDisplayList(void)
{
	if (! DidCGGetActiveDisplayList) {
		if (HaveApplicationServicesBun()) {
			MyCGGetActiveDisplayList =
				(CGGetActiveDisplayListProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGGetActiveDisplayList"));
		}
		DidCGGetActiveDisplayList = trueblnr;
	}
	return (MyCGGetActiveDisplayList != NULL);
}

#else

#define HaveMyCGGetActiveDisplayList() trueblnr
#define MyCGGetActiveDisplayList CGGetActiveDisplayList

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef CGRect
(*CGDisplayBoundsProcPtr) (CGDirectDisplayID display);
LOCALVAR CGDisplayBoundsProcPtr MyCGDisplayBounds = NULL;
LOCALVAR blnr DidCGDisplayBounds = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayBounds(void)
{
	if (! DidCGDisplayBounds) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayBounds =
				(CGDisplayBoundsProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayBounds"));
		}
		DidCGDisplayBounds = trueblnr;
	}
	return (MyCGDisplayBounds != NULL);
}

#else

#define HaveMyCGDisplayBounds() trueblnr
#define MyCGDisplayBounds CGDisplayBounds

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef size_t
(*CGDisplayPixelsWideProcPtr) (CGDirectDisplayID display);
LOCALVAR CGDisplayPixelsWideProcPtr MyCGDisplayPixelsWide = NULL;
LOCALVAR blnr DidCGDisplayPixelsWide = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayPixelsWide(void)
{
	if (! DidCGDisplayPixelsWide) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayPixelsWide =
				(CGDisplayPixelsWideProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayPixelsWide"));
		}
		DidCGDisplayPixelsWide = trueblnr;
	}
	return (MyCGDisplayPixelsWide != NULL);
}

#else

#define HaveMyCGDisplayPixelsWide() trueblnr
#define MyCGDisplayPixelsWide CGDisplayPixelsWide

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef size_t
(*CGDisplayPixelsHighProcPtr) (CGDirectDisplayID display);
LOCALVAR CGDisplayPixelsHighProcPtr MyCGDisplayPixelsHigh = NULL;
LOCALVAR blnr DidCGDisplayPixelsHigh = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayPixelsHigh(void)
{
	if (! DidCGDisplayPixelsHigh) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayPixelsHigh =
				(CGDisplayPixelsHighProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayPixelsHigh"));
		}
		DidCGDisplayPixelsHigh = trueblnr;
	}
	return (MyCGDisplayPixelsHigh != NULL);
}

#else

#define HaveMyCGDisplayPixelsHigh() trueblnr
#define MyCGDisplayPixelsHigh CGDisplayPixelsHigh

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef CGDisplayErr
(*CGDisplayHideCursorProcPtr) (CGDirectDisplayID display);
LOCALVAR CGDisplayHideCursorProcPtr MyCGDisplayHideCursor = NULL;
LOCALVAR blnr DidCGDisplayHideCursor = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayHideCursor(void)
{
	if (! DidCGDisplayHideCursor) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayHideCursor =
				(CGDisplayHideCursorProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayHideCursor"));
		}
		DidCGDisplayHideCursor = trueblnr;
	}
	return (MyCGDisplayHideCursor != NULL);
}

#else

#define HaveMyCGDisplayHideCursor() trueblnr
#define MyCGDisplayHideCursor CGDisplayHideCursor

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef CGDisplayErr
(*CGDisplayShowCursorProcPtr) (CGDirectDisplayID display);
LOCALVAR CGDisplayShowCursorProcPtr MyCGDisplayShowCursor = NULL;
LOCALVAR blnr DidCGDisplayShowCursor = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayShowCursor(void)
{
	if (! DidCGDisplayShowCursor) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayShowCursor =
				(CGDisplayShowCursorProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayShowCursor"));
		}
		DidCGDisplayShowCursor = trueblnr;
	}
	return (MyCGDisplayShowCursor != NULL);
}

#else

#define HaveMyCGDisplayShowCursor() trueblnr
#define MyCGDisplayShowCursor CGDisplayShowCursor

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef CGEventErr
(*CGWarpMouseCursorPositionProcPtr) (CGPoint newCursorPosition);
LOCALVAR CGWarpMouseCursorPositionProcPtr MyCGWarpMouseCursorPosition = NULL;
LOCALVAR blnr DidCGWarpMouseCursorPosition = falseblnr;

LOCALFUNC blnr HaveMyCGWarpMouseCursorPosition(void)
{
	if (! DidCGWarpMouseCursorPosition) {
		if (HaveApplicationServicesBun()) {
			MyCGWarpMouseCursorPosition =
				(CGWarpMouseCursorPositionProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGWarpMouseCursorPosition"));
		}
		DidCGWarpMouseCursorPosition = trueblnr;
	}
	return (MyCGWarpMouseCursorPosition != NULL);
}

#else

#define HaveMyCGWarpMouseCursorPosition() trueblnr
#define MyCGWarpMouseCursorPosition CGWarpMouseCursorPosition

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef CGEventErr
(*CGSetLocalEventsSuppressionIntervalProcPtr) (CFTimeInterval seconds);
LOCALVAR CGSetLocalEventsSuppressionIntervalProcPtr MyCGSetLocalEventsSuppressionInterval = NULL;
LOCALVAR blnr DidCGSetLocalEventsSuppressionInterval = falseblnr;

LOCALFUNC blnr HaveMyCGSetLocalEventsSuppressionInterval(void)
{
	if (! DidCGSetLocalEventsSuppressionInterval) {
		if (HaveApplicationServicesBun()) {
			MyCGSetLocalEventsSuppressionInterval =
				(CGSetLocalEventsSuppressionIntervalProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGSetLocalEventsSuppressionInterval"));
		}
		DidCGSetLocalEventsSuppressionInterval = trueblnr;
	}
	return (MyCGSetLocalEventsSuppressionInterval != NULL);
}

#else

#define HaveMyCGSetLocalEventsSuppressionInterval() trueblnr
#define MyCGSetLocalEventsSuppressionInterval CGSetLocalEventsSuppressionInterval

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef OSStatus (*CreateStandardAlertProcPtr) (
	AlertType alertType,
	CFStringRef error,
	CFStringRef explanation,
	const AlertStdCFStringAlertParamRec * param,
	DialogRef * outAlert
);
LOCALVAR CreateStandardAlertProcPtr MyCreateStandardAlert = NULL;
LOCALVAR blnr DidCreateStandardAlert = falseblnr;

LOCALFUNC blnr HaveMyCreateStandardAlert(void)
{
	if (! DidCreateStandardAlert) {
		if (HaveHIToolboxBunRef()) {
			MyCreateStandardAlert =
				(CreateStandardAlertProcPtr)
				CFBundleGetFunctionPointerForName(
					HIToolboxBunRef, CFSTR("CreateStandardAlert"));
		}
		DidCreateStandardAlert = trueblnr;
	}
	return (MyCreateStandardAlert != NULL);
}

#else

#define HaveMyCreateStandardAlert() trueblnr
#define MyCreateStandardAlert CreateStandardAlert

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef OSStatus (*RunStandardAlertProcPtr) (
	DialogRef inAlert,
	ModalFilterUPP filterProc,
	DialogItemIndex * outItemHit
);
LOCALVAR RunStandardAlertProcPtr MyRunStandardAlert = NULL;
LOCALVAR blnr DidRunStandardAlert = falseblnr;

LOCALFUNC blnr HaveMyRunStandardAlert(void)
{
	if (! DidRunStandardAlert) {
		if (HaveHIToolboxBunRef()) {
			MyRunStandardAlert =
				(RunStandardAlertProcPtr)
				CFBundleGetFunctionPointerForName(
					HIToolboxBunRef, CFSTR("RunStandardAlert"));
		}
		DidRunStandardAlert = trueblnr;
	}
	return (MyRunStandardAlert != NULL);
}

#else

#define HaveMyRunStandardAlert() trueblnr
#define MyRunStandardAlert RunStandardAlert

#endif /* ! UsingCarbonLib */


/*--- display utilities ---*/

/* DoForEachDisplay adapted from Apple Technical Q&A QA1168 */

typedef void
(*ForEachDisplayProcPtr) (CGDirectDisplayID display);

LOCALPROC DoForEachDisplay0(CGDisplayCount dspCount,
	CGDirectDisplayID *displays, ForEachDisplayProcPtr p)
{
	int i;

	if (noErr == MyCGGetActiveDisplayList(dspCount,
		displays, &dspCount))
	{
		for (i = 0; i < dspCount; ++i) {
			p(displays[i]);
		}
	}
}

LOCALPROC DoForEachDisplay(ForEachDisplayProcPtr p)
{
	CGDisplayCount dspCount = 0;

	if (HaveMyCGGetActiveDisplayList()
		&& (noErr == MyCGGetActiveDisplayList(0, NULL, &dspCount)))
	{
		if (dspCount <= 2) {
			CGDirectDisplayID displays[2];
			DoForEachDisplay0(dspCount, displays, p);
		} else {
			CGDirectDisplayID *displays =
				calloc((size_t)dspCount, sizeof(CGDirectDisplayID));
			if (NULL != displays) {
				DoForEachDisplay0(dspCount, displays, p);
				free(displays);
			}
		}
	}
}

LOCALVAR void *datp;

typedef CGDirectDisplayID (*CGMainDisplayIDProcPtr)(void);

LOCALVAR CGMainDisplayIDProcPtr MyCGMainDisplayID = NULL;
LOCALVAR blnr DidCGMainDisplayID = falseblnr;

LOCALFUNC blnr HaveMyCGMainDisplayID(void)
{
	if (! DidCGMainDisplayID) {
		if (HaveApplicationServicesBun()) {
			MyCGMainDisplayID =
				(CGMainDisplayIDProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGMainDisplayID"));
		}
		DidCGMainDisplayID = trueblnr;
	}
	return (MyCGMainDisplayID != NULL);
}

#ifndef kCGNullDirectDisplay /* not in MPW Headers */
#define kCGNullDirectDisplay ((CGDirectDisplayID)0)
#endif

LOCALPROC MyMainDisplayIDProc(CGDirectDisplayID display)
{
	CGDirectDisplayID *p = (CGDirectDisplayID *)datp;

	if (kCGNullDirectDisplay == *p) {
		*p = display;
	}
}

LOCALFUNC CGDirectDisplayID MyMainDisplayID(void)
{
	if (HaveMyCGMainDisplayID()) {
		return MyCGMainDisplayID();
	} else {
		/* for before OS X 10.2 */
		CGDirectDisplayID r = kCGNullDirectDisplay;
		void *savedatp = datp;
		datp = (void *)&r;
		DoForEachDisplay(MyMainDisplayIDProc);
		datp = savedatp;
		return r;
	}
}

/*--- utilities for adapting to the environment ---*/

LOCALPROC MyGetScreenBitsBounds(Rect *r)
{
	if (HaveMyCGDisplayBounds()) {
		CGRect cgr = MyCGDisplayBounds(MyMainDisplayID());

		r->left = cgr.origin.x;
		r->top = cgr.origin.y;
		r->right = cgr.origin.x + cgr.size.width;
		r->bottom = cgr.origin.y + cgr.size.height;
	}
}

LOCALPROC InvalWholeWindow(WindowRef mw)
{
	Rect bounds;

	GetWindowPortBounds(mw, &bounds);
	InvalWindowRect(mw, &bounds);
}

LOCALPROC MySetMacWindContRect(WindowRef mw, Rect *r)
{
	(void) SetWindowBounds (mw, kWindowContentRgn, r);
	InvalWholeWindow(mw);
}

LOCALFUNC blnr MyGetWindowTitleBounds(WindowRef mw, Rect *r)
{
	return (noErr == GetWindowBounds(mw, kWindowTitleBarRgn, r));
}

LOCALFUNC blnr MyGetWindowContBounds(WindowRef mw, Rect *r)
{
	return (noErr == GetWindowBounds(mw, kWindowContentRgn, r));
}

LOCALPROC MyGetGrayRgnBounds(Rect *r)
{
	GetRegionBounds(GetGrayRgn(), (Rect *)r);
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
	if (NULL == DumpFile) {
		return falseblnr;
	} else {
		fprintf(DumpFile, "Dump File\n");
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

EXPORTPROC DumpATable(ui5b *p, ui5b n);

GLOBALPROC DumpATable(ui5b *p, ui5b n)
{
	si5b i;

	for (i = 0; i < n; ++i) {
		fprintf(DumpFile, "%d\n", p[i]);
	}
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

#ifndef MyWindowScale
#define MyWindowScale 2
#endif

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

#define EnableScalingBuff 1

#if EnableScalingBuff
LOCALVAR char *ScalingBuff = nullpr;
#endif

#if 0 /* some experiments */
LOCALVAR CGrafPtr GrabbedPort = NULL;
#endif

#if 0
LOCALPROC AdjustMainScreenGrab(void)
{
	if (GrabMachine) {
		if (NULL == GrabbedPort) {
			/* CGDisplayCapture(MyMainDisplayID()); */
			CGCaptureAllDisplays();
			/* CGDisplayHideCursor( MyMainDisplayID() ); */
			GrabbedPort = CreateNewPortForCGDisplayID((UInt32)MyMainDisplayID());
			LockPortBits (GrabbedPort);
		}
	} else {
		if (GrabbedPort != NULL) {
			UnlockPortBits (GrabbedPort);
			/* CGDisplayShowCursor( MyMainDisplayID() ); */
			/* CGDisplayRelease(MyMainDisplayID()); */
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
	if (HaveApplicationServicesBun()) {
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
	if (HaveApplicationServicesBun()) {
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

LOCALPROC UpdateLuminanceCopy(si4b top, si4b left, si4b bottom, si4b right)
{
	int i;
	int j;
	int k;
	ui3b *p1 = (ui3b *)GetCurDrawBuff() + vMacScreenWidth / 8 * top;
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

LOCALVAR AGLContext ctx = NULL;
LOCALVAR short GLhOffset;
LOCALVAR short GLvOffset;

#ifndef UseAGLdoublebuff
#define UseAGLdoublebuff 0
#endif

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
			(const GLubyte *)GetCurDrawBuff());
#endif
#if 1
		UpdateLuminanceCopy(top, left, bottom, right);
		{
#if UseAGLdoublebuff
			glRasterPos2i(GLhOffset, GLvOffset);
			glDrawPixels(vMacScreenWidth,
				vMacScreenHeight,
				GL_LUMINANCE,
				GL_UNSIGNED_BYTE,
				ScalingBuff);
#else
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
#endif
		}
#endif
#if UseAGLdoublebuff
		aglSwapBuffers(ctx);
#else
		glFlush();
#endif
	}
}

LOCALPROC Update_Screen(void)
{
	if (ctx != NULL) {
		MyDrawWithOpenGL(0, 0, vMacScreenHeight, vMacScreenWidth);
	}
}

LOCALPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right)
{
	if (ctx != NULL) {
		MyDrawWithOpenGL(top, left, bottom, right);
	}
}

LOCALPROC MyAdjustGLforSize(int h, int v)
{
	if (GL_TRUE != aglSetCurrentContext(ctx)) {
		/* err = aglReportError() */
	} else {

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

		NeedWholeScreenDraw = trueblnr;
	}
}

LOCALVAR AGLPixelFormat window_fmt = NULL;
LOCALVAR AGLContext window_ctx = NULL;

#ifndef UseAGLfullscreen
#define UseAGLfullscreen 0
#endif

#if UseAGLfullscreen
LOCALVAR AGLPixelFormat fullscreen_fmt = NULL;
LOCALVAR AGLContext fullscreen_ctx = NULL;
#endif

#if UseAGLfullscreen
LOCALPROC MaybeFullScreen(void)
{
	if (UseFullScreen && (NULL == fullscreen_ctx)
		&& HaveMyCGDisplayPixelsWide()
		&& HaveMyCGDisplayPixelsHigh())
	{
		static const GLint fullscreen_attrib[] = {AGL_RGBA,
			AGL_NO_RECOVERY,
			AGL_FULLSCREEN,
			AGL_SINGLE_RENDERER, AGL_ACCELERATED,
#if UseAGLdoublebuff
			AGL_DOUBLEBUFFER,
#endif
			AGL_NONE};
		GDHandle theDevice;
		CGDirectDisplayID CurMainDisplayID = MyMainDisplayID();

		DMGetGDeviceByDisplayID((DisplayIDType)CurMainDisplayID, &theDevice, false);
		fullscreen_fmt = aglChoosePixelFormat(&theDevice, 1, fullscreen_attrib);
		if (NULL == fullscreen_fmt) {
			/* err = aglReportError() */
		} else {
			fullscreen_ctx = aglCreateContext(fullscreen_fmt, NULL);
			if (NULL == fullscreen_ctx) {
				/* err = aglReportError() */
			} else {
				/* MyCaptureAllDisplays(); */
				if (GL_TRUE != aglSetFullScreen(fullscreen_ctx, 0, 0, 0, 0))
				{
					/* err = aglReportError() */
				} else {
					Rect r;

					int h = MyCGDisplayPixelsWide(CurMainDisplayID);
					int v = MyCGDisplayPixelsHigh(CurMainDisplayID);

					GetWindowBounds(gMyMainWindow, kWindowContentRgn, &r);

					GLhOffset = r.left + hOffset;
					GLvOffset = v - (r.top + vOffset);

					ctx = fullscreen_ctx;
					MyAdjustGLforSize(h, v);
					return;
				}
				/* MyReleaseAllDisplays(); */

				if (GL_TRUE != aglDestroyContext(fullscreen_ctx)) {
					/* err = aglReportError() */
				}
				fullscreen_ctx = NULL;
			}

			aglDestroyPixelFormat (fullscreen_fmt);
			fullscreen_fmt = NULL;
		}
	}
}
#endif

#if UseAGLfullscreen
LOCALPROC NoFullScreen(void)
{
	if (fullscreen_ctx != NULL) {
		Rect r;
		int h;
		int v;

		GetWindowBounds(gMyMainWindow, kWindowContentRgn, &r);

		h = r.right - r.left;
		v = r.bottom - r.top;

		GLhOffset = hOffset;
		GLvOffset = v - vOffset;

		ctx = window_ctx;

		MyAdjustGLforSize(h, v);

		Update_Screen();

		if (fullscreen_ctx != NULL) {
			if (GL_TRUE != aglDestroyContext(fullscreen_ctx)) {
				/* err = aglReportError() */
			}
			fullscreen_ctx = NULL;
		}
		if (fullscreen_fmt != NULL) {
			aglDestroyPixelFormat(fullscreen_fmt);
			fullscreen_fmt = NULL;
		}
	}
}
#endif

#if UseAGLfullscreen
LOCALPROC AdjustOpenGLGrab(void)
{
	if (GrabMachine) {
		MaybeFullScreen();
	} else {
		NoFullScreen();
	}
}
#endif

/*--- keyboard ---*/

LOCALVAR UInt32 SavedModifiers = 0;

LOCALPROC MyUpdateKeyboardModifiers(UInt32 theModifiers)
{
	UInt32 ChangedModifiers = theModifiers ^ SavedModifiers;

	if (ChangedModifiers & shiftKey) {
		Keyboard_UpdateKeyMap2(MKC_Shift, (shiftKey & theModifiers) != 0);
	}
	if (ChangedModifiers & cmdKey) {
		Keyboard_UpdateKeyMap2(MKC_Command, (cmdKey & theModifiers) != 0);
	}
	if (ChangedModifiers & alphaLock) {
		Keyboard_UpdateKeyMap2(MKC_CapsLock, (alphaLock & theModifiers) != 0);
	}
	if (ChangedModifiers & optionKey) {
		Keyboard_UpdateKeyMap2(MKC_Option, (optionKey & theModifiers) != 0);
	}
	if (ChangedModifiers & controlKey) {
		Keyboard_UpdateKeyMap2(MKC_Control, (controlKey & theModifiers) != 0);
	}
	SavedModifiers = theModifiers;
}

LOCALVAR blnr WantCmdOptOnReconnect = falseblnr;

LOCALPROC ReconnectKeyCodes3(void)
{
	UInt32 theModifiers = GetCurrentKeyModifiers();
	UInt32 ChangedModifiers = theModifiers ^ SavedModifiers;

	if (ChangedModifiers & alphaLock) {
		Keyboard_UpdateKeyMap2(MKC_CapsLock, (alphaLock & theModifiers) != 0);
	}

	/* ignore changes to other modifiers */
	/* except for command/option on receive drop */

	if (WantCmdOptOnReconnect) {
		WantCmdOptOnReconnect = falseblnr;

		if ((optionKey & theModifiers) != 0) {
			Keyboard_UpdateKeyMap2(MKC_Option, trueblnr);
		}

		if ((cmdKey & theModifiers) != 0) {
			Keyboard_UpdateKeyMap2(MKC_Command, trueblnr);
		}
	}

	SavedModifiers = theModifiers;
}

/*--- cursor hiding ---*/

#if 0
LOCALPROC MyShowCursorProc(CGDirectDisplayID display)
{
	(void) CGDisplayShowCursor(display);
}
#endif

LOCALPROC MyShowCursor(void)
{
#if 0
	/* ShowCursor(); deprecated */
	DoForEachDisplay(MyShowCursorProc);
#endif
	if (HaveMyCGDisplayShowCursor()) {
		(void) MyCGDisplayShowCursor(MyMainDisplayID());
			/* documentation now claims argument ignored */
	}
}

#if 0
LOCALPROC MyHideCursorProc(CGDirectDisplayID display)
{
	(void) CGDisplayHideCursor(display);
}
#endif

LOCALPROC MyHideCursor(void)
{
#if 0
	/* HideCursor(); deprecated */
	DoForEachDisplay(MyHideCursorProc);
#endif
	if (HaveMyCGDisplayHideCursor()) {
		(void) MyCGDisplayHideCursor(MyMainDisplayID());
			/* documentation now claims argument ignored */
	}
}

LOCALVAR blnr HaveCursorHidden = falseblnr;
LOCALVAR blnr WantCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		MyShowCursor();
	}
}

/*--- cursor moving ---*/

LOCALPROC SetCursorArrow(void)
{
	SetThemeCursor(kThemeArrowCursor);
}

#if 0

typedef CGDisplayErr (*CGDisplayMoveCursorToPointProcPtr)
	(CGDirectDisplayID display, CGPoint point);
LOCALVAR CGDisplayMoveCursorToPointProcPtr MyCGDisplayMoveCursorToPoint = NULL;
LOCALVAR blnr DidCGDisplayMoveCursorToPoint = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayMoveCursorToPoint(void)
{
	if (! DidCGDisplayMoveCursorToPoint) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayMoveCursorToPoint =
				(CGDisplayMoveCursorToPointProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayMoveCursorToPoint"));
		}
		DidCGDisplayMoveCursorToPoint = trueblnr;
	}
	return (MyCGDisplayMoveCursorToPoint != NULL);
}

#endif /* 0 */

#if EnableMouseMotion && EnableFullScreen
LOCALFUNC blnr MyMoveMouse(si4b h, si4b v)
{
	Point CurMousePos;
	Rect r;

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
	GetWindowBounds(gMyMainWindow, kWindowContentRgn, &r);
	CurMousePos.h = r.left + h;
	CurMousePos.v = r.top + v;

	/* This method from SDL_QuartzWM.m, "Simple DirectMedia Layer", Copyright (C) 1997-2003 Sam Lantinga */
	if (HaveMyCGSetLocalEventsSuppressionInterval()) {
		if (noErr != MyCGSetLocalEventsSuppressionInterval(0.0)) {
			/* don't use MacMsg which can call MyMoveMouse */
		}
	}
	if (HaveMyCGWarpMouseCursorPosition()) {
		CGPoint pt;
		pt.x = CurMousePos.h;
		pt.y = CurMousePos.v;
		if (noErr != MyCGWarpMouseCursorPosition(pt)) {
			/* don't use MacMsg which can call MyMoveMouse */
		}
	}
#if 0
	if (HaveMyCGDisplayMoveCursorToPoint()) {
		CGPoint pt;
		pt.x = CurMousePos.h;
		pt.y = CurMousePos.v;
		if (noErr != MyCGDisplayMoveCursorToPoint(MyMainDisplayID(), pt)) {
			/* don't use MacMsg which can call MyMoveMouse */
		}
	}
#endif

	return trueblnr;
}
#endif

#if EnableMouseMotion && EnableFullScreen
LOCALVAR si4b SavedMouseH;
LOCALVAR si4b SavedMouseV;
#endif

#if EnableMouseMotion && EnableFullScreen
LOCALPROC AdjustMouseMotionGrab(void)
{
	if (gMyMainWindow != NULL) {
#if EnableFullScreen
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
		} else
#endif
		{
			if (HaveMouseMotion) {
				(void) MyMoveMouse(CurMouseH, CurMouseV);
				HaveMouseMotion = falseblnr;
			}
		}
	}
}
#endif

LOCALVAR blnr MouseIsOutside = falseblnr;
	/*
		MouseIsOutside true if sure mouse outside our window. If in
		our window, or not sure, set false.
	*/

LOCALPROC CheckMouseState(void)
{
	/*
		Not MouseIsOutside includes in the title bar, etc, so have
		to check if in content area.
	*/

	Rect r;
	Point NewMousePos;
	blnr ShouldHaveCursorHidden = ! MouseIsOutside;

	GetWindowBounds(gMyMainWindow, kWindowContentRgn, &r);
	GetGlobalMouse(&NewMousePos);
		/* deprecated, but haven't found useable replace.
			between window deactivate and then reactivate,
			mouse can move without getting kEventMouseMoved.
			and no way to get initial position.
		*/

	NewMousePos.h -= r.left;
	NewMousePos.v -= r.top;

#if EnableFullScreen
	if (UseFullScreen) {
		NewMousePos.h -= hOffset;
		NewMousePos.v -= vOffset;
	}
#endif

#if EnableMagnify
	if (UseMagnify) {
		NewMousePos.h /= MyWindowScale;
		NewMousePos.v /= MyWindowScale;
	}
#endif

#if EnableMouseMotion && EnableFullScreen
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

		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/* for a game like arkanoid, would like mouse to still
		move even when outside window in one direction */
		{
			CurMouseV = NewMousePos.v;
			CurMouseH = NewMousePos.h;
		}
	}

	WantCursorHidden = ShouldHaveCursorHidden;
}

#if 0
LOCALFUNC blnr InitMousePosition(void)
{
	/*
		Since there doesn't seem to be any nondeprecated
		way to get initial cursor position, instead
		start by moving cursor to known position.
	*/

#if EnableFullScreen
	if (! UseFullScreen)
#endif
	{
		CurMouseH = 16;
		CurMouseV = 16;
		WantCursorHidden = trueblnr;
		(void) MyMoveMouse(CurMouseH, CurMouseV);
	}

	return trueblnr;
}
#endif

LOCALPROC DisconnectKeyCodes3(void)
{
	DisconnectKeyCodes2();

	CurMouseButton = falseblnr;

	ForceShowCursor();
}

/*--- time, date, location ---*/

/*
	be sure to avoid getting confused if TickCount
	overflows and wraps.
*/

LOCALVAR ui5b TrueEmulatedTime = 0;
LOCALVAR ui5b CurEmulatedTime = 0;

LOCALVAR EventTime NextTickChangeTime;

#define MyTickDuration (kEventDurationSecond / 60.14742)

#include "DATE2SEC.h"

/* LOCALVAR EventTime ETimeBase; */
LOCALVAR CFAbsoluteTime ATimeBase;
LOCALVAR ui5b TimeSecBase;

LOCALFUNC blnr InitLocationDat(void)
{
	MachineLocation loc;

	ReadLocation(&loc);
	CurMacLatitude = (ui5b)loc.latitude;
	CurMacLongitude = (ui5b)loc.longitude;
	CurMacDelta = (ui5b)loc.u.gmtDelta;

	{
		CFTimeZoneRef tz = CFTimeZoneCopySystem();
		if (tz) {
			/* CFAbsoluteTime */ ATimeBase = CFAbsoluteTimeGetCurrent();
			/* ETimeBase = GetCurrentEventTime(); */
			{
				CFGregorianDate d = CFAbsoluteTimeGetGregorianDate(ATimeBase, tz);
				double floorsec = floor(d.second);
				ATimeBase -= (d.second - floorsec);
				/* ETimeBase -= (d.second - floorsec); */
				TimeSecBase = Date2MacSeconds(floorsec, d.minute, d.hour,
					d.day, d.month, d.year);
			}
			CFRelease(tz);
		}
	}

	return trueblnr;
}

LOCALPROC StartUpTimeAdjust(void)
{
	NextTickChangeTime = GetCurrentEventTime() + MyTickDuration;
}

LOCALPROC UpdateTrueEmulatedTime(void)
{
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
}

LOCALFUNC blnr CheckDateTime(void)
{
	ui5b NewMacDateInSecond = TimeSecBase + (ui5b)(CFAbsoluteTimeGetCurrent() - ATimeBase);
	/* ui5b NewMacDateInSecond = TimeSecBase + (ui5b)(GetCurrentEventTime() - ETimeBase); */
	/* ui5b NewMacDateInSecond = ((ui5b)(CFAbsoluteTimeGetCurrent()))  + 3061137600UL; */

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

#define kLn2BuffLen 9
#define kLnBuffSz (kLn2SoundBuffers + kLn2BuffLen)
#define My_Sound_Len (1UL << kLn2BuffLen)
#define kBufferSize (1UL << kLnBuffSz)
#define kBufferMask (kBufferSize - 1)
#define dbhBufferSize (kBufferSize + SOUND_LEN)

#define PrintSoundBuffStats (0 && MakeDumpFile)

/* Structs */
struct PerChanInfo {
	volatile ui4b  CurPlayOffset;
	volatile ui4b  CurFillOffset;
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

LOCALPROC FillWithSilence(ui3p p, int n, ui3b v)
{
	int i;

	for (i = n; --i >= 0; ) {
		*p++ = v;
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
		ui4b CurPlayOffset =  perChanInfoPtr->CurPlayOffset;
		ui4b NextPlayOffset =  CurPlayOffset + My_Sound_Len;
		ui4b ToPlaySize = perChanInfoPtr->CurFillOffset - NextPlayOffset;
		ui4b FilledSoundBuffs = ToPlaySize >> kLn2BuffLen;

		if (FilledSoundBuffs < perChanInfoPtr->MinFilledSoundBuffs) {
			perChanInfoPtr->MinFilledSoundBuffs = FilledSoundBuffs;
		}

		if (0 == FilledSoundBuffs) {
			/* out of sound to play. play a bit of silence */
			perChanInfoPtr->soundHeader.samplePtr = (Ptr)perChanInfoPtr->dbhBufferPtr + (CurPlayOffset & kBufferMask);
			FillWithSilence((ui3p)perChanInfoPtr->soundHeader.samplePtr, My_Sound_Len,
				/* 0x80 */ *((ui3p)perChanInfoPtr->soundHeader.samplePtr + My_Sound_Len - 1));
		} else {
			CurPlayOffset = NextPlayOffset;
			perChanInfoPtr->CurPlayOffset = CurPlayOffset;
			perChanInfoPtr->soundHeader.samplePtr = (Ptr)perChanInfoPtr->dbhBufferPtr + (CurPlayOffset & kBufferMask);
		}

		perChanInfoPtr->soundHeader.numFrames = (unsigned long)My_Sound_Len;

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
	if (NULL == sndChannel) {
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
				if (noErr == SndChannelStatus(sndChannel,
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
		perChanInfoPtr = (PerChanInfoPtr) malloc(sizeof(PerChanInfo));
		if (perChanInfoPtr != NULL) {
			buffer = (Ptr) malloc(dbhBufferSize);
			if (buffer != NULL) {
				FillWithSilence((ui3p)buffer, dbhBufferSize, 0x80);
				/* RampSound((ui3p)buffer, dbhBufferSize, 0x80, 0x00); */
				perChanInfoPtr->dbhBufferPtr = buffer;

				perChanInfoPtr->CurPlayOffset= 0;
				perChanInfoPtr->CurFillOffset = (kSoundBuffers - 1) * My_Sound_Len;
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
				perChanInfoPtr->soundHeader.numFrames = (unsigned long)My_Sound_Len;
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
				/* free(buffer); */
			}
			free(perChanInfoPtr);
		}
	}
	return falseblnr;
}

LOCALPROC MySound_UnInit(void)
{
	if (TheperChanInfoPtr != NULL) {
		if (TheperChanInfoPtr->dbhBufferPtr != NULL) {
			free(TheperChanInfoPtr->dbhBufferPtr);
		}
		free(TheperChanInfoPtr);
		TheperChanInfoPtr = NULL;
	}
}

GLOBALFUNC ui3p GetCurSoundOutBuff(void)
{
	if (NULL == sndChannel) {
		return nullpr;
	} else {
		ui4b CurFillOffset = TheperChanInfoPtr->CurFillOffset;
		ui4b NextFillOffset = CurFillOffset + SOUND_LEN;
		ui4b ToPlaySize = NextFillOffset - TheperChanInfoPtr->CurPlayOffset;

		if (ToPlaySize < kBufferSize) {
			ui4b CurFillSeq = CurFillOffset >> kLnBuffSz;
			ui4b NextFillSeq = NextFillOffset >> kLnBuffSz;

			if (CurFillSeq != NextFillSeq) {
				MyMoveBytes((anyp)(TheperChanInfoPtr->dbhBufferPtr + kBufferSize),
					(anyp)TheperChanInfoPtr->dbhBufferPtr,
					NextFillOffset & kBufferMask);
			}

#if PrintSoundBuffStats
			{
				ui4b FilledSoundBuffs = ToPlaySize >> kLnBuffSz;

				if (FilledSoundBuffs > TheperChanInfoPtr->MaxFilledSoundBuffs) {
					TheperChanInfoPtr->MaxFilledSoundBuffs = FilledSoundBuffs;
				}
			}
#endif

			CurFillOffset = NextFillOffset;
			TheperChanInfoPtr->CurFillOffset = CurFillOffset;
		}

		return (ui3p)TheperChanInfoPtr->dbhBufferPtr + (CurFillOffset & kBufferMask);
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
#if UseAGLfullscreen
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

#if EnableFullScreen
LOCALPROC UngrabMachine(void)
{
	GrabMachine = falseblnr;
	AdjustMachineGrab();
}
#endif

/*--- basic dialogs ---*/

LOCALFUNC CFStringRef UnicodeStrFromCStr(char *s, blnr AddEllipsis)
{
	int i;
	int L;
	ui3b ps[ClStrMaxLength];
	UniChar x[ClStrMaxLength];

	ClStrFromSubstCStr(&L, ps, s);
	if (AddEllipsis) {
		ClStrAppendChar(&L, ps, kCellEllipsis);
	}

	for (i = 0; i < L; ++i) {
		x[i] = Cell2UnicodeMap[ps[i]];
	}

	return CFStringCreateWithCharacters(kCFAllocatorDefault, x, L);
}

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

LOCALVAR blnr ADialogIsUp = falseblnr;

LOCALPROC MyBeginDialog(void)
{
	DisconnectKeyCodes3();
	ADialogIsUp = trueblnr;
#if EnableFullScreen
	UngrabMachine();
#endif
}

LOCALPROC MyEndDialog(void)
{
	ADialogIsUp = falseblnr;
	ReconnectKeyCodes3();
}

#define kMyStandardAlert 128

LOCALPROC CheckSavedMacMsg(void)
{
	if (nullpr != SavedBriefMsg) {
		MyBeginDialog();
		if (HaveMyCreateStandardAlert() && HaveMyRunStandardAlert()) {
			CFStringRef briefMsgu = UnicodeStrFromCStr(SavedBriefMsg, falseblnr);
			if (NULL != briefMsgu) {
				CFStringRef longMsgu = UnicodeStrFromCStr(SavedLongMsg, falseblnr);
				if (NULL != longMsgu) {
					DialogRef TheAlert;
					OSStatus err = MyCreateStandardAlert(
						(SavedFatalMsg) ? kAlertStopAlert : kAlertCautionAlert,
						briefMsgu, longMsgu, NULL,
						&TheAlert);
					if (noErr == err) {
						err = MyRunStandardAlert(TheAlert, NULL, NULL);
					}
					CFRelease(longMsgu);
				}
				CFRelease(briefMsgu);
			}
		}
		MyEndDialog();

		SavedBriefMsg = nullpr;
	}
}

/*--- hide/show menubar ---*/

#if EnableFullScreen
LOCALPROC My_HideMenuBar(void)
{
	if (HaveMySetSystemUIMode()) {
		(void) MySetSystemUIMode(MykUIModeAllHidden,
			MykUIOptionDisableAppleMenu
			| MykUIOptionDisableProcessSwitch
			| MykUIOptionDisableForceQuit
			| MykUIOptionDisableSessionTerminate);
	} else {
		if (IsMenuBarVisible()) {
			HideMenuBar();
		}
	}
}
#endif

#if EnableFullScreen
LOCALPROC My_ShowMenuBar(void)
{
	if (HaveMySetSystemUIMode()) {
		(void) MySetSystemUIMode(MykUIModeNormal,
			0);
	} else {
		if (! IsMenuBarVisible()) {
			ShowMenuBar();
		}
	}
}
#endif

#if IncludePbufs
LOCALVAR void *PbufDat[NumPbufs];
#endif

#if IncludePbufs
GLOBALFUNC tMacErr PbufNew(ui5b count, tPbuf *r)
{
	tPbuf i;
	void *p;
	tMacErr err = mnvm_miscErr;

	if (FirstFreePbuf(&i)) {
		p = calloc(1, count);
		if (p != NULL) {
			*r = i;
			PbufDat[i] = p;
			PbufNewNotify(i, count);

			err = mnvm_noErr;
		}
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALPROC PbufDispose(tPbuf i)
{
	free(PbufDat[i]);
	PbufDisposeNotify(i);
}
#endif

#if IncludePbufs
LOCALPROC UnInitPbufs(void)
{
	tPbuf i;

	for (i = 0; i < NumPbufs; ++i) {
		if (PbufIsAllocated(i)) {
			PbufDispose(i);
		}
	}
}
#endif

#if IncludePbufs
GLOBALPROC PbufTransfer(ui3p Buffer,
	tPbuf i, ui5r offset, ui5r count, blnr IsWrite)
{
	void *p = ((ui3p)PbufDat[i]) + offset;
	if (IsWrite) {
		(void) memcpy(p, Buffer, count);
	} else {
		(void) memcpy(Buffer, p, count);
	}
}
#endif

#if IncludeHostTextClipExchange
GLOBALFUNC tMacErr HTCEexport(tPbuf i)
{
	OSErr err;

	err = ClearCurrentScrap();
	if (noErr == err) {
		ScrapRef scrapRef;

		err = GetCurrentScrap(&scrapRef);
		if (noErr == err) {
			err = PutScrapFlavor(
				scrapRef,
				FOUR_CHAR_CODE('TEXT'),
				kScrapFlavorMaskNone,
				PbufSize[i],
				PbufDat[i]);
		}
	}

	PbufDispose(i);

	return (tMacErr)err;
}
#endif

#if IncludeHostTextClipExchange
GLOBALFUNC tMacErr HTCEimport(tPbuf *r)
{
	OSErr err;
	ScrapRef scrap;
	ScrapFlavorFlags flavorFlags;
	Size byteCount;

	err = GetCurrentScrap(&scrap);
	if (noErr == err) {
		err = GetScrapFlavorFlags(scrap,
			'TEXT', &flavorFlags);
		if (noErr == err) {
			err = (GetScrapFlavorSize(scrap,
				'TEXT', &byteCount));
			if (noErr == err) {
				tPbuf i;

				err = (OSErr)PbufNew(byteCount, &i);
				if (noErr == err) {
					Size byteCount2 = byteCount;
					err = GetScrapFlavorData(scrap,
						'TEXT', &byteCount2,
						PbufDat[i]);
					if (noErr == err) {
						if (byteCount != byteCount2) {
							err = (OSErr)mnvm_miscErr;
						}
					}
					if (noErr != err) {
						PbufDispose(i);
					} else {
						*r = i;
					}
				}
			}
		}
	}

	return (tMacErr)err;
}
#endif

/*--- drives ---*/

#define NotAfileRef (-1)

LOCALVAR SInt16 Drives[NumDrives]; /* open disk image files */

LOCALPROC InitDrives(void)
{
	/*
		This isn't really needed, Drives[i]
		need not have valid value when not vSonyIsInserted[i].
	*/
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
	}
}

GLOBALFUNC tMacErr vSonyRead(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count)
{
	ByteCount actualCount;
	OSErr err = FSReadFork(
		Drives[Drive_No],
		fsFromStart,
		Sony_Start,
		*Sony_Count,
		Buffer,
		&actualCount);

	*Sony_Count = actualCount;
	return (tMacErr)err;
}

GLOBALFUNC tMacErr vSonyWrite(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count)
{
	ByteCount actualCount;
	OSErr err = FSWriteFork(
		Drives[Drive_No],
		fsFromStart,
		Sony_Start,
		*Sony_Count,
		Buffer,
		&actualCount);

	*Sony_Count = actualCount;
	return (tMacErr)err;
}

GLOBALFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
	SInt64 forkSize;
	OSErr err = FSGetForkSize(Drives[Drive_No], &forkSize);
	*Sony_Count = forkSize;
	return (tMacErr)err;
}

GLOBALFUNC tMacErr vSonyEject(tDrive Drive_No)
{
	OSErr result;

	SInt16 refnum = Drives[Drive_No];
	Drives[Drive_No] = NotAfileRef;

	DiskEjectedNotify(Drive_No);

	result = FSCloseFork(refnum);

	return mnvm_noErr;
}

LOCALPROC UnInitDrives(void)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			(void) vSonyEject(i);
		}
	}
}

#if IncludeSonyNew
GLOBALFUNC tMacErr vSonyEjectDelete(tDrive Drive_No)
{
	FSRef ref;
	OSErr err0;
	tMacErr err;

	err0 = FSGetForkCBInfo(Drives[Drive_No], 0,
		NULL /* iterator */,
		NULL /* actualRefNum */,
		NULL /* forkInfo */,
		&ref /* ref */,
		NULL /* outForkName */);
	err = vSonyEject(Drive_No);

	if (noErr != err0) {
		err = (tMacErr)err0;
	} else {
		(void) FSDeleteObject(&ref);
	}

	return err;
}
#endif

#if IncludeSonyGetName
GLOBALFUNC tMacErr vSonyGetName(tDrive Drive_No, tPbuf *r)
{
	FSRef ref;
	HFSUniStr255 outName;
	CFStringRef DiskName;
	OSErr err;

	err = FSGetForkCBInfo(Drives[Drive_No], 0,
		NULL /* iterator */,
		NULL /* actualRefNum */,
		NULL /* forkInfo */,
		&ref /* ref */,
		NULL /* outForkName */);
	if (noErr == err) {
		err = FSGetCatalogInfo(&ref,
			kFSCatInfoNone /* whichInfo */,
			NULL /* catalogInfo */,
			&outName /* outName */,
			NULL /*fsSpec */,
			NULL /* parentRef */);
		if (noErr == err) {
			DiskName = CFStringCreateWithCharacters(
				kCFAllocatorDefault, outName.unicode,
				outName.length);
			if (NULL != DiskName) {
				tPbuf i;

				err = (OSErr)PbufNew(outName.length, &i);
				if (noErr == err) {
					if (CFStringGetBytes(DiskName,
						CFRangeMake(0, outName.length),
						kCFStringEncodingMacRoman,
						'?', false,
						PbufDat[i],
						outName.length,
						NULL) != outName.length)
					{
						err = (OSErr)mnvm_miscErr;
					}
					if (noErr != err) {
						PbufDispose(i);
					} else {
						*r = i;
					}
				}
				CFRelease(DiskName);
			}
		}
	}

	return (tMacErr)err;
}
#endif

LOCALFUNC OSErr Sony_Insert0(SInt16 refnum, blnr locked)
{
	tDrive Drive_No;

	if (! FirstFreeDisk(&Drive_No)) {
		(void) FSCloseFork(refnum);
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage, falseblnr);
		return 0xFFD6; /* too many files open */
	} else {
		Drives[Drive_No] = refnum;
		DiskInsertNotify(Drive_No, locked);
		return noErr;
	}
}

LOCALFUNC OSErr InsertADiskFromFSRef(FSRef *theRef)
{
	HFSUniStr255 forkName;
	SInt16 refnum;
	OSErr err;
	blnr locked = falseblnr;

	err = FSGetDataForkName(&forkName);
	if (noErr == err) {
		err = FSOpenFork(theRef, forkName.length,
			forkName.unicode, fsRdWrPerm, &refnum);
		switch (err) {
			case permErr:
			case wrPermErr:
			case afpAccessDenied:
				locked = trueblnr;
				err = FSOpenFork(theRef, forkName.length,
					forkName.unicode, fsRdPerm, &refnum);
				break;
			default:
				break;
		}
		if (noErr != err) {
			if (opWrErr == err) {
				MacMsg(kStrImageInUseTitle, kStrImageInUseMessage, falseblnr);
			} else {
				MacMsg(kStrOpenFailTitle, kStrOpenFailMessage, falseblnr);
			}
		} else {
			err = Sony_Insert0(refnum, locked);
		}
	}

	return err;
}

LOCALPROC InsertDisksFromDocList(AEDescList *docList)
{
	long itemsInList;
	long index;
	AEKeyword keyword;
	DescType typeCode;
	FSRef theRef;
	Size actualSize;

	if (noErr == AECountItems(docList, &itemsInList)) {
		for (index = 1; index <= itemsInList; ++index) {
			if (noErr == AEGetNthPtr(docList, index, typeFSRef, &keyword, &typeCode,
								(Ptr)&theRef, sizeof(FSRef), &actualSize))
			{
				if (noErr != InsertADiskFromFSRef(&theRef)) {
					break;
				}
			}
		}
	}
}

LOCALFUNC blnr MyMakeFSRefC(FSRef *ParentRef, char *fileName,
	blnr *Exists, blnr *isFolder, FSRef *ChildRef)
{
	int i;
	int L;
	ui3b ps[ClStrMaxLength];
	UniChar x[ClStrMaxLength];
	OSErr err;
	Boolean isFolder0;
	Boolean isAlias;

	ClStrFromSubstCStr(&L, ps, fileName);
	for (i = 0; i < L; ++i) {
		x[i] = Cell2UnicodeMap[ps[i]];
	}
	err = FSMakeFSRefUnicode(ParentRef,
		L, x, kTextEncodingUnknown,
		ChildRef);
	if (fnfErr == err) {
		*Exists = falseblnr;
		return trueblnr;
	} else if (noErr == err) {
		if (noErr == FSResolveAliasFile(ChildRef,
			TRUE, &isFolder0, &isAlias))
		{
			*isFolder = isFolder0;
			*Exists = trueblnr;
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALFUNC blnr InsertADiskFromNameEtc(FSRef *ParentRef, char *fileName)
{
	blnr Exists;
	blnr isFolder;
	FSRef ChildRef;

	if (MyMakeFSRefC(ParentRef, fileName, &Exists, &isFolder, &ChildRef))
	if (Exists && ! isFolder)
	if (noErr == InsertADiskFromFSRef(&ChildRef))
	{
		return trueblnr;
	}
	return falseblnr;
}

pascal Boolean NavigationFilterProc(AEDesc* theItem, void* info, void* NavCallBackUserData, NavFilterModes theNavFilterModes);
pascal Boolean NavigationFilterProc(AEDesc* theItem, void* info, void* NavCallBackUserData, NavFilterModes theNavFilterModes)
{
	/* NavFileOrFolderInfo* theInfo = (NavFileOrFolderInfo*)info; */
	UnusedParam(theItem);
	UnusedParam(info);
	UnusedParam(theNavFilterModes);
	UnusedParam(NavCallBackUserData);

	return true; /* display all items */
}


pascal void NavigationEventProc(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void *NavCallBackUserData);
pascal void NavigationEventProc(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void *NavCallBackUserData)
{
	UnusedParam(NavCallBackUserData);

	switch (callBackSelector) {
		case kNavCBEvent: /* probably not needed in os x */
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
			break;
#if 0
		case kNavCBUserAction:
			{
				NavUserAction userAction = NavDialogGetUserAction(
					callBackParms->context);
				switch (userAction) {
					case kNavUserActionOpen: {
						/* got something to open */
						break;
					}
				}
			}
			break;
		case kNavCBTerminate:
			break;
#endif
	}
}

LOCALPROC InsertADisk0(void)
{
	NavDialogRef theOpenDialog;
	NavDialogCreationOptions dialogOptions;
	NavReplyRecord theReply;
	NavTypeListHandle openList = NULL;
	NavEventUPP gEventProc = NewNavEventUPP(NavigationEventProc);
	NavObjectFilterUPP filterUPP = NewNavObjectFilterUPP(NavigationFilterProc);
	NavEventUPP eventUPP = NewNavEventUPP(NavigationEventProc);

	if (noErr == NavGetDefaultDialogCreationOptions(&dialogOptions)) {
		dialogOptions.modality = kWindowModalityAppModal;
		dialogOptions.optionFlags |= kNavDontAutoTranslate;
		dialogOptions.optionFlags &= ~ kNavAllowPreviews;
		if (noErr == NavCreateGetFileDialog(&dialogOptions,
			(NavTypeListHandle)openList,
			gEventProc, NULL,
			filterUPP, NULL, &theOpenDialog))
		{
			MyBeginDialog();
			(void) NavDialogRun(theOpenDialog);
			MyEndDialog();
			if (noErr == NavDialogGetReply(theOpenDialog,
				&theReply))
			{
				if (theReply.validRecord) {
					InsertDisksFromDocList(&theReply.selection);
				}
				(void) NavDisposeReply(&theReply);
			}
			NavDialogDispose(theOpenDialog);
		}
	}

	DisposeNavEventUPP(gEventProc);
	DisposeNavObjectFilterUPP(filterUPP);
	DisposeNavEventUPP(eventUPP);
}

#if IncludeSonyNew
LOCALFUNC OSErr WriteZero(SInt16 refnum, ui5b L)
{
#define ZeroBufferSize 2048
	ui5b i;
	ui3b buffer[ZeroBufferSize];
	ByteCount actualCount;
	OSErr err;
	ui5b offset = 0;

	memset(&buffer, 0, ZeroBufferSize);

	while (L > 0) {
		i = (L > ZeroBufferSize) ? ZeroBufferSize : L;
		err = FSWriteFork(refnum,
			fsFromStart,
			offset,
			i,
			buffer,
			&actualCount);
		if (noErr != err) {
			return err;
		}
		L -= i;
		offset += i;
	}

	return noErr;
}
#endif

#if IncludeSonyNew
LOCALFUNC blnr MyCreateFileCFStringRef(FSRef *saveFileParent, CFStringRef saveFileName, FSRef *NewRef)
{
	OSErr err;
	UniChar buffer[255];

	CFIndex len = CFStringGetLength(saveFileName);

	if (len > 255) {
		len = 255;
	}

	CFStringGetCharacters(saveFileName, CFRangeMake(0, len), buffer);

	err = FSMakeFSRefUnicode(saveFileParent, len, buffer, kTextEncodingUnknown, NewRef);
	if (fnfErr == err) { /* file is not there yet - create it */
		err = FSCreateFileUnicode(saveFileParent, len, buffer, 0, NULL, NewRef, NULL);
	}

	return (noErr == err);
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDisk0(FSRef *saveFileParent, CFStringRef saveFileName, ui5b L)
{
	OSErr err;
	FSRef NewRef;

	if (MyCreateFileCFStringRef(saveFileParent, saveFileName, &NewRef)) {
		HFSUniStr255 forkName;
		SInt16 refnum;

		err = FSGetDataForkName(&forkName);
		if (noErr != err) {
			/* return falseblnr; */
		} else {
			err = FSOpenFork(&NewRef, forkName.length,
				forkName.unicode, fsRdWrPerm, &refnum);
			if (noErr != err) {
				if (opWrErr == err) {
					MacMsg(kStrImageInUseTitle, kStrImageInUseMessage, falseblnr);
				} else {
					MacMsg(kStrOpenFailTitle, kStrOpenFailMessage, falseblnr);
				}
				/* return falseblnr; */
			} else {
				SInt64 forkSize = L;
				err = FSSetForkSize(refnum,  fsFromStart, forkSize);
				if (noErr == err) {
					/*
						zero out fork. It looks like this is already done,
						but documentation says this isn't guaranteed.
					*/
					err = WriteZero(refnum, L);
					if (noErr == err) {
						err = Sony_Insert0(refnum, falseblnr);
						refnum = NotAfileRef;
					}
				}
				if (NotAfileRef != refnum) {
					(void) FSCloseFork(refnum);
				}
			}
		}
		if (noErr != err) {
			(void) FSDeleteObject(&NewRef);
		}
	}
}
#endif

#if IncludeSonyNameNew
LOCALFUNC CFStringRef PbufToCFString(tPbuf i)
{
	return CFStringCreateWithBytes(NULL,
		(UInt8 *)PbufDat[i], PbufSize[i],
		kCFStringEncodingMacRoman, false);
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDisk(ui5b L, CFStringRef NewDiskName)
{
	NavDialogRef theSaveDialog;
	NavDialogCreationOptions dialogOptions;
	NavReplyRecord theReply;
	NavEventUPP gEventProc = NewNavEventUPP(NavigationEventProc);
	NavObjectFilterUPP filterUPP = NewNavObjectFilterUPP(NavigationFilterProc);
	NavEventUPP eventUPP = NewNavEventUPP(NavigationEventProc);

	if (noErr == NavGetDefaultDialogCreationOptions(&dialogOptions)) {
		dialogOptions.modality = kWindowModalityAppModal;
		dialogOptions.saveFileName = NewDiskName;
		if (noErr == NavCreatePutFileDialog(&dialogOptions,
			'TEXT', 'MPS ',
			gEventProc, NULL,
			&theSaveDialog))
		{
			MyBeginDialog();
			(void) NavDialogRun(theSaveDialog);
			MyEndDialog();
			if (noErr == NavDialogGetReply(theSaveDialog,
				&theReply))
			{
				if (theReply.validRecord) {
					long itemsInList;
					AEKeyword keyword;
					DescType typeCode;
					FSRef theRef;
					Size actualSize;

					if (noErr == AECountItems(&theReply.selection, &itemsInList)) {
						if (itemsInList == 1) {
							if (noErr == AEGetNthPtr(&theReply.selection, 1, typeFSRef, &keyword, &typeCode,
												(Ptr)&theRef, sizeof(FSRef), &actualSize))
							{
								MakeNewDisk0(&theRef, theReply.saveFileName, L);
							}
						}
					}
				}
				(void) NavDisposeReply(&theReply);
			}
			NavDialogDispose(theSaveDialog);
		}
	}

	DisposeNavEventUPP(gEventProc);
	DisposeNavObjectFilterUPP(filterUPP);
	DisposeNavEventUPP(eventUPP);

	if (NewDiskName != NULL) {
		CFRelease(NewDiskName);
	}
}
#endif

LOCALFUNC blnr AllocateMacROM(void)
{
	ROM = (ui3p)calloc(1, kROM_Size);
	if (NULL == ROM) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALFUNC blnr FindNamedChildRef(FSRef *ParentRef,
	char *ChildName, FSRef *ChildRef)
{
	blnr Exists;
	blnr isFolder;

	if (MyMakeFSRefC(ParentRef, ChildName, &Exists, &isFolder, ChildRef))
	if (Exists && isFolder)
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALVAR FSRef MyDatDirRef;

LOCALVAR CFStringRef MyAppName = NULL;

LOCALPROC UnInitMyApplInfo(void)
{
	if (MyAppName != NULL) {
		CFRelease(MyAppName);
	}
}

LOCALFUNC blnr InitMyApplInfo(void)
{
	ProcessSerialNumber currentProcess = {0, kCurrentProcess};
	FSRef fsRef;
	FSRef parentRef;

	if (noErr == GetProcessBundleLocation(&currentProcess,
		&fsRef))
	if (noErr == FSGetCatalogInfo(&fsRef, kFSCatInfoNone,
		NULL, NULL, NULL, &parentRef))
	{
		FSRef ContentsRef;
		FSRef DatRef;

		MyDatDirRef = parentRef;
		if (FindNamedChildRef(&fsRef, "Contents", &ContentsRef))
		if (FindNamedChildRef(&ContentsRef, "mnvm_dat", &DatRef))
		{
			MyDatDirRef = DatRef;
		}

		if (HaveMyLSCopyDisplayNameForRef()) {
			if (noErr == MyLSCopyDisplayNameForRef(&fsRef, &MyAppName)) {
				return trueblnr;
			}
		}

		if (noErr == CopyProcessName(&currentProcess, &MyAppName)) {
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALFUNC blnr OpenNamedFileInFolderRef(FSRef *ParentRef,
	char *fileName, short *refnum, blnr *Exists)
{
	blnr isFolder;
	FSRef ChildRef;
	HFSUniStr255 forkName;

	if (MyMakeFSRefC(ParentRef, fileName, Exists, &isFolder, &ChildRef)) {
		if (! *Exists) {
			return trueblnr;
		} else {
			if (noErr == FSGetDataForkName(&forkName))
			if (noErr == FSOpenFork(&ChildRef, forkName.length,
				forkName.unicode, fsRdPerm, refnum))
			{
				return trueblnr;
			}
		}
	}
	return falseblnr;
}

LOCALFUNC blnr LoadMacRom(void)
{
	OSErr err;
	SInt16 refnum;
	blnr Exists;
	ByteCount count = kTrueROM_Size;

	if (! OpenNamedFileInFolderRef(&MyDatDirRef, RomFileName,
		&refnum, &Exists))
	{
		MacMsg(kStrNoReadROMTitle, kStrNoReadROMMessage, trueblnr);
	} else {
		if (! Exists) {
			MacMsg(kStrNoROMTitle, kStrNoROMMessage, trueblnr);
			SpeedStopped = trueblnr;
		} else {
			ByteCount actualCount;
			err = FSReadFork(refnum, fsFromStart, 0,
				count, ROM, &actualCount);
			if (noErr != err) {
				if (eofErr == err) {
					MacMsg(kStrShortROMTitle, kStrShortROMMessage, trueblnr);
				} else {
					MacMsg(kStrNoReadROMTitle, kStrNoReadROMMessage, trueblnr);
				}
				SpeedStopped = trueblnr;
			}
			(void) FSCloseFork(refnum);
		}
		return trueblnr;
	}
	return falseblnr;
}

LOCALFUNC blnr LoadInitialImages(void)
{
	int n = NumDrives > 9 ? 9 : NumDrives;
	int i;
	char s[16] = "disk?.dsk";

	for (i = 1; i <= n; ++i) {
		s[4] = '0' + i;
		if (! InsertADiskFromNameEtc(&MyDatDirRef, s)) {
			/* stop on first error (including file not found) */
			return trueblnr;
		}
	}

	return trueblnr;
}

LOCALFUNC blnr AllocateMacRAM(void)
{
	RAM = (ui3p)calloc(1, kRAM_Size + RAMSafetyMarginFudge);
	if (NULL == RAM) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
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
	if (errAEDescNotFound == theErr) { /* No more required params. */
		return trueblnr;
	} else if (noErr == theErr) { /* More required params! */
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
	if (errAEDescNotFound == theErr) { /* No more required params. */
		return trueblnr;
	} else if (noErr == theErr) { /* More required params! */
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
			/* printIt = (openPrint == aRefCon) */
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

LOCALFUNC blnr MyInstallEventHandler(AEEventClass theAEEventClass, AEEventID theAEEventID,
						ProcPtr p, long handlerRefcon, blnr isSysHandler)
{
	return noErr == (AEInstallEventHandler(theAEEventClass, theAEEventID,
			NewAEEventHandlerUPP((AEEventHandlerProcPtr)p),
			handlerRefcon, isSysHandler));
}

LOCALPROC InstallAppleEventHandlers(void)
{
	if (noErr == AESetInteractionAllowed(kAEInteractWithLocal))
	if (MyInstallEventHandler(kCoreEventClass, kAEOpenApplication, (ProcPtr)DoOpenEvent, 0, falseblnr))
	if (MyInstallEventHandler(kCoreEventClass, kAEOpenDocuments, (ProcPtr)OpenOrPrintFiles, openOnly, falseblnr))
	if (MyInstallEventHandler(kCoreEventClass, kAEPrintDocuments, (ProcPtr)OpenOrPrintFiles, openPrint, falseblnr))
	if (MyInstallEventHandler(kCoreEventClass, kAEQuitApplication, (ProcPtr)DoQuitEvent, 0, falseblnr))
	{
	}
}

static pascal OSErr GlobalTrackingHandler(DragTrackingMessage message, WindowRef theWindow, void *handlerRefCon, DragReference theDragRef)
{
	RgnHandle hilightRgn;
	Rect Bounds;

	UnusedParam(theWindow);
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

LOCALFUNC OSErr InsertADiskOrAliasFromFSRef(FSRef *theRef)
{
	Boolean isFolder;
	Boolean isAlias;
	OSErr theErr;

	theErr = FSResolveAliasFile(theRef, true, &isFolder, &isAlias);
	if (noErr == theErr) {
		theErr = InsertADiskFromFSRef(theRef);
	}

	return theErr;
}

LOCALFUNC OSErr InsertADiskOrAliasFromSpec(FSSpec *spec)
{
	FSRef newRef;
	OSErr theErr;

	theErr = FSpMakeFSRef(spec, &newRef);
	if (noErr == theErr) {
		theErr = InsertADiskOrAliasFromFSRef(&newRef);
	}

	return theErr;
}

static pascal OSErr GlobalReceiveHandler(WindowRef pWindow, void *handlerRefCon, DragReference theDragRef)
{
	unsigned short items;
	unsigned short index;
	ItemReference theItem;
	Size SentSize;
	HFSFlavor r;

	UnusedParam(pWindow);
	UnusedParam(handlerRefCon);

	if (noErr == CountDragItems(theDragRef, &items)) {
		for (index = 1; index <= items; ++index) {
			if (noErr == GetDragItemReferenceNumber(theDragRef, index, &theItem))
			if (noErr == GetFlavorDataSize(theDragRef, theItem, flavorTypeHFS, &SentSize))
				/*
					On very old macs SentSize might only be big enough to hold the actual
					file name. Have not seen this in OS X, but still leave the check
					as '<=' instead of '=='.
				*/
			if (SentSize <= sizeof(HFSFlavor))
			if (noErr == GetFlavorData(theDragRef, theItem, flavorTypeHFS,
				(Ptr)&r, &SentSize, 0))
			if (noErr == InsertADiskOrAliasFromSpec(&r.fileSpec))
			{
			}
		}

		if (gTrueBackgroundFlag) {
			ProcessSerialNumber currentProcess = {0, kCurrentProcess};

			(void) SetFrontProcess(&currentProcess);

			WantCmdOptOnReconnect = trueblnr;
		}
	}

	return noErr;
}

LOCALVAR DragTrackingHandlerUPP gGlobalTrackingHandler = NULL;
LOCALVAR DragReceiveHandlerUPP gGlobalReceiveHandler = NULL;

LOCALPROC UnPrepareForDragging(void)
{
	if (NULL != gGlobalReceiveHandler) {
		RemoveReceiveHandler(gGlobalReceiveHandler, gMyMainWindow);
		DisposeDragReceiveHandlerUPP(gGlobalReceiveHandler);
		gGlobalReceiveHandler = NULL;
	}
	if (NULL != gGlobalTrackingHandler) {
		RemoveTrackingHandler(gGlobalTrackingHandler, gMyMainWindow);
		DisposeDragTrackingHandlerUPP(gGlobalTrackingHandler);
		gGlobalTrackingHandler = NULL;
	}
}

LOCALFUNC blnr PrepareForDragging(void)
{
	blnr IsOk = falseblnr;

	gGlobalTrackingHandler = NewDragTrackingHandlerUPP(GlobalTrackingHandler);
	if (gGlobalTrackingHandler != NULL) {
		gGlobalReceiveHandler = NewDragReceiveHandlerUPP(GlobalReceiveHandler);
		if (gGlobalReceiveHandler != NULL) {
			if (noErr == InstallTrackingHandler(gGlobalTrackingHandler, gMyMainWindow, nil)) {
				if (noErr == InstallReceiveHandler(gGlobalReceiveHandler, gMyMainWindow, nil)) {
					IsOk = trueblnr;
				}
			}
		}
	}

	return IsOk;
}

LOCALVAR EventRef EventForMyLowPriority = nil;

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

#if 0
LOCALPROC MouseMovedNotify(EventRef theEvent)
{
	Point NewMousePos;

	if (gBackgroundFlag || ADialogIsUp) {
		return;
	}

	if (noErr == GetEventParameter(theEvent,
		kEventParamMouseLocation,
		typeQDPoint,
		NULL,
		sizeof(NewMousePos),
		NULL,
		&NewMousePos))
	{
		GlobalMousePos = NewMousePos;
	}
}
#endif

LOCALVAR blnr IsOurMouseMove;

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
					if (CurSpeedStopped) {
						PostMyLowPriorityTasksEvent();
					}
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
		case kEventClassMouse:
			switch(eventKind) {
				case kEventMouseMoved:
				case kEventMouseDragged:
					/* MouseMovedNotify(theEvent); */
					MouseIsOutside = falseblnr;
					IsOurMouseMove = trueblnr;
					result = noErr;
					break;
			}
			break;
	}

	return result;
}

#if EnableScalingBuff
#define ScaleBuffSzMult (MyWindowScale * MyWindowScale)
#endif

LOCALFUNC blnr AllocateScreenCompare(void)
{
	screencomparebuff = malloc(vMacScreenNumBytes);
	if (NULL == screencomparebuff) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	}
	SetLongs((ui5b *)screencomparebuff, vMacScreenNumBytes / 4);

#if UseControlKeys
	CntrlDisplayBuff = malloc(vMacScreenNumBytes);
	if (NULL == CntrlDisplayBuff) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	}
#endif
#if EnableScalingBuff
	ScalingBuff = malloc(vMacScreenNumBytes * (
		ScaleBuffSzMult < 8 ? 8 :
		ScaleBuffSzMult
		));
	if (NULL == ScalingBuff) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	}
#endif
	return trueblnr;
}

LOCALFUNC blnr MyCreateNewWindow(Rect *Bounds, WindowPtr *theWindow)
{
	WindowPtr ResultWin;
	blnr IsOk = falseblnr;

	if (noErr == CreateNewWindow(
#if EnableFullScreen
		UseFullScreen ? kPlainWindowClass :
#endif
			kDocumentWindowClass,
#if EnableFullScreen
		UseFullScreen ? /* kWindowStandardHandlerAttribute */ 0 :
#endif
			kWindowCloseBoxAttribute
				| kWindowCollapseBoxAttribute,
		Bounds, &ResultWin
	))
	{
#if EnableFullScreen
		if (! UseFullScreen)
#endif
		{
			SetWindowTitleWithCFString(ResultWin,
				MyAppName /* CFSTR("Mini vMac") */);
		}
		InstallStandardEventHandler(GetWindowEventTarget(ResultWin));
		{
			static const EventTypeSpec windowEvents[] =
				{
					{kEventClassWindow, kEventWindowClose},
					{kEventClassWindow, kEventWindowDrawContent},
					{kEventClassWindow, kEventWindowClickContentRgn},
					{kEventClassMouse, kEventMouseMoved},
					{kEventClassMouse, kEventMouseDragged}
				};
			InstallWindowEventHandler(ResultWin,
				NewEventHandlerUPP(windowEventHandler),
				GetEventTypeCount(windowEvents),
				windowEvents, 0, NULL);
		}

		*theWindow = ResultWin;

		IsOk = trueblnr;
	}

	return IsOk;
}

LOCALPROC ZapMyWState(void)
{
	gMyMainWindow = NULL;
	window_fmt = NULL;
	window_ctx = NULL;
	gGlobalReceiveHandler = NULL;
	gGlobalTrackingHandler = NULL;
}

LOCALPROC CloseAglCurrentContext(void)
{
	if (ctx != NULL) {
		/*
			Only because MyDrawWithOpenGL doesn't
			bother to do this. No one
			uses the CurrentContext
			without settting it first.
		*/
		if (GL_TRUE != aglSetCurrentContext(NULL)) {
			/* err = aglReportError() */
		}
		ctx = NULL;
	}
}

LOCALPROC CloseMainWindow(void)
{
	UnPrepareForDragging();

	if (window_ctx != NULL) {
		if (GL_TRUE != aglDestroyContext(window_ctx)) {
			/* err = aglReportError() */
		}
		window_ctx = NULL;
	}

	if (window_fmt != NULL) {
		aglDestroyPixelFormat(window_fmt);
		window_fmt = NULL;
	}

	if (gMyMainWindow != NULL) {
		DisposeWindow(gMyMainWindow);
		gMyMainWindow = NULL;
	}
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

LOCALFUNC blnr CreateMainWindow(void)
{
	int WinIndx;
	Rect MainScrnBounds;
	Rect AllScrnBounds;
	Rect NewWinRect;
	short leftPos;
	short topPos;
	short NewWindowHeight = vMacScreenHeight;
	short NewWindowWidth = vMacScreenWidth;
	blnr IsOk = falseblnr;

#if EnableFullScreen
	UseFullScreen = WantFullScreen;
#endif
#if EnableMagnify
	UseMagnify = WantMagnify;
#endif

#if EnableFullScreen
	if (UseFullScreen) {
		My_HideMenuBar();
	} else {
		My_ShowMenuBar();
	}
#endif

	MyGetGrayRgnBounds(&AllScrnBounds);
	MyGetScreenBitsBounds(&MainScrnBounds);

#if EnableMagnify
	if (UseMagnify) {
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
	if (UseFullScreen) {
		NewWinRect = AllScrnBounds;
	} else
#endif
	{
#if EnableMagnify
		if (UseMagnify) {
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

	CurWinIndx = WinIndx;

#if EnableFullScreen
	if (UseFullScreen) {
		hOffset = MainScrnBounds.left - AllScrnBounds.left;
		vOffset = MainScrnBounds.top - AllScrnBounds.top;
	}
#endif

	if (MyCreateNewWindow(&NewWinRect, &gMyMainWindow)) {
		static const GLint window_attrib[] = {AGL_RGBA,
#if UseAGLdoublebuff
			AGL_DOUBLEBUFFER,
#endif
			/* AGL_DEPTH_SIZE, 16,  */
			AGL_NONE};

		window_fmt = aglChoosePixelFormat(NULL, 0, window_attrib);
		if (NULL == window_fmt) {
			/* err = aglReportError() */
		} else {
			window_ctx = aglCreateContext(window_fmt, NULL);
			if (NULL == window_ctx) {
				/* err = aglReportError() */
			} else {

				ShowWindow(gMyMainWindow);

				if (GL_TRUE != aglSetDrawable(window_ctx, GetWindowPort(gMyMainWindow)))
				{
					/* err = aglReportError() */
				} else {
					ctx = window_ctx;

#if EnableFullScreen
					if (UseFullScreen) {
						int h = NewWinRect.right - NewWinRect.left;
						int v = NewWinRect.bottom - NewWinRect.top;

						GLhOffset = hOffset;
						GLvOffset = v - vOffset;
						MyAdjustGLforSize(h, v);
					} else
#endif
					{
						GLhOffset = 0;
						GLvOffset = NewWindowHeight;
						MyAdjustGLforSize(NewWindowWidth, NewWindowHeight);
					}

#if EnableFullScreen
					if (! UseFullScreen)
#endif
					{
						/* check if window rect valid */
						Rect tr;

						if (MyGetWindowTitleBounds(gMyMainWindow, &tr)) {
							if (! RectInRgn(&tr, GetGrayRgn())) {
								MySetMacWindContRect(gMyMainWindow,
									&MainScrnBounds);
								if (MyGetWindowTitleBounds(gMyMainWindow, &tr)) {
									if (! RectInRgn(&tr, GetGrayRgn())) {
										OffsetRect(&MainScrnBounds, 0, AllScrnBounds.top - tr.top);
										MySetMacWindContRect(gMyMainWindow,
											&MainScrnBounds);
									}
								}
							}
						}
					}

					(void) PrepareForDragging();

					IsOk = trueblnr;

				}
			}
		}
	}

	return IsOk;
}

struct MyWState {
	WindowPtr f_MainWindow;
	AGLPixelFormat f_fmt;
	AGLContext f_ctx;
	short f_hOffset;
	short f_vOffset;
	blnr f_UseFullScreen;
	blnr f_UseMagnify;
	int f_CurWinIndx;
	short f_GLhOffset;
	short f_GLvOffset;
	DragTrackingHandlerUPP f_gGlobalTrackingHandler;
	DragReceiveHandlerUPP f_gGlobalReceiveHandler;
};
typedef struct MyWState MyWState;

LOCALPROC GetMyWState(MyWState *r)
{
	r->f_MainWindow = gMyMainWindow;
	r->f_fmt = window_fmt;
	r->f_ctx = window_ctx;
	r->f_hOffset = hOffset;
	r->f_vOffset = vOffset;
	r->f_UseFullScreen = UseFullScreen;
	r->f_UseMagnify = UseMagnify;
	r->f_CurWinIndx = CurWinIndx;
	r->f_GLhOffset = GLhOffset;
	r->f_GLvOffset = GLvOffset;
	r->f_gGlobalTrackingHandler = gGlobalTrackingHandler;
	r->f_gGlobalReceiveHandler = gGlobalReceiveHandler;
}

LOCALPROC SetMyWState(MyWState *r)
{
	gMyMainWindow = r->f_MainWindow;
	window_fmt = r->f_fmt;
	window_ctx = r->f_ctx;
	hOffset = r->f_hOffset;
	vOffset = r->f_vOffset;
	UseFullScreen = r->f_UseFullScreen;
	UseMagnify = r->f_UseMagnify;
	CurWinIndx = r->f_CurWinIndx;
	GLhOffset = r->f_GLhOffset;
	GLvOffset = r->f_GLvOffset;
	gGlobalTrackingHandler = r->f_gGlobalTrackingHandler;
	gGlobalReceiveHandler = r->f_gGlobalReceiveHandler;

	ctx = window_ctx;
}

LOCALFUNC blnr ReCreateMainWindow(void)
{
	MyWState old_state;
	MyWState new_state;

#if EnableFullScreen
	if (! UseFullScreen)
#endif
	{
		/* save old position */
		if (gMyMainWindow != NULL) {
			Rect r;

			if (MyGetWindowContBounds(gMyMainWindow, &r)) {
				WinPositionWins[CurWinIndx].h = r.left;
				WinPositionWins[CurWinIndx].v = r.top;
			}
		}
	}

#if EnableFullScreen
	UngrabMachine();
#endif

	CloseAglCurrentContext();

	GetMyWState(&old_state);

	ZapMyWState();

	if (! CreateMainWindow()) {
		CloseMainWindow();
		SetMyWState(&old_state);

#if EnableFullScreen
		if (UseFullScreen) {
			My_HideMenuBar();
		} else {
			My_ShowMenuBar();
		}
#endif

		return falseblnr;
	} else {
		GetMyWState(&new_state);
		SetMyWState(&old_state);
		CloseMainWindow();
		SetMyWState(&new_state);

#if EnableMouseMotion && EnableFullScreen
		if (HaveCursorHidden) {
			(void) MyMoveMouse(CurMouseH, CurMouseV);
			WantCursorHidden = trueblnr;
		} else
#endif
		{
			MouseIsOutside = falseblnr; /* don't know */
		}

		return trueblnr;
	}
}

#if EnableMagnify || EnableFullScreen
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
	}
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
LOCALPROC ToggleWantFullScreen(void)
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
			WantMagnify = (kMagStateMagnifgy == NewMagState);
		} else {
			WantMagnify = falseblnr;
			if (WantFullScreen
				&& HaveMyCGDisplayPixelsWide()
				&& HaveMyCGDisplayPixelsHigh())
			{
				CGDirectDisplayID CurMainDisplayID = MyMainDisplayID();

				if ((MyCGDisplayPixelsWide(CurMainDisplayID) >= vMacScreenWidth * MyWindowScale)
					&& (MyCGDisplayPixelsHigh(CurMainDisplayID) >= vMacScreenHeight * MyWindowScale)
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

LOCALVAR EventLoopTimerRef MyTimerRef;

LOCALPROC LeaveBackground(void)
{
	ReconnectKeyCodes3();
	SetCursorArrow();
}

LOCALPROC EnterBackground(void)
{
	DisconnectKeyCodes3();

#if EnableFullScreen
	if (WantFullScreen) {
		ToggleWantFullScreen();
	}
#endif
}

LOCALPROC LeaveSpeedStopped(void)
{
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Start();
#endif

	StartUpTimeAdjust();
	(void) SetEventLoopTimerNextFireTime(
		MyTimerRef, MyTickDuration);
}

LOCALPROC EnterSpeedStopped(void)
{
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
}

LOCALPROC CheckStateAfterEvents(void)
{
	if (RequestMacOff) {
		RequestMacOff = falseblnr;
		if (AnyDiskInserted()) {
			MacMsgOverride(kStrQuitWarningTitle,
				kStrQuitWarningMessage);
		} else {
			ForceMacOff = trueblnr;
		}
	}

	if (ForceMacOff) {
		QuitApplicationEventLoop();
		return;
	}

	if (gTrueBackgroundFlag != gBackgroundFlag) {
		gBackgroundFlag = gTrueBackgroundFlag;
		if (gTrueBackgroundFlag) {
			EnterBackground();
		} else {
			LeaveBackground();
		}
	}

	if (CurSpeedStopped != (SpeedStopped ||
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
	if (! (gTrueBackgroundFlag || ADialogIsUp)) {
		CheckMagnifyAndFullScreen();
	}
#endif

#if EnableFullScreen
	if (GrabMachine != (UseFullScreen && ! (gTrueBackgroundFlag || ADialogIsUp || CurSpeedStopped))) {
		GrabMachine = ! GrabMachine;
		AdjustMachineGrab();
	}
#endif

	if ((nullpr != SavedBriefMsg) & ! MacMsgDisplayed) {
		MacMsgDisplayOn();
	}

	if (NeedWholeScreenDraw) {
		NeedWholeScreenDraw = falseblnr;
		HaveChangedScreenBuff(0, 0, vMacScreenHeight, vMacScreenWidth);
	}

	if (gTrueBackgroundFlag || ADialogIsUp) {
		/*
			dialog during drag and drop hangs if in background
				and don't want recursive dialogs
			so wait til later to display dialog
		*/
	} else {
#if IncludeSonyNew
		if (vSonyNewDiskWanted) {
#if IncludeSonyNameNew
			if (vSonyNewDiskName != NotAPbuf) {
				MakeNewDisk(vSonyNewDiskSize, PbufToCFString(vSonyNewDiskName));
				PbufDispose(vSonyNewDiskName);
				vSonyNewDiskName = NotAPbuf;
			} else
#endif
			{
				MakeNewDisk(vSonyNewDiskSize, NULL);
			}
			vSonyNewDiskWanted = falseblnr;
				/* must be done after may have gotten disk */
		}
#endif
		if (RequestInsertDisk) {
			RequestInsertDisk = falseblnr;
			InsertADisk0();

			if (CurSpeedStopped) {
				PostMyLowPriorityTasksEvent();
			}
		}

		/* CheckSavedMacMsg(); */
	}
}

#if 0
typedef boolean_t (*CGCursorIsVisibleProcPtr)(void);

LOCALVAR CGCursorIsVisibleProcPtr MyCGCursorIsVisible = NULL;
LOCALVAR blnr DidCGCursorIsVisible = falseblnr;

LOCALFUNC blnr HaveMyCGCursorIsVisible(void)
{
	if (! DidCGCursorIsVisible) {
		if (HaveApplicationServicesBun()) {
			MyCGCursorIsVisible =
				(CGCursorIsVisibleProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGCursorIsVisible"));
		}
		DidCGCursorIsVisible = trueblnr;
	}
	return (MyCGCursorIsVisible != NULL);
}
#endif

LOCALPROC DoNotInBackgroundTasks(void)
{
#if 0
	if (HaveMyCGCursorIsVisible()) {
		HaveCursorHidden = ! MyCGCursorIsVisible();

		/*
			This shouldn't be needed, but have seen
			cursor state get messed up in 10.4.
			If the os x hide count gets off, this
			should fix it within a few ticks.

			oops, no, doesn't seem to be accurate,
			and makes things worse. particularly if
			cursor in obscured state after typing
			into dialog.
		*/
	}
#endif

	if (HaveCursorHidden != ((WantCursorHidden
#if EnableFullScreen
		|| UseFullScreen
#endif
		) && ! (gTrueBackgroundFlag || ADialogIsUp || CurSpeedStopped)))
	{
		HaveCursorHidden = ! HaveCursorHidden;
		if (HaveCursorHidden) {
			MyHideCursor();
		} else {
			/*
				kludge for OS X, where mouse over Dock devider
				changes cursor, and never sets it back.
			*/
			SetCursorArrow();

			MyShowCursor();
		}
	}
}

LOCALPROC CheckForSavedTasks(void)
{
	CheckStateAfterEvents();
	if (! (gTrueBackgroundFlag || ADialogIsUp)) {
		DoNotInBackgroundTasks();
	}
}

LOCALVAR ui5b OnTrueTime = 0;

GLOBALFUNC blnr ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	return TrueEmulatedTime == OnTrueTime;
}

LOCALPROC RunEmulatedTicksToTrueTime(void)
{
	si3b n;

	if (CheckDateTime()) {
#if MySoundEnabled
		MySound_SecondNotify();
#endif
	}

	n = OnTrueTime - CurEmulatedTime;
	if (n > 0) {
		if (n > 8) {
			/* emulation not fast enough */
			n = 8;
			CurEmulatedTime = OnTrueTime - n;
		}

		if (! (gBackgroundFlag || ADialogIsUp)) {
			CheckMouseState();
		}

		do {
			DoEmulateOneTick();
			++CurEmulatedTime;
		} while (ExtraTimeNotOver()
			&& (--n > 0));

		Screen_Draw(n);
	}
}

static pascal void MyTimerProc(EventLoopTimerRef inTimer,
	void *inUserData)
{
	UnusedParam(inUserData);

	if (CurSpeedStopped) {
		(void) SetEventLoopTimerNextFireTime(
			inTimer, kEventDurationForever);
	} else {
		UpdateTrueEmulatedTime();
		{
			EventTimeout inTimeout = NextTickChangeTime - GetCurrentEventTime();
			if (inTimeout > 0.0) {
				(void) SetEventLoopTimerNextFireTime(
					inTimer, inTimeout);
				/*
					already a periodic timer, this is just to make
					sure of accuracy.
				*/
			}
		}

		OnTrueTime = TrueEmulatedTime;
		RunEmulatedTicksToTrueTime();

		PostMyLowPriorityTasksEvent();
	}
}

LOCALFUNC blnr InitMyLowPriorityTasksEvent(void)
{
	if (noErr != InstallEventLoopTimer(GetMainEventLoop(),
		kEventDurationMinute, /* will set actual time later */
		MyTickDuration,
		NewEventLoopTimerUPP(MyTimerProc),
		NULL, &MyTimerRef))
	{
		return falseblnr;
	}

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

#define CheckItem CheckMenuItem

/* Menu Constants */

#define kAppleMenu   128
#define kFileMenu    129
#define kSpecialMenu 130

enum {
	kCmdIdNull,
	kCmdIdMoreCommands,

	kNumCmdIds
};

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

LOCALFUNC OSStatus Keyboard_UpdateKeyMap3(EventRef theEvent, blnr down)
{
	UInt32 uiKeyCode;

	GetEventParameter(theEvent, kEventParamKeyCode, typeUInt32, NULL, sizeof(uiKeyCode), NULL, &uiKeyCode);
	Keyboard_UpdateKeyMap2(uiKeyCode & 0x000000FF, down);
	return noErr;
}

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
				case kEventMouseMoved:
				case kEventMouseDragged:
					IsOurMouseMove = falseblnr;
					result = CallNextEventHandler(nextHandler, theEvent);
						/*
							Actually, mousing over window does't seem
							to go through here, it goes directly to
							the window routine. But since not documented
							either way, leave the check in case this
							changes.
						*/
					if (! IsOurMouseMove) {
						MouseIsOutside = trueblnr;
						/* MouseMovedNotify(theEvent); */
					}
					break;
			}
			break;
		case kEventClassApplication:
			switch (eventKind) {
				case kEventAppActivated:
					gTrueBackgroundFlag = falseblnr;
					result = noErr;
					if (CurSpeedStopped) {
						PostMyLowPriorityTasksEvent();
					}
					break;
				case kEventAppDeactivated:
					gTrueBackgroundFlag = trueblnr;
					result = noErr;
					if (CurSpeedStopped) {
						PostMyLowPriorityTasksEvent();
					}
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
						if (CurSpeedStopped) {
							PostMyLowPriorityTasksEvent();
						}
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
						if (CurSpeedStopped) {
							PostMyLowPriorityTasksEvent();
						}
						break;
					case kEventRawKeyUp:
						result = Keyboard_UpdateKeyMap3(theEvent, falseblnr);
						if (CurSpeedStopped) {
							PostMyLowPriorityTasksEvent();
						}
						break;
					case kEventRawKeyModifiersChanged:
						{
							UInt32 theModifiers;

							GetEventParameter(theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(typeUInt32), NULL, &theModifiers);

							MyUpdateKeyboardModifiers(theModifiers);
						}
						result = noErr;
						if (CurSpeedStopped) {
							PostMyLowPriorityTasksEvent();
						}
						break;
					default:
						break;
				}
			}
			break;
		case 'MnvM':
			if ('MnvM' == eventKind) {
				CheckForSavedTasks();
				if (! CurSpeedStopped) {
					DoEmulateExtraTime();
				}
			}
			break;
		default:
			break;
	}
	return result;
}

LOCALPROC AppendMenuConvertCStr(
	MenuRef menu,
	MenuCommand inCommandID,
	char *s, blnr WantEllipsis)
{
	CFStringRef cfStr = UnicodeStrFromCStr(s, WantEllipsis);
	if (NULL != cfStr) {
		AppendMenuItemTextWithCFString(menu, cfStr,
			0, inCommandID, NULL);
	}
}

LOCALPROC AppendMenuSep(MenuRef menu)
{
	AppendMenuItemTextWithCFString(menu, NULL,
		kMenuItemAttrSeparator, 0, NULL);
}

LOCALFUNC MenuRef NewMenuFromConvertCStr(short menuID, char *s)
{
	MenuRef menu = NULL;

	CFStringRef cfStr = UnicodeStrFromCStr(s, falseblnr);
	if (NULL != cfStr) {
		OSStatus err = CreateNewMenu(menuID, 0, &menu);
		if (err != noErr) {
			menu = NULL;
		} else {
			(void) SetMenuTitleWithCFString(menu, cfStr);
		}
		CFRelease(cfStr);
	}

	return menu;
}

LOCALPROC RemoveCommandKeysInMenu(MenuRef theMenu)
{
	MenuRef outHierMenu;
	int i;
	UInt16 n = CountMenuItems(theMenu);

	for (i = 1; i <= n; ++i) {
		SetItemCmd(theMenu, i, 0);
		if (noErr == GetMenuItemHierarchicalMenu(
			theMenu, i, &outHierMenu)
			&& (NULL != outHierMenu))
		{
			RemoveCommandKeysInMenu(outHierMenu);
		}
	}
}

LOCALFUNC blnr InstallOurMenus(void)
{
	MenuRef menu;
	Str255 s;

	PStrFromChar(s, (char)20);
	menu = NewMenu(kAppleMenu, s);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kHICommandAbout,
			kStrMenuItemAbout,
			falseblnr
			);
		AppendMenuSep(menu);
		InsertMenu(menu, 0);
	}

	menu = NewMenuFromConvertCStr(kFileMenu, kStrMenuFile);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kHICommandOpen,
			kStrMenuItemOpen, trueblnr);
		InsertMenu(menu, 0);
	}

	menu = NewMenuFromConvertCStr(kSpecialMenu, kStrMenuSpecial);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kCmdIdMoreCommands,
			kStrMenuItemMore, trueblnr);
		InsertMenu(menu, 0);
	}

	{
		MenuRef outMenu;
		MenuItemIndex outIndex;

		if (noErr == GetIndMenuItemWithCommandID(
			NULL, kHICommandQuit, 1, &outMenu, &outIndex))
		{
			RemoveCommandKeysInMenu(outMenu);
		}
	}

	DrawMenuBar();

	return trueblnr;
}

LOCALFUNC blnr InstallOurAppearanceClient(void)
{
	RegisterAppearanceClient(); /* maybe not needed ? */
	return trueblnr;
}

LOCALFUNC blnr InstallOurEventHandlers(void)
{
	EventTypeSpec eventTypes[] = {
		{kEventClassMouse, kEventMouseDown},
		{kEventClassMouse, kEventMouseUp},
		{kEventClassMouse, kEventMouseMoved},
		{kEventClassMouse, kEventMouseDragged},
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

	InstallAppleEventHandlers();

	/* (void) SetMouseCoalescingEnabled(false, NULL); */

	return trueblnr;
}

LOCALPROC ZapOSGLUVars(void)
{
	InitDrives();
	{
		int i;

		for (i = 0; i < kNumMagStates; ++i) {
			HavePositionWins[i] = falseblnr;
		}
#if EnableFullScreen && EnableMagnify
		for (i = 0; i < kNumWinStates; ++i) {
			WinMagStates[i] = kMagStateAuto;
		}
#endif
	}
}

LOCALFUNC blnr InitOSGLU(void)
{
	if (InitMyApplInfo())
	if (InstallOurAppearanceClient())
	if (InstallOurEventHandlers())
	if (InstallOurMenus())
#if MySoundEnabled
	if (MySound_Init())
#endif
	if (AllocateScreenCompare())
	if (CreateMainWindow())
	if (AllocateMacROM())
	if (LoadMacRom())
	if (LoadInitialImages())
#if MakeDumpFile
	if (StartDump())
#endif
	if (AllocateMacRAM())
	if (InitLocationDat())
	if (InitMyLowPriorityTasksEvent())
	if (InitEmulation())
	{
		return trueblnr;
	}
	return falseblnr;
}

#if MakeDumpFile
IMPORTPROC DoDumpTable(void);
#endif

LOCALPROC UnInitOSGLU(void)
{
#if MakeDumpFile
	DoDumpTable();
#endif

	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}

	if (! gTrueBackgroundFlag) {
		CheckSavedMacMsg();
	}

#if EnableFullScreen
	UngrabMachine();
#endif

	CloseAglCurrentContext();
	CloseMainWindow();
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

	UnInitMyApplInfo();

#if IncludePbufs
	UnInitPbufs();
#endif
	UnInitDrives();

	ForceShowCursor();
}

int main(void)
{
	ZapOSGLUVars();
	if (InitOSGLU()) {
		RunApplicationEventLoop();
	}
	UnInitOSGLU();

	return 0;
}
