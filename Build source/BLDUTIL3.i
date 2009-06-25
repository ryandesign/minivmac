/*
	BLDUTIL3.i
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
	BuiLD system UTILities part 3
*/


#define src_d_name "src"

#define obj_d_name "bld"
	/* not "obj", so as to work in freebsd make */

LOCALVAR MyDir_R BaseDirR;
LOCALVAR MyDir_R OutputDir0R;
LOCALVAR MyDir_R OutputDirR;
LOCALVAR MyDir_R ProjDirR;
LOCALVAR MyDir_R ObjDirR;
LOCALVAR MyDir_R SrcDirR;

LOCALVAR blnr HaveAsm;
LOCALVAR blnr HaveLanguage;
LOCALVAR blnr HavePlatform;
LOCALVAR blnr HaveAltSrc;

LOCALPROC WritepDtString(void)
{
	WriteCStrToDestFile((char *)pDt);
}

LOCALPROC WriteMaintainerName(void)
{
	WriteHandToDestFile(hMaintainerName);
}

LOCALPROC WriteHomePage(void)
{
	WriteHandToDestFile(hHomePage);
}

/* end of default value of options */

LOCALPROC WriteVersionStr(void)
{
	WriteCStrToDestFile(kMajorVersion);
	WriteCStrToDestFile(".");
	WriteCStrToDestFile(kMinorVersion);
	WriteCStrToDestFile(".");
	WriteCStrToDestFile(kMinorSubVersion);
}

LOCALPROC WriteAppVariationStr(void)
{
	WriteHandToDestFile(hVariationName);
}

LOCALPROC WriteAppCopyrightYearStr(void)
{
	WriteCStrToDestFile(kStrCopyrightYear);
}

LOCALPROC WriteGetInfoString(void)
{
	WriteAppVariationStr();
	WriteCStrToDestFile(", Copyright ");
	WriteAppCopyrightYearStr();
	WriteCStrToDestFile(" maintained by ");
	WriteMaintainerName();
	WriteCStrToDestFile(".");
}

LOCALPROC WriteLProjName(void)
{
	WriteCStrToDestFile(GetLProjName(gbo_lang));
}

LOCALFUNC char * GetPlatformDirName(void)
{
	char *s;

	switch (gbo_apifam) {
		case gbk_apifam_mac:
			s = "mac";
			break;
		case gbk_apifam_osx:
#if OSXplatsep
			s = "osx";
#else
			s = "mac";
#endif
			break;
		case gbk_apifam_win:
			s = "win";
			break;
		case gbk_apifam_xwn:
			s = "xwn";
			break;
		case gbk_apifam_gtk:
			s = "gtk";
			break;
		default:
			s = "gen";
			break;
	}
	return s;
}

/* --- XML utilities --- */

TYPEDEFPROC (*MyProc)(void);

LOCALPROC WriteXMLtagBegin(char *s)
{
	WriteCStrToDestFile("<");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(">");
}

LOCALPROC WriteXMLtagEnd(char *s)
{
	WriteCStrToDestFile("</");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(">");
}

LOCALPROC WriteBeginXMLtagLine(char *s)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin(s);
	WriteEndDestFileLn();
	++DestFileIndent;
}

LOCALPROC WriteEndXMLtagLine(char *s)
{
	--DestFileIndent;
	WriteBgnDestFileLn();
	WriteXMLtagEnd(s);
	WriteEndDestFileLn();
}

LOCALPROC WriteXMLtagBeginValEndLine(char *t, char *v)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin(t);
	WriteCStrToDestFile(v);
	WriteXMLtagEnd(t);
	WriteEndDestFileLn();
}

LOCALPROC WriteXMLtagBeginProcValEndLine(char *t, MyProc v)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin(t);
	v();
	WriteXMLtagEnd(t);
	WriteEndDestFileLn();
}

/* --- end XML utilities --- */

/* --- c preprocessor utilities --- */

