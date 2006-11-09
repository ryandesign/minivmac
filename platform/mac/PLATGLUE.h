/*
	PLATGLUE.h

	Copyright (C) 2006 Philip Cummins, Richard F. Bannister, Paul C. Pratt

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
	PLATform GLUE. (for Macintosh)

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

#if defined(__SC__) || ((defined(powerc) || defined(__powerc)) && ! defined(__MWERKS__))
/* GLOBALVAR */ QDGlobals qd;
#endif

LOCALFUNC blnr InitMacManagers(void)
{
	MaxApplZone();

	{
		int i;

		for (i = 7; --i >= 0; ) {
			MoreMasters();
		}
	}

	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(NULL);
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

LOCALPROC PStrFromHandle(ps3p r, Handle h, ui5b MaxL)
{
	ui5b L = GetHandleSize(h);

	if (L > MaxL) {
		L = MaxL;
	}

	*r++ = L;
	BlockMove(*h, (Ptr)r, L);
}

LOCALFUNC OSErr PStrToHand(ps3p s, Handle *r)
{
	return PtrToHand((Ptr)(s + 1), r, s[0]);
}

/*--- control mode and internationalization ---*/

#define NeedRequestInsertDisk 1
#define NeedDoMoreCommandsMsg 1
#define NeedDoAboutMsg 1

#define NeedCell2MacAsciiMap 1

#include "CONTROLM.h"

/*--- information about the environment ---*/

#ifndef HaveCPUfamM68K
#define HaveCPUfamM68K 0
#endif

LOCALFUNC TrapType GetTrapType(short theTrap)
{
	if ((theTrap & 0x0800) != 0) {
		return ToolTrap;
	} else {
		return OSTrap;
	}
}

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

	return trueblnr;
}

/*--- utilities for adapting to the environment ---*/

#ifndef Windows85APIAvail
#define Windows85APIAvail 1
#endif

#ifndef MightNotHaveAppearanceMgrAvail
#define MightNotHaveAppearanceMgrAvail 1
#endif

#ifndef MightNotHaveWindows85Avail
#define MightNotHaveWindows85Avail MightNotHaveAppearanceMgrAvail
#endif

#ifndef LowMemAPIAvail
#define LowMemAPIAvail 1
#endif

#ifndef My_LMGetTime
#if LowMemAPIAvail
#define My_LMGetTime LMGetTime
#else
#define My_LMGetTime() (*(SInt32 *)(0x020C))
#endif
#endif

#ifndef My_LMGetMBarHeight
#if LowMemAPIAvail
#define My_LMGetMBarHeight LMGetMBarHeight
#else
#define My_LMGetMBarHeight() (*(short *)(0x0BAA))
#endif
#endif

#ifndef My_GetGrayRgn
#if /* LowMemAPIAvail */ 0
#define My_GetGrayRgn LMGetGrayRgn
#else
#define My_GetGrayRgn() (*(RgnHandle *)(0x9EE))
#endif
#endif

#ifndef My_LMGetCurApName
#if LowMemAPIAvail
#define My_LMGetCurApName LMGetCurApName
#else
#define My_LMGetCurApName() ((StringPtr) 0x0910)
#endif
#endif

#define MyGetScreenBitsBounds(r) (*r) = qd.screenBits.bounds

#define My_GetRegionBounds(region, bounds) *(bounds) = (**(region)).rgnBBox

#define My_GetPortPixMap(p) ((p)->portPixMap)

#ifndef My_WindowRef
#if NewNamesAvail
#define My_WindowRef WindowRef
#else
#define My_WindowRef WindowPtr
#endif
#endif

#define My_SetPortWindowPort(w) SetPort(w)

LOCALPROC My_InvalWindowRect(My_WindowRef mw, Rect *r)
{
	GrafPtr SavePort;

	GetPort(&SavePort);
	My_SetPortWindowPort(mw);
	InvalRect(r);
	SetPort(SavePort);
}

#define My_GetWindowPortBounds(w, r) *(r) = ((w)->portRect)

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

#define EnableScalingBuff (1 && EnableMagnify && (MyWindowScale == 2))

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

	src.baseAddr = GetCurDrawBuff();
	src.rowBytes = vMacScreenByteWidth;
	SetRect(&src.bounds, 0, 0, vMacScreenWidth, vMacScreenHeight);
