/*
	XWOSGLUE.h

	Copyright (C) 2002 Michael Hanni, Christian Bauer,
	Paul Pratt, and others

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
*/

/*
	to do:
		how to get icon in window title bar
		how to get path of application
			so can find rom file
		update theKeys better
		for menus and dialogs
			probably need to use gnome or kde
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

/*--- some simple utilities ---*/

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, LONG byteCount)
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

GLOBALFUNC blnr OkCancelAlert(char *briefMsg, char *longMsg)
{
	UnusedParam(briefMsg);
	UnusedParam(longMsg);
	return trueblnr;
}

#define NotAfileRef NULL

LOCALVAR FILE *Drives[NumDrives]; /* open disk image files */

LOCALPROC InitDrives(void)
{
	WORD i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
	}
}

GLOBALFUNC WORD vSonyRead(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count)
{
	WORD result;

	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			fseek(Drives[Drive_No], Sony_Start, SEEK_SET);
			fread(Buffer, 1, *Sony_Count, Drives[Drive_No]);
			result = 0; /*& figure out what really to return &*/
		} else {
			result = 0xFFBF; // Say it's offline (-65)
		}
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC WORD vSonyWrite(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count)
{
	WORD result;

	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			fseek(Drives[Drive_No], Sony_Start, SEEK_SET);
			fwrite(Buffer, 1, *Sony_Count, Drives[Drive_No]);
			result = 0; /*& figure out what really to return &*/
		} else {
			result = 0xFFBF; // Say it's offline (-65)
		}
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC blnr vSonyDiskLocked(UWORD Drive_No)
{
	UnusedParam(Drive_No);
	return falseblnr;
}

GLOBALFUNC WORD vSonyGetSize(UWORD Drive_No, ULONG *Sony_Count)
{
	WORD result;

	UnusedParam(Sony_Count);
	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			result = -1; /*& figure out what really to return &*/
		} else {
			result = 0xFFBF; // Say it's offline (-65)
		}
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC WORD vSonyEject(UWORD Drive_No)
{
	WORD result;

	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			fclose(Drives[Drive_No]);
			Drives[Drive_No] = NotAfileRef;
		}
		result = 0x0000;
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC WORD vSonyVerify(UWORD Drive_No)
{
	WORD result;

	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			result = 0x0000; // No Error (0)
		} else {
			result = 0xFFBF; // Say it's offline (-65)
		}
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC WORD vSonyFormat(UWORD Drive_No)
{
	WORD result;

	if (Drive_No < NumDrives) {
		if (Drives[Drive_No] != NotAfileRef) {
			result = 0xFFD4; // Write Protected (-44)
		} else {
			result = 0xFFBF; // Say it's offline (-65)
		}
	} else {
		result = 0xFFC8; // No Such Drive (-56)
	}
	return result;
}

GLOBALFUNC blnr vSonyInserted (UWORD Drive_No)
{
	if (Drive_No >= NumDrives) {
		return falseblnr;
	} else {
		return (Drives[Drive_No] != NotAfileRef);
	}
}

LOCALFUNC blnr FirstFreeDisk(UWORD *Drive_No)
{
	WORD i;

	for (i = 0; i < NumDrives; ++i) {
		if (Drives[i] == NotAfileRef) {
			*Drive_No = i;
			return trueblnr;
		}
	}
	return falseblnr;
}

GLOBALFUNC blnr AnyDiskInserted(void)
{
	WORD i;

	for (i = 0; i < NumDrives; ++i) {
		if (Drives[i] != NotAfileRef) {
			return trueblnr;
		}
	}
	return falseblnr;
}

LOCALPROC Sony_Insert0(FILE *refnum)
{
	UWORD Drive_No;

	if (! FirstFreeDisk(&Drive_No)) {
		fclose(refnum);
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage, falseblnr);
	} else {
		Drives[Drive_No] = refnum;
		MountPending |= ((ULONG)1 << Drive_No);
	}
}

