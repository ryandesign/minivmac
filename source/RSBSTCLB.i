/*
	RSBSTCLB.i
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
	Recursively SuBSTitute Character LiBrary
*/


LOCALPROC rsubstchar_ptr(ui3b *inbuff,
	uimr inputsize,
	ui3r fromchar, ui3r tochar)
{
	ui3b *p = inbuff;
	char c;
	long i;

	for (i = inputsize; --i >= 0; ) {
		c = *p;
		if (c == fromchar) {
			*p = tochar;
		}
		p++;
	}
}

/*
	The rsubstchar_r structure is used to minimize the amount of
	stack space used when recursively calling rsubstchar_ProcessOne_v2
	and to hold global information that might be needed at any time.
*/

struct rsubstchar_r
{
	MyPtr SavepDt;
	CInfoPBRec cPB;
	ui3r from_c;
	ui3r to_c;
};

typedef struct rsubstchar_r rsubstchar_r;

#define rsubstchar_gd() ((rsubstchar_r *)(pDt))


LOCALFUNC tMyErr rsubstchar_refnum_v2(short src_refnum,
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
			if (noErr == (err = MyReadBytes_v2(src_refnum, p, n))) {
				rsubstchar_ptr((ui3b *)p, n,
					rsubstchar_gd()->from_c,
					rsubstchar_gd()->to_c);
				if (noErr == (err = MyWriteBytes_v2(dst_refnum, p, n)))
				{
					L -= n;
					goto doGetMore;
				}
			}
		}

		TempBuffHEndUse();
	}

	return err;
}

LOCALFUNC tMyErr rsubstchar_DF_v2(CInfoPBRec *cPB, MyDir_R *dst_d,
	StringPtr dst_s)
{
	tMyErr err;
	short src_refnum;
	short dst_refnum;
	uimr n = cPB->hFileInfo.ioFlLgLen;

	if (0 == n) {
		err = noErr;
	} else if (noErr == (err = CatInfoOpenReadDF_v2(cPB, &src_refnum)))
	{
		if (noErr == (err = MyFileOpenWrite_v2(dst_d, dst_s,
			&dst_refnum)))
		{
			if (noErr == (err = rsubstchar_refnum_v2(src_refnum,
				dst_refnum, n)))
			{
			}
			err = CombineErr(err, MyCloseFile_v2(dst_refnum));
		}
		err = CombineErr(err, MyCloseFile_v2(src_refnum));
	}

	return err;
}

LOCALFUNC tMyErr rsubstchar_File_v2(CInfoPBRec *cPB,
	MyDir_R *dst_d, ps3p dst_s)
{
	tMyErr err;

	if (noErr == (err = MyCreateFile_v2(dst_d, dst_s))) {
		if (noErr == (err = rsubstchar_DF_v2(cPB, dst_d, dst_s)))
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

LOCALFUNC tMyErr rsubstchar_ProcessOne_v2(MyDir_R *dst_d, ps3p dst_s)
{
	tMyErr err;

	if (! CatInfoIsFolder(&rsubstchar_gd()->cPB)) {
		if ('TEXT'
			== rsubstchar_gd()->cPB.hFileInfo.ioFlFndrInfo.fdType)
		{
			err = rsubstchar_File_v2(&rsubstchar_gd()->cPB,
				dst_d, dst_s);
		} else {
			err = MyCatInfoCopyFile_v2(&rsubstchar_gd()->cPB,
				dst_d, dst_s);
		}
	} else {
		MyDir_R old_d;
		MyDir_R new_d;
		int index = 1;

		MyCatInfoGetMyDir(&rsubstchar_gd()->cPB, &old_d);
		if (noErr == (err = MyMakeNamedDir_v2(dst_d, dst_s, &new_d))) {

Label_1:
			err = MyCatGetNextChild_v2(&rsubstchar_gd()->cPB,
				&old_d, &index);
			if (noErr == err) {
				if (noErr == (err = rsubstchar_ProcessOne_v2(&new_d,
					rsubstchar_gd()->cPB.hFileInfo.ioNamePtr)))
				{
					goto Label_1;
				}
			} else if (fnfErr == err) {
				err = MyFileCopyFolderInfo_v2(&old_d, &new_d);
			}

			if (noErr != err) {
				(void) dellib_DeleteOne_v2(&new_d, NULL);
					/* ignore any error, since already got one */
			}
		}
	}

	return err;
}

GLOBALFUNC tMyErr rsubstchar_WriteFile_v2(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s,
	ui3r from_c, ui3r to_c)
{
	tMyErr err;
	MyPStr itemName;
	rsubstchar_r r;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	r.from_c = from_c;
	r.to_c = to_c;

	if (noErr == (err = MyFileGetCatInfo_v2(src_d, src_s,
		itemName, &r.cPB)))
	if (noErr == (err = rsubstchar_ProcessOne_v2(dst_d, dst_s)))
	{
		err = MyFileClearInitted_v2(dst_d, dst_s);

		if (noErr != err) {
			(void) dellib_DeleteOne_v2(dst_d, dst_s);
				/* ignore any error, since already got one */
		}
	}

	pDt = r.SavepDt;

	return err;
}