#if EnableScalingBuff && EnableMagnify
	if (UseMagnify) {
		int i;
		int j;
		int k;
#if BigEndianUnaligned
		ui5b *p1 = (ui5b *)src.baseAddr + vMacScreenWidth / 32 * top;
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
#else
		ui3b *p1 = ((ui3b *)src.baseAddr) + top * (vMacScreenWidth >> 3);
		ui3b *p2 = (ui3b *)ScalingBuff + MyWindowScale * MyWindowScale * vMacScreenWidth / 8 * top;
		ui3b *p3;
		ui3b t0;
		ui3b t1;
		ui3b t2;
		ui3b m;

		for (i = bottom - top; --i >= 0; ) {
			p3 = p2;
			for (j = vMacScreenWidth / 8; --j >= 0; ) {
				t0 = *p1++;
				t1 = t0;
				m = 0x80;
				t2 = 0;
				for (k = 4; --k >= 0; ) {
					t2 |= t1 & m;
					t1 >>= 1;
					m >>= 2;
				}
				*p2++ = t2 | (t2 >> 1);

				t1 = t0 << 4;
				m = 0x80;
				t2 = 0;
				for (k = 4; --k >= 0; ) {
					t2 |= t1 & m;
					t1 >>= 1;
					m >>= 2;
				}
				*p2++ = t2 | (t2 >> 1);
			}
			for (j = MyScaledWidth / 8; --j >= 0; ) {
				*p2++ = *p3++;
			}
		}
#endif

		MyScaleRect(&SrcRect);
		MyScaleRect(&src.bounds);

		src.baseAddr = ScalingBuff;
		src.rowBytes *= MyWindowScale;
	}
#endif
	SetScrnRectFromCoords(&DstRect, top, left, bottom, right);
	CopyBits(&src,
		&gMyMainWindow->portBits,
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
		PaintRect(&gMyMainWindow->portRect);
	}
#endif
	DefaultDrawScreenBuff(0, 0, vMacScreenHeight, vMacScreenWidth);
	SetPort(savePort);
}

LOCALPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right)
{
#if 0 /* experimental code in progress */
	if (UseFullScreen)
	{
		{
			PixMapHandle pm= (**GetMainDevice()).gdPMap;

			/* LockPixels(pm); */
#if EnableMagnify
			if (! UseMagnify) {
#define PixelT ui5b
				PixelT *p1 = (PixelT *)GetPixBaseAddr(pm);
				int i;
				int j;
				int k;
				ui5b *p0 = (ui5b *)GetCurDrawBuff();
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
				ui5b *p0 = (ui5b *)GetCurDrawBuff();
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

LOCALVAR ui5b LastEmKeys[4];

LOCALPROC ZapEmKeys(void)
{
	LastEmKeys[0] = 0;
	LastEmKeys[1] = 0;
	LastEmKeys[2] = 0;
	LastEmKeys[3] = 0;
}

LOCALPROC InitKeyCodes(void)
{
	theKeys[0] = 0;
	theKeys[1] = 0;
	theKeys[2] = 0;
	theKeys[3] = 0;

	Keyboard_UpdateKeyMap(MKC_Control, ControlKeyPressed);

	((ui3b *)LastEmKeys)[(MKC_CapsLock) / 8] &= ~ (1 << ((MKC_CapsLock) & 7));
}

LOCALPROC CheckKeyBoardState(void)
{
	int i;
	int j;
	ui5b NewKeys[4];

	GetKeys(*(KeyMap *)NewKeys);

	for (j = 0; j < 16; ++j) {
		ui3b k1 = ((ui3b *)NewKeys)[j];
		ui3b k2 = ((ui3b *)LastEmKeys)[j];
		ui3b k3 = k1 ^ k2;

		if (k3 != 0) {
			for (i = 0; i < 8; ++i) {
				if ((k3 & (1 << i)) != 0) {
					Keyboard_UpdateKeyMap2(j * 8 + i,
						(k1 & (1 << i)) != 0);
				}
			}
		}
	}
	for (i = 0; i < 4; ++i) {
		LastEmKeys[i] = NewKeys[i];
	}
}

/*--- cursor hiding ---*/

LOCALVAR blnr HaveCursorHidden = falseblnr;
LOCALVAR blnr WantCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		ShowCursor();
	}
}

/*--- cursor moving ---*/

LOCALPROC SetCursorArrow(void)
{
	SetCursor(&qd.arrow);
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

#if ! TARGET_CPU_PPC
pascal void CallCursorTask(void) =
{
	0x2078, 0x08EE,  /* MOVE.L jCrsrTask,A0 */
	0x4E90           /* JSR (A0) */
};
#endif

/*
	Low memory globals for the mouse
*/

#define MyRawMouse   0x082C  /* low memory global that has current mouse loc */
#define MyMTemp      0x0828  /* low memory global that has current mouse loc */
#define MyCrsrNew    0x08CE  /* set after you change mtemp and rawmouse */
#define MyCrsrCouple 0x08CF  /* true if the cursor is tied to the mouse */

#if EnableMouseMotion && EnableFullScreen
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

		GetMouse(&NewMousePos);
		IsOk = (h == NewMousePos.h) && (v == NewMousePos.v);
		difftime = (ui5b)(TickCount() - StartTime);
	} while ((! IsOk) && (difftime < 5));

	SetPort(oldPort);
	return IsOk;
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

LOCALVAR blnr CurTrueMouseButton = falseblnr;

LOCALPROC CheckMouseState(void)
{
	blnr ShouldHaveCursorHidden;
	ui3b NewMouseButton;
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
		if (! PtInRgn(NewMousePos, gMyMainWindow->visRgn)) {
			ShouldHaveCursorHidden = falseblnr;
		}
	}

	SetPort(oldPort);

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

	WantCursorHidden = ShouldHaveCursorHidden;
}


/*--- time, date, location ---*/

/*
	be sure to avoid getting confused if TickCount
	overflows and wraps.
*/

LOCALVAR ui5b TrueEmulatedTime = 0;
LOCALVAR ui5b CurEmulatedTime = 0;

LOCALVAR long int LastTime;

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
	LastTime = TickCount();
}

