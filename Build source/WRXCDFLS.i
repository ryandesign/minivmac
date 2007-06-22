/*
	WRXCDFLS.i
	Copyright (C) 2007 Paul Pratt

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
	WRite XCoDe specific FiLes

	(also support older Apple Project Builder)
*/

#pragma segment XCodeSupport

LOCALPROC WriteNextLineSameDent(void)
{
	WriteEndDestFileLn();
	WriteBgnDestFileLn();
}

LOCALPROC WriteNextLineInDent(void)
{
	WriteEndDestFileLn();
	++DestFileIndent;
		WriteBgnDestFileLn();
}

LOCALPROC WriteNextLineOutDent(void)
{
		WriteEndDestFileLn();
	--DestFileIndent;
	WriteBgnDestFileLn();
}

static void WriteAPBXCDObjectId(unsigned int theClass, unsigned int v)
{
	WriteHexWordToOutput(theClass);
	WriteHexWordToOutput(v);
	WriteCStrToDestFile("0000000000000000");
}

enum {
	APBoclsSrcBld,
	APBoclsIcnsBld,
	APBoclsFramBld,
	APBoclsSrcRf,
	APBoclsHdr, /* only if cur_ide == gbk_ide_xcd */
	APBoclsInc, /* only if cur_ide == gbk_ide_xcd */
	APBoclsIcnsRf,
	APBoclsFramRf,
	APBoclsLibs,
	APBoclsGen,
	kNumAPBocls
};

enum {
	APBgenoProductRef,
	APBgenoBuildStyle,
	APBgenoPlist,
	APBgenoLangDummyBld,
	APBgenoLangRf,
	APBgenoMnRsrcBld, /* only if HaveMacRrscs */
	APBgenoMainRsrcRf, /* only if HaveMacRrscs */

	APBgenoPhaseLibs,
	APBgenoSources,
	APBgenoResources, /* only if HaveMacBundleApp */
	APBgenoLibraries,
	APBgenoProducts,
	APBgenoMainGroup,
	APBgenoSrcHeaders, /* only if cur_ide == gbk_ide_xcd */
	APBgenoIncludes, /* only if cur_ide == gbk_ide_xcd */
	APBgenoTarget,
	APBgenoRoot,
	APBgenoBunRsrcs, /* only if HaveMacBundleApp */
	APBgenoPhaseRsrc, /* only if HaveMacRrscs */
	APBgenoHeaders,
	APBgenoPhaseSrcs,
	APBgenoLangDummyRf,
	APBgenoNatCnfg, /* only if cur_ide == gbk_ide_xcd */
	APBgenoPrjCnfg, /* only if cur_ide == gbk_ide_xcd */
	APBgenoLstNatCnfg, /* only if cur_ide == gbk_ide_xcd */
	APBgenoLstPrjCnfg, /* only if cur_ide == gbk_ide_xcd */

	kNumAPBgeno
};

enum {
	APBliboLibStdcRf,
	APBliboLibStdcBld,
	kNumAPBlibo
};

#define HaveAPBXCD_LangDummy (ide_vers >= 1500)
#define HaveAPBXCD_PlistFile (ide_vers >= 1500)
#define HaveAPBXCD_NameCmmnt (ide_vers >= 2000)
#define HaveAPBXCD_Headers (ide_vers >= 1500)
#define HaveAPBXCD_StdcLib (ide_vers < 1500)

static void WriteAPBXCDBgnObjList(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(" = (");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteAPBXCDEndObjList(void)
{
	--DestFileIndent;
	WriteDestFileLn(");");
}

LOCALPROC WriteAPBXCDObjectIdAndComment(unsigned int theClass, unsigned int v,
	MyProc comment)
{
	WriteAPBXCDObjectId(theClass, v);
	if (HaveAPBXCD_NameCmmnt) {
		WriteCStrToDestFile(" /* ");
		comment();
		WriteCStrToDestFile(" */");
	}
}

static void WriteAPBXCDBeginObject(unsigned int theClass, unsigned int v,
	MyProc comment)
{
	WriteBgnDestFileLn();
	WriteAPBXCDObjectIdAndComment(theClass, v, comment);
	WriteCStrToDestFile(" = {");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteAPBXCDEndObject(void)
{
	--DestFileIndent;
	WriteDestFileLn("};");
}

static void WriteAPBQuotedField(char *s, char *v)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(" = ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(v);
	WriteQuoteToDestFile();
	WriteCStrToDestFile(";");
	WriteEndDestFileLn();
}

LOCALPROC WriteAPBXCDobjlistelmp(unsigned int theClass, unsigned int v,
	MyProc comment)
{
	WriteBgnDestFileLn();
	WriteAPBXCDObjectIdAndComment(theClass, v, comment);
	WriteCStrToDestFile(",");
	WriteEndDestFileLn();
}

LOCALPROC WriteAPBXCDSepA(void)
{
	if (ide_vers < 2000) {
		WriteNextLineSameDent();
	} else {
		WriteCStrToDestFile(" ");
	}
}

LOCALPROC WriteAPBXCDObjectAp(unsigned int theClass, unsigned int v,
	MyProc comment, MyProc body)
{
	WriteBgnDestFileLn();
	WriteAPBXCDObjectIdAndComment(theClass, v, comment);
	WriteCStrToDestFile(" = {");
	if (ide_vers < 2000) {
		WriteNextLineInDent();
	}
	body();
	if (ide_vers < 2000) {
		WriteNextLineOutDent();
	} else {
		WriteCStrToDestFile(" ");
	}
	WriteCStrToDestFile("};");
	WriteEndDestFileLn();
}

LOCALPROC WriteSrcFileAPBXCDNameInSources(void)
{
	WriteSrcFileFileName();
	WriteCStrToDestFile(" in Sources");
}

LOCALPROC DoSrcFileAPBXCDaddFileBody(void)
{
	WriteCStrToDestFile("isa = PBXBuildFile;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileRef = ");
	WriteAPBXCDObjectIdAndComment(APBoclsSrcRf,
		FileCounter, WriteSrcFileFileName);
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("settings = {");
		WriteAPBXCDSepA();
		WriteCStrToDestFile("};");
	}
}

LOCALPROC DoSrcFileAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBoclsSrcBld, FileCounter,
		WriteSrcFileAPBXCDNameInSources, DoSrcFileAPBXCDaddFileBody);
}

