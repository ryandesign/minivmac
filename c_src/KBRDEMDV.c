/*
	KBRDEMDV.c

	Copyright (C) 2002 Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with with this file; see the file COPYING.

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
#include "ADDRSPAC.h"
#include "VIAEMDEV.h"
#endif

#include "KBRDEMDV.h"

LOCALVAR UBYTE CommandPending = 0;

GLOBALPROC Keyboard_Put(UBYTE in)
{
	CommandPending = in;
}

LOCALVAR ULONG theKeyCopys[4];
	/*
		What the emulated keyboard thinks the mac thinks is the
		state of the keyboard. This is compared to theKeys,
		which is the actual state of the real keyboard.
	*/

LOCALFUNC blnr FindKeyEvent(int *VirtualKey, blnr *KeyDown)
{
	int j;
	int b;

	for (j = 0; j < 16; ++j) {
		UBYTE k1 = ((UBYTE *)theKeys)[j];
		UBYTE k2 = ((UBYTE *)theKeyCopys)[j];

		if (k1 != k2) {
			UBYTE bit = 1;
			for (b = 0; b < 8; ++b) {
				UBYTE newk = k1 & bit;
				UBYTE oldk = k2 & bit;

				if (oldk != newk) {
					((UBYTE *)theKeyCopys)[j] = (k2 & ~ bit) | newk;

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

LOCALVAR UBYTE InstantCommandData = 0x7B;

LOCALFUNC blnr AttemptToFinishInquiry(void)
{
	int i;
	blnr KeyDown;
	UBYTE Keyboard_Data;

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

GLOBALPROC Keyboard_Get(void)
{
	if (InquiryCommandTimer != 0) {
		InquiryCommandTimer = 0; /* abort Inquiry */
	}
	switch (CommandPending) {
		case 0x10 : // Inquiry Command
			if (! AttemptToFinishInquiry()) {
				InquiryCommandTimer = MaxKeyboardWait;
			}
			break;
		case 0x14 : // Instant Command
			GotKeyBoardData(InstantCommandData);
			break;
		case 0x16 : // Model Command
			{
				int i;

				for (i = 4; --i >=0; ) {
					theKeyCopys[i] = 0;
				}
			}

			GotKeyBoardData(0x0b /*0x01*/); // Test value, means Model 0, no extra devices
			//Fixed by Hoshi Takanori - it uses the proper keyboard type now
			break;
		case 0x36 : // Test Command
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

GLOBALPROC KeyBoard_Update(void)
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
