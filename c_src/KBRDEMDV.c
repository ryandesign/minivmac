/*
	KBRDEMDV.c

	Copyright (C) 2001 Philip Cummins, Paul Pratt

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

#include "SYSDEPNS.h"

#include "GLOBGLUE.h"
#include "ENDIANAC.h"
#include "ADDRSPAC.h"

#include "KBRDEMDV.h"
#include "OSCOMVAR.h"

UBYTE Keyboard_Data = 0;
UBYTE Keyboard_Enabled = 0;

UBYTE KeyCode_Table [128] =
{
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x0x
//   Del   Tab                     Ret
	0x33, 0x30, 0xFF, 0xFF, 0xFF, 0x49, 0xFF, 0xFF,
//
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x1x
//                           Lft   Rgt    Up   Dwn
	0xFF, 0xFF, 0xFF, 0xFF, 0x46, 0x42, 0x4D, 0x48,
//   Spc     !     "     #     $     %     &     '
	0x31, 0x92, 0xA7, 0x94, 0x95, 0x97, 0x9A, 0x27, // 0x2x
//     (     )     *     +     ,     -     .     /
	0x99, 0x9D, 0x9C, 0x98, 0x2B, 0x1B, 0x2F, 0x2C,
//     0     1     2     3     4     5     6     7
	0x1D, 0x12, 0x13, 0x14, 0x15, 0x17, 0x16, 0x1A, // 0x3x
//     8     9     :     ;     <     =     >     ?
	0x1C, 0x19, 0xA9, 0x29, 0xAB, 0x18, 0xAF, 0xAC,
//     @     A     B     C     D     E     F     G
	0x93, 0x80, 0x8B, 0x88, 0x82, 0x8E, 0x83, 0x85, // 0x4x
//     H     I     J     K     L     M     N     O
	0x84, 0xA2, 0xA6, 0xA8, 0xA5, 0xAE, 0xAD, 0x9F,
//     P     Q     R     S     T     U     V     W
	0xA3, 0x8C, 0x8F, 0x81, 0x91, 0xA0, 0x89, 0x8D, // 0x5x
//     X     Y     Z           \           ^     _
	0x87, 0x90, 0x86, 0xFF, 0x2A, 0xFF, 0x96, 0x9B,
//     `     a     b     c     d     e     f     g
	0x32, 0x00, 0x0B, 0x08, 0x02, 0x0E, 0x03, 0x05, // 0x6x
//     h     i     j     k     l     m     n     o
	0x04, 0x22, 0x26, 0x28, 0x25, 0x2E, 0x2D, 0x1F,
//     p     q     r     s     t     u     v     w
	0x23, 0x0C, 0x0F, 0x01, 0x11, 0x20, 0x09, 0x0D, // 0x7x
//     x     y     z           |           ~
	0x07, 0x10, 0x06, 0xFF, 0xAA, 0xFF, 0xB2, 0xFF
};

int  Keyboard_Init (void)
{
	Keyboard_Data = 0;
	Keyboard_Enabled = 0;
	return 0;
}

void Keyboard_Reset (void)
{
	(void) Keyboard_Init();
}

void  Keyboard_Put (UBYTE in)
{
	switch (in) {
		case 0x10 : // Inquiry Command
			Keyboard_Data = 0x7B; // Return NULL
			break;
		case 0x14 : // Instant Command
			Keyboard_Data = 0x7B; // Return NULL
			break;
		case 0x16 : // Model Command
			Keyboard_Enabled = 1;
			Keyboard_Data = 0x0b /*0x01*/; // Test value, means Model 0, no extra devices
			//Fixed by Hoshi Takanori - it uses the proper keyboard type now
			break;
		case 0x36 : // Test Command
			Keyboard_Data = 0x7D; // Return ACK
			break;
		case 0x00:
			Keyboard_Data = 0;
			break;
		default :
			/* Debugger(); */
			Keyboard_Data = 0;
			break;
	}
}

UBYTE Keyboard_Get (void)
{
	return Keyboard_Data;
}

/*
	Keyboard Routines

	These use a KeyDown KeyUp principle, as well as MacOS utilizing a KeyAuto.
	If the key is held down for an extended period of time.

	The PC_PostEvent takes an Event type, a Scan and ASCII code, and a Modifiers
	field. The Modifiers field is responsible for implementing Command or Shift
	keys. You'll need to write code for your system to find out at the time of
	the key press whether a modifer key was held down, then set the Modifiers
	field and then call PC_PostEvent.

	The alternative I have done below gets an ASCII character, converts it to
	a scan code and is sent to PC_PostEvent as a word, with the scan code in the
	high byte and the ASCII in the low byte. I have used bit 7 of the scan code
	to indicate shift down (temporarily, you might have a better way) and this
	sets the modifiers field.

	The only draw back is you won't get all the nifty extra characters on the
	keyboard, but I'll see what I can do about fixing this up once you've worked
	an interface out for it.

	Modifier Bit Meanings are

	0x0100 = Command
	0x0200 = Shift
	0x0400 = Caps Lock
	0x0800 = Option
	0x1000 = Control (Unused in Mac Plus)
*/