LOCALPROC WriteCompCondBool(char *s, blnr v)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define ");
	WriteCStrToDestFile(s);
	if (v) {
		WriteCStrToDestFile(" 1");
	} else {
		WriteCStrToDestFile(" 0");
	}
	WriteEndDestFileLn();
}

LOCALPROC WriteCDefQuote(char *s, MyProc p)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define ");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(" ");
	WriteQuoteToDestFile();
	p();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

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

/* --- end c preprocessor utilities --- */

LOCALPROC WritePathInDirToDestFile0(MyProc p, MyProc ps,
	blnr isdir)
{
	switch (cur_ide) {
		case gbk_ide_mpw:
		case gbk_ide_mw8:
			if (p != NULL) {
				p();
			} else {
				WriteCStrToDestFile(":");
			}
			ps();
			if (isdir) {
				WriteCStrToDestFile(":");
			}
			break;
		case gbk_ide_bgc:
		case gbk_ide_xcd:
		case gbk_ide_snc:
			if (p != NULL) {
				p();
			}
			ps();
			if (isdir) {
				WriteCStrToDestFile("/");
			}
			break;
		case gbk_ide_msv:
		case gbk_ide_dvc:
		case gbk_ide_lcc:
			if (p != NULL) {
				p();
				WriteBackSlashToDestFile();
			} else {
				if (gbk_ide_lcc == cur_ide) {
					if (! UseCmndLine) {
						WriteCStrToDestFile("c:\\output\\");
					}
				} else if ((gbk_ide_msv == cur_ide) && (ide_vers >= 8000)) {
					WriteCStrToDestFile(".\\");
				}
			}
			ps();
			break;
	}
}

LOCALPROC WriteFileInDirToDestFile0(MyProc p, MyProc ps)
{
	WritePathInDirToDestFile0(p, ps, falseblnr);
}

LOCALPROC WriteSubDirToDestFile(MyProc p, MyProc ps)
{
	WritePathInDirToDestFile0(p, ps, trueblnr);
}

LOCALPROC Write_toplevel_f_ToDestFile(MyProc ps)
{
	WritePathInDirToDestFile0(NULL, ps, falseblnr);
}

LOCALPROC Write_toplevel_d_ToDestFile(MyProc ps)
{
	WritePathInDirToDestFile0(NULL, ps, trueblnr);
}

LOCALPROC Write_src_d_Name(void)
{
	WriteCStrToDestFile(src_d_name);
}

LOCALPROC Write_src_d_ToDestFile(void)
{
	Write_toplevel_d_ToDestFile(Write_src_d_Name);
}

LOCALPROC Write_obj_d_Name(void)
{
	WriteCStrToDestFile(obj_d_name);
}

LOCALPROC Write_obj_d_ToDestFile(void)
{
	Write_toplevel_d_ToDestFile(Write_obj_d_Name);
}

LOCALPROC WriteLProjFolderName(void)
{
	WriteLProjName();
	WriteCStrToDestFile(".lproj");
}

LOCALPROC WriteLProjFolderPath(void)
{
	WriteSubDirToDestFile(Write_src_d_ToDestFile,
		WriteLProjFolderName);
}

LOCALPROC WriteStrAppAbbrev(void)
{
	WriteCStrToDestFile(vStrAppAbbrev);
}

LOCALPROC WriteAppNameStr(void)
{
	WriteStrAppAbbrev();
	switch (gbo_apifam) {
		case gbk_apifam_osx:
			if (HaveMacBundleApp) {
				WriteCStrToDestFile(".app");
			}
			break;
		case gbk_apifam_win:
			WriteCStrToDestFile(".exe");
			break;
		default:
			break;
	}
}

LOCALPROC WriteAppNamePath(void)
{
	if (HaveMacBundleApp) {
		Write_toplevel_d_ToDestFile(WriteAppNameStr);
	} else {
		Write_toplevel_f_ToDestFile(WriteAppNameStr);
	}
}

