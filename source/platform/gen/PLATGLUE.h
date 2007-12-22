/*
	PLATGLUE.h

	Copyright (C) 2006 Paul C. Pratt

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
	PLATform GLUE. (stub)

	If the target operating system hasn't been specified,
	this file is used. It can be used at the starting
	point for a port to a different operating system.

	The main entry point 'main' is at the end of this file.
*/


/*
	The comments in this file document how the platform
	dependent code should operate. Most of them should
	be removed from a finished implementation for a
	specific platform, to avoid needing to maintain
	redundant versions.
*/


/*
	If the C standard libraries are available, then
	can define UseStdCLibs to 1, at least to get
	started with.
*/

#ifndef UseStdCLibs
#define UseStdCLibs 1
#endif

/*
	If have the function gettimeofday available,
	then can define Use_gettimeofday to 1, at
	least to get started with.

	If Use_gettimeofday is defined as 0, then
	UpdateTrueEmulatedTime must be implemented
	in some other way immediately. Otherwise the
	program will go into an infine loop.
*/

#ifndef Use_gettimeofday
#define Use_gettimeofday 1
#endif

/*
	If UseStdCLibs and Use_gettimeofday can both be
	defined as 1, then this stub gives a working
	emulator as is. If passed a disk image as
	a command line argument, and this disk image
	is set up so that autoquit gets called with
	no user intervention, then the program will
	quit automatically. Otherwise the program
	will never quit.

	The next step could be to implement
	HaveChangedScreenBuff, so that you can see
	what the emulation is doing. This probably
	requires setting up a window to draw into
	(in ReCreateMainWindow).
*/

/*--- some simple utilities ---*/

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
#if UseStdCLibs
	(void) memcpy((char *)destPtr, (char *)srcPtr, byteCount);
#else
	/*
		IMPLEMENT ME sometime
		usually the following code can be replaced by some
		more efficient platform specific utility.
	*/
	si5b i;
	ui3b *s = (ui3b *)srcPtr;
	ui3b *d = (ui3b *)destPtr;

	for (i = byteCount; --i >= 0; ) {
		*d++ = *s++;
	}
#endif
}

/*--- control mode and internationalization ---*/

#define NeedCell2PlainAsciiMap 1

#include "CONTROLM.h"
	/*
		implements the control mode user interface.
		Using this is not required, but all current
		ports do.
	*/

/*--- debug settings and utilities ---*/

/*
	code to help debugging can go here
*/

/*--- information about the environment ---*/

/*
	code to determine characteristics
	of the current run time environment
	can go here.
*/

/*--- paramter buffers ---*/

#if IncludePbufs
LOCALVAR void *PbufDat[NumPbufs];
#endif

#if IncludePbufs && UseStdCLibs
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

#if UseStdCLibs
	void *p = calloc(1, count);
	if (NULL != p) {
		err = PbufNewFromPtr(p, count, r);
	}
#endif

	return err;
}
#endif

#if IncludePbufs
GLOBALPROC PbufDispose(tPbuf i)
{
#if UseStdCLibs
	free(PbufDat[i]);
#endif
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
#if UseStdCLibs
	void *p = ((ui3p)PbufDat[i]) + offset;
	if (IsWrite) {
		(void) memcpy(p, Buffer, count);
	} else {
		(void) memcpy(Buffer, p, count);
	}
#endif
}
#endif

/*--- text translation ---*/

LOCALFUNC tMacErr NativeTextToMacRomanPbuf(char *x, tPbuf *r)
{
#if UseStdCLibs
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
				if (v == 10) {
					v = 13;
				}
				*p1++ = v;
			}

			return PbufNewFromPtr(p, L, r);
		}
	}
#else
	return mnvm_miscErr;
