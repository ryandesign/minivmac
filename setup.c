/*
	setup.c
	Copyright (C) 2004 Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#include <stdio.h>
#include <string.h>

/*
	This program is used to help build the following program:
*/

#define kStrAppName "Mini vMac"
#define kStrAppAbbrev "minivmac" /* [a-z0-9_]{1,8} */
#define kMajorVersion "2"
#define kMinorVersion "5"
#define kMinorSubVersion "0"
#define kStrCopyrightYear "2004"
#define kMaintainerName "Paul C. Pratt"
#define kStrHomePage "http://minivmac.sourceforge.net"
#define kMacCreatorSig "MnvM"
#define kBundleIdentifier "com.gryphel.minivmac"
#define kShortDescription "miniature Macintosh emulator"

/* --- list of source files --- */

#ifndef UsePragmaSegment
#define UsePragmaSegment 0
#endif

#if UsePragmaSegment
#pragma segment Seg1
#endif

typedef void (*tDoOneExtraHeader)(char *s);

static void DoAllExtraHeaders(tDoOneExtraHeader p)
{
	p("SYSDEPNS.h");
	p("DATE2SEC.h");
	p("POSTKEYS.h");
	p("ENDIANAC.h");
}

typedef void (*tDoOneDepends)(char *s);

static void DoMYOSGLUEdepends(tDoOneDepends p)
{
	p("PROSGLUE.h");
	p("MCOSGLUE.h");
	p("WNOSGLUE.h");
	p("XWOSGLUE.h");
}

#define blnr int
#define trueblnr 1
#define falseblnr 0

#define nullpr 0

typedef void (*tDoDependsForC)(tDoOneDepends p);

typedef void (*tDoOneCFile)(char *s, blnr AsmAvail, tDoDependsForC depends);

static void DoAllSrcFiles(tDoOneCFile p)
{
	p("MYOSGLUE", falseblnr, DoMYOSGLUEdepends);
	p("MINEM68K", trueblnr, nullpr);
	p("GLOBGLUE", falseblnr, nullpr);
	p("ADDRSPAC", falseblnr, nullpr);
	p("PROGMAIN", falseblnr, nullpr);
	p("CONTROLM", falseblnr, nullpr);
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

typedef void (*tWriteOneExtension)(char *s);

typedef void (*tWriteExtensionList)(tWriteOneExtension p);

static void WriteRomExtensions(tWriteOneExtension p)
{
	p("rom");
}

static void WriteDskExtensions(tWriteOneExtension p)
{
	p("dsk");
}

typedef void (*tWriteOneDocType)(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList);

static void DoAllDocTypes(tWriteOneDocType p)
{
	p("Rom", "ROM!", "Rom image", WriteRomExtensions);
	p("Dsk", "MvIm", "Disk image", WriteDskExtensions);
}

/* --- definition of options --- */

enum {
	kScriptMPW, /* Macintosh Programmers Workshop */
	kScriptAppleScript, /* AppleScript */
	kScriptBash, /* Bash */
	kScriptVBScript, /* Visual Basic Script */
	kScriptAuto,
	kNumScripts
};

#ifndef TheScriptLang
#define TheScriptLang kScriptAuto
#endif

static int CurScriptLang = TheScriptLang;

enum {
	kTargMac68K, /* MacOS 68K */
	kTargMac68KFPU, /* MacOS 68K with FPU */
	kTargClassicPPC, /* MacOS OS 9 for PowerPC */
	kTargCarbon, /* MacOS Carbon lib for OS 9 and OS X */
	kTargMacho, /* MacOS X Macho */
	kTargWinx86, /* Windows */
	kTargMacX11, /* X11 for MacOS X */
	kTargLinuxx86, /* X11 for linux on x86 */
	kTargAuto,
	kNumTargets
};

#ifndef TheTarget
#define TheTarget kTargAuto
#endif

static int CurTarget = TheTarget;

enum {
	kDbgLvlQuick, /* Optimizations off, Debugging checks on */
	kDbgLvlTest, /* Optimizations on, Debugging checks on */
	kDbgLvlShip, /* Optimizations on, Debugging checks off */
	kNumDebugLevels
};

#ifndef TheDbgLvl
#define TheDbgLvl kDbgLvlQuick
#endif

static int CurDbgLvl = TheDbgLvl;

enum {
	kIdeMPW3_6_a1, /* Macintosh Programmers Workshop */
	kIdeMW8, /* Metrowerks CodeWarrior */
	kIdeBashGcc, /* Gnu tools */
	kIdeMSVC, /* Microsoft Visual C++ */
	kIdeLccW32, /* lcc-win32 - Jacob Navia */
	kIdeDevC, /* Bloodshed Dev-C++ */
	kIdeAPB, /* Apple Project Builder */
	kIdeRH7, /* Red Hat 7, Gnu tools */
	kNumIdes
};

#ifndef TheIde
#define TheIde kIdeMPW3_6_a1
#endif

static int CurIde = TheIde;

enum {
	kCPUFam68K, /* Motorola 680x0 */
	kCPUFamPPC, /* PowerPC */
	kCPUFamX86, /* Intel 80x86 */
	kNumCPUFamilies
};

static int TheCPUFam;

#ifndef TheUseAbsolute
#define TheUseAbsolute falseblnr
#endif

static int CurUseAbsolute = TheUseAbsolute;

#ifndef ThePrintCFiles
#define ThePrintCFiles falseblnr
#endif

static int CurPrintCFiles = ThePrintCFiles;

#ifndef AllowAsm
#define AllowAsm 1
#endif

static blnr HaveAsm = falseblnr;

/* --- end of definition of options --- */


/* --- names of options --- */

static char * GetScriptLangName(int i)
{
	char *s;

	switch (i) {
		case kScriptMPW:
			s = "MPW";
			break;
		case kScriptAppleScript:
			s = "AppleScript";
			break;
		case kScriptBash:
			s = "Bash";
			break;
		case kScriptVBScript:
			s = "VBScript";
			break;
		case kScriptAuto:
			s = "Auto";
			break;
		default:
			s = "(unknown script language)";
			break;
	}
	return s;
}

static char * GetTargetName(int i)
{
	char *s;

	switch (i) {
		case kTargMac68K:
			s = "m68k";
			break;
		case kTargMac68KFPU:
			s = "mfpu";
			break;
		case kTargClassicPPC:
			s = "mppc";
			break;
		case kTargCarbon:
			s = "carb";
			break;
		case kTargMacho:
			s = "mach";
			break;
		case kTargWinx86:
			s = "wx86";
			break;
		case kTargMacX11:
			s = "mx11";
			break;
		case kTargLinuxx86:
			s = "lx86";
			break;
		case kTargAuto:
			s = "auto";
			break;
		default:
			s = "(unknown Target)";
			break;
	}
	return s;
}

static char * GetDbgLvlName(int i)
{
	char *s;

	switch (i) {
		case kDbgLvlQuick:
			s = "d";
			break;
		case kDbgLvlTest:
			s = "t";
			break;
		case kDbgLvlShip:
			s = "s";
			break;
		default:
			s = "(unknown Debug Level)";
			break;
	}
	return s;
}

static char * GetIdeName(int i)
{
	char *s;

	switch (i) {
		case kIdeMPW3_6_a1:
			s = "mpw";
			break;
		case kIdeMW8:
			s = "mw8";
			break;
		case kIdeBashGcc:
			s = "bgc";
			break;
		case kIdeMSVC:
			s = "msv";
			break;
		case kIdeLccW32:
			s = "lcc";
			break;
		case kIdeDevC:
			s = "dvc";
			break;
		case kIdeAPB:
			s = "apb";
			break;
		case kIdeRH7:
			s = "rh7";
			break;
		default:
			s = "(unknown IDE)";
			break;
	}
	return s;
}

/* --- end of names of options --- */


/* --- output utilities --- */

static void WriteCStrToOutput(char *s)
{
	printf("%s", s);
}

static void WriteUnsignedToOutput(unsigned int v)
{
	printf("%u", v);
}

static void WriteHexWordToOutput(unsigned int v)
{
	printf("%04X", v);
}

static void WriteEolToOutput(void)
{
	printf("\n");
}

static void WriteLnCStrToOutput(char *s)
{
	WriteCStrToOutput(s);
	WriteEolToOutput();
}

/* --- choosing options --- */

typedef char * (* tGetName)(int i);

static blnr FindNamedOption(char *s, int n, tGetName p, int *r)
{
	int i;

	for (i = 0; i < n; ++i) {
		if (strcmp(s, p(i)) == 0) {
			*r = i;
			return trueblnr;
		}
	}
	return falseblnr;
}

static blnr IsVersionQuerry(int argc, char *argv[])
{
	if (1 < argc) {
		if (strcmp(argv[1], "-v") == 0) {
			WriteCStrToOutput(kStrAppAbbrev);
			WriteCStrToOutput("-");
			WriteCStrToOutput(kMajorVersion);
			WriteCStrToOutput(".");
			WriteCStrToOutput(kMinorVersion);
			WriteCStrToOutput(".");
			WriteCStrToOutput(kMinorSubVersion);
			WriteEolToOutput();
			return trueblnr;
		}
	}
	return falseblnr;
}

static blnr ProcessCommandLineArguments(int argc, char *argv[])
{
	char *s;
	int i = 1;

	while (i < argc) {
		s = argv[i++];
		if (strcmp(s, "-e") == 0) {
			if (i < argc) {
				s = argv[i++];
				if (! FindNamedOption(s, kNumIdes, GetIdeName, &CurIde)) {
					fprintf(stderr, "unknown IDE option\n");
					return falseblnr;
				}
			}
		} else
		if (strcmp(s, "-s") == 0) {
			if (i < argc) {
				s = argv[i++];
				if (! FindNamedOption(s, kNumScripts, GetScriptLangName, &CurScriptLang)) {
					fprintf(stderr, "unknown Script Language option\n");
					return falseblnr;
				}
			}
		} else
		if (strcmp(s, "-t") == 0) {
			if (i < argc) {
				s = argv[i++];
				if (! FindNamedOption(s, kNumTargets, GetTargetName, &CurTarget)) {
					fprintf(stderr, "unknown Target option\n");
					return falseblnr;
				}
			}
		} else
		if (strcmp(s, "-d") == 0) {
			if (i < argc) {
				s = argv[i++];
				if (! FindNamedOption(s, kNumDebugLevels, GetDbgLvlName, &CurDbgLvl)) {
					fprintf(stderr, "unknown Debug Level option\n");
					return falseblnr;
				}
			}
		} else
		if (strcmp(s, "-a") == 0) {
			CurUseAbsolute = trueblnr;
		} else
		if (strcmp(s, "-l") == 0) {
			CurPrintCFiles = trueblnr;
		} else
		{
			fprintf(stderr, "unknown switch\n");
			return falseblnr;
		}
	}
	return trueblnr;
}

static void ChooseScriptLang(void)
{
	switch (CurIde) {
		case kIdeMPW3_6_a1:
			CurScriptLang = kScriptMPW;
			break;
		case kIdeMW8:
			CurScriptLang = kScriptAppleScript;
			break;
		case kIdeBashGcc:
		case kIdeAPB:
		case kIdeRH7:
			CurScriptLang = kScriptBash;
			break;
		case kIdeMSVC:
		case kIdeLccW32:
		case kIdeDevC:
			CurScriptLang = kScriptVBScript;
			break;
	}
}

static void ChooseCPUFam(void)
{
	switch (CurTarget) {
		case kTargMac68K:
		case kTargMac68KFPU:
			TheCPUFam = kCPUFam68K;
			break;
		case kTargClassicPPC:
		case kTargCarbon:
		case kTargMacho:
		case kTargMacX11:
			TheCPUFam = kCPUFamPPC;
			break;
		case kTargWinx86:
		case kTargLinuxx86:
			TheCPUFam = kCPUFamX86;
			break;
	}
}

static void AutoChooseSettings(void)
{
	if (CurScriptLang == kScriptAuto) {
		ChooseScriptLang();
	}
	if (CurTarget == kTargAuto) {
		switch (CurIde) {
			case kIdeMPW3_6_a1:
				CurTarget = kTargMac68K;
				break;
			case kIdeMW8:
			case kIdeBashGcc:
			case kIdeAPB:
				CurTarget = kTargMacho;
				break;
			case kIdeMSVC:
			case kIdeLccW32:
			case kIdeDevC:
				CurTarget = kTargWinx86;
				break;
			case kIdeRH7:
				CurTarget = kTargLinuxx86;
				break;
		}
	}

	ChooseCPUFam();

	if (AllowAsm) {
		if (TheCPUFam == kCPUFamPPC) {
			if (! ((CurTarget == kTargMacho) && (CurIde == kIdeMW8))) {
				HaveAsm = trueblnr;
			}
		}
	}
}

/* --- end of choosing options --- */


/* --- code specific to Scripting Language --- */

#if UsePragmaSegment
#pragma segment Seg3
#endif

static void WriteScriptLangExtension(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
			WriteCStrToOutput(".mpw");
			break;
		case kScriptAppleScript:
			WriteCStrToOutput(".scpt");
			break;
		case kScriptBash:
			WriteCStrToOutput(".sh");
			break;
		case kScriptVBScript:
			WriteCStrToOutput(".vbs");
			break;
	}
}

static void WriteScriptLangHeader(void)
{
	if (CurScriptLang == kScriptBash) {
		WriteLnCStrToOutput("#! /bin/bash");
		WriteEolToOutput();
	}
}

static void WriteSectionCommentDestFile(char * Description)
{
	WriteEolToOutput();
	WriteEolToOutput();
	switch (CurScriptLang) {
		case kScriptAppleScript:
			WriteCStrToOutput("\t--- ");
			WriteCStrToOutput(Description);
			WriteCStrToOutput(" -----");
			break;
		case kScriptVBScript:
			WriteCStrToOutput("' ----- ");
			WriteCStrToOutput(Description);
			WriteCStrToOutput(" -----");
			break;
		case kScriptMPW:
		case kScriptBash:
			WriteCStrToOutput("# ----- ");
			WriteCStrToOutput(Description);
			WriteCStrToOutput(" -----");
			break;
	}
	WriteEolToOutput();
}

#ifndef MPWOneEchoPerFile
#define MPWOneEchoPerFile 1
#endif

static void WriteOpenDestFile(char *DirVar, char *FileName, char *FileExt, char * Description)
{
	WriteSectionCommentDestFile(Description);

	WriteEolToOutput();
	switch (CurScriptLang) {
		case kScriptMPW:
#if MPWOneEchoPerFile
			WriteCStrToOutput("Echo -n > \"{");
			WriteCStrToOutput(DirVar);
			WriteCStrToOutput("}");
			WriteCStrToOutput(FileName);
			WriteCStrToOutput(FileExt);
			WriteCStrToOutput("\" \266");
			WriteEolToOutput();
#else
			WriteCStrToOutput("Set DestFile \"{");
			WriteCStrToOutput(DirVar);
			WriteCStrToOutput("}");
			WriteCStrToOutput(FileName);
			WriteCStrToOutput(FileExt);
			WriteCStrToOutput("\"");
			WriteEolToOutput();
			WriteLnCStrToOutput("Echo -n > \"{DestFile}\"");
			WriteEolToOutput();
#endif
			break;
		case kScriptAppleScript:
			WriteCStrToOutput("\tset DestFile to open for access file (");
			WriteCStrToOutput(DirVar);
			WriteCStrToOutput(" & \"");
			WriteCStrToOutput(FileName);
			WriteCStrToOutput(FileExt);
			WriteCStrToOutput("\") with write permission");
			WriteEolToOutput();
			WriteEolToOutput();
			WriteLnCStrToOutput("\tset eof DestFile to 0");
			break;
		case kScriptBash:
			WriteCStrToOutput("DestFile=\"${");
			WriteCStrToOutput(DirVar);
			WriteCStrToOutput("}");
			WriteCStrToOutput(FileName);
			WriteCStrToOutput(FileExt);
			WriteCStrToOutput("\"");
			WriteEolToOutput();
			WriteLnCStrToOutput("echo -n > \"${DestFile}\"");
			WriteEolToOutput();
			break;
		case kScriptVBScript:
			WriteCStrToOutput("Set f = fso.CreateTextFile(");
			WriteCStrToOutput(DirVar);
			WriteCStrToOutput(" & \"\\");
			WriteCStrToOutput(FileName);
			WriteCStrToOutput(FileExt);
			WriteCStrToOutput("\", True)");
			WriteEolToOutput();
			WriteEolToOutput();
			break;
	}
}

static void WriteCloseDestFile(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
#if MPWOneEchoPerFile
			WriteLnCStrToOutput("''");
#endif
			break;
		case kScriptAppleScript:
			WriteEolToOutput();
			WriteLnCStrToOutput("\tclose access DestFile");
			break;
		case kScriptBash:
			break;
		case kScriptVBScript:
			WriteEolToOutput();
			WriteLnCStrToOutput("f.Close");
			break;
	}
}

static void WriteBlankLineToDestFile(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
#if MPWOneEchoPerFile
			WriteLnCStrToOutput("''\266n\266");
#else
			WriteLnCStrToOutput("Echo '' >> \"{DestFile}\"");
#endif
			break;
		case kScriptAppleScript:
			WriteLnCStrToOutput("\twrite \"\" & return to DestFile");
			break;
		case kScriptBash:
			WriteLnCStrToOutput("echo '' >> \"${DestFile}\"");
			break;
		case kScriptVBScript:
			WriteLnCStrToOutput("f.WriteLine(\"\")");
			break;
	}
}

static int DestFileIndent = 0;

static void WriteBgnDestFileLn(void)
{
	int i;

	switch (CurScriptLang) {
		case kScriptMPW:
#if MPWOneEchoPerFile
			WriteCStrToOutput("'");
#else
			WriteCStrToOutput("Echo '");
#endif
			break;
		case kScriptAppleScript:
			WriteCStrToOutput("\twrite \"");
			break;
		case kScriptBash:
			WriteCStrToOutput("echo '");
			break;
		case kScriptVBScript:
			WriteCStrToOutput("f.WriteLine(\"");
			break;
	}
	for (i = 0; i < DestFileIndent; ++i) {
		WriteCStrToOutput("\t");
	}
}

static void WriteEndDestFileLn(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
#if MPWOneEchoPerFile
			WriteCStrToOutput("'\266n\266");
#else
			WriteCStrToOutput("' >> \"{DestFile}\"");
#endif
			break;
		case kScriptAppleScript:
			WriteCStrToOutput("\" & return to DestFile");
			break;
		case kScriptBash:
			WriteCStrToOutput("' >> \"${DestFile}\"");
			break;
		case kScriptVBScript:
			WriteCStrToOutput("\")");
			break;
	}
	WriteEolToOutput();
}

static void WriteDestFileLn(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput(s);
	WriteEndDestFileLn();
}

static void WriteQuoteToDestFile(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
		case kScriptBash:
			WriteCStrToOutput("\"");
			break;
		case kScriptAppleScript:
			WriteCStrToOutput("\\\"");
			break;
		case kScriptVBScript:
			WriteCStrToOutput("\"\"");
			break;
	}
}

static void WriteSingleQuoteToDestFile(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
			WriteCStrToOutput("'\266''");
			break;
		case kScriptBash:
			WriteCStrToOutput("'\\''");
			break;
		case kScriptAppleScript:
		case kScriptVBScript:
			WriteCStrToOutput("'");
			break;
	}
}

static void WriteBackSlashToDestFile(void)
{
	WriteCStrToOutput("\\");
}

static void WriteScriptVarToDestFile(char *name)
{
	switch (CurScriptLang) {
		case kScriptMPW:
			WriteCStrToOutput("'\"{");
			WriteCStrToOutput(name);
			WriteCStrToOutput("}\"'");
			break;
		case kScriptBash:
			WriteCStrToOutput("'\"${");
			WriteCStrToOutput(name);
			WriteCStrToOutput("}\"'");
			break;
		case kScriptAppleScript:
			WriteCStrToOutput("* not implemented yet *");
			break;
		case kScriptVBScript:
			WriteCStrToOutput("\" & ");
			WriteCStrToOutput(name);
			WriteCStrToOutput(" & \"");
			break;
	}
}

static void WriteBgnCommentBlock(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
		case kScriptBash:
		case kScriptVBScript:
			break;
		case kScriptAppleScript:
			WriteLnCStrToOutput("(*");
			break;
	}
}

static void WriteEndCommentBlock(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
		case kScriptBash:
		case kScriptVBScript:
			break;
		case kScriptAppleScript:
			WriteLnCStrToOutput("*)");
			break;
	}
}

static void WriteBgnCommentBlockLn(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
		case kScriptBash:
			WriteCStrToOutput("# ");
			break;
		case kScriptAppleScript:
			WriteCStrToOutput("\t");
			break;
		case kScriptVBScript:
			WriteCStrToOutput("' ");
			break;
	}
}

static void WriteEndCommentBlockLn(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
		case kScriptAppleScript:
		case kScriptBash:
		case kScriptVBScript:
			WriteEolToOutput();
			break;
	}
}

static void WriteCommentBlockLn(char *s)
{
	WriteBgnCommentBlockLn();
	WriteCStrToOutput(s);
	WriteEndCommentBlockLn();
}

