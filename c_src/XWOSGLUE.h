/*
	XWOSGLUE.h

	Copyright (C) 2004 Michael Hanni, Christian Bauer,
	Stephan Kochen, Paul C. Pratt, and others

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
	X Window system Operating System GLUE.

	All operating system dependent code for the
	X Window System should go here.

	This code is descended from Michael Hanni's X
	port of vMac, by Philip Cummins.
	I learned more about how X programs work by
	looking at other programs such as Basilisk II,
	the UAE Amiga Emulator, Bochs, QuakeForge,
	DooM Legacy, and the FLTK. A few snippets
	from them are used here.

	Drag and Drop support is based on the specification
	"XDND: Drag-and-Drop Protocol for the X Window System"
	developed by John Lindal at New Planet Software, and
	looking at included examples, one by Paul Sheer.

	ALSA sound support by Stephan Kochen.
*/

/*--- some simple utilities ---*/

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
	(void) memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

/*--- basic dialogs ---*/

GLOBALPROC MacMsg(char *briefMsg, char *longMsg, blnr fatal)
{
	UnusedParam(fatal);
	fprintf(stderr, briefMsg);
	fprintf(stderr, "\n");
	fprintf(stderr, longMsg);
	fprintf(stderr, "\n");
}

#define NotAfileRef NULL

LOCALVAR FILE *Drives[NumDrives]; /* open disk image files */

LOCALPROC InitDrives(void)
{
	si4b i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
	}
}

GLOBALFUNC si4b vSonyRead(void *Buffer, ui4b Drive_No, ui5b Sony_Start, ui5b *Sony_Count)
{
	fseek(Drives[Drive_No], Sony_Start, SEEK_SET);
	fread(Buffer, 1, *Sony_Count, Drives[Drive_No]);
	return 0; /*& figure out what really to return &*/
}

GLOBALFUNC si4b vSonyWrite(void *Buffer, ui4b Drive_No, ui5b Sony_Start, ui5b *Sony_Count)
{
	fseek(Drives[Drive_No], Sony_Start, SEEK_SET);
	fwrite(Buffer, 1, *Sony_Count, Drives[Drive_No]);
	return 0; /*& figure out what really to return &*/
}

GLOBALFUNC si4b vSonyGetSize(ui4b Drive_No, ui5b *Sony_Count)
{
	fseek(Drives[Drive_No], 0, SEEK_END);
	*Sony_Count = ftell(Drives[Drive_No]);
	return 0; /*& figure out what really to return &*/
}

GLOBALFUNC si4b vSonyEject(ui4b Drive_No)
{
	fclose(Drives[Drive_No]);
	Drives[Drive_No] = NotAfileRef;
	return 0x0000;
}

LOCALPROC Sony_Insert0(FILE *refnum, blnr locked)
{
	ui4b Drive_No;

	if (! FirstFreeDisk(&Drive_No)) {
		fclose(refnum);
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage, falseblnr);
	} else {
		/* printf("Sony_Insert0 %d\n", (int)Drive_No); */
		Drives[Drive_No] = refnum;
		vSonyInsertedMask |= ((ui5b)1 << Drive_No);
		if (! locked) {
			vSonyWritableMask |= ((ui5b)1 << Drive_No);
		}
	}
}

LOCALFUNC blnr Sony_Insert1(char *drivepath)
{
	blnr locked = falseblnr;
	/* printf("Sony_Insert1 %s\n", drivepath); */
	FILE *refnum = fopen(drivepath, "rb+");
	if (refnum == NULL) {
		locked = trueblnr;
		refnum = fopen(drivepath, "rb");
	}
	if (refnum == NULL) {
		/* fprintf(stderr, "'%s'could not be opened.\n", drivepath); */
	} else {
		Sony_Insert0(refnum, locked);
		return trueblnr;
	}
	return falseblnr;
}

LOCALFUNC blnr LoadInitialImages(void)
{
	int n = NumDrives > 9 ? 9 : NumDrives;
	int i;
	char s[] = "disk?.dsk";

	for (i = 1; i <= n; ++i) {
		s[4] = '0' + i;
		if (! Sony_Insert1(s)) {
			/* stop on first error (including file not found) */
			return trueblnr;
		}
	}

	return trueblnr;
}

LOCALPROC InsertADisk0(void)
{
	MacMsg("Not implemented.", "That commands is not yet implemented in this version.", trueblnr);
}

