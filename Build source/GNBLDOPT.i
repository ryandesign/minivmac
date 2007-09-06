/*
	GNBLDOPT.i
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
	GeNeric BuiLD OPTions
*/

/* --- default definitions for SPBASDEF --- */

#ifndef kStrHomePage
#define kStrHomePage "*unknown*"
#endif

#ifndef UseOpenGLinOSX
#define UseOpenGLinOSX 0
#endif

#ifndef UseMachinOSX
#define UseMachinOSX 0
#endif

#ifndef OSXplatsep
#define OSXplatsep 1
#endif

#ifndef HaveExtraRrscs
#define HaveExtraRrscs 0
#endif

#ifndef UseAlignMac68k
#define UseAlignMac68k 0
#endif

/* --- end of default definitions for SPBASDEF --- */

#define nanblnr 2

#define kListOptionAuto (-1)

typedef char * (* tGetName)(int i);

LOCALFUNC blnr FindNamedOption(char *s, int n, tGetName p, int *r)
{
	int i;

	if (! CurArgIsCStr(s)) {
		return trueblnr;
	} else {
		if (kListOptionAuto != *r) {
			ReportParseFailure("This option has already been defined");
		} else {
			AdvanceTheArg();
			for (i = 0; i < n; ++i) {
				if (CurArgIsCStr(p(i))) {
					AdvanceTheArg();
					*r = i;
					goto label_1;
				}
			}
			ReportParseFailure("unknown value for this option");
label_1:
			;
		}
		return falseblnr;
	}
}

LOCALPROC StringResetOption(Handle *r)
{
	if (NULL != *r) {
		DisposHandle(*r);
		*r = NULL;
	}
}

LOCALFUNC blnr StringTryAsOptionNot(char *s, Handle *r)
{
	if (! CurArgIsCStr(s)) {
		return trueblnr;
	} else {
		if (NULL != *r) {
			ReportParseFailure("This option has already been defined");
		} else {
			AdvanceTheArg();
			(void) GetCurArgAsHandle(r);
			AdvanceTheArg();
		}
		return falseblnr;
	}
}

LOCALFUNC blnr BooleanTryAsOptionNot(char *s, blnr *r)
{
	if (! CurArgIsCStr(s)) {
		return trueblnr;
	} else {
		if (nanblnr != *r) {
			ReportParseFailure("This option has already been defined");
		} else {
			AdvanceTheArg();
			if (CurArgIsCStrAdvance("1")) {
				*r = trueblnr;
			} else if (CurArgIsCStrAdvance("0")) {
				*r = falseblnr;
			} else {
				ReportParseFailure("boolean option - expecting 0 or 1");
			}
		}
		return falseblnr;
	}
}

LOCALFUNC blnr FlagTryAsOptionNot(char *s, blnr *r)
{
	if (! CurArgIsCStr(s)) {
		return trueblnr;
	} else {
		if (falseblnr != *r) {
			ReportParseFailure("This option has already been defined");
		} else {
			AdvanceTheArg();
			*r = trueblnr;
		}
		return falseblnr;
	}
}

LOCALFUNC blnr NumberTryAsOptionNot(char *s, long *r, blnr *have_r)
{
	if (! CurArgIsCStr(s)) {
		return trueblnr;
	} else {
		if (*have_r) {
			ReportParseFailure("This option has already been defined");
		} else {
			MyPStr s;

			AdvanceTheArg();
			GetCurArgAsPStr(s);
			StringToNum(s, r);
			AdvanceTheArg();
			*have_r = trueblnr;
		}
		return falseblnr;
	}
}

/* option: target */

enum {
	gbk_targ_m68k, /* MacOS 68K */
	gbk_targ_mfpu, /* MacOS 68K with FPU */
	gbk_targ_mppc, /* MacOS OS 9 for PowerPC */
	gbk_targ_carb, /* MacOS Carbon lib for OS 9 and OS X */
	gbk_targ_mach, /* MacOS X Macho */
	gbk_targ_imch, /* MacOS X Intel */
	gbk_targ_wx86, /* Windows */
	gbk_targ_mx11, /* X11 for MacOS X PowerPC */
	gbk_targ_mi11, /* X11 for MacOS X Intel */
	gbk_targ_lx86, /* X11 for linux on x86 */
	gbk_targ_slrs, /* Solaris SPARC */
	gbk_targ_sl86, /* Solaris Intel */
	kNumTargets
};