LOCALPROC WriteStrAppUnabrevName(void)
{
	WriteCStrToDestFile(kStrAppName);
}

LOCALPROC Write_contents_d_Name(void)
{
	WriteCStrToDestFile("Contents");
}

LOCALPROC Write_machocontents_d_ToDestFile(void)
{
	WriteSubDirToDestFile(WriteAppNamePath,
		Write_contents_d_Name);
}

LOCALPROC Write_macos_d_Name(void)
{
	WriteCStrToDestFile("MacOS");
}

LOCALPROC Write_machomac_d_ToDestFile(void)
{
	WriteSubDirToDestFile(Write_machocontents_d_ToDestFile,
		Write_macos_d_Name);
}

LOCALPROC Write_resources_d_Name(void)
{
	WriteCStrToDestFile("Resources");
}

LOCALPROC Write_machores_d_ToDestFile(void)
{
	WriteSubDirToDestFile(Write_machocontents_d_ToDestFile,
		Write_resources_d_Name);
}

LOCALPROC Write_machobinpath_ToDestFile(void)
{
	if (HaveMacBundleApp) {
		WriteFileInDirToDestFile0(Write_machomac_d_ToDestFile, WriteStrAppAbbrev);
	} else {
		WriteAppNamePath();
	}
}

LOCALPROC Write_tmachobun_d_Name(void)
{
	WriteCStrToDestFile("AppTemp");
}

LOCALPROC Write_tmachobun_d_ToDestFile(void)
{
	Write_toplevel_d_ToDestFile(Write_tmachobun_d_Name);
}

LOCALPROC Write_tmachocontents_d_ToDestFile(void)
{
	WriteSubDirToDestFile(Write_tmachobun_d_ToDestFile,
		Write_contents_d_Name);
}

LOCALPROC Write_tmachomac_d_ToDestFile(void)
{
	WriteSubDirToDestFile(Write_tmachocontents_d_ToDestFile,
		Write_macos_d_Name);
}

LOCALPROC Write_tmachores_d_ToDestFile(void)
{
	WriteSubDirToDestFile(Write_tmachocontents_d_ToDestFile,
		Write_resources_d_Name);
}

LOCALPROC Write_tmacholang_d_ToDestFile(void)
{
	WriteSubDirToDestFile(Write_tmachores_d_ToDestFile,
		WriteLProjFolderName);
}

LOCALPROC Write_tmachobinpath_ToDestFile(void)
{
	WriteFileInDirToDestFile0(Write_tmachomac_d_ToDestFile, WriteStrAppAbbrev);
}

LOCALPROC Write_umachobun_d_Name(void)
{
	WriteStrAppAbbrev();
	WriteCStrToDestFile("_u.app");
}

LOCALPROC Write_umachobun_d_ToDestFile(void)
{
	Write_toplevel_d_ToDestFile(Write_umachobun_d_Name);
}

LOCALPROC Write_umachocontents_d_ToDestFile(void)
{
	WriteSubDirToDestFile(Write_umachobun_d_ToDestFile,
		Write_contents_d_Name);
}

LOCALPROC Write_umachomac_d_ToDestFile(void)
{
	WriteSubDirToDestFile(Write_umachocontents_d_ToDestFile,
		Write_macos_d_Name);
}

LOCALPROC Write_umachobinpath_ToDestFile(void)
{
	WriteFileInDirToDestFile0(Write_umachomac_d_ToDestFile, WriteStrAppAbbrev);
}

LOCALPROC WriteInfoPlistFileName(void)
{
	WriteCStrToDestFile("Info.plist");
}

LOCALPROC WriteInfoPlistFilePath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile, WriteInfoPlistFileName);
}

LOCALPROC WriteDummyLangFileName(void)
{
	WriteCStrToDestFile("dummy.txt");
}

LOCALPROC Write_tmachoLangDummyPath(void)
{
	WriteFileInDirToDestFile0(Write_tmacholang_d_ToDestFile, WriteDummyLangFileName);
}