LOCALFUNC blnr AllocateMacROM(void)
{
	ROM = (ui4b *)malloc(kROM_Size);
	if (ROM == NULL) {
		MacMsg("Not enough Memory.", "Unable to allocate ROM.", trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALVAR char *rom_path = NULL;

LOCALFUNC blnr LoadMacRom(void)
{
	FILE *ROM_File;
	int  File_Size;
	blnr IsOk = falseblnr;

	if (rom_path == NULL) {
		rom_path = RomFileName;
	}
	ROM_File = fopen(rom_path, "rb");
	if (ROM_File == NULL) {
		fprintf(stderr, "The ROM image file could not be found.\n");
	} else {
		File_Size = fread(ROM, kTrueROM_Size, 1, ROM_File);
		IsOk = trueblnr;
		fclose(ROM_File);
	}
	return IsOk;
}

LOCALFUNC blnr AllocateMacRAM(void)
{
	RAM = (ui4b *)malloc(kRAM_Size + RAMSafetyMarginFudge);
	if (RAM == NULL) {
		MacMsg("Not enough Memory.", "Unable to allocate ROM.", trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}


/*--- time, date, location ---*/

LOCALVAR ui5b TrueEmulatedTime = 0;
LOCALVAR ui5b CurEmulatedTime = 0;

#include "DATE2SEC.h"

#define TicksPerSecond 1000000

LOCALVAR blnr HaveTimeDelta = falseblnr;
LOCALVAR ui5b TimeDelta;

LOCALVAR ui5b NewMacDateInSeconds;

LOCALVAR ui5b LastTimeSec;
LOCALVAR ui5b LastTimeUsec;

LOCALPROC GetCurrentTicks(void)
{
	struct timeval t;

	gettimeofday(&t, NULL);
	if (! HaveTimeDelta) {
		time_t Current_Time;
		struct tm *s;

		(void) time(&Current_Time);
		s = localtime(&Current_Time);
		TimeDelta = Date2MacSeconds(s->tm_sec, s->tm_min, s->tm_hour,
			s->tm_mday, 1 + s->tm_mon, 1900 + s->tm_year) - t.tv_sec;
#if 0 /* how portable is this ? */
		CurMacDelta = ((ui5b)(s->tm_gmtoff) & 0x00FFFFFF)
			| ((s->tm_isdst ? 0x80 : 0) << 24);
#endif
		HaveTimeDelta = trueblnr;
	}

	NewMacDateInSeconds = t.tv_sec + TimeDelta;
	LastTimeSec = (ui5b)t.tv_sec;
	LastTimeUsec = (ui5b)t.tv_usec;
}

#define MyInvTimeStep 16626 /* TicksPerSecond / 60.14742 */

LOCALVAR ui5b NextTimeSec;
LOCALVAR ui5b NextTimeUsec;

LOCALPROC IncrNextTime(void)
{
	NextTimeUsec += MyInvTimeStep;
	if (NextTimeUsec >= TicksPerSecond) {
		NextTimeUsec -= TicksPerSecond;
		NextTimeSec += 1;
	}
}

LOCALPROC InitNextTime(void)
{
	NextTimeSec = LastTimeSec;
	NextTimeUsec = LastTimeUsec;
	IncrNextTime();
}

LOCALPROC StartUpTimeAdjust(void)
{
	GetCurrentTicks();
	InitNextTime();
}

LOCALFUNC si5b GetTimeDiff(void)
{
	return ((si5b)(LastTimeSec - NextTimeSec)) * TicksPerSecond
		+ ((si5b)(LastTimeUsec - NextTimeUsec));
}

LOCALPROC UpdateTrueEmulatedTime(void)
{
	si5b TimeDiff;

	GetCurrentTicks();

	TimeDiff = GetTimeDiff();
	if (TimeDiff >= 0) {
		if (TimeDiff > 4 * MyInvTimeStep) {
			/* emulation interrupted, forget it */
			++TrueEmulatedTime;
			InitNextTime();
		} else {
			do {
				++TrueEmulatedTime;
				IncrNextTime();
				TimeDiff -= TicksPerSecond;
			} while (TimeDiff >= 0);
		}
	} else if (TimeDiff < - 2 * MyInvTimeStep) {
		/* clock goofed if ever get here, reset */
		InitNextTime();
	}
}

LOCALFUNC blnr CheckDateTime(void)
{
	if (CurMacDateInSeconds != NewMacDateInSeconds) {
		CurMacDateInSeconds = NewMacDateInSeconds;
		return trueblnr;
	} else {
		return falseblnr;
	}
}

LOCALVAR int my_argc;
LOCALVAR char **my_argv;

LOCALVAR char *display_name = NULL;
#if MySoundEnabled
LOCALVAR char *alsadev_name = NULL;
#endif

LOCALFUNC blnr ScanCommandLine(void)
{
	int i;

	for (i = 1; i < my_argc; i++) {
		if (my_argv[i][0] == '-') {
			if ((strcmp(my_argv[i], "--display") == 0) ||
				(strcmp(my_argv[i], "-display") == 0))
			{
				i++;
				if (i < my_argc) {
					display_name = my_argv[i];
				}
			} else if ((strcmp(my_argv[i], "--rom") == 0) ||
				(strcmp(my_argv[i], "-r") == 0))
			{
				i++;
				if (i < my_argc) {
					rom_path = my_argv[i];
				}
			} else
#if MySoundEnabled
			if ((strcmp(my_argv[i], "--alsadev") == 0) ||
				(strcmp(my_argv[i], "-alsadev") == 0))
			{
				i++;
				if (i < my_argc) {
					alsadev_name = my_argv[i];
				}
			} else
#endif
			if (strcmp(my_argv[i], "-l") == 0) {
				SpeedValue = 0;
			} else {
				printf("%s, Copyright %s.\n", kAppVariationStr, kStrCopyrightYear);
				printf("%s contains the work of many people.\n", kStrAppName);
				printf("This version is maintained by:\n");
				printf("%s\n", kMaintainerName);
				printf("For more information, see:\n");
				printf("   %s\n", kStrHomePage);
				printf("%s is distributed under the terms\n", kStrAppName);
				printf("of the GNU Public License, version 2.\n");
				printf("%s is distributed in the hope that it\n",  kStrAppName);
				printf("will be useful, but WITHOUT ANY WARRANTY;\n");
				printf("without even the implied warranty of\n");
				printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR\n");
				printf("PURPOSE.\n");
				printf("Usage: minivmac [diskimagefile]...\n");
				printf("       [-r romfile] [-l] [-display displayname]");
#if MySoundEnabled
				printf(" [-alsadev devicename]");
#endif
				printf("\n");
				return falseblnr;
			}
		} else {
			(void) Sony_Insert1(my_argv[i]);
		}
	}
	return trueblnr;
}

#if EnableFullScreen
LOCALVAR blnr UseFullScreen = falseblnr;
#endif

#if EnableMagnify
LOCALVAR blnr UseMagnify = falseblnr;
#endif

#define MyWindowScale 2

LOCALVAR Display *x_display = NULL;
LOCALVAR XColor x_black;
LOCALVAR XColor x_white;
LOCALVAR XImage *my_image = NULL;
LOCALVAR Cursor blankCursor = None;

#define EnableScalingBuff (1 && EnableMagnify && (MyWindowScale == 2))

#if EnableScalingBuff
LOCALVAR char *ScalingBuff = nullpr;
LOCALVAR XImage *my_scaled_image = NULL;
#endif

LOCALVAR Atom MyXA_DeleteW = (Atom)0;
#if EnableDragDrop
LOCALVAR Atom MyXA_UriList = (Atom)0;
LOCALVAR Atom MyXA_DndAware = (Atom)0;
LOCALVAR Atom MyXA_DndEnter = (Atom)0;
LOCALVAR Atom MyXA_DndLeave = (Atom)0;
LOCALVAR Atom MyXA_DndDrop = (Atom)0;
LOCALVAR Atom MyXA_DndPosition = (Atom)0;
LOCALVAR Atom MyXA_DndStatus = (Atom)0;
LOCALVAR Atom MyXA_DndActionCopy = (Atom)0;
LOCALVAR Atom MyXA_DndActionPrivate = (Atom)0;
LOCALVAR Atom MyXA_DndSelection = (Atom)0;
LOCALVAR Atom MyXA_DndFinished = (Atom)0;
LOCALVAR Atom MyXA_MinivMac_DndXchng = (Atom)0;
#endif

LOCALFUNC blnr CreateMyBlankCursor(Window rootwin)
/*
	adapted from X11_CreateNullCursor in context.x11.c
	in quakeforge 0.5.5, copyright Id Software, Inc.
	Zephaniah E. Hull, and Jeff Teunissen.
*/
{
	Pixmap cursormask;
	XGCValues xgc;
	GC gc;
	blnr IsOk = falseblnr;

	cursormask = XCreatePixmap(x_display, rootwin, 1, 1, 1);
	if (cursormask == None) {
		fprintf(stderr, "XCreatePixmap failed.\n");
	} else {
		xgc.function = GXclear;
		gc = XCreateGC(x_display, cursormask, GCFunction, &xgc);
		if (gc == None) {
			fprintf(stderr, "XCreateGC failed.\n");
		} else {
			XFillRectangle(x_display, cursormask, gc, 0, 0, 1, 1);
			XFreeGC(x_display, gc);

			blankCursor = XCreatePixmapCursor(x_display, cursormask,
							cursormask, &x_black, &x_white, 0, 0);


			if (blankCursor == None) {
				fprintf(stderr, "XCreatePixmapCursor failed.\n");
			} else {
				IsOk = trueblnr;
			}
		}

		XFreePixmap(x_display, cursormask);
		/*
			assuming that XCreatePixmapCursor doesn't think it
			owns the pixmaps passed to it. I've seen code that
			assumes this, and other code that seems to assume
			the opposite.
		*/
	}
	return IsOk;
}

LOCALFUNC blnr Screen_Init(void)
{
	Window rootwin;
	int screen;
	Colormap Xcmap;
	Visual *Xvisual;
	char *image_Mem1;

	x_display = XOpenDisplay(display_name);
	if (x_display == NULL) {
		fprintf(stderr, "Cannot connect to X server.\n");
		return falseblnr;
	}

	screen = DefaultScreen(x_display);

	rootwin = XRootWindow(x_display, screen);

	Xcmap = DefaultColormap(x_display, screen);

	Xvisual = DefaultVisual(x_display, screen);

	MyXA_DeleteW = XInternAtom(x_display, "WM_DELETE_WINDOW", False);
#if EnableDragDrop
	MyXA_UriList = XInternAtom (x_display, "text/uri-list", False);
	MyXA_DndAware = XInternAtom (x_display, "XdndAware", False);
	MyXA_DndEnter = XInternAtom(x_display, "XdndEnter", False);
	MyXA_DndLeave = XInternAtom(x_display, "XdndLeave", False);
	MyXA_DndDrop = XInternAtom(x_display, "XdndDrop", False);
	MyXA_DndPosition = XInternAtom(x_display, "XdndPosition", False);
	MyXA_DndStatus = XInternAtom(x_display, "XdndStatus", False);
	MyXA_DndActionCopy = XInternAtom(x_display, "XdndActionCopy", False);
	MyXA_DndActionPrivate = XInternAtom(x_display, "XdndActionPrivate", False);
	MyXA_DndSelection = XInternAtom(x_display, "XdndSelection", False);
	MyXA_DndFinished = XInternAtom(x_display, "XdndFinished", False);
	MyXA_MinivMac_DndXchng = XInternAtom(x_display, "_MinivMac_DndXchng", False);
#endif

	XParseColor(x_display, Xcmap, "#000000", &x_black);
	if (! XAllocColor(x_display, Xcmap, &x_black)) {
		fprintf(stderr, "Whoops??\n");
	}
	XParseColor(x_display, Xcmap, "#ffffff", &x_white);
	if (! XAllocColor(x_display, Xcmap, &x_white)) {
		fprintf(stderr, "Whoops??\n");
	}

	if (! CreateMyBlankCursor(rootwin)) {
		return falseblnr;
	}

	image_Mem1 = (char *)calloc(1, vMacScreenNumBytes);
	if (image_Mem1 == NULL) {
		fprintf(stderr, "calloc failed.\n");
		return falseblnr;
	}

	my_image = XCreateImage(x_display, Xvisual, 1, XYBitmap, 0,
		(char *)image_Mem1,
		vMacScreenWidth, vMacScreenHeight, 32, vMacScreenByteWidth);
	if (my_image == NULL) {
		fprintf(stderr, "XCreateImage failed.\n");
		return falseblnr;
	}

	my_image->bitmap_bit_order = MSBFirst;
	my_image->byte_order = MSBFirst;
	screencomparebuff = my_image->data;

#if EnableScalingBuff
	image_Mem1 = (char *)malloc((long)vMacScreenNumBytes * MyWindowScale * MyWindowScale);
	if (image_Mem1 == NULL) {
		fprintf(stderr, "malloc failed.\n");
		return falseblnr;
	}

	my_scaled_image = XCreateImage(x_display, Xvisual, 1, XYBitmap, 0,
		(char *)image_Mem1,
		vMacScreenWidth * MyWindowScale, vMacScreenHeight * MyWindowScale, 32, vMacScreenByteWidth * MyWindowScale);
	if (my_scaled_image == NULL) {
		fprintf(stderr, "XCreateImage failed.\n");
		return falseblnr;
	}

	my_scaled_image->bitmap_bit_order = MSBFirst;
	my_scaled_image->byte_order = MSBFirst;
	ScalingBuff = my_scaled_image->data;
#endif

#if UseControlKeys
	CntrlDisplayBuff = malloc(vMacScreenNumBytes);
	if (CntrlDisplayBuff == NULL) {
		MacMsg("Not enough memory", "There is not enough memory available to allocate the CntrlDisplayBuff.", trueblnr);
		return falseblnr;
	}
#endif

	return trueblnr;
}

#if EnableFullScreen
LOCALVAR short hOffset;
LOCALVAR short vOffset;
#endif

LOCALVAR Window my_main_wind = 0;
LOCALVAR GC my_gc = NULL;

#if EnableMagnify
#define MyScaledHeight (MyWindowScale * vMacScreenHeight)
#define MyScaledWidth (MyWindowScale * vMacScreenWidth)
#endif

GLOBALPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right)
{
	int XDest = left;
	int YDest = top;
#if EnableMagnify
	if (UseMagnify) {
		XDest *= MyWindowScale;
		YDest *= MyWindowScale;
	}
#endif
#if EnableFullScreen
	if (UseFullScreen) {
		XDest += hOffset;
		YDest += vOffset;
	}
#endif

#if EnableScalingBuff
	if (UseMagnify) {
#if 1
		int i;
		int j;
		int k;
		ui3b *p1 = (ui3b *)screencomparebuff + vMacScreenWidth / 8 * top;
		ui3b *p2 = (ui3b *)ScalingBuff + MyWindowScale * MyWindowScale * vMacScreenWidth / 8 * top;
		ui3b *p3;
		ui3b t0;
		ui3b t1;
		ui3b t2;
		ui3b m;

		for (i = bottom - top; --i >= 0; ) {
			p3 = p2;
			for (j = vMacScreenWidth / 8; --j >= 0; ) {
				t0 = *p1++;
				t1 = t0;
				m = 0x80;
				t2 = 0;
				for (k = 4; --k >= 0; ) {
					t2 |= t1 & m;
					t1 >>= 1;
					m >>= 2;
				}
				*p2++ = t2 | (t2 >> 1);

				t1 = t0 << 4;
				m = 0x80;
				t2 = 0;
				for (k = 4; --k >= 0; ) {
					t2 |= t1 & m;
					t1 >>= 1;
					m >>= 2;
				}
				*p2++ = t2 | (t2 >> 1);
			}
			for (j = MyScaledWidth / 8; --j >= 0; ) {
				*p2++ = *p3++;
			}
		}
#endif

		XPutImage(x_display, my_main_wind, my_gc, my_scaled_image,
			left * MyWindowScale, top * MyWindowScale,
			XDest, YDest,
			(right - left) * MyWindowScale,
			(bottom - top) * MyWindowScale);
	} else
#endif
	{
		XPutImage(x_display, my_main_wind, my_gc, my_image,
			left, top, XDest, YDest,
			right - left, bottom - top);
	}
}

LOCALPROC DisposeMainWindow(void)
{
	if (my_gc != NULL) {
		XFreeGC(x_display, my_gc);
	}
	if (my_main_wind) {
		XDestroyWindow(x_display, my_main_wind);
	}
}

#if EnableFullScreen
FORWARDPROC GrabTheMachine(void);
FORWARDPROC UnGrabTheMachine(void);
#endif

FORWARDPROC InitKeyCodes(void);

enum {
	kMagStateNormal,
#if EnableMagnify
	kMagStateMagnifgy,
#endif
	kNumMagStates
};

#define kMagStateAuto kNumMagStates

LOCALVAR int CurWinIndx;
LOCALVAR blnr HavePositionWins[kNumMagStates];
LOCALVAR int WinPositionWinsH[kNumMagStates];
LOCALVAR int WinPositionWinsV[kNumMagStates];
LOCALVAR int SavedTransH;
LOCALVAR int SavedTransV;

LOCALFUNC blnr ReCreateMainWindow(void)
{
	Window rootwin;
	int screen;
	int xr;
	int yr;
	unsigned int dr;
	unsigned int wr;
	unsigned int hr;
	unsigned int bwr;
	Window rr;
	int leftPos;
	int topPos;
	int WinIndx;
	Window NewMainWindow;
	GC new_gc = NULL;
#if EnableDragDrop
	long int xdnd_version = 5;
#endif
	int NewWindowHeight = vMacScreenHeight;
	int NewWindowWidth = vMacScreenWidth;
	Window OldMainWindow = my_main_wind;

	/* Get connection to X Server */
	screen = DefaultScreen(x_display);

	rootwin = XRootWindow(x_display, screen);

#if EnableFullScreen
	if (! UseFullScreen)
#endif
	{
		/* save old position */
		if (OldMainWindow) {

			/*
				Couldn't reliably find out where window
				is now, due to what seem to be some
				broken X implementations, and so instead
				track how far window has moved.
			*/
			XTranslateCoordinates(x_display, OldMainWindow, rootwin,
				0, 0, &xr, &yr, &rr);
			WinPositionWinsH[CurWinIndx] += (xr - SavedTransH);
			WinPositionWinsV[CurWinIndx] += (yr - SavedTransV);
			SavedTransH = xr;
			SavedTransV = yr;
		}
	}

#if EnableMagnify
	if (WantMagnify) {
		NewWindowHeight *= MyWindowScale;
		NewWindowWidth *= MyWindowScale;
	}
#endif

	XGetGeometry(x_display, rootwin,
		&rr, &xr, &yr, &wr, &hr, &bwr, &dr);

	if (wr > NewWindowWidth) {
		leftPos = (wr - NewWindowWidth) / 2;
	} else {
		leftPos = 0;
	}
	if (hr > NewWindowHeight) {
		topPos = (hr - NewWindowHeight) / 2;
	} else {
		topPos = 0;
	}

#if EnableFullScreen
	if (! WantFullScreen)
#endif
	{
#if EnableMagnify
		if (WantMagnify) {
			WinIndx = kMagStateMagnifgy;
		} else
#endif
		{
			WinIndx = kMagStateNormal;
		}

		if (! HavePositionWins[WinIndx]) {
			WinPositionWinsH[WinIndx] = leftPos;
			WinPositionWinsV[WinIndx] = topPos;
			HavePositionWins[WinIndx] = trueblnr;
		} else {
			leftPos = WinPositionWinsH[WinIndx];
			topPos = WinPositionWinsV[WinIndx];
		}
	}

	if ((! OldMainWindow)
#if EnableFullScreen
		|| (WantFullScreen != UseFullScreen)
#endif
		)
	{
#if EnableFullScreen
		if (WantFullScreen) {
			XSetWindowAttributes xattr;
			xattr.override_redirect = True;
			xattr.background_pixel = x_black.pixel;
			xattr.border_pixel = x_white.pixel;

			NewMainWindow = XCreateWindow(x_display, rr,
				0, 0, wr, hr, 0,
				CopyFromParent, /* depth */
				InputOutput,  /* class */
				CopyFromParent, /* visual */
				CWOverrideRedirect | CWBackPixel | CWBorderPixel, /* valuemask */
				&xattr /* attributes */);
		} else
#endif
		{
			NewMainWindow = XCreateSimpleWindow(x_display, rootwin,
				leftPos,
				topPos,
				NewWindowWidth, NewWindowHeight, 4,
				x_white.pixel,
				x_black.pixel);
		}
		if (! NewMainWindow) {
			fprintf(stderr, "XCreateSimpleWindow failed.\n");
			return falseblnr;
		} else {
			XSelectInput(x_display, NewMainWindow,
				ExposureMask | KeyPressMask | KeyReleaseMask |
				ButtonPressMask | ButtonReleaseMask |
				FocusChangeMask);

			XStoreName(x_display, NewMainWindow, kStrAppName);
			XSetIconName(x_display, NewMainWindow, kStrAppName);

			{
				XClassHint *hints = XAllocClassHint();
				if (hints) {
					hints->res_name = "minivmac";
					hints->res_class = "minivmac";
					XSetClassHint(x_display, NewMainWindow, hints);
					XFree(hints);
				}
			}

			{
				XWMHints *hints = XAllocWMHints();
				if (hints) {
					hints->input = True;
					hints->initial_state = NormalState;
					hints->flags = InputHint | StateHint;
					XSetWMHints(x_display, NewMainWindow, hints);
					XFree(hints);
				}

			}

			XSetCommand(x_display, NewMainWindow, my_argv, my_argc);

			/* let us handle a click on the close box */
			XSetWMProtocols(x_display, NewMainWindow, &MyXA_DeleteW, 1);

#if EnableDragDrop
			XChangeProperty (x_display, NewMainWindow, MyXA_DndAware,
				XA_ATOM, 32, PropModeReplace,
				(unsigned char *) &xdnd_version, 1);
#endif

			new_gc = XCreateGC(x_display, NewMainWindow, 0, NULL);
			if (new_gc == NULL) {
				fprintf(stderr, "XCreateGC failed.\n");
				XDestroyWindow(x_display, NewMainWindow);
				return falseblnr;
			}
			XSetState(x_display, new_gc, x_black.pixel, x_white.pixel, GXcopy, AllPlanes);
		}
	} else {
		NewMainWindow = OldMainWindow;
	}


#if EnableFullScreen
	if (! WantFullScreen)
#endif
	{
		XSizeHints *hints = XAllocSizeHints();
		if (hints) {
			hints->min_width = NewWindowWidth;
			hints->max_width = NewWindowWidth;
			hints->min_height = NewWindowHeight;
			hints->max_height = NewWindowHeight;
			hints->flags = PMinSize | PMaxSize;
			XSetWMNormalHints(x_display, NewMainWindow, hints);
			XFree(hints);
		}
	}

#if EnableFullScreen
	if (WantFullScreen) {
		hOffset = leftPos;
		vOffset = topPos;
	}
#endif

	my_main_wind = NewMainWindow;
#if EnableMagnify
	UseMagnify = WantMagnify;
#endif
#if EnableFullScreen
	UseFullScreen = WantFullScreen;
#endif

	if (NewMainWindow != OldMainWindow) {
		if (my_gc != NULL) {
			XFreeGC(x_display, my_gc);
		}
		my_gc = new_gc;

		XMapRaised(x_display, NewMainWindow);

#if EnableFullScreen
		if (! WantFullScreen)
#endif
		{
			if (OldMainWindow) {
				XRaiseWindow(x_display, OldMainWindow);
				/* try and hide the move */
			}

			XSync(x_display, 0);

			XMoveResizeWindow(x_display, NewMainWindow,
				leftPos, topPos, NewWindowWidth, NewWindowHeight);
				/*
					Needed after XMapRaised, because window
					managers will apparently ignore where the
					window was asked to be put.
				*/
		}

		if (OldMainWindow) {
			XDestroyWindow(x_display, OldMainWindow);
			/* XUnmapWindow(x_display, OldMainWindow); */
		}

		XSync(x_display, 0);

#if EnableFullScreen
		if (WantFullScreen) {
			XSetInputFocus(x_display, NewMainWindow,
				RevertToPointerRoot, CurrentTime);
		}
#endif

		InitKeyCodes();
			/* since will lose keystrokes to old window */
	} else {
#if EnableFullScreen
		if (! WantFullScreen)
#endif
		{
			XMoveResizeWindow(x_display, NewMainWindow,
				leftPos, topPos, NewWindowWidth, NewWindowHeight);
		}

		/*
			no API to invalidate window area?
			ok, just draw now.
		*/
		XFillRectangle(x_display, my_main_wind, my_gc,
			0, 0, wr, hr);
		HaveChangedScreenBuff(0, 0, vMacScreenHeight, vMacScreenWidth);
	}
	CurWinIndx = WinIndx;

#if EnableFullScreen
	if (! WantFullScreen)
#endif
	{
		XSync(x_display, 0);
			/*
				apparently, XTranslateCoordinates can be inaccurate
				without this
			*/
		XTranslateCoordinates(x_display, NewMainWindow, rootwin,
			0, 0, &SavedTransH, &SavedTransV, &rr);
	}

#if EnableFullScreen
	if (UseFullScreen) {
		GrabTheMachine();
	} else {
		UnGrabTheMachine();
	}
#endif

	return trueblnr;
}

LOCALVAR blnr HaveCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		if (my_main_wind) {
			XUndefineCursor(x_display, my_main_wind);
		}
	}
}

LOCALPROC CheckMagnifyAndFullScreen(void)
{
	if (0
#if EnableMagnify
		|| (UseMagnify != WantMagnify)
#endif
#if EnableFullScreen
		|| (UseFullScreen != WantFullScreen)
#endif
		)
	{
		ForceShowCursor(); /* hide/show cursor api is per window */
		(void) ReCreateMainWindow();
	}
}

#if EnableMagnify
GLOBALPROC ToggleWantMagnify(void)
{
	WantMagnify = ! WantMagnify;
}
#endif

#if EnableFullScreen && EnableMagnify
enum {
	kWinStateWindowed,
#if EnableMagnify
	kWinStateFullScreen,
#endif
	kNumWinStates
};
#endif

#if EnableFullScreen && EnableMagnify
LOCALVAR int WinMagStates[kNumWinStates];
#endif

#if EnableFullScreen
GLOBALPROC ToggleWantFullScreen(void)
{
	WantFullScreen = ! WantFullScreen;

#if EnableMagnify
	{
		int OldWinState = UseFullScreen ? kWinStateFullScreen : kWinStateWindowed;
		int OldMagState = UseMagnify ? kMagStateMagnifgy : kMagStateNormal;

		int NewWinState = WantFullScreen ? kWinStateFullScreen : kWinStateWindowed;
		int NewMagState = WinMagStates[NewWinState];
		WinMagStates[OldWinState] = OldMagState;
		if (kMagStateAuto != NewMagState) {
			WantMagnify = (NewMagState == kMagStateMagnifgy);
		} else {
			WantMagnify = falseblnr;
			if (WantFullScreen) {
				Window rootwin;
				int xr;
				int yr;
				unsigned int dr;
				unsigned int wr;
				unsigned int hr;
				unsigned int bwr;
				Window rr;

				rootwin = XRootWindow(x_display, DefaultScreen(x_display));
				XGetGeometry(x_display, rootwin,
					&rr, &xr, &yr, &wr, &hr, &bwr, &dr);
				if ((wr >= vMacScreenWidth * MyWindowScale)
					&& (hr >= vMacScreenHeight * MyWindowScale)
					)
				{
					WantMagnify = trueblnr;
				}
			}
		}
	}
#endif
}
#endif

LOCALPROC Keyboard_UpdateKeyMap(int key, int down)
{
	ui3b *kp = (ui3b *)theKeys;

	if (key >= 0 && key < 128) {
		int bit = 1 << (key & 7);
		if (down) {
			kp[key / 8] |= bit;
		} else {
			kp[key / 8] &= ~ bit;
		}
	}
}

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

LOCALVAR blnr CurTrueMouseButton = falseblnr;

LOCALVAR KeyCode TheCapsLockCode;

LOCALVAR si3b KC2MKC[256];

LOCALPROC KC2MKCAssignOne(KeySym ks, int key)
{
	KeyCode code = XKeysymToKeycode(x_display, ks);
	if (code != NoSymbol) {
		KC2MKC[code] = key;
	}
#if 0
	fprintf(stderr, "%d %d %d\n", (int)ks, key, (int)code);
#endif
}

LOCALFUNC blnr KC2MKCInit(void)
{
	int i;

	for (i = 0; i < 256; i++) {
		KC2MKC[i] = -1;
	}

	/*
	start with redundant mappings, should get overwritten
	by main mappings but define them just in case
	*/

#if 0 /* find Keysym for a code */
	for (i = 0; i < 64 * 1024; i++) {
		KeyCode code = XKeysymToKeycode(x_display, i);
		if (code == 115) {
			fprintf(stderr, "i %d\n", i);
		}
	}
#endif

#if defined(XK_KP_Prior) && defined(XK_KP_Left) && defined(XK_KP_Insert) && defined (XK_KP_End)
	KC2MKCAssignOne(XK_KP_Insert, MKC_KP0);
	KC2MKCAssignOne(XK_KP_End, MKC_KP1);
	KC2MKCAssignOne(XK_KP_Down, MKC_KP2);
	KC2MKCAssignOne(XK_KP_Next, MKC_KP3);
	KC2MKCAssignOne(XK_KP_Left, MKC_KP4);
	KC2MKCAssignOne(XK_KP_Begin, MKC_KP5);
	KC2MKCAssignOne(XK_KP_Right, MKC_KP6);
	KC2MKCAssignOne(XK_KP_Home, MKC_KP7);
	KC2MKCAssignOne(XK_KP_Up, MKC_KP8);
	KC2MKCAssignOne(XK_KP_Prior, MKC_KP9);
	KC2MKCAssignOne(XK_KP_Delete, MKC_Decimal);
#endif

	KC2MKCAssignOne(XK_asciitilde, MKC_Grave);
	KC2MKCAssignOne(XK_underscore, MKC_Minus);
	KC2MKCAssignOne(XK_plus, MKC_Equal);
	KC2MKCAssignOne(XK_braceleft, MKC_LeftBracket);
	KC2MKCAssignOne(XK_braceright, MKC_RightBracket);
	KC2MKCAssignOne(XK_bar, MKC_BackSlash);
	KC2MKCAssignOne(XK_colon, MKC_SemiColon);
	KC2MKCAssignOne(XK_quotedbl, MKC_SingleQuote);
	KC2MKCAssignOne(XK_less, MKC_Comma);
	KC2MKCAssignOne(XK_greater, MKC_Period);
	KC2MKCAssignOne(XK_question, MKC_Slash);

	KC2MKCAssignOne(XK_a, MKC_A);
	KC2MKCAssignOne(XK_b, MKC_B);
	KC2MKCAssignOne(XK_c, MKC_C);
	KC2MKCAssignOne(XK_d, MKC_D);
	KC2MKCAssignOne(XK_e, MKC_E);
	KC2MKCAssignOne(XK_f, MKC_F);
	KC2MKCAssignOne(XK_g, MKC_G);
	KC2MKCAssignOne(XK_h, MKC_H);
	KC2MKCAssignOne(XK_i, MKC_I);
	KC2MKCAssignOne(XK_j, MKC_J);
	KC2MKCAssignOne(XK_k, MKC_K);
	KC2MKCAssignOne(XK_l, MKC_L);
	KC2MKCAssignOne(XK_m, MKC_M);
	KC2MKCAssignOne(XK_n, MKC_N);
	KC2MKCAssignOne(XK_o, MKC_O);
	KC2MKCAssignOne(XK_p, MKC_P);
	KC2MKCAssignOne(XK_q, MKC_Q);
	KC2MKCAssignOne(XK_r, MKC_R);
	KC2MKCAssignOne(XK_s, MKC_S);
	KC2MKCAssignOne(XK_t, MKC_T);
	KC2MKCAssignOne(XK_u, MKC_U);
	KC2MKCAssignOne(XK_v, MKC_V);
	KC2MKCAssignOne(XK_w, MKC_W);
	KC2MKCAssignOne(XK_x, MKC_X);
	KC2MKCAssignOne(XK_y, MKC_Y);
	KC2MKCAssignOne(XK_z, MKC_Z);

	/*
	main mappings
	*/

	KC2MKCAssignOne(XK_F1, MKC_Option);
	KC2MKCAssignOne(XK_F2, MKC_Command);
	KC2MKCAssignOne(XK_F3, MKC_F3);
	KC2MKCAssignOne(XK_F4, MKC_F4);
	KC2MKCAssignOne(XK_F5, MKC_F5);
	KC2MKCAssignOne(XK_F6, MKC_F6);
	KC2MKCAssignOne(XK_F7, MKC_F7);
	KC2MKCAssignOne(XK_F8, MKC_F8);
	KC2MKCAssignOne(XK_F9, MKC_F9);
	KC2MKCAssignOne(XK_F10, MKC_F10);
	KC2MKCAssignOne(XK_F11, MKC_F11);
	KC2MKCAssignOne(XK_F12, MKC_F12);

#if 0
	KC2MKCAssignOne(XK_Delete, 0x75);
	KC2MKCAssignOne(XK_Insert, 0x72);
	KC2MKCAssignOne(XK_Home, 0x73);
	KC2MKCAssignOne(XK_Help, 0x73);
	KC2MKCAssignOne(XK_End, 0x77);
#ifdef __hpux
	KC2MKCAssignOne(XK_Prior, 0x74);
	KC2MKCAssignOne(XK_Next, 0x79);
#else
	KC2MKCAssignOne(XK_Page_Up, 0x74);
	KC2MKCAssignOne(XK_Page_Down, 0x79);
#endif
	KC2MKCAssignOne(XK_Print, 0x69);
	KC2MKCAssignOne(XK_Scroll_Lock, 0x6b);
	KC2MKCAssignOne(XK_Pause, 0x71);
#endif

	KC2MKCAssignOne(XK_KP_Add, MKC_KPAdd);
	KC2MKCAssignOne(XK_KP_Subtract, MKC_KPSubtract);
	KC2MKCAssignOne(XK_KP_Multiply, MKC_KPMultiply);
	KC2MKCAssignOne(XK_KP_Divide, MKC_KPDevide);
	KC2MKCAssignOne(XK_KP_Enter, MKC_Enter);
	KC2MKCAssignOne(XK_KP_Equal, MKC_KPEqual);

	KC2MKCAssignOne(XK_KP_0, MKC_KP0);
	KC2MKCAssignOne(XK_KP_1, MKC_KP1);
	KC2MKCAssignOne(XK_KP_2, MKC_KP2);
	KC2MKCAssignOne(XK_KP_3, MKC_KP3);
	KC2MKCAssignOne(XK_KP_4, MKC_KP4);
	KC2MKCAssignOne(XK_KP_5, MKC_KP5);
	KC2MKCAssignOne(XK_KP_6, MKC_KP6);
	KC2MKCAssignOne(XK_KP_7, MKC_KP7);
	KC2MKCAssignOne(XK_KP_8, MKC_KP8);
	KC2MKCAssignOne(XK_KP_9, MKC_KP9);
	KC2MKCAssignOne(XK_KP_Decimal, MKC_Decimal);

	KC2MKCAssignOne(XK_Left, MKC_Left);
	KC2MKCAssignOne(XK_Right, MKC_Right);
	KC2MKCAssignOne(XK_Up, MKC_Up);
	KC2MKCAssignOne(XK_Down, MKC_Down);

	KC2MKCAssignOne(XK_grave, MKC_Grave);
	KC2MKCAssignOne(XK_minus, MKC_Minus);
	KC2MKCAssignOne(XK_equal, MKC_Equal);
	KC2MKCAssignOne(XK_bracketleft, MKC_LeftBracket);
	KC2MKCAssignOne(XK_bracketright, MKC_RightBracket);
	KC2MKCAssignOne(XK_backslash, MKC_BackSlash);
	KC2MKCAssignOne(XK_semicolon, MKC_SemiColon);
	KC2MKCAssignOne(XK_apostrophe, MKC_SingleQuote);
	KC2MKCAssignOne(XK_comma, MKC_Comma);
	KC2MKCAssignOne(XK_period, MKC_Period);
	KC2MKCAssignOne(XK_slash, MKC_Slash);

	KC2MKCAssignOne(XK_Escape, MKC_Escape);

	KC2MKCAssignOne(XK_Tab, MKC_Tab);
	KC2MKCAssignOne(XK_Return, MKC_Return);
	KC2MKCAssignOne(XK_space, MKC_Space);
	KC2MKCAssignOne(XK_BackSpace, MKC_BackSpace);

	KC2MKCAssignOne(XK_Caps_Lock, MKC_CapsLock);
	KC2MKCAssignOne(XK_Num_Lock, MKC_Clear);

	KC2MKCAssignOne(XK_Meta_L, MKC_Command);
	KC2MKCAssignOne(XK_Meta_R, MKC_Command);

	KC2MKCAssignOne(XK_Mode_switch, MKC_Option);
	KC2MKCAssignOne(XK_Menu, MKC_Option);
	KC2MKCAssignOne(XK_Super_L, MKC_Option);
	KC2MKCAssignOne(XK_Super_R, MKC_Option);
	KC2MKCAssignOne(XK_Hyper_L, MKC_Option);
	KC2MKCAssignOne(XK_Hyper_R, MKC_Option);

	KC2MKCAssignOne(XK_F13, MKC_Option);
		/* seen being used in Mandrake Linux 9.2
		for windows key */

	KC2MKCAssignOne(XK_Shift_L, MKC_Shift);
	KC2MKCAssignOne(XK_Shift_R, MKC_Shift);
	KC2MKCAssignOne(XK_Alt_L, MKC_Command);
	KC2MKCAssignOne(XK_Alt_R, MKC_Command);
	KC2MKCAssignOne(XK_Control_L, MKC_Control);
	KC2MKCAssignOne(XK_Control_R, MKC_Control);

	KC2MKCAssignOne(XK_1, MKC_1);
	KC2MKCAssignOne(XK_2, MKC_2);
	KC2MKCAssignOne(XK_3, MKC_3);
	KC2MKCAssignOne(XK_4, MKC_4);
	KC2MKCAssignOne(XK_5, MKC_5);
	KC2MKCAssignOne(XK_6, MKC_6);
	KC2MKCAssignOne(XK_7, MKC_7);
	KC2MKCAssignOne(XK_8, MKC_8);
	KC2MKCAssignOne(XK_9, MKC_9);
	KC2MKCAssignOne(XK_0, MKC_0);

	KC2MKCAssignOne(XK_A, MKC_A);
	KC2MKCAssignOne(XK_B, MKC_B);
	KC2MKCAssignOne(XK_C, MKC_C);
	KC2MKCAssignOne(XK_D, MKC_D);
	KC2MKCAssignOne(XK_E, MKC_E);
	KC2MKCAssignOne(XK_F, MKC_F);
	KC2MKCAssignOne(XK_G, MKC_G);
	KC2MKCAssignOne(XK_H, MKC_H);
	KC2MKCAssignOne(XK_I, MKC_I);
	KC2MKCAssignOne(XK_J, MKC_J);
	KC2MKCAssignOne(XK_K, MKC_K);
	KC2MKCAssignOne(XK_L, MKC_L);
	KC2MKCAssignOne(XK_M, MKC_M);
	KC2MKCAssignOne(XK_N, MKC_N);
	KC2MKCAssignOne(XK_O, MKC_O);
	KC2MKCAssignOne(XK_P, MKC_P);
	KC2MKCAssignOne(XK_Q, MKC_Q);
	KC2MKCAssignOne(XK_R, MKC_R);
	KC2MKCAssignOne(XK_S, MKC_S);
	KC2MKCAssignOne(XK_T, MKC_T);
	KC2MKCAssignOne(XK_U, MKC_U);
	KC2MKCAssignOne(XK_V, MKC_V);
	KC2MKCAssignOne(XK_W, MKC_W);
	KC2MKCAssignOne(XK_X, MKC_X);
	KC2MKCAssignOne(XK_Y, MKC_Y);
	KC2MKCAssignOne(XK_Z, MKC_Z);

	TheCapsLockCode = XKeysymToKeycode(x_display, XK_Caps_Lock);

	return trueblnr;
}

LOCALPROC DoKeyCode(int i, blnr down)
{
	if (i >= 0 && i < 256) {
		int key = KC2MKC[i];
		if (key >= 0) {
			Keyboard_UpdateKeyMap(key, down);
		}
	}
}

LOCALPROC CheckTheCapsLock(void)
{
	int NewMousePosh;
	int NewMousePosv;
	int root_x_return;
	int root_y_return;
	Window root_return;
	Window child_return;
	unsigned int mask_return;

	XQueryPointer(x_display, my_main_wind,
		&root_return, &child_return,
		&root_x_return, &root_y_return,
		&NewMousePosh, &NewMousePosv,
		&mask_return);

	Keyboard_UpdateKeyMap(MKC_CapsLock, (mask_return & LockMask) != 0);
}

LOCALPROC InitKeyCodes(void)
{
	theKeys[0] = 0;
	theKeys[1] = 0;
	theKeys[2] = 0;
	theKeys[3] = 0;

	CheckTheCapsLock();
}

#if EnableFullScreen
LOCALVAR blnr KeyboardIsGrabbed = falseblnr;
#endif

#if EnableFullScreen
LOCALPROC MyGrabKeyboard(void)
{
	if (! KeyboardIsGrabbed) {
		(void) XGrabKeyboard(x_display, my_main_wind,
			False, GrabModeAsync, GrabModeAsync,
			CurrentTime);
		KeyboardIsGrabbed = trueblnr;
	}
}
#endif

#if EnableFullScreen
LOCALPROC MyUnGrabKeyboard(void)
{
	if (KeyboardIsGrabbed && my_main_wind) {
		XUngrabKeyboard(x_display, CurrentTime);
		KeyboardIsGrabbed = falseblnr;
	}
}
#endif

#if EnableMouseMotion
LOCALFUNC blnr MyMoveMouse(si4b h, si4b v)
{
	int NewMousePosh;
	int NewMousePosv;
	int root_x_return;
	int root_y_return;
	Window root_return;
	Window child_return;
	unsigned int mask_return;
	blnr IsOk;
	int attempts = 0;

#if EnableMagnify
	if (UseMagnify) {
		h *= MyWindowScale;
		v *= MyWindowScale;
	}
#endif
#if EnableFullScreen
	if (UseFullScreen) {
		h += hOffset;
		v += vOffset;
	}
#endif
	do {
		XWarpPointer(x_display, None, my_main_wind, 0, 0, 0, 0, h, v);
		XQueryPointer(x_display, my_main_wind,
			&root_return, &child_return,
			&root_x_return, &root_y_return,
			&NewMousePosh, &NewMousePosv,
			&mask_return);
		IsOk = (h == NewMousePosh) && (v == NewMousePosv);
		++attempts;
	} while ((! IsOk) && (attempts < 10));
	return IsOk;
}
#endif

#if EnableMouseMotion
LOCALVAR int SavedMouseH;
LOCALVAR int SavedMouseV;
#endif

#if EnableMouseMotion
LOCALPROC StartSaveMouseMotion(void)
{
	if (! HaveMouseMotion) {
		if (MyMoveMouse(vMacScreenWidth / 2, vMacScreenHeight / 2)) {
			SavedMouseH = vMacScreenWidth / 2;
			SavedMouseV = vMacScreenHeight / 2;
			HaveMouseMotion = trueblnr;
		}
	}
}
#endif

#if EnableMouseMotion
LOCALPROC StopSaveMouseMotion(void)
{
	if (HaveMouseMotion) {
		(void) MyMoveMouse(CurMouseH, CurMouseV);
		HaveMouseMotion = falseblnr;
	}
}
#endif

LOCALPROC CheckMouseState(void)
{
	blnr ShouldHaveCursorHidden;
	ui3b NewMouseButton;
	int NewMousePosh;
	int NewMousePosv;
	int root_x_return;
	int root_y_return;
	Window root_return;
	Window child_return;
	unsigned int mask_return;

	ShouldHaveCursorHidden = trueblnr;

	XQueryPointer(x_display, my_main_wind,
		&root_return, &child_return,
		&root_x_return, &root_y_return,
		&NewMousePosh, &NewMousePosv,
		&mask_return);

#if EnableFullScreen
	if (UseFullScreen) {
		NewMousePosh -= hOffset;
		NewMousePosv -= vOffset;
	}
#endif
#if EnableMagnify
	if (UseMagnify) {
		NewMousePosh /= MyWindowScale;
		NewMousePosv /= MyWindowScale;
	}
#endif
#if EnableMouseMotion
	if (HaveMouseMotion) {
		si4b shiftdh;
		si4b shiftdv;

		MouseMotionH += NewMousePosh - SavedMouseH;
		MouseMotionV += NewMousePosv - SavedMouseV;
		if (NewMousePosh < vMacScreenWidth / 4) {
			shiftdh = vMacScreenWidth / 2;
		} else if (NewMousePosh > vMacScreenWidth - vMacScreenWidth / 4) {
			shiftdh = - vMacScreenWidth / 2;
		} else {
			shiftdh = 0;
		}
		if (NewMousePosv < vMacScreenHeight / 4) {
			shiftdv = vMacScreenHeight / 2;
		} else if (NewMousePosv > vMacScreenHeight - vMacScreenHeight / 4) {
			shiftdv = - vMacScreenHeight / 2;
		} else {
			shiftdv = 0;
		}
		if ((shiftdh != 0) || (shiftdv != 0)) {
			NewMousePosh += shiftdh;
			NewMousePosv += shiftdv;
			if (! MyMoveMouse(NewMousePosh, NewMousePosv)) {
				HaveMouseMotion = falseblnr;
			}
		}
		SavedMouseH = NewMousePosh;
		SavedMouseV = NewMousePosv;
	} else
#endif
	{
		if (NewMousePosh < 0) {
			NewMousePosh = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePosh >= vMacScreenWidth) {
			NewMousePosh = vMacScreenWidth - 1;
			ShouldHaveCursorHidden = falseblnr;
		}
		if (NewMousePosv < 0) {
			NewMousePosv = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePosv >= vMacScreenHeight) {
			NewMousePosv = vMacScreenHeight - 1;
			ShouldHaveCursorHidden = falseblnr;
		}

#if EnableFullScreen
		if (UseFullScreen) {
			ShouldHaveCursorHidden = trueblnr;
		}
#endif

		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/* for a game like arkanoid, would like mouse to still
		move even when outside window in one direction */
		{
			CurMouseV = NewMousePosv;
			CurMouseH = NewMousePosh;
		}
	}

	NewMouseButton = ((mask_return &
		(Button1Mask | Button2Mask | Button3Mask))
		!= 0);

	if (CurTrueMouseButton != NewMouseButton) {
		CurTrueMouseButton = NewMouseButton;
		CurMouseButton = CurTrueMouseButton && ShouldHaveCursorHidden;
		/*
			CurMouseButton changes only when the button state changes.
			So if have mouse down outside our window, CurMouseButton will
			stay false even if mouse dragged back over our window.
			and if mouse down inside our window, CurMouseButton will
			stay true even if mouse dragged outside our window.
		*/
	}

	if (HaveCursorHidden != ShouldHaveCursorHidden) {
		HaveCursorHidden = ShouldHaveCursorHidden;
		if (HaveCursorHidden) {
			XDefineCursor(x_display, my_main_wind, blankCursor);
		} else {
			XUndefineCursor(x_display, my_main_wind);
		}
	}
}

/*--- sound ---*/

#if MySoundEnabled

#define SOUND_SAMPLERATE 22255 /* = round(7833600 * 2 / 704) */

#define kLn2SoundBuffers 4 /* kSoundBuffers must be a power of two */
#define kSoundBuffers (1 << kLn2SoundBuffers)
#define kSoundBuffMask (kSoundBuffers - 1)

#define DesiredMinFilledSoundBuffs 3
	/*
		if too big then sound lags behind emulation.
		if too small then sound will have pauses.
	*/

#define dbhBufferSize (kSoundBuffers * SOUND_LEN)

LOCALVAR snd_pcm_t *pcm_handle = NULL;
LOCALVAR snd_async_handler_t *pcm_ahandler;
LOCALVAR snd_pcm_uframes_t buffer_size;
LOCALVAR snd_pcm_uframes_t period_size;

LOCALVAR ui3p MyBuffer = NULL;

LOCALVAR ui4b CurPlayBuffer;
LOCALVAR ui4b CurFillBuffer;
LOCALVAR ui4b MinFilledSoundBuffs;

LOCALPROC FillWithSilence(ui3p p, int n)
{
	int i;

	for (i = n; --i >= 0; ) {
		*p++ = 0x80 /* 0 */;
	}
}

/* call back */
LOCALPROC MySound_CallBack(snd_async_handler_t *ahandler)
{
	if (pcm_handle != NULL) {
		snd_pcm_sframes_t avail;
		avail = snd_pcm_avail_update(pcm_handle);
		if (avail < 0) {
			avail = snd_pcm_prepare(pcm_handle);
			if (avail < 0) {
				fprintf(stderr, "pcm reset error: %s\n",
					snd_strerror(avail));
				return;
			}
			avail = snd_pcm_avail_update(pcm_handle);
			if (avail < 0) {
				fprintf(stderr, "pcm update error: %s\n",
					snd_strerror(avail));
				return;
			}
		}
		while (avail >= SOUND_LEN) {
			ui5b NextPlayOffset;
			ui4b CurPlayBuff = CurPlayBuffer;
			ui4b NextPlayBuffer = CurPlayBuff + 1;
			ui4b FilledSoundBuffs = CurFillBuffer - NextPlayBuffer;
			ui3p NextPlayPtr;
			int err;

			if (FilledSoundBuffs < MinFilledSoundBuffs) {
				MinFilledSoundBuffs = FilledSoundBuffs;
			}
			if (FilledSoundBuffs == 0) {
				/* out of sound to play. play a bit of silence */
				NextPlayOffset = (CurPlayBuff & kSoundBuffMask) * SOUND_LEN;
				NextPlayPtr = MyBuffer + NextPlayOffset;
				FillWithSilence(NextPlayPtr, SOUND_LEN);
			} else {
				NextPlayOffset = (NextPlayBuffer & kSoundBuffMask) * SOUND_LEN;
				CurPlayBuffer = NextPlayBuffer;
				NextPlayPtr = MyBuffer + NextPlayOffset;
			}

			if ((err = snd_pcm_writei (pcm_handle, NextPlayPtr, SOUND_LEN)) < 0) {
				fprintf(stderr, "pcm write error: %s\n",
					snd_strerror(err));
				return;
			}
			else if (err != SOUND_LEN) {
				fprintf(stderr, "pcm write error: written %i expected %i\n",
					err, SOUND_LEN);
				return;
			}

			avail = snd_pcm_avail_update(pcm_handle);
			if (avail < 0) {
				fprintf(stderr, "pcm update error: %s\n",
					snd_strerror(avail));
				return;
			}
		}
	}
}

LOCALPROC MySound_Start(void)
{
	if (pcm_handle == NULL) {
		snd_pcm_hw_params_t *hw_params = NULL;
		snd_pcm_sw_params_t *sw_params = NULL;
		unsigned int rrate = SOUND_SAMPLERATE;
		int err;

		buffer_size = dbhBufferSize;
		period_size = SOUND_LEN;

		/* Fill buffer with silence */
		FillWithSilence(MyBuffer, dbhBufferSize);

		/* Reset variables */
		CurPlayBuffer = 0;
		CurFillBuffer = kSoundBuffers - 1;
		MinFilledSoundBuffs = kSoundBuffers;

		pcm_ahandler = NULL;

		/* Open the sound device */
		if (alsadev_name == NULL) {
			alsadev_name = getenv("AUDIODEV");
			if (alsadev_name == NULL) {
				alsadev_name = strdup("default");
			}
		}

		if ((err = snd_pcm_open(&pcm_handle, alsadev_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)) < 0) {
			fprintf(stderr, "cannot open audio device %s (%s)\n",
				alsadev_name, snd_strerror(err));
			pcm_handle = NULL;
		} else
		/* Set some hardware parameters */
		if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
			fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
				snd_strerror(err));
			hw_params = NULL;
		} else
		if ((err = snd_pcm_hw_params_any(pcm_handle, hw_params)) < 0) {
			fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
				snd_strerror(err));
		} else
		if ((err = snd_pcm_hw_params_set_access(pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
			fprintf(stderr, "cannot set access type (%s)\n",
				snd_strerror(err));
		} else
		if ((err = snd_pcm_hw_params_set_format(pcm_handle, hw_params, SND_PCM_FORMAT_U8)) < 0) {
			fprintf(stderr, "cannot set sample format (%s)\n",
				snd_strerror(err));
		} else
		if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &rrate, NULL)) < 0) {
			fprintf(stderr, "cannot set sample rate (%s)\n",
				snd_strerror(err));
		} else
		if ((err = snd_pcm_hw_params_set_channels(pcm_handle, hw_params, 1)) < 0) {
			fprintf(stderr, "cannot set channel count (%s)\n",
				snd_strerror(err));
		} else
		if ((err = snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hw_params, &buffer_size)) < 0) {
			fprintf(stderr, "cannot set buffer size count (%s)\n",
				snd_strerror(err));
		} else
		if ((err = snd_pcm_hw_params_set_period_size_near(pcm_handle, hw_params, &period_size, NULL)) < 0) {
			fprintf(stderr, "cannot set period size count (%s)\n",
				snd_strerror(err));
		} else
		if ((err = snd_pcm_hw_params(pcm_handle, hw_params)) < 0) {
			fprintf(stderr, "cannot set parameters (%s)\n",
				snd_strerror(err));
		} else
		{
			if (rrate != SOUND_SAMPLERATE) {
				fprintf(stderr, "Warning: sample rate is off by %i Hz\n",
					SOUND_SAMPLERATE - rrate);
			}

			if (buffer_size != dbhBufferSize) {
				fprintf(stderr, "Warning: buffer size is off by %li\n",
					dbhBufferSize - buffer_size);
			}

			if (period_size != SOUND_LEN) {
				fprintf(stderr, "Warning: period size is off by %li\n",
					SOUND_LEN - period_size);
			}

			snd_pcm_hw_params_free(hw_params);
			hw_params = NULL;

			snd_pcm_nonblock(pcm_handle, 0);

			/* Set some software parameters */
			if ((err = snd_pcm_sw_params_malloc(&sw_params)) < 0) {
				fprintf(stderr, "cannot allocate software parameter structure (%s)\n",
					snd_strerror(err));
				sw_params = NULL;
			} else
			if ((err = snd_pcm_sw_params_current(pcm_handle, sw_params)) < 0) {
				fprintf(stderr, "Unable to determine current sw_params for playback: %s\n",
					snd_strerror(err));
			} else
			if ((snd_pcm_sw_params_set_start_threshold(pcm_handle, sw_params, buffer_size - period_size)) < 0) {
				fprintf(stderr, "Unable to set start threshold mode for playback: %s\n",
					snd_strerror(err));
			} else
			if ((err = snd_pcm_sw_params_set_avail_min(pcm_handle, sw_params, period_size)) < 0) {
				fprintf(stderr, "Unable to set avail min for playback: %s\n",
					snd_strerror(err));
			} else
			/* Setting this to SOUND_LEN sounds like a better idea,
				but it breaks occasionally (my onboard chip). */
			if ((err = snd_pcm_sw_params_set_xfer_align(pcm_handle, sw_params, 1)) < 0) {
				fprintf(stderr, "Unable to set transfer align for playback: %s\n",
					snd_strerror(err));
			} else
			if ((err = snd_pcm_sw_params(pcm_handle, sw_params)) < 0) {
				fprintf(stderr, "Unable to set sw params for playback: %s\n",
					snd_strerror(err));
			} else
			{
				snd_pcm_sw_params_free(sw_params);
				sw_params = NULL;

				/* Prepare */
				if ((err = snd_pcm_prepare(pcm_handle)) < 0) {
					fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
						snd_strerror(err));
				} else
				if ((err = snd_async_add_pcm_handler(&pcm_ahandler, pcm_handle, MySound_CallBack, NULL)) < 0) {
					fprintf(stderr, "unable to register async pcm handler (%s)\n",
						snd_strerror(err));
					pcm_ahandler = NULL;
				} else
				/* Fill alsa buffer with silence */
				if ((err = snd_pcm_writei(pcm_handle, MyBuffer, 2 * period_size)) < 0) {
					fprintf(stderr, "Initial pcm write error: %s\n", snd_strerror(err));
				} else
				if (err != 2 * period_size) {
					fprintf(stderr, "Initial pcm write error: written %i expected %li\n", err, 2 * period_size);
				} else
				/* Start playback */
				if ((err = snd_pcm_start(pcm_handle)) < 0) {
					fprintf(stderr, "pcm start error: %s\n",
						snd_strerror(err));
				} else
				{
					return; /* success */
				}
			}
		}

		/* clean up after failure */

		if (pcm_ahandler != NULL) {
			snd_async_del_handler(pcm_ahandler);
		}
		if (sw_params != NULL) {
			snd_pcm_sw_params_free(sw_params);
		}
		if (hw_params != NULL) {
			snd_pcm_hw_params_free(hw_params);
		}
		if (pcm_handle != NULL) {
			snd_pcm_close(pcm_handle);
			pcm_handle = NULL;
		}
	}
}

