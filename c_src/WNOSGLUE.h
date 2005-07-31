/*
	WNOSGLUE.h

	Copyright (C) 2005 Philip Cummins, Weston Pawlowski,
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
	microsoft WiNdows Operating System GLUE.

	All operating system dependent code for the
	Microsoft Windows platform should go here.

	This code is descended from Weston Pawlowski's Windows
	port of vMac, by Philip Cummins.
	Adapted by Fabio Concas to run on Pocket PC 2003 devices.

	The main entry point 'WinMain' is at the end of this file.
*/

/* Resource Ids */

#define IDI_VMAC                        256
#define IDI_DISK                        257
#define IDI_ROM                         258

#define IDR_MAINMENU                    256

enum {
	ID_MENU_NULL = 256,
	ID_FILE_INSERTDISK1,
	ID_FILE_QUIT,
	ID_SPECIAL_MORECOMMANDS,
	ID_HELP_ABOUT,

	kNum_ID_MENU
};

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

/* main window info */

LOCALVAR HWND MainWnd;

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

#define MyWindowScale 2

#if EnableFullScreen || UseWinCE
LOCALVAR short hOffset;
LOCALVAR short vOffset;
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

LOCALVAR si3b WinKey2Mac[256];

LOCALPROC AssignOneMacKey(ui3b WinKey, si3b MacKey)
{
	WinKey2Mac[WinKey] = MacKey;
}