LOCALPROC UpdateTrueEmulatedTime(void)
{
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
}

LOCALFUNC blnr CheckDateTime(void)
{
	ui5b NewMacDateInSecond;

	NewMacDateInSecond = My_LMGetTime();
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

		if (FilledSoundBuffs == 0) {
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
#if 0
	AdjustMainScreenGrab();
#endif
#if MySoundEnabled && MySoundFullScreenOnly
	AdjustSoundGrab();
#endif
}
#endif

/*--- basic dialogs ---*/

LOCALPROC NativeStrFromCStr(ps3p r, char *s, blnr AddEllipsis)
{
	int i;
	int L;
	ui3b ps[ClStrMaxLength];

	ClStrFromSubstCStr(&L, ps, s);
	if (AddEllipsis) {
		ClStrAppendChar(&L, ps, kCellEllipsis);
	}

	if (L > 255) {
		L = 255;
	}

	for (i = 0; i < L; ++i) {
		r[i + 1] = Cell2MacAsciiMap[ps[i]];
	}

	r[0] = L;
}

#ifndef HogCPU
#define HogCPU 1
#endif

#if HogCPU
LOCALVAR long NoEventsCounter = 0;
#endif

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

LOCALVAR blnr ADialogIsUp = falseblnr;

LOCALPROC MyBeginDialog(void)
{
	Keyboard_UpdateControlKey(falseblnr);

	KeyBoardAttachedToEm = falseblnr;
	ADialogIsUp = trueblnr;
#if EnableFullScreen
	GrabMachine = falseblnr;
	AdjustMachineGrab();
#endif
	ForceShowCursor();
}

LOCALPROC MyEndDialog(void)
{
#if HogCPU
	NoEventsCounter = 0;
#endif
	ADialogIsUp = falseblnr;
}


#define kMyStandardAlert 128

LOCALPROC CheckSavedMacMsg(void)
{
	Str255 briefMsgp;
	Str255 longMsgp;

	if (nullpr != SavedBriefMsg) {
		MyBeginDialog();
		NativeStrFromCStr(briefMsgp, SavedBriefMsg, falseblnr);
		NativeStrFromCStr(longMsgp, SavedLongMsg, falseblnr);
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

			StandardAlert((SavedFatalMsg) ? kAlertStopAlert : kAlertCautionAlert, briefMsgp, longMsgp, &param, &itemHit);
		} else
#endif
		{
			ParamText(briefMsgp, longMsgp, "\p", "\p");
			if (SavedFatalMsg) {
				while (StopAlert(kMyStandardAlert, NULL) != 1) {
				}
			} else {
				while (CautionAlert(kMyStandardAlert, NULL) != 1) {
				}
			}
			/* Alert (kMyStandardAlert, 0L); */
		}
		MyEndDialog();

		SavedBriefMsg = nullpr;
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

#if IncludePbufs
LOCALVAR Handle PbufDat[NumPbufs];
#endif

#if IncludePbufs
LOCALFUNC si4b PbufNewFromHandle(Handle h, ui5b count, ui4b *r)
{
	ui4b i;
	OSErr err;

	if (! FirstFreePbuf(&i)) {
		DisposeHandle(h);
		err = -1;
	} else {
		*r = i;
		PbufDat[i] = h;
		PbufNewNotify(i, count);
		err = noErr;
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALFUNC si4b PbufNew(ui5b count, ui4b *r)
{
	Handle h;
	si4b err = -1;

	h = NewHandleClear(count);
	if (h != NULL) {
		err = PbufNewFromHandle(h, count, r);
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALPROC PbufDispose(ui4b i)
{
	DisposeHandle(PbufDat[i]);
	PbufDisposeNotify(i);
}
#endif

#if IncludePbufs
LOCALPROC UnInitPbufs(void)
{
	si4b i;

	for (i = 0; i < NumPbufs; ++i) {
		if (PbufIsAllocated(i)) {
			PbufDispose(i);
		}
	}
}
#endif

#if IncludePbufs
GLOBALPROC PbufTransfer(void *Buffer,
	ui4b i, ui5b offset, ui5b count, blnr IsWrite)
{
	Handle h = PbufDat[i];

	HLock(h);
	{
		void *p = ((ui3p)*h) + offset;
		if (IsWrite) {
			BlockMove(Buffer, p, count);
		} else {
			BlockMove(p, Buffer, count);
		}
	}
	HUnlock(h);
}
#endif

#if IncludeHostTextClipExchange
GLOBALFUNC si4b HTCEexport(ui4b i)
{
	OSErr err;

	err = ZeroScrap();
	if (noErr == err) {
		ui5b L = PbufSize[i];
		Handle h = PbufDat[i];
		HLock(h);
		err = PutScrap(L, 'TEXT', *h);
		HUnlock(h);
	}

	PbufDispose(i);

	return err;
}
#endif

#if IncludeHostTextClipExchange
GLOBALFUNC si4b HTCEimport(ui4b *r)
{
	long off;
	long v;
	Handle h;
	OSErr err = -1;

	h = NewHandle(0);
	if (h != NULL) {
		v = GetScrap(h, 'TEXT', &off);
		if (v < 0) {
			err = v;
		} else {
			err = PbufNewFromHandle(h, v, r);
			h = NULL;
		}
		if (NULL != h) {
			DisposeHandle(h);
		}
	}

	return err;
}
#endif

/*--- drives ---*/

#define NotAfileRef (-1)

LOCALVAR short Drives[NumDrives]; /* open disk image files */
#if (IncludeSonyGetName || IncludeSonyNew) && HaveCPUfamM68K
LOCALVAR Handle DriveNames[NumDrives];
#endif

LOCALPROC InitDrives(void)
{
	/*
		This isn't really needed, Drives[i] and DriveNames[i]
		need not have valid values when not vSonyIsInserted[i].
	*/
	si4b i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
#if (IncludeSonyGetName || IncludeSonyNew) && HaveCPUfamM68K
		DriveNames[i] = NULL;
#endif
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

LOCALFUNC si4b vSonyEject0(ui4b Drive_No)
{
	short refnum = Drives[Drive_No];
	Drives[Drive_No] = NotAfileRef; /* not really needed */

	DiskEjectedNotify(Drive_No);

#if (IncludeSonyGetName || IncludeSonyNew) && HaveCPUfamM68K
	{
		Handle h = DriveNames[Drive_No];
		if (NULL != h) {
			DisposeHandle(h);
			DriveNames[Drive_No] = NULL; /* not really needed */
		}
	}
#endif

	return FSClose(refnum);
}

GLOBALFUNC si4b vSonyEject(ui4b Drive_No)
{
	OSErr result;
	short vRefNum;
	blnr DidEject = falseblnr;
	short refnum = Drives[Drive_No];

	result = GetVRefNum(refnum, &vRefNum);
	if (noErr == result) {
		DidEject = trueblnr;
		result = vSonyEject0(Drive_No);
		(void) FlushVol(NULL, vRefNum);
	}

	if (! DidEject) {
		result = vSonyEject0(Drive_No);
	}

	return result;
}

#if IncludeSonyNew
GLOBALFUNC si4b vSonyEjectDelete(ui4b Drive_No)
{
	OSErr result;
	Str255 s;
	blnr DidEject = falseblnr;
	short refnum = Drives[Drive_No];

#if HaveCPUfamM68K
	if (! MyEnvrAttrFSSpecCallsAvail) {
		Handle h = DriveNames[Drive_No];
		if (NULL != h) {
			short vRefNum;
			result = GetVRefNum(refnum, &vRefNum);
			if (noErr == result) {
				PStrFromHandle(s, h, 255);
				result = vSonyEject0(Drive_No);
				DidEject = trueblnr;
				(void) FSDelete(s, vRefNum);
			}
		}
	} else
#endif
	{
		FCBPBRec b;

		b.ioCompletion = NULL;
		b.ioNamePtr = (StringPtr)s;
		b.ioVRefNum = 0;
		b.ioRefNum = refnum;
		b.ioFCBIndx = 0;
		result = PBGetFCBInfoSync(&b);
		if (noErr == result) {
			FSSpec spec;
			result = FSMakeFSSpec(b.ioFCBVRefNum, b.ioFCBParID, s, &spec);
			if (noErr == result) {
				result = vSonyEject0(Drive_No);
				DidEject = trueblnr;
				(void) FSpDelete(&spec);
			}
		}
	}

	if (! DidEject) {
		(void) vSonyEject0(Drive_No);
	}

	return result;
}
#endif

LOCALPROC UnInitDrives(void)
{
	si4b i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			(void) vSonyEject(i);
		}
	}
}

#if IncludeSonyGetName
GLOBALFUNC si4b vSonyGetName(ui4b Drive_No, ui4b *r)
{
	FCBPBRec b;
	Str255 s;
	OSErr err = -1;

#if HaveCPUfamM68K
	if (! MyEnvrAttrFSSpecCallsAvail) {
		Handle h = DriveNames[Drive_No];
		if (NULL != h) {
			PStrFromHandle(s, h, 255);
			err = noErr;
		}
	} else
#endif
	{
		b.ioCompletion = NULL;
		b.ioNamePtr = (StringPtr)s;
		b.ioVRefNum = 0;
		b.ioRefNum = Drives[Drive_No];
		b.ioFCBIndx = 0;
		err = PBGetFCBInfoSync(&b);
	}

	if (noErr == err) {
		Handle h;
		err = PStrToHand(s, &h);
		if (noErr == err) {
			err = PbufNewFromHandle(h, s[0], r);
		}
	}

	return err;
}
#endif

LOCALFUNC OSErr Sony_Insert0(short refnum, blnr locked, ps3p s)
{
	ui4b Drive_No;

#if ! ((IncludeSonyGetName || IncludeSonyNew) && HaveCPUfamM68K)
	UnusedParam(s);
#endif

	if (! FirstFreeDisk(&Drive_No)) {
		(void) FSClose(refnum);
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage, falseblnr);
		return 0xFFD6; /* too many files open */
	} else {
		Drives[Drive_No] = refnum;
		DiskInsertNotify(Drive_No, locked);
#if (IncludeSonyGetName || IncludeSonyNew) && HaveCPUfamM68K
		if (s != NULL) {
			Handle h;

			if (noErr != PStrToHand(s, &h)) {
				h = NULL;
			}
			DriveNames[Drive_No] = h;
		}
#endif
		return noErr;
	}
}

LOCALFUNC OSErr InsertADiskFromFileRef(FSSpec *spec)
{
	short refnum;
	OSErr err;
	blnr locked = falseblnr;

	err = FSpOpenDF(spec, fsRdWrPerm, &refnum);
	switch (err) {
		case permErr:
		case wrPermErr:
		case afpAccessDenied:
			locked = trueblnr;
			err = FSpOpenDF(spec, fsRdPerm, &refnum);
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
		err = Sony_Insert0(refnum, locked, NULL);
	}

	return err;
}

#if HaveCPUfamM68K
LOCALFUNC OSErr InsertADiskFromNamevRef(ConstStr255Param fileName, short vRefNum)
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
	switch (err) {
		case permErr:
		case wrPermErr:
		case afpAccessDenied:
			locked = trueblnr;
			R.ioParam.ioPermssn = fsRdPerm;
			err = PBOpen(&R, false);
			break;
		default:
			break;
	}
	if (noErr != err) {
		/* report this */
#if 1
		if (opWrErr == err) {
			MacMsg(kStrImageInUseTitle, kStrImageInUseMessage, falseblnr);
		}
#endif
	} else {
		err = Sony_Insert0(R.ioParam.ioRefNum, locked, (ps3p)fileName);
	}

	return err;
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
				if (noErr != InsertADiskFromFileRef(&spec)) {
					break;
				}
			}
		}
	}
}

