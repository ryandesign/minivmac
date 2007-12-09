/*
	PLATGLUE.h

	Copyright (C) 2006 Michael Hanni, Christian Bauer,
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
	PLATform GLUE. (for X window system)

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

/*--- control mode and internationalization ---*/

#define NeedCell2PlainAsciiMap 1

#include "CONTROLM.h"

/*--- debug settings and utilities ---*/

LOCALVAR Display *x_display = NULL;

#define MyDbgOut 0
#define MyDbgEvents (1 && MyDbgOut)

#if ! MyDbgOut
#define WriteExtraErr(s)
#else
LOCALPROC WriteExtraErr(char *s)
{
	fprintf(stderr, "*** error: %s\n", s);
}
#endif

#if MyDbgOut
LOCALPROC WriteDbgAtom(char *s, Atom x)
{
	char *name = XGetAtomName(x_display, x);
	if (name != NULL) {
		fprintf(stderr, "Atom %s: %s\n", s, name);
		XFree(name);
	}
}
#endif

/*--- information about the environment ---*/

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
LOCALVAR Atom MyXA_NetActiveWindow = (Atom)0;
LOCALVAR Atom MyXA_NetSupported = (Atom)0;
#endif
#if IncludeHostTextClipExchange
LOCALVAR Atom MyXA_CLIPBOARD = (Atom)0;
LOCALVAR Atom MyXA_TARGETS = (Atom)0;
LOCALVAR Atom MyXA_MinivMac_Clip = (Atom)0;
#endif

LOCALPROC LoadMyXA(void)
{
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
	MyXA_NetActiveWindow = XInternAtom(x_display, "_NET_ACTIVE_WINDOW", False);
	MyXA_NetSupported = XInternAtom(x_display, "_NET_SUPPORTED", False);
#endif
#if IncludeHostTextClipExchange
	MyXA_CLIPBOARD = XInternAtom(x_display, "CLIPBOARD", False);
	MyXA_TARGETS = XInternAtom(x_display, "TARGETS", False);
	MyXA_MinivMac_Clip = XInternAtom(x_display, "_MinivMac_Clip", False);
#endif
}

LOCALFUNC blnr NetSupportedContains(Atom x)
{
	/*
		Note that the window manager could be replaced at
		any time, so don't cache results of this function.
	*/
	Atom ret_type;
	int ret_format;
	unsigned long ret_item;
	unsigned long remain_byte;
	unsigned long i;
	unsigned char *s = 0;
	blnr foundit = falseblnr;
	Window rootwin = XRootWindow(x_display,
		DefaultScreen(x_display));

	if (Success != XGetWindowProperty(x_display, rootwin,
		MyXA_NetSupported,
		0, 65535, False, XA_ATOM, &ret_type, &ret_format,
		&ret_item, &remain_byte, &s))
	{
		WriteExtraErr("XGetWindowProperty failed");
	} else if (! s) {
		WriteExtraErr("XGetWindowProperty failed");
	} else if (ret_type != XA_ATOM) {
		WriteExtraErr("XGetWindowProperty returns wrong type");
	} else {
		Atom *v = (Atom *)s;

		for (i = 0; i < ret_item; ++i) {
			if (v[i] == x) {
				foundit = trueblnr;
				/* fprintf(stderr, "found the hint\n"); */
			}
		}
	}
	if (s) {
		XFree(s);
	}
	return foundit;
}

/*--- paramter buffers ---*/

#if IncludePbufs
LOCALVAR void *PbufDat[NumPbufs];
#endif

