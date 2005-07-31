/*
	setup.c
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

#include <stdio.h>
#include <string.h>

#define blnr int
#define trueblnr 1
#define falseblnr 0

#define nullpr 0

#ifndef UseSetupOpt
#define UseSetupOpt 1
#endif

#if UseSetupOpt
#include "setupopt.h"
#endif

/* --- definition of options --- */

#ifndef IsVersionQuerry
#define IsVersionQuerry 0
#endif

#ifndef UseIdeMPW3_6_a1
#define UseIdeMPW3_6_a1 0
#endif
#if UseIdeMPW3_6_a1
#ifdef HaveSomeIde
#error "double IDE definition"
#else
#define HaveSomeIde 1
#endif
#endif

#ifndef UseIdeMW8
#define UseIdeMW8 0
#endif
#if UseIdeMW8
#ifdef HaveSomeIde
#error "double IDE definition"
#else
#define HaveSomeIde 1
#endif
#endif

#ifndef UseIdeBashGcc
#define UseIdeBashGcc 0
#endif
#if UseIdeBashGcc
#ifdef HaveSomeIde
#error "double IDE definition"
#else
#define HaveSomeIde 1
#endif
#endif

#ifndef UseIdeMSVC
#define UseIdeMSVC 0
#endif
#if UseIdeMSVC
#ifdef HaveSomeIde
#error "double IDE definition"
#else
#define HaveSomeIde 1
#endif
#endif

#ifndef UseIdeNMake
#define UseIdeNMake 0
#endif
#if UseIdeNMake
#ifdef HaveSomeIde
#error "double IDE definition"
#else
#define HaveSomeIde 1
#endif
#endif

#ifndef UseIdeNMakMSVC42
#define UseIdeNMakMSVC42 0
#endif
#if UseIdeNMakMSVC42
#ifdef HaveSomeIde
#error "double IDE definition"
#else
#define HaveSomeIde 1
#endif
#endif

#ifndef UseIdeLccW32
#define UseIdeLccW32 0
#endif
#if UseIdeLccW32
#ifdef HaveSomeIde
#error "double IDE definition"
#else
#define HaveSomeIde 1
#endif
#endif

#ifndef UseIdeDevC
#define UseIdeDevC 0
#endif
#if UseIdeDevC
#ifdef HaveSomeIde
#error "double IDE definition"
#else
#define HaveSomeIde 1
#endif
#endif

#ifndef UseIdeAPB
#define UseIdeAPB 0
#endif
#if UseIdeAPB
#ifdef HaveSomeIde
#error "double IDE definition"
#else
#define HaveSomeIde 1
#endif
#endif

#ifndef HaveSomeIde
#if ! IsVersionQuerry
#error "no IDE definition"
#endif
#endif

#ifndef UseTargMac68K
#define UseTargMac68K 0
#endif
#if UseTargMac68K
#ifdef HaveSomeTarg
#error "double Targ definition"
#else
#define HaveSomeTarg 1
#endif
#endif

#ifndef UseTargMac68KFPU
#define UseTargMac68KFPU 0
#endif
#if UseTargMac68KFPU
#ifdef HaveSomeTarg
#error "double Targ definition"
#else
#define HaveSomeTarg 1
#endif
#endif

#ifndef UseTargClassicPPC
#define UseTargClassicPPC 0
#endif
#if UseTargClassicPPC
#ifdef HaveSomeTarg
#error "double Targ definition"
#else
#define HaveSomeTarg 1
#endif
#endif

#ifndef UseTargCarbon
#define UseTargCarbon 0
#endif
#if UseTargCarbon
#ifdef HaveSomeTarg
#error "double Targ definition"
#else
#define HaveSomeTarg 1
#endif
#endif

#ifndef UseTargMacho
#define UseTargMacho 0
#endif
#if UseTargMacho
#ifdef HaveSomeTarg
#error "double Targ definition"
#else
#define HaveSomeTarg 1
#endif
#endif

#ifndef UseTargWinx86
#define UseTargWinx86 0
#endif
#if UseTargWinx86
#ifdef HaveSomeTarg
#error "double Targ definition"
#else
#define HaveSomeTarg 1
#endif
#endif

#ifndef UseTargMacX11
#define UseTargMacX11 0
#endif
#if UseTargMacX11
#ifdef HaveSomeTarg
#error "double Targ definition"
#else
#define HaveSomeTarg 1
#endif
#endif

#ifndef UseTargLinuxx86
#define UseTargLinuxx86 0
#endif
#if UseTargLinuxx86
#ifdef HaveSomeTarg
#error "double Targ definition"
#else
#define HaveSomeTarg 1
#endif
#endif

#ifndef HaveSomeTarg
#if ! IsVersionQuerry
#if UseIdeMPW3_6_a1 || UseIdeMW8
#undef UseTargMac68K
#define UseTargMac68K 1
#elif UseIdeBashGcc
#undef UseTargLinuxx86
#define UseTargLinuxx86 1
#elif UseIdeMW8 || UseIdeAPB
#undef UseTargMacho
#define UseTargMacho 1
#elif UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42 || UseIdeLccW32 || UseIdeDevC
#undef UseTargWinx86
#define UseTargWinx86 1
#else
#error "undefined for this IDE"
#endif /* IDE */
#endif
#endif /* HaveSomeTarg */

#ifndef UseDbgLvlQuick
#define UseDbgLvlQuick 0
#endif
#if UseDbgLvlQuick
#ifdef HaveSomeDbgLvl
#error "double DbgLvl definition"
#else
#define HaveSomeDbgLvl 1
#endif
#endif

#ifndef UseDbgLvlTest
#define UseDbgLvlTest 0
#endif
#if UseDbgLvlTest
#ifdef HaveSomeDbgLvl
#error "double DbgLvl definition"
#else
#define HaveSomeDbgLvl 1
#endif
#endif

#ifndef UseDbgLvlShip
#define UseDbgLvlShip 0
#endif
#if UseDbgLvlShip
#ifdef HaveSomeDbgLvl
#error "double DbgLvl definition"
#else
#define HaveSomeDbgLvl 1
#endif
#endif

#ifndef HaveSomeDbgLvl
#undef UseDbgLvlShip
#define UseDbgLvlShip 1
#endif

#ifndef UseScriptMPW
#define UseScriptMPW 0
#endif
#if UseScriptMPW
#ifdef HaveSomeScriptLang
#error "double Script definition"
#else
#define HaveSomeScriptLang 1
#endif
#endif

#ifndef UseScriptAppleScript
#define UseScriptAppleScript 0
#endif
#if UseScriptAppleScript
#ifdef HaveSomeScriptLang
#error "double Script definition"
#else
#define HaveSomeScriptLang 1
#endif
#endif

#ifndef UseScriptBash
#define UseScriptBash 0
#endif
#if UseScriptBash
#ifdef HaveSomeScriptLang
#error "double Script definition"
#else
#define HaveSomeScriptLang 1
#endif
#endif

#ifndef UseScriptXPCmdLn
#define UseScriptXPCmdLn 0
#endif
#if UseScriptXPCmdLn
#ifdef HaveSomeScriptLang
#error "double Script definition"
#else
#define HaveSomeScriptLang 1
#endif
#endif

#ifndef UseScriptVBScript
#define UseScriptVBScript 0
#endif
#if UseScriptVBScript
#ifdef HaveSomeScriptLang
#error "double Script definition"
#else
#define HaveSomeScriptLang 1
#endif
#endif

#ifndef HaveSomeScriptLang
#if ! IsVersionQuerry
#error "no ScriptLang definition"
#endif
#endif

#define UseCPUFam68K 0
#define UseCPUFamPPC 0
#define UseCPUFamX86 0

#if UseTargMac68K || UseTargMac68KFPU
#undef UseCPUFam68K
#define UseCPUFam68K 1
#elif UseTargClassicPPC || UseTargCarbon || UseTargMacho || UseTargMacX11
#undef UseCPUFamPPC
#define UseCPUFamPPC 1
#elif UseTargWinx86 || UseTargLinuxx86
#undef UseCPUFamX86
#define UseCPUFamX86 1
#endif

#ifndef TheUseAbsolute
#define TheUseAbsolute 1
#endif

#ifndef ThePrintCFiles
#define ThePrintCFiles 0
#endif

#ifndef AllowAsm
#define AllowAsm 1
#endif

#if AllowAsm && UseCPUFamPPC
#if ! (UseTargMacho && UseIdeMW8)
#define HaveAsm 1
#endif
#endif
#ifndef HaveAsm
#define HaveAsm 0
#endif

#ifndef UseOpenGL
#define UseOpenGL 0
#endif

/* --- end of definition of options --- */


/* --- names of options --- */

#if UseTargMac68K
#define CurTargetName "m68k"
#endif
#if UseTargMac68KFPU
#define CurTargetName "mfpu"
#endif
#if UseTargClassicPPC
#define CurTargetName "mppc"
#endif
#if UseTargCarbon
#define CurTargetName "carb"
#endif
#if UseTargMacho
#define CurTargetName "mach"
#endif
#if UseTargWinx86
#define CurTargetName "wx86"
#endif
#if UseTargMacX11
#define CurTargetName "mx11"
#endif
#if UseTargLinuxx86
#define CurTargetName "lx86"
#endif

#if UseDbgLvlQuick
#define CurDbgLvlName "d"
#endif
#if UseDbgLvlTest
#define CurDbgLvlName "t"
#endif
#if UseDbgLvlShip
#define CurDbgLvlName "s"
#endif

#if UseIdeMPW3_6_a1
#define CurIdeName "mpw"
#endif
#if UseIdeMW8
#define CurIdeName "mw8"
#endif
#if UseIdeBashGcc
#define CurIdeName "bgc"
#endif
#if UseIdeMSVC
#define CurIdeName "msv"
#endif
#if UseIdeNMake || UseIdeNMakMSVC42
#define CurIdeName "nmk"
#endif
#if UseIdeLccW32
#define CurIdeName "lcc"
#endif
#if UseIdeDevC
#define CurIdeName "dvc"
#endif
#if UseIdeAPB
#define CurIdeName "apb"
#endif

/* --- end of names of options --- */


/* --- output utilities --- */

#define WriteCharToOutput putchar

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


/* --- code specific to Scripting Language --- */

static void WriteScriptLangExtension(void)
{
#if UseScriptMPW
	WriteCStrToOutput(".mpw");
#endif
#if UseScriptAppleScript
	WriteCStrToOutput(".scpt");
#endif
#if UseScriptBash
	WriteCStrToOutput(".sh");
#endif
#if UseScriptVBScript
	WriteCStrToOutput(".vbs");
#endif
#if UseScriptXPCmdLn
	WriteCStrToOutput(".bat");
#endif
}

static void WriteScriptLangHeader(void)
{
#if UseScriptBash
	WriteLnCStrToOutput("#! /bin/bash");
	WriteEolToOutput();
#endif
#if UseScriptXPCmdLn
	WriteLnCStrToOutput("@echo off");
	WriteEolToOutput();
#endif
}

static void WriteSectionCommentDestFile(char * Description)
{
	WriteEolToOutput();
	WriteEolToOutput();
#if UseScriptMPW || UseScriptBash
	{
		WriteCStrToOutput("# ----- ");
		WriteCStrToOutput(Description);
		WriteCStrToOutput(" -----");
	}
#endif
#if UseScriptAppleScript
	{
		WriteCStrToOutput("\t--- ");
		WriteCStrToOutput(Description);
		WriteCStrToOutput(" -----");
	}
#endif
#if UseScriptVBScript
	{
		WriteCStrToOutput("' ----- ");
		WriteCStrToOutput(Description);
		WriteCStrToOutput(" -----");
	}
#endif
#if UseScriptXPCmdLn
	{
		WriteCStrToOutput("rem ----- ");
		WriteCStrToOutput(Description);
		WriteCStrToOutput(" -----");
	}
#endif

	WriteEolToOutput();
}

#ifndef MPWOneEchoPerFile
#define MPWOneEchoPerFile 1
#endif

static void WriteOpenDestFile(char *DirVar, char *FileName, char *FileExt, char * Description)
{
	WriteSectionCommentDestFile(Description);

	WriteEolToOutput();

#if UseScriptMPW
	{
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
	}
#endif
#if UseScriptAppleScript
	{
		WriteCStrToOutput("\tset DestFile to open for access file (");
		WriteCStrToOutput(DirVar);
		WriteCStrToOutput(" & \"");
		WriteCStrToOutput(FileName);
		WriteCStrToOutput(FileExt);
		WriteCStrToOutput("\") with write permission");
		WriteEolToOutput();
		WriteEolToOutput();
		WriteLnCStrToOutput("\tset eof DestFile to 0");
	}
#endif
#if UseScriptBash
	{
		WriteCStrToOutput("DestFile=\"${");
		WriteCStrToOutput(DirVar);
		WriteCStrToOutput("}");
		WriteCStrToOutput(FileName);
		WriteCStrToOutput(FileExt);
		WriteCStrToOutput("\"");
		WriteEolToOutput();
		WriteLnCStrToOutput("echo -n > \"${DestFile}\"");
		WriteEolToOutput();
	}
#endif
#if UseScriptXPCmdLn
	{
		WriteCStrToOutput("set DestFile=%");
		WriteCStrToOutput(DirVar);
		WriteCStrToOutput("%");
		WriteCStrToOutput("\\");
		WriteCStrToOutput(FileName);
		WriteCStrToOutput(FileExt);
		WriteEolToOutput();
		WriteLnCStrToOutput("echo.>\"%DestFile%\"");
		WriteLnCStrToOutput("del \"%DestFile%\"");
	}
#endif
#if UseScriptVBScript
	{
		WriteCStrToOutput("Set f = fso.CreateTextFile(");
		WriteCStrToOutput(DirVar);
		WriteCStrToOutput(" & \"\\");
		WriteCStrToOutput(FileName);
		WriteCStrToOutput(FileExt);
		WriteCStrToOutput("\", True)");
		WriteEolToOutput();
		WriteEolToOutput();
	}
#endif
}

static void WriteCloseDestFile(void)
{
#if UseScriptMPW
#if MPWOneEchoPerFile
	WriteLnCStrToOutput("''");
#endif
#endif /* UseScriptMPW */
#if UseScriptAppleScript
	WriteEolToOutput();
	WriteLnCStrToOutput("\tclose access DestFile");
#endif
#if UseScriptBash
#endif
#if UseScriptVBScript
	WriteEolToOutput();
	WriteLnCStrToOutput("f.Close");
#endif
#if UseScriptXPCmdLn
#endif
}

static void WriteBlankLineToDestFile(void)
{
#if UseScriptMPW
#if MPWOneEchoPerFile
	WriteLnCStrToOutput("''\266n\266");
#else
	WriteLnCStrToOutput("Echo '' >> \"{DestFile}\"");
#endif
#endif /* UseScriptMPW */
#if UseScriptAppleScript
	WriteLnCStrToOutput("\twrite \"\" & return to DestFile");
#endif
#if UseScriptBash
	WriteLnCStrToOutput("echo '' >> \"${DestFile}\"");
#endif
#if UseScriptVBScript
	WriteLnCStrToOutput("f.WriteLine(\"\")");
#endif
#if UseScriptXPCmdLn
	WriteLnCStrToOutput("echo.>>\"%DestFile%\"");
#endif
}

static int DestFileIndent = 0;

static void WriteBgnDestFileLn(void)
{
	int i;

#if UseScriptMPW
#if MPWOneEchoPerFile
	WriteCStrToOutput("'");
#else
	WriteCStrToOutput("Echo '");
#endif
#endif /* UseScriptMPW */
#if UseScriptAppleScript
	WriteCStrToOutput("\twrite \"");
#endif
#if UseScriptBash
	WriteCStrToOutput("echo '");
#endif
#if UseScriptVBScript
	WriteCStrToOutput("f.WriteLine(\"");
#endif
#if UseScriptXPCmdLn
	WriteCStrToOutput("echo ");
#endif

	for (i = 0; i < DestFileIndent; ++i) {
		WriteCStrToOutput("\t");
	}
}

static void WriteEndDestFileLn(void)
{
#if UseScriptMPW
#if MPWOneEchoPerFile
	WriteCStrToOutput("'\266n\266");
#else
	WriteCStrToOutput("' >> \"{DestFile}\"");
#endif
#endif /* UseScriptMPW */
#if UseScriptAppleScript
	WriteCStrToOutput("\" & return to DestFile");
#endif
#if UseScriptBash
	WriteCStrToOutput("' >> \"${DestFile}\"");
#endif
#if UseScriptVBScript
	WriteCStrToOutput("\")");
#endif
#if UseScriptXPCmdLn
	WriteCStrToOutput(">>\"%DestFile%\"");
#endif

	WriteEolToOutput();
}