LOCALPROC DoSrcFileAPBXCDaddFileRefBody(void)
{
	blnr IsAsmFile = HaveAsm && ((DoSrcFile_gd()->Flgm & kCSrcFlgmAsmAvail) != 0);

	WriteCStrToDestFile("isa = PBXFileReference;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileEncoding = 30;");
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		if (IsAsmFile) {
			WriteCStrToDestFile("lastKnownFileType = sourcecode.asm;");
		} else {
			WriteCStrToDestFile("lastKnownFileType = sourcecode.c.c;");
		}
	}
	WriteAPBXCDSepA();
	WriteCStrToDestFile("name = ");
	WriteSrcFileFileName();
	WriteCStrToDestFile(";");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("path = ");
	WriteSrcFileFilePath();
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("refType = 2;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("sourceTree = SOURCE_ROOT;");
	}
}

LOCALPROC DoSrcFileAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsSrcRf, FileCounter,
		WriteSrcFileFileName,
		DoSrcFileAPBXCDaddFileRefBody);
}

LOCALPROC DoSrcFileAPBXCDaddToGroup(void)
{
	WriteAPBXCDobjlistelmp(APBoclsSrcRf, FileCounter,
		WriteSrcFileFileName);
}

LOCALPROC DoSrcFileAPBXCDaddToSources(void)
{
	WriteAPBXCDobjlistelmp(APBoclsSrcBld, FileCounter,
		WriteSrcFileAPBXCDNameInSources);
}

LOCALPROC DoHeaderFileXCDaddFileRefBody(void)
{
	WriteCStrToDestFile("isa = PBXFileReference;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileEncoding = 30;");
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("lastKnownFileType = sourcecode.c.h;");
	}
	WriteAPBXCDSepA();
	WriteCStrToDestFile("name = ");
	WriteSrcFileHeaderName();
	WriteCStrToDestFile(";");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("path = ");
	WriteSrcFileHeaderPath();
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("refType = 2;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("sourceTree = SOURCE_ROOT;");
	}
}

LOCALPROC DoHeaderFileXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsHdr, FileCounter,
		WriteSrcFileHeaderName,
		DoHeaderFileXCDaddFileRefBody);
}

LOCALPROC DoHeaderFileXCDaddToGroup(void)
{
	WriteAPBXCDobjlistelmp(APBoclsHdr, FileCounter,
		WriteSrcFileHeaderName);
}

LOCALPROC WriteDocTypeAPBXCDIconFileInResources(void)
{
	WriteDocTypeIconFileName();
	WriteCStrToDestFile(" in Resources");
}

LOCALPROC DoDocTypeAPBXCDaddFileBody(void)
{
	WriteCStrToDestFile("isa = PBXBuildFile;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileRef = ");
	WriteAPBXCDObjectIdAndComment(APBoclsIcnsRf,
		DocTypeCounter, WriteDocTypeIconFileName);
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("settings = {");
		WriteAPBXCDSepA();
		WriteCStrToDestFile("};");
	}
}

LOCALPROC DoDocTypeAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBoclsIcnsBld, DocTypeCounter,
		WriteDocTypeAPBXCDIconFileInResources, DoDocTypeAPBXCDaddFileBody);
}

LOCALPROC DoDocTypeAPBXCDaddFileRefBody(void)
{
	WriteCStrToDestFile("isa = PBXFileReference;");
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("lastKnownFileType = image.icns;");
	}

	WriteAPBXCDSepA();
	WriteCStrToDestFile("name = ");
	WriteDocTypeIconFileName();
	WriteCStrToDestFile(";");

	WriteAPBXCDSepA();
	WriteCStrToDestFile("path = ");
	WriteDocTypeIconFilePath();
	WriteCStrToDestFile(";");

	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("refType = 2;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("sourceTree = SOURCE_ROOT;");
	}
}

LOCALPROC DoDocTypeAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsIcnsRf, DocTypeCounter,
		WriteDocTypeIconFileName, DoDocTypeAPBXCDaddFileRefBody);
}

LOCALPROC DoDocTypeAPBXCDaddToGroup(void)
{
	WriteAPBXCDobjlistelmp(APBoclsIcnsRf, DocTypeCounter,
		WriteDocTypeIconFileName);
}

LOCALPROC DoDocTypeAPBXCDaddToSources(void)
{
	WriteAPBXCDobjlistelmp(APBoclsIcnsBld, DocTypeCounter,
		WriteDocTypeAPBXCDIconFileInResources);
}

LOCALPROC WriteFrameWorkAPBXCDile(void)
{
	WriteCStrToDestFile(DoFrameWork_gd()->s);
	WriteCStrToDestFile(".framework");
}

LOCALPROC WriteFrameWorkAPBXCDileInFrameworks(void)
{
	WriteFrameWorkAPBXCDile();
	WriteCStrToDestFile(" in Frameworks");
}

LOCALPROC DoFrameWorkAPBXCDaddFileBody(void)
{
	WriteCStrToDestFile("isa = PBXBuildFile;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileRef = ");
	WriteAPBXCDObjectIdAndComment(APBoclsFramRf,
		FileCounter, WriteFrameWorkAPBXCDile);
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("settings = {");
		WriteAPBXCDSepA();
		WriteCStrToDestFile("};");
	}
}

LOCALPROC DoFrameWorkAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBoclsFramBld, FileCounter,
		WriteFrameWorkAPBXCDileInFrameworks,
		DoFrameWorkAPBXCDaddFileBody);
}

LOCALPROC DoFrameWorkAPBXCDaddFileRefBody(void)
{
	if (ide_vers < 1500) {
		WriteCStrToDestFile("isa = PBXFrameworkReference;");
	} else {
		WriteCStrToDestFile("isa = PBXFileReference;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("lastKnownFileType = wrapper.framework;");
	}
	WriteAPBXCDSepA();
	WriteCStrToDestFile("name = ");
	WriteFrameWorkAPBXCDile();
	WriteCStrToDestFile(";");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("path = /System/Library/Frameworks/");
	WriteFrameWorkAPBXCDile();
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("refType = 0;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("sourceTree = \"<absolute>\";");
	}
}

LOCALPROC DoFrameWorkAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsFramRf, FileCounter,
		WriteFrameWorkAPBXCDile,
		DoFrameWorkAPBXCDaddFileRefBody);
}

LOCALPROC DoFrameworkAPBXCDaddToBuild(void)
{
	WriteAPBXCDobjlistelmp(APBoclsFramBld, FileCounter,
		WriteFrameWorkAPBXCDileInFrameworks);
}

LOCALPROC DoFrameworkAPBXCDaddToLibraries(void)
{
	WriteAPBXCDobjlistelmp(APBoclsFramRf, FileCounter,
		WriteFrameWorkAPBXCDile);
}

LOCALPROC DoExtraHeaderFileXCDaddFileRefBody(void)
{
	WriteCStrToDestFile("isa = PBXFileReference;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileEncoding = 30;");
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("lastKnownFileType = sourcecode.c.h;");
	}
	WriteAPBXCDSepA();
	WriteCStrToDestFile("name = ");
	WriteExtraHeaderFileName();
	WriteCStrToDestFile(";");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("path = ");
	WriteExtraHeaderFilePath();
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("refType = 2;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("sourceTree = SOURCE_ROOT;");
	}
}