LOCALVAR int cur_targ;

LOCALPROC ResetTargetOption(void)
{
	cur_targ = kListOptionAuto;
}

LOCALFUNC char * GetTargetName(int i)
{
	char *s;

	switch (i) {
		case gbk_targ_m68k:
			s = "m68k";
			break;
		case gbk_targ_mfpu:
			s = "mfpu";
			break;
		case gbk_targ_mppc:
			s = "mppc";
			break;
		case gbk_targ_carb:
			s = "carb";
			break;
		case gbk_targ_mach:
			s = "mach";
			break;
		case gbk_targ_imch:
			s = "imch";
			break;
		case gbk_targ_wx86:
			s = "wx86";
			break;
		case gbk_targ_mx11:
			s = "mx11";
			break;
		case gbk_targ_mi11:
			s = "mi11";
			break;
		case gbk_targ_lx86:
			s = "lx86";
			break;
		case gbk_targ_slrs:
			s = "slrs";
			break;
		case gbk_targ_sl86:
			s = "sl86";
			break;
		default:
			s = "(unknown Target)";
			break;
	}
	return s;
}

LOCALFUNC blnr TryAsTargetOptionNot(void)
{
	return FindNamedOption("-t", kNumTargets, GetTargetName, &cur_targ);
}

LOCALFUNC blnr ChooseTarg(void)
{
	if (kListOptionAuto == cur_targ) {
		ReportParseFailure("target not specified ('-t' switch)");
		return falseblnr;
	} else {
		return trueblnr;
	}
}

/* option: ide */

enum {
	gbk_ide_mpw, /* Macintosh Programmers Workshop */
	gbk_ide_mw8, /* Metrowerks CodeWarrior */
	gbk_ide_bgc, /* Gnu tools */
#if SupportWinIDE
	gbk_ide_msv, /* Microsoft Visual C++ */
	gbk_ide_lcc, /* lcc-win32 - Jacob Navia */
	gbk_ide_dvc, /* Bloodshed Dev-C++ */
#endif
	gbk_ide_xcd, /* Apple XCode */
		/* previously Apple Project Builder */
	kNumIdes
};

LOCALVAR int cur_ide;

LOCALPROC ResetIdeOption(void)
{
	cur_ide = kListOptionAuto;
}

LOCALFUNC char * GetIdeName(int i)
{
	char *s;

	switch (i) {
		case gbk_ide_mpw:
			s = "mpw";
			break;
		case gbk_ide_mw8:
			s = "mw8";
			break;
		case gbk_ide_bgc:
			s = "bgc";
			break;
#if SupportWinIDE
		case gbk_ide_msv:
			s = "msv";
			break;
		case gbk_ide_lcc:
			s = "lcc";
			break;
		case gbk_ide_dvc:
			s = "dvc";
			break;
#endif
		case gbk_ide_xcd:
			s = "xcd";
			break;
		default:
			s = "(unknown IDE)";
			break;
	}
	return s;
}

LOCALFUNC blnr TryAsIdeOptionNot(void)
{
	return FindNamedOption("-e", kNumIdes, GetIdeName, &cur_ide);
}

LOCALPROC ChooseIde(void)
{
	if (kListOptionAuto == cur_ide) {
		switch (cur_targ) {
			case gbk_targ_mach:
			case gbk_targ_imch:
			case gbk_targ_mx11:
			case gbk_targ_mi11:
				cur_ide = gbk_ide_xcd;
				break;
			case gbk_targ_m68k:
			case gbk_targ_mfpu:
			case gbk_targ_mppc:
			case gbk_targ_carb:
				cur_ide = gbk_ide_mpw;
				break;
			case gbk_targ_wx86:
				cur_ide = gbk_ide_msv;
				break;
			case gbk_targ_lx86:
			case gbk_targ_slrs:
			case gbk_targ_sl86:
			default:
				cur_ide = gbk_ide_bgc;
				break;
		}
	}
}

