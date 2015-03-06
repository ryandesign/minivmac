/*
	USFILDEF.i
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
	USe program SPecific FILe DEFinitions
*/

LOCALPROC NullProc(void)
{
}

LOCALVAR unsigned int FileCounter;

struct DoSrcFile_r
{
	MyPtr SavepDt;
	char *s;
	long Flgm;
	tDoDependsForC depends;
	tMyErr err;
	MyProc p;
};

typedef struct DoSrcFile_r DoSrcFile_r;

#define DoSrcFile_gd() ((DoSrcFile_r *)(pDt))

LOCALPROC DoAllSrcFilesWithSetupProc(
	char *s, long Flgm, tDoDependsForC depends)
{
	if (noErr == DoSrcFile_gd()->err) {
		DoSrcFile_gd()->s = s;
		DoSrcFile_gd()->Flgm = Flgm;
		DoSrcFile_gd()->depends = depends;

		DoSrcFile_gd()->p();

		++FileCounter;
	}
}

LOCALFUNC tMyErr DoAllSrcFilesWithSetup(MyProc p)
{
	DoSrcFile_r r;

	r.SavepDt = pDt;
	r.p = p;
	r.err = noErr;
	pDt = (MyPtr)&r;

	FileCounter = 0;
	DoAllSrcFiles(DoAllSrcFilesWithSetupProc);

	pDt = r.SavepDt;

	return r.err;
}

LOCALPROC DoAllSrcFilesSortWithSetup1(
	char *s, long Flgm, tDoDependsForC depends)
{
	if (0 != (Flgm & kCSrcFlgmSortFirst)) {
		DoAllSrcFilesWithSetupProc(s, Flgm, depends);
	} else {
		++FileCounter;
	}
}

LOCALPROC DoAllSrcFilesSortWithSetup2(
	char *s, long Flgm, tDoDependsForC depends)
{
	if (0 == (Flgm & kCSrcFlgmSortFirst)) {
		DoAllSrcFilesWithSetupProc(s, Flgm, depends);
	} else {
		++FileCounter;
	}
}

LOCALFUNC tMyErr DoAllSrcFilesSortWithSetup(MyProc p)
{
	DoSrcFile_r r;

	r.SavepDt = pDt;
	r.p = p;
	r.err = noErr;
	pDt = (MyPtr)&r;

	FileCounter = 0;
	DoAllSrcFiles(DoAllSrcFilesSortWithSetup1);
	FileCounter = 0;
	DoAllSrcFiles(DoAllSrcFilesSortWithSetup2);

	pDt = r.SavepDt;

	return r.err;
}

LOCALFUNC char * GetSrcFileFileXtns(void)
{
	char *s;
#if AsmSupported
	blnr IsAsmFile = HaveAsm
		&& ((DoSrcFile_gd()->Flgm & kCSrcFlgmAsmAvail) != 0);
#endif
	blnr UseAPI = ((DoSrcFile_gd()->Flgm & kCSrcFlgmUseAPI) != 0);

#if AsmSupported
	if (IsAsmFile) {
		s = ".S";
	} else
#endif
	{
		if (UseAPI && (gbk_apifam_cco == gbo_apifam)) {
			s = ".m";
		} else {
			s = ".c";
		}
	}

	return s;
}

LOCALPROC WriteSrcFileFileName(void)
{
	WriteCStrToDestFile(DoSrcFile_gd()->s);
	WriteCStrToDestFile(GetSrcFileFileXtns());
}

LOCALPROC WriteSrcFileFilePath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile,
		WriteSrcFileFileName);
}

LOCALPROC WriteSrcFileHeaderName(void)
{
	WriteCStrToDestFile(DoSrcFile_gd()->s);
	WriteCStrToDestFile(".h");
}

LOCALPROC WriteSrcFileHeaderPath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile,
		WriteSrcFileHeaderName);
}

LOCALPROC WriteSrcFileObjName(void)
{
	WriteCStrToDestFile(DoSrcFile_gd()->s);
	switch (cur_ide) {
		case gbk_ide_msv:
		case gbk_ide_dmc:
		case gbk_ide_plc:
			WriteCStrToDestFile(".obj");
			break;
		default:
			WriteCStrToDestFile(".o");
			break;
	}
}

LOCALPROC WriteSrcFileObjPath(void)
{
	WriteFileInDirToDestFile0(Write_obj_d_ToDestFile,
		WriteSrcFileObjName);
}

struct DoXtraHdr_r
{
	MyPtr SavepDt;
	int DepDir;
	char *s;
	tMyErr err;
	MyProc p;
};

typedef struct DoXtraHdr_r DoXtraHdr_r;

#define DoXtraHdr_gd() ((DoXtraHdr_r *)(pDt))