LOCALPROC MySound_Stop(void)
{
	if (pcm_handle != NULL) {
		snd_pcm_drop(pcm_handle);
		snd_async_del_handler(pcm_ahandler);
		snd_pcm_close(pcm_handle);
		pcm_handle = NULL;
	}
}

LOCALFUNC blnr MySound_Init(void)
{
	MyBuffer = (ui3p)malloc(dbhBufferSize);
	if (MyBuffer == NULL) {
		return falseblnr;
	}
	return trueblnr;
}

LOCALPROC MySound_UnInit(void)
{
	if (MyBuffer != NULL) {
		free(MyBuffer);
	}
}

GLOBALFUNC ui3p GetCurSoundOutBuff(void)
{
	if (pcm_handle == NULL) {
		return nullpr;
	} else {
		ui4b CurFillBuff = CurFillBuffer;
		ui4b NextFillBuffer = CurFillBuff + 1;
		ui4b FilledSoundBuffs = NextFillBuffer - CurPlayBuffer;

		if (FilledSoundBuffs < kSoundBuffers) {
			CurFillBuffer = NextFillBuffer;
		}

		return MyBuffer + (CurFillBuffer & kSoundBuffMask) * SOUND_LEN;
	}
}

LOCALPROC MySound_SecondNotify(void)
{
	if (pcm_handle != NULL) {
		if (MinFilledSoundBuffs > DesiredMinFilledSoundBuffs) {
			++CurEmulatedTime;
		} else if (MinFilledSoundBuffs < DesiredMinFilledSoundBuffs) {
			--CurEmulatedTime;
		}
		MinFilledSoundBuffs = kSoundBuffers;
	}
}