#endif
}

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
				switch (x) {
					case 13:
					case '/':
					case '<':
					case '>':
					case '|':
						x = '?';
					default:
						break;
				}
				*p1++ = x;
			}
			if ('.' == p[0]) {
				p[0] = '?';
			}
		} else {
			for (j = L; --j >= 0; ) {
				ui3b x = *p0++;
				if (x == 13) {
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

#if UseStdCLibs
#define OpenFileRef FILE *
#else
/*
	IMPLEMENT ME
	define OpenFileRef to a type that
	represents an open file.
*/
#define OpenFileRef void *
#endif

#if UseStdCLibs
#define NotAfileRef NULL
#else
/*
	IMPLEMENT ME
	define NotAfileRef to some value that is different
	from any valid OpenFileRef.
*/
#define NotAfileRef ((void *) 0)
#endif

LOCALVAR OpenFileRef Drives[NumDrives]; /* open disk image files */
#if (IncludeSonyGetName || IncludeSonyNew) && UseStdCLibs
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
#if (IncludeSonyGetName || IncludeSonyNew) && UseStdCLibs
		DriveNames[i] = NULL;
#endif
	}
}

GLOBALFUNC tMacErr vSonyRead(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count)
{
#if UseStdCLibs
	fseek(Drives[Drive_No], Sony_Start, SEEK_SET);
	fread(Buffer, 1, *Sony_Count, Drives[Drive_No]);
	return mnvm_noErr;
#else
	/*
		IMPLEMENT ME
		read Sony_Count bytes into Buffer, from disk image number Drive_No,
		starting at offset Sony_Start.

		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/
	UnusedParam(Buffer);
	UnusedParam(Drive_No);
	UnusedParam(Sony_Start);
	UnusedParam(Sony_Count);
	return mnvm_miscErr;
#endif
}

GLOBALFUNC tMacErr vSonyWrite(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count)
{
#if UseStdCLibs
	fseek(Drives[Drive_No], Sony_Start, SEEK_SET);
	fwrite(Buffer, 1, *Sony_Count, Drives[Drive_No]);
	return mnvm_noErr;
#else
	/*
		IMPLEMENT ME
		write Sony_Count bytes from Buffer, to disk image number Drive_No,
		starting at offset Sony_Start.

		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/
	UnusedParam(Buffer);
	UnusedParam(Drive_No);
	UnusedParam(Sony_Start);
	UnusedParam(Sony_Count);
	return mnvm_miscErr;
#endif
}

GLOBALFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
#if UseStdCLibs
	fseek(Drives[Drive_No], 0, SEEK_END);
	*Sony_Count = ftell(Drives[Drive_No]);
	return mnvm_noErr;
#else
	/*
		IMPLEMENT ME
		set Sony_Count to the size of disk image number Drive_No.

		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/
	UnusedParam(Drive_No);
	UnusedParam(Sony_Count);
	return mnvm_miscErr;
#endif
}

LOCALFUNC tMacErr vSonyEject0(tDrive Drive_No, blnr deleteit)
{
	DiskEjectedNotify(Drive_No);

#if UseStdCLibs
	fclose(Drives[Drive_No]);
#else
	/*
		IMPLEMENT ME
		close disk image number Drive_No.

		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/
#endif
	Drives[Drive_No] = NotAfileRef; /* not really needed */

#if (IncludeSonyGetName || IncludeSonyNew) && UseStdCLibs
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
#if UseStdCLibs
	char *drivepath = DriveNames[Drive_No];
	if (NULL == drivepath) {
		return mnvm_miscErr;
	} else {
		char *s = strrchr(drivepath, '/');
		if (s == NULL) {
			s = drivepath;
		} else {
			s++;
		}
		return NativeTextToMacRomanPbuf(s, r);
	}
#else
	return mnvm_miscErr;
#endif
}
#endif

LOCALFUNC blnr Sony_Insert0(OpenFileRef refnum, blnr locked,
	char *drivepath)
{
	/*
		Given reference to open file, mount it as
		a disk image file. if "locked", then mount
		it as a locked disk.
	*/

	tDrive Drive_No;

	if (! FirstFreeDisk(&Drive_No)) {
#if UseStdCLibs
		fclose(refnum);
#else
		/* IMPLEMENT ME */
#endif
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage, falseblnr);
		return falseblnr;
	} else {
		/* printf("Sony_Insert0 %d\n", (int)Drive_No); */
		Drives[Drive_No] = refnum;
		DiskInsertNotify(Drive_No, locked);

#if (IncludeSonyGetName || IncludeSonyNew) && UseStdCLibs
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

#if UseStdCLibs
LOCALFUNC blnr Sony_Insert1(char *drivepath, blnr silentfail)
{
	blnr locked = falseblnr;
	/* printf("Sony_Insert1 %s\n", drivepath); */
	FILE *refnum = fopen(drivepath, "rb+");
	if (refnum == NULL) {
		locked = trueblnr;
		refnum = fopen(drivepath, "rb");
	}
	if (refnum == NULL) {
		if (! silentfail) {
			MacMsg(kStrOpenFailTitle, kStrOpenFailMessage, falseblnr);
		}
	} else {
		return Sony_Insert0(refnum, locked, drivepath);
	}
	return falseblnr;
}
#endif

LOCALFUNC blnr LoadInitialImages(void)
{
#if UseStdCLibs
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
#else
	/* IMPLEMENT ME sometime */
#endif

	return trueblnr;
}

#if IncludeSonyNew && UseStdCLibs
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

#if IncludeSonyNew && UseStdCLibs
LOCALPROC MakeNewDisk(ui5b L, char *drivepath)
{
	blnr IsOk = falseblnr;
	FILE *refnum = fopen(drivepath, "wb+");
	if (refnum == NULL) {
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

#if IncludeSonyNew && UseStdCLibs
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
#if UseStdCLibs
	ROM = (ui3p)calloc(1, kROM_Size);
	if (ROM == NULL) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
#else
	/*
		IMPLEMENT ME
		allocate kROM_Size bytes of memory, address in ptr ROM
	*/
	return falseblnr;
#endif
}

LOCALPROC DeallocateMacROM(void)
{
#if UseStdCLibs
	if (NULL != ROM) {
		free((char *)ROM);
	}
#else
	/* IMPLEMENT ME */
#endif
}

#if UseStdCLibs
LOCALVAR char *rom_path = NULL;
#endif

LOCALFUNC blnr LoadMacRom(void)
{
#if UseStdCLibs
	FILE *ROM_File;
	int File_Size;

	if (rom_path == NULL) {
		rom_path = RomFileName;
	}
	ROM_File = fopen(rom_path, "rb");
	if (ROM_File == NULL) {
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
#else
	/*
		IMPLEMENT ME
		load the ROM image file into ptr ROM
	*/
	return falseblnr;
#endif
}

/*--- RAM ---*/

LOCALFUNC blnr AllocateMacRAM(void)
{
#if UseStdCLibs
	RAM = (ui3p)calloc(1, kRAM_Size + RAMSafetyMarginFudge);
	if (RAM == NULL) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
#else
	/*
		IMPLEMENT ME
		allocate (kRAM_Size + RAMSafetyMarginFudge) bytes of memory, address in RAM
	*/
	return falseblnr;
#endif
}

LOCALPROC DeallocateMacRAM(void)
{
#if UseStdCLibs
	if (NULL != RAM) {
		free((char *)RAM);
	}
#else
	/* IMPLEMENT ME */
#endif
}

/*--- video out ---*/

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

LOCALPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right)
{
	char *p = GetCurDrawBuff();

	/*
		p now points to a bitmap (8 pixels per byte) of height vMacScreenHeight
		and width vMacScreenWidth.
	*/

	/*
		IMPLEMENT ME
		the part of this bitmap bounded by the rectangle (top, left, bottom, right)
		should now be drawn somewhere the user can see it.
	*/
		(void) p; /* avoid compile warning that not used */

	/*
		IMPLEMENT ME sometime
		UseMagnify and UseFullScreen can eventually be implemented, but may
		be ignored to start with.
	*/
}

/*--- mouse ---*/

/* cursor hiding */

LOCALPROC MyHideCursor(void)
{
	/*
		IMPLEMENT ME sometime
		make the platform specific cursor invisible,
		assuming it is currently visible.
	*/
}

LOCALPROC MyShowCursor(void)
{
	/*
		IMPLEMENT ME sometime
		undo effects of MyHideCursor.
		make the platform specific cursor visible,
		assuming it is currently invisible.
	*/
}

LOCALVAR blnr HaveCursorHidden = falseblnr;
LOCALVAR blnr WantCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		MyShowCursor();
	}
}

/* cursor moving */

/*
	When "EnableMouseMotion" the platform
	specific code can get relative mouse
	motion, instead of absolute coordinates
	on the emulated screen. It should
	set HaveMouseMotion to true when
	it is doing this (normally when in
	full screen mode.)

	This can usually be implemented by
	hiding the platform specific cursor,
	and then keeping it within a box,
	moving the cursor back to the center whenever
	it leaves the box. This requires the
	ability to move the cursor (in MyMoveMouse).
	The code below implements EnableMouseMotion
	this way, but it can be replaced by
	some other technique if needed.
*/

#if EnableMouseMotion
LOCALFUNC blnr MyMoveMouse(si4b h, si4b v)
{
	/*
		IMPLEMENT ME sometime
		Move the cursor to the point h, v
		on the emulated screen.
		if detect that this fails return falseblnr,
		otherwise return trueblnr.
		(on some platforms it is possible to
		move the curser, but there is no
		way to detect failure.)
	*/

	/*
		To begin with, EnableMouseMotion
			may be defined 0
	*/
	return falseblnr;
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

#if EnableMouseMotion
LOCALPROC MyMouseConstrain(void)
{
	si4b shiftdh;
	si4b shiftdv;

	if (SavedMouseH < vMacScreenWidth / 4) {
		shiftdh = vMacScreenWidth / 2;
	} else if (SavedMouseH > vMacScreenWidth - vMacScreenWidth / 4) {
		shiftdh = - vMacScreenWidth / 2;
	} else {
		shiftdh = 0;
	}
	if (SavedMouseV < vMacScreenHeight / 4) {
		shiftdv = vMacScreenHeight / 2;
	} else if (SavedMouseV > vMacScreenHeight - vMacScreenHeight / 4) {
		shiftdv = - vMacScreenHeight / 2;
	} else {
		shiftdv = 0;
	}
	if ((shiftdh != 0) || (shiftdv != 0)) {
		SavedMouseH += shiftdh;
		SavedMouseV += shiftdv;
		if (! MyMoveMouse(SavedMouseH, SavedMouseV)) {
			HaveMouseMotion = falseblnr;
		}
	}
}
#endif

LOCALVAR blnr CurTrueMouseButton = falseblnr;

LOCALPROC CheckMouseState(void)
{
	blnr ShouldHaveCursorHidden;
	ui3b NewMouseButton;
	int NewMousePosh;
	int NewMousePosv;

	ShouldHaveCursorHidden = trueblnr;

	/*
		Determine the position of the
		cursor on the emulated screen.
	*/
		NewMousePosh = 0;
		NewMousePosv = 0;

	/* Determinie the current mouse state. */
		NewMouseButton = falseblnr;

#if EnableMouseMotion
	if (HaveMouseMotion) {
		MyMousePositionSetDelta(NewMousePosh - SavedMouseH, NewMousePosv - SavedMouseV);
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
		MyMousePositionSet(CurMouseH, CurMouseV);
	}

	if (CurTrueMouseButton != NewMouseButton) {
		CurTrueMouseButton = NewMouseButton;
		MyMouseButtonSet(CurTrueMouseButton && ShouldHaveCursorHidden);
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

LOCALFUNC blnr KC2MKCInit(void)
{
	/*
		May be used to initialize a mapping between
		native keycode and Macintosh keycodes.
	*/

	return trueblnr;
}

LOCALPROC InitKeyCodes(void)
{
	/*
		IMPLEMENT ME
		Determine the state of the real capslock key.
	*/
		blnr Current_Capslock_State = falseblnr;


	theKeys[0] = 0;
	theKeys[1] = 0;
	theKeys[2] = 0;
	theKeys[3] = 0;

	Keyboard_UpdateKeyMap2(MKC_CapsLock, Current_Capslock_State);
}

#if EnableFullScreen
LOCALVAR blnr KeyboardIsGrabbed = falseblnr;
#endif

#if EnableFullScreen
LOCALPROC MyGrabKeyboard(void)
{
	if (! KeyboardIsGrabbed) {
		/*
			IMPLEMENT ME sometime
			If possible and useful, try to prevent the host operating
			system from intercepting any keys.
		*/
		KeyboardIsGrabbed = trueblnr;
	}
}
#endif

#if EnableFullScreen
LOCALPROC MyUnGrabKeyboard(void)
{
	if (KeyboardIsGrabbed) {
		/*
			IMPLEMENT ME sometime
			undo any effects of MyGrabKeyboard
		*/
		KeyboardIsGrabbed = falseblnr;
	}
}
#endif

LOCALVAR blnr NoKeyRepeat = falseblnr;

LOCALPROC DisableKeyRepeat(void)
{
	if (! NoKeyRepeat) {
		NoKeyRepeat = trueblnr;

		/*
			IMPLEMENT ME sometime
			If possible and useful, disable keyboard autorepeat.
		*/
	}
}

LOCALPROC RestoreKeyRepeat(void)
{
	if (NoKeyRepeat) {
		NoKeyRepeat = falseblnr;

		/*
			IMPLEMENT ME sometime
			Undo any effects of DisableKeyRepeat.
		*/
	}
}


/*--- time, date, location ---*/

#if 0
#include "DATE2SEC.h"

/*
	has the utility Date2MacSeconds, which converts
	julian date and time to number
	of seconds since midnight January 1, 1904.
*/
#endif


LOCALVAR ui5b TrueEmulatedTime = 0;
	/*
		The amount of time the program has
		been running, measured in Macintosh
		"ticks". There are 60.14742 ticks per
		second.

		(time when the emulation is
		stopped for more than a few ticks
		should not be counted.)
	*/
LOCALVAR ui5b CurEmulatedTime = 0;
	/*
		The number of ticks that have been
		emulated so far.

		That is, the number of times
		"DoEmulateOneTick" has been called,
		with the exception that CurEmulatedTime
		can be adjusted if the emulation
		needs to sped up or slowed down,
		such as if sound output is lagging
		or gaining.
	*/


#if Use_gettimeofday
#define UsecPerSec 1000000
#define MyInvTimeStep 16626 /* UsecPerSec / 60.14742 */
#endif

#if Use_gettimeofday
LOCALVAR ui5b LastTimeSec;
LOCALVAR ui5b LastTimeUsec;
#endif

#if Use_gettimeofday
LOCALPROC GetCurrentTicks(void)
{
	struct timeval t;

	gettimeofday(&t, NULL);

	LastTimeSec = (ui5b)t.tv_sec;
	LastTimeUsec = (ui5b)t.tv_usec;
}
#endif

#if Use_gettimeofday
LOCALVAR ui5b NextTimeSec;
LOCALVAR ui5b NextTimeUsec;
	/*
		This is the time in seconds and microseconds
		when the current tick will be over.
	*/
#endif

#if Use_gettimeofday
LOCALPROC IncrNextTime(void)
{
	/* increment NextTime by one tick */

	NextTimeUsec += MyInvTimeStep;
	if (NextTimeUsec >= UsecPerSec) {
		NextTimeUsec -= UsecPerSec;
		NextTimeSec += 1;
	}
}
#endif

#if Use_gettimeofday
LOCALPROC InitNextTime(void)
{
	NextTimeSec = LastTimeSec;
	NextTimeUsec = LastTimeUsec;
	IncrNextTime();
}
#endif

LOCALPROC StartUpTimeAdjust(void)
{
#if Use_gettimeofday
	GetCurrentTicks();
	InitNextTime();
#else
	/*
		IMPLEMENT ME
		prepare to call UpdateTrueEmulatedTime.

		will be called again when haven't been
		regularly calling UpdateTrueEmulatedTime,
		(such as the emulation has been stopped).
	*/
#endif
}

#if Use_gettimeofday
LOCALFUNC si5b GetTimeDiff(void)
{
	return ((si5b)(LastTimeSec - NextTimeSec)) * UsecPerSec
		+ ((si5b)(LastTimeUsec - NextTimeUsec));
}
#endif

LOCALPROC UpdateTrueEmulatedTime(void)
{
#if Use_gettimeofday
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
				TimeDiff -= UsecPerSec;
			} while (TimeDiff >= 0);
		}
	} else if (TimeDiff < - 2 * MyInvTimeStep) {
		/* clock goofed if ever get here, reset */
		InitNextTime();
	}
#else
	/*
		IMPLEMENT ME
		Update TrueEmulatedTime. probably
		need to convert between how the
		host operating system measures
		time and Macintosh ticks.
	*/
#endif
}

LOCALFUNC blnr CheckDateTime(void)
{
	/*
		IMPLEMENT ME
		Update CurMacDateInSeconds, the number
		of seconds since midnight January 1, 1904.

		return true if CurMacDateInSeconds is
		different than it was on the last
		call to CheckDateTime.
	*/
	return falseblnr;
}

/*--- sound ---*/

#if MySoundEnabled

LOCALPROC MySound_Start(void)
{
	/*
		IMPLEMENT ME sometime
		start sound output
		(if fail, then should have no effect)
	*/
}

LOCALPROC MySound_Stop(void)
{
	/*
		IMPLEMENT ME sometime
		stop sound output (if sound output wasn't started, do nothing)
	*/
}

LOCALFUNC blnr MySound_Init(void)
{
	/*
		IMPLEMENT ME sometime
		Any initialization for sound output that
		only need to be done once when the program
		starts.
	*/
	return trueblnr;
}

LOCALPROC MySound_UnInit(void)
{
	/*
		IMPLEMENT ME sometime
		any clean up needed before program quits.
	*/
}

GLOBALFUNC ui3p GetCurSoundOutBuff(void)
{
	/*
		IMPLEMENT ME sometime
		Get next buffer of size SOUND_LEN
		bytes for platform independent code
		to write sound to.
		return nullpr if fail, such as if
		sound output is not started.

		The buffer returned by the previous call
		to GetCurSoundOutBuff should be
		played, at
		22254.54545 = 7833600 * 2 / 704
		samples per second.
	*/
	return nullpr;
}

LOCALPROC MySound_SecondNotify(void)
{
	/*
		called once a second.
		can be used to check if sound output it
		lagging or gaining, and if so
		adjust CurEmulatedTime forward or
		backward by one tick.
	*/
}

#endif

/*--- basic dialogs ---*/

LOCALPROC CheckSavedMacMsg(void)
{
	/*
		This is currently only used in the
		rare case where there is a message
		still pending as the program quits.
	*/
	if (nullpr != SavedBriefMsg) {
		char briefMsg0[ClStrMaxLength + 1];
		char longMsg0[ClStrMaxLength + 1];

		NativeStrFromCStr(briefMsg0, SavedBriefMsg);
		NativeStrFromCStr(longMsg0, SavedLongMsg);

#if UseStdCLibs
		fprintf(stderr, briefMsg0);
		fprintf(stderr, "\n");
		fprintf(stderr, longMsg0);
		fprintf(stderr, "\n");
#else
		/* IMPLEMENT ME sometime */
#endif

		SavedBriefMsg = nullpr;
	}
}

/*--- clipboard ---*/

/*
	Can define IncludeHostTextClipExchange as 0
	to start with. And can leave it as 0 if the
	host platform has no concept of clipboard.
*/

#if IncludeHostTextClipExchange
GLOBALFUNC tMacErr HTCEexport(tPbuf i)
{
	/*
		IMPLEMENT ME sometime
		PBuf i is an array of macintosh
		style characters. (using the
		MacRoman character set.)

		Should export this Buffer to the
		native clipboard, performing character
		set translation, and eof character translation
		as needed.

		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/

	PbufDispose(i);

	return mnvm_miscErr;
}
#endif

#if IncludeHostTextClipExchange
GLOBALFUNC tMacErr HTCEimport(tPbuf *r)
{
	/*
		IMPLEMENT ME sometime
		Import the native clipboard as text,
		and convert it to Macintosh format,
		in a Pbuf.

		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/

	return mnvm_miscErr;
}
#endif

/*--- event handling for main window ---*/

LOCALPROC HandleTheEvent(
		/*
			some kind of event from the host operating system.
		*/
		void
	)
{
	/* IMPLEMENT ME */

	/*
		On Redraw requested:
			determine the smallest rectangle that
			bounds the entire request, translate it
			to the coordinates of the emulated screen,
			then call HaveChangedScreenBuff.
	*/

	/*
		On quit requested:
			(such as click on close box.)

			RequestMacOff = trueblnr;
	*/

	/*
		On Key pressed:
			translate to a Macintosh keycode, then call

			Keyboard_UpdateKeyMap2(the_mac_keycode, trueblnr);
	*/

	/*
		On our window activated:

			gTrueBackgroundFlag = falseblnr;
	*/

	/*
		On our window deactivated:

			gTrueBackgroundFlag = trueblnr;
	*/

	/*
		On Key released:
			translate to a Macintosh keycode, then call

			Keyboard_UpdateKeyMap2(the_mac_keycode, falseblnr);
	*/

	/*
		On Drag and drop:
			On drop a disk image file, mount the disk image
			(open the file and call Sony_Insert0),
			and then preferably should activate our window
			(if our window isn't active, then usually the
			emulation isn't running, and nothing will happen).
	*/
}

/*--- main window creation and disposal ---*/

#if UseStdCLibs
LOCALVAR int my_argc;
LOCALVAR char **my_argv;
#endif

LOCALFUNC blnr Screen_Init(void)
{
#if UseStdCLibs
	screencomparebuff = (char *)calloc(1, vMacScreenNumBytes);
	if (! screencomparebuff) {
		fprintf(stderr, "calloc failed.\n");
		return falseblnr;
	}
	SetLongs((ui5b *)screencomparebuff, vMacScreenNumBytes / 4);

#if UseControlKeys
	CntrlDisplayBuff = (char *)malloc(vMacScreenNumBytes);
	if (! CntrlDisplayBuff) {
#if UseStdCLibs
		fprintf(stderr, "malloc failed.\n");
#endif
		return falseblnr;
	}
#endif
#else
	/* IMPLEMENT ME */
#endif

	DisableKeyRepeat();

	return trueblnr;
}

LOCALPROC DisposeMainWindow(void)
{
	/*
		IMPLEMENT ME
		Dispose of anything set up by ReCreateMainWindow.
	*/
}

LOCALFUNC blnr ReCreateMainWindow(void)
{
	/*
		IMPLEMENT ME
		Set up somewhere for us to draw the emulated screen and
		receive mouse input. i.e. a window, usually.

		Should look at the current value of WantMagnify and
		WantFullScreen, when get around to implementing them.
	*/

	/*
		May be called again, without DisposeMainWindow
		being called first. (Usually with different
		values of WantMagnify and WantFullScreen than
		on the previous call.)

		If there is existing window, and fail to create
		the new one, then existing window must be left alone,
		in valid state. (and return falseblnr. otherwise,
		if succeed, return trueblnr)

		i.e. can allocate the new one before disposing
		of the old one.
	*/

	/*
		if possible, the title of the window should
		be the name of the application file,
		otherwise can use kStrAppName.

		The window should not be resizeable.
	*/


	/*
		update UseMagnify and UseFullScreen, but
		only do this if succeed (and will return trueblnr)
	*/
#if EnableMagnify
	UseMagnify = WantMagnify;
#endif
#if EnableFullScreen
	UseFullScreen = WantFullScreen;
#endif

	/*
		reset the saved keyboard state if pending key events to the old window
		can be lost. Don't do this if key events can't be lost, such as if key
		events are per application, not per window.
	*/
		KeyBoardAttachedToEm = falseblnr;
		Keyboard_UpdateControlKey(falseblnr);

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
		(void) ReCreateMainWindow();
	}
}

#if EnableFullScreen
LOCALPROC ToggleWantFullScreen(void)
{
	WantFullScreen = ! WantFullScreen;

}
#endif

/*--- SavedTasks ---*/

LOCALPROC LeaveBackground(void)
{
	DisableKeyRepeat();
}

LOCALPROC EnterBackground(void)
{
	RestoreKeyRepeat();
	KeyBoardAttachedToEm = falseblnr;
	Keyboard_UpdateControlKey(falseblnr);

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
	if (MyEvtQNeedRecover) {
		MyEvtQNeedRecover = falseblnr;

		/* attempt cleanup, MyEvtQNeedRecover may get set again */
		MyEvtQTryRecoverFromFull();
	}

#if EnableMouseMotion
	if (HaveMouseMotion) {
		MyMouseConstrain();
	}
#endif

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
			IMPLEMENT ME sometime
			either leave Full Screen mode
			(call ToggleWantFullScreen), or
			get us activated.
		*/
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
#if UseStdCLibs
			ui3p NewDiskNameDat;
			if (MacRomanTextToNativePtr(vSonyNewDiskName, trueblnr,
				&NewDiskNameDat))
			{
				MakeNewDisk(vSonyNewDiskSize, (char *)NewDiskNameDat);
				free(NewDiskNameDat);
			}
#endif
			PbufDispose(vSonyNewDiskName);
			vSonyNewDiskName = NotAPbuf;
		} else
#endif
		{
#if UseStdCLibs
			MakeNewDiskAtDefault(vSonyNewDiskSize);
#endif
		}
		vSonyNewDiskWanted = falseblnr;
			/* must be done after may have gotten disk */
	}
