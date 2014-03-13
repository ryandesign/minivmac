/*
	EXTNUTIL.i
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
	mini vmac EXTeNSions library UTILities
*/


GLOBALFUNC tMyErr Ptr2Pbuf_v2(MyPtr p, uimr L, tPbuf *r)
{
	tMyErr err;
	tPbuf v;

	if (noErr == (err = PbufNew_v2(L, &v))) {
		err = PbufTransfer_v2(p, v, 0, L, trueblnr);

		if (noErr != err) {
			(void) PbufDispose_v2(v);
				/* ignore any error, since already got one */
		} else {
			*r = v;
		}
	}

	return err;
}

GLOBALFUNC tMyErr CStr2Pbuf_v2(char *s, tPbuf *r)
{
	return Ptr2Pbuf_v2((MyPtr)s, CStrLength(s), r);
}

GLOBALFUNC tMyErr PStr2Pbuf_v2(ps3p s, tPbuf *r)
{
	return Ptr2Pbuf_v2(PStrToPtr(s), PStrLength(s), r);
}

GLOBALFUNC tMyErr Pbuf2PStr_v2(tPbuf Pbuf_No, ps3p s)
{
	tMyErr err;
	ui5r L;

	if (noErr == (err = PBufGetSize_v2(Pbuf_No, &L))) {
		if (L > PStrMaxLength) {
			L = PStrMaxLength;
		}
		s[0] = L;
		err = PbufTransfer_v2(PStrToPtr(s),
			Pbuf_No, 0, L, falseblnr);
	}

	return err;
}

GLOBALFUNC tMyErr DiskGetName2Pstr_v2(tDrive Drive_No, ps3p s)
{
	tMyErr err;
	tPbuf Pbuf_No;

	if (noErr == (err = DiskGetName_v2(Drive_No, &Pbuf_No))) {
		err = Pbuf2PStr_v2(Pbuf_No, s);
		err = CombineErr(err, PbufDispose_v2(Pbuf_No));
	}

	return err;
}

GLOBALFUNC tMyErr HostVolWaitForInsertOld_v2(tDrive *r)
{
	tMyErr err;
	ui5r SaveCallBack;
	ui4r SaveRawMode;
	tDrive InsertVol;
	blnr InsertGotOne = falseblnr;

	if (noErr == (err = DiskGetCallBack_v2(&SaveCallBack))) {
		if (noErr == (err = DiskSetCallBack_v2(0))) {
			if (noErr == (err = DiskGetRawMode_v2(&SaveRawMode))) {
				if (noErr == (err = DiskSetRawMode_v2(1))) {
label_retry:
					if (noErr == (err = DiskNextPendingInsert(
						&InsertVol)))
					{
						InsertGotOne = trueblnr;
					} else {
						if (err == (OSErr)0xFFC8) {
							if (noErr == (err = CheckAbortRequested()))
							{
								goto label_retry;
							}
						}
					}
				}
				err = CombineErr(err, DiskSetRawMode_v2(SaveRawMode));
			}
		}
		err = CombineErr(err, DiskSetCallBack_v2(SaveCallBack));
	}

	if (InsertGotOne) {
		if (noErr == err) {
			*r = InsertVol;
		} else {
			(void) DiskEject_v2(InsertVol);
				/* ignore any error, since already got one */
		}
	}

	return err;
}

GLOBALFUNC tMyErr HostVolInsertNew_v2(ui5r L, tPbuf Name,
	tDrive *Drive_No)
{
	tMyErr err;
	ui5r SaveCallBack;
	ui4r SaveRawMode;
	ui4r NewWanted;
	tDrive InsertVol;
	blnr InsertGotOne = falseblnr;

	UpdateProgressBar(); /* before modal dialog */
	if (noErr == (err = DiskGetCallBack_v2(&SaveCallBack))) {
		if (noErr == (err = DiskSetCallBack_v2(0))) {
			if (noErr == (err = DiskGetRawMode_v2(&SaveRawMode))) {
				if (noErr == (err = DiskSetRawMode_v2(1))) {
					if (noErr == (err = DiskNew_v2(L, Name))) {
label_retry:
						if (noErr == (err = DiskGetNewWanted_v2(
							&NewWanted)))
						{
							if (noErr == (err = DiskNextPendingInsert(
								&InsertVol)))
							{
								InsertGotOne = trueblnr;
							} else {
								if (err == (OSErr)0xFFC8) {
									if (NewWanted == 0) {
										/*
											important to look at
											NewWanted result of
											DiskGetNewWanted_v2
											from before call to
											DiskNextPendingInsert.
											if look at result from
											after, insert might
											occur between calls.
										*/
										/*
											MyAlertFromCStr("Aborted!");
										*/
										err = kMyErrReported;
									} else {
										if (noErr ==
											(err =
												CheckAbortRequested()))
										{
											goto label_retry;
										}
									}
								}
							}
						}
					}
					Name = NotAPbuf;
				}
				err = CombineErr(err, DiskSetRawMode_v2(SaveRawMode));
			}
		}
		err = CombineErr(err, DiskSetCallBack_v2(SaveCallBack));
	}

	if (InsertGotOne) {
		ui5r L0;

		if (noErr == err) {
			if (noErr == (err = DiskGetSize_v2(InsertVol, &L0))) {
				if (L != L0) {
					MyAlertFromCStr("new disk size wrong");
					err = kMyErrReported;
				} else {
					/* ok */
				}
			}
		}

		if (noErr == err) {
			*Drive_No = InsertVol;
		} else {
			/*
				might be some other disk image entirely, that
				just got mounted at a bad time.
				So don't delete it.
			*/
			(void) DiskEject_v2(InsertVol);
				/* ignore any error, since already got one */
		}
	}

	if (NotAPbuf != Name) {
		(void) PbufDispose_v2(Name);
	}

	return err;
}