#endif

/*--- overall grab ---*/

#if EnableFullScreen
LOCALPROC GrabTheMachine(void)
{
#if EnableMouseMotion
	StartSaveMouseMotion();
#endif
	MyGrabKeyboard();
#if MySoundEnabled && MySoundFullScreenOnly
	MySound_Start();
#endif
}
#endif

#if EnableFullScreen
LOCALPROC UnGrabTheMachine(void)
{
#if EnableMouseMotion
	StopSaveMouseMotion();
#endif
	MyUnGrabKeyboard();
#if MySoundEnabled && MySoundFullScreenOnly
	MySound_Stop();
#endif
}
#endif

LOCALPROC LeaveBackground(void)
{
	InitKeyCodes();
}

LOCALPROC EnterBackground(void)
{
#if EnableFullScreen
	if (WantFullScreen) {
		ToggleWantFullScreen();
	}
#endif
	ForceShowCursor();
}

LOCALPROC LeaveSpeedStopped(void)
{
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Start();
#endif

	StartUpTimeAdjust();
}

LOCALPROC EnterSpeedStopped(void)
{
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
}

GLOBALFUNC blnr ExtraTimeNotOver(void)
{
	if (CurSpeedStopped) {
		return falseblnr;
	} else {
		UpdateTrueEmulatedTime();
		TimeAdjust = TrueEmulatedTime - CurEmulatedTime;
		return (TimeAdjust < 0);
	}
}