static void WriteCStrToDestFile(char *s)
{
	char c;

	while ((c = *s++) != 0) {
		switch (c) {
#if UseScriptMPW
			case '\'':
				WriteCStrToOutput("'\266''");
				break;
#endif
#if UseScriptBash
			case '\'':
				WriteCStrToOutput("'\\''");
				break;
#endif
#if UseScriptAppleScript
			case '"':
				WriteCStrToOutput("\\\"");
				break;
#endif
#if UseScriptVBScript
			case '"':
				WriteCStrToOutput("\"\"");
				break;
#endif
#if UseScriptXPCmdLn
			case '%':
				WriteCStrToOutput("%%");
				break;
			case '^':
			case '<':
			case '>':
			case '|':
			case '"':

				/*
					digit preceeding redirection
					modifies the redirection
				*/
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				WriteCStrToOutput("^");
				WriteCharToOutput(c);
				break;
#endif
			default:
				WriteCharToOutput(c);
				break;
		}
	}
}

static void WriteScriptVarToDestFile(char *name)
{
#if UseScriptMPW
	WriteCStrToOutput("'\"{");
	WriteCStrToOutput(name);
	WriteCStrToOutput("}\"'");
#endif
#if UseScriptAppleScript
	WriteCStrToOutput("* not implemented yet *");
#endif
#if UseScriptBash
	WriteCStrToOutput("'\"${");
	WriteCStrToOutput(name);
	WriteCStrToOutput("}\"'");
#endif
#if UseScriptVBScript
	WriteCStrToOutput("\" & ");
	WriteCStrToOutput(name);
	WriteCStrToOutput(" & \"");
#endif
#if UseScriptXPCmdLn
	WriteCStrToOutput("%");
	WriteCStrToOutput(name);
	WriteCStrToOutput("%");
#endif
}

static void WriteBgnCommentBlock(void)
{
#if UseScriptAppleScript
	WriteLnCStrToOutput("(*");
#endif
}

static void WriteEndCommentBlock(void)
{
#if UseScriptAppleScript
	WriteLnCStrToOutput("*)");
#endif
}

static void WriteBgnCommentBlockLn(void)
{
#if UseScriptMPW || UseScriptBash
	WriteCStrToOutput("# ");
#endif
#if UseScriptAppleScript
	WriteCStrToOutput("\t");
#endif
#if UseScriptVBScript
	WriteCStrToOutput("' ");
#endif
#if UseScriptXPCmdLn
	WriteCStrToOutput("rem ");
#endif
}

static void WriteEndCommentBlockLn(void)
{
#if UseScriptMPW || UseScriptAppleScript || UseScriptBash || UseScriptVBScript || UseScriptXPCmdLn
	WriteEolToOutput();
#endif
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

#if UseScriptMPW
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
#endif
#if UseScriptAppleScript
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
#endif
#if UseScriptBash
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
#endif
#if UseScriptVBScript
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
#endif
#if UseScriptXPCmdLn
	WriteCStrToOutput("set ");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("=%");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput("%");
	WriteCStrToOutput("\\");
	WriteCStrToOutput(name);
	WriteEolToOutput();

	WriteCStrToOutput("if not exist \"%");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("%\" echo \"%");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("%\" is missing");
	WriteEolToOutput();
#endif
}

static void MakeSubDirectory(char *new_d, char *parent_d, char *name, char *FileExt)
{
	WriteEolToOutput();

#if UseScriptMPW
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
#endif
#if UseScriptAppleScript
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
#endif
#if UseScriptBash
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
#endif
#if UseScriptVBScript
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
#endif
#if UseScriptXPCmdLn
	WriteCStrToOutput("set ");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("=%");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput("%");
	WriteCStrToOutput("\\");
	WriteCStrToOutput(name);
	WriteCStrToOutput(FileExt);
	WriteEolToOutput();

	WriteCStrToOutput("if not exist \"%");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("%\" mkdir \"%");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("%\"");
	WriteEolToOutput();
#endif
}

#if 0
static void WriteCopyFile(char *old_d, char *name, char *new_d)
{
#if UseScriptMPW || UseScriptVBScript
	WriteLnCStrToOutput("* not implemented yet *");
#endif
#if UseScriptAppleScript
	WriteCStrToOutput("\tduplicate (alias (");
	WriteCStrToOutput(old_d);
	WriteCStrToOutput(" & \"");
	WriteCStrToOutput(name);
	WriteCStrToOutput("\")) to (alias ");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput(")");
	WriteEolToOutput();
#endif
#if UseScriptBash
	WriteCStrToOutput("cp \"${");
	WriteCStrToOutput(old_d);
	WriteCStrToOutput("}");
	WriteCStrToOutput(name);
	WriteCStrToOutput("\" \"${");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("}\"");
	WriteEolToOutput();
#endif
}
#endif

static void WriteStartScript(void)
{
	WriteEolToOutput();

#if UseScriptMPW

#if ! TheUseAbsolute
	{
		WriteLnCStrToOutput("IF {#} != 1");
		WriteLnCStrToOutput("\tEcho 'usage:'");
		WriteLnCStrToOutput("\tEcho 'setup.mpw my_output_d'");
		WriteLnCStrToOutput("\tExit 1");
		WriteLnCStrToOutput("END");
	}
#else
	{
		WriteLnCStrToOutput("IF {#} != 2");
		WriteLnCStrToOutput("\tEcho 'usage:'");
		WriteLnCStrToOutput("\tEcho 'setup.mpw my_output_d my_source_d'");
		WriteLnCStrToOutput("\tExit 1");
		WriteLnCStrToOutput("END");
	}
#endif

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
#if TheUseAbsolute
	{
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
#endif /* TheUseAbsolute */
#endif /* UseScriptMPW */
#if UseScriptAppleScript
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
#if TheUseAbsolute
	{
		WriteEolToOutput();

		WriteCStrToOutput("\tchoose folder with prompt (\"Please find the source folder.\")");
		WriteEolToOutput();

		WriteLnCStrToOutput("\tset my_source_d to result as text");
	}
#endif
#endif /* UseScriptAppleScript */
#if UseScriptBash
	WriteSectionCommentDestFile("get arguments");
	WriteEolToOutput();
#if ! TheUseAbsolute
	{
		WriteLnCStrToOutput("if test $# -ne 1 ; then");
		WriteLnCStrToOutput("\techo \"usage:\" >&2");
		WriteLnCStrToOutput("\techo \"setup.sh my_output_d\" >&2");
		WriteLnCStrToOutput("\texit 1");
		WriteLnCStrToOutput("fi");
	}
#else
	{
		WriteLnCStrToOutput("if test $# -ne 2 ; then");
		WriteLnCStrToOutput("\techo \"usage:\" >&2");
		WriteLnCStrToOutput("\techo \"setup.sh my_output_d my_source_d\" >&2");
		WriteLnCStrToOutput("\texit 1");
		WriteLnCStrToOutput("fi");
	}
#endif
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
#if TheUseAbsolute
	{
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
#endif
#endif /* UseScriptBash */
#if UseScriptVBScript
	WriteLnCStrToOutput("wscript.echo \"Starting\"");
	WriteEolToOutput();
	WriteLnCStrToOutput("dim f, fso, WshShell");
	WriteLnCStrToOutput("set fso = CreateObject(\"Scripting.FileSystemObject\")");
	WriteLnCStrToOutput("set WshShell = CreateObject(\"WScript.Shell\")");
	WriteEolToOutput();
	WriteLnCStrToOutput("dim my_args");
	WriteLnCStrToOutput("Set my_args = WScript.Arguments");
#if ! TheUseAbsolute
	{
		WriteLnCStrToOutput("if (my_args.Count <> 1) then");
		WriteLnCStrToOutput("\twscript.echo \"usage:\"");
		WriteLnCStrToOutput("\twscript.echo \"setup.scpt my_output_d\"");
		WriteLnCStrToOutput("\twscript.quit");
		WriteLnCStrToOutput("end if");
	}
#else
	{
		WriteLnCStrToOutput("if (my_args.Count <> 2) then");
		WriteLnCStrToOutput("\twscript.echo \"usage:\"");
		WriteLnCStrToOutput("\twscript.echo \"setup.scpt my_output_d my_source_d\"");
		WriteLnCStrToOutput("\twscript.quit");
		WriteLnCStrToOutput("end if");
	}
#endif
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
#if TheUseAbsolute
	{
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
#endif
#endif /* UseScriptVBScript */

#if UseScriptXPCmdLn
	WriteLnCStrToOutput("setlocal");
	WriteLnCStrToOutput("if not exist \"%1\" mkdir \"%1\"");

	WriteLnCStrToOutput("set save_pwd=%cd%");
	WriteLnCStrToOutput("cd \"%1\"");
	WriteLnCStrToOutput("set my_output_d=%cd%");
#if TheUseAbsolute
	{
		WriteLnCStrToOutput("cd \"%2\"");
		WriteLnCStrToOutput("set my_source_d=%cd%");
	}
#endif
	WriteLnCStrToOutput("cd \"%save_pwd%\"");
#endif /* UseScriptXPCmdLn */
}

static void WriteEndScript(void)
{
#if UseScriptMPW
#endif
#if UseScriptAppleScript
	WriteEolToOutput();
	WriteLnCStrToOutput("end tell");
#endif
#if UseScriptBash
#endif
#if UseScriptVBScript
	WriteEolToOutput();
	WriteLnCStrToOutput("wscript.echo \"Finished\"");
#endif
#if UseScriptXPCmdLn
	WriteEolToOutput();
	WriteLnCStrToOutput("endlocal");
#endif
}

/* --- end of code specific to Scripting Language --- */

static void WriteDestFileLn(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteEndDestFileLn();
}

static void WriteQuoteToDestFile(void)
{
	WriteCStrToDestFile("\"");
}

static void WriteSingleQuoteToDestFile(void)
{
	WriteCStrToDestFile("'");
}

static void WriteBackSlashToDestFile(void)
{
	WriteCStrToDestFile("\\");
}

/* get information about the current program */

typedef void (*tDoOneExtraHeader)(char *s);

typedef void (*tDoOneDepends)(char *s);

typedef void (*tDoDependsForC)(tDoOneDepends p);

typedef void (*tDoOneCFile)(char *s, blnr AsmAvail, tDoDependsForC depends);

typedef void (*tWriteOneExtension)(char *s);

typedef void (*tWriteExtensionList)(tWriteOneExtension p);

typedef void (*tWriteOneDocType)(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList);

#include "setup.h"

#if IsVersionQuerry
static void WriteVersionQuerry(void)
{
	WriteCStrToOutput(kStrAppAbbrev);
	WriteCStrToOutput("-");
	WriteCStrToOutput(kMajorVersion);
	WriteCStrToOutput(".");
	WriteCStrToOutput(kMinorVersion);
	WriteCStrToOutput(".");
	WriteCStrToOutput(kMinorSubVersion);
	WriteEolToOutput();
}
#endif

static void WriteVersionStr(void)
{
	WriteCStrToDestFile(kMajorVersion);
	WriteCStrToDestFile(".");
	WriteCStrToDestFile(kMinorVersion);
	WriteCStrToDestFile(".");
	WriteCStrToDestFile(kMinorSubVersion);
}

#if ! IsVersionQuerry
static void WriteAppVariationStr(void)
{
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile("-");
	WriteVersionStr();
	WriteCStrToDestFile("-");
	WriteCStrToDestFile(CurIdeName);
	WriteCStrToDestFile(CurTargetName);
	WriteCStrToDestFile(CurDbgLvlName);
}
#endif

#if ! IsVersionQuerry
static void WriteGetInfoString(void)
{
	WriteAppVariationStr();
	WriteCStrToDestFile(", Copyright ");
	WriteCStrToDestFile(kStrCopyrightYear);
	WriteCStrToDestFile(" maintained by ");
	WriteCStrToDestFile(kMaintainerName);
	WriteCStrToDestFile(".");
}
#endif

static void WriteHeaderComments(void)
{
	WriteBgnCommentBlock();

	WriteBgnCommentBlockLn();
	WriteCStrToOutput("setup");
	WriteScriptLangExtension();
	WriteEndCommentBlockLn();

	WriteCommentBlockLn("");

	WriteCommentBlockLn("This file was automatically generated from setup.c");

	WriteEndCommentBlock();
}

static void WriteFindSourceDirectories(void)
{
	WriteSectionCommentDestFile("find source directories");

	FindSubDirectory("my_c_src_d", "my_source_d", "c_src");
#if HaveAsm
		FindSubDirectory("my_a_src_d", "my_source_d", "a_src");
#endif
	FindSubDirectory("my_platform_d", "my_source_d", "platform");
#if UseTargWinx86
	FindSubDirectory("my_rz_src_d", "my_platform_d", "win");
#else
	FindSubDirectory("my_rz_src_d", "my_platform_d", "mac");
#if UseTargMacho
		FindSubDirectory("my_osxrz_d", "my_rz_src_d", "osx");
#endif
#endif /* ! UseTargWinx86 */
}

static void WriteMakeOutputDirectories(void)
{
	WriteSectionCommentDestFile("make output directories");

	MakeSubDirectory("my_config_d", "my_output_d", "config", "");
#if UseIdeAPB
	MakeSubDirectory("my_proj_d", "my_output_d", kStrAppAbbrev, ".pbproj");
#elif ! UseIdeMW8
	MakeSubDirectory("my_obj_d", "my_output_d", "obj", "");

	WriteOpenDestFile("my_obj_d", "dummy", "", "Dummy");
	WriteDestFileLn("This file is here because some archive extraction");
	WriteDestFileLn("software will not create an empty directory.");
	WriteCloseDestFile();
#endif
}

/* --- XML utilities --- */

typedef void (*MyProc)(void);

static void WriteXMLtagBegin(char *s)
{
	WriteCStrToDestFile("<");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(">");
}

static void WriteXMLtagEnd(char *s)
{
	WriteCStrToDestFile("</");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(">");
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
	WriteCStrToDestFile(v);
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

#if UseIdeMW8
static void WriteWrongCNFGGLOB(void)
{
	WriteDestFileLn("#error \"wrong CNFGGLOB.h\"");
}
#endif

#if UseIdeMW8
static void WriteCheckPreDef(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#ifndef ");
	WriteCStrToDestFile(s);
	WriteEndDestFileLn();
	WriteWrongCNFGGLOB();
	WriteDestFileLn("#endif");
}
#endif

#if UseIdeMW8
static void WriteCheckPreNDef(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#ifdef ");
	WriteCStrToDestFile(s);
	WriteEndDestFileLn();
	WriteWrongCNFGGLOB();
	WriteDestFileLn("#endif");
}
#endif

#if UseIdeBashGcc
#ifndef UsingAppleTools
#define UsingAppleTools (UseTargMacho || UseTargMacX11)
#endif
#endif

#ifndef MySoundEnabled
#if UseTargMac68K || UseTargMac68KFPU || UseTargClassicPPC || UseTargCarbon || UseTargMacho || UseTargWinx86
#define MySoundEnabled 1
#else
#define MySoundEnabled 0
#endif
#endif

#if ! IsVersionQuerry
static void WriteCommonCNFGGLOB(void)
{
	WriteOpenDestFile("my_config_d", "CNFGGLOB", ".h", "C Configuration file");

#if UseIdeMW8
	WriteDestFileLn("/* make sure this is correct CNFGGLOB */");

	WriteCheckPreDef("__MWERKS__");

#if UseCPUFam68K
	{
		WriteCheckPreDef("__MC68K__");
#if UseTargMac68KFPU
		WriteCheckPreDef("__MC68881__");
#else
		WriteCheckPreNDef("__MC68881__");
#endif
	}
#endif /* UseCPUFam68K */
#if UseCPUFamPPC
		WriteCheckPreDef("__POWERPC__");
#endif
#if UseCPUFamX86
		WriteCheckPreDef("__INTEL__");
#endif

#endif /* UseIdeMW8 */

	WriteBlankLineToDestFile();

#if UseCPUFam68K || UseCPUFamPPC
		WriteDestFileLn("#define BigEndianUnaligned 1");
#endif

#if UseCPUFam68K
		WriteDestFileLn("#define HaveCPUfamM68K 1");
#endif

#if UseIdeMW8
	WriteDestFileLn("#define MayInline __inline__");
#elif UseIdeNMakMSVC42
	WriteDestFileLn("#define MayInline __inline");
#elif UseIdeMSVC || UseIdeNMake
	WriteDestFileLn("#define MayInline __forceinline");
#endif

#if UseIdeBashGcc || UseIdeAPB
	WriteBlankLineToDestFile();
	WriteDestFileLn("#define cIncludeUnused 0");
#elif UseIdeLccW32
	WriteBlankLineToDestFile();
	WriteDestFileLn("#define cIncludeUnused 0");
	WriteDestFileLn("#define UnusedParam(x)");
#elif UseIdeDevC
	WriteBlankLineToDestFile();
	WriteDestFileLn("#define cIncludeUnused 0");
#endif

#if ! UseDbgLvlShip
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define Debug 1");
#endif

	WriteBlankLineToDestFile();
#if UseTargMac68K || UseTargMac68KFPU || UseTargClassicPPC || UseTargCarbon || UseTargMacho
	WriteDestFileLn("#define MacTarget 1");
#elif UseTargWinx86
	WriteDestFileLn("#define WinTarget 1");
#elif UseTargMacX11 || UseTargLinuxx86
	WriteDestFileLn("#define XWnTarget 1");
#else
#error "undefined for this Target"
#endif

#if MySoundEnabled
	WriteDestFileLn("#define MySoundEnabled 1");
#endif
#if UseOpenGL
	WriteDestFileLn("#define UseOpenGL 1");
#endif

	WriteAppSpecificCNFGGLOBoptions();

	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kStrAppName \"");
	WriteCStrToDestFile(kStrAppName);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kAppVariationStr ");
	WriteQuoteToDestFile();
	WriteAppVariationStr();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kStrCopyrightYear ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrCopyrightYear);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kMaintainerName ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kMaintainerName);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kStrHomePage ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrHomePage);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

#if UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42
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
#endif
#if UseIdeMW8
#if ! UseDbgLvlQuick
		WriteBlankLineToDestFile();
		WriteDestFileLn("#ifdef OptForSpeed");
		WriteDestFileLn("#pragma optimize_for_size off");
		WriteDestFileLn("#endif");
#endif
#endif

	WriteBlankLineToDestFile();

	WriteCloseDestFile();
}
#endif

