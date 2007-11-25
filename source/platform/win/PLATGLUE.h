/*
	PLATGLUE.h

	Copyright (C) 2006 Philip Cummins, Weston Pawlowski,
	Bradford L. Barrett, Paul C. Pratt, Fabio Concas

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
	PLATform GLUE. (for Microsoft Windows)

	All operating system dependent code for the
	Microsoft Windows platform should go here.

	This code is descended from Weston Pawlowski's Windows
	port of vMac, by Philip Cummins.
	Adapted by Fabio Concas to run on Pocket PC 2003 devices.

	The main entry point 'WinMain' is at the end of this file.
*/

#ifndef InstallFileIcons
#define InstallFileIcons 0
#endif

/* Resource Ids */

#define IDI_VMAC                        256
#if InstallFileIcons
#define IDI_ROM                         257
#define IDI_DISK                        258
#endif

/*--- some simple utilities ---*/

#define PowOf2(p) ((unsigned long)1 << (p))
#define TestBit(i, p) (((unsigned long)(i) & PowOf2(p)) != 0)

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
/*
	must work even if blocks overlap in memory
*/
	(void) memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

/*--- control mode and internationalization ---*/

#ifdef UNICODE
#define MyUseUni 1
#else
#define MyUseUni 0
#endif

#define NeedRequestInsertDisk 1
#define NeedDoMoreCommandsMsg 1
#define NeedDoAboutMsg 1

#if MyUseUni
#define NeedCell2UnicodeMap 1
#else
#define NeedCell2WinAsciiMap 1
#endif

#include "CONTROLM.h"

/*--- main window info ---*/

LOCALVAR HWND MainWnd = NULL;

LOCALVAR int WndX;
LOCALVAR int WndY;

#ifdef _WIN32_WCE
#define UseWinCE 1
#else
#define UseWinCE 0
#endif

#if UseWinCE
LOCALVAR short oldOrientation;
LOCALVAR unsigned long oldDisplayOrientation;
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

#if UseWinCE
/* This cheesy workaround is needed because XDest - hOffset is always >= 3 (why?) */
#define hOffsetFudge 3
#endif

#if EnableFullScreen || UseWinCE
LOCALVAR short hOffset;
LOCALVAR short vOffset;
#endif

#if UseWinCE
/* Number of pre-calculated screen offsets for this device */
LOCALVAR unsigned char numHOffsets;
LOCALVAR unsigned char numVOffsets;
/* Pre-calculated screen offsets */
LOCALVAR short *hOffsetsList;
LOCALVAR short *vOffsetsList;
/* Indices of current offsets */
LOCALVAR unsigned char curHOffset;
LOCALVAR unsigned char curVOffset;
/* Screen resolution, in pixels; values will be changed upon initialization (see Init_ChangeOrientation) */
LOCALVAR short hRes = vMacScreenWidth;
LOCALVAR short vRes = vMacScreenHeight;
#endif

#if UseWinCE
/* Sip Status ON/OFF */
LOCALVAR blnr SipOn = falseblnr;
#endif

#if UseWinCE
/* Are we in control mode? */
/* Needed because you can't hold down a key with the virtual keyboard */
LOCALVAR blnr CtrlMode = falseblnr;
#endif

/* cursor hiding */

LOCALVAR blnr HaveCursorHidden = falseblnr;
LOCALVAR blnr WantCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		(void) ShowCursor(TRUE);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}

/* cursor moving */

#if EnableMouseMotion
LOCALFUNC blnr MyMoveMouse(si4b h, si4b v)
{
	POINT NewMousePos;
	ui5b difftime;
	blnr IsOk;
	DWORD StartTime = GetTickCount();
	LONG x = h;
	LONG y = v;

#if EnableMagnify
	if (UseMagnify) {
		x *= MyWindowScale;
		y *= MyWindowScale;
	}
#endif
#if EnableFullScreen
	if (UseFullScreen) {
		x += hOffset;
		y += vOffset;
	}
#endif
	x += WndX;
	y += WndY;

	do {
		(void) SetCursorPos(x, y);
		if (! GetCursorPos(&NewMousePos)) {
			IsOk = falseblnr;
		} else {
			IsOk = (x == NewMousePos.x) && (y == NewMousePos.y);
		}
		difftime = (ui5b)(GetTickCount() - StartTime);
	} while ((! IsOk) && (difftime < 100));
	return IsOk;
}
#endif

#if EnableMouseMotion
LOCALVAR LONG SavedMouseH;
LOCALVAR LONG SavedMouseV;
#endif

#if EnableMouseMotion
LOCALPROC StartSaveMouseMotion(void)
{
	if (! HaveMouseMotion) {
		if (MyMoveMouse(vMacScreenWidth / 2, vMacScreenHeight / 2)) {
			SavedMouseH = vMacScreenWidth / 2;
			SavedMouseV = vMacScreenHeight / 2;
			HaveMouseMotion = trueblnr;
		}
	}
}
#endif

#if EnableMouseMotion
LOCALPROC StopSaveMouseMotion(void)
{
	if (HaveMouseMotion) {
		(void) MyMoveMouse(CurMouseH, CurMouseV);
		HaveMouseMotion = falseblnr;
	}
}
#endif

LOCALPROC SetCurMouseButton(blnr v)
{
	if (v) {
		if (! CurMouseButton) {
			(void) SetCapture(MainWnd);
			CurMouseButton = trueblnr;
		}
	} else {
		if (CurMouseButton) {
			(void) ReleaseCapture();
			CurMouseButton = falseblnr;
		}
	}
}

/* keyboard */

LOCALVAR HINSTANCE AppInstance;

/* these constants weren't in the header files I have */
#define myVK_Subtract 0xBD
#define myVK_Equal 0xBB
#define myVK_BackSlash 0xDC
#define myVK_Comma 0xBC
#define myVK_Period 0xBE
#define myVK_Slash 0xBF
#define myVK_SemiColon 0xBA
#define myVK_SingleQuote 0xDE
#define myVK_LeftBracket 0xDB
#define myVK_RightBracket 0xDD
#define myVK_Grave 0xC0

/* some new ones, need to check if in all header versions */
#define myVK_PRIOR 0x21
#define myVK_NEXT 0x22
#define myVK_END 0x23
#define myVK_HOME 0x24
#define myVK_INSERT 0x2D
#define myVK_DELETE 0x2E
#define myVK_HELP 0x2F
#define myVK_SCROLL 0x91
#define myVK_SNAPSHOT 0x2C
#define myVK_PAUSE 0x13

LOCALVAR si3b WinKey2Mac[256];

LOCALPROC AssignOneMacKey(ui3b WinKey, si3b MacKey)
{
	WinKey2Mac[WinKey] = MacKey;
}

LOCALFUNC blnr InitWinKey2Mac(void)
{
	int i;

	for (i = 0; i < 256; ++i) {
		WinKey2Mac[i] = -1;
	}

	AssignOneMacKey('A', MKC_A);
	AssignOneMacKey('S', MKC_S);
	AssignOneMacKey('D', MKC_D);
	AssignOneMacKey('F', MKC_F);
	AssignOneMacKey('H', MKC_H);
	AssignOneMacKey('G', MKC_G);
	AssignOneMacKey('Z', MKC_Z);
	AssignOneMacKey('X', MKC_X);
	AssignOneMacKey('C', MKC_C);
	AssignOneMacKey('V', MKC_V);
	AssignOneMacKey('B', MKC_B);
	AssignOneMacKey('Q', MKC_Q);
	AssignOneMacKey('W', MKC_W);
	AssignOneMacKey('E', MKC_E);
	AssignOneMacKey('R', MKC_R);
	AssignOneMacKey('Y', MKC_Y);
	AssignOneMacKey('T', MKC_T);
	AssignOneMacKey('1', MKC_1);
	AssignOneMacKey('2', MKC_2);
	AssignOneMacKey('3', MKC_3);
	AssignOneMacKey('4', MKC_4);
	AssignOneMacKey('6', MKC_6);
	AssignOneMacKey('5', MKC_5);
	AssignOneMacKey(myVK_Equal, MKC_Equal);
	AssignOneMacKey('9', MKC_9);
	AssignOneMacKey('7', MKC_7);
	AssignOneMacKey(myVK_Subtract, MKC_Minus);
	AssignOneMacKey('8', MKC_8);
	AssignOneMacKey('0', MKC_0);
	AssignOneMacKey(myVK_RightBracket, MKC_RightBracket);
	AssignOneMacKey('O', MKC_O);
	AssignOneMacKey('U', MKC_U);
	AssignOneMacKey(myVK_LeftBracket, MKC_LeftBracket);
	AssignOneMacKey('I', MKC_I);
	AssignOneMacKey('P', MKC_P);
	AssignOneMacKey(VK_RETURN, MKC_Return);
	AssignOneMacKey('L', MKC_L);
	AssignOneMacKey('J', MKC_J);
	AssignOneMacKey(myVK_SingleQuote, MKC_SingleQuote);
	AssignOneMacKey('K', MKC_K);
	AssignOneMacKey(myVK_SemiColon, MKC_SemiColon);
	AssignOneMacKey(myVK_BackSlash, MKC_BackSlash);
	AssignOneMacKey(myVK_Comma, MKC_Comma);
	AssignOneMacKey(myVK_Slash, MKC_Slash);
	AssignOneMacKey('N', MKC_N);
	AssignOneMacKey('M', MKC_M);
	AssignOneMacKey(myVK_Period, MKC_Period);

	AssignOneMacKey(VK_TAB, MKC_Tab);
	AssignOneMacKey(VK_SPACE, MKC_Space);
	AssignOneMacKey(myVK_Grave, MKC_Grave);
	AssignOneMacKey(VK_BACK, MKC_BackSpace);
	AssignOneMacKey(VK_ESCAPE, MKC_Escape);

#ifndef MKC_for_MENU
#define MKC_for_MENU MKC_Command
#endif
	AssignOneMacKey(VK_MENU, MKC_for_MENU);

#ifndef MKC_for_LMENU
#define MKC_for_LMENU MKC_for_MENU
#endif
	AssignOneMacKey(VK_LMENU, MKC_for_LMENU);

#ifndef MKC_for_RMENU
#define MKC_for_RMENU MKC_for_MENU
#endif
	AssignOneMacKey(VK_RMENU, MKC_for_RMENU);

	AssignOneMacKey(VK_SHIFT, MKC_Shift);
	AssignOneMacKey(VK_LSHIFT, MKC_Shift);
	AssignOneMacKey(VK_RSHIFT, MKC_Shift);

	AssignOneMacKey(VK_CAPITAL, MKC_CapsLock);

#ifndef MKC_for_APPS
#define MKC_for_APPS MKC_Option
#endif
	AssignOneMacKey(VK_APPS, MKC_for_APPS);

#ifndef MKC_for_WIN
#define MKC_for_WIN MKC_Option
#endif

#ifndef MKC_for_LWIN
#define MKC_for_LWIN MKC_for_WIN
#endif
	AssignOneMacKey(VK_LWIN, MKC_for_LWIN);

#ifndef MKC_for_RWIN
#define MKC_for_RWIN MKC_for_WIN
#endif
	AssignOneMacKey(VK_RWIN, MKC_for_RWIN);

#ifndef MKC_for_CONTROL
#define MKC_for_CONTROL MKC_Control
#endif
	AssignOneMacKey(VK_CONTROL, MKC_for_CONTROL);

#ifndef MKC_for_LCONTROL
#define MKC_for_LCONTROL MKC_for_CONTROL
#endif
	AssignOneMacKey(VK_LCONTROL, MKC_for_LCONTROL);

#ifndef MKC_for_RCONTROL
#define MKC_for_RCONTROL MKC_for_CONTROL
#endif
	AssignOneMacKey(VK_RCONTROL, MKC_for_RCONTROL);

	AssignOneMacKey(VK_F1, MKC_F1);
	AssignOneMacKey(VK_F2, MKC_F2);
	AssignOneMacKey(VK_F3, MKC_F3);
	AssignOneMacKey(VK_F4, MKC_F4);
	AssignOneMacKey(VK_F5, MKC_F5);
	AssignOneMacKey(VK_F6, MKC_F6);
	AssignOneMacKey(VK_F7, MKC_F7);
	AssignOneMacKey(VK_F8, MKC_F8);
	AssignOneMacKey(VK_F9, MKC_F9);
	AssignOneMacKey(VK_F10, MKC_F10);
	AssignOneMacKey(VK_F11, MKC_F11);
	AssignOneMacKey(VK_F12, MKC_F12);

	AssignOneMacKey(VK_DECIMAL, MKC_Decimal);
	AssignOneMacKey(VK_DELETE, MKC_Decimal);
	/* AssignOneMacKey(VK_RIGHT, 0x42); */
	AssignOneMacKey(VK_MULTIPLY, MKC_KPMultiply);
	AssignOneMacKey(VK_ADD, MKC_KPAdd);
	/* AssignOneMacKey(VK_LEFT, 0x46); */
	AssignOneMacKey(VK_NUMLOCK, MKC_Clear);

	/* AssignOneMacKey(VK_DOWN, 0x48); */
	AssignOneMacKey(VK_DIVIDE, MKC_KPDevide);
	/* AssignOneMacKey(VK_RETURN, MKC_Enter); */
	/* AssignOneMacKey(VK_UP, 0x4D); */
	AssignOneMacKey(VK_DIVIDE, MKC_KPDevide);
	AssignOneMacKey(VK_SUBTRACT, MKC_KPSubtract);

	AssignOneMacKey(VK_SEPARATOR, MKC_KPEqual);
	AssignOneMacKey(VK_NUMPAD0, MKC_KP0);
	AssignOneMacKey(VK_NUMPAD1, MKC_KP1);
	AssignOneMacKey(VK_NUMPAD2, MKC_KP2);
	AssignOneMacKey(VK_NUMPAD3, MKC_KP3);
	AssignOneMacKey(VK_NUMPAD4, MKC_KP4);
	AssignOneMacKey(VK_NUMPAD5, MKC_KP5);

	AssignOneMacKey(VK_NUMPAD6, MKC_KP6);
	AssignOneMacKey(VK_NUMPAD7, MKC_KP7);
	AssignOneMacKey(VK_NUMPAD8, MKC_KP8);
	AssignOneMacKey(VK_NUMPAD9, MKC_KP9);

	AssignOneMacKey(VK_LEFT, MKC_Left);
	AssignOneMacKey(VK_RIGHT, MKC_Right);
	AssignOneMacKey(VK_DOWN, MKC_Down);
	AssignOneMacKey(VK_UP, MKC_Up);

	AssignOneMacKey(myVK_PRIOR, MKC_PageUp);
	AssignOneMacKey(myVK_NEXT, MKC_PageDown);
	AssignOneMacKey(myVK_END, MKC_End);
	AssignOneMacKey(myVK_HOME, MKC_Home);
	AssignOneMacKey(myVK_INSERT, MKC_Help);
	AssignOneMacKey(myVK_DELETE, MKC_ForwardDel);
	AssignOneMacKey(myVK_HELP, MKC_Help);
	AssignOneMacKey(myVK_SNAPSHOT, MKC_Print);
	AssignOneMacKey(myVK_SCROLL, MKC_ScrollLock);
	AssignOneMacKey(myVK_PAUSE, MKC_Pause);

	InitKeyCodes();

	return trueblnr;
}

LOCALPROC DoKeyCode(int i, blnr down)
{
	int key = WinKey2Mac[i];
	if (key >= 0) {
		Keyboard_UpdateKeyMap2(key, down);
	}
}

