/*
	WNOSGLUE.h

	Copyright (C) 2004 Philip Cummins, Weston Pawlowski,
	Bradford L. Barrett, Paul Pratt

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
	ID_SPECIAL_LIMITSPEED,
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

#if EnableFullScreen
LOCALVAR blnr UseFullScreen = falseblnr;
#endif

#if EnableMagnify
LOCALVAR blnr UseMagnify = falseblnr;
#endif

#define MyWindowScale 2

#if EnableFullScreen
LOCALVAR short hOffset;
LOCALVAR short vOffset;
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

/* keyboard */

LOCALVAR HINSTANCE AppInstance;

static BYTE a[256];

LOCALVAR blnr ShouldCheckKeyBoard = trueblnr;

LOCALPROC DoVKcode(int i, blnr down)
{
	a[i] = down ? 0x80 : 0;
	ShouldCheckKeyBoard = trueblnr;
}

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

LOCALPROC InitVKcodes(void)
{
	si4b i;

	for (i = 0; i < 256; ++i) {
		a[i] = 0;
	}
	ShouldCheckKeyBoard = trueblnr;
	SetCurMouseButton(falseblnr);
}

#ifndef EnableGrabSpecialKeys
#define EnableGrabSpecialKeys EnableFullScreen
#endif /* EnableGrabSpecialKeys */

