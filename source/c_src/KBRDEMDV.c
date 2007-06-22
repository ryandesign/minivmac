/*
	KBRDEMDV.c

	Copyright (C) 2006 Philip Cummins, Paul C. Pratt

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
	KeyBoaRD EMulated DeVice

	Emulation of the keyboard in the Mac Plus.

	This code adapted from "Keyboard.c" in vMac by Philip Cummins.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#include "MYOSGLUE.h"
#include "GLOBGLUE.h"
#include "ADDRSPAC.h"
#include "VIAEMDEV.h"
#endif

#include "KBRDEMDV.h"

#ifdef _VIA_Debug
#include <stdio.h>
#endif

LOCALVAR ui5b theKeyCopys[4];
	/*
		What the emulated keyboard thinks the mac thinks is the
		state of the keyboard. This is compared to theKeys.
	*/

LOCALFUNC blnr FindKeyEvent(int *VirtualKey, blnr *KeyDown)
{
	int j;
	int b;

	for (j = 0; j < 16; ++j) {
		ui3b k1 = ((ui3b *)theKeys)[j];
		ui3b k2 = ((ui3b *)theKeyCopys)[j];

		if (k1 != k2) {
			ui3b bit = 1;
			for (b = 0; b < 8; ++b) {
				ui3b newk = k1 & bit;
				ui3b oldk = k2 & bit;

				if (oldk != newk) {
					((ui3b *)theKeyCopys)[j] = (k2 & ~ bit) | newk;

					*VirtualKey = j * 8 + b;
					*KeyDown = (newk != 0);

					return trueblnr;
				}
				bit <<= 1;
			}
		}
	}
	return falseblnr;
}

#if CurEmu <= kEmuPlus
#define EmClassicKbrd 1
#define EmADB 0
#else
#define EmClassicKbrd 0
#define EmADB 1
#endif

#if EmClassicKbrd
enum {
	kKybdStateIdle,
	kKybdStateRecievingCommand,
	kKybdStateRecievedCommand,
	kKybdStateRecievingEndCommand,

	kKybdStates
};
#endif

#if EmClassicKbrd
LOCALVAR int KybdState = kKybdStateIdle;
#endif

#if EmClassicKbrd
LOCALVAR blnr HaveKeyBoardResult = falseblnr;
LOCALVAR ui3b KeyBoardResult;
#endif

#if EmClassicKbrd
LOCALPROC GotKeyBoardData(ui3b v)
{
	if (KybdState != kKybdStateIdle) {
		HaveKeyBoardResult = trueblnr;
		KeyBoardResult = v;
	} else {
		VIA_ShiftInData(v);
		VIAiCB2 = 1;
	}
}
#endif

#if EmClassicKbrd
LOCALVAR ui3b InstantCommandData = 0x7B;
#endif

#if EmClassicKbrd
LOCALFUNC blnr AttemptToFinishInquiry(void)
{
	int i;
	blnr KeyDown;
	ui3b Keyboard_Data;

	if (FindKeyEvent(&i, &KeyDown)) {
		if (i < 64) {
			Keyboard_Data = i << 1;
			if (! KeyDown) {
				Keyboard_Data += 128;
			}
		} else {
			Keyboard_Data = 121;
			InstantCommandData = (i - 64) << 1;
			if (! KeyDown) {
				InstantCommandData += 128;
			}
		}
		GotKeyBoardData(Keyboard_Data);
		return trueblnr;
	} else {
		return falseblnr;
	}
}
#endif

#if EmClassicKbrd
#define MaxKeyboardWait 16 /* in 60ths of a second */
#endif
	/*
		Code in the mac rom will reset the keyboard if
		it hasn't been heard from in 32/60th of a second.
		So time out and send something before that
		to keep connection.
	*/

#if EmClassicKbrd
LOCALVAR int InquiryCommandTimer = 0;
#endif

#if EmClassicKbrd
GLOBALPROC DoKybd_ReceiveCommand(void)
{
	if (KybdState != kKybdStateRecievingCommand) {
		ReportAbnormal("KybdState != kKybdStateRecievingCommand");
	} else {
		ui3b in = VIA_ShiftOutData();

		KybdState = kKybdStateRecievedCommand;

		switch (in) {
			case 0x10 : /* Inquiry Command */
				if (! AttemptToFinishInquiry()) {
					InquiryCommandTimer = MaxKeyboardWait;
				}
				break;
			case 0x14 : /* Instant Command */
				GotKeyBoardData(InstantCommandData);
				InstantCommandData = 0x7B;
				break;
			case 0x16 : /* Model Command */
				{
					int i;

					for (i = 4; --i >= 0; ) {
						theKeyCopys[i] = 0;
					}
				}

				GotKeyBoardData(0x0b /*0x01*/); /* Test value, means Model 0, no extra devices */
				/* Fixed by Hoshi Takanori - it uses the proper keyboard type now */
				break;
			case 0x36 : /* Test Command */
				GotKeyBoardData(0x7D);
				break;
			case 0x00:
				GotKeyBoardData(0);
				break;
			default :
				/* Debugger(); */
				GotKeyBoardData(0);
				break;
		}
	}
}
#endif

