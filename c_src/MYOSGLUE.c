/*
	MYOSGLUE.c

	Copyright (C) 2002 Paul Pratt

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
	MY Operating System GLUE
*/

#include "CNFGRAPI.h"
#include "SYSDEPNS.h"

#include "MYOSGLUE.h"

IMPORTPROC ProgramMain(void);


GLOBALVAR char *screencomparebuff;

GLOBALVAR ui4b *RAM = nullpr;

GLOBALVAR ui5b kRAM_Size = 0;

GLOBALVAR ui4b *ROM;

GLOBALVAR ui4b CurMouseV = 0;
GLOBALVAR ui4b CurMouseH = 0;
GLOBALVAR ui3b CurMouseButton = falseblnr;

GLOBALVAR ui5b theKeys[4];

GLOBALVAR blnr RequestMacOff = falseblnr;
GLOBALVAR blnr RequestMacInterrupt = falseblnr;
GLOBALVAR blnr RequestMacReset = falseblnr;

GLOBALVAR ui5b MountPending = 0;

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

#if MacTarget /* This entire file is for macintosh only */
#include "MCOSGLUE.h"
#define Have_OSGLUEcore 1
#endif /* MacTarget */

#if WinTarget /* This entire file is for Windows only */
#include "WNOSGLUE.h"
#define Have_OSGLUEcore 1
#endif /* WinTarget */

#if XWnTarget /* This entire file is for X only */
#include "XWOSGLUE.h"
#define Have_OSGLUEcore 1
#endif /* XWnTarget */

#ifndef Have_OSGLUEcore
#include "PROSGLUE.h"
#endif