#ifndef EnableGrabSpecialKeys
#if UseWinCE
#define EnableGrabSpecialKeys 0
#else
#define EnableGrabSpecialKeys EnableFullScreen
#endif
#endif /* EnableGrabSpecialKeys */

#if EnableGrabSpecialKeys
LOCALVAR blnr HaveSetSysParam = falseblnr;
#endif

LOCALPROC CheckTheCapsLock(void)
{
	DoKeyCode(VK_CAPITAL, (GetKeyState(VK_CAPITAL) & 1) != 0);
}

LOCALVAR blnr VK_LWIN_pressed = falseblnr;
LOCALVAR blnr VK_RWIN_pressed = falseblnr;

#if EnableGrabSpecialKeys
LOCALPROC CheckForLostKeyUps(void)
{
	if (HaveSetSysParam) {
		/* check for lost key ups */
		if (VK_LWIN_pressed) {
			if ((GetAsyncKeyState(VK_LWIN) & 0x8000) == 0) {
				DoKeyCode(VK_LWIN, falseblnr);
				VK_LWIN_pressed = falseblnr;
			}
		}
		if (VK_RWIN_pressed) {
			if ((GetAsyncKeyState(VK_RWIN) & 0x8000) == 0) {
				DoKeyCode(VK_RWIN, falseblnr);
				VK_RWIN_pressed = falseblnr;
			}
		}
	}
}
#endif

LOCALPROC DoVKcode0(int i, blnr down)
{
#if EnableGrabSpecialKeys
	if (HaveSetSysParam) {
		/* will need to check for lost key ups */
		if (VK_LWIN == i) {
			VK_LWIN_pressed = down;
		} else if (VK_RWIN == i) {
			VK_RWIN_pressed = down;
		}
	}
#endif
	DoKeyCode(i, down);
}

LOCALPROC DoVKcode(int i, blnr down)
{
	if (i == VK_CAPITAL) {
		CheckTheCapsLock();
	} else if (i >= 0 && i < 256) {
		DoVKcode0(i, down);
	}
}

LOCALVAR blnr WantCmdOptOnReconnect = falseblnr;

LOCALPROC ReconnectKeyCodes3(void)
{
	int i;

	CheckTheCapsLock();

	if (WantCmdOptOnReconnect) {
		WantCmdOptOnReconnect = falseblnr;

		for (i = 0; i < 256; ++i) {
			if ((GetKeyState(i) & 0x8000) != 0) {
				if (i != VK_CAPITAL) {
					DoVKcode0(i, trueblnr);
				}
			}
		}
	}
}

LOCALPROC DisconnectKeyCodes3(void)
{
	DisconnectKeyCodes2();
	SetCurMouseButton(falseblnr);
}

#if EnableGrabSpecialKeys
static HHOOK hKeyHook = NULL;
#endif

#if EnableGrabSpecialKeys
typedef struct {
	DWORD   vkCode;
	DWORD   scanCode;
	DWORD   flags;
	DWORD   time;
	DWORD   dwExtraInfo;
} My_KBDLLHOOKSTRUCT;
#endif

#if EnableGrabSpecialKeys
LRESULT CALLBACK LowLevelKeyboardProc(
	int nCode,     /* hook code */
	WPARAM wParam, /* message identifier */
	LPARAM lParam  /* pointer to structure with message data */
);
#endif

#if EnableGrabSpecialKeys
LRESULT CALLBACK LowLevelKeyboardProc(
	int nCode,     /* hook code */
	WPARAM wParam, /* message identifier */
	LPARAM lParam  /* pointer to structure with message data */
)
{
	if (nCode == HC_ACTION) {
		My_KBDLLHOOKSTRUCT *p = (My_KBDLLHOOKSTRUCT *)lParam;
		if (p->vkCode != VK_CAPITAL) {
			switch (wParam) {
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
					if (! TestBit(lParam, 30)) { /* ignore repeats */
						DoVKcode(p->vkCode, trueblnr);
					}
					return 1;
					break;
				case WM_KEYUP:
				case WM_SYSKEYUP:
					DoVKcode(p->vkCode, falseblnr);
					return 1;
					break;
			}
		}
	}
	return CallNextHookEx(hKeyHook,      /* handle to current hook */
		nCode,      /* hook code passed to hook procedure */
		wParam,  /* value passed to hook procedure */
		lParam   /* value passed to hook procedure */
	);

}
#endif

#if EnableGrabSpecialKeys
#define My_WH_KEYBOARD_LL 13
#endif

#if EnableGrabSpecialKeys
LOCALVAR UINT nPreviousState;
#endif

#if EnableGrabSpecialKeys
LOCALPROC GrabSpecialKeys(void)
{
	if ((hKeyHook == NULL) && ! HaveSetSysParam) {
		/* this works on Windows XP */
		hKeyHook = SetWindowsHookEx(
			My_WH_KEYBOARD_LL,        /* type of hook to install */
			(HOOKPROC)LowLevelKeyboardProc,     /* address of hook procedure */
			AppInstance,    /* handle to application instance */
			0   /* identity of thread to install hook for */
		);
		if (hKeyHook == NULL) {
			/* this works on Windows 95/98 */
			SystemParametersInfo(SPI_SCREENSAVERRUNNING, TRUE, &nPreviousState, 0);
			HaveSetSysParam = trueblnr;
		}
	}
}
#endif

#if EnableGrabSpecialKeys
LOCALPROC UnGrabSpecialKeys(void)
{
	if (hKeyHook != NULL) {
		(void) UnhookWindowsHookEx(hKeyHook);
		hKeyHook = NULL;
	}
	if (HaveSetSysParam) {
		SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, &nPreviousState, 0);
		HaveSetSysParam = falseblnr;
	}
}
#endif

/*--- priority ---*/

#ifndef EnableChangePriority
#if UseWinCE
#define EnableChangePriority 0
#else
#define EnableChangePriority EnableFullScreen
#endif
#endif /* EnableChangePriority */

#if EnableChangePriority
LOCALVAR blnr MyPriorityRaised = falseblnr;
#endif

#if EnableChangePriority
LOCALPROC RaiseMyPriority(void)
{
	if (! MyPriorityRaised) {
		if (! SetPriorityClass(
			GetCurrentProcess(),        /* handle to the process */
			HIGH_PRIORITY_CLASS /* REALTIME_PRIORITY_CLASS (not, killer) */   /* priority class value */
			))
		{
			/* not recursive: MacMsg("SetPriorityClass failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr); */
		}
		MyPriorityRaised = trueblnr;
	}
}
#endif

#if EnableChangePriority
LOCALPROC LowerMyPriority(void)
{
	if (MyPriorityRaised) {
		if (! SetPriorityClass(
			GetCurrentProcess(),        /* handle to the process */
			NORMAL_PRIORITY_CLASS /* priority class value */
			))
		{
			/* not recursive: MacMsg("SetPriorityClass failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr); */
		}
		MyPriorityRaised = falseblnr;
	}
}
#endif


/*--- time, date, location ---*/

LOCALVAR ui5b TrueEmulatedTime = 0;
LOCALVAR ui5b CurEmulatedTime = 0;

#define MyInvTimeDivPow 16
#define MyInvTimeDiv (1 << MyInvTimeDivPow)
#define MyInvTimeDivMask (MyInvTimeDiv - 1)
#define MyInvTimeStep 1089590 /* 1000 / 60.14742 * MyInvTimeDiv */

LOCALVAR DWORD LastTime;

LOCALVAR DWORD NextIntTime;
LOCALVAR ui5b NextFracTime;

LOCALPROC IncrNextTime(void)
{
	NextFracTime += MyInvTimeStep;
	NextIntTime += (NextFracTime >> MyInvTimeDivPow);
	NextFracTime &= MyInvTimeDivMask;
}

LOCALPROC InitNextTime(void)
{
	NextIntTime = LastTime;
	NextFracTime = 0;
	IncrNextTime();
}

LOCALFUNC blnr UpdateTrueEmulatedTime(void)
{
	DWORD LatestTime;
	si5b TimeDiff;

	LatestTime = timeGetTime();
	if (LatestTime != LastTime) {
		LastTime = LatestTime;
		TimeDiff = (LatestTime - NextIntTime);
			/* this should work even when time wraps */
		if (TimeDiff >= 0) {
			if (TimeDiff > 64) {
				/* emulation interrupted, forget it */
				++TrueEmulatedTime;
				InitNextTime();
			} else {
				do {
					++TrueEmulatedTime;
					IncrNextTime();
					TimeDiff = (LatestTime - NextIntTime);
				} while (TimeDiff >= 0);
			}
			return trueblnr;
		} else {
			if (TimeDiff < -20) {
				/* clock goofed if ever get here, reset */
				InitNextTime();
			}
		}
	}
	return falseblnr;
}

LOCALVAR ui5b TimeSecBase;
LOCALVAR DWORD TimeMilliBase;

#include "DATE2SEC.h"

LOCALFUNC blnr CheckDateTime(void)
{
	ui5b NewMacDateInSecond;

	NewMacDateInSecond = ((ui5b)(LastTime - TimeMilliBase)) / 1000 + TimeSecBase;
	if (CurMacDateInSeconds != NewMacDateInSecond) {
		CurMacDateInSeconds = NewMacDateInSecond;

		return trueblnr;
	} else {
		return falseblnr;
	}
}

LOCALFUNC blnr Init60thCheck(void)
{
	SYSTEMTIME s;
	TIME_ZONE_INFORMATION r;
	DWORD v;
	DWORD t;

	GetLocalTime(&s);
	t = timeGetTime();
	TimeSecBase = Date2MacSeconds(s.wSecond, s.wMinute, s.wHour,
		s.wDay, s.wMonth, s.wYear);
	TimeMilliBase = t - s.wMilliseconds;

	v = GetTimeZoneInformation(&r);
	if ((v != 0xFFFFFFFF) && (v != TIME_ZONE_ID_UNKNOWN)) {
		si5b dlsBias = (v != TIME_ZONE_ID_DAYLIGHT)
			? r.StandardBias : r.DaylightBias;
		CurMacDelta = (((ui5b)(- (r.Bias + dlsBias) * 60))
			& 0x00FFFFFF)
			| (((v != TIME_ZONE_ID_DAYLIGHT) ? 0 : 0x80)
				<< 24);
	}

	return trueblnr;
}

/*--- timer thread ---*/

#ifndef UseTimerThread
#define UseTimerThread 1
#endif

#if UseTimerThread
LOCALVAR HANDLE hMyThread = NULL;
LOCALVAR DWORD MyThreadID;

LOCALVAR blnr volatile QuitMyThread = falseblnr;

LOCALVAR blnr volatile MyTimerIsRunning = falseblnr;
LOCALVAR blnr volatile MyTimerShouldRun = falseblnr;

#ifndef MyTimeResolution
#define MyTimeResolution 3
#endif
	/*
		Setting MyTimeResolution to 1 seems to drastically slow down
		the clock in Virtual PC 7.0.2 for Mac. Using 3 is more polite
		anyway, and should not cause much observable difference.
	*/

DWORD WINAPI MyThreadProc(void *pDat);

DWORD WINAPI MyThreadProc(void *pDat)
{
	MSG msg;

	UnusedParam(pDat);

	(void) PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_REMOVE);
		/* force creation of message queue */

	while (! QuitMyThread) {
		if (! MyTimerShouldRun) {
			(void) GetMessage(&msg, NULL, WM_USER, WM_USER);
			/* sleep until someone sends us a message,
			any WM_USER message, contents don't matter */
		} else {
#if MyTimeResolution != 0
			TIMECAPS tc;
			blnr HaveSetTimeResolution = falseblnr;
			/*
				Not clear whether timeBeginPeriod affects
				thread or entire application, so call it
				in here.
			*/
			if (timeGetDevCaps(&tc, sizeof(TIMECAPS))
				== TIMERR_NOERROR)
			{
				if ((MyTimeResolution >= tc.wPeriodMin)
					&& (MyTimeResolution <= tc.wPeriodMax))
				{
					if (timeBeginPeriod(MyTimeResolution)
						== TIMERR_NOERROR)
					{
						HaveSetTimeResolution = trueblnr;
					}
				}
			}
#endif

			LastTime = timeGetTime();
			InitNextTime();

			MyTimerIsRunning = trueblnr;
			do {
				if (UpdateTrueEmulatedTime()) {
					/* MSG msg; */
					/* (void) PeekMessage(&msg, MainWnd, WM_USER, WM_USER, PM_REMOVE); */
						/* if already have message, remove it */
						/* no, not clear if thread safe */
					(void) PostMessage(MainWnd, WM_USER, 0, 0);
				}

				Sleep(NextIntTime - LastTime);
			} while (MyTimerShouldRun && ! QuitMyThread);
			MyTimerIsRunning = falseblnr;

#if MyTimeResolution != 0
			if (HaveSetTimeResolution) {
				(void) timeEndPeriod(MyTimeResolution);
			}
#endif
		}
	}
	QuitMyThread = falseblnr;
	return (0);
}

LOCALPROC MyTimer_Suspend(void)
{
	MyTimerShouldRun = falseblnr;
}

LOCALPROC MyTimer_Resume(void)
{
	if (! MyTimerShouldRun) {
		MyTimerShouldRun = trueblnr;

		(void) PostThreadMessage(
			MyThreadID, WM_USER, 0, 0);
			/* wake up timer thread */
	}
}

LOCALFUNC blnr MyTimer_Init(void)
{
	hMyThread = CreateThread(NULL, 0, MyThreadProc,
		NULL, 0, &MyThreadID);
	if (hMyThread == NULL) {
		return falseblnr;
	} else {
		SetThreadPriority(hMyThread,
			THREAD_PRIORITY_HIGHEST);
		return trueblnr;
	}
}

LOCALPROC MyTimer_UnInit(void)
{
	if (hMyThread != NULL) {
		QuitMyThread = trueblnr;
		if (! MyTimerShouldRun) {
			(void) PostThreadMessage(
				MyThreadID, WM_USER, 0, 0);
				/* wake up timer thread */
		}
		while (QuitMyThread) {
			Sleep(1);
		}
	}
}
#endif

/*--- sound ---*/

#if MySoundEnabled

#define SOUND_SAMPLERATE /* 22050 */ 22255 /* = round(7833600 * 2 / 704) */

#define kLn2SoundBuffers 4 /* kSoundBuffers must be a power of two */
#define kSoundBuffers (1 << kLn2SoundBuffers)
#define kSoundBuffMask (kSoundBuffers - 1)

#define DesiredMinFilledSoundBuffs 3
	/*
		if too big then sound lags behind emulation.
		if too small then sound will have pauses.
	*/

#define kLn2BuffLen 9
#define My_Sound_Len (1UL << kLn2BuffLen)
#define kBufferSize (1UL << (kLn2SoundBuffers + kLn2BuffLen))
#define kBufferMask (kBufferSize - 1)
#define dbhBufferSize (kBufferSize + SOUND_LEN)

LOCALPROC FillWithSilence(ui3p p, int n, ui3b v)
{
	int i;

	for (i = n; --i >= 0; ) {
		*p++ = v;
	}
}

LOCALVAR ui4b CurFillOffset;
LOCALVAR ui4b CurPlayOffset;
LOCALVAR ui4b MinFilledSoundBuffs;

LOCALVAR HWAVEOUT hWaveOut = NULL;
LOCALVAR WAVEHDR whdr[kSoundBuffers];

