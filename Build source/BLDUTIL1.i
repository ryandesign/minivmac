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

LOCALFUNC ui4b CStr2Pbuf(char *s)
{
	ui4b v;
	ui5r L = CStrLength(s);

	if (PbufNew(L, &v)) {
		if (PbufTransfer(s, v, 0, L, trueblnr)) {
			return v;
		}
		(void) PbufDispose(v);
	}

	return NotAPbuf;
}

LOCALFUNC blnr PStr2Pbuf(ps3p s, ui4b *r)
{
	ui4b v;
	ui5b L = PStrLength(s);

	if (PbufNew(L, &v)) {
		if (PbufTransfer(PStrToPtr(s), v, 0, L, trueblnr)) {
			*r = v;
			return trueblnr;
		}
		(void) PbufDispose(v);
	}

	return falseblnr;
}

LOCALFUNC blnr WriteFromFileToVolUsingBuff(short refNum,
	ui4b Drive_No, ui5b Sony_Start, ui5b Sony_Count,
	void *buff, ui5b BuffL)
{
	ui5b L = Sony_Count;
	ui5b offset = Sony_Start;

	while ((L != 0) && (! AbortRequested)) {
		ui5b n = (L > BuffL) ? BuffL : L;

		/* OccasionalSpin(); */

		if (! MyReadBytes(refNum, buff, n)) {
			return falseblnr;
		} else {
			if (! DiskWrite(buff, Drive_No, offset, &n)) {
				return falseblnr;
			} else {
				L -= n;
			}
		}

		offset += n;
	}

	return (L == 0);
}

LOCALFUNC blnr WriteFromFileToVol(short refNum,
	ui4b Drive_No, ui5b Sony_Start, ui5b Sony_Count)
{
	Handle h;
	blnr v = falseblnr;

	ui5b MaxBuffL = 2 * Sony_Count + 1;
	ui5b Free = FreeMem();
	ui5b BuffL = 64 * 1024;

	if (MaxBuffL > Free) {
		MaxBuffL = Free;
	}
	if (MaxBuffL < 512) {
		MaxBuffL = 512;
	}

	while (BuffL > MaxBuffL) {
		BuffL >>= 1;
	}

	if (MyHandleNew(BuffL, &h)) {
		HLock(h);

		v = WriteFromFileToVolUsingBuff(refNum, Drive_No,
			Sony_Start, Sony_Count, *h, BuffL);

		HUnlock(h);
		DisposeHandle(h);
	}

	return v;
}

LOCALFUNC blnr ExportFromFile2HostFile(short refNum, ui5b L, ui4b Name)
{
	ui5b SaveCallBack;
	ui4b InsertVol;
	blnr InsertGotOne = falseblnr;
	blnr IsOk = falseblnr;

	if (DiskGetCallBack(&SaveCallBack)) {
		if (DiskSetCallBack(0)) {
			ui4b mode;

			if (DiskGetRawMode(&mode)) {
				if (DiskSetRawMode(1)) {
					if (DiskNew(L, Name)) {
						OSErr InsertErr;
						ui4b NewWanted;

						do {
							if (! DiskGetNewWanted(&NewWanted)) {
								NewWanted = (ui4b)0;
							}
							InsertErr = DiskNextPendingInsert(&InsertVol);
						} while ((InsertErr == (OSErr)0xFFC8)
							&& (! AbortRequested) && (NewWanted != 0));

						if (noErr == InsertErr) {
							InsertGotOne = trueblnr;
						} else if (AbortRequested || (NewWanted == 0)) {
							DisplayRunErr("Aborted!");
						} else {
							CheckSysErr(InsertErr);
						}
					}
				}
				(void) DiskSetRawMode(mode);
			}

		}

		(void) DiskSetCallBack(SaveCallBack);
	}

	if (InsertGotOne) {
		ui5b L0;

		if (DiskGetSize(InsertVol, &L0)) {
			if (L != L0) {
				DisplayRunErr("new disk size wrong");
			} else {
				IsOk = WriteFromFileToVol(refNum, InsertVol, 0, L);
			}
		}

		if (IsOk) {
			(void) DiskEject(InsertVol);
		} else {
			(void) DiskEjectDelete(InsertVol);
		}
	}

	return IsOk;
}

LOCALFUNC blnr ExportFromFile2HostFile2(MyDir_R *d, StringPtr s)
{
	short refnum;
	ui5b L;
	ui4b Name;
	blnr IsOk = falseblnr;

	if (MyOpenOldFileRead(d, s, &refnum)) {
		if (CheckSysErr(GetEOF(refnum, (long *)&L))) {
			if (PStr2Pbuf(s, &Name)) {
				if (ExportFromFile2HostFile(refnum, L, Name)) {
					IsOk = trueblnr;
					/* v = 0; */
				}
			}
		}
		(void) MyCloseFile(refnum);
	}

	return trueblnr;
}

LOCALPROC Uimr2Dec(uimr v, ps3p s)
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

LOCALPROC ui2r2Hex(ui3r v, MyCharR *r)
{
	if (v < 10) {
		*r = '0' + v;
	} else {
		*r = 'A' + v - 10;
	}
}

LOCALPROC ui3r2Hex(ui3r v, MyCharR *r)
{
	ui2r2Hex((v     ) & 0x0F, r + 1);
	ui2r2Hex((v >> 4) & 0x0F, r);
}

LOCALPROC ui4r2Hex(ui4r v, MyCharR *r)
{
	ui3r2Hex((v     ) & 0xFF, r + 2);
	ui3r2Hex((v >> 8) & 0xFF, r);
}

LOCALPROC ui5r2Hex(ui5r v, MyCharR *r)
{
	ui4r2Hex((v      ) & 0xFFFF, r + 4);
	ui4r2Hex((v >> 16) & 0xFFFF, r);
}

LOCALFUNC blnr MakeSubDirectory(MyDir_R *new_d, MyDir_R *d, char *name, char *FileExt)
{
	Str255 s;

	PStrFromCStr(s, name);
	PStrApndCStr(s, FileExt);
	return MyMakeNamedDir(d, s, new_d);
}

LOCALFUNC blnr MyDeleteOneIfExists(MyDir_R *d, StringPtr s)
{
	blnr Exists;
	blnr IsOk = falseblnr;

	if (MyFileExists(d, s, &Exists)) {
		if (Exists) {
			IsOk = dellib_DeleteOne(d, s);
		} else {
			IsOk = trueblnr;
		}
	}

	return IsOk;
}

LOCALFUNC blnr DelMakeSubDirectory(MyDir_R *new_d, MyDir_R *d, char *name, char *FileExt)
{
	Str255 s;
	blnr IsOk = falseblnr;

	PStrFromCStr(s, name);
	PStrApndCStr(s, FileExt);

	if (MyDeleteOneIfExists(d, s)) {
		IsOk = MyMakeNamedDir(d, s, new_d);
	}

	return IsOk;
}

LOCALFUNC blnr FindSubDirectory(MyDir_R *new_d, MyDir_R *d, char *name, char *FileExt)
{
	Str255 s;

	PStrFromCStr(s, name);
	PStrApndCStr(s, FileExt);
	return MyFindNamedChildDir(d, s, new_d);
}