LOCALPROC Write_tmachoLangDummyContents(void)
{
	WriteCStrToDestFile("dummy");
}

LOCALPROC Write_tmachoPkgInfoName(void)
{
	WriteCStrToDestFile("PkgInfo");
}

LOCALPROC Write_tmachoPkgInfoPath(void)
{
	WriteFileInDirToDestFile0(Write_tmachocontents_d_ToDestFile, Write_tmachoPkgInfoName);
}

LOCALPROC Write_MacCreatorSigOrGeneric(void)
{
	if (WantIconMaster) {
		WriteCStrToDestFile(kMacCreatorSig);
	} else {
		WriteCStrToDestFile("????");
	}
}

LOCALPROC Write_tmachoPkgInfoContents(void)
{
	WriteCStrToDestFile("APPL");
	Write_MacCreatorSigOrGeneric();
}

LOCALPROC Write_machoRsrcName(void)
{
	WriteStrAppAbbrev();
	WriteCStrToDestFile(".rsrc");
}

LOCALPROC Write_machoRsrcPath(void)
{
	WriteFileInDirToDestFile0(Write_machores_d_ToDestFile, Write_machoRsrcName);
}

LOCALPROC Write_AppIconName(void)
{
	WriteCStrToDestFile("AppIcon.icns");
}

LOCALPROC Write_machoAppIconPath(void)
{
	WriteFileInDirToDestFile0(Write_machores_d_ToDestFile, Write_AppIconName);
}

LOCALPROC Write_srcAppIconPath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile, Write_AppIconName);
}

LOCALPROC WriteMainRsrcName(void)
{
	switch (cur_ide) {
		case gbk_ide_msv:
		case gbk_ide_dvc:
		case gbk_ide_lcc:
			WriteCStrToDestFile("main.rc");
			break;
		default:
			WriteCStrToDestFile("main.r");
			break;
	}
}

LOCALPROC WriteMainRsrcSrcPath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile, WriteMainRsrcName);
}

LOCALPROC WriteMainRsrcObjName(void)
{
	switch (cur_ide) {
		case gbk_ide_msv:
		case gbk_ide_lcc:
			WriteCStrToDestFile("main.res");
			break;
		case gbk_ide_mpw:
			WriteCStrToDestFile("main.rsrc");
			break;
	}
}

LOCALPROC WriteMainRsrcObjPath(void)
{
	WriteFileInDirToDestFile0(Write_obj_d_ToDestFile, WriteMainRsrcObjName);
}

LOCALPROC WriteCNFGGLOBName(void)
{
	WriteCStrToDestFile("CNFGGLOB.h");
}

LOCALPROC WriteCNFGGLOBPath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile, WriteCNFGGLOBName);
}

LOCALPROC WriteCNFGRAPIName(void)
{
	WriteCStrToDestFile("CNFGRAPI.h");
}

LOCALPROC WriteCNFGRAPIPath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile, WriteCNFGRAPIName);
}

LOCALPROC WritePathArgInMakeCmnd(MyProc p)
{
	switch (cur_ide) {
		case gbk_ide_mpw:
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
		case gbk_ide_msv:
			WriteCStrToDestFile(" \"");
			p();
			WriteCStrToDestFile("\"");
			break;
		case gbk_ide_lcc:
			WriteCStrToDestFile(" ");
			/* saw some glitches with quotes */
			p();
			break;
		default:
			break;
	}
}

LOCALPROC WriteMakeVar(char *s)
{
	switch (cur_ide) {
		case gbk_ide_mpw:
			WriteCStrToDestFile("{");
			WriteCStrToDestFile(s);
			WriteCStrToDestFile("}");
			break;
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
		case gbk_ide_msv:
		case gbk_ide_lcc:
			WriteCStrToDestFile("$(");
			WriteCStrToDestFile(s);
			WriteCStrToDestFile(")");
			break;
		default:
			break;
	}
}

