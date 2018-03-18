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

LOCALFUNC tMyErr WriteADestFile(MyDir_R *d,
	char *FileName, char *FileExt, MyProc p)
{
	tMyErr err;
	Str255 s;

	PStrFromCStr(s, FileName);
	PStrApndCStr(s, FileExt);

	if (noErr == (err = strmo_open_v2(d, s))) {
		p();
		err = strmo_close_v2();
	}

	return err;
}

GLOBALPROC vCheckWriteDestErr(tMyErr theErr)
{
	if (noErr != theErr) {
		if (noErr == strmo_err) {
#if 0
			Debugger();
#endif
			strmo_err = theErr;
		}
	}
}

#define WriteCStrToDestFile strmo_writeCStr

LOCALPROC WriteHandToDestFile(Handle h)
{
	if (NULL != h) {
		HLock(h);
		strmo_write((MyPtr)*h, GetHandleSize(h));
		HUnlock(h);
	}
}

LOCALPROC MyWriteEolChar(void)
{
	switch (cur_eol) {
		case gbk_eol_mac:
			strmo_writeChar(((char)13));
			break;
		case gbk_eol_win:
			strmo_writeChar(((char)13));
			strmo_writeChar(((char)10));
			break;
		case gbk_eol_unx:
			strmo_writeChar(((char)10));
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
		strmo_writeChar('\t');
	}
}

LOCALPROC WriteEndDestFileLn(void)
{
	MyWriteEolChar();
}



/* ------- utilities for writing to text files -------- */

#define WriteUnsignedToOutput strmo_writeUimr
#if 0
LOCALPROC WriteUnsignedToOutput(uimr v)
{
	char s[16];

	sprintf(s, "%u", v);
	WriteCStrToDestFile(s);
}
#endif

LOCALPROC WriteHexByteToOutput(unsigned int v)
{
	MyPStr s;

	s[0] = 2;
	ui3r2Hex(v, PStrToMyCharPtr(s));
	strmo_writePStr(s);
}

LOCALPROC WriteHexWordToOutput(unsigned int v)
{
	MyPStr s;

	s[0] = 4;
	ui4r2Hex(v, PStrToMyCharPtr(s));
	strmo_writePStr(s);
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
	strmo_writePStr(s);
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