LOCALFUNC blnr InitWinKey2Mac(void)
{
	int i;

	for (i = 0; i < 256; i++) {
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
	AssignOneMacKey(VK_MENU, MKC_Command);
	AssignOneMacKey(VK_LMENU, MKC_Command);
	AssignOneMacKey(VK_RMENU, MKC_Command);
	AssignOneMacKey(VK_F2, MKC_Command);
	AssignOneMacKey(VK_SHIFT, MKC_Shift);
	AssignOneMacKey(VK_LSHIFT, MKC_Shift);
	AssignOneMacKey(VK_RSHIFT, MKC_Shift);

	AssignOneMacKey(VK_CAPITAL, MKC_CapsLock);

	AssignOneMacKey(VK_APPS, MKC_Option);
	AssignOneMacKey(VK_LWIN, MKC_Option);
	AssignOneMacKey(VK_RWIN, MKC_Option);
	AssignOneMacKey(VK_F1, MKC_Option);
	AssignOneMacKey(VK_CONTROL, MKC_Control);
	AssignOneMacKey(VK_LCONTROL, MKC_Control);
	AssignOneMacKey(VK_RCONTROL, MKC_Control);

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

	return trueblnr;
}

LOCALPROC Keyboard_UpdateKeyMap(int key, int down)
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

LOCALPROC DoKeyCode(int i, blnr down)
{
	if (i >= 0 && i < 256) {
		int key = WinKey2Mac[i];
		if (key >= 0) {
			Keyboard_UpdateKeyMap(key, down);
		}
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

LOCALPROC DoVKcode(int i, blnr down)
{
	if (i == VK_CAPITAL) {
		CheckTheCapsLock();
	} else {
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
}

LOCALPROC InitVKcodes(void)
{
	theKeys[0] = 0;
	theKeys[1] = 0;
	theKeys[2] = 0;
	theKeys[3] = 0;

	CheckTheCapsLock();

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
		if ((p->vkCode < 256) && (p->vkCode != VK_CAPITAL)) {
			switch (wParam) {
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
					DoVKcode(p->vkCode, trueblnr);
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

LOCALFUNC blnr UpdateCurrentTimeAdjust(void)
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
				if (UpdateCurrentTimeAdjust()) {
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

	hMyThread = CreateThread(NULL, 0, MyThreadProc,
		NULL, 0, &MyThreadID);
	if (hMyThread == NULL) {
		return falseblnr;
	} else {
		SetThreadPriority(hMyThread,
			THREAD_PRIORITY_HIGHEST);
	}

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

LOCALPROC MyUnInitTime(void)
{
	if (hMyThread != NULL) {
		QuitMyThread = trueblnr;
		while (QuitMyThread) {
			Sleep(1);
		}
	}
}

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
		mmr = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfex, 0, (DWORD) AppInstance,
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

/*--- basic dialogs ---*/

LOCALVAR blnr ADialogIsUp = falseblnr;

LOCALPROC MyBeginDialog(void)
{
	ADialogIsUp = trueblnr;
#if EnableFullScreen
	UnGrabTheMachine();
#endif
	ForceShowCursor();
}

LOCALPROC MyEndDialog(void)
{
	InitVKcodes();
	ADialogIsUp = falseblnr;
#if EnableFullScreen
	if (UseFullScreen && (MainWnd != NULL)) {
		GrabTheMachine();
	}
#endif
}

#if UseWinCE
#define SysCharT TCHAR
#else
#define SysCharT char
#endif

#if UseWinCE
#define SysCharP LPWSTR
#else
#define SysCharP LPSTR
#endif

#if UseWinCE
#define SysCharConst(s) (TEXT(s))
#else
#define SysCharConst(s) (s)
#endif

#if UseWinCE
#define SysChar_cat(r, y) (wcscat(r, y))
#else
#define SysChar_cat(r, y) (strcat(r, y))
#endif

#if UseWinCE
#define SysChar_cpy(r, y) (wcscpy(r, y))
#else
#define SysChar_cpy(r, y) (strcpy(r, y))
#endif

#if UseWinCE
#define SysChar_len(s) (wcslen(s))
#else
#define SysChar_len(s) (strlen(s))
#endif

#if UseWinCE
#define SysChar_fromchar(x) ((SysCharT)x)
#else
#define SysChar_fromchar(x) (x)
#endif

#if UseWinCE
#define SysChar_cmp(r, y) (wcscmp(r, y))
#else
#define SysChar_cmp(r, y) (strcmp(r, y))
#endif

LOCALFUNC SysCharP FindLastTerm(SysCharP s, SysCharT delim)
{
	SysCharT c;
	SysCharP p0 = s;
	SysCharP p = (SysCharP)nullpr;

	while ((c = *p0++) != SysChar_fromchar('\0')) {
		if (c == delim) {
			p = p0;
		}
	}

	return p;
}

/*
	ConvertCToT adapted from "Working With Unicode in C++"
	MSDN Library Archive, Copyright 2005, Microsoft Corporation.
*/

#if UseWinCE
LOCALPROC ConvertCToT(TCHAR* pszDest, const CHAR* pszSrc, int maxn)
{
	int i;
	int n = strlen(pszSrc);

	if (n >= maxn) {
		n = maxn - 1;
	}
	for (i = 0; i < n; i++) {
		pszDest[i] = (TCHAR) pszSrc[i];
	}
	pszDest[n] = 0;
}
#endif

GLOBALPROC MacMsg(char *briefMsg, char *longMsg, blnr fatal)
{
	UnusedParam(fatal);

	MyBeginDialog();
#if UseWinCE
	{
#define MyMaxStrSz 256
		TCHAR briefMsg0[MyMaxStrSz];
		TCHAR longMsg0[MyMaxStrSz];

		ConvertCToT(briefMsg0, briefMsg, MyMaxStrSz);
		ConvertCToT(longMsg0, longMsg, MyMaxStrSz);
		MessageBox(MainWnd, longMsg0, briefMsg0, MB_APPLMODAL | MB_OK | (fatal ? MB_ICONSTOP : 0));
	}
#else
	MessageBox(MainWnd, longMsg, briefMsg, MB_APPLMODAL | MB_OK);
#endif
	MyEndDialog();
}

/*--- main window ---*/

#define EnableScalingBuff (1 && EnableMagnify && (MyWindowScale == 2))

#if EnableScalingBuff
LOCALVAR char *ScalingBuff = NULL;
#endif

LOCALVAR HDC MainWndDC;

LOCALVAR si5b CmdShow;

LOCALVAR SysCharT WndTitle[_MAX_PATH];
LOCALVAR SysCharP WndClassName = SysCharConst("minivmac");

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

LOCALPROC GetWndTitle(void)
{
	SysCharT pathName[_MAX_PATH];
	WIN32_FIND_DATA fd;
	blnr IsOk = falseblnr;

	if (GetModuleFileName(AppInstance, pathName, _MAX_PATH) != 0) {
		HANDLE hf = FindFirstFile(pathName, &fd);

		if (hf != INVALID_HANDLE_VALUE) {
			/* get rid of extension, presumably '.exe' */
			SysCharP p = FindLastTerm(fd.cFileName,
				SysChar_fromchar('.'));
			if (p != nullpr) {
				*--p = SysChar_fromchar('\0');
			}

			SysChar_cpy(WndTitle, fd.cFileName);
			IsOk = trueblnr;
			FindClose(hf);
		}
	}
	if (! IsOk) {
		SysChar_cpy(WndTitle, SysCharConst(kStrAppName));
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
		XSize = 320; /* ScreenX; */
		YSize = 214; /* ScreenY; */
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
					(void) AppendMenu(m, MF_ENABLED + MF_STRING,
						ID_FILE_INSERTDISK1, SysCharConst("Open Disk Image..."));
					(void) AppendMenu(m, MF_SEPARATOR, 0, NULL);
					(void) AppendMenu(m, MF_ENABLED + MF_STRING,
						ID_FILE_QUIT, SysCharConst("Quit"));
					(void) InsertMenu(mb, 0xFFFFFFFF,
						MF_BYPOSITION + MF_POPUP + MF_STRING + MF_ENABLED,
						(UINT)m, SysCharConst("File"));
				}
				m = CreateMenu();
				if (m != NULL) {
					(void) AppendMenu(m, MF_ENABLED + MF_STRING,
						ID_SPECIAL_MORECOMMANDS, SysCharConst("More Commands..."));
					(void) InsertMenu(mb, 0xFFFFFFFF,
						MF_BYPOSITION + MF_POPUP + MF_STRING + MF_ENABLED,
						(UINT)m, SysCharConst("Special"));
				}
				m = CreateMenu();
				if (m != NULL) {
					(void) AppendMenu(m, MF_ENABLED + MF_STRING,
						ID_HELP_ABOUT, SysCharConst("About..."));
					(void) InsertMenu(mb, 0xFFFFFFFF,
						MF_BYPOSITION + MF_POPUP + MF_STRING + MF_ENABLED,
						(UINT)m, SysCharConst("Help"));
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

		InitVKcodes(); /* since key events per window */
	} else {
		(void) InvalidateRgn(MainWnd, NULL, FALSE);
	}

#if EnableFullScreen
	if (UseFullScreen) {
		GrabTheMachine();
	} else {
		UnGrabTheMachine();
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
		MacMsg("Not enough memory", "There is not enough memory available to allocate the screencomparebuff.", trueblnr);
		return falseblnr;
	}
#if UseControlKeys
	CntrlDisplayBuff = (char *)GlobalAlloc(GMEM_FIXED, vMacScreenNumBytes);
	if (CntrlDisplayBuff == NULL) {
		MacMsg("Not enough memory", "There is not enough memory available to allocate the CntrlDisplayBuff.", trueblnr);
		return falseblnr;
	}
#endif
#if EnableScalingBuff
	ScalingBuff = (char *)GlobalAlloc(GMEM_FIXED, vMacScreenNumBytes * MyWindowScale * MyWindowScale);
	if (ScalingBuff == NULL) {
		MacMsg("Not enough memory", "There is not enough memory available to allocate the ScalingBuff.", trueblnr);
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

GLOBALPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right)
{
	BITMAPINFOHEADER256 bmh;
	ui3b *p = ((ui3b *)screencomparebuff) + top * (vMacScreenWidth >> 3);
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
	memset(&bmh, sizeof (bmh), 0);
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
	bmh.colors[0].rgbRed = 255;
	bmh.colors[0].rgbGreen = 255;
	bmh.colors[0].rgbBlue = 255;
	bmh.colors[0].rgbReserved = 0;
	bmh.colors[1].rgbRed = 0;
	bmh.colors[1].rgbGreen = 0;
	bmh.colors[1].rgbBlue = 0;
	bmh.colors[1].rgbReserved = 0;
#if EnableMagnify
	if (UseMagnify) {
#if EnableScalingBuff
		if (ScalingBuff != NULL) {
			int i;
			int j;
			int k;
			ui3b *p1 = ((ui3b *)screencomparebuff) + top * (vMacScreenWidth >> 3);
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
				DIB_RGB_COLORS /* RGB or palette indices */
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
			DIB_RGB_COLORS, /* RGB or palette indices */
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
			320, /* dest. rectangle width */
			214, /* dest. rectangle height */
			0, /* x-coordinate of lower-left corner of source rect. */
			0, /* y-coordinate of lower-left corner of source rect. */
			vMacScreenWidth, /* source rectangle width */
			bottom, /* source rectangle height */
			(CONST VOID *)p, /* address of array with DIB bits */
			(const struct tagBITMAPINFO *)&bmh, /* address of structure with bitmap info. */
			DIB_RGB_COLORS, /* RGB or palette indices */
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
			DIB_RGB_COLORS /* RGB or palette indices */
		) == 0) {
			/* ReportWinLastError(); */
		}
	}
}

LOCALPROC Screen_DrawAll(void)
{
	HaveChangedScreenBuff(0, 0, vMacScreenHeight, vMacScreenWidth);
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

	if (HaveCursorHidden != ShouldHaveCursorHidden) {
		HaveCursorHidden = ShouldHaveCursorHidden;
		if (HaveCursorHidden) {
			(void) ShowCursor(FALSE);
		} else {
			(void) ShowCursor(TRUE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}
}

#define NotAfileRef NULL

LOCALVAR HANDLE Drives[NumDrives]; /* open disk image files */

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
		result = -1; /*& figure out what really to return &*/
	} else if (Sony_Start != (ui5b)newL) {
		/* not supposed to get here */
		result = -1; /*& figure out what really to return &*/
	} else {
		if (! ReadFile(refnum, /* handle of file to read */
			(LPVOID)Buffer, /* address of buffer that receives data */
			(DWORD)*Sony_Count, /* number of bytes to read */
			&BytesRead, /* address of number of bytes read */
			nullpr)) /* address of structure for data */
		{
			result = -1; /*& figure out what really to return &*/
		} else if ((ui5b)BytesRead != *Sony_Count) {
			result = -1; /*& figure out what really to return &*/
		} else {
			result = 0;
		}
	}
	*Sony_Count = BytesRead;
	return result;
}

GLOBALFUNC si4b vSonyWrite(void *Buffer, ui4b Drive_No, ui5b Sony_Start, ui5b *Sony_Count)
{
	si4b result;
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
		result = -1; /*& figure out what really to return &*/
	} else if (Sony_Start != (ui5b)newL) {
		/* not supposed to get here */
		result = -1; /*& figure out what really to return &*/
	} else {
		if (! WriteFile(refnum, /* handle of file to read */
			(LPVOID)Buffer, /* address of buffer that receives data */
			(DWORD)*Sony_Count, /* number of bytes to read */
			&BytesWritten, /* address of number of bytes read */
			nullpr)) /* address of structure for data */
		{
			result = -1; /*& figure out what really to return &*/
		} else if ((ui5b)BytesWritten != *Sony_Count) {
			result = -1; /*& figure out what really to return &*/
		} else {
			result = 0;
		}
	}
	*Sony_Count = BytesWritten;
	return result;
}

GLOBALFUNC si4b vSonyGetSize(ui4b Drive_No, ui5b *Sony_Count)
{
	si4b result;
	DWORD L;

	L = GetFileSize (Drives[Drive_No], nullpr);
	if (L == 0xFFFFFFFF) {
		result = -1; /*& figure out what really to return &*/
	} else {
		*Sony_Count = L;
		result = 0;
	}
	return result;
}

GLOBALFUNC si4b vSonyEject(ui4b Drive_No)
{
	HANDLE refnum = Drives[Drive_No];
	Drives[Drive_No] = NotAfileRef;

	(void) FlushFileBuffers(refnum);
	(void) CloseHandle(refnum);
	return 0x0000;
}

LOCALPROC Sony_Insert0(HANDLE refnum, blnr locked)
{
	ui4b Drive_No;

	if (! FirstFreeDisk(&Drive_No)) {
		(void) CloseHandle(refnum);
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage, falseblnr);
	} else {
		Drives[Drive_No] = refnum;
		vSonyInsertedMask |= ((ui5b)1 << Drive_No);
		if (! locked) {
			vSonyWritableMask |= ((ui5b)1 << Drive_No);
		}
	}
}

LOCALFUNC blnr Sony_Insert1(SysCharP drivepath)
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
		if (GetLastError() == ERROR_ACCESS_DENIED) {
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
	if (refnum != INVALID_HANDLE_VALUE) {
		Sony_Insert0(refnum, locked);
		return trueblnr;
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
LOCALFUNC blnr MyResolveShortcut(SysCharP FilePath)
/* adapted from Microsoft example code */
{
	HRESULT hres;
	IShellLink *psl;
	IPersistFile* ppf;
	WORD wsz[MAX_PATH];
	char szGotPath[MAX_PATH];
	WIN32_FIND_DATA wfd;
	blnr IsOk = falseblnr;

	if (MyNeedCOM()) {
		hres = CoCreateInstance(&CLSID_ShellLink, NULL,
			CLSCTX_INPROC_SERVER, &IID_IShellLink,
			(LPVOID *)&psl);
		if (SUCCEEDED(hres)) {
			/* Get a pointer to the IPersistFile interface. */
			hres = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile,
				(void **)&ppf);
			if (SUCCEEDED(hres)) {
				/* Ensure that the string is Unicode. */
				MultiByteToWideChar(CP_ACP, 0, FilePath, -1, wsz,
					MAX_PATH);

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
LOCALFUNC blnr InsertAnAlias(SysCharP FilePath)
{
	if (MyResolveShortcut(FilePath)) {
		return Sony_Insert1(FilePath);
	}
	return falseblnr;
}
#endif

#if EnableShellLinks
LOCALFUNC blnr FileIsLink(SysCharP drivepath)
{
	SysCharP p = FindLastTerm(drivepath, SysChar_fromchar('.'));

	if (p != nullpr) {
		if (SysChar_cmp(p, SysCharConst("lnk")) == 0) {
			return trueblnr;
		}
	}
	return falseblnr;
}
#endif

LOCALFUNC blnr InsertDiskOrAlias(SysCharP drivepath)
{
#if EnableShellLinks
	if (FileIsLink(drivepath)) {
		return InsertAnAlias(drivepath);
	} else
#endif
	{
		return Sony_Insert1(drivepath);
	}
}

#if EnableShellLinks
LOCALFUNC blnr MyFileExists(SysCharP pathName)
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
	SysCharT szDirName[256];
	SysCharT szFile[256];
#if ! UseWinCE
	SysCharT szFileTitle[256];
#endif
	UINT  i, cbString;
	SysCharT  chReplace;
	SysCharT  szFilter[256];
	blnr IsOk;

	szDirName[0] = SysChar_fromchar('\0');
	szFile[0] = SysChar_fromchar('\0');
	SysChar_cpy(szFilter, SysCharConst("Disk images|*.DSK;*.HF?;*.IMG;*.IMA;*.IMAGE|All files (*.*)|*.*|\0"));

	cbString = SysChar_len(szFilter);

	chReplace = szFilter[cbString - 1];

	for (i = 0; szFilter[i] != SysChar_fromchar('\0'); ++i)
	{
		if (szFilter[i] == chReplace) {
			szFilter[i] = SysChar_fromchar('\0');
		}
	}

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = MainWnd;
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 1;
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
		(void) Sony_Insert1(ofn.lpstrFile);
	}

#if UseWinCE
	CtrlMode = falseblnr;
	SipOn = falseblnr;
#endif
}

LOCALFUNC blnr GetAppDir (SysCharP pathName)
/* be sure at least _MAX_PATH long! */
{
	if (GetModuleFileName(AppInstance, pathName, _MAX_PATH) == 0) {
		MacMsg("error", "GetModuleFileName failed", falseblnr);
	} else {
		SysCharP p = FindLastTerm(pathName,
			SysChar_fromchar('\\'));
		if (p == nullpr) {
			MacMsg("error", "strrchr failed", falseblnr);
		} else {
			*p = SysChar_fromchar('\0');
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALFUNC blnr LoadInitialImageFromName(SysCharP ImageName)
{
	SysCharT ImageFile[_MAX_PATH];

	if (GetAppDir(ImageFile)) {
		SysChar_cat(ImageFile, ImageName);
		if (Sony_Insert1(ImageFile)) {
			return trueblnr;
		} else {
#if EnableShellLinks
			SysChar_cat(ImageFile, SysCharConst(".lnk"));
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
	SysCharT s[16];
	int i;
	int n = NumDrives > 9 ? 9 : NumDrives;

	SysChar_cpy(s, SysCharConst("disk?.dsk"));

	for (i = 1; i <= n; ++i) {
		s[4] = SysChar_fromchar('0' + i);
		if (! LoadInitialImageFromName(s)) {
			/* stop on first error (including file not found) */
			return trueblnr;
		}
	}

	return trueblnr;
}

LOCALFUNC blnr MyReadDat(HANDLE refnum, ui5b L, void *p)
{
	DWORD BytesRead;

	if (! ReadFile(refnum, /* handle of file to read */
		(LPVOID)p, /* address of buffer that receives data */
		(DWORD)L, /* number of bytes to read */
		&BytesRead, /* address of number of bytes read */
		nullpr)) /* address of structure for data */
	{
		/*& should report the error *\&*/
		return falseblnr;
	} else if ((ui5b)BytesRead != L) {
		/*& should report the error *\&*/
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALFUNC blnr AllocateMacROM(void)
{
	ROM = (ui4b *)GlobalAlloc(GMEM_FIXED, kROM_Size);
	if (ROM == NULL) {
		MacMsg("Not enough memory", "There is not enough memory available to allocate the ROM.", trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALFUNC blnr LoadMacRom(void)
{
	SysCharT ROMFile[_MAX_PATH];
	HANDLE refnum;
	blnr IsOk = falseblnr;

	if (! GetAppDir(ROMFile)) {
		MacMsg("Mini vMac error", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
	} else {
		SysChar_cat(ROMFile, SysCharConst(RomFileName));

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
			SysChar_cat(ROMFile, SysCharConst(".lnk"));
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
			MacMsg("ROM not found", "The ROM image file could not be found. Please read the documentation.", trueblnr);
		} else {
			if (! MyReadDat(refnum, kTrueROM_Size, ROM)) {
				MacMsg("Mini vMac error", "Couldn't read the ROM image file.", trueblnr);
			} else {
				IsOk = trueblnr;
			}
			(void) CloseHandle(refnum);
		}
	}
	return IsOk;
}

LOCALFUNC blnr AllocateMacRAM(void)
{
	RAM = (ui4b *)GlobalAlloc(GMEM_FIXED, kRAM_Size + RAMSafetyMarginFudge);
	if (RAM == NULL) {
		MacMsg("Not enough memory", "There is not enough memory available to allocate RAM for the emulated Mac.", trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALVAR SysCharP CommandLine;

LOCALFUNC blnr ScanCommandLine(void)
{
	SysCharT fileName[_MAX_PATH];
	SysCharT *filePtr;
	SysCharT *p = CommandLine;

	while (*p != 0) {
		if (*p == ' ') {
			p++;
		} else if (*p == '/') {
			p++;
			if (*p == 'l') {
				p++;
				SpeedValue = 0;
			}
		} else {
			filePtr = fileName;
			if (*p == '\"') {
				p++;
				while (*p != '\"' && *p != 0) {
					*filePtr++ = *p++;
				}
				if (*p == '\"') {
					p++;
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
	char a[_MAX_PATH];

	n = DragQueryFile(hDrop, (UINT) -1, NULL, 0);
	for (i = 0; i < n; i++) {
		if (DragQueryFile(hDrop, i, NULL, 0) < _MAX_PATH - 1) {
			(void) DragQueryFile(hDrop, i, a, _MAX_PATH);
			(void) InsertDiskOrAlias(a);
		}
	}

	DragFinish(hDrop);

	if (! SetForegroundWindow(MainWnd)) {
		/* error message here ? */
	}
}
#endif

GLOBALFUNC blnr ExtraTimeNotOver(void)
{
#if MySoundEnabled
	SoundCheckVeryOften();
#endif
	TimeAdjust = TrueEmulatedTime - CurEmulatedTime;
	return (TimeAdjust < 0) && MyTimerIsRunning;
}

LOCALPROC RunEmulatedTicksToTrueTime(void)
{
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

#if EnableGrabSpecialKeys
			CheckForLostKeyUps();
#endif
		}

		do {
#if MySoundEnabled
			SoundCheckVeryOften();
#endif
			DoEmulateOneTick();
			++CurEmulatedTime;

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

LOCALPROC LeaveBackground(void)
{
	InitVKcodes();
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
	MyTimer_Resume();
}

LOCALPROC EnterSpeedStopped(void)
{
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
	MyTimer_Suspend();
}

LOCALPROC CheckForSavedTasks(void)
{
	/*
		Check for things to do that rather wouldn't
		have done at an awkward time.
	*/

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
				"To force Mini vMac to quit, at the risk of corrupting the mounted disk image files, use the 'Q' command of the Mini vMac Control Mode. To learn about the Control Mode, see the 'More Commands...' item in the 'Special' menu.",
				falseblnr);
		} else {
			ForceMacOff = trueblnr;
		}
	}
}

LOCALVAR blnr GotTheTick = falseblnr;

LRESULT CALLBACK Win32WMProc(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam);

LRESULT CALLBACK Win32WMProc(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	switch (uMessage)
	{
		case WM_USER:
			RunEmulatedTicksToTrueTime();
			GotTheTick = trueblnr;
			break;
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
			DoVKcode(wparam, trueblnr);

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
			/***  ***/
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
						hOffset = 0;
						break;
					case VK_UP:
						vOffset = 0;
						break;
					case VK_RIGHT:
						hOffset = 192;
						break;
					case VK_DOWN:
						vOffset = 102;
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
					MacMsg("More commands are available in the Mini vMac Control Mode.",
						"To enter the Control Mode, press and hold down the 'control' key (after closing this message). You will remain in the Control Mode until you release the 'control' key. Type 'H' in the Control Mode to list available commands.",
						falseblnr);
					break;
				case ID_HELP_ABOUT:
					MacMsg("About",
						"To display information about this program, use the 'A' command of the Mini vMac Control Mode. To learn about the Control Mode, see the 'More Commands...' item in the 'Special' menu.",
						falseblnr);
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

	if (! RegisterClass((LPWNDCLASS)&wc.style)) {
		MacMsg("RegisterClass failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALPROC MainEventLoop(void)
{
	MSG msg;

	CheckForSavedTasks();
	while (! ForceMacOff) {
		if (GetMessage(&msg, NULL, 0, 0) != -1) {
			DispatchMessage(&msg);
		}

		if (GotTheTick) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				DispatchMessage(&msg);
			}
			DoEmulateExtraTime();
			GotTheTick = falseblnr;
		}

		CheckForSavedTasks();
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

	/* Switch to Landscape mode */
	dm.dmOrientation = DMORIENT_LANDSCAPE;
	dm.dmDisplayOrientation = DMDO_90;
	dm.dmFields = DM_ORIENTATION | DM_DISPLAYORIENTATION;
	if (ChangeDisplaySettingsEx(NULL, &dm, NULL, 0, 0) != DISP_CHANGE_SUCCESSFUL) {
		MacMsg ("warning", "Couldn't switch to Landscape mode.", falseblnr);
	}

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

	hCoreDLL = LoadLibrary(SysCharConst("coredll.dll"));
	if (! hCoreDLL) {
		MacMsg ("Fatal", "Could not load coredll.dll", trueblnr);
	} else {
		procUndergisterFunc =
			(UnregisterFunc1Proc) GetProcAddress(hCoreDLL,
			SysCharConst("UnregisterFunc1"));
		if (! procUndergisterFunc) {
			MacMsg ("Fatal", "Could not get UnregisterFunc1 procedure", trueblnr);
		} else {
			for (i = 0xc1; i <= 0xcf; i++) {
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
	ROM = NULL;
	RAM = NULL;
	screencomparebuff = NULL;
#if UseControlKeys
	CntrlDisplayBuff = NULL;
#endif
	MainWnd = NULL;
	MainWndDC = NULL;
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
	if (RegisterOurClass())
	if (LoadInitialImages())
	if (ScanCommandLine())
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
	if (InitEmulation())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
	MyUnInitTime();
	SetCurMouseButton(falseblnr);
#if EnableFullScreen
	UnGrabTheMachine();
#endif
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
#if MySoundEnabled
	MySound_UnInit();
#endif
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, SysCharP lpCmdLine, int nCmdShow)
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
