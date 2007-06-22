/*
	FILEUTIL.i
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
	FILE UTILilities
*/


struct MyDir_R {
	short VRefNum;
	long DirId;
};
typedef struct MyDir_R MyDir_R;

GLOBALFUNC blnr MyHGetDir(MyDir_R *d)
{
	WDPBRec r;
	blnr IsOk = falseblnr;

	r.ioCompletion = NULL;
	r.ioNamePtr = NULL;

	if (CheckSysErr(PBHGetVol(&r, false))) {
		d->VRefNum = r.ioWDVRefNum;
		d->DirId = r.ioWDDirID;
		IsOk = trueblnr;
	}
	return IsOk;
}

GLOBALFUNC blnr MyMakeNamedDir(MyDir_R *d, StringPtr s,
	MyDir_R *new_d)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioDirID = d->DirId;
	r.fileParam.ioNamePtr = s;
	if (CheckSysErr(PBDirCreate(&r, false))) {
		new_d->VRefNum = d->VRefNum;
		new_d->DirId = r.fileParam.ioDirID;
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALFUNC blnr MyDeleteFile(MyDir_R *d, StringPtr s)
{
	HParamBlockRec r;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioDirID = d->DirId;
	r.fileParam.ioNamePtr = s;
	return CheckSysErr(PBHDelete(&r, false));
}

/*
	Probably should use PBHOpenDF instead
	of PBHOpen when it is available.
	(System 7 according to Technical Note FL515)
*/

GLOBALFUNC blnr MyOpenOldFileRead(MyDir_R *d, StringPtr s,
	short *refnum)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = s;
	r.ioParam.ioVRefNum = d->VRefNum;
	r.ioParam.ioPermssn = (char)fsRdPerm;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.fileParam.ioDirID = d->DirId;
	if (CheckSysErr(PBHOpen(&r, false))) {
		*refnum = r.ioParam.ioRefNum;
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALFUNC blnr MyFileOpenWrite(MyDir_R *d, StringPtr s,
	short *refnum)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = s;
	r.ioParam.ioVRefNum = d->VRefNum;
	r.ioParam.ioPermssn = (char)fsWrPerm;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.fileParam.ioDirID = d->DirId;
	if (CheckSysErr(PBHOpen(&r, false))) {
		*refnum = r.ioParam.ioRefNum;
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALFUNC blnr MyFileOpenRFWrite(MyDir_R *d, StringPtr s,
	short *refnum)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = s;
	r.ioParam.ioVRefNum = d->VRefNum;
	r.ioParam.ioPermssn = (char)fsWrPerm;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.fileParam.ioDirID = d->DirId;
	if (CheckSysErr(PBHOpenRF(&r, false))) {
		*refnum = r.ioParam.ioRefNum;
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALFUNC blnr MyOpenNewFile(MyDir_R *d, StringPtr s,
	OSType creator, OSType fileType,
	short *refnum)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioDirID = d->DirId;
	r.fileParam.ioNamePtr = s;
	if (CheckSysErr(PBHCreate(&r, false))) {
		r.fileParam.ioCompletion = NULL;
		r.fileParam.ioNamePtr = s;
		r.fileParam.ioDirID = d->DirId;
		r.fileParam.ioVRefNum = d->VRefNum;
		r.fileParam.ioFVersNum = (char)0;
		r.fileParam.ioFDirIndex = (short)0;
		if (CheckSysErr(PBHGetFInfo(&r, false))) {
			r.fileParam.ioFlFndrInfo.fdType = fileType;
			r.fileParam.ioFlFndrInfo.fdCreator = creator;
			r.fileParam.ioNamePtr = s;
			r.fileParam.ioDirID = d->DirId;
			if (CheckSysErr(PBHSetFInfo(&r, false))) {
				r.ioParam.ioCompletion = NULL;
				r.ioParam.ioNamePtr = s;
				r.ioParam.ioVRefNum = d->VRefNum;
				r.ioParam.ioPermssn = (char)fsWrPerm;
				r.ioParam.ioMisc = 0; /* use volume buffer */
				r.fileParam.ioDirID = d->DirId;
				if (CheckSysErr(PBHOpen(&r, false))) {
					*refnum = r.ioParam.ioRefNum;
					IsOk = trueblnr;
				}
			}
		}
		if (! IsOk) {
			(void) MyDeleteFile(d, s);
		}
	}

	return IsOk;
}

GLOBALFUNC blnr MyCloseFile(short refNum)
{
	return CheckSysErr(FSClose(refNum));
}

GLOBALFUNC blnr MyWriteBytes(short refNum, MyPtr p, uimr L)
{
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;
	r.ioParam.ioBuffer = p;
	r.ioParam.ioReqCount = L;
	r.ioParam.ioPosMode = (short) fsFromMark;
	r.ioParam.ioPosOffset = 0;

	return CheckSysErr(PBWrite(&r, false));
}

GLOBALFUNC blnr MyReadBytes(short refNum, MyPtr p, uimr L)
{
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;
	r.ioParam.ioBuffer = p;
	r.ioParam.ioReqCount = L;
	r.ioParam.ioPosMode = (short) fsFromMark;
	r.ioParam.ioPosOffset = 0;

	return CheckSysErr(PBRead(&r, false));
}

GLOBALFUNC blnr MyBackWriteBytes(short refNum, uimr offset,
	MyPtr p, uimr L)
{
	long savepos;
	blnr IsOk = falseblnr;

	if (CheckSysErr(GetFPos(refNum, &savepos)))
	if (CheckSysErr(SetFPos(refNum, fsFromStart, offset)))
	if (MyWriteBytes(refNum, p, L))
	if (CheckSysErr(SetFPos(refNum, fsFromStart, savepos)))
	{
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALFUNC blnr MyBackReadBytes(short refNum, uimr offset,
	MyPtr p, uimr L)
{
	long savepos;
	blnr IsOk = falseblnr;

	if (CheckSysErr(GetFPos(refNum, &savepos)))
	if (CheckSysErr(SetFPos(refNum, fsFromStart, offset)))
	if (MyReadBytes(refNum, p, L))
	if (CheckSysErr(SetFPos(refNum, fsFromStart, savepos)))
	{
		IsOk = trueblnr;
	}

	return IsOk;
}

#define CatInfoIsFolder(cPB) (((cPB)->hfileInfo.ioFlAttrib & ioDirMask) != 0)

GLOBALFUNC blnr CatInfoOpenReadDF(CInfoPBRec *cPB,
	short *refnum)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = cPB->hfileInfo.ioNamePtr;
	r.ioParam.ioVRefNum = cPB->hfileInfo.ioVRefNum;
	r.ioParam.ioPermssn = (char)fsRdPerm;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.fileParam.ioDirID = cPB->hfileInfo.ioFlParID;
	if (CheckSysErr(PBHOpen(&r, false))) {
		*refnum = r.ioParam.ioRefNum;
		IsOk = trueblnr;
	}
	return IsOk;
}

GLOBALFUNC blnr CatInfoOpenReadRF(CInfoPBRec *cPB,
	short *refnum)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = cPB->hfileInfo.ioNamePtr;
	r.ioParam.ioVRefNum = cPB->hfileInfo.ioVRefNum;
	r.ioParam.ioPermssn = (char)fsRdPerm;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.fileParam.ioDirID = cPB->hfileInfo.ioFlParID;
	if (CheckSysErr(PBHOpenRF(&r, false))) {
		*refnum = r.ioParam.ioRefNum;
		IsOk = trueblnr;
	}
	return IsOk;
}

#ifdef HaveTempBuffH
GLOBALFUNC blnr MyFileCopyBytes(short src_refnum, short dst_refnum, uimr L)
{
	uimr maxn;
	uimr n;
	MyPtr p;
	blnr IsOk = falseblnr;

	if (TempBuffHBeginUse(&p)) {
		maxn = TempBuffSize;

doGetMore:
		if (0 == L) {
			IsOk = trueblnr;
		} else {
			if (L > maxn) {
				n = maxn;
			} else {
				n = L;
			}
			if (MyReadBytes(src_refnum, p, n)) {
				if (MyWriteBytes(dst_refnum, p, n)) {
					L -= n;
					goto doGetMore;
				}
			}
		}

		TempBuffHEndUse();
	}

	return IsOk;
}
#endif

#ifdef HaveTempBuffH
GLOBALFUNC blnr MyCatInfoCopyRF(CInfoPBRec *cPB, MyDir_R *dst_d, StringPtr dst_s)
{
	short src_refnum;
	short dst_refnum;
	uimr n = cPB->hfileInfo.ioFlRLgLen;
	blnr IsOk = falseblnr;

	if (0 == n) {
		IsOk = trueblnr;
	} else if (CatInfoOpenReadRF(cPB, &src_refnum)) {
		if (MyFileOpenRFWrite(dst_d, dst_s, &dst_refnum)) {
			if (MyFileCopyBytes(src_refnum, dst_refnum, n)) {
				IsOk = trueblnr;
			}
			(void) MyCloseFile(dst_refnum);
		}
		(void) MyCloseFile(src_refnum);
	}

	return IsOk;
}
#endif

#ifdef HaveTempBuffH
GLOBALFUNC blnr MyCatInfoCopyDF(CInfoPBRec *cPB, MyDir_R *dst_d, StringPtr dst_s)
{
	short src_refnum;
	short dst_refnum;
	uimr n = cPB->hfileInfo.ioFlLgLen;
	blnr IsOk = falseblnr;

	if (0 == n) {
		IsOk = trueblnr;
	} else if (CatInfoOpenReadDF(cPB, &src_refnum)) {
		if (MyFileOpenWrite(dst_d, dst_s, &dst_refnum)) {
			if (MyFileCopyBytes(src_refnum, dst_refnum, n)) {
				IsOk = trueblnr;
			}
			(void) MyCloseFile(dst_refnum);
		}
		(void) MyCloseFile(src_refnum);
	}

	return IsOk;
}
#endif

GLOBALFUNC blnr MyFileGetCatInfo(MyDir_R *d, StringPtr s,
	StringPtr NameBuffer, CInfoPBRec *cPB)
{
	blnr IsOk = falseblnr;

	cPB->hfileInfo.ioCompletion = NULL;
	cPB->hfileInfo.ioVRefNum = d->VRefNum;
	cPB->dirInfo.ioDrDirID = d->DirId;
	if (NULL == s) {
		if (NULL == NameBuffer) {
			/*
				then this field must already be
				set up by caller
			*/
		} else {
			cPB->hfileInfo.ioNamePtr = NameBuffer;
		}
		cPB->dirInfo.ioFDirIndex = (short)(- 1);
	} else {
		if (NULL == NameBuffer) {
			cPB->hfileInfo.ioNamePtr = s;
		} else {
			cPB->hfileInfo.ioNamePtr = NameBuffer;
			PStrCopy(NameBuffer, s);
		}
		cPB->dirInfo.ioFDirIndex = 0;
	}

	return CheckSysErr(PBGetCatInfo(cPB, false));
}

GLOBALFUNC blnr MyFileExists(MyDir_R *d, StringPtr s,
	blnr *Exists)
{
	MyPStr NameBuffer;
	CInfoPBRec cPB;
	OSErr err;
	blnr IsOk = falseblnr;

	cPB.hfileInfo.ioCompletion = NULL;
	cPB.hfileInfo.ioVRefNum = d->VRefNum;
	cPB.dirInfo.ioDrDirID = d->DirId;
	cPB.hfileInfo.ioNamePtr = NameBuffer;
	PStrCopy(NameBuffer, s);
	cPB.dirInfo.ioFDirIndex = 0;
	err = PBGetCatInfo(&cPB, false);
	if (noErr == err) {
		*Exists = trueblnr;
		IsOk = trueblnr;
	} else if (fnfErr == err) {
		*Exists = falseblnr;
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALFUNC blnr MyCatInfoCopyInfo(CInfoPBRec *cPB, MyDir_R *dst_d, StringPtr dst_s)
{
	MyPStr NameBuffer;
	CInfoPBRec r;
	blnr IsOk = falseblnr;

	if (MyFileGetCatInfo(dst_d, dst_s, NameBuffer, &r)) {

		r.hfileInfo.ioFlFndrInfo.fdType = cPB->hfileInfo.ioFlFndrInfo.fdType;
		r.hfileInfo.ioFlFndrInfo.fdCreator = cPB->hfileInfo.ioFlFndrInfo.fdCreator;
			/* or frRect for folder */
		r.hfileInfo.ioFlFndrInfo.fdLocation = cPB->hfileInfo.ioFlFndrInfo.fdLocation;
			/* or frLocation for folder */
		r.hfileInfo.ioFlFndrInfo.fdFlags =
			cPB->hfileInfo.ioFlFndrInfo.fdFlags;
			/* (0x0100 & r.hfileInfo.ioFlFndrInfo.fdFlags)
			|
			(0xFEFF & cPB->hfileInfo.ioFlFndrInfo.fdFlags) */;
			/* or frFlags */

		r.hfileInfo.ioFlCrDat = cPB->hfileInfo.ioFlCrDat;
		r.hfileInfo.ioFlMdDat = cPB->hfileInfo.ioFlMdDat;
		r.hfileInfo.ioFlBkDat = cPB->hfileInfo.ioFlBkDat;
			/* or ioDrCrDat, ioDrMdDat, and ioDrBkDat */

		if (CatInfoIsFolder(cPB)) {
			r.dirInfo.ioDrFndrInfo.frScroll = cPB->dirInfo.ioDrFndrInfo.frScroll;
			r.dirInfo.ioDrUsrWds.frView = cPB->dirInfo.ioDrUsrWds.frView;
		}

		r.hfileInfo.ioDirID = r.hfileInfo.ioFlParID;
		if (CheckSysErr(PBSetCatInfo(&r, false))) {
			IsOk = trueblnr;
		}
	}

	return IsOk;
}

#ifdef HaveTempBuffH
GLOBALFUNC blnr MyCatInfoCopyFile(CInfoPBRec *cPB, MyDir_R *dst_d, ps3p dst_s)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = dst_d->VRefNum;
	r.fileParam.ioDirID = dst_d->DirId;
	r.fileParam.ioNamePtr = dst_s;
	if (CheckSysErr(PBHCreate(&r, false))) {
		if (MyCatInfoCopyRF(cPB, dst_d, dst_s))
		if (MyCatInfoCopyDF(cPB, dst_d, dst_s))
		if (MyCatInfoCopyInfo(cPB, dst_d, dst_s))
		{
			IsOk = trueblnr;
		}
		if (! IsOk) {
			(void) MyDeleteFile(dst_d, dst_s);
		}
	}

	return IsOk;
}
#endif

GLOBALFUNC blnr MyFileClearInitted(MyDir_R *dst_d, StringPtr dst_s)
{
	MyPStr NameBuffer;
	CInfoPBRec r;
	blnr IsOk = falseblnr;

	if (MyFileGetCatInfo(dst_d, dst_s, NameBuffer, &r)) {

		r.hfileInfo.ioFlFndrInfo.fdFlags &= 0xFEFF;
			/* or frFlags */

		r.hfileInfo.ioDirID = r.hfileInfo.ioFlParID;
		if (CheckSysErr(PBSetCatInfo(&r, false))) {
			IsOk = trueblnr;
		}
	}

	return IsOk;
}

GLOBALFUNC blnr MyFileCopyFolderInfo(MyDir_R *src_d, MyDir_R *dst_d)
{
	MyPStr NameBuffer;
	CInfoPBRec cPB;
	blnr IsOk = falseblnr;

	if (MyFileGetCatInfo(src_d, NULL, NameBuffer, &cPB))
	if (MyCatInfoCopyInfo(&cPB, dst_d, NULL))
	{
		IsOk = trueblnr;
	}

	return IsOk;
}

GLOBALPROC MyCatInfoGetMyDir(CInfoPBRec *cPB, MyDir_R *d)
/* assumes CatInfoIsFolder(cPB) */
{
	d->VRefNum = cPB->hfileInfo.ioVRefNum;
	d->DirId = cPB->dirInfo.ioDrDirID;
}

GLOBALFUNC blnr MyCatGetNextChild(CInfoPBRec *cPB,
	MyDir_R *old_d, int *index, blnr *FinishOk)
{
	OSErr err;
	blnr GotOne = falseblnr;
	blnr FinishOk0 = falseblnr;

	cPB->dirInfo.ioFDirIndex = (*index)++;
	cPB->dirInfo.ioDrDirID = old_d->DirId;
	err = PBGetCatInfo(cPB, false);
	if (noErr == err) {
		GotOne = trueblnr;
	} else if (fnfErr != err) {
		vCheckSysErr(err);
	} else {
		FinishOk0 = trueblnr;
	}

	*FinishOk = FinishOk0;
	return GotOne;
}

GLOBALFUNC blnr MyFindNamedChildDir(MyDir_R *src_d, StringPtr s, MyDir_R *dst_d)
{
	MyPStr NameBuffer;
	CInfoPBRec cPB;
	blnr IsOk = falseblnr;

	if (MyFileGetCatInfo(src_d, s, NameBuffer, &cPB)) {
		if (! CatInfoIsFolder(&cPB)) {
			vCheckSysErr(dirNFErr);
		} else {
			MyCatInfoGetMyDir(&cPB, dst_d);
			IsOk = trueblnr;
		}
	}

	return IsOk;
}