/* option: ide version */

LOCALVAR uimr ide_vers;
LOCALVAR blnr have_ide_vers;

LOCALPROC ResetIdeVersOption(void)
{
	have_ide_vers = falseblnr;
}

LOCALFUNC blnr TryAsIdeVersOptionNot(void)
{
	return NumberTryAsOptionNot("-ev", (int *)&ide_vers, &have_ide_vers);
}

LOCALPROC ChooseIdeVers(void)
{
	if (! have_ide_vers) {
		switch (cur_ide) {
			case gbk_ide_xcd:
				ide_vers = 2410;
				break;
			case gbk_ide_msv:
				ide_vers = 8000;
				break;
			default:
				ide_vers = 1;
				break;
		}
		have_ide_vers = trueblnr;
	}
}

/* option: print file list */

LOCALVAR blnr CurPackageOut;

LOCALPROC ResetPackageOut(void)
{
	CurPackageOut = falseblnr;
}

LOCALFUNC blnr TryAsPackageOutNot(void)
{
	return FlagTryAsOptionNot("-pk", &CurPackageOut);
}

/* option: use command line tools */

LOCALVAR blnr UseCmndLine;

LOCALPROC ResetCmndLine(void)
{
	UseCmndLine = falseblnr;
}

LOCALFUNC blnr TryAsCmndLineOptionNot(void)
{
	return FlagTryAsOptionNot("-cl", &UseCmndLine);
}

LOCALPROC ChooseCmndLine(void)
{
	if (! UseCmndLine) {
		UseCmndLine = CurPackageOut;
	}
}

/* derived option: target cpu family */

enum {
	gbk_cpufam_68k, /* Motorola 680x0 */
	gbk_cpufam_ppc, /* PowerPC */
	gbk_cpufam_x86, /* Intel 80x86 */
	gbk_cpufam_spr, /* SPARC */
	kNumCPUFamilies
};

LOCALVAR int gbo_cpufam;

LOCALPROC ChooseCPUFam(void)
{
	switch (cur_targ) {
		case gbk_targ_m68k:
		case gbk_targ_mfpu:
			gbo_cpufam = gbk_cpufam_68k;
			break;
		case gbk_targ_mppc:
		case gbk_targ_carb:
		case gbk_targ_mach:
		case gbk_targ_mx11:
			gbo_cpufam = gbk_cpufam_ppc;
			break;
		case gbk_targ_wx86:
		case gbk_targ_lx86:
		case gbk_targ_sl86:
		case gbk_targ_imch:
		case gbk_targ_mi11:
			gbo_cpufam = gbk_cpufam_x86;
			break;
		case gbk_targ_slrs:
			gbo_cpufam = gbk_cpufam_spr;
			break;
	}
}

/* derived option: api family */

enum {
	gbk_apifam_mac,
	gbk_apifam_osx,
	gbk_apifam_win,
	gbk_apifam_xwn,
	kNumAPIFamilies
};

LOCALVAR int gbo_apifam;

LOCALPROC ChooseAPIFam(void)
{
	switch (cur_targ) {
		case gbk_targ_m68k:
		case gbk_targ_mfpu:
		case gbk_targ_mppc:
			gbo_apifam = gbk_apifam_mac;
			break;
		case gbk_targ_carb:
		case gbk_targ_mach:
		case gbk_targ_imch:
			gbo_apifam = gbk_apifam_osx;
			break;
		case gbk_targ_wx86:
			gbo_apifam = gbk_apifam_win;
			break;
		case gbk_targ_lx86:
		case gbk_targ_slrs:
		case gbk_targ_sl86:
		case gbk_targ_mx11:
		case gbk_targ_mi11:
			gbo_apifam = gbk_apifam_xwn;
			break;
	}
}

/* option: debug level */

enum {
	gbk_dbg_off,
	gbk_dbg_test,
	gbk_dbg_on,
	kNumDebugLevels
};

LOCALVAR int gbo_dbg;

LOCALPROC ResetDbgOption(void)
{
	gbo_dbg = kListOptionAuto;
}

