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

	MyHandleToPStr_v2(hVariationName, s);
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
#if ide_mvc_Supported
		case gbk_ide_mvc:
			err = WriteMVCSpecificFiles();
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

	MyHandleToPStr_v2(hVariationName, sarc);
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

	if (noErr != (err = rMakeArchiveThing(
		&OutputDirR, NULL,
		&OutputDir0R, sarc)))
	{
		/* fail */
	} else
	if (noErr != (err = dellib_DeleteOne_v2(&OutputDirR, NULL))) {
		/* fail */
	} else
	if (noErr != (err = HaveDiskExtenstion_v2())) {
		/* don't bother */
		err = noErr;
	} else
	if (noErr != (err = DiskVersion_v2(&version))) {
		MyAlertFromCStr("DiskVersion fails");
		err = kMyErrReported;
	} else
	if (version < 2) {
		MyAlertFromCStr("Disk Extension Version version too old");
		err = kMyErrReported;
	} else
	if (noErr != (err = DiskFeatures_v2(&features))) {
		MyAlertFromCStr("DiskFeatures fails");
		err = kMyErrReported;
	} else
	if (0 == (features & ((ui5r)1 << kFeatureCmndDisk_New)))
	{
		MyAlertFromCStr("New disk creation not enabled");
		err = kMyErrReported;
	} else
	if (0 == (
		features & ((ui5r)1 << kFeatureCmndDisk_RawMode)))
	{
		MyAlertFromCStr("Raw mode access not enabled");
		err = kMyErrReported;
	} else
	if (noErr != (err = ExportFromFile2HostFile2_v2(
		&OutputDir0R, sarc)))
	{
		/* fail */
	} else
	{
		err = dellib_DeleteOne_v2(&OutputDir0R, sarc);
	}

	return err;
}

LOCALPROC ResetAllCommandLineParameters(void)
{
	GNResetCommandLineParameters();
	GNDevResetCommandLineParameters();
#ifdef Have_SPBLDOPT
	SPResetCommandLineParameters();
#endif
	olv_cur = 1;
	OnlyUserOptions = falseblnr;
}

LOCALFUNC tMyErr TryAsAtOptionNot(void)
{
	tMyErr err;

	if (! CurArgIsCStr_v2("@")) {
		err = kMyErrNoMatch;
	} else
	if (OnlyUserOptions) {
		err = ReportParseFailure("Already have @");
	} else
	if (kMyErr_noErr != (err = AdvanceTheArg())) {
		/* fail */
	} else
	{
		OnlyUserOptions = trueblnr;
		err = kMyErr_noErr;
	}

	return err;
}

LOCALFUNC tMyErr TryAsXClmOptionNot(void)
{
	tMyErr err;

	if (! CurArgIsCStr_v2("!")) {
		err = kMyErrNoMatch;
	} else
	if (kMyErr_noErr != (err = AdvanceTheArg())) {
		/* fail */
	} else
	{
		err = noErr;
		++olv_cur;
	}

	return err;
}