#endif

	if ((nullpr != SavedBriefMsg) & ! MacMsgDisplayed) {
		MacMsgDisplayOn();
	}

	if (KeyBoardAttachedToEm != (! (MacMsgDisplayed || LastControlKey
		|| gTrueBackgroundFlag)))
	{
		KeyBoardAttachedToEm = ! KeyBoardAttachedToEm;
		if (KeyBoardAttachedToEm) {
			InitKeyCodes();
		}
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
			MyHideCursor();
		} else {
			MyShowCursor();
		}
	}

	if (ControlKeyPressed != Keyboard_TestKeyMap(MKC_Control)) {
		Keyboard_UpdateKeyMap(MKC_Control, ControlKeyPressed);
	}
}

/*--- command line parsing ---*/

LOCALFUNC blnr ScanCommandLine(void)
{
#if UseStdCLibs
	int i;

	for (i = 1; i < my_argc; i++) {
		if (my_argv[i][0] == '-') {
			if ((strcmp(my_argv[i], "--rom") == 0) ||
				(strcmp(my_argv[i], "-r") == 0))
			{
				i++;
				if (i < my_argc) {
					rom_path = my_argv[i];
				}
			} else
			if (strcmp(my_argv[i], "-l") == 0) {
				SpeedValue = 0;
			} else
			{
				MacMsg(kStrBadArgTitle, kStrBadArgMessage, falseblnr);
			}
		} else {
			(void) Sony_Insert1(my_argv[i], falseblnr);
		}
	}
#else
	/*
		IMPLEMENT ME sometime
		parse command line parameters, if the platform
		has any such concept.
	*/
#endif
	return trueblnr;
}