LOCALVAR ui3p MyBuffer = NULL;

LOCALPROC MySound_Start(void)
{
	if (hWaveOut == NULL) {
		WAVEFORMATEX wfex;
		MMRESULT mmr;
		int i;
		ui3p p;
		WAVEHDR *pwh;
		ui4b CurFillBuffer;

		wfex.wFormatTag = WAVE_FORMAT_PCM;
		wfex.nChannels = 1;
		wfex.nSamplesPerSec = SOUND_SAMPLERATE;
		wfex.nAvgBytesPerSec = SOUND_SAMPLERATE;
		wfex.nBlockAlign = 1;
		wfex.wBitsPerSample = 8;
		wfex.cbSize = 0;
		mmr = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfex, 0, 0 /* (DWORD) AppInstance */,
						CALLBACK_NULL);
		if (mmr != MMSYSERR_NOERROR) {
			/* not recursive: MacMsg("waveOutOpen failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr); */
		} else {
			p = MyBuffer;
			pwh = whdr;
			for (i = 0; i < kSoundBuffers; ++i) {
				pwh->lpData = (LPSTR)p;
				pwh->dwBufferLength = My_Sound_Len;
				pwh->dwBytesRecorded = 0;
				pwh->dwUser = 0;
				pwh->dwFlags = 0;
				pwh->dwLoops = 0;
				mmr = waveOutPrepareHeader(hWaveOut, pwh, sizeof(WAVEHDR));
				if (mmr != MMSYSERR_NOERROR) {
					/* not recursive: MacMsg("waveOutPrepareHeader failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr); */
				} else {
					pwh->dwFlags |= WHDR_DONE;
				}
				p += My_Sound_Len;
				++pwh;
			}

			FillWithSilence(MyBuffer, dbhBufferSize, 0x80);

			CurFillOffset = 0;
			CurPlayOffset = 0;
			CurFillBuffer = kSoundBuffers >> 1;
			CurFillOffset = CurFillBuffer * My_Sound_Len;
			MinFilledSoundBuffs = kSoundBuffers;

			for (i = 0; i < CurFillBuffer; ++i) {
				mmr = waveOutWrite(hWaveOut, &whdr[i], sizeof(WAVEHDR));
				if (mmr != MMSYSERR_NOERROR) {
					whdr[i].dwFlags |= WHDR_DONE; /* make sure */
					/* not recursive: MacMsg("waveOutWrite failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr); */
				}
			}
		}
	}
}

LOCALPROC MySound_Stop(void)
{
	MMRESULT mmr;
	int i;

	if (hWaveOut != NULL) {
		DWORD StartTime = GetTickCount();
		for (i = 0; i < kSoundBuffers; ++i) {
			while (((whdr[i].dwFlags & WHDR_DONE) == 0)
				&& ((ui5b)(GetTickCount() - StartTime) < 1000))
			{
				Sleep(1);
			}

			mmr = waveOutUnprepareHeader(hWaveOut, &whdr[i], sizeof(WAVEHDR));
			if (mmr != MMSYSERR_NOERROR) {
				/* not recursive: MacMsg("waveOutUnprepareHeader failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr); */
			}
		}

		mmr = waveOutClose(hWaveOut);
		if (mmr != MMSYSERR_NOERROR) {
			/* MacMsg("waveOutClose failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr); */
		}
		hWaveOut = NULL;
	}
}

LOCALFUNC blnr MySound_Init(void)
{
	MyBuffer = (ui3p)GlobalAlloc(GMEM_FIXED, dbhBufferSize);
	if (MyBuffer == NULL) {
		return falseblnr;
	}
	return trueblnr;
}

LOCALPROC MySound_UnInit(void)
{
	if (MyBuffer != NULL) {
		if (GlobalFree(MyBuffer) != NULL) {
			/* MacMsg("error", "GlobalFree failed", falseblnr); */
		}
	}
}

LOCALPROC SoundCheckVeryOften(void)
{
label_retry:
	if (hWaveOut != NULL) {
		ui4b FilledSoundBuffs;
		ui4b ToPlaySize = CurFillOffset - CurPlayOffset;
		ui4b CurPlayBuffer = (CurPlayOffset >> kLn2BuffLen) & kSoundBuffMask;

		if ((ToPlaySize > My_Sound_Len)
			&& ((whdr[CurPlayBuffer].dwFlags & WHDR_DONE) != 0))
		{
			CurPlayOffset += My_Sound_Len;
			goto label_retry;
		}
		FilledSoundBuffs = ToPlaySize >> kLn2BuffLen;

		if (FilledSoundBuffs < MinFilledSoundBuffs) {
			MinFilledSoundBuffs = FilledSoundBuffs;
		}

		if (FilledSoundBuffs < 2) {
			MMRESULT mmr;
			ui4b PrevPlayOffset = CurPlayOffset - My_Sound_Len;
			ui4b PrevPlayBuffer = (PrevPlayOffset >> kLn2BuffLen) & kSoundBuffMask;
			ui4b LastPlayedOffset = ((CurFillOffset >> kLn2BuffLen) << kLn2BuffLen) - 1;

			FillWithSilence(MyBuffer + (PrevPlayOffset & kBufferMask),
				My_Sound_Len,
				*(MyBuffer + (LastPlayedOffset & kBufferMask)));
			mmr = waveOutWrite(hWaveOut, &whdr[PrevPlayBuffer], sizeof(WAVEHDR));
			if (mmr != MMSYSERR_NOERROR) {
				whdr[PrevPlayBuffer].dwFlags |= WHDR_DONE;
				/* not recursive: MacMsg("waveOutWrite failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr); */
			}
			CurPlayOffset = PrevPlayOffset;
			goto label_retry;
		}
	}
}

GLOBALFUNC ui3p GetCurSoundOutBuff(void)
{
	if (hWaveOut == NULL) {
		return nullpr;
	} else {
		MMRESULT mmr;
		ui4b NextFillOffset = CurFillOffset + SOUND_LEN;
		ui4b ToPlaySize = NextFillOffset - CurPlayOffset;

		if (ToPlaySize < kBufferSize) {
			ui4b CurFillBuffer = CurFillOffset >> kLn2BuffLen;
			ui4b NextFillBuffer = NextFillOffset >> kLn2BuffLen;

			if (NextFillBuffer != CurFillBuffer) {
				ui4b CurFillSeq = CurFillBuffer >> kLn2SoundBuffers;
				ui4b NextFillSeq = NextFillBuffer >> kLn2SoundBuffers;

				mmr = waveOutWrite(hWaveOut, &whdr[CurFillBuffer & kSoundBuffMask], sizeof(WAVEHDR));
				if (mmr != MMSYSERR_NOERROR) {
					whdr[CurFillBuffer & kSoundBuffMask].dwFlags |= WHDR_DONE;
					/* not recursive: MacMsg("waveOutWrite failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr); */
				}

				if (CurFillSeq != NextFillSeq) {
					MyMoveBytes((anyp)(MyBuffer + kBufferSize),
						(anyp)MyBuffer,
						NextFillOffset & kBufferMask);
				}
			}

			CurFillOffset = NextFillOffset;
		}
		return MyBuffer + (CurFillOffset & kBufferMask);
	}
}

LOCALPROC MySound_SecondNotify(void)
{
	if (hWaveOut != NULL) {
		if (MinFilledSoundBuffs > DesiredMinFilledSoundBuffs) {
			++CurEmulatedTime;
		} else if (MinFilledSoundBuffs < DesiredMinFilledSoundBuffs) {
			--CurEmulatedTime;
		}
		MinFilledSoundBuffs = kSoundBuffers;
	}
}

#endif

/*--- overall grab ---*/

#if EnableFullScreen
LOCALPROC GrabTheMachine(void)
{
#if EnableMouseMotion
	StartSaveMouseMotion();
#endif
#if EnableChangePriority
	if (! SpeedLimit) {
		RaiseMyPriority();
	}
#endif
#if EnableGrabSpecialKeys
	GrabSpecialKeys();
#endif
#if MySoundEnabled && MySoundFullScreenOnly
	MySound_Start();
#endif
}
#endif

#if EnableFullScreen
LOCALPROC UnGrabTheMachine(void)
{
#if EnableGrabSpecialKeys
	UnGrabSpecialKeys();
#endif
#if EnableMouseMotion
	StopSaveMouseMotion();
#endif
#if EnableChangePriority
	LowerMyPriority();
#endif
#if MySoundEnabled && MySoundFullScreenOnly
	MySound_Stop();
#endif
}
#endif

#if EnableFullScreen
LOCALVAR blnr GrabMachine = falseblnr;
#endif

#if EnableFullScreen
LOCALPROC AdjustMachineGrab(void)
{
	if (GrabMachine) {
		if (MainWnd != NULL) {
			GrabTheMachine();
		}
	} else {
		UnGrabTheMachine();
	}
}
#endif

/*--- basic dialogs ---*/

LOCALVAR blnr ADialogIsUp = falseblnr;

LOCALPROC MyBeginDialog(void)
{
	DisconnectKeyCodes3();
	ADialogIsUp = trueblnr;
#if EnableFullScreen
	GrabMachine = falseblnr;
	UnGrabTheMachine();
#endif
	ForceShowCursor();
}

LOCALPROC MyEndDialog(void)
{
	ADialogIsUp = falseblnr;
	ReconnectKeyCodes3();
}

LOCALFUNC LPTSTR FindLastTerm(LPTSTR s, TCHAR delim)
{
	TCHAR c;
	LPTSTR p0 = s;
	LPTSTR p = (LPTSTR)nullpr;

	while ((c = *p0++) != (TCHAR)('\0')) {
		if (c == delim) {
			p = p0;
		}
	}

	return p;
}

LOCALPROC NativeStrFromCStr(LPTSTR r, char *s, blnr AddEllipsis)
{
	ui3b ps[ClStrMaxLength];
	int i;
	int L;

	ClStrFromSubstCStr(&L, ps, s);

	for (i = 0; i < L; ++i) {
		r[i] = (TCHAR)
#if MyUseUni
			Cell2UnicodeMap[ps[i]];
#else
			Cell2WinAsciiMap[ps[i]];
#endif
	}

	if (AddEllipsis) {
#if MyUseUni
		r[L] = 0x2026;
		++L;
#else
		r[L] = '.';
		++L;
		r[L] = '.';
		++L;
		r[L] = '.';
		++L;
#endif
	}

	r[L] = 0;
}

LOCALPROC CheckSavedMacMsg(void)
{
	if (nullpr != SavedBriefMsg) {
		TCHAR briefMsg0[ClStrMaxLength + 1];
		TCHAR longMsg0[ClStrMaxLength + 1];

		NativeStrFromCStr(briefMsg0, SavedBriefMsg, falseblnr);
		NativeStrFromCStr(longMsg0, SavedLongMsg, falseblnr);

		MyBeginDialog();
		MessageBox(MainWnd, longMsg0, briefMsg0, MB_APPLMODAL | MB_OK | (SavedFatalMsg ? MB_ICONSTOP : 0));
		MyEndDialog();

		SavedBriefMsg = nullpr;
	}
}

/*--- main window ---*/

enum {
	ID_MENU_NULL = 256,
	ID_FILE_INSERTDISK1,
	ID_FILE_QUIT,
	ID_SPECIAL_MORECOMMANDS,
	ID_HELP_ABOUT,

	kNum_ID_MENU
};


#define EnableScalingBuff (1 && EnableMagnify && (MyWindowScale == 2))

#if EnableScalingBuff
LOCALVAR char *ScalingBuff = NULL;
#endif

LOCALVAR HDC MainWndDC = NULL;

LOCALVAR si5b CmdShow;

LOCALVAR TCHAR WndTitle[_MAX_PATH];
LOCALVAR const TCHAR WndClassName[] = TEXT("minivmac");

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

LOCALPROC GetWndTitle(void)
{
	TCHAR pathName[_MAX_PATH];
	WIN32_FIND_DATA fd;
	blnr IsOk = falseblnr;

	if (GetModuleFileName(AppInstance, pathName, _MAX_PATH) != 0) {
		HANDLE hf = FindFirstFile(pathName, &fd);

		if (hf != INVALID_HANDLE_VALUE) {
			/* get rid of extension, presumably '.exe' */
			LPTSTR p = FindLastTerm(fd.cFileName,
				(TCHAR)('.'));
			if (p != nullpr) {
				*--p = (TCHAR)('\0');
			}

			_tcscpy(WndTitle, fd.cFileName);
			IsOk = trueblnr;
			FindClose(hf);
		}
	}
	if (! IsOk) {
		_tcscpy(WndTitle, TEXT(kStrAppName));
	}
}