static void FindSubDirectory(char *new_d, char *parent_d, char *name)
{
	WriteEolToOutput();
	switch (CurScriptLang) {
		case kScriptMPW:
			WriteCStrToOutput("Set ");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput(" \"{");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput("}");
			WriteCStrToOutput(name);
			WriteCStrToOutput(":\"");
			WriteEolToOutput();


			WriteCStrToOutput("IF not \"`exists -d -q \"{");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput("}\"`\"");
			WriteEolToOutput();

			WriteCStrToOutput("\tEcho \"{");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput("} is not an existing directory\"");
			WriteEolToOutput();

			WriteLnCStrToOutput("\tExit 1");

			WriteLnCStrToOutput("END");
			break;
		case kScriptAppleScript:
			WriteCStrToOutput("\tset ");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput(" to ");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput(" & \"");
			WriteCStrToOutput(name);
			WriteCStrToOutput(":\"");
			WriteEolToOutput();

			WriteCStrToOutput("\tif not (exists alias ");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput(") then");
			WriteEolToOutput();

			WriteCStrToOutput("\t\tdisplay dialog \"The folder '");
			WriteCStrToOutput(name);
			WriteCStrToOutput("' is missing from '\" & ");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput(" & \"'.\" buttons {\"OK\"} default button 1");
			WriteEolToOutput();

			WriteLnCStrToOutput("\t\treturn");
			WriteLnCStrToOutput("\tend if");
			break;
		case kScriptBash:
			WriteCStrToOutput(new_d);
			WriteCStrToOutput("=\"${");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput("}");
			WriteCStrToOutput(name);
			WriteCStrToOutput("/\"");
			WriteEolToOutput();

			WriteCStrToOutput("if test ! -d \"${");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput("}\" ; then");
			WriteEolToOutput();

			WriteCStrToOutput("\techo \"The folder '");
			WriteCStrToOutput(name);
			WriteCStrToOutput("' is missing from ${");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput("}\"");
			WriteEolToOutput();

			WriteLnCStrToOutput("\texit 1");
			WriteLnCStrToOutput("fi");
			break;
		case kScriptVBScript:
			WriteCStrToOutput("dim ");
			WriteCStrToOutput(new_d);
			WriteEolToOutput();

			WriteCStrToOutput(new_d);
			WriteCStrToOutput(" = ");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput(" & \"\\");
			WriteCStrToOutput(name);
			WriteCStrToOutput("\"");
			WriteEolToOutput();
			break;
	}
}

static void MakeSubDirectory(char *new_d, char *parent_d, char *name, char *FileExt)
{
	WriteEolToOutput();

	switch (CurScriptLang) {
		case kScriptMPW:
			WriteCStrToOutput("Set ");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput(" \"{");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput("}");
			WriteCStrToOutput(name);
			WriteCStrToOutput(FileExt);
			WriteCStrToOutput(":\"");
			WriteEolToOutput();

			WriteCStrToOutput("IF not \"`exists -q \"{");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput("}\"`\"");
			WriteEolToOutput();

			WriteCStrToOutput("\tNewFolder \"{");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput("}\"");
			WriteEolToOutput();

			WriteLnCStrToOutput("END");
			break;
		case kScriptAppleScript:
			WriteCStrToOutput("\tset ");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput(" to ");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput(" & \"");
			WriteCStrToOutput(name);
			WriteCStrToOutput(FileExt);
			WriteCStrToOutput(":\"");
			WriteEolToOutput();

			WriteCStrToOutput("\tmake new folder at alias ");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput(" with properties {name:\"");
			WriteCStrToOutput(name);
			WriteCStrToOutput(FileExt);
			WriteCStrToOutput("\"}");
			WriteEolToOutput();
			break;
		case kScriptBash:
			WriteCStrToOutput(new_d);
			WriteCStrToOutput("=\"${");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput("}");
			WriteCStrToOutput(name);
			WriteCStrToOutput(FileExt);
			WriteCStrToOutput("/\"");
			WriteEolToOutput();

			WriteCStrToOutput("if test ! -d \"${");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput("}\" ; then");
			WriteEolToOutput();

			WriteCStrToOutput("\tmkdir \"${");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput("}\"");
			WriteEolToOutput();

			WriteLnCStrToOutput("fi");
			break;
		case kScriptVBScript:
			WriteCStrToOutput("dim ");
			WriteCStrToOutput(new_d);
			WriteEolToOutput();

			WriteCStrToOutput(new_d);
			WriteCStrToOutput(" = ");
			WriteCStrToOutput(parent_d);
			WriteCStrToOutput(" & \"\\");
			WriteCStrToOutput(name);
			WriteCStrToOutput(FileExt);
			WriteCStrToOutput("\"");
			WriteEolToOutput();

			WriteCStrToOutput("if (NOT fso.FolderExists(");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput(")) Then");
			WriteEolToOutput();

			WriteCStrToOutput("\tfso.CreateFolder(");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput(")");
			WriteEolToOutput();

			WriteLnCStrToOutput("End If");
			break;
	}
}

#if 0
static void WriteCopyFile(char *old_d, char *name, char *new_d)
{
	switch (CurScriptLang) {
		case kScriptMPW:
		case kScriptVBScript:
			WriteLnCStrToOutput("* not implemented yet *");
			break;
		case kScriptAppleScript:
			WriteCStrToOutput("\tduplicate (alias (");
			WriteCStrToOutput(old_d);
			WriteCStrToOutput(" & \"");
			WriteCStrToOutput(name);
			WriteCStrToOutput("\")) to (alias ");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput(")");
			WriteEolToOutput();
			break;
		case kScriptBash:
			WriteCStrToOutput("cp \"${");
			WriteCStrToOutput(old_d);
			WriteCStrToOutput("}");
			WriteCStrToOutput(name);
			WriteCStrToOutput("\" \"${");
			WriteCStrToOutput(new_d);
			WriteCStrToOutput("}\"");
			WriteEolToOutput();
			break;
	}
}
#endif

static void WriteStartScript(void)
{
	WriteEolToOutput();
	switch (CurScriptLang) {
		case kScriptMPW:
			if (! CurUseAbsolute) {
				WriteLnCStrToOutput("IF {#} != 1");
				WriteLnCStrToOutput("\tEcho 'usage:'");
				WriteLnCStrToOutput("\tEcho 'setup.mpw my_output_d'");
				WriteLnCStrToOutput("\tExit 1");
				WriteLnCStrToOutput("END");
			} else {
				WriteLnCStrToOutput("IF {#} != 2");
				WriteLnCStrToOutput("\tEcho 'usage:'");
				WriteLnCStrToOutput("\tEcho 'setup.mpw my_output_d my_source_d'");
				WriteLnCStrToOutput("\tExit 1");
				WriteLnCStrToOutput("END");
			}
			WriteEolToOutput();
			WriteLnCStrToOutput("IF NOT \"`exists -d -q \"{1}\"`\"");
			WriteLnCStrToOutput("\tNewFolder \"{1}\"");
			WriteLnCStrToOutput("END");

			/* make sure we have an absolute path */
			WriteEolToOutput();
#if 0
			WriteLnCStrToOutput("Set my_output_d \"`ResolveAlias \"{1}\"`\"");
#endif
			WriteLnCStrToOutput("Set save_pwd \"`Directory -q`\"");
			WriteLnCStrToOutput("Directory \"{1}\"");
			WriteLnCStrToOutput("Set my_output_d \"`Directory`\"");
			WriteLnCStrToOutput("Directory \"{save_pwd}\"");

			WriteEolToOutput();
			if (CurUseAbsolute) {
				WriteLnCStrToOutput("Set my_source_d \"{2}\"");
				WriteLnCStrToOutput("IF NOT \"`exists -d -q \"{my_source_d}\"`\"");
				WriteLnCStrToOutput("\tEcho \"{my_source_d} is not an existing directory\"");
				WriteLnCStrToOutput("\tExit 1");
				WriteLnCStrToOutput("END");
#if 0
				WriteLnCStrToOutput("Set my_source_d \"`ResolveAlias \"{my_source_d}\"`\"");
#endif
				WriteLnCStrToOutput("Set save_pwd \"`Directory -q`\"");
				WriteLnCStrToOutput("Directory \"{my_source_d}\"");
				WriteLnCStrToOutput("Set my_source_d \"`Directory`\"");
				WriteLnCStrToOutput("Directory \"{save_pwd}\"");
			}
			break;
		case kScriptAppleScript:
			WriteLnCStrToOutput("tell application \"Finder\"");
			WriteLnCStrToOutput("\tactivate");
			WriteEolToOutput();
			WriteLnCStrToOutput("\tchoose file name with prompt (\"Where should the built program (and intermediate results) be put?\") default name \"drv\"");
			WriteLnCStrToOutput("\tset my_output_d to result as text");
			WriteEolToOutput();
			WriteLnCStrToOutput("\tset old_delimits to AppleScript's text item delimiters");
			WriteLnCStrToOutput("\tset AppleScript's text item delimiters to \":\"");
			WriteLnCStrToOutput("\tset my_output_parent to ((text items 1 thru ((count of text items of my_output_d) - 1) of my_output_d) as string) & \":\"");
			WriteLnCStrToOutput("\tset my_output_name to the last text item of the my_output_d");
			WriteLnCStrToOutput("\tset AppleScript's text item delimiters to old_delimits");
			WriteEolToOutput();
			WriteLnCStrToOutput("\tset my_output_d to (my_output_d & \":\")");
			WriteEolToOutput();
			WriteLnCStrToOutput("\tif (exists alias my_output_d) then");
			WriteLnCStrToOutput("\t\tdisplay dialog \"'\" & my_output_d & \"' already exists.\" buttons {\"OK\"} default button 1");
			WriteLnCStrToOutput("\t\treturn");
			WriteLnCStrToOutput("\tend if");
			WriteEolToOutput();
			WriteLnCStrToOutput("\tmake new folder at folder my_output_parent with properties {name:my_output_name}");
			if (CurUseAbsolute) {
				WriteEolToOutput();

				WriteCStrToOutput("\tchoose folder with prompt (\"Please find the \\\"");
				WriteCStrToOutput(kStrAppAbbrev);
				WriteCStrToOutput("\\\" folder.\")");
				WriteEolToOutput();

				WriteLnCStrToOutput("\tset my_source_d to result as text");
			}
			break;
		case kScriptBash:
			WriteSectionCommentDestFile("get arguments");
			WriteEolToOutput();
			if (! CurUseAbsolute) {
				WriteLnCStrToOutput("if test $# -ne 1 ; then");
				WriteLnCStrToOutput("\techo \"usage:\" >&2");
				WriteLnCStrToOutput("\techo \"setup.sh my_output_d\" >&2");
				WriteLnCStrToOutput("\texit 1");
				WriteLnCStrToOutput("fi");
			} else {
				WriteLnCStrToOutput("if test $# -ne 2 ; then");
				WriteLnCStrToOutput("\techo \"usage:\" >&2");
				WriteLnCStrToOutput("\techo \"setup.sh my_output_d my_source_d\" >&2");
				WriteLnCStrToOutput("\texit 1");
				WriteLnCStrToOutput("fi");
			}
			WriteEolToOutput();
			WriteLnCStrToOutput("if test ! -d \"${1}\" ; then");
			WriteLnCStrToOutput("\tmkdir \"${1}\"");
			WriteLnCStrToOutput("\tif test $? -ne 0 ; then");
			WriteLnCStrToOutput("\t\techo \"couldn't create directory ${1}\" >&2");
			WriteLnCStrToOutput("\t\texit 1");
			WriteLnCStrToOutput("\tfi");
			WriteLnCStrToOutput("fi");
			WriteEolToOutput();
			WriteLnCStrToOutput("save_pwd=\"$(pwd)/\"");
			WriteEolToOutput();
			WriteLnCStrToOutput("cd \"${1}\"");
			WriteLnCStrToOutput("my_output_d=\"`/bin/pwd`/\"");
			WriteLnCStrToOutput("cd \"${save_pwd}\"");
			if (CurUseAbsolute) {
				WriteEolToOutput();
				WriteLnCStrToOutput("my_source_d=\"${2}\"");
				WriteLnCStrToOutput("if test ! -d \"${my_source_d}\" ; then");
				WriteLnCStrToOutput("\techo \"${my_source_d} is not an existing directory\" >&2");
				WriteLnCStrToOutput("\texit 1");
				WriteLnCStrToOutput("fi");
				WriteEolToOutput();
				WriteLnCStrToOutput("cd \"${my_source_d}\"");
				WriteLnCStrToOutput("my_source_d=\"`/bin/pwd`/\"");
				WriteLnCStrToOutput("cd \"${save_pwd}\"");
			}
			break;
		case kScriptVBScript:
			WriteLnCStrToOutput("wscript.echo \"Starting\"");
			WriteEolToOutput();
			WriteLnCStrToOutput("dim f, fso, WshShell");
			WriteLnCStrToOutput("set fso = CreateObject(\"Scripting.FileSystemObject\")");
			WriteLnCStrToOutput("set WshShell = CreateObject(\"WScript.Shell\")");
			WriteEolToOutput();
			WriteLnCStrToOutput("dim my_args");
			WriteLnCStrToOutput("Set my_args = WScript.Arguments");
			if (! CurUseAbsolute) {
				WriteLnCStrToOutput("if (my_args.Count <> 1) then");
				WriteLnCStrToOutput("\twscript.echo \"usage:\"");
				WriteLnCStrToOutput("\twscript.echo \"setup.scpt my_output_d\"");
				WriteLnCStrToOutput("\twscript.quit");
				WriteLnCStrToOutput("end if");
			} else {
				WriteLnCStrToOutput("if (my_args.Count <> 2) then");
				WriteLnCStrToOutput("\twscript.echo \"usage:\"");
				WriteLnCStrToOutput("\twscript.echo \"setup.scpt my_output_d my_source_d\"");
				WriteLnCStrToOutput("\twscript.quit");
				WriteLnCStrToOutput("end if");
			}
			WriteEolToOutput();
			WriteLnCStrToOutput("dim my_output_d");
			WriteLnCStrToOutput("my_output_d = fso.GetAbsolutePathName(my_args(0))");
			WriteLnCStrToOutput("wscript.echo \"my_output_d: \" & my_output_d");
			WriteLnCStrToOutput("if (NOT fso.FolderExists(my_output_d)) Then");
			WriteLnCStrToOutput("\tdim my_output_parent");
			WriteLnCStrToOutput("\tmy_output_parent = fso.GetParentFolderName(my_output_d)");
			WriteLnCStrToOutput("\tif (NOT fso.FolderExists(my_output_parent)) Then");
			WriteLnCStrToOutput("\t\twscript.echo my_output_parent & \" is not an existing directory\"");
			WriteLnCStrToOutput("\t\twscript.quit");
			WriteLnCStrToOutput("\tEnd if");
			WriteLnCStrToOutput("\tfso.CreateFolder(my_output_d)");
			WriteLnCStrToOutput("End If");
			if (CurUseAbsolute) {
				WriteEolToOutput();
				WriteLnCStrToOutput("dim my_source_d");
				WriteLnCStrToOutput("my_source_d = my_output_d & \"\\\\source\"");
				WriteLnCStrToOutput("my_source_d = my_args(1)");
				WriteLnCStrToOutput("wscript.echo \"my_source_d: \" & my_source_d");
				WriteLnCStrToOutput("if (NOT fso.FolderExists(my_source_d)) Then");
				WriteLnCStrToOutput("\twscript.echo my_source_d & \" is not an existing directory\"");
				WriteLnCStrToOutput("\twscript.quit");
				WriteLnCStrToOutput("End If");
				WriteLnCStrToOutput("my_source_d = fso.GetAbsolutePathName(my_source_d)");
			}
			break;
	}
}

static void WriteEndScript(void)
{
	switch (CurScriptLang) {
		case kScriptMPW:
			break;
		case kScriptAppleScript:
			WriteEolToOutput();
			WriteLnCStrToOutput("end tell");
			break;
		case kScriptBash:
			break;
		case kScriptVBScript:
			WriteEolToOutput();
			WriteLnCStrToOutput("wscript.echo \"Finished\"");
			break;
	}
}

/* --- end of code specific to Scripting Language --- */


#if UsePragmaSegment
#pragma segment Seg4
#endif

static void WriteVersionStr(void)
{
	WriteCStrToOutput(kMajorVersion);
	WriteCStrToOutput(".");
	WriteCStrToOutput(kMinorVersion);
	WriteCStrToOutput(".");
	WriteCStrToOutput(kMinorSubVersion);
}

static void WriteAppVariationStr(void)
{
	WriteCStrToOutput(kStrAppAbbrev);
	WriteCStrToOutput("-");
	WriteVersionStr();
	WriteCStrToOutput("-");
	WriteCStrToOutput(GetIdeName(CurIde));
	WriteCStrToOutput(GetTargetName(CurTarget));
	WriteCStrToOutput(GetDbgLvlName(CurDbgLvl));
}

static void WriteGetInfoString(void)
{
	WriteAppVariationStr();
	WriteCStrToOutput(", Copyright ");
	WriteCStrToOutput(kStrCopyrightYear);
	WriteCStrToOutput(" maintained by ");
	WriteCStrToOutput(kMaintainerName);
	WriteCStrToOutput(".");
}

static void WriteHeaderComments(void)
{
	WriteBgnCommentBlock();

	WriteBgnCommentBlockLn();
	WriteCStrToOutput("setup");
	WriteScriptLangExtension();
	WriteEndCommentBlockLn();

	WriteCommentBlockLn("");

	WriteCommentBlockLn("This file was automatically generated from setup.c");

	WriteCommentBlockLn("");

	WriteBgnCommentBlockLn();
	WriteCStrToOutput("for: ");
	WriteAppVariationStr();
	WriteEndCommentBlockLn();

	WriteEndCommentBlock();
}

static void WriteFindSourceDirectories(void)
{
	WriteSectionCommentDestFile("find source directories");

	FindSubDirectory("my_c_src_d", "my_source_d", "c_src");
	if (HaveAsm) {
		FindSubDirectory("my_a_src_d", "my_source_d", "a_src");
	}
	FindSubDirectory("my_platform_d", "my_source_d", "platform");
	if (CurTarget == kTargWinx86) {
		FindSubDirectory("my_rz_src_d", "my_platform_d", "win");
	} else {
		FindSubDirectory("my_rz_src_d", "my_platform_d", "mac");
		if (CurTarget == kTargMacho) {
			FindSubDirectory("my_osxrz_d", "my_rz_src_d", "osx");
		}
	}
}

static void WriteMakeOutputDirectories(void)
{
	WriteSectionCommentDestFile("make output directories");

	MakeSubDirectory("my_config_d", "my_output_d", "config", "");
	if (CurIde == kIdeAPB) {
		MakeSubDirectory("my_proj_d", "my_output_d", kStrAppAbbrev, ".pbproj");
	} else if (CurIde != kIdeMW8) {
		MakeSubDirectory("my_obj_d", "my_output_d", "obj", "");

		WriteOpenDestFile("my_obj_d", "dummy", "", "Dummy");
		WriteDestFileLn("This file is here because some archive extraction");
		WriteDestFileLn("software will not create an empty directory.");
		WriteCloseDestFile();
	}
}

/* --- XML utilities --- */

typedef void (*MyProc)(void);

static void WriteXMLtagBegin(char *s)
{
	WriteCStrToOutput("<");
	WriteCStrToOutput(s);
	WriteCStrToOutput(">");
}

static void WriteXMLtagEnd(char *s)
{
	WriteCStrToOutput("</");
	WriteCStrToOutput(s);
	WriteCStrToOutput(">");
}

static void WriteBeginXMLtagLine(char *s)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin(s);
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteEndXMLtagLine(char *s)
{
	--DestFileIndent;
	WriteBgnDestFileLn();
	WriteXMLtagEnd(s);
	WriteEndDestFileLn();
}

static void WriteXMLtagBeginValEndLine(char *t, char *v)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin(t);
	WriteCStrToOutput(v);
	WriteXMLtagEnd(t);
	WriteEndDestFileLn();
}

static void WriteXMLtagBeginProcValEndLine(char *t, MyProc v)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin(t);
	v();
	WriteXMLtagEnd(t);
	WriteEndDestFileLn();
}

/* --- code for configuration files --- */

static void WriteWrongCNFGGLOB(void)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput("#error ");
	WriteQuoteToDestFile();
	WriteCStrToOutput("wrong CNFGGLOB.h");
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

static void WriteCheckPreDef(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput("#ifndef ");
	WriteCStrToOutput(s);
	WriteEndDestFileLn();
	WriteWrongCNFGGLOB();
	WriteDestFileLn("#endif");
}

static void WriteCheckPreNDef(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput("#ifdef ");
	WriteCStrToOutput(s);
	WriteEndDestFileLn();
	WriteWrongCNFGGLOB();
	WriteDestFileLn("#endif");
}