void PC_PostEvent (UWORD What, ULONG Msg, UWORD Mods);

#ifndef MacTarget
#define MacTarget 0
#endif

void Keyboard_Down (ULONG Key)
{
#if MacTarget
	PC_PostEvent(0x0003, (Key & 0xFFFF), ((Key & 0x1F000000) >> 16));
#else
	UBYTE ScanCode = KeyCode_Table[Key & 0x7F];
	UWORD Modifier = ((Key & 0x1F000000) >> 16);

	if ((ScanCode & 0x80) == 0x80) {
		Modifier = 0x0200; // Set Modifiers up (add any if required!)
		ScanCode &= 0x7F; // Strip Shift Indicator
	}
	PC_PostEvent(0x0003, ((ScanCode << 8) + (Key & 0x7F)), Modifier);
#endif
}

void Keyboard_Up (ULONG Key)
{
#if MacTarget
	PC_PostEvent(0x0004, (Key & 0xFFFF), ((Key & 0x1F000000) >> 16));
#else
	// Work via ASCII Input
	UBYTE ScanCode = KeyCode_Table[Key & 0x7F];
	UWORD Modifier = 0;

	if ((ScanCode & 0x80) == 0x80) {
		Modifier = 0x0200; // Set Modifiers up (add any if required!)
		ScanCode &= 0x7F; // Strip Shift Indicator
	}
	PC_PostEvent(0x0004, ((ScanCode << 8) + (Key & 0x7F)), Modifier);
#endif
}

void Keyboard_Auto (ULONG Key)
{
#if MacTarget
	PC_PostEvent(0x0005, (Key & 0xFFFF), ((Key & 0x1F000000) >> 16));
#else
	// Work via ASCII Input
	UBYTE ScanCode = KeyCode_Table[Key & 0x7F];
	UWORD Modifier = 0;

	if ((ScanCode & 0x80) == 0x80) {
		Modifier = 0x0200; // Set Modifiers up (add any if required!)
		ScanCode &= 0x7F; // Strip Shift Indicator
	}
	PC_PostEvent(0x0005, ((ScanCode << 8) + (Key & 0x7F)), Modifier);
#endif
}

// Test, this slips events in without using an A-Trap
// Returns 0 = NoErr, 1 = Ignored (implemented later)

#define kTicks       0x016A
#define kMouse       0x0830
#define kEventBuffer 0x0146
#define kEventQueue  0x014A
#define kEventCount  0x0154
#define kSysEvtMask  0x0144

#define kqFlags 0
#define kqHead  2
#define kqTail  6

#define kqLink   0
#define kqType   4
#define kqWhat   6
#define kqMsg    8
#define kqWhen  12
#define kqWhere 16
#define kqMods  20


// This adds keyboard events to the event queue
// What = Type of Event (3 = KeyDown, 4 = KeyUp, 5 = KeyAuto)
// Msg = HiByte = Scan Code, LoByte = ASCII Char
// Mods = Modifiers for the Keys

void PC_PostEvent (UWORD What, ULONG Msg, UWORD Mods)
{
	ULONG Buffer = get_long(kEventBuffer); // Buffer for Event Queue
	ULONG Limit = Buffer + (get_word(kEventCount) * 22);  // Address of Tail
	ULONG FirstEvent = get_long(kEventQueue + kqHead);
	ULONG NewEvent;

	if (Keyboard_Enabled == 0) {
		return; // Not initalized!
	}
	if ((get_word(kSysEvtMask) & (1 << What)) == 0) {
		return; /* this event is masked out */
	}

	if (FirstEvent == 0x00000000) { // Nothing in the Queue
		NewEvent = Buffer;
		put_long(kEventQueue + kqHead, NewEvent);
	} else {
		ULONG LastEvent = get_long(kEventQueue + kqTail);
		NewEvent = LastEvent + 22; // Get to next

		if (NewEvent == Limit) {
			NewEvent = Buffer; /* wrap back to first */
		}

		if (NewEvent == FirstEvent) {
			return; // No more space!
		}

		put_long(LastEvent + kqLink, NewEvent);
	}
	put_long(kEventQueue + kqTail, NewEvent);

	put_long(NewEvent + kqLink, 0x00000000);
	put_word(NewEvent + kqType, 0x0004); // Event Queue
	put_word(NewEvent + kqWhat, What);
	put_long(NewEvent + kqMsg, Msg);
	put_long(NewEvent + kqWhen, get_long(kTicks));
	put_long(NewEvent + kqWhere, get_long(kMouse));
	put_word(NewEvent + kqMods, Mods);
}

void KeyBoard_Update(void)
{
	if (Keyboard_Enabled) {
		put_long(0x174, theKeys[0]);
		put_long(0x178, theKeys[1]);
	}
}