LOCALPROC WaitForEndOfSixtieth(void)
{
	while (ExtraTimeNotOver()) {
		struct timespec rqt;
		struct timespec rmt;

		si5b TimeDiff = GetTimeDiff();
		if (TimeDiff < 0) {
			rqt.tv_sec = 0;
			rqt.tv_nsec = (- TimeDiff) * 1000;
			(void) nanosleep(&rqt, &rmt);
		}
	}
}

LOCALPROC RunEmulatedTicksToTrueTime(void)
{
	if (! CurSpeedStopped) {
		UpdateTrueEmulatedTime();

		if (CheckDateTime()) {
#if MySoundEnabled
			MySound_SecondNotify();
#endif
		}

		TimeAdjust = TrueEmulatedTime - CurEmulatedTime;
		if (TimeAdjust >= 0) {
			ui5b InitialTime = TrueEmulatedTime;

			if (! gBackgroundFlag) {
				CheckMouseState();
			}

			do {
				DoEmulateOneTick();
				++CurEmulatedTime;

				UpdateTrueEmulatedTime();
				TimeAdjust = TrueEmulatedTime - CurEmulatedTime;
			} while ((TimeAdjust >= 0)
				&& (TrueEmulatedTime == InitialTime));

			if (TimeAdjust > 7) {
				/* emulation not fast enough */
				TimeAdjust = 7;
				CurEmulatedTime = TrueEmulatedTime - TimeAdjust;
			}
		}
	}
}