static void WriteCommonCNFGRAPI(void)
{
	WriteOpenDestFile("my_config_d", "CNFGRAPI", ".h", "C API Configuration file");

#if UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42
	WriteDestFileLn("/* ignore warning generated by system includes */");
#if ! UseIdeNMakMSVC42
	WriteDestFileLn("#pragma warning(push)");
#endif
	WriteDestFileLn("#pragma warning(disable : 4201 4115 4214)");
	WriteBlankLineToDestFile();
#endif

#if UseTargMacho
	{
		/* kIdeMW8 or kIdeBashGcc or kIdeAPB */
#if UseIdeMW8
		WriteDestFileLn("#include <MSL MacHeadersMach-O.h>");
#endif
		WriteDestFileLn("#include <Carbon/Carbon.h>");
#if UseOpenGL
		WriteDestFileLn("#include <AGL/agl.h>");
#endif
	}
#elif UseTargMacX11 || UseTargLinuxx86
	{
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
#if MySoundEnabled
		WriteDestFileLn("#include <alsa/asoundlib.h>");
#endif
	}
#elif UseTargCarbon
	{
		/* kIdeMW8 or kIdeMPW3_6_a1 */
#if UseIdeMW8
		WriteDestFileLn("#include <MacHeadersCarbon.h>");
#else
		WriteDestFileLn("#include <Carbon.h>");
#if UseOpenGL
		WriteDestFileLn("#include <agl.h>");
#endif
#endif
	}
#elif UseTargWinx86
	{
#if UseIdeMW8
		WriteDestFileLn("#include <Win32Headers.h>");
#else
		WriteDestFileLn("#include <windows.h>");
		WriteDestFileLn("#include <shlobj.h>");
		WriteDestFileLn("#include <time.h>");
#if UseIdeLccW32
		WriteDestFileLn("#include <shellapi.h>");
		WriteDestFileLn("#include <mmsystem.h>");
#endif
#endif
	}
#else
	{
#if UseIdeMW8
		{
			WriteDestFileLn("#include <MacHeaders.h>");
			WriteDestFileLn("#include <CursorDevices.h>");
			WriteDestFileLn("#define ShouldDefineQDGlobals 0");
		}
#elif UseIdeMPW3_6_a1
		{
			WriteDestFileLn("#include <MacTypes.h>");
#if ! UseCPUFam68K
				WriteDestFileLn("#include <MixedMode.h>");
#endif
			WriteDestFileLn("#include <Gestalt.h>");
			WriteDestFileLn("#include <MacErrors.h>");
			WriteDestFileLn("#include <MacMemory.h>");
			WriteDestFileLn("#include <OSUtils.h>");
			WriteDestFileLn("#include <QuickdrawText.h>");
			WriteDestFileLn("#include <QuickDraw.h>");
#if UseCPUFam68K
				WriteDestFileLn("#include <SegLoad.h>");
#endif
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
#endif
	}
#endif /* Target */

#if UseIdeMSVC || UseIdeNMake
	WriteBlankLineToDestFile();
	WriteDestFileLn("/* restore warnings */");
	WriteDestFileLn("#pragma warning(pop)");
#endif

#if UseCPUFam68K
#if UseIdeMPW3_6_a1
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define ShouldUnloadDataInit 1");
		WriteDestFileLn("#define Windows85APIAvail 0");
#endif
#endif

#if UseTargMacho
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define MainReturnsInt 1");
#endif
#if UseTargMacho || (UseTargCarbon && UseIdeMPW3_6_a1)
		WriteDestFileLn("#define MyAppIsBundle 1");
#endif
#if UseTargMac68K || UseTargMac68KFPU || UseTargClassicPPC || UseTargCarbon || UseTargMacho
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kMacCreatorSig ");
	WriteSingleQuoteToDestFile();
	WriteCStrToDestFile(kMacCreatorSig);
	WriteSingleQuoteToDestFile();
	WriteEndDestFileLn();
#endif
	WriteCloseDestFile();
}

#if UseTargMac68K || UseTargMac68KFPU || UseTargClassicPPC || (UseTargCarbon && ! UseIdeMPW3_6_a1)
#define HaveMacRrscs 1
#else
#define HaveMacRrscs 0
#endif

#if HaveMacRrscs
static void WriteQuotedInclude(char *name)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#include ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(name);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}
#endif

#if HaveMacRrscs
static void WriteBeginResResource(char *types, int id)
{
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("resource '");
	WriteCStrToDestFile(types);
	WriteSingleQuoteToDestFile();
	WriteCStrToDestFile(" (");
	WriteUnsignedToOutput(id);
	WriteCStrToDestFile(") {");
	WriteEndDestFileLn();
	++DestFileIndent;
}
#endif

#if HaveMacRrscs
static void WriteEndResResource(void)
{
	--DestFileIndent;
	WriteDestFileLn("};");
}
#endif

#if HaveMacRrscs
static void WriteResTypeComma(char *types)
{
	WriteBgnDestFileLn();
	WriteSingleQuoteToDestFile();
	WriteCStrToDestFile(types);
	WriteSingleQuoteToDestFile();
	WriteCStrToDestFile(",");
	WriteEndDestFileLn();
}
#endif

#if HaveMacRrscs
static void WriteAFREFres(char *types, int i)
{
	WriteBeginResResource("FREF", 128 + i);
		WriteResTypeComma(types);

		WriteBgnDestFileLn();
		WriteUnsignedToOutput(i);
		WriteCStrToDestFile(",");
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteQuoteToDestFile();
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
	WriteEndResResource();
}
#endif

#if HaveMacRrscs || UseIdeAPB
static unsigned int DocTypeCounter;
#endif

#ifndef UnusedParam
#define UnusedParam(p) (void) p
#endif

#if HaveMacRrscs
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
	WriteCStrToDestFile("#define ");
	WriteCStrToDestFile(ShortName);
	WriteCStrToDestFile("IconId ");
	WriteUnsignedToOutput(128 + (++DocTypeCounter));
	WriteEndDestFileLn();
}
#endif

#if HaveMacRrscs
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
#endif

#if HaveMacRrscs
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
	WriteCStrToDestFile(",");
	WriteUnsignedToOutput(DocTypeCounter);
	WriteCStrToDestFile(", ");
	WriteUnsignedToOutput(128 + DocTypeCounter);
	WriteEndDestFileLn();
}
#endif

#if HaveMacRrscs
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
	WriteCStrToDestFile(",");
	WriteUnsignedToOutput(++DocTypeCounter);
	WriteCStrToDestFile(", ");
	WriteCStrToDestFile(ShortName);
	WriteCStrToDestFile("IconId");
	WriteEndDestFileLn();
}
#endif

#if HaveMacRrscs
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
	WriteCStrToDestFile("#include ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(ShortName);
	WriteCStrToDestFile("Icon.r");
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}
#endif

#if HaveMacRrscs
static void WriteCommonCNFGRSRC(void)
{
	WriteOpenDestFile("my_config_d", "CNFGRSRC", ".h", "Resource Configuration file");

	WriteBlankLineToDestFile();

#if UseIdeMW8
	{
#if UseTargMacho
			WriteDestFileLn("#include <Carbon/Carbon.r>");
#else
			WriteQuotedInclude("Types.r");
			WriteQuotedInclude("Icons.r");
#endif
	}
#elif UseIdeBashGcc || UseIdeAPB
	WriteQuotedInclude("Carbon.r");
#elif UseIdeMPW3_6_a1
	WriteQuotedInclude("Types.r");
	WriteQuotedInclude("Icons.r");
#endif

	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kStrAppName ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrAppName);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kAppVariationStr ");
	WriteQuoteToDestFile();
	WriteAppVariationStr();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kStrCopyrightYear ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrCopyrightYear);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kStrHomePage ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrHomePage);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

#if ! UseTargMacho
	{
#if UseTargCarbon
		{
			WriteBlankLineToDestFile();
			WriteDestFileLn("data 'plst' (0) {");
			++DestFileIndent;
				WriteDestFileLn("$\"00\"");
			--DestFileIndent;
			WriteDestFileLn("};");
		}
#endif

		WriteBlankLineToDestFile();
		WriteBgnDestFileLn();
		WriteCStrToDestFile("data '");
		WriteCStrToDestFile(kMacCreatorSig);
		WriteCStrToDestFile("' (0, \"Owner resource\") {");
		WriteEndDestFileLn();
		++DestFileIndent;
			WriteDestFileLn("$\"00\"");
		--DestFileIndent;
		WriteDestFileLn("};");

		WriteBeginResResource("vers", 1);
			WriteBgnDestFileLn();
			WriteCStrToDestFile(kMajorVersion);
			WriteCStrToDestFile(",");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile(kMinorVersion);
			WriteCStrToDestFile(" * 16 + ");
			WriteCStrToDestFile(kMinorSubVersion);
			WriteCStrToDestFile(",");
			WriteEndDestFileLn();

			WriteDestFileLn("release,");
			WriteDestFileLn("0x0,");
			WriteDestFileLn("0,");

			WriteBgnDestFileLn();
			WriteQuoteToDestFile();
			WriteVersionStr();
			WriteQuoteToDestFile();
			WriteCStrToDestFile(",");
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
			WriteCStrToDestFile(",");
			WriteEndDestFileLn();
			WriteBgnDestFileLn();
			WriteQuoteToDestFile();
			WriteCStrToDestFile(kShortDescription);
			WriteQuoteToDestFile();
			WriteEndDestFileLn();
		WriteEndResResource();
#endif

#if ! UseIdeMW8
		WriteBlankLineToDestFile();
		WriteDestFileLn("resource 'SIZE' (-1) {");
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
#endif

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
					WriteCStrToDestFile(",");
					WriteUnsignedToOutput(++DocTypeCounter);
					WriteCStrToDestFile(", 0");
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
#endif /* ! UseTargMacho */

	WriteCloseDestFile();
}
#endif

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
		WriteCStrToDestFile("string ");
		WriteQuoteToDestFile();
		p();
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
	}
}

static char *pDt = nullpr;

static void WritepDtString(void)
{
	WriteCStrToDestFile(pDt);
}

#if UseTargMacho || UseTargCarbon
static void WritePListString(char *s)
{
	char *SavepDt = pDt;

	pDt = s;
	WritePListProcString(WritepDtString);
	pDt = SavepDt;
}
#endif

#if UseTargMacho || UseTargCarbon
static void WritePListKeyProcString(char *k, MyProc p)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginValEndLine("key", k);
		WriteXMLtagBeginProcValEndLine("string", p);
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("key ");
		WriteQuoteToDestFile();
		WriteCStrToDestFile(k);
		WriteQuoteToDestFile();
		WriteCStrToDestFile(" value string ");
		WriteQuoteToDestFile();
		p();
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
	}
}
#endif

#if UseTargMacho || UseTargCarbon
static void WritePListKeyString(char *k, char *s)
{
	char *SavepDt = pDt;

	pDt = s;
	WritePListKeyProcString(k, WritepDtString);
	pDt = SavepDt;
}
#endif

#if UseTargMacho || UseTargCarbon
static void WritePListBeginKeyArray(char *k)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginValEndLine("key", k);
		WriteBeginXMLtagLine("array");
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("key ");
		WriteQuoteToDestFile();
		WriteCStrToDestFile(k);
		WriteQuoteToDestFile();
		WriteCStrToDestFile(" value array");
		WriteEndDestFileLn();
		WriteDestFileLn("[");
		++DestFileIndent;
	}
}
#endif

#if UseTargMacho || UseTargCarbon
static void WritePListEndKeyArray(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteEndXMLtagLine("array");
	} else {
		--DestFileIndent;
		WriteDestFileLn("]");
	}
}
#endif

#if UseTargMacho || UseTargCarbon
static void WritePListBeginDict(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteBeginXMLtagLine("dict");
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("dictionary");
		WriteEndDestFileLn();
		WriteDestFileLn("{");
		++DestFileIndent;
	}
}
#endif

#if UseTargMacho || UseTargCarbon
static void WritePListEndDict(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteEndXMLtagLine("dict");
	} else {
		--DestFileIndent;
		WriteDestFileLn("}");
	}
}
#endif

#if UseTargMacho || UseTargCarbon
static void WriteInfoPList(MyProc p)
{
	CurPListFormat = kPListRaw;

	WriteDestFileLn("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	WriteDestFileLn("<!DOCTYPE plist SYSTEM \"file://localhost/System/Library/DTDs/PropertyList.dtd\">");
	WriteDestFileLn("<plist version=\"0.9\">");

	p();

	WriteDestFileLn("</plist>");
}
#endif

#if UseIdeMW8 && UseTargMacho
static void WriteMainPLC(MyProc p)
{
	CurPListFormat = kPListPLC;

	WriteDestFileLn("plist");
	WriteDestFileLn("{");
	++DestFileIndent;

	p();

	--DestFileIndent;
	WriteDestFileLn("}");
}
#endif

static void WritepDIcontMachoFile(void)
{
	WritepDtString();
	WriteCStrToDestFile("Icon.icns");
}

#if UseTargMacho || UseTargCarbon
static void WritepDtIconTypeName(void)
{
	WriteCStrToDestFile(kStrAppName);
	WriteCStrToDestFile(" ");
	WritepDtString();
}
#endif

#if UseTargMacho || UseTargCarbon
static void WriteOneExtension2Array(char *s)
{
	WritePListString(s);
}
#endif

#if UseTargMacho || UseTargCarbon
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
#endif

#if UseTargMacho || UseTargCarbon
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
#endif

#if (UseTargMacho && (! UseIdeAPB)) || (UseTargCarbon && UseIdeMPW3_6_a1)
static void WritePListData(void)
{
#if UseIdeMW8
	WriteOpenDestFile("my_config_d", "main", ".plc", "plist source");
	WriteMainPLC(WriteMyInfoPListContents);
	WriteCloseDestFile();
#else
	WriteOpenDestFile("my_config_d", "Info", ".plist", "Info.plist file");
	WriteInfoPList(WriteMyInfoPListContents);
	WriteCloseDestFile();
#endif
}
#endif

static void WriteConfigFiles(void)
{
	WriteCommonCNFGGLOB();
	WriteCommonCNFGRAPI();

#if HaveMacRrscs
	WriteCommonCNFGRSRC();
#endif

#if (UseTargMacho && (! UseIdeAPB)) || (UseTargCarbon && UseIdeMPW3_6_a1)
	WritePListData();
#endif
}

/* --- end of code for configuration files --- */


