/*
	MYOSGLUE.c

	Copyright (C) 2002 Paul Pratt

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
	MY Operating System GLUE
*/

#include "SYSDEPNS.h"
#include "VERSINFO.h"

#include "MYOSGLUE.h"

IMPORTPROC ProgramMain(void);


GLOBALVAR char *screencomparebuff;

GLOBALVAR UWORD *RAM = nullpr;

GLOBALVAR ULONG kRAM_Size = 0;

GLOBALVAR UWORD *ROM;

GLOBALVAR UWORD CurMouseV = 0;
GLOBALVAR UWORD CurMouseH = 0;
GLOBALVAR UBYTE CurMouseButton = falseblnr;

GLOBALVAR ULONG theKeys[4];

GLOBALVAR blnr RequestMacOff = falseblnr;
GLOBALVAR blnr RequestMacInterrupt = falseblnr;
GLOBALVAR blnr RequestMacReset = falseblnr;

GLOBALVAR ULONG MountPending = 0;

#define kStrTooManyImagesTitle "Too many Disk Images."
#define kStrTooManyImagesMessage "Mini vMac can not mount more than three Disk Images. Try ejecting one."

#define kStrImageInUseTitle "Disk Image in use."
#define kStrImageInUseMessage "The Disk Image can not be mounted because it is already in use by another application."

#ifndef MacTarget
#define MacTarget 0
#endif
#ifndef WinTarget
#define WinTarget 0
#endif
#ifndef XWnTarget
#define XWnTarget 0
#endif

#ifndef HaveOSTarget

#if MacTarget || WinTarget || XWnTarget
#error "HaveOSTarget undefined"
#endif

#include "PROSGLUE.h"

#else

#if MacTarget /* This entire file is for macintosh only */
#include "MCOSGLUE.h"
#endif /* MacTarget */

#if WinTarget /* This entire file is for Windows only */
#include "WNOSGLUE.h"
#endif /* WinTarget */

#if XWnTarget /* This entire file is for X only */
#include "XWOSGLUE.h"
#endif /* XWnTarget */

#endif /* HaveOSTarget defined */
