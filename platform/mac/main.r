/*
	main.r

	Copyright (C) 2003 Philip Cummins, Richard F. Bannister, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#include "CNFGRSRC.h"

#ifndef UseCarbonLib
#define UseCarbonLib 0
#endif

/* Alerts Constants */

#define kMyAboutAlert 128
#define kMyStandardAlert 131
#define kMyOkCancelAlert 132

/* Menu Constants */

#define kMyMenuBar   128

#define kAppleMenu   128
#define kFileMenu    129
#define kSpecialMenu 130

resource 'MBAR' (kMyMenuBar, preload) {
	{ /* array MenuArray: 3 elements */
		/* [1] */
		kAppleMenu,
		/* [2] */
		kFileMenu,
		/* [3] */
		kSpecialMenu
	}
};

resource 'MENU' (kAppleMenu, preload) {
	kAppleMenu,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	apple,
	{ /* array: 2 elements */
		/* [1] */
		"About " kStrAppName "...", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (kFileMenu, preload) {
	kFileMenu,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	"File",
	{ /* array: 3 elements */
		/* [1] */
		"Open Disk Image É", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Quit", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (kSpecialMenu, preload) {
	kSpecialMenu,
	textMenuProc,
#if UseCarbonLib
	0x7FFFFF8D,
#else
	0x7FFFFFFD,
#endif
	enabled,
	"Special",
	{ /* array: 4 elements */
		/* [1] */
		"Limit Speed", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Reset", noIcon, noKey, noMark, plain,
		/* [4] */
		"Interrupt", noIcon, noKey, noMark, plain
#if UseCarbonLib
		,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"Command-Q", noIcon, "Q", noMark, plain,
		/* [7] */
		"Command-H", noIcon, "H", noMark, plain
#endif
	}
};

resource 'DITL' (kMyStandardAlert, purgeable) {
	{ /* array DITLarray: 2 elements */
		/* [1] */
		{145, 293, 165, 351},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 72, 130, 353},
		StaticText {
			disabled,
			"^0\n\n^1^2^3"
		}
	}
};

resource 'DITL' (kMyOkCancelAlert, purgeable) {
	{ /* array DITLarray: 3 elements */
		/* [1] */
		{145, 197, 165, 255},
		Button {
			enabled,
			"Cancel"
		},
		/* [2] */
		{145, 293, 165, 351},
		Button {
			enabled,
			"OK"
		},
		/* [3] */
		{10, 72, 130, 353},
		StaticText {
			disabled,
			"^0\n\n^1^2^3"
		}
	}
};

resource 'ALRT' (kMyStandardAlert, "Non Fatal Error", purgeable) {
	{40, 43, 217, 405},
	kMyStandardAlert,
	{ /* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	alertPositionMainScreen
};

resource 'ALRT' (kMyOkCancelAlert, "Ok Cancel", purgeable) {
	{40, 43, 217, 405},
	kMyOkCancelAlert,
	{ /* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	alertPositionMainScreen
};

resource 'DITL' (kMyAboutAlert, purgeable) {
	{ /* array DITLarray: 7 elements */
		/* [1] */
		{261, 377, 281, 435},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 97, 38, 455},
		StaticText {
			disabled,
			kAppVariationStr ", Copyright " kStrCopyrightYear "."
		},
		/* [3] */
		{47, 97, 100, 455},
		StaticText {
			disabled,
			"Including or based upon code by Bernd Sc"
			"hmidt, Philip Cummins, Richard F. Bannis"
			"ter, Weston Pawlowski, Michael Hanni, "
			"Paul Pratt, and others."
		},
		/* [4] */
		{110, 97, 151, 455},
		StaticText {
			disabled,
			kStrAppName " is distributed under the terms"
			" of the GNU Public License, version 2."
		},
		/* [5] */
		{165, 6, 223, 455},
		StaticText {
			disabled,
			kStrAppName " is distributed in the hope tha"
			"t it will be useful, but WITHOUT ANY WAR"
			"RANTY; without even the implied warranty"
			" of MERCHANTABILITY or FITNESS FOR A PAR"
			"TICULAR PURPOSE."
		},
		/* [6] */
		{235, 6, 258, 339},
		StaticText {
			disabled,
			"For more information, see:"
		},
		/* [7] */
		{267, 21, 290, 339},
		StaticText {
			disabled,
			kStrHomePage
		}
	}
};

resource 'ALRT' (kMyAboutAlert, "About", purgeable) {
	{34, 27, 330, 489},
	kMyAboutAlert,
	{ /* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	alertPositionMainScreen
};