LOCALPROC DoExtraHeaderFileXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsInc, FileCounter,
		WriteExtraHeaderFileName,
		DoExtraHeaderFileXCDaddFileRefBody);
}

LOCALPROC DoExtraHeaderFileXCDaddToGroup(void)
{
	WriteAPBXCDobjlistelmp(APBoclsInc, FileCounter,
		WriteExtraHeaderFileName);
}

LOCALPROC WriteMainAPBXCDRsrcNameinRez(void)
{
	WriteMainRsrcName();
	WriteCStrToDestFile(" in Rez");
}

LOCALPROC DoRsrcAPBXCDaddFileBody(void)
{
	WriteCStrToDestFile("isa = PBXBuildFile;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileRef = ");
	WriteAPBXCDObjectIdAndComment(APBoclsGen,
		APBgenoMainRsrcRf, WriteMainRsrcName);
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("settings = {");
		WriteAPBXCDSepA();
		WriteCStrToDestFile("};");
	}
}

LOCALPROC DoRsrcAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBoclsGen, APBgenoMnRsrcBld,
		WriteMainAPBXCDRsrcNameinRez,
		DoRsrcAPBXCDaddFileBody);
}

LOCALPROC DoRsrcAPBXCDaddFileRefBody(void)
{
	WriteCStrToDestFile("isa = PBXFileReference;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileEncoding = 30;");
	if (ide_vers >= 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("lastKnownFileType = sourcecode.rez;");
	}
	WriteAPBXCDSepA();
	WriteCStrToDestFile("name = ");
	WriteMainRsrcName();
	WriteCStrToDestFile(";");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("path = ");
	WriteMainRsrcSrcPath();
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("refType = 2;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("sourceTree = SOURCE_ROOT;");
	}
}

LOCALPROC DoRsrcAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsGen, APBgenoMainRsrcRf,
		WriteMainRsrcName,
		DoRsrcAPBXCDaddFileRefBody);
}

LOCALPROC WriteLibStdcName(void)
{
	WriteCStrToDestFile("libstdc++.a");
}

LOCALPROC DoLibStdcAPBXCDaddFileBody(void)
{
	WriteCStrToDestFile("isa = PBXBuildFile;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileRef = ");
	WriteAPBXCDObjectIdAndComment(APBoclsLibs, APBliboLibStdcRf,
		WriteLibStdcName);
	WriteCStrToDestFile(";");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("settings = {");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("};");
}

LOCALPROC DoLibStdcAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBoclsLibs, APBliboLibStdcBld,
		WriteLibStdcName,
		DoLibStdcAPBXCDaddFileBody);
}

LOCALPROC DoLibStdcAPBXCDaddFileRefBody(void)
{
	WriteDestFileLn("isa = PBXFileReference;");
	if (ide_vers >= 1500) {
		WriteDestFileLn("lastKnownFileType = archive.ar;");
	}
	WriteAPBQuotedField("name", "libstdc++.a");
	WriteAPBQuotedField("path", "/usr/lib/libstdc++.a");
	if (ide_vers < 2000) {
		WriteDestFileLn("refType = 0;");
	}
	if (ide_vers >= 1500) {
		WriteDestFileLn("sourceTree = \"<absolute>\";");
	}
}

LOCALPROC DoLibStdcAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsLibs, APBliboLibStdcRf,
		WriteLibStdcName,
		DoLibStdcAPBXCDaddFileRefBody);
}

LOCALPROC WriteAPBXCDsourceTreeGroup(void)
{
	if (ide_vers < 2000) {
		WriteDestFileLn("refType = 4;");
	}
	if (ide_vers >= 1500) {
		WriteDestFileLn("sourceTree = \"<group>\";");
	}
}

LOCALPROC WriteDummyLangFileNameInResources(void)
{
	WriteDummyLangFileName();
	WriteCStrToDestFile(" in Resources");
}

LOCALPROC DoDummyLangAPBXCDaddFileBody(void)
{
	WriteCStrToDestFile("isa = PBXBuildFile;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileRef = ");
	WriteAPBXCDObjectIdAndComment(APBoclsGen,
		APBgenoLangDummyRf, WriteDummyLangFileName);
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("settings = {");
		WriteAPBXCDSepA();
		WriteCStrToDestFile("};");
	}
}

LOCALPROC DoDummyLangAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBoclsGen, APBgenoLangDummyBld,
		WriteDummyLangFileNameInResources,
		DoDummyLangAPBXCDaddFileBody);
}

LOCALPROC DoLangDummyAPBXCDaddFileRefBody(void)
{
	WriteCStrToDestFile("isa = PBXFileReference;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileEncoding = 30;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("lastKnownFileType = text;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("name = ");
	WriteLProjName();
	WriteCStrToDestFile(";");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("path = ");
	WriteLProjFolderPath();
	WriteDummyLangFileName();
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("refType = 4;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("sourceTree = \"<group>\";");
	}
}

LOCALPROC DoLangDummyAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsGen, APBgenoLangRf,
		WriteLProjName,
		DoLangDummyAPBXCDaddFileRefBody);
}

LOCALPROC DoLangDummyAPBXCDaddToSources(void)
{
	WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoLangDummyBld,
		WriteDummyLangFileNameInResources);
}

LOCALPROC DoLangDummyAPBXCDaddVariant(void)
{
	WriteAPBXCDBeginObject(APBoclsGen, APBgenoLangDummyRf, WriteDummyLangFileName);
		WriteDestFileLn("isa = PBXVariantGroup;");
		WriteAPBXCDBgnObjList("children");
			WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoLangRf, WriteLProjName);
		WriteAPBXCDEndObjList();
		WriteDestFileLn("name = dummy.txt;");
		WriteAPBXCDsourceTreeGroup();
	WriteAPBXCDEndObject();
}

static void DoBeginSectionAPBXCD(char *Name)
{
	if (ide_vers >= 2000) {
		--DestFileIndent; --DestFileIndent;
		WriteBlankLineToDestFile();
		WriteBgnDestFileLn();
		WriteCStrToDestFile("/* Begin ");
		WriteCStrToDestFile(Name);
		WriteCStrToDestFile(" section */");
		WriteEndDestFileLn();
		++DestFileIndent; ++DestFileIndent;
	}
}

static void DoEndSectionAPBXCD(char *Name)
{
	if (ide_vers >= 2000) {
		--DestFileIndent; --DestFileIndent;
		WriteBgnDestFileLn();
		WriteCStrToDestFile("/* End ");
		WriteCStrToDestFile(Name);
		WriteCStrToDestFile(" section */");
		WriteEndDestFileLn();
		++DestFileIndent; ++DestFileIndent;
	}
}

