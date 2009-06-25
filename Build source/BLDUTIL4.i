/*
	BLDUTIL4.i
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
	BuiLD system UTILities part 4
*/


LOCALFUNC blnr rConverTextInThing(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s)
{
	blnr IsOk = falseblnr;

	switch (cur_eol) {
		case gbk_eol_mac:
			IsOk = duplib_WriteFile(src_d, src_s,
				dst_d, dst_s);
			break;
		case gbk_eol_win:
			IsOk = rinscharafter_WriteFile(src_d, src_s,
				dst_d, dst_s,
				((char)13), ((char)10));
			break;
		case gbk_eol_unx:
			IsOk = rsubstchar_WriteFile(src_d, src_s,
				dst_d, dst_s,
				((char)13), ((char)10));
			break;
	}

	return IsOk;
}

LOCALFUNC blnr rConverTextInThingXtn(
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

LOCALFUNC OSErr FindPrefFolder(MyDir_R *d)
{
	OSErr err;
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
			err = MyDirFromWD0(info.sysVRefNum, d);
		}
	}

	return err;
}

LOCALFUNC blnr FindOutPutDir(void)
{
	OSErr err;
	MyDir_R PrefDirR;
	MyDir_R GryphelDirR;
	MyDir_R BuildDirR;

	if (noErr == (err = FindPrefFolder(&PrefDirR)))
	if (noErr == (err = MyResolveNamedChildDir0(&PrefDirR, "\pGryphel", &GryphelDirR)))
	if (noErr == (err = MyResolveNamedChildDir0(&GryphelDirR, "\pBuild", &BuildDirR)))
	{
		/* ok */
	}

	if (noErr == err) {
		return FindOrMakeSubDirectory(&OutputDir0R, &BuildDirR, "output", "");
	} else {
		return FindOrMakeSubDirectory(&OutputDir0R, &BaseDirR, "output", "");
	}
}

LOCALFUNC blnr WriteMakeOutputDirectories(void)
{
	Str255 s;
	blnr IsOk = falseblnr;

	HandToPStr(hVariationName, s);
	if (MyHGetDir(&BaseDirR))
	if (FindOutPutDir())
	if (DelMakeNamedDir(&OutputDirR, &OutputDir0R, s))
	if (rConverTextInThingXtn(&BaseDirR,
		&OutputDirR, "README", ".txt"))
	if (rConverTextInThingXtn(&BaseDirR,
		&OutputDirR, "COPYING", ".txt"))
	{
		if ((cur_ide == gbk_ide_xcd) && (! UseCmndLine)) {
			IsOk = MakeSubDirectory(&ProjDirR, &OutputDirR, vStrAppAbbrev,
				(ide_vers >= 2100) ? ".xcodeproj" : ".pbproj");
		} else if (cur_ide != gbk_ide_mw8) {
			if (MakeSubDirectory(&ObjDirR, &OutputDirR, obj_d_name, "")) {
				if (WriteOpenDestFile(&ObjDirR, "dummy", "")) { /* Dummy */
					WriteDestFileLn("This file is here because some archive extraction");
					WriteDestFileLn("software will not create an empty directory.");
					WriteCloseDestFile();
					IsOk = trueblnr;
				}
			}
		} else {
			IsOk = trueblnr;
		}
	}

	return IsOk;
}

#pragma segment Main

LOCALPROC WriteIdeSpecificFiles(void)
{
	switch (cur_ide) {
		case gbk_ide_mpw:
			WriteMPWSpecificFiles();
			break;
		case gbk_ide_mw8:
			WriteMetrowerksSpecificFiles();
			break;
		case gbk_ide_bgc:
			WriteBashGccSpecificFiles();
			break;
		case gbk_ide_snc:
			WriteSncSpecificFiles();
			break;
		case gbk_ide_msv:
			if (UseCmndLine) {
				WriteNMakeSpecificFiles();
			} else {
				if (ide_vers >= 8000) {
					WriteMSVCXMLSpecificFiles();
				} else {
					WriteMSVCSpecificFiles();
				}
			}
			break;
		case gbk_ide_lcc:
			if (UseCmndLine) {
				WriteLccW32clSpecificFiles();
			} else {
				WriteLccW32SpecificFiles();
			}
			break;
		case gbk_ide_dvc:
			WriteDevCSpecificFiles();
			break;
		case gbk_ide_xcd:
			if (UseCmndLine) {
				WriteBashGccSpecificFiles();
			} else {
				WriteXCDSpecificFiles();
			}
			break;
	}
}