static void WriteCommonCNFGGLOB(void)
{
	WriteOpenDestFile("my_config_d", "CNFGGLOB", ".h", "C Configuration file");

	WriteDestFileLn("/* make sure this is correct CNFGGLOB */");

	switch (CurIde) {
		case kIdeMW8:
			WriteCheckPreDef("__MWERKS__");
			switch (TheCPUFam) {
				case kCPUFam68K:
					WriteCheckPreDef("__MC68K__");
					if (CurTarget == kTargMac68KFPU) {
						WriteCheckPreDef("__MC68881__");
					} else {
						WriteCheckPreNDef("__MC68881__");
					}
					break;
				case kCPUFamPPC:
					WriteCheckPreDef("__POWERPC__");
					break;
				case kCPUFamX86:
					WriteCheckPreDef("__INTEL__");
					break;
			}
			break;
		case kIdeMPW3_6_a1:
			if (TheCPUFam == kCPUFamPPC) {
				WriteCheckPreDef("__MRC__");
			} else if (TheCPUFam == kCPUFam68K) {
				WriteCheckPreDef("__SC__");
				WriteCheckPreNDef("powerc");
				WriteCheckPreNDef("__powerc");
				if (CurTarget == kTargMac68KFPU) {
					WriteCheckPreDef("mc68881");
				} else {
					WriteCheckPreNDef("mc68881");
				}
			}
			break;
		case kIdeBashGcc:
		case kIdeAPB:
		case kIdeRH7:
			WriteCheckPreDef("__GNUC__");
			break;
		case kIdeMSVC:
			WriteCheckPreDef("_MSC_VER");
			WriteCheckPreDef("_M_IX86");
			break;
		case kIdeLccW32:
			WriteCheckPreDef("__LCC__");
			break;
	}

	WriteBlankLineToDestFile();
	switch (CurIde) {
		case kIdeMW8:
			WriteDestFileLn("#define MyCompilerMetrowerks 1");
			break;
		case kIdeMPW3_6_a1:
			if (TheCPUFam == kCPUFamPPC) {
				WriteDestFileLn("#define MyCompilerMrC 1");
			} else {
				WriteDestFileLn("#define MyCompilerSC 1");
			}
			break;
		case kIdeBashGcc:
		case kIdeAPB:
		case kIdeDevC:
		case kIdeRH7:
			WriteDestFileLn("#define MyCompilerGcc 1");
			break;
		case kIdeMSVC:
			WriteDestFileLn("#define MyCompilerMSVC 1");
			break;
		case kIdeLccW32:
			WriteDestFileLn("#define MyCompilerLCC 1");
			break;
	}

	switch (TheCPUFam) {
		case kCPUFam68K:
		case kCPUFamPPC:
			WriteDestFileLn("#define BigEndianUnaligned 1");
			break;
	}
	if (kCPUFam68K == TheCPUFam) {
		WriteDestFileLn("#define HaveCPUfamM68K 1");
	}

	if (CurIde == kIdeMW8) {
		WriteDestFileLn("#define MayInline __inline__");
	} else if (CurIde == kIdeMSVC) {
		WriteDestFileLn("#define MayInline __forceinline");
	}

	if ((CurIde == kIdeBashGcc) || (CurIde == kIdeAPB) || (CurIde == kIdeRH7)) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define cIncludeUnused 0");
	} else if (CurIde == kIdeLccW32) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define cIncludeUnused 0");
		WriteDestFileLn("#define UnusedParam(x)");
	} else if (CurIde == kIdeDevC) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define cIncludeUnused 0");
	}

	WriteBlankLineToDestFile();
	if (CurDbgLvl != kDbgLvlShip) {
		WriteDestFileLn("#define Debug 1");
	}
	WriteBlankLineToDestFile();
	switch (CurTarget) {
		case kTargMac68K:
		case kTargMac68KFPU:
		case kTargClassicPPC:
		case kTargCarbon:
		case kTargMacho:
			WriteDestFileLn("#define MacTarget 1");
			break;
		case kTargWinx86:
			WriteDestFileLn("#define WinTarget 1");
			break;
		case kTargMacX11:
		case kTargLinuxx86:
			WriteDestFileLn("#define XWnTarget 1");
			break;
	}

	switch (CurTarget) {
		case kTargMac68K:
		case kTargMac68KFPU:
		case kTargClassicPPC:
		case kTargCarbon:
		case kTargMacho:
		case kTargWinx86:
			WriteDestFileLn("#define MySoundEnabled 1");
			break;
	}

	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToOutput("#define kStrAppName ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(kStrAppName);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("#define kAppVariationStr ");
	WriteQuoteToDestFile();
	WriteAppVariationStr();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("#define kStrCopyrightYear ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(kStrCopyrightYear);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("#define kMaintainerName ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(kMaintainerName);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("#define kStrHomePage ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(kStrHomePage);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	if (CurIde == kIdeMSVC) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("/* --- set up compiler options --- */");
		WriteBlankLineToDestFile();
		WriteDestFileLn("/* ignore integer conversion warnings */");
		WriteDestFileLn("#pragma warning(disable : 4244 4761 4018 4245 4024 4305)");
		WriteBlankLineToDestFile();
		WriteDestFileLn("/* ignore unused inline warning */");
		WriteDestFileLn("#pragma warning(disable : 4514)");
		WriteBlankLineToDestFile();
		WriteDestFileLn("/* ignore type redefinition warning */");
		WriteDestFileLn("#pragma warning(disable : 4142)");
	}
	if (CurIde == kIdeMW8) {
		if (CurDbgLvl != kDbgLvlQuick) {
			WriteBlankLineToDestFile();
			WriteDestFileLn("#ifdef OptForSpeed");
			WriteDestFileLn("#pragma optimize_for_size off");
			WriteDestFileLn("#endif");
		}
	}
	
	if (kCPUFam68K == TheCPUFam) {
		WriteDestFileLn("#define CurEmu kEmuPlus2M");
	}

	WriteBlankLineToDestFile();

	WriteCloseDestFile();
}

static void WriteCommonCNFGRAPI(void)
{
	WriteOpenDestFile("my_config_d", "CNFGRAPI", ".h", "C API Configuration file");
	if (CurIde == kIdeMSVC) {
		WriteDestFileLn("/* ignore warning generated by system includes */");
		WriteDestFileLn("#pragma warning(push)");
		WriteDestFileLn("#pragma warning(disable : 4201 4115 4214)");
		WriteBlankLineToDestFile();
	}
	if (CurTarget == kTargMacho) {
		/* kIdeMW8 or kIdeBashGcc or kIdeAPB */
		if (CurIde == kIdeMW8) {
			WriteDestFileLn("#include <MSL MacHeadersMach-O.h>");
		}
		WriteDestFileLn("#include <Carbon/Carbon.h>");
	} else if ((CurTarget == kTargMacX11) || (CurTarget == kTargLinuxx86)) {
		WriteDestFileLn("#include <stdio.h>");
		WriteDestFileLn("#include <stdlib.h>");
		WriteDestFileLn("#include <string.h>");
		WriteDestFileLn("#include <time.h>");
		WriteDestFileLn("#include <sys/time.h>");
		WriteDestFileLn("#include <sys/times.h>");
		WriteDestFileLn("#include <X11/Xlib.h>");
		WriteDestFileLn("#include <X11/Xutil.h>");
		WriteDestFileLn("#include <X11/keysym.h>");
		WriteDestFileLn("#include <X11/keysymdef.h>");
		WriteDestFileLn("#include <X11/Xatom.h>");
	} else if (CurTarget == kTargCarbon) {
		/* kIdeMW8 or kIdeMPW3_6_a1 */
		if (CurIde == kIdeMW8) {
			WriteDestFileLn("#include <MacHeadersCarbon.h>");
		} else {
			WriteDestFileLn("#include <Carbon.h>");
		}
	} else if (CurTarget == kTargWinx86) {
		/* kIdeMW8 or kIdeMSVC */
		if (CurIde == kIdeMW8) {
			WriteDestFileLn("#include <Win32Headers.h>");
		} else {
			WriteDestFileLn("#include <windows.h>");
			WriteDestFileLn("#include <time.h>");
			if (CurIde == kIdeLccW32) {
				WriteDestFileLn("#include <shellapi.h>");
				WriteDestFileLn("#include <mmsystem.h>");
			}
		}
	} else {
		if (CurIde == kIdeMW8) {
			WriteDestFileLn("#include <MacHeaders.h>");
			WriteDestFileLn("#include <CursorDevices.h>");
			WriteDestFileLn("#define ShouldDefineQDGlobals 0");
		} else if (CurIde == kIdeMPW3_6_a1) {
			WriteDestFileLn("#include <MacTypes.h>");
			if (TheCPUFam != kCPUFam68K) {
				WriteDestFileLn("#include <MixedMode.h>");
			}
			WriteDestFileLn("#include <Gestalt.h>");
			WriteDestFileLn("#include <MacErrors.h>");
			WriteDestFileLn("#include <MacMemory.h>");
			WriteDestFileLn("#include <OSUtils.h>");
			WriteDestFileLn("#include <QuickdrawText.h>");
			WriteDestFileLn("#include <QuickDraw.h>");
			if (TheCPUFam == kCPUFam68K) {
				WriteDestFileLn("#include <SegLoad.h>");
			}
			WriteDestFileLn("#include <IntlResources.h>");
			WriteDestFileLn("#include <Events.h>");
			WriteDestFileLn("#include <Script.h>");
			WriteDestFileLn("#include <Files.h>");
			WriteDestFileLn("#include <Resources.h>");
			WriteDestFileLn("#include <Fonts.h>");
			WriteDestFileLn("#include <TextUtils.h>");
			WriteDestFileLn("#include <FixMath.h>");
			WriteDestFileLn("#include <ToolUtils.h>");
			WriteDestFileLn("#include <Menus.h>");
			WriteDestFileLn("#include <Scrap.h>");
			WriteDestFileLn("#include <Controls.h>");
			WriteDestFileLn("#include <ControlDefinitions.h>");
			WriteDestFileLn("#include <AppleEvents.h>");
			WriteDestFileLn("#include <Processes.h>");
			WriteDestFileLn("#include <EPPC.h>");
			WriteDestFileLn("#include <MacWindows.h>");
			WriteDestFileLn("#include <TextEdit.h>");
			WriteDestFileLn("#include <Dialogs.h>");
			WriteDestFileLn("#include <Devices.h>");
			WriteDestFileLn("#include <Palettes.h>");
			WriteDestFileLn("#include <StandardFile.h>");
			WriteDestFileLn("#include <Aliases.h>");
			WriteDestFileLn("#include <Folders.h>");
			WriteDestFileLn("#include <Balloons.h>");
			WriteDestFileLn("#include <DiskInit.h>");
			WriteDestFileLn("#include <LowMem.h>");
			WriteDestFileLn("#include <Appearance.h>");
			WriteDestFileLn("#include <Navigation.h>");
			WriteDestFileLn("#include <Sound.h>");
			WriteDestFileLn("#include <CursorDevices.h>");
			WriteDestFileLn("#include <Traps.h>");
		}
	}
	if (CurIde == kIdeMSVC) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("/* restore warnings */");
		WriteDestFileLn("#pragma warning(pop)");
	}

	if (TheCPUFam == kCPUFam68K) {
		if (CurIde == kIdeMPW3_6_a1) {
			WriteBlankLineToDestFile();
			WriteDestFileLn("#define ShouldUnloadDataInit 1");
			WriteDestFileLn("#define Windows85APIAvail 0");
		}
	}

	if (CurTarget == kTargMacho) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define MainReturnsInt 1");
		WriteDestFileLn("#define MyAppIsBundle 1");
	}
	switch (CurTarget) {
		case kTargMac68K:
		case kTargMac68KFPU:
		case kTargClassicPPC:
		case kTargCarbon:
		case kTargMacho:
			WriteBlankLineToDestFile();
			WriteBgnDestFileLn();
			WriteCStrToOutput("#define kMacCreatorSig ");
			WriteSingleQuoteToDestFile();
			WriteCStrToOutput(kMacCreatorSig);
			WriteSingleQuoteToDestFile();
			WriteEndDestFileLn();
			break;
	}
	WriteCloseDestFile();
}

static void WriteQuotedInclude(char *name)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput("#include ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(name);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

static void WriteBeginResResource(char *types, int id)
{
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToOutput("resource ");
	WriteSingleQuoteToDestFile();
	WriteCStrToOutput(types);
	WriteSingleQuoteToDestFile();
	WriteCStrToOutput(" (");
	WriteUnsignedToOutput(id);
	WriteCStrToOutput(") {");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteEndResResource(void)
{
	--DestFileIndent;
	WriteDestFileLn("};");
}

static void WriteResTypeComma(char *types)
{
	WriteBgnDestFileLn();
	WriteSingleQuoteToDestFile();
	WriteCStrToOutput(types);
	WriteSingleQuoteToDestFile();
	WriteCStrToOutput(",");
	WriteEndDestFileLn();
}

static void WriteAFREFres(char *types, int i)
{
	WriteBeginResResource("FREF", 128+i);
		WriteResTypeComma(types);

		WriteBgnDestFileLn();
		WriteUnsignedToOutput(i);
		WriteCStrToOutput(",");
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteQuoteToDestFile();
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
	WriteEndResResource();
}

static unsigned int DocTypeCounter;

#ifndef UnusedParam
#define UnusedParam(p) (void) p
#endif

static void WriteDocTypeDefIconId(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	WriteBgnDestFileLn();
	WriteCStrToOutput("#define ");
	WriteCStrToOutput(ShortName);
	WriteCStrToOutput("IconId ");
	WriteUnsignedToOutput(128 + (++DocTypeCounter));
	WriteEndDestFileLn();
}

static void WriteDocTypeFREFres(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(ShortName);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	WriteAFREFres(MacType, ++DocTypeCounter);
}

static void WriteDocTypeBNDLFREF(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(ShortName);
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	++DocTypeCounter;
	WriteBgnDestFileLn();
	WriteCStrToOutput(",");
	WriteUnsignedToOutput(DocTypeCounter);
	WriteCStrToOutput(", ");
	WriteUnsignedToOutput(128+DocTypeCounter);
	WriteEndDestFileLn();
}

static void WriteDocTypeBNDLICN(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	WriteBgnDestFileLn();
	WriteCStrToOutput(",");
	WriteUnsignedToOutput(++DocTypeCounter);
	WriteCStrToOutput(", ");
	WriteCStrToOutput(ShortName);
	WriteCStrToOutput("IconId");
	WriteEndDestFileLn();
}

static void WriteDocTypeIncludeIconFile(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	WriteBgnDestFileLn();
	WriteCStrToOutput("#include ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(ShortName);
	WriteCStrToOutput("Icon.r");
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

static void WriteCommonCNFGRSRC(void)
{
	WriteOpenDestFile("my_config_d", "CNFGRSRC", ".h", "Resource Configuration file");

	WriteBlankLineToDestFile();
	if (CurIde == kIdeMW8) {
		if (CurTarget == kTargMacho) {
			WriteDestFileLn("#include <Carbon/Carbon.r>");
		} else {
			WriteQuotedInclude("Types.r");
			WriteQuotedInclude("Icons.r");
		}
	} else if ((CurIde == kIdeBashGcc) || (CurIde == kIdeAPB)) {
		WriteQuotedInclude("Carbon.r");
	} else if (CurIde == kIdeMPW3_6_a1) {
		WriteQuotedInclude("Types.r");
		WriteQuotedInclude("Icons.r");
	}

	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToOutput("#define kStrAppName ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(kStrAppName);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("#define kAppVariationStr ");
	WriteQuoteToDestFile();
	WriteAppVariationStr();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("#define kStrCopyrightYear ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(kStrCopyrightYear);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("#define kStrHomePage ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(kStrHomePage);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	if (CurTarget != kTargMacho) {
		if (CurTarget == kTargCarbon) {
			WriteBlankLineToDestFile();
			WriteBgnDestFileLn();
			WriteCStrToOutput("data ");
			WriteSingleQuoteToDestFile();
			WriteCStrToOutput("plst");
			WriteSingleQuoteToDestFile();
			WriteCStrToOutput(" (0) {");
			WriteEndDestFileLn();
			++DestFileIndent;
				WriteBgnDestFileLn();
				WriteCStrToOutput("$");
				WriteQuoteToDestFile();
				WriteCStrToOutput("00");
				WriteQuoteToDestFile();
				WriteEndDestFileLn();
			--DestFileIndent;
			WriteDestFileLn("};");
		}

		WriteBlankLineToDestFile();
		WriteBgnDestFileLn();
		WriteCStrToOutput("data ");
		WriteSingleQuoteToDestFile();
		WriteCStrToOutput(kMacCreatorSig);
		WriteSingleQuoteToDestFile();
		WriteCStrToOutput(" (0, ");
		WriteQuoteToDestFile();
		WriteCStrToOutput("Owner resource");
		WriteQuoteToDestFile();
		WriteCStrToOutput(") {");
		WriteEndDestFileLn();
		++DestFileIndent;
			WriteBgnDestFileLn();
			WriteCStrToOutput("$");
			WriteQuoteToDestFile();
			WriteCStrToOutput("00");
			WriteQuoteToDestFile();
			WriteEndDestFileLn();
		--DestFileIndent;
		WriteDestFileLn("};");

		WriteBeginResResource("vers", 1);
			WriteBgnDestFileLn();
			WriteCStrToOutput(kMajorVersion);
			WriteCStrToOutput(",");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToOutput(kMinorVersion);
			WriteCStrToOutput(" * 16 + ");
			WriteCStrToOutput(kMinorSubVersion);
			WriteCStrToOutput(",");
			WriteEndDestFileLn();

			WriteDestFileLn("release,");
			WriteDestFileLn("0x0,");
			WriteDestFileLn("0,");

			WriteBgnDestFileLn();
			WriteQuoteToDestFile();
			WriteVersionStr();
			WriteQuoteToDestFile();
			WriteCStrToOutput(",");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteQuoteToDestFile();
			WriteGetInfoString();
			WriteQuoteToDestFile();
			WriteEndDestFileLn();
		WriteEndResResource();

#if 0
		WriteBeginResResource("vers", 2);
			WriteDestFileLn("0x0,");
			WriteDestFileLn("0x0,");
			WriteDestFileLn("release,");
			WriteDestFileLn("0x0,");
			WriteDestFileLn("0,");
			WriteBgnDestFileLn();
			WriteQuoteToDestFile();
			WriteQuoteToDestFile();
			WriteCStrToOutput(",");
			WriteEndDestFileLn();
			WriteBgnDestFileLn();
			WriteQuoteToDestFile();
			WriteCStrToOutput(kShortDescription);
			WriteQuoteToDestFile();
			WriteEndDestFileLn();
		WriteEndResResource();
#endif

		if (CurIde != kIdeMW8) {
			WriteBlankLineToDestFile();
			WriteBgnDestFileLn();
			WriteCStrToOutput("resource ");
			WriteSingleQuoteToDestFile();
			WriteCStrToOutput("SIZE");
			WriteSingleQuoteToDestFile();
			WriteCStrToOutput(" (-1) {");
			WriteEndDestFileLn();
			++DestFileIndent;
				WriteDestFileLn("reserved,");
				WriteDestFileLn("acceptSuspendResumeEvents,");
				WriteDestFileLn("reserved,");
				WriteDestFileLn("canBackground,");
				WriteDestFileLn("multiFinderAware,");
				WriteDestFileLn("backgroundAndForeground,");
				WriteDestFileLn("dontGetFrontClicks,");
				WriteDestFileLn("ignoreChildDiedEvents,");
				WriteDestFileLn("is32BitCompatible,");

				/* following 4 should be "reserved" if api not available */
				WriteDestFileLn("isHighLevelEventAware,");
				WriteDestFileLn("localAndRemoteHLEvents,");
				WriteDestFileLn("isStationeryAware,");
				WriteDestFileLn("useTextEditServices,");

				WriteDestFileLn("reserved,");
				WriteDestFileLn("reserved,");
				WriteDestFileLn("reserved,");
				WriteDestFileLn("5242880,");
				WriteDestFileLn("1572864");
			WriteEndResResource();
		}

		WriteBlankLineToDestFile();
		WriteDestFileLn("#define AppIconId 128");
		DocTypeCounter = 0;
		DoAllDocTypes(WriteDocTypeDefIconId);


		WriteAFREFres("APPL", 0);
		DocTypeCounter = 0;
		DoAllDocTypes(WriteDocTypeFREFres);
		WriteAFREFres("****", ++DocTypeCounter);

		WriteBeginResResource("BNDL", 128);
			WriteResTypeComma(kMacCreatorSig);

			WriteDestFileLn("0,");
			WriteDestFileLn("{");
			++DestFileIndent;
				WriteResTypeComma("FREF");

				WriteDestFileLn("{");
				++DestFileIndent;
					WriteDestFileLn("0, 128");
					DocTypeCounter = 0;
					DoAllDocTypes(WriteDocTypeBNDLFREF);
					WriteDocTypeBNDLFREF(nullpr, nullpr, nullpr, nullpr);
				--DestFileIndent;
				WriteDestFileLn("},");

				WriteResTypeComma("ICN#");

				WriteDestFileLn("{");
				++DestFileIndent;
					WriteDestFileLn("0, AppIconId");
					DocTypeCounter = 0;
					DoAllDocTypes(WriteDocTypeBNDLICN);
					WriteBgnDestFileLn();
					WriteCStrToOutput(",");
					WriteUnsignedToOutput(++DocTypeCounter);
					WriteCStrToOutput(", 0");
					WriteEndDestFileLn();
				--DestFileIndent;
				WriteDestFileLn("}");
			--DestFileIndent;
			WriteDestFileLn("}");
		WriteEndResResource();

		WriteBlankLineToDestFile();
		WriteDestFileLn("#define SmallIconAPIAvail 1");
		WriteDestFileLn("#define ColorIconAPIAvail 1");

		WriteBlankLineToDestFile();
		WriteQuotedInclude("AppIcon.r");
		DoAllDocTypes(WriteDocTypeIncludeIconFile);
	}

	WriteCloseDestFile();
}

enum {
	kPListRaw, /* native plist data */
	kPListPLC, /* metrowerks property list compiler */
	kNumPListFormats
};

static int CurPListFormat = kPListPLC;

static void WritePListProcString(MyProc p)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginProcValEndLine("string", p);
	} else {
		WriteBgnDestFileLn();
		WriteCStrToOutput("string ");
		WriteQuoteToDestFile();
		p();
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
	}
}

static char *pDt = nullpr;

static void WritepDtString(void)
{
	WriteCStrToOutput(pDt);
}

static void WritePListString(char *s)
{
	char *SavepDt = pDt;

	pDt = s;
	WritePListProcString(WritepDtString);
	pDt = SavepDt;
}

static void WritePListKeyProcString(char *k, MyProc p)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginValEndLine("key", k);
		WriteXMLtagBeginProcValEndLine("string", p);
	} else {
		WriteBgnDestFileLn();
		WriteCStrToOutput("key ");
		WriteQuoteToDestFile();
		WriteCStrToOutput(k);
		WriteQuoteToDestFile();
		WriteCStrToOutput(" value string ");
		WriteQuoteToDestFile();
		p();
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
	}
}

static void WritePListKeyString(char *k, char *s)
{
	char *SavepDt = pDt;

	pDt = s;
	WritePListKeyProcString(k, WritepDtString);
	pDt = SavepDt;
}

static void WritePListBeginKeyArray(char *k)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginValEndLine("key", k);
		WriteBeginXMLtagLine("array");
	} else {
		WriteBgnDestFileLn();
		WriteCStrToOutput("key ");
		WriteQuoteToDestFile();
		WriteCStrToOutput(k);
		WriteQuoteToDestFile();
		WriteCStrToOutput(" value array");
		WriteEndDestFileLn();
		WriteDestFileLn("[");
		++DestFileIndent;
	}
}