LOCALPROC DisposeMainWindow(void)
{
#if UseWinCE
	/* Show the taskbar */
	SHFullScreen(MainWnd, SHFS_SHOWTASKBAR);
#endif

	if (MainWndDC != NULL) {
		ReleaseDC(MainWnd, MainWndDC);
	}
	if (MainWnd != NULL) {
		DestroyWindow(MainWnd);
		MainWnd = NULL; /* so MacMsg will still work */
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
LOCALVAR POINT WinPositionWins[kNumMagStates];

#if ! UseWinCE
LOCALPROC MyAppendConvertMenuItem(HMENU hMenu,
	UINT uIDNewItem, char *s, blnr AddEllipsis)
{
	TCHAR ts[ClStrMaxLength + 1];

	NativeStrFromCStr(ts, s, AddEllipsis);

	(void) AppendMenu(hMenu, MF_ENABLED + MF_STRING,
		uIDNewItem, ts);
}
#endif

#if ! UseWinCE
LOCALPROC MyAppendSubmenuConvertName(HMENU hMenu,
	HMENU hSubMenu, char *s)
{
	TCHAR ts[ClStrMaxLength + 1];
	MENUITEMINFO mii;

	NativeStrFromCStr(ts, s, falseblnr);

#if 0
	(void) InsertMenu(hMenu, 0xFFFFFFFF,
		MF_BYPOSITION + MF_POPUP + MF_STRING + MF_ENABLED,
		(UINT)hSubMenu, ts);
#endif

	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_TYPE | MIIM_SUBMENU;
	mii.fType = MFT_STRING;
	mii.hSubMenu = hSubMenu;
	mii.dwTypeData = ts;
	mii.cch = (UINT)_tcslen(ts);
	(void) InsertMenuItem(hMenu, (UINT) -1, TRUE,
		&mii);
}
#endif

#ifndef kStrMenuFile_win
#define kStrMenuFile_win kStrMenuFile
#endif

LOCALFUNC blnr ReCreateMainWindow(void)
{
#if ! UseWinCE
	HMENU m;
	HMENU mb;
#endif
	int DfltWndX;
	int DfltWndY;
	int NewWndX;
	int NewWndY;
	int XSize;
	int YSize;
	int WinIndx;
	HWND NewMainWindow;
	HDC NewMainWndDC = NULL;
	int XBorder = GetSystemMetrics(SM_CXFIXEDFRAME);
	int YBorder = GetSystemMetrics(SM_CYFIXEDFRAME);
	int YCaption = GetSystemMetrics(SM_CYCAPTION);
	int YMenu = GetSystemMetrics(SM_CYMENU);
	int ScreenX = GetSystemMetrics(SM_CXSCREEN);
	int ScreenY = GetSystemMetrics(SM_CYSCREEN);
	short NewWindowHeight = vMacScreenHeight;
	short NewWindowWidth = vMacScreenWidth;
	HWND OldMainWindow = MainWnd;
	HDC OldMainWndDC = MainWndDC;

#if EnableFullScreen
	if (! UseFullScreen)
#endif
	{
		/* save old position */
		if (OldMainWindow != NULL) {
			WinPositionWins[CurWinIndx].x = WndX;
			WinPositionWins[CurWinIndx].y = WndY;
		}
	}

#if EnableMagnify
	if (WantMagnify) {
		NewWindowHeight *= MyWindowScale;
		NewWindowWidth *= MyWindowScale;
	}
#endif
#if EnableFullScreen
	if (WantFullScreen) {
#if UseWinCE
		XSize = hRes; /* 320 */ /* ScreenX; */
		YSize = vRes; /* 214 */ /* ScreenY; */
#else
		XSize = ScreenX;
		YSize = ScreenY;

		hOffset = (ScreenX - NewWindowWidth) / 2;
		vOffset = (ScreenY - NewWindowHeight) / 2;
		if (hOffset < 0) {
			hOffset = 0;
		}
		if (vOffset < 0) {
			vOffset = 0;
		}
#endif

		DfltWndX = 0;
		DfltWndY = 0;
	} else
#endif
	{
		XSize = NewWindowWidth;
		YSize = NewWindowHeight;

		DfltWndX = (ScreenX - XSize) / 2;
		DfltWndY = (ScreenY - YSize) / 2;

		if (DfltWndX < 0) {
			DfltWndX = 0;
		}
		if (DfltWndY < 0) {
			DfltWndY = 0;
		}
	}
#if UseWinCE
	hOffset = 0 - hOffsetFudge;
	vOffset = 0;
#endif

#if EnableMagnify
	if (WantMagnify) {
		WinIndx = kMagStateMagnifgy;
	} else
#endif
	{
		WinIndx = kMagStateNormal;
	}

#if EnableFullScreen
	if (WantFullScreen) {
		NewWndX = DfltWndX;
		NewWndY = DfltWndX;
	} else
#endif
	{
		if (! HavePositionWins[WinIndx]) {
			WinPositionWins[WinIndx].x = DfltWndX;
			WinPositionWins[WinIndx].y = DfltWndY;
			HavePositionWins[WinIndx] = trueblnr;
		}
		NewWndX = WinPositionWins[WinIndx].x;
		NewWndY = WinPositionWins[WinIndx].y;

		if ((NewWndX + YSize <= 0)
			|| (NewWndX >= ScreenX)
			|| (NewWndY - YMenu <= 0)
			|| (NewWndY - YMenu - YCaption >= ScreenY))
		{
			NewWndX = DfltWndX;
			NewWndY = DfltWndY;
			if (NewWndY - YMenu <= 0) {
				NewWndY = YBorder + YCaption + YMenu;
			}
		}

		XSize += XBorder + XBorder;
		YSize += YBorder + YCaption + YMenu + YBorder;
		NewWndX -= XBorder;
		NewWndY -= YBorder + YCaption + YMenu;
	}

	if ((OldMainWindow == NULL)
#if EnableFullScreen
		|| (WantFullScreen != UseFullScreen)
#endif
		)
	{
#if ! UseWinCE
#if EnableFullScreen
		if (WantFullScreen) {
			mb = NULL;
		} else
#endif
		{
			mb = CreateMenu();
			if (mb != NULL) {
				m = CreateMenu();
				if (m != NULL) {
					MyAppendConvertMenuItem(m, ID_FILE_INSERTDISK1, kStrMenuItemOpen, trueblnr);
					(void) AppendMenu(m, MF_SEPARATOR, 0, NULL);
					MyAppendConvertMenuItem(m, ID_FILE_QUIT, kStrMenuItemQuit, falseblnr);
					MyAppendSubmenuConvertName(mb, m, kStrMenuFile_win);
				}
				m = CreateMenu();
				if (m != NULL) {
					MyAppendConvertMenuItem(m, ID_SPECIAL_MORECOMMANDS, kStrMenuItemMore, trueblnr);
					MyAppendSubmenuConvertName(mb, m, kStrMenuSpecial);
				}
				m = CreateMenu();
				if (m != NULL) {
					MyAppendConvertMenuItem(m, ID_HELP_ABOUT, kStrMenuItemAbout, trueblnr);
					MyAppendSubmenuConvertName(mb, m, kStrMenuHelp);
				}
			}
		}
#endif

		NewMainWindow = CreateWindowEx(
#if UseWinCE
			WS_EX_TOPMOST,
#else
#if EnableFullScreen
			WantFullScreen ? WS_EX_TOPMOST :
#endif
			WS_EX_ACCEPTFILES,
#endif
			WndClassName,
			WndTitle,
#if UseWinCE
			WS_VISIBLE | WS_POPUP,
#else
#if EnableFullScreen
			WantFullScreen ? WS_VISIBLE | WS_POPUP :
#endif
				WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
#endif
			NewWndX, NewWndY, XSize, YSize, NULL,
#if UseWinCE
			NULL,
#else
			mb,
#endif
			AppInstance, NULL);
		if (NewMainWindow == NULL) {
			MacMsg("CreateWindow failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
			return falseblnr;
		}

		NewMainWndDC = GetDC(NewMainWindow);
		if (NewMainWndDC == NULL) {
			MacMsg("GetDC failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
			DestroyWindow(NewMainWindow);
			return falseblnr;
		}
	} else {
		NewMainWndDC = OldMainWndDC;
		NewMainWindow = OldMainWindow;
		(void) MoveWindow(NewMainWindow, NewWndX, NewWndY,
			XSize, YSize, TRUE);
	}

	{
		RECT r;

		/*
			Find out where the window really went, on
			the off chance that the WM_MOVE message wasn't
			called on CreateWindowEx/MoveWindow, or that
			the window wasn't put where asked for.
		*/
		(void) GetWindowRect(NewMainWindow, &r);
		WndX = (si4b) r.left;
		WndY = (si4b) r.top;
#if EnableFullScreen
		if (! WantFullScreen)
#endif
		{
			WndX += XBorder;
			WndY += YBorder + YCaption + YMenu;
		}
	}

#if EnableGrabSpecialKeys
	UnGrabSpecialKeys();
#endif

#if UseWinCE && 0
	/* Show the taskbar */
	SHFullScreen(MainWnd, SHFS_SHOWTASKBAR);
#endif

	CurWinIndx = WinIndx;
	MainWnd = NewMainWindow;
	MainWndDC = NewMainWndDC;
	gTrueBackgroundFlag = falseblnr;
#if EnableFullScreen
	UseFullScreen = WantFullScreen;
#endif
#if EnableMagnify
	UseMagnify = WantMagnify;
#endif

	if (NewMainWindow != OldMainWindow) {
		ShowWindow(NewMainWindow, SW_SHOW /* CmdShow */);
		if (OldMainWndDC != NULL) {
			ReleaseDC(MainWnd, OldMainWndDC);
		}
		if (OldMainWindow != NULL) {
			/* ShowWindow(OldMainWindow, SW_HIDE); */
			DestroyWindow(OldMainWindow);
		}

		DisconnectKeyCodes3();
			/* since key events per window */
	} else {
		(void) InvalidateRgn(MainWnd, NULL, FALSE);
	}

#if UseWinCE
	/* Create and set logical palette for this window */
	{
		HPALETTE hpal;
		LOGPALETTE *lppal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +
			sizeof(PALETTEENTRY) * 2);

		if (! lppal)
		{
			MacMsg("CreateWindow failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
			return falseblnr;
		}

		lppal->palNumEntries = 2;
		lppal->palVersion = 0x0300;
		lppal->palPalEntry[0].peRed   = 255;
		lppal->palPalEntry[0].peGreen = 255;
		lppal->palPalEntry[0].peBlue  = 255;
		lppal->palPalEntry[0].peFlags = 0;
		lppal->palPalEntry[1].peRed   = 0;
		lppal->palPalEntry[1].peGreen = 0;
		lppal->palPalEntry[1].peBlue  = 0;
		lppal->palPalEntry[1].peFlags = 0;

		hpal = CreatePalette(lppal);

		if (hpal == NULL)
		{
			free(lppal);
			MacMsg("CreateWindow failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
			return falseblnr;
		}

		if (SelectPalette(MainWndDC, hpal, FALSE) == NULL)
		{
			free(lppal);
			MacMsg("CreateWindow failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
			return falseblnr;
		}

		if (RealizePalette(MainWndDC) == GDI_ERROR)
		{
			free(lppal);
			MacMsg("CreateWindow failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
			return falseblnr;
		}

		free(lppal);
	}
#endif

#if UseWinCE
	/* Hide the taskbar */
	SHFullScreen(MainWnd, SHFS_HIDETASKBAR);
#endif

	return trueblnr;
}

#if UseWinCE
LOCALFUNC blnr AlreadyRunningCheck(void)
{
	/* Adapted from example program from Microsoft eMbedded Visual C++ */

	/* If it is already running, then focus on the window */
	HWND hWnd = FindWindow(WndClassName, WndTitle);
	if (hWnd == NULL) {
		return falseblnr;
	} else {
		/*
			Set focus to foremost child window.
			The "| 0x01" is used to bring any owned
			windows to the foreground and activate them.
		*/
		SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
		return trueblnr;
	}
}
#endif

LOCALFUNC blnr AllocateScreenCompare(void)
{
	screencomparebuff = (char *)GlobalAlloc(GMEM_FIXED, vMacScreenNumBytes);
	if (screencomparebuff == NULL) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	}
	SetLongs((ui5b *)screencomparebuff, vMacScreenNumBytes / 4);
#if UseControlKeys
	CntrlDisplayBuff = (char *)GlobalAlloc(GMEM_FIXED, vMacScreenNumBytes);
	if (CntrlDisplayBuff == NULL) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	}
#endif
#if EnableScalingBuff
	ScalingBuff = (char *)GlobalAlloc(GMEM_FIXED, vMacScreenNumBytes * MyWindowScale * MyWindowScale);
	if (ScalingBuff == NULL) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	}
#endif
	return trueblnr;
}

typedef struct BITMAPINFOHEADER256 {
	BITMAPINFOHEADER bmi;
	RGBQUAD colors[2];
} BITMAPINFOHEADER256;

#if EnableMagnify
#define MyScaledHeight (MyWindowScale * vMacScreenHeight)
#define MyScaledWidth (MyWindowScale * vMacScreenWidth)
#endif

LOCALPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right)
{
	BITMAPINFOHEADER256 bmh;
	ui3b *cdb = (ui3b *)GetCurDrawBuff();
	ui3b *p = cdb + top * (vMacScreenWidth >> 3);
	int XDest = 0;
	int YDest = top;

#if EnableMagnify
	if (UseMagnify) {
		XDest *= MyWindowScale;
		YDest *= MyWindowScale;
	}
#endif
#if ! UseWinCE
#if EnableFullScreen
	if (UseFullScreen) {
		XDest += hOffset;
		YDest += vOffset;
	}
#endif
#endif

	UnusedParam(left);
	UnusedParam(right);
#if 0
	{ /* testing code */
		if (PatBlt(MainWndDC,
			(int)left - 1,
			(int)top - 1,
			(int)right - left + 2,
			(int)bottom - top + 2, PATCOPY)) {
		}
	}
#endif
	memset(&bmh, 0, sizeof (bmh));
	bmh.bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmh.bmi.biWidth = vMacScreenWidth;
	bmh.bmi.biHeight = - (bottom - top);
	bmh.bmi.biPlanes = 1;
	bmh.bmi.biBitCount = 1;
	bmh.bmi.biCompression= BI_RGB;
	bmh.bmi.biSizeImage = 0;
	bmh.bmi.biXPelsPerMeter = 0;
	bmh.bmi.biYPelsPerMeter = 0;
	bmh.bmi.biClrUsed = 0;
	bmh.bmi.biClrImportant = 0;
#if ! UseWinCE
	bmh.colors[0].rgbRed = 255;
	bmh.colors[0].rgbGreen = 255;
	bmh.colors[0].rgbBlue = 255;
	bmh.colors[0].rgbReserved = 0;
	bmh.colors[1].rgbRed = 0;
	bmh.colors[1].rgbGreen = 0;
	bmh.colors[1].rgbBlue = 0;
	bmh.colors[1].rgbReserved = 0;
#endif

#if EnableMagnify
	if (UseMagnify) {
#if EnableScalingBuff
		if (ScalingBuff != NULL) {
			int i;
			int j;
			int k;
			ui3b *p1 = cdb + top * (vMacScreenWidth >> 3);
			ui3b *p2 = (ui3b *)ScalingBuff /* + MyWindowScale * MyWindowScale * vMacScreenWidth / 8 * top */;
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

			bmh.bmi.biWidth = vMacScreenWidth * MyWindowScale;
			bmh.bmi.biHeight = - ((bottom - top) * MyWindowScale);
			if (SetDIBitsToDevice(
				MainWndDC, /* handle of device context */
				XDest, /* x-coordinate of upper-left corner of dest. rect. */
				YDest, /* y-coordinate of upper-left corner of dest. rect. */
				vMacScreenWidth * MyWindowScale, /* source rectangle width */
				(bottom - top) * MyWindowScale, /* source rectangle height */
				0, /* x-coordinate of lower-left corner of source rect. */
				0, /* y-coordinate of lower-left corner of source rect. */
				0, /* first scan line in array */
				(bottom - top) * MyWindowScale, /* number of scan lines */
				(CONST VOID *)ScalingBuff, /* address of array with DIB bits */
				(const struct tagBITMAPINFO *)&bmh, /* address of structure with bitmap info. */
#if ! UseWinCE
				DIB_RGB_COLORS /* RGB or palette indices */
#else
				DIB_PAL_COLORS /* palette indices */
#endif
			) == 0) {
				/* ReportWinLastError(); */
			}
		}
#else
		if (StretchDIBits(
			MainWndDC, /* handle of device context */
			XDest, /* x-coordinate of upper-left corner of dest. rect. */
			YDest, /* y-coordinate of upper-left corner of dest. rect. */
			vMacScreenWidth * MyWindowScale, /* dest. rectangle width */
			(bottom - top) * MyWindowScale, /* dest. rectangle height */
			0, /* x-coordinate of lower-left corner of source rect. */
			0, /* y-coordinate of lower-left corner of source rect. */
			vMacScreenWidth, /* source rectangle width */
			(bottom - top), /* source rectangle height */
			(CONST VOID *)p, /* address of array with DIB bits */
			(const struct tagBITMAPINFO *)&bmh, /* address of structure with bitmap info. */
#if ! UseWinCE
			DIB_RGB_COLORS, /* RGB or palette indices */
#else
			DIB_PAL_COLORS, /* palette indices */
#endif
			SRCCOPY
		) == 0) {
			/* ReportWinLastError(); */
		}
#endif
	} else
#endif
#if UseWinCE && EnableFullScreen
	if (UseFullScreen) {
		if (StretchDIBits(
			MainWndDC, /* handle of device context */
			XDest, /* x-coordinate of upper-left corner of dest. rect. */
			YDest, /* y-coordinate of upper-left corner of dest. rect. */
			hRes, /* dest. rectangle width */
			vRes, /* dest. rectangle height */
			0, /* x-coordinate of lower-left corner of source rect. */
			0, /* y-coordinate of lower-left corner of source rect. */
			vMacScreenWidth, /* source rectangle width */
			bottom, /* source rectangle height */
			(CONST VOID *)p, /* address of array with DIB bits */
			(const struct tagBITMAPINFO *)&bmh, /* address of structure with bitmap info. */
#if ! UseWinCE
			DIB_RGB_COLORS, /* RGB or palette indices */
#else
			DIB_PAL_COLORS, /* palette indices */
#endif
			SRCCOPY
		) == 0) {
			/* ReportWinLastError(); */
		}
	} else
#endif
	{
		if (SetDIBitsToDevice(
			MainWndDC, /* handle of device context */
#if UseWinCE
			XDest - hOffset, /* x-coordinate of upper-left corner of dest. rect. */
			YDest - vOffset, /* y-coordinate of upper-left corner of dest. rect. */
#else
			XDest, /* x-coordinate of upper-left corner of dest. rect. */
			YDest, /* y-coordinate of upper-left corner of dest. rect. */
#endif
			vMacScreenWidth, /* source rectangle width */
			(bottom - top), /* source rectangle height */
			0, /* x-coordinate of lower-left corner of source rect. */
			0, /* y-coordinate of lower-left corner of source rect. */
			0, /* first scan line in array */
			(bottom - top), /* number of scan lines */
			(CONST VOID *)p, /* address of array with DIB bits */
			(const struct tagBITMAPINFO *)&bmh, /* address of structure with bitmap info. */
#if ! UseWinCE
			DIB_RGB_COLORS /* RGB or palette indices */
#else
			DIB_PAL_COLORS /* palette indices */
#endif
		) == 0) {
			/* ReportWinLastError(); */
		}
	}
}

LOCALPROC Screen_DrawAll(void)
{
#if UseWinCE
	/*
	HaveChangedScreenBuff should figure out for itself
	what part it really needs to draw.
	*/
	HaveChangedScreenBuff(vOffset, 0, vRes + vOffset, hRes + hOffset);
#else
	HaveChangedScreenBuff(0, 0, vMacScreenHeight, vMacScreenWidth);
#endif
}

LOCALFUNC blnr InitTheCursor(void)
{
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	return trueblnr;
}

LOCALPROC CheckMouseState(void)
{
	blnr ShouldHaveCursorHidden;
	POINT NewMousePos;

	ShouldHaveCursorHidden = trueblnr;

	GetCursorPos(&NewMousePos);
	NewMousePos.x -= WndX;
	NewMousePos.y -= WndY;
#if ! UseWinCE
#if EnableFullScreen
	if (UseFullScreen) {
		NewMousePos.x -= hOffset;
		NewMousePos.y -= vOffset;
	}
#endif
#endif
#if EnableMagnify
	if (UseMagnify) {
		NewMousePos.x /= MyWindowScale;
		NewMousePos.y /= MyWindowScale;
	}
#endif

#if EnableMouseMotion
	if (HaveMouseMotion) {
		si4b shiftdh;
		si4b shiftdv;

		MouseMotionH += NewMousePos.x - SavedMouseH;
		MouseMotionV += NewMousePos.y - SavedMouseV;
		if (NewMousePos.x < vMacScreenWidth / 4) {
			shiftdh = vMacScreenWidth / 2;
		} else if (NewMousePos.x > vMacScreenWidth - vMacScreenWidth / 4) {
			shiftdh = - vMacScreenWidth / 2;
		} else {
			shiftdh = 0;
		}
		if (NewMousePos.y < vMacScreenHeight / 4) {
			shiftdv = vMacScreenHeight / 2;
		} else if (NewMousePos.y > vMacScreenHeight - vMacScreenHeight / 4) {
			shiftdv = - vMacScreenHeight / 2;
		} else {
			shiftdv = 0;
		}
		if ((shiftdh != 0) || (shiftdv != 0)) {
			NewMousePos.x += shiftdh;
			NewMousePos.y += shiftdv;
			if (! MyMoveMouse(NewMousePos.x, NewMousePos.y)) {
				HaveMouseMotion = falseblnr;
			}
		}
		SavedMouseH = NewMousePos.x;
		SavedMouseV = NewMousePos.y;
	} else
#endif
	{
		if (NewMousePos.x < 0) {
			NewMousePos.x = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePos.x > vMacScreenWidth) {
			NewMousePos.x = vMacScreenWidth - 1;
			ShouldHaveCursorHidden = falseblnr;
		}
		if (NewMousePos.y < 0) {
			NewMousePos.y = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePos.y > vMacScreenHeight) {
			NewMousePos.y = vMacScreenHeight - 1;
			ShouldHaveCursorHidden = falseblnr;
		}

#if EnableFullScreen
		if (UseFullScreen) {
			ShouldHaveCursorHidden = trueblnr;
		}
#endif

#if ! UseWinCE
		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/* for a game like arkanoid, would like mouse to still
		move even when outside window in one direction */
		{
			CurMouseV = NewMousePos.y;
			CurMouseH = NewMousePos.x;
		}
#endif
	}

	WantCursorHidden = ShouldHaveCursorHidden;
}

#if IncludePbufs
LOCALVAR HGLOBAL PbufDat[NumPbufs];
#endif

#if IncludePbufs
LOCALFUNC tMacErr PbufNewFromHandle(HGLOBAL h, ui5b count, tPbuf *r)
{
	tPbuf i;
	tMacErr err;

	if (! FirstFreePbuf(&i)) {
		(void) GlobalFree(h);
		err = mnvm_miscErr;
	} else {
		*r = i;
		PbufDat[i] = h;
		PbufNewNotify(i, count);
		err = mnvm_noErr;
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALFUNC tMacErr PbufNew(ui5b count, tPbuf *r)
{
	HGLOBAL h;
	tMacErr err = mnvm_miscErr;

	h = GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, count);
	if (h != NULL) {
		/* need to clear h */
		err = PbufNewFromHandle(h, count, r);
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALPROC PbufDispose(tPbuf i)
{
	(void) GlobalFree(PbufDat[i]);
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
	HGLOBAL h = PbufDat[i];
	ui3p p0 = GlobalLock(h);
	if (p0 != NULL) {
		void *p = p0 + offset;
		if (IsWrite) {
			(void) memcpy(p, Buffer, count);
		} else {
			(void) memcpy(Buffer, p, count);
		}
	}
	(void) GlobalUnlock(h);
}
#endif

LOCALVAR const ui3b Native2MacRomanTab[] = {
	0xAD, 0xB0, 0xE2, 0xC4, 0xE3, 0xC9, 0xA0, 0xE0,
	0xF6, 0xE4, 0xB6, 0xDC, 0xCE, 0xB2, 0xB3, 0xB7,
	0xB8, 0xD4, 0xD5, 0xD2, 0xD3, 0xA5, 0xD0, 0xD1,
	0xF7, 0xAA, 0xC5, 0xDD, 0xCF, 0xB9, 0xC3, 0xD9,
	0xCA, 0xC1, 0xA2, 0xA3, 0xDB, 0xB4, 0xBA, 0xA4,
	0xAC, 0xA9, 0xBB, 0xC7, 0xC2, 0xBD, 0xA8, 0xF8,
	0xA1, 0xB1, 0xC6, 0xD7, 0xAB, 0xB5, 0xA6, 0xE1,
	0xFC, 0xDA, 0xBC, 0xC8, 0xDE, 0xDF, 0xF0, 0xC0,
	0xCB, 0xE7, 0xE5, 0xCC, 0x80, 0x81, 0xAE, 0x82,
	0xE9, 0x83, 0xE6, 0xE8, 0xED, 0xEA, 0xEB, 0xEC,
	0xF5, 0x84, 0xF1, 0xEE, 0xEF, 0xCD, 0x85, 0xF9,
	0xAF, 0xF4, 0xF2, 0xF3, 0x86, 0xFA, 0xFB, 0xA7,
	0x88, 0x87, 0x89, 0x8B, 0x8A, 0x8C, 0xBE, 0x8D,
	0x8F, 0x8E, 0x90, 0x91, 0x93, 0x92, 0x94, 0x95,
	0xFD, 0x96, 0x98, 0x97, 0x99, 0x9B, 0x9A, 0xD6,
	0xBF, 0x9D, 0x9C, 0x9E, 0x9F, 0xFE, 0xFF, 0xD8
};

LOCALFUNC tMacErr NativeTextToMacRomanPbuf(HGLOBAL x, tPbuf *r)
{
#if MyUseUni
#define MyUnsignedChar ui4b
#else
#define MyUnsignedChar ui3b
#endif
	HGLOBAL h;
	LPTSTR p1;
	ui5b n;
	MyUnsignedChar v;
	tMacErr err = mnvm_miscErr;

	p1 = GlobalLock(x);
	if (p1 != NULL) {
		n = 0;
		while ((v = *p1++) != 0) {
			if (v != 10) {
				++n;
			}
		}
		(void) GlobalUnlock(x);

		h = GlobalAlloc(GMEM_DDESHARE, n);
		if (h != NULL) {
			p1 = GlobalLock(x);
			if (p1 != NULL) {
				ui3b *p2 = GlobalLock(h);
				if (p2 != NULL) {
					while ((v = (MyUnsignedChar)*p1++) != 0) {
						if (v >= 128) {
							*p2++ = Native2MacRomanTab[v & 0x7F];
								/*
									if MyUseUni, then for gives
									garbage for v > 256.
								*/
						} else if (v != 10) {
							*p2++ = v;
						}
					}

					err = mnvm_noErr;

					(void) GlobalUnlock(h);
				}
				(void) GlobalUnlock(x);
			}

			if (mnvm_noErr != err) {
				(void) GlobalFree(h);
			} else {
				err = PbufNewFromHandle(h, n, r);
			}
		}
	}

	return err;
}

LOCALVAR const ui3b MacRoman2NativeTab[] = {
	0xC4, 0xC5, 0xC7, 0xC9, 0xD1, 0xD6, 0xDC, 0xE1,
	0xE0, 0xE2, 0xE4, 0xE3, 0xE5, 0xE7, 0xE9, 0xE8,
	0xEA, 0xEB, 0xED, 0xEC, 0xEE, 0xEF, 0xF1, 0xF3,
	0xF2, 0xF4, 0xF6, 0xF5, 0xFA, 0xF9, 0xFB, 0xFC,
	0x86, 0xB0, 0xA2, 0xA3, 0xA7, 0x95, 0xB6, 0xDF,
	0xAE, 0xA9, 0x99, 0xB4, 0xA8, 0x80, 0xC6, 0xD8,
	0x81, 0xB1, 0x8D, 0x8E, 0xA5, 0xB5, 0x8A, 0x8F,
	0x90, 0x9D, 0xA6, 0xAA, 0xBA, 0xAD, 0xE6, 0xF8,
	0xBF, 0xA1, 0xAC, 0x9E, 0x83, 0x9A, 0xB2, 0xAB,
	0xBB, 0x85, 0xA0, 0xC0, 0xC3, 0xD5, 0x8C, 0x9C,
	0x96, 0x97, 0x93, 0x94, 0x91, 0x92, 0xF7, 0xB3,
	0xFF, 0x9F, 0xB9, 0xA4, 0x8B, 0x9B, 0xBC, 0xBD,
	0x87, 0xB7, 0x82, 0x84, 0x89, 0xC2, 0xCA, 0xC1,
	0xCB, 0xC8, 0xCD, 0xCE, 0xCF, 0xCC, 0xD3, 0xD4,
	0xBE, 0xD2, 0xDA, 0xDB, 0xD9, 0xD0, 0x88, 0x98,
	0xAF, 0xD7, 0xDD, 0xDE, 0xB8, 0xF0, 0xFD, 0xFE
};

LOCALFUNC blnr MacRomanTextToNativeHand(tPbuf Pbuf_no, blnr IsFileName, HGLOBAL *r)
{
	HGLOBAL h;
	ui5b i;
	ui5b rn = 0;
	HGLOBAL bh = PbufDat[Pbuf_no];
	ui5b L = PbufSize[Pbuf_no];
	blnr IsOk = falseblnr;

	if (IsFileName) {
		if (L > 255) {
			L = 255;
		}
	} else {
		ui3b *Buffer = (ui3b *)GlobalLock(bh);
		if (Buffer != NULL) {
			for (i = 0; i < L; ++i) {
				if (Buffer[i] == 13) {
					++rn;
				}
			}
			(void) GlobalUnlock(bh);
		}
	}

	h = GlobalAlloc(GMEM_DDESHARE, (L + rn + 1) * sizeof(TCHAR));
	if (h != NULL) {
		ui3b *Buffer = (ui3b *)GlobalLock(bh);
		if (Buffer != NULL) {
			LPTSTR p1 = GlobalLock(h);
			if (p1 != NULL) {
				for (i = 0; i < L; ++i) {
					TCHAR y;
					ui3b x = ((ui3b *)Buffer)[i];
					if (x >= 128) {
						y = (TCHAR)MacRoman2NativeTab[x - 128];
					} else {
						if (IsFileName) {
							if ((x < 32)
								|| ('\\' == x) || ('/' == x)
								|| (':' == x) || ('*' == x)
								|| ('?' == x) || ('"' == x)
								|| ('<' == x) || ('>' == x)
								|| ('|' == x))
							{
								y = (TCHAR)('-');
							} else {
								y = (TCHAR)x;
							}
						} else {
							if (13 == x) {
								*p1++ = (TCHAR)(13);
								y = (TCHAR)(10);
							} else {
								y = (TCHAR)x;
							}
						}
					}
					*p1++ = y;
				}
				*p1++ = (TCHAR) 0; /* null character */

				*r = h;
				IsOk = trueblnr;

				(void) GlobalUnlock(h);
			}
			(void) GlobalUnlock(bh);
		}
		if (! IsOk) {
			(void) GlobalFree(h);
		}
	}

	return IsOk;
}

#if IncludeHostTextClipExchange
GLOBALFUNC tMacErr HTCEexport(tPbuf i)
{
	HGLOBAL h;
	tMacErr err = mnvm_miscErr;

	if (MacRomanTextToNativeHand(i, falseblnr, &h)) {
		if (! OpenClipboard(MainWnd)) {
			/* ReportGetLastError(); */
		} else {
			if (! EmptyClipboard()) {
				/* ReportGetLastError(); */
			}
			if (SetClipboardData(CF_TEXT, h) == NULL) {
				/* ReportGetLastError(); */
			} else {
				err = mnvm_noErr;
			}
			h = NULL;
			if (! CloseClipboard()) {
				/* ReportGetLastError(); */
			}
		}
		if (h != NULL) {
			(void) GlobalFree(h);
		}
	}

	PbufDispose(i);

	return err;
}
#endif

#if IncludeHostTextClipExchange
GLOBALFUNC tMacErr HTCEimport(tPbuf *r)
{
	tMacErr err = mnvm_miscErr;

	if (IsClipboardFormatAvailable(CF_TEXT)) {
		if (! OpenClipboard(MainWnd)) {
			/* ReportGetLastError(); */
		} else {
			HGLOBAL h = GetClipboardData(CF_TEXT);
			if (h == NULL) {
				/* ReportGetLastError(); */
			} else {
				err = NativeTextToMacRomanPbuf(h, r);
			}
			if (! CloseClipboard()) {
				/* ReportGetLastError(); */
			}
		}
	}

	return err;
}
#endif

/*--- drives ---*/

#define NotAfileRef INVALID_HANDLE_VALUE

LOCALVAR HANDLE Drives[NumDrives]; /* open disk image files */
#if IncludeSonyGetName || IncludeSonyNew
LOCALVAR HGLOBAL DriveNames[NumDrives];
	/*
		It is supposed to be possible to use
		GetMappedFileName to get name of open file,
		but that seems ugly kludge, so instead
		save the name on open.
	*/
#endif

LOCALPROC InitDrives(void)
{
	/*
		This isn't really needed, Drives[i] and DriveNames[i]
		need not have valid values when not vSonyIsInserted[i].
	*/
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
#if IncludeSonyGetName || IncludeSonyNew
		DriveNames[i] = NULL;
#endif
	}
}

GLOBALFUNC tMacErr vSonyRead(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count)
{
	tMacErr result;
	HANDLE refnum;
	DWORD newL;
	DWORD BytesRead = 0;

	refnum = Drives[Drive_No];
	newL = SetFilePointer(
		refnum, /* handle of file */
		Sony_Start, /* number of bytes to move file pointer */
		nullpr, /* address of high-order word of distance to move */
		FILE_BEGIN /* how to move */
	);
	if (newL == 0xFFFFFFFF) {
		result = mnvm_miscErr; /*& figure out what really to return &*/
	} else if (Sony_Start != (ui5b)newL) {
		/* not supposed to get here */
		result = mnvm_miscErr; /*& figure out what really to return &*/
	} else {
		if (! ReadFile(refnum, /* handle of file to read */
			(LPVOID)Buffer, /* address of buffer that receives data */
			(DWORD)*Sony_Count, /* number of bytes to read */
			&BytesRead, /* address of number of bytes read */
			nullpr)) /* address of structure for data */
		{
			result = mnvm_miscErr; /*& figure out what really to return &*/
		} else if ((ui5b)BytesRead != *Sony_Count) {
			result = mnvm_miscErr; /*& figure out what really to return &*/
		} else {
			result = mnvm_noErr;
		}
	}
	*Sony_Count = BytesRead;
	return result;
}

GLOBALFUNC tMacErr vSonyWrite(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count)
{
	tMacErr result;
	HANDLE refnum;
	DWORD newL;
	DWORD BytesWritten = 0;

	refnum = Drives[Drive_No];
	newL = SetFilePointer(
		refnum, /* handle of file */
		Sony_Start, /* number of bytes to move file pointer */
		nullpr, /* address of high-order word of distance to move */
		FILE_BEGIN /* how to move */
	);
	if (newL == 0xFFFFFFFF) {
		result = mnvm_miscErr; /*& figure out what really to return &*/
	} else if (Sony_Start != (ui5b)newL) {
		/* not supposed to get here */
		result = mnvm_miscErr; /*& figure out what really to return &*/
	} else {
		if (! WriteFile(refnum, /* handle of file to read */
			(LPVOID)Buffer, /* address of buffer that receives data */
			(DWORD)*Sony_Count, /* number of bytes to read */
			&BytesWritten, /* address of number of bytes read */
			nullpr)) /* address of structure for data */
		{
			result = mnvm_miscErr; /*& figure out what really to return &*/
		} else if ((ui5b)BytesWritten != *Sony_Count) {
			result = mnvm_miscErr; /*& figure out what really to return &*/
		} else {
			result = mnvm_noErr;
		}
	}
	*Sony_Count = BytesWritten;
	return result;
}

GLOBALFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
	tMacErr result;
	DWORD L;

	L = GetFileSize (Drives[Drive_No], nullpr);
	if (L == 0xFFFFFFFF) {
		result = mnvm_miscErr; /*& figure out what really to return &*/
	} else {
		*Sony_Count = L;
		result = mnvm_noErr;
	}
	return result;
}

LOCALFUNC tMacErr vSonyEject0(tDrive Drive_No, blnr deleteit)
{
	HANDLE refnum = Drives[Drive_No];
	Drives[Drive_No] = NotAfileRef; /* not really needed */

	DiskEjectedNotify(Drive_No);

	(void) FlushFileBuffers(refnum);
	(void) CloseHandle(refnum);

#if IncludeSonyGetName || IncludeSonyNew
	{
		HGLOBAL h = DriveNames[Drive_No];
		if (NULL != h) {
			if (deleteit) {
				LPTSTR drivepath = GlobalLock(h);
				if (drivepath != NULL) {
					(void) DeleteFile(drivepath);
					(void) GlobalUnlock(h);
				}
			}
			(void) GlobalFree(h);
			DriveNames[Drive_No] = NULL; /* not really needed */
		}
	}
#endif

	return mnvm_noErr;
}

GLOBALFUNC tMacErr vSonyEject(tDrive Drive_No)
{
	return vSonyEject0(Drive_No, falseblnr);
}

#if IncludeSonyNew
GLOBALFUNC tMacErr vSonyEjectDelete(tDrive Drive_No)
{
	return vSonyEject0(Drive_No, trueblnr);
}
#endif

LOCALPROC UnInitDrives(void)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			(void) vSonyEject(i);
		}
	}
}

#if IncludeSonyGetName || IncludeSonyNew
LOCALFUNC blnr LPTSTRtoHand(LPTSTR s, HGLOBAL *r)
{
	blnr IsOk = falseblnr;

	size_t L = _tcslen(s);
	HGLOBAL h = GlobalAlloc(GMEM_DDESHARE,
		(L + 1) * sizeof(TCHAR));
	if (h != NULL) {
		LPTSTR p = GlobalLock(h);
		if (p != NULL) {
			_tcscpy(p, s);
			IsOk = trueblnr;
			(void) GlobalUnlock(h);
		}
		if (! IsOk) {
			(void) GlobalFree(h);
		} else {
			*r = h;
		}
	}

	return IsOk;
}
#endif

#if IncludeSonyGetName
GLOBALFUNC tMacErr vSonyGetName(tDrive Drive_No, tPbuf *r)
{
	WIN32_FIND_DATA fd;
	tMacErr err = mnvm_miscErr;
	HGLOBAL ph = DriveNames[Drive_No];
	if (NULL != ph) {
		LPTSTR drivepath = GlobalLock(ph);
		if (drivepath != NULL) {
			HANDLE hf = FindFirstFile(drivepath, &fd);
			(void) GlobalUnlock(ph);

			if (hf != INVALID_HANDLE_VALUE) {
				HGLOBAL h;
				if (LPTSTRtoHand(fd.cFileName, &h)) {
					err = NativeTextToMacRomanPbuf(h, r);
				}
				FindClose(hf);
			}
		}
	}

	return err;
}
#endif

LOCALFUNC blnr Sony_Insert0(HANDLE refnum, blnr locked, LPTSTR drivepath)
{
	tDrive Drive_No;

	if (! FirstFreeDisk(&Drive_No)) {
		(void) CloseHandle(refnum);
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage, falseblnr);
		return falseblnr;
	} else {
		Drives[Drive_No] = refnum;
		DiskInsertNotify(Drive_No, locked);
#if IncludeSonyGetName || IncludeSonyNew
		{
			HGLOBAL h;

			if (! LPTSTRtoHand(drivepath, &h)) {
				h = NULL;
			}

			DriveNames[Drive_No] = h;
		}
#endif
		return trueblnr;
	}
}