#if EmClassicKbrd
GLOBALPROC DoKybd_ReceiveEndCommand(void)
{
	if (KybdState != kKybdStateRecievingEndCommand) {
		ReportAbnormal("KybdState != kKybdStateRecievingEndCommand");
	} else {
		KybdState = kKybdStateIdle;
#ifdef _VIA_Debug
		fprintf(stderr, "enter DoKybd_ReceiveEndCommand\n");
#endif
		if (HaveKeyBoardResult) {
#ifdef _VIA_Debug
			fprintf(stderr, "HaveKeyBoardResult: %d\n", KeyBoardResult);
#endif
			HaveKeyBoardResult = falseblnr;
			VIA_ShiftInData(KeyBoardResult);
			VIAiCB2 = 1;
		}
	}
}
#endif

#if EmClassicKbrd
GLOBALPROC Kybd_DataLineChngNtfy(void)
{
	switch (KybdState) {
		case kKybdStateIdle:
			if (VIAiCB2 == 0) {
				KybdState = kKybdStateRecievingCommand;
#ifdef _VIA_Debug
				fprintf(stderr, "posting kICT_Kybd_ReceiveCommand\n");
#endif
				ICT_add(kICT_Kybd_ReceiveCommand, 10);

				if (InquiryCommandTimer != 0) {
					InquiryCommandTimer = 0; /* abort Inquiry */
				}
			}
			break;
		case kKybdStateRecievedCommand:
			if (VIAiCB2 == 1) {
				KybdState = kKybdStateRecievingEndCommand;
#ifdef _VIA_Debug
				fprintf(stderr, "posting kICT_Kybd_ReceiveEndCommand\n");
#endif
				ICT_add(kICT_Kybd_ReceiveEndCommand, 10);
			}
			break;
	}
}
#endif

#if EmClassicKbrd
LOCALPROC DoOnClassicKbrdIdle(void)
{
	if (InquiryCommandTimer != 0) {
		if (AttemptToFinishInquiry()) {
			InquiryCommandTimer = 0;
		} else {
			--InquiryCommandTimer;
			if (InquiryCommandTimer == 0) {
				GotKeyBoardData(0x7B);
			}
		}
	}
}
#endif

#if EmADB
#define ADB_MaxSzDatBuf 8

LOCALVAR blnr ADB_ListenDatBuf;
LOCALVAR ui3b ADB_IndexDatBuf;
LOCALVAR ui3b ADB_SzDatBuf;
LOCALVAR ui3b ADB_DatBuf[ADB_MaxSzDatBuf];
LOCALVAR ui3b ADB_CurCmd = 0;
LOCALVAR ui3b NotSoRandAddr = 1;
#endif

#if EmADB
LOCALVAR ui3b MouseADBAddress;
LOCALVAR blnr SavedCurMouseButton = falseblnr;
#endif

#define CheckForADBmouseEvt() (SavedCurMouseButton != CurMouseButton)

#if EmADB
LOCALPROC ADB_DoMouseTalk(void)
{
	switch (ADB_CurCmd & 3) {
		case 0:
			if (SavedCurMouseButton != CurMouseButton) {
				ADB_SzDatBuf = 2;
				ADB_DatBuf[0] = CurMouseButton ? 0x00 : 0x80;
				ADB_DatBuf[1] = 0x00;
				SavedCurMouseButton = CurMouseButton;
			}
			ADBMouseDisabled = 0;
			break;
		case 3:
			ADB_SzDatBuf = 2;
			ADB_DatBuf[0] = 0x60 | (NotSoRandAddr & 0x0f);
			ADB_DatBuf[1] = 0x01;
			NotSoRandAddr += 1;
			break;
		default:
			ReportAbnormal("Talk to unknown mouse register");
			break;
	}
}
#endif