LOCALPROC DoMyLowPriorityTasks(void)
{
	if (gTrueBackgroundFlag != gBackgroundFlag) {
		gBackgroundFlag = gTrueBackgroundFlag;
		if (gTrueBackgroundFlag) {
			EnterBackground();
		} else {
			LeaveBackground();
		}
	}

	if (CurSpeedStopped != ( /* SpeedStopped || */
		(gBackgroundFlag && ! RunInBackground)))
	{
		CurSpeedStopped = ! CurSpeedStopped;
		if (CurSpeedStopped) {
			EnterSpeedStopped();
		} else {
			LeaveSpeedStopped();
		}
	}

#if EnableMagnify || EnableFullScreen
	CheckMagnifyAndFullScreen();
#endif

	if (RequestInsertDisk) {
		RequestInsertDisk = falseblnr;
		InsertADisk0();
	}

	DoEmulateExtraTime();
}

#if EnableDragDrop
LOCALPROC ParseOneUri(char *s)
{
	/* printf("ParseOneUri %s\n", s); */
	if ((s[0] == 'f') && (s[1] == 'i') && (s[2] == 'l')
		&& (s[3] == 'e') && (s[4] == ':'))
	{
		s += 5;
		if ((s[0] == '/') && (s[1] == '/')) {
			/* skip hostname */
			char c;

			s += 2;
			while ((c = *s) != '/') {
				if (c == 0) {
					return;
				}
				s++;
			}
		}
		(void) Sony_Insert1(s);
	}
}
#endif