LOCALFUNC blnr Sony_Insert1(LPTSTR drivepath, blnr SilentOnMissing)
{
	blnr locked = falseblnr;
	HANDLE refnum = CreateFile(
		drivepath, /* pointer to name of the file */
		GENERIC_READ + GENERIC_WRITE, /* access (read-write) mode */
		0, /* share mode */
		nullpr, /* pointer to security descriptor */
		OPEN_EXISTING, /* how to create */
		FILE_ATTRIBUTE_NORMAL, /* file attributes */
		nullpr /* handle to file with attributes to copy */
	);
	if (refnum == INVALID_HANDLE_VALUE) {
		if (ERROR_ACCESS_DENIED == GetLastError()) {
			locked = trueblnr;
			refnum = CreateFile(
				drivepath, /* pointer to name of the file */
				GENERIC_READ, /* access (read-write) mode */
				FILE_SHARE_READ, /* share mode */
				nullpr, /* pointer to security descriptor */
				OPEN_EXISTING, /* how to create */
				FILE_ATTRIBUTE_NORMAL, /* file attributes */
				nullpr /* handle to file with attributes to copy */
			);
		}
	}
	if (refnum == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		if (ERROR_SHARING_VIOLATION == err) {
			MacMsg(kStrImageInUseTitle, kStrImageInUseMessage, falseblnr);
		} else if ((ERROR_FILE_NOT_FOUND == err) && SilentOnMissing) {
			/* ignore it */
		} else {
			MacMsg(kStrOpenFailTitle, kStrOpenFailMessage, falseblnr);
		}
	} else {
		return Sony_Insert0(refnum, locked, drivepath);
	}
	return falseblnr;
}