#if EnableGrabSpecialKeys
LOCALPROC RecheckKey(int i)
{
	if (a[i] != 0) {
		if ((GetAsyncKeyState(i) & 0x8000) == 0) {
			a[i] = 0;
			ShouldCheckKeyBoard = trueblnr;
		}
	}
}
#endif

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
		if (p->vkCode < 256) {
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
LOCALVAR blnr HaveSetSysParam = falseblnr;
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

#if 0
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

/*--- priority ---*/

#if EnableFullScreen
LOCALVAR blnr MyPriorityRaised = falseblnr;
#endif

#if EnableFullScreen
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

#if EnableFullScreen
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

	for (i = n; --i >=0; ) {
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
				pwh->dwBytesRecorded=0;
				pwh->dwUser=0;
				pwh->dwFlags=0;
				pwh->dwLoops=0;
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
			--TimeAdjust;
		} else if (MinFilledSoundBuffs < DesiredMinFilledSoundBuffs) {
			++TimeAdjust;
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
	RaiseMyPriority();
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
	LowerMyPriority();
#if MySoundEnabled && MySoundFullScreenOnly
	MySound_Stop();
#endif
}
#endif

/*--- basic dialogs ---*/

LOCALPROC MyBeginDialog(void)
{
#if EnableFullScreen
	UnGrabTheMachine();
#endif
	ForceShowCursor();
}

LOCALPROC MyEndDialog(void)
{
	InitVKcodes();
#if EnableFullScreen
	if (UseFullScreen && (MainWnd != NULL)) {
		GrabTheMachine();
	}
#endif
}

GLOBALPROC MacMsg(char *briefMsg, char *longMsg, blnr fatal)
{
	UnusedParam(fatal);

	MyBeginDialog();
	MessageBox(MainWnd, longMsg, briefMsg, MB_APPLMODAL|MB_OK);
	MyEndDialog();
}

/*--- main window ---*/

#define EnableScalingBuff (1 && EnableMagnify && (MyWindowScale == 2))

#if EnableScalingBuff
LOCALVAR char *ScalingBuff = NULL;
#endif

LOCALVAR HDC MainWndDC;

LOCALVAR si5b CmdShow;

LOCALVAR char *WndTitle = kStrAppName;
LOCALVAR char *WndClassName = "minivmac";

LOCALVAR blnr gBackgroundFlag = falseblnr;

LOCALPROC DisposeMainWindow(void)
{
	if (MainWndDC != NULL) {
		ReleaseDC(MainWnd, MainWndDC);
	}
	if (MainWnd != NULL) {
		DestroyWindow(MainWnd);
		MainWnd = NULL; /* so MacMsg will still work */
	}
}

LOCALFUNC blnr ReCreateMainWindow(void)
{
	HMENU m;
	HMENU mb;
	int XSize;
	int YSize;
	HWND NewMainWnd;
	HDC NewMainWndDC;
	int XBorder = GetSystemMetrics(SM_CXFIXEDFRAME);
	int YBorder = GetSystemMetrics(SM_CYFIXEDFRAME);
	int YCaption = GetSystemMetrics(SM_CYCAPTION);
	int YMenu = GetSystemMetrics(SM_CYMENU);
	int ScreenX = GetSystemMetrics(SM_CXSCREEN);
	int ScreenY = GetSystemMetrics(SM_CYSCREEN);
	short NewWindowHeight = vMacScreenHeight;
	short NewWindowWidth = vMacScreenWidth;

#if EnableMagnify
	if (WantMagnify) {
		NewWindowHeight *= MyWindowScale;
		NewWindowWidth *= MyWindowScale;
	}
#endif
#if EnableFullScreen
	if (WantFullScreen) {
		XSize = ScreenX;
		YSize = ScreenY;

		hOffset = (ScreenX - NewWindowWidth) / 2;
		vOffset = (ScreenY - NewWindowHeight) / 2;
		if (hOffset < 0) {
			WndX = 0;
		}
		if (vOffset < 0) {
			WndX = 0;
		}

		WndX = 0;
		WndY = 0;
	} else
#endif
	{
		XSize = XBorder + NewWindowWidth + XBorder;
		YSize = YBorder + YCaption + YMenu + NewWindowHeight + YBorder;

		WndX = (ScreenX - XSize) / 2;
		WndY = (ScreenY - YSize) / 2;

		if (WndX < 0) {
			WndX = 0;
		}
		if (WndY < 0) {
			WndY = 0;
		}
	}

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
					ID_FILE_INSERTDISK1, "Open Disk Image...");
				(void) AppendMenu(m, MF_SEPARATOR, 0, NULL);
				(void) AppendMenu(m, MF_ENABLED + MF_STRING,
					ID_FILE_QUIT, "Quit");
				(void) InsertMenu(mb, 0xFFFFFFFF,
					MF_BYPOSITION + MF_POPUP + MF_STRING + MF_ENABLED,
					(UINT)m, "File");
			}
			m = CreateMenu();
			if (m != NULL) {
				(void) AppendMenu(m, MF_ENABLED + MF_STRING,
					ID_SPECIAL_LIMITSPEED, "Limit Speed");
				(void) AppendMenu(m, MF_ENABLED + MF_STRING,
					ID_SPECIAL_MORECOMMANDS, "More Commands...");
				(void) InsertMenu(mb, 0xFFFFFFFF,
					MF_BYPOSITION + MF_POPUP + MF_STRING + MF_ENABLED,
					(UINT)m, "Special");
			}
			m = CreateMenu();
			if (m != NULL) {
				(void) AppendMenu(m, MF_ENABLED + MF_STRING,
					ID_HELP_ABOUT, "About...");
				(void) InsertMenu(mb, 0xFFFFFFFF,
					MF_BYPOSITION + MF_POPUP + MF_STRING + MF_ENABLED,
					(UINT)m, "Help");
			}
		}
	}

	{
		NewMainWnd = CreateWindowEx(
#if EnableFullScreen
			WantFullScreen ? WS_EX_TOPMOST :
#endif
			WS_EX_ACCEPTFILES,
			WndClassName,
			WndTitle,
#if EnableFullScreen
			WantFullScreen ? WS_VISIBLE|WS_POPUP :
#endif
			WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
			WndX, WndY, XSize, YSize, NULL,
			mb, AppInstance, NULL);
		if (NewMainWnd == NULL) {
			MacMsg("CreateWindow failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
		} else {
			ShowWindow(NewMainWnd, CmdShow);
			NewMainWndDC = GetDC(NewMainWnd);
			if (NewMainWndDC == NULL) {
				MacMsg("GetDC failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
			} else {
#if EnableGrabSpecialKeys
				UnGrabSpecialKeys();
#endif
				DisposeMainWindow();

#if EnableScalingBuff
				if (WantMagnify) {
					if (ScalingBuff == NULL) {
						ScalingBuff = (char *)GlobalAlloc(GMEM_FIXED, vMacScreenNumBytes * MyWindowScale * MyWindowScale);
					}
				} else {
					if (ScalingBuff != NULL) {
						(void) GlobalFree(ScalingBuff);
						ScalingBuff = NULL;
					}
				}
#endif

				MainWnd = NewMainWnd;
				MainWndDC = NewMainWndDC;
				gBackgroundFlag = falseblnr;
#if EnableFullScreen
				UseFullScreen = WantFullScreen;
#endif
#if EnableMagnify
				UseMagnify = WantMagnify;
#endif

#if EnableFullScreen
				if (UseFullScreen) {
					GrabTheMachine();
				} else {
					UnGrabTheMachine();
				}
#endif

				InitVKcodes(); /* since key events per window */
				return trueblnr;
				/* ReleaseDC(MainWnd, MainWndDC); */
			}
			DestroyWindow(NewMainWnd);
		}
	}
	return falseblnr;
}

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
#if EnableFullScreen
	if (UseFullScreen) {
		XDest += hOffset;
		YDest += vOffset;
	}
#endif

	UnusedParam(left);
	UnusedParam(right);
#if 0
	{ /* testing code */
		if (PatBlt(MainWndDC,
			(int)left - 1,
			(int)top - 1,
			(int)right-left + 2,
			(int)bottom-top + 2, PATCOPY)) {
		}
	}
#endif
	memset (&bmh, sizeof (bmh), 0);
	bmh.bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmh.bmi.biWidth = vMacScreenWidth;
	bmh.bmi.biHeight = -(bottom - top);
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

			for (i = bottom - top; --i >=0; ) {
				p3 = p2;
				for (j = vMacScreenWidth / 8; --j >=0; ) {
					t0 = *p1++;
					t1 = t0;
					m = 0x80;
					t2 = 0;
					for (k = 4; --k >=0; ) {
						t2 |= t1 & m;
						t1 >>= 1;
						m >>= 2;
					}
					*p2++ = t2 | (t2 >> 1);

					t1 = t0 << 4;
					m = 0x80;
					t2 = 0;
					for (k = 4; --k >=0; ) {
						t2 |= t1 & m;
						t1 >>= 1;
						m >>= 2;
					}
					*p2++ = t2 | (t2 >> 1);
				}
				for (j = MyScaledWidth / 8; --j >=0; ) {
					*p2++ = *p3++;
				}
			}

			bmh.bmi.biWidth = vMacScreenWidth * MyWindowScale;
			bmh.bmi.biHeight = -((bottom - top) * MyWindowScale);
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
	{
		if (SetDIBitsToDevice(
			MainWndDC, /* handle of device context */
			XDest, /* x-coordinate of upper-left corner of dest. rect. */
			YDest, /* y-coordinate of upper-left corner of dest. rect. */
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

#if EnableFullScreen
	if (UseFullScreen) {
		NewMousePos.x -= hOffset;
		NewMousePos.y -= vOffset;
	}
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

		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/* for a game like arkanoid, would like mouse to still
		move even when outside window in one direction */
		{
			CurMouseV = NewMousePos.y;
			CurMouseH = NewMousePos.x;
		}
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

LOCALFUNC blnr Sony_Insert1(char *drivepath)
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

GLOBALPROC InsertADisk(void)
{
	OPENFILENAME ofn;
	char szDirName[256];
	char szFile[256], szFileTitle[256];
	UINT  i, cbString;
	char  chReplace;
	char  szFilter[256];
	blnr IsOk;

	szDirName[0] = '\0';
	szFile[0] = '\0';
	strcpy(szFilter,"Disk images|*.DSK;*.HF?;*.IMG;*.IMA;*.IMAGE|All files (*.*)|*.*|\0");

	cbString = strlen(szFilter);

	chReplace = szFilter[cbString - 1];

	for (i = 0; szFilter[i] != '\0'; ++i)
	{
		if (szFilter[i] == chReplace)
			szFilter[i] = '\0';
	}

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = MainWnd;
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile= szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = sizeof(szFileTitle);
	ofn.lpstrInitialDir = szDirName;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	MyBeginDialog();
	IsOk = GetOpenFileName(&ofn);
	MyEndDialog();

	if(! IsOk) {
		/* report error */
	} else {
		(void) Sony_Insert1(ofn.lpstrFile);
	}
}

LOCALFUNC blnr GetAppDir (char* pathName)
/* be sure at least _MAX_PATH long! */
{
	if (GetModuleFileName (AppInstance, pathName, _MAX_PATH) == 0) {
		MacMsg("error", "GetModuleFileName failed", falseblnr);
	} else {
		char *p0 = pathName;
		char *p = nullpr;
		char c;

		while ((c = *p0++) != 0) {
			if (c == '\\') {
				p = p0;
			}
		}
		if (p == nullpr) {
			MacMsg("error", "strrchr failed", falseblnr);
		} else {
			*p = '\0';
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALFUNC blnr LoadInitialImageFromName(char* ImageName)
{
	char ImageFile[_MAX_PATH];

	if (GetAppDir(ImageFile)) {
		strcat(ImageFile, ImageName);
		if (Sony_Insert1(ImageFile)) {
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALFUNC blnr LoadInitialImages(void)
{
	if (LoadInitialImageFromName("disk1.dsk"))
	if (LoadInitialImageFromName("disk2.dsk"))
	if (LoadInitialImageFromName("disk3.dsK"))
	{
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
	char ROMFile[_MAX_PATH];
	HANDLE refnum;
	blnr IsOk = falseblnr;

	if (! GetAppDir(ROMFile)) {
		MacMsg("Mini vMac error", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
	} else {
		strcat(ROMFile, RomFileName);

		refnum = CreateFile(
			ROMFile, /* pointer to name of the file */
			GENERIC_READ, /* access (read-write) mode */
			FILE_SHARE_READ, /* share mode */
			nullpr, /* pointer to security descriptor */
			OPEN_EXISTING, /* how to create */
			FILE_ATTRIBUTE_NORMAL, /* file attributes */
			nullpr /* handle to file with attributes to copy */
		);
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

LOCALVAR char *CommandLine;

LOCALFUNC blnr ScanCommandLine(void)
{
	char fileName[_MAX_PATH];
	char *filePtr;
	char *p = CommandLine;

	while (*p != 0) {
		if (*p == ' ') {
			p++;
		} else if (*p == '/') {
			p++;
			if (*p == 'l') {
				p++;
				SpeedLimit = trueblnr;
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
			(void) Sony_Insert1(fileName);
		}
	}

	return trueblnr;
}

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

LOCALPROC UpdateKeyMapByte(int i, ui3b *b)
{
	ui3b *kp = ((ui3b *)theKeys) + i;
	ui3b v = 0;
	int k;

	for (k = 0; k < 8; k++) {
		if (b[k] & 0x80) {
			v |= (1 << k);
		}
	}
	*kp = v;
}

LOCALPROC GetWindowsKeysAndTranslate(void)
{
	ui3b b[8];

	b[0] = a['A'];
	b[1] = a['S'];
	b[2] = a['D'];
	b[3] = a['F'];
	b[4] = a['H'];
	b[5] = a['G'];
	b[6] = a['Z'];
	b[7] = a['X'];
	UpdateKeyMapByte(0, b);

	b[0] = a['C'];
	b[1] = a['V'];
	b[2] = 0;
	b[3] = a['B'];
	b[4] = a['Q'];
	b[5] = a['W'];
	b[6] = a['E'];
	b[7] = a['R'];
	UpdateKeyMapByte(1, b);

	b[0] = a['Y'];
	b[1] = a['T'];
	b[2] = a['1'];
	b[3] = a['2'];
	b[4] = a['3'];
	b[5] = a['4'];
	b[6] = a['6'];
	b[7] = a['5'];
	UpdateKeyMapByte(2, b);

	b[0] = a[myVK_Equal];
	b[1] = a['9'];
	b[2] = a['7'];
	b[3] = a[myVK_Subtract];
	b[4] = a['8'];
	b[5] = a['0'];
	b[6] = a[myVK_RightBracket];
	b[7] = a['O'];
	UpdateKeyMapByte(3, b);

	b[0] = a['U'];
	b[1] = a[myVK_LeftBracket];
	b[2] = a['I'];
	b[3] = a['P'];
	b[4] = a[VK_RETURN];
	b[5] = a['L'];
	b[6] = a['J'];
	b[7] = a[myVK_SingleQuote];
	UpdateKeyMapByte(4, b);

	b[0] = a['K'];
	b[1] = a[myVK_SemiColon];
	b[2] = a[myVK_BackSlash];
	b[3] = a[myVK_Comma];
	b[4] = a[myVK_Slash];
	b[5] = a['N'];
	b[6] = a['M'];
	b[7] = a[myVK_Period];
	UpdateKeyMapByte(5, b);

	b[0] = a[VK_TAB];
	b[1] = a[VK_SPACE];
	b[2] = a[myVK_Grave];
	b[3] = a[VK_BACK];
	b[4] = 0;
	b[5] = VK_ESCAPE;
	b[6] = 0;
	b[7] = a[VK_MENU] | a[VK_LMENU] | a[VK_RMENU] | a[VK_F2];
	UpdateKeyMapByte(6, b);

	b[0] = a[VK_SHIFT] | a[VK_LSHIFT] | a[VK_RSHIFT];
	b[1] = (GetKeyState(VK_CAPITAL) & 1) ? 0x80 : 0x00 /* (a[VK_CAPITAL] & 1) ? 0x80 : 0x00 */;
	b[2] = a[VK_APPS] | a[VK_LWIN] | a[VK_RWIN] | a[VK_F1];
	b[3] = a[VK_CONTROL] | a[VK_LCONTROL] | a[VK_RCONTROL];
	b[4] = 0;
	b[5] = 0;
	b[6] = 0;
	b[7] = 0;
	UpdateKeyMapByte(7, b);

	b[0] = 0;
	b[1] = a[VK_DECIMAL] | a[VK_DELETE];
	b[2] = 0 /* a[VK_RIGHT] */;
	b[3] = a[VK_MULTIPLY];
	b[4] = 0;
	b[5] = a[VK_ADD];
	b[6] = 0 /* a[VK_LEFT] */;
	b[7] = a[VK_NUMLOCK];
	UpdateKeyMapByte(8, b);

	b[0] = 0 /* a[VK_DOWN] */;
	b[1] = 0;
	b[2] = 0;
	b[3] = a[VK_DIVIDE];
	b[4] = 0 /* a[VK_RETURN], but right */;
	b[5] = 0 /* a[VK_UP] */;
	b[6] = a[VK_SUBTRACT];
	b[7] = 0;
	UpdateKeyMapByte(9, b);

	b[0] = 0;
	b[1] = a[VK_SEPARATOR];
	b[2] = a[VK_NUMPAD0];
	b[3] = a[VK_NUMPAD1];
	b[4] = a[VK_NUMPAD2];
	b[5] = a[VK_NUMPAD3];
	b[6] = a[VK_NUMPAD4];
	b[7] = a[VK_NUMPAD5];
	UpdateKeyMapByte(10, b);

	b[0] = a[VK_NUMPAD6];
	b[1] = a[VK_NUMPAD7];
	b[2] = 0;
	b[3] = a[VK_NUMPAD8];
	b[4] = a[VK_NUMPAD9];
	b[5] = 0;
	b[6] = 0;
	b[7] = 0;
	UpdateKeyMapByte(11, b);

	theKeys[3] = 0;

	b[0] = 0;
	b[1] = 0;
	b[2] = 0;
	b[3] = a[VK_LEFT];
	b[4] = a[VK_RIGHT];
	b[5] = a[VK_DOWN];
	b[6] = a[VK_UP];
	b[7] = 0;
	UpdateKeyMapByte(15, b);
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
	CheckMagnifyAndFullScreen();
}
#endif

#if EnableFullScreen
GLOBALPROC ToggleWantFullScreen(void)
{
	WantFullScreen = ! WantFullScreen;
#if EnableMagnify
	if (! WantFullScreen) {
		WantMagnify = falseblnr;
	} else {
		if ((GetSystemMetrics(SM_CXSCREEN) >= vMacScreenWidth * MyWindowScale)
			&& (GetSystemMetrics(SM_CYSCREEN) >= vMacScreenHeight * MyWindowScale)
			)
		{
			WantMagnify = trueblnr;
		}
	}
#endif
	CheckMagnifyAndFullScreen();
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
			(void) Sony_Insert1(a);
		}
	}

	DragFinish(hDrop);

	if (! SetForegroundWindow(MainWnd)) {
		/* error message here ? */
	}
}
#endif

LRESULT CALLBACK Win32WMProc(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam);

LRESULT CALLBACK Win32WMProc(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	switch (uMessage)
	{
		case WM_MOUSEMOVE:
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
			DoVKcode(wparam, trueblnr);
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			DoVKcode(wparam, falseblnr);
			break;

		case WM_CLOSE:
			RequestMacOff = trueblnr;
			break;
		case WM_ACTIVATE:
			if (MainWnd == hwnd) {
				gBackgroundFlag = (LOWORD(wparam) == WA_INACTIVE);
			}
			break;
		case WM_COMMAND:
			switch(LOWORD(wparam))
			{
				case ID_FILE_INSERTDISK1:
					InsertADisk();
					break;
				case ID_FILE_QUIT:
					RequestMacOff = trueblnr;
					break;
				case ID_SPECIAL_LIMITSPEED:
					SpeedLimit = ! SpeedLimit;
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
		case WM_INITMENU:
			{
				HMENU CurMenuHdl = GetMenu(hwnd);
				(void) CheckMenuItem(CurMenuHdl,
					ID_SPECIAL_LIMITSPEED,
					MF_BYCOMMAND + (SpeedLimit ? MF_CHECKED : MF_UNCHECKED));
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
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			SetCurMouseButton(falseblnr);
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

	wc.style         = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wc.lpfnWndProc   = (WNDPROC)Win32WMProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = AppInstance;
	wc.hIcon         = LoadIcon(AppInstance, MAKEINTRESOURCE(IDI_VMAC));
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (SS_WHITERECT);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = WndClassName;

	if (! RegisterClass((LPWNDCLASS)&wc.style)) {
		MacMsg("RegisterClass failed", "Sorry, Mini vMac encountered errors and cannot continue.", trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALPROC WaitInBackground(void)
{
	MSG msg;
	blnr GotMessage;

#if EnableFullScreen
	if (WantFullScreen) {
		ToggleWantFullScreen();
	}
#endif
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
	ForceShowCursor();
	do {
		GotMessage = (GetMessage(&msg, NULL, 0, 0) != -1);
		if (GotMessage) {
			DispatchMessage(&msg);
		}
	} while (gBackgroundFlag);
	InitVKcodes();
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Start();
#endif
}

LOCALPROC DoOnEachSixtieth(void)
{
	MSG msg;
	blnr GotMessage;

	if (! gBackgroundFlag) {
		GotMessage = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (GotMessage) {
			DispatchMessage(&msg);
		}
	}

	if (gBackgroundFlag) {
		WaitInBackground();
	}
		/*
			When in background, halt the emulator by
			continuously running the event loop
		*/
	if (RequestMacOff) {
		if (AnyDiskInserted()) {
			RequestMacOff = falseblnr;
			MacMsg("Please shut down the emulated machine before quitting.",
				"To force Mini vMac to quit, at the risk of corrupting the mounted disk image files, use the 'Q' command of the Mini vMac Control Mode. To learn about the Control Mode, see the 'More Commands...' item in the 'Special' menu.",
				falseblnr);
		}
	}

	CheckMouseState();

#if EnableGrabSpecialKeys
	if (HaveSetSysParam) {
		/* check for lost key ups */
		RecheckKey(VK_LWIN);
		RecheckKey(VK_RWIN);
		/* RecheckKey(VK_APPS); */
	}
#endif

	if (ShouldCheckKeyBoard) {
		ShouldCheckKeyBoard = falseblnr;
		GetWindowsKeysAndTranslate();
	}
}

LOCALVAR ui5b TimeSecBase;
LOCALVAR DWORD TimeMilliBase;

LOCALVAR DWORD LastTime;
LOCALVAR ui5b TimeCounter = 0;

#include "DATE2SEC.h"

LOCALPROC CheckDateTime(void)
{
	ui5b NewMacDateInSecond;

	NewMacDateInSecond = ((ui5b)(LastTime - TimeMilliBase)) / 1000 + TimeSecBase;
	if (CurMacDateInSeconds != NewMacDateInSecond) {
		CurMacDateInSeconds = NewMacDateInSecond;

#if MySoundEnabled
		MySound_SecondNotify();
#endif
	}
}

LOCALFUNC blnr Init60thCheck(void)
{
	SYSTEMTIME s;
	TIME_ZONE_INFORMATION r;
	DWORD v;

	GetLocalTime(&s);
	LastTime = GetTickCount();
	TimeSecBase = Date2MacSeconds(s.wSecond, s.wMinute, s.wHour,
		s.wDay, s.wMonth, s.wYear);
	TimeMilliBase = LastTime - s.wMilliseconds;

	v = GetTimeZoneInformation(&r);
	if ((v != 0xFFFFFFFF) && (v != TIME_ZONE_ID_UNKNOWN)) {
		si5b dlsBias = (v != TIME_ZONE_ID_DAYLIGHT)
			? r.StandardBias : r.DaylightBias;
		CurMacDelta = (((ui5b)(-(r.Bias + dlsBias) * 60))
			& 0x00FFFFFF)
			| (((v != TIME_ZONE_ID_DAYLIGHT) ? 0 : 0x80)
				<< 24);
	}

	return trueblnr;
}

#define MyTimeDivPow 24
#define MyTimeDiv (1 << MyTimeDivPow)
#define MyTimeDivMask (MyTimeDiv - 1)
#define MyTimeMult 1009106 /* 60.14742 / 1000 * MyTimeDiv */

GLOBALPROC CheckIntSixtieth(blnr MayWaitForIt)
{
	DWORD LatestTime;
	ui5b TimeDiff;

	do {
#if MySoundEnabled
		SoundCheckVeryOften();
#endif
		LatestTime = GetTickCount();
		if (LatestTime != LastTime) {
			TimeCounter += MyTimeMult * (LatestTime - LastTime);
			LastTime = LatestTime;
			TimeDiff = TimeCounter >> MyTimeDivPow;
			if (TimeDiff != 0) {
				TimeCounter &= MyTimeDivMask;

				CheckDateTime();
				DoOnEachSixtieth();
				if (TimeDiff > 4) {
					/* emulation interrupted, forget it */
					TimeAdjust = 1;
				} else {
					TimeAdjust += TimeDiff;
				}
				return;
			}
		}
	} while (MayWaitForIt); /* loop forever if this true */
}

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
	if (AllocateMacRAM())
	if (InitTheCursor())
	if (Init60thCheck())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
	SetCurMouseButton(falseblnr);
#if EnableFullScreen
	UnGrabTheMachine();
#endif
#if MySoundEnabled
	MySound_UnInit();
#endif
	ForceShowCursor();

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UnusedParam(hPrevInstance);
	AppInstance = hInstance;
	CmdShow = nCmdShow;
	CommandLine = lpCmdLine;

	ZapOSGLUVars();
	if (InitOSGLU()) {
#if MySoundEnabled && (! MySoundFullScreenOnly)
		MySound_Start();
#endif
		ProgramMain();
#if MySoundEnabled && (! MySoundFullScreenOnly)
		MySound_Stop();
#endif
	}
	UnInitOSGLU();

	return(0);
}