LOCALFUNC char * GetDbgLvlName(int i)
{
	char *s;

	switch (i) {
		case gbk_dbg_on:
			s = "d";
			break;
		case gbk_dbg_test:
			s = "t";
			break;
		case gbk_dbg_off:
			s = "s";
			break;
		default:
			s = "(unknown Debug Level)";
			break;
	}
	return s;
}

LOCALFUNC blnr TryAsDbgOptionNot(void)
{
	return FindNamedOption("-d", kNumDebugLevels, GetDbgLvlName, &gbo_dbg);
}

LOCALPROC ChooseDbgOption(void)
{
	if (kListOptionAuto == gbo_dbg) {
		gbo_dbg = gbk_dbg_off;
	}
}

/* option: language */

enum {
	gbk_lang_eng,
	gbk_lang_fre,
	gbk_lang_ita,
	gbk_lang_ger,
	gbk_lang_dut,
	gbk_lang_spa,
	kNumLangLevels
};

LOCALVAR int gbo_lang;

LOCALPROC ResetLangOption(void)
{
	gbo_lang = kListOptionAuto;
}

LOCALFUNC char * GetLangName(int i)
{
	char *s;

	switch (i) {
		case gbk_lang_eng:
			s = "eng";
			break;
		case gbk_lang_fre:
			s = "fre";
			break;
		case gbk_lang_ita:
			s = "ita";
			break;
		case gbk_lang_ger:
			s = "ger";
			break;
		case gbk_lang_dut:
			s = "dut";
			break;
		case gbk_lang_spa:
			s = "spa";
			break;
		default:
			s = "(unknown Language Level)";
			break;
	}
	return s;
}

LOCALFUNC blnr TryAsLangOptionNot(void)
{
	return FindNamedOption("-lang", kNumLangLevels, GetLangName, &gbo_lang);
}

LOCALFUNC char * GetLProjName(int i)
{
	char *s;

	switch (i) {
		case gbk_lang_eng:
			s = "English";
			break;
		case gbk_lang_fre:
			s = "French";
			break;
		case gbk_lang_ita:
			s = "Italian";
			break;
		case gbk_lang_ger:
			s = "German";
			break;
		case gbk_lang_dut:
			s = "Dutch";
			break;
		case gbk_lang_spa:
			s = "Spanish";
			break;
		default:
			s = "(unknown Language Level)";
			break;
	}
	return s;
}

LOCALPROC ChooseLangOption(void)
{
	if (kListOptionAuto == gbo_lang) {
		gbo_lang = gbk_lang_eng;
	}
}

/* option: export archive */

LOCALVAR blnr NotWantExport;

LOCALPROC ResetWantExport(void)
{
	NotWantExport = falseblnr;
}

LOCALFUNC blnr TryAsWantExportNot(void)
{
	return FlagTryAsOptionNot("-nex", &NotWantExport);
}

/* option: eol source code format */

enum {
	gbk_eol_mac, /* Macintosh */
#if SupportWinIDE
	gbk_eol_win, /* Windows */
#endif
	gbk_eol_unx, /* Unix */
	kNumEol
};

LOCALVAR int cur_eol;

LOCALPROC ResetEolOption(void)
{
	cur_eol = kListOptionAuto;
}

LOCALFUNC char * GetEolName(int i)
{
	char *s;

	switch (i) {
		case gbk_eol_mac:
			s = "mac";
			break;
#if SupportWinIDE
		case gbk_eol_win:
			s = "win";
			break;
#endif
		case gbk_eol_unx:
			s = "unx";
			break;
		default:
			s = "(unknown eol)";
			break;
	}
	return s;
}

LOCALPROC ChooseEOL(void)
{
	if (kListOptionAuto == cur_eol) {
		if (NotWantExport) {
			cur_eol = gbk_eol_mac;
		} else {
			switch (cur_ide) {
				case gbk_ide_mpw:
				case gbk_ide_mw8:
					cur_eol = gbk_eol_mac;
					break;
				case gbk_ide_bgc:
				case gbk_ide_xcd:
					cur_eol = gbk_eol_unx;
					break;
#if SupportWinIDE
				case gbk_ide_msv:
				case gbk_ide_lcc:
				case gbk_ide_dvc:
					cur_eol = gbk_eol_win;
					break;
#endif
			}
		}
	}
}