#ifndef EnableShellLinks
#define EnableShellLinks 1
#endif

#if EnableShellLinks
LOCALVAR blnr COMinited = falseblnr;
LOCALVAR blnr COMinitedOK;
#endif

#if EnableShellLinks
LOCALPROC MyUninitCOM(void)
{
	if (COMinited) {
		CoUninitialize();
	}
}
#endif

#if EnableShellLinks
LOCALFUNC blnr MyNeedCOM(void)
{
	HRESULT hres;

	if (! COMinited) {
		COMinitedOK = falseblnr;
		hres = CoInitialize(NULL);
		if (SUCCEEDED(hres)) {
			COMinitedOK = trueblnr;
		}

		COMinited = trueblnr;
	}
	return COMinitedOK;
}
#endif

#if EnableShellLinks
LOCALFUNC blnr MyResolveShortcut(LPTSTR FilePath)
/* adapted from Microsoft example code */
{
	HRESULT hres;
	IShellLink *psl;
	IPersistFile* ppf;
	TCHAR szGotPath[MAX_PATH];
	WIN32_FIND_DATA wfd;
	blnr IsOk = falseblnr;

	if (MyNeedCOM()) {
		hres = CoCreateInstance(&CLSID_ShellLink, NULL,
			CLSCTX_INPROC_SERVER, &IID_IShellLink,
			(LPVOID *)(void *)&psl);
			/*
				the (void *) prevents a compiler warning
				from gcc
			*/
		if (SUCCEEDED(hres)) {
			/* Get a pointer to the IPersistFile interface. */
			hres = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile,
				(void **)(void *)&ppf);
			if (SUCCEEDED(hres)) {
				/* Ensure that the string is Unicode. */
#if MyUseUni
#define wsz FilePath
#else
				WORD wsz[MAX_PATH];
				MultiByteToWideChar(CP_ACP, 0, FilePath, -1, wsz,
					MAX_PATH);
#endif

				/* Load the shortcut. */
				hres = ppf->lpVtbl->Load(ppf, wsz, STGM_READ);
				if (SUCCEEDED(hres)) {
					/* Resolve the link. */
					hres = psl->lpVtbl->Resolve(psl, MainWnd, SLR_ANY_MATCH);
					if (SUCCEEDED(hres)) {
						/* Get the path to the link target. */
						hres = psl->lpVtbl->GetPath(psl, szGotPath,
							MAX_PATH, (WIN32_FIND_DATA *)&wfd,
							SLGP_SHORTPATH);
						if (SUCCEEDED(hres)) {
							/*
							This is in the sample code, but doesn't
							seem to be needed:
							Get the description of the target.
							char szDescription[MAX_PATH];
							hres = psl->lpVtbl->GetDescription(psl,
								szDescription, MAX_PATH);
							if (SUCCEEDED(hres)) {
							}
							*/
							lstrcpy(FilePath, szGotPath);
							IsOk = trueblnr;
						}
					}
				}

				ppf->lpVtbl->Release(ppf);
			}
			psl->lpVtbl->Release(psl);
		}
	}
	return IsOk;
}
#endif

#if EnableShellLinks
LOCALFUNC blnr InsertAnAlias(LPTSTR FilePath)
{
	if (MyResolveShortcut(FilePath)) {
		return Sony_Insert1(FilePath, falseblnr);
	}
	return falseblnr;
}
#endif

#if EnableShellLinks
LOCALFUNC blnr FileIsLink(LPTSTR drivepath)
{
	LPTSTR p = FindLastTerm(drivepath, (TCHAR)('.'));

	if (p != nullpr) {
		if (_tcscmp(p, TEXT("lnk")) == 0) {
			return trueblnr;
		}
	}
	return falseblnr;
}
#endif

LOCALFUNC blnr InsertDiskOrAlias(LPTSTR drivepath)
{
#if EnableShellLinks
	if (FileIsLink(drivepath)) {
		return InsertAnAlias(drivepath);
	} else
#endif
	{
		return Sony_Insert1(drivepath, falseblnr);
	}
}

#if EnableShellLinks
LOCALFUNC blnr MyFileExists(LPTSTR pathName)
{
	WIN32_FIND_DATA fd;
	HANDLE hf = FindFirstFile(pathName, &fd);

	if (hf == INVALID_HANDLE_VALUE) {
		return falseblnr;
	} else {
		FindClose(hf);
		return trueblnr;
	}
}
#endif

LOCALPROC InsertADisk0(void)
{
	OPENFILENAME ofn;
	TCHAR szDirName[256];
	TCHAR szFile[256];
#if ! UseWinCE
	TCHAR szFileTitle[256];
#endif
	UINT i;
	size_t cbString;
	TCHAR chReplace;
	TCHAR szFilter[256];
	blnr IsOk;

	szDirName[0] = (TCHAR)('\0');
	szFile[0] = (TCHAR)('\0');
	_tcscpy(szFilter, TEXT("Disk images|*.dsk;*.HF?;*.IMG;*.IMA;*.IMAGE|All files (*.*)|*.*|\0"));

	cbString = _tcslen(szFilter);

	chReplace = szFilter[cbString - 1];

	for (i = 0; szFilter[i] != (TCHAR)('\0'); ++i)
	{
		if (szFilter[i] == chReplace) {
			szFilter[i] = (TCHAR)('\0');
		}
	}

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = MainWnd;
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 2;
	ofn.lpstrFile= szFile;
	ofn.nMaxFile = sizeof(szFile);
#if ! UseWinCE
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = sizeof(szFileTitle);
#endif
	ofn.lpstrInitialDir = szDirName;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	MyBeginDialog();
	IsOk = GetOpenFileName(&ofn);
	MyEndDialog();

	if(! IsOk) {
		/* report error */
#if UseWinCE
		if (szFile[0]) {
			char wMsg[1024];
			sprintf(wMsg, "Couldn't open %ls", szFile);
			MacMsg("error", wMsg, falseblnr);
		}
#endif
	} else {
		(void) Sony_Insert1(ofn.lpstrFile, falseblnr);
	}

#if UseWinCE
	CtrlMode = falseblnr;
	SipOn = falseblnr;
#endif
}