static void Write_output_d_ToDestFile(void)
{
#if ! TheUseAbsolute
	{

#if UseIdeMPW3_6_a1 || UseIdeMW8
		WriteCStrToDestFile(":");
#elif UseIdeBashGcc || UseIdeAPB
		WriteCStrToDestFile("./");
#elif UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42
		WriteCStrToDestFile(".");
#elif UseIdeLccW32
		WriteCStrToDestFile("c:\\output");
#endif

	}
#else
	{
		WriteScriptVarToDestFile("my_output_d");
	}
#endif /* TheUseAbsolute */
}

#ifndef OutputInProjects
#define OutputInProjects 0
#endif

static void Write_source_d_ToDestFile(void)
{
#if ! TheUseAbsolute
	{

#if OutputInProjects

#if UseIdeMPW3_6_a1 || UseIdeMW8
		WriteCStrToDestFile(":::source:");
#elif UseIdeBashGcc || UseIdeAPB
		WriteCStrToDestFile("../../source/");
#elif UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42 || UseIdeDevC
		WriteCStrToDestFile("..\\..\\source");
#elif UseIdeLccW32
		WriteCStrToDestFile("c:\\output\\..\\..\\source");
#else
#error "undefined for this IDE"
#endif

#else

#if UseIdeMPW3_6_a1 || UseIdeMW8
		WriteCStrToDestFile("::");
#elif UseIdeBashGcc || UseIdeAPB
		WriteCStrToDestFile("../");
#elif UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42 || UseIdeDevC
		WriteCStrToDestFile("..");
#elif UseIdeLccW32
		WriteCStrToDestFile("c:\\output\\..");
#else
#error "undefined for this IDE"
#endif

#endif

	}
#else
	{
		WriteScriptVarToDestFile("my_source_d");
	}
#endif
}

#if UseIdeMPW3_6_a1 || UseIdeBashGcc
#define DefineMakeIde 1
#else
#define DefineMakeIde 0
#endif

#if ! IsVersionQuerry
static void WriteSubDirToDestFile(MyProc p, char *s)
{
	p();

#if UseIdeMPW3_6_a1 || UseIdeMW8
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(":");
#elif UseIdeBashGcc || UseIdeAPB
	WriteCStrToDestFile(s);
	WriteCStrToDestFile("/");
#elif UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42 || UseIdeLccW32 || UseIdeDevC
	WriteBackSlashToDestFile();
	WriteCStrToDestFile(s);
#else
#error "undefined for this IDE"
#endif
}
#endif

#if UseIdeDevC || UseIdeLccW32 || UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42 || ThePrintCFiles
static void WriteFileInDirToDestFile(MyProc p, char *s, char *FileExt)
{
	p();

#if UseIdeMPW3_6_a1 || UseIdeMW8
	WriteCStrToDestFile(s);
#elif UseIdeBashGcc || UseIdeAPB
	WriteCStrToDestFile(s);
#elif UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42 || UseIdeLccW32 || UseIdeDevC
	WriteBackSlashToDestFile();
	WriteCStrToDestFile(s);
#else
#error "undefined for this IDE"
#endif

	WriteCStrToDestFile(FileExt);
}
#endif

#if (! DefineMakeIde) || ThePrintCFiles
static void Write_c_src_d_ToDestFile(void)
{
#if ! TheUseAbsolute
	{
		WriteSubDirToDestFile(Write_source_d_ToDestFile, "c_src");
	}
#else
	{
		WriteScriptVarToDestFile("my_c_src_d");
	}
#endif
}
#endif

#if HaveAsm && (UseIdeMW8 || UseIdeAPB)
static void Write_a0_src_d_ToDestFile(void)
{
#if ! TheUseAbsolute
	{
		WriteSubDirToDestFile(Write_source_d_ToDestFile, "a_src");
	}
#else
	{
		WriteScriptVarToDestFile("my_a_src_d");
	}
#endif
}
#endif

#if HaveAsm && (UseIdeMW8 || UseIdeAPB)
static void Write_a1_src_d_ToDestFile(void)
{
	WriteSubDirToDestFile(Write_a0_src_d_ToDestFile, "ppc");
}
#endif

#if HaveAsm && (UseIdeMW8 || UseIdeAPB)
static void Write_a_src_d_ToDestFile(void)
{
#if UseTargMacho
		WriteSubDirToDestFile(Write_a1_src_d_ToDestFile, "as");
#else
		WriteSubDirToDestFile(Write_a1_src_d_ToDestFile, "ppcasm");
#endif
}
#endif

#if UseIdeDevC || UseIdeLccW32 || UseIdeMSVC || UseIdeMW8
static void Write_platform_d_ToDestFile(void)
{
#if ! TheUseAbsolute
	{
		WriteSubDirToDestFile(Write_source_d_ToDestFile, "platform");
	}
#else
	{
		WriteScriptVarToDestFile("my_platform_d");
	}
#endif
}
#endif

#if UseIdeDevC || UseIdeLccW32 || UseIdeMSVC || UseIdeMW8
static void Write_rz_src_ToDestFile(void)
{
#if ! TheUseAbsolute
	{
#if UseTargWinx86
			WriteSubDirToDestFile(Write_platform_d_ToDestFile, "win");
#else
			WriteSubDirToDestFile(Write_platform_d_ToDestFile, "mac");
#endif
	}
#else
	{
		WriteScriptVarToDestFile("my_rz_src_d");
	}
#endif /* TheUseAbsolute */
}
#endif

#if ThePrintCFiles || UseIdeMSVC
static void Write_config_d_ToDestFile(void)
{
#if ! TheUseAbsolute
	{
		WriteSubDirToDestFile(Write_output_d_ToDestFile, "config");
	}
#else
	{
		WriteScriptVarToDestFile("my_config_d");
	}
#endif
}
#endif

#if UseIdeLccW32
static void Write_obj_d_ToDestFile(void)
{
#if ! TheUseAbsolute
	{
		WriteSubDirToDestFile(Write_output_d_ToDestFile, "obj");
	}
#else
	{
		WriteScriptVarToDestFile("my_obj_d");
	}
#endif
}
#endif

#if UseIdeDevC || UseIdeLccW32 || UseIdeAPB
static unsigned int FileCounter;
#endif

#if UseIdeDevC || UseIdeLccW32
static void DoSrcFileIncrFileCounter(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(s);
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	++FileCounter;
}
#endif

#if ThePrintCFiles
static void WriteFileToCFilesList(MyProc p, char *s, char *FileExt)
{
	WriteBgnDestFileLn();
	WriteFileInDirToDestFile(p, s, FileExt);
	WriteEndDestFileLn();
}
#endif

#if ThePrintCFiles
static void DoConfigHeaderFile(char *s)
{
	WriteFileToCFilesList(Write_config_d_ToDestFile, s, "");
}
#endif

#if ThePrintCFiles
static void DoSrcExtraHeaderFile(char *s)
{
	WriteFileToCFilesList(Write_c_src_d_ToDestFile, s, "");
}
#endif

#if ThePrintCFiles
static void DoSrcDependsAddToList(char *s)
{
	WriteFileToCFilesList(Write_c_src_d_ToDestFile, s, "");
}
#endif

#if ThePrintCFiles
static void DoSrcFileAddToList(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	WriteFileToCFilesList(Write_c_src_d_ToDestFile, s, ".h");
	WriteFileToCFilesList(Write_c_src_d_ToDestFile, s, ".c");

	if (depends != nullpr) {
		depends(DoSrcDependsAddToList);
	}
}
#endif

#if ThePrintCFiles
static void WriteCFilesList(void)
{
	WriteOpenDestFile("my_output_d", "c_files", "", "list of c files");

	DoConfigHeaderFile("CNFGGLOB.h");
	DoConfigHeaderFile("CNFGRAPI.h");

	DoAllExtraHeaders(DoSrcExtraHeaderFile);

	DoAllSrcFiles(DoSrcFileAddToList);
	WriteCloseDestFile();
}
#endif

#if UseTargMacho || (UseTargCarbon && UseIdeMPW3_6_a1)
static void WriteMachoAppNameStr(void)
{
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(".app");
}
#endif

#if UseTargWinx86
static void WriteWinAppNameStr(void)
{
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(".exe");
}
#endif

#if UseIdeBashGcc || UseIdeMPW3_6_a1
static void WriteCommonCOptions(blnr fast)
{
#if UseDbgLvlQuick
	UnusedParam(fast);
#endif

#if UseIdeMPW3_6_a1

#if UseCPUFam68K
	{
		WriteCStrToDestFile(" -proto strict -w 17 -align mac68k -b");
#if UseTargMac68KFPU
		WriteCStrToDestFile(" -mc68020 -mc68881 -elems881");
#endif
#if UseDbgLvlShip
			WriteCStrToDestFile(" -mbg off");
#endif
		WriteCStrToDestFile(" -model farCode");
	}
#endif /* UseCPUFam68K */
#if UseCPUFamPPC
	{
		WriteCStrToDestFile(" -proto strict -w 17");
#if ! UseDbgLvlShip
			WriteCStrToDestFile(" -traceback");
#endif
	}
#endif /* UseCPUFamPPC */

#if ! UseDbgLvlQuick
	{
		if (fast) {
#if UseCPUFam68K
				WriteCStrToDestFile(" -opt speed");
#endif
#if UseCPUFamPPC
				WriteCStrToDestFile(" -opt speed");
#endif
		} else {
#if UseCPUFam68K
				WriteCStrToDestFile(" -opt space");
#endif
#if UseCPUFamPPC
				WriteCStrToDestFile(" -opt size");
				/* this may not be reliable? */
#endif
		}
	}
#endif /* ! UseDbgLvlQuick */

#elif UseIdeBashGcc
	WriteCStrToDestFile(" -c -Wall -Wmissing-prototypes -Wstrict-prototypes -Wno-uninitialized");
#if UsingAppleTools
	WriteCStrToDestFile(" -mdynamic-no-pic -fpascal-strings");
#endif
#if ! UseDbgLvlQuick
		if (fast) {
			WriteCStrToDestFile(" -O3");
		} else {
			WriteCStrToDestFile(" -Os");
		}
#else
		WriteCStrToDestFile(" -g -O0");
#endif /* UseDbgLvlQuick */
#endif /* Ide */

#if UseIdeMPW3_6_a1
	WriteCStrToDestFile(" -i ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("{mk_config_d}");
	WriteQuoteToDestFile();
#elif UseIdeBashGcc
	WriteCStrToDestFile(" -I$(mk_config_d)");
#endif

#if UseTargMacX11
		WriteCStrToDestFile(" -I/usr/X11R6/include/");
#endif
}
#endif

static void WriteSrcSuffix(blnr AsmAvail)
{
#if ! HaveAsm
	UnusedParam(AsmAvail);
#else
	if (AsmAvail) {
		WriteCStrToDestFile(".s");
	} else
#endif
	{
		WriteCStrToDestFile(".c");
	}
}

#if UseIdeMPW3_6_a1
static void DoSrcDependsMPWMakeCompile(char *s)
{
	WriteCStrToDestFile(" ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("{mk_c_src_d}");
	WriteCStrToDestFile(s);
	WriteQuoteToDestFile();
}
#endif

#if UseIdeMPW3_6_a1
static void DoSrcFileMPWMakeCompile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	WriteBgnDestFileLn();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("{mk_obj_d}");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(".o");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" \304 ");
	WriteQuoteToDestFile();
#if HaveAsm
	if (AsmAvail) {
		WriteCStrToDestFile("{mk_a_src_d}");
	} else
#endif
	{
		WriteCStrToDestFile("{mk_c_src_d}");
	}
	WriteCStrToDestFile(s);
	WriteSrcSuffix(AsmAvail);
	WriteQuoteToDestFile();
	if (depends != nullpr) {
		depends(DoSrcDependsMPWMakeCompile);
	}
#if HaveAsm
	if (! AsmAvail)
#endif
	{
		WriteCStrToDestFile(" ");
		WriteQuoteToDestFile();
		WriteCStrToDestFile("{mk_config_d}CNFGGLOB.h");
		WriteQuoteToDestFile();
	}
	WriteEndDestFileLn();

	++DestFileIndent;
		WriteBgnDestFileLn();
#if HaveAsm
		if (AsmAvail) {
			WriteCStrToDestFile("{mk_CallA} ");
		} else
#endif
		{
			WriteCStrToDestFile("{mk_CallC} ");
		}
		WriteQuoteToDestFile();
#if HaveAsm
		if (AsmAvail) {
			WriteCStrToDestFile("{mk_a_src_d}");
		} else
#endif
		{
			WriteCStrToDestFile("{mk_c_src_d}");
		}
		WriteCStrToDestFile(s);
		WriteSrcSuffix(AsmAvail);
		WriteQuoteToDestFile();
		WriteCStrToDestFile(" -o ");
		WriteQuoteToDestFile();
		WriteCStrToDestFile("{mk_obj_d}");
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(".o");
		WriteQuoteToDestFile();
#if HaveAsm
		if (! AsmAvail)
#endif
		{
			WriteCStrToDestFile(" {mk_COptions}");
		}
		WriteEndDestFileLn();
	--DestFileIndent;
}
#endif

#if UseIdeMPW3_6_a1
static void DoSrcFileMPWMakeObjects(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBgnDestFileLn();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("{mk_obj_d}");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(".o");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" \266");
	WriteEndDestFileLn();
}
#endif

#if UseTargMacho || (UseTargCarbon && UseIdeMPW3_6_a1)
#define HaveMacBundleApp 1
#else
#define HaveMacBundleApp 0
#endif

#if UseIdeMPW3_6_a1 && HaveMacBundleApp
static void WriteDocTypeCopyMachoFileMPW(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	WriteBgnDestFileLn();
	WriteCStrToDestFile("Duplicate \"{mk_osxrz_d}");
	WriteCStrToDestFile(ShortName);
	WriteCStrToDestFile("Icon.icns\" \"{mk_tresources_d}\"");
	WriteEndDestFileLn();
}
#endif

#if UseIdeMPW3_6_a1
static void WriteMPWSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", "Makefile", "", "Make file");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# make file generated by ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("setup.mpw");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" script");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_output_d = ");
	Write_output_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_source_d = ");
	Write_source_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_config_d = {mk_output_d}config:");
	WriteDestFileLn("mk_obj_d = {mk_output_d}obj:");

#if HaveMacBundleApp
	{
		WriteBgnDestFileLn();
		WriteCStrToDestFile("mk_built_program = {mk_output_d}");
		WriteMachoAppNameStr();
		WriteCStrToDestFile(":");
		WriteEndDestFileLn();

		WriteDestFileLn("mk_contents_d = {mk_built_program}Contents:");
		WriteDestFileLn("mk_macos_d = {mk_contents_d}MacOS:");
		WriteDestFileLn("mk_resources_d = {mk_contents_d}Resources:");

		WriteBgnDestFileLn();
		WriteCStrToDestFile("mk_built_exec = {mk_macos_d}");
		WriteCStrToDestFile(kStrAppAbbrev);
		WriteEndDestFileLn();
	}
#else
	{
		WriteBgnDestFileLn();
		WriteCStrToDestFile("mk_built_program = {mk_output_d}");
		WriteCStrToDestFile(kStrAppAbbrev);
		WriteEndDestFileLn();
	}
#endif /* ! HaveMacBundleApp */


	WriteBlankLineToDestFile();
#if HaveAsm
		WriteDestFileLn("mk_a_src_d = {mk_source_d}a_src:ppc:ppcasm:");
#endif
	WriteDestFileLn("mk_c_src_d = {mk_source_d}c_src:");
	WriteDestFileLn("mk_platform_d = {mk_source_d}platform:");
	WriteDestFileLn("mk_rz_src_d = {mk_platform_d}mac:");
#if HaveMacBundleApp
		WriteDestFileLn("mk_osxrz_d = {mk_rz_src_d}osx:");
#endif
	WriteBlankLineToDestFile();
#if HaveAsm
	{
		WriteDestFileLn("mk_CallA = PPCAsm");
		WriteDestFileLn("mk_AOptions = ");
	}
#endif
#if UseCPUFam68K
		WriteDestFileLn("mk_CallC = SC");
#endif
#if UseCPUFamPPC
		WriteDestFileLn("mk_CallC = MrC");
#endif

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptions =");
	WriteCommonCOptions(falseblnr);
	WriteEndDestFileLn();
	WriteBlankLineToDestFile();

	WriteBlankLineToDestFile();
#if HaveMacBundleApp
		WriteDestFileLn("TheApplication \304 \"{mk_built_exec}\"");
#else
		WriteDestFileLn("TheApplication \304 \"{mk_built_program}\"");
#endif /* ! HaveMacBundleApp */
	WriteBlankLineToDestFile();
	DoAllSrcFiles(DoSrcFileMPWMakeCompile);
	WriteBlankLineToDestFile();
	WriteDestFileLn("ObjFiles = \266");
	++DestFileIndent;
		DoAllSrcFiles(DoSrcFileMPWMakeObjects);
		WriteBlankLineToDestFile();
	--DestFileIndent;