LOCALFUNC tMyErr ProcessCommandLineArguments(void)
{
	tMyErr err;

	err = noErr;
	while ((! The_arg_end) && (noErr == err)) {
		if (kMyErrNoMatch == (err = TryAsGNOptionNot()))
		if (kMyErrNoMatch == (err = TryAsGNDevOptionNot()))
#ifdef Have_SPBLDOPT
		if (kMyErrNoMatch == (err = TryAsSPOptionNot()))
#endif
		if (kMyErrNoMatch == (err = TryAsAtOptionNot()))
		if (kMyErrNoMatch == (err = TryAsXClmOptionNot()))
		{
			if (CurArgIsCStr_v2(";")) {
				err = AdvanceTheArg();
				goto Label_1;
			} else {
				err = ReportParseFailure("unknown switch");
			}
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

	return WriteADestFile(&CfgDirR, "main", ".rc",
		WriteWinMainRCcontents);
}

LOCALFUNC tMyErr WriteConfigFiles(void)
{
	tMyErr err;

	if (noErr == (err = WriteAppSpecificConfigFiles()))
	if ((! HaveMacRrscs) || (noErr == (err = WriteCommonCNFGRSRC())))
	if ((gbk_apifam_win != gbo_apifam)
		|| (noErr == (err = WriteWinMainRC())))
	{
		/* ok */
	}

	return err;
}

LOCALVAR MyDir_R SourceDirR;
LOCALVAR MyDir_R C_srcDirR;


LOCALPROC DoSrcFileAddToSrcDir(void)
{
	tMyErr err = noErr;

	if ((0 == (DoSrcFile_gd()->Flgm & kCSrcFlgmSkip)) || WantAllSrc) {
		if (0 == (DoSrcFile_gd()->Flgm & kCSrcFlgmNoHeader)) {
			switch (DoSrcFile_gd()->DepDir) {
				case kDepDirCSrc:
					err = rConverTextInThingXtn(&C_srcDirR,
						&SrcDirR, DoSrcFile_gd()->s, ".h");
					break;
				case kDepDirCnfg:
					break;
			}
		}
		if (noErr == err) {
			if (0 == (DoSrcFile_gd()->Flgm & kCSrcFlgmNoSource)) {
				err = rConverTextInThingXtn(&C_srcDirR,
					&SrcDirR, DoSrcFile_gd()->s, GetSrcFileFileXtns());
			}
		}
	}

	DoSrcFile_gd()->err = err;
}

LOCALPROC DoDocTypeAddToSrcDir(void)
{
	tMyErr err = noErr;
	Str255 s;

	if ((noErr == err) && (WantAllSrc
		|| (gbk_targfam_cmac == gbo_targfam)))
	{
		PStrFromCStr(s, "ICON");
		PStrApndCStr(s, DoDocType_gd()->ShortName);
		PStrApndCStr(s, "M.r");
		err = rConverTextInThing(&C_srcDirR, s, &SrcDirR, s);
	}
	if ((noErr == err) && (WantAllSrc
		|| (gbk_targfam_mach == gbo_targfam)
		|| (gbk_targfam_carb == gbo_targfam)))
	{
		PStrFromCStr(s, "ICON");
		PStrApndCStr(s, DoDocType_gd()->ShortName);
		PStrApndCStr(s, "O.icns");
		err = duplib_WriteFile_v2(&C_srcDirR, s, &SrcDirR, s);
	}
	if ((noErr == err) && (WantAllSrc
		|| (gbk_targfam_mswn == gbo_targfam)
		|| (gbk_targfam_wnce == gbo_targfam)))
	{
		PStrFromCStr(s, "ICON");
		PStrApndCStr(s, DoDocType_gd()->ShortName);
		PStrApndCStr(s, "W.ico");
		err = duplib_WriteFile_v2(&C_srcDirR, s, &SrcDirR, s);
	}

	DoDocType_gd()->err = err;
}

LOCALFUNC tMyErr MakeSrcFolder(void)
{
	tMyErr err;

	if (noErr == (err = FindSubDirectory_v2(&SourceDirR,
		&BaseDirR, "data", "")))
	if (noErr == (err = FindSubDirectory_v2(&C_srcDirR,
		&SourceDirR, "src", "")))
	if (noErr == (err = MakeSubDirectory_v2(&SrcDirR,
		&OutputDirR, src_d_name, "")))
	if ((! WantConfigDir)
		|| (noErr == (err = MakeSubDirectory_v2(&CfgDirR,
			&OutputDirR, cfg_d_name, ""))))
	if (noErr == (err = DoAllSrcFilesWithSetup0(DoSrcFileAddToSrcDir)))
	if (noErr == (err = DoAllDocTypesWithSetup0(DoDocTypeAddToSrcDir)))
	{
		if (! WantConfigDir) {
			CfgDirR = SrcDirR;
		}
		if (WantAllSrc || (HaveMacRrscs
			&& ((gbk_apifam_mac == gbo_apifam)
			|| (gbk_apifam_osx == gbo_apifam))))
		{
			if (noErr == (err = rConverTextInThingXtn(&C_srcDirR,
				&SrcDirR, "main", ".r")))
			{
				/* ok */
			}
		}
	}

	return err;
}

LOCALFUNC tMyErr WriteErrToFile0(MyDir_R *d, StringPtr s, tMyErr err0)
{
	MyPStr t;
	tMyErr err;

	if (noErr == (err = strmo_open_v2(d, s))) {
		switch (err0) {
			case kMyErr_noErr:
				/* ok */
				break;
			case kMyErrSyntaxErr:
				if (noErr == (err = MyHandleToCStr(SyntaxErrH, t))) {
					strmo_writeCStr(t);
				}
				break;
			default:
				strmo_writeCStr(GetTextForSavedSysErr_v2(err0));
				break;
		}
		err = strmo_close_v2();
	}

	return err;
}

LOCALFUNC tMyErr WriteErrToFile(tMyErr err0)
{
	tMyErr err;
	Str255 sarc;

	if (0 == vStrAppAbbrev[0]) {
		PStrFromCStr(sarc, kStrAppAbbrev);
	} else {
		PStrFromCStr(sarc, vStrAppAbbrev);
	}
	PStrApndCStr(sarc, ".err.txt");

	if (noErr != (err = MyHGetDir_v2(&BaseDirR))) {
		/* fail */
	} else
	if (noErr != (err = FindOutPutDir())) {
		/* fail */
	} else
	if (noErr != (err = WriteErrToFile0(&OutputDir0R,
		sarc, err0)))
	{
		/* fail */
	} else
	{
		err = noErr;
	}

	return err;
}

LOCALFUNC tMyErr DoTheCommand0(void)
{
	tMyErr err;

	ResetAllCommandLineParameters();

	if (noErr == (err = ProcessCommandLineArguments()))
	if (noErr == (err = AutoChooseGNSettings()))
	if (noErr == (err = AutoChooseGNDevSettings()))
#ifdef Have_SPBLDOPT
	if (noErr == (err = AutoChooseSPSettings()))
#endif
	if (noErr == (err = WriteMakeOutputDirectories()))
	if (noErr == (err = MakeSrcFolder()))
	if (noErr == (err = WriteConfigFiles()))
	if ((! CurPrintCFiles) || (noErr == (err = WriteCFilesList())))
	if (noErr == (err = WriteIdeSpecificFiles()))
	if ((NotWantExport) || (noErr == (err = ArchiveAndExport())))
	{
	}

	if ((kMyErr_noErr != err)
		&& (nanblnr != gbo_Err2File)
		&& gbo_Err2File)
	{
		err = WriteErrToFile(err);
	}

	return err;
}

LOCALFUNC tMyErr DoTheCommand(void)
{
	tMyErr err;

	do {
		err = DoTheCommand0();
	} while ((! The_arg_end)
		&& (noErr == err));

	return err;
}
