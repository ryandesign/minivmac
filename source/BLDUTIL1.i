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

LOCALFUNC tMyErr CStr2Pbuf_v2(char *s, tPbuf *r)
{
	tMyErr err;
	tPbuf v;
	ui5r L = CStrLength(s);

	if (noErr == (err = PbufNew_v2(L, &v))) {
		err = PbufTransfer_v2(s, v, 0, L, trueblnr);
		if (noErr == err) {
			*r = v;
		} else {
			(void) PbufDispose_v2(v);
				/* ignore any error, since already got one */
		}
	}

	return err;
}

LOCALFUNC tMyErr PStr2Pbuf_v2(ps3p s, tPbuf *r)
{
	tMyErr err;
	tPbuf v;
	ui5r L = PStrLength(s);

	if (noErr == (err = PbufNew_v2(L, &v))) {
		err = PbufTransfer_v2(PStrToPtr(s), v, 0, L, trueblnr);
		if (noErr == err) {
			*r = v;
		} else {
			(void) PbufDispose_v2(v);
				/* ignore any error, since already got one */
		}
	}

	return err;
}

LOCALFUNC tMyErr WriteFromFileToVolUsingBuff_v2(short refNum,
	tDrive Drive_No, ui5r Sony_Start, ui5r Sony_Count,
	void *buff, ui5r BuffL)
{
	tMyErr err;
	ui5r L = Sony_Count;
	ui5r offset = Sony_Start;

Label_1:
	if (L == 0) {
		err = noErr;
	} else {
		ui5r n = (L > BuffL) ? BuffL : L;

		/* OccasionalSpin(); */

		if (noErr == (err = CheckAbortRequested()))
		if (noErr == (err = MyReadBytes_v2(refNum, buff, n)))
		if (noErr == (err = DiskWrite_v2(buff, Drive_No, offset, &n)))
		{
			L -= n;
			offset += n;

			goto Label_1;
		}
	}

	return err;
}

LOCALFUNC tMyErr WriteFromFileToVol_v2(short refNum,
	tDrive Drive_No, ui5r Sony_Start, ui5r Sony_Count)
{
	tMyErr err;
	Handle h;

	ui5r MaxBuffL = 2 * Sony_Count + 1;
	ui5r Free = FreeMem();
	ui5r BuffL = 64 * (ui5r)1024;

	if (MaxBuffL > Free) {
		MaxBuffL = Free;
	}
	if (MaxBuffL < 512) {
		MaxBuffL = 512;
	}

	while (BuffL > MaxBuffL) {
		BuffL >>= 1;
	}

	if (noErr == (err = MyHandleNew_v2(BuffL, &h))) {
		HLock(h);

		err = WriteFromFileToVolUsingBuff_v2(refNum, Drive_No,
			Sony_Start, Sony_Count, *h, BuffL);

		HUnlock(h);
		DisposeHandle(h);
	}

	return err;
}

LOCALFUNC tMyErr DiskGetRawNew(ui5r L, tPbuf Name, tDrive *Drive_No)
{
	tMyErr err;
	ui5r SaveCallBack;
	ui4r mode;
	ui4r NewWanted;
	tDrive InsertVol;
	blnr InsertGotOne = falseblnr;

	if (noErr == (err = DiskGetCallBack_v2(&SaveCallBack))) {
		if (noErr == (err = DiskSetCallBack_v2(0))) {
			if (noErr == (err = DiskGetRawMode_v2(&mode))) {
				if (noErr == (err = DiskSetRawMode_v2(1))) {
					if (noErr == (err = DiskNew_v2(L, Name))) {
						do {
							if (noErr != (err = DiskGetNewWanted_v2(
								&NewWanted)))
							{
								NewWanted = (ui4b)0;
							} else {
								err = DiskNextPendingInsert(&InsertVol);
							}
						} while ((NewWanted != 0)
							&& (err == (OSErr)0xFFC8)
							&& (noErr ==
								(err = CheckAbortRequested())));

						if (noErr == err) {
							InsertGotOne = trueblnr;
						} else if (AbortRequested || (NewWanted == 0))
						{
							DisplayRunErr("Aborted!");
							err = kMyErrReported;
						}
					}
					Name = NotAPbuf;
				}
				err = CombineErr(err, DiskSetRawMode_v2(mode));
			}
		}
		err = CombineErr(err, DiskSetCallBack_v2(SaveCallBack));
	}

	if (InsertGotOne) {
		ui5r L0;

		if (noErr == err) {
			if (noErr == (err = DiskGetSize_v2(InsertVol, &L0))) {
				if (L != L0) {
					DisplayRunErr("new disk size wrong");
					err = kMyErrReported;
				} else {
					/* ok */
				}
			}
		}

		if (noErr == err) {
			*Drive_No = InsertVol;
		} else {
			DiskEjectDelete_v2(InsertVol);
				/* ignore any error, since already got one */
		}
	}

	if (NotAPbuf != Name) {
		PbufDispose_v2(Name);
	}

	return err;
}

LOCALFUNC tMyErr ExportFromFile2HostFile_v2(short refNum,
	ui5r L, tPbuf Name)
/* takes ownership of Name */
{
	tMyErr err;
	tDrive InsertVol;

	if (noErr == (err = DiskGetRawNew(L, Name, &InsertVol))) {
		err = WriteFromFileToVol_v2(refNum, InsertVol, 0, L);
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