GLOBALFUNC tMyErr NewDiskEject_v2(tDrive Drive_No, tMyErr err0)
{
	tMyErr err;

	if (noErr == err0) {
		err = DiskEject_v2(Drive_No);
	} else {
		err = err0;
		(void) DiskEjectDelete_v2(Drive_No);
			/* ignore any error, since already got one */
	}

	return err;
}

GLOBALFUNC tMyErr WriteFromFileToVolUsingBuff_v2(short refNum,
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

		if (noErr == (err = CheckAbortRequested()))
		if (noErr == (err = MyReadBytes_v2(refNum, buff, n)))
		if (noErr == (err = DiskWrite_v2(buff, Drive_No, offset, &n)))
		{
			L -= n;
			offset += n;
			ProgressBarVal += n;

			goto Label_1;
		}
	}

	return err;
}

GLOBALFUNC tMyErr WriteFromVolToFileUsingBuff_v2(short refNum,
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

		if (noErr == (err = CheckAbortRequested()))
		if (noErr == (err = DiskRead_v2(buff, Drive_No, offset, &n)))
		if (noErr == (err = MyWriteBytes_v2(refNum, buff, n)))
		{
			L -= n;
			offset += n;
			ProgressBarVal += n;

			goto Label_1;
		}
	}

	return err;
}

GLOBALFUNC tMyErr WriteFromVolToVolUsingBuff(
	tDrive DriveIn_No, ui5r SonyIn_Start,
	tDrive DriveOut_No, ui5r SonyOut_Start,
	ui5r Sony_Count, void *buff, ui5r BuffL)
{
	tMyErr err;
	ui5r L = Sony_Count;
	ui5r offset_in = SonyIn_Start;
	ui5r offset_out = SonyOut_Start;

Label_1:
	if (L == 0) {
		err = noErr;
	} else {
		ui5r n = (L > BuffL) ? BuffL : L;

		if (noErr == (err = CheckAbortRequested()))
		if (noErr == (err = DiskRead_v2(buff, DriveIn_No,
			offset_in, &n)))
		if (noErr == (err = DiskWrite_v2(buff, DriveOut_No,
			offset_out, &n)))
		{
			L -= n;
			offset_in += n;
			offset_out += n;
			ProgressBarVal += n;

			goto Label_1;
		}
	}

	return err;
}

GLOBALFUNC tMyErr WriteFromFileToVol_v2(short refNum,
	tDrive Drive_No, ui5r Sony_Start, ui5r Sony_Count)
{
	tMyErr err;
	MyPtr p;

	if (noErr == (err = TempBuffHBeginUse_v2(&p))) {
		err = WriteFromFileToVolUsingBuff_v2(refNum, Drive_No,
			Sony_Start, Sony_Count, p, TempBuffSize);
		TempBuffHEndUse();
	}

	return err;
}

GLOBALFUNC tMyErr WriteFromVolToFile_v2(short refNum,
	tDrive Drive_No, ui5r Sony_Start, ui5r Sony_Count)
{
	tMyErr err;
	MyPtr p;

	if (noErr == (err = TempBuffHBeginUse_v2(&p))) {
		err = WriteFromVolToFileUsingBuff_v2(refNum, Drive_No,
			Sony_Start, Sony_Count, p, TempBuffSize);
		TempBuffHEndUse();
	}

	return err;
}

GLOBALFUNC tMyErr WriteFromVolToVol(
	tDrive DriveIn_No, ui5r SonyIn_Start,
	tDrive DriveOut_No, ui5r SonyOut_Start,
	ui5r Sony_Count)
{
	tMyErr err;
	MyPtr p;

	if (noErr == (err = TempBuffHBeginUse_v2(&p))) {
		err = WriteFromVolToVolUsingBuff(
			DriveIn_No, SonyIn_Start,
			DriveOut_No, SonyOut_Start,
			Sony_Count, p, TempBuffSize);
		TempBuffHEndUse();
	}

	return err;
}
