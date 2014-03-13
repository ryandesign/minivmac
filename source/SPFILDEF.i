/*
	SPFILDEF.i
	Copyright (C) 2012 Paul C. Pratt

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
	program SPecific FILe DEFinitions
*/



/* --- list of source files --- */

static void DoAllExtraHeaders(tDoOneExtraHeader p)
{
	p(kDepDirCnfg, "CNFGGLOB.h");
	p(kDepDirCnfg, "CNFGRAPI.h");
	p(kDepDirCSrc, "SYSDEPNS.h");
	p(kDepDirCSrc, "INTLCHAR.h");
	p(kDepDirCSrc, "DATE2SEC.h");
	p(kDepDirCSrc, "ENDIANAC.h");
	p(kDepDirCSrc, "COMOSGLU.h");
	p(kDepDirLang, "STRCONST.h");
	if (gbk_sndapi_none != gbo_sndapi) {
		p(kDepDirSndA, "SOUNDGLU.h");
	}
	if (WantAltKeysMode) {
		p(kDepDirCSrc, "ALTKEYSM.h");
	}
	p(kDepDirCSrc, "CONTROLM.h");
	p(kDepDirCnfg, "EMCONFIG.h");
	if (UseAsm68k) {
		p(kDepDirCnfg, "CNFGRASM.i");
	}
	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		p(kDepDirCSrc, "FPMATHEM.h");
		p(kDepDirCSrc, "FPCPEMDV.h");
	}
	if (NeedScrnHack) {
		p(kDepDirCSrc, "SCRNHACK.h");
	}
	if (cur_mdl >= gbk_mdl_SE) {
		p(kDepDirCSrc, "ADBSHARE.h");
	}
	if (WantActvCode) {
		p(kDepDirCSrc, "ACTVCODE.h");
	}
	if (WantLocalTalk) {
		p(kDepDirCSrc, "BPFILTER.h");
	}
	if ((gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
		|| (gbk_apifam_xwn == gbo_apifam)
		|| (gbk_apifam_sdl == gbo_apifam))
	{
		p(kDepDirCSrc, "SCRNMAPR.h");
		if (cur_ScrnDpth != 0) {
			p(kDepDirCSrc, "SCRNTRNS.h");
		}
	}
}

static void DoMYOSGLUEdepends(tDoOneDepends p)
{
	p(kDepDirCSrc, "COMOSGLU.h");
	p(kDepDirLang, "STRCONST.h");
	p(kDepDirCSrc, "CONTROLM.h");
	if (gbk_sndapi_none != gbo_sndapi) {
		p(kDepDirSndA, "SOUNDGLU.h");
	}
}

static void DoAllSrcFiles(tDoOneCFile p)
{
	if ((gbk_asm_none != cur_asm)
		&& (gbk_targ_ndsa == cur_targ))
	{
		p("FB1BPP2I", kCSrcFlgmAsmAvail, nullpr);
	}
	p("MYOSGLUE", kCSrcFlgmUseAPI,
		DoMYOSGLUEdepends);
	p("GLOBGLUE", kCSrcFlgmNone, nullpr);
	p("M68KITAB", kCSrcFlgmNone, nullpr);
	if (WantDisasm) {
		p("DISAM68K", kCSrcFlgmNone, nullpr);
	}

	p("MINEM68K", kCSrcFlgmSortFirst
		| (UseAsm68k ? kCSrcFlgmAsmAvail : 0),
		nullpr);
		/*
			Put the most speed critical part of the
			program first, to help ensure consistent
			alignment for it, regardless of changes
			to rest of program.
			Speed can depend subtly, basically
			randomly, on how code is aligned.
		*/

	p("VIAEMDEV", kCSrcFlgmNone, nullpr);
	if (EmVIA2) {
		p("VIA2EMDV", kCSrcFlgmNone, nullpr);
	}
	p("IWMEMDEV", kCSrcFlgmNone, nullpr);
	p("SCCEMDEV", kCSrcFlgmNone, nullpr);
	if (EmRTC) {
		p("RTCEMDEV", kCSrcFlgmNone, nullpr);
	}
	p("ROMEMDEV", kCSrcFlgmNone, nullpr);
	p("SCSIEMDV", kCSrcFlgmNone, nullpr);
	p("SONYEMDV", kCSrcFlgmNone, nullpr);
	p("SCRNEMDV", kCSrcFlgmNone, nullpr);
	if (EmVidCard) {
		p("VIDEMDEV", kCSrcFlgmNone, nullpr);
	}
	if (EmClassicKbrd) {
		p("KBRDEMDV", kCSrcFlgmNone, nullpr);
	} else if (EmPMU) {
		p("PMUEMDEV", kCSrcFlgmNone, nullpr);
	} else {
		p("ADBEMDEV", kCSrcFlgmNone, nullpr);
	}
	if (EmASC) {
		p("ASCEMDEV", kCSrcFlgmNone, nullpr);
	} else {
		if ((gbk_mdl_PB100 != cur_mdl) && MySoundEnabled) {
			p("SNDEMDEV", kCSrcFlgmNone, nullpr);
		}
	}
	p("MOUSEMDV", kCSrcFlgmNone, nullpr);
	p("PROGMAIN", kCSrcFlgmNone, nullpr);
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