static void WritePListEndKeyArray(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteEndXMLtagLine("array");
	} else {
		--DestFileIndent;
		WriteDestFileLn("]");
	}
}

static void WritePListBeginDict(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteBeginXMLtagLine("dict");
	} else {
		WriteBgnDestFileLn();
		WriteCStrToOutput("dictionary");
		WriteEndDestFileLn();
		WriteDestFileLn("{");
		++DestFileIndent;
	}
}

static void WritePListEndDict(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteEndXMLtagLine("dict");
	} else {
		--DestFileIndent;
		WriteDestFileLn("}");
	}
}

static void WriteInfoPList(MyProc p)
{
	CurPListFormat = kPListRaw;

	WriteBgnDestFileLn();
	WriteCStrToOutput("<?xml version=");
	WriteQuoteToDestFile();
	WriteCStrToOutput("1.0");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" encoding=");
	WriteQuoteToDestFile();
	WriteCStrToOutput("UTF-8");
	WriteQuoteToDestFile();
	WriteCStrToOutput("?>");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("<!DOCTYPE plist SYSTEM ");
	WriteQuoteToDestFile();
	WriteCStrToOutput("file://localhost/System/Library/DTDs/PropertyList.dtd");
	WriteQuoteToDestFile();
	WriteCStrToOutput(">");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("<plist version=");
	WriteQuoteToDestFile();
	WriteCStrToOutput("0.9");
	WriteQuoteToDestFile();
	WriteCStrToOutput(">");
	WriteEndDestFileLn();

	p();

	WriteDestFileLn("</plist>");
}

static void WriteMainPLC(MyProc p)
{
	CurPListFormat = kPListPLC;

	WriteBgnDestFileLn();
	WriteCStrToOutput("plist");
	WriteEndDestFileLn();
	WriteDestFileLn("{");
	++DestFileIndent;

	p();

	--DestFileIndent;
	WriteDestFileLn("}");
}

static void WritepDIcontMachoFile(void)
{
	WritepDtString();
	WriteCStrToOutput("Icon.icns");
}

static void WritepDtIconTypeName(void)
{
	WriteCStrToOutput(kStrAppName);
	WriteCStrToOutput(" ");
	WritepDtString();
}

static void WriteOneExtension2Array(char *s)
{
	WritePListString(s);
}

static void WriteOneCFBundleDocumentType(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	char *SavepDt = pDt;

	WritePListBeginDict();
		pDt = ShortName;
		WritePListKeyProcString("CFBundleTypeIconFile", WritepDIcontMachoFile);
		WritePListBeginKeyArray("CFBundleTypeOSTypes");
			pDt = MacType;
			WritePListProcString(WritepDtString);
		WritePListEndKeyArray();
		if (WriteExtensionList != nullpr) {
			WritePListBeginKeyArray("CFBundleTypeExtensions");
				WriteExtensionList(WriteOneExtension2Array);
			WritePListEndKeyArray();
		}
		pDt = LongName;
		WritePListKeyProcString("CFBundleTypeName", WritepDtIconTypeName);
		WritePListKeyString("CFBundleTypeRole", "Editor");
	WritePListEndDict();
	pDt = SavepDt;
}

static void WriteMyInfoPListContents(void)
{
	WritePListBeginDict();
		WritePListKeyString("CFBundleInfoDictionaryVersion", "6.0");
		WritePListKeyString("CFBundleIdentifier", kBundleIdentifier);

		WritePListKeyString("CFBundleName", kStrAppName);

		WritePListKeyString("CFBundlePackageType", "APPL");
		WritePListKeyString("CFBundleSignature", kMacCreatorSig);
		WritePListKeyString("CFBundleDevelopmentRegion", "English");
		WritePListKeyString("LSRequiresCarbon", "true/");
		WritePListKeyProcString("CFBundleVersion", WriteVersionStr);
		WritePListKeyProcString("CFBundleShortVersionString", WriteVersionStr);
		WritePListKeyProcString("CFBundleGetInfoString", WriteGetInfoString);

		WritePListKeyString("CFBundleExecutable", kStrAppAbbrev);
		WritePListKeyString("CFBundleIconFile", "AppIcon.icns");
		WritePListBeginKeyArray("CFBundleDocumentTypes");
			WritePListBeginDict();
				WritePListBeginKeyArray("CFBundleTypeOSTypes");
					WritePListString("****");
				WritePListEndKeyArray();
				WritePListKeyString("CFBundleTypeRole", "Editor");
			WritePListEndDict();
			DoAllDocTypes(WriteOneCFBundleDocumentType);
		WritePListEndKeyArray();
	WritePListEndDict();
}

static void WritePListData(void)
{
	if (CurIde == kIdeMW8) {
		WriteOpenDestFile("my_config_d", "main", ".plc", "plist source");
		WriteMainPLC(WriteMyInfoPListContents);
		WriteCloseDestFile();
	} else {
		WriteOpenDestFile("my_config_d", "Info", ".plist", "Info.plist file");
		WriteInfoPList(WriteMyInfoPListContents);
		WriteCloseDestFile();
	}
}

static void WriteConfigFiles(void)
{
	WriteCommonCNFGGLOB();
	WriteCommonCNFGRAPI();
	switch (CurTarget) {
		case kTargMac68K:
		case kTargMac68KFPU:
		case kTargClassicPPC:
		case kTargCarbon:
			WriteCommonCNFGRSRC();
			break;
	}
	if (CurTarget == kTargMacho) {
		if (CurIde != kIdeAPB) {
			WritePListData();
		}
	}
}

/* --- end of code for configuration files --- */


static void Write_output_d_ToDestFile(void)
{
	if (! CurUseAbsolute) {
		switch (CurIde) {
			case kIdeMW8:
			case kIdeMPW3_6_a1:
				WriteCStrToOutput(":");
				break;
			case kIdeBashGcc:
			case kIdeAPB:
			case kIdeRH7:
				WriteCStrToOutput("./");
				break;
			case kIdeMSVC:
				WriteCStrToOutput(".");
				break;
			case kIdeLccW32:
				WriteCStrToOutput("c:\\output");
				break;
		}
	} else {
		WriteScriptVarToDestFile("my_output_d");
	}
}

static void Write_source_d_ToDestFile(void)
{
	if (! CurUseAbsolute) {
		switch (CurIde) {
			case kIdeMW8:
			case kIdeMPW3_6_a1:
				WriteCStrToOutput(":::source:");
				break;
			case kIdeBashGcc:
			case kIdeAPB:
			case kIdeRH7:
				WriteCStrToOutput("../../source/");
				break;
			case kIdeMSVC:
			case kIdeDevC:
				WriteCStrToOutput("..\\..\\source");
				break;
			case kIdeLccW32:
				WriteCStrToOutput("c:\\output\\..\\..\\source");
				break;
		}
	} else {
		WriteScriptVarToDestFile("my_source_d");
	}
}

static void WriteSubDirToDestFile(MyProc p, char *s)
{
	p();
	switch (CurIde) {
		case kIdeMW8:
		case kIdeMPW3_6_a1:
			WriteCStrToOutput(s);
			WriteCStrToOutput(":");
			break;
		case kIdeBashGcc:
		case kIdeAPB:
		case kIdeRH7:
			WriteCStrToOutput(s);
			WriteCStrToOutput("/");
			break;
		case kIdeMSVC:
		case kIdeLccW32:
		case kIdeDevC:
			WriteBackSlashToDestFile();
			WriteCStrToOutput(s);
			break;
	}
}

static void WriteFileInDirToDestFile(MyProc p, char *s, char *FileExt)
{
	p();
	switch (CurIde) {
		case kIdeMW8:
		case kIdeMPW3_6_a1:
			WriteCStrToOutput(s);
			break;
		case kIdeBashGcc:
		case kIdeAPB:
		case kIdeRH7:
			WriteCStrToOutput(s);
			break;
		case kIdeMSVC:
		case kIdeLccW32:
		case kIdeDevC:
			WriteBackSlashToDestFile();
			WriteCStrToOutput(s);
			break;
	}
	WriteCStrToOutput(FileExt);
}

static void Write_c_src_d_ToDestFile(void)
{
	if (! CurUseAbsolute) {
		WriteSubDirToDestFile(Write_source_d_ToDestFile, "c_src");
	} else {
		WriteScriptVarToDestFile("my_c_src_d");
	}
}

static void Write_a0_src_d_ToDestFile(void)
{
	if (! CurUseAbsolute) {
		WriteSubDirToDestFile(Write_source_d_ToDestFile, "a_src");
	} else {
		WriteScriptVarToDestFile("my_a_src_d");
	}
}

static void Write_a1_src_d_ToDestFile(void)
{
	WriteSubDirToDestFile(Write_a0_src_d_ToDestFile, "ppc");
}

static void Write_a_src_d_ToDestFile(void)
{
	if (CurTarget == kTargMacho) {
		WriteSubDirToDestFile(Write_a1_src_d_ToDestFile, "as");
	} else {
		WriteSubDirToDestFile(Write_a1_src_d_ToDestFile, "ppcasm");
	}
}

static void Write_platform_d_ToDestFile(void)
{
	if (! CurUseAbsolute) {
		WriteSubDirToDestFile(Write_source_d_ToDestFile, "platform");
	} else {
		WriteScriptVarToDestFile("my_platform_d");
	}
}

static void Write_rz_src_ToDestFile(void)
{
	if (! CurUseAbsolute) {
		if (CurTarget == kTargWinx86) {
			WriteSubDirToDestFile(Write_platform_d_ToDestFile, "win");
		} else {
			WriteSubDirToDestFile(Write_platform_d_ToDestFile, "mac");
		}
	} else {
		WriteScriptVarToDestFile("my_rz_src_d");
	}
}

static void Write_config_d_ToDestFile(void)
{
	if (! CurUseAbsolute) {
		WriteSubDirToDestFile(Write_output_d_ToDestFile, "config");
	} else {
		WriteScriptVarToDestFile("my_config_d");
	}
}

static void Write_obj_d_ToDestFile(void)
{
	if (! CurUseAbsolute) {
		WriteSubDirToDestFile(Write_output_d_ToDestFile, "obj");
	} else {
		WriteScriptVarToDestFile("my_obj_d");
	}
}

static unsigned int FileCounter;

static void DoSrcFileIncrFileCounter(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(s);
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	++FileCounter;
}

static void WriteFileToCFilesList(MyProc p, char *s, char *FileExt)
{
	WriteBgnDestFileLn();
	WriteFileInDirToDestFile(p, s, FileExt);
	WriteEndDestFileLn();
}

static void DoConfigHeaderFile(char *s)
{
	WriteFileToCFilesList(Write_config_d_ToDestFile, s, "");
}

static void DoSrcExtraHeaderFile(char *s)
{
	WriteFileToCFilesList(Write_c_src_d_ToDestFile, s, "");
}

static void DoSrcDependsAddToList(char *s)
{
	WriteFileToCFilesList(Write_c_src_d_ToDestFile, s, "");
}

static void DoSrcFileAddToList(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	WriteFileToCFilesList(Write_c_src_d_ToDestFile, s, ".h");
	WriteFileToCFilesList(Write_c_src_d_ToDestFile, s, ".c");

	if (depends != nullpr) {
		depends(DoSrcDependsAddToList);
	}
}

static void WriteCFilesList(void)
{
	WriteOpenDestFile("my_output_d", "c_files", "", "list of c files");

	DoConfigHeaderFile("CNFGGLOB.h");
	DoConfigHeaderFile("CNFGRAPI.h");

	DoAllExtraHeaders(DoSrcExtraHeaderFile);

	DoAllSrcFiles(DoSrcFileAddToList);
	WriteCloseDestFile();
}

static void WriteMachoAppNameStr(void)
{
	WriteCStrToOutput(kStrAppAbbrev);
	WriteCStrToOutput(".app");
}

static void WriteWinAppNameStr(void)
{
	WriteCStrToOutput(kStrAppAbbrev);
	WriteCStrToOutput(".exe");
}

static void WriteCommonCOptions(blnr fast)
{
	if (CurIde == kIdeMPW3_6_a1) {
		if (TheCPUFam == kCPUFam68K) {
			WriteCStrToOutput(" -proto strict -w 17 -align mac68k -b");
			if (CurTarget == kTargMac68KFPU) {
				WriteCStrToOutput(" -mc68020 -mc68881 -elems881");
			}
			if (CurDbgLvl == kDbgLvlShip) {
				WriteCStrToOutput(" -mbg off");
			}
			WriteCStrToOutput(" -model farCode");
		} else if (TheCPUFam == kCPUFamPPC) {
			WriteCStrToOutput(" -proto strict -w 17");
			if (CurDbgLvl != kDbgLvlShip) {
				WriteCStrToOutput(" -traceback");
			}
		}
		if (CurDbgLvl != kDbgLvlQuick) {
			if (fast) {
				if (TheCPUFam == kCPUFam68K) {
					WriteCStrToOutput(" -opt speed");
				} else if (TheCPUFam == kCPUFamPPC) {
					WriteCStrToOutput(" -opt speed");
				}
			} else {
				if (TheCPUFam == kCPUFam68K) {
					WriteCStrToOutput(" -opt space");
				} else if (TheCPUFam == kCPUFamPPC) {
					WriteCStrToOutput(" -opt size");
					/* this may not be reliable? */
				}
			}
		}
	} else if ((CurIde == kIdeBashGcc) || (CurIde == kIdeRH7)) {
		WriteCStrToOutput(" -c -Wall -Wmissing-prototypes -Wstrict-prototypes -Wno-uninitialized");
		if (CurIde == kIdeBashGcc) {
			WriteCStrToOutput(" -mdynamic-no-pic -fpascal-strings");
		}
		if (CurDbgLvl != kDbgLvlQuick) {
			if (fast) {
				WriteCStrToOutput(" -O3");
			} else {
				WriteCStrToOutput(" -Os");
			}
		} else {
			WriteCStrToOutput(" -g -O0");
		}
	}
	if (CurIde == kIdeMPW3_6_a1) {
		WriteCStrToOutput(" -i ");
		WriteQuoteToDestFile();
		WriteCStrToOutput("{mk_config_d}");
		WriteQuoteToDestFile();
	} else if ((CurIde == kIdeBashGcc) || (CurIde == kIdeRH7)) {
		WriteCStrToOutput(" -I$(mk_config_d)");
	}
	if (CurTarget == kTargMacX11) {
		WriteCStrToOutput(" -I/usr/X11R6/include/");
	}
}

static void WriteSrcSuffix(blnr AsmAvail)
{
	if (AsmAvail && HaveAsm) {
		WriteCStrToOutput(".s");
	} else {
		WriteCStrToOutput(".c");
	}
}

#if UsePragmaSegment
#pragma segment Seg6
#endif

static void DoSrcDependsMPWMakeCompile(char *s)
{
	WriteCStrToOutput(" ");
	WriteQuoteToDestFile();
	WriteCStrToOutput("{mk_c_src_d}");
	WriteCStrToOutput(s);
	WriteQuoteToDestFile();
}

static void DoSrcFileMPWMakeCompile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	WriteBgnDestFileLn();
	WriteQuoteToDestFile();
	WriteCStrToOutput("{mk_obj_d}");
	WriteCStrToOutput(s);
	WriteCStrToOutput(".o");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" \304 ");
	WriteQuoteToDestFile();
	if (AsmAvail && HaveAsm) {
		WriteCStrToOutput("{mk_a_src_d}");
	} else {
		WriteCStrToOutput("{mk_c_src_d}");
	}
	WriteCStrToOutput(s);
	WriteSrcSuffix(AsmAvail);
	WriteQuoteToDestFile();
	if (depends != nullpr) {
		depends(DoSrcDependsMPWMakeCompile);
	}
	WriteEndDestFileLn();

	++DestFileIndent;
		WriteBgnDestFileLn();
		if (AsmAvail && HaveAsm) {
			WriteCStrToOutput("{mk_CallA} ");
		} else {
			WriteCStrToOutput("{mk_CallC} ");
		}
		WriteQuoteToDestFile();
		if (AsmAvail && HaveAsm) {
			WriteCStrToOutput("{mk_a_src_d}");
		} else {
			WriteCStrToOutput("{mk_c_src_d}");
		}
		WriteCStrToOutput(s);
		WriteSrcSuffix(AsmAvail);
		WriteQuoteToDestFile();
		WriteCStrToOutput(" -o ");
		WriteQuoteToDestFile();
		WriteCStrToOutput("{mk_obj_d}");
		WriteCStrToOutput(s);
		WriteCStrToOutput(".o");
		WriteQuoteToDestFile();
		if (! (AsmAvail && HaveAsm)) {
			WriteCStrToOutput(" {mk_COptions}");
		}
		WriteEndDestFileLn();
	--DestFileIndent;
}

static void DoSrcFileMPWMakeObjects(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBgnDestFileLn();
	WriteQuoteToDestFile();
	WriteCStrToOutput("{mk_obj_d}");
	WriteCStrToOutput(s);
	WriteCStrToOutput(".o");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" \266");
	WriteEndDestFileLn();
}

