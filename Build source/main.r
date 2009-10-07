#include "Types.r"
#include "SysTypes.r"

resource 'MENU' (128, preload) {
	128,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	apple,
	{	/* array: 2 elements */
		/* [1] */
		"About Build…", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (129, preload) {
	129,
	textMenuProc,
	0x5,
	enabled,
	"File",
	{	/* array: 3 elements */
		/* [1] */
		"Go", noIcon, "G", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Quit", noIcon, "Q", noMark, plain
	}
};

resource 'MENU' (130, preload) {
	130,
	textMenuProc,
	0x0,
	enabled,
	"Edit",
	{	/* array: 8 elements */
		/* [1] */
		"Undo", noIcon, "Z", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Cut", noIcon, "X", noMark, plain,
		/* [4] */
		"Copy", noIcon, "C", noMark, plain,
		/* [5] */
		"Paste", noIcon, "V", noMark, plain,
		/* [6] */
		"Clear", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"Select All", noIcon, "A", noMark, plain
	}
};

resource 'ALRT' (128, purgeable) {
	{40, 20, 174, 338},
	128,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	}
};

resource 'ALRT' (129, purgeable) {
	{40, 20, 150, 260},
	129,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	}
};

resource 'DITL' (128, purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{103, 223, 123, 303},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{8, 8, 24, 274},
		StaticText {
			disabled,
			"Gryphel Build System"
		},
		/* [3] */
		{32, 8, 48, 237},
		StaticText {
			disabled,
			"Copyright © 2009"
		},
		/* [4] */
		{53, 8, 90, 308},
		StaticText {
			disabled,
			"This program contains the work of many p"
			"eople. This version is maintained by:"
		},
		/* [5] */
		{94, 8, 110, 172},
		StaticText {
			disabled,
			"Paul C. Pratt"
		}
	}
};

resource 'DITL' (129, purgeable) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{80, 150, 100, 230},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 60, 60, 230},
		StaticText {
			disabled,
			"Error. ^0."
		},
		/* [3] */
		{8, 8, 40, 40},
		Icon {
			disabled,
			2
		}
	}
};

resource 'vers' (1, purgeable) {
	0x1,
	0x10,
	release,
	0x0,
	verUs,
	"1.1",
	"Copyright © 2009, maintained by Paul C. "
	"Pratt"
};

resource 'MBAR' (128, preload) {
	{	/* array MenuArray: 3 elements */
		/* [1] */
		128,
		/* [2] */
		129,
		/* [3] */
		130
	}
};

resource 'STR#' (128, purgeable) {
	{	/* array StringArray: 11 elements */
		/* [1] */
		"You must run on 512Ke or later",
		/* [2] */
		"Application Memory Size is too small",
		/* [3] */
		"Not enough memory to run TESample",
		/* [4] */
		"Not enough memory to do Cut",
		/* [5] */
		"Cannot do Cut",
		/* [6] */
		"Cannot do Copy",
		/* [7] */
		"Cannot exceed 32,000 characters with Pas"
		"te",
		/* [8] */
		"Not enough memory to do Paste",
		/* [9] */
		"Cannot create window",
		/* [10] */
		"Cannot exceed 32,000 characters",
		/* [11] */
		"Cannot do Paste"
	}
};

resource 'SIZE' (-1, purgeable) {
	dontSaveScreen,
	acceptSuspendResumeEvents,
	enableOptionSwitch,
	canBackground,
	multiFinderAware,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreChildDiedEvents,
	not32BitCompatible,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	1048576,
	1048576
};

resource 'BNDL' (128, purgeable) {
	'GrBl',
	0,
	{	/* array TypeArray: 2 elements */
		/* [1] */
		'FREF',
		{	/* array IDArray: 1 elements */
			/* [1] */
			0, 128
		},
		/* [2] */
		'ICN#',
		{	/* array IDArray: 1 elements */
			/* [1] */
			0, 128
		}
	}
};

resource 'FREF' (128, purgeable) {
	'APPL',
	0,
	""
};

resource 'ICN#' (128, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"00 01 00 00 00 02 80 00 00 04 40 00 00 08 20 00"
		$"00 10 10 00 00 20 08 00 00 40 04 00 00 87 C2 00"
		$"01 04 41 00 02 04 40 80 04 44 40 40 08 87 C0 20"
		$"11 20 00 10 22 40 00 08 44 10 3F 04 81 24 40 82"
		$"42 48 80 41 20 81 30 22 11 21 C8 14 08 4E 7F 8F"
		$"04 12 30 07 02 21 00 07 01 00 80 07 00 80 60 07"
		$"00 40 1F E7 00 20 02 1F 00 10 04 07 00 08 08 00"
		$"00 04 10 00 00 02 20 00 00 01 40 00 00 00 80",
		/* [2] */
		$"00 01 00 00 00 03 80 00 00 07 C0 00 00 0F E0 00"
		$"00 1F F0 00 00 3F F8 00 00 7F FC 00 00 FF FE 00"
		$"01 FF FF 00 03 FF FF 80 07 FF FF C0 0F FF FF E0"
		$"1F FF FF F0 3F FF FF F8 7F FF FF FC FF FF FF FE"
		$"7F FF FF FF 3F FF FF FE 1F FF FF FC 0F FF FF FF"
		$"07 FF FF FF 03 FF FF FF 01 FF FF FF 00 FF FF FF"
		$"00 7F FF FF 00 3F FE 1F 00 1F FC 07 00 0F F8 00"
		$"00 07 F0 00 00 03 E0 00 00 01 C0 00 00 00 80"
	}
};

data 'GrBl' (0, "Owner resource", purgeable) {
	$"14 47 72 79 70 68 65 6C 20 42 75 69 6C 64 20 53"    /* .Gryphel Build S */
	$"79 73 74 65 6D"                                     /* ystem */
};

data 'ics#' (128, purgeable) {
	$"00 00 01 00 02 80 04 40 0B A0 12 90 23 88 48 64"    /* .....Ä.@.†.ê#àHd */
	$"94 92 49 4E 26 F6 11 06 08 FE 04 46 02 80 01 00"    /* îíIN&......F.Ä.. */
	$"00 00 01 00 03 80 07 C0 0F E0 1F F0 3F F8 7F FC"    /* .....Ä.¿....?... */
	$"FF FE 7F FE 3F FE 1F FE 0F FE 07 C6 03 80 01 00"    /* ....?......∆.Ä.. */
};

