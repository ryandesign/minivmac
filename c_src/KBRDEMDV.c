/*
	KBRDEMDV.c

	Copyright (C) 2004 Philip Cummins, Paul Pratt

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
#include "ENDIANAC.h"
#include "MINEM68K.h"
#include "ADDRSPAC.h"
#if UseControlKeys
#include "CONTROLM.h"
#endif
#include "VIAEMDEV.h"
#endif

#include "KBRDEMDV.h"

#if UseControlKeys
LOCALVAR ui5b theEmKeys[4];
#else
#define theEmKeys theKeys
#endif
	/*
		What the emulated keyboard thinks is the
		state of the keyboard.
	*/

LOCALVAR ui5b theKeyCopys[4];
	/*
		What the emulated keyboard thinks the mac thinks is the
		state of the keyboard. This is compared to theEmKeys.
	*/

LOCALFUNC blnr FindKeyEvent(int *VirtualKey, blnr *KeyDown)
{
	int j;
	int b;

	for (j = 0; j < 16; ++j) {
		ui3b k1 = ((ui3b *)theEmKeys)[j];
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

LOCALVAR ui3b InstantCommandData = 0x7B;

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

#define MaxKeyboardWait 16 /* in 60ths of a second */
	/*
		Code in the mac rom will reset the keyboard if
		it hasn't been heard from in 32/60th of a second.
		So time out and send something before that
		to keep connection.
	*/

LOCALVAR int InquiryCommandTimer = 0;

GLOBALPROC Keyboard_Put(ui3b in)
{
	if (InquiryCommandTimer != 0) {
		InquiryCommandTimer = 0; /* abort Inquiry */
	}
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

#if UseControlKeys
LOCALVAR ui5b LastKeys[4];
LOCALVAR blnr LastControlKey = falseblnr;
#endif

GLOBALPROC KeyBoard_Update(void)
{
#if UseControlKeys
	int i;
	int j;

	SetInterruptButton(falseblnr);
		/*
			in case has been set. so only stays set
			for 60th of a second.
		*/

	if ((((ui3b *)theKeys)[7] & (1 << 3)) == 0) {
		if (LastControlKey) {
			LastControlKey = falseblnr;
			DoLeaveControlMode();
		}
		for (i = 0; i < 4; ++i) {
			theEmKeys[i] = theKeys[i];
		}
		if (ControlKeyPressed) {
			((ui3b *)theEmKeys)[7] |=  (1 << 3);
		}
	} else {
		if (! LastControlKey) {
			LastControlKey = trueblnr;
			DoEnterControlMode();
		} else {
			for (j = 0; j < 16; ++j) {
				ui3b k1 = ((ui3b *)theKeys)[j];
				ui3b k2 = ((ui3b *)LastKeys)[j];
				ui3b k3 = k1 & (~ k2);

				if (k3 != 0) {
					for (i = 0; i < 8; ++i) {
						if ((k3 & (1 << i)) != 0) {
							DoControlModeKey(j * 8 + i);
						}
					}
				}
			}
		}

		for (i = 0; i < 4; ++i) {
			LastKeys[i] = theKeys[i];
		}
	}
#endif

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
