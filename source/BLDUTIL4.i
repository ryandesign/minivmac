/*
	BLDUTIL4.i
	Copyright (C) 2009 Paul C. Pratt

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
	BuiLD system UTILities part 4
*/


LOCALFUNC tMyErr rConverTextInThing(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s)
{
	tMyErr err;

	switch (cur_eol) {
		case gbk_eol_mac:
			err = duplib_WriteFile_v2(src_d, src_s,
				dst_d, dst_s);
			break;
		case gbk_eol_win:
			err = rinscharafter_WriteFile_v2(src_d, src_s,
				dst_d, dst_s,
				((char)13), ((char)10));
			break;
		case gbk_eol_unx:
			err = rsubstchar_WriteFile_v2(src_d, src_s,
				dst_d, dst_s,
				((char)13), ((char)10));
			break;
		default:
			err = kMyErrProgramBug;
			break;
	}

	return err;
}

LOCALFUNC tMyErr rConverTextInThingXtn(
	MyDir_R *src_d, MyDir_R *dst_d,
	char *name, char *FileExt)
{
	Str255 s;

	PStrFromCStr(s, name);
	PStrApndCStr(s, FileExt);
	return rConverTextInThing(
		src_d, s,
		dst_d, s);
}

LOCALFUNC tMyErr FindPrefFolder(MyDir_R *d)
{
	tMyErr err;
	long reply;

	if (HaveGestaltAvail()
		&& (noErr == Gestalt(gestaltFindFolderAttr, &reply))
		&& TestBit(reply, gestaltFindFolderPresent)
		)
	{
		err = FindFolder(
			kOnSystemDisk,
			kPreferencesFolderType,
			kDontCreateFolder,
			&d->VRefNum,
			&d->DirId);
	} else {
		SysEnvRec info;

		err = SysEnvirons(1, &info);
		if (noErr == err) {
			err = MyDirFromWD_v2(info.sysVRefNum, d);
		}
	}

	return err;
}

LOCALFUNC tMyErr FindOutPutDir(void)
{
	tMyErr err;
	MyDir_R PrefDirR;
	MyDir_R GryphelDirR;
	MyDir_R BuildDirR;

	if (noErr == (err = FindPrefFolder(&PrefDirR)))
	if (noErr == (err = MyResolveNamedChildDir_v2(&PrefDirR,
		(StringPtr)"\pGryphel", &GryphelDirR)))
	if (noErr == (err = MyResolveNamedChildDir_v2(&GryphelDirR,
		(StringPtr)"\pBuild", &BuildDirR)))
	{
		/* ok */
	}

	if (noErr == err) {
		err = FindOrMakeSubDirectory_v2(&OutputDir0R,
			&BuildDirR, "output", "");
	} else {
		err = FindOrMakeSubDirectory_v2(&OutputDir0R,
			&BaseDirR, "output", "");
	}

	return err;
}

LOCALPROC WriteOutDummyContents(void)
{
	WriteDestFileLn(
		"This file is here because some archive extraction");
	WriteDestFileLn("software will not create an empty directory.");
}

LOCALFUNC tMyErr WriteMakeOutputDirectories(void)
{
	tMyErr err;
	Str255 s;

	HandToPStr(hVariationName, s);
	if (noErr == (err = MyHGetDir_v2(&BaseDirR)))
	if (noErr == (err = FindOutPutDir()))
	if (noErr == (err = DelMakeNamedDir_v2(&OutputDirR,
		&OutputDir0R, s)))
	if (noErr == (err = rConverTextInThingXtn(&BaseDirR,
		&OutputDirR, "README", ".txt")))
	if (noErr == (err = rConverTextInThingXtn(&BaseDirR,
		&OutputDirR, "COPYING", ".txt")))
	{
		if ((gbk_ide_xcd == cur_ide) && (! UseCmndLine)) {
			err = MakeSubDirectory_v2(&ProjDirR, &OutputDirR,
				vStrAppAbbrev,
				(ide_vers >= 2100) ? ".xcodeproj" : ".pbproj");
		} else if (gbk_ide_mw8 != cur_ide) {
			if (noErr == (err = MakeSubDirectory_v2(&ObjDirR,
				&OutputDirR, obj_d_name, "")))
			{
				err = WriteADestFile(&ObjDirR, "dummy", "",
					WriteOutDummyContents);
			}
		}
	}

	return err;
}