LOCALFUNC blnr GetAppDir (LPTSTR pathName)
/* be sure at least _MAX_PATH long! */
{
	if (GetModuleFileName(AppInstance, pathName, _MAX_PATH) == 0) {
		MacMsg("error", "GetModuleFileName failed", falseblnr);
	} else {
		LPTSTR p = FindLastTerm(pathName,
			(TCHAR)('\\'));
		if (p == nullpr) {
			MacMsg("error", "strrchr failed", falseblnr);
		} else {
			*p = (TCHAR)('\0');
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALFUNC blnr LoadInitialImageFromName(LPTSTR ImageName)
{
	TCHAR ImageFile[_MAX_PATH];

	if (GetAppDir(ImageFile)) {
		_tcscat(ImageFile, ImageName);
		if (Sony_Insert1(ImageFile, trueblnr)) {
			return trueblnr;
		} else {
#if EnableShellLinks
			_tcscat(ImageFile, TEXT(".lnk"));
			if (MyFileExists(ImageFile)) {
				if (InsertAnAlias(ImageFile)) {
					return trueblnr;
				}
			}
#endif
		}
	}
	return falseblnr;
}

LOCALFUNC blnr LoadInitialImages(void)
{
	TCHAR s[16];
	int i;
	int n = NumDrives > 9 ? 9 : NumDrives;

	_tcscpy(s, TEXT("disk?.dsk"));

	for (i = 1; i <= n; ++i) {
		s[4] = (TCHAR)('0' + i);
		if (! LoadInitialImageFromName(s)) {
			/* stop on first error (including file not found) */
			return trueblnr;
		}
	}

	return trueblnr;
}

#if IncludeSonyNew
LOCALFUNC blnr WriteZero(HANDLE refnum, ui5b L)
{
	if (SetFilePointer(
		refnum, /* handle of file */
		0, /* number of bytes to move file pointer */
		nullpr, /* address of high-order word of distance to move */
		FILE_BEGIN /* how to move */
		) != 0)
	{
		return falseblnr;
	} else {
#define ZeroBufferSize 2048
		ui5b i;
		ui3b buffer[ZeroBufferSize];
		DWORD BytesWritten;

		memset(&buffer, 0, ZeroBufferSize);

		while (L > 0) {
			i = (L > ZeroBufferSize) ? ZeroBufferSize : L;
			if (! WriteFile(refnum, /* handle of file to read */
				(LPVOID)buffer, /* address of buffer that receives data */
				(DWORD)i, /* number of bytes to read */
				&BytesWritten, /* address of number of bytes read */
				nullpr) /* address of structure for data */
				|| ((ui5b)BytesWritten != i))
			{
				return falseblnr;
			}
			L -= i;
		}
		return trueblnr;
	}
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDisk(ui5b L, HGLOBAL NewDiskNameDat)
{
	OPENFILENAME ofn;
	blnr IsOk = falseblnr;
	TCHAR szFile[256];
	TCHAR szFileTitle[256];
	HANDLE newrefNum;

	memset(&ofn, 0, sizeof(OPENFILENAME));

#if IncludeSonyGetName
	if (NewDiskNameDat != NULL) {
		LPTSTR p = GlobalLock(NewDiskNameDat);
		if (p != NULL) {
			_tcscpy(szFile, p);
			(void) GlobalUnlock(NewDiskNameDat);
		}
	} else
#endif
	{
		NativeStrFromCStr(szFile, "untitled", falseblnr);
	}

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = szFile;
	ofn.hwndOwner = MainWnd;
	/* ofn.lpstrFilter = "All\0*.*\0Text\0*.txt\0Datafile\0*.dsk\0"; */
	/* ofn.lpstrFilter = NULL */ /* szFilter */;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = sizeof(szFileTitle);
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT + OFN_HIDEREADONLY /* + OFN_SHOWHELP */;

	MyBeginDialog();
	IsOk = GetSaveFileName(&ofn);
	MyEndDialog();

	if (! IsOk) {
		/* report error */
	} else {
		IsOk = falseblnr;
		newrefNum = CreateFile(
			ofn.lpstrFile, /* pointer to name of the file */
			GENERIC_READ + GENERIC_WRITE, /* access (read-write) mode */
			0, /* share mode */
			NULL, /* pointer to security descriptor */
			CREATE_ALWAYS, /* how to create */
			FILE_ATTRIBUTE_NORMAL, /* file attributes */
			NULL /* handle to file with attributes to copy */
		);
		if (newrefNum == INVALID_HANDLE_VALUE) {
			/* report error */
		} else {
			if (SetFilePointer(
				newrefNum, /* handle of file */
				L, /* number of bytes to move file pointer */
				nullpr, /* address of high-order word of distance to move */
				FILE_BEGIN /* how to move */
				) != L)
			{
				/* report error */
			} else if (! SetEndOfFile(newrefNum)) {
				/* report error */
			} else if (! WriteZero(newrefNum, L)) {
				/* report error */
			} else {
				IsOk = Sony_Insert0(newrefNum, falseblnr, ofn.lpstrFile);
				newrefNum = INVALID_HANDLE_VALUE;
			}
			if (INVALID_HANDLE_VALUE != newrefNum) {
				(void) CloseHandle(newrefNum);
			}
			if (! IsOk) {
				(void) DeleteFile(ofn.lpstrFile);
			}
		}
	}
}
#endif

LOCALFUNC blnr AllocateMacROM(void)
{
	ROM = (ui3p)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, kROM_Size);
	if (ROM == NULL) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALFUNC blnr LoadMacRom(void)
{
	TCHAR ROMFile[_MAX_PATH];
	HANDLE refnum;
	blnr IsOk = falseblnr;

	if (! GetAppDir(ROMFile)) {
		MacMsg("Mini vMac error", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
	} else {
		_tcscat(ROMFile, TEXT(RomFileName));

		refnum = CreateFile(
			ROMFile, /* pointer to name of the file */
			GENERIC_READ, /* access (read-write) mode */
			FILE_SHARE_READ, /* share mode */
			nullpr, /* pointer to security descriptor */
			OPEN_EXISTING, /* how to create */
			FILE_ATTRIBUTE_NORMAL, /* file attributes */
			nullpr /* handle to file with attributes to copy */
		);
#if EnableShellLinks
		if (refnum == INVALID_HANDLE_VALUE) {
			_tcscat(ROMFile, TEXT(".lnk"));
			if (MyFileExists(ROMFile))
			if (MyResolveShortcut(ROMFile))
			{
				refnum = CreateFile(
					ROMFile, /* pointer to name of the file */
					GENERIC_READ, /* access (read-write) mode */
					FILE_SHARE_READ, /* share mode */
					nullpr, /* pointer to security descriptor */
					OPEN_EXISTING, /* how to create */
					FILE_ATTRIBUTE_NORMAL, /* file attributes */
					nullpr /* handle to file with attributes to copy */
				);
			}
		}
#endif
		if (refnum == INVALID_HANDLE_VALUE) {
			MacMsg(kStrNoROMTitle, kStrNoROMMessage, trueblnr);
			SpeedStopped = trueblnr;
		} else {
			DWORD BytesRead;

			if (! ReadFile(refnum, /* handle of file to read */
				(LPVOID)ROM, /* address of buffer that receives data */
				(DWORD)kTrueROM_Size, /* number of bytes to read */
				&BytesRead, /* address of number of bytes read */
				nullpr)) /* address of structure for data */
			{
				MacMsg(kStrNoReadROMTitle, kStrNoReadROMMessage, trueblnr);
				SpeedStopped = trueblnr;
			} else if ((ui5b)BytesRead != kTrueROM_Size) {
				MacMsg(kStrShortROMTitle, kStrShortROMMessage, trueblnr);
				SpeedStopped = trueblnr;
			}
			(void) CloseHandle(refnum);
		}
		IsOk = trueblnr;
	}
	return IsOk;
}

LOCALFUNC blnr AllocateMacRAM(void)
{
	RAM = (ui3p)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, kRAM_Size + RAMSafetyMarginFudge);
	if (RAM == NULL) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

#if InstallFileIcons
LOCALPROC MySetRegKey(HKEY hKeyRoot, LPTSTR strRegKey, LPTSTR strRegValue)
{
	HKEY RegKey;
	DWORD dwDisposition;

	if (ERROR_SUCCESS == RegCreateKeyEx(hKeyRoot, strRegKey, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
		NULL, &RegKey, &dwDisposition))
	{
		RegSetValueEx(RegKey, NULL, 0, REG_SZ,
			(CONST BYTE *)strRegValue,
			(_tcslen(strRegValue) + 1) * sizeof(TCHAR));
		RegCloseKey(RegKey);
	}
}

LOCALPROC RegisterShellFileType (LPTSTR AppPath, LPTSTR strFilterExt,
	LPTSTR strFileTypeId, LPTSTR strFileTypeName,
	LPTSTR strIconId, blnr CanOpen)
{
	TCHAR strRegKey[_MAX_PATH];
	TCHAR strRegValue[_MAX_PATH + 2]; /* extra room for ","{strIconId} */

	MySetRegKey(HKEY_CLASSES_ROOT, strFileTypeId, strFileTypeName);
	MySetRegKey(HKEY_CLASSES_ROOT, strFilterExt, strFileTypeId);

	_tcscpy(strRegKey, strFileTypeId);
	_tcscat(strRegKey, TEXT("\\DefaultIcon"));
	_tcscpy(strRegValue, AppPath);
	_tcscat(strRegValue, TEXT(","));
	_tcscat(strRegValue, strIconId);
	MySetRegKey(HKEY_CLASSES_ROOT, strRegKey, strRegValue);

	if (CanOpen) {
		_tcscpy(strRegKey, strFileTypeId);
		_tcscat(strRegKey, TEXT("\\shell\\open\\command"));
		_tcscpy(strRegValue, AppPath);
		_tcscat(strRegValue, TEXT(" \"%1\""));
		MySetRegKey(HKEY_CLASSES_ROOT, strRegKey, strRegValue);
	}
}

LOCALFUNC blnr RegisterInRegistry(void)
{
	TCHAR AppPath[_MAX_PATH];

	GetModuleFileName(NULL, AppPath, _MAX_PATH);
	GetShortPathName(AppPath, AppPath, _MAX_PATH);

	RegisterShellFileType(AppPath, TEXT(".rom"), TEXT("minivmac.rom"),
		TEXT("Mini vMac ROM Image"), TEXT("1"), falseblnr);
	RegisterShellFileType(AppPath, TEXT(".dsk"), TEXT("minivmac.dsk"),
		TEXT("Mini vMac Disk Image"), TEXT("2"), trueblnr);

	return trueblnr;
}
#endif

LOCALVAR LPTSTR CommandLine;

LOCALFUNC blnr ScanCommandLine(void)
{
	TCHAR fileName[_MAX_PATH];
	TCHAR *filePtr;
	TCHAR *p = CommandLine;

	while (*p != 0) {
		if (*p == ' ') {
			++p;
		} else if (*p == '/') {
			++p;
			if (*p == 'l') {
				++p;
				SpeedValue = 0;
			} else {
				MacMsg(kStrBadArgTitle, kStrBadArgMessage, falseblnr);
			}
		} else {
			filePtr = fileName;
			if (*p == '\"') {
				++p;
				while (*p != '\"' && *p != 0) {
					*filePtr++ = *p++;
				}
				if (*p == '\"') {
					++p;
				}
			} else {
				while (*p != ' ' && *p != 0) {
					*filePtr++ = *p++;
				}
			}
			*filePtr = (char)0;
			(void) InsertDiskOrAlias(fileName);
		}
	}

	return trueblnr;
}

#if EnableMagnify || EnableFullScreen
LOCALPROC CheckMagnifyAndFullScreen(void)
{
	if (
#if EnableMagnify
		(UseMagnify != WantMagnify)
#endif
#if EnableMagnify && EnableFullScreen
		||
#endif
#if EnableFullScreen
		(UseFullScreen != WantFullScreen)
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
			WantMagnify = (NewMagState == kMagStateMagnifgy);
		} else {
			WantMagnify = falseblnr;
			if (WantFullScreen) {
				if ((GetSystemMetrics(SM_CXSCREEN) >= vMacScreenWidth * MyWindowScale)
					&& (GetSystemMetrics(SM_CYSCREEN) >= vMacScreenHeight * MyWindowScale)
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

#if EnableDragDrop
LOCALPROC DragFunc(HDROP hDrop)
{
	WORD n;
	WORD i;
	TCHAR a[_MAX_PATH];

	n = DragQueryFile(hDrop, (UINT) -1, NULL, 0);
	for (i = 0; i < n; ++i) {
		if (DragQueryFile(hDrop, i, NULL, 0) < _MAX_PATH - 1) {
			(void) DragQueryFile(hDrop, i, a, _MAX_PATH);
			(void) InsertDiskOrAlias(a);
		}
	}

	DragFinish(hDrop);

	if (gTrueBackgroundFlag) {
		if (! SetForegroundWindow(MainWnd)) {
			/* error message here ? */
		}

		WantCmdOptOnReconnect = trueblnr;
	}
}
#endif

LOCALVAR ui5b OnTrueTime = 0;

GLOBALFUNC blnr ExtraTimeNotOver(void)
{
#if MySoundEnabled
	SoundCheckVeryOften();
#endif
#if ! UseTimerThread
	(void) UpdateTrueEmulatedTime();
#endif
	return (TrueEmulatedTime == OnTrueTime)
#if UseTimerThread
		&& MyTimerIsRunning
#endif
		;
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

#if EnableGrabSpecialKeys
			CheckForLostKeyUps();
#endif
		}

		do {
			DoEmulateOneTick();
			++CurEmulatedTime;
		} while (ExtraTimeNotOver()
			&& (--n > 0));

		Screen_Draw(n);
	}
}

LOCALPROC LeaveBackground(void)
{
	ReconnectKeyCodes3();
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
#if UseTimerThread
	MyTimer_Resume();
#endif
}

LOCALPROC EnterSpeedStopped(void)
{
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
#if UseTimerThread
	MyTimer_Suspend();
#endif
}

LOCALPROC CheckForSavedTasks(void)
{
	/*
		Check for things to do that rather wouldn't
		have done at an awkward time.
	*/

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

	if (gTrueBackgroundFlag || ADialogIsUp) {
		/*
			wait til later
			(shouldn't actually be possible to
			get here if ADialogIsUp, but leave
			test in to match mac version.)
		*/
	} else {
#if IncludeSonyNew
		if (vSonyNewDiskWanted) {
#if IncludeSonyNameNew
			if (vSonyNewDiskName != NotAPbuf) {
				HGLOBAL NewDiskNameDat;
				if (MacRomanTextToNativeHand(vSonyNewDiskName, trueblnr,
					&NewDiskNameDat))
				{
					MakeNewDisk(vSonyNewDiskSize, NewDiskNameDat);
					GlobalFree(NewDiskNameDat);
				}
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

	if (HaveCursorHidden != (WantCursorHidden
		&& ! (gTrueBackgroundFlag || ADialogIsUp || CurSpeedStopped)))
	{
		HaveCursorHidden = ! HaveCursorHidden;
		if (HaveCursorHidden) {
			(void) ShowCursor(FALSE);
		} else {
			(void) ShowCursor(TRUE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}

	if ((nullpr != SavedBriefMsg) & ! MacMsgDisplayed) {
		MacMsgDisplayOn();
	}

	if (NeedWholeScreenDraw) {
		NeedWholeScreenDraw = falseblnr;
#if UseWinCE
		HaveChangedScreenBuff(vOffset, 0, vRes + vOffset, hRes + hOffset);
#else
		HaveChangedScreenBuff(0, 0, vMacScreenHeight, vMacScreenWidth);
#endif
	}
}

#if UseTimerThread
LOCALVAR blnr GotTheTick = falseblnr;
#endif

LRESULT CALLBACK Win32WMProc(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam);

LRESULT CALLBACK Win32WMProc(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	switch (uMessage)
	{
#if UseTimerThread
		case WM_USER:
			OnTrueTime = TrueEmulatedTime;
			RunEmulatedTicksToTrueTime();
			GotTheTick = trueblnr;
			break;
#endif
		case WM_MOUSEMOVE:
#if UseWinCE
#if EnableFullScreen
			CurMouseV = WantFullScreen? ((HIWORD (lparam)) / 1.6) : HIWORD (lparam) + vOffset;
			CurMouseH = WantFullScreen? ((LOWORD (lparam)) / 1.6) : LOWORD (lparam) + hOffset;
#else
			CurMouseV = HIWORD (lparam) + vOffset;
			CurMouseH = LOWORD (lparam) + hOffset;
#endif
#endif
			/* windows may have messed up cursor */
			/*
				there is no notification when the mouse moves
				outside the window, and the cursor is automatically
				changed
			*/
			if (! HaveCursorHidden) {
				/* SetCursor(LoadCursor(NULL, IDC_ARROW)); */
			}
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;

				BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
#if EnableFullScreen
				if (UseFullScreen) {
					if (! FillRect(ps.hdc,
						&ps.rcPaint,
						GetStockObject(BLACK_BRUSH)))
					{
						/* ReportGetLastError(); */
					}
				}
#endif
				if (MainWnd == hwnd) {
					Screen_DrawAll();
				}
				EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
			}
			break;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
#if UseWinCE
			if (wparam == 0xAE) {
				break;
			} else if ((! SipOn) && (wparam == VK_RETURN)) {
				break;
			} else if ((! SipOn) && (wparam >= VK_LEFT) && (wparam <= VK_DOWN)) {
				break;
			} else if (wparam == VK_CONTROL && CtrlMode) {
				DoVKcode(wparam, falseblnr);
				CtrlMode = falseblnr;
				break;
			} else if (wparam == VK_CONTROL) {
				DoVKcode(wparam, trueblnr);
				CtrlMode = trueblnr;
				break;
			}
#endif
			if (! TestBit(lparam, 30)) { /* ignore repeats */
				DoVKcode(wparam, trueblnr);
			}

#if UseWinCE
			return TRUE; /* So that hardware keys won't be processed by the default handler */
#endif

			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
#if UseWinCE
			/*** DEBUG ***
			if (wparam != VK_CONTROL) {
				char Msg[256];
				sprintf(Msg, "Key released: %x", wparam);
				MacMsg("debug", Msg, falseblnr);
			}
			***/
			if (wparam == 0xAE) { /* to hide SoftInput panel */
				SipShowIM(SIPF_OFF);
				SipOn = falseblnr;
				break;
			} else if ((! SipOn) && (wparam == VK_RETURN)) { /* DPad Action to show SIP */
				/* Show SoftInput Panel */
				SipShowIM(SIPF_ON);
				SipOn = trueblnr;
				break;
			} else if (
#if EnableFullScreen
			(! WantFullScreen) &&
#endif
			(! SipOn) && (wparam >= VK_LEFT) && (wparam <= VK_DOWN))
			{
				switch (wparam) {
					case VK_LEFT:
						if (curHOffset > 0)
						{
							--curHOffset;
							hOffset = hOffsetsList[curHOffset];
						}
						break;
					case VK_UP:
						if (curVOffset > 0)
						{
							--curVOffset;
							vOffset = vOffsetsList[curVOffset];
						}
						break;
					case VK_RIGHT:
						if (curHOffset < numHOffsets - 1)
						{
							++curHOffset;
							hOffset = hOffsetsList[curHOffset];
						}
						break;
					case VK_DOWN:
						if (curVOffset < numVOffsets - 1)
						{
							++curVOffset;
							vOffset = vOffsetsList[curVOffset];
						}
						break;
				}
				Screen_DrawAll();
			} else if (wparam == VK_CONTROL && CtrlMode) {
				break;
			}
#endif
			DoVKcode(wparam, falseblnr);

#if UseWinCE
			return TRUE; /* So that hardware keys won't be processed by the default handler */
#endif

			break;

		case WM_CLOSE:
			RequestMacOff = trueblnr;
			break;
		case WM_ACTIVATE:
			if (MainWnd == hwnd) {
				gTrueBackgroundFlag = (LOWORD(wparam) == WA_INACTIVE);
			}
			break;
		case WM_COMMAND:
			switch(LOWORD(wparam))
			{
				case ID_FILE_INSERTDISK1:
					RequestInsertDisk = trueblnr;
					break;
				case ID_FILE_QUIT:
					RequestMacOff = trueblnr;
					break;
				case ID_SPECIAL_MORECOMMANDS:
					DoMoreCommandsMsg();
					break;
				case ID_HELP_ABOUT:
					DoAboutMsg();
					break;
			}
			break;
		case WM_MOVE:
			WndX = (si4b) LOWORD(lparam);
			WndY = (si4b) HIWORD(lparam);
			break;
		case WM_SYSCHAR:
		case WM_CHAR:
			/* prevent any further processing */
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			SetCurMouseButton(trueblnr);
#if UseWinCE
#if EnableFullScreen
			CurMouseV = WantFullScreen? ((HIWORD (lparam)) / 1.6) : HIWORD (lparam) + vOffset;
			CurMouseH = WantFullScreen? ((LOWORD (lparam)) / 1.6) : LOWORD (lparam) + hOffset;
#else
			CurMouseV = HIWORD (lparam) + vOffset;
			CurMouseH = LOWORD (lparam) + hOffset;
#endif
#endif
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			SetCurMouseButton(falseblnr);
#if UseWinCE
#if EnableFullScreen
			CurMouseV = WantFullScreen? ((HIWORD (lparam)) / 1.6) : HIWORD (lparam) + vOffset;
			CurMouseH = WantFullScreen? ((LOWORD (lparam)) / 1.6) : LOWORD (lparam) + hOffset;
#else
			CurMouseV = HIWORD (lparam) + vOffset;
			CurMouseH = LOWORD (lparam) + hOffset;
#endif
#endif
			break;
#if EnableDragDrop
		case WM_CREATE:
			DragAcceptFiles(hwnd, TRUE);
			break;
		case WM_DROPFILES:
			DragFunc((HDROP) wparam);
			break;
		case WM_DESTROY:
			DragAcceptFiles(hwnd, FALSE);
			break;
#endif
		default:
			return DefWindowProc(hwnd, uMessage, wparam, lparam);
	}
	return 0;
}

LOCALFUNC blnr RegisterOurClass(void)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW
#if ! UseWinCE
		| CS_OWNDC
#endif
		;
	wc.lpfnWndProc   = (WNDPROC)Win32WMProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = AppInstance;
	wc.hIcon         = LoadIcon(AppInstance, MAKEINTRESOURCE(IDI_VMAC));
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = WndClassName;

	if (! RegisterClass(&wc)) {
		MacMsg("RegisterClass failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALPROC CheckForSystemEvents(void)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
#if UseTimerThread
		if (WM_USER == msg.message) {
			/* ignore it, not ready for next tick yet */
		} else
#endif
		{
			DispatchMessage(&msg);
		}
	}
}

LOCALPROC WaitForTheNextEvent(void)
{
	MSG msg;

	if (GetMessage(&msg, NULL, 0, 0) != -1) {
		DispatchMessage(&msg);
	}
}

LOCALPROC RunOnEndOfSixtieth(void)
{
#if UseTimerThread
	GotTheTick = falseblnr;

	while (! GotTheTick) {
		WaitForTheNextEvent();
	}
#else
	while (ExtraTimeNotOver()) {
		Sleep(NextIntTime - LastTime);
	}

	OnTrueTime = TrueEmulatedTime;
	RunEmulatedTicksToTrueTime();
#endif
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

#if UseWinCE
LOCALFUNC blnr Init_ChangeOrientation(void)
{
	DEVMODE dm;

	/* initialize the DEVMODE structure */
	ZeroMemory(&dm, sizeof (dm));
	dm.dmSize = sizeof (dm);

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

	/* Backup old values */
	oldOrientation = dm.dmOrientation;
	oldDisplayOrientation = dm.dmDisplayOrientation;


	/* Hide SIP (you can never tell...) */
	SipShowIM(SIPF_OFF);
	SipOn = falseblnr;

	/* Switch to Landscape mode if possible */
	dm.dmOrientation = DMORIENT_LANDSCAPE;
	dm.dmDisplayOrientation = DMDO_90;
	dm.dmFields = DM_ORIENTATION | DM_DISPLAYORIENTATION;
	/*if (*/ChangeDisplaySettingsEx(NULL, &dm, NULL, 0, 0)/* != DISP_CHANGE_SUCCESSFUL) {
		MacMsg ("warning", "Couldn't switch to Landscape mode.", falseblnr);
	}*/;

	/* Save screen caps */
	hRes = GetDeviceCaps(GetDC(NULL), HORZRES);
	vRes = GetDeviceCaps(GetDC(NULL), VERTRES);

	/* Pre-calculate screen offsets for this device */
	hOffsetsList = NULL;
	vOffsetsList = NULL;

	curHOffset = 0;
	curVOffset = 0;

	switch (hRes)
	{
	case 320: /* 320x240 */
		numHOffsets = 2;
		hOffsetsList = (short*)malloc(numHOffsets * sizeof(short));
		hOffsetsList[0] = -3;
		hOffsetsList[1] = 189;
		break;

	case 240: /* 240x320 and 240x240 */
		numHOffsets = 3;
		hOffsetsList = (short*)malloc(numHOffsets * sizeof(short));
		hOffsetsList[0] = -3;
		hOffsetsList[1] = 135;
		hOffsetsList[2] = 269;
		break;

	default: /* VGA devices */
		numHOffsets = 1;
		hOffsetsList = (short*)malloc(numHOffsets * sizeof(short));
		hOffsetsList[0] = 0;
		break;
	}

	switch (vRes)
	{
	case 320: /* 240x320 */
		numVOffsets = 2;
		vOffsetsList = (short*)malloc(numVOffsets * sizeof(short));
		vOffsetsList[0] = 0;
		vOffsetsList[1] = 22;
		break;

	case 240: /* 320x240 and 240x240 */
		numVOffsets = 2;
		vOffsetsList = (short*)malloc(numVOffsets * sizeof(short));
		vOffsetsList[0] = 0;
		vOffsetsList[1] = 102;
		break;

	default: /* VGA devices */
		numVOffsets = 1;
		vOffsetsList = (short*)malloc(numVOffsets * sizeof(short));
		vOffsetsList[0] = 0;
		break;
	}

	hOffset = hOffsetsList[0];
	vOffset = vOffsetsList[0];

#ifdef DEBUG
	/* DEBUG: print out device screen info */
	{
		TCHAR szDbg[256];
		wsprintf(szDbg, _T("Caps: %dx%d.\nNumber of horizontal offsets: %d.\nOffsets: %d, %d, %d."),
			hRes, vRes, numHOffsets, hOffsetsList[0],
			(numHOffsets > 1)? hOffsetsList[1] : -1,
			(numHOffsets > 2)? hOffsetsList[2] : -1);
		MessageBox(GetActiveWindow(), szDbg, _T("DEBUG"), MB_ICONINFORMATION);
	}
#endif

	return trueblnr;
}
#endif

#if UseWinCE
LOCALPROC Uninit_ChangeOrientation(void)
{
	DEVMODE dm;

	/* Restore old display orientation */
	ZeroMemory(&dm, sizeof (dm));
	dm.dmSize = sizeof(dm);

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

	dm.dmOrientation = oldOrientation;
	dm.dmDisplayOrientation = oldDisplayOrientation;
	dm.dmFields = DM_ORIENTATION | DM_DISPLAYORIENTATION;

	ChangeDisplaySettingsEx(NULL, &dm, 0, 0, 0);

	/* Deallocate pre-calculated offsets */
	if (hOffsetsList) {
		free(hOffsetsList);
	}
	if (vOffsetsList) {
		free(vOffsetsList);
	}
}
#endif


/*** code for handling hardware keys in Pocket PC devices ***/

#if UseWinCE
typedef BOOL (__stdcall *UnregisterFunc1Proc)(UINT, UINT);
LOCALVAR HINSTANCE hCoreDLL = NULL;
#endif

#if UseWinCE
LOCALFUNC blnr InitHotKeys(void)
{
	UnregisterFunc1Proc procUndergisterFunc;
	int i;

	hCoreDLL = LoadLibrary(TEXT("coredll.dll"));
	if (! hCoreDLL) {
		MacMsg ("Fatal", "Could not load coredll.dll", trueblnr);
	} else {
		procUndergisterFunc =
			(UnregisterFunc1Proc) GetProcAddress(hCoreDLL,
			TEXT("UnregisterFunc1"));
		if (! procUndergisterFunc) {
			MacMsg ("Fatal", "Could not get UnregisterFunc1 procedure", trueblnr);
		} else {
			for (i = 0xc1; i <= 0xcf; ++i) {
				procUndergisterFunc(MOD_WIN, i);
				RegisterHotKey(MainWnd, i, MOD_WIN, i);
			}
		}
	}
	return trueblnr;
}
#endif

#if UseWinCE
LOCALPROC UninitHotKeys(void)
{
	if (! hCoreDLL) {
		FreeLibrary(hCoreDLL);
	}
}
#endif

/**************************/


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
	if (RegisterOurClass())
	if (LoadInitialImages())
	if (ScanCommandLine())
#if InstallFileIcons
	if (RegisterInRegistry())
#endif
	if (AllocateMacROM())
	if (LoadMacRom())
#if MySoundEnabled
	if (MySound_Init())
#endif
	if (AllocateScreenCompare())
	if (ReCreateMainWindow())
	if (InitWinKey2Mac())
	if (AllocateMacRAM())
	if (InitTheCursor())
#if UseWinCE
	if (Init_ChangeOrientation())
	if (InitHotKeys())
#endif
	if (Init60thCheck())
#if UseTimerThread
	if (MyTimer_Init())
#endif
	if (InitEmulation())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
#if UseTimerThread
	MyTimer_UnInit();
#endif
	SetCurMouseButton(falseblnr);

	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}

	if (! gTrueBackgroundFlag) {
		CheckSavedMacMsg();
	}

#if EnableFullScreen
	UnGrabTheMachine();
#endif
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
#if MySoundEnabled
	MySound_UnInit();
#endif
#if IncludePbufs
	UnInitPbufs();
#endif
	UnInitDrives();

	ForceShowCursor();
#if UseWinCE
	Uninit_ChangeOrientation();
	UninitHotKeys();
#endif

#if EnableShellLinks
	MyUninitCOM();
#endif

	if (RAM != NULL) {
		if (GlobalFree(RAM) != NULL) {
			MacMsg("error", "GlobalFree failed", falseblnr);
		}
	}

	DisposeMainWindow();
	if (screencomparebuff != NULL) {
		if (GlobalFree(screencomparebuff) != NULL) {
			MacMsg("error", "GlobalFree failed", falseblnr);
		}
	}
#if UseControlKeys
	if (CntrlDisplayBuff != NULL) {
		if (GlobalFree(CntrlDisplayBuff) != NULL) {
			MacMsg("error", "GlobalFree failed", falseblnr);
		}
	}
#endif
	if (ROM != NULL) {
		if (GlobalFree(ROM) != NULL) {
			MacMsg("error", "GlobalFree failed", falseblnr);
		}
	}
#if EnableScalingBuff
	if (ScalingBuff != NULL) {
		(void) GlobalFree(ScalingBuff);
	}
#endif
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	UnusedParam(hPrevInstance);
	AppInstance = hInstance;
	CmdShow = nCmdShow;
	CommandLine = lpCmdLine;

	GetWndTitle();
#if UseWinCE
	if (AlreadyRunningCheck()) {
		return 0;
	}
#endif

	ZapOSGLUVars();
	if (InitOSGLU()) {
		MainEventLoop();
	}
	UnInitOSGLU();

	return(0);
}
