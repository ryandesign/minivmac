/*
	setup.h
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
	information for the scripts in the "build" folder
*/

#define kStrAppName "Mini vMac"
#define kStrAppAbbrev "minivmac" /* [a-z0-9_]{1,8} */
#define kMajorVersion "2"
#define kMinorVersion "7"
#define kMinorSubVersion "0"
#define kStrCopyrightYear "2005"
#define kMaintainerName "Paul C. Pratt"
#define kStrHomePage "http://minivmac.sourceforge.net/"
#define kMacCreatorSig "MnvM"
#define kBundleIdentifier "com.gryphel.minivmac"
#define kShortDescription "miniature Macintosh emulator"

/* --- list of source files --- */

static void DoAllExtraHeaders(tDoOneExtraHeader p)
{
	p("SYSDEPNS.h");
	p("DATE2SEC.h");
	p("ENDIANAC.h");
}

static void DoMYOSGLUEdepends(tDoOneDepends p)
{
	p("PROSGLUE.h");
	p("MCOSGLUE.h");
	p("WNOSGLUE.h");
	p("XWOSGLUE.h");
}

#ifndef Use68020
#define Use68020 0
#endif

static void DoAllSrcFiles(tDoOneCFile p)
{
	p("MYOSGLUE", falseblnr, DoMYOSGLUEdepends);
	p("GLOBGLUE", falseblnr, nullpr);
	p("ADDRSPAC", falseblnr, nullpr);
	p("PROGMAIN", falseblnr, nullpr);
	p("CONTROLM", falseblnr, nullpr);
#if Use68020
	p("MINEM68K", falseblnr, nullpr);
#else
	p("MINEM68K", trueblnr, nullpr);
#endif
	p("VIAEMDEV", falseblnr, nullpr);
	p("IWMEMDEV", falseblnr, nullpr);
	p("SCCEMDEV", falseblnr, nullpr);
	p("RTCEMDEV", falseblnr, nullpr);
	p("ROMEMDEV", falseblnr, nullpr);
	p("SCSIEMDV", falseblnr, nullpr);
	p("SONYEMDV", falseblnr, nullpr);
	p("SNDEMDEV", falseblnr, nullpr);
	p("SCRNEMDV", falseblnr, nullpr);
	p("KBRDEMDV", falseblnr, nullpr);
	p("MOUSEMDV", falseblnr, nullpr);
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
#ifdef WantInitSpeedLimit
#if WantInitSpeedLimit
	WriteDestFileLn("#define WantInitSpeedLimit 1");
#endif
#endif

#ifdef WantInitFullScreen
#if WantInitFullScreen
	WriteDestFileLn("#define WantInitFullScreen 1");
#endif
#endif

#ifdef WantInitMagnify
#if WantInitMagnify
	WriteDestFileLn("#define WantInitMagnify 1");
#endif
#endif

#if Use68020
	WriteDestFileLn("#define Use68020 1");
#endif

#ifdef NumDrives
#ifdef (NumDrives > 0) && (NumDrives < 32)
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define NumDrives ");
	WriteUnsignedToOutput(NumDrives);
	WriteEndDestFileLn();
#endif
#endif

#ifdef DetailedAbormalReport
#if DetailedAbormalReport
	WriteDestFileLn("#define DetailedAbormalReport 1");
#endif
#endif

#if UseCPUFam68K
		WriteDestFileLn("#define CurEmu kEmuPlus2M");
#endif
}
