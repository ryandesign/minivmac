/*
	SNDEMDEV.c

	Copyright (C) 2003 Philip Cummins, Paul C. Pratt

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
	SouND EMulated DEVice

	Emulation of Sound in the Mac Plus could go here.

	This code adapted from "Sound.c" in vMac by Philip Cummins.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"

#include "MYOSGLUE.h"
#include "PROGMAIN.h"
#include "ADDRSPAC.h"
#endif

#include "SNDEMDEV.h"

#define kSnd_Main_Offset   0x0300
#define kSnd_Alt_Offset    0x5F00

#define kSnd_Main_Buffer (kRAM_Size - kSnd_Main_Offset)
#define kSnd_Alt_Buffer (kRAM_Size - kSnd_Alt_Offset)


#if MySoundEnabled

LOCALVAR const ui4b vol_mult[] = {
	8192, 9362, 10922, 13107, 16384, 21845, 32768
};

LOCALVAR const ui3b vol_offset[] = {
	112, 110, 107, 103, 96, 86, 64, 0
};

/*
	approximate volume levels of vMac, so:

	x * vol_mult[SoundVolume] >> 16
		+ vol_offset[SoundVolume]
	= {approx} (x - 128) / (8 - SoundVolume) + 128;
*/

LOCALVAR const ui4b SubTick_offset[kNumSubTicks + 1] = {
	0,    23,  46,  69,  92, 115, 138, 161,
	185, 208, 231, 254, 277, 300, 323, 346,
	370
};

LOCALVAR ui3p TheCurSoundBuff = nullpr;

LOCALVAR ui5b SoundInvertPhase = 0;
LOCALVAR ui4b SoundInvertState = 0;

IMPORTFUNC ui4b GetSoundInvertTime(void);

GLOBALPROC MacSound_SubTick(int SubTick)
{
	if (SubTick == 0) {
		TheCurSoundBuff = GetCurSoundOutBuff();
	}

	if (TheCurSoundBuff != nullpr) {
		int i;
		ui4b SoundInvertTime = GetSoundInvertTime();
		ui5b StartOffset = SubTick_offset[SubTick];
		ui5b n = SubTick_offset[SubTick + 1] - StartOffset;
		unsigned long addy =
#ifdef SoundBuffer
			(SoundBuffer == 0) ? kSnd_Alt_Buffer :
#endif
			kSnd_Main_Buffer;
		ui3p addr = addy + (2 * StartOffset) + (ui3p)RAM;
		ui3p p = StartOffset + TheCurSoundBuff;

		if (SoundDisable && (SoundInvertTime == 0)) {
			for (i = 0; i < n; i++) {
#if 1
				*p++ = 0x80;
#else
				*p++ = 0x00;
#endif
				/*
					0x00 would be more acurate, but 0x80 works
					nicer, so can pause emulation without click.
				*/
			}
		} else {
			ui3b SoundVolume = SoundVolb0
				| (SoundVolb1 << 1)
				| (SoundVolb2 << 2);
			ui3b offset = vol_offset[SoundVolume];

			if (SoundVolume >= 7) {
				/* Volume is full, so we can do it the fast way */

				for (i = 0; i < n; i++) {
					/* Copy the buffer over */
					*p++ = *addr;

					/* Move the address on */
					addr += 2;
				}
			} else {
				ui5b mult = (ui5b)vol_mult[SoundVolume];

				for (i = 0; i < n; i++) {
					/* Copy the buffer over */
					*p++ = (ui3b)((ui5b)(*addr) * mult >> 16);

					/* Move the address on */
					addr += 2;
				}
			}
			if (SoundInvertTime != 0) {
				ui5b PhaseIncr = (ui5b)SoundInvertTime * (ui5b)20;
				p -= n;

				for (i = 0; i < n; i++) {
					if (SoundInvertPhase < 704) {
						ui5b OnPortion = 0;
						ui5b LastPhase = 0;
						do {
							if (! SoundInvertState) {
								OnPortion += (SoundInvertPhase - LastPhase);
							}
							SoundInvertState = ! SoundInvertState;
							LastPhase = SoundInvertPhase;
							SoundInvertPhase += PhaseIncr;
						} while (SoundInvertPhase < 704);
						if (! SoundInvertState) {
							OnPortion += 704 - LastPhase;
						}
						*p = (*p * OnPortion) / 704;
					} else {
						if (SoundInvertState) {
							*p = 0;
						}
					}
					SoundInvertPhase -= 704;
					p++;
				}
			}
#if 1
			if (offset != 0) {
				p -= n;
				for (i = 0; i < n; i++) {
					*p++ += offset;
				}
			}
#endif
		}
	}
}
#endif
