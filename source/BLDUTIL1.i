/*
	BLDUTIL1.i
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
	BuiLD system UTILities part 1
*/

LOCALFUNC tMyErr ExportFromFile2HostFile_v2(short refNum,
	ui5r L, tPbuf Name)
/* takes ownership of Name */
{
	tMyErr err;
	tDrive InsertVol;

	if (noErr == (err = ProgressBar_SetStage_v2(
		"Making the new file\311", 0)))
	if (noErr == (err = HostVolInsertNew_v2(L, Name, &InsertVol))) {
		if (noErr == (err = ProgressBar_SetStage_v2(
			"Exporting, type command-period to abort\311", L)))
		{
			err = WriteFromFileToVol_v2(refNum, InsertVol, 0, L);
		}
		if (noErr == err) {
			err = DiskEject_v2(InsertVol);
		} else {
			DiskEjectDelete_v2(InsertVol);
				/* ignore any error, since already got one */
		}
	}

	return err;
}

GLOBALFUNC tMyErr ExportFromFile2HostFile2_v2(MyDir_R *d, StringPtr s)
{
	tMyErr err;
	short refnum;
	uimr L;
	tPbuf Name;

	if (noErr == (err = MyOpenOldFileRead_v2(d, s, &refnum))) {
		if (noErr == (err = MyOpenFileGetEOF_v2(refnum, &L)))
		if (noErr == (err = PStr2Pbuf_v2(s, &Name)))
		if (noErr == (err = ExportFromFile2HostFile_v2(refnum,
			L, Name)))
		{
		}
		err = CombineErr(err, MyCloseFile_v2(refnum));
	}

	return err;
}

GLOBALPROC Uimr2Dec(uimr v, ps3p s)
{
	MyCharR *p = PStrToMyCharPtr(s);
	uimr newv;

	do {
		newv = v / (uimr)10;
		*p++ = '0' + (v - newv * 10);
		v = newv;
	} while (v != 0);
	s[0] = p - PStrToMyCharPtr(s);

	ReversePStr(s);
}

GLOBALPROC ui2r2Hex(ui3r v, MyCharR *r)
{
	if (v < 10) {
		*r = '0' + v;
	} else {
		*r = 'A' + v - 10;
	}
}

GLOBALPROC ui3r2Hex(ui3r v, MyCharR *r)
{
	ui2r2Hex((v     ) & 0x0F, r + 1);
	ui2r2Hex((v >> 4) & 0x0F, r);
}

GLOBALPROC ui4r2Hex(ui4r v, MyCharR *r)
{
	ui3r2Hex((v     ) & 0xFF, r + 2);
	ui3r2Hex((v >> 8) & 0xFF, r);
}

GLOBALPROC ui5r2Hex(ui5r v, MyCharR *r)
{
	ui4r2Hex((v      ) & 0xFFFF, r + 4);
	ui4r2Hex((v >> 16) & 0xFFFF, r);
}

LOCALFUNC tMyErr MyDeleteOneIfExists_v2(MyDir_R *d, StringPtr s)
{
	tMyErr err;
	blnr Exists;

	if (noErr == (err = MyFileExists_v2(d, s, &Exists))) {
		if (Exists) {
			err = dellib_DeleteOne_v2(d, s);
		}
	}

	return err;
}

LOCALFUNC tMyErr DelMakeNamedDir_v2(MyDir_R *new_d, MyDir_R *d,
	StringPtr s)
{
	tMyErr err;

	if (noErr == (err = MyDeleteOneIfExists_v2(d, s))) {
		err = MyMakeNamedDir_v2(d, s, new_d);
	}

	return err;
}

LOCALFUNC tMyErr FindOrMakeMakeNamedDir_v2(MyDir_R *new_d,
	MyDir_R *d, StringPtr s)
{
	tMyErr err;

	err = MyResolveNamedChildDir_v2(d, s, new_d);
	if (fnfErr == err) {
		err = MyMakeNamedDir_v2(d, s, new_d);
	}

	return err;
}

LOCALFUNC tMyErr MakeSubDirectory_v2(MyDir_R *new_d, MyDir_R *d,
	char *name, char *FileExt)
{
	Str255 s;

	PStrFromCStr(s, name);
	PStrApndCStr(s, FileExt);
	return MyMakeNamedDir_v2(d, s, new_d);
}

LOCALFUNC tMyErr DelMakeSubDirectory_v2(MyDir_R *new_d,
	MyDir_R *d, char *name, char *FileExt)
{
	Str255 s;

	PStrFromCStr(s, name);
	PStrApndCStr(s, FileExt);

	return DelMakeNamedDir_v2(new_d, d, s);
}

LOCALFUNC tMyErr FindOrMakeSubDirectory_v2(MyDir_R *new_d,
	MyDir_R *d, char *name, char *FileExt)
{
	Str255 s;

	PStrFromCStr(s, name);
	PStrApndCStr(s, FileExt);

	return FindOrMakeMakeNamedDir_v2(new_d, d, s);
}

LOCALFUNC tMyErr FindSubDirectory_v2(MyDir_R *new_d,
	MyDir_R *d, char *name, char *FileExt)
{
	Str255 s;

	PStrFromCStr(s, name);
	PStrApndCStr(s, FileExt);
	return MyResolveNamedChildDir_v2(d, s, new_d);
}

TYPEDEFPROC (*MyProc)(void);
