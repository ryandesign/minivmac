/*
	RINSCAFT.i
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
	Recursively INSert Character AFTer character
*/


static void rinscharafter_ptr(ui3b *inbuff, ui3b *outbuff,
	uimr inputsize, uimr *outputsize,
	ui3r after_c, ui3r insert_c)
{
	ui3b *pSrc = inbuff;
	ui3b *pDst = outbuff;
	ui3b c;
	long i;

	for (i = inputsize; --i >= 0; ) {
		c = *pSrc++;
		*pDst++ = c;
		if (c == after_c) {
			*pDst++ = insert_c;
		}
	}
	*outputsize = (long)pDst - (long)outbuff;
}

/*
	The rinscharafter_r structure is used to minimize the amount of
	stack space used when recursively calling rinscharafter_ProcessOne
	and to hold global information that might be needed at any time.
*/

struct rinscharafter_r
{
	MyPtr SavepDt;
	CInfoPBRec cPB;
	ui3r after_c;
	ui3r insert_c;
};

typedef struct rinscharafter_r rinscharafter_r;

#define rinscharafter_gd() ((rinscharafter_r *)(pDt))


static blnr rinscharafter_refnum(short src_refnum, short dst_refnum, uimr L)
{
	uimr maxn;
	uimr n;
	MyPtr p;
	MyPtr pDst;
	uimr outputsize;
	blnr IsOk = falseblnr;

	if (TempBuffHBeginUse(&p)) {
		maxn = TempBuffSize >> 2;
		pDst = p + maxn;

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
				rinscharafter_ptr((ui3b *)p, (ui3b *)pDst,
					n, &outputsize,
					rinscharafter_gd()->after_c,
					rinscharafter_gd()->insert_c);
				if (MyWriteBytes(dst_refnum, pDst, outputsize)) {
					L -= n;
					goto doGetMore;
				}
			}
		}

		TempBuffHEndUse();
	}

	return IsOk;
}

static blnr rinscharafter_DF(CInfoPBRec *cPB, MyDir_R *dst_d, StringPtr dst_s)
{
	short src_refnum;
	short dst_refnum;
	uimr n = cPB->hFileInfo.ioFlLgLen;
	blnr IsOk = falseblnr;

	if (0 == n) {
		IsOk = trueblnr;
	} else if (CatInfoOpenReadDF(cPB, &src_refnum)) {
		if (MyFileOpenWrite(dst_d, dst_s, &dst_refnum)) {
			if (rinscharafter_refnum(src_refnum, dst_refnum, n)) {
				IsOk = trueblnr;
			}
			(void) MyCloseFile(dst_refnum);
		}
		(void) MyCloseFile(src_refnum);
	}

	return IsOk;
}

static blnr rinscharafter_File(CInfoPBRec *cPB, MyDir_R *dst_d, ps3p dst_s)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = dst_d->VRefNum;
	r.fileParam.ioDirID = dst_d->DirId;
	r.fileParam.ioNamePtr = dst_s;
	if (CheckSysErr(PBHCreate(&r, false))) {
		if (rinscharafter_DF(cPB, dst_d, dst_s))
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

static blnr rinscharafter_ProcessOne(MyDir_R *dst_d, ps3p dst_s)
{
	blnr IsOk = falseblnr;

	if (! CatInfoIsFolder(&rinscharafter_gd()->cPB)) {
		if ('TEXT' == rinscharafter_gd()->cPB.hFileInfo.ioFlFndrInfo.fdType) {
			IsOk = rinscharafter_File(&rinscharafter_gd()->cPB, dst_d, dst_s);
		} else {
			IsOk = MyCatInfoCopyFile(&rinscharafter_gd()->cPB, dst_d, dst_s);
		}
	} else {
		blnr FinishOk;
		MyDir_R old_d;
		MyDir_R new_d;
		int index = 1;

		MyCatInfoGetMyDir(&rinscharafter_gd()->cPB, &old_d);
		if (MyMakeNamedDir(dst_d, dst_s, &new_d)) {
			while (MyCatGetNextChild(&rinscharafter_gd()->cPB,
				&old_d, &index, &FinishOk)
				&& rinscharafter_ProcessOne(&new_d,
					rinscharafter_gd()->cPB.hFileInfo.ioNamePtr))
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

GLOBALFUNC blnr rinscharafter_WriteFile(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s,
	ui3r after_c, ui3r insert_c)
{
	MyPStr itemName;
	rinscharafter_r r;
	blnr IsOk = falseblnr;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	r.after_c = after_c;
	r.insert_c = insert_c;

	if (MyFileGetCatInfo(src_d, src_s,
		itemName, &r.cPB))
	if (rinscharafter_ProcessOne(dst_d, dst_s))
	{
		(void) MyFileClearInitted(dst_d, dst_s);
		IsOk = trueblnr;
	}

	pDt = r.SavepDt;

	return IsOk;
}