static void WriteMPWSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", "Makefile", "", "Make file");

	WriteBgnDestFileLn();
	WriteCStrToOutput("# make file generated by ");
	WriteQuoteToDestFile();
	WriteCStrToOutput("setup.mpw");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" script");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_output_d = ");
	Write_output_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_source_d = ");
	Write_source_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_config_d = {mk_output_d}config:");
	WriteDestFileLn("mk_obj_d = {mk_output_d}obj:");

	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_built_program = {mk_output_d}");
	WriteCStrToOutput(kStrAppAbbrev);
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_c_src_d = {mk_source_d}c_src:");
	if (HaveAsm) {
		WriteDestFileLn("mk_a_src_d = {mk_source_d}a_src:ppc:ppcasm:");
	}
	WriteDestFileLn("mk_rz_src_d = {mk_source_d}platform:mac:");
	WriteBlankLineToDestFile();
	if (TheCPUFam == kCPUFam68K) {
		WriteDestFileLn("mk_CallC = SC");
	} else if (TheCPUFam == kCPUFamPPC) {
		WriteDestFileLn("mk_CallC = MrC");
	}

	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_COptions =");
	WriteCommonCOptions(falseblnr);
	WriteEndDestFileLn();

	if (HaveAsm) {
		WriteDestFileLn("mk_CallA = PPCAsm");
	}

	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteDestFileLn("LibFiles = \266");
	++DestFileIndent;
		if (CurTarget == kTargCarbon) {
			WriteDestFileLn("\"{SharedLibraries}CarbonLib\" \266");
			WriteDestFileLn("\"{PPCLibraries}PPCToolLibs.o\" \266");
			WriteDestFileLn("\"{PPCLibraries}PPCCRuntime.o\" \266");
			WriteDestFileLn("\"{SharedLibraries}StdCLib\" \266");
		} else if (CurTarget == kTargClassicPPC) {
			WriteDestFileLn("\"{PPCLibraries}PPCToolLibs.o\" \266");
			WriteDestFileLn("\"{PPCLibraries}PPCCRuntime.o\" \266");
			WriteDestFileLn("\"{PPCLibraries}StdCRuntime.o\" \266");
			WriteDestFileLn("\"{SharedLibraries}InterfaceLib\" \266");
			WriteDestFileLn("\"{SharedLibraries}MathLib\" \266");
			WriteDestFileLn("\"{SharedLibraries}StdCLib\" \266");
			WriteDestFileLn("\"{SharedLibraries}AppearanceLib\" \266");
			WriteDestFileLn("\"{SharedLibraries}MenusLib\" \266");
			WriteDestFileLn("\"{SharedLibraries}NavigationLib\" \266");
			WriteDestFileLn("\"{SharedLibraries}DragLib\" \266");
			WriteDestFileLn("\"{SharedLibraries}WindowsLib\" \266");
		} else if (CurTarget == kTargMac68K) {
			WriteDestFileLn("\"{Libraries}Interface.o\" \266");
			WriteDestFileLn("\"{Libraries}MacRuntime.o\" \266");
			/* WriteDestFileLn("\"{Libraries}MathLib.o\" \266"); */
			WriteDestFileLn("\"{Libraries}Navigation.o\" \266");
		} else if (CurTarget == kTargMac68KFPU) {
			WriteDestFileLn("\"{Libraries}Interface.o\" \266");
			WriteDestFileLn("\"{Libraries}MacRuntime.o\" \266");
			/* WriteDestFileLn("\"{Libraries}MathLib881.o\" \266"); */
			WriteDestFileLn("\"{Libraries}Navigation.o\" \266");
		}
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteDestFileLn("TheApplication \304 \"{mk_built_program}\"");
	WriteBlankLineToDestFile();
	DoAllSrcFiles(DoSrcFileMPWMakeCompile);
	WriteBlankLineToDestFile();
	WriteDestFileLn("ObjFiles = \266");
	++DestFileIndent;
		DoAllSrcFiles(DoSrcFileMPWMakeObjects);
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteDestFileLn("\"{mk_obj_d}main.rsrc\" \304 \"{mk_rz_src_d}main.r\"");
	++DestFileIndent;
		WriteDestFileLn("Rez -t rsrc -c RSED -i \"{RIncludes}\" -i \"{mk_rz_src_d}\" \"{mk_rz_src_d}main.r\" -o \"{mk_obj_d}main.rsrc\" -i \"{mk_config_d}\"");
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteDestFileLn("\"{mk_built_program}\" \304 {ObjFiles} \"{mk_obj_d}main.rsrc\"");
	++DestFileIndent;
		WriteDestFileLn("Duplicate -y \"{mk_obj_d}main.rsrc\" \"{mk_built_program}\"");

		WriteBgnDestFileLn();
		if (TheCPUFam == kCPUFam68K) {
			WriteCStrToOutput("Link");
			if (CurDbgLvl == kDbgLvlShip) {
				WriteCStrToOutput(" -rn");
			}
			WriteCStrToOutput(" -model far -srtsg all -sg Main=STDCLIB,SANELIB,CSANELib,SADEV,STDIO");
		} else if (TheCPUFam == kCPUFamPPC) {
			WriteCStrToOutput("PPCLink");
		}
		if (CurTarget == kTargCarbon) {
			WriteCStrToOutput(" -m main");
		}
		WriteCStrToOutput(" -t APPL -c ");
		WriteCStrToOutput(kMacCreatorSig);
		WriteCStrToOutput(" \266");
		WriteEndDestFileLn();

		++DestFileIndent;
			WriteDestFileLn("-o {mk_built_program} \266");
			WriteDestFileLn("{LibFiles} \266");
			WriteDestFileLn("{ObjFiles}");
		--DestFileIndent;

		WriteDestFileLn("SetFile -d . -m . -a B \"{mk_built_program}\"");
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteCloseDestFile();

}

#if UsePragmaSegment
#pragma segment Seg7
#endif

static void WriteXMLtagSettingNameProcVal(char *n, MyProc v)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin("SETTING");
	WriteXMLtagBegin("NAME");
	WriteCStrToOutput(n);
	WriteXMLtagEnd("NAME");
	WriteXMLtagBegin("VALUE");
	v();
	WriteXMLtagEnd("VALUE");
	WriteXMLtagEnd("SETTING");
	WriteEndDestFileLn();
}

static void WriteXMLtagSettingNameVal(char *n, char *v)
{
	char *SavepDt = pDt;

	pDt = v;
	WriteXMLtagSettingNameProcVal(n, WritepDtString);
	pDt = SavepDt;
}

static void WriteBeginNamedSettingXMLtagLine(char *s)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin("SETTING");
	WriteXMLtagBegin("NAME");
	WriteCStrToOutput(s);
	WriteXMLtagEnd("NAME");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteAllMWTargetName(void)
{
	WriteXMLtagBeginProcValEndLine("TARGETNAME", WriteAppVariationStr);
}

typedef void (*tWriteMWLib)(char *s);

static void WriteMWLibs(tWriteMWLib p)
{
	if (CurTarget == kTargWinx86) {
		p("comdlg32.lib");
		p("gdi32.lib");
		p("kernel32.lib");
		p("user32.lib");
		p("shell32.lib");
		p("winmm.lib");
		p("MSL_All_x86.lib");
	} else if (CurTarget == kTargMacho) {
		p("crt1.o");
		p("MSL_All_Mach-O_D.lib");
	} else {
		if (CurTarget == kTargCarbon) {
			p("CarbonLib");
		} else {
			p("InterfaceLib");
			p("MathLib");
			p("AppearanceLib");
			p("MenusLib");
			p("NavigationLib");
			p("DragLib");
			p("WindowsLib");
		}
		p("MSL_Runtime_PPC.Lib");
	}
}

static void WriteMWLibAddFile(char *s)
{
	WriteBeginXMLtagLine("FILE");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginValEndLine("PATH", s);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
		if (CurTarget == kTargWinx86) {
			WriteXMLtagBeginValEndLine("FILEKIND", "Unknown");
		} else {
			WriteXMLtagBeginValEndLine("FILEKIND", "Library");
		}
		WriteXMLtagBeginValEndLine("FILEFLAGS", "");
	WriteEndXMLtagLine("FILE");
}

static void WriteMWLibMakeObjects(char *s)
{
	WriteBeginXMLtagLine("FILEREF");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginValEndLine("PATH", s);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}

static void WriteMWLibGroupList(char *s)
{
	WriteBeginXMLtagLine("FILEREF");
		WriteAllMWTargetName();
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginValEndLine("PATH", s);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}

static void WriteMWProjRelSearchPath(MyProc p)
{
	WriteBeginXMLtagLine("SETTING");
		WriteBeginNamedSettingXMLtagLine("SearchPath");
			WriteBgnDestFileLn();
			WriteXMLtagBegin("SETTING");
			WriteXMLtagBegin("NAME");
			WriteCStrToOutput("Path");
			WriteXMLtagEnd("NAME");
			WriteXMLtagBegin("VALUE");
			p();
			WriteXMLtagEnd("VALUE");
			WriteXMLtagEnd("SETTING");
			WriteEndDestFileLn();

			WriteXMLtagSettingNameVal("PathFormat", "MacOS");
			if (! CurUseAbsolute) {
				WriteXMLtagSettingNameVal("PathRoot", "Project");
			} else {
				WriteXMLtagSettingNameVal("PathRoot", "Absolute");
			}
		WriteEndXMLtagLine("SETTING");
		WriteXMLtagSettingNameVal("Recursive", "true");
		WriteXMLtagSettingNameVal("FrameworkPath", "false");
		WriteXMLtagSettingNameVal("HostFlags", "All");
	WriteEndXMLtagLine("SETTING");
}

static void WriteMWDrvRelSearchPath(char *s)
{
	WriteBeginXMLtagLine("SETTING");
		WriteBeginNamedSettingXMLtagLine("SearchPath");
			WriteXMLtagSettingNameVal("Path", s);
			WriteXMLtagSettingNameVal("PathFormat", "MacOS");
			WriteXMLtagSettingNameVal("PathRoot", "Project");
		WriteEndXMLtagLine("SETTING");
		WriteXMLtagSettingNameVal("Recursive", "true");
		WriteXMLtagSettingNameVal("FrameworkPath", "false");
		WriteXMLtagSettingNameVal("HostFlags", "All");
	WriteEndXMLtagLine("SETTING");
}

static void WriteBeginXMLMWGroup(char *s)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin("GROUP");
	WriteXMLtagBegin("NAME");
	WriteCStrToOutput(s);
	WriteXMLtagEnd("NAME");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteMWSettingsPanelComment(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput("<!-- Settings for ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(s);
	WriteQuoteToDestFile();
	WriteCStrToOutput(" panel -->");
	WriteEndDestFileLn();
}

static void WriteMWSource_icnsAddFile(char *s)
{
	char *SavepDt = pDt;

	pDt = s;
	WriteBeginXMLtagLine("FILE");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginProcValEndLine("PATH", WritepDIcontMachoFile);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
		WriteXMLtagBeginValEndLine("FILEKIND", "Unknown");
		WriteXMLtagBeginValEndLine("FILEFLAGS", "");
	WriteEndXMLtagLine("FILE");
	pDt = SavepDt;
}

static void WriteMWSource_icnsMakeObjects(char *s)
{
	char *SavepDt = pDt;

	pDt = s;
	WriteBeginXMLtagLine("FILEREF");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginProcValEndLine("PATH", WritepDIcontMachoFile);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
	pDt = SavepDt;
}

static void WriteMWSource_icnsGroupList(char *s)
{
	char *SavepDt = pDt;

	pDt = s;
	WriteBeginXMLtagLine("FILEREF");
		WriteAllMWTargetName();
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginProcValEndLine("PATH", WritepDIcontMachoFile);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
	pDt = SavepDt;
}

static void WriteDocTypeMWAddFile(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	WriteMWSource_icnsAddFile(ShortName);
}

static void WriteDocTypeMWMakeObjects(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	WriteMWSource_icnsMakeObjects(ShortName);
}

static void WriteDocTypeMWGroupList(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	WriteMWSource_icnsGroupList(ShortName);
}

static void DoSrcFileMW8AddFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBeginXMLtagLine("FILE");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");

		WriteBgnDestFileLn();
		WriteXMLtagBegin("PATH");
		WriteCStrToOutput(s);
		WriteSrcSuffix(AsmAvail);
		WriteXMLtagEnd("PATH");
		WriteEndDestFileLn();

		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
		WriteXMLtagBeginValEndLine("FILEKIND", "Text");
		if (CurDbgLvl == kDbgLvlQuick) {
			WriteXMLtagBeginValEndLine("FILEFLAGS", "Debug");
		} else {
			WriteXMLtagBeginValEndLine("FILEFLAGS", "");
		}
	WriteEndXMLtagLine("FILE");
}

static void DoSrcFileMW8sMakeObjects(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBeginXMLtagLine("FILEREF");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");

		WriteBgnDestFileLn();
		WriteXMLtagBegin("PATH");
		WriteCStrToOutput(s);
		WriteSrcSuffix(AsmAvail);
		WriteXMLtagEnd("PATH");
		WriteEndDestFileLn();

		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}

static void DoSrcFileMW8GroupList(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBeginXMLtagLine("FILEREF");
		WriteAllMWTargetName();
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");

		WriteBgnDestFileLn();
		WriteXMLtagBegin("PATH");
		WriteCStrToOutput(s);
		WriteSrcSuffix(AsmAvail);
		WriteXMLtagEnd("PATH");
		WriteEndDestFileLn();

		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}