#if HaveMacBundleApp
	{
		WriteBlankLineToDestFile();
		WriteDestFileLn("mk_tbuilt_program = {mk_output_d}AppTemp:");
		WriteDestFileLn("mk_tcontents_d = {mk_tbuilt_program}Contents:");
		WriteDestFileLn("mk_tmacos_d = {mk_tcontents_d}MacOS:");
		WriteDestFileLn("mk_tresources_d = {mk_tcontents_d}Resources:");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\"{mk_resources_d}AppIcon.icns\" \304 \"{mk_osxrz_d}AppIcon.icns\"");
		++DestFileIndent;
			WriteDestFileLn("Delete -i -y \"{mk_built_program}\"");
			WriteDestFileLn("Delete -i -y \"{mk_tbuilt_program}\"");
			WriteDestFileLn("NewFolder \"{mk_tbuilt_program}\"");
			WriteDestFileLn("NewFolder \"{mk_tcontents_d}\"");
			WriteDestFileLn("NewFolder \"{mk_tmacos_d}\"");
			WriteDestFileLn("NewFolder \"{mk_tresources_d}\"");
			WriteDestFileLn("Duplicate \"{mk_osxrz_d}AppIcon.icns\" \"{mk_tresources_d}\"");
			DoAllDocTypes(WriteDocTypeCopyMachoFileMPW);
			WriteDestFileLn("Duplicate \"{mk_config_d}Info.plist\" \"{mk_tcontents_d}\"");

			WriteBgnDestFileLn();
			WriteCStrToDestFile("Echo -n \"APPL");
			WriteCStrToDestFile(kMacCreatorSig);
			WriteCStrToDestFile("\" > \"{mk_tcontents_d}PkgInfo\"");
			WriteEndDestFileLn();

			WriteDestFileLn("Move \"{mk_tbuilt_program}\" \"{mk_built_program}\"");
		--DestFileIndent;
	}
#endif /* HaveMacBundleApp */

#if HaveMacRrscs
	WriteBlankLineToDestFile();
	WriteDestFileLn("\"{mk_obj_d}main.rsrc\" \304 \"{mk_rz_src_d}main.r\"");
	++DestFileIndent;
		WriteDestFileLn("Rez -t rsrc -c RSED -i \"{RIncludes}\" -i \"{mk_rz_src_d}\" \"{mk_rz_src_d}main.r\" -o \"{mk_obj_d}main.rsrc\" -i \"{mk_config_d}\"");
	--DestFileIndent;
#endif /* HaveMacRrscs */
	WriteBlankLineToDestFile();
#if HaveMacBundleApp
		WriteDestFileLn("\"{mk_built_exec}\" \304 {ObjFiles} \"{mk_resources_d}AppIcon.icns\"");
#else
		WriteDestFileLn("\"{mk_built_program}\" \304 {ObjFiles} \"{mk_obj_d}main.rsrc\"");
#endif
	++DestFileIndent;
#if HaveMacRrscs
		WriteDestFileLn("Duplicate -y \"{mk_obj_d}main.rsrc\" \"{mk_built_program}\"");
#endif

		WriteBgnDestFileLn();
#if UseCPUFam68K
		{
			WriteCStrToDestFile("Link");
#if UseDbgLvlShip
				WriteCStrToDestFile(" -rn");
#endif
			WriteCStrToDestFile(" -model far -srtsg all -sg Main=STDCLIB,SANELIB,CSANELib,SADEV,STDIO");
		}
#endif /* UseCPUFam68K */
#if UseCPUFamPPC
			WriteCStrToDestFile("PPCLink");
#endif

#if UseTargCarbon
			WriteCStrToDestFile(" -m main");
#endif
		WriteCStrToDestFile(" -t APPL -c ");
		WriteCStrToDestFile(kMacCreatorSig);
		WriteCStrToDestFile(" \266");
		WriteEndDestFileLn();

		++DestFileIndent;
#if HaveMacBundleApp
				WriteDestFileLn("-o \"{mk_built_exec}\" \266");
#else
				WriteDestFileLn("-o \"{mk_built_program}\" \266");
#endif

#if UseTargCarbon
			WriteDestFileLn("\"{SharedLibraries}CarbonLib\" \266");
#if UseOpenGL
			WriteDestFileLn("\"{SharedLibraries}OpenGLLibraryStub\" \266");
#endif
			WriteDestFileLn("\"{PPCLibraries}PPCToolLibs.o\" \266");
			WriteDestFileLn("\"{PPCLibraries}PPCCRuntime.o\" \266");
			WriteDestFileLn("\"{SharedLibraries}StdCLib\" \266");
#elif UseTargClassicPPC
			WriteDestFileLn("\"{PPCLibraries}PPCToolLibs.o\" \266");
			WriteDestFileLn("\"{PPCLibraries}PPCCRuntime.o\" \266");
			WriteDestFileLn("\"{PPCLibraries}StdCRuntime.o\" \266");
			WriteDestFileLn("\"{SharedLibraries}InterfaceLib\" \266");
			WriteDestFileLn("\"{SharedLibraries}MathLib\" \266");
			WriteDestFileLn("\"{SharedLibraries}StdCLib\" \266");
			WriteDestFileLn("-weaklib AppearanceLib \266");
			WriteDestFileLn("\"{SharedLibraries}AppearanceLib\" \266");
			WriteDestFileLn("-weaklib MenusLib \266");
			WriteDestFileLn("\"{SharedLibraries}MenusLib\" \266");
			WriteDestFileLn("-weaklib NavigationLib \266");
			WriteDestFileLn("\"{SharedLibraries}NavigationLib\" \266");
			WriteDestFileLn("-weaklib DragLib \266");
			WriteDestFileLn("\"{SharedLibraries}DragLib\" \266");
			WriteDestFileLn("-weaklib WindowsLib \266");
			WriteDestFileLn("\"{SharedLibraries}WindowsLib\" \266");
#elif UseTargMac68K
			WriteDestFileLn("\"{Libraries}Interface.o\" \266");
			WriteDestFileLn("\"{Libraries}MacRuntime.o\" \266");
			/* WriteDestFileLn("\"{Libraries}MathLib.o\" \266"); */
			WriteDestFileLn("\"{Libraries}Navigation.o\" \266");
#elif UseTargMac68KFPU
			WriteDestFileLn("\"{Libraries}Interface.o\" \266");
			WriteDestFileLn("\"{Libraries}MacRuntime.o\" \266");
			/* WriteDestFileLn("\"{Libraries}MathLib881.o\" \266"); */
			WriteDestFileLn("\"{Libraries}Navigation.o\" \266");
#endif

			WriteDestFileLn("{ObjFiles}");
		--DestFileIndent;

#if HaveMacBundleApp
			WriteDestFileLn("SetFile -d . -m . \"{mk_built_exec}\"");
#else
			WriteDestFileLn("SetFile -d . -m . -a B \"{mk_built_program}\"");
#endif
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteDestFileLn("clean \304");
	++DestFileIndent;
#if HaveMacBundleApp
		WriteDestFileLn("Delete -i {ObjFiles}");
		WriteDestFileLn("Delete -i -y \"{mk_built_program}\"");
#else
		WriteDestFileLn("Delete -i \"{mk_built_program}\" {ObjFiles} \"{mk_obj_d}main.rsrc\"");
#endif
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteCloseDestFile();

}
#endif

#if UseIdeMW8
static void WriteXMLtagSettingNameProcVal(char *n, MyProc v)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin("SETTING");
	WriteXMLtagBegin("NAME");
	WriteCStrToDestFile(n);
	WriteXMLtagEnd("NAME");
	WriteXMLtagBegin("VALUE");
	v();
	WriteXMLtagEnd("VALUE");
	WriteXMLtagEnd("SETTING");
	WriteEndDestFileLn();
}
#endif

#if UseIdeMW8
static void WriteXMLtagSettingNameVal(char *n, char *v)
{
	char *SavepDt = pDt;

	pDt = v;
	WriteXMLtagSettingNameProcVal(n, WritepDtString);
	pDt = SavepDt;
}
#endif

#if UseIdeMW8
static void WriteBeginNamedSettingXMLtagLine(char *s)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin("SETTING");
	WriteXMLtagBegin("NAME");
	WriteCStrToDestFile(s);
	WriteXMLtagEnd("NAME");
	WriteEndDestFileLn();
	++DestFileIndent;
}
#endif

#if UseIdeMW8
static void WriteAllMWTargetName(void)
{
	WriteXMLtagBeginProcValEndLine("TARGETNAME", WriteAppVariationStr);
}
#endif

#if UseIdeMW8
typedef void (*tWriteMWLib)(char *s);
#endif

#if UseIdeMW8
static void WriteMWLibs(tWriteMWLib p)
{
#if UseTargWinx86
	p("kernel32.lib");
	p("user32.lib");
	p("gdi32.lib");
	p("comdlg32.lib");
	p("shell32.lib");
	p("ole32.lib");
	p("uuid.lib");
	p("winmm.lib");
	p("MSL_All_x86.lib");
#elif UseTargMacho
	p("crt1.o");
	p("MSL_All_Mach-O_D.lib");
#else
#if UseTargCarbon
		p("CarbonLib");
#if UseOpenGL
			p("OpenGLLibraryStub");
#endif
#else
		p("InterfaceLib");
		p("MathLib");
		p("AppearanceLib");
		p("MenusLib");
		p("NavigationLib");
		p("DragLib");
		p("WindowsLib");
#endif /* ! UseTargCarbon */
	p("MSL_Runtime_PPC.Lib");
#endif
}
#endif

#if UseIdeMW8
static void WriteMWLibAddFile(char *s)
{
	WriteBeginXMLtagLine("FILE");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginValEndLine("PATH", s);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
#if UseTargWinx86
			WriteXMLtagBeginValEndLine("FILEKIND", "Unknown");
#else
			WriteXMLtagBeginValEndLine("FILEKIND", "Library");
#endif
		WriteXMLtagBeginValEndLine("FILEFLAGS", "");
	WriteEndXMLtagLine("FILE");
}
#endif

#if UseIdeMW8
static void WriteMWLibMakeObjects(char *s)
{
	WriteBeginXMLtagLine("FILEREF");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginValEndLine("PATH", s);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}
#endif

#if UseIdeMW8
static void WriteMWLibGroupList(char *s)
{
	WriteBeginXMLtagLine("FILEREF");
		WriteAllMWTargetName();
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginValEndLine("PATH", s);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}
#endif

#if UseIdeMW8
static void WriteMWProjRelSearchPath(MyProc p)
{
	WriteBeginXMLtagLine("SETTING");
		WriteBeginNamedSettingXMLtagLine("SearchPath");
			WriteBgnDestFileLn();
			WriteXMLtagBegin("SETTING");
			WriteXMLtagBegin("NAME");
			WriteCStrToDestFile("Path");
			WriteXMLtagEnd("NAME");
			WriteXMLtagBegin("VALUE");
			p();
			WriteXMLtagEnd("VALUE");
			WriteXMLtagEnd("SETTING");
			WriteEndDestFileLn();

			WriteXMLtagSettingNameVal("PathFormat", "MacOS");
#if ! TheUseAbsolute
				WriteXMLtagSettingNameVal("PathRoot", "Project");
#else
				WriteXMLtagSettingNameVal("PathRoot", "Absolute");
#endif
		WriteEndXMLtagLine("SETTING");
		WriteXMLtagSettingNameVal("Recursive", "true");
		WriteXMLtagSettingNameVal("FrameworkPath", "false");
		WriteXMLtagSettingNameVal("HostFlags", "All");
	WriteEndXMLtagLine("SETTING");
}
#endif

#if UseIdeMW8
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
#endif

#if UseIdeMW8
static void WriteBeginXMLMWGroup(char *s)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin("GROUP");
	WriteXMLtagBegin("NAME");
	WriteCStrToDestFile(s);
	WriteXMLtagEnd("NAME");
	WriteEndDestFileLn();
	++DestFileIndent;
}
#endif

#if UseIdeMW8
static void WriteMWSettingsPanelComment(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<!-- Settings for ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(s);
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" panel -->");
	WriteEndDestFileLn();
}
#endif

#if UseIdeMW8
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
#endif

#if UseIdeMW8
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
#endif

#if UseIdeMW8
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
#endif

#if UseIdeMW8
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
#endif

#if UseIdeMW8
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
#endif

#if UseIdeMW8
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
#endif

#if UseIdeMW8
static void DoSrcFileMW8AddFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBeginXMLtagLine("FILE");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");

		WriteBgnDestFileLn();
		WriteXMLtagBegin("PATH");
		WriteCStrToDestFile(s);
		WriteSrcSuffix(AsmAvail);
		WriteXMLtagEnd("PATH");
		WriteEndDestFileLn();

		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
		WriteXMLtagBeginValEndLine("FILEKIND", "Text");
#if UseDbgLvlQuick
			WriteXMLtagBeginValEndLine("FILEFLAGS", "Debug");
#else
			WriteXMLtagBeginValEndLine("FILEFLAGS", "");
#endif
	WriteEndXMLtagLine("FILE");
}
#endif

#if UseIdeMW8
static void DoSrcFileMW8sMakeObjects(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBeginXMLtagLine("FILEREF");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");

		WriteBgnDestFileLn();
		WriteXMLtagBegin("PATH");
		WriteCStrToDestFile(s);
		WriteSrcSuffix(AsmAvail);
		WriteXMLtagEnd("PATH");
		WriteEndDestFileLn();

		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}
#endif

#if UseIdeMW8
static void DoSrcFileMW8GroupList(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBeginXMLtagLine("FILEREF");
		WriteAllMWTargetName();
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");

		WriteBgnDestFileLn();
		WriteXMLtagBegin("PATH");
		WriteCStrToDestFile(s);
		WriteSrcSuffix(AsmAvail);
		WriteXMLtagEnd("PATH");
		WriteEndDestFileLn();

		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}
#endif

#if UseIdeMW8
static void WriteMetrowerksSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", kStrAppAbbrev, ".mcp.xml", "Project Import");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("<?xml version=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("1.0");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" encoding=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("UTF-8");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" standalone=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("yes");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" ?>");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("<?codewarrior exportversion=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("1.0.1");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" ideversion=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("5.0");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" ?>");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteBeginXMLtagLine("PROJECT");
	WriteBeginXMLtagLine("TARGETLIST");
	WriteBeginXMLtagLine("TARGET");
		WriteXMLtagBeginProcValEndLine("NAME", WriteAppVariationStr);
		WriteBeginXMLtagLine("SETTINGLIST");
			WriteMWSettingsPanelComment("Access Paths");
#if UseTargMacho
				WriteXMLtagSettingNameVal("InterpretDOSAndUnixPaths", "true");
				WriteXMLtagSettingNameVal("RequireFrameworkStyleIncludes", "true");
#endif
			WriteBeginNamedSettingXMLtagLine("UserSearchPaths");
				WriteMWDrvRelSearchPath(":config:");
				WriteMWProjRelSearchPath(Write_c_src_d_ToDestFile);
#if HaveAsm
					WriteMWProjRelSearchPath(Write_a_src_d_ToDestFile);
#endif
				WriteMWProjRelSearchPath(Write_rz_src_ToDestFile);
#if UseTargMacho
					/* seems to be wanted by propertly list compiler */
					WriteMWDrvRelSearchPath(":");
#endif
			WriteEndXMLtagLine("SETTING");
			WriteBeginNamedSettingXMLtagLine("SystemSearchPaths");
#if UseTargWinx86
				{
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
				}