#pragma segment Main

LOCALFUNC tMyErr WriteIdeSpecificFiles(void)
{
	tMyErr err;

	switch (cur_ide) {
#if ide_mpw_Supported
		case gbk_ide_mpw:
			err = WriteMPWSpecificFiles();
			break;
#endif
#if ide_bgc_Supported
		case gbk_ide_bgc:
		case gbk_ide_cyg:
		case gbk_ide_mgw:
		case gbk_ide_dkp:
			err = WriteBashGccSpecificFiles();
			break;
#endif
#if ide_mw8_Supported
		case gbk_ide_mw8:
			err = WriteMetrowerksSpecificFiles();
			break;
#endif
#if ide_snc_Supported
		case gbk_ide_snc:
			err = WriteSncSpecificFiles();
			break;
#endif
#if ide_msv_Supported
		case gbk_ide_msv:
			err = WriteMsvSpecificFiles();
			break;
#endif
#if ide_lcc_Supported
		case gbk_ide_lcc:
			if (UseCmndLine) {
				err = WriteLccW32clSpecificFiles();
			} else {
				err = WriteLccW32SpecificFiles();
			}
			break;
#endif
#if ide_dvc_Supported
		case gbk_ide_dvc:
			if (UseCmndLine) {
				err = WriteBashGccSpecificFiles();
			} else {
				err = WriteDevCSpecificFiles();
			}
			break;
#endif
#if ide_xcd_Supported
		case gbk_ide_xcd:
			if (UseCmndLine) {
				err = WriteBashGccSpecificFiles();
			} else {
				err = WriteXCDSpecificFiles();
			}
			break;
#endif
#if ide_dmc_Supported
		case gbk_ide_dmc:
			err = WriteDMCSpecificFiles();
			break;
#endif
#if ide_plc_Supported
		case gbk_ide_plc:
			if (UseCmndLine) {
				err = WritePLCclSpecificFiles();
			} else {
				err = WritePLCSpecificFiles();
			}
			break;
#endif
#if ide_ccc_Supported
		case gbk_ide_ccc:
			err = WriteCccSpecificFiles();
			break;
#endif
		default:
			err = kMyErrProgramBug;
			break;
	}

	return err;
}

LOCALFUNC tMyErr rMakeArchiveThing(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s)
{
	tMyErr err;

	switch (cur_arc) {
		case gbk_arc_sit:
			err = sitlib_WriteFile_v2(
				src_d, src_s,
				dst_d, dst_s);
			break;
		case gbk_arc_zip:
			err = nzplib_WriteFile_v2(
				src_d, src_s,
				dst_d, dst_s);
			break;
		case gbk_arc_tar:
			err = tarlib_WriteTarFile2_v2(
				src_d, src_s,
				dst_d, dst_s);
			break;
		default:
			err = kMyErrProgramBug;
			break;
	}

	return err;
}

LOCALFUNC tMyErr ArchiveAndExport(void)
{
	tMyErr err;
	Str255 sarc;
	ui4r version;
	ui5r features;

	HandToPStr(hVariationName, sarc);
	switch (cur_arc) {
		case gbk_arc_sit:
			PStrApndCStr(sarc, ".sit");
			break;
		case gbk_arc_zip:
			PStrApndCStr(sarc, ".zip");
			break;
		case gbk_arc_tar:
			PStrApndCStr(sarc, ".tar");
			break;
	}

	err = rMakeArchiveThing(
		&OutputDirR, NULL,
		&OutputDir0R, sarc);
	if (dupFNErr == err) {
		if (noErr == (err = dellib_DeleteOne_v2(&OutputDir0R, sarc))) {
			/* retry */
			err = rMakeArchiveThing(
				&OutputDirR, NULL,
				&OutputDir0R, sarc);
		}
	}

	if (noErr == err) {
		if (noErr != (err = dellib_DeleteOne_v2(&OutputDirR, NULL))) {
		} else if (noErr != (err = HaveDiskExtenstion_v2())) {
			/* don't bother */
			err = noErr;
		} else if (noErr != (err = DiskVersion_v2(&version))) {
			MyAlertFromCStr("DiskVersion fails");
			err = kMyErrReported;
		} else if (version < 2) {
			MyAlertFromCStr("Disk Extension Version version too old");
			err = kMyErrReported;
		} else if (noErr != (err = DiskFeatures_v2(&features))) {
			MyAlertFromCStr("DiskFeatures fails");
			err = kMyErrReported;
		} else if (0 == (features & ((ui5r)1 << kFeatureCmndDisk_New)))
		{
			MyAlertFromCStr("New disk creation not enabled");
			err = kMyErrReported;
		} else if (0 == (
			features & ((ui5r)1 << kFeatureCmndDisk_RawMode)))
		{
			MyAlertFromCStr("Raw mode access not enabled");
			err = kMyErrReported;
		} else {
			if (noErr == (err = ExportFromFile2HostFile2_v2(
				&OutputDir0R, sarc)))
			{
				err = dellib_DeleteOne_v2(&OutputDir0R, sarc);
			}
		}
	}

	return err;
}

