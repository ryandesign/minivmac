/*
	FILEUTIL.i
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
	FILE UTILilities
*/


struct MyDir_R {
	long DirId;
	short VRefNum;
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
	OSErr err;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioNamePtr = s;

#if Support64kROM
	if (Have64kROM()) {
		r.fileParam.ioFVersNum = 0;
		err = PBDelete((ParamBlockRec *)&r, false);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHDelete(&r, false);
	}

	return CheckSysErr(err);
}

/*
	Probably should use PBHOpenDF instead
	of PBHOpen when it is available.
	(System 7 according to Technical Note FL515)
*/

LOCALFUNC blnr MyFileOpen0(MyDir_R *d, StringPtr s,
	char Permssn, short *refnum)
{
	HParamBlockRec r;
	OSErr err;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = s;
	r.ioParam.ioVRefNum = d->VRefNum;
	r.ioParam.ioPermssn = Permssn;
	r.ioParam.ioMisc = 0; /* use volume buffer */

#if Support64kROM
	if (Have64kROM()) {
		r.ioParam.ioVersNum = 0;
		err = PBOpen((ParamBlockRec *)&r, false);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHOpen(&r, false);
	}

	*refnum = r.ioParam.ioRefNum;
	return CheckSysErr(err);
}

GLOBALFUNC blnr MyOpenOldFileRead(MyDir_R *d, StringPtr s,
	short *refnum)
{
	return MyFileOpen0(d, s, (char)fsRdPerm, refnum);
}

GLOBALFUNC blnr MyFileOpenWrite(MyDir_R *d, StringPtr s,
	short *refnum)
{
	return MyFileOpen0(d, s, (char)fsWrPerm, refnum);
}

GLOBALFUNC blnr MyFileOpenRFWrite(MyDir_R *d, StringPtr s,
	short *refnum)
{
	HParamBlockRec r;
	OSErr err;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = s;
	r.ioParam.ioVRefNum = d->VRefNum;
	r.ioParam.ioPermssn = (char)fsWrPerm;
	r.ioParam.ioMisc = 0; /* use volume buffer */

#if Support64kROM
	if (Have64kROM()) {
		r.ioParam.ioVersNum = 0;
		err = PBOpenRF((ParamBlockRec *)&r, false);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHOpenRF(&r, false);
	}

	*refnum = r.ioParam.ioRefNum;
	return CheckSysErr(err);
}

LOCALFUNC OSErr MyCreateFile0(MyDir_R *d, StringPtr s)
{
	HParamBlockRec r;
	OSErr err;

	r.fileParam.ioFlVersNum = 0;
		/*
			Think reference say to do this,
			but not Inside Mac IV
		*/

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioNamePtr = s;
	r.fileParam.ioVRefNum = d->VRefNum;

#if Support64kROM
	if (Have64kROM()) {
		r.fileParam.ioFVersNum = 0;
		err = PBCreate((ParamBlockRec *)&r, false);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHCreate(&r, false);
	}

	return err;
}

LOCALFUNC OSErr MyCreateFile(MyDir_R *d, StringPtr s)
{
	return CheckSysErr(MyCreateFile0(d, s));
}

LOCALFUNC blnr MyCreateFileOverWrite(MyDir_R *d, StringPtr s)
{
	OSErr err;
	blnr IsOk = falseblnr;

	err = MyCreateFile0(d, s);
	if (dupFNErr == err) {
		if (MyDeleteFile(d, s)) {
			IsOk = MyCreateFile(d, s);
		}
	} else {
		IsOk = CheckSysErr(err);
	}

	return IsOk;
}

GLOBALFUNC blnr MyFileGetInfo(MyDir_R *d, StringPtr s,
	HParamBlockRec *r)
{
	OSErr err;

	r->fileParam.ioCompletion = NULL;
	r->fileParam.ioNamePtr = s;
	r->fileParam.ioVRefNum = d->VRefNum;
	r->fileParam.ioFVersNum = (char)0;
	r->fileParam.ioFDirIndex = (short)0;

#if Support64kROM
	if (Have64kROM()) {
		err = PBGetFInfo((ParamBlockRec *)r, false);
	} else
#endif
	{
		r->fileParam.ioDirID = d->DirId;
		err = PBHGetFInfo(r, false);
	}

	return CheckSysErr(err);
}

GLOBALFUNC blnr MyFileSetInfo(MyDir_R *d, StringPtr s,
	HParamBlockRec *r)
{
	OSErr err;

	r->fileParam.ioCompletion = NULL;
	r->fileParam.ioNamePtr = s;
	r->fileParam.ioVRefNum = d->VRefNum;

#if Support64kROM
	if (Have64kROM()) {
		r->fileParam.ioFVersNum = (char)0;
		err = PBSetFInfo((ParamBlockRec *)r, false);
	} else
#endif
	{
		r->fileParam.ioDirID = d->DirId;
		err = PBHSetFInfo(r, false);
	}

	return CheckSysErr(err);
}

GLOBALFUNC blnr MyFileSetTypeCreator(MyDir_R *d, StringPtr s,
	OSType creator, OSType fileType)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	if (MyFileGetInfo(d, s, &r)) {
		r.fileParam.ioFlFndrInfo.fdType = fileType;
		r.fileParam.ioFlFndrInfo.fdCreator = creator;
		if (MyFileSetInfo(d, s, &r)) {
			IsOk = trueblnr;
		}
	}

	return IsOk;
}

