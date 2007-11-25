/*
	WRCNFGGL.i
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
	WRite "CNFGGLob.h"
*/


LOCALPROC WriteWrongCNFGGLOB(void)
{
	WriteDestFileLn("#error \"wrong CNFGGLOB.h\"");
}

LOCALPROC WriteCheckPreDef(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#ifndef ");
	WriteCStrToDestFile(s);
	WriteEndDestFileLn();
	WriteWrongCNFGGLOB();
	WriteDestFileLn("#endif");
}

LOCALPROC WriteCheckPreNDef(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#ifdef ");
	WriteCStrToDestFile(s);
	WriteEndDestFileLn();
	WriteWrongCNFGGLOB();
	WriteDestFileLn("#endif");
}

#ifndef NeedIntFormatInfo
#define NeedIntFormatInfo 0
#endif

LOCALPROC WriteCommonCNFGGLOB(void)
{
	if (WriteOpenDestFile(&SrcDirR, "CNFGGLOB", ".h")) { /* C Configuration file */

#if support_ide_mw8
	if (cur_ide == gbk_ide_mw8) {
		WriteDestFileLn("/* make sure this is correct CNFGGLOB */");

		WriteCheckPreDef("__MWERKS__");
		switch (gbo_cpufam) {
			case gbk_cpufam_68k:
				WriteCheckPreDef("__MC68K__");
				if (cur_targ == gbk_targ_mfpu) {
					WriteCheckPreDef("__MC68881__");
				} else {
					WriteCheckPreNDef("__MC68881__");
				}
				break;
			case gbk_cpufam_ppc:
				WriteCheckPreDef("__POWERPC__");
				break;
			case gbk_cpufam_x86:
				WriteCheckPreDef("__INTEL__");
				break;
		}
	}
#endif

	WriteBlankLineToDestFile();

#if NeedIntFormatInfo
	if ((gbo_cpufam == gbk_cpufam_68k) || (gbo_cpufam == gbk_cpufam_ppc)) {
		WriteDestFileLn("#define MostSigByteFirst 1");
	}
	if (gbo_cpufam == gbk_cpufam_x86) {
		WriteDestFileLn("#define LeastSigByteFirst 1");
	}
	if ((gbo_cpufam == gbk_cpufam_68k) || (gbo_cpufam == gbk_cpufam_ppc) || (gbo_cpufam == gbk_cpufam_x86)) {
		WriteDestFileLn("#define TwosCompSigned 1");
	}
#endif
	if ((gbo_cpufam == gbk_cpufam_68k) || (gbo_cpufam == gbk_cpufam_ppc)) {
		WriteDestFileLn("#define BigEndianUnaligned 1");
	}

	if (gbo_cpufam == gbk_cpufam_68k) {
		WriteDestFileLn("#define HaveCPUfamM68K 1");
	}

	if ((cur_ide == gbk_ide_bgc) || (cur_ide == gbk_ide_xcd) || (cur_ide == gbk_ide_snc)) {
		WriteDestFileLn("#define MayInline inline");
	} else
#if support_ide_mw8
	if (cur_ide == gbk_ide_mw8) {
		WriteDestFileLn("#define MayInline __inline__");
	} else
#endif
#if SupportWinIDE
	if (cur_ide == gbk_ide_msv) {
		if (ide_vers >= 6000) {
			WriteDestFileLn("#define MayInline __forceinline");
		} else {
			WriteDestFileLn("#define MayInline __inline");
		}
	} else
#endif
	{
	}

	if ((cur_ide == gbk_ide_bgc) || (cur_ide == gbk_ide_xcd)) {
		WriteDestFileLn("#define MayNotInline __attribute__((noinline))");
	} else
#if SupportWinIDE
	if (cur_ide == gbk_ide_msv) {
		if (ide_vers >= 7000) {
			WriteDestFileLn("#define MayNotInline __declspec(noinline)");
		}
	} else
#endif
	{
	}

	if ((cur_ide == gbk_ide_bgc) || (cur_ide == gbk_ide_xcd)) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define cIncludeUnused 0");
	} else
#if SupportWinIDE
	if (cur_ide == gbk_ide_lcc) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define cIncludeUnused 0");
		WriteDestFileLn("#define UnusedParam(x)");
	} else if (cur_ide == gbk_ide_dvc) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define cIncludeUnused 0");
	} else
#endif
	{
	}

	if (gbo_dbg != gbk_dbg_off) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define Debug 1");
	}

	WriteBlankLineToDestFile();

	if (MySoundEnabled) {
		WriteDestFileLn("#define MySoundEnabled 1");
	}

#ifdef Have_SPCNFGGL
	WriteAppSpecificCNFGGLOBoptions();
#endif

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
	WriteMaintainerName();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kStrHomePage ");
	WriteQuoteToDestFile();
	WriteHomePage();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

#if SupportWinIDE
	if (cur_ide == gbk_ide_msv) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("/* --- set up compiler options --- */");
		WriteBlankLineToDestFile();
		WriteDestFileLn("/* ignore integer conversion warnings */");
		WriteDestFileLn("#pragma warning(disable : 4244 4761 4018 4245 4024 4305)");
		WriteBlankLineToDestFile();
		WriteDestFileLn("/* ignore unused inline warning */");
		WriteDestFileLn("#pragma warning(disable : 4514 4714)");
		WriteBlankLineToDestFile();
		WriteDestFileLn("/* ignore type redefinition warning */");
		WriteDestFileLn("#pragma warning(disable : 4142)");
	}
#endif
#if support_ide_mw8
	if (cur_ide == gbk_ide_mw8) {
		if (gbo_dbg != gbk_dbg_on) {
			WriteBlankLineToDestFile();
			WriteDestFileLn("#ifdef OptForSpeed");
			WriteDestFileLn("#pragma optimize_for_size off");
			WriteDestFileLn("#endif");
		}
	}
#endif

	WriteBlankLineToDestFile();

	WriteCloseDestFile();
	}
}