LOCALPROC WriteXCDconfigname(void)
{
	char *s;

	switch (gbo_dbg) {
		case gbk_dbg_on:
			if (ide_vers < 2000) {
				s = "Development";
			} else {
				s = "Debug";
			}
			break;
		case gbk_dbg_test:
			s = "Test";
			break;
		case gbk_dbg_off:
			if (ide_vers < 2000) {
				s = "Deployment";
			} else {
				s = "Release";
			}
			break;
		default:
			s = "(unknown Debug Level)";
			break;
	}

	WriteCStrToDestFile(s);
}

LOCALPROC WriteAPBXCDProductName(void)
{
	if (HaveMacBundleApp) {
		WriteMachoAppNameStr();
	} else {
		WriteStrAppAbbrev();
	}
}

LOCALPROC DoProductAPBXCDaddFileRefBody(void)
{
	if (ide_vers < 1500) {
		WriteCStrToDestFile("isa = PBXApplicationReference;");
	} else {
		WriteCStrToDestFile("isa = PBXFileReference;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("includeInIndex = 0;");
		WriteAPBXCDSepA();
		if (HaveMacBundleApp) {
			WriteCStrToDestFile("lastKnownFileType = wrapper.application;");
		} else {
			WriteCStrToDestFile("lastKnownFileType = \"compiled.mach-o.executable\";");
		}
	}
	WriteAPBXCDSepA();
	WriteCStrToDestFile("path = ");
	WriteAPBXCDProductName();
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("refType = 3;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("sourceTree = BUILT_PRODUCTS_DIR;");
	}
}

LOCALPROC DoProductAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsGen, APBgenoProductRef,
		WriteAPBXCDProductName,
		DoProductAPBXCDaddFileRefBody);
}

LOCALPROC DoPlistAPBXCDaddFileRefBody(void)
{
	WriteCStrToDestFile("isa = PBXFileReference;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("fileEncoding = 4;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("lastKnownFileType = text.xml;");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("name = ");
	WriteInfoPlistFileName();
	WriteCStrToDestFile(";");
	WriteAPBXCDSepA();
	WriteCStrToDestFile("path = ");
	WriteInfoPlistFilePath();
	WriteCStrToDestFile(";");
	if (ide_vers < 2000) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("refType = 2;");
	}
	if (ide_vers >= 1500) {
		WriteAPBXCDSepA();
		WriteCStrToDestFile("sourceTree = SOURCE_ROOT;");
	}
}

LOCALPROC DoPlistAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsGen, APBgenoPlist,
		WriteInfoPlistFileName,
		DoPlistAPBXCDaddFileRefBody);
}

LOCALPROC WriteAPBXCDBuildSettings(void)
{
	if (ide_vers >= 2000) {
		/* if (CrossCompile) */ {
			if (gbo_cpufam == gbk_cpufam_x86) {
				WriteDestFileLn("ARCHS = i386;");
			} else {
				WriteDestFileLn("ARCHS = ppc;");
			}
		}
	}
	if (ide_vers >= 2000) { /*^*/
		WriteDestFileLn("CONFIGURATION_BUILD_DIR = \"$(PROJECT_DIR)\";");
	}
	if (ide_vers >= 2000) { /*^*/
		WriteDestFileLn("COPY_PHASE_STRIP = NO;");
	} else {
		if (gbo_dbg != gbk_dbg_on) {
			WriteDestFileLn("COPY_PHASE_STRIP = YES;");
		} else {
			WriteDestFileLn("COPY_PHASE_STRIP = NO;");
		}
	}
	if (ide_vers >= 1500) {
		if (gbo_dbg == gbk_dbg_off) {
			WriteDestFileLn("DEPLOYMENT_POSTPROCESSING = YES;");
		}
	}
	if (ide_vers < 1500) {
		if (gbo_dbg == gbk_dbg_off) {
			WriteDestFileLn("DEBUGGING_SYMBOLS = NO;");
		}
	}
	if (ide_vers >= 1500) {
		WriteDestFileLn("GCC_CW_ASM_SYNTAX = NO;");
	}
	if (ide_vers >= 1500) {
		WriteDestFileLn("GCC_DYNAMIC_NO_PIC = YES;");
	}
	if (ide_vers < 1500) {
		WriteAPBQuotedField("FRAMEWORK_SEARCH_PATHS", "");
	}
	if (ide_vers >= 1500) {
		WriteDestFileLn("GCC_ENABLE_FIX_AND_CONTINUE = NO;");
		if (gbo_dbg == gbk_dbg_off) {
			WriteDestFileLn("GCC_GENERATE_DEBUGGING_SYMBOLS = NO;");
		}
		WriteDestFileLn("GCC_MODEL_TUNING = \"\";");
		if (gbo_dbg == gbk_dbg_on) {
			WriteDestFileLn("GCC_OPTIMIZATION_LEVEL = 0;");
		} else {
			WriteDestFileLn("GCC_OPTIMIZATION_LEVEL = s;");
		}
	}
	if (ide_vers >= 2000) {
		WriteDestFileLn("GCC_PRECOMPILE_PREFIX_HEADER = NO;");
		WriteDestFileLn("GCC_PREFIX_HEADER = \"\";");
		WriteDestFileLn("GCC_SYMBOLS_PRIVATE_EXTERN = NO;");
	}
	if (ide_vers >= 1500) {
		WriteDestFileLn("GCC_WARN_ABOUT_MISSING_PROTOTYPES = YES;");
	}

	if (HaveMacBundleApp) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("INFOPLIST_FILE = ");
		WriteInfoPlistFilePath();
		WriteCStrToDestFile(";");
		WriteEndDestFileLn();
	}
	if (ide_vers >= 1500) {
		WriteDestFileLn("INSTALL_PATH = \"$(HOME)/Applications\";");
	}
	if (ide_vers < 1500) {
		WriteAPBQuotedField("LIBRARY_SEARCH_PATHS", "");
	}
	if (ide_vers < 1500) {
		if (gbk_dbg_on == gbo_dbg) {
			WriteAPBQuotedField("OPTIMIZATION_CFLAGS", "-O0");
		}
	}
#if UseAlignMac68k
	WriteDestFileLn("OTHER_CFLAGS = \"-malign-mac68k\";");