#if IncludePbufs
LOCALFUNC tMacErr PbufNewFromPtr(void *p, ui5b count, tPbuf *r)
{
	tPbuf i;
	tMacErr err;

	if (! FirstFreePbuf(&i)) {
		free(p);
		err = mnvm_miscErr;
	} else {
		*r = i;
		PbufDat[i] = p;
		PbufNewNotify(i, count);
		err = mnvm_noErr;
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALFUNC tMacErr PbufNew(ui5b count, tPbuf *r)
{
	tMacErr err = mnvm_miscErr;

	void *p = calloc(1, count);
	if (NULL != p) {
		err = PbufNewFromPtr(p, count, r);
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALPROC PbufDispose(tPbuf i)
{
	free(PbufDat[i]);
	PbufDisposeNotify(i);
}
#endif

#if IncludePbufs
LOCALPROC UnInitPbufs(void)
{
	tPbuf i;

	for (i = 0; i < NumPbufs; ++i) {
		if (PbufIsAllocated(i)) {
			PbufDispose(i);
		}
	}
}
#endif

#if IncludePbufs
GLOBALPROC PbufTransfer(ui3p Buffer,
	tPbuf i, ui5r offset, ui5r count, blnr IsWrite)
{
	void *p = ((ui3p)PbufDat[i]) + offset;
	if (IsWrite) {
		(void) memcpy(p, Buffer, count);
	} else {
		(void) memcpy(Buffer, p, count);
	}
}
#endif

/*--- text translation ---*/

/* this is table for Windows, any changes needed for X? */
LOCALVAR const ui3b Native2MacRomanTab[] = {
	0xAD, 0xB0, 0xE2, 0xC4, 0xE3, 0xC9, 0xA0, 0xE0,
	0xF6, 0xE4, 0xB6, 0xDC, 0xCE, 0xB2, 0xB3, 0xB7,
	0xB8, 0xD4, 0xD5, 0xD2, 0xD3, 0xA5, 0xD0, 0xD1,
	0xF7, 0xAA, 0xC5, 0xDD, 0xCF, 0xB9, 0xC3, 0xD9,
	0xCA, 0xC1, 0xA2, 0xA3, 0xDB, 0xB4, 0xBA, 0xA4,
	0xAC, 0xA9, 0xBB, 0xC7, 0xC2, 0xBD, 0xA8, 0xF8,
	0xA1, 0xB1, 0xC6, 0xD7, 0xAB, 0xB5, 0xA6, 0xE1,
	0xFC, 0xDA, 0xBC, 0xC8, 0xDE, 0xDF, 0xF0, 0xC0,
	0xCB, 0xE7, 0xE5, 0xCC, 0x80, 0x81, 0xAE, 0x82,
	0xE9, 0x83, 0xE6, 0xE8, 0xED, 0xEA, 0xEB, 0xEC,
	0xF5, 0x84, 0xF1, 0xEE, 0xEF, 0xCD, 0x85, 0xF9,
	0xAF, 0xF4, 0xF2, 0xF3, 0x86, 0xFA, 0xFB, 0xA7,
	0x88, 0x87, 0x89, 0x8B, 0x8A, 0x8C, 0xBE, 0x8D,
	0x8F, 0x8E, 0x90, 0x91, 0x93, 0x92, 0x94, 0x95,
	0xFD, 0x96, 0x98, 0x97, 0x99, 0x9B, 0x9A, 0xD6,
	0xBF, 0x9D, 0x9C, 0x9E, 0x9F, 0xFE, 0xFF, 0xD8
};

LOCALFUNC tMacErr NativeTextToMacRomanPbuf(char *x, tPbuf *r)
{
	if (NULL == x) {
		return mnvm_miscErr;
	} else {
		ui3p p;
		ui5b L = strlen(x);

		p = (ui3p)malloc(L);
		if (NULL == p) {
			return mnvm_miscErr;
		} else {
			ui3b *p0 = (ui3b *)x;
			ui3b *p1 = (ui3b *)p;
			int i;

			for (i = L; --i >= 0; ) {
				ui3b v = *p0++;
				if (v >= 128) {
					v = Native2MacRomanTab[v - 128];
				} else if (10 == v) {
					v = 13;
				}
				*p1++ = v;
			}

			return PbufNewFromPtr(p, L, r);
		}
	}
}

/* this is table for Windows, any changes needed for X? */
LOCALVAR const ui3b MacRoman2NativeTab[] = {
	0xC4, 0xC5, 0xC7, 0xC9, 0xD1, 0xD6, 0xDC, 0xE1,
	0xE0, 0xE2, 0xE4, 0xE3, 0xE5, 0xE7, 0xE9, 0xE8,
	0xEA, 0xEB, 0xED, 0xEC, 0xEE, 0xEF, 0xF1, 0xF3,
	0xF2, 0xF4, 0xF6, 0xF5, 0xFA, 0xF9, 0xFB, 0xFC,
	0x86, 0xB0, 0xA2, 0xA3, 0xA7, 0x95, 0xB6, 0xDF,
	0xAE, 0xA9, 0x99, 0xB4, 0xA8, 0x80, 0xC6, 0xD8,
	0x81, 0xB1, 0x8D, 0x8E, 0xA5, 0xB5, 0x8A, 0x8F,
	0x90, 0x9D, 0xA6, 0xAA, 0xBA, 0xAD, 0xE6, 0xF8,
	0xBF, 0xA1, 0xAC, 0x9E, 0x83, 0x9A, 0xB2, 0xAB,
	0xBB, 0x85, 0xA0, 0xC0, 0xC3, 0xD5, 0x8C, 0x9C,
	0x96, 0x97, 0x93, 0x94, 0x91, 0x92, 0xF7, 0xB3,
	0xFF, 0x9F, 0xB9, 0xA4, 0x8B, 0x9B, 0xBC, 0xBD,
	0x87, 0xB7, 0x82, 0x84, 0x89, 0xC2, 0xCA, 0xC1,
	0xCB, 0xC8, 0xCD, 0xCE, 0xCF, 0xCC, 0xD3, 0xD4,
	0xBE, 0xD2, 0xDA, 0xDB, 0xD9, 0xD0, 0x88, 0x98,
	0xAF, 0xD7, 0xDD, 0xDE, 0xB8, 0xF0, 0xFD, 0xFE
};

LOCALFUNC blnr MacRomanTextToNativePtr(tPbuf i, blnr IsFileName,
	ui3p *r)
{
	ui3p p;
	void *Buffer = PbufDat[i];
	ui5b L = PbufSize[i];

	p = (ui3p)malloc(L + 1);
	if (p != NULL) {
		ui3b *p0 = (ui3b *)Buffer;
		ui3b *p1 = (ui3b *)p;
		int j;

		if (IsFileName) {
			for (j = L; --j >= 0; ) {
				ui3b x = *p0++;
				if (x < 32) {
					x = '-';
				} else if (x >= 128) {
					x = MacRoman2NativeTab[x - 128];
				} else {
					switch (x) {
						case '/':
						case '<':
						case '>':
						case '|':
						case ':':
							x = '-';
						default:
							break;
					}
				}
				*p1++ = x;
			}
			if ('.' == p[0]) {
				p[0] = '-';
			}
		} else {
			for (j = L; --j >= 0; ) {
				ui3b x = *p0++;
				if (x >= 128) {
					x = MacRoman2NativeTab[x - 128];
				} else if (13 == x) {
					x = '\n';
				}
				*p1++ = x;
			}
		}
		*p1 = 0;

		*r = p;
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC NativeStrFromCStr(char *r, char *s)
{
	ui3b ps[ClStrMaxLength];
	int i;
	int L;

	ClStrFromSubstCStr(&L, ps, s);

	for (i = 0; i < L; ++i) {
		r[i] = Cell2PlainAsciiMap[ps[i]];
	}

	r[L] = 0;
}

/*--- drives ---*/

#define NotAfileRef NULL

LOCALVAR FILE *Drives[NumDrives]; /* open disk image files */
#if IncludeSonyGetName || IncludeSonyNew
LOCALVAR char *DriveNames[NumDrives];
#endif

LOCALPROC InitDrives(void)
{
	/*
		This isn't really needed, Drives[i] and DriveNames[i]
		need not have valid values when not vSonyIsInserted[i].
	*/
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
#if IncludeSonyGetName || IncludeSonyNew
		DriveNames[i] = NULL;
#endif
	}
}

GLOBALFUNC tMacErr vSonyRead(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count)
{
	fseek(Drives[Drive_No], Sony_Start, SEEK_SET);
	fread(Buffer, 1, *Sony_Count, Drives[Drive_No]);
	return mnvm_noErr; /*& figure out what really to return &*/
}

GLOBALFUNC tMacErr vSonyWrite(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count)
{
	fseek(Drives[Drive_No], Sony_Start, SEEK_SET);
	fwrite(Buffer, 1, *Sony_Count, Drives[Drive_No]);
	return mnvm_noErr; /*& figure out what really to return &*/
}

GLOBALFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
	fseek(Drives[Drive_No], 0, SEEK_END);
	*Sony_Count = ftell(Drives[Drive_No]);
	return mnvm_noErr; /*& figure out what really to return &*/
}

LOCALFUNC tMacErr vSonyEject0(tDrive Drive_No, blnr deleteit)
{
	DiskEjectedNotify(Drive_No);

	fclose(Drives[Drive_No]);
	Drives[Drive_No] = NotAfileRef; /* not really needed */

#if IncludeSonyGetName || IncludeSonyNew
	{
		char *s = DriveNames[Drive_No];
		if (NULL != s) {
			if (deleteit) {
				remove(s);
			}
			free(s);
			DriveNames[Drive_No] = NULL; /* not really needed */
		}
	}
#endif

	return mnvm_noErr;
}

GLOBALFUNC tMacErr vSonyEject(tDrive Drive_No)
{
	return vSonyEject0(Drive_No, falseblnr);
}

#if IncludeSonyNew
GLOBALFUNC tMacErr vSonyEjectDelete(tDrive Drive_No)
{
	return vSonyEject0(Drive_No, trueblnr);
}
#endif

LOCALPROC UnInitDrives(void)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			(void) vSonyEject(i);
		}
	}
}

#if IncludeSonyGetName
GLOBALFUNC tMacErr vSonyGetName(tDrive Drive_No, tPbuf *r)
{
	char *drivepath = DriveNames[Drive_No];
	if (NULL == drivepath) {
		return mnvm_miscErr;
	} else {
		char *s = strrchr(drivepath, '/');
		if (NULL == s) {
			s = drivepath;
		} else {
			++s;
		}
		return NativeTextToMacRomanPbuf(s, r);
	}
}
#endif

LOCALFUNC blnr Sony_Insert0(FILE *refnum, blnr locked,
	char *drivepath)
{
	tDrive Drive_No;

	if (! FirstFreeDisk(&Drive_No)) {
		fclose(refnum);
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage, falseblnr);
		return falseblnr;
	} else {
		/* printf("Sony_Insert0 %d\n", (int)Drive_No); */
		Drives[Drive_No] = refnum;
		DiskInsertNotify(Drive_No, locked);

#if IncludeSonyGetName || IncludeSonyNew
		{
			ui5b L = strlen(drivepath);
			char *p = malloc(L + 1);
			if (p != NULL) {
				(void) memcpy(p, drivepath, L + 1);
			}
			DriveNames[Drive_No] = p;
		}
#endif

		return trueblnr;
	}
}

LOCALFUNC blnr Sony_Insert1(char *drivepath, blnr silentfail)
{
	blnr locked = falseblnr;
	/* printf("Sony_Insert1 %s\n", drivepath); */
	FILE *refnum = fopen(drivepath, "rb+");
	if (NULL == refnum) {
		locked = trueblnr;
		refnum = fopen(drivepath, "rb");
	}
	if (NULL == refnum) {
		if (! silentfail) {
			MacMsg(kStrOpenFailTitle, kStrOpenFailMessage, falseblnr);
		}
	} else {
		return Sony_Insert0(refnum, locked, drivepath);
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
		if (! Sony_Insert1(s, trueblnr)) {
			/* stop on first error (including file not found) */
			return trueblnr;
		}
	}

	return trueblnr;
}

#if IncludeSonyNew
LOCALFUNC blnr WriteZero(FILE *refnum, ui5b L)
{
#define ZeroBufferSize 2048
	ui5b i;
	ui3b buffer[ZeroBufferSize];

	memset(&buffer, 0, ZeroBufferSize);

	while (L > 0) {
		i = (L > ZeroBufferSize) ? ZeroBufferSize : L;
		if (fwrite(buffer, 1, i, refnum) != i) {
			return falseblnr;
		}
		L -= i;
	}
	return trueblnr;
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDisk(ui5b L, char *drivepath)
{
	blnr IsOk = falseblnr;
	FILE *refnum = fopen(drivepath, "wb+");
	if (NULL == refnum) {
		MacMsg(kStrOpenFailTitle, kStrOpenFailMessage, falseblnr);
	} else {
		if (WriteZero(refnum, L)) {
			IsOk = Sony_Insert0(refnum, falseblnr, drivepath);
			refnum = NULL;
		}
		if (refnum != NULL) {
			fclose(refnum);
		}
		if (! IsOk) {
			(void) remove(drivepath);
		}
	}
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDiskAtDefault(ui5b L)
{
	char s[ClStrMaxLength + 1];

	NativeStrFromCStr(s, "untitled.dsk");
	MakeNewDisk(L, s);
}
#endif

/*--- ROM ---*/

LOCALFUNC blnr AllocateMacROM(void)
{
	ROM = (ui3p)calloc(1, kROM_Size);
	if (NULL == ROM) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALPROC DeallocateMacROM(void)
{
	if (NULL != ROM) {
		free((char *)ROM);
	}
}

LOCALVAR char *rom_path = NULL;

LOCALFUNC blnr LoadMacRom(void)
{
	FILE *ROM_File;
	int File_Size;

	if (NULL == rom_path) {
		rom_path = RomFileName;
	}
	ROM_File = fopen(rom_path, "rb");
	if (NULL == ROM_File) {
		MacMsg(kStrNoROMTitle, kStrNoROMMessage, trueblnr);
		SpeedStopped = trueblnr;
	} else {
		File_Size = fread(ROM, 1, kTrueROM_Size, ROM_File);
		if (File_Size != kTrueROM_Size) {
			if (feof(ROM_File)) {
				MacMsg(kStrShortROMTitle, kStrShortROMMessage, trueblnr);
			} else {
				MacMsg(kStrNoReadROMTitle, kStrNoReadROMMessage, trueblnr);
			}
			SpeedStopped = trueblnr;
		}
		fclose(ROM_File);
	}
	return trueblnr;
}

/*--- RAM ---*/

LOCALFUNC blnr AllocateMacRAM(void)
{
	RAM = (ui3p)calloc(1, kRAM_Size + RAMSafetyMarginFudge);
	if (NULL == RAM) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALPROC DeallocateMacRAM(void)
{
	if (NULL != RAM) {
		free((char *)RAM);
	}
}

/*--- video out ---*/

LOCALVAR Window my_main_wind = 0;
LOCALVAR GC my_gc = NULL;

LOCALVAR XColor x_black;
LOCALVAR XColor x_white;

#if EnableFullScreen
LOCALVAR short hOffset;
LOCALVAR short vOffset;
#endif

#if EnableFullScreen
LOCALVAR blnr UseFullScreen = falseblnr;
#endif

#if EnableMagnify
LOCALVAR blnr UseMagnify = falseblnr;
#endif

#ifndef MyWindowScale
#define MyWindowScale 2
#endif

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

LOCALVAR XImage *my_image = NULL;
#if UseControlKeys
LOCALVAR XImage *my_cntrl_image = NULL;
#endif

#define EnableScalingBuff (1 && EnableMagnify && (MyWindowScale == 2))

#if EnableScalingBuff
LOCALVAR char *ScalingBuff = nullpr;
LOCALVAR XImage *my_scaled_image = NULL;
#endif

#if EnableMagnify
#define MyScaledHeight (MyWindowScale * vMacScreenHeight)
#define MyScaledWidth (MyWindowScale * vMacScreenWidth)
#endif

LOCALPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right)
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
		ui3b *p1 = (ui3b *)GetCurDrawBuff() + vMacScreenWidth / 8 * top;
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
		(void) GetCurDrawBuff();
		XPutImage(x_display, my_main_wind, my_gc,
#if UseControlKeys
			(CurControlMode != 0) ? my_cntrl_image :
#endif
				my_image,
			left, top, XDest, YDest,
			right - left, bottom - top);
	}
}

/*--- mouse ---*/

/* cursor hiding */

LOCALVAR blnr HaveCursorHidden = falseblnr;
LOCALVAR blnr WantCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		if (my_main_wind) {
			XUndefineCursor(x_display, my_main_wind);
		}
	}
}

