/*
	SPBASDEF.i
	Copyright (C) 2007 Paul C. Pratt

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
	program SPecific BASic DEFinitions
*/


#define kStrAppName "Mini vMac"
#define kStrAppAbbrev "minivmac" /* [a-z0-9_]{1,8} */
#define kMajorVersion "3"
#define kMinorVersion "1"
#define kMinorSubVersion "0"
#define kStrCopyrightYear "2007"
#define kMacCreatorSig "MnvM"
#define kBundleIdentifier "com.gryphel.minivmac"
#define kShortDescription "miniature Macintosh emulator"

#if 0 /* (gbo_lang == gbk_lang_fre) */
#define kStrHomePage "http://pages.videotron.com/plemieux/minivmac/"
#else
#define kStrHomePage "http://minivmac.sourceforge.net/"
#endif

#define Have_SPBLDOPT 1
#define Have_SPCNFGGL 1
#define Have_SPCNFGAP 1
#define Have_SPOTHRCF 1

#define UseOpenGLinOSX 1