static void WriteMetrowerksSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", kStrAppAbbrev, ".mcp.xml", "Project Import");

	WriteBgnDestFileLn();
	WriteCStrToOutput("<?xml version=");
	WriteQuoteToDestFile();
	WriteCStrToOutput("1.0");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" encoding=");
	WriteQuoteToDestFile();
	WriteCStrToOutput("UTF-8");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" standalone=");
	WriteQuoteToDestFile();
	WriteCStrToOutput("yes");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" ?>");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("<?codewarrior exportversion=");
	WriteQuoteToDestFile();
	WriteCStrToOutput("1.0.1");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" ideversion=");
	WriteQuoteToDestFile();
	WriteCStrToOutput("5.0");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" ?>");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteBeginXMLtagLine("PROJECT");
	WriteBeginXMLtagLine("TARGETLIST");
	WriteBeginXMLtagLine("TARGET");
		WriteXMLtagBeginProcValEndLine("NAME", WriteAppVariationStr);
		WriteBeginXMLtagLine("SETTINGLIST");
			WriteMWSettingsPanelComment("Access Paths");
			if (CurTarget == kTargMacho) {
				WriteXMLtagSettingNameVal("InterpretDOSAndUnixPaths", "true");
				WriteXMLtagSettingNameVal("RequireFrameworkStyleIncludes", "true");
			}
			WriteBeginNamedSettingXMLtagLine("UserSearchPaths");
				WriteMWDrvRelSearchPath(":config:");
				WriteMWProjRelSearchPath(Write_c_src_d_ToDestFile);
				if (HaveAsm) {
					WriteMWProjRelSearchPath(Write_a_src_d_ToDestFile);
				}
				WriteMWProjRelSearchPath(Write_rz_src_ToDestFile);
				if (CurTarget == kTargMacho) {
					/* seems to be wanted by propertly list compiler */
					WriteMWDrvRelSearchPath(":");
				}
			WriteEndXMLtagLine("SETTING");
			WriteBeginNamedSettingXMLtagLine("SystemSearchPaths");
				if (CurTarget == kTargWinx86) {
					WriteBeginXMLtagLine("SETTING");
						WriteBeginNamedSettingXMLtagLine("SearchPath");
							WriteXMLtagSettingNameVal("Path", ":MSL:");
							WriteXMLtagSettingNameVal("PathFormat", "MacOS");
							WriteXMLtagSettingNameVal("PathRoot", "CodeWarrior");
						WriteEndXMLtagLine("SETTING");
						WriteXMLtagSettingNameVal("Recursive", "true");
						WriteXMLtagSettingNameVal("FrameworkPath", "false");
						WriteXMLtagSettingNameVal("HostFlags", "All");
					WriteEndXMLtagLine("SETTING");

					WriteBeginXMLtagLine("SETTING");
						WriteBeginNamedSettingXMLtagLine("SearchPath");
							WriteXMLtagSettingNameVal("Path", ":Win32-x86 Support:");
							WriteXMLtagSettingNameVal("PathFormat", "MacOS");
							WriteXMLtagSettingNameVal("PathRoot", "CodeWarrior");
						WriteEndXMLtagLine("SETTING");
						WriteXMLtagSettingNameVal("Recursive", "true");
						WriteXMLtagSettingNameVal("FrameworkPath", "false");
						WriteXMLtagSettingNameVal("HostFlags", "All");
					WriteEndXMLtagLine("SETTING");
				} else {
					WriteBeginXMLtagLine("SETTING");
						WriteBeginNamedSettingXMLtagLine("SearchPath");
							WriteXMLtagSettingNameVal("Path", ":MSL:MSL_C:");
							WriteXMLtagSettingNameVal("PathFormat", "MacOS");
							WriteXMLtagSettingNameVal("PathRoot", "CodeWarrior");
						WriteEndXMLtagLine("SETTING");
						WriteXMLtagSettingNameVal("Recursive", "true");
						WriteXMLtagSettingNameVal("FrameworkPath", "false");
						WriteXMLtagSettingNameVal("HostFlags", "All");
					WriteEndXMLtagLine("SETTING");

					if (CurTarget == kTargMacho) {
						WriteBeginXMLtagLine("SETTING");
							WriteBeginNamedSettingXMLtagLine("SearchPath");
								WriteXMLtagSettingNameVal("Path", ":MacOS X Support:");
								WriteXMLtagSettingNameVal("PathFormat", "MacOS");
								WriteXMLtagSettingNameVal("PathRoot", "CodeWarrior");
							WriteEndXMLtagLine("SETTING");
							WriteXMLtagSettingNameVal("Recursive", "true");
							WriteXMLtagSettingNameVal("FrameworkPath", "false");
							WriteXMLtagSettingNameVal("HostFlags", "All");
						WriteEndXMLtagLine("SETTING");

						WriteBeginXMLtagLine("SETTING");
							WriteBeginNamedSettingXMLtagLine("SearchPath");
								WriteXMLtagSettingNameVal("Path", "/usr/include");
								WriteXMLtagSettingNameVal("PathFormat", "Unix");
								WriteXMLtagSettingNameVal("PathRoot", "Absolute");
							WriteEndXMLtagLine("SETTING");
							WriteXMLtagSettingNameVal("Recursive", "false");
							WriteXMLtagSettingNameVal("FrameworkPath", "false");
							WriteXMLtagSettingNameVal("HostFlags", "All");
						WriteEndXMLtagLine("SETTING");

						WriteBeginXMLtagLine("SETTING");
							WriteBeginNamedSettingXMLtagLine("SearchPath");
								WriteXMLtagSettingNameVal("Path", "/usr/lib");
								WriteXMLtagSettingNameVal("PathFormat", "Unix");
								WriteXMLtagSettingNameVal("PathRoot", "Absolute");
							WriteEndXMLtagLine("SETTING");
							WriteXMLtagSettingNameVal("Recursive", "false");
							WriteXMLtagSettingNameVal("FrameworkPath", "false");
							WriteXMLtagSettingNameVal("HostFlags", "All");
						WriteEndXMLtagLine("SETTING");

						WriteBeginXMLtagLine("SETTING");
							WriteBeginNamedSettingXMLtagLine("SearchPath");
								WriteXMLtagSettingNameVal("Path", "System/Library/Frameworks");
								WriteXMLtagSettingNameVal("PathFormat", "Unix");
								WriteXMLtagSettingNameVal("PathRoot", "OS X Volume");
							WriteEndXMLtagLine("SETTING");
							WriteXMLtagSettingNameVal("Recursive", "false");
							WriteXMLtagSettingNameVal("FrameworkPath", "true");
							WriteXMLtagSettingNameVal("HostFlags", "All");
						WriteEndXMLtagLine("SETTING");
					} else {
						WriteBeginXMLtagLine("SETTING");
							WriteBeginNamedSettingXMLtagLine("SearchPath");
								WriteXMLtagSettingNameVal("Path", ":MacOS Support:");
								WriteXMLtagSettingNameVal("PathFormat", "MacOS");
								WriteXMLtagSettingNameVal("PathRoot", "CodeWarrior");
							WriteEndXMLtagLine("SETTING");
							WriteXMLtagSettingNameVal("Recursive", "true");
							WriteXMLtagSettingNameVal("FrameworkPath", "false");
							WriteXMLtagSettingNameVal("HostFlags", "All");
						WriteEndXMLtagLine("SETTING");
					}
				}

			WriteEndXMLtagLine("SETTING");
			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("Target Settings");
			if (CurTarget == kTargWinx86) {
				WriteXMLtagSettingNameVal("Linker", "Win32 x86 Linker");
			} else if (CurTarget == kTargMacho) {
				WriteXMLtagSettingNameVal("Linker", "MacOS X PPC Linker");
			} else {
				WriteXMLtagSettingNameVal("Linker", "MacOS PPC Linker");
			}
			WriteXMLtagSettingNameProcVal("Targetname", WriteAppVariationStr);

			WriteBeginNamedSettingXMLtagLine("OutputDirectory");
				WriteXMLtagSettingNameVal("Path", ":");
				WriteXMLtagSettingNameVal("PathFormat", "MacOS");
				WriteXMLtagSettingNameVal("PathRoot", "Project");
			WriteEndXMLtagLine("SETTING");

			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("Build Extras");
			WriteXMLtagSettingNameVal("BrowserGenerator", "0");

			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("Debugger Target");
			WriteXMLtagSettingNameVal("StopAtTempBPOnLaunch", "false");

			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("C/C++ Compiler");
			WriteXMLtagSettingNameVal("MWFrontEnd_C_checkprotos", "1");
			WriteXMLtagSettingNameVal("MWFrontEnd_C_enableexceptions", "0");
			if (CurDbgLvl == kDbgLvlQuick) {
				WriteXMLtagSettingNameVal("MWFrontEnd_C_dontinline", "1");
			} else {
				WriteXMLtagSettingNameVal("MWFrontEnd_C_dontinline", "0");
			}
			WriteXMLtagSettingNameVal("MWFrontEnd_C_useRTTI", "0");
			if (CurDbgLvl == kDbgLvlQuick) {
				WriteXMLtagSettingNameVal("MWFrontEnd_C_autoinline", "0");
			} else {
				WriteXMLtagSettingNameVal("MWFrontEnd_C_autoinline", "1");
			}

			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("C/C++ Warnings");
			WriteXMLtagSettingNameVal("MWWarning_C_warn_illpragma", "1");
			WriteXMLtagSettingNameVal("MWWarning_C_warn_emptydecl", "1");
			WriteXMLtagSettingNameVal("MWWarning_C_warn_possunwant", "1");
			WriteXMLtagSettingNameVal("MWWarning_C_warn_unusedvar", "1");
			WriteXMLtagSettingNameVal("MWWarning_C_warn_unusedarg", "1");
			WriteXMLtagSettingNameVal("MWWarning_C_warn_extracomma", "1");
			WriteXMLtagSettingNameVal("MWWarning_C_pedantic", "1");

			WriteBlankLineToDestFile();
			if (CurTarget == kTargWinx86) {
				WriteMWSettingsPanelComment("x86 CodeGen");
				if (CurDbgLvl == kDbgLvlQuick) {
					WriteXMLtagSettingNameVal("MWCodeGen_X86_intrinsics", "0");
				} else {
					WriteXMLtagSettingNameVal("MWCodeGen_X86_intrinsics", "1");
				}

			} else if (CurTarget == kTargMacho) {
				WriteMWSettingsPanelComment("PPC CodeGen Mach-O");
				WriteXMLtagSettingNameVal("MWCodeGen_MachO_structalignment", "PPC");
				if (CurDbgLvl == kDbgLvlQuick) {
					WriteXMLtagSettingNameVal("MWCodeGen_MachO_peephole", "0");
					WriteXMLtagSettingNameVal("MWCodeGen_MachO_schedule", "0");
				} else {
					WriteXMLtagSettingNameVal("MWCodeGen_MachO_peephole", "1");
					WriteXMLtagSettingNameVal("MWCodeGen_MachO_schedule", "1");
				}
			} else {
				WriteMWSettingsPanelComment("PPC CodeGen");
				if (CurDbgLvl != kDbgLvlShip) {
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_tracebacktables", "Inline");
				} else {
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_tracebacktables", "None");
				}
				WriteXMLtagSettingNameVal("MWCodeGen_PPC_vectortocdata", "0");
				if (CurDbgLvl == kDbgLvlQuick) {
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_peephole", "0");
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_schedule", "0");
				} else {
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_peephole", "1");
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_schedule", "1");
				}
			}

			WriteBlankLineToDestFile();
			if (CurTarget == kTargWinx86) {
				WriteMWSettingsPanelComment("x86 Global Optimizer");
				if (CurDbgLvl == kDbgLvlQuick) {
					WriteXMLtagSettingNameVal("GlobalOptimizer_X86_optimizationlevel", "Level0");
				} else {
					WriteXMLtagSettingNameVal("GlobalOptimizer_X86_optimizationlevel", "Level4");
				}
				WriteXMLtagSettingNameVal("GlobalOptimizer_X86_optfor", "Size");

				/* work around what is probably bug in windows version of mw8 */
				if (CurDbgLvl == kDbgLvlQuick) {
					WriteXMLtagSettingNameVal("GlobalOptimizer_X86__optimizationlevel", "Level0");
				} else {
					WriteXMLtagSettingNameVal("GlobalOptimizer_X86__optimizationlevel", "Level4");
				}
				WriteXMLtagSettingNameVal("GlobalOptimizer_X86__optfor", "Size");
			} else {
				WriteMWSettingsPanelComment("PPC Global Optimizer");
				if (CurDbgLvl == kDbgLvlQuick) {
					WriteXMLtagSettingNameVal("GlobalOptimizer_PPC_optimizationlevel", "Level0");
				} else {
					WriteXMLtagSettingNameVal("GlobalOptimizer_PPC_optimizationlevel", "Level4");
				}
				WriteXMLtagSettingNameVal("GlobalOptimizer_PPC_optfor", "Size");
			}

			WriteBlankLineToDestFile();
			if (CurTarget == kTargWinx86) {
				WriteMWSettingsPanelComment("x86 Linker");
				WriteXMLtagSettingNameVal("MWLinker_X86_subsystem", "WinGUI");
				if (CurDbgLvl == kDbgLvlQuick) {
					WriteXMLtagSettingNameVal("MWLinker_X86_linkdebug", "true");
				} else {
					WriteXMLtagSettingNameVal("MWLinker_X86_linkdebug", "false");
				}
				WriteXMLtagSettingNameVal("MWLinker_X86_usedefaultlibs", "false");
			} else if (CurTarget == kTargMacho) {
				WriteMWSettingsPanelComment("PPC Mac OS X Linker");
				if (CurDbgLvl == kDbgLvlQuick) {
					WriteXMLtagSettingNameVal("MWLinker_MacOSX_linksym", "1");
				} else {
					WriteXMLtagSettingNameVal("MWLinker_MacOSX_linksym", "0");
				}
				WriteXMLtagSettingNameVal("MWLinker_MacOSX_symfullpath", "1");
				WriteXMLtagSettingNameVal("MWLinker_MacOSX_permitmultdefs", "0");
				if (CurDbgLvl == kDbgLvlShip) {
					WriteXMLtagSettingNameVal("MWLinker_MacOSX_strip_debug_symbols", "1");
				}
			} else {
				WriteMWSettingsPanelComment("PPC Linker");
				if (CurDbgLvl == kDbgLvlQuick) {
					WriteXMLtagSettingNameVal("MWLinker_PPC_linksym", "1");
				} else {
					WriteXMLtagSettingNameVal("MWLinker_PPC_linksym", "0");
				}
				WriteXMLtagSettingNameVal("MWLinker_PPC_linkmode", "Normal");
			}

			WriteBlankLineToDestFile();
			if (CurTarget == kTargWinx86) {
				WriteMWSettingsPanelComment("x86 Project");
				WriteXMLtagSettingNameProcVal("MWProject_X86_outfile", WriteWinAppNameStr);
			} else if (CurTarget == kTargMacho) {
				WriteMWSettingsPanelComment("PPC Mac OS X Project");
				WriteXMLtagSettingNameVal("MWProject_MacOSX_type", "ApplicationPackage");
				WriteXMLtagSettingNameProcVal("MWProject_MacOSX_outfile", WriteMachoAppNameStr);
				WriteXMLtagSettingNameVal("MWProject_MacOSX_filecreator", kMacCreatorSig);
				WriteXMLtagSettingNameVal("MWProject_MacOSX_filetype", "APPL");
				WriteXMLtagSettingNameVal("MWProject_MacOSX_vmaddress", "0");
				WriteXMLtagSettingNameVal("MWProject_MacOSX_flatrsrc", "1");
				WriteXMLtagSettingNameVal("MWProject_MacOSX_flatrsrcfilename", "");
				WriteBeginNamedSettingXMLtagLine("MWProject_MacOSX_flatrsrcoutputdir");
					WriteXMLtagSettingNameVal("Path", ":");
					WriteXMLtagSettingNameVal("PathFormat", "MacOS");
					WriteXMLtagSettingNameVal("PathRoot", "Project");
				WriteEndXMLtagLine("SETTING");
			} else {
				WriteMWSettingsPanelComment("PPC Project");
				WriteXMLtagSettingNameVal("MWProject_PPC_outfile", kStrAppAbbrev);
				WriteXMLtagSettingNameVal("MWProject_PPC_filecreator", kMacCreatorSig);
				WriteXMLtagSettingNameVal("MWProject_PPC_size", "6000");
				WriteXMLtagSettingNameVal("MWProject_PPC_minsize", "3000");
				WriteXMLtagSettingNameVal("MWProject_PPC_flags", "22768");
			}
		WriteEndXMLtagLine("SETTINGLIST");

		WriteBeginXMLtagLine("FILELIST");
			WriteMWLibs(WriteMWLibAddFile);
			if (CurTarget == kTargWinx86) {
				WriteBeginXMLtagLine("FILE");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.RC");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteXMLtagBeginValEndLine("FILEKIND", "Text");
					WriteXMLtagBeginValEndLine("FILEFLAGS", "");
				WriteEndXMLtagLine("FILE");
			} else if (CurTarget == kTargMacho) {
				WriteBeginXMLtagLine("FILE");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.plc");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteXMLtagBeginValEndLine("FILEKIND", "Text");
					WriteXMLtagBeginValEndLine("FILEFLAGS", "Debug");
				WriteEndXMLtagLine("FILE");
				WriteMWSource_icnsAddFile("App");
				DoAllDocTypes(WriteDocTypeMWAddFile);
			} else {
				WriteBeginXMLtagLine("FILE");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.r");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteXMLtagBeginValEndLine("FILEKIND", "Text");
					WriteXMLtagBeginValEndLine("FILEFLAGS", "");
				WriteEndXMLtagLine("FILE");
			}
			DoAllSrcFiles(DoSrcFileMW8AddFile);
		WriteEndXMLtagLine("FILELIST");

		WriteBeginXMLtagLine("LINKORDER");
			WriteMWLibs(WriteMWLibMakeObjects);
			if (CurTarget == kTargWinx86) {
				WriteBeginXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.RC");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
				WriteEndXMLtagLine("FILEREF");
			} else if (CurTarget == kTargMacho) {
				WriteBeginXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.plc");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
				WriteEndXMLtagLine("FILEREF");
				WriteMWSource_icnsMakeObjects("App");
				DoAllDocTypes(WriteDocTypeMWMakeObjects);
			} else {
				WriteBeginXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.r");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
				WriteEndXMLtagLine("FILEREF");
			}
			DoAllSrcFiles(DoSrcFileMW8sMakeObjects);
		WriteEndXMLtagLine("LINKORDER");

		if (CurTarget == kTargMacho) {
			WriteBeginXMLtagLine("FRAMEWORKLIST");
				WriteBeginXMLtagLine("FRAMEWORK");
					WriteBeginXMLtagLine("FILEREF");
						WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
						WriteXMLtagBeginValEndLine("PATH", "Carbon.framework");
						WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteEndXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("DYNAMICLIBRARY", "Carbon");
				WriteEndXMLtagLine("FRAMEWORK");

				WriteBeginXMLtagLine("FRAMEWORK");
					WriteBeginXMLtagLine("FILEREF");
						WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
						WriteXMLtagBeginValEndLine("PATH", "System.framework");
						WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteEndXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("DYNAMICLIBRARY", "System");
				WriteEndXMLtagLine("FRAMEWORK");
			WriteEndXMLtagLine("FRAMEWORKLIST");
		}
	WriteEndXMLtagLine("TARGET");
	WriteEndXMLtagLine("TARGETLIST");

	WriteBlankLineToDestFile();
	WriteBeginXMLtagLine("TARGETORDER");
		WriteBgnDestFileLn();
		WriteXMLtagBegin("ORDEREDTARGET");
		WriteXMLtagBegin("NAME");
		WriteAppVariationStr();
		WriteXMLtagEnd("NAME");
		WriteXMLtagEnd("ORDEREDTARGET");
		WriteEndDestFileLn();
	WriteEndXMLtagLine("TARGETORDER");

	WriteBlankLineToDestFile();
	WriteBeginXMLtagLine("GROUPLIST");
		WriteBeginXMLMWGroup("Libraries");
			WriteMWLibs(WriteMWLibGroupList);
		WriteEndXMLtagLine("GROUP");
		WriteBeginXMLMWGroup("Resources");
			WriteBeginXMLtagLine("FILEREF");
				WriteAllMWTargetName();
				WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
				if (CurTarget == kTargWinx86) {
					WriteXMLtagBeginValEndLine("PATH", "main.RC");
				} else if (CurTarget == kTargMacho) {
					WriteXMLtagBeginValEndLine("PATH", "main.plc");
				} else {
					WriteXMLtagBeginValEndLine("PATH", "main.r");
				}
				WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
			WriteEndXMLtagLine("FILEREF");
			if (CurTarget == kTargMacho) {
				WriteMWSource_icnsGroupList("App");
				DoAllDocTypes(WriteDocTypeMWGroupList);
			}
		WriteEndXMLtagLine("GROUP");
		DoAllSrcFiles(DoSrcFileMW8GroupList);
	WriteEndXMLtagLine("GROUPLIST");

	WriteBlankLineToDestFile();
	WriteEndXMLtagLine("PROJECT");

	WriteCloseDestFile();
}

#if UsePragmaSegment
#pragma segment Seg8
#endif

static void DoSrcDependsBgcMakeCompile(char *s)
{
	WriteCStrToOutput(" $(mk_c_src_d)");
	WriteCStrToOutput(s);
}

static void WriteBgcmkAorCd(blnr AsmAvail)
{
	if (AsmAvail && HaveAsm) {
		WriteCStrToOutput("$(mk_a_src_d)");
	} else {
		WriteCStrToOutput("$(mk_c_src_d)");
	}
}

static void DoSrcFileBgcMakeCompile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput("$(mk_obj_d)");
	WriteCStrToOutput(s);
	WriteCStrToOutput(".o : ");
	WriteBgcmkAorCd(AsmAvail);
	WriteCStrToOutput(s);
	WriteSrcSuffix(AsmAvail);
	if (depends != nullpr) {
		depends(DoSrcDependsBgcMakeCompile);
	}
	WriteEndDestFileLn();

	++DestFileIndent;
		WriteBgnDestFileLn();
		if (AsmAvail && HaveAsm) {
			WriteCStrToOutput("$(mk_CallA) ");
		} else {
			WriteCStrToOutput("$(mk_CallC) ");
		}
		WriteQuoteToDestFile();
		WriteBgcmkAorCd(AsmAvail);
		WriteCStrToOutput(s);
		WriteSrcSuffix(AsmAvail);
		WriteQuoteToDestFile();
		WriteCStrToOutput(" -o ");
		WriteQuoteToDestFile();
		WriteCStrToOutput("$(mk_obj_d)");
		WriteCStrToOutput(s);
		WriteCStrToOutput(".o");
		WriteQuoteToDestFile();
		if (AsmAvail && HaveAsm) {
			WriteCStrToOutput(" $(mk_AOptions)");
		} else {
			WriteCStrToOutput(" $(mk_COptions)");
		}
		WriteEndDestFileLn();
	--DestFileIndent;
}

static void DoSrcFileBgcMakeObjects(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBgnDestFileLn();
	WriteCStrToOutput("$(mk_obj_d)");
	WriteCStrToOutput(s);
	WriteCStrToOutput(".o ");
	WriteBackSlashToDestFile();
	WriteEndDestFileLn();
}

static void WriteDocTypeCopyMachoFile(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	WriteBgnDestFileLn();
	WriteCStrToOutput("cp \"$(mk_osxrz_d)");
	WriteCStrToOutput(ShortName);
	WriteCStrToOutput("Icon.icns\" \"$(mk_tresources_d)\"");
	WriteEndDestFileLn();
}

static void WriteBashGccSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", "Makefile", "", "Make file");

	WriteDestFileLn("# make file generated by bash configure script");
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_output_d = ");
	Write_output_d_ToDestFile();
	WriteEndDestFileLn();
	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_source_d = ");
	Write_source_d_ToDestFile();
	WriteEndDestFileLn();
	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_obj_d = $(mk_output_d)obj/");
	WriteDestFileLn("mk_config_d = $(mk_output_d)config/");
	if (CurTarget == kTargMacX11) {
		WriteBgnDestFileLn();
		WriteCStrToOutput("mk_built_program = $(mk_output_d)");
		WriteCStrToOutput(kStrAppAbbrev);
		WriteEndDestFileLn();
	} else {
		WriteBgnDestFileLn();
		WriteCStrToOutput("mk_built_program = $(mk_output_d)");
		WriteMachoAppNameStr();
		WriteCStrToOutput("/");
		WriteEndDestFileLn();

		WriteDestFileLn("mk_contents_d = $(mk_built_program)Contents/");
		WriteDestFileLn("mk_macos_d = $(mk_contents_d)MacOS/");
		WriteDestFileLn("mk_resources_d = $(mk_contents_d)Resources/");

		WriteBgnDestFileLn();
		WriteCStrToOutput("mk_built_exec = $(mk_macos_d)");
		WriteCStrToOutput(kStrAppAbbrev);
		WriteEndDestFileLn();

#if 0
		WriteBgnDestFileLn();
		WriteCStrToOutput("mk_built_rsrc = $(mk_resources_d)");
		WriteCStrToOutput(kStrAppAbbrev);
		WriteCStrToOutput(".rsrc");
		WriteEndDestFileLn();
#endif
	}
	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_a_src_d = $(mk_source_d)a_src/ppc/as/");
	WriteDestFileLn("mk_c_src_d = $(mk_source_d)c_src/");
	WriteDestFileLn("mk_platform_d = $(mk_source_d)platform/");
	if (CurTarget != kTargMacX11) {
		WriteDestFileLn("mk_rz_src_d = $(mk_platform_d)mac/");
		WriteDestFileLn("mk_osxrz_d = $(mk_rz_src_d)osx/");
	}
	WriteBlankLineToDestFile();
	if (HaveAsm) {
		WriteDestFileLn("mk_CallA = gcc");
		WriteDestFileLn("mk_AOptions = -c");
	}
	WriteDestFileLn("mk_CallC = gcc");
	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_COptions =");
	WriteCommonCOptions(falseblnr);
	WriteEndDestFileLn();
	WriteBlankLineToDestFile();
	if (CurTarget != kTargMacX11) {
#if 0
		WriteDestFileLn("TheApplication : $(mk_built_exec) $(mk_built_rsrc)");
#endif
		WriteDestFileLn("TheApplication : $(mk_built_exec)");
	} else {
		WriteDestFileLn("TheApplication : $(mk_built_program)");
	}
	WriteBlankLineToDestFile();
	DoAllSrcFiles(DoSrcFileBgcMakeCompile);
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToOutput("ObjFiles = ");
	WriteBackSlashToDestFile();
	WriteEndDestFileLn();
	++DestFileIndent;
		DoAllSrcFiles(DoSrcFileBgcMakeObjects);
		WriteBlankLineToDestFile();
	--DestFileIndent;
	if (CurTarget != kTargMacX11) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("mk_tbuilt_program = AppTemp/");
		WriteDestFileLn("mk_tcontents_d = $(mk_tbuilt_program)Contents/");
		WriteDestFileLn("mk_tmacos_d = $(mk_tcontents_d)MacOS/");
		WriteDestFileLn("mk_tresources_d = $(mk_tcontents_d)Resources/");
		WriteBlankLineToDestFile();
		WriteDestFileLn("$(mk_resources_d)AppIcon.icns : $(mk_osxrz_d)AppIcon.icns");
		++DestFileIndent;
			WriteDestFileLn("rm -fr \"$(mk_built_program)\"");
			WriteDestFileLn("rm -fr \"$(mk_tbuilt_program)\"");
			WriteDestFileLn("mkdir \"$(mk_tbuilt_program)\"");
			WriteDestFileLn("mkdir \"$(mk_tcontents_d)\"");
			WriteDestFileLn("mkdir \"$(mk_tmacos_d)\"");
			WriteDestFileLn("mkdir \"$(mk_tresources_d)\"");
			WriteDestFileLn("cp \"$(mk_osxrz_d)AppIcon.icns\" \"$(mk_tresources_d)\"");
			DoAllDocTypes(WriteDocTypeCopyMachoFile);
			WriteDestFileLn("cp \"$(mk_config_d)Info.plist\" \"$(mk_tcontents_d)\"");
			WriteDestFileLn("mv \"$(mk_tbuilt_program)\" \"$(mk_built_program)\"");
		--DestFileIndent;
	}
	WriteBlankLineToDestFile();
	if (CurTarget != kTargMacX11) {
		WriteDestFileLn("$(mk_built_exec) : $(ObjFiles) $(mk_resources_d)AppIcon.icns");
	} else {
		WriteDestFileLn("$(mk_built_program) : $(ObjFiles)");
	}
	++DestFileIndent;
		WriteDestFileLn("gcc \\");
		++DestFileIndent;
			if (CurTarget == kTargMacX11) {
				WriteDestFileLn("-o \"$(mk_built_program)\" \\");
				WriteDestFileLn("-L/usr/X11R6/lib -lXext -lX11 \\");
			} else {
				WriteDestFileLn("-o \"$(mk_built_exec)\" \\");
				WriteDestFileLn("-framework Carbon \\");
			}
			WriteDestFileLn("$(ObjFiles)");
		--DestFileIndent;
		if (CurDbgLvl == kDbgLvlShip) {
			if (CurTarget == kTargMacX11) {
				WriteDestFileLn("strip -u -r \"$(mk_built_program)\"");
			} else {
				WriteDestFileLn("strip -u -r \"$(mk_built_exec)\"");
			}
		}
	--DestFileIndent;
#if 0
	if (CurTarget != kTargMacX11) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("$(mk_built_rsrc) : $(mk_rz_src_d)main.r $(mk_resources_d)AppIcon.icns");
		++DestFileIndent;
			WriteDestFileLn("/Developer/Tools/Rez \\");
			++DestFileIndent;
				WriteDestFileLn("-i /Developer/Headers/FlatCarbon \\");
				WriteDestFileLn("-i \"$(mk_config_d)\" \\");
				WriteDestFileLn("\"$(mk_rz_src_d)main.r\" \\");
				WriteDestFileLn("-o \"$(mk_built_rsrc)\" \\");
				WriteDestFileLn("-useDF");
			--DestFileIndent;
		--DestFileIndent;
	}
