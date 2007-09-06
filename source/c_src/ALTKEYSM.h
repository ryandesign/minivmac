/*
	ALTKEYSM.h

	Copyright (C) 2007 Paul C. Pratt

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
	ALTernate KEYs Mode
*/

#ifdef ALTKEYSM_H
#error "header already included"
#else
#define ALTKEYSM_H
#endif

LOCALVAR blnr AltKeysTempMode = falseblnr;
LOCALVAR blnr AltKeysLockMode = falseblnr;
LOCALVAR blnr AltKeysTrueCmnd = falseblnr;
LOCALVAR blnr AltKeysTrueOption = falseblnr;
LOCALVAR blnr AltKeysTrueShift = falseblnr;
LOCALVAR blnr AltKeysModeOn = falseblnr;

LOCALPROC CheckAltKeyUseMode(void)
{
	blnr NewAltKeyUseMode = (AltKeysTempMode | AltKeysLockMode)
		&& (! AltKeysTrueCmnd);
	if (NewAltKeyUseMode != AltKeysModeOn) {
		DisconnectKeyCodes(kKeepMaskControl | kKeepMaskCapsLock
			| (AltKeysTrueCmnd ? kKeepMaskCommand : 0)
			| (AltKeysTrueOption ? kKeepMaskOption : 0)
			| (AltKeysTrueShift ? kKeepMaskShift : 0));
		AltKeysModeOn = NewAltKeyUseMode;
	}
}

LOCALPROC Keyboard_UpdateKeyMap1(int key, blnr down)
{
	if (MKC_SemiColon == key) {
		AltKeysTempMode = down;
		CheckAltKeyUseMode();
	} else if (MKC_Command == key) {
		AltKeysTrueCmnd = down;
		CheckAltKeyUseMode();
		Keyboard_UpdateKeyMap(key, down);
	} else if (MKC_Option == key) {
		AltKeysTrueOption = down;
		Keyboard_UpdateKeyMap(key, down);
	} else if (MKC_Shift == key) {
		AltKeysTrueShift = down;
		Keyboard_UpdateKeyMap(key, down);
	} else if (! AltKeysModeOn) {
		Keyboard_UpdateKeyMap(key, down);
	} else if (MKC_M == key) {
		if (down) {
			AltKeysLockMode = trueblnr;
			CheckAltKeyUseMode();
		}
	} else if (MKC_U == key) {
		if (down) {
			AltKeysLockMode = falseblnr;
			CheckAltKeyUseMode();
		}
	} else {
		switch (key) {
			case MKC_A:
				key = MKC_BackSpace;
				break;
			case MKC_B:
				key = MKC_ForwardDel;
				break;
			case MKC_C:
				key = MKC_F3;
				break;
			case MKC_D:
				key = MKC_Option;
				break;
			case MKC_E:
				key = MKC_Return;
				break;
			case MKC_F:
				key = MKC_Command;
				break;
			case MKC_G:
				key = MKC_Enter;
				break;
			case MKC_H:
				key = MKC_SemiColon;
				break;
			case MKC_I:
				key = MKC_Up;
				break;
			case MKC_J:
				key = MKC_Left;
				break;
			case MKC_K:
				key = MKC_Down;
				break;
			case MKC_L:
				key = MKC_Right;
				break;
			case MKC_M:
				/* handled above */
				break;
			case MKC_N:
				key = MKC_Help;
				break;
			case MKC_O:
				key = MKC_Home;
				break;
			case MKC_P:
				key = MKC_End;
				break;
			case MKC_Q:
				key = MKC_Grave;
				break;
			case MKC_R:
				key = MKC_PageUp;
				break;
			case MKC_S:
				key = MKC_Shift;
				break;
			case MKC_T:
				key = MKC_PageDown;
				break;
			case MKC_U:
				/* handled above */
				break;
			case MKC_V:
				key = MKC_F4;
				break;
			case MKC_W:
				key = MKC_BackSlash;
				break;
			case MKC_X:
				key = MKC_F2;
				break;
			case MKC_Y:
				key = MKC_Escape;
				break;
			case MKC_Z:
				key = MKC_F1;
				break;
			default:
				break;
		}
		Keyboard_UpdateKeyMap(key, down);
	}
}

LOCALPROC DisconnectKeyCodes1(ui5b KeepMask)
{
	DisconnectKeyCodes(KeepMask);

	AltKeysTempMode = falseblnr;
	if (! (0 != (KeepMask & kKeepMaskCommand))) {
		AltKeysTrueCmnd = falseblnr;
	}
	if (! (0 != (KeepMask & kKeepMaskOption))) {
		AltKeysTrueOption = falseblnr;
	}
	if (! (0 != (KeepMask & kKeepMaskShift))) {
		AltKeysTrueShift = falseblnr;
	}

	if (AltKeysLockMode != AltKeysModeOn) {
		AltKeysModeOn = AltKeysLockMode;
	}
}
