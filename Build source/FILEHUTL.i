/*
	FILEHUTL.i
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
	FILE Higher level UTiLilities
*/


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
			CheckAbort();
			if (! AbortRequested)
			if (MyReadBytes(src_refnum, p, n))
			if (MyWriteBytes(dst_refnum, p, n))
			{
				L -= n;
				ProgressBarVal += n;
				goto doGetMore;
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

#ifdef HaveTempBuffH
GLOBALFUNC blnr MyCatInfoCopyFile(CInfoPBRec *cPB, MyDir_R *dst_d, ps3p dst_s)
{
	blnr IsOk = falseblnr;

	if (MyCreateFile(dst_d, dst_s)) {
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
