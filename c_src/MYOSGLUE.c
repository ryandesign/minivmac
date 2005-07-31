/*
	MYOSGLUE.c

	Copyright (C) 2005 Paul C. Pratt

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

IMPORTPROC DoEmulateOneTick(void);
IMPORTPROC DoEmulateExtraTime(void);
IMPORTFUNC blnr InitEmulation(void);

GLOBALVAR char *screencomparebuff = nullpr;
GLOBALVAR char *CntrlDisplayBuff = nullpr;

GLOBALVAR ui4b *RAM = nullpr;

GLOBALVAR ui4b *ROM = nullpr;

GLOBALVAR ui4b CurMouseV = 0;
GLOBALVAR ui4b CurMouseH = 0;
GLOBALVAR ui3b CurMouseButton = falseblnr;

GLOBALVAR ui5b theKeys[4];

#ifndef WantInitSpeedLimit
#define WantInitSpeedLimit 1
#endif

GLOBALVAR blnr SpeedLimit = (WantInitSpeedLimit != 0);

GLOBALVAR blnr SpeedStopped = falseblnr;

GLOBALVAR ui3b SpeedValue = 3;

GLOBALVAR blnr RunInBackground = falseblnr;

GLOBALVAR si3b TimeAdjust = 0;

#if UseControlKeys
GLOBALVAR blnr ControlKeyPressed = falseblnr;
#endif

#ifndef WantInitFullScreen
#define WantInitFullScreen 0
#endif

#if EnableFullScreen
GLOBALVAR blnr WantFullScreen = (WantInitFullScreen != 0);
#endif

#ifndef WantInitMagnify
#define WantInitMagnify 0
#endif

#if EnableMagnify
GLOBALVAR blnr WantMagnify = (WantInitMagnify != 0);
#endif

#if EnableMouseMotion
GLOBALVAR blnr HaveMouseMotion = falseblnr;
GLOBALVAR ui4b MouseMotionV = 0;
GLOBALVAR ui4b MouseMotionH = 0;
#endif

#if MySoundEnabled
#ifndef MySoundFullScreenOnly
#define MySoundFullScreenOnly 0
#endif
#endif

GLOBALVAR blnr RequestMacOff = falseblnr;

GLOBALVAR blnr ForceMacOff = falseblnr;

GLOBALVAR blnr RequestInsertDisk = falseblnr;

GLOBALVAR ui5b vSonyWritableMask = 0;
GLOBALVAR ui5b vSonyInsertedMask = 0;
GLOBALVAR ui5b vSonyMountedMask = 0;

GLOBALVAR ui5b CurMacDateInSeconds = 0;
GLOBALVAR ui5b CurMacLatitude = 0;
GLOBALVAR ui5b CurMacLongitude = 0;
GLOBALVAR ui5b CurMacDelta = 0;

LOCALFUNC blnr FirstFreeDisk(ui4b *Drive_No)
{
	si4b i;

	for (i = 0; i < NumDrives; ++i) {
		if ((vSonyInsertedMask & ((ui5b)1 << i)) == 0) {
			*Drive_No = i;
			return trueblnr;
		}
	}
	return falseblnr;
}

GLOBALFUNC blnr AnyDiskInserted(void)
{
	si4b i;

	for (i = 0; i < NumDrives; ++i) {
		if ((vSonyInsertedMask & ((ui5b)1 << i)) != 0) {
			return trueblnr;
		}
	}
	return falseblnr;
}

#define kStrTooManyImagesTitle "Too many Disk Images."
#define kStrTooManyImagesMessage "Mini vMac can not mount that many Disk Images. Try ejecting one."

#define kStrImageInUseTitle "Disk Image in use."
#define kStrImageInUseMessage "The Disk Image can not be mounted because it is already in use by another application."

#ifndef EnableDragDrop
#define EnableDragDrop 1
#endif

#ifndef RomFileName
#if CurEmu <= kEmu512K
#define RomFileName "Mac128K.ROM"
#elif CurEmu <= kEmuPlus
#define RomFileName "vMac.ROM"
#elif CurEmu <= kEmuSE
#define RomFileName "MacSE.ROM"
#elif CurEmu <= kEmuClassic
#define RomFileName "Classic.ROM"
#else
#error "RomFileName not defined"
#endif
#endif

#ifndef MacTarget
#define MacTarget 0
#endif
#ifndef WinTarget
#define WinTarget 0
#endif
#ifndef XWnTarget
#define XWnTarget 0
#endif

#if MacTarget /* This entire file is for Macintosh only */
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
