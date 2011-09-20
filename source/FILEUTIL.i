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

GLOBALFUNC tMyErr MyHGetDir_v2(MyDir_R *d)
{
	tMyErr err;
	WDPBRec r;

	r.ioCompletion = NULL;
	r.ioNamePtr = NULL;

#if Support64kROM
	if (Have64kROM()) {
		err = PBGetVolSync((ParamBlockRec *)&r);
		d->VRefNum = r.ioVRefNum;
		d->DirId = 0;
	} else
#endif
	{
		err = PBHGetVolSync(&r);
		d->VRefNum = r.ioWDVRefNum;
		d->DirId = r.ioWDDirID;
	}

	return err;
}

GLOBALFUNC tMyErr MyMakeNamedDir_v2(MyDir_R *d, StringPtr s,
	MyDir_R *new_d)
{
	tMyErr err;
	HParamBlockRec r;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioDirID = d->DirId;
	r.fileParam.ioNamePtr = s;
	err = PBDirCreateSync(&r);
	if (noErr == err) {
		new_d->VRefNum = d->VRefNum;
		new_d->DirId = r.fileParam.ioDirID;
	}

	return err;
}

GLOBALFUNC tMyErr MyDeleteFile_v2(MyDir_R *d, StringPtr s)
{
	tMyErr err;
	HParamBlockRec r;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioNamePtr = s;
	r.fileParam.ioFVersNum = 0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = PBDeleteSync((ParamBlockRec *)&r);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHDeleteSync(&r);
	}

	return err;
}

#define NotAfileRef (-1)

/*
	Probably should use PBHOpenDF instead
	of PBHOpen when it is available.
	(System 7 according to Technical Note FL515)
*/

GLOBALFUNC tMyErr MyFileOpen_v2(MyDir_R *d, StringPtr s,
	char Permssn, short *refnum)
{
	tMyErr err;
	HParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = s;
	r.ioParam.ioVRefNum = d->VRefNum;
	r.ioParam.ioPermssn = Permssn;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.ioParam.ioVersNum = 0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = PBOpenSync((ParamBlockRec *)&r);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHOpenSync(&r);
	}

	if (noErr == err) {
		*refnum = r.ioParam.ioRefNum;
		/*
			Don't change *refnum unless file opened,
			so can initialize to NotAfileRef, and
			compare later before closing in uninit.
		*/
	}
	return err;
}

GLOBALFUNC tMyErr MyOpenOldFileRead_v2(MyDir_R *d, StringPtr s,
	short *refnum)
{
	return MyFileOpen_v2(d, s, (char)fsRdPerm, refnum);
}

GLOBALFUNC tMyErr MyFileOpenWrite_v2(MyDir_R *d, StringPtr s,
	short *refnum)
{
	return MyFileOpen_v2(d, s, (char)fsWrPerm, refnum);
}

GLOBALFUNC tMyErr MyFileOpenRF(MyDir_R *d, StringPtr s,
	char Permssn, short *refnum)
{
	tMyErr err;
	HParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = s;
	r.ioParam.ioVRefNum = d->VRefNum;
	r.ioParam.ioPermssn = Permssn;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.ioParam.ioVersNum = 0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = PBOpenRFSync((ParamBlockRec *)&r);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHOpenRFSync(&r);
	}

	if (noErr == err) {
		*refnum = r.ioParam.ioRefNum;
		/*
			Don't change *refnum unless file opened,
			so can initialize to NotAfileRef, and
			compare later before closing in uninit.
		*/
	}
	return err;
}

GLOBALFUNC tMyErr MyFileOpenRFRead(MyDir_R *d, StringPtr s,
	short *refnum)
{
	return MyFileOpenRF(d, s, (char)fsRdPerm, refnum);
}

GLOBALFUNC tMyErr MyFileOpenRFWrite_v2(MyDir_R *d, StringPtr s,
	short *refnum)
{
	return MyFileOpenRF(d, s, (char)fsWrPerm, refnum);
}