#if EmADB
LOCALPROC ADB_DoMouseListen(void)
{
	switch (ADB_CurCmd & 3) {
		case 3:
			if (ADB_DatBuf[1] == 0xFE) {
				/* change address */
				MouseADBAddress = (ADB_DatBuf[0] & 0x0F);
			} else {
				ReportAbnormal("unknown listen op to mouse register 3");
			}
			break;
		default:
			ReportAbnormal("listen to unknown mouse register");
			break;
	}
}
#endif

#if EmADB
LOCALVAR ui3b KeyboardADBAddress;
LOCALVAR ui3b NextADBkeyevt = 0xFF;
#endif

#if EmADB
LOCALFUNC blnr CheckForADBkeyEvt(void)
{
	int i;
	blnr KeyDown;

	if (0xFF == NextADBkeyevt) {
		if (! FindKeyEvent(&i, &KeyDown)) {
			return falseblnr;
		} else {
			switch (i) {
				case MKC_Control:
					i = 0x36;
					break;
				case MKC_Left:
					i = 0x3B;
					break;
				case MKC_Right:
					i = 0x3C;
					break;
				case MKC_Down:
					i = 0x3D;
					break;
				case MKC_Up:
					i = 0x3E;
					break;
				default:
					/* unchanged */
					break;
			}
			NextADBkeyevt = (KeyDown ? 0x00 : 0x80) | i;
		}
	}
	return trueblnr;
}
#endif

#if EmADB
#define GotADBKeyEvt() (NextADBkeyevt = 0xFF)
#endif

#if EmADB
LOCALPROC ADB_DoKeyboardTalk(void)
{
	switch (ADB_CurCmd & 3) {
		case 0:
			{
				if (CheckForADBkeyEvt()) {
					ADB_SzDatBuf = 2;
					ADB_DatBuf[0] = NextADBkeyevt;
					GotADBKeyEvt();
					if (! CheckForADBkeyEvt()) {
						ADB_DatBuf[1] = 0xFF;
					} else {
						ADB_DatBuf[1] = NextADBkeyevt;
						GotADBKeyEvt();
					}
				}
			}
			break;
		case 3:
			ADB_SzDatBuf = 2;
			ADB_DatBuf[0] = 0x60 | (NotSoRandAddr & 0x0f);
			ADB_DatBuf[1] = 0x01;
			NotSoRandAddr += 1;
			break;
		default:
			ReportAbnormal("Talk to unknown keyboard register");
			break;
	}
}
#endif

#if EmADB
LOCALPROC ADB_DoKeyboardListen(void)
{
	switch (ADB_CurCmd & 3) {
		case 3:
			if (ADB_DatBuf[1] == 0xFE) {
				/* change address */
				KeyboardADBAddress = (ADB_DatBuf[0] & 0x0F);
			} else {
				ReportAbnormal("unknown listen op to keyboard register 3");
			}
			break;
		default:
			ReportAbnormal("listen to unknown keyboard register");
			break;
	}
}
#endif

#if EmADB
LOCALFUNC blnr CheckForADBanyEvt(void)
{
	return CheckForADBmouseEvt()
		|| CheckForADBkeyEvt();
}
#endif

#if EmADB
LOCALPROC ADB_DoTalk(void)
{
	ui3b Address = ADB_CurCmd >> 4;
	if (Address == MouseADBAddress) {
		ADB_DoMouseTalk();
	} else if (Address == KeyboardADBAddress) {
		ADB_DoKeyboardTalk();
	}
}
#endif

#if EmADB
LOCALPROC ADB_EndListen(void)
{
	ui3b Address = ADB_CurCmd >> 4;
	if (Address == MouseADBAddress) {
		ADB_DoMouseListen();
	} else if (Address == KeyboardADBAddress) {
		ADB_DoKeyboardListen();
	}
}
#endif

#if EmADB
LOCALPROC ADB_DoReset(void)
{
	MouseADBAddress = 3;
	KeyboardADBAddress = 2;
}
#endif

