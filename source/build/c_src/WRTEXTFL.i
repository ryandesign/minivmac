/*
	WRTEXTFL.i
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
	WRite TEXT FiLe
*/



/* --- routines for writing text files --- */

LOCALVAR short DestFileRefNum;

LOCALFUNC blnr WriteOpenDestFile(MyDir_R *d, char *FileName, char *FileExt)
{
	Str255 s;

	PStrFromCStr(s, FileName);
	PStrApndCStr(s, FileExt);

	return MyOpenNewFile(d, s,
		'MPS ', 'TEXT', &DestFileRefNum);
}

LOCALPROC WriteCloseDestFile(void)
{
	(void) MyCloseFile(DestFileRefNum);
}

LOCALPROC MyWriteChar(char x)
{
	(void) MyWriteBytes(DestFileRefNum, (MyPtr)&x, 1);
}

LOCALPROC WriteCStrToDestFile(char *s)
{
	(void) MyWriteBytes(DestFileRefNum, (MyPtr)s, CStrLength(s));
}

LOCALPROC WritePStrToDestFile(ps3p s)
{
	(void) MyWriteBytes(DestFileRefNum, PStrToPtr(s), PStrToSize(s));
}

LOCALPROC WriteHandToDestFile(Handle h)
{
	if (NULL != h) {
		HLock(h);
		(void) MyWriteBytes(DestFileRefNum, (MyPtr)*h, GetHandleSize(h));
		HUnlock(h);
	}
}

LOCALPROC MyWriteEolChar(void)
{
	switch (cur_eol) {
		case gbk_eol_mac:
			MyWriteChar(((char)13));
			break;
#if SupportWinIDE
		case gbk_eol_win:
			MyWriteChar(((char)13));
			MyWriteChar(((char)10));
			break;
#endif
		case gbk_eol_unx:
			MyWriteChar(((char)10));
			break;
	}
}

LOCALPROC WriteBlankLineToDestFile(void)
{
	MyWriteEolChar();
}

LOCALVAR int DestFileIndent = 0;

LOCALPROC WriteBgnDestFileLn(void)
{
	int i;

	for (i = 0; i < DestFileIndent; ++i) {
		MyWriteChar('\t');
	}
}

LOCALPROC WriteEndDestFileLn(void)
{
	MyWriteEolChar();
}



/* ------- utilities for writing to text files -------- */

LOCALPROC WriteUnsignedToOutput(uimr v)
{
	MyPStr s;

	Uimr2Dec(v, s);
	WritePStrToDestFile(s);
#if 0
	char s[16];

	sprintf(s, "%u", v);
	WriteCStrToDestFile(s);
#endif
}

LOCALPROC WriteHexWordToOutput(unsigned int v)
{
	MyPStr s;

	s[0] = 4;
	ui4r2Hex(v, PStrToMyCharPtr(s));
	WritePStrToDestFile(s);
#if 0
	char s[16];

	sprintf(s, "%04X", v);
	WriteCStrToDestFile(s);
#endif
}

LOCALPROC WriteHexLongToOutput(ui5r v)
{
	MyPStr s;

	s[0] = 8;
	ui5r2Hex(v, PStrToMyCharPtr(s));
	WritePStrToDestFile(s);
#if 0
	char s[16];

	sprintf(s, "%08X", v);
	WriteCStrToDestFile(s);
#endif
}

LOCALPROC WriteDestFileLn(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteEndDestFileLn();
}

LOCALPROC WriteQuoteToDestFile(void)
{
	WriteCStrToDestFile("\"");
}

LOCALPROC WriteSingleQuoteToDestFile(void)
{
	WriteCStrToDestFile("'");
}

LOCALPROC WriteBackSlashToDestFile(void)
{
	WriteCStrToDestFile("\\");
}