#endif
	WriteBlankLineToDestFile();
	WriteDestFileLn("clean :");
	++DestFileIndent;
		WriteDestFileLn("rm -f $(ObjFiles)");
		if (CurTarget == kTargMacX11) {
			WriteDestFileLn("rm -f \"$(mk_built_program)\"");
		} else {
			WriteDestFileLn("rm -fr \"$(mk_built_program)\"");
		}
	--DestFileIndent;
	WriteCloseDestFile();
}

static void WriteRH7SpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", "Makefile", "", "Make file");

	WriteDestFileLn("# make file generated by bash configure script");
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_output_d = ");
	Write_output_d_ToDestFile();
	WriteEndDestFileLn();
	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_source_d = ");
	Write_source_d_ToDestFile();
	WriteEndDestFileLn();
	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_obj_d = $(mk_output_d)obj/");
	WriteDestFileLn("mk_config_d = $(mk_output_d)config/");
	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_built_program = $(mk_output_d)");
	WriteCStrToOutput(kStrAppAbbrev);
	WriteEndDestFileLn();
	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_c_src_d = $(mk_source_d)c_src/");
	WriteDestFileLn("mk_platform_d = $(mk_source_d)platform/");
	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_CallC = gcc");
	WriteBgnDestFileLn();
	WriteCStrToOutput("mk_COptions =");
	WriteCommonCOptions(falseblnr);
	WriteEndDestFileLn();
	WriteBlankLineToDestFile();
	WriteDestFileLn("TheApplication : $(mk_built_program)");
	WriteBlankLineToDestFile();
	DoAllSrcFiles(DoSrcFileBgcMakeCompile);
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToOutput("ObjFiles = ");
	WriteBackSlashToDestFile();
	WriteEndDestFileLn();
	++DestFileIndent;
		DoAllSrcFiles(DoSrcFileBgcMakeObjects);
		WriteBlankLineToDestFile();
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteDestFileLn("$(mk_built_program) : $(ObjFiles)");
	++DestFileIndent;
		WriteDestFileLn("gcc \\");
		++DestFileIndent;
			WriteDestFileLn("-o \"$(mk_built_program)\" \\");
			WriteDestFileLn("-L/usr/X11R6/lib -lXext -lX11 \\");
			WriteDestFileLn("$(ObjFiles)");
		--DestFileIndent;
		if (CurDbgLvl == kDbgLvlShip) {
			if (CurIde != kIdeRH7) {
				WriteDestFileLn("strip -u -r \"$(mk_built_program)\"");
			}
		}
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteDestFileLn("clean :");
	++DestFileIndent;
		WriteDestFileLn("rm -f $(ObjFiles)");
		WriteDestFileLn("rm -f \"$(mk_built_program)\"");
	--DestFileIndent;
	WriteCloseDestFile();
}

static void WriteFileToMSVCSource(MyProc p, char *s, char *FileExt)
{
	WriteDestFileLn("# Begin Source File");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToOutput("SOURCE=");
	WriteFileInDirToDestFile(p, s, FileExt);
	WriteEndDestFileLn();

	WriteDestFileLn("# End Source File");
}

static void WriteAddMSVCIcon(char *s)
{
	WriteFileToMSVCSource(Write_rz_src_ToDestFile, s, ".ico");
}

static void WriteAddMSVC_config(char *s)
{
	WriteFileToMSVCSource(Write_config_d_ToDestFile, s, ".h");
}

static void WriteMSVCBeginGroup(char *group, char *filter)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput("# Begin Group ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(group);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToOutput("# PROP Default_Filter ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(filter);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

static void WriteMSVCEndGroup(void)
{
	WriteDestFileLn("# End Group");
}

static void WriteMSVCTargetName(void)
{
	WriteQuoteToDestFile();
	WriteCStrToOutput(kStrAppAbbrev);
	WriteCStrToOutput(" - Win32");
	/* WriteAppVariationStr(); */
	WriteQuoteToDestFile();
}

static void WriteMSVCMakefileName(void)
{
	WriteQuoteToDestFile();
	WriteCStrToOutput(kStrAppAbbrev);
	WriteCStrToOutput(".mak");
	WriteQuoteToDestFile();
}

static void WriteMSVCQuotedDefine(char *s)
{
	WriteCStrToOutput(" /D ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(s);
	WriteQuoteToDestFile();
}

static void WriteMSVCQuotedProp(char *p, char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput("# PROP ");
	WriteCStrToOutput(p);
	WriteCStrToOutput(" ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(s);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

static void DoSrcFileMSVCAddFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteDestFileLn("# Begin Source File");
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToOutput("SOURCE=");
	WriteFileInDirToDestFile(Write_c_src_d_ToDestFile, s, ".c");
	WriteEndDestFileLn();
#if 0
	if (CurDbgLvl != kDbgLvlQuick) {
		if (fast) {
			WriteDestFileLn("# ADD CPP /O2 /Ob2");
		}
	}
#endif
	WriteDestFileLn("# End Source File");
}

static void WriteMSVCSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", kStrAppAbbrev, ".dsw", "workspace file");
	WriteDestFileLn("Microsoft Developer Studio Workspace File, Format Version 6.00");
	WriteDestFileLn("# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!");
	WriteBlankLineToDestFile();
	WriteDestFileLn("###############################################################################");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToOutput("Project: ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(kStrAppAbbrev);
	WriteQuoteToDestFile();
	WriteCStrToOutput("=.");
	WriteBackSlashToDestFile();
	WriteCStrToOutput(kStrAppAbbrev);
	WriteCStrToOutput(".dsp - Package Owner=<4>");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<5>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<4>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn("###############################################################################");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Global:");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<5>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<3>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn("###############################################################################");
	WriteBlankLineToDestFile();
	WriteCloseDestFile();

	WriteOpenDestFile("my_output_d", kStrAppAbbrev, ".dsp", "project file");

	WriteBgnDestFileLn();
	WriteCStrToOutput("# Microsoft Developer Studio Project File - Name=");
	WriteQuoteToDestFile();
	WriteCStrToOutput(kStrAppAbbrev);
	WriteQuoteToDestFile();
	WriteCStrToOutput(" - Package Owner=<4>");
	WriteEndDestFileLn();

	WriteDestFileLn("# Microsoft Developer Studio Generated Build File, Format Version 6.00");
	WriteDestFileLn("# ** DO NOT EDIT **");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToOutput("# TARGTYPE ");
	WriteQuoteToDestFile();
	WriteCStrToOutput("Win32 (x86) Application");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" 0x0101");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToOutput("CFG=");
	/* WriteAppVariationStr(); */
	WriteCStrToOutput(kStrAppAbbrev);
	WriteCStrToOutput(" - Win32");
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE This is not a valid makefile. To build this project using NMAKE,");
	WriteDestFileLn("!MESSAGE use the Export Makefile command and run");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToOutput("!MESSAGE NMAKE /f ");
	WriteMSVCMakefileName();
	WriteCStrToOutput(".");
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteDestFileLn("!MESSAGE You can specify a configuration when running NMAKE");
	WriteDestFileLn("!MESSAGE by defining the macro CFG on the command line. For example:");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToOutput("!MESSAGE NMAKE /f ");
	WriteMSVCMakefileName();
	WriteCStrToOutput(" CFG=");
	WriteMSVCTargetName();
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteDestFileLn("!MESSAGE Possible choices for configuration are:");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToOutput("!MESSAGE ");
	WriteMSVCTargetName();
	WriteCStrToOutput(" (based on ");
	WriteQuoteToDestFile();
	WriteCStrToOutput("Win32 (x86) Application");
	WriteQuoteToDestFile();
	WriteCStrToOutput(")");
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteBlankLineToDestFile();
	WriteDestFileLn("# Begin Project");
	WriteDestFileLn("# PROP AllowPerConfigDependencies 0");

	WriteMSVCQuotedProp("Scc_ProjName", "");
	WriteMSVCQuotedProp("Scc_LocalPath", "");

	WriteDestFileLn("CPP=cl.exe");
	WriteDestFileLn("MTL=midl.exe");
	WriteDestFileLn("RSC=rc.exe");
	WriteDestFileLn("# PROP BASE Use_MFC 0");
	WriteDestFileLn("# PROP BASE Use_Debug_Libraries 1");

	WriteMSVCQuotedProp("BASE Output_Dir", "Debug");
	WriteMSVCQuotedProp("BASE Intermediate_Dir", "Debug");
	WriteMSVCQuotedProp("BASE Target_Dir", "");

	WriteDestFileLn("# PROP Use_MFC 0");
	if (CurDbgLvl == kDbgLvlQuick) {
		WriteDestFileLn("# PROP Use_Debug_Libraries 1");
	} else {
		WriteDestFileLn("# PROP Use_Debug_Libraries 0");
	}

	WriteMSVCQuotedProp("Output_Dir", "obj");
	WriteMSVCQuotedProp("Intermediate_Dir", "obj");

	WriteDestFileLn("# PROP Ignore_Export_Lib 0");

	WriteMSVCQuotedProp("Target_Dir", "");

	WriteBgnDestFileLn();
	WriteCStrToOutput("# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od");
	WriteMSVCQuotedDefine("WIN32");
	WriteMSVCQuotedDefine("_DEBUG");
	WriteMSVCQuotedDefine("_WINDOWS");
	WriteMSVCQuotedDefine("_MBCS");
	WriteCStrToOutput(" /YX /FD /GZ /c");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("# ADD CPP /nologo");
	if (CurDbgLvl == kDbgLvlQuick) {
		WriteCStrToOutput(" /W4 /Gm /GX /ZI /Od");
	} else {
		WriteCStrToOutput(" /ML /W4 /O1");
	}
	WriteCStrToOutput(" /I ");
	WriteQuoteToDestFile();
	WriteCStrToOutput("config");
	WriteQuoteToDestFile();
	WriteMSVCQuotedDefine("WIN32");
	if (CurDbgLvl == kDbgLvlQuick) {
		WriteMSVCQuotedDefine("_DEBUG");
	} else {
		WriteMSVCQuotedDefine("NDEBUG");
	}
	WriteMSVCQuotedDefine("_WINDOWS");
	WriteMSVCQuotedDefine("_MBCS");
	WriteCStrToOutput(" /FD");
	if (CurDbgLvl == kDbgLvlQuick) {
		WriteCStrToOutput(" /GZ");
	} else {
		WriteCStrToOutput(" /GF");
	}
	WriteCStrToOutput(" /c");
	WriteEndDestFileLn();

	WriteDestFileLn("# SUBTRACT CPP /YX /Yc /Yu");

	WriteBgnDestFileLn();
	WriteCStrToOutput("# ADD BASE MTL /nologo /D ");
	WriteQuoteToDestFile();
	WriteCStrToOutput("_DEBUG");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" /mktyplib203 /win32");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("# ADD MTL /nologo /D ");
	WriteQuoteToDestFile();
	if (CurDbgLvl == kDbgLvlQuick) {
		WriteCStrToOutput("_DEBUG");
	} else {
		WriteCStrToOutput("NDEBUG");
	}
	WriteQuoteToDestFile();
	WriteCStrToOutput(" /mktyplib203 /win32");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("# ADD BASE RSC /l 0x409 /d ");
	WriteQuoteToDestFile();
	WriteCStrToOutput("_DEBUG");
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("# ADD RSC /l 0x409 /d ");
	WriteQuoteToDestFile();
	if (CurDbgLvl == kDbgLvlQuick) {
		WriteCStrToOutput("_DEBUG");
	} else {
		WriteCStrToOutput("NDEBUG");
	}
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteDestFileLn("BSC32=bscmake.exe");
	WriteDestFileLn("# ADD BASE BSC32 /nologo");
	WriteDestFileLn("# ADD BSC32 /nologo");
	WriteDestFileLn("LINK32=link.exe");
	WriteDestFileLn("# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept");

	WriteBgnDestFileLn();
	WriteCStrToOutput("# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib winmm.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows");
	if (CurDbgLvl == kDbgLvlQuick) {
		WriteCStrToOutput(" /debug");
	} else {
		WriteCStrToOutput(" /incremental:no");
	}
	WriteCStrToOutput(" /machine:I386 /out:");
	WriteQuoteToDestFile();
	WriteWinAppNameStr();
	WriteQuoteToDestFile();
	if (CurDbgLvl == kDbgLvlQuick) {
		WriteCStrToOutput(" /pdbtype:sept");
	}
	WriteEndDestFileLn();

	if (CurDbgLvl != kDbgLvlQuick) {
		WriteDestFileLn("# SUBTRACT LINK32 /debug /pdbtype:<none>");
	}
	WriteDestFileLn("# Begin Target");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToOutput("# Name ");
	WriteMSVCTargetName();
	WriteEndDestFileLn();

	WriteMSVCBeginGroup("Source Files", "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat");
		DoAllSrcFiles(DoSrcFileMSVCAddFile);
		WriteFileToMSVCSource(Write_rz_src_ToDestFile, "main", ".RC");
	WriteMSVCEndGroup();

	WriteMSVCBeginGroup("Header Files", "h;hpp;hxx;hm;inl");
		WriteAddMSVC_config("CNFGGLOB");
		WriteAddMSVC_config("CNFGRAPI");
	WriteMSVCEndGroup();

	WriteMSVCBeginGroup("Resource Files", "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe");
		WriteAddMSVCIcon("AppIcon");
		WriteAddMSVCIcon("DataIcon");
		WriteAddMSVCIcon("ShoeIcon");
	WriteMSVCEndGroup();

	WriteDestFileLn("# End Target");
	WriteDestFileLn("# End Project");
	WriteCloseDestFile();
}

static void DoSrcFileLccAddFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBgnDestFileLn();
	WriteCStrToOutput("File");
	WriteUnsignedToOutput(++FileCounter);
	WriteCStrToOutput("=");
	WriteFileInDirToDestFile(Write_c_src_d_ToDestFile, s, ".c");
	WriteEndDestFileLn();
}

static void WriteLccW32SpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", kStrAppAbbrev, ".prj", "workspace file");

	WriteDestFileLn("; Wedit project file. Syntax: Name = value");

	WriteBgnDestFileLn();
	WriteCStrToOutput("[");
	WriteAppVariationStr();
	WriteCStrToOutput("]");
	WriteEndDestFileLn();

	FileCounter = 0;
	DoAllSrcFiles(DoSrcFileIncrFileCounter);
	++FileCounter; /* main.rc */

	WriteBgnDestFileLn();
	WriteCStrToOutput("PrjFiles=");
	WriteUnsignedToOutput(FileCounter);
	WriteEndDestFileLn();

	FileCounter = 0;
	DoAllSrcFiles(DoSrcFileLccAddFile);

	WriteBgnDestFileLn();
	WriteCStrToOutput("File");
	WriteUnsignedToOutput(++FileCounter);
	WriteCStrToOutput("=");
	WriteFileInDirToDestFile(Write_rz_src_ToDestFile, "main", ".rc");
	WriteEndDestFileLn();

	WriteDestFileLn("ProjectFlags=0");

	WriteBgnDestFileLn();
	WriteCStrToOutput("Name=");
	WriteAppVariationStr();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("ProjectPath=");
	Write_output_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("SourcesDir=");
	/* setting it to my_c_src_d does not work */
	Write_output_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("Includes=");
	Write_output_d_ToDestFile();
	WriteBackSlashToDestFile();
	WriteCStrToOutput("config");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("MakeDir=");
	Write_obj_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("Exe=");
	Write_output_d_ToDestFile();
	WriteBackSlashToDestFile();
	WriteWinAppNameStr();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("DbgExeName=");
	Write_output_d_ToDestFile();
	WriteBackSlashToDestFile();
	WriteWinAppNameStr();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("DbgDir=");
	Write_obj_d_ToDestFile();
	WriteEndDestFileLn();

	switch (CurDbgLvl) {
		case kDbgLvlQuick:
			WriteDestFileLn("CompilerFlags=6728");
			break;
		case kDbgLvlTest:
			WriteDestFileLn("CompilerFlags=580");
			break;
		case kDbgLvlShip:
			WriteDestFileLn("CompilerFlags=581");
			break;
	}

	WriteDestFileLn("Libraries=shell32.lib winmm.lib");

	WriteBgnDestFileLn();
	WriteCStrToOutput("ErrorFile=");
	WriteFileInDirToDestFile(Write_obj_d_ToDestFile, kStrAppAbbrev, ".err");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("CurrentFile=");
	WriteFileInDirToDestFile(Write_c_src_d_ToDestFile, "WNOSGLUE", ".h");
	WriteEndDestFileLn();

	WriteDestFileLn("OpenFiles=1");

	WriteBgnDestFileLn();
	WriteCStrToOutput("OpenFile1=");
	WriteQuoteToDestFile();
	WriteFileInDirToDestFile(Write_c_src_d_ToDestFile, "WNOSGLUE", ".h");
	WriteQuoteToDestFile();
	WriteCStrToOutput(" 1 29 14 532 435");
	WriteEndDestFileLn();

	WriteCloseDestFile();
}

static void DoSrcFileDvcAddFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToOutput("[Unit");
	WriteUnsignedToOutput(++FileCounter);
	WriteCStrToOutput("]");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("FileName=");
	WriteFileInDirToDestFile(Write_c_src_d_ToDestFile, s, ".c");
	WriteEndDestFileLn();

	WriteDestFileLn("CompileCpp=0");
	WriteBgnDestFileLn();
	WriteCStrToOutput("Folder=");
	WriteAppVariationStr();
	WriteEndDestFileLn();
	WriteDestFileLn("Compile=1");
	WriteDestFileLn("Link=1");
	WriteDestFileLn("Priority=1000");
	WriteDestFileLn("OverrideBuildCmd=0");
	WriteDestFileLn("BuildCmd=");
}

static void WriteDevCSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", kStrAppAbbrev, ".dev", "project file");

	WriteDestFileLn("[Project]");

	WriteBgnDestFileLn();
	WriteCStrToOutput("FileName=");
	WriteCStrToOutput(kStrAppAbbrev);
	WriteCStrToOutput(".dev");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("Name=");
	WriteAppVariationStr();
	WriteEndDestFileLn();

	FileCounter = 0;
	DoAllSrcFiles(DoSrcFileIncrFileCounter);
	++FileCounter; /* main.rc */

	WriteBgnDestFileLn();
	WriteCStrToOutput("UnitCount=");
	WriteUnsignedToOutput(FileCounter);
	WriteEndDestFileLn();

	WriteDestFileLn("Type=0");
	WriteDestFileLn("Ver=1");
	WriteDestFileLn("Includes=config");

	WriteBgnDestFileLn();
	WriteCStrToOutput("PrivateResource=");
	WriteAppVariationStr();
	WriteCStrToOutput("_private.rc");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("ResourceIncludes=");
	Write_rz_src_ToDestFile();
	WriteEndDestFileLn();

	WriteDestFileLn("IsCpp=0");
	WriteDestFileLn("ObjectOutput=obj");
	WriteDestFileLn("OverrideOutput=1");

	WriteBgnDestFileLn();
	WriteCStrToOutput("OverrideOutputName=");
	WriteWinAppNameStr();
	WriteEndDestFileLn();

	WriteDestFileLn("IncludeVersionInfo=0");
	WriteDestFileLn("CompilerSet=0");
	if (CurDbgLvl == kDbgLvlQuick) {
		WriteDestFileLn("CompilerSettings=000000000000000100");
		WriteDestFileLn("Compiler= -Wall -Wstrict-prototypes -Wno-uninitialized -O0_@@_");
	} else {
		WriteDestFileLn("CompilerSettings=000000000000000000");
		WriteDestFileLn("Compiler= -Wall -Wstrict-prototypes -Wno-uninitialized -Os_@@_");
	}
	WriteDestFileLn("Linker=-lwinmm_@@_");
	WriteBlankLineToDestFile();
	WriteDestFileLn("[VersionInfo]");
	WriteDestFileLn("Major=0");
	WriteDestFileLn("Minor=1");
	WriteDestFileLn("Release=1");
	WriteDestFileLn("Build=1");
	WriteDestFileLn("LanguageID=1033");
	WriteDestFileLn("CharsetID=1252");
	WriteDestFileLn("CompanyName=");
	WriteDestFileLn("FileVersion=");
	WriteDestFileLn("FileDescription=Developed using the Dev-C++ IDE");
	WriteDestFileLn("InternalName=");
	WriteDestFileLn("LegalCopyright=");
	WriteDestFileLn("LegalTrademarks=");
	WriteDestFileLn("OriginalFilename=");
	WriteDestFileLn("ProductName=");
	WriteDestFileLn("ProductVersion=");
	WriteDestFileLn("AutoIncBuildNr=0");

	FileCounter = 0;
	DoAllSrcFiles(DoSrcFileDvcAddFile);

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToOutput("[Unit");
	WriteUnsignedToOutput(++FileCounter);
	WriteCStrToOutput("]");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("FileName=");
	WriteFileInDirToDestFile(Write_rz_src_ToDestFile, "main", ".RC");
	WriteEndDestFileLn();

	WriteDestFileLn("CompileCpp=0");
	WriteBgnDestFileLn();
	WriteCStrToOutput("Folder=");
	WriteAppVariationStr();
	WriteEndDestFileLn();
	WriteDestFileLn("Compile=1");
	WriteDestFileLn("Link=0");
	WriteDestFileLn("Priority=1000");
	WriteDestFileLn("OverrideBuildCmd=0");
	WriteDestFileLn("BuildCmd=");

	WriteBlankLineToDestFile();

	WriteCloseDestFile();
}

