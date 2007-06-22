/*
	WRMSCFLS.i
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
	WRite Microsoft C specific FiLes
*/


#pragma segment MSCSupport

LOCALPROC WriteMSVCQuotedDefine(char *s)
{
	WriteCStrToDestFile(" /D ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(s);
	WriteQuoteToDestFile();
}

LOCALPROC WriteCLexeFlags(void)
{
	WriteCStrToDestFile("/nologo");
	if (gbo_dbg == gbk_dbg_on) {
		WriteCStrToDestFile(" /Od");
	} else {
		WriteCStrToDestFile(" /O1");
	}
	WriteMSVCQuotedDefine("WIN32");
	if (gbo_dbg == gbk_dbg_on) {
		WriteMSVCQuotedDefine("_DEBUG");
	} else {
		WriteMSVCQuotedDefine("NDEBUG");
	}
	WriteMSVCQuotedDefine("_WINDOWS");
	if (ide_vers >= 6000) {
		WriteMSVCQuotedDefine("_MBCS");
	}

	if (gbo_dbg == gbk_dbg_on) {
		WriteCStrToDestFile(" /Gm"); /* minimal rebuild */
	}

	/* WriteCStrToDestFile(" /GX"); enable exception handling */

	if (gbo_dbg == gbk_dbg_on) {
		if (ide_vers >= 8000) {
			WriteCStrToDestFile(" /RTC1");
		} else {
			WriteCStrToDestFile(" /GZ");
		}
	} else {
		WriteCStrToDestFile(" /GF"); /* string pooling */
	}

	if (gbo_dbg == gbk_dbg_on) {
		if (ide_vers >= 8000) {
			WriteCStrToDestFile(" /MTd");
		} else {
			WriteCStrToDestFile(" /MLd");
		}
	} else {
		if (ide_vers >= 8000) {
			WriteCStrToDestFile(" /MT");
		} else {
			WriteCStrToDestFile(" /ML");
		}
	}

	WriteCStrToDestFile(" /W4");

	WriteCStrToDestFile(" /c");

	if (ide_vers >= 8000) {
		WriteCStrToDestFile(" /Wp64");
	}

	if (gbo_dbg == gbk_dbg_on) {
		WriteCStrToDestFile(" /ZI");
	}
}

LOCALPROC WriteRCexeFlags(void)
{
	WriteCStrToDestFile("/l 0x409 /d ");
	WriteQuoteToDestFile();
	if (gbo_dbg == gbk_dbg_on) {
		WriteCStrToDestFile("_DEBUG");
	} else {
		WriteCStrToDestFile("NDEBUG");
	}
	WriteQuoteToDestFile();
}

LOCALPROC WriteFileToMSVCSource(MyProc p)
{
	WriteDestFileLn("# Begin Source File");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("SOURCE=");
	p();
	WriteEndDestFileLn();

	WriteDestFileLn("# End Source File");
}

LOCALPROC WriteDocTypeMSVCresource(void)
{
	WriteFileToMSVCSource(WriteDocTypeIconFilePath);
}

LOCALPROC WriteMSVCBeginGroup(char *group, char *filter)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("# Begin Group ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(group);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# PROP Default_Filter ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(filter);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC WriteMSVCEndGroup(void)
{
	WriteDestFileLn("# End Group");
}

LOCALPROC WriteMSVCTargetName(void)
{
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(" - Win32");
	/* WriteAppVariationStr(); */
	WriteQuoteToDestFile();
}

LOCALPROC WriteMSVCMakefileName(void)
{
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(".mak");
	WriteQuoteToDestFile();
}

LOCALPROC WriteMSVCQuotedProp(char *p, char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("# PROP ");
	WriteCStrToDestFile(p);
	WriteCStrToDestFile(" ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(s);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC DoSrcFileMSVCAddFile(void)
{
	WriteDestFileLn("# Begin Source File");
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("SOURCE=");
	WriteSrcFileFilePath();
	WriteEndDestFileLn();
#if 0
	if (gbo_dbg != gbk_dbg_on) {
		if (fast) {
			WriteDestFileLn("# ADD CPP /O2 /Ob2");
		}
	}
#endif
	WriteDestFileLn("# End Source File");
}

LOCALPROC WriteMSVCSpecificFiles(void)
{
	if (WriteOpenDestFile(&OutputDirR, kStrAppAbbrev, ".dsw")) { /* workspace file */
	WriteDestFileLn("Microsoft Developer Studio Workspace File, Format Version 6.00");
	WriteDestFileLn("# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!");
	WriteBlankLineToDestFile();
	WriteDestFileLn("###############################################################################");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("Project: ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteQuoteToDestFile();
	WriteCStrToDestFile("=.");
	WriteBackSlashToDestFile();
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(".dsp - Package Owner=<4>");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<5>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<4>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn("###############################################################################");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Global:");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<5>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<3>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn("###############################################################################");
	WriteBlankLineToDestFile();

	WriteCloseDestFile();
	}

	if (WriteOpenDestFile(&OutputDirR, kStrAppAbbrev, ".dsp")) { /* project file */

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# Microsoft Developer Studio Project File - Name=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" - Package Owner=<4>");
	WriteEndDestFileLn();

	WriteDestFileLn("# Microsoft Developer Studio Generated Build File, Format Version 6.00");
	WriteDestFileLn("# ** DO NOT EDIT **");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# TARGTYPE ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("Win32 (x86) Application");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" 0x0101");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("CFG=");
	/* WriteAppVariationStr(); */
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(" - Win32");
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE This is not a valid makefile. To build this project using NMAKE,");
	WriteDestFileLn("!MESSAGE use the Export Makefile command and run");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("!MESSAGE NMAKE /f ");
	WriteMSVCMakefileName();
	WriteCStrToDestFile(".");
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteDestFileLn("!MESSAGE You can specify a configuration when running NMAKE");
	WriteDestFileLn("!MESSAGE by defining the macro CFG on the command line. For example:");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("!MESSAGE NMAKE /f ");
	WriteMSVCMakefileName();
	WriteCStrToDestFile(" CFG=");
	WriteMSVCTargetName();
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteDestFileLn("!MESSAGE Possible choices for configuration are:");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("!MESSAGE ");
	WriteMSVCTargetName();
	WriteCStrToDestFile(" (based on ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("Win32 (x86) Application");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(")");
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteBlankLineToDestFile();
	WriteDestFileLn("# Begin Project");
	WriteDestFileLn("# PROP AllowPerConfigDependencies 0");

	WriteMSVCQuotedProp("Scc_ProjName", "");
	WriteMSVCQuotedProp("Scc_LocalPath", "");

	WriteDestFileLn("CPP=cl.exe");
	WriteDestFileLn("MTL=midl.exe");
	WriteDestFileLn("RSC=rc.exe");
	WriteDestFileLn("# PROP BASE Use_MFC 0");
	WriteDestFileLn("# PROP BASE Use_Debug_Libraries 1");

	WriteMSVCQuotedProp("BASE Output_Dir", "Debug");
	WriteMSVCQuotedProp("BASE Intermediate_Dir", "Debug");
	WriteMSVCQuotedProp("BASE Target_Dir", "");

	WriteDestFileLn("# PROP Use_MFC 0");
	if (gbo_dbg == gbk_dbg_on) {
		WriteDestFileLn("# PROP Use_Debug_Libraries 1");
	} else {
		WriteDestFileLn("# PROP Use_Debug_Libraries 0");
	}

	WriteMSVCQuotedProp("Output_Dir", "obj");
	WriteMSVCQuotedProp("Intermediate_Dir", "obj");

	WriteDestFileLn("# PROP Ignore_Export_Lib 0");

	WriteMSVCQuotedProp("Target_Dir", "");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od");
	WriteMSVCQuotedDefine("WIN32");
	WriteMSVCQuotedDefine("_DEBUG");
	WriteMSVCQuotedDefine("_WINDOWS");
	WriteMSVCQuotedDefine("_MBCS");
	WriteCStrToDestFile(" /YX /FD /GZ /c");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD CPP ");
	WriteCLexeFlags();
	WriteCStrToDestFile(" /FD");
	WriteEndDestFileLn();

	WriteDestFileLn("# SUBTRACT CPP /YX /Yc /Yu");

	WriteDestFileLn("# ADD BASE MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD MTL /nologo /D ");
	WriteQuoteToDestFile();
	if (gbo_dbg == gbk_dbg_on) {
		WriteCStrToDestFile("_DEBUG");
	} else {
		WriteCStrToDestFile("NDEBUG");
	}
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" /mktyplib203 /win32");
	WriteEndDestFileLn();

	WriteDestFileLn("# ADD BASE RSC /l 0x409 /d \"_DEBUG\"");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD RSC ");
	WriteRCexeFlags();
	WriteEndDestFileLn();

	WriteDestFileLn("BSC32=bscmake.exe");
	WriteDestFileLn("# ADD BASE BSC32 /nologo");
	WriteDestFileLn("# ADD BSC32 /nologo");
	WriteDestFileLn("LINK32=link.exe");
	WriteDestFileLn("# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /subsystem:windows");
	if (gbo_dbg == gbk_dbg_on) {
		WriteCStrToDestFile(" /debug");
	} else {
		WriteCStrToDestFile(" /incremental:no");
	}
	WriteCStrToDestFile(" /machine:I386 /out:");
	WriteQuoteToDestFile();
	WriteWinAppNameStr();
	WriteQuoteToDestFile();
	if (gbo_dbg == gbk_dbg_on) {
		WriteCStrToDestFile(" /pdbtype:sept");
	}
	WriteEndDestFileLn();

	if (gbo_dbg != gbk_dbg_on) {
		WriteDestFileLn("# SUBTRACT LINK32 /debug /pdbtype:<none>");
	}
	WriteDestFileLn("# Begin Target");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# Name ");
	WriteMSVCTargetName();
	WriteEndDestFileLn();

	WriteMSVCBeginGroup("Source Files", "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat");
		DoAllSrcFilesWithSetup(DoSrcFileMSVCAddFile);
		WriteFileToMSVCSource(WriteMainRsrcSrcPath);
	WriteMSVCEndGroup();

	WriteMSVCBeginGroup("Header Files", "h;hpp;hxx;hm;inl");
		WriteFileToMSVCSource(WriteCNFGGLOBPath);
		WriteFileToMSVCSource(WriteCNFGRAPIPath);
	WriteMSVCEndGroup();

	WriteMSVCBeginGroup("Resource Files", "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe");
		DoAllDocTypesWithSetup(WriteDocTypeMSVCresource);
	WriteMSVCEndGroup();

	WriteDestFileLn("# End Target");
	WriteDestFileLn("# End Project");

	WriteCloseDestFile();
	}
}

LOCALPROC WriteXMLtaggedLines(char *s, MyProc p)
{
	WriteBeginXMLtagLine(s);
		if (NULL != p) {
			p();
		}
	WriteEndXMLtagLine(s);
}

LOCALPROC WriteXMLtaglinewithprops(char *s, MyProc p)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<");
	WriteCStrToDestFile(s);
	WriteEndDestFileLn();
	++DestFileIndent;
		if (NULL != p) {
			p();
		}
	--DestFileIndent;
	WriteDestFileLn("/>");
}

LOCALPROC WriteXMLtaggedLinesWithProps(char *s, MyProc pp, MyProc p)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<");
	WriteCStrToDestFile(s);
	WriteEndDestFileLn();
	++DestFileIndent;
		if (NULL != pp) {
			pp();
		}
		WriteDestFileLn(">");
		if (NULL != p) {
			p();
		}
	WriteEndXMLtagLine(s);
}

LOCALPROC WriteXMLQuotedProp(char *s, MyProc p)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteCStrToDestFile("=");
	WriteQuoteToDestFile();
	p();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC WriteMSVCXMLPlatformName(void)
{
	WriteCStrToDestFile("Win32");
}

LOCALPROC WriteMSVCXMLPlatformProps(void)
{
	WriteXMLQuotedProp("Name", WriteMSVCXMLPlatformName);
}

LOCALPROC WriteMSVCXMLPlatforms(void)
{
	WriteXMLtaglinewithprops("Platform", WriteMSVCXMLPlatformProps);
}

LOCALPROC WriteMSVCXMLConfigurationName(void)
{
	char *s;

	switch (gbo_dbg) {
		case gbk_dbg_on:
			s = "Debug";
			break;
		case gbk_dbg_test:
			s = "Test";
			break;
		case gbk_dbg_off:
			s = "Release";
			break;
		default:
			s = "(unknown Debug Level)";
			break;
	}

	WriteCStrToDestFile(s);
	WriteCStrToDestFile("|");
	WriteMSVCXMLPlatformName();
}

LOCALPROC WriteMSVCXMLConfigurationProps(void)
{
	WriteXMLQuotedProp("Name", WriteMSVCXMLConfigurationName);
	WriteXMLQuotedProp("OutputDirectory", Write_obj_d_ToDestFile);
	WriteXMLQuotedProp("IntermediateDirectory", Write_obj_d_ToDestFile);
	WriteDestFileLn("ConfigurationType=\"1\"");
	WriteDestFileLn("CharacterSet=\"2\"");
	if (gbo_dbg != gbk_dbg_on) {
		WriteDestFileLn("WholeProgramOptimization=\"0\"");
	}
}

LOCALPROC WriteMSVCToolConfig(char *s, MyProc p)
{
	WriteDestFileLn("<Tool");
	++DestFileIndent;
		WriteBgnDestFileLn();
		WriteCStrToDestFile("Name=");
		WriteQuoteToDestFile();
		WriteCStrToDestFile(s);
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
		if (NULL != p) {
			p();
		}
	--DestFileIndent;
	WriteDestFileLn("/>");
}

LOCALPROC WriteMSVCCompilerToolConfig(void)
{
	if (gbo_dbg == gbk_dbg_on) {
		WriteDestFileLn("Optimization=\"0\"");
		WriteDestFileLn("PreprocessorDefinitions=\"WIN32;_DEBUG;_WINDOWS\"");
		WriteDestFileLn("MinimalRebuild=\"true\"");
	} else {
		WriteDestFileLn("Optimization=\"1\"");
		WriteDestFileLn("FavorSizeOrSpeed=\"0\"");
		WriteDestFileLn("WholeProgramOptimization=\"false\"");
		WriteDestFileLn("PreprocessorDefinitions=\"WIN32;NDEBUG;_WINDOWS\"");
		WriteDestFileLn("StringPooling=\"true\"");
	}
	WriteDestFileLn("ExceptionHandling=\"0\"");
	if (gbo_dbg == gbk_dbg_on) {
		WriteDestFileLn("BasicRuntimeChecks=\"3\"");
		WriteDestFileLn("RuntimeLibrary=\"1\"");
	} else {
		WriteDestFileLn("RuntimeLibrary=\"0\"");
	}
	WriteDestFileLn("UsePrecompiledHeader=\"0\"");
	WriteDestFileLn("WarningLevel=\"4\"");
	WriteDestFileLn("Detect64BitPortabilityProblems=\"true\"");
	if (gbo_dbg == gbk_dbg_on) {
		WriteDestFileLn("DebugInformationFormat=\"4\"");
	} else {
		WriteDestFileLn("DebugInformationFormat=\"0\"");
	}
	WriteDestFileLn("CompileAs=\"0\"");
}

LOCALPROC WriteMSVCLinkerToolConfig(void)
{
	WriteDestFileLn("AdditionalDependencies=\"winmm.lib\"");
	WriteXMLQuotedProp("OutputFile", WriteWinAppNamePath);
	if (gbo_dbg == gbk_dbg_on) {
		WriteDestFileLn("LinkIncremental=\"2\"");
	} else {
		WriteDestFileLn("LinkIncremental=\"1\"");
	}
	WriteDestFileLn("GenerateManifest=\"false\"");
	if (gbo_dbg == gbk_dbg_on) {
		WriteDestFileLn("GenerateDebugInformation=\"true\"");
	} else {
		WriteDestFileLn("GenerateDebugInformation=\"false\"");
	}
	WriteDestFileLn("SubSystem=\"2\"");
	if (gbo_dbg != gbk_dbg_on) {
		WriteDestFileLn("OptimizeReferences=\"2\"");
		WriteDestFileLn("EnableCOMDATFolding=\"2\"");
	}
	WriteDestFileLn("TargetMachine=\"1\"");
}

LOCALPROC WriteMSVCXMLConfigurationBody(void)
{
	WriteMSVCToolConfig("VCPreBuildEventTool", NULL);
	WriteMSVCToolConfig("VCCustomBuildTool", NULL);
	WriteMSVCToolConfig("VCXMLDataGeneratorTool", NULL);
	WriteMSVCToolConfig("VCWebServiceProxyGeneratorTool", NULL);
	WriteMSVCToolConfig("VCMIDLTool", NULL);
	WriteMSVCToolConfig("VCCLCompilerTool", WriteMSVCCompilerToolConfig);
	WriteMSVCToolConfig("VCManagedResourceCompilerTool", NULL);
	WriteMSVCToolConfig("VCResourceCompilerTool", NULL);
	WriteMSVCToolConfig("VCPreLinkEventTool", NULL);
	WriteMSVCToolConfig("VCLinkerTool", WriteMSVCLinkerToolConfig);
	WriteMSVCToolConfig("VCALinkTool", NULL);
	WriteMSVCToolConfig("VCManifestTool", NULL);
	WriteMSVCToolConfig("VCXDCMakeTool", NULL);
	WriteMSVCToolConfig("VCBscMakeTool", NULL);
	WriteMSVCToolConfig("VCFxCopTool", NULL);
	WriteMSVCToolConfig("VCAppVerifierTool", NULL);
	WriteMSVCToolConfig("VCWebDeploymentTool", NULL);
	WriteMSVCToolConfig("VCPostBuildEventTool", NULL);
}

LOCALPROC WriteMSVCXMLConfigurations(void)
{
	WriteXMLtaggedLinesWithProps("Configuration",
		WriteMSVCXMLConfigurationProps,
		WriteMSVCXMLConfigurationBody);
}

LOCALPROC WriteMSVCXMLSourceFilesProps(void)
{
	WriteDestFileLn("Name=\"Source Files\"");
	WriteDestFileLn("Filter=\"cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx\"");
	WriteDestFileLn("UniqueIdentifier=\"{00020000-0000-0000-0000-000000000000}\"");
}

LOCALPROC DoMSVCXMLAddFile(MyProc p)
{
	WriteDestFileLn("<File");
	++DestFileIndent;
		WriteXMLQuotedProp("RelativePath", p);
		WriteDestFileLn(">");
	--DestFileIndent;
	WriteDestFileLn("</File>");
}

LOCALPROC DoSrcFileMSVCXMLAddFile(void)
{
	DoMSVCXMLAddFile(WriteSrcFileFilePath);
}

LOCALPROC WriteMSVCXMLSourceFilesBody(void)
{
	DoAllSrcFilesWithSetup(DoSrcFileMSVCXMLAddFile);
}

LOCALPROC WriteMSVCXMLHeaderFilesProps(void)
{
	WriteDestFileLn("Name=\"Header Files\"");
	WriteDestFileLn("Filter=\"h;hpp;hxx;hm;inl;inc;xsd\"");
	WriteDestFileLn("UniqueIdentifier=\"{00030000-0000-0000-0000-000000000000}\"");
}

LOCALPROC DoSrcFileMSVCXMLAddHeader(void)
{
	DoMSVCXMLAddFile(WriteSrcFileHeaderPath);
}

LOCALPROC WriteMSVCXMLHeaderFilesBody(void)
{
	DoAllSrcFilesWithSetup(DoSrcFileMSVCXMLAddHeader);
}

LOCALPROC WriteMSVCXMLResourceFilesProps(void)
{
	WriteDestFileLn("Name=\"Resource Files\"");
	WriteDestFileLn("Filter=\"rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav\"");
	WriteDestFileLn("UniqueIdentifier=\"{00040000-0000-0000-0000-000000000000}\"");
}

LOCALPROC WriteDocTypeMSVCXMLresource(void)
{
	DoMSVCXMLAddFile(WriteDocTypeIconFilePath);
}

LOCALPROC WriteMSVCXMLResourceFilesBody(void)
{
	DoMSVCXMLAddFile(WriteMainRsrcSrcPath);
	DoAllDocTypesWithSetup(WriteDocTypeMSVCXMLresource);
}

LOCALPROC WriteMSVCXMLIncludeFilesProps(void)
{
	WriteDestFileLn("Name=\"Include Files\"");
	WriteDestFileLn("Filter=\"h;hpp;hxx;hm;inl;inc;xsd\"");
}

LOCALPROC DoMSVCXMLAddAddExtraHeader(void)
{
	DoMSVCXMLAddFile(WriteExtraHeaderFilePath);
}

LOCALPROC WriteMSVCXMLIncludeFilesBody(void)
{
	DoAllExtraHeaders2WithSetup(DoMSVCXMLAddAddExtraHeader);
}

LOCALPROC WriteMSVCXMLFiles(void)
{
	WriteXMLtaggedLinesWithProps("Filter",
		WriteMSVCXMLSourceFilesProps,
		WriteMSVCXMLSourceFilesBody);
	WriteXMLtaggedLinesWithProps("Filter",
		WriteMSVCXMLHeaderFilesProps,
		WriteMSVCXMLHeaderFilesBody);
	WriteXMLtaggedLinesWithProps("Filter",
		WriteMSVCXMLResourceFilesProps,
		WriteMSVCXMLResourceFilesBody);
	WriteXMLtaggedLinesWithProps("Filter",
		WriteMSVCXMLIncludeFilesProps,
		WriteMSVCXMLIncludeFilesBody);
}

LOCALPROC WriteMSVCXMLProjectProps(void)
{
	WriteDestFileLn("ProjectType=\"Visual C++\"");
	WriteDestFileLn("Version=\"8.00\"");
	WriteXMLQuotedProp("Name", WriteStrAppAbbrev);
	WriteDestFileLn("ProjectGUID=\"{00010000-0000-0000-0000-000000000000}\"");
	WriteXMLQuotedProp("RootNamespace", WriteStrAppAbbrev);
	WriteDestFileLn("Keyword=\"Win32Proj\"");
}

LOCALPROC WriteMSVCXMLProjectBody(void)
{
	WriteXMLtaggedLines("Platforms", WriteMSVCXMLPlatforms);
	WriteXMLtaggedLines("ToolFiles", NULL);
	WriteXMLtaggedLines("Configurations", WriteMSVCXMLConfigurations);
	WriteXMLtaggedLines("References", NULL);
	WriteXMLtaggedLines("Files", WriteMSVCXMLFiles);
	WriteXMLtaggedLines("Globals", NULL);
}

LOCALPROC WriteMSVCXMLSpecificFiles(void)
{
	if (WriteOpenDestFile(&OutputDirR, kStrAppAbbrev, ".sln")) {
	WriteDestFileLn("\357\273\277"); /* UTF-8 byte-order mark */
	WriteDestFileLn("Microsoft Visual Studio Solution File, Format Version 9.00");
	WriteDestFileLn("# Visual C++ Express 2005");
	WriteBgnDestFileLn();
	WriteCStrToDestFile("Project(\"{00000000-0000-0000-0000-000000000000}\") = \"");
	WriteStrAppAbbrev();
	WriteCStrToDestFile("\", \"");
	WriteStrAppAbbrev();
	WriteCStrToDestFile(".vcproj\", \"{00010000-0000-0000-0000-000000000000}\"");
	WriteEndDestFileLn();
	WriteDestFileLn("EndProject");
	WriteDestFileLn("Global");
	++DestFileIndent;
		WriteDestFileLn("GlobalSection(SolutionConfigurationPlatforms) = preSolution");
		++DestFileIndent;
			WriteBgnDestFileLn();
			WriteMSVCXMLConfigurationName();
			WriteCStrToDestFile(" = ");
			WriteMSVCXMLConfigurationName();
			WriteEndDestFileLn();
		--DestFileIndent;
		WriteDestFileLn("EndGlobalSection");
		WriteDestFileLn("GlobalSection(ProjectConfigurationPlatforms) = postSolution");
		++DestFileIndent;
			WriteBgnDestFileLn();
			WriteCStrToDestFile("{00010000-0000-0000-0000-000000000000}.");
			WriteMSVCXMLConfigurationName();
			WriteCStrToDestFile(".ActiveCfg = ");
			WriteMSVCXMLConfigurationName();
			WriteEndDestFileLn();
			WriteBgnDestFileLn();
			WriteCStrToDestFile("{00010000-0000-0000-0000-000000000000}.");
			WriteMSVCXMLConfigurationName();
			WriteCStrToDestFile(".Build.0 = ");
			WriteMSVCXMLConfigurationName();
			WriteEndDestFileLn();
		--DestFileIndent;
		WriteDestFileLn("EndGlobalSection");
		WriteDestFileLn("GlobalSection(SolutionProperties) = preSolution");
		++DestFileIndent;
			WriteDestFileLn("HideSolutionNode = FALSE");
		--DestFileIndent;
		WriteDestFileLn("EndGlobalSection");
	--DestFileIndent;
	WriteDestFileLn("EndGlobal");

	WriteCloseDestFile();
	}

	if (WriteOpenDestFile(&OutputDirR, kStrAppAbbrev, ".vcproj")) {
		WriteDestFileLn("<?xml version=\"1.0\" encoding=\"Windows-1252\"?>");
		WriteXMLtaggedLinesWithProps("VisualStudioProject",
			WriteMSVCXMLProjectProps,
			WriteMSVCXMLProjectBody);
		WriteCloseDestFile();
	}
}

LOCALPROC DoSrcFileNMakeAddSrcFileBody(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("$(mk_CallC) \"");
	WriteSrcFileFilePath();
	WriteCStrToDestFile("\" $(mk_COptions)");
	WriteEndDestFileLn();
}

LOCALPROC DoSrcFileNMakeAddSrcFile(void)
{
	WriteMakeRule(WriteSrcFileObjPath,
		DoSrcFileMakeCompileDeps,
		DoSrcFileNMakeAddSrcFileBody);
}

LOCALPROC DoSrcFileNMakeAddObjFile(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("\"");
	WriteSrcFileObjPath();
	WriteCStrToDestFile("\" \\");
	WriteEndDestFileLn();
}

LOCALPROC DoSrcFileNMakeEraseFile(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("-@erase \"");
	WriteSrcFileObjPath();
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();
}

LOCALPROC WriteMainRsrcObjMSCdeps(void)
{
	WriteMakeDependFile(WriteMainRsrcSrcPath);
}

LOCALPROC WriteMainRsrcObjMSCbuild(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("rc.exe ");
	WriteRCexeFlags();
	WriteCStrToDestFile(" /fo\"");
	WriteMainRsrcObjPath();
	WriteCStrToDestFile("\" \"");
	WriteMainRsrcSrcPath();
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();
}

LOCALPROC WriteNMakeSpecificFiles(void)
{
	if (WriteOpenDestFile(&OutputDirR, "MAKEFILE", "")) { /* Make file */

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# Microsoft Developer Studio Generated NMAKE File, Based on ");
	WriteCStrToDestFile(kStrAppAbbrev);
	WriteCStrToDestFile(".dsp");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteDestFileLn("mk_CallC=cl.exe");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptions=");
	WriteCLexeFlags();
	WriteCStrToDestFile(" /Fo\"");
	Write_obj_d_ToDestFile();
	WriteCStrToDestFile("\\\\\" /Fd\"");
	Write_obj_d_ToDestFile();
	WriteCStrToDestFile("\\\\\"");
		/* yes, a double backslash is what is needed */
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("TheApplication : \"");
	WriteWinAppNamePath();
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	DoAllSrcFilesWithSetup(DoSrcFileNMakeAddSrcFile);
	WriteBlankLineToDestFile();
	WriteDestFileLn("ObjFiles= \\");
	++DestFileIndent;
		DoAllSrcFilesWithSetup(DoSrcFileNMakeAddObjFile);
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteMakeRule(WriteMainRsrcObjPath,
		WriteMainRsrcObjMSCdeps, WriteMainRsrcObjMSCbuild);
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("\"");
	WriteWinAppNamePath();
	WriteCStrToDestFile("\" : $(ObjFiles) \"");
	WriteMainRsrcObjPath();
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();
	++DestFileIndent;
		WriteDestFileLn("link.exe @<<");
		++DestFileIndent;
			WriteBgnDestFileLn();
			WriteCStrToDestFile("/out:\"");
			WriteWinAppNamePath();
			WriteCStrToDestFile("\"");
			WriteEndDestFileLn();
			WriteDestFileLn("/nologo /subsystem:windows /incremental:no");
			WriteBgnDestFileLn();
			WriteCStrToDestFile("/pdb:\"");
			Write_obj_d_ToDestFile();
			WriteCStrToDestFile("\\");
			WriteCStrToDestFile(kStrAppAbbrev);
			WriteCStrToDestFile(".pdb\" /machine:I386");
			WriteEndDestFileLn();
			WriteDestFileLn("kernel32.lib");
			WriteDestFileLn("user32.lib");
			WriteDestFileLn("gdi32.lib");
			WriteDestFileLn("winspool.lib");
			WriteDestFileLn("comdlg32.lib");
			WriteDestFileLn("advapi32.lib");
			WriteDestFileLn("shell32.lib");
			WriteDestFileLn("winmm.lib");
			WriteDestFileLn("ole32.lib");
			WriteDestFileLn("oleaut32.lib");
			WriteDestFileLn("uuid.lib");
			WriteBgnDestFileLn();
			WriteCStrToDestFile("\"");
			WriteMainRsrcObjPath();
			WriteCStrToDestFile("\"");
			WriteEndDestFileLn();
			WriteDestFileLn("$(ObjFiles)");
		--DestFileIndent;
	--DestFileIndent;
	WriteDestFileLn("<<");
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteDestFileLn("CLEAN :");
	++DestFileIndent;
		DoAllSrcFilesWithSetup(DoSrcFileNMakeEraseFile);
		WriteBgnDestFileLn();
		WriteCStrToDestFile("-@erase \"");
		WriteMainRsrcObjPath();
		WriteCStrToDestFile("\"");
		WriteEndDestFileLn();
		WriteBgnDestFileLn();
		WriteCStrToDestFile("-@erase \"");
		WriteWinAppNamePath();
		WriteCStrToDestFile("\"");
		WriteEndDestFileLn();
	--DestFileIndent;

	WriteCloseDestFile();
	}
}