LOCALFUNC blnr Sony_Insert1(char *drivepath)
{
	FILE *refnum = fopen(drivepath, "rb+");
	if (refnum != NULL) {
		Sony_Insert0(refnum);
		return trueblnr;
	}
	return falseblnr;
}

LOCALFUNC blnr LoadInitialImages(void)
{
	if (Sony_Insert1("disk1.dsk"))
	if (Sony_Insert1("disk2.dsk"))
	if (Sony_Insert1("disk3.dsK"))
	{
	}
	return trueblnr;
}

LOCALFUNC blnr AllocateMacROM(void)
{
	ROM = (UWORD *)malloc(kROM_Size);
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
		rom_path = "vMac.ROM";
	}
	ROM_File = fopen(rom_path, "rb");
	if (ROM_File == NULL) {
		fprintf(stderr, "The vMac.ROM could not be found.\n");
	} else {
		File_Size = fread(ROM, kROM_Size, 1, ROM_File);
		IsOk = trueblnr;
		fclose(ROM_File);
	}
	return IsOk;
}

LOCALFUNC blnr AllocateMacRAM (void)
{
	kRAM_Size = 0x00400000; // Try 4 MB
	RAM = (UWORD *)malloc(kRAM_Size + RAMSafetyMarginFudge);
	if (RAM == NULL) {
		kRAM_Size = 0x00200000; // Try 2 MB
		RAM = (UWORD *)malloc(kRAM_Size + RAMSafetyMarginFudge);
		if (RAM == NULL) {
			kRAM_Size = 0x00100000; // Try 1 MB
			RAM = (UWORD *)malloc(kRAM_Size + RAMSafetyMarginFudge);
			if (RAM == NULL) {
				MacMsg("Not enough Memory.", "Unable to allocate ROM.", trueblnr);
				return falseblnr;
			}
		}
	}
	return trueblnr;
}

#include "DATE2SEC.h"

GLOBALFUNC ULONG GetMacDateInSecond(void)
{
	time_t Current_Time;
	struct tm *s;

	(void) time(&Current_Time);
	s = localtime(&Current_Time);
	return Date2MacSeconds(s->tm_sec, s->tm_min, s->tm_hour,
		s->tm_mday, s->tm_mon, s->tm_year);
}

#define TickType ULONG
#define TicksPerSecond 1000000

LOCALFUNC TickType GetCurrentTicks(void)
{
	struct timeval t;

	gettimeofday(&t, NULL);
	return (ULONG)t.tv_sec * 1000000 + t.tv_usec;
}

LOCALVAR blnr SpeedLimit = falseblnr;

LOCALPROC ZapOSGLUVars(void)
{
	InitDrives();
}

LOCALVAR int my_argc;
LOCALVAR char **my_argv;

LOCALVAR Display *x_display = NULL;
LOCALVAR Window my_main_wind = 0;
LOCALVAR GC my_gc;
LOCALVAR XImage *my_image = NULL;
LOCALVAR Atom delete_win;
LOCALVAR Cursor blankCursor = None;

LOCALVAR char *display_name = NULL;

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
			} else {
				printf("%s %s, Copyright %s.\n", kStrAppName, kStrVersion, kStrCopyrightYear);
				printf("Including or based upon code by Bernd Schmidt,\n");
				printf("Philip Cummins, Michael Hanni, Richard F.\n");
				printf("Bannister, Weston Pawlowski, Paul Pratt,\n");
				printf("and others.\n");
				printf("%s is distributed under the terms\n", kStrAppName);
				printf("of the GNU Public License, version 2.\n");
				printf("%s is distributed in the hope that it\n",  kStrAppName);
				printf("will be useful, but WITHOUT ANY WARRANTY;\n");
				printf("without even the implied warranty of\n");
				printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR\n");
				printf("PURPOSE.\n");
				printf("For more information, see:\n");
				printf("   %s\n", kStrHomePage);
				printf("Usage: minivmac [diskimagefile]...\n");
				printf("       [-r romfile] [-display displayname]\n");
				return falseblnr;
			}
		} else {
			(void) Sony_Insert1(my_argv[i]);
		}
	}
	return trueblnr;
}

