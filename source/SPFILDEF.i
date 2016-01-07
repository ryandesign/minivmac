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
	p("CNFGGLOB", kDepDirCnfg, kCSrcFlgmNoSource, nullpr);
	p("CNFGRAPI", kDepDirCnfg, kCSrcFlgmNoSource, nullpr);
	p("SYSDEPNS", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("INTLCHAR", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("DATE2SEC", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("ENDIANAC", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("COMOSGLU", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("STRCONST", kDepDirLang, kCSrcFlgmNoSource, nullpr);
	if (gbk_sndapi_none != gbo_sndapi) {
		p("SOUNDGLU", kDepDirSndA, kCSrcFlgmNoSource, nullpr);
	}
	if (WantAltKeysMode) {
		p("ALTKEYSM", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	}
	p("CONTROLM", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("EMCONFIG", kDepDirCnfg, kCSrcFlgmNoSource, nullpr);
#if 0
	if (UseAsm68k) {
		p("CNFGRASM", kDepDirCnfg, kCSrcFlgmNoSource, nullpr);
		/* this is a ".i" file, not a C language ".h" file */
	}
#endif
	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		p("FPMATHEM", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
		p("FPCPEMDV", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	}
	if (NeedScrnHack) {
		p("SCRNHACK", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	}
	if (cur_mdl >= gbk_mdl_SE) {
		p("ADBSHARE", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	}
	if (WantActvCode) {
		p("ACTVCODE", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	}
	if (WantLocalTalk) {
		p("BPFILTER", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	}
	if ((gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
		|| (gbk_apifam_xwn == gbo_apifam)
		|| (gbk_apifam_sdl == gbo_apifam)
		|| (gbk_apifam_sd2 == gbo_apifam))
	{
		p("SCRNMAPR", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
		if (cur_ScrnDpth != 0) {
			p("SCRNTRNS", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
		}
	}

	if ((gbk_asm_none != cur_asm)
		&& (gbk_targ_ndsa == cur_targ))
	{
		p("FB1BPP2I", kDepDirCSrc, kCSrcFlgmAsmAvail, nullpr);
	}
	p("MYOSGLUE", kDepDirCSrc, kCSrcFlgmUseAPI
		| ((gbk_apifam_cco == gbo_apifam) ? kCSrcFlgmOjbc : 0),
		DoMYOSGLUEdepends);
	p("GLOBGLUE", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("M68KITAB", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	if (WantDisasm) {
		p("DISAM68K", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	}

	p("MINEM68K", kDepDirCSrc, kCSrcFlgmSortFirst
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

	p("VIAEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	if (EmVIA2) {
		p("VIA2EMDV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	}
	p("IWMEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("SCCEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	if (EmRTC) {
		p("RTCEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	}
	p("ROMEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("SCSIEMDV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("SONYEMDV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("SCRNEMDV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	if (EmVidCard) {
		p("VIDEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	}
	if (EmClassicKbrd) {
		p("KBRDEMDV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	} else if (EmPMU) {
		p("PMUEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	} else {
		p("ADBEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	}
	if (EmASC) {
		p("ASCEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	} else {
		if ((gbk_mdl_PB100 != cur_mdl) && MySoundEnabled) {
			p("SNDEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
		}
	}
	p("MOUSEMDV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("PROGMAIN", kDepDirCSrc, kCSrcFlgmNone, nullpr);
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