#if EnableDragDrop
LOCALFUNC int HexChar2Nib(char x)
{
	if ((x >= '0') && (x <= '9')) {
		return x - '0';
	} else if ((x >= 'A') && (x <= 'F')) {
		return x - 'A' + 10;
	} else if ((x >= 'a') && (x <= 'f')) {
		return x - 'a' + 10;
	} else {
		return -1;
	}
}
#endif

#if EnableDragDrop
LOCALPROC ParseUriList(char *s)
{
	char *p1 = s;
	char *p0 = s;
	char *p = s;
	char c;

	/* printf("ParseUriList %s\n", s); */
	while ((c = *p++) != 0) {
		if (c == '%') {
			int a;
			int b;

			if (((a = HexChar2Nib(p[0])) >= 0) &&
				((b = HexChar2Nib(p[1])) >= 0))
			{
				p += 2;
				*p1++ = (a << 4) + b;
			} else {
				*p1++ = c;
			}
		} else if ((c == '\n') || (c == '\r')) {
			*p1++ = 0;
			ParseOneUri(p0);
			p0 = p1;
		} else {
			*p1++ = c;
		}
	}
	*p1++ = 0;
	ParseOneUri(p0);
}
#endif

#if EnableDragDrop
LOCALVAR Window PendingDragWindow = None;
#endif