#else
				{
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

#if UseTargMacho
					{
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
					}
#else
					{
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
#endif /* UseTargMacho */
				}
#endif /* ! UseTargWinx86 */

			WriteEndXMLtagLine("SETTING");
			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("Target Settings");
#if UseTargWinx86
			{
				WriteXMLtagSettingNameVal("Linker", "Win32 x86 Linker");
			}
#elif UseTargMacho
			{
				WriteXMLtagSettingNameVal("Linker", "MacOS X PPC Linker");
			}
#else
			{
				WriteXMLtagSettingNameVal("Linker", "MacOS PPC Linker");
			}
#endif /* Target */
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
#if UseDbgLvlQuick
				WriteXMLtagSettingNameVal("MWFrontEnd_C_dontinline", "1");
#else
				WriteXMLtagSettingNameVal("MWFrontEnd_C_dontinline", "0");
#endif
			WriteXMLtagSettingNameVal("MWFrontEnd_C_useRTTI", "0");
#if UseDbgLvlQuick
				WriteXMLtagSettingNameVal("MWFrontEnd_C_autoinline", "0");
#else
				WriteXMLtagSettingNameVal("MWFrontEnd_C_autoinline", "1");
#endif

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
#if UseTargWinx86
			{
				WriteMWSettingsPanelComment("x86 CodeGen");
#if UseDbgLvlQuick
					WriteXMLtagSettingNameVal("MWCodeGen_X86_intrinsics", "0");
#else
					WriteXMLtagSettingNameVal("MWCodeGen_X86_intrinsics", "1");
#endif
			}
#elif UseTargMacho
			{
				WriteMWSettingsPanelComment("PPC CodeGen Mach-O");
				WriteXMLtagSettingNameVal("MWCodeGen_MachO_structalignment", "PPC");
#if UseDbgLvlQuick
				{
					WriteXMLtagSettingNameVal("MWCodeGen_MachO_peephole", "0");
					WriteXMLtagSettingNameVal("MWCodeGen_MachO_schedule", "0");
				}
#else
				{
					WriteXMLtagSettingNameVal("MWCodeGen_MachO_peephole", "1");
					WriteXMLtagSettingNameVal("MWCodeGen_MachO_schedule", "1");
				}
#endif /* ! UseDbgLvlQuick */
			}
#else /* Target */
			{
				WriteMWSettingsPanelComment("PPC CodeGen");
#if ! UseDbgLvlShip
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_tracebacktables", "Inline");
#else
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_tracebacktables", "None");
#endif
				WriteXMLtagSettingNameVal("MWCodeGen_PPC_vectortocdata", "0");
#if UseDbgLvlQuick
				{
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_peephole", "0");
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_schedule", "0");
				}
#else
				{
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_peephole", "1");
					WriteXMLtagSettingNameVal("MWCodeGen_PPC_schedule", "1");
				}
#endif /* ! UseDbgLvlQuick */
			}
#endif /* Target */

			WriteBlankLineToDestFile();
#if UseTargWinx86
			{
				WriteMWSettingsPanelComment("x86 Global Optimizer");
#if UseDbgLvlQuick
					WriteXMLtagSettingNameVal("GlobalOptimizer_X86_optimizationlevel", "Level0");
#else
					WriteXMLtagSettingNameVal("GlobalOptimizer_X86_optimizationlevel", "Level4");
#endif
				WriteXMLtagSettingNameVal("GlobalOptimizer_X86_optfor", "Size");

				/* work around what is probably bug in windows version of mw8 */
#if UseDbgLvlQuick
					WriteXMLtagSettingNameVal("GlobalOptimizer_X86__optimizationlevel", "Level0");
#else
					WriteXMLtagSettingNameVal("GlobalOptimizer_X86__optimizationlevel", "Level4");
#endif
				WriteXMLtagSettingNameVal("GlobalOptimizer_X86__optfor", "Size");
			}
#else
			{
				WriteMWSettingsPanelComment("PPC Global Optimizer");
#if UseDbgLvlQuick
					WriteXMLtagSettingNameVal("GlobalOptimizer_PPC_optimizationlevel", "Level0");
#else
					WriteXMLtagSettingNameVal("GlobalOptimizer_PPC_optimizationlevel", "Level4");
#endif
				WriteXMLtagSettingNameVal("GlobalOptimizer_PPC_optfor", "Size");
			}
#endif /* ! UseTargWinx86 */

			WriteBlankLineToDestFile();
#if UseTargWinx86
			{
				WriteMWSettingsPanelComment("x86 Linker");
				WriteXMLtagSettingNameVal("MWLinker_X86_subsystem", "WinGUI");
#if UseDbgLvlQuick
					WriteXMLtagSettingNameVal("MWLinker_X86_linkdebug", "true");
#else
					WriteXMLtagSettingNameVal("MWLinker_X86_linkdebug", "false");
#endif
				WriteXMLtagSettingNameVal("MWLinker_X86_usedefaultlibs", "false");
			}
#elif UseTargMacho
			{
				WriteMWSettingsPanelComment("PPC Mac OS X Linker");
#if UseDbgLvlQuick
					WriteXMLtagSettingNameVal("MWLinker_MacOSX_linksym", "1");
#else
					WriteXMLtagSettingNameVal("MWLinker_MacOSX_linksym", "0");
#endif
				WriteXMLtagSettingNameVal("MWLinker_MacOSX_symfullpath", "1");
				WriteXMLtagSettingNameVal("MWLinker_MacOSX_permitmultdefs", "0");
#if UseDbgLvlShip
					WriteXMLtagSettingNameVal("MWLinker_MacOSX_strip_debug_symbols", "1");
#endif
			}
#else
			{
				WriteMWSettingsPanelComment("PPC Linker");
#if UseDbgLvlQuick
					WriteXMLtagSettingNameVal("MWLinker_PPC_linksym", "1");
#else
					WriteXMLtagSettingNameVal("MWLinker_PPC_linksym", "0");
#endif
				WriteXMLtagSettingNameVal("MWLinker_PPC_linkmode", "Normal");
			}
#endif /* Target */

			WriteBlankLineToDestFile();
#if UseTargWinx86
			{
				WriteMWSettingsPanelComment("x86 Project");
				WriteXMLtagSettingNameProcVal("MWProject_X86_outfile", WriteWinAppNameStr);
			}
#elif UseTargMacho
			{
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
			}
#else
			{
				WriteMWSettingsPanelComment("PPC Project");
				WriteXMLtagSettingNameVal("MWProject_PPC_outfile", kStrAppAbbrev);
				WriteXMLtagSettingNameVal("MWProject_PPC_filecreator", kMacCreatorSig);
				WriteXMLtagSettingNameVal("MWProject_PPC_size", "6000");
				WriteXMLtagSettingNameVal("MWProject_PPC_minsize", "3000");
				WriteXMLtagSettingNameVal("MWProject_PPC_flags", "22768");
			}
#endif /* Target */
		WriteEndXMLtagLine("SETTINGLIST");

		WriteBeginXMLtagLine("FILELIST");
			WriteMWLibs(WriteMWLibAddFile);

#if UseTargWinx86
			{
				WriteBeginXMLtagLine("FILE");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.RC");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteXMLtagBeginValEndLine("FILEKIND", "Text");
					WriteXMLtagBeginValEndLine("FILEFLAGS", "");
				WriteEndXMLtagLine("FILE");
			}
#elif UseTargMacho
			{
				WriteBeginXMLtagLine("FILE");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.plc");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteXMLtagBeginValEndLine("FILEKIND", "Text");
					WriteXMLtagBeginValEndLine("FILEFLAGS", "Debug");
				WriteEndXMLtagLine("FILE");
				WriteMWSource_icnsAddFile("App");
				DoAllDocTypes(WriteDocTypeMWAddFile);
			}
#else
			{
				WriteBeginXMLtagLine("FILE");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.r");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteXMLtagBeginValEndLine("FILEKIND", "Text");
					WriteXMLtagBeginValEndLine("FILEFLAGS", "");
				WriteEndXMLtagLine("FILE");
			}
#endif /* Target */

			DoAllSrcFiles(DoSrcFileMW8AddFile);
		WriteEndXMLtagLine("FILELIST");

		WriteBeginXMLtagLine("LINKORDER");
			WriteMWLibs(WriteMWLibMakeObjects);
#if UseTargWinx86
			{
				WriteBeginXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.RC");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
				WriteEndXMLtagLine("FILEREF");
			}
#elif UseTargMacho
			{
				WriteBeginXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.plc");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
				WriteEndXMLtagLine("FILEREF");
				WriteMWSource_icnsMakeObjects("App");
				DoAllDocTypes(WriteDocTypeMWMakeObjects);
			}
#else
			{
				WriteBeginXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.r");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
				WriteEndXMLtagLine("FILEREF");
			}
#endif /* Target */
			DoAllSrcFiles(DoSrcFileMW8sMakeObjects);
		WriteEndXMLtagLine("LINKORDER");

#if UseTargMacho
		{
			WriteBeginXMLtagLine("FRAMEWORKLIST");
				WriteBeginXMLtagLine("FRAMEWORK");
					WriteBeginXMLtagLine("FILEREF");
						WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
						WriteXMLtagBeginValEndLine("PATH", "Carbon.framework");
						WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteEndXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("DYNAMICLIBRARY", "Carbon");
				WriteEndXMLtagLine("FRAMEWORK");

#if UseOpenGL
				WriteBeginXMLtagLine("FRAMEWORK");
					WriteBeginXMLtagLine("FILEREF");
						WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
						WriteXMLtagBeginValEndLine("PATH", "OpenGL.framework");
						WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteEndXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("DYNAMICLIBRARY", "OpenGL");
				WriteEndXMLtagLine("FRAMEWORK");

				WriteBeginXMLtagLine("FRAMEWORK");
					WriteBeginXMLtagLine("FILEREF");
						WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
						WriteXMLtagBeginValEndLine("PATH", "AGL.framework");
						WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteEndXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("DYNAMICLIBRARY", "AGL");
				WriteEndXMLtagLine("FRAMEWORK");
#endif

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
#endif /* UseTargMacho */
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
#if UseTargWinx86
					WriteXMLtagBeginValEndLine("PATH", "main.RC");
#elif UseTargMacho
					WriteXMLtagBeginValEndLine("PATH", "main.plc");
#else
					WriteXMLtagBeginValEndLine("PATH", "main.r");
#endif
				WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
			WriteEndXMLtagLine("FILEREF");
#if UseTargMacho
			{
				WriteMWSource_icnsGroupList("App");
				DoAllDocTypes(WriteDocTypeMWGroupList);
			}
#endif
		WriteEndXMLtagLine("GROUP");
		DoAllSrcFiles(DoSrcFileMW8GroupList);
	WriteEndXMLtagLine("GROUPLIST");

	WriteBlankLineToDestFile();
	WriteEndXMLtagLine("PROJECT");

	WriteCloseDestFile();
}
#endif

#if UseIdeBashGcc
static void DoSrcDependsBgcMakeCompile(char *s)
{
	WriteCStrToDestFile(" $(mk_c_src_d)");
	WriteCStrToDestFile(s);
}
#endif

#if UseIdeBashGcc
static void WriteBgcmkAorCd(blnr AsmAvail)
{
#if ! HaveAsm
	UnusedParam(AsmAvail);
#endif

#if HaveAsm
	if (AsmAvail) {
		WriteCStrToDestFile("$(mk_a_src_d)");
	} else
#endif
	{
		WriteCStrToDestFile("$(mk_c_src_d)");
	}
}
#endif

#if UseIdeBashGcc
static void DoSrcFileBgcMakeCompile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("$(mk_obj_d)");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(".o : ");
	WriteBgcmkAorCd(AsmAvail);
	WriteCStrToDestFile(s);
	WriteSrcSuffix(AsmAvail);
	if (depends != nullpr) {
		depends(DoSrcDependsBgcMakeCompile);
	}
#if HaveAsm
	if (! AsmAvail)
#endif
	{
		WriteCStrToDestFile(" $(mk_config_d)CNFGGLOB.h");
	}
	WriteEndDestFileLn();

	++DestFileIndent;
		WriteBgnDestFileLn();
#if HaveAsm
		if (AsmAvail) {
			WriteCStrToDestFile("$(mk_CallA) ");
		} else
#endif
		{
			WriteCStrToDestFile("$(mk_CallC) ");
		}
		WriteQuoteToDestFile();
		WriteBgcmkAorCd(AsmAvail);
		WriteCStrToDestFile(s);
		WriteSrcSuffix(AsmAvail);
		WriteQuoteToDestFile();
		WriteCStrToDestFile(" -o ");
		WriteQuoteToDestFile();
		WriteCStrToDestFile("$(mk_obj_d)");
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(".o");
		WriteQuoteToDestFile();
#if HaveAsm
		if (AsmAvail) {
			WriteCStrToDestFile(" $(mk_AOptions)");
		} else
#endif
		{
			WriteCStrToDestFile(" $(mk_COptions)");
		}
		WriteEndDestFileLn();
	--DestFileIndent;
}
#endif

#if UseIdeBashGcc
static void DoSrcFileBgcMakeObjects(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBgnDestFileLn();
	WriteCStrToDestFile("$(mk_obj_d)");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(".o ");
	WriteBackSlashToDestFile();
	WriteEndDestFileLn();
}
#endif

#if UseIdeBashGcc && HaveMacBundleApp
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
	WriteCStrToDestFile("cp \"$(mk_osxrz_d)");
	WriteCStrToDestFile(ShortName);
	WriteCStrToDestFile("Icon.icns\" \"$(mk_tresources_d)\"");
	WriteEndDestFileLn();
}
#endif

#if UseIdeBashGcc
static void WriteBashGccSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", "Makefile", "", "Make file");

	WriteDestFileLn("# make file generated by bash configure script");
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_output_d = ");
	Write_output_d_ToDestFile();
	WriteEndDestFileLn();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_source_d = ");
	Write_source_d_ToDestFile();
	WriteEndDestFileLn();
	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_obj_d = $(mk_output_d)obj/");
	WriteDestFileLn("mk_config_d = $(mk_output_d)config/");
#if HaveMacBundleApp
	{
		WriteBgnDestFileLn();
		WriteCStrToDestFile("mk_built_program = $(mk_output_d)");
		WriteMachoAppNameStr();
		WriteCStrToDestFile("/");
		WriteEndDestFileLn();

		WriteDestFileLn("mk_contents_d = $(mk_built_program)Contents/");
		WriteDestFileLn("mk_macos_d = $(mk_contents_d)MacOS/");
		WriteDestFileLn("mk_resources_d = $(mk_contents_d)Resources/");

		WriteBgnDestFileLn();
		WriteCStrToDestFile("mk_built_exec = $(mk_macos_d)");
		WriteCStrToDestFile(kStrAppAbbrev);
		WriteEndDestFileLn();

#if 0
		WriteBgnDestFileLn();
		WriteCStrToDestFile("mk_built_rsrc = $(mk_resources_d)");
		WriteCStrToDestFile(kStrAppAbbrev);
		WriteCStrToDestFile(".rsrc");
		WriteEndDestFileLn();
#endif
	}
#else
	{
		WriteBgnDestFileLn();
		WriteCStrToDestFile("mk_built_program = $(mk_output_d)");
		WriteCStrToDestFile(kStrAppAbbrev);
		WriteEndDestFileLn();
	}
#endif /* ! HaveMacBundleApp */
	WriteBlankLineToDestFile();
#if HaveAsm
		WriteDestFileLn("mk_a_src_d = $(mk_source_d)a_src/ppc/as/");
#endif
	WriteDestFileLn("mk_c_src_d = $(mk_source_d)c_src/");
	WriteDestFileLn("mk_platform_d = $(mk_source_d)platform/");
#if HaveMacBundleApp
	{
		WriteDestFileLn("mk_rz_src_d = $(mk_platform_d)mac/");
		WriteDestFileLn("mk_osxrz_d = $(mk_rz_src_d)osx/");
	}
#endif /* HaveMacBundleApp */
	WriteBlankLineToDestFile();
#if HaveAsm
	{
		WriteDestFileLn("mk_CallA = gcc");
		WriteDestFileLn("mk_AOptions = -c");
	}
#endif
	WriteDestFileLn("mk_CallC = gcc");
	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptions =");
	WriteCommonCOptions(falseblnr);
	WriteEndDestFileLn();
	WriteBlankLineToDestFile();

#if HaveMacBundleApp
#if 0
		WriteDestFileLn("TheApplication : $(mk_built_exec) $(mk_built_rsrc)");
#endif
		WriteDestFileLn("TheApplication : $(mk_built_exec)");
#else
		WriteDestFileLn("TheApplication : $(mk_built_program)");
#endif /* ! HaveMacBundleApp */
	WriteBlankLineToDestFile();
	DoAllSrcFiles(DoSrcFileBgcMakeCompile);
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("ObjFiles = ");
	WriteBackSlashToDestFile();
	WriteEndDestFileLn();
	++DestFileIndent;
		DoAllSrcFiles(DoSrcFileBgcMakeObjects);
		WriteBlankLineToDestFile();
	--DestFileIndent;
#if HaveMacBundleApp
	{
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

			WriteBgnDestFileLn();
			WriteCStrToDestFile("echo -n \"APPL");
			WriteCStrToDestFile(kMacCreatorSig);
			WriteCStrToDestFile("\" > \"$(mk_tcontents_d)PkgInfo\"");
			WriteEndDestFileLn();

			WriteDestFileLn("mv \"$(mk_tbuilt_program)\" \"$(mk_built_program)\"");
		--DestFileIndent;
	}