LOCALFUNC tMyErr ProcessCommandLineArguments(void)
{
	tMyErr err;

	err = noErr;
	GNResetCommandLineParameters();
#ifdef Have_SPBLDOPT
	SPResetCommandLineParameters();
#endif
	while ((! The_arg_end) && (noErr == err)) {
		if (kMyErrNoMatch == (err = TryAsGNOptionNot()))
#ifdef Have_SPBLDOPT
		if (kMyErrNoMatch == (err = TryAsSPOptionNot()))
#endif
		{
			if (CurArgIsCStr_v2(";")) {
				AdvanceTheArg();
				err = noErr;
				goto Label_1;
			} else {
				ReportParseFailure("unknown switch");
				err = kMyErrReported;
			}
		}
		if (ParseArgsFailed && (noErr == err)) {
			err = kMyErrProgramBug;
		}
	}

Label_1:
	return err;
}

LOCALPROC DoDocTypeAddToMainRC(void)
{
	WriteBgnDestFileLn();
	WriteUnsignedToOutput(256 + DocTypeCounter);
	WriteCStrToDestFile(
		"                     ICON    DISCARDABLE     ");
	WriteQuoteToDestFile();
	WriteDocTypeIconFileName();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC WriteWinMainRCcontents(void)
{
	vCheckWriteDestErr(DoAllDocTypesWithSetup(DoDocTypeAddToMainRC));
}

LOCALFUNC tMyErr WriteWinMainRC(void)
{
	/* Resource Configuration file */

	return WriteADestFile(&SrcDirR, "main", ".rc",
		WriteWinMainRCcontents);
}

LOCALFUNC tMyErr WriteConfigFiles(void)
{
	tMyErr err;

	if (noErr == (err = WriteAppSpecificConfigFiles()))
	if ((! HaveMacRrscs) || (noErr == (err = WriteCommonCNFGRSRC())))
#if ! ManualMainRC
	if ((gbk_apifam_win != gbo_apifam)
		|| (noErr == (err = WriteWinMainRC())))
#endif
	{
		/* ok */
	}

	return err;
}

LOCALVAR MyDir_R SourceDirR;
LOCALVAR MyDir_R C_srcDirR;
LOCALVAR MyDir_R AltSrcDirR;
LOCALVAR MyDir_R PlatDirR;
LOCALVAR MyDir_R ApiDirR;
#if ! OSXplatsep
LOCALVAR MyDir_R IconDirR;
#endif
#if AsmSupported
LOCALVAR MyDir_R A_srcDirR;
#endif

struct RStr {
	MyCharPtr p;
	uimr n;
};
typedef struct RStr RStr;

GLOBALPROC RStrFromCStr(RStr *r, /*CONST*/ char *s)
{
	r->p = (MyCharPtr)s;
	r->n = CStrLength(s);
}

GLOBALPROC RStrFromPStr(RStr *r, ps3p s)
{
	MyCharPtr p1 = s;

	r->n = *p1++;
	r->p = p1;
}

GLOBALFUNC blnr RStrBeginsWith(RStr *s1, RStr *s2, RStr *r)
{
	register si4r i;
	MyCharPtr p1 = s1->p;
	MyCharPtr p2 = s2->p;
	MyCharR n = s1->n;
	MyCharR m = s2->n;

	if (n < m) {
		return falseblnr;
	} else {
		for (i = m; --i >= 0; ) {
			if (*p1++ != *p2++) {
				return falseblnr;
			}
		}
		if (r != nullpr) {
			r->p = s1->p + m;
			r->n = n - m;
		}
		return trueblnr;
	}
}

GLOBALFUNC blnr RStrEndsWith(RStr *s1, RStr *s2, RStr *r)
{
	register si4r i;
	MyCharPtr p1 = s1->p;
	MyCharPtr p2 = s2->p;
	MyCharR n = s1->n;
	MyCharR m = s2->n;

	if (n < m) {
		return falseblnr;
	} else {
		p1 += (n - m);
		for (i = m; --i >= 0; ) {
			if (*p1++ != *p2++) {
				return falseblnr;
			}
		}
		if (r != nullpr) {
			r->p = s1->p;
			r->n = n - m;
		}
		return trueblnr;
	}
}

GLOBALFUNC blnr RStrEqual(RStr *s1, RStr *s2)
{
	register si4r i;
	MyCharPtr p1 = s1->p;
	MyCharPtr p2 = s2->p;
	MyCharR n = s1->n;
	MyCharR m = s2->n;

	if (n != m) {
		return falseblnr;
	} else {
		for (i = m; --i >= 0; ) {
			if (*p1++ != *p2++) {
				return falseblnr;
			}
		}
		return trueblnr;
	}
}

LOCALPROC strmo_writeRStr(RStr *r)
{
	strmo_write((MyPtr)r->p, SizeOfListMyChar(r->n));
}

LOCALFUNC blnr strmi_ReadLn(ps3p s)
{
	PStrClear(s);

Label_1:
	if (! strmi_neof) {
		return 0 != PStrLength(s);
	} else if (13 != strmi_peek) {
		PStrApndChar(s, strmi_peek);
		strmi_advance();
		goto Label_1;
	} else {
		strmi_advance();
		return trueblnr;
	}
}

GLOBALFUNC strmi_ReadLnRStr(RStr *r, ps3p s)
{
	if (! strmi_ReadLn(s)) {
		return falseblnr;
	} else {
		RStrFromPStr(r, s);
		return trueblnr;
	}
}

LOCALPROC rConverPPCAsmText0(void)
{
	MyPStr s;
	RStr r;
	RStr r2;
	RStr r3;
	RStr t_include_pre_a;
	RStr t_include_post_a;
	RStr t_include_pre_b;
	RStr t_include_post_b;
	RStr t_if_pre_a;
	RStr t_if_pre_b;
	RStr t_endif_pre_a;
	RStr t_endif_pre_b;
	RStr t_comment1_start_a;
	RStr t_comment1_mid_a;
	RStr t_comment1_end_a;
	RStr t_comment1_mid_b;
	RStr t_comment2_start_a;
	RStr t_comment2_mid_a;
	RStr t_comment2_end_a;
	RStr t_comment2_mid_b;
	RStr t_comment1a_pre_a;
	RStr t_comment1a_post_a;
	RStr t_comment1a_pre_b;
	RStr t_comment2a_pre_a;
	RStr t_comment2a_post_a;
	RStr t_comment2a_pre_b;
	RStr t_comment3a_pre_a;
	RStr t_comment3a_post_a;
	RStr t_comment3a_pre_b;

	RStrFromCStr(&t_include_pre_a, "#include\t\"");
	RStrFromCStr(&t_include_post_a, "\"");
	RStrFromCStr(&t_include_pre_b, "\tinclude\t'");
	RStrFromCStr(&t_include_post_b, "'");

	RStrFromCStr(&t_if_pre_a, "#if ");
	RStrFromCStr(&t_if_pre_b, "\tif ");

	RStrFromCStr(&t_endif_pre_a, "#endif");
	RStrFromCStr(&t_endif_pre_b, "\tendif");

	RStrFromCStr(&t_comment1_start_a, "/*");
	RStrFromCStr(&t_comment1_mid_a, "\t");
	RStrFromCStr(&t_comment1_end_a, "*/");
	RStrFromCStr(&t_comment1_mid_b, "# ");

	RStrFromCStr(&t_comment2_start_a, "\t/*");
	RStrFromCStr(&t_comment2_mid_a, "\t\t");
	RStrFromCStr(&t_comment2_end_a, "\t*/");
	RStrFromCStr(&t_comment2_mid_b, "\t# ");

	RStrFromCStr(&t_comment1a_pre_a, "/* ");
	RStrFromCStr(&t_comment1a_post_a, " */");
	RStrFromCStr(&t_comment1a_pre_b, "# ");

	RStrFromCStr(&t_comment2a_pre_a, "\t/* ");
	RStrFromCStr(&t_comment2a_post_a, " */");
	RStrFromCStr(&t_comment2a_pre_b, "\t# ");

	RStrFromCStr(&t_comment3a_pre_a, "\t\t/* ");
	RStrFromCStr(&t_comment3a_post_a, " */");
	RStrFromCStr(&t_comment3a_pre_b, "\t\t# ");

	while (strmi_ReadLnRStr(&r, s)) {
		if (RStrBeginsWith(&r, &t_include_pre_a, &r2)
			&& RStrEndsWith(&r2, &t_include_post_a, &r3))
		{
			strmo_writeRStr(&t_include_pre_b);
			strmo_writeRStr(&r3);
			strmo_writeRStr(&t_include_post_b);
			MyWriteEolChar();
		} else
		if (RStrBeginsWith(&r, &t_if_pre_a, &r2)) {
			strmo_writeRStr(&t_if_pre_b);
			strmo_writeRStr(&r2);
			MyWriteEolChar();
		} else
		if (RStrEqual(&r, &t_endif_pre_a)) {
			strmo_writeRStr(&t_endif_pre_b);
			MyWriteEolChar();
		} else
		if (RStrEqual(&r, &t_comment1_start_a)) {
			while (strmi_ReadLnRStr(&r, s)
				&& ! RStrEqual(&r, &t_comment1_end_a))
			{
				if (RStrBeginsWith(&r, &t_comment1_mid_a, &r2)) {
					strmo_writeRStr(&t_comment1_mid_b);
					strmo_writeRStr(&r2);
				} else {
					strmo_writeRStr(&r); /* oops */
				}
				MyWriteEolChar();
			}
		} else
		if (RStrEqual(&r, &t_comment2_start_a)) {
			while (strmi_ReadLnRStr(&r, s)
				&& ! RStrEqual(&r, &t_comment2_end_a))
			{
				if (RStrBeginsWith(&r, &t_comment2_mid_a, &r2)) {
					strmo_writeRStr(&t_comment2_mid_b);
					strmo_writeRStr(&r2);
				} else {
					strmo_writeRStr(&r); /* oops */
				}
				MyWriteEolChar();
			}
		} else
		if (RStrBeginsWith(&r, &t_comment1a_pre_a, &r2)
			&& RStrEndsWith(&r2, &t_comment1a_post_a, &r3))
		{
			strmo_writeRStr(&t_comment1a_pre_b);
			strmo_writeRStr(&r3);
			MyWriteEolChar();
		} else
		if (RStrBeginsWith(&r, &t_comment2a_pre_a, &r2)
			&& RStrEndsWith(&r2, &t_comment2a_post_a, &r3))
		{
			strmo_writeRStr(&t_comment2a_pre_b);
			strmo_writeRStr(&r3);
			MyWriteEolChar();
		} else
		if (RStrBeginsWith(&r, &t_comment3a_pre_a, &r2)
			&& RStrEndsWith(&r2, &t_comment3a_post_a, &r3))
		{
			strmo_writeRStr(&t_comment3a_pre_b);
			strmo_writeRStr(&r3);
			MyWriteEolChar();
		} else
		{
			strmo_writeRStr(&r);
			MyWriteEolChar();
		}
	}
}

LOCALFUNC tMyErr rConverPPCAsmText(
	MyDir_R *src_d, MyDir_R *dst_d,
	char *name, char *FileExt)
{
	tMyErr err;
	Str255 s;

	PStrFromCStr(s, name);
	PStrApndCStr(s, FileExt);
	if (noErr == (err = strmo_open_v2(dst_d, s))) {
		if (noErr == (err = strmi_open_v2(src_d, s, nullpr))) {
			rConverPPCAsmText0();
			err = strmi_close_v2();
		}
		err = CombineErr(err, strmo_close_v2());
		if (noErr != err) {
			(void) MyDeleteFile_v2(dst_d, s);
				/* ignore any error, since already got one */
		}
	}

	return err;
}

LOCALPROC DoSrcFileAddToSrcDir(void)
{
	tMyErr err;
#if AsmSupported
	blnr IsAsmFile = HaveAsm
		&& ((DoSrcFile_gd()->Flgm & kCSrcFlgmAsmAvail) != 0);
#endif
	blnr IsAltFile = ((DoSrcFile_gd()->Flgm & kCSrcFlgmAltSrc) != 0);
	blnr NoHeader = ((DoSrcFile_gd()->Flgm & kCSrcFlgmNoHeader) != 0);
	blnr UseAPI = ((DoSrcFile_gd()->Flgm & kCSrcFlgmUseAPI) != 0);

	if (NoHeader || (noErr == (err = rConverTextInThingXtn(&C_srcDirR,
			&SrcDirR, DoSrcFile_gd()->s, ".h"))))
	{
		if (IsAltFile) {
			if (noErr == (err = rConverTextInThingXtn(&AltSrcDirR,
				&SrcDirR, DoSrcFile_gd()->s, ".c")))
			{
			}
		} else
#if AsmSupported
		if (IsAsmFile) {
			switch (cur_asm) {
				case gbk_asm_mpwppc:
					if (noErr == (err = rConverPPCAsmText(&A_srcDirR,
						&SrcDirR, DoSrcFile_gd()->s, ".S")))
					{
					}
					break;
				default:
					if (noErr == (err = rConverTextInThingXtn(
						&A_srcDirR,
						&SrcDirR, DoSrcFile_gd()->s, ".S")))
					{
					}
					break;
			}
		} else
#endif
		if (UseAPI) {
			if (noErr == (err = rConverTextInThingXtn(&ApiDirR,
				&SrcDirR, DoSrcFile_gd()->s, GetSrcFileFileXtns())))
			{
			}
		} else
		{
			if (noErr == (err = rConverTextInThingXtn(&C_srcDirR,
				&SrcDirR, DoSrcFile_gd()->s, GetSrcFileFileXtns())))
			{
			}
		}
	}

	DoSrcFile_gd()->err = err;
}

LOCALPROC DoExtraHeaderFileAddToSrcDir(void)
{
	tMyErr err = noErr;

	switch (DoXtraHdr_gd()->DepDir) {
		case kDepDirCSrc:
			err = rConverTextInThingXtn(&C_srcDirR,
				&SrcDirR, DoXtraHdr_gd()->s, ".h");
			break;
#if AsmSupported
		case kDepDirASrc:
			err = rConverTextInThingXtn(&A_srcDirR,
				&SrcDirR, DoXtraHdr_gd()->s, ".h");
			break;
#endif
		case kDepDirPlat:
			err = rConverTextInThingXtn(&PlatDirR,
				&SrcDirR, DoXtraHdr_gd()->s, ".h");
			break;
		case kDepDirLang:
			{
				MyDir_R LanguageDirR;
				MyDir_R LangDirR;

				if (noErr == (err = FindSubDirectory_v2(&LanguageDirR,
					&SourceDirR, "language", "")))
				if (noErr == (err = FindSubDirectory_v2(&LangDirR,
					&LanguageDirR, GetLangName(gbo_lang), "")))
				{
					err = rConverTextInThingXtn(&LangDirR,
						&SrcDirR, DoXtraHdr_gd()->s, ".h");
				}
			}
			break;
#if MayUseSound
		case kDepDirSndA:
			{
				MyDir_R SoundDirR;
				MyDir_R SndDirR;

				if (noErr == (err = FindSubDirectory_v2(&SoundDirR,
					&SourceDirR, "sound", "")))
				if (noErr == (err = FindSubDirectory_v2(&SndDirR,
					&SoundDirR, GetSndApiName(gbo_sndapi), "")))
				{
					err = rConverTextInThingXtn(&SndDirR,
						&SrcDirR, DoXtraHdr_gd()->s, ".h");
				}
			}
			break;
#endif
		case kDepDirCnfg:
			break;
	}

	DoXtraHdr_gd()->err = err;
}

LOCALPROC DoDocTypeAddToSrcDir(void)
{
	tMyErr err = noErr;
	Str255 s;

	PStrFromCStr(s, DoDocType_gd()->ShortName);

	switch (gbo_targfam) {
		case gbk_targfam_cmac:
			PStrApndCStr(s, "Icon.r");
			err = duplib_WriteFile_v2(&PlatDirR, s, &SrcDirR, s);
			break;
		case gbk_targfam_mach:
		case gbk_targfam_carb:
			PStrApndCStr(s, "Icon.icns");
#if OSXplatsep
			err = duplib_WriteFile_v2(&PlatDirR, s, &SrcDirR, s);
#else
			err = duplib_WriteFile_v2(&IconDirR, s, &SrcDirR, s);
#endif
			break;
		case gbk_targfam_mswn:
		case gbk_targfam_wnce:
			PStrApndCStr(s, "Icon.ico");
			err = duplib_WriteFile_v2(&PlatDirR, s, &SrcDirR, s);
			break;
		default:
			break;
	}

	DoDocType_gd()->err = err;
}

LOCALFUNC tMyErr MayFindPlatformDirectory(void)
{
	tMyErr err;
	MyDir_R PlatformDirR;

	if (noErr == (err = FindSubDirectory_v2(&PlatformDirR,
		&SourceDirR, "platform", "")))
	if (noErr == (err = FindSubDirectory_v2(&PlatDirR,
		&PlatformDirR, GetPlatformDirName(), "")))
#if ! OSXplatsep
	if ((gbk_apifam_osx != gbo_apifam)
		|| (noErr == (err = FindSubDirectory_v2(
			&IconDirR, &PlatDirR, "osx", ""))))
#endif
	{
		/* ok */
	}

	return err;
}

LOCALFUNC tMyErr MayFindApiDirectory(void)
{
	tMyErr err;
	MyDir_R ApiDir0R;

	if (noErr == (err = FindSubDirectory_v2(&ApiDir0R,
		&SourceDirR, "api", "")))
	if (noErr == (err = FindSubDirectory_v2(&ApiDirR,
		&ApiDir0R, GetAPIFamName(gbo_apifam), "")))
	{
		/* ok */
	}

	return err;
}

LOCALFUNC tMyErr MayFindAltSrcDirectory(void)
{
	tMyErr err;

	if (! HaveAltSrc) {
		err = noErr;
	} else {
		if (noErr == (err = FindSubDirectory_v2(&AltSrcDirR,
			&SourceDirR, "alt_src", "")))
		{
			/* ok */
		}
	}

	return err;
}

LOCALFUNC char * GetAsmDirName(void)
{
	char *s;

	switch (gbo_cpufam) {
		case gbk_cpufam_ppc:
			s = "ppc";
			break;
		case gbk_cpufam_x86:
			s = "x86";
			break;
		case gbk_cpufam_68k:
			s = "68k";
			break;
		case gbk_cpufam_arm:
			s = "arm";
			break;
		default:
			s = "???";
			break;
	}

	return s;
}

#if AsmSupported
LOCALFUNC tMyErr MayFindASrcDirectory(void)
{
	tMyErr err;

	if (! HaveAsm) {
		err = noErr;
	} else {
		MyDir_R A_src0DirR;

		if (noErr == (err = FindSubDirectory_v2(&A_src0DirR,
			&SourceDirR, "a_src", "")))
		if (noErr == (err = FindSubDirectory_v2(&A_srcDirR,
			&A_src0DirR, GetAsmDirName(), "")))
		{
			/* ok */
		}
	}

	return err;
}
#endif

LOCALFUNC tMyErr MakeSrcFolder(void)
{
	tMyErr err;

	if (noErr == (err = FindSubDirectory_v2(&SourceDirR,
		&BaseDirR, "data", "")))
	if (noErr == (err = FindSubDirectory_v2(&C_srcDirR,
		&SourceDirR, "c_src", "")))
	if (noErr == (err = MayFindPlatformDirectory()))
	if (noErr == (err = MayFindApiDirectory()))
	if (noErr == (err = MayFindAltSrcDirectory()))
#if AsmSupported
	if (noErr == (err = MayFindASrcDirectory()))
#endif
	if (noErr == (err = MakeSubDirectory_v2(&SrcDirR,
		&OutputDirR, src_d_name, "")))
	if (noErr == (err = DoAllSrcFilesWithSetup(DoSrcFileAddToSrcDir)))
	if (noErr == (err = DoAllExtraHeaders2WithSetup(
		DoExtraHeaderFileAddToSrcDir)))
	if (noErr == (err = DoAllDocTypesWithSetup(DoDocTypeAddToSrcDir)))
	{
		switch (gbo_apifam) {
			case gbk_apifam_mac:
			case gbk_apifam_osx:
				if (HaveMacRrscs)
				if (noErr == (err = rConverTextInThingXtn(&PlatDirR,
					&SrcDirR, "main", ".r")))
				{
					/* ok */
				}
				break;
#if ManualMainRC
			case gbk_apifam_win:
				if (noErr == (err = rConverTextInThingXtn(&PlatDirR,
					&SrcDirR, "main", ".rc")))
				if (noErr == (err = rConverTextInThingXtn(&PlatDirR,
					&SrcDirR, "resourc1", ".h")))
				if (noErr == (err = rConverTextInThingXtn(&PlatDirR,
					&SrcDirR, "RESOURCE", ".HM")))
				{
					/* ok */
				}
				break;
#endif
			default:
				break;
		}
	}

	return err;
}

LOCALPROC DoSrcFileScanSettings(void)
{
#if AsmSupported
	blnr IsAsmFile = ((DoSrcFile_gd()->Flgm & kCSrcFlgmAsmAvail) != 0);
#endif
	blnr IsAltFile = ((DoSrcFile_gd()->Flgm & kCSrcFlgmAltSrc) != 0);

#if AsmSupported
	HaveAsm |= IsAsmFile;
#endif
	HaveAltSrc |= IsAltFile;
}

LOCALPROC DoExtraHeaderFileScanSettings(void)
{
	switch (DoXtraHdr_gd()->DepDir) {
		case kDepDirCSrc:
			break;
#if AsmSupported
		case kDepDirASrc:
			HaveAsm = trueblnr;
			break;
#endif
		case kDepDirPlat:
		case kDepDirLang:
		case kDepDirSndA:
		case kDepDirCnfg:
			break;
	}
}

LOCALFUNC tMyErr ScanSourceSettings(void)
{
	tMyErr err;

#if AsmSupported
	HaveAsm = falseblnr;
#endif
	HaveAltSrc = falseblnr;

	if (noErr == (err = DoAllSrcFilesWithSetup(DoSrcFileScanSettings)))
	if (noErr == (err = DoAllExtraHeaders2WithSetup(
		DoExtraHeaderFileScanSettings)))
	{
		/* ok */
	}

#if AsmSupported
	HaveAsm &= (gbk_asm_none != cur_asm);
#endif

	return err;
}

LOCALFUNC tMyErr DoTheCommand0(void)
{
	tMyErr err;

	if (noErr == (err = ProcessCommandLineArguments()))
	if (noErr == (err = AutoChooseGNSettings()))
#ifdef Have_SPBLDOPT
	if (noErr == (err = AutoChooseSPSettings()))
#endif
	if (noErr == (err = ScanSourceSettings()))
	if (noErr == (err = WriteMakeOutputDirectories()))
	if (noErr == (err = MakeSrcFolder()))
	if (noErr == (err = WriteConfigFiles()))
	if ((! CurPrintCFiles) || (noErr == (err = WriteCFilesList())))
	if (noErr == (err = WriteIdeSpecificFiles()))
	if ((NotWantExport) || (noErr == (err = ArchiveAndExport())))
	{
	}

	return err;
}

LOCALFUNC tMyErr DoTheCommand(void)
{
	tMyErr err;

	do {
		err = DoTheCommand0();
	} while ((! The_arg_end)
		&& (! ParseArgsFailed)
		&& (noErr == err));

	return err;
}