LOCALFUNC blnr Screen_Init(void)
{
	Window rootwin;
	int screen;
	Visual *Xvisual;
	XColor black;
	XColor white;
	Colormap Xcmap;
	char *image_Mem1;

	/* Get connection to X Server */
	x_display = XOpenDisplay(display_name);
	if (x_display == NULL) {
		fprintf(stderr, "Cannot connect to X server.\n");
		return falseblnr;
	}
	screen = DefaultScreen(x_display);

	Xvisual = DefaultVisual(x_display, screen);
	Xcmap = DefaultColormap(x_display, screen);

	XParseColor(x_display, Xcmap, "#000000", &black);
	if (!XAllocColor(x_display, Xcmap, &black)) {
		fprintf(stderr, "Whoops??\n");
	}
	XParseColor(x_display, Xcmap, "#ffffff", &white);
	if (!XAllocColor(x_display, Xcmap, &white)) {
		fprintf(stderr, "Whoops??\n");
	}

	rootwin = XRootWindow(x_display, screen);

	my_main_wind = XCreateSimpleWindow(x_display, rootwin,
		0, 0, vMacScreenWidth, vMacScreenHeight, 4,
		white.pixel,
		black.pixel);
	if (! my_main_wind) {
		fprintf(stderr, "XCreateSimpleWindow failed.\n");
		return falseblnr;
	}

	XSelectInput(x_display, my_main_wind,
		ExposureMask | KeyPressMask | KeyReleaseMask |
		FocusChangeMask);

	XStoreName(x_display, my_main_wind, "Mini vMac");
	XSetIconName(x_display, my_main_wind, "Mini vMac");

	{
		XClassHint *hints = XAllocClassHint();
		if (hints) {
			hints->res_name = "minivmac";
			hints->res_class = "minivmac";
			XSetClassHint(x_display, my_main_wind, hints);
			XFree(hints);
		}
	}

	{
		XWMHints *hints = XAllocWMHints();
		if (hints) {
			hints->input = True;
			hints->initial_state = NormalState;
			hints->flags = InputHint | StateHint;
			XSetWMHints(x_display, my_main_wind, hints);
			XFree(hints);
		}

	}

	{
		XSizeHints *hints = XAllocSizeHints();
		if (hints) {
			hints->min_width = vMacScreenWidth;
			hints->max_width = vMacScreenWidth;
			hints->min_height = vMacScreenHeight;
			hints->max_height = vMacScreenHeight;
			hints->flags = PMinSize | PMaxSize;
			XSetWMNormalHints(x_display, my_main_wind, hints);
			XFree(hints);
		}
	}

	XSetCommand(x_display, my_main_wind, my_argv, my_argc);

	/* let us handle a click on the close box */
	delete_win = XInternAtom(x_display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(x_display, my_main_wind, &delete_win, 1);

	blankCursor = XCreatePixmapCursor(x_display, XCreatePixmap(x_display, rootwin, 1, 1, 1),
					XCreatePixmap(x_display, rootwin, 1, 1, 1),
					&black, &white, 0, 0);
	if (blankCursor == None ) {
		fprintf(stderr, "XCreatePixmapCursor failed.\n");
		return falseblnr;
	}
	/* question: who now owns the Pixmaps - do they need to be deleted? */

	my_gc = XCreateGC(x_display, my_main_wind, 0, NULL);
	if (my_gc == NULL) {
		fprintf(stderr, "XCreateGC failed.\n");
		return falseblnr;
	}
	XSetState(x_display, my_gc, black.pixel, white.pixel, GXcopy, AllPlanes);

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

	XMapRaised(x_display, my_main_wind);

	XSync(x_display, 0);

	return trueblnr;
}

GLOBALPROC HaveChangedScreenBuff(WORD top, WORD left, WORD bottom, WORD right)
{
	XPutImage(x_display, my_main_wind, my_gc, my_image, left, top, left, top,
				right - left, bottom - top);
}

LOCALVAR blnr HaveCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		XUndefineCursor(x_display, my_main_wind);
	}
}