LOCALFUNC blnr rMakeArchiveThing(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s)
{
	blnr IsOk = falseblnr;

	switch (cur_arc) {
		case gbk_arc_sit:
			IsOk = sitlib_WriteFile(
				src_d, src_s,
				dst_d, dst_s);
			break;
		case gbk_arc_zip:
			IsOk = nzplib_WriteFile(
				src_d, src_s,
				dst_d, dst_s);
			break;
		case gbk_arc_tar:
			IsOk = tarlib_WriteTarFile2(
				src_d, src_s,
				dst_d, dst_s);
			break;
	}

	return IsOk;
}

LOCALPROC ArchiveAndExport(void)
{
	Str255 sarc;
	ui4b version;
	ui5b features;

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

	if (rMakeArchiveThing(
		&OutputDirR, NULL,
		&OutputDir0R, sarc))
	{
		(void) dellib_DeleteOne(&OutputDirR, NULL);
		if (! HaveDiskExtenstion()) {
			/* don't bother */
		} else if (! DiskVersion(&version)) {
			DisplayRunErr("DiskVersion fails");
		} else if (version < 2) {
			DisplayRunErr("Disk Extension Version version too old");
		} else if (! DiskFeatures(&features)) {
			DisplayRunErr("DiskFeatures fails");
		} else if (0 == (features & ((ui5b)1 << kFeatureCmndDisk_New))) {
			DisplayRunErr("New disk creation not enabled");
		} else if (0 == (features & ((ui5b)1 << kFeatureCmndDisk_RawMode))) {
			DisplayRunErr("Raw mode access not enabled");
		} else {
			if (ExportFromFile2HostFile2(&OutputDir0R, sarc)) {
				(void) dellib_DeleteOne(&OutputDir0R, sarc);
			}
		}
	}
}

LOCALFUNC blnr ProcessCommandLineArguments(void)
{
	GNResetCommandLineParameters();
#ifdef Have_SPBLDOPT
	SPResetCommandLineParameters();
#endif
	while (! The_arg_end) {
		if (TryAsGNOptionNot())
#ifdef Have_SPBLDOPT
		if (TryAsSPOptionNot())
#endif
		{
			if (CurArgIsCStr(";")) {
				AdvanceTheArg();
				return trueblnr;
			} else {
				ReportParseFailure("unknown switch");
			}
		}
		if (ParseArgsFailed) {
			return falseblnr;
		}
	}
	return trueblnr;
}