LOCALFUNC blnr TryAsEolOptionNot(void)
{
	return FindNamedOption("-eol", kNumEol, GetEolName, &cur_eol);
}

/* option: archive format */

enum {
	gbk_arc_sit, /* stuff it */
#if SupportWinIDE
	gbk_arc_zip, /* zip */
#endif
	gbk_arc_tar, /* tar */
	kNumArc
};

LOCALVAR int cur_arc;

LOCALPROC ResetArcOption(void)
{
	cur_arc = kListOptionAuto;
}

LOCALFUNC char * GetArcName(int i)
{
	char *s;

	switch (i) {
		case gbk_arc_sit:
			s = "sit";
			break;
#if SupportWinIDE
		case gbk_arc_zip:
			s = "zip";
			break;
#endif
		case gbk_arc_tar:
			s = "tar";
			break;
		default:
			s = "(unknown archive format)";
			break;
	}
	return s;
}

LOCALPROC ChooseArc(void)
{
	if (kListOptionAuto == cur_arc) {
		switch (cur_ide) {
			case gbk_ide_mpw:
			case gbk_ide_mw8:
				cur_arc = gbk_arc_sit;
				break;
			case gbk_ide_bgc:
			case gbk_ide_xcd:
				cur_arc = gbk_arc_tar;
				break;
#if SupportWinIDE
			case gbk_ide_msv:
			case gbk_ide_lcc:
			case gbk_ide_dvc:
				cur_arc = gbk_arc_zip;
				break;
#endif
		}
	}
}

LOCALFUNC blnr TryAsArcOptionNot(void)
{
	return FindNamedOption("-arc", kNumArc, GetArcName, &cur_arc);
}

/* option: print file list */

LOCALVAR blnr CurPrintCFiles;

LOCALPROC ResetListOption(void)
{
	CurPrintCFiles = falseblnr;
}

LOCALFUNC blnr TryAsListOptionNot(void)
{
	return FlagTryAsOptionNot("-l", &CurPrintCFiles);
}

/* option: maintainer name */

LOCALVAR Handle hMaintainerName = NULL;

LOCALPROC ResetMaintainerName(void)
{
	StringResetOption(&hMaintainerName);
}

LOCALFUNC blnr TryAsMaintainerNameOptionNot(void)
{
	return StringTryAsOptionNot("-maintainer", &hMaintainerName);
}

LOCALPROC ChooseMaintainerName(void)
{
	if (NULL == hMaintainerName) {
		(void) CStr2Hand("unknown", &hMaintainerName);
	}
}

/* option: home page */

LOCALVAR Handle hHomePage = NULL;

LOCALPROC ResetHomePage(void)
{
	StringResetOption(&hHomePage);
}

LOCALFUNC blnr TryAsHomePageOptionNot(void)
{
	return StringTryAsOptionNot("-homepage", &hHomePage);
}

LOCALPROC ChooseHomePage(void)
{
	if (NULL == hHomePage) {
		(void) CStr2Hand(kStrHomePage, &hHomePage);
	}
}

/* derived option: application is os x bundle (folder) */

LOCALVAR blnr HaveMacBundleApp;

LOCALPROC ChooseHaveMacBundleApp(void)
{
	HaveMacBundleApp = ((cur_targ == gbk_targ_mach) || (cur_targ == gbk_targ_imch)) || ((cur_targ == gbk_targ_carb) && (cur_ide == gbk_ide_mpw));
}

/* derived option: have macintosh resources */

LOCALVAR blnr HaveMacRrscs;

LOCALPROC ChooseHaveMacRrscs(void)
{
	HaveMacRrscs = (gbo_apifam == gbk_apifam_mac) || ((cur_targ == gbk_targ_carb) && ! (cur_ide == gbk_ide_mpw)) || HaveExtraRrscs;
}

/* option: Variation Name */

LOCALVAR Handle hVariationName = NULL;

LOCALPROC ResetVariationName(void)
{
	StringResetOption(&hVariationName);
}

LOCALFUNC blnr TryAsVariationNameOptionNot(void)
{
	return StringTryAsOptionNot("-n", &hVariationName);
}