static void WriteAPBBeginObject(unsigned int theClass, unsigned int v)
{
	WriteBgnDestFileLn();
	WriteHexWordToOutput(theClass);
	WriteHexWordToOutput(v);
	WriteCStrToOutput("0000000000000000");
	WriteCStrToOutput(" = {");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteAPBEndObject(void)
{
	--DestFileIndent;
	WriteDestFileLn("};");
}

static void WriteAPBobjlistelm(unsigned int theClass, unsigned int v)
{
	WriteBgnDestFileLn();
	WriteHexWordToOutput(theClass);
	WriteHexWordToOutput(v);
	WriteCStrToOutput("0000000000000000");
	WriteCStrToOutput(",");
	WriteEndDestFileLn();
}

static void WriteAPBBgnObjList(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput(s);
	WriteCStrToOutput(" = (");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteAPBEndObjList(void)
{
	--DestFileIndent;
	WriteDestFileLn(");");
}

static void WriteAPBObjRef(char *s, unsigned int theClass, unsigned int v)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput(s);
	WriteCStrToOutput(" = ");
	WriteHexWordToOutput(theClass);
	WriteHexWordToOutput(v);
	WriteCStrToOutput("0000000000000000");
	WriteCStrToOutput(";");
	WriteEndDestFileLn();
}

static void WriteAPBQuotedField(char *s, char *v)
{
	WriteBgnDestFileLn();
	WriteCStrToOutput(s);
	WriteCStrToOutput(" = ");
	WriteQuoteToDestFile();
	WriteCStrToOutput(v);
	WriteQuoteToDestFile();
	WriteCStrToOutput(";");
	WriteEndDestFileLn();
}

enum {
	APBoclsSrc,
	APBoclsIcns,
	APBoclsLibs,
	APBoclsGen,
	kNumAPBocls
};

enum {
#if 0
	APBgenoMainRsrcRf,
	APBgenoMnRsrcBld,
#endif
	APBgenoProductRef,
	APBgenoBuildStyle,
	APBgenoSources,
	APBgenoResources,
	APBgenoLibraries,
	APBgenoProducts,
	APBgenoMainGroup,
	APBgenoHeaders,
	APBgenoBunRsrcs,
	APBgenoPhaseSrcs,
	APBgenoPhaseLibs,
#if 0
	APBgenoPhaseRsrc,
#endif
	APBgenoTarget,
	APBgenoRoot,
	kNumAPBgeno
};

enum {
	APBliboCarbonRf,
	APBliboCarbonBld,
	APBliboLibStdcRf,
	APBliboLibStdcBld,
	kNumAPBlibo
};

static void DoSrcFileAPBadd(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(depends);
	WriteAPBBeginObject(APBoclsSrc, 2 * FileCounter);
		WriteDestFileLn("fileEncoding = 30;");
		WriteDestFileLn("isa = PBXFileReference;");
		WriteBgnDestFileLn();
		WriteCStrToOutput("name = ");
		WriteCStrToOutput(s);
		WriteSrcSuffix(AsmAvail);
		WriteCStrToOutput(";");
		WriteEndDestFileLn();
		WriteBgnDestFileLn();
		WriteCStrToOutput("path = ");
		if (AsmAvail && HaveAsm) {
			Write_a_src_d_ToDestFile();
		} else {
			Write_c_src_d_ToDestFile();
		}
		WriteCStrToOutput(s);
		WriteSrcSuffix(AsmAvail);
		WriteCStrToOutput(";");
		WriteEndDestFileLn();
		WriteDestFileLn("refType = 2;");
	WriteAPBEndObject();
	WriteAPBBeginObject(APBoclsSrc, 2 * FileCounter + 1);
		WriteAPBObjRef("fileRef", APBoclsSrc, 2 * FileCounter);
		WriteDestFileLn("isa = PBXBuildFile;");
		WriteDestFileLn("settings = {");
		WriteDestFileLn("};");
	WriteAPBEndObject();
	++FileCounter;
}

static void DoSrcFileAPBaddRef(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(s);
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteAPBobjlistelm(APBoclsSrc, 2 * FileCounter);
	++FileCounter;
}

static void DoSrcFileAPBaddBld(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(s);
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteAPBobjlistelm(APBoclsSrc, 2 * FileCounter + 1);
	++FileCounter;
}

static void WriteAPBplist(void)
{
	int SaveDestFileIndent = DestFileIndent;

	DestFileIndent = 0;

	WriteBgnDestFileLn();
	WriteCStrToOutput("<!DOCTYPE plist PUBLIC ");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToOutput("-//Apple Computer//DTD PLIST 1.0//EN");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToOutput(" ");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToOutput("http://www.apple.com/DTDs/PropertyList-1.0.dtd");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToOutput(">");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToOutput("<plist version=");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToOutput("1.0");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToOutput(">");
	WriteEndDestFileLn();

	CurPListFormat = kPListRaw;

	WriteMyInfoPListContents();

	WriteDestFileLn("</plist>");
	WriteBgnDestFileLn();
	WriteQuoteToDestFile();
	WriteCStrToOutput(";");
	WriteEndDestFileLn();

	DestFileIndent = SaveDestFileIndent;
}

static void WriteDocTypeAPBadd(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);

	++DocTypeCounter;
	WriteAPBBeginObject(APBoclsIcns, 2 * DocTypeCounter);
		WriteDestFileLn("isa = PBXFileReference;");

		WriteBgnDestFileLn();
		WriteCStrToOutput("name = ");
		WriteCStrToOutput(ShortName);
		WriteCStrToOutput("Icon.icns;");
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToOutput("path = ");
		Write_source_d_ToDestFile();
		WriteCStrToOutput("platform/mac/osx/");
		WriteCStrToOutput(ShortName);
		WriteCStrToOutput("Icon.icns;");
		WriteEndDestFileLn();

		WriteDestFileLn("refType = 2;");
	WriteAPBEndObject();
	WriteAPBBeginObject(APBoclsIcns, 2 * DocTypeCounter + 1);
		WriteAPBObjRef("fileRef", APBoclsIcns, 2 * DocTypeCounter);
		WriteDestFileLn("isa = PBXBuildFile;");
		WriteDestFileLn("settings = {");
		WriteDestFileLn("};");
	WriteAPBEndObject();
}

static void WriteDocTypeAPBaddRef(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(ShortName);
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);

	++DocTypeCounter;
	WriteAPBobjlistelm(APBoclsIcns, 2 * DocTypeCounter);
}

static void WriteDocTypeAPBaddBld(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(ShortName);
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);

	++DocTypeCounter;
	WriteAPBobjlistelm(APBoclsIcns, 2 * DocTypeCounter + 1);
}

static void WriteAPBSpecificFiles(void)
{
	WriteOpenDestFile("my_proj_d", "project", ".pbxproj", "project file");

	WriteDestFileLn("// !$*UTF8*$!");
	WriteDestFileLn("{");
	++DestFileIndent;
		WriteDestFileLn("archiveVersion = 1;");
		WriteDestFileLn("classes = {");
		WriteDestFileLn("};");
		WriteDestFileLn("objectVersion = 38;");
		WriteDestFileLn("objects = {");
		++DestFileIndent;
			FileCounter = 0;
			DoAllSrcFiles(DoSrcFileAPBadd);

			WriteAPBBeginObject(APBoclsIcns, 0);
				WriteDestFileLn("isa = PBXFileReference;");
				WriteDestFileLn("name = AppIcon.icns;");
				WriteBgnDestFileLn();
				WriteCStrToOutput("path = ");
				Write_source_d_ToDestFile();
				WriteCStrToOutput("platform/mac/osx/AppIcon.icns;");
				WriteEndDestFileLn();
				WriteDestFileLn("refType = 2;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsIcns, 1);
				WriteAPBObjRef("fileRef", APBoclsIcns, 0);
				WriteDestFileLn("isa = PBXBuildFile;");
				WriteDestFileLn("settings = {");
				WriteDestFileLn("};");
			WriteAPBEndObject();
			DocTypeCounter = 0;
			DoAllDocTypes(WriteDocTypeAPBadd);

			WriteAPBBeginObject(APBoclsLibs, APBliboCarbonRf);
				WriteDestFileLn("isa = PBXFrameworkReference;");
				WriteDestFileLn("name = Carbon.framework;");
				WriteDestFileLn("path = /System/Library/Frameworks/Carbon.framework;");
				WriteDestFileLn("refType = 0;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsLibs, APBliboCarbonBld);
				WriteAPBObjRef("fileRef", APBoclsLibs, APBliboCarbonRf);
				WriteDestFileLn("isa = PBXBuildFile;");
				WriteDestFileLn("settings = {");
				WriteDestFileLn("};");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsLibs, APBliboLibStdcRf);
				WriteDestFileLn("isa = PBXFileReference;");
				WriteAPBQuotedField("name", "libstdc++.a");
				WriteAPBQuotedField("path", "/usr/lib/libstdc++.a");
				WriteDestFileLn("refType = 0;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsLibs, APBliboLibStdcBld);
				WriteAPBObjRef("fileRef", APBoclsLibs, APBliboLibStdcRf);
				WriteDestFileLn("isa = PBXBuildFile;");
				WriteDestFileLn("settings = {");
				WriteDestFileLn("};");
			WriteAPBEndObject();
#if 0
			WriteAPBBeginObject(APBoclsGen, APBgenoMainRsrcRf);
				WriteDestFileLn("fileEncoding = 30;");
				WriteDestFileLn("isa = PBXFileReference;");
				WriteDestFileLn("name = main.r;");
				WriteBgnDestFileLn();
				WriteCStrToOutput("path = ");
				Write_source_d_ToDestFile();
				WriteCStrToOutput("platform/mac/main.r;");
				WriteEndDestFileLn();
				WriteDestFileLn("refType = 2;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoMnRsrcBld);
				WriteAPBObjRef("fileRef", APBoclsGen, APBgenoMainRsrcRf);
				WriteDestFileLn("isa = PBXBuildFile;");
				WriteDestFileLn("settings = {");
				WriteDestFileLn("};");
			WriteAPBEndObject();
#endif
			WriteAPBBeginObject(APBoclsGen, APBgenoProductRef);
				WriteDestFileLn("isa = PBXApplicationReference;");
				WriteBgnDestFileLn();
				WriteCStrToOutput("path = ");
				WriteMachoAppNameStr();
				WriteCStrToOutput(";");
				WriteEndDestFileLn();
				WriteDestFileLn("refType = 3;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoBuildStyle);
				WriteDestFileLn("buildRules = (");
				WriteDestFileLn(");");
				WriteDestFileLn("buildSettings = {");
				++DestFileIndent;
					if (CurDbgLvl != kDbgLvlQuick) {
						WriteDestFileLn("COPY_PHASE_STRIP = YES;");
					} else {
						WriteDestFileLn("COPY_PHASE_STRIP = NO;");
						WriteAPBQuotedField("OPTIMIZATION_CFLAGS", "-O0");
					}
				--DestFileIndent;
				WriteDestFileLn("};");
				WriteDestFileLn("isa = PBXBuildStyle;");
				switch (CurDbgLvl) {
					case kDbgLvlQuick:
						WriteDestFileLn("name = Development;");
						break;
					case kDbgLvlTest:
						WriteDestFileLn("name = Test;");
						break;
					case kDbgLvlShip:
						WriteDestFileLn("name = Deployment;");
						break;
				}
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoSources);
				WriteAPBBgnObjList("children");
					FileCounter = 0;
					DoAllSrcFiles(DoSrcFileAPBaddRef);
				WriteAPBEndObjList();
				WriteDestFileLn("isa = PBXGroup;");
				WriteDestFileLn("name = Sources;");
				WriteAPBQuotedField("path", "");
				WriteDestFileLn("refType = 4;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoResources);
				WriteAPBBgnObjList("children");
#if 0
					WriteAPBobjlistelm(APBoclsGen, APBgenoMainRsrcRf);
#endif
					WriteAPBobjlistelm(APBoclsIcns, 0);
					DocTypeCounter = 0;
					DoAllDocTypes(WriteDocTypeAPBaddRef);
				WriteAPBEndObjList();
				WriteDestFileLn("isa = PBXGroup;");
				WriteDestFileLn("name = Resources;");
				WriteAPBQuotedField("path", "");
				WriteDestFileLn("refType = 4;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoLibraries);
				WriteAPBBgnObjList("children");
					WriteAPBobjlistelm(APBoclsLibs, APBliboCarbonRf);
					WriteAPBobjlistelm(APBoclsLibs, APBliboLibStdcRf);
				WriteAPBEndObjList();
				WriteDestFileLn("isa = PBXGroup;");
				WriteAPBQuotedField("name", "External Frameworks and Libraries");
				WriteAPBQuotedField("path", "");
				WriteDestFileLn("refType = 4;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoProducts);
				WriteAPBBgnObjList("children");
					WriteAPBobjlistelm(APBoclsGen, APBgenoProductRef);
				WriteAPBEndObjList();
				WriteDestFileLn("isa = PBXGroup;");
				WriteDestFileLn("name = Products;");
				WriteDestFileLn("refType = 4;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoMainGroup);
				WriteAPBBgnObjList("children");
					WriteAPBobjlistelm(APBoclsGen, APBgenoSources);
					WriteAPBobjlistelm(APBoclsGen, APBgenoResources);
					WriteAPBobjlistelm(APBoclsGen, APBgenoLibraries);
					WriteAPBobjlistelm(APBoclsGen, APBgenoProducts);
				WriteAPBEndObjList();
				WriteDestFileLn("isa = PBXGroup;");

				WriteBgnDestFileLn();
				WriteCStrToOutput("name = ");
				WriteQuoteToDestFile();
				WriteAppVariationStr();
				WriteQuoteToDestFile();
				WriteCStrToOutput(";");
				WriteEndDestFileLn();

				WriteAPBQuotedField("path", "");
				WriteDestFileLn("refType = 4;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoHeaders);
				WriteDestFileLn("buildActionMask = 2147483647;");
				WriteDestFileLn("files = (");
				WriteDestFileLn(");");
				WriteDestFileLn("isa = PBXHeadersBuildPhase;");
				WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoBunRsrcs);
				WriteDestFileLn("buildActionMask = 2147483647;");
				WriteAPBBgnObjList("files");
					WriteAPBobjlistelm(APBoclsIcns, 1);
					DocTypeCounter = 0;
					DoAllDocTypes(WriteDocTypeAPBaddBld);
				WriteAPBEndObjList();
				WriteDestFileLn("isa = PBXResourcesBuildPhase;");
				WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoPhaseSrcs);
				WriteDestFileLn("buildActionMask = 2147483647;");
				WriteAPBBgnObjList("files");
					FileCounter = 0;
					DoAllSrcFiles(DoSrcFileAPBaddBld);
				WriteAPBEndObjList();
				WriteDestFileLn("isa = PBXSourcesBuildPhase;");
				WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoPhaseLibs);
				WriteDestFileLn("buildActionMask = 2147483647;");
				WriteAPBBgnObjList("files");
					WriteAPBobjlistelm(APBoclsLibs, APBliboCarbonBld);
					WriteAPBobjlistelm(APBoclsLibs, APBliboLibStdcBld);
				WriteAPBEndObjList();
				WriteDestFileLn("isa = PBXFrameworksBuildPhase;");
				WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
			WriteAPBEndObject();
#if 0
			WriteAPBBeginObject(APBoclsGen, APBgenoPhaseRsrc);
				WriteDestFileLn("buildActionMask = 2147483647;");
				WriteAPBBgnObjList("files");
					WriteAPBobjlistelm(APBoclsGen, APBgenoMnRsrcBld);
				WriteAPBEndObjList();
				WriteDestFileLn("isa = PBXRezBuildPhase;");
				WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
			WriteAPBEndObject();
#endif
			WriteAPBBeginObject(APBoclsGen, APBgenoTarget);
				WriteAPBBgnObjList("buildPhases");
					WriteAPBobjlistelm(APBoclsGen, APBgenoHeaders);
					WriteAPBobjlistelm(APBoclsGen, APBgenoBunRsrcs);
					WriteAPBobjlistelm(APBoclsGen, APBgenoPhaseSrcs);
					WriteAPBobjlistelm(APBoclsGen, APBgenoPhaseLibs);
#if 0
					WriteAPBobjlistelm(APBoclsGen, APBgenoPhaseRsrc);
#endif
				WriteAPBEndObjList();
				WriteDestFileLn("buildSettings = {");
				++DestFileIndent;
					if (CurDbgLvl != kDbgLvlQuick) {
						WriteDestFileLn("DEBUGGING_SYMBOLS = NO;");
					}
					WriteAPBQuotedField("FRAMEWORK_SEARCH_PATHS", "");
					WriteDestFileLn("HEADER_SEARCH_PATHS = config;");
					WriteAPBQuotedField("LIBRARY_SEARCH_PATHS", "");
					WriteAPBQuotedField("OTHER_LDFLAGS", "");
					WriteAPBQuotedField("OTHER_REZFLAGS", "");
					WriteBgnDestFileLn();
					WriteCStrToOutput("PRODUCT_NAME = ");
					WriteCStrToOutput(kStrAppAbbrev);
					WriteCStrToOutput(";");
					WriteEndDestFileLn();
					WriteAPBQuotedField("SECTORDER_FLAGS", "");
					WriteAPBQuotedField("WARNING_CFLAGS", "-Wall -Wstrict-prototypes -Wno-uninitialized -Wno-four-char-constants -Wno-unknown-pragmas");
					WriteDestFileLn("WRAPPER_EXTENSION = app;");
				--DestFileIndent;
				WriteDestFileLn("};");
				WriteDestFileLn("dependencies = (");
				WriteDestFileLn(");");
				WriteDestFileLn("isa = PBXApplicationTarget;");

				WriteBgnDestFileLn();
				WriteCStrToOutput("name = ");
				WriteCStrToOutput(kStrAppAbbrev);
				WriteCStrToOutput(";");
				WriteEndDestFileLn();

				WriteAPBQuotedField("productName", kStrAppAbbrev);

				WriteAPBObjRef("productReference", APBoclsGen, APBgenoProductRef);

				WriteBgnDestFileLn();
				WriteCStrToOutput("productSettingsXML = ");
				WriteQuoteToDestFile();
				WriteCStrToOutput("<?xml version=");
				WriteBackSlashToDestFile();
				WriteQuoteToDestFile();
				WriteCStrToOutput("1.0");
				WriteBackSlashToDestFile();
				WriteQuoteToDestFile();
				WriteCStrToOutput(" encoding=");
				WriteBackSlashToDestFile();
				WriteQuoteToDestFile();
				WriteCStrToOutput("UTF-8");
				WriteBackSlashToDestFile();
				WriteQuoteToDestFile();
				WriteCStrToOutput("?>");
				WriteEndDestFileLn();

				WriteAPBplist();

			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoRoot);
				WriteAPBBgnObjList("buildStyles");
					WriteAPBobjlistelm(APBoclsGen, APBgenoBuildStyle);
				WriteAPBEndObjList();
				WriteDestFileLn("hasScannedForEncodings = 1;");
				WriteDestFileLn("isa = PBXProject;");
				WriteAPBObjRef("mainGroup", APBoclsGen, APBgenoMainGroup);
				WriteAPBQuotedField("projectDirPath", "");
				WriteAPBBgnObjList("targets");
					WriteAPBobjlistelm(APBoclsGen, APBgenoTarget);
				WriteAPBEndObjList();
			WriteAPBEndObject();
		--DestFileIndent;
		WriteDestFileLn("};");
		WriteAPBObjRef("rootObject", APBoclsGen, APBgenoRoot);
	--DestFileIndent;
	WriteDestFileLn("}");

	WriteCloseDestFile();
}

static void WriteIdeSpecificFiles(void)
{
	switch (CurIde) {
		case kIdeMPW3_6_a1:
			WriteMPWSpecificFiles();
			break;
		case kIdeMW8:
			WriteMetrowerksSpecificFiles();
			break;
		case kIdeBashGcc:
			WriteBashGccSpecificFiles();
			break;
		case kIdeRH7:
			WriteRH7SpecificFiles();
			break;
		case kIdeMSVC:
			WriteMSVCSpecificFiles();
			break;
		case kIdeLccW32:
			WriteLccW32SpecificFiles();
			break;
		case kIdeDevC:
			WriteDevCSpecificFiles();
			break;
		case kIdeAPB:
			WriteAPBSpecificFiles();
			break;
	}
}

int main(int argc, char *argv[])
{
	if (IsVersionQuerry(argc, argv)) {
		return 0;
	}
	if (! ProcessCommandLineArguments(argc, argv)) {
		return 1;
	}

	AutoChooseSettings();

	WriteScriptLangHeader();
	WriteHeaderComments();

	WriteStartScript();

	if (CurUseAbsolute) {
		WriteFindSourceDirectories();
	}
	WriteMakeOutputDirectories();
	WriteConfigFiles();
	if (CurPrintCFiles) {
		WriteCFilesList();
	}
	WriteIdeSpecificFiles();

	WriteEndScript();

	return 0;
}