#endif /* HaveMacBundleApp */
	WriteBlankLineToDestFile();
#if HaveMacBundleApp
		WriteDestFileLn("$(mk_built_exec) : $(ObjFiles) $(mk_resources_d)AppIcon.icns");
#else
		WriteDestFileLn("$(mk_built_program) : $(ObjFiles)");
#endif
	++DestFileIndent;
		WriteDestFileLn("gcc \\");
		++DestFileIndent;
#if HaveMacBundleApp
			{
				WriteDestFileLn("-o \"$(mk_built_exec)\" \\");
				WriteDestFileLn("-framework Carbon \\");
#if UseOpenGL
				WriteDestFileLn("-framework OpenGL \\");
				WriteDestFileLn("-framework AGL \\");
#endif
			}
#else
			{
				WriteDestFileLn("-o \"$(mk_built_program)\" \\");
				WriteBgnDestFileLn();
				WriteCStrToDestFile("-L/usr/X11R6/lib -lXext -lX11");
#if MySoundEnabled
				WriteCStrToDestFile(" -lasound");
#endif
				WriteCStrToDestFile(" \\");
				WriteEndDestFileLn();
			}
#endif /* ! HaveMacBundleApp */
			WriteDestFileLn("$(ObjFiles)");
		--DestFileIndent;
#if UseDbgLvlShip
		{
#if UsingAppleTools
#if HaveMacBundleApp
				WriteDestFileLn("strip -u -r \"$(mk_built_exec)\"");
#else
				WriteDestFileLn("strip -u -r \"$(mk_built_program)\"");
#endif
#endif /* UsingAppleTools */
		}
#endif /* UseDbgLvlShip */
	--DestFileIndent;
#if 0
#if ! UseTargMacX11
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
#endif
#endif
	WriteBlankLineToDestFile();
	WriteDestFileLn("clean :");
	++DestFileIndent;
		WriteDestFileLn("rm -f $(ObjFiles)");
#if HaveMacBundleApp
			WriteDestFileLn("rm -fr \"$(mk_built_program)\"");
#else
			WriteDestFileLn("rm -f \"$(mk_built_program)\"");
#endif
	--DestFileIndent;
	WriteCloseDestFile();
}
#endif

#if UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42
static void WriteMSVCQuotedDefine(char *s)
{
	WriteCStrToDestFile(" /D ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(s);
	WriteQuoteToDestFile();
}
#endif

#if UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42
static void WriteCLexeFlags(void)
{
	WriteCStrToDestFile("/nologo");
#if UseDbgLvlQuick
		WriteCStrToDestFile(" /W4 /Gm /GX /ZI /Od");
#else
		WriteCStrToDestFile(" /ML /W4 /O1");
#endif
	WriteMSVCQuotedDefine("WIN32");
#if UseDbgLvlQuick
		WriteMSVCQuotedDefine("_DEBUG");
#else
		WriteMSVCQuotedDefine("NDEBUG");
#endif
	WriteMSVCQuotedDefine("_WINDOWS");
#if UseIdeMSVC || UseIdeNMake
	WriteMSVCQuotedDefine("_MBCS");
#endif
#if UseDbgLvlQuick
		WriteCStrToDestFile(" /GZ");
#else
		WriteCStrToDestFile(" /GF");
#endif
	WriteCStrToDestFile(" /c");
}
#endif

#if UseIdeMSVC || UseIdeNMake || UseIdeNMakMSVC42
static void WriteRCexeFlags(void)
{
	WriteCStrToDestFile("/l 0x409 /d ");
	WriteQuoteToDestFile();
#if UseDbgLvlQuick
		WriteCStrToDestFile("_DEBUG");
#else
		WriteCStrToDestFile("NDEBUG");
#endif
	WriteQuoteToDestFile();
}
#endif

#if UseIdeMSVC
static void WriteFileToMSVCSource(MyProc p, char *s, char *FileExt)
{
	WriteDestFileLn("# Begin Source File");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("SOURCE=");
	WriteFileInDirToDestFile(p, s, FileExt);
	WriteEndDestFileLn();

	WriteDestFileLn("# End Source File");
}
#endif

#if UseIdeMSVC
static void WriteDocTypeMSVCresource(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	UnusedParam(MacType);
	UnusedParam(LongName);
	UnusedParam(WriteExtensionList);
	WriteFileToMSVCSource(Write_rz_src_ToDestFile, ShortName, "Icon.ico");
}
#endif

#if UseIdeMSVC
static void WriteAddMSVC_config(char *s)
{
	WriteFileToMSVCSource(Write_config_d_ToDestFile, s, ".h");
}
#endif

#if UseIdeMSVC
static void WriteMSVCBeginGroup(char *group, char *filter)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("# Begin Group ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(group);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# PROP Default_Filter ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(filter);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}
#endif

#if UseIdeMSVC
static void WriteMSVCEndGroup(void)
{
	WriteDestFileLn("# End Group");
}
#endif

#if UseIdeMSVC
static void WriteMSVCTargetName(void)
{
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(" - Win32");
	/* WriteAppVariationStr(); */
	WriteQuoteToDestFile();
}
#endif

#if UseIdeMSVC
static void WriteMSVCMakefileName(void)
{
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(".mak");
	WriteQuoteToDestFile();
}
#endif

#if UseIdeMSVC
static void WriteMSVCQuotedProp(char *p, char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("# PROP ");
	WriteCStrToDestFile(p);
	WriteCStrToDestFile(" ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(s);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}
#endif

#if UseIdeMSVC
static void DoSrcFileMSVCAddFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteDestFileLn("# Begin Source File");
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("SOURCE=");
	WriteFileInDirToDestFile(Write_c_src_d_ToDestFile, s, ".c");
	WriteEndDestFileLn();
#if 0
#if ! UseDbgLvlQuick
		if (fast) {
			WriteDestFileLn("# ADD CPP /O2 /Ob2");
		}
#endif
#endif
	WriteDestFileLn("# End Source File");
}
#endif

#if UseIdeMSVC
static void WriteMSVCSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", kStrAppAbbrev, ".dsw", "workspace file");
	WriteDestFileLn("Microsoft Developer Studio Workspace File, Format Version 6.00");
	WriteDestFileLn("# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!");
	WriteBlankLineToDestFile();
	WriteDestFileLn("###############################################################################");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("Project: ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteQuoteToDestFile();
	WriteCStrToDestFile("=.");
	WriteBackSlashToDestFile();
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(".dsp - Package Owner=<4>");
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
	WriteCStrToDestFile("# Microsoft Developer Studio Project File - Name=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" - Package Owner=<4>");
	WriteEndDestFileLn();

	WriteDestFileLn("# Microsoft Developer Studio Generated Build File, Format Version 6.00");
	WriteDestFileLn("# ** DO NOT EDIT **");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# TARGTYPE ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("Win32 (x86) Application");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" 0x0101");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("CFG=");
	/* WriteAppVariationStr(); */
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(" - Win32");
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE This is not a valid makefile. To build this project using NMAKE,");
	WriteDestFileLn("!MESSAGE use the Export Makefile command and run");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("!MESSAGE NMAKE /f ");
	WriteMSVCMakefileName();
	WriteCStrToDestFile(".");
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteDestFileLn("!MESSAGE You can specify a configuration when running NMAKE");
	WriteDestFileLn("!MESSAGE by defining the macro CFG on the command line. For example:");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("!MESSAGE NMAKE /f ");
	WriteMSVCMakefileName();
	WriteCStrToDestFile(" CFG=");
	WriteMSVCTargetName();
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteDestFileLn("!MESSAGE Possible choices for configuration are:");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("!MESSAGE ");
	WriteMSVCTargetName();
	WriteCStrToDestFile(" (based on ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("Win32 (x86) Application");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(")");
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
#if UseDbgLvlQuick
		WriteDestFileLn("# PROP Use_Debug_Libraries 1");
#else
		WriteDestFileLn("# PROP Use_Debug_Libraries 0");
#endif

	WriteMSVCQuotedProp("Output_Dir", "obj");
	WriteMSVCQuotedProp("Intermediate_Dir", "obj");

	WriteDestFileLn("# PROP Ignore_Export_Lib 0");

	WriteMSVCQuotedProp("Target_Dir", "");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od");
	WriteMSVCQuotedDefine("WIN32");
	WriteMSVCQuotedDefine("_DEBUG");
	WriteMSVCQuotedDefine("_WINDOWS");
	WriteMSVCQuotedDefine("_MBCS");
	WriteCStrToDestFile(" /YX /FD /GZ /c");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD CPP ");
	WriteCLexeFlags();
	WriteCStrToDestFile(" /FD");
	WriteCStrToDestFile(" /I ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("config");
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteDestFileLn("# SUBTRACT CPP /YX /Yc /Yu");

	WriteDestFileLn("# ADD BASE MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD MTL /nologo /D ");
	WriteQuoteToDestFile();
#if UseDbgLvlQuick
		WriteCStrToDestFile("_DEBUG");
#else
		WriteCStrToDestFile("NDEBUG");
#endif
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" /mktyplib203 /win32");
	WriteEndDestFileLn();

	WriteDestFileLn("# ADD BASE RSC /l 0x409 /d \"_DEBUG\"");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD RSC ");
	WriteRCexeFlags();
	WriteEndDestFileLn();

	WriteDestFileLn("BSC32=bscmake.exe");
	WriteDestFileLn("# ADD BASE BSC32 /nologo");
	WriteDestFileLn("# ADD BSC32 /nologo");
	WriteDestFileLn("LINK32=link.exe");
	WriteDestFileLn("# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /subsystem:windows");
#if UseDbgLvlQuick
		WriteCStrToDestFile(" /debug");
#else
		WriteCStrToDestFile(" /incremental:no");
#endif
	WriteCStrToDestFile(" /machine:I386 /out:");
	WriteQuoteToDestFile();
	WriteWinAppNameStr();
	WriteQuoteToDestFile();
#if UseDbgLvlQuick
		WriteCStrToDestFile(" /pdbtype:sept");
#endif
	WriteEndDestFileLn();

#if ! UseDbgLvlQuick
		WriteDestFileLn("# SUBTRACT LINK32 /debug /pdbtype:<none>");
#endif
	WriteDestFileLn("# Begin Target");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# Name ");
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
		WriteFileToMSVCSource(Write_rz_src_ToDestFile, "AppIcon", ".ico");
		DoAllDocTypes(WriteDocTypeMSVCresource);
	WriteMSVCEndGroup();

	WriteDestFileLn("# End Target");
	WriteDestFileLn("# End Project");
	WriteCloseDestFile();
}
#endif

#if UseIdeNMake || UseIdeNMakMSVC42
static void DoSrcDependsNMakeCompile(char *s)
{
	WriteCStrToDestFile(" \"$(mk_c_src_d)\\");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile("\"");
}
#endif

#if UseIdeNMake || UseIdeNMakMSVC42
static void DoSrcFileNMakeAddSrcFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);

	WriteBgnDestFileLn();
	WriteCStrToDestFile("\"$(mk_obj_d)\\");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(".obj\" : \"$(mk_c_src_d)\\");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(".c\"");
	if (depends != nullpr) {
		depends(DoSrcDependsNMakeCompile);
	}
	WriteEndDestFileLn();
	++DestFileIndent;
		WriteBgnDestFileLn();
		WriteCStrToDestFile("$(mk_CallC) \"$(mk_c_src_d)\\");
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(".c\" $(mk_COptions)");
		WriteEndDestFileLn();
	--DestFileIndent;
}
#endif

#if UseIdeNMake || UseIdeNMakMSVC42
static void DoSrcFileNMakeAddObjFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);

	WriteBgnDestFileLn();
	WriteCStrToDestFile("\"$(mk_obj_d)\\");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(".obj\" \\");
	WriteEndDestFileLn();
}
#endif

#if UseIdeNMake || UseIdeNMakMSVC42
static void DoSrcFileNMakeEraseFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);

	WriteBgnDestFileLn();
	WriteCStrToDestFile("-@erase \"$(mk_obj_d)\\");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(".obj\"");
	WriteEndDestFileLn();
}
#endif

#if UseIdeNMake || UseIdeNMakMSVC42
static void WriteNMakeSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", "MAKEFILE", "", "Make file");

	WriteDestFileLn("# Microsoft Developer Studio Generated NMAKE File, Based on minivmac.dsp");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_output_d=");
	Write_output_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_source_d=");
	Write_source_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_config_d=$(mk_output_d)\\config");
	WriteDestFileLn("mk_obj_d=$(mk_output_d)\\obj");
	WriteDestFileLn("mk_built_program=$(mk_output_d)\\minivmac.exe");
	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_c_src_d=$(mk_source_d)\\c_src");
	WriteDestFileLn("mk_platform_d=$(mk_source_d)\\platform");
	WriteDestFileLn("mk_rz_src_d=$(mk_platform_d)\\win");
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_CallC=cl.exe");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptions=");
	WriteCLexeFlags();
	WriteCStrToDestFile(" /I \"$(mk_config_d)\" /Fo\"$(mk_obj_d)\\\\\" /Fd\"$(mk_obj_d)\\\\\"");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteDestFileLn("TheApplication : \"$(mk_built_program)\"");
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	DoAllSrcFiles(DoSrcFileNMakeAddSrcFile);
	WriteBlankLineToDestFile();
	WriteDestFileLn("ObjFiles= \\");
	++DestFileIndent;
		DoAllSrcFiles(DoSrcFileNMakeAddObjFile);
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteDestFileLn("\"$(mk_obj_d)\\main.res\" : \"$(mk_rz_src_d)\\main.RC\"");
	++DestFileIndent;
		WriteBgnDestFileLn();
		WriteCStrToDestFile("rc.exe ");
		WriteRCexeFlags();
		WriteCStrToDestFile(" /fo\"$(mk_obj_d)\\main.res\" /i \"$(mk_rz_src_d)\" \"$(mk_rz_src_d)\\main.RC\"");
		WriteEndDestFileLn();
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteDestFileLn("\"$(mk_built_program)\" : $(ObjFiles) \"$(mk_obj_d)\\main.res\"");
	++DestFileIndent;
		WriteDestFileLn("link.exe @<<");
		++DestFileIndent;
			WriteDestFileLn("/out:\"$(mk_built_program)\"");
			WriteDestFileLn("/nologo /subsystem:windows /incremental:no");
			WriteDestFileLn("/pdb:\"$(mk_obj_d)\\minivmac.pdb\" /machine:I386");
			WriteDestFileLn("kernel32.lib");
			WriteDestFileLn("user32.lib");
			WriteDestFileLn("gdi32.lib");
			WriteDestFileLn("winspool.lib");
			WriteDestFileLn("comdlg32.lib");
			WriteDestFileLn("advapi32.lib");
			WriteDestFileLn("shell32.lib");
			WriteDestFileLn("winmm.lib");
			WriteDestFileLn("ole32.lib");
			WriteDestFileLn("oleaut32.lib");
			WriteDestFileLn("uuid.lib");
			WriteDestFileLn("\"$(mk_obj_d)\\main.res\"");
			WriteDestFileLn("$(ObjFiles)");
		--DestFileIndent;
	--DestFileIndent;
	WriteDestFileLn("<<");
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteDestFileLn("CLEAN :");
	++DestFileIndent;
		DoAllSrcFiles(DoSrcFileNMakeEraseFile);
		WriteDestFileLn("-@erase \"$(mk_obj_d)\\main.res\"");
		WriteDestFileLn("-@erase \"$(mk_built_program)\"");
	--DestFileIndent;

	WriteCloseDestFile();
}
#endif

#if UseIdeLccW32
static void DoSrcFileLccAddFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBgnDestFileLn();
	WriteCStrToDestFile("File");
	WriteUnsignedToOutput(++FileCounter);
	WriteCStrToDestFile("=");
	WriteFileInDirToDestFile(Write_c_src_d_ToDestFile, s, ".c");
	WriteEndDestFileLn();
}
#endif

