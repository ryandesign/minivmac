/*
	POSTKEYS.h
	Copyright (C) 2002 Weston Pawlowski, Paul Pratt

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
	POST KEYboard EventS

	Convert mac style keyscan codes to mac
	events and then post them. Also updates
	theKeys variable.
*/

#ifdef POSTKEYS_H
#error "header already included"
#else
#define POSTKEYS_H
#endif

#define MKC_A 0x00
#define MKC_B 0x0B
#define MKC_C 0x08
#define MKC_D 0x02
#define MKC_E 0x0E
#define MKC_F 0x03
#define MKC_G 0x05
#define MKC_H 0x04
#define MKC_I 0x22
#define MKC_J 0x26
#define MKC_K 0x28
#define MKC_L 0x25
#define MKC_M 0x2E
#define MKC_N 0x2D
#define MKC_O 0x1F
#define MKC_P 0x23
#define MKC_Q 0x0C
#define MKC_R 0x0F
#define MKC_S 0x01
#define MKC_T 0x11
#define MKC_U 0x20
#define MKC_V 0x09
#define MKC_W 0x0D
#define MKC_X 0x07
#define MKC_Y 0x10
#define MKC_Z 0x06

#define MKC_1 0x12
#define MKC_2 0x13
#define MKC_3 0x14
#define MKC_4 0x15
#define MKC_5 0x17
#define MKC_6 0x16
#define MKC_7 0x1A
#define MKC_8 0x1C
#define MKC_9 0x19
#define MKC_0 0x1D

#define MKC_Command 0x37
#define MKC_Shift 0x38
#define MKC_CapsLock 0x39
#define MKC_Option 0x3A

#define MKC_Space 0x31
#define MKC_Return 0x24
#define MKC_BackSpace 0x33
#define MKC_Tab 0x30
#define MKC_Right 0x42
#define MKC_Left 0x46
#define MKC_Down 0x48
#define MKC_Up 0x4D

#define MKC_Minus 0x1B
#define MKC_Equal 0x18
#define MKC_BackSlash 0x2A
#define MKC_Comma 0x2B
#define MKC_Period 0x2F
#define MKC_Slash 0x2C
#define MKC_SemiColon 0x29
#define MKC_SingleQuote 0x27
#define MKC_LeftBracket 0x21
#define MKC_RightBracket 0x1E
#define MKC_Grave 0x32
#define MKC_Clear 0x47
#define MKC_KPEqual 0x51
#define MKC_KPDevide 0x4B
#define MKC_KPMultiply 0x43
#define MKC_KPSubtract 0x4E
#define MKC_KPAdd 0x45
#define MKC_Enter 0x4C

#define MKC_KP1 0x53
#define MKC_KP2 0x54
#define MKC_KP3 0x55
#define MKC_KP4 0x56
#define MKC_KP5 0x57
#define MKC_KP6 0x58
#define MKC_KP7 0x59
#define MKC_KP8 0x5B
#define MKC_KP9 0x5C
#define MKC_KP0 0x52
#define MKC_Decimal 0x41

/* these aren't on the Mac Plus keyboard */

#define MKC_Control 0x3B
#define MKC_Escape 0x35
#define MKC_F1 0x7a
#define MKC_F2 0x78
#define MKC_F3 0x63
#define MKC_F4 0x76
#define MKC_F5 0x60
#define MKC_F6 0x61
#define MKC_F7 0x62
#define MKC_F8 0x64
#define MKC_F9 0x65
#define MKC_F10 0x6d
#define MKC_F11 0x67
#define MKC_F12 0x6f

LOCALPROC Keyboard_UpdateKeyMap(int key, int down)
{
	ui3b *kp = (ui3b *)theKeys;

	if (key >= 0 && key < 128) {
		int bit = 1 << (key & 7);
		if (down) {
			kp[key / 8] |= bit;
		} else {
			kp[key / 8] &= ~bit;
		}
	}
}
