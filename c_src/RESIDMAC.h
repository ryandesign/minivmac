/*
	RESIDMAC.h
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
	RESource IDs for MACintosh

	Used by both c compiler (for OSGLUMAC.c) and
	by resource compiler (for main.r).
*/

#ifdef RESIDMAC_H
#error "header already included"
#else
#define RESIDMAC_H
#endif

/* Menu Constants */

#define kMyMenuBar   128

#define kAppleMenu   128
#define kFileMenu    129
#define kSpecialMenu 130

/* Apple */

#define kAppleAboutItem 1

/* File */

#define kFileOpenDiskImage 1
/* -- seperator 2  */
#define kFileQuitItem 3

/* Special */

#define kSpecialLimitSpeedItem 1
/* -- seperator 2  */
#define kSpecialResetItem 3
#define kSpecialInterruptItem 4

/* Alerts */

#define kMyAboutAlert 128
#define kMyStandardAlert 131
#define kMyOkCancelAlert 132
