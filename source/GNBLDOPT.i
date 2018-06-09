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

#ifndef MayUseSound
#define MayUseSound 1
#endif

#ifndef UseOpenGLinOSX
#define UseOpenGLinOSX 0
#endif

#ifndef UseMachinOSX
#define UseMachinOSX 0
#endif

#ifndef NeedIntFormatInfo
#define NeedIntFormatInfo 0
#endif

#ifndef ModPPCi3rTypes
#define ModPPCi3rTypes 0
#endif

/* --- end of default definitions for SPBASDEF --- */

LOCALVAR blnr OnlyUserOptions = falseblnr;
LOCALVAR blnr DoingDevOpts = falseblnr;

LOCALVAR ui3r olv_cur;

LOCALFUNC tMyErr CurArgIsOption(char *s, ui3r *olv)
{
	tMyErr err;
	MyPStr t;

	if (! CurArgIsCStr_v2(s)) {
		err = kMyErrNoMatch;
	} else
	if (DoingDevOpts && OnlyUserOptions) {
		PStrFromCStr(t, s);
		PStrApndCStr(t, " is a developer only option");
		err = ReportParseFailPStr(t);
	} else
	if (*olv == olv_cur) {
		PStrFromCStr(t, s);
		PStrApndCStr(t, " has appeared more than once");
		err = ReportParseFailPStr(t);
	} else
	if (kMyErr_noErr != (err = AdvanceTheArg())) {
		/* fail */
	} else
	{
		*olv = olv_cur;
		err = kMyErr_noErr;
	}

	return err;
}

typedef char * (* tGetName)(int i);

LOCALFUNC blnr GetCurArgNameIndex(int n, tGetName p,
	int *r)
{
	blnr v;
	int i;

	for (i = 0; i < n; ++i) {
		if (CurArgIsCStr_v2(p(i))) {
			*r = i;
			v = trueblnr;
			goto label_1;
		}
	}
	v = falseblnr;

label_1:
	return v;
}

#define nanblnr 2

#define kListOptionAuto (-1)

LOCALFUNC tMyErr FindNamedOption(char *s, int n, tGetName p,
	int *r, ui3r *olv)
{
	tMyErr err;
	MyPStr t;

	if (kMyErr_noErr != (err = CurArgIsOption(s, olv))) {
		/* no */
	} else
	if (The_arg_end) {
		PStrFromCStr(t, "Expecting an argument for ");
		PStrApndCStr(t, s);
		PStrApndCStr(t, " when reached end");
		err = ReportParseFailPStr(t);
	} else
	if (GetCurArgNameIndex(n, p, r)) {
		err = AdvanceTheArg();
	} else
	if (CurArgIsCStr_v2("*")) {
		*r = kListOptionAuto;
		err = AdvanceTheArg();
	} else
	{
		PStrFromCStr(t, "Unknown value for ");
		PStrApndCStr(t, s);
		err = ReportParseFailPStr(t);
	}

	return err;
}

LOCALPROC StringResetOption(Handle *r)
{
	if (NULL != *r) {
		DisposeHandle(*r);
		*r = NULL;
	}
}

LOCALFUNC tMyErr StringTryAsOptionNot(char *s, Handle *r, ui3r *olv)
{
	tMyErr err;
	MyPStr t;

	if (kMyErr_noErr != (err = CurArgIsOption(s, olv))) {
		/* no */
	} else
	if (The_arg_end) {
		PStrFromCStr(t, "Expecting an argument for ");
		PStrApndCStr(t, s);
		PStrApndCStr(t, " when reached end");
		err = ReportParseFailPStr(t);
	} else
	{
		StringResetOption(r);
		if (kMyErr_noErr != (err = GetCurArgAsHandle_v2(r))) {
			/* fail */
		} else
		{
			err = AdvanceTheArg();
		}
	}

	return err;
}

LOCALFUNC tMyErr BooleanTryAsOptionNot(char *s, blnr *r, ui3r *olv)
{
	tMyErr err;
	MyPStr t;

	if (kMyErr_noErr != (err = CurArgIsOption(s, olv))) {
		/* no */
	} else
	if (The_arg_end) {
		PStrFromCStr(t, "Expecting a boolean argument for ");
		PStrApndCStr(t, s);
		PStrApndCStr(t, " when reached end");
		err = ReportParseFailPStr(t);
	} else
	if (CurArgIsCStr_v2("1")) {
		*r = trueblnr;
		err = AdvanceTheArg();
	} else
	if (CurArgIsCStr_v2("0")) {
		*r = falseblnr;
		err = AdvanceTheArg();
	} else
	if (CurArgIsCStr_v2("*")) {
		*r = nanblnr;
		err = AdvanceTheArg();
	} else
	{
		PStrFromCStr(t, "Expecting a boolean argument for ");
		PStrApndCStr(t, s);
		err = ReportParseFailPStr(t);
	}

	return err;
}

LOCALFUNC tMyErr FlagTryAsOptionNot(char *s, blnr *r, ui3r *olv)
{
	tMyErr err;

	if (kMyErr_noErr != (err = CurArgIsOption(s, olv))) {
		/* no */
	} else
	{
		err = kMyErr_noErr;
		*r = trueblnr;
	}

	return err;
}

LOCALFUNC tMyErr GetCurArgOptionAsNumber(char *s, long *r)
{
	tMyErr err;
	MyPStr t0;
	MyPStr t;

	if (The_arg_end) {
		PStrFromCStr(t, "Expecting a number argument for ");
		PStrApndCStr(t, s);
		PStrApndCStr(t, " when reached end");
		err = ReportParseFailPStr(t);
	} else {
		GetCurArgAsPStr(t0);
		StringToNum(t0, r);
		NumToString(*r, t);
		if (! PStrEq(t0, t)) {
			PStrFromCStr(t, "Expecting a number argument for ");
			PStrApndCStr(t, s);
			PStrApndCStr(t, " but got ");
			PStrAppend(t, t0);
			err = ReportParseFailPStr(t);
		} else
		{
			err = AdvanceTheArg();
		}
	}

	return err;
}

LOCALFUNC tMyErr NumberTryAsOptionNot(char *s, long *r, ui3r *olv)
{
	tMyErr err;

	if (kMyErr_noErr != (err = CurArgIsOption(s, olv))) {
		/* no */
	} else
	if (kMyErr_noErr != (err = GetCurArgOptionAsNumber(s, r))) {
		/* fail */
	} else
	{
		err = kMyErr_noErr;
	}

	return err;
}

LOCALPROC WrtOptNamedOption(char *s, tGetName p, int i, int i0)
{
	if (i != i0) {
		strmo_writeCStr(" ");
		strmo_writeCStr(s);
		strmo_writeCStr(" ");
		strmo_writeCStr(p(i));
	}
}

LOCALPROC WrtOptNumberOption(char *s, int i, int i0)
{
	if (i != i0) {
		strmo_writeCStr(" ");
		strmo_writeCStr(s);
		strmo_writeCStr(" ");
		strmo_writeUimr(i);
	}
}

LOCALPROC WrtOptSimrOption(char *s, simr i, simr i0)
{
	if (i != i0) {
		strmo_writeCStr(" ");
		strmo_writeCStr(s);
		strmo_writeCStr(" ");
		strmo_writeNum(i);
	}
}