LOCALVAR Cursor blankCursor = None;

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
	if (None == cursormask) {
		WriteExtraErr("XCreatePixmap failed.");
	} else {
		xgc.function = GXclear;
		gc = XCreateGC(x_display, cursormask, GCFunction, &xgc);
		if (None == gc) {
			WriteExtraErr("XCreateGC failed.");
		} else {
			XFillRectangle(x_display, cursormask, gc, 0, 0, 1, 1);
			XFreeGC(x_display, gc);

			blankCursor = XCreatePixmapCursor(x_display, cursormask,
							cursormask, &x_black, &x_white, 0, 0);
			if (None == blankCursor) {
				WriteExtraErr("XCreatePixmapCursor failed.");
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

/* cursor moving */

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

/* cursor state */

LOCALVAR blnr CurTrueMouseButton = falseblnr;

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

	WantCursorHidden = ShouldHaveCursorHidden;
}

/*--- keyboard input ---*/

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

	for (i = 0; i < 256; ++i) {
		KC2MKC[i] = -1;
	}

#if 0 /* find Keysym for a code */
	for (i = 0; i < 64 * 1024; ++i) {
		KeyCode code = XKeysymToKeycode(x_display, i);
		if (115 == code) {
			fprintf(stderr, "i %d\n", i);
		}
	}
#endif

	/*
	start with redundant mappings, should get overwritten
	by main mappings but define them just in case
	*/

#ifdef XK_KP_Insert
	KC2MKCAssignOne(XK_KP_Insert, MKC_KP0);
#endif
#ifdef XK_KP_End
	KC2MKCAssignOne(XK_KP_End, MKC_KP1);
#endif
#ifdef XK_KP_Down
	KC2MKCAssignOne(XK_KP_Down, MKC_KP2);
#endif
#ifdef XK_KP_Next
	KC2MKCAssignOne(XK_KP_Next, MKC_KP3);
#endif
#ifdef XK_KP_Left
	KC2MKCAssignOne(XK_KP_Left, MKC_KP4);
#endif
#ifdef XK_KP_Begin
	KC2MKCAssignOne(XK_KP_Begin, MKC_KP5);
#endif
#ifdef XK_KP_Right
	KC2MKCAssignOne(XK_KP_Right, MKC_KP6);
#endif
#ifdef XK_KP_Home
	KC2MKCAssignOne(XK_KP_Home, MKC_KP7);
#endif
#ifdef XK_KP_Up
	KC2MKCAssignOne(XK_KP_Up, MKC_KP8);
#endif
#ifdef XK_KP_Prior
	KC2MKCAssignOne(XK_KP_Prior, MKC_KP9);
#endif
#ifdef XK_KP_Delete
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

	KC2MKCAssignOne(XK_F1, MKC_F1);
	KC2MKCAssignOne(XK_F2, MKC_F2);
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

#ifdef XK_Delete
	KC2MKCAssignOne(XK_Delete, MKC_ForwardDel);
#endif
#ifdef XK_Insert
	KC2MKCAssignOne(XK_Insert, MKC_Help);
#endif
#ifdef XK_Help
	KC2MKCAssignOne(XK_Help, MKC_Help);
#endif
#ifdef XK_Home
	KC2MKCAssignOne(XK_Home, MKC_Home);
#endif
#ifdef XK_End
	KC2MKCAssignOne(XK_End, MKC_End);
#endif

#ifdef XK_Page_Up
	KC2MKCAssignOne(XK_Page_Up, MKC_PageUp);
#else
#ifdef XK_Prior
	KC2MKCAssignOne(XK_Prior, MKC_PageUp);
#endif
#endif

#ifdef XK_Page_Down
	KC2MKCAssignOne(XK_Page_Down, MKC_PageDown);
#else
#ifdef XK_Next
	KC2MKCAssignOne(XK_Next, MKC_PageDown);
#endif
#endif

#ifdef XK_Print
	KC2MKCAssignOne(XK_Print, MKC_Print);
#endif
#ifdef XK_Scroll_Lock
	KC2MKCAssignOne(XK_Scroll_Lock, MKC_ScrollLock);
#endif
#ifdef XK_Pause
	KC2MKCAssignOne(XK_Pause, MKC_Pause);
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

#ifndef MKC_for_Meta
#define MKC_for_Meta MKC_Command
#endif

#ifndef MKC_for_Meta_L
#define MKC_for_Meta_L MKC_for_Meta
#endif
	KC2MKCAssignOne(XK_Meta_L, MKC_for_Meta_L);

#ifndef MKC_for_Meta_R
#define MKC_for_Meta_R MKC_for_Meta
#endif
	KC2MKCAssignOne(XK_Meta_R, MKC_for_Meta_R);

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

#ifndef MKC_for_Alt
#define MKC_for_Alt MKC_Command
#endif

#ifndef MKC_for_Alt_L
#define MKC_for_Alt_L MKC_for_Alt
#endif
	KC2MKCAssignOne(XK_Alt_L, MKC_for_Alt_L);

#ifndef MKC_for_Alt_R
#define MKC_for_Alt_R MKC_for_Alt
#endif
	KC2MKCAssignOne(XK_Alt_R, MKC_for_Alt_R);

#ifndef MKC_for_Control
#define MKC_for_Control MKC_Control
#endif

#ifndef MKC_for_Control_L
#define MKC_for_Control_L MKC_for_Control
#endif
	KC2MKCAssignOne(XK_Control_L, MKC_for_Control_L);

#ifndef MKC_for_Control_R
#define MKC_for_Control_R MKC_for_Control
#endif
	KC2MKCAssignOne(XK_Control_R, MKC_for_Control_R);

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

	InitKeyCodes();

	return trueblnr;
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

	Keyboard_UpdateKeyMap2(MKC_CapsLock, (mask_return & LockMask) != 0);
}

LOCALPROC DoKeyCode0(int i, blnr down)
{
	int key = KC2MKC[i];
	if (key >= 0) {
		Keyboard_UpdateKeyMap2(key, down);
	}
}

LOCALPROC DoKeyCode(int i, blnr down)
{
	if (i == TheCapsLockCode) {
		CheckTheCapsLock();
	} else if (i >= 0 && i < 256) {
		DoKeyCode0(i, down);
	}
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

LOCALVAR blnr NoKeyRepeat = falseblnr;
LOCALVAR int SaveKeyRepeat;

LOCALPROC DisableKeyRepeat(void)
{
	XKeyboardState r;
	XKeyboardControl k;

	if ((! NoKeyRepeat) && (x_display != NULL)) {
		NoKeyRepeat = trueblnr;

		XGetKeyboardControl(x_display, &r);
		SaveKeyRepeat = r.global_auto_repeat;

		k.auto_repeat_mode = AutoRepeatModeOff;
		XChangeKeyboardControl(x_display, KBAutoRepeatMode, &k);
	}
}

LOCALPROC RestoreKeyRepeat(void)
{
	XKeyboardControl k;

	if (NoKeyRepeat && (x_display != NULL)) {
		NoKeyRepeat = falseblnr;

		k.auto_repeat_mode = SaveKeyRepeat;
		XChangeKeyboardControl(x_display, KBAutoRepeatMode, &k);
	}
}

LOCALVAR blnr WantCmdOptOnReconnect = falseblnr;

LOCALPROC GetTheDownKeys(void)
{
	char keys_return[32];
	int i;
	int v;
	int j;

	XQueryKeymap(x_display, keys_return);

	for (i = 0; i < 32; ++i) {
		v = keys_return[i];
		for (j = 0; j < 8; ++j) {
			if (0 != ((1 << j) & v)) {
				int k = i * 8 + j;
				if (k != TheCapsLockCode) {
					DoKeyCode0(k, trueblnr);
				}
			}
		}
	}
}

LOCALPROC ReconnectKeyCodes3(void)
{
	CheckTheCapsLock();

	if (WantCmdOptOnReconnect) {
		WantCmdOptOnReconnect = falseblnr;

		GetTheDownKeys();
	}
}

LOCALPROC DisconnectKeyCodes3(void)
{
	DisconnectKeyCodes2();
	CurMouseButton = falseblnr;
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

/*--- sound ---*/

#if MySoundEnabled

#define SOUND_SAMPLERATE 22255 /* = round(7833600 * 2 / 704) */

#define kLn2SoundBuffers 4 /* kSoundBuffers must be a power of two */
#define kSoundBuffers (1 << kLn2SoundBuffers)
#define kSoundBuffMask (kSoundBuffers - 1)

#define DesiredMinFilledSoundBuffs 4
	/*
		if too big then sound lags behind emulation.
		if too small then sound will have pauses.
	*/

#define kLn2BuffLen 9
#define kLnBuffSz (kLn2SoundBuffers + kLn2BuffLen)
#define My_Sound_Len (1UL << kLn2BuffLen)
#define kBufferSize (1UL << kLnBuffSz)
#define kBufferMask (kBufferSize - 1)
#define dbhBufferSize (kBufferSize + SOUND_LEN)

#define desired_alsa_buffer_size kBufferSize
#define desired_alsa_period_size My_Sound_Len

LOCALVAR char *alsadev_name = NULL;

LOCALVAR snd_pcm_t *pcm_handle = NULL;
LOCALVAR snd_pcm_uframes_t buffer_size;
LOCALVAR snd_pcm_uframes_t period_size;

LOCALVAR ui3p MyBuffer = NULL;

LOCALVAR ui4b MinFilledSoundBuffs;
LOCALVAR ui4b CurPlayOffset;
LOCALVAR ui4b CurFillOffset;

/*
	The elaborate private buffer is mostly
	redundant since alsa has its own ring
	buffer. But using it keeps the code
	closer to the other ports. And anyway
	there is no guarantee just what size
	buffer you'll get from alsa.
*/

LOCALPROC FillWithSilence(ui3p p, int n, ui3b v)
{
	int i;

	for (i = n; --i >= 0; ) {
		*p++ = v;
	}
}

LOCALPROC MySound_WriteOut(void)
{
	int retry_count = 32;

label_retry:
	if (--retry_count > 0) {
		snd_pcm_sframes_t avail;
		snd_pcm_sframes_t delayp;
		int err;
		ui4b ToPlaySize = CurFillOffset - CurPlayOffset;
		snd_pcm_state_t cur_state = snd_pcm_state(pcm_handle);

		if (SND_PCM_STATE_RUNNING != cur_state) {
			if ((ToPlaySize >> kLn2BuffLen) < 12) {
				/* just wait */
			} else {
				switch (cur_state) {
					case SND_PCM_STATE_SETUP:
					case SND_PCM_STATE_XRUN:
						err = snd_pcm_prepare(pcm_handle);
						if (err < 0) {
							fprintf(stderr, "pcm prepare error: %s\n",
								snd_strerror(avail));
						} else {
							/* fprintf(stderr, "prepare succeeded\n"); */
							goto label_retry;
						}
						break;
					case SND_PCM_STATE_PREPARED:
						if ((err = snd_pcm_start(pcm_handle)) < 0) {
							fprintf(stderr, "pcm start error: %s\n",
								snd_strerror(err));
						} else {
							/* fprintf(stderr, "start succeeded\n"); */
							goto label_retry;
						}
						break;
					case SND_PCM_STATE_SUSPENDED:
						err = snd_pcm_resume(pcm_handle);
						if (err < 0) {
							fprintf(stderr, "pcm resume error: %s\n",
								snd_strerror(avail));
						} else {
							/* fprintf(stderr, "resume succeeded\n"); */
							goto label_retry;
						}
						break;
					case SND_PCM_STATE_DISCONNECTED:
						/* just abort ? */
						break;
					default:
						fprintf(stderr, "unknown alsa pcm state\n");
						break;
				}
			}
		} else if ((err = snd_pcm_delay(pcm_handle, &delayp)) < 0) {
			fprintf(stderr, "snd_pcm_delay error: %s\n",
				snd_strerror(err));
		} else if ((avail = snd_pcm_avail_update(pcm_handle)) < 0) {
			fprintf(stderr, "pcm update error: %s\n",
				snd_strerror(avail));
		} else {
			ui3p NextPlayPtr;
			ui4b PlayNowSize = 0;
			ui4b MaskedFillOffset = CurPlayOffset & (My_Sound_Len - 1);
			ui4b TotPendBuffs = delayp >> kLn2BuffLen;

			if (TotPendBuffs < MinFilledSoundBuffs) {
				MinFilledSoundBuffs = TotPendBuffs;
			}

			if (MaskedFillOffset != 0) {
				/* take care of left overs */
				PlayNowSize = My_Sound_Len - MaskedFillOffset;
				NextPlayPtr = MyBuffer + (CurPlayOffset & kBufferMask);
			} else if (0 != (ToPlaySize >> kLn2BuffLen)) {
				PlayNowSize = My_Sound_Len;
				NextPlayPtr = MyBuffer + (CurPlayOffset & kBufferMask);
			} else {
				if (delayp < 1024) {
					/* low on sound to play. play a bit of silence */
					CurPlayOffset -= My_Sound_Len;
					NextPlayPtr = MyBuffer + (CurPlayOffset & kBufferMask);
					PlayNowSize = My_Sound_Len;
					FillWithSilence(NextPlayPtr, My_Sound_Len,
						/* 0x80 */ *((ui3p)NextPlayPtr + My_Sound_Len - 1));
					/* fprintf(stderr, "need silence\n"); */
				}
			}

			if (PlayNowSize > avail) {
				/*
					This isn't supposed to be needed with nonblock
					mode. But in Ubuntu 7.04 running in Parallels,
					snd_pcm_writei seemed to block anyway.
				*/
				PlayNowSize = avail;
			}

			if (0 != PlayNowSize) {
				err = snd_pcm_writei(pcm_handle, NextPlayPtr, PlayNowSize);
				if (err < 0) {
					if ((- EAGAIN == err) || (- ESTRPIPE == err)) {
						/* buffer full, try again later */
						/* fprintf(stderr, "pcm write: EAGAIN\n"); */
					} else if (- EPIPE == err) {
						/* buffer seems to have emptied */
						/* fprintf(stderr, "pcm write emptied\n"); */
						goto label_retry;
					} else {
						fprintf(stderr, "pcm write error: %s\n",
							snd_strerror(err));
					}
				} else {
					CurPlayOffset += err;
					goto label_retry;
				}
			}
		}
	}
}

LOCALPROC MySound_Start(void)
{
	if (pcm_handle != NULL) {
		/* Fill buffer with silence */
		FillWithSilence(MyBuffer, dbhBufferSize, 0x80);

		/* Reset variables */
		CurPlayOffset = 0;
		CurFillOffset = 0;
		MinFilledSoundBuffs = kSoundBuffers;
	}
}

LOCALPROC MySound_Stop(void)
{
	if (pcm_handle != NULL) {
		snd_pcm_drop(pcm_handle);
	}
}

LOCALPROC MySound_Init0(void)
{
	snd_pcm_hw_params_t *hw_params = NULL;
	snd_pcm_sw_params_t *sw_params = NULL;
	unsigned int rrate = SOUND_SAMPLERATE;
	int err;

	buffer_size = desired_alsa_buffer_size;
	period_size = desired_alsa_period_size;

	/* Open the sound device */
	if (NULL == alsadev_name) {
		alsadev_name = getenv("AUDIODEV");
		if (NULL == alsadev_name) {
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

#if 0
		if (buffer_size != desired_alsa_buffer_size) {
			fprintf(stderr, "Warning: buffer size is off, desired %li, actual %li\n",
				desired_alsa_buffer_size, buffer_size);
		}

		if (period_size != desired_alsa_period_size) {
			fprintf(stderr, "Warning: period size is off, desired %li, actual %li\n",
				desired_alsa_period_size, period_size);
		}
#endif

		snd_pcm_hw_params_free(hw_params);
		hw_params = NULL;

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

			snd_pcm_nonblock(pcm_handle, 0);

			return; /* success */
		}
	}

	/* clean up after failure */

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

LOCALFUNC blnr MySound_Init(void)
{
	MyBuffer = (ui3p)malloc(dbhBufferSize);
	if (NULL == MyBuffer) {
		return falseblnr;
	} else {
		MySound_Init0();
		return trueblnr;
	}
}

LOCALPROC MySound_UnInit(void)
{
	if (pcm_handle != NULL) {
		snd_pcm_close(pcm_handle);
		pcm_handle = NULL;
	}
	if (MyBuffer != NULL) {
		free(MyBuffer);
	}
}

GLOBALFUNC ui3p GetCurSoundOutBuff(void)
{
	if (NULL == pcm_handle) {
		return nullpr;
	} else {
		ui4b NextFillOffset = CurFillOffset + SOUND_LEN;
		ui4b ToPlaySize = NextFillOffset - CurPlayOffset;

		if (ToPlaySize < kBufferSize) {
			ui4b CurFillSeq = CurFillOffset >> kLnBuffSz;
			ui4b NextFillSeq = NextFillOffset >> kLnBuffSz;

			if (CurFillSeq != NextFillSeq) {
				MyMoveBytes((anyp)(MyBuffer + kBufferSize),
					(anyp)MyBuffer,
					NextFillOffset & kBufferMask);
			}

			CurFillOffset = NextFillOffset;
		}

		MySound_WriteOut();

		return MyBuffer + (CurFillOffset & kBufferMask);
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

/*--- basic dialogs ---*/

LOCALPROC CheckSavedMacMsg(void)
{
	if (nullpr != SavedBriefMsg) {
		char briefMsg0[ClStrMaxLength + 1];
		char longMsg0[ClStrMaxLength + 1];

		NativeStrFromCStr(briefMsg0, SavedBriefMsg);
		NativeStrFromCStr(longMsg0, SavedLongMsg);

		fprintf(stderr, briefMsg0);
		fprintf(stderr, "\n");
		fprintf(stderr, longMsg0);
		fprintf(stderr, "\n");

		SavedBriefMsg = nullpr;
	}
}

/*--- clipboard ---*/

#if IncludeHostTextClipExchange
LOCALVAR ui3p MyClipBuffer = NULL;
#endif

#if IncludeHostTextClipExchange
LOCALPROC FreeMyClipBuffer(void)
{
	if (MyClipBuffer != NULL) {
		free(MyClipBuffer);
		MyClipBuffer = NULL;
	}
}
#endif

#if IncludeHostTextClipExchange
GLOBALFUNC tMacErr HTCEexport(tPbuf i)
{
	tMacErr err = mnvm_miscErr;

	FreeMyClipBuffer();
	if (MacRomanTextToNativePtr(i, falseblnr,
		&MyClipBuffer))
	{
		XSetSelectionOwner(x_display, MyXA_CLIPBOARD,
			my_main_wind, CurrentTime);
		err = mnvm_noErr;
	}

	PbufDispose(i);

	return err;
}
#endif

#if IncludeHostTextClipExchange
LOCALFUNC blnr WaitForClipboardSelection(XEvent *xevent)
{
	struct timespec rqt;
	struct timespec rmt;
	int i;

	for (i = 100; --i >= 0; ) {
		while (XCheckTypedWindowEvent(x_display, my_main_wind,
			SelectionNotify, xevent))
		{
			if (xevent->xselection.selection != MyXA_CLIPBOARD) {
				/*
					not what we were looking for. lose it.
					(and hope it wasn't too important).
				*/
				WriteExtraErr("Discarding unwanted SelectionNotify");
			} else {
				/* this is our event */
				return trueblnr;
			}
		}

		rqt.tv_sec = 0;
		rqt.tv_nsec = 10000000;
		(void) nanosleep(&rqt, &rmt);
	}
	return falseblnr;
}
#endif

#if IncludeHostTextClipExchange
LOCALPROC HTCEimport_do(void)
{
	Window w = XGetSelectionOwner(x_display, MyXA_CLIPBOARD);

	if (w == my_main_wind) {
		/* We own the clipboard, already have MyClipBuffer */
	} else {
		FreeMyClipBuffer();
		if (w != None) {
			XEvent xevent;

			XDeleteProperty(x_display, my_main_wind,
				MyXA_MinivMac_Clip);
			XConvertSelection(x_display, MyXA_CLIPBOARD, XA_STRING,
				MyXA_MinivMac_Clip, my_main_wind, CurrentTime);

			if (WaitForClipboardSelection(&xevent)) {
				if (None == xevent.xselection.property) {
					/* oops, target not supported */
				} else {
					if (xevent.xselection.property != MyXA_MinivMac_Clip) {
						/* not where we expected it */
					} else {
						Atom ret_type;
						int ret_format;
						unsigned long ret_item;
						unsigned long remain_byte;
						unsigned char *s = NULL;

						if ((Success != XGetWindowProperty(x_display, my_main_wind,
							MyXA_MinivMac_Clip,
							0, 65535, False, AnyPropertyType, &ret_type,
							&ret_format, &ret_item, &remain_byte, &s))
							|| (ret_type != XA_STRING)
							|| (ret_format != 8)
							|| (NULL == s))
						{
							WriteExtraErr("XGetWindowProperty failed in HTCEimport_do");
						} else {
							MyClipBuffer = (ui3p)malloc(ret_item + 1);
							if (NULL == MyClipBuffer) {
								MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, falseblnr);
							} else {
								MyMoveBytes((anyp)s, (anyp)MyClipBuffer, ret_item);
								MyClipBuffer[ret_item] = 0;
							}
							XFree(s);
						}
					}
					XDeleteProperty(x_display, my_main_wind,
						MyXA_MinivMac_Clip);
				}
			}
		}
	}
}
#endif

#if IncludeHostTextClipExchange
GLOBALFUNC tMacErr HTCEimport(tPbuf *r)
{
	HTCEimport_do();

	return NativeTextToMacRomanPbuf((char *)MyClipBuffer, r);
}
#endif

#if IncludeHostTextClipExchange
LOCALFUNC blnr HandleSelectionRequestClipboard(XEvent *theEvent)
{
	blnr RequestFilled = falseblnr;

#if MyDbgEvents
	fprintf(stderr, "Requested MyXA_CLIPBOARD\n");
#endif

	if (NULL == MyClipBuffer) {
		/* our clipboard is empty */
	} else if (theEvent->xselectionrequest.target == MyXA_TARGETS) {
		Atom a[2];

		a[0] = MyXA_TARGETS;
		a[1] = XA_STRING;

		XChangeProperty(x_display,
			theEvent->xselectionrequest.requestor,
			theEvent->xselectionrequest.property,
			MyXA_TARGETS,
			32,
				/*
					most, but not all, other programs I've
					look at seem to use 8 here, but that
					can't be right. can it?
				*/
			PropModeReplace,
			(unsigned char *)a,
			sizeof(a) / sizeof(Atom));

		RequestFilled = trueblnr;
	} else if (theEvent->xselectionrequest.target == XA_STRING) {
		XChangeProperty(x_display,
			theEvent->xselectionrequest.requestor,
			theEvent->xselectionrequest.property,
			XA_STRING,
			8,
			PropModeReplace,
			(unsigned char *)MyClipBuffer,
			strlen((char *)MyClipBuffer));

		RequestFilled = trueblnr;
	}

	return RequestFilled;
}
#endif

/*--- drag and drop ---*/

#if EnableDragDrop
LOCALPROC MyActivateWind(Time time)
{
	if (NetSupportedContains(MyXA_NetActiveWindow)) {
		XEvent xevent;
		Window rootwin = XRootWindow(x_display,
			DefaultScreen(x_display));

		memset(&xevent, 0, sizeof (xevent));

		xevent.xany.type = ClientMessage;
		xevent.xclient.send_event = True;
		xevent.xclient.window = my_main_wind;
		xevent.xclient.message_type = MyXA_NetActiveWindow;
		xevent.xclient.format = 32;
		xevent.xclient.data.l[0] = 1;
		xevent.xclient.data.l[1]= time;

		if (0 == XSendEvent(x_display, rootwin, 0,
			SubstructureRedirectMask | SubstructureNotifyMask,
			&xevent))
		{
			WriteExtraErr("XSendEvent failed in MyActivateWind");
		}
	}

	XRaiseWindow(x_display, my_main_wind);
		/*
			In RedHat 7.1, _NET_ACTIVE_WINDOW supported,
			but XSendEvent of _NET_ACTIVE_WINDOW
			doesn't raise the window. So just always
			call XRaiseWindow. Hopefully calling
			XRaiseWindow won't do any harm on window
			managers where it isn't needed.
			(Such as in Ubuntu 5.10)
		*/
	XSetInputFocus(x_display, my_main_wind,
		RevertToPointerRoot, time);
		/* And call this always too, just in case */
}
#endif

#if EnableDragDrop
LOCALPROC ParseOneUri(char *s)
{
	/* printf("ParseOneUri %s\n", s); */
	if (('f' == s[0]) && ('i' == s[1]) && ('l' == s[2])
		&& ('e' == s[3]) && (':' == s[4]))
	{
		s += 5;
		if (('/' == s[0]) && ('/' == s[1])) {
			/* skip hostname */
			char c;

			s += 2;
			while ((c = *s) != '/') {
				if (0 == c) {
					return;
				}
				++s;
			}
		}
		(void) Sony_Insert1(s, falseblnr);
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
		if ('%' == c) {
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
		} else if (('\n' == c) || ('\r' == c)) {
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

#if EnableDragDrop
LOCALPROC HandleSelectionNotifyDnd(XEvent *theEvent)
{
	blnr DropOk = falseblnr;

#if MyDbgEvents
	fprintf(stderr, "Got MyXA_DndSelection\n");
#endif

	if ((theEvent->xselection.property == MyXA_MinivMac_DndXchng)
		&& (theEvent->xselection.target == MyXA_UriList))
	{
		Atom ret_type;
		int ret_format;
		unsigned long ret_item;
		unsigned long remain_byte;
		unsigned char *s = NULL;

		if ((Success != XGetWindowProperty(x_display, my_main_wind,
			MyXA_MinivMac_DndXchng,
			0, 65535, False, MyXA_UriList, &ret_type, &ret_format,
			&ret_item, &remain_byte, &s))
			|| (NULL == s))
		{
			WriteExtraErr("XGetWindowProperty failed in SelectionNotify");
		} else {
			ParseUriList((char *)s);
			DropOk = trueblnr;
			XFree(s);
		}
	} else {
		WriteExtraErr("Got Unknown SelectionNotify");
	}

	XDeleteProperty(x_display, my_main_wind,
		MyXA_MinivMac_DndXchng);

	if (PendingDragWindow != None) {
		XEvent xevent;

		memset(&xevent, 0, sizeof(xevent));

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

		if (0 == XSendEvent(x_display,
			PendingDragWindow, 0, 0, &xevent))
		{
			WriteExtraErr("XSendEvent failed in SelectionNotify");
		}
	}
	if (DropOk && gTrueBackgroundFlag) {
		MyActivateWind(theEvent->xselection.time);

		WantCmdOptOnReconnect = trueblnr;
	}
}
#endif

#if EnableDragDrop
LOCALPROC HandleClientMessageDndPosition(XEvent *theEvent)
{
	XEvent xevent;
	int xr;
	int yr;
	unsigned int dr;
	unsigned int wr;
	unsigned int hr;
	unsigned int bwr;
	Window rr;
	Window srcwin = theEvent->xclient.data.l[0];

#if MyDbgEvents
	fprintf(stderr, "Got XdndPosition\n");
#endif

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

	if (0 == XSendEvent(x_display, srcwin, 0, 0, &xevent)) {
		WriteExtraErr("XSendEvent failed in HandleClientMessageDndPosition");
	}
}
#endif

#if EnableDragDrop
LOCALPROC HandleClientMessageDndDrop(XEvent *theEvent)
{
	Time timestamp = theEvent->xclient.data.l[2];
	PendingDragWindow = (Window) theEvent->xclient.data.l[0];

#if MyDbgEvents
	fprintf(stderr, "Got XdndDrop\n");
#endif

	XConvertSelection(x_display, MyXA_DndSelection, MyXA_UriList,
		MyXA_MinivMac_DndXchng, my_main_wind, timestamp);
}
#endif

/*--- event handling for main window ---*/

LOCALPROC HandleTheEvent(XEvent *theEvent)
{
	if (theEvent->xany.display != x_display) {
		WriteExtraErr("Got event for some other display");
	} else switch(theEvent->type) {
		case KeyPress:
			if (theEvent->xkey.window != my_main_wind) {
				WriteExtraErr("Got KeyPress for some other window");
			} else {
#if MyDbgEvents
				fprintf(stderr, "- event - KeyPress\n");
#endif

				DoKeyCode(theEvent->xkey.keycode, trueblnr);
			}
			break;
		case KeyRelease:
			if (theEvent->xkey.window != my_main_wind) {
				WriteExtraErr("Got KeyRelease for some other window");
			} else {
#if MyDbgEvents
				fprintf(stderr, "- event - KeyRelease\n");
#endif

				DoKeyCode(theEvent->xkey.keycode, falseblnr);
			}
			break;
		case Expose:
			if (theEvent->xexpose.window != my_main_wind) {
				WriteExtraErr("Got SelectionRequest for some other window");
			} else {
				int x0 = theEvent->xexpose.x;
				int y0 = theEvent->xexpose.y;
				int x1 = x0 + theEvent->xexpose.width;
				int y1 = y0 + theEvent->xexpose.height;

#if 0 && MyDbgEvents
				fprintf(stderr, "- event - Expose\n");
#endif

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
				if (x1 > vMacScreenWidth) {
					x1 = vMacScreenWidth;
				}
				if (y0 < 0) {
					y0 = 0;
				}
				if (y1 > vMacScreenHeight) {
					y1 = vMacScreenHeight;
				}
				if ((x0 < x1) && (y0 < y1)) {
					HaveChangedScreenBuff(y0, x0, y1, x1);
				}
			}
			break;
#if IncludeHostTextClipExchange
		case SelectionRequest:
			if (theEvent->xselectionrequest.owner != my_main_wind) {
				WriteExtraErr("Got SelectionRequest for some other window");
			} else {
				XEvent xevent;
				blnr RequestFilled = falseblnr;

#if MyDbgEvents
				fprintf(stderr, "- event - SelectionRequest\n");
				WriteDbgAtom("selection", theEvent->xselectionrequest.selection);
				WriteDbgAtom("target", theEvent->xselectionrequest.target);
				WriteDbgAtom("property", theEvent->xselectionrequest.property);
#endif

				if (theEvent->xselectionrequest.selection == MyXA_CLIPBOARD) {
					RequestFilled = HandleSelectionRequestClipboard(theEvent);
				}


				memset(&xevent, 0, sizeof(xevent));
				xevent.xselection.type = SelectionNotify;
				xevent.xselection.display = x_display;
				xevent.xselection.requestor = theEvent->xselectionrequest.requestor;
				xevent.xselection.selection = theEvent->xselectionrequest.selection;
				xevent.xselection.target = theEvent->xselectionrequest.target;
				xevent.xselection.property = (! RequestFilled) ? None
					: theEvent->xselectionrequest.property ;
				xevent.xselection.time = theEvent->xselectionrequest.time;

				if (0 == XSendEvent(x_display, xevent.xselection.requestor,
					False, 0, &xevent))
				{
					WriteExtraErr("XSendEvent failed in SelectionRequest");
				}
			}
			break;
		case SelectionClear:
			if (theEvent->xselectionclear.window != my_main_wind) {
				WriteExtraErr("Got SelectionClear for some other window");
			} else {
#if MyDbgEvents
				fprintf(stderr, "- event - SelectionClear\n");
				WriteDbgAtom("selection", theEvent->xselectionclear.selection);
#endif

				if (theEvent->xselectionclear.selection == MyXA_CLIPBOARD) {
					FreeMyClipBuffer();
				}
			}
			break;
#endif
#if EnableDragDrop
		case SelectionNotify:
			if (theEvent->xselection.requestor != my_main_wind) {
				WriteExtraErr("Got SelectionNotify for some other window");
			} else {
#if MyDbgEvents
				fprintf(stderr, "- event - SelectionNotify\n");
				WriteDbgAtom("selection", theEvent->xselection.selection);
				WriteDbgAtom("target", theEvent->xselection.target);
				WriteDbgAtom("property", theEvent->xselection.property);
#endif

				if (theEvent->xselection.selection == MyXA_DndSelection) {
					HandleSelectionNotifyDnd(theEvent);
				} else {
					WriteExtraErr("Got Unknown selection in SelectionNotify");
				}
			}
			break;
#endif
		case ClientMessage:
			if (theEvent->xclient.window != my_main_wind) {
				WriteExtraErr("Got ClientMessage for some other window");
			} else {
#if MyDbgEvents
				fprintf(stderr, "- event - ClientMessage\n");
				WriteDbgAtom("message_type", theEvent->xclient.message_type);
#endif

#if EnableDragDrop
				if (theEvent->xclient.message_type == MyXA_DndEnter) {
					/* printf("Got XdndEnter\n"); */
				} else if (theEvent->xclient.message_type == MyXA_DndLeave) {
					/* printf("Got XdndLeave\n"); */
				} else if (theEvent->xclient.message_type == MyXA_DndPosition) {
					HandleClientMessageDndPosition(theEvent);
				} else if (theEvent->xclient.message_type == MyXA_DndDrop) {
					HandleClientMessageDndDrop(theEvent);
				} else
#endif
				{
					if ((32 == theEvent->xclient.format) &&
						(theEvent->xclient.data.l[0] == MyXA_DeleteW))
					{
						/*
							I would think that should test that
								WM_PROTOCOLS == message_type
							but none of the other programs I looked
							at did.
						*/
						RequestMacOff = trueblnr;
					}
				}
			}
			break;
		case FocusIn:
			if (theEvent->xfocus.window != my_main_wind) {
				WriteExtraErr("Got FocusIn for some other window");
			} else {
#if MyDbgEvents
				fprintf(stderr, "- event - FocusIn\n");
#endif

				gTrueBackgroundFlag = falseblnr;
			}
			break;
		case FocusOut:
			if (theEvent->xfocus.window != my_main_wind) {
				WriteExtraErr("Got FocusOut for some other window");
			} else {
#if MyDbgEvents
				fprintf(stderr, "- event - FocusOut\n");
#endif

				gTrueBackgroundFlag = trueblnr;
			}
			break;
		default:
			break;
	}
}

/*--- main window creation and disposal ---*/

LOCALVAR int my_argc;
LOCALVAR char **my_argv;

LOCALVAR char *display_name = NULL;

LOCALFUNC blnr Screen_Init(void)
{
	Window rootwin;
	int screen;
	Colormap Xcmap;
	Visual *Xvisual;
	char *image_Mem1;

	x_display = XOpenDisplay(display_name);
	if (NULL == x_display) {
		fprintf(stderr, "Cannot connect to X server.\n");
		return falseblnr;
	}

	screen = DefaultScreen(x_display);

	rootwin = XRootWindow(x_display, screen);

	Xcmap = DefaultColormap(x_display, screen);

	Xvisual = DefaultVisual(x_display, screen);

	LoadMyXA();

	XParseColor(x_display, Xcmap, "#000000", &x_black);
	if (! XAllocColor(x_display, Xcmap, &x_black)) {
		WriteExtraErr("XParseColor black fails");
	}
	XParseColor(x_display, Xcmap, "#ffffff", &x_white);
	if (! XAllocColor(x_display, Xcmap, &x_white)) {
		WriteExtraErr("XParseColor white fails");
	}

	if (! CreateMyBlankCursor(rootwin)) {
		return falseblnr;
	}

#if IncludeVidMem
	VidMem = (ui3p)malloc(kVidMemRAM_Size + RAMSafetyMarginFudge);
	if (NULL == VidMem) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	}
	SetLongs((ui5b *)VidMem, (kVidMemRAM_Size + RAMSafetyMarginFudge) / 4);
#endif

	image_Mem1 = (char *)calloc(1, vMacScreenNumBytes);
	if (NULL == image_Mem1) {
		fprintf(stderr, "calloc failed.\n");
		return falseblnr;
	}
	SetLongs((ui5b *)image_Mem1, vMacScreenNumBytes / 4);

	my_image = XCreateImage(x_display, Xvisual, 1, XYBitmap, 0,
		(char *)image_Mem1,
		vMacScreenWidth, vMacScreenHeight, 32, vMacScreenByteWidth);
	if (NULL == my_image) {
		fprintf(stderr, "XCreateImage failed.\n");
		return falseblnr;
	}

	my_image->bitmap_bit_order = MSBFirst;
	my_image->byte_order = MSBFirst;
	screencomparebuff = my_image->data;

#if EnableScalingBuff
	image_Mem1 = (char *)malloc((long)vMacScreenNumBytes * MyWindowScale * MyWindowScale);
	if (NULL == image_Mem1) {
		fprintf(stderr, "malloc failed.\n");
		return falseblnr;
	}

	my_scaled_image = XCreateImage(x_display, Xvisual, 1, XYBitmap, 0,
		(char *)image_Mem1,
		vMacScreenWidth * MyWindowScale, vMacScreenHeight * MyWindowScale, 32, vMacScreenByteWidth * MyWindowScale);
	if (NULL == my_scaled_image) {
		fprintf(stderr, "XCreateImage failed.\n");
		return falseblnr;
	}

	my_scaled_image->bitmap_bit_order = MSBFirst;
	my_scaled_image->byte_order = MSBFirst;
	ScalingBuff = my_scaled_image->data;
#endif

#if UseControlKeys
	image_Mem1 = (char *)malloc(vMacScreenNumBytes);
	if (NULL == image_Mem1) {
		fprintf(stderr, "malloc failed.\n");
		return falseblnr;
	}

	my_cntrl_image = XCreateImage(x_display, Xvisual, 1, XYBitmap, 0,
		(char *)image_Mem1,
		vMacScreenWidth, vMacScreenHeight, 32, vMacScreenByteWidth);
	if (NULL == my_cntrl_image) {
		fprintf(stderr, "XCreateImage failed.\n");
		return falseblnr;
	}

	my_cntrl_image->bitmap_bit_order = MSBFirst;
	my_cntrl_image->byte_order = MSBFirst;
	CntrlDisplayBuff = my_cntrl_image->data;
#endif

	DisableKeyRepeat();

	return trueblnr;
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
	Window rr2;
	int leftPos;
	int topPos;
	int WinIndx;
	Window NewMainWindow;
	GC new_gc = NULL;
#if IncludeHostTextClipExchange
	blnr OwnClipboard = falseblnr;
#endif
#if EnableDragDrop
	long int xdnd_version = 5;
#endif
	int NewWindowHeight = vMacScreenHeight;
	int NewWindowWidth = vMacScreenWidth;

	/* Get connection to X Server */
	screen = DefaultScreen(x_display);

	rootwin = XRootWindow(x_display, screen);

	XGetGeometry(x_display, rootwin,
		&rr, &xr, &yr, &wr, &hr, &bwr, &dr);

#if EnableFullScreen
	if (! UseFullScreen)
#endif
	{
		/* save old position */
		if (my_main_wind) {

			/*
				Couldn't reliably find out where window
				is now, due to what seem to be some
				broken X implementations, and so instead
				track how far window has moved.
			*/
			XSync(x_display, 0);
			if (XTranslateCoordinates(x_display, my_main_wind, rootwin,
				0, 0, &xr, &yr, &rr2))
			{
				int newposh = WinPositionWinsH[CurWinIndx] + (xr - SavedTransH);
				int newposv = WinPositionWinsV[CurWinIndx] + (yr - SavedTransV);
				if ((newposv > 0) && (newposv < hr) && (newposh < wr)) {
					WinPositionWinsH[CurWinIndx] = newposh;
					WinPositionWinsV[CurWinIndx] = newposv;
					SavedTransH = xr;
					SavedTransV = yr;
				}
			}
		}
	}

#if EnableMagnify
	if (WantMagnify) {
		NewWindowHeight *= MyWindowScale;
		NewWindowWidth *= MyWindowScale;
	}
#endif

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

#if EnableFullScreen
	if (WantFullScreen) {
		XSetWindowAttributes xattr;
		xattr.override_redirect = True;
		xattr.background_pixel = x_black.pixel;
		xattr.border_pixel = x_white.pixel;

		NewMainWindow = XCreateWindow(x_display, rr,
			0, 0, wr, hr, 0,
			CopyFromParent, /* depth */
			InputOutput, /* class */
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
		WriteExtraErr("XCreateSimpleWindow failed.");
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
		if (NULL == new_gc) {
			WriteExtraErr("XCreateGC failed.");
			XDestroyWindow(x_display, NewMainWindow);
			return falseblnr;
		}
		XSetState(x_display, new_gc, x_black.pixel, x_white.pixel, GXcopy, AllPlanes);
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

			/*
				Try again to say where the window ought to go.
				I've seen this described as obsolete, but it
				seems to work on all x implementations tried
				so far, and nothing else does.
			*/
			hints->x = leftPos;
			hints->y = topPos;
			hints->width = NewWindowWidth;
			hints->height = NewWindowHeight;

			hints->flags = PMinSize | PMaxSize | PPosition | PSize;
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

#if IncludeHostTextClipExchange
	if (my_main_wind) {
		if (XGetSelectionOwner(x_display, MyXA_CLIPBOARD) == my_main_wind) {
			OwnClipboard = trueblnr;
		}
	}
#endif

	DisposeMainWindow();

	my_main_wind = NewMainWindow;
	my_gc = new_gc;
#if EnableMagnify
	UseMagnify = WantMagnify;
#endif
#if EnableFullScreen
	UseFullScreen = WantFullScreen;
#endif

	DisconnectKeyCodes3();
		/* since will lose keystrokes to old window */

	CurWinIndx = WinIndx;

	XMapRaised(x_display, my_main_wind);

	XSync(x_display, 0);

#if 1
	/*
		This helps in Red Hat 9 to get the new window
		activated, and I've seen other programs
		do similar things.
	*/
	{
		XEvent event;

		do {
			XNextEvent(x_display, &event);
			HandleTheEvent(&event);
		} while (! ((Expose == event.type) && (event.xexpose.window == my_main_wind)));
	}
#endif

#if EnableFullScreen
	if (WantFullScreen) {
		XSetInputFocus(x_display, my_main_wind,
			RevertToPointerRoot, CurrentTime);
	} else
#endif
	{
#if 0
		/*
			This doesn't work right in Red Hat 6, and may not
			be needed anymore, now that using PPosition hint.
		*/
		XMoveWindow(x_display, my_main_wind,
			leftPos, topPos);
			/*
				Needed after XMapRaised, because some window
				managers will apparently ignore where the
				window was asked to be put.
			*/

		XSync(x_display, 0);
			/*
				apparently, XTranslateCoordinates can be inaccurate
				without this
			*/
#endif
		XTranslateCoordinates(x_display, my_main_wind, rootwin,
			0, 0, &SavedTransH, &SavedTransV, &rr);
	}

#if IncludeHostTextClipExchange
	if (OwnClipboard) {
		XSetSelectionOwner(x_display, MyXA_CLIPBOARD,
			my_main_wind, CurrentTime);
	}
#endif

	return trueblnr;
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
LOCALPROC ToggleWantFullScreen(void)
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
			WantMagnify = (kMagStateMagnifgy == NewMagState);
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

/*--- SavedTasks ---*/

LOCALPROC LeaveBackground(void)
{
	ReconnectKeyCodes3();
	DisableKeyRepeat();
}

LOCALPROC EnterBackground(void)
{
	RestoreKeyRepeat();
	DisconnectKeyCodes3();

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

#if EnableFullScreen
LOCALVAR blnr GrabMachine = falseblnr;
#endif

LOCALPROC CheckForSavedTasks(void)
{
	if (RequestMacOff) {
		RequestMacOff = falseblnr;
		if (AnyDiskInserted()) {
			MacMsgOverride(kStrQuitWarningTitle,
				kStrQuitWarningMessage);
		} else {
			ForceMacOff = trueblnr;
		}
	}

	if (ForceMacOff) {
		return;
	}

	if (gTrueBackgroundFlag != gBackgroundFlag) {
		gBackgroundFlag = gTrueBackgroundFlag;
		if (gTrueBackgroundFlag) {
			EnterBackground();
		} else {
			LeaveBackground();
		}
	}

	if (CurSpeedStopped != (SpeedStopped ||
		(gBackgroundFlag && ! RunInBackground)))
	{
		CurSpeedStopped = ! CurSpeedStopped;
		if (CurSpeedStopped) {
			EnterSpeedStopped();
		} else {
			LeaveSpeedStopped();
		}
	}

#if EnableFullScreen
	if (gTrueBackgroundFlag && WantFullScreen) {
		/*
			Since often get here on Ubuntu Linux 5.10
			running on a slow machine (emulated) when
			attempt to enter full screen, don't abort
			full screen, but try to fix it.
		*/
#if 0
		ToggleWantFullScreen();
#else
		XRaiseWindow(x_display, my_main_wind);
		XSetInputFocus(x_display, my_main_wind,
			RevertToPointerRoot, CurrentTime);
#endif
	}
#endif

#if EnableMagnify || EnableFullScreen
	CheckMagnifyAndFullScreen();
#endif


#if EnableFullScreen
	if (GrabMachine != (UseFullScreen && ! (gTrueBackgroundFlag || CurSpeedStopped))) {
		GrabMachine = ! GrabMachine;
		if (GrabMachine) {
			GrabTheMachine();
		} else {
			UnGrabTheMachine();
		}
	}
#endif

#if IncludeSonyNew
	if (vSonyNewDiskWanted) {
#if IncludeSonyNameNew
		if (vSonyNewDiskName != NotAPbuf) {
			ui3p NewDiskNameDat;
			if (MacRomanTextToNativePtr(vSonyNewDiskName, trueblnr,
				&NewDiskNameDat))
			{
				MakeNewDisk(vSonyNewDiskSize, (char *)NewDiskNameDat);
				free(NewDiskNameDat);
			}
			PbufDispose(vSonyNewDiskName);
			vSonyNewDiskName = NotAPbuf;
		} else
#endif
		{
			MakeNewDiskAtDefault(vSonyNewDiskSize);
		}
		vSonyNewDiskWanted = falseblnr;
			/* must be done after may have gotten disk */
	}
#endif

	if ((nullpr != SavedBriefMsg) & ! MacMsgDisplayed) {
		MacMsgDisplayOn();
	}

	if (NeedWholeScreenDraw) {
		NeedWholeScreenDraw = falseblnr;
		HaveChangedScreenBuff(0, 0, vMacScreenHeight, vMacScreenWidth);
	}

	/* CheckSavedMacMsg(); */

	if (HaveCursorHidden != (WantCursorHidden
		&& ! (gTrueBackgroundFlag || CurSpeedStopped)))
	{
		HaveCursorHidden = ! HaveCursorHidden;
		if (HaveCursorHidden) {
			XDefineCursor(x_display, my_main_wind, blankCursor);
		} else {
			XUndefineCursor(x_display, my_main_wind);
		}
	}
}

/*--- command line parsing ---*/

LOCALFUNC blnr ScanCommandLine(void)
{
	int i;

	for (i = 1; i < my_argc; ++i) {
		if ('-' == my_argv[i][0]) {
			if ((0 == strcmp(my_argv[i], "--display")) ||
				(0 == strcmp(my_argv[i], "-display")))
			{
				++i;
				if (i < my_argc) {
					display_name = my_argv[i];
				}
			} else if ((0 == strcmp(my_argv[i], "--rom")) ||
				(0 == strcmp(my_argv[i], "-r")))
			{
				++i;
				if (i < my_argc) {
					rom_path = my_argv[i];
				}
			} else
#if MySoundEnabled
			if ((0 == strcmp(my_argv[i], "--alsadev")) ||
				(0 == strcmp(my_argv[i], "-alsadev")))
			{
				++i;
				if (i < my_argc) {
					alsadev_name = my_argv[i];
				}
			} else
#endif
			if (0 == strcmp(my_argv[i], "-l")) {
				SpeedValue = 0;
			} else {
				MacMsg(kStrBadArgTitle, kStrBadArgMessage, falseblnr);
			}
		} else {
			(void) Sony_Insert1(my_argv[i], falseblnr);
		}
	}
	return trueblnr;
}

/*--- main program flow ---*/

LOCALVAR ui5b OnTrueTime = 0;

GLOBALFUNC blnr ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	return TrueEmulatedTime == OnTrueTime;
}

LOCALPROC RunEmulatedTicksToTrueTime(void)
{
	si3b n;

	if (CheckDateTime()) {
#if MySoundEnabled
		MySound_SecondNotify();
#endif
	}

	n = OnTrueTime - CurEmulatedTime;
	if (n > 0) {
		if (n > 8) {
			/* emulation not fast enough */
			n = 8;
			CurEmulatedTime = OnTrueTime - n;
		}

		if (! gBackgroundFlag) {
			CheckMouseState();
		}

		do {
			DoEmulateOneTick();
			++CurEmulatedTime;
		} while (ExtraTimeNotOver()
			&& (--n > 0));

		Screen_Draw(n);
	}
}

LOCALPROC RunOnEndOfSixtieth(void)
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

	OnTrueTime = TrueEmulatedTime;
	RunEmulatedTicksToTrueTime();
}

LOCALPROC WaitForTheNextEvent(void)
{
	XEvent event;

	XNextEvent(x_display, &event);
	HandleTheEvent(&event);
}

LOCALPROC CheckForSystemEvents(void)
{
	int i = 10;

	while ((XEventsQueued(x_display, QueuedAfterReading) > 0) && (--i >= 0)) {
		WaitForTheNextEvent();
	}
	XFlush(x_display);
}

LOCALPROC MainEventLoop(void)
{
	for (; ; ) {
		CheckForSystemEvents();
		CheckForSavedTasks();
		if (ForceMacOff) {
			return;
		}

		if (CurSpeedStopped) {
			WaitForTheNextEvent();
		} else {
			DoEmulateExtraTime();
			RunOnEndOfSixtieth();
		}
	}
}

LOCALPROC ZapOSGLUVars(void)
{
	InitDrives();
	{
		int i;

		for (i = 0; i < kNumMagStates; ++i) {
			HavePositionWins[i] = falseblnr;
		}
#if EnableFullScreen && EnableMagnify
		for (i = 0; i < kNumWinStates; ++i) {
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
	if (AllocateMacRAM())
#if MySoundEnabled
	if (MySound_Init())
#endif
	if (Screen_Init())
	if (ReCreateMainWindow())
	if (KC2MKCInit())
	if (InitEmulation())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}

	CheckSavedMacMsg();

	RestoreKeyRepeat();
#if EnableFullScreen
	UnGrabTheMachine();
#endif
#if MySoundEnabled && (! MySoundFullScreenOnly)
	MySound_Stop();
#endif
#if MySoundEnabled
	MySound_UnInit();
#endif
#if IncludeHostTextClipExchange
	FreeMyClipBuffer();
#endif
#if IncludePbufs
	UnInitPbufs();
#endif
	UnInitDrives();

	ForceShowCursor();
	if (blankCursor != None) {
		XFreeCursor(x_display, blankCursor);
	}
	if (my_image != NULL) {
		XDestroyImage(my_image);
	}
#if UseControlKeys
	if (my_cntrl_image != NULL) {
		XDestroyImage(my_cntrl_image);
	}
#endif
#if EnableScalingBuff
	if (my_scaled_image != NULL) {
		XDestroyImage(my_scaled_image);
	}
#endif
	DisposeMainWindow();
	if (x_display != NULL) {
		XCloseDisplay(x_display);
	}
#if IncludeVidMem
	if (NULL != VidMem) {
		free((char *)VidMem);
	}
#endif

	DeallocateMacRAM();
	DeallocateMacROM();
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