LOCALPROC DoAllExtraHeaders2WithSetupProc(int DepDir,
	char *s)
{
	if (noErr == DoXtraHdr_gd()->err) {
		DoXtraHdr_gd()->DepDir = DepDir;
		DoXtraHdr_gd()->s = s;

		DoXtraHdr_gd()->p();

		++FileCounter;
	}
}

LOCALFUNC tMyErr DoAllExtraHeaders2WithSetup(MyProc p)
{
	DoXtraHdr_r r;

	r.SavepDt = pDt;
	r.p = p;
	r.err = noErr;
	pDt = (MyPtr)&r;

	FileCounter = 0;
	DoAllExtraHeaders(DoAllExtraHeaders2WithSetupProc);

	pDt = r.SavepDt;

	return r.err;
}

LOCALPROC WriteExtraHeaderFileName(void)
{
	WriteCStrToDestFile(DoXtraHdr_gd()->s);
}

LOCALPROC WriteExtraHeaderFilePath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile,
		WriteExtraHeaderFileName);
}

LOCALVAR unsigned int DocTypeCounter;

struct DoDocType_r
{
	MyPtr SavepDt;
	char *ShortName;
	char *MacType;
	char *LongName;
	tWriteExtensionList WriteExtensionList;
	tMyErr err;
	MyProc p;
};

typedef struct DoDocType_r DoDocType_r;

#define DoDocType_gd() ((DoDocType_r *)(pDt))

LOCALPROC DoAllDocTypesWithSetupProc(char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	if (noErr == DoDocType_gd()->err) {
		DoDocType_gd()->ShortName = ShortName;
		DoDocType_gd()->MacType = MacType;
		DoDocType_gd()->LongName = LongName;
		DoDocType_gd()->WriteExtensionList = WriteExtensionList;

		DoDocType_gd()->p();

		++DocTypeCounter;
	}
}

LOCALPROC DoAppAndAllDocTypes(tWriteOneDocType p)
{
	p("App", "APPL", "Application", NULL);
	if (WantIconMaster) {
		DoAllDocTypes(p);
	}
}

LOCALFUNC tMyErr DoAllDocTypesWithSetup(MyProc p)
{
	DoDocType_r r;

	r.SavepDt = pDt;
	r.p = p;
	r.err = noErr;
	pDt = (MyPtr)&r;

	DocTypeCounter = 0;
	DoAppAndAllDocTypes(DoAllDocTypesWithSetupProc);

	pDt = r.SavepDt;

	return r.err;
}

LOCALPROC WriteDocTypeIconShortName(void)
{
	WriteCStrToDestFile(DoDocType_gd()->ShortName);
}

LOCALPROC WriteDocTypeIconFileName(void)
{
	WriteDocTypeIconShortName();
	WriteCStrToDestFile("Icon.");
	switch (gbo_targfam) {
		case gbk_targfam_mach:
		case gbk_targfam_carb:
			WriteCStrToDestFile("icns");
			break;
		case gbk_targfam_mswn:
		case gbk_targfam_wnce:
			WriteCStrToDestFile("ico");
			break;
	}
}

LOCALPROC WriteDocTypeIconFilePath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile,
		WriteDocTypeIconFileName);
}

LOCALPROC WriteDocTypeIconMacType(void)
{
	WriteCStrToDestFile(DoDocType_gd()->MacType);
}

LOCALPROC WriteDocTypeCopyMachoFile(void)
{
	WriteCopyFile(WriteDocTypeIconFilePath,
		Write_tmachores_d_ToDestFile);
}

typedef void (*tWriteOneFrameWorkType)(char *s);

static void DoAllFrameWorks(tWriteOneFrameWorkType p)
{
	if (gbk_apifam_cco == gbo_apifam) {
		p("AppKit");
		p("AudioUnit");
#if UseOpenGLinOSX
		p("OpenGL");
#endif
	} else {
		p("Carbon");
#if UseOpenGLinOSX
		p("OpenGL");
		p("AGL");
#endif
	}
}

struct DoFrameWork_r
{
	MyPtr SavepDt;
	char *s;
	MyProc p;
};

typedef struct DoFrameWork_r DoFrameWork_r;

#define DoFrameWork_gd() ((DoFrameWork_r *)(pDt))

LOCALPROC DoAllFrameWorksWithSetupProc(char *s)
{
	DoFrameWork_gd()->s = s;

	DoFrameWork_gd()->p();

	++FileCounter;
}

LOCALPROC DoAllFrameWorksWithSetup(MyProc p)
{
	DoFrameWork_r r;

	r.SavepDt = pDt;
	r.p = p;
	pDt = (MyPtr)&r;

	FileCounter = 0;
	DoAllFrameWorks(DoAllFrameWorksWithSetupProc);

	pDt = r.SavepDt;
}