LOCALPROC WrtOptBooleanOption(char *s, blnr i, blnr i0)
{
	if (i != i0) {
		strmo_writeCStr(" ");
		strmo_writeCStr(s);
		strmo_writeCStr(" ");
		strmo_writeCStr(i ? "1" : "0");
	}
}

LOCALPROC WrtOptFlagOption(char *s, blnr v)
{
	if (v) {
		strmo_writeCStr(" ");
		strmo_writeCStr(s);
	}
}


/* option: Branch */

LOCALVAR uimr Branch;
LOCALVAR ui3r olv_Branch;

LOCALPROC ResetBranchOption(void)
{
	olv_Branch = 0;
}

LOCALFUNC tMyErr TryAsBranchOptionNot(void)
{
	return NumberTryAsOptionNot("-br",
		(long *)&Branch, &olv_Branch);
}

LOCALFUNC tMyErr ChooseBranch(void)
{
	if (0 == olv_Branch) {
		Branch = MajorVersion;
	}

	return noErr;
}

LOCALPROC WrtOptBranchOption(void)
{
	strmo_writeCStr("-br");
	strmo_writeCStr(" ");
	strmo_writeUimr(MajorVersion);
}


/* option: target */

enum {
	gbk_targ_m68k, /* MacOS 68K */
	gbk_targ_mfpu, /* MacOS 68K with FPU */
	gbk_targ_mppc, /* MacOS OS 9 for PowerPC */
	gbk_targ_mach, /* MacOS X Macho */
	gbk_targ_imch, /* MacOS X Intel */
	gbk_targ_mc64, /* MacOS X for x64 */
	gbk_targ_wx86, /* Windows */
	gbk_targ_wx64, /* Windows on x64 */
	gbk_targ_lx86, /* X11 for linux on x86 */
	gbk_targ_lppc, /* X11 for linux on PowerPC */
	gbk_targ_lx64, /* X11 for linux on x64 */
	gbk_targ_larm, /* X11 for linux on arm (debian armel) */
	gbk_targ_lspr, /* X11 for linux on SPARC */
	gbk_targ_fbsd, /* FreeBSD for x86 */
	gbk_targ_fb64, /* FreeBSD for x64 */
	gbk_targ_fbpc, /* FreeBSD for PowerPC */
	gbk_targ_obsd, /* OpenBSD for x86 */
	gbk_targ_ob64, /* OpenBSD for x64 */
	gbk_targ_nbsd, /* NetBSD for x86 */
	gbk_targ_nb64, /* NetBSD for x64 */
	gbk_targ_dbsd, /* Dragonfly BSD for x86 */
	gbk_targ_db64, /* Dragonfly BSD for x64 */
	gbk_targ_slrs, /* Solaris SPARC */
	gbk_targ_sl86, /* Solaris Intel */
	gbk_targ_oind, /* OpenIndiana for x86 */
	gbk_targ_oi64, /* OpenIndiana for x64 */
	gbk_targ_minx, /* Minix on x86 */
	gbk_targ_wcar, /* Windows CE on ARM */
	gbk_targ_wc86, /* Windows CE (emulator) on x86 */
	gbk_targ_carb, /* MacOS Carbon lib for OS 9 and OS X */
	gbk_targ_mx11, /* X11 for MacOS X PowerPC */
	gbk_targ_mi11, /* X11 for MacOS X Intel */
	gbk_targ_mx64, /* X11 for MacOS X x64 */
	gbk_targ_cygw, /* Cygwin/X */
	gbk_targ_xgen, /* Generic X11 */
	gbk_targ_ndsa, /* Nintendo DS on ARM  */
	gbk_targ_irix, /* Silicon Graphics's IRIX on MIPS */
	kNumTargets
};

LOCALVAR int cur_targ;
LOCALVAR ui3r olv_targ;

