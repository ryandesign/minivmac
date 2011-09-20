/*
	FILEHUTL.i
	Copyright (C) 2009 Paul C. Pratt

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


GLOBALFUNC tMyErr MyFileCopyBytes_v2(short src_refnum,
	short dst_refnum, uimr L)
{
	tMyErr err;
	uimr maxn;
	uimr n;
	MyPtr p;

	if (noErr == (err = TempBuffHBeginUse_v2(&p))) {
		maxn = TempBuffSize;

doGetMore:
		if (0 != L) {
			if (L > maxn) {
				n = maxn;
			} else {
				n = L;
			}
			if (noErr == (err = CheckAbortRequested()))
			if (noErr == (err = MyReadBytes_v2(src_refnum, p, n)))
			if (noErr == (err = MyWriteBytes_v2(dst_refnum, p, n)))
			{
				L -= n;
				ProgressBarVal += n;
				goto doGetMore;
			}
		}

		TempBuffHEndUse();
	}

	return err;
}

GLOBALFUNC tMyErr MyCatInfoCopyRF_v2(CInfoPBRec *cPB,
	MyDir_R *dst_d, StringPtr dst_s)
{
	tMyErr err;
	short src_refnum;
	short dst_refnum;
	uimr n = cPB->hFileInfo.ioFlRLgLen;

	if (0 == n) {
		err = noErr;
	} else {
		if (noErr == (err = CatInfoOpenReadRF_v2(cPB, &src_refnum))) {
			if (noErr == (err = MyFileOpenRFWrite_v2(dst_d, dst_s,
				&dst_refnum)))
			{
				if (noErr == (err = MyFileCopyBytes_v2(src_refnum,
					dst_refnum, n)))
				{
				}
				err = CombineErr(err, MyCloseFile_v2(dst_refnum));
			}
			err = CombineErr(err, MyCloseFile_v2(src_refnum));
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyCatInfoCopyDF_v2(CInfoPBRec *cPB,
	MyDir_R *dst_d, StringPtr dst_s)
{
	tMyErr err;
	short src_refnum;
	short dst_refnum;
	uimr n = cPB->hFileInfo.ioFlLgLen;

	if (0 == n) {
		err = noErr;
	} else {
		if (noErr == (err = CatInfoOpenReadDF_v2(cPB, &src_refnum))) {
			if (noErr == (err = MyFileOpenWrite_v2(dst_d, dst_s,
				&dst_refnum)))
			{
				if (noErr == (err = MyFileCopyBytes_v2(src_refnum,
					dst_refnum, n)))
				{
				}
				err = CombineErr(err, MyCloseFile_v2(dst_refnum));
			}
			err = CombineErr(err, MyCloseFile_v2(src_refnum));
		}
	}

	return err;
}

GLOBALFUNC tMyErr MyCatInfoCopyFile_v2(CInfoPBRec *cPB,
	MyDir_R *dst_d, ps3p dst_s)
{
	tMyErr err;

	if (noErr == (err = MyCreateFile_v2(dst_d, dst_s))) {
		if (noErr == (err = MyCatInfoCopyRF_v2(cPB, dst_d, dst_s)))
		if (noErr == (err = MyCatInfoCopyDF_v2(cPB, dst_d, dst_s)))
		if (noErr == (err = MyCatInfoCopyInfo_v2(cPB, dst_d, dst_s)))
		{
		}
		if (noErr != err) {
			(void) MyDeleteFile_v2(dst_d, dst_s);
				/* ignore any error, since already got one */
		}
	}

	return err;
}