/*--- main program flow ---*/

LOCALVAR ui5b OnTrueTime = 0;
	/*
		The time slice we are currently dealing
		with, in the same units as TrueEmulatedTime.
	*/

GLOBALFUNC blnr ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	return TrueEmulatedTime == OnTrueTime;
}

LOCALPROC RunEmulatedTicksToTrueTime(void)
{
	/*
		The general idea is to call DoEmulateOneTick
		once per tick.

		But if emulation is lagging, we'll try to
		catch up by calling DoEmulateOneTick multiple
		times, unless we're too far behind, in
		which case we forget it.

		If emulating one tick takes longer than
		a tick we don't want to sit here
		forever. So the maximum number of calls
		to DoEmulateOneTick is determined at
		the beginning, rather than just
		calling DoEmulateOneTick until
		CurEmulatedTime >= TrueEmulatedTime.
	*/

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
	/*
		Wait until the end of the current
		tick, then emulate the next tick.
	*/

	while (ExtraTimeNotOver()) {
		/*
			IMPLEMENT ME sometime
			If possible, try to sleep
			until the current tick
			is over, that is, give
			other processes in the host
			operating system time to execute.
		*/
	}

	OnTrueTime = TrueEmulatedTime;
	RunEmulatedTicksToTrueTime();
}