LOCALPROC WriteFileToCFilesList(MyProc p)
{
	WriteBgnDestFileLn();
	p();
	WriteEndDestFileLn();
}

LOCALPROC DoSrcExtraHeaderFile(void)
{
	WriteFileToCFilesList(WriteExtraHeaderFilePath);
}

LOCALPROC DoSrcFileAddToList(void)
{
	WriteFileToCFilesList(WriteSrcFileHeaderPath);
	WriteFileToCFilesList(WriteSrcFileFilePath);
}

LOCALPROC WriteCFilesListContents(void)
{
	vCheckWriteDestErr(
		DoAllExtraHeaders2WithSetup(DoSrcExtraHeaderFile));
	vCheckWriteDestErr(DoAllSrcFilesWithSetup(DoSrcFileAddToList));
}

LOCALFUNC tMyErr WriteCFilesList(void)
{
	/* list of c files */

	return WriteADestFile(&OutputDirR,
		"c_files", "", WriteCFilesListContents);
}

LOCALPROC Write_tmachoShell(void)
{
	WriteRmDir(WriteAppNamePath);
	WriteRmDir(Write_tmachobun_d_ToDestFile);
	WriteMkDir(Write_tmachobun_d_ToDestFile);
	WriteMkDir(Write_tmachocontents_d_ToDestFile);
	WriteMkDir(Write_tmachomac_d_ToDestFile);
	WriteMkDir(Write_tmachores_d_ToDestFile);
	WriteMkDir(Write_tmacholang_d_ToDestFile);
	vCheckWriteDestErr(
		DoAllDocTypesWithSetup(WriteDocTypeCopyMachoFile));
	WriteCopyFile(WriteInfoPlistFilePath,
		Write_tmachocontents_d_ToDestFile);
	WriteEchoToNewFile(Write_tmachoLangDummyContents,
		Write_tmachoLangDummyPath, trueblnr);
	WriteEchoToNewFile(Write_tmachoPkgInfoContents,
		Write_tmachoPkgInfoPath, falseblnr);
	WriteMoveDir(Write_tmachobun_d_ToDestFile, WriteAppNamePath);
}

LOCALPROC Write_tmachoShellDeps(void)
{
	WriteMakeDependFile(Write_srcAppIconPath);
}

LOCALPROC WritepDtSrcPath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile, WritepDtString);
}

LOCALPROC DoSrcDependsMakeCompile(int DepDir, char *s)
{
#pragma unused(DepDir)
	MyPtr SavepDt = pDt;
	pDt = (MyPtr)s;
	WriteMakeDependFile(WritepDtSrcPath);
	pDt = SavepDt;
}

LOCALPROC DoSrcFileMakeCompileDeps(void)
{
#if AsmSupported
	blnr IsAsmFile = HaveAsm
		&& ((DoSrcFile_gd()->Flgm & kCSrcFlgmAsmAvail) != 0);
#endif

	WriteMakeDependFile(WriteSrcFileFilePath);
	if (DoSrcFile_gd()->depends != nullpr) {
		DoSrcFile_gd()->depends(DoSrcDependsMakeCompile);
	}
#if AsmSupported
	if (! IsAsmFile)
#endif
	{
		WriteMakeDependFile(WriteCNFGGLOBPath);
	}
}

LOCALPROC DoSrcFileMakeCompileBody(void)
{
#if AsmSupported
	blnr IsAsmFile = HaveAsm
		&& ((DoSrcFile_gd()->Flgm & kCSrcFlgmAsmAvail) != 0);

	if (IsAsmFile) {
		WriteCompileAsm(WriteSrcFileFilePath, WriteSrcFileObjPath);
	} else
#endif
	{
		WriteCompileC(WriteSrcFileFilePath, WriteSrcFileObjPath,
			(DoSrcFile_gd()->Flgm & kCSrcFlgmUseAPI) != 0);
	}
}

LOCALPROC DoSrcFileMakeCompile(void)
{
	WriteMakeRule(WriteSrcFileObjPath,
		DoSrcFileMakeCompileDeps,
		DoSrcFileMakeCompileBody);
}

LOCALPROC DoSrcFileStandardMakeObjects(void)
{
	WriteBgnDestFileLn();
	WriteSrcFileObjPath();
	strmo_writeSpace();
	WriteBackSlashToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC DoAllSrcFilesStandardMakeObjects(void)
{
	vCheckWriteDestErr(
		DoAllSrcFilesSortWithSetup(DoSrcFileStandardMakeObjects));
}

LOCALPROC DoSrcFileStandardEraseFile(void)
{
	WriteRmFile(WriteSrcFileObjPath);
}

LOCALPROC DoAllSrcFilesStandardErase(void)
{
	vCheckWriteDestErr(
		DoAllSrcFilesWithSetup(DoSrcFileStandardEraseFile));
}
