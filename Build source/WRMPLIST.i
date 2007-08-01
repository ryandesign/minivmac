/*
	WRMPLIST.i
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
	WRite Macintosh PLIST
*/


enum {
	kPListRaw, /* native plist data */
	kPListPLC, /* metrowerks property list compiler */
	kNumPListFormats
};

LOCALVAR int CurPListFormat = kPListPLC;

LOCALPROC WritePListProcString(MyProc p)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginProcValEndLine("string", p);
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("string ");
		WriteQuoteToDestFile();
		p();
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
	}
}

LOCALPROC WritePListString(char *s)
{
	MyPtr SavepDt = pDt;

	pDt = (MyPtr)s;
	WritePListProcString(WritepDtString);
	pDt = SavepDt;
}

LOCALPROC WritePListKeyProcString(char *k, MyProc p)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginValEndLine("key", k);
		WriteXMLtagBeginProcValEndLine("string", p);
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("key ");
		WriteQuoteToDestFile();
		WriteCStrToDestFile(k);
		WriteQuoteToDestFile();
		WriteCStrToDestFile(" value string ");
		WriteQuoteToDestFile();
		p();
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
	}
}

LOCALPROC WritePListKeyString(char *k, char *s)
{
	MyPtr SavepDt = pDt;

	pDt = (MyPtr)s;
	WritePListKeyProcString(k, WritepDtString);
	pDt = SavepDt;
}

LOCALPROC WritePListBeginKeyArray(char *k)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginValEndLine("key", k);
		WriteBeginXMLtagLine("array");
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("key ");
		WriteQuoteToDestFile();
		WriteCStrToDestFile(k);
		WriteQuoteToDestFile();
		WriteCStrToDestFile(" value array");
		WriteEndDestFileLn();
		WriteDestFileLn("[");
		++DestFileIndent;
	}
}

LOCALPROC WritePListEndKeyArray(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteEndXMLtagLine("array");
	} else {
		--DestFileIndent;
		WriteDestFileLn("]");
	}
}

LOCALPROC WritePListBeginDict(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteBeginXMLtagLine("dict");
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("dictionary");
		WriteEndDestFileLn();
		WriteDestFileLn("{");
		++DestFileIndent;
	}
}

LOCALPROC WritePListEndDict(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteEndXMLtagLine("dict");
	} else {
		--DestFileIndent;
		WriteDestFileLn("}");
	}
}

LOCALPROC WriteInfoPList(MyProc p)
{
	CurPListFormat = kPListRaw;

	WriteDestFileLn("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
#if 0
	WriteDestFileLn("<!DOCTYPE plist SYSTEM \"file://localhost/System/Library/DTDs/PropertyList.dtd\">");
	WriteDestFileLn("<plist version=\"0.9\">");
#else
	WriteDestFileLn("<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">");
	WriteDestFileLn("<plist version=\"1.0\">");
#endif

	p();

	WriteDestFileLn("</plist>");
}

LOCALPROC WriteMainPLC(MyProc p)
{
	CurPListFormat = kPListPLC;

	WriteDestFileLn("plist");
	WriteDestFileLn("{");
	++DestFileIndent;

	p();

	--DestFileIndent;
	WriteDestFileLn("}");
}

LOCALPROC WritepDtIconTypeName(void)
{
	WriteCStrToDestFile(kStrAppName);
	WriteCStrToDestFile(" ");
	WriteCStrToDestFile(DoDocType_gd()->LongName);
}

LOCALPROC WriteOneExtension2Array(char *s)
{
	WritePListString(s);
}

LOCALPROC WriteOneCFBundleDocumentType(void)
{
	if (0 != DocTypeCounter) {
		WritePListBeginDict();
			if (DoDocType_gd()->WriteExtensionList != nullpr) {
				WritePListBeginKeyArray("CFBundleTypeExtensions");
					DoDocType_gd()->WriteExtensionList(WriteOneExtension2Array);
				WritePListEndKeyArray();
			}
			WritePListKeyProcString("CFBundleTypeIconFile", WriteDocTypeIconFileName);
			WritePListKeyProcString("CFBundleTypeName", WritepDtIconTypeName);
			WritePListBeginKeyArray("CFBundleTypeOSTypes");
				WritePListProcString(WriteDocTypeIconMacType);
			WritePListEndKeyArray();
			WritePListKeyString("CFBundleTypeRole", "Editor");
		WritePListEndDict();
	}
}

LOCALPROC WriteMyInfoPListContents(void)
{
	WritePListBeginDict();

		/* in order preferred by latest xcode (alphabetical) */

		WritePListKeyString("CFBundleDevelopmentRegion", "English");

		WritePListBeginKeyArray("CFBundleDocumentTypes");
			WritePListBeginDict();
				WritePListBeginKeyArray("CFBundleTypeOSTypes");
					WritePListString("****");
				WritePListEndKeyArray();
				WritePListKeyString("CFBundleTypeRole", "Editor");
			WritePListEndDict();
			DoAllDocTypesWithSetup(WriteOneCFBundleDocumentType);
		WritePListEndKeyArray();

		WritePListKeyString("CFBundleExecutable", kStrAppAbbrev);
		WritePListKeyProcString("CFBundleGetInfoString", WriteGetInfoString);
		WritePListKeyString("CFBundleIconFile", "AppIcon.icns");
		WritePListKeyString("CFBundleIdentifier", kBundleIdentifier);
		WritePListKeyString("CFBundleInfoDictionaryVersion", "6.0");
		WritePListKeyString("CFBundleName", kStrAppName);
		WritePListKeyString("CFBundlePackageType", "APPL");
		WritePListKeyProcString("CFBundleShortVersionString", WriteVersionStr);
		WritePListKeyString("CFBundleSignature", kMacCreatorSig);
		WritePListKeyProcString("CFBundleVersion", WriteVersionStr);
		WritePListKeyString("LSRequiresCarbon", "true/");

	WritePListEndDict();
}

LOCALPROC WritePListData(void)
{
#if support_ide_mw8
	if (cur_ide == gbk_ide_mw8) {
		if (WriteOpenDestFile(&SrcDirR, "main", ".plc")) { /* plist source */
		WriteMainPLC(WriteMyInfoPListContents);
		WriteCloseDestFile();
		}
	} else
#endif
	{
		if (WriteOpenDestFile(&SrcDirR, "Info", ".plist")) { /* Info.plist file */
		WriteInfoPList(WriteMyInfoPListContents);
		WriteCloseDestFile();
		}
	}
}