LOCALPROC WriteMakeDependFile(MyProc p)
{
	switch (cur_ide) {
		case gbk_ide_msv:
		case gbk_ide_mpw:
			WriteCStrToDestFile(" ");
			WriteQuoteToDestFile();
			p();
			WriteQuoteToDestFile();
			break;
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
		case gbk_ide_lcc:
			WriteCStrToDestFile(" ");
			p();
			break;
		default:
			break;
	}
}

LOCALPROC WriteMakeRule(MyProc ptarg,
	MyProc pdeps, MyProc pbody)
{
	WriteBgnDestFileLn();
	switch (cur_ide) {
		case gbk_ide_mpw:
			WriteQuoteToDestFile();
			ptarg();
			WriteQuoteToDestFile();
			WriteCStrToDestFile(" \304");
			pdeps();
			break;
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
			ptarg();
			WriteCStrToDestFile(" :");
			pdeps();
			break;
		case gbk_ide_msv:
			WriteQuoteToDestFile();
			ptarg();
			WriteQuoteToDestFile();
			WriteCStrToDestFile(" :");
			pdeps();
			break;
		case gbk_ide_lcc:
			ptarg();
			WriteCStrToDestFile(":");
			pdeps();
			break;
		default:
			break;
	}
	WriteEndDestFileLn();
	++DestFileIndent;
		pbody();
	--DestFileIndent;
}

LOCALPROC WriteMkDir(MyProc p)
{
	WriteBgnDestFileLn();
	switch (cur_ide) {
		case gbk_ide_mpw:
			WriteCStrToDestFile("NewFolder");
			break;
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
			WriteCStrToDestFile("mkdir");
			break;
		default:
			break;
	}
	WritePathArgInMakeCmnd(p);
	WriteEndDestFileLn();
}

LOCALPROC WriteRmDir(MyProc p)
{
	WriteBgnDestFileLn();
	switch (cur_ide) {
		case gbk_ide_mpw:
			WriteCStrToDestFile("Delete -i -y");
			break;
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
			WriteCStrToDestFile("rm -fr");
			break;
		default:
			break;
	}
	WritePathArgInMakeCmnd(p);
	WriteEndDestFileLn();
}

LOCALPROC WriteRmFile(MyProc p)
{
	WriteBgnDestFileLn();
	switch (cur_ide) {
		case gbk_ide_mpw:
			WriteCStrToDestFile("Delete -i");
			break;
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
			WriteCStrToDestFile("rm -f");
			break;
		case gbk_ide_msv:
			WriteCStrToDestFile("-@erase");
			break;
		case gbk_ide_lcc:
			WriteCStrToDestFile("del");
			break;
		default:
			break;
	}
	WritePathArgInMakeCmnd(p);
	WriteEndDestFileLn();
}

LOCALPROC WriteCopyFile(MyProc pfrom, MyProc pto)
{
	WriteBgnDestFileLn();
	switch (cur_ide) {
		case gbk_ide_mpw:
			WriteCStrToDestFile("Duplicate");
			break;
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
			WriteCStrToDestFile("cp");
			break;
		default:
			break;
	}
	WritePathArgInMakeCmnd(pfrom);
	WritePathArgInMakeCmnd(pto);
	WriteEndDestFileLn();
}

LOCALPROC WriteMoveDir(MyProc pfrom, MyProc pto)
{
	WriteBgnDestFileLn();
	switch (cur_ide) {
		case gbk_ide_mpw:
			WriteCStrToDestFile("Move");
			break;
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
			WriteCStrToDestFile("mv");
			break;
		default:
			break;
	}
	WritePathArgInMakeCmnd(pfrom);
	WritePathArgInMakeCmnd(pto);
	WriteEndDestFileLn();
}