#include "POSTKEYS.h"

LOCALVAR blnr gBackgroundFlag = falseblnr;

LOCALVAR blnr CurTrueMouseButton = falseblnr;

LOCALPROC CheckMouseState (void)
{
	blnr ShouldHaveCursorHidden;
	UBYTE NewMouseButton;
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
	NewMouseButton = ((mask_return & Button1Mask) != 0);

#if 0
	{
		char key_vector[32];
		int i;

		XQueryKeymap(x_display, key_vector);
		for (i = 0; i < 256; i++) {
			if ((key_vector[i / 8] & (1 << (i % 8))) != 0) {
				fprintf(stderr, "key %d\n",i);
			}
		}
	}
#endif

	ShouldHaveCursorHidden = trueblnr;
	if (gBackgroundFlag) {
		ShouldHaveCursorHidden = falseblnr;
	}
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

	/* if (ShouldHaveCursorHidden || CurMouseButton) */
	/* for a game like arkanoid, would like mouse to still
	move even when outside window in one direction */
	{
		CurMouseV = NewMousePosv;
		CurMouseH = NewMousePosh;
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

/* kc_decode and event2keycode are adapted from Basilisk II */

LOCALFUNC int kc_decode(KeySym ks)
{
	switch (ks) {
		case XK_A: case XK_a: return MKC_A;
		case XK_B: case XK_b: return MKC_B;
		case XK_C: case XK_c: return MKC_C;
		case XK_D: case XK_d: return MKC_D;
		case XK_E: case XK_e: return MKC_E;
		case XK_F: case XK_f: return MKC_F;
		case XK_G: case XK_g: return MKC_G;
		case XK_H: case XK_h: return MKC_H;
		case XK_I: case XK_i: return MKC_I;
		case XK_J: case XK_j: return MKC_J;
		case XK_K: case XK_k: return MKC_K;
		case XK_L: case XK_l: return MKC_L;
		case XK_M: case XK_m: return MKC_M;
		case XK_N: case XK_n: return MKC_N;
		case XK_O: case XK_o: return MKC_O;
		case XK_P: case XK_p: return MKC_P;
		case XK_Q: case XK_q: return MKC_Q;
		case XK_R: case XK_r: return MKC_R;
		case XK_S: case XK_s: return MKC_S;
		case XK_T: case XK_t: return MKC_T;
		case XK_U: case XK_u: return MKC_U;
		case XK_V: case XK_v: return MKC_V;
		case XK_W: case XK_w: return MKC_W;
		case XK_X: case XK_x: return MKC_X;
		case XK_Y: case XK_y: return MKC_Y;
		case XK_Z: case XK_z: return MKC_Z;

		case XK_1: case XK_exclam: return MKC_1;
		case XK_2: case XK_at: return MKC_2;
		case XK_3: case XK_numbersign: return MKC_3;
		case XK_4: case XK_dollar: return MKC_4;
		case XK_5: case XK_percent: return MKC_5;
		case XK_6: return MKC_6;
		case XK_7: return MKC_7;
		case XK_8: return MKC_8;
		case XK_9: return MKC_9;
		case XK_0: return MKC_0;

		case XK_grave: case XK_asciitilde: return MKC_Grave;
		case XK_minus: case XK_underscore: return MKC_Minus;
		case XK_equal: case XK_plus: return MKC_Equal;
		case XK_bracketleft: case XK_braceleft: return MKC_LeftBracket;
		case XK_bracketright: case XK_braceright: return MKC_RightBracket;
		case XK_backslash: case XK_bar: return MKC_BackSlash;
		case XK_semicolon: case XK_colon: return MKC_SemiColon;
		case XK_apostrophe: case XK_quotedbl: return MKC_SingleQuote;
		case XK_comma: case XK_less: return MKC_Comma;
		case XK_period: case XK_greater: return MKC_Period;
		case XK_slash: case XK_question: return MKC_Slash;

		case XK_Tab: return MKC_Tab;

		case XK_Return: return MKC_Return;
		case XK_space: return MKC_Space;
		case XK_BackSpace: return MKC_BackSpace;

		case XK_Escape: return MKC_Escape;


#if defined(XK_KP_Prior) && defined(XK_KP_Left) && defined(XK_KP_Insert) && defined (XK_KP_End)
		case XK_KP_0: case XK_KP_Insert: return MKC_KP0;
		case XK_KP_1: case XK_KP_End: return MKC_KP1;
		case XK_KP_2: case XK_KP_Down: return MKC_KP2;
		case XK_KP_3: case XK_KP_Next: return MKC_KP3;
		case XK_KP_4: case XK_KP_Left: return MKC_KP4;
		case XK_KP_5: case XK_KP_Begin: return MKC_KP5;
		case XK_KP_6: case XK_KP_Right: return MKC_KP6;
		case XK_KP_7: case XK_KP_Home: return MKC_KP7;
		case XK_KP_8: case XK_KP_Up: return MKC_KP8;
		case XK_KP_9: case XK_KP_Prior: return MKC_KP9;
		case XK_KP_Decimal: case XK_KP_Delete: return MKC_Decimal;
#else
		case XK_KP_0: return MKC_KP0;
		case XK_KP_1: return MKC_KP1;
		case XK_KP_2: return MKC_KP2;
		case XK_KP_3: return MKC_KP3;
		case XK_KP_4: return MKC_KP4;
		case XK_KP_5: return MKC_KP5;
		case XK_KP_6: return MKC_KP6;
		case XK_KP_7: return MKC_KP7;
		case XK_KP_8: return MKC_KP8;
		case XK_KP_9: return MKC_KP9;
		case XK_KP_Decimal: return MKC_Decimal;
#endif
		case XK_KP_Add: return MKC_KPAdd;
		case XK_KP_Subtract: return MKC_KPSubtract;
		case XK_KP_Multiply: return MKC_KPMultiply;
		case XK_KP_Divide: return MKC_KPDevide;
		case XK_KP_Enter: return MKC_Enter;
		case XK_KP_Equal: return MKC_KPEqual;

		case XK_Shift_L: return MKC_Shift;
		case XK_Shift_R: return MKC_Shift;
		case XK_Caps_Lock: return MKC_CapsLock;
		case XK_Control_L: return MKC_Command;
		case XK_Control_R: return MKC_Command;
		case XK_Alt_L: return MKC_Option;
		case XK_Alt_R: return MKC_Option;
		case XK_Meta_L: return MKC_Option;
		case XK_Meta_R: return MKC_Option;
		case XK_Num_Lock: return MKC_Clear;
		case XK_Left: return MKC_Left;
		case XK_Right: return MKC_Right;
		case XK_Up: return MKC_Up;
		case XK_Down: return MKC_Down;

		case XK_F1: return MKC_F1;
		case XK_F2: return MKC_F2;
		case XK_F3: return MKC_F3;
		case XK_F4: return MKC_F4;
		case XK_F5: return MKC_F5;
		case XK_F6: return MKC_F6;
		case XK_F7: return MKC_F7;
		case XK_F8: return MKC_F8;
		case XK_F9: return MKC_F9;
		case XK_F10: return MKC_F10;
		case XK_F11: return MKC_F11;
		case XK_F12: return MKC_F12;
#if 0
		case XK_Control_L: return 0x36;
		case XK_Control_R: return 0x36;
		case XK_Menu: return 0x32;
		case XK_Delete: return 0x75;
		case XK_Insert: return 0x72;
		case XK_Home: case XK_Help: return 0x73;
		case XK_End: return 0x77;
#ifdef __hpux
		case XK_Prior: return 0x74;
		case XK_Next: return 0x79;
#else
		case XK_Page_Up: return 0x74;
		case XK_Page_Down: return 0x79;
#endif
		case XK_Print: return 0x69;
		case XK_Scroll_Lock: return 0x6b;
		case XK_Pause: return 0x71;
#endif
	}
	return -1;
}

LOCALFUNC int event2keycode(XKeyEvent *ev)
{
	KeySym ks;
	int as;
	int i = 0;

	do {
		ks = XLookupKeysym(ev, i++);
		as = kc_decode(ks);
		if (as != -1)
			return as;
	} while (ks != NoSymbol);

	return -1;
}

LOCALPROC DoOnEachSixtieth(void)
{
	int i = 0;
	XEvent event;
	int code;

	while(gBackgroundFlag || (XPending(x_display) > 0 && i < 10))
	{
		XNextEvent(x_display, &event);

		switch(event.type) {
			case KeyPress:
				code = event2keycode(&event.xkey);
				if (code != -1) {
					switch (code) {
						case MKC_F9:
							SpeedLimit = trueblnr;
							break;
						case MKC_F10:
							SpeedLimit = falseblnr;
							break;
						case MKC_F12:
							RequestMacOff = trueblnr;
							break;
						default:
							Keyboard_UpdateKeyMap(code, trueblnr);
							break;
					}
				}
				break;
			case KeyRelease:
				code = event2keycode(&event.xkey);
				if (code != -1) {
					Keyboard_UpdateKeyMap(code, falseblnr);
				}
				break;
			case Expose:
				{
					int x = ((XExposeEvent *)&event)->x;
					int y = ((XExposeEvent *)&event)->y;
					int width = ((XExposeEvent *)&event)->width;
					int height = ((XExposeEvent *)&event)->height;

					XPutImage(x_display, my_main_wind, my_gc, my_image, x, y, x, y,
						x + width, y + height);
				}
				break;
			case ClientMessage:
				if (event.xclient.data.l[0]==delete_win) {
					RequestMacOff = trueblnr;
				}
				break;
			case FocusIn:
				gBackgroundFlag = falseblnr;
				break;
			case FocusOut:
				ForceShowCursor();
				gBackgroundFlag = trueblnr;
				break;
			default:
				break;
		}
		i++;
	}
	CheckMouseState();
}

LOCALVAR TickType LastTime;
LOCALVAR ULONG TimeCounter = 0;

LOCALFUNC blnr Init60thCheck(void)
{
	LastTime = GetCurrentTicks();
	return trueblnr;
}

GLOBALFUNC blnr CheckIntSixtieth(blnr overdue)
{
	TickType LatestTime;

	do {
		LatestTime = GetCurrentTicks();
		if (LatestTime != LastTime) {
			TimeCounter += 60 * (ULONG)(LatestTime - LastTime);
			LastTime = LatestTime;
			if (TimeCounter > TicksPerSecond) {
				TimeCounter %= TicksPerSecond;
				/*
					Idea is to get here every
					1000/60 milliseconds, on average.
					Unless emulation has been interupted
					too long, which is why use '%='
					and not '-='.

				*/
				DoOnEachSixtieth();
				return trueblnr;
			}
		}
	} while (SpeedLimit && overdue);
	return falseblnr;
}

LOCALFUNC blnr InitOSGLU(void)
{
	if (LoadInitialImages())
	if (ScanCommandLine())
	if (AllocateMacROM())
	if (LoadMacRom())
	if (Screen_Init())
	if (AllocateMacRAM())
	if (Init60thCheck())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
	ForceShowCursor();
	if (blankCursor != None ) {
		XFreeCursor(x_display, blankCursor);
	}
	if (my_image != NULL) {
		XDestroyImage(my_image);
	}
	if (my_main_wind) {
		XDestroyWindow(x_display, my_main_wind);
	}
	if (x_display != NULL) {
		XCloseDisplay(x_display);
	}
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
		ProgramMain();
	}
	UnInitOSGLU();
	return 0;
}
