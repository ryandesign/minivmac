/*
	setup.h
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
	information for the scripts in the "build" folder
*/

#define kStrAppName "Mini vMac"
#define kStrAppAbbrev "minivmac" /* [a-z0-9_]{1,8} */
#define kMajorVersion "3"
#define kMinorVersion "0"
#define kMinorSubVersion "0"
#define kStrCopyrightYear "2006"
#define kMacCreatorSig "MnvM"
#define kBundleIdentifier "com.gryphel.minivmac"
#define kShortDescription "miniature Macintosh emulator"

#ifndef kStrHomePage
#if (gbo_lang == gbk_lang_fre)
#define kStrHomePage "http://pages.videotron.com/plemieux/minivmac/"
#else
#define kStrHomePage "http://minivmac.sourceforge.net/"
#endif
#endif

/* --- list of source files --- */

static void DoAllExtraHeaders(tDoOneExtraHeader p)
{
	p(kDepDirCSrc, "SYSDEPNS.h");
	p(kDepDirCSrc, "DATE2SEC.h");
	p(kDepDirCSrc, "ENDIANAC.h");
	p(kDepDirPlat, "PLATGLUE.h");
	p(kDepDirLang, "STRCONST.h");
	p(kDepDirCSrc, "CONTROLM.h");
}

static void DoMYOSGLUEdepends(tDoOneDepends p)
{
	p(kDepDirPlat, "PLATGLUE.h");
	p(kDepDirLang, "STRCONST.h");
	p(kDepDirCSrc, "CONTROLM.h");
}

#ifndef Use68020
#define Use68020 0
#endif

static void DoAllSrcFiles(tDoOneCFile p)
{
	p("MYOSGLUE", kCSrcFlgmInclPlat | kCSrcFlgmInclLang,
		DoMYOSGLUEdepends);
	p("GLOBGLUE", kCSrcFlgmNone, nullpr);
	p("ADDRSPAC", kCSrcFlgmNone, nullpr);
	p("PROGMAIN", kCSrcFlgmNone, nullpr);
#if Use68020
	p("MINEM68K", kCSrcFlgmNone, nullpr);
#else
	p("MINEM68K", kCSrcFlgmAsmAvail, nullpr);
#endif
	p("VIAEMDEV", kCSrcFlgmNone, nullpr);
	p("IWMEMDEV", kCSrcFlgmNone, nullpr);
	p("SCCEMDEV", kCSrcFlgmNone, nullpr);
	p("RTCEMDEV", kCSrcFlgmNone, nullpr);
	p("ROMEMDEV", kCSrcFlgmNone, nullpr);
	p("SCSIEMDV", kCSrcFlgmNone, nullpr);
	p("SONYEMDV", kCSrcFlgmNone, nullpr);
	p("SNDEMDEV", kCSrcFlgmNone, nullpr);
	p("SCRNEMDV", kCSrcFlgmNone, nullpr);
	p("KBRDEMDV", kCSrcFlgmNone, nullpr);
	p("MOUSEMDV", kCSrcFlgmNone, nullpr);
}

/* --- list of document types --- */

static void WriteRomExtensions(tWriteOneExtension p)
{
	p("rom");
}

static void WriteDskExtensions(tWriteOneExtension p)
{
	p("dsk");
}

static void DoAllDocTypes(tWriteOneDocType p)
{
	p("Rom", "ROM!", "Rom image", WriteRomExtensions);
	p("Dsk", "MvIm", "Disk image", WriteDskExtensions);
}

static void WriteAppSpecificCNFGGLOBoptions(void)
{
#if Use68020
	WriteDestFileLn("#define Use68020 1");
#endif
}
