/*
	POSTKEYS.h
	Copyright (C) 2003 Weston Pawlowski, Paul Pratt

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

LOCALFUNC blnr Keyboard_TestKeyMap(int key)
{
	if ((key >= 0) && (key < 128)) {
		ui3b *kp = (ui3b *)theKeys;
		return (kp[key / 8] & (1 << (key & 7))) != 0;
	} else {
		return falseblnr;
	}
}