#endif
	if (ide_vers < 1500) {
		WriteAPBQuotedField("OTHER_LDFLAGS", "");
		WriteAPBQuotedField("OTHER_REZFLAGS", "");
	} else {
		if (! HaveMacBundleApp) {
			WriteAPBXCDBgnObjList("OTHER_LDFLAGS");
				WriteDestFileLn("\"-L/usr/X11R6/lib\",");
				WriteDestFileLn("\"-lXext\",");
				WriteDestFileLn("\"-lX11\",");
			WriteAPBXCDEndObjList();
		}
	}
	if (ide_vers >= 1500) {
		WriteDestFileLn("PREBINDING = NO;");
	}
	WriteBgnDestFileLn();
	WriteCStrToDestFile("PRODUCT_NAME = ");
	WriteStrAppAbbrev();
	WriteCStrToDestFile(";");
	WriteEndDestFileLn();
	if (ide_vers >= 2000) {
		/* if (! CrossCompile) {
			WriteDestFileLn("SDKROOT = \"\";");
		} */
	}
	if (ide_vers < 1500) {
		WriteAPBQuotedField("SECTORDER_FLAGS", "");
	}
	if (ide_vers >= 2000) {
		if (gbo_dbg == gbk_dbg_off) {
			WriteDestFileLn("SEPARATE_STRIP = YES;");
			WriteDestFileLn("STRIPFLAGS = \"-u -r\";");
			WriteDestFileLn("STRIP_INSTALLED_PRODUCT = YES;");
		}
	}
	if (ide_vers >= 2000) {
		WriteAPBXCDBgnObjList("WARNING_CFLAGS");
			WriteDestFileLn("\"-Wall\",");
			WriteDestFileLn("\"-Wstrict-prototypes\",");
			WriteDestFileLn("\"-Wno-uninitialized\",");
		WriteAPBXCDEndObjList();
	} else {
		WriteAPBQuotedField("WARNING_CFLAGS", "-Wall -Wstrict-prototypes -Wno-uninitialized -Wno-four-char-constants -Wno-unknown-pragmas");
	}
	if (HaveMacBundleApp) {
		WriteDestFileLn("WRAPPER_EXTENSION = app;");
	}
	if (ide_vers >= 1500) {
		WriteDestFileLn("ZERO_LINK = NO;");
	}
}

LOCALPROC WriteAPBplist(void)
{
	int SaveDestFileIndent = DestFileIndent;

	DestFileIndent = 0;

	WriteBgnDestFileLn();
	WriteCStrToDestFile("<!DOCTYPE plist PUBLIC ");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("-//Apple Computer//DTD PLIST 1.0//EN");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" ");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("http://www.apple.com/DTDs/PropertyList-1.0.dtd");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(">");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("<plist version=");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("1.0");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(">");
	WriteEndDestFileLn();

	CurPListFormat = kPListRaw;

	WriteMyInfoPListContents();

	WriteDestFileLn("</plist>");
	WriteBgnDestFileLn();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(";");
	WriteEndDestFileLn();

	DestFileIndent = SaveDestFileIndent;
}

LOCALPROC WriteStrFrameworks(void)
{
	WriteCStrToDestFile("Frameworks");
}

LOCALPROC WriteStrSources(void)
{
	WriteCStrToDestFile("Sources");
}

LOCALPROC WriteStrResources(void)
{
	WriteCStrToDestFile("Resources");
}

LOCALPROC WriteStrRez(void)
{
	WriteCStrToDestFile("Rez");
}

LOCALPROC WriteStrFrameworksLibraries(void)
{
	WriteCStrToDestFile("External Frameworks and Libraries");
}

LOCALPROC WriteStrProducts(void)
{
	WriteCStrToDestFile("Products");
}

LOCALPROC WriteStrHeaders(void)
{
	WriteCStrToDestFile("Headers");
}

LOCALPROC WriteStrIncludes(void)
{
	WriteCStrToDestFile("Includes");
}

LOCALPROC WriteStrProjectObject(void)
{
	WriteCStrToDestFile("Project object");
}

LOCALPROC WriteStrConfListPBXProject(void)
{
	WriteCStrToDestFile("Build configuration list for PBXProject \"");
	WriteStrAppAbbrev();
	WriteCStrToDestFile("\"");
}

LOCALPROC WriteStrConfListPBXNativeTarget(void)
{
	WriteCStrToDestFile("Build configuration list for PBXNativeTarget \"");
	WriteStrAppAbbrev();
	WriteCStrToDestFile("\"");
}

#define HaveFrameworks HaveMacBundleApp