LOCALPROC ChooseVariationName(void)
{
	if (NULL == hVariationName) {
		MyPStr s;

		PStrFromCStr(s, kStrAppAbbrev);
		PStrApndCStr(s, "-");
		PStrApndCStr(s, kMajorVersion);
		PStrApndCStr(s, ".");
		PStrApndCStr(s, kMinorVersion);
		PStrApndCStr(s, ".");
		PStrApndCStr(s, kMinorSubVersion);
		PStrApndCStr(s, "-");
		/* PStrApndCStr(s, GetIdeName(cur_ide)); */
		PStrApndCStr(s, GetTargetName(cur_targ));
		/* PStrApndCStr(s, GetDbgLvlName(gbo_dbg)); */
		(void) PStr2Hand(s, &hVariationName);
	}
}

/* option: IconMaster */

LOCALVAR blnr WantIconMaster;

LOCALPROC ResetIconMaster(void)
{
	WantIconMaster = nanblnr;
}

LOCALFUNC blnr TryAsIconMasterNot(void)
{
	return BooleanTryAsOptionNot("-im", &WantIconMaster);
}

LOCALPROC ChooseIconMaster(void)
{
	if (nanblnr == WantIconMaster) {
		WantIconMaster = falseblnr;
	}
}

/* option: NoAsm */

LOCALVAR blnr WantNoAsm;

LOCALPROC ResetNoAsm(void)
{
	WantNoAsm = falseblnr;
}

LOCALFUNC blnr TryAsNoAsmNot(void)
{
	return FlagTryAsOptionNot("-no-asm", &WantNoAsm);
}

/* derived option: AsmPossible */

LOCALVAR blnr AsmPossible;

LOCALPROC ChooseAsmPossible(void)
{
	AsmPossible = falseblnr;
	if (! WantNoAsm) {
		if (gbo_cpufam == gbk_cpufam_ppc) {
			if (! ((cur_ide == gbk_ide_mw8) && (cur_targ == gbk_targ_mach)))
			if (! ((cur_ide == gbk_ide_xcd) && (! UseCmndLine) && (ide_vers < 1500)))
			{
				AsmPossible = trueblnr;
			}
		}
	}
}


/* --- end of default definition of options --- */

LOCALPROC GNResetCommandLineParameters(void)
{
	ResetTargetOption();
	ResetIdeOption();
	ResetIdeVersOption();
	ResetPackageOut();
	ResetCmndLine();
	ResetDbgOption();
	ResetLangOption();
	ResetEolOption();
	ResetWantExport();
	ResetArcOption();
	ResetListOption();
	ResetMaintainerName();
	ResetHomePage();
	ResetVariationName();
	ResetIconMaster();
	ResetNoAsm();
}

LOCALFUNC blnr TryAsGNOptionNot(void)
{
	if (TryAsTargetOptionNot())
	if (TryAsIdeOptionNot())
	if (TryAsIdeVersOptionNot())
	if (TryAsPackageOutNot())
	if (TryAsCmndLineOptionNot())
	if (TryAsDbgOptionNot())
	if (TryAsEolOptionNot())
	if (TryAsWantExportNot())
	if (TryAsArcOptionNot())
	if (TryAsLangOptionNot())
	if (TryAsMaintainerNameOptionNot())
	if (TryAsHomePageOptionNot())
	if (TryAsListOptionNot())
	if (TryAsVariationNameOptionNot())
	if (TryAsIconMasterNot())
	if (TryAsNoAsmNot())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALFUNC blnr AutoChooseGNSettings(void)
{
	if (ChooseTarg())
	{
		ChooseIde();
		ChooseIdeVers();
		ChooseCmndLine();
		ChooseCPUFam();
		ChooseAPIFam();
		ChooseDbgOption();
		ChooseLangOption();
		ChooseEOL();
		ChooseArc();
		ChooseMaintainerName();
		ChooseHomePage();
		ChooseHaveMacBundleApp();
		ChooseHaveMacRrscs();
		ChooseVariationName();
		ChooseIconMaster();
		ChooseAsmPossible();
		return trueblnr;
	}

	return falseblnr;
}