LOCALPROC DoDocTypeAddToMainRC(void)
{
	WriteBgnDestFileLn();
	WriteUnsignedToOutput(256 + DocTypeCounter);
	WriteCStrToDestFile("                     ICON    DISCARDABLE     ");
	WriteQuoteToDestFile();
	WriteDocTypeIconFileName();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC WriteWinMainRC(void)
{
	if (WriteOpenDestFile(&SrcDirR, "main", ".rc")) { /* Resource Configuration file */
		DoAllDocTypesWithSetup(DoDocTypeAddToMainRC);
		WriteCloseDestFile();
	}
}

LOCALPROC WriteConfigFiles(void)
{
	WriteCommonCNFGGLOB();
	WriteCommonCNFGRAPI();
#ifdef Have_SPOTHRCF
	WriteAppSpecificConfigFiles();
#endif

	if (HaveMacRrscs) {
		WriteCommonCNFGRSRC();
	}
	if (gbk_apifam_win == gbo_apifam) {
		WriteWinMainRC();
	}
	WritePPCasmMINEM68K();
}

LOCALVAR MyDir_R SourceDirR;
LOCALVAR MyDir_R C_srcDirR;
LOCALVAR MyDir_R AltSrcDirR;
LOCALVAR MyDir_R LangDirR;
LOCALVAR MyDir_R PlatDirR;
#if ! OSXplatsep
LOCALVAR MyDir_R IconDirR;
#endif

LOCALPROC DoSrcFileAddToSrcDir(void)
{
	blnr IsAsmFile = HaveAsm && ((DoSrcFile_gd()->Flgm & kCSrcFlgmAsmAvail) != 0);
	blnr IsAltFile = ((DoSrcFile_gd()->Flgm & kCSrcFlgmAltSrc) != 0);

	if (rConverTextInThingXtn(&C_srcDirR,
			&SrcDirR, DoSrcFile_gd()->s, ".h"))
	{
		if (IsAltFile) {
			if (rConverTextInThingXtn(&AltSrcDirR,
					&SrcDirR, DoSrcFile_gd()->s, ".c"))
			{
			}
		} else if (IsAsmFile) {
			/* must be generated */
		} else {
			if (rConverTextInThingXtn(&C_srcDirR,
					&SrcDirR, DoSrcFile_gd()->s, ".c"))
			{
			}
		}
	}
}

LOCALPROC DoExtraHeaderFileAddToSrcDir(int DepDir,
	char *s)
{
	switch (DepDir) {
		case kDepDirCSrc:
			if (rConverTextInThingXtn(&C_srcDirR,
					&SrcDirR, s, ""))
			{
			}
			break;
		case kDepDirPlat:
			if (rConverTextInThingXtn(&PlatDirR,
					&SrcDirR, s, ""))
			{
			}
			break;
		case kDepDirLang:
			if (rConverTextInThingXtn(&LangDirR,
					&SrcDirR, s, ""))
			{
			}
			break;
		case kDepDirCnfg:
			break;
	}
}

LOCALPROC DoDocTypeAddToSrcDir(void)
{
	Str255 s;

	PStrFromCStr(s, DoDocType_gd()->ShortName);

	switch (gbo_apifam) {
		case gbk_apifam_mac:
			PStrApndCStr(s, "Icon.r");
			if (duplib_WriteFile(&PlatDirR, s,
						&SrcDirR, s))
			{
			}
			break;
		case gbk_apifam_osx:
			PStrApndCStr(s, "Icon.icns");
#if OSXplatsep
			if (duplib_WriteFile(&PlatDirR, s, &SrcDirR, s))
#else
			if (duplib_WriteFile(&IconDirR, s, &SrcDirR, s))
#endif
			{
			}
			break;
		case gbk_apifam_win:
			PStrApndCStr(s, "Icon.ico");
			if (duplib_WriteFile(&PlatDirR, s,
						&SrcDirR, s))
			{
			}
			break;
		default:
			break;
	}
}

LOCALFUNC blnr MayFindLanguageDirectory(void)
{
	if (! HaveLanguage) {
		return trueblnr;
	} else {
		MyDir_R LanguageDirR;

		if (FindSubDirectory(&LanguageDirR, &SourceDirR, "language", ""))
		if (FindSubDirectory(&LangDirR, &LanguageDirR, GetLangName(gbo_lang), ""))
		{
			return trueblnr;
		}
		return falseblnr;
	}
}

LOCALFUNC blnr MayFindPlatformDirectory(void)
{
#if 0 /* still need icons */
	if (! HavePlatform) {
		return trueblnr;
	} else
#endif
	{
		MyDir_R PlatformDirR;

		if (FindSubDirectory(&PlatformDirR, &SourceDirR, "platform", ""))
		if (FindSubDirectory(&PlatDirR, &PlatformDirR, GetPlatformDirName(), ""))
#if ! OSXplatsep
		if ((gbk_apifam_osx != gbo_apifam) || FindSubDirectory(&IconDirR, &PlatDirR, "osx", ""))
#endif
		{
			return trueblnr;
		}
		return falseblnr;
	}
}

LOCALFUNC blnr MayFindAltSrcDirectory(void)
{
	if (! HaveAltSrc) {
		return trueblnr;
	} else {
		if (FindSubDirectory(&AltSrcDirR, &SourceDirR, "alt_src", ""))
		{
			return trueblnr;
		}
		return falseblnr;
	}
}

LOCALFUNC blnr MakeSrcFolder(void)
{
	if (FindSubDirectory(&SourceDirR, &BaseDirR, "source", ""))
	if (FindSubDirectory(&C_srcDirR, &SourceDirR, "c_src", ""))
	if (MayFindLanguageDirectory())
	if (MayFindPlatformDirectory())
	if (MayFindAltSrcDirectory())
	if (MakeSubDirectory(&SrcDirR, &OutputDirR, src_d_name, ""))
	{
		DoAllSrcFilesWithSetup(DoSrcFileAddToSrcDir);
		DoAllExtraHeaders(DoExtraHeaderFileAddToSrcDir);
		DoAllDocTypesWithSetup(DoDocTypeAddToSrcDir);
		switch (gbo_apifam) {
			case gbk_apifam_mac:
			case gbk_apifam_osx:
				if (HaveMacRrscs)
				if (rConverTextInThingXtn(&PlatDirR,
					&SrcDirR, "main", ".r"))
				{
				}
				break;
#if 0
			case gbk_apifam_win:
				if (rConverTextInThingXtn(&PlatDirR,
					&SrcDirR, "main", ".rc"))
				{
				}
				break;
#endif
			default:
				break;
		}
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC DoSrcFileScanSettings(void)
{
	blnr IsAsmFile = ((DoSrcFile_gd()->Flgm & kCSrcFlgmAsmAvail) != 0);
	blnr IsAltFile = ((DoSrcFile_gd()->Flgm & kCSrcFlgmAltSrc) != 0);

	HaveAsm |= IsAsmFile;
	HaveAltSrc |= IsAltFile;
}

LOCALPROC DoExtraHeaderFileScanSettings(int DepDir,
	char *s)
{
#pragma unused(s)
	switch (DepDir) {
		case kDepDirCSrc:
			break;
		case kDepDirPlat:
			HavePlatform = trueblnr;
			break;
		case kDepDirLang:
			HaveLanguage = trueblnr;
			break;
		case kDepDirCnfg:
			break;
	}
}

LOCALFUNC blnr ScanSourceSettings(void)
{
	HaveAsm = falseblnr;
	HaveAltSrc = falseblnr;
	HaveLanguage = falseblnr;
	HavePlatform = falseblnr;

	DoAllSrcFilesWithSetup(DoSrcFileScanSettings);
	DoAllExtraHeaders(DoExtraHeaderFileScanSettings);

	HaveAsm &= AsmPossible;

	return trueblnr;
}

LOCALPROC DoTheCommand0(void)
{
	if (ProcessCommandLineArguments())
	if (AutoChooseGNSettings())
#ifdef Have_SPBLDOPT
	if (AutoChooseSPSettings())
#endif
	if (ScanSourceSettings())
	if (WriteMakeOutputDirectories())
	if (MakeSrcFolder())
	{
		WriteConfigFiles();
		if (CurPrintCFiles) {
			WriteCFilesList();
		}
		WriteIdeSpecificFiles();

		if (noErr == SavedSysErr) {
			if (! NotWantExport) {
				ArchiveAndExport();
			}
		}
	}
}

LOCALPROC DoTheCommand(void)
{
	do {
		DoTheCommand0();
	} while ((! The_arg_end)
		&& (! ParseArgsFailed)
		&& (noErr == SavedSysErr));
}