LOCALPROC WriteEchoToNewFile(MyProc ptext, MyProc pto, blnr newline)
{
	WriteBgnDestFileLn();
	switch (cur_ide) {
		case gbk_ide_mpw:
			WriteCStrToDestFile("Echo");
			if (! newline) {
				WriteCStrToDestFile(" -n");
			}
			WriteCStrToDestFile(" \"");
			ptext();
			WriteCStrToDestFile("\" >");
			WritePathArgInMakeCmnd(pto);
			break;
			break;
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
			WriteCStrToDestFile("echo");
			if (! newline) {
				WriteCStrToDestFile(" -n");
			}
			WriteCStrToDestFile(" \"");
			ptext();
			WriteCStrToDestFile("\" >");
			WritePathArgInMakeCmnd(pto);
			break;
		default:
			break;
	}
	WriteEndDestFileLn();
}

LOCALPROC WriteCompileCExec(void)
{
	switch (cur_ide) {
		case gbk_ide_mpw:
			if (gbk_cpufam_68k == gbo_cpufam) {
				WriteCStrToDestFile("SC");
			} else if (gbk_cpufam_ppc == gbo_cpufam) {
				WriteCStrToDestFile("MrC");
			}
			break;
		case gbk_ide_bgc:
		case gbk_ide_xcd:
			WriteCStrToDestFile("gcc");
			break;
		case gbk_ide_snc:
			WriteCStrToDestFile("cc");
			break;
		case gbk_ide_msv:
			if (gbk_cpufam_arm == gbo_cpufam) {
				WriteCStrToDestFile("clarm.exe");
			} else {
				WriteCStrToDestFile("cl.exe");
			}
			break;
		case gbk_ide_lcc:
			WriteCStrToDestFile("lcc.exe");
			break;
		default:
			break;
	}
}

LOCALPROC WriteCompileC(MyProc psrc, MyProc pobj, blnr UseAPI)
{
	WriteBgnDestFileLn();
	switch (cur_ide) {
		case gbk_ide_mpw:
		case gbk_ide_bgc:
		case gbk_ide_xcd:
		case gbk_ide_snc:
			WriteCompileCExec();
			WritePathArgInMakeCmnd(psrc);
			WriteCStrToDestFile(" -o");
			WritePathArgInMakeCmnd(pobj);
			WriteCStrToDestFile(" ");
			WriteMakeVar("mk_COptions");
			if (UseAPI) {
				if (gbk_apifam_gtk == gbo_apifam) {
					WriteCStrToDestFile(" `pkg-config --cflags gtk+-2.0`");
				}
			}
			break;
		case gbk_ide_msv:
			WriteCompileCExec();
			WritePathArgInMakeCmnd(psrc);
			WriteCStrToDestFile(" ");
			WriteMakeVar("mk_COptions");
			break;
		case gbk_ide_lcc:
			WriteCompileCExec();
			WritePathArgInMakeCmnd(psrc);
			WriteCStrToDestFile(" -Fo");
			pobj();
			WriteCStrToDestFile(" ");
			WriteMakeVar("mk_COptions");
			break;
		default:
			break;
	}
	WriteEndDestFileLn();
}

LOCALPROC WriteCompileAsm(MyProc psrc, MyProc obj)
{
	WriteBgnDestFileLn();
	switch (cur_ide) {
		case gbk_ide_mpw:
			if (gbo_cpufam == gbk_cpufam_68k) {
				WriteCStrToDestFile("Asm");
			} else if (gbo_cpufam == gbk_cpufam_ppc) {
				WriteCStrToDestFile("PPCAsm");
			}
			break;
		case gbk_ide_bgc:
		case gbk_ide_snc:
		case gbk_ide_xcd:
			WriteCStrToDestFile("gcc");
			break;
		default:
			break;
	}
	WritePathArgInMakeCmnd(psrc);
	WriteCStrToDestFile(" -o");
	WritePathArgInMakeCmnd(obj);
	WriteCStrToDestFile(" ");
	WriteMakeVar("mk_AOptions");
	WriteEndDestFileLn();
}