LOCALPROC WaitForTheNextEvent(void)
{
	/*
		Wait for the next event
		from the operating system, we have nothing better
		to do. Call HandleTheEvent and return (only
		wait for one event).
	*/
	HandleTheEvent();
}

LOCALPROC CheckForSystemEvents(void)
{
	/*
		Handle any events that are waiting for us with
		HandleTheEvent.
		Return immediately when no more events
		are waiting, don't wait for more.
	*/
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
				/*
					DoEmulateExtraTime is used for
					anything over emulation speed
					of 1x. It periodically calls
					ExtraTimeNotOver and stops
					when this returns false (or it
					is finished with emulating the
					extra time).
				*/
			RunOnEndOfSixtieth();
		}
	}
}

LOCALPROC ZapOSGLUVars(void)
{
	/*
		Set initial values of variables for
		platform dependent code, where not
		done using c initializers. (such
		as for arrays.)
	*/

	InitDrives();
}

LOCALFUNC blnr InitOSGLU(void)
{
	/*
		run all the initializations
		needed for the program.
	*/

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
	/*
		Do all clean ups needed
		before the program quits.
	*/

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
#if IncludePbufs
	UnInitPbufs();
#endif
	UnInitDrives();

	ForceShowCursor();

	DisposeMainWindow();

	DeallocateMacRAM();
	DeallocateMacROM();
}

int main(int argc, char **argv)
{
#if UseStdCLibs
	my_argc = argc;
	my_argv = argv;
#endif

	ZapOSGLUVars();
	if (InitOSGLU()) {
		MainEventLoop();
	}
	UnInitOSGLU();

	return 0;
}