LOCALPROC HandleTheEvent(XEvent *theEvent)
{
	switch(theEvent->type) {
		case KeyPress:
		case KeyRelease:
			{
				int keycode = theEvent->xkey.keycode;
				if (keycode == TheCapsLockCode) {
					CheckTheCapsLock();
				} else {
					DoKeyCode(keycode, theEvent->type == KeyPress);
				}
			}
			break;
		case Expose:
			{
				int x0 = ((XExposeEvent *)theEvent)->x;
				int y0 = ((XExposeEvent *)theEvent)->y;
				int x1 = x0 + ((XExposeEvent *)theEvent)->width;
				int y1 = y0 + ((XExposeEvent *)theEvent)->height;

#if EnableFullScreen
				if (UseFullScreen) {
					x0 -= hOffset;
					y0 -= vOffset;
					x1 -= hOffset;
					y1 -= vOffset;
				}
#endif
#if EnableMagnify
				if (UseMagnify) {
					x0 /= MyWindowScale;
					y0 /= MyWindowScale;
					x1 = (x1 + (MyWindowScale - 1)) / MyWindowScale;
					y1 = (y1 + (MyWindowScale - 1)) / MyWindowScale;
				}
#endif
				if (x0 < 0) {
					x0 = 0;
				}
				if (x1 > vMacScreenWidth)  {
					x1 = vMacScreenWidth;
				}
				if (y0 < 0) {
					y0 = 0;
				}
				if (y1 > vMacScreenHeight)  {
					y1 = vMacScreenHeight;
				}
				if ((x0 < x1) && (y0 < y1)) {
					HaveChangedScreenBuff(y0, x0, y1, x1);
				}
			}
			break;
#if EnableDragDrop
		case SelectionNotify:
			{
				blnr DropOk = falseblnr;
				/* printf("Got SelectionNotify\n"); */
				if ((theEvent->xselection.property == MyXA_MinivMac_DndXchng)
					&& (theEvent->xselection.requestor == my_main_wind)
					&& (theEvent->xselection.target ==  MyXA_UriList))
				{
					Atom ret_type;
					int ret_format;
					unsigned long ret_item;
					unsigned long remain_byte;
					char *s;

					XGetWindowProperty(x_display, my_main_wind,
						MyXA_MinivMac_DndXchng,
						0, 65536, False, MyXA_UriList, &ret_type, &ret_format,
						&ret_item, &remain_byte, (unsigned char **)&s);
					if (s != NULL) {
						ParseUriList(s);
						DropOk = trueblnr;
						XFree(s);
					}
				} else {
					fprintf(stderr, "Got Unknown SelectionNotify\n");
				}
				XDeleteProperty(x_display, my_main_wind, MyXA_MinivMac_DndXchng);
				if (PendingDragWindow != None) {
					XEvent xevent;

					memset (&xevent, 0, sizeof (xevent));

					xevent.xany.type = ClientMessage;
					xevent.xany.display = x_display;
					xevent.xclient.window = PendingDragWindow;
					xevent.xclient.message_type = MyXA_DndFinished;
					xevent.xclient.format = 32;

					xevent.xclient.data.l[0] = my_main_wind;
					if (DropOk) {
						xevent.xclient.data.l[1] = 1;
					}
					xevent.xclient.data.l[2] = MyXA_DndActionPrivate;

					XSendEvent (x_display, PendingDragWindow, 0, 0, &xevent);
				}
				if (DropOk) {
					XRaiseWindow(x_display, my_main_wind);
					XSetInputFocus(x_display, my_main_wind,
						RevertToPointerRoot, CurrentTime);
				}
			}
			break;
#endif
		case ClientMessage:
#if EnableDragDrop
			if (theEvent->xclient.message_type == MyXA_DndEnter) {
				/* printf("Got XdndEnter\n"); */
			} else if (theEvent->xclient.message_type == MyXA_DndLeave) {
				/* printf("Got XdndLeave\n"); */
			} else if (theEvent->xclient.message_type == MyXA_DndPosition) {
				XEvent xevent;
				int xr;
				int yr;
				unsigned int dr;
				unsigned int wr;
				unsigned int hr;
				unsigned int bwr;
				Window rr;
				Window srcwin = theEvent->xclient.data.l[0];

				/* printf("Got XdndPosition\n"); */

				XGetGeometry(x_display, my_main_wind,
					&rr, &xr, &yr, &wr, &hr, &bwr, &dr);
				memset (&xevent, 0, sizeof(xevent));
				xevent.xany.type = ClientMessage;
				xevent.xany.display = x_display;
				xevent.xclient.window = srcwin;
				xevent.xclient.message_type = MyXA_DndStatus;
				xevent.xclient.format = 32;

				xevent.xclient.data.l[0] = theEvent->xclient.window; /* Target Window */
				xevent.xclient.data.l[1] = 1; /* Accept */
				xevent.xclient.data.l[2] = ((xr) << 16) | ((yr) & 0xFFFFUL);
				xevent.xclient.data.l[3] = ((wr) << 16) | ((hr) & 0xFFFFUL);
				xevent.xclient.data.l[4] = MyXA_DndActionPrivate; /* Action */

				XSendEvent(x_display, srcwin, 0, 0, &xevent);
			} else if (theEvent->xclient.message_type == MyXA_DndDrop) {
				/* printf("Got XdndDrop\n"); */
				Time timestamp = theEvent->xclient.data.l[2];
				PendingDragWindow = (Window) theEvent->xclient.data.l[0];
				XConvertSelection(x_display, MyXA_DndSelection, MyXA_UriList,
					MyXA_MinivMac_DndXchng, my_main_wind, timestamp);
			} else
#endif
			{
#if 0
				char *name = XGetAtomName(x_display, theEvent->xclient.message_type);
				fprintf(stderr, "Get %s\n", name);
				XFree(name);
#endif

				if ((theEvent->xclient.format == 32) &&
					(theEvent->xclient.data.l[0] == MyXA_DeleteW))
				{
					/*
						I would think that should test that
							message_type == WM_PROTOCOLS
						but none of the other programs I looked
						at did.
					*/
					if (AnyDiskInserted()) {
						MacMsg("Please shut down first",
							"Please shut down the emulated machine before quitting. To force Mini vMac to quit, at the risk of corrupting the mounted disk image files, press and hold down the 'control' key, and type the letter 'Q' (after closing this message).",
							falseblnr);
					} else {
						ForceMacOff = trueblnr;
					}
				}
			}
			break;
		case FocusIn:
			gTrueBackgroundFlag = falseblnr;
			break;
		case FocusOut:
			gTrueBackgroundFlag = trueblnr;
			break;
		default:
			break;
	}
}

LOCALPROC CheckForSystemEvents(void)
{
	XEvent event;

	if (CurSpeedStopped || (XEventsQueued(x_display, QueuedAfterFlush) > 0)) {
		int i = 0;

		do {
			XNextEvent(x_display, &event);
			HandleTheEvent(&event);
			i++;
		} while ((XEventsQueued(x_display, QueuedAfterReading) > 0) && (i < 10));
	}
}

LOCALPROC MainEventLoop(void)
{
	do {
		CheckForSystemEvents();
		RunEmulatedTicksToTrueTime();
		DoMyLowPriorityTasks();
		WaitForEndOfSixtieth();
	} while (! ForceMacOff);
}

LOCALPROC ZapOSGLUVars(void)
{
	ROM = NULL;
	RAM = NULL;
#if UseControlKeys
	CntrlDisplayBuff = NULL;
#endif
	InitDrives();
	{
		int i;

		for (i = 0; i < kNumMagStates; i++) {
			HavePositionWins[i] = falseblnr;
		}
#if EnableFullScreen && EnableMagnify
		for (i = 0; i < kNumWinStates; i++) {
			WinMagStates[i] = kMagStateAuto;
		}
#endif
	}
}

LOCALFUNC blnr InitOSGLU(void)
{
	if (LoadInitialImages())
	if (ScanCommandLine())
	if (AllocateMacROM())
	if (LoadMacRom())
#if MySoundEnabled
	if (MySound_Init())
#endif
	if (Screen_Init())
	if (ReCreateMainWindow())
	if (KC2MKCInit())
	if (AllocateMacRAM())
	if (InitEmulation())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
#if EnableFullScreen
	UnGrabTheMachine();
#endif
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
#if MySoundEnabled
	MySound_UnInit();
#endif

	ForceShowCursor();
	if (blankCursor != None) {
		XFreeCursor(x_display, blankCursor);
	}
	if (my_image != NULL) {
		XDestroyImage(my_image);
	}
#if EnableScalingBuff
	if (my_scaled_image != NULL) {
		XDestroyImage(my_scaled_image);
	}
#endif
	DisposeMainWindow();
	if (x_display != NULL) {
		XCloseDisplay(x_display);
	}
#if UseControlKeys
	if (CntrlDisplayBuff != NULL) {
		free((char *)CntrlDisplayBuff);
	}
#endif
	if (RAM != NULL) {
		free((char *)RAM);
	}
	if (ROM != NULL) {
		free((char *)ROM);
	}
}

int main(int argc, char **argv)
{
	my_argc = argc;
	my_argv = argv;

	ZapOSGLUVars();
	if (InitOSGLU()) {
		MainEventLoop();
	}
	UnInitOSGLU();
	return 0;
}