#if EmADB
GLOBALPROC ADB_DoNewState(void)
{
	ui3b state = ADBst1 * 2 + ADBst0;
#ifdef _VIA_Debug
	fprintf(stderr, "ADB_DoNewState: %d\n", state);
#endif
	{
		VIAiB3 = 1;
		switch (state) {
			case 0: /* Start a new command */
				if (ADB_ListenDatBuf) {
					ADB_ListenDatBuf = falseblnr;
					ADB_SzDatBuf = ADB_IndexDatBuf;
					ADB_EndListen();
				}
				ADB_CurCmd = VIA_ShiftOutData();
#ifdef _VIA_Debug
				fprintf(stderr, "in: %d\n", ADB_CurCmd);
#endif
				ADB_SzDatBuf = 0;
				ADB_IndexDatBuf = 0;
				switch ((ADB_CurCmd >> 2) & 3) {
					case 0: /* reserved */
						switch (ADB_CurCmd & 3) {
							case 0: /* Send Reset */
								ADB_DoReset();
								break;
							case 1: /* Flush */
								{
									ui3b Address = ADB_CurCmd >> 4;

									if ((Address == KeyboardADBAddress)
										|| (Address == MouseADBAddress))
									{
										ADB_SzDatBuf = 2;
										ADB_DatBuf[0] = 0x00;
										ADB_DatBuf[1] = 0x00;
									} else {
										ReportAbnormal("Unhandled ADB Flush");
									}
								}
								break;
							case 2: /* reserved */
							case 3: /* reserved */
								ReportAbnormal("Reserved ADB command");
								break;
						}
						break;
					case 1: /* reserved */
						ReportAbnormal("Reserved ADB command");
						break;
					case 2: /* listen */
						ADB_ListenDatBuf = trueblnr;
						ADB_SzDatBuf = 8;
#ifdef _VIA_Debug
						fprintf(stderr, "*** listening\n");
#endif
						break;
					case 3: /* talk */
						ADB_DoTalk();
						break;
				}
				break;
			case 1: /* Transfer date byte (even) */
			case 2: /* Transfer date byte (odd) */
				if (ADB_IndexDatBuf >= ADB_SzDatBuf) {
					if (! ADB_ListenDatBuf) {
#ifdef _VIA_Debug
						fprintf(stderr, "*** talk too much\n");
#endif
						VIA_ShiftInData(0xFF);
						VIAiCB2 = 1;
					} else {
#ifdef _VIA_Debug
						fprintf(stderr, "*** listen too much\n");
#endif
						(void) VIA_ShiftOutData();
					}
					VIAiB3 = 0;
				} else {
					if (! ADB_ListenDatBuf) {
#ifdef _VIA_Debug
						fprintf(stderr, "*** talk one\n");
#endif
						VIA_ShiftInData(ADB_DatBuf[ADB_IndexDatBuf]);
						VIAiCB2 = 1;
					} else {
#ifdef _VIA_Debug
						fprintf(stderr, "*** listen one\n");
#endif
						ADB_DatBuf[ADB_IndexDatBuf] = VIA_ShiftOutData();
					}
					ADB_IndexDatBuf += 1;
				}
				break;
			case 3: /* idle */
				if ((ADB_SzDatBuf != 0) && (ADB_IndexDatBuf == 0)) {
					VIA_ShiftInData(0xFF);
					/* VIAiB3 = 0; */
				} else if (CheckForADBanyEvt()) {
					VIA_ShiftInData(0xFF);
					/* VIAiB3 = 0; */
				}
				break;
		}
	}
}
#endif

#if EmADB
GLOBALPROC ADBstate_ChangeNtfy(void)
{
#ifdef _VIA_Debug
	fprintf(stderr, "ADBstate_ChangeNtfy: %d, %d, %d\n", ADBst1, ADBst0, GetCuriCount());
#endif
	ICT_add(kICT_ADB_NewState, 50);
}
#endif

#if EmADB
GLOBALPROC ADB_DataLineChngNtfy(void)
{
#ifdef _VIA_Debug
	fprintf(stderr, "ADB_DataLineChngNtfy: %d\n", VIAiCB2);
#endif
}
#endif

#if EmADB
LOCALPROC ADB_DoOnIdle(void)
{
	ui3b state = ADBst1 * 2 + ADBst0;

	if (state == 3) { /* idle */
		if ((ADB_SzDatBuf != 0) && (ADB_IndexDatBuf == 0)) {
			VIA_ShiftInData(0xFF);
			/* VIAiB3 = 0; */
		} else if (CheckForADBanyEvt())
		{
			if (((ADB_CurCmd >> 2) & 3) == 3) {
				ADB_DoTalk();
			}
			VIA_ShiftInData(0xFF);
			/* VIAiB3 = 0; */
		}
	}
}
#endif

IMPORTPROC DoMacReset(void);

GLOBALPROC KeyBoard_Update(void)
{
	SetInterruptButton(falseblnr);
		/*
			in case has been set. so only stays set
			for 60th of a second.
		*/

#if EmClassicKbrd
	DoOnClassicKbrdIdle();
#elif EmADB
	ADB_DoOnIdle();
#endif
	if (WantMacInterrupt) {
		SetInterruptButton(trueblnr);
		WantMacInterrupt = falseblnr;
	}
	if (WantMacReset) {
		DoMacReset();
		WantMacReset = falseblnr;
	}
}