LOCALPROC ResetTargetOption(void)
{
	cur_targ = kListOptionAuto;
	olv_targ = 0;
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
		case gbk_targ_mc64:
			s = "mc64";
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
		case gbk_targ_mx64:
			s = "mx64";
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
		case gbk_targ_fbsd:
			s = "fbsd";
			break;
		case gbk_targ_fb64:
			s = "fb64";
			break;
		case gbk_targ_fbpc:
			s = "fbpc";
			break;
		case gbk_targ_obsd:
			s = "obsd";
			break;
		case gbk_targ_ob64:
			s = "ob64";
			break;
		case gbk_targ_nbsd:
			s = "nbsd";
			break;
		case gbk_targ_nb64:
			s = "nb64";
			break;
		case gbk_targ_dbsd:
			s = "dbsd";
			break;
		case gbk_targ_db64:
			s = "db64";
			break;
		case gbk_targ_oind:
			s = "oind";
			break;
		case gbk_targ_oi64:
			s = "oi64";
			break;
		case gbk_targ_minx:
			s = "minx";
			break;
		case gbk_targ_wcar:
			s = "wcar";
			break;
		case gbk_targ_wc86:
			s = "wc86";
			break;
		case gbk_targ_lppc:
			s = "lppc";
			break;
		case gbk_targ_lx64:
			s = "lx64";
			break;
		case gbk_targ_wx64:
			s = "wx64";
			break;
		case gbk_targ_larm:
			s = "larm";
			break;
		case gbk_targ_lspr:
			s = "lspr";
			break;
		case gbk_targ_cygw:
			s = "cygw";
			break;
		case gbk_targ_xgen:
			s = "xgen";
			break;
		case gbk_targ_ndsa:
			s = "ndsa";
			break;
		case gbk_targ_irix:
			s = "irix";
			break;
		default:
			s = "(unknown Target)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr TryAsTargetOptionNot(void)
{
	return FindNamedOption("-t", kNumTargets, GetTargetName,
		&cur_targ, &olv_targ);
}

LOCALFUNC tMyErr ChooseTarg(void)
{
	tMyErr err;

	if (kListOptionAuto == cur_targ) {
		err = ReportParseFailure("target not specified ('-t' switch)");
	} else {
		err = noErr;
	}

	return err;
}

LOCALPROC WrtOptTarg(void)
{
	strmo_writeCStr(" ");
	strmo_writeCStr("-t");
	strmo_writeCStr(" ");
	strmo_writeCStr(GetTargetName(cur_targ));
}


/* option: debug level */

enum {
	gbk_dbg_off,
	gbk_dbg_test,
	gbk_dbg_on,
	kNumDebugLevels
};

LOCALVAR int gbo_dbg;
LOCALVAR ui3r olv_dbg;

LOCALPROC ResetDbgOption(void)
{
	gbo_dbg = kListOptionAuto;
	olv_dbg = 0;
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

LOCALFUNC tMyErr TryAsDbgOptionNot(void)
{
	return FindNamedOption("-d",
		kNumDebugLevels, GetDbgLvlName, &gbo_dbg, &olv_dbg);
}

#define dfo_dbg() gbk_dbg_off

LOCALFUNC tMyErr ChooseDbgOption(void)
{
	if (kListOptionAuto == gbo_dbg) {
		gbo_dbg = dfo_dbg();
	}

	return noErr;
}

LOCALPROC WrtOptDbgOption(void)
{
	WrtOptNamedOption("-d", GetDbgLvlName, gbo_dbg, dfo_dbg());
}


/* option: language */

enum {
	gbk_lang_eng,
	gbk_lang_fre,
	gbk_lang_ita,
	gbk_lang_ger,
	gbk_lang_dut,
	gbk_lang_spa,
	gbk_lang_pol,
	gbk_lang_ptb,
	gbk_lang_cat,
	gbk_lang_cze,
	kNumLangLevels
};

LOCALVAR int gbo_lang;
LOCALVAR ui3r olv_lang;

LOCALPROC ResetLangOption(void)
{
	gbo_lang = kListOptionAuto;
	olv_lang = 0;
}

LOCALFUNC char * GetLangName(int i)
{
	/* ISO 639-2/B */
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
		case gbk_lang_pol:
			s = "pol";
			break;
		case gbk_lang_ptb:
			s = "ptb";
			break;
		case gbk_lang_cat:
			s = "cat";
			break;
		case gbk_lang_cze:
			s = "cze";
			break;
		default:
			s = "(unknown Language Level)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr TryAsLangOptionNot(void)
{
	return FindNamedOption("-lang",
		kNumLangLevels, GetLangName, &gbo_lang, &olv_lang);
}

LOCALFUNC char * GetLProjName(int i)
{
	/*
		As used in OS X, IETF language tags, except when not
	*/
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
		case gbk_lang_pol:
			s = "pl";
			break;
		case gbk_lang_ptb:
			s = "pt_BR";
			break;
		case gbk_lang_cat:
			s = "ca";
			break;
		case gbk_lang_cze:
			s = "cs";
			break;
		default:
			s = "(unknown Language Level)";
			break;
	}
	return s;
}

#define dfo_lang() gbk_lang_eng

LOCALFUNC tMyErr ChooseLangOption(void)
{
	if (kListOptionAuto == gbo_lang) {
		gbo_lang = dfo_lang();
	}

	return noErr;
}

LOCALPROC WrtOptLangOption(void)
{
	WrtOptNamedOption("-lang", GetLangName, gbo_lang, dfo_lang());
}


/* option: IconMaster */

#ifndef WantIconMasterDflt
#define WantIconMasterDflt falseblnr
#endif

LOCALVAR blnr WantIconMaster;
LOCALVAR ui3r olv_IconMaster;

LOCALPROC ResetIconMaster(void)
{
	WantIconMaster = nanblnr;
	olv_IconMaster = 0;
}

LOCALFUNC tMyErr TryAsIconMasterNot(void)
{
	return BooleanTryAsOptionNot("-im",
		&WantIconMaster, &olv_IconMaster);
}

LOCALFUNC tMyErr ChooseIconMaster(void)
{
	if (nanblnr == WantIconMaster) {
		WantIconMaster = WantIconMasterDflt;
	}

	return noErr;
}

LOCALPROC WrtOptIconMaster(void)
{
	WrtOptBooleanOption("-im", WantIconMaster, WantIconMasterDflt);
}


/* option: Test Compile Time Error */

LOCALVAR blnr gbo_TstCompErr;
LOCALVAR ui3r olv_TstCompErr;

LOCALPROC ResetTstCompErr(void)
{
	gbo_TstCompErr = nanblnr;
	olv_TstCompErr = 0;
}

LOCALFUNC tMyErr TryAsTstCompErrNot(void)
{
	return BooleanTryAsOptionNot("-cte",
		&gbo_TstCompErr, &olv_TstCompErr);
}

#define dfo_TstCompErr() falseblnr

LOCALFUNC tMyErr ChooseTstCompErr(void)
{
	if (nanblnr == gbo_TstCompErr) {
		gbo_TstCompErr = dfo_TstCompErr();
	}

	return noErr;
}

LOCALPROC WrtOptTstCompErr(void)
{
	WrtOptBooleanOption("-cte", gbo_TstCompErr, dfo_TstCompErr());
}


/* option: Test Build System Error */

LOCALFUNC tMyErr TryAsTstBldSysErr(void)
{
	tMyErr err;

	if (! CurArgIsCStr_v2("-bte")) {
		err = kMyErrNoMatch;
	} else {
		err = ReportParseFailure("Testing Build System Error");
	}

	return err;
}


/* option: target cpu family */

enum {
	gbk_cpufam_68k, /* Motorola 680x0 */
	gbk_cpufam_ppc, /* PowerPC */
	gbk_cpufam_x86, /* Intel 80x86 */
	gbk_cpufam_spr, /* SPARC */
	gbk_cpufam_arm, /* ARM */
	gbk_cpufam_x64, /* x86-64 (aka AMD64, Intel 64) */
	gbk_cpufam_mip, /* MIPS */
	gbk_cpufam_gen, /* Generic (don't know) */
	kNumCPUFamilies
};

LOCALVAR int gbo_cpufam;
LOCALVAR ui3r olv_cpufam;

LOCALPROC ResetCPUFamOption(void)
{
	gbo_cpufam = kListOptionAuto;
	olv_cpufam = 0;
}

LOCALFUNC char * GetCPUFamName(int i)
{
	char *s;

	switch (i) {
		case gbk_cpufam_68k:
			s = "68k";
			break;
		case gbk_cpufam_ppc:
			s = "ppc";
			break;
		case gbk_cpufam_x86:
			s = "x86";
			break;
		case gbk_cpufam_spr:
			s = "spr";
			break;
		case gbk_cpufam_arm:
			s = "arm";
			break;
		case gbk_cpufam_x64:
			s = "x64";
			break;
		case gbk_cpufam_mip:
			s = "mip";
			break;
		case gbk_cpufam_gen:
			s = "gen";
			break;
		default:
			s = "(unknown CPU)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr TryAsCPUFamOptionNot(void)
{
	return FindNamedOption("-cpu",
		kNumCPUFamilies, GetCPUFamName, &gbo_cpufam, &olv_cpufam);
}

LOCALFUNC int dfo_cpufam(void)
{
	int v;

	switch (cur_targ) {
		case gbk_targ_m68k:
		case gbk_targ_mfpu:
			v = gbk_cpufam_68k;
			break;
		case gbk_targ_mppc:
		case gbk_targ_carb:
		case gbk_targ_mach:
		case gbk_targ_mx11:
		case gbk_targ_lppc:
		case gbk_targ_fbpc:
			v = gbk_cpufam_ppc;
			break;
		case gbk_targ_wx86:
		case gbk_targ_wc86:
		case gbk_targ_lx86:
		case gbk_targ_sl86:
		case gbk_targ_fbsd:
		case gbk_targ_obsd:
		case gbk_targ_nbsd:
		case gbk_targ_dbsd:
		case gbk_targ_oind:
		case gbk_targ_minx:
		case gbk_targ_imch:
		case gbk_targ_mi11:
		case gbk_targ_cygw:
			v = gbk_cpufam_x86;
			break;
		case gbk_targ_lspr:
		case gbk_targ_slrs:
			v = gbk_cpufam_spr;
			break;
		case gbk_targ_wcar:
		case gbk_targ_ndsa:
		case gbk_targ_larm:
			v = gbk_cpufam_arm;
			break;
		case gbk_targ_mc64:
		case gbk_targ_lx64:
		case gbk_targ_wx64:
		case gbk_targ_fb64:
		case gbk_targ_ob64:
		case gbk_targ_nb64:
		case gbk_targ_db64:
		case gbk_targ_oi64:
		case gbk_targ_mx64:
			v = gbk_cpufam_x64;
			break;
		case gbk_targ_irix:
			v = gbk_cpufam_mip;
			break;
		case gbk_targ_xgen:
			v = gbk_cpufam_gen;
			break;
	}

	return v;
}

LOCALFUNC tMyErr ChooseCPUFam(void)
{
	if (kListOptionAuto == gbo_cpufam) {
		gbo_cpufam = dfo_cpufam();
	}

	return noErr;
}

LOCALPROC WrtOptCPUFam(void)
{
	WrtOptNamedOption("-cpu", GetCPUFamName, gbo_cpufam, dfo_cpufam());
}

/* derived option: target family */

enum {
	gbk_targfam_cmac, /* Classic Mac */
	gbk_targfam_mach, /* OS X Macho */
	gbk_targfam_mswn, /* Microsoft Windows */
	gbk_targfam_linx, /* Linux */
	gbk_targfam_fbsd, /* FreeBSD */
	gbk_targfam_obsd, /* OpenBSD */
	gbk_targfam_nbsd, /* NetBSD */
	gbk_targfam_dbsd, /* Dragonfly BSD */
	gbk_targfam_slrs, /* Solaris */
	gbk_targfam_oind, /* OpenIndiana */
	gbk_targfam_irix, /* Silicon Graphics's IRIX */
	gbk_targfam_minx, /* Minix */
	gbk_targfam_wnce, /* Windows CE */
	gbk_targfam_carb, /* MacOS Carbon lib for OS 9 and OS X */
	gbk_targfam_mx11, /* X11 for MacOS X */
	gbk_targfam_cygw, /* Cygwin/X */
	gbk_targfam_xgen, /* Generic X11 */
	gbk_targfam_lnds, /* libnds for Nintendo DS */
	kNumTargFamilies
};

LOCALVAR int gbo_targfam;

LOCALFUNC tMyErr ChooseTargFam(void)
{
	switch (cur_targ) {
		case gbk_targ_m68k:
		case gbk_targ_mfpu:
		case gbk_targ_mppc:
			gbo_targfam = gbk_targfam_cmac;
			break;
		case gbk_targ_mach:
		case gbk_targ_imch:
		case gbk_targ_mc64:
			gbo_targfam = gbk_targfam_mach;
			break;
		case gbk_targ_wx86:
		case gbk_targ_wx64:
			gbo_targfam = gbk_targfam_mswn;
			break;
		case gbk_targ_lx86:
		case gbk_targ_lppc:
		case gbk_targ_lx64:
		case gbk_targ_larm:
		case gbk_targ_lspr:
			gbo_targfam = gbk_targfam_linx;
			break;
		case gbk_targ_slrs:
		case gbk_targ_sl86:
			gbo_targfam = gbk_targfam_slrs;
			break;
		case gbk_targ_fbsd:
		case gbk_targ_fb64:
		case gbk_targ_fbpc:
			gbo_targfam = gbk_targfam_fbsd;
			break;
		case gbk_targ_obsd:
		case gbk_targ_ob64:
			gbo_targfam = gbk_targfam_obsd;
			break;
		case gbk_targ_nbsd:
		case gbk_targ_nb64:
			gbo_targfam = gbk_targfam_nbsd;
			break;
		case gbk_targ_dbsd:
		case gbk_targ_db64:
			gbo_targfam = gbk_targfam_dbsd;
			break;
		case gbk_targ_oind:
		case gbk_targ_oi64:
			gbo_targfam = gbk_targfam_oind;
			break;
		case gbk_targ_minx:
			gbo_targfam = gbk_targfam_minx;
			break;
		case gbk_targ_irix:
			gbo_targfam = gbk_targfam_irix;
			break;
		case gbk_targ_wcar:
		case gbk_targ_wc86:
			gbo_targfam = gbk_targfam_wnce;
			break;
		case gbk_targ_carb:
			gbo_targfam = gbk_targfam_carb;
			break;
		case gbk_targ_mx11:
		case gbk_targ_mi11:
		case gbk_targ_mx64:
			gbo_targfam = gbk_targfam_mx11;
			break;
		case gbk_targ_cygw:
			gbo_targfam = gbk_targfam_cygw;
			break;
		case gbk_targ_ndsa:
			gbo_targfam = gbk_targfam_lnds;
			break;
		case gbk_targ_xgen:
		default:
			gbo_targfam = gbk_targfam_xgen;
			break;
	}

	return noErr;
}


/* option: api family */

enum {
	gbk_apifam_mac,
	gbk_apifam_osx,
	gbk_apifam_win,
	gbk_apifam_xwn,
	gbk_apifam_nds,
	gbk_apifam_gtk,
	gbk_apifam_sdl,
	gbk_apifam_sd2,
	gbk_apifam_cco,
	kNumAPIFamilies
};

LOCALVAR int gbo_apifam;
LOCALVAR ui3r olv_apifam;

LOCALPROC ResetAPIFamOption(void)
{
	gbo_apifam = kListOptionAuto;
	olv_apifam = 0;
}

LOCALFUNC char * GetAPIFamName(int i)
{
	char *s;

	switch (i) {
		case gbk_apifam_mac:
			s = "mac";
			break;
		case gbk_apifam_osx:
			s = "osx";
			break;
		case gbk_apifam_win:
			s = "win";
			break;
		case gbk_apifam_xwn:
			s = "xwn";
			break;
		case gbk_apifam_nds:
			s = "nds";
			break;
		case gbk_apifam_gtk:
			s = "gtk";
			break;
		case gbk_apifam_sdl:
			s = "sdl";
			break;
		case gbk_apifam_sd2:
			s = "sd2";
			break;
		case gbk_apifam_cco:
			s = "cco";
			break;
		default:
			s = "(unknown API)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr TryAsAPIFamOptionNot(void)
{
	return FindNamedOption("-api",
		kNumAPIFamilies, GetAPIFamName, &gbo_apifam, &olv_apifam);
}

LOCALFUNC int dfo_apifam(void)
{
	int v;

	switch (gbo_targfam) {
		case gbk_targfam_cmac:
			v = gbk_apifam_mac;
			break;
		case gbk_targfam_mach:
		case gbk_targfam_carb:
			if (gbk_cpufam_x64 == gbo_cpufam) {
				v = gbk_apifam_cco;
			} else {
				v = gbk_apifam_osx;
			}
			break;
		case gbk_targfam_mswn:
		case gbk_targfam_wnce:
			v = gbk_apifam_win;
			break;
		case gbk_targfam_linx:
		case gbk_targfam_slrs:
		case gbk_targfam_fbsd:
		case gbk_targfam_obsd:
		case gbk_targfam_nbsd:
		case gbk_targfam_dbsd:
		case gbk_targfam_oind:
		case gbk_targfam_minx:
		case gbk_targfam_irix:
		case gbk_targfam_mx11:
		case gbk_targfam_cygw:
		case gbk_targfam_xgen:
			v = gbk_apifam_xwn;
			break;
		case gbk_targfam_lnds:
			v = gbk_apifam_nds;
			break;
	}

	return v;
}

LOCALFUNC tMyErr ChooseAPIFam(void)
{
	if (kListOptionAuto == gbo_apifam) {
		gbo_apifam = dfo_apifam();
	}

	return noErr;
}

LOCALPROC WrtOptAPIFam(void)
{
	WrtOptNamedOption("-api", GetAPIFamName, gbo_apifam, dfo_apifam());
}


/* option: ide */

enum {
	gbk_ide_mpw, /* Macintosh Programmers Workshop */
	gbk_ide_mw8, /* Metrowerks CodeWarrior */
	gbk_ide_bgc, /* Gnu tools */
	gbk_ide_snc, /* Sun tools */
	gbk_ide_msv, /* Microsoft Visual C++ */
	gbk_ide_lcc, /* lcc-win32 - Jacob Navia */
	gbk_ide_dvc, /* Bloodshed Dev-C++ */
	gbk_ide_xcd, /* Apple XCode */
		/* previously Apple Project Builder */
	gbk_ide_dmc, /* Digital Mars Compiler */
	gbk_ide_plc, /* Pelles C Compiler */
	gbk_ide_mgw, /* MinGW */
	gbk_ide_cyg, /* Cygwin */
	gbk_ide_dkp, /* devkitpro */
	gbk_ide_ccc, /* Generic command line c compiler */
	gbk_ide_mvc, /* Mini vMac C (a specific version of gcc) */
	kNumIdes
};

LOCALVAR int cur_ide;
LOCALVAR ui3r olv_ide;

LOCALPROC ResetIdeOption(void)
{
	cur_ide = kListOptionAuto;
	olv_ide = 0;
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
		case gbk_ide_snc:
			s = "snc";
			break;
		case gbk_ide_msv:
			s = "msv";
			break;
		case gbk_ide_lcc:
			s = "lcc";
			break;
		case gbk_ide_dvc:
			s = "dvc";
			break;
		case gbk_ide_mgw:
			s = "mgw";
			break;
		case gbk_ide_xcd:
			s = "xcd";
			break;
		case gbk_ide_dmc:
			s = "dmc";
			break;
		case gbk_ide_plc:
			s = "plc";
			break;
		case gbk_ide_cyg:
			s = "cyg";
			break;
		case gbk_ide_dkp:
			s = "dkp";
			break;
		case gbk_ide_ccc:
			s = "ccc";
			break;
		case gbk_ide_mvc:
			s = "mvc";
			break;
		default:
			s = "(unknown IDE)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr TryAsIdeOptionNot(void)
{
	return FindNamedOption("-e", kNumIdes, GetIdeName,
		&cur_ide, &olv_ide);
}

LOCALFUNC int dfo_ide(void)
{
	int v;

	switch (gbo_targfam) {
		case gbk_targfam_cmac:
		case gbk_targfam_carb:
			v = gbk_ide_mpw;
			break;
		case gbk_targfam_mach:
		case gbk_targfam_mx11:
			v = gbk_ide_xcd;
			break;
		case gbk_targfam_mswn:
		case gbk_targfam_wnce:
			v = gbk_ide_msv;
			break;
		case gbk_targfam_linx:
		case gbk_targfam_slrs:
		case gbk_targfam_fbsd:
		case gbk_targfam_obsd:
		case gbk_targfam_nbsd:
		case gbk_targfam_dbsd:
		case gbk_targfam_oind:
		case gbk_targfam_minx:
		case gbk_targfam_irix:
			v = gbk_ide_bgc;
			break;
		case gbk_targfam_cygw:
			v = gbk_ide_cyg;
			break;
		case gbk_targfam_lnds:
			v = gbk_ide_dkp;
			break;
		case gbk_targfam_xgen:
		default:
			v = gbk_ide_ccc;
			break;
	}

	return v;
}

LOCALFUNC tMyErr ChooseIde(void)
{

	if (kListOptionAuto == cur_ide) {
		cur_ide = dfo_ide();
	}

	return noErr;
}

LOCALPROC WrtOptIdeOption(void)
{
	WrtOptNamedOption("-e", GetIdeName, cur_ide, dfo_ide());
}


/* option: ide version */

LOCALVAR uimr ide_vers;
LOCALVAR ui3r olv_ide_vers;

LOCALPROC ResetIdeVersOption(void)
{
	olv_ide_vers = 0;
}

LOCALFUNC tMyErr TryAsIdeVersOptionNot(void)
{
	return NumberTryAsOptionNot("-ev",
		(long *)&ide_vers, &olv_ide_vers);
}

LOCALFUNC uimr dfo_ide_vers(void)
{
	uimr v;

	switch (cur_ide) {
		case gbk_ide_xcd:
			v = 2410;
			break;
		case gbk_ide_msv:
			v = 8000;
			break;
		default:
			v = 1;
			break;
	}

	return v;
}

LOCALFUNC tMyErr ChooseIdeVers(void)
{
	if (0 == olv_ide_vers) {
		ide_vers = dfo_ide_vers();
	}

	return noErr;
}

LOCALPROC WrtOptIdeVersOption(void)
{
	WrtOptNumberOption("-ev", ide_vers, dfo_ide_vers());
}


/* option: use command line tools */

LOCALVAR blnr UseCmndLine;
LOCALVAR ui3r olv_CmndLine;

LOCALPROC ResetCmndLine(void)
{
	UseCmndLine = falseblnr;
	olv_CmndLine = 0;
}

LOCALFUNC tMyErr TryAsCmndLineOptionNot(void)
{
	return FlagTryAsOptionNot("-cl", &UseCmndLine, &olv_CmndLine);
}

LOCALFUNC tMyErr ChooseCmndLineOption(void)
{
	return noErr;
}

LOCALPROC WrtOptCmndLine(void)
{
	WrtOptFlagOption("-cl", UseCmndLine);
}


/* option: export all source files */

LOCALVAR blnr WantAllSrc;
LOCALVAR ui3r olv_WantAllSrc;

LOCALPROC ResetWantAllSrc(void)
{
	WantAllSrc = nanblnr;
	olv_WantAllSrc = 0;
}

LOCALFUNC tMyErr TryAsWantAllSrcNot(void)
{
	return BooleanTryAsOptionNot("-all-src",
		&WantAllSrc, &olv_WantAllSrc);
}

#define dfo_WantAllSrc() falseblnr

LOCALFUNC tMyErr ChooseWantAllSrc(void)
{
	if (nanblnr == WantAllSrc) {
		WantAllSrc = dfo_WantAllSrc();
	}

	return noErr;
}

LOCALPROC WrtOptAllSrc(void)
{
	WrtOptBooleanOption("-all-src", WantAllSrc, dfo_WantAllSrc());
}


/* option: export no source files */

LOCALVAR blnr gbo_WantNoSrc;
LOCALVAR ui3r olv_WantNoSrc;

LOCALPROC ResetWantNoSrc(void)
{
	gbo_WantNoSrc = nanblnr;
	olv_WantNoSrc = 0;
}

LOCALFUNC tMyErr TryAsWantNoSrcNot(void)
{
	return BooleanTryAsOptionNot("-no-src",
		&gbo_WantNoSrc, &olv_WantNoSrc);
}

#define dfo_WantNoSrc() falseblnr

LOCALFUNC tMyErr ChooseWantNoSrc(void)
{
	tMyErr err = noErr;

	if (nanblnr == gbo_WantNoSrc) {
		gbo_WantNoSrc = dfo_WantNoSrc();
	} else {
		if (gbo_WantNoSrc && WantAllSrc) {
			err = ReportParseFailure(
				"-no-src 1 may not be used with -all-src 1");
		}
	}

	return err;
}

LOCALPROC WrtOptNoSrc(void)
{
	WrtOptBooleanOption("-no-src", gbo_WantNoSrc, dfo_WantNoSrc());
}


/* option: export archive */

LOCALVAR blnr NotWantExport;
LOCALVAR ui3r olv_WantExport;

LOCALPROC ResetWantExport(void)
{
	NotWantExport = falseblnr;
	olv_WantExport = 0;
}

LOCALFUNC tMyErr TryAsWantExportNot(void)
{
	return FlagTryAsOptionNot("-nex", &NotWantExport, &olv_WantExport);
}

LOCALFUNC tMyErr ChooseWantExport(void)
{
	return noErr;
}

LOCALPROC WrtOptWantExport(void)
{
	WrtOptFlagOption("-nex", NotWantExport);
}


/* option: eol source code format */

enum {
	gbk_eol_mac, /* Macintosh */
	gbk_eol_win, /* Windows */
	gbk_eol_unx, /* Unix */
	kNumEol
};

LOCALVAR int cur_eol;
LOCALVAR ui3r olv_eol;

LOCALPROC ResetEolOption(void)
{
	cur_eol = kListOptionAuto;
	olv_eol = 0;
}

LOCALFUNC char * GetEolName(int i)
{
	char *s;

	switch (i) {
		case gbk_eol_mac:
			s = "mac";
			break;
		case gbk_eol_win:
			s = "win";
			break;
		case gbk_eol_unx:
			s = "unx";
			break;
		default:
			s = "(unknown eol)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr TryAsEolOptionNot(void)
{
	return FindNamedOption("-eol", kNumEol, GetEolName,
		&cur_eol, &olv_eol);
}

LOCALFUNC int dfo_eol(void)
{
	int v;

	if (NotWantExport) {
		v = gbk_eol_mac;
	} else {
		switch (cur_ide) {
			case gbk_ide_mpw:
			case gbk_ide_mw8:
				v = gbk_eol_mac;
				break;
			case gbk_ide_bgc:
			case gbk_ide_xcd:
			case gbk_ide_snc:
			case gbk_ide_ccc:
			case gbk_ide_mvc:
				v = gbk_eol_unx;
				break;
			case gbk_ide_msv:
			case gbk_ide_lcc:
			case gbk_ide_dvc:
			case gbk_ide_mgw:
			case gbk_ide_cyg:
			case gbk_ide_dmc:
			case gbk_ide_plc:
			case gbk_ide_dkp:
				v = gbk_eol_win;
				break;
		}
	}

	return v;
}

LOCALFUNC tMyErr ChooseEOL(void)
{

	if (kListOptionAuto == cur_eol) {
		cur_eol = dfo_eol();
	}

	return noErr;
}

LOCALPROC WrtOptEOL(void)
{
	WrtOptNamedOption("-eol", GetEolName, cur_eol, dfo_eol());
}


/* option: archive format */

enum {
	gbk_arc_sit, /* stuff it */
	gbk_arc_zip, /* zip */
	gbk_arc_tar, /* tar */
	kNumArc
};

LOCALVAR int cur_arc;
LOCALVAR ui3r olv_arc;

LOCALPROC ResetArcOption(void)
{
	cur_arc = kListOptionAuto;
	olv_arc = 0;
}

LOCALFUNC char * GetArcName(int i)
{
	char *s;

	switch (i) {
		case gbk_arc_sit:
			s = "sit";
			break;
		case gbk_arc_zip:
			s = "zip";
			break;
		case gbk_arc_tar:
			s = "tar";
			break;
		default:
			s = "(unknown archive format)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr TryAsArcOptionNot(void)
{
	return FindNamedOption("-arc", kNumArc, GetArcName,
		&cur_arc, &olv_arc);
}

LOCALFUNC int dfo_arc(void)
{
	int v;

	switch (cur_ide) {
		case gbk_ide_mpw:
		case gbk_ide_mw8:
			v = gbk_arc_sit;
			break;
		case gbk_ide_bgc:
		case gbk_ide_xcd:
		case gbk_ide_snc:
		case gbk_ide_ccc:
		case gbk_ide_mvc:
			v = gbk_arc_tar;
			break;
		case gbk_ide_msv:
		case gbk_ide_lcc:
		case gbk_ide_dvc:
		case gbk_ide_mgw:
		case gbk_ide_cyg:
		case gbk_ide_dmc:
		case gbk_ide_plc:
		case gbk_ide_dkp:
			v = gbk_arc_zip;
			break;
	}

	return v;
}

LOCALFUNC tMyErr ChooseArc(void)
{
	if (kListOptionAuto == cur_arc) {
		cur_arc = dfo_arc();
	}

	return noErr;
}

LOCALPROC WrtOptArc(void)
{
	WrtOptNamedOption("-arc", GetArcName, cur_arc, dfo_arc());
}


/* option: print file list */

LOCALVAR blnr CurPrintCFiles;
LOCALVAR ui3r olv_PrintCFiles;

LOCALPROC ResetListOption(void)
{
	CurPrintCFiles = falseblnr;
	olv_PrintCFiles = 0;
}

LOCALFUNC tMyErr TryAsListOptionNot(void)
{
	return FlagTryAsOptionNot("-l", &CurPrintCFiles, &olv_PrintCFiles);
}

LOCALFUNC tMyErr ChooseListOption(void)
{
	return noErr;
}

LOCALPROC WrtOptListOption(void)
{
	WrtOptFlagOption("-l", CurPrintCFiles);
}


/* option: maintainer name */

LOCALVAR Handle hMaintainerName = NULL;
LOCALVAR ui3r olv_MaintainerName;

LOCALPROC ResetMaintainerName(void)
{
	StringResetOption(&hMaintainerName);
	olv_MaintainerName = 0;
}

LOCALFUNC tMyErr TryAsMaintainerNameOptionNot(void)
{
	return StringTryAsOptionNot("-maintainer",
		&hMaintainerName, &olv_MaintainerName);
}

LOCALFUNC tMyErr ChooseMaintainerName(void)
{
	tMyErr err;

	if (NULL != hMaintainerName) {
		err = noErr;
	} else {
		err = MyHandleNewFromCStr(&hMaintainerName, "unknown");
	}

	return err;
}


/* option: home page */

LOCALVAR Handle hHomePage = NULL;
LOCALVAR ui3r olv_HomePage;

LOCALPROC ResetHomePage(void)
{
	StringResetOption(&hHomePage);
	olv_HomePage = 0;
}

LOCALFUNC tMyErr TryAsHomePageOptionNot(void)
{
	return StringTryAsOptionNot("-homepage", &hHomePage, &olv_HomePage);
}

LOCALFUNC tMyErr ChooseHomePage(void)
{
	tMyErr err;

	if (NULL != hHomePage) {
		err = noErr;
	} else {
		err = MyHandleNewFromCStr(&hHomePage, kStrHomePage);
	}

	return err;
}


/* option: sponsor name */

LOCALVAR Handle hSponsorName = NULL;
LOCALVAR ui3r olv_SponsorName;

LOCALPROC ResetSponsorName(void)
{
	StringResetOption(&hSponsorName);
	olv_SponsorName = 0;
}

LOCALFUNC tMyErr TryAsSponsorNameOptionNot(void)
{
	return StringTryAsOptionNot("-sponsor",
		&hSponsorName, &olv_SponsorName);
}

LOCALFUNC tMyErr ChooseSponsorNameOption(void)
{
	return noErr;
}

/* derived option: application is os x bundle (folder) */

LOCALVAR blnr HaveMacBundleApp;
LOCALVAR blnr WantUnTranslocate;

LOCALFUNC tMyErr ChooseHaveMacBundleApp(void)
{
	HaveMacBundleApp = (gbk_targfam_mach == gbo_targfam)
		|| ((gbk_targfam_carb == gbo_targfam)
			&& (gbk_ide_mpw == cur_ide));
	WantUnTranslocate = (gbk_apifam_cco == gbo_apifam)
		&& ((gbk_cpufam_x64 == gbo_cpufam)
			|| (gbk_cpufam_x86 == gbo_cpufam));

	return noErr;
}

/* derived option: have macintosh resources */

LOCALVAR blnr HaveMacRrscs;

LOCALFUNC tMyErr ChooseHaveMacRrscs(void)
{
	HaveMacRrscs = (gbk_apifam_mac == gbo_apifam)
		|| ((gbk_targfam_carb == gbo_targfam)
			&& ! (gbk_ide_mpw == cur_ide));

	return noErr;
}


/* option: Abbrev Name */

LOCALVAR char vStrAppAbbrev[8 + 1];
LOCALVAR ui3r olv_AbbrevName;

LOCALPROC ResetAbbrevName(void)
{
	vStrAppAbbrev[0] = 0;
	olv_AbbrevName = 0;
}

LOCALFUNC tMyErr TryAsAbbrevNameOptionNot(void)
{
	tMyErr err;
	MyPStr t;

	if (kMyErr_noErr != (err =
		CurArgIsOption("-an", &olv_AbbrevName)))
	{
		/* no */
	} else
	if (The_arg_end) {
		PStrFromCStr(t, "Expecting an argument for ");
		PStrApndCStr(t, "-an");
		PStrApndCStr(t, " when reached end");
		err = ReportParseFailPStr(t);
	} else
	{
		GetCurArgAsCStr(vStrAppAbbrev, 8);
		err = AdvanceTheArg();
	}

	return err;
}

LOCALFUNC tMyErr ChooseAbbrevName(void)
{
	if (0 == vStrAppAbbrev[0]) {
		CStrCopy(vStrAppAbbrev, kStrAppAbbrev);
	}

	return noErr;
}


/* option: Variation Name */

LOCALVAR Handle hVariationName = NULL;
LOCALVAR ui3r olv_VariationName;

LOCALPROC ResetVariationName(void)
{
	StringResetOption(&hVariationName);
	olv_VariationName = 0;
}

LOCALFUNC tMyErr TryAsVariationNameOptionNot(void)
{
	return StringTryAsOptionNot("-n",
		&hVariationName, &olv_VariationName);
}

LOCALFUNC tMyErr ChooseVariationName(void)
{
	tMyErr err;

	if (NULL != hVariationName) {
		err = noErr;
	} else {
		MyPStr s;

		PStrFromCStr(s, vStrAppAbbrev);
		PStrApndCStr(s, "-");
		PStrApndNUimr(s, MajorVersion, 2);
		PStrApndCStr(s, ".");
		PStrApndNUimr(s, MinorVersion, 2);
		PStrApndCStr(s, "-");
		/* PStrApndCStr(s, GetIdeName(cur_ide)); */
		PStrApndCStr(s, GetTargetName(cur_targ));
		/* PStrApndCStr(s, GetDbgLvlName(gbo_dbg)); */
		err = MyHandleNewFromPStr(&hVariationName, s);
	}

	return err;
}


/* option: Configuration Directory */

LOCALVAR blnr WantConfigDir;
LOCALVAR ui3r olv_ConfigDir;

LOCALPROC ResetConfigDir(void)
{
	WantConfigDir = nanblnr;
	olv_ConfigDir = 0;
}

LOCALFUNC tMyErr TryAsConfigDirNot(void)
{
	return BooleanTryAsOptionNot("-cfg",
		&WantConfigDir, &olv_ConfigDir);
}

LOCALFUNC blnr dfo_ConfigDir(void)
{
	int v;

	if (gbo_WantNoSrc) {
		v = trueblnr;
	} else {
		v = falseblnr;
	}

	return v;
}

LOCALFUNC tMyErr ChooseConfigDir(void)
{
	tMyErr err = noErr;

	if (nanblnr == WantConfigDir) {
		WantConfigDir = dfo_ConfigDir();
	} else {
		if ((! WantConfigDir) && gbo_WantNoSrc) {
			err = ReportParseFailure(
				"-cfg 0 may not be used with -no-src 1");
		}
	}

	return err;
}


/* option: Errors To File */

LOCALVAR blnr gbo_Err2File;
LOCALVAR ui3r olv_Err2File;

LOCALPROC ResetErr2File(void)
{
	gbo_Err2File = nanblnr;
	olv_Err2File = 0;
}

LOCALFUNC tMyErr TryAsErr2FileNot(void)
{
	return BooleanTryAsOptionNot("-ef", &gbo_Err2File, &olv_Err2File);
}

#define dfo_Err2File() falseblnr

LOCALFUNC tMyErr ChooseErr2File(void)
{
	if (nanblnr == gbo_Err2File) {
		gbo_Err2File = dfo_Err2File();
	}

	return noErr;
}


/* option: Need International Characters */

LOCALVAR blnr NeedIntl;
LOCALVAR ui3r olv_NeedIntl;

LOCALPROC ResetNeedIntl(void)
{
	NeedIntl = falseblnr;
	olv_NeedIntl = 0;
}

LOCALFUNC tMyErr TryAsNeedIntlNot(void)
{
	return FlagTryAsOptionNot("-intl", &NeedIntl, &olv_NeedIntl);
}

LOCALFUNC tMyErr ChooseNeedIntl(void)
{
	return noErr;
}


/* option: Demo Message */

LOCALVAR blnr WantDemoMsg;
LOCALVAR ui3r olv_DemoMsg;

LOCALPROC ResetDemoMsg(void)
{
	WantDemoMsg = nanblnr;
	olv_DemoMsg = 0;
}

LOCALFUNC tMyErr TryAsDemoMsgNot(void)
{
	return BooleanTryAsOptionNot("-dmo", &WantDemoMsg, &olv_DemoMsg);
}

#define dfo_DemoMsg() falseblnr

LOCALFUNC tMyErr ChooseDemoMsg(void)
{
	if (nanblnr == WantDemoMsg) {
		WantDemoMsg = dfo_DemoMsg();
	}

	return noErr;
}


/* option: Activation Code */

LOCALVAR blnr WantActvCode;
#define NumKeyCon 7
LOCALVAR long KeyCon[NumKeyCon];
LOCALVAR ui3r olv_ActvCode;

LOCALPROC ResetActvCode(void)
{
	WantActvCode = falseblnr;
	olv_ActvCode = 0;
}

LOCALFUNC tMyErr TryAsActvCodeNot(void)
{
	tMyErr err;
	int i;

	if (kMyErr_noErr != (err = CurArgIsOption("-act", &olv_ActvCode))) {
		/* no */
	} else
	{
		WantActvCode = trueblnr;
		for (i = 0; i < NumKeyCon; ++i) {
			err = GetCurArgOptionAsNumber("-act", &KeyCon[i]);
			if (noErr != err) {
				goto Label_1;
			}
		}
		err = noErr;
Label_1:
		;
	}

	return err;
}

LOCALFUNC tMyErr ChooseActvCode(void)
{
	return noErr;
}


/* --- end of default definition of options --- */

LOCALPROC GNResetCommandLineParameters(void)
{
	ResetBranchOption();
	ResetTargetOption();
	ResetDbgOption();
	ResetLangOption();
	ResetIconMaster();
	ResetTstCompErr();
}

LOCALFUNC tMyErr TryAsGNOptionNot(void)
{
	tMyErr err;

	if (kMyErrNoMatch == (err = TryAsBranchOptionNot()))
	if (kMyErrNoMatch == (err = TryAsTargetOptionNot()))
	if (kMyErrNoMatch == (err = TryAsDbgOptionNot()))
	if (kMyErrNoMatch == (err = TryAsLangOptionNot()))
	if (kMyErrNoMatch == (err = TryAsIconMasterNot()))
	if (kMyErrNoMatch == (err = TryAsTstCompErrNot()))
	if (kMyErrNoMatch == (err = TryAsTstBldSysErr()))
	{
	}

	return err;
}

LOCALFUNC tMyErr AutoChooseGNSettings(void)
{
	tMyErr err;

	if (noErr == (err = ChooseBranch()))
	if (noErr == (err = ChooseTarg()))
	if (noErr == (err = ChooseDbgOption()))
	if (noErr == (err = ChooseLangOption()))
	if (noErr == (err = ChooseIconMaster()))
	if (noErr == (err = ChooseTstCompErr()))
	{
		err = noErr;
	}

	return err;
}

LOCALPROC WrtOptGNSettings(void)
{
	WrtOptBranchOption();
	WrtOptTarg();
	WrtOptDbgOption();
	WrtOptLangOption();
	WrtOptIconMaster();
	WrtOptTstCompErr();
}

LOCALPROC GNDevResetCommandLineParameters(void)
{
	ResetCPUFamOption();
	ResetAPIFamOption();
	ResetIdeOption();
	ResetIdeVersOption();
	ResetCmndLine();
	ResetWantAllSrc();
	ResetWantNoSrc();
	ResetWantExport();
	ResetEolOption();
	ResetArcOption();
	ResetListOption();
	ResetMaintainerName();
	ResetHomePage();
	ResetSponsorName();
	ResetAbbrevName();
	ResetVariationName();
	ResetConfigDir();
	ResetErr2File();
	ResetNeedIntl();
	ResetDemoMsg();
	ResetActvCode();
}

LOCALFUNC tMyErr TryAsGNDevOptionNot(void)
{
	tMyErr err;

	DoingDevOpts = trueblnr;

	if (kMyErrNoMatch == (err = TryAsCPUFamOptionNot()))
	if (kMyErrNoMatch == (err = TryAsAPIFamOptionNot()))
	if (kMyErrNoMatch == (err = TryAsIdeOptionNot()))
	if (kMyErrNoMatch == (err = TryAsIdeVersOptionNot()))
	if (kMyErrNoMatch == (err = TryAsCmndLineOptionNot()))
	if (kMyErrNoMatch == (err = TryAsWantAllSrcNot()))
	if (kMyErrNoMatch == (err = TryAsWantNoSrcNot()))
	if (kMyErrNoMatch == (err = TryAsWantExportNot()))
	if (kMyErrNoMatch == (err = TryAsEolOptionNot()))
	if (kMyErrNoMatch == (err = TryAsArcOptionNot()))
	if (kMyErrNoMatch == (err = TryAsListOptionNot()))
	if (kMyErrNoMatch == (err = TryAsMaintainerNameOptionNot()))
	if (kMyErrNoMatch == (err = TryAsHomePageOptionNot()))
	if (kMyErrNoMatch == (err = TryAsSponsorNameOptionNot()))
	if (kMyErrNoMatch == (err = TryAsAbbrevNameOptionNot()))
	if (kMyErrNoMatch == (err = TryAsVariationNameOptionNot()))
	if (kMyErrNoMatch == (err = TryAsConfigDirNot()))
	if (kMyErrNoMatch == (err = TryAsErr2FileNot()))
	if (kMyErrNoMatch == (err = TryAsNeedIntlNot()))
	if (kMyErrNoMatch == (err = TryAsDemoMsgNot()))
	if (kMyErrNoMatch == (err = TryAsActvCodeNot()))
	{
	}

	DoingDevOpts = falseblnr;

	return err;
}

LOCALFUNC tMyErr AutoChooseGNDevSettings(void)
{
	tMyErr err;

	if (noErr == (err = ChooseCPUFam()))
	if (noErr == (err = ChooseTargFam())) /* derived */
	if (noErr == (err = ChooseAPIFam()))
	if (noErr == (err = ChooseIde()))
	if (noErr == (err = ChooseIdeVers()))
	if (noErr == (err = ChooseCmndLineOption()))
	if (noErr == (err = ChooseWantAllSrc()))
	if (noErr == (err = ChooseWantNoSrc()))
	if (noErr == (err = ChooseWantExport()))
	if (noErr == (err = ChooseEOL()))
	if (noErr == (err = ChooseArc()))
	if (noErr == (err = ChooseListOption()))
	if (noErr == (err = ChooseMaintainerName()))
	if (noErr == (err = ChooseHomePage()))
	if (noErr == (err = ChooseSponsorNameOption()))
	if (noErr == (err = ChooseHaveMacBundleApp())) /* derived */
	if (noErr == (err = ChooseHaveMacRrscs())) /* derived */
	if (noErr == (err = ChooseAbbrevName()))
	if (noErr == (err = ChooseVariationName()))
	if (noErr == (err = ChooseConfigDir()))
	if (noErr == (err = ChooseErr2File()))
	if (noErr == (err = ChooseNeedIntl()))
	if (noErr == (err = ChooseDemoMsg()))
	if (noErr == (err = ChooseActvCode()))
	{
		err = noErr;
	}

	return err;
}

#if 0
LOCALPROC WrtOptGNDevSettings(void)
{
	WrtOptCPUFam();
	WrtOptAPIFam();
	WrtOptIdeOption();
	WrtOptIdeVersOption();
	WrtOptCmndLine();
	WrtOptAllSrc();
	WrtOptNoSrc();
	WrtOptWantExport();
	WrtOptEOL();
	WrtOptArc();
	WrtOptListOption();
	/* Maintainer */
	/* HomePage */
	/* Sponsor */
	/* VariationName */
	/* AbbrevName */
	/* ConfigDir */
	/* Err2File */
	/* WantDemoMsg */
	/* WantActvCode */
}
#endif