LOCALFUNC OSErr InsertADiskOrAliasFromSpec(FSSpec *spec)
{
	Boolean isFolder;
	Boolean isAlias;
	OSErr theErr;

	theErr = ResolveAliasFile(spec, true, &isFolder, &isAlias);
	if (noErr == theErr) {
		theErr = InsertADiskFromFileRef(spec);
	}

	return theErr;
}

LOCALFUNC OSErr InsertADiskFromNameEtc(short vRefNum, long dirID, ConstStr255Param fileName)
{
	OSErr theErr;

#if HaveCPUfamM68K
	if (! MyEnvrAttrFSSpecCallsAvail) {
		theErr = InsertADiskFromNamevRef(fileName, vRefNum);
	} else
#endif
	{
		FSSpec spec;

		theErr = FSMakeFSSpec(vRefNum, dirID, fileName, &spec);
		if (noErr == theErr) {
			theErr = InsertADiskOrAliasFromSpec(&spec);
		}
	}

	return theErr;
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
		} else if (noErr == err) {
			if (noErr == ResolveAliasFile(&spec, trueblnr, &isFolder, &isAlias)) {
				err = FSpOpenDF(&spec, fsRdPerm, refnum);
				if (noErr == err) {
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
#define MyDisposeNavEventUPP(userUPP) DisposeRoutineDescriptor(userUPP)
#define MyDisposeNavObjectFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
#define MyNewNavObjectFilterUPP NewNavObjectFilterProc
#define MyNewNavEventUPP NewNavEventProc

	if (gNavServicesExists) {
		NavReplyRecord theReply;
		NavDialogOptions dialogOptions;
		OSErr theErr = noErr;
		NavTypeListHandle openList = NULL;
		NavObjectFilterUPP filterUPP = MyNewNavObjectFilterUPP(/* (NavObjectFilterProcPtr) */NavigationFilterProc);
		NavEventUPP eventUPP = MyNewNavEventUPP(/* (NavEventProcPtr) */NavigationEventProc);

		theErr = NavGetDefaultDialogOptions(&dialogOptions);

		dialogOptions.dialogOptionFlags |= kNavDontAutoTranslate;
		/* dialogOptions.dialogOptionFlags &= ~ kNavAllowMultipleFiles; */
		dialogOptions.dialogOptionFlags &= ~ kNavAllowPreviews;

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
		StandardFileReply reply;

		MyBeginDialog();
		StandardGetFile(0L, -1, 0L, &reply);
		MyEndDialog();
		if (reply.sfGood) {
			(void) InsertADiskFromFileRef(&reply.sfFile);
		}
	}
}

#if IncludeSonyNew
LOCALFUNC OSErr WriteZero(SInt16 refnum, ui5b L)
{
	OSErr err = SetFPos(refnum, fsFromStart, 0);
	if (noErr == err) {
#define ZeroBufferSize 2048
		ui5b i;
		ui3b buffer[ZeroBufferSize];

		for (i = 0; i < ZeroBufferSize; i++) {
			buffer[i] = 0;
		}
		while (L > 0) {
			i = (L > ZeroBufferSize) ? ZeroBufferSize : L;
			err = FSWrite(refnum, (long *)&i, buffer);
			if (noErr != err) {
				return err;
			}
			L -= i;
		}
	}

	return err;
}
#endif

#if HaveCPUfamM68K && IncludeSonyNew
LOCALPROC MakeNewDiskFromNamevRef(ps3p Name, short vRefNum,
	ui5b L)
{
	short refNum;
	OSErr theErr;

	theErr = Create(Name, vRefNum, '????', '????');
	if (theErr == dupFNErr) {
		theErr = FSDelete(Name, vRefNum);
		if (noErr == theErr) {
			theErr = Create(Name, vRefNum, '????', '????');
		}
	}
	if (noErr == theErr) {
		theErr = FSOpen(Name, vRefNum, &refNum);
		if (noErr == theErr) {
			theErr = SetEOF(refNum, L);
			if (noErr == theErr) {
				theErr = WriteZero(refNum, L);
				if (noErr == theErr) {
					theErr = Sony_Insert0(refNum, falseblnr, Name);
					refNum = NotAfileRef;
				}
			}
			if (NotAfileRef != refNum) {
				(void) FSClose(refNum);
			}
		}
		if (noErr != theErr) {
			(void) FSDelete(Name, vRefNum);
		}
	}
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDiskFromSpec(FSSpec *NewFileSpec,
	ui5b L)
{
	short refNum;
	OSErr theErr;

	theErr = FSpCreate(NewFileSpec, '????', '????', smSystemScript);
	if (theErr == dupFNErr) {
		theErr = FSpDelete(NewFileSpec);
		if (noErr == theErr) {
			theErr = FSpCreate(NewFileSpec, '????', '????', smSystemScript);
		}
	}
	if (noErr == theErr) {
		theErr = FSpOpenDF(NewFileSpec, fsRdWrPerm, &refNum);
		if (noErr == theErr) {
			theErr = SetEOF(refNum, L);
			if (noErr == theErr) {
				theErr = WriteZero(refNum, L);
				if (noErr == theErr) {
					theErr = Sony_Insert0(refNum, falseblnr, NULL);
					refNum = NotAfileRef;
				}
			}
			if (NotAfileRef != refNum) {
				(void) FSClose(refNum);
			}
		}
		if (noErr != theErr) {
			(void) FSpDelete(NewFileSpec);
		}
	}
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDisk(ui5b L, Handle NewDiskName)
{
	OSErr theErr;

#if NavigationAvail
	if (gNavServicesExists) {
		NavReplyRecord theReply;
		NavDialogOptions dialogOptions;
		NavEventUPP eventUPP = MyNewNavEventUPP(/* (NavEventProcPtr) */NavigationEventProc);

		theErr = NavGetDefaultDialogOptions(&dialogOptions);
		dialogOptions.dialogOptionFlags |= kNavNoTypePopup;
#if IncludeSonyNameNew
		if (NewDiskName != NULL) {
			PStrFromHandle(dialogOptions.savedFileName, NewDiskName, 255);
		}
#endif
		MyBeginDialog();
		theErr = NavPutFile(NULL, &theReply, &dialogOptions,
			/* NULL */ eventUPP, '????', '????', NULL);
		MyEndDialog();

		MyDisposeNavEventUPP(eventUPP);

		if (theErr == noErr) {
			if (theReply.validRecord) {
				long itemsInList;
				AEKeyword keyword;
				DescType typeCode;
				Size actualSize;
				FSSpec NewFileSpec;

				if (noErr == AECountItems(&theReply.selection, &itemsInList))
				if (1 == itemsInList)
				if (noErr == AEGetNthPtr(&theReply.selection,
					1, typeFSS, &keyword, &typeCode,
					(Ptr)&NewFileSpec, sizeof(FSSpec), &actualSize))
				{
					MakeNewDiskFromSpec(&NewFileSpec, L);
				}
			}
			NavDisposeReply(&theReply);
		}
	} else
#endif
	{
		Str255 Title;
		Str255 prompt;

#if IncludeSonyNameNew
		if (NewDiskName != NULL) {
			PStrFromHandle(Title, NewDiskName, 255);
		} else {
			NativeStrFromCStr(Title, "untitled", falseblnr);
		}
#endif
		NativeStrFromCStr(prompt, "Please select a file", falseblnr);

#if HaveCPUfamM68K
		if (! MyEnvrAttrFSSpecCallsAvail) {
			Point where;
			SFReply reply;

			where.h = 50;
			where.v = 50;
			MyBeginDialog();
			SFPutFile(*(Point *)&where, prompt, Title, NULL, &reply);
			MyEndDialog();

			if (reply.good) {
				MakeNewDiskFromNamevRef(reply.fName,
					reply.vRefNum, L);
			}
		} else
#endif
		{
			StandardFileReply reply;

			MyBeginDialog();
			StandardPutFile(prompt, Title, &reply);
			MyEndDialog();

			if (reply.sfGood) {
				MakeNewDiskFromSpec(&reply.sfFile, L);
			}
		}
	}
}
#endif

LOCALFUNC blnr AllocateMacROM(void)
{
	ROM = (ui4b *)NewPtr(kROM_Size);
	if (ROM == NULL) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
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

LOCALFUNC blnr InitMyApplInfo(void)
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
	if (! OpenNamedFileInFolder(MyDatvRefNum, MyDatdirID, fileName,
		&refnum, &Exists))
	{
		MacMsg(kStrNoReadROMTitle, kStrNoReadROMMessage, trueblnr);
	} else {
		if (! Exists) {
			MacMsg(kStrNoROMTitle, kStrNoROMMessage, trueblnr);
			SpeedStopped = trueblnr;
		} else {
			err = FSRead(refnum, &count, ROM);
			if (noErr != err) {
				if (eofErr == err) {
					MacMsg(kStrShortROMTitle, kStrShortROMMessage, trueblnr);
				} else {
					MacMsg(kStrNoReadROMTitle, kStrNoReadROMMessage, trueblnr);
				}
				SpeedStopped = trueblnr;
			}
			(void) FSClose(refnum);
		}
		return trueblnr;
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
		if (noErr != InsertADiskFromNameEtc(MyDatvRefNum, MyDatdirID, s)) {
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

#define MyNewAEEventHandlerUPP NewAEEventHandlerProc

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
				if (noErr != InsertADiskOrAliasFromSpec(&r.fileSpec)) {
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
#define MyNewDragTrackingHandlerUPP NewDragTrackingHandlerProc
#define MyNewDragReceiveHandlerUPP NewDragReceiveHandlerProc
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

FORWARDPROC PostMyLowPriorityTasksEvent(void);

#if EnableScalingBuff
#define ScaleBuffSzMult (MyWindowScale * MyWindowScale)
#endif

LOCALFUNC blnr AllocateScreenCompare(void)
{
	screencomparebuff = NewPtr(vMacScreenNumBytes);
	if (screencomparebuff == NULL) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	}
	SetLongs((ui5b *)screencomparebuff, vMacScreenNumBytes / 4);

#if UseControlKeys
	CntrlDisplayBuff = NewPtr(vMacScreenNumBytes);
	if (CntrlDisplayBuff == NULL) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	}
#endif
#if EnableScalingBuff
	ScalingBuff = NewPtr(vMacScreenNumBytes * (
		ScaleBuffSzMult
		));
	if (ScalingBuff == NULL) {
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

	ResultWin = NewWindow(
		0L, Bounds, LMGetCurApName() /* "\pMini vMac" */, false,
		noGrowDocProc, /* Could use kWindowSimpleProc for Full Screen */
		(WindowPtr) -1, true, 0);
	if (ResultWin != NULL) {
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

	return trueblnr;
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
#if HogCPU
		NoEventsCounter = 0;
#endif
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

LOCALPROC LeaveBackground(void)
{
#if HogCPU
	NoEventsCounter = 0;
#endif

	SetCursorArrow();
}

LOCALPROC EnterBackground(void)
{
	KeyBoardAttachedToEm = falseblnr;
	Keyboard_UpdateControlKey(falseblnr);

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

	if (KeyBoardAttachedToEm != (! (MacMsgDisplayed || LastControlKey
		|| gTrueBackgroundFlag || ADialogIsUp)))
	{
		KeyBoardAttachedToEm = ! KeyBoardAttachedToEm;
		if (KeyBoardAttachedToEm) {
			InitKeyCodes();
		}
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
				MakeNewDisk(vSonyNewDiskSize, PbufDat[vSonyNewDiskName]);
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
		}

		/* CheckSavedMacMsg(); */
	}

	if (ControlKeyPressed != Keyboard_TestKeyMap(MKC_Control)) {
		Keyboard_UpdateKeyMap(MKC_Control, ControlKeyPressed);
	}
}

LOCALPROC DoNotInBackgroundTasks(void)
{
	if (HaveCursorHidden != (WantCursorHidden
		&& ! (gTrueBackgroundFlag || ADialogIsUp || CurSpeedStopped)))
	{
		HaveCursorHidden = ! HaveCursorHidden;
		if (HaveCursorHidden) {
			HideCursor();
		} else {
			ShowCursor();
		}
	}
}

LOCALPROC CheckForSavedTasks(void)
{
	CheckStateAfterEvents();
	DoNotInBackgroundTasks();
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

#define CheckItem CheckMenuItem

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

LOCALPROC MacOS_HandleMenu(short menuID, short menuItem)
{
	switch (menuID) {
		case kAppleMenu:
			if (menuItem == kAppleAboutItem) {
				DoAboutMsg();
			} else {
				DoOpenDA(menuItem);
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

LOCALPROC HandleMacEvent(EventRecord *theEvent)
{
	WindowPtr whichWindow;
	GrafPtr savePort;

	switch(theEvent->what) {
		case mouseDown:
			switch (FindWindow(theEvent->where, &whichWindow)) {
				case inSysWindow:
					SystemClick(theEvent, whichWindow);
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

LOCALPROC WaitForTheNextEvent(void)
{
	EventRecord theEvent;

	/* we're not doing anything, let system do as it pleases */
	if (
#if HaveCPUfamM68K
		(! MyEnvrAttrWaitNextEventAvail) ?
		GetNextEvent(everyEvent, &theEvent) :
#endif
		WaitNextEvent(everyEvent, &theEvent,
			(gTrueBackgroundFlag && ! RunInBackground)
				? 5*60*60
				: 5, /* still need to check for
					control key when SpeedStopped,
					don't get event */
			NULL))
	{
		HandleMacEvent(&theEvent);
	}
}

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
}

#define PrivateEventMask (mDownMask | mUpMask | keyDownMask | keyUpMask | autoKeyMask)

#define IsPowOf2(x) (((x) & ((x) - 1)) == 0)

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

	if (! gBackgroundFlag) {
		CheckKeyBoardState();
	}
}

LOCALPROC RunOnEndOfSixtieth(void)
{
	while (ExtraTimeNotOver()) {
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
	}

	OnTrueTime = TrueEmulatedTime;
	RunEmulatedTicksToTrueTime();
}

LOCALPROC MainEventLoop(void)
{
	for (; ; ) {
		CheckForSystemEvents();
		CheckForSavedTasks();
		if (ForceMacOff) {
			return;
		}

		if (CurSpeedStopped) {
			WaitForTheNextEvent();
		} else {
			DoEmulateExtraTime();
			RunOnEndOfSixtieth();
		}
	}
}

LOCALPROC AppendMenuCStr(MenuHandle menu, char *s)
{
	Str255 t;

	PStrFromCStr(t, s);
	AppendMenu(menu, t);
}

LOCALPROC AppendMenuConvertCStr(MenuHandle menu,
	char *s, blnr WantEllipsis)
{
	Str255 t;

	NativeStrFromCStr(t, s, WantEllipsis);
	AppendMenu(menu, t);
}

LOCALPROC AppendMenuSep(MenuHandle menu)
{
	AppendMenuCStr(menu, "(-");
}

LOCALFUNC MenuHandle NewMenuFromConvertCStr(short menuID, char *s)
{
	Str255 r;

	NativeStrFromCStr(r, s, falseblnr);
	return NewMenu(menuID, r);
}

LOCALFUNC blnr InstallOurMenus(void)
{
	MenuHandle menu;
	Str255 s;

	PStrFromChar(s, (char)20);
	menu = NewMenu(kAppleMenu, s);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kStrMenuItemAbout, trueblnr);
		AppendMenuSep(menu);
		AppendResMenu(menu, 'DRVR');
		InsertMenu(menu, 0);
	}

	menu = NewMenuFromConvertCStr(kFileMenu, kStrMenuFile);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kStrMenuItemOpen, trueblnr);
		{
			AppendMenuSep(menu);
			AppendMenuConvertCStr(menu,
				kStrMenuItemQuit, falseblnr);
		}
		InsertMenu(menu, 0);
	}

	menu = NewMenuFromConvertCStr(kSpecialMenu, kStrMenuSpecial);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kStrMenuItemMore, trueblnr);
		InsertMenu(menu, 0);
	}

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
	InitKeyCodes();

	if (MyEnvrAttrAppleEvtMgrAvail) {
		InstallAppleEventHandlers();
	}
#if EnableDragDrop
	if (gHaveDragMgr) {
		gHaveDragMgr = PrepareForDragging();
	}
#endif
	return trueblnr;
}

LOCALPROC ZapOSGLUVars(void)
{
	ZapEmKeys();
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
	if (InitEmulation())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}

	if (! gTrueBackgroundFlag) {
		CheckSavedMacMsg();
	}

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

#if IncludePbufs
	UnInitPbufs();
#endif
	UnInitDrives();

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
		MainEventLoop();
	}
	UnInitOSGLU();

#if MainReturnsInt
	return 0;
#endif
}