static void WriteXCDSpecificFiles(void)
{
	MyDir_R LangDirR;

	if (HaveMacBundleApp) {
		if (HaveAPBXCD_PlistFile) {
			WritePListData();
		}

		if (MakeSubDirectory(&LangDirR, &SrcDirR, GetLProjName(gbo_lang), ".lproj")) {
			if (WriteOpenDestFile(&LangDirR, "dummy", ".txt")) {
				WriteDestFileLn("dummy");
				WriteCloseDestFile();
			}
		}
	}

	if (WriteOpenDestFile(&ProjDirR, "project", ".pbxproj")) { /* project file */

	WriteDestFileLn("// !$*UTF8*$!");
	WriteDestFileLn("{");
	++DestFileIndent;
		WriteDestFileLn("archiveVersion = 1;");
		WriteDestFileLn("classes = {");
		WriteDestFileLn("};");
		if (ide_vers >= 2000) {
			WriteDestFileLn("objectVersion = 42;");
		} else if (ide_vers >= 1500) {
			WriteDestFileLn("objectVersion = 39;");
		} else {
			WriteDestFileLn("objectVersion = 38;");
		}
		WriteDestFileLn("objects = {");
	++DestFileIndent;

	DoBeginSectionAPBXCD("PBXBuildFile");
		DoAllSrcFilesWithSetup(DoSrcFileAPBXCDaddFile);
		if (HaveMacBundleApp) {
			DoAllDocTypesWithSetup(DoDocTypeAPBXCDaddFile);
		}

		if (HaveFrameworks) {
			DoAllFrameWorksWithSetup(DoFrameWorkAPBXCDaddFile);
		}
		if (HaveAPBXCD_StdcLib) {
			DoLibStdcAPBXCDaddFile();
		}

		if (HaveMacRrscs) {
			DoRsrcAPBXCDaddFile();
		}

		if (HaveMacBundleApp) {
			if (HaveAPBXCD_LangDummy) {
				DoDummyLangAPBXCDaddFile();
			}
		}
	DoEndSectionAPBXCD("PBXBuildFile");

	if (ide_vers < 2000) {
		DoBeginSectionAPBXCD("PBXBuildStyle");
			WriteAPBXCDBeginObject(APBoclsGen, APBgenoBuildStyle, WriteXCDconfigname);
				WriteDestFileLn("isa = PBXBuildStyle;");
				if (ide_vers < 1500) {
					WriteAPBXCDBgnObjList("buildRules");
					WriteAPBXCDEndObjList();
				}
				WriteDestFileLn("buildSettings = {");
				WriteDestFileLn("};");
				WriteBgnDestFileLn();
				WriteCStrToDestFile("name = ");
				WriteXCDconfigname();
				WriteCStrToDestFile(";");
				WriteEndDestFileLn();
			WriteAPBXCDEndObject();
		DoEndSectionAPBXCD("PBXBuildStyle");
	}

	DoBeginSectionAPBXCD("PBXFileReference");
		DoAllSrcFilesWithSetup(DoSrcFileAPBXCDaddFileRef);

		if (HaveAPBXCD_Headers) {
			DoAllSrcFilesWithSetup(DoHeaderFileXCDaddFileRef);
			DoAllExtraHeaders2WithSetup(DoExtraHeaderFileXCDaddFileRef);
		}

		if (HaveMacBundleApp) {
			DoAllDocTypesWithSetup(DoDocTypeAPBXCDaddFileRef);
		}

		if (HaveFrameworks) {
			DoAllFrameWorksWithSetup(DoFrameWorkAPBXCDaddFileRef);
		}
		if (HaveAPBXCD_StdcLib) {
			DoLibStdcAPBXCDaddFileRef();
		}

		DoProductAPBXCDaddFileRef();

		if (HaveMacBundleApp) {
			if (HaveAPBXCD_PlistFile) {
				DoPlistAPBXCDaddFileRef();
			}
		}
		if (HaveMacRrscs) {
			DoRsrcAPBXCDaddFileRef();
		}
		if (HaveMacBundleApp) {
			if (HaveAPBXCD_LangDummy) {
				DoLangDummyAPBXCDaddFileRef();
			}
		}
	DoEndSectionAPBXCD("PBXFileReference");

	DoBeginSectionAPBXCD("PBXFrameworksBuildPhase");
		WriteAPBXCDBeginObject(APBoclsGen, APBgenoPhaseLibs, WriteStrFrameworks);
			WriteDestFileLn("isa = PBXFrameworksBuildPhase;");
			WriteDestFileLn("buildActionMask = 2147483647;");
			WriteAPBXCDBgnObjList("files");
				if (HaveFrameworks) {
					DoAllFrameWorksWithSetup(DoFrameworkAPBXCDaddToBuild);
				}
				if (HaveAPBXCD_StdcLib) {
					WriteAPBXCDobjlistelmp(APBoclsLibs, APBliboLibStdcBld, WriteLibStdcName);
				}
			WriteAPBXCDEndObjList();
			WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
		WriteAPBXCDEndObject();
	DoEndSectionAPBXCD("PBXFrameworksBuildPhase");

	DoBeginSectionAPBXCD("PBXGroup");
		WriteAPBXCDBeginObject(APBoclsGen, APBgenoSources, WriteStrSources);
			WriteDestFileLn("isa = PBXGroup;");
			WriteAPBXCDBgnObjList("children");
				DoAllSrcFilesWithSetup(DoSrcFileAPBXCDaddToGroup);
				if (HaveMacRrscs) {
					if (ide_vers >= 2000) {
						WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoMainRsrcRf,
							WriteMainRsrcName);
					}
				}
			WriteAPBXCDEndObjList();
			WriteDestFileLn("name = Sources;");
			if (ide_vers < 2000) {
				WriteAPBQuotedField("path", "");
			}
			WriteAPBXCDsourceTreeGroup();
		WriteAPBXCDEndObject();

		if (HaveMacBundleApp) {
			WriteAPBXCDBeginObject(APBoclsGen, APBgenoResources, WriteStrResources);
				WriteDestFileLn("isa = PBXGroup;");
				WriteAPBXCDBgnObjList("children");
					if (HaveMacRrscs) {
						if (ide_vers < 2000) {
							WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoMainRsrcRf, WriteMainRsrcName);
						}
					}
					DoAllDocTypesWithSetup(DoDocTypeAPBXCDaddToGroup);
					if (HaveAPBXCD_PlistFile) {
						WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoPlist, WriteInfoPlistFileName);
					}
					if (HaveAPBXCD_LangDummy) {
						WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoLangDummyRf, WriteDummyLangFileName);
					}
				WriteAPBXCDEndObjList();
				WriteDestFileLn("name = Resources;");
				if (ide_vers < 2000) {
					WriteAPBQuotedField("path", "");
				}
				WriteAPBXCDsourceTreeGroup();
			WriteAPBXCDEndObject();
		}

		if (HaveFrameworks) {
			WriteAPBXCDBeginObject(APBoclsGen, APBgenoLibraries, WriteStrFrameworksLibraries);
				WriteDestFileLn("isa = PBXGroup;");
				WriteAPBXCDBgnObjList("children");
					DoAllFrameWorksWithSetup(DoFrameworkAPBXCDaddToLibraries);
					if (HaveAPBXCD_StdcLib) {
						WriteAPBXCDobjlistelmp(APBoclsLibs, APBliboLibStdcRf, WriteLibStdcName);
					}
				WriteAPBXCDEndObjList();
				WriteAPBQuotedField("name", "External Frameworks and Libraries");
				if (ide_vers < 2000) {
					WriteAPBQuotedField("path", "");
				}
				WriteAPBXCDsourceTreeGroup();
			WriteAPBXCDEndObject();
		}

		WriteAPBXCDBeginObject(APBoclsGen, APBgenoProducts, WriteStrProducts);
			WriteDestFileLn("isa = PBXGroup;");
			WriteAPBXCDBgnObjList("children");
				WriteBgnDestFileLn();
				WriteAPBXCDObjectIdAndComment(APBoclsGen, APBgenoProductRef, WriteAPBXCDProductName);
				WriteCStrToDestFile(",");
				WriteEndDestFileLn();
			WriteAPBXCDEndObjList();
			WriteDestFileLn("name = Products;");
			WriteAPBXCDsourceTreeGroup();
		WriteAPBXCDEndObject();

		WriteAPBXCDBeginObject(APBoclsGen, APBgenoMainGroup, WriteStrAppAbbrev);
			WriteDestFileLn("isa = PBXGroup;");
			WriteAPBXCDBgnObjList("children");
				WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoSources, WriteStrSources);
				if (HaveAPBXCD_Headers) {
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoSrcHeaders, WriteStrHeaders);
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoIncludes, WriteStrIncludes);
				}
				if (HaveMacBundleApp) {
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoResources, WriteStrResources);
				}
				if (HaveFrameworks) {
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoLibraries, WriteStrFrameworksLibraries);
				}
				WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoProducts, WriteStrProducts);
			WriteAPBXCDEndObjList();
			WriteBgnDestFileLn();

			WriteCStrToDestFile("name = ");
			if (ide_vers < 1500) {
				WriteQuoteToDestFile();
				WriteAppVariationStr();
				WriteQuoteToDestFile();
			} else {
				WriteStrAppAbbrev();
			}
			WriteCStrToDestFile(";");
			WriteEndDestFileLn();
			if (ide_vers < 2000) {
				WriteAPBQuotedField("path", "");
			}
			WriteAPBXCDsourceTreeGroup();
		WriteAPBXCDEndObject();
		if (HaveAPBXCD_Headers) {
			WriteAPBXCDBeginObject(APBoclsGen, APBgenoSrcHeaders, WriteStrHeaders);
				WriteDestFileLn("isa = PBXGroup;");
				WriteAPBXCDBgnObjList("children");
					DoAllSrcFilesWithSetup(DoHeaderFileXCDaddToGroup);
				WriteAPBXCDEndObjList();
				WriteDestFileLn("name = Headers;");
				WriteAPBXCDsourceTreeGroup();
			WriteAPBXCDEndObject();
			WriteAPBXCDBeginObject(APBoclsGen, APBgenoIncludes, WriteStrIncludes);
				WriteDestFileLn("isa = PBXGroup;");
				WriteAPBXCDBgnObjList("children");
					DoAllExtraHeaders2WithSetup(DoExtraHeaderFileXCDaddToGroup);
				WriteAPBXCDEndObjList();
				WriteDestFileLn("name = Includes;");
				WriteAPBXCDsourceTreeGroup();
			WriteAPBXCDEndObject();
		}
	DoEndSectionAPBXCD("PBXGroup");

	DoBeginSectionAPBXCD("PBXNativeTarget");
		WriteAPBXCDBeginObject(APBoclsGen, APBgenoTarget, WriteStrAppAbbrev);
			if (ide_vers < 1500) {
				WriteDestFileLn("isa = PBXApplicationTarget;");
			} else {
				WriteDestFileLn("isa = PBXNativeTarget;");
			}
			if (ide_vers >= 2000) {
				WriteBgnDestFileLn();
				WriteCStrToDestFile("buildConfigurationList = ");
				WriteAPBXCDObjectIdAndComment(APBoclsGen, APBgenoLstNatCnfg,
					WriteStrConfListPBXNativeTarget);
				WriteCStrToDestFile(";");
				WriteEndDestFileLn();
			}
			WriteAPBXCDBgnObjList("buildPhases");
				if (ide_vers < 1500) {
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoHeaders, WriteStrHeaders);
				}
				if (HaveMacBundleApp) {
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoBunRsrcs, WriteStrResources);
				}
				WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoPhaseSrcs, WriteStrSources);
				WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoPhaseLibs, WriteStrFrameworks);
				if (HaveMacRrscs) {
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoPhaseRsrc, WriteStrRez);
				}
			WriteAPBXCDEndObjList();
			if (ide_vers >= 1500) {
				WriteAPBXCDBgnObjList("buildRules");
				WriteAPBXCDEndObjList();
			}
			if (ide_vers < 2000) {
				WriteDestFileLn("buildSettings = {");
				++DestFileIndent;
				WriteAPBXCDBuildSettings();
				--DestFileIndent;
				WriteDestFileLn("};");
			}
			WriteAPBXCDBgnObjList("dependencies");
			WriteAPBXCDEndObjList();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("name = ");
			WriteStrAppAbbrev();
			WriteCStrToDestFile(";");
			WriteEndDestFileLn();

			if (ide_vers >= 1500) {
				WriteDestFileLn("productInstallPath = \"$(HOME)/Applications\";");
			}

			WriteBgnDestFileLn();
			WriteCStrToDestFile("productName = ");
			if (ide_vers < 1500) {
				WriteQuoteToDestFile();
			}
			WriteStrAppAbbrev();
			if (ide_vers < 1500) {
				WriteQuoteToDestFile();
			}
			WriteCStrToDestFile(";");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("productReference = ");
			WriteAPBXCDObjectIdAndComment(APBoclsGen, APBgenoProductRef, WriteAPBXCDProductName);
			WriteCStrToDestFile(";");
			WriteEndDestFileLn();

			if (ide_vers >= 1500) {
				if (HaveMacBundleApp) {
					WriteDestFileLn("productType = \"com.apple.product-type.application\";");
				} else {
					WriteDestFileLn("productType = \"com.apple.product-type.tool\";");
				}
			}
			if ((! HaveAPBXCD_PlistFile) && HaveMacBundleApp) {
				WriteBgnDestFileLn();
				WriteCStrToDestFile("productSettingsXML = ");
				WriteQuoteToDestFile();
				WriteCStrToDestFile("<?xml version=");
				WriteBackSlashToDestFile();
				WriteQuoteToDestFile();
				WriteCStrToDestFile("1.0");
				WriteBackSlashToDestFile();
				WriteQuoteToDestFile();
				WriteCStrToDestFile(" encoding=\\\"UTF-8\\\"?>");
				WriteEndDestFileLn();

				WriteAPBplist();
			}
		WriteAPBXCDEndObject();
	DoEndSectionAPBXCD("PBXNativeTarget");

	DoBeginSectionAPBXCD("PBXProject");
		WriteAPBXCDBeginObject(APBoclsGen, APBgenoRoot, WriteStrProjectObject);
			WriteDestFileLn("isa = PBXProject;");
			if (ide_vers >= 2000) {
				WriteBgnDestFileLn();
				WriteCStrToDestFile("buildConfigurationList = ");
				WriteAPBXCDObjectIdAndComment(APBoclsGen, APBgenoLstPrjCnfg, WriteStrConfListPBXProject);
				WriteCStrToDestFile(";");
				WriteEndDestFileLn();
			}
			if (ide_vers < 2000) {
				if (ide_vers >= 1500) {
					WriteDestFileLn("buildSettings = {");
					WriteDestFileLn("};");
				}
				WriteAPBXCDBgnObjList("buildStyles");
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoBuildStyle, WriteXCDconfigname);
				WriteAPBXCDEndObjList();
			}

			WriteDestFileLn("hasScannedForEncodings = 1;");

			WriteBgnDestFileLn();
			WriteCStrToDestFile("mainGroup = ");
			WriteAPBXCDObjectIdAndComment(APBoclsGen, APBgenoMainGroup, WriteStrAppAbbrev);
			WriteCStrToDestFile(";");
			WriteEndDestFileLn();

			WriteDestFileLn("projectDirPath = \"\";");

			WriteAPBXCDBgnObjList("targets");
				WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoTarget, WriteStrAppAbbrev);
			WriteAPBXCDEndObjList();
		WriteAPBXCDEndObject();
	DoEndSectionAPBXCD("PBXProject");

	if (HaveMacBundleApp) {
		DoBeginSectionAPBXCD("PBXResourcesBuildPhase");
			WriteAPBXCDBeginObject(APBoclsGen, APBgenoBunRsrcs, WriteStrResources);
				WriteDestFileLn("isa = PBXResourcesBuildPhase;");
				WriteDestFileLn("buildActionMask = 2147483647;");
				WriteAPBXCDBgnObjList("files");
					DoAllDocTypesWithSetup(DoDocTypeAPBXCDaddToSources);
					if (HaveAPBXCD_LangDummy) {
						DoLangDummyAPBXCDaddToSources();
					}
				WriteAPBXCDEndObjList();
				WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
			WriteAPBXCDEndObject();
		DoEndSectionAPBXCD("PBXResourcesBuildPhase");
	}
	if (HaveMacRrscs) {
		DoBeginSectionAPBXCD("PBXRezBuildPhase");
			WriteAPBXCDBeginObject(APBoclsGen, APBgenoPhaseRsrc, WriteStrRez);
				WriteDestFileLn("isa = PBXRezBuildPhase;");
				WriteDestFileLn("buildActionMask = 2147483647;");
				WriteAPBXCDBgnObjList("files");
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoMnRsrcBld,
						WriteMainAPBXCDRsrcNameinRez);
				WriteAPBXCDEndObjList();
				WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
			WriteAPBXCDEndObject();
		DoEndSectionAPBXCD("PBXRezBuildPhase");
	}

	if (ide_vers < 1500) {
		WriteAPBXCDBeginObject(APBoclsGen, APBgenoHeaders, WriteStrHeaders);
			WriteDestFileLn("isa = PBXHeadersBuildPhase;");
			WriteDestFileLn("buildActionMask = 2147483647;");
			WriteAPBXCDBgnObjList("files");
			WriteAPBXCDEndObjList();
			WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
		WriteAPBXCDEndObject();
	}

	DoBeginSectionAPBXCD("PBXSourcesBuildPhase");
		WriteAPBXCDBeginObject(APBoclsGen, APBgenoPhaseSrcs, WriteStrSources);
			WriteDestFileLn("isa = PBXSourcesBuildPhase;");
			WriteDestFileLn("buildActionMask = 2147483647;");
			WriteAPBXCDBgnObjList("files");
				DoAllSrcFilesWithSetup(DoSrcFileAPBXCDaddToSources);
			WriteAPBXCDEndObjList();
			WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
		WriteAPBXCDEndObject();
	DoEndSectionAPBXCD("PBXSourcesBuildPhase");

	if (HaveMacBundleApp) {
		if (HaveAPBXCD_LangDummy) {
			DoBeginSectionAPBXCD("PBXVariantGroup");
				DoLangDummyAPBXCDaddVariant();
			DoEndSectionAPBXCD("PBXVariantGroup");
		}
	}

	if (ide_vers >= 2000) {
		DoBeginSectionAPBXCD("XCBuildConfiguration");
			WriteAPBXCDBeginObject(APBoclsGen, APBgenoNatCnfg, WriteXCDconfigname);
				WriteDestFileLn("isa = XCBuildConfiguration;");
				WriteDestFileLn("buildSettings = {");
				++DestFileIndent;
					WriteAPBXCDBuildSettings();
				--DestFileIndent;
				WriteDestFileLn("};");
				WriteBgnDestFileLn();
				WriteCStrToDestFile("name = ");
				WriteXCDconfigname();
				WriteCStrToDestFile(";");
				WriteEndDestFileLn();
			WriteAPBXCDEndObject();
			WriteAPBXCDBeginObject(APBoclsGen, APBgenoPrjCnfg, WriteXCDconfigname);
				WriteDestFileLn("isa = XCBuildConfiguration;");
				WriteDestFileLn("buildSettings = {");
				/* if (CrossCompile) */ {
					++DestFileIndent;
						if (gbo_cpufam == gbk_cpufam_x86) {
							WriteDestFileLn("MACOSX_DEPLOYMENT_TARGET = 10.4;");
						} else {
							WriteDestFileLn("MACOSX_DEPLOYMENT_TARGET = 10.1;");
						}
						WriteDestFileLn("SDKROOT = /Developer/SDKs/MacOSX10.4u.sdk;");
					--DestFileIndent;
				}
				WriteDestFileLn("};");
				WriteBgnDestFileLn();
				WriteCStrToDestFile("name = ");
				WriteXCDconfigname();
				WriteCStrToDestFile(";");
				WriteEndDestFileLn();
			WriteAPBXCDEndObject();
		DoEndSectionAPBXCD("XCBuildConfiguration");
	}

	if (ide_vers >= 2000) {
		DoBeginSectionAPBXCD("XCConfigurationList");
			WriteBgnDestFileLn();
			WriteAPBXCDObjectIdAndComment(APBoclsGen, APBgenoLstNatCnfg,
				WriteStrConfListPBXNativeTarget);
			WriteCStrToDestFile(" = {");
			WriteEndDestFileLn();
			++DestFileIndent;
				WriteDestFileLn("isa = XCConfigurationList;");
				WriteAPBXCDBgnObjList("buildConfigurations");
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoNatCnfg, WriteXCDconfigname);
				WriteAPBXCDEndObjList();
				WriteDestFileLn("defaultConfigurationIsVisible = 0;");
				WriteBgnDestFileLn();
				WriteCStrToDestFile("defaultConfigurationName = ");
				WriteXCDconfigname();
				WriteCStrToDestFile(";");
				WriteEndDestFileLn();
			--DestFileIndent;
			WriteDestFileLn("};");
			WriteBgnDestFileLn();
			WriteAPBXCDObjectIdAndComment(APBoclsGen, APBgenoLstPrjCnfg,
				WriteStrConfListPBXProject);
			WriteCStrToDestFile(" = {");
			WriteEndDestFileLn();
			++DestFileIndent;
				WriteDestFileLn("isa = XCConfigurationList;");
				WriteAPBXCDBgnObjList("buildConfigurations");
					WriteAPBXCDobjlistelmp(APBoclsGen, APBgenoPrjCnfg, WriteXCDconfigname);
				WriteAPBXCDEndObjList();
				WriteDestFileLn("defaultConfigurationIsVisible = 0;");
				WriteBgnDestFileLn();
				WriteCStrToDestFile("defaultConfigurationName = ");
				WriteXCDconfigname();
				WriteCStrToDestFile(";");
				WriteEndDestFileLn();
			--DestFileIndent;
			WriteDestFileLn("};");
		DoEndSectionAPBXCD("XCConfigurationList");
	}

	--DestFileIndent;
		WriteDestFileLn("};");
		WriteBgnDestFileLn();
		WriteCStrToDestFile("rootObject = ");
		WriteAPBXCDObjectIdAndComment(APBoclsGen, APBgenoRoot, WriteStrProjectObject);
		WriteCStrToDestFile(";");
		WriteEndDestFileLn();
	--DestFileIndent;
	WriteDestFileLn("}");

	WriteCloseDestFile();
	}
}