#if UseIdeLccW32
static void WriteLccW32SpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", kStrAppAbbrev, ".prj", "workspace file");

	WriteDestFileLn("; Wedit project file. Syntax: Name = value");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("[");
	WriteAppVariationStr();
	WriteCStrToDestFile("]");
	WriteEndDestFileLn();

	FileCounter = 0;
	DoAllSrcFiles(DoSrcFileIncrFileCounter);
	++FileCounter; /* main.rc */

	WriteBgnDestFileLn();
	WriteCStrToDestFile("PrjFiles=");
	WriteUnsignedToOutput(FileCounter);
	WriteEndDestFileLn();

	FileCounter = 0;
	DoAllSrcFiles(DoSrcFileLccAddFile);

	WriteBgnDestFileLn();
	WriteCStrToDestFile("File");
	WriteUnsignedToOutput(++FileCounter);
	WriteCStrToDestFile("=");
	WriteFileInDirToDestFile(Write_rz_src_ToDestFile, "main", ".rc");
	WriteEndDestFileLn();

	WriteDestFileLn("ProjectFlags=0");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("Name=");
	WriteAppVariationStr();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("ProjectPath=");
	Write_output_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("SourcesDir=");
	/* setting it to my_c_src_d does not work */
	Write_output_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("Includes=");
	Write_output_d_ToDestFile();
	WriteBackSlashToDestFile();
	WriteCStrToDestFile("config");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("MakeDir=");
	Write_obj_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("Exe=");
	Write_output_d_ToDestFile();
	WriteBackSlashToDestFile();
	WriteWinAppNameStr();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("DbgExeName=");
	Write_output_d_ToDestFile();
	WriteBackSlashToDestFile();
	WriteWinAppNameStr();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("DbgDir=");
	Write_obj_d_ToDestFile();
	WriteEndDestFileLn();

#if UseDbgLvlQuick
		WriteDestFileLn("CompilerFlags=6728");
#endif
#if UseDbgLvlTest
		WriteDestFileLn("CompilerFlags=580");
#endif
#if UseDbgLvlShip
		WriteDestFileLn("CompilerFlags=581");
#endif

	WriteDestFileLn("Libraries=shell32.lib winmm.lib ole32.lib uuid.lib");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("ErrorFile=");
	WriteFileInDirToDestFile(Write_obj_d_ToDestFile, kStrAppAbbrev, ".err");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("CurrentFile=");
	WriteFileInDirToDestFile(Write_c_src_d_ToDestFile, "WNOSGLUE", ".h");
	WriteEndDestFileLn();

	WriteDestFileLn("OpenFiles=1");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("OpenFile1=");
	WriteQuoteToDestFile();
	WriteFileInDirToDestFile(Write_c_src_d_ToDestFile, "WNOSGLUE", ".h");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" 1 29 14 532 435");
	WriteEndDestFileLn();

	WriteCloseDestFile();
}
#endif

#if UseIdeDevC
static void DoSrcFileDvcAddFile(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("[Unit");
	WriteUnsignedToOutput(++FileCounter);
	WriteCStrToDestFile("]");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("FileName=");
	WriteFileInDirToDestFile(Write_c_src_d_ToDestFile, s, ".c");
	WriteEndDestFileLn();

	WriteDestFileLn("CompileCpp=0");
	WriteBgnDestFileLn();
	WriteCStrToDestFile("Folder=");
	WriteAppVariationStr();
	WriteEndDestFileLn();
	WriteDestFileLn("Compile=1");
	WriteDestFileLn("Link=1");
	WriteDestFileLn("Priority=1000");
	WriteDestFileLn("OverrideBuildCmd=0");
	WriteDestFileLn("BuildCmd=");
}
#endif

#if UseIdeDevC
static void WriteDevCSpecificFiles(void)
{
	WriteOpenDestFile("my_output_d", kStrAppAbbrev, ".dev", "project file");

	WriteDestFileLn("[Project]");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("FileName=");
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(".dev");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("Name=");
	WriteAppVariationStr();
	WriteEndDestFileLn();

	FileCounter = 0;
	DoAllSrcFiles(DoSrcFileIncrFileCounter);
	++FileCounter; /* main.rc */

	WriteBgnDestFileLn();
	WriteCStrToDestFile("UnitCount=");
	WriteUnsignedToOutput(FileCounter);
	WriteEndDestFileLn();

	WriteDestFileLn("Type=0");
	WriteDestFileLn("Ver=1");
	WriteDestFileLn("Includes=config");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("PrivateResource=");
	WriteAppVariationStr();
	WriteCStrToDestFile("_private.rc");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("ResourceIncludes=");
	Write_rz_src_ToDestFile();
	WriteEndDestFileLn();

	WriteDestFileLn("IsCpp=0");
	WriteDestFileLn("ObjectOutput=obj");
	WriteDestFileLn("OverrideOutput=1");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("OverrideOutputName=");
	WriteWinAppNameStr();
	WriteEndDestFileLn();

	WriteDestFileLn("IncludeVersionInfo=0");
	WriteDestFileLn("CompilerSet=0");
#if UseDbgLvlQuick
	{
		WriteDestFileLn("CompilerSettings=000000000000000100");
		WriteDestFileLn("Compiler= -Wall -Wstrict-prototypes -Wno-uninitialized -O0_@@_");
	}
#else
	{
		WriteDestFileLn("CompilerSettings=000000000000000000");
		WriteDestFileLn("Compiler= -Wall -Wstrict-prototypes -Wno-uninitialized -Os_@@_");
	}
#endif
	WriteDestFileLn("Linker=-lwinmm -lole32 -luuid_@@_");
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
	WriteCStrToDestFile("[Unit");
	WriteUnsignedToOutput(++FileCounter);
	WriteCStrToDestFile("]");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("FileName=");
	WriteFileInDirToDestFile(Write_rz_src_ToDestFile, "main", ".RC");
	WriteEndDestFileLn();

	WriteDestFileLn("CompileCpp=0");
	WriteBgnDestFileLn();
	WriteCStrToDestFile("Folder=");
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
#endif

#if UseIdeAPB
static void WriteAPBBeginObject(unsigned int theClass, unsigned int v)
{
	WriteBgnDestFileLn();
	WriteHexWordToOutput(theClass);
	WriteHexWordToOutput(v);
	WriteCStrToDestFile("0000000000000000");
	WriteCStrToDestFile(" = {");
	WriteEndDestFileLn();
	++DestFileIndent;
}
#endif

#if UseIdeAPB
static void WriteAPBEndObject(void)
{
	--DestFileIndent;
	WriteDestFileLn("};");
}
#endif

#if UseIdeAPB
static void WriteAPBobjlistelm(unsigned int theClass, unsigned int v)
{
	WriteBgnDestFileLn();
	WriteHexWordToOutput(theClass);
	WriteHexWordToOutput(v);
	WriteCStrToDestFile("0000000000000000");
	WriteCStrToDestFile(",");
	WriteEndDestFileLn();
}
#endif

#if UseIdeAPB
static void WriteAPBBgnObjList(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(" = (");
	WriteEndDestFileLn();
	++DestFileIndent;
}
#endif

#if UseIdeAPB
static void WriteAPBEndObjList(void)
{
	--DestFileIndent;
	WriteDestFileLn(");");
}
#endif

#if UseIdeAPB
static void WriteAPBObjRef(char *s, unsigned int theClass, unsigned int v)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(" = ");
	WriteHexWordToOutput(theClass);
	WriteHexWordToOutput(v);
	WriteCStrToDestFile("0000000000000000");
	WriteCStrToDestFile(";");
	WriteEndDestFileLn();
}
#endif

#if UseIdeAPB
static void WriteAPBQuotedField(char *s, char *v)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(" = ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(v);
	WriteQuoteToDestFile();
	WriteCStrToDestFile(";");
	WriteEndDestFileLn();
}
#endif

#if UseIdeAPB
enum {
	APBoclsSrc,
	APBoclsIcns,
	APBoclsLibs,
	APBoclsGen,
	kNumAPBocls
};
#endif

#if UseIdeAPB
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
#endif

#if UseIdeAPB
enum {
	APBliboCarbonRf,
	APBliboCarbonBld,
#if UseOpenGL
	APBliboOpenGLRf,
	APBliboOpenGLBld,
	APBliboAGLRf,
	APBliboAGLBld,
#endif
	APBliboLibStdcRf,
	APBliboLibStdcBld,
	kNumAPBlibo
};
#endif

#if UseIdeAPB
static void DoSrcFileAPBadd(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(depends);
	WriteAPBBeginObject(APBoclsSrc, 2 * FileCounter);
		WriteDestFileLn("fileEncoding = 30;");
		WriteDestFileLn("isa = PBXFileReference;");
		WriteBgnDestFileLn();
		WriteCStrToDestFile("name = ");
		WriteCStrToDestFile(s);
		WriteSrcSuffix(AsmAvail);
		WriteCStrToDestFile(";");
		WriteEndDestFileLn();
		WriteBgnDestFileLn();
		WriteCStrToDestFile("path = ");
#if HaveAsm
		if (AsmAvail) {
			Write_a_src_d_ToDestFile();
		} else
#endif
		{
			Write_c_src_d_ToDestFile();
		}
		WriteCStrToDestFile(s);
		WriteSrcSuffix(AsmAvail);
		WriteCStrToDestFile(";");
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
#endif

#if UseIdeAPB
static void DoSrcFileAPBaddRef(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(s);
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteAPBobjlistelm(APBoclsSrc, 2 * FileCounter);
	++FileCounter;
}
#endif

#if UseIdeAPB
static void DoSrcFileAPBaddBld(char *s, blnr AsmAvail, tDoDependsForC depends)
{
	UnusedParam(s);
	UnusedParam(AsmAvail);
	UnusedParam(depends);
	WriteAPBobjlistelm(APBoclsSrc, 2 * FileCounter + 1);
	++FileCounter;
}
#endif

#if UseIdeAPB
static void WriteAPBplist(void)
{
	int SaveDestFileIndent = DestFileIndent;

	DestFileIndent = 0;

	WriteBgnDestFileLn();
	WriteCStrToDestFile("<!DOCTYPE plist PUBLIC ");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("-//Apple Computer//DTD PLIST 1.0//EN");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" ");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("http://www.apple.com/DTDs/PropertyList-1.0.dtd");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(">");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("<plist version=");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("1.0");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(">");
	WriteEndDestFileLn();

	CurPListFormat = kPListRaw;

	WriteMyInfoPListContents();

	WriteDestFileLn("</plist>");
	WriteBgnDestFileLn();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(";");
	WriteEndDestFileLn();

	DestFileIndent = SaveDestFileIndent;
}
#endif

#if UseIdeAPB
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
		WriteCStrToDestFile("name = ");
		WriteCStrToDestFile(ShortName);
		WriteCStrToDestFile("Icon.icns;");
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("path = ");
		Write_source_d_ToDestFile();
		WriteCStrToDestFile("platform/mac/osx/");
		WriteCStrToDestFile(ShortName);
		WriteCStrToDestFile("Icon.icns;");
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
#endif

#if UseIdeAPB
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
#endif

#if UseIdeAPB
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
#endif

#if UseIdeAPB
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
				WriteCStrToDestFile("path = ");
				Write_source_d_ToDestFile();
				WriteCStrToDestFile("platform/mac/osx/AppIcon.icns;");
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
#if UseOpenGL
			WriteAPBBeginObject(APBoclsLibs, APBliboOpenGLRf);
				WriteDestFileLn("isa = PBXFrameworkReference;");
				WriteDestFileLn("name = OpenGL.framework;");
				WriteDestFileLn("path = /System/Library/Frameworks/OpenGL.framework;");
				WriteDestFileLn("refType = 0;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsLibs, APBliboOpenGLBld);
				WriteAPBObjRef("fileRef", APBoclsLibs, APBliboOpenGLRf);
				WriteDestFileLn("isa = PBXBuildFile;");
				WriteDestFileLn("settings = {");
				WriteDestFileLn("};");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsLibs, APBliboAGLRf);
				WriteDestFileLn("isa = PBXFrameworkReference;");
				WriteDestFileLn("name = AGL.framework;");
				WriteDestFileLn("path = /System/Library/Frameworks/AGL.framework;");
				WriteDestFileLn("refType = 0;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsLibs, APBliboAGLBld);
				WriteAPBObjRef("fileRef", APBoclsLibs, APBliboAGLRf);
				WriteDestFileLn("isa = PBXBuildFile;");
				WriteDestFileLn("settings = {");
				WriteDestFileLn("};");
			WriteAPBEndObject();
#endif
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
				WriteCStrToDestFile("path = ");
				Write_source_d_ToDestFile();
				WriteCStrToDestFile("platform/mac/main.r;");
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
				WriteCStrToDestFile("path = ");
				WriteMachoAppNameStr();
				WriteCStrToDestFile(";");
				WriteEndDestFileLn();
				WriteDestFileLn("refType = 3;");
			WriteAPBEndObject();
			WriteAPBBeginObject(APBoclsGen, APBgenoBuildStyle);
				WriteDestFileLn("buildRules = (");
				WriteDestFileLn(");");
				WriteDestFileLn("buildSettings = {");
				++DestFileIndent;
#if ! UseDbgLvlQuick
						WriteDestFileLn("COPY_PHASE_STRIP = YES;");
#else
						WriteDestFileLn("COPY_PHASE_STRIP = NO;");
						WriteAPBQuotedField("OPTIMIZATION_CFLAGS", "-O0");
#endif
				--DestFileIndent;
				WriteDestFileLn("};");
				WriteDestFileLn("isa = PBXBuildStyle;");
#if UseDbgLvlQuick
					WriteDestFileLn("name = Development;");
#endif
#if UseDbgLvlTest
					WriteDestFileLn("name = Test;");
#endif
#if UseDbgLvlShip
					WriteDestFileLn("name = Deployment;");
#endif
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
#if UseOpenGL
					WriteAPBobjlistelm(APBoclsLibs, APBliboOpenGLRf);
					WriteAPBobjlistelm(APBoclsLibs, APBliboAGLRf);
#endif
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
				WriteCStrToDestFile("name = ");
				WriteQuoteToDestFile();
				WriteAppVariationStr();
				WriteQuoteToDestFile();
				WriteCStrToDestFile(";");
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
#if UseOpenGL
					WriteAPBobjlistelm(APBoclsLibs, APBliboOpenGLBld);
					WriteAPBobjlistelm(APBoclsLibs, APBliboAGLBld);
#endif
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
#if ! UseDbgLvlQuick
						WriteDestFileLn("DEBUGGING_SYMBOLS = NO;");
#endif
					WriteAPBQuotedField("FRAMEWORK_SEARCH_PATHS", "");
					WriteDestFileLn("HEADER_SEARCH_PATHS = config;");
					WriteAPBQuotedField("LIBRARY_SEARCH_PATHS", "");
					WriteAPBQuotedField("OTHER_LDFLAGS", "");
					WriteAPBQuotedField("OTHER_REZFLAGS", "");
					WriteBgnDestFileLn();
					WriteCStrToDestFile("PRODUCT_NAME = ");
					WriteCStrToDestFile(kStrAppAbbrev);
					WriteCStrToDestFile(";");
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
				WriteCStrToDestFile("name = ");
				WriteCStrToDestFile(kStrAppAbbrev);
				WriteCStrToDestFile(";");
				WriteEndDestFileLn();

				WriteAPBQuotedField("productName", kStrAppAbbrev);

				WriteAPBObjRef("productReference", APBoclsGen, APBgenoProductRef);

				WriteBgnDestFileLn();
				WriteCStrToDestFile("productSettingsXML = ");
				WriteQuoteToDestFile();
				WriteCStrToDestFile("<?xml version=");
				WriteBackSlashToDestFile();
				WriteQuoteToDestFile();
				WriteCStrToDestFile("1.0");
				WriteBackSlashToDestFile();
				WriteQuoteToDestFile();
				WriteCStrToDestFile(" encoding=\\\"UTF-8\\\"?>");
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
#endif

static void WriteIdeSpecificFiles(void)
{
#if UseIdeMPW3_6_a1
	WriteMPWSpecificFiles();
#endif
#if UseIdeMW8
	WriteMetrowerksSpecificFiles();
#endif
#if UseIdeBashGcc
	WriteBashGccSpecificFiles();
#endif
#if UseIdeMSVC
	WriteMSVCSpecificFiles();
#endif
#if UseIdeNMake || UseIdeNMakMSVC42
	WriteNMakeSpecificFiles();
#endif
#if UseIdeLccW32
	WriteLccW32SpecificFiles();
#endif
#if UseIdeDevC
	WriteDevCSpecificFiles();
#endif
#if UseIdeAPB
	WriteAPBSpecificFiles();
#endif
}

int main(int argc, char *argv[])
{
	UnusedParam(argc);
	UnusedParam(argv);

#if IsVersionQuerry
	WriteVersionQuerry();
#else

	WriteScriptLangHeader();
	WriteHeaderComments();

	WriteStartScript();

#if TheUseAbsolute
		WriteFindSourceDirectories();
#endif
	WriteMakeOutputDirectories();
	WriteConfigFiles();
#if ThePrintCFiles
		WriteCFilesList();
#endif
	WriteIdeSpecificFiles();

	WriteEndScript();

#endif /* ! IsVersionQuerry */

	return 0;
}