GLOBALFUNC blnr MyOpenNewFile(MyDir_R *d, StringPtr s,
	OSType creator, OSType fileType,
	short *refnum)
{
	blnr IsOk = falseblnr;

	if (MyCreateFile(d, s)) {
		if (MyFileSetTypeCreator(d, s,
			creator, fileType))
		{
			IsOk = MyFileOpenWrite(d, s, refnum);
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

LOCALFUNC blnr MyOpenFileGetEOF(short refnum, uimr *L)
{
	return CheckSysErr(GetEOF(refnum, (long *)L));
}

LOCALFUNC blnr MyOpenFileSetEOF(short refnum, uimr L)
{
	return CheckSysErr(SetEOF(refnum, (long)L));
}

LOCALPROC MyFileFlush(short refnum)
{
	short vRefNum;

#if Support64kROM
	if (Have64kROM()) {
		vCheckSysErr(-1);
			/*
				fix me, GetVRefNum glue broken in MPW 3.
				Thinks FCB for pre-HFS is 94 instead of 30.
			*/
		return;
	}
#endif

	if (CheckSysErr(GetVRefNum(refnum, &vRefNum))) {
		vCheckSysErr(FlushVol(NULL, vRefNum));
	}
}

LOCALFUNC blnr MyOpenFileUpdtLocation(short refNum,
	MyDir_R *d, ps3p s)
{
	/*
		because the user could move or rename the
		file while the program is working on it.
	*/
	blnr IsOk = falseblnr;

#if Support64kROM
	if (Have64kROM()) {
		IsOk = trueblnr;
		/*
			Do nothing. Volume of open file can't
			change. And haven't figured out a
			way to get name.
		*/
	} else
#endif
	{
		FCBPBRec b;

		b.ioCompletion = NULL;
		b.ioNamePtr = (StringPtr)s;
		b.ioVRefNum = 0;
		b.ioRefNum = refNum;
		b.ioFCBIndx = 0;
		if (CheckSysErr(PBGetFCBInfo(&b, false))) {
			d->VRefNum = b.ioFCBVRefNum;
			d->DirId = b.ioFCBParID;
			IsOk = trueblnr;
		}
	}

	return IsOk;
}

LOCALPROC MyCloseNewFile(short refNum, MyDir_R *d, ps3p s, blnr KeepIt)
/* s may be modified, must be Str255 (and d modified too) */
{
	blnr DoDelete;

	DoDelete = (! KeepIt)
		&& MyOpenFileUpdtLocation(refNum, d, s);
	(void) MyCloseFile(refNum);
	if (DoDelete) {
		(void) MyDeleteFile(d, s);
	}
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

LOCALFUNC blnr MyDirFromWD(short VRefNum, MyDir_R *d)
{
	Str63 s;
	WDPBRec pb;
	blnr IsOk = falseblnr;

#if Support64kROM
	if (Have64kROM()) {
		d->VRefNum = VRefNum;
		d->DirId = 0;
		IsOk = trueblnr;
	} else
#endif
	{
		pb.ioCompletion = NULL;
		pb.ioNamePtr = s;
		pb.ioVRefNum = VRefNum;
		pb.ioWDIndex = 0;
		pb.ioWDProcID = 0;
		if (CheckSysErr(PBGetWDInfo(&pb, false))) {
			d->VRefNum = pb.ioWDVRefNum;
			d->DirId = pb.ioWDDirID;
			IsOk = trueblnr;
		}
	}

	return IsOk;
}

#ifdef Have_MACINITS
LOCALFUNC blnr MyFilePutNew(StringPtr prompt, StringPtr origName,
		MyDir_R *d, StringPtr s)
{
	blnr IsOk = falseblnr;

	if (! HaveCustomPutFileAvail()) {
		SFReply reply;
		Point tempPt;

		tempPt.h = 100;
		tempPt.v = 100;
		SFPutFile(tempPt, prompt, origName, NULL, &reply);
		if (reply.good) {
			PStrCopy(s, reply.fName);
			IsOk = MyDirFromWD(reply.vRefNum, d);
		}
	} else {
		StandardFileReply reply;

		StandardPutFile(prompt, origName, &reply);
		if (reply.sfGood) {
			d->VRefNum = reply.sfFile.vRefNum;
			d->DirId = reply.sfFile.parID;
			PStrCopy(s, reply.sfFile.name);
			IsOk = trueblnr;
		}
	}

	return IsOk;
}
#endif

#ifdef Have_MACINITS
LOCALFUNC blnr MyFileGetOld(simr nInputTypes,
		ConstSFTypeListPtr pfInputType,
		MyDir_R *d, StringPtr s)
{
	blnr IsOk = falseblnr;

	if (! HaveCustomPutFileAvail()) {
		SFReply reply;
		Point tempPt;

		tempPt.h = 50;
		tempPt.v = 50;
		SFGetFile(tempPt, "\p", NULL, nInputTypes,
			(SFTypeList)pfInputType, NULL, &reply);
		if (reply.good) {
			PStrCopy(s, reply.fName);
			IsOk = MyDirFromWD(reply.vRefNum, d);
		}
	} else {
		StandardFileReply reply;

		StandardGetFile (NULL, nInputTypes, (SFTypeList)pfInputType, &reply);
		if (reply.sfGood) {
			d->VRefNum = reply.sfFile.vRefNum;
			d->DirId = reply.sfFile.parID;
			PStrCopy(s, reply.sfFile.name);
			IsOk = trueblnr;
		}
	}

	return IsOk;
}
#endif

#ifdef Have_MACINITS
LOCALFUNC blnr CreateOpenNewFile(StringPtr prompt, StringPtr origName, OSType creator, OSType fileType,
		MyDir_R *d, StringPtr s, short *refNum)
{
	blnr IsOk = falseblnr;

	if (MyFilePutNew(prompt, origName, d, s)) {
		IsOk = MyOpenNewFile(d, s, creator, fileType, refNum);
	}

	return IsOk;
}
#endif
