/*
	RSBSTCLB.i
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
	Recursively SuBSTitute Character LiBrary
*/


void rsubstchar_ptr(ui3b *inbuff,
	uimr inputsize,
	ui3r fromchar, ui3r tochar)
{
	char *p = inbuff;
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
	The duplib_r structure is used to minimize the amount of
	stack space used when recursively calling duplib_ProcessOne
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


static blnr rsubstchar_refnum(short src_refnum, short dst_refnum, uimr L)
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
				rsubstchar_ptr((ui3b *)p, n,
					rsubstchar_gd()->from_c,
					rsubstchar_gd()->to_c);
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

static blnr rsubstchar_DF(CInfoPBRec *cPB, MyDir_R *dst_d, StringPtr dst_s)
{
	short src_refnum;
	short dst_refnum;
	uimr n = cPB->hfileInfo.ioFlLgLen;
	blnr IsOk = falseblnr;

	if (0 == n) {
		IsOk = trueblnr;
	} else if (CatInfoOpenReadDF(cPB, &src_refnum)) {
		if (MyFileOpenWrite(dst_d, dst_s, &dst_refnum)) {
			if (rsubstchar_refnum(src_refnum, dst_refnum, n)) {
				IsOk = trueblnr;
			}
			(void) MyCloseFile(dst_refnum);
		}
		(void) MyCloseFile(src_refnum);
	}

	return IsOk;
}

static blnr rsubstchar_File(CInfoPBRec *cPB, MyDir_R *dst_d, ps3p dst_s)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = dst_d->VRefNum;
	r.fileParam.ioDirID = dst_d->DirId;
	r.fileParam.ioNamePtr = dst_s;
	if (CheckSysErr(PBHCreate(&r, false))) {
		if (rsubstchar_DF(cPB, dst_d, dst_s))
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

static blnr rsubstchar_ProcessOne(MyDir_R *dst_d, ps3p dst_s)
{
	blnr IsOk = falseblnr;

	if (! CatInfoIsFolder(&rsubstchar_gd()->cPB)) {
		if ('TEXT' == rsubstchar_gd()->cPB.hfileInfo.ioFlFndrInfo.fdType) {
			IsOk = rsubstchar_File(&rsubstchar_gd()->cPB, dst_d, dst_s);
		} else {
			IsOk = MyCatInfoCopyFile(&rsubstchar_gd()->cPB, dst_d, dst_s);
		}
	} else {
		blnr FinishOk;
		MyDir_R old_d;
		MyDir_R new_d;
		int index = 1;

		MyCatInfoGetMyDir(&rsubstchar_gd()->cPB, &old_d);
		if (MyMakeNamedDir(dst_d, dst_s, &new_d)) {
			while (MyCatGetNextChild(&rsubstchar_gd()->cPB,
				&old_d, &index, &FinishOk)
				&& rsubstchar_ProcessOne(&new_d,
					rsubstchar_gd()->cPB.hfileInfo.ioNamePtr))
			{
			}
			if (FinishOk) {
				IsOk = MyFileCopyFolderInfo(&old_d, &new_d);
			}
			if (! IsOk) {
				(void) dellib_DeleteOne(&new_d, NULL);
			}
		}
	}

	return IsOk;
}

GLOBALFUNC blnr rsubstchar_WriteFile(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s,
	ui3r from_c, ui3r to_c)
{
	MyPStr itemName;
	rsubstchar_r r;
	blnr IsOk = falseblnr;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	r.from_c = from_c;
	r.to_c = to_c;

	if (MyFileGetCatInfo(src_d, src_s,
		itemName, &r.cPB))
	if (rsubstchar_ProcessOne(dst_d, dst_s))
	{
		(void) MyFileClearInitted(dst_d, dst_s);
		IsOk = trueblnr;
	}

	pDt = r.SavepDt;

	return IsOk;
}