GLOBALFUNC tMyErr MyCreateFile_v2(MyDir_R *d, StringPtr s)
{
	tMyErr err;
	HParamBlockRec r;

	r.fileParam.ioFlVersNum = 0;
		/*
			Think reference says to do this,
			but not Inside Mac IV
		*/

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioNamePtr = s;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioFVersNum = 0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = PBCreateSync((ParamBlockRec *)&r);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHCreateSync(&r);
	}

	return err;
}

GLOBALFUNC tMyErr MyCreateFileOverWrite_v2(MyDir_R *d, StringPtr s)
{
	tMyErr err;

	err = MyCreateFile_v2(d, s);
	if (dupFNErr == err) {
		if (noErr == (err = MyDeleteFile_v2(d, s))) {
			err = MyCreateFile_v2(d, s);
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyFileGetInfo_v2(MyDir_R *d, StringPtr s,
	HParamBlockRec *r)
{
	tMyErr err;

	r->fileParam.ioCompletion = NULL;
	r->fileParam.ioNamePtr = s;
	r->fileParam.ioVRefNum = d->VRefNum;
	r->fileParam.ioFVersNum = (char)0; /* needed if MFS volume */
	r->fileParam.ioFDirIndex = (short)0;

#if Support64kROM
	if (Have64kROM()) {
		err = PBGetFInfoSync((ParamBlockRec *)r);
	} else
#endif
	{
		r->fileParam.ioDirID = d->DirId;
		err = PBHGetFInfoSync(r);
	}

	return err;
}

GLOBALFUNC tMyErr MyFileSetInfo_v2(MyDir_R *d, StringPtr s,
	HParamBlockRec *r)
{
	tMyErr err;

	r->fileParam.ioCompletion = NULL;
	r->fileParam.ioNamePtr = s;
	r->fileParam.ioVRefNum = d->VRefNum;
	r->fileParam.ioFVersNum = (char)0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = PBSetFInfoSync((ParamBlockRec *)r);
	} else
#endif
	{
		r->fileParam.ioDirID = d->DirId;
		err = PBHSetFInfoSync(r);
	}

	return err;
}

GLOBALFUNC tMyErr MyFileSetTypeCreator_v2(MyDir_R *d, StringPtr s,
	OSType creator, OSType fileType)
{
	tMyErr err;
	HParamBlockRec r;

	if (noErr == (err = MyFileGetInfo_v2(d, s, &r))) {
		r.fileParam.ioFlFndrInfo.fdType = fileType;
		r.fileParam.ioFlFndrInfo.fdCreator = creator;
		err = MyFileSetInfo_v2(d, s, &r);
	}

	return err;
}

GLOBALFUNC tMyErr MyOpenNewFile_v2(MyDir_R *d, StringPtr s,
	OSType creator, OSType fileType,
	short *refnum)
{
	tMyErr err;

	err = MyCreateFile_v2(d, s);
	if (noErr == err) {
		err = MyFileSetTypeCreator_v2(d, s,
			creator, fileType);
		if (noErr == err) {
			err = MyFileOpenWrite_v2(d, s, refnum);
		}

		if (noErr != err) {
			(void) MyDeleteFile_v2(d, s);
				/* ignore any error, since already got one */
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyCloseFile_v2(short refNum)
{
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;

	return PBCloseSync(&r);
#if 0
	return (tMyErr)FSClose(refNum);
#endif
}

GLOBALFUNC tMyErr MyWriteBytes_v2(short refNum, MyPtr p, uimr L)
{
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;
	r.ioParam.ioBuffer = (Ptr)p;
	r.ioParam.ioReqCount = L;
	r.ioParam.ioPosMode = (short) fsFromMark;
	r.ioParam.ioPosOffset = 0;

	return PBWriteSync(&r);
}

GLOBALFUNC tMyErr MyReadBytes_v2(short refNum, MyPtr p, uimr L)
{
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;
	r.ioParam.ioBuffer = (Ptr)p;
	r.ioParam.ioReqCount = L;
	r.ioParam.ioPosMode = (short) fsFromMark;
	r.ioParam.ioPosOffset = 0;

	return PBReadSync(&r);
}

GLOBALFUNC tMyErr MyOpenFileGetFPos(short refNum, uimr *offset)
{
	tMyErr err;
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;

	err = PBGetFPosSync(&r);
	if (noErr == err) {
		*offset = r.ioParam.ioPosOffset;
	}

	return err;
#if 0
	return GetFPos(refnum, (long *)offset);
#endif
}

GLOBALFUNC tMyErr MyOpenFileSetFPos(short refNum, uimr offset)
{
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;
	r.ioParam.ioPosMode = fsFromStart;
	r.ioParam.ioPosOffset = offset;

	return PBSetFPosSync(&r);
#if 0
	return SetFPos(refnum, fsFromStart, (long)offset);
#endif
}

GLOBALFUNC tMyErr MyBackWriteBytes_v2(short refNum, uimr offset,
	MyPtr p, uimr L)
{
	tMyErr err;
	uimr savepos;

	if (noErr == (err = MyOpenFileGetFPos(refNum, &savepos)))
	if (noErr == (err = MyOpenFileSetFPos(refNum, offset)))
	if (noErr == (err = MyWriteBytes_v2(refNum, p, L)))
	if (noErr == (err = MyOpenFileSetFPos(refNum, savepos)))
	{
	}

	return err;
}

GLOBALFUNC tMyErr MyBackReadBytes_v2(short refNum, uimr offset,
	MyPtr p, uimr L)
{
	tMyErr err;
	uimr savepos;

	if (noErr == (err = MyOpenFileGetFPos(refNum, &savepos)))
	if (noErr == (err = MyOpenFileSetFPos(refNum, offset)))
	if (noErr == (err = MyReadBytes_v2(refNum, p, L)))
	if (noErr == (err = MyOpenFileSetFPos(refNum, savepos)))
	{
	}

	return err;
}

GLOBALFUNC tMyErr MyOpenFileGetEOF_v2(short refNum, uimr *L)
{
	tMyErr err;
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;

	err = PBGetEOFSync(&r);
	if (noErr == err) {
		*L = (uimr)r.ioParam.ioMisc;
	}

	return err;
#if 0
	return GetEOF(refNum, (long *)L);
#endif
}

GLOBALFUNC tMyErr MyOpenFileSetEOF_v2(short refNum, uimr L)
{
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;
	r.ioParam.ioMisc = (Ptr)L;

	return PBSetEOFSync(&r);
#if 0
	return SetEOF(refNum, (long)L);
#endif
}

GLOBALFUNC tMyErr MyFileFlushVol(
	ConstStr63Param   volName,       /* can be NULL */
	short             vRefNum)
{
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = (StringPtr)volName;
	r.ioParam.ioVRefNum = vRefNum;

	return PBFlushVolSync(&r);
}

GLOBALFUNC tMyErr MyFileFlush_v2(short refnum)
{
	tMyErr err;
	short vRefNum;

	if (noErr == (err = GetVRefNum(refnum, &vRefNum))) {
		err = MyFileFlushVol(NULL, vRefNum);
	}

	return err;
}

GLOBALFUNC tMyErr MyOpenFileUpdtLocation_v3(short refNum,
	MyDir_R *old_d, ps3p old_s, MyDir_R *new_d, ps3p new_s)
{
	/*
		because the user could move or rename the
		file while the program is working on it.

		new_d/s and old_d/s may be the same variables.
	*/
	tMyErr err;

#if ! Support64kROM
#pragma unused(old_d, old_s)
#endif

#if Support64kROM
	if (Have64kROM()) {
		/*
			Do nothing. Volume of open file can't
			change. And haven't figured out a
			way to get name.
		*/
		if (old_s != new_s) {
			PStrCopy(new_s, old_s);
		}
		if (old_d != new_d) {
			new_d->VRefNum = old_d->VRefNum;
			new_d->DirId = old_d->DirId;
		}
		err = noErr;
	} else
#endif
	{
		FCBPBRec b;

		b.ioCompletion = NULL;
		b.ioNamePtr = (StringPtr)new_s;
		b.ioVRefNum = 0;
		b.ioRefNum = refNum;
		b.ioFCBIndx = 0;
		if (noErr == (err = PBGetFCBInfoSync(&b))) {
			new_d->VRefNum = b.ioFCBVRefNum;
			new_d->DirId = b.ioFCBParID;
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyCloseNewFile_v3(short refNum, MyDir_R *d, ps3p s,
	tMyErr err0)
{
	tMyErr err;
	tMyErr err2;
	MyDir_R new_d;
	MyPStr new_s;

	err2 = MyOpenFileUpdtLocation_v3(refNum,
		d, s, &new_d, new_s);
	err = CombineErr(err0, err2);
	err = CombineErr(err, MyCloseFile_v2(refNum));
	if (noErr != err) {
		if (noErr == err2) {
			(void) MyDeleteFile_v2(&new_d, new_s);
				/* ignore any error, since already got one */
		}
	}

	return err;
}

#define CatInfoIsFolder(cPB) \
	(((cPB)->hFileInfo.ioFlAttrib & kioFlAttribDirMask) != 0)

GLOBALFUNC tMyErr CatInfoOpenReadDF_v2(CInfoPBRec *cPB,
	short *refnum)
{
	tMyErr err;
	HParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = cPB->hFileInfo.ioNamePtr;
	r.ioParam.ioVRefNum = cPB->hFileInfo.ioVRefNum;
	r.ioParam.ioPermssn = (char)fsRdPerm;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.fileParam.ioDirID = cPB->hFileInfo.ioFlParID;
	err = PBHOpenSync(&r);
	if (noErr == err) {
		*refnum = r.ioParam.ioRefNum;
	}
	return err;
}

GLOBALFUNC tMyErr CatInfoOpenReadRF_v2(CInfoPBRec *cPB,
	short *refnum)
{
	tMyErr err;
	HParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = cPB->hFileInfo.ioNamePtr;
	r.ioParam.ioVRefNum = cPB->hFileInfo.ioVRefNum;
	r.ioParam.ioPermssn = (char)fsRdPerm;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.fileParam.ioDirID = cPB->hFileInfo.ioFlParID;
	err = PBHOpenRFSync(&r);
	if (noErr == err) {
		*refnum = r.ioParam.ioRefNum;
	}
	return err;
}

GLOBALFUNC tMyErr MyFileGetCatInfo_v2(MyDir_R *d, StringPtr s,
	StringPtr NameBuffer, CInfoPBRec *cPB)
{
	cPB->hFileInfo.ioCompletion = NULL;
	cPB->hFileInfo.ioVRefNum = d->VRefNum;
	cPB->dirInfo.ioDrDirID = d->DirId;
	if (NULL == s) {
		if (NULL == NameBuffer) {
			/*
				then this field must already be
				set up by caller
			*/
		} else {
			cPB->hFileInfo.ioNamePtr = NameBuffer;
		}
		cPB->dirInfo.ioFDirIndex = (short)(- 1);
	} else {
		if (NULL == NameBuffer) {
			cPB->hFileInfo.ioNamePtr = s;
		} else {
			cPB->hFileInfo.ioNamePtr = NameBuffer;
			PStrCopy(NameBuffer, s);
		}
		if (0 == PStrLength(s)) {
			cPB->dirInfo.ioFDirIndex = (short)(- 1);
		} else {
			cPB->dirInfo.ioFDirIndex = 0;
		}
	}

	return PBGetCatInfoSync(cPB);
}

GLOBALFUNC tMyErr MyFileExists_v2(MyDir_R *d, StringPtr s,
	blnr *Exists)
{
	tMyErr err;
	MyPStr NameBuffer;
	CInfoPBRec cPB;

	err = MyFileGetCatInfo_v2(d, s, NameBuffer, &cPB);
	if (noErr == err) {
		*Exists = trueblnr;
	} else if (fnfErr == err) {
		*Exists = falseblnr;
		err = noErr;
	}

	return err;
}

GLOBALFUNC tMyErr MyCatInfoCopyInfo_v2(CInfoPBRec *cPB,
	MyDir_R *dst_d, StringPtr dst_s)
{
	tMyErr err;
	MyPStr NameBuffer;
	CInfoPBRec r;

	if (noErr == (err = MyFileGetCatInfo_v2(
		dst_d, dst_s, NameBuffer, &r)))
	{

		r.hFileInfo.ioFlFndrInfo.fdType =
			cPB->hFileInfo.ioFlFndrInfo.fdType;
		r.hFileInfo.ioFlFndrInfo.fdCreator =
			cPB->hFileInfo.ioFlFndrInfo.fdCreator;
			/* or frRect for folder */
		r.hFileInfo.ioFlFndrInfo.fdLocation =
			cPB->hFileInfo.ioFlFndrInfo.fdLocation;
			/* or frLocation for folder */
		r.hFileInfo.ioFlFndrInfo.fdFlags =
			cPB->hFileInfo.ioFlFndrInfo.fdFlags;
			/*
				(0x0100 & r.hFileInfo.ioFlFndrInfo.fdFlags)
				|
				(0xFEFF & cPB->hFileInfo.ioFlFndrInfo.fdFlags)
			*/;
			/* or frFlags */

		r.hFileInfo.ioFlCrDat = cPB->hFileInfo.ioFlCrDat;
		r.hFileInfo.ioFlMdDat = cPB->hFileInfo.ioFlMdDat;
		r.hFileInfo.ioFlBkDat = cPB->hFileInfo.ioFlBkDat;
			/* or ioDrCrDat, ioDrMdDat, and ioDrBkDat */

		if (CatInfoIsFolder(cPB)) {
			r.dirInfo.ioDrFndrInfo.frScroll =
				cPB->dirInfo.ioDrFndrInfo.frScroll;
			r.dirInfo.ioDrUsrWds.frView =
				cPB->dirInfo.ioDrUsrWds.frView;
		}

		r.hFileInfo.ioDirID = r.hFileInfo.ioFlParID;

		err = PBSetCatInfoSync(&r);
	}

	return err;
}

GLOBALFUNC tMyErr MyFileClearInitted_v2(
	MyDir_R *dst_d, StringPtr dst_s)
{
	tMyErr err;
	MyPStr NameBuffer;
	CInfoPBRec r;

	if (noErr == (err = MyFileGetCatInfo_v2(dst_d, dst_s,
		NameBuffer, &r)))
	{

		r.hFileInfo.ioFlFndrInfo.fdFlags &= 0xFEFF;
			/* or frFlags */

		r.hFileInfo.ioDirID = r.hFileInfo.ioFlParID;

		err = PBSetCatInfoSync(&r);
	}

	return err;
}

GLOBALFUNC tMyErr MyFileCopyFolderInfo_v2(MyDir_R *src_d,
	MyDir_R *dst_d)
{
	tMyErr err;
	MyPStr NameBuffer;
	CInfoPBRec cPB;

	if (noErr == (err = MyFileGetCatInfo_v2(src_d, NULL, NameBuffer,
		&cPB)))
	if (noErr == (err = MyCatInfoCopyInfo_v2(&cPB, dst_d, NULL)))
	{
	}

	return err;
}

GLOBALPROC MyCatInfoGetMyDir(CInfoPBRec *cPB, MyDir_R *d)
/* assumes CatInfoIsFolder(cPB) */
{
	d->VRefNum = cPB->hFileInfo.ioVRefNum;
	d->DirId = cPB->dirInfo.ioDrDirID;
}

GLOBALFUNC tMyErr MyCatGetNextChild_v2(CInfoPBRec *cPB,
	MyDir_R *old_d, int *index)
{
	cPB->dirInfo.ioFDirIndex = (*index)++;
	cPB->dirInfo.ioDrDirID = old_d->DirId;
	return PBGetCatInfoSync(cPB);
}

GLOBALFUNC tMyErr MyGetNamedVolDir(StringPtr s, MyDir_R *d)
{
	tMyErr err;
	ParamBlockRec r;
	MyPStr NameBuffer;

	PStrCopy(NameBuffer, s);
	PStrApndChar(NameBuffer, ':');

	r.volumeParam.ioCompletion = NULL;
	r.volumeParam.ioNamePtr = NameBuffer;
	r.volumeParam.ioVRefNum = 0;
	r.volumeParam.ioVolIndex = -1;
	err = PBGetVInfoSync(&r);
	if (noErr == err) {
		d->VRefNum = r.volumeParam.ioVRefNum;
		d->DirId = 2;
	}

	return err;
}

GLOBALFUNC tMyErr MyFindParentDir(MyDir_R *src_d, MyDir_R *dst_d)
{
	tMyErr err;
	MyPStr NameBuffer;
	CInfoPBRec cPB;

	err = MyFileGetCatInfo_v2(src_d, NULL, NameBuffer, &cPB);
	if (noErr == err) {
		if (! CatInfoIsFolder(&cPB)) {
			err = dirNFErr;
		} else {
			dst_d->VRefNum = cPB.hFileInfo.ioVRefNum;
			dst_d->DirId = cPB.hFileInfo.ioFlParID;
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyFindDirNameParentDir(MyDir_R *src_d,
	MyDir_R *dst_d, StringPtr dst_s)
{
	tMyErr err;
	CInfoPBRec cPB;

	err = MyFileGetCatInfo_v2(src_d, NULL, dst_s, &cPB);
	if (noErr == err) {
		if (! CatInfoIsFolder(&cPB)) {
			err = dirNFErr;
		} else {
			dst_d->VRefNum = cPB.hFileInfo.ioVRefNum;
			dst_d->DirId = cPB.hFileInfo.ioFlParID;
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyFindNamedChildDir_v2(MyDir_R *src_d, StringPtr s,
	MyDir_R *dst_d)
{
	tMyErr err;
	MyPStr NameBuffer;
	CInfoPBRec cPB;

	err = MyFileGetCatInfo_v2(src_d, s, NameBuffer, &cPB);
	if (noErr == err) {
		if (! CatInfoIsFolder(&cPB)) {
			err = dirNFErr;
		} else {
			MyCatInfoGetMyDir(&cPB, dst_d);
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyResolveAliasDir_v2(MyDir_R *src_d, StringPtr s,
	MyDir_R *dst_d)
{
	tMyErr err;
	FSSpec spec;
	Boolean isFolder;
	Boolean isAlias;
	MyDir_R src2_d;

	spec.vRefNum = src_d->VRefNum;
	spec.parID = src_d->DirId;
	PStrCopy(spec.name, s);
	err = ResolveAliasFile(&spec, true, &isFolder, &isAlias);
	if (noErr == err) {
		if (! isAlias) {
			err = dirNFErr;
		} else {
			src2_d.VRefNum = spec.vRefNum;
			src2_d.DirId = spec.parID;
			err = MyFindNamedChildDir_v2(&src2_d, spec.name, dst_d);
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyResolveNamedChildDir_v2(MyDir_R *src_d, StringPtr s,
	MyDir_R *dst_d)
{
	tMyErr err;

	err = MyFindNamedChildDir_v2(src_d, s, dst_d);
	if (dirNFErr == err) {
		if (HaveAliasMgrAvail()) {
			err = MyResolveAliasDir_v2(src_d, s, dst_d);
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyResolveIfAlias(MyDir_R *d, StringPtr s)
{
	tMyErr err;
	FSSpec spec;
	Boolean isFolder;
	Boolean isAlias;

	if ((! HaveAliasMgrAvail())
		|| (0 == PStrLength(s)))
			/* means the directory, which can't be alias anyway */
	{
		err = noErr;
	} else {
		spec.vRefNum = d->VRefNum;
		spec.parID = d->DirId;
		PStrCopy(spec.name, s);
		err = ResolveAliasFile(&spec, true, &isFolder, &isAlias);
		if (noErr == err) {
			if (isAlias) {
				d->VRefNum = spec.vRefNum;
				d->DirId = spec.parID;
				PStrCopy(s, spec.name);
			}
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyDirFromWD_v2(short VRefNum, MyDir_R *d)
{
	tMyErr err;
	Str63 s;
	WDPBRec pb;

#if Support64kROM
	if (Have64kROM()) {
		d->VRefNum = VRefNum;
		d->DirId = 0;
		err = noErr;
	} else
#endif
	{
		pb.ioCompletion = NULL;
		pb.ioNamePtr = s;
		pb.ioVRefNum = VRefNum;
		pb.ioWDIndex = 0;
		pb.ioWDProcID = 0;
		err = PBGetWDInfoSync(&pb);
		if (noErr == err) {
			d->VRefNum = pb.ioWDVRefNum;
			d->DirId = pb.ioWDDirID;
		}
	}

	return err;
}

#ifdef Have_MACINITS
GLOBALFUNC tMyErr MyFilePutNew_v2(StringPtr prompt, StringPtr origName,
		MyDir_R *d, StringPtr s)
{
	tMyErr err;

	if (! HaveCustomPutFileAvail()) {
		SFReply reply;
		Point tempPt;

		tempPt.h = 100;
		tempPt.v = 100;
		SFPutFile(tempPt, prompt, origName, NULL, &reply);
		if (! reply.good) {
			err = kMyErrUsrCancel;
		} else {
			PStrCopy(s, reply.fName);
			err = MyDirFromWD_v2(reply.vRefNum, d);
		}
	} else {
		StandardFileReply reply;

		StandardPutFile(prompt, origName, &reply);
		if (! reply.sfGood) {
			err = kMyErrUsrCancel;
		} else {
			d->VRefNum = reply.sfFile.vRefNum;
			d->DirId = reply.sfFile.parID;
			PStrCopy(s, reply.sfFile.name);
			err = noErr;
		}
	}

	return err;
}
#endif

#ifdef Have_MACINITS
GLOBALFUNC tMyErr MyFileGetOld_v2(simr nInputTypes,
		ConstSFTypeListPtr pfInputType,
		MyDir_R *d, StringPtr s)
{
	tMyErr err;

	if (! HaveCustomPutFileAvail()) {
		SFReply reply;
		Point tempPt;

		tempPt.h = 50;
		tempPt.v = 50;
		SFGetFile(tempPt, (ConstStr255Param)"\p", NULL, nInputTypes,
			pfInputType, NULL, &reply);
		if (! reply.good) {
			err = kMyErrUsrCancel;
		} else {
			PStrCopy(s, reply.fName);
			err = MyDirFromWD_v2(reply.vRefNum, d);
		}
	} else {
		StandardFileReply reply;

		StandardGetFile(NULL, nInputTypes, pfInputType, &reply);
		if (! reply.sfGood) {
			err = kMyErrUsrCancel;
		} else {
			d->VRefNum = reply.sfFile.vRefNum;
			d->DirId = reply.sfFile.parID;
			PStrCopy(s, reply.sfFile.name);
			err = noErr;
		}
	}

	return err;
}
#endif

#ifdef Have_MACINITS
GLOBALFUNC tMyErr CreateOpenNewFile_v2(StringPtr prompt,
	StringPtr origName, OSType creator, OSType fileType,
	MyDir_R *d, StringPtr s, short *refNum)
{
	tMyErr err;

	err = MyFilePutNew_v2(prompt, origName, d, s);
	if (noErr == err) {
		err = MyOpenNewFile_v2(d, s, creator, fileType, refNum);
	}

	return err;
}
#endif
