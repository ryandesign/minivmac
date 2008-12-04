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


static void rsubstcharwith2_ptr(ui3b *inbuff, ui3b *outbuff,
	uimr inputsize, uimr *outputsize,
	ui3r find_c, ui3r subst1_c, ui3r subst2_c)
{
	ui3b *pSrc = inbuff;
	ui3b *pDst = outbuff;
	ui3b c;
	long i;

	for (i = inputsize; --i >= 0; ) {
		c = *pSrc++;
		if (c == find_c) {
			*pDst++ = subst1_c;
			*pDst++ = subst2_c;
		} else {
			*pDst++ = c;
		}
	}
	*outputsize = (long)pDst - (long)outbuff;
}

/*
	The rsubstcharwith2_r structure is used to minimize the amount of
	stack space used when recursively calling rsubstcharwith2_ProcessOne
	and to hold global information that might be needed at any time.
*/

struct rsubstcharwith2_r
{
	MyPtr SavepDt;
	CInfoPBRec cPB;
	ui3r find_c;
	ui3r subst1_c;
	ui3r subst2_c;
};

typedef struct rsubstcharwith2_r rsubstcharwith2_r;

#define rsubstcharwith2_gd() ((rsubstcharwith2_r *)(pDt))


static blnr rsubstcharwith2_refnum(short src_refnum, short dst_refnum, uimr L)
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
				rsubstcharwith2_ptr((ui3b *)p, (ui3b *)pDst,
					n, &outputsize,
					rsubstcharwith2_gd()->find_c,
					rsubstcharwith2_gd()->subst1_c,
					rsubstcharwith2_gd()->subst2_c);
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

static blnr rsubstcharwith2_DF(CInfoPBRec *cPB, MyDir_R *dst_d, StringPtr dst_s)
{
	short src_refnum;
	short dst_refnum;
	uimr n = cPB->hFileInfo.ioFlLgLen;
	blnr IsOk = falseblnr;

	if (0 == n) {
		IsOk = trueblnr;
	} else if (CatInfoOpenReadDF(cPB, &src_refnum)) {
		if (MyFileOpenWrite(dst_d, dst_s, &dst_refnum)) {
			if (rsubstcharwith2_refnum(src_refnum, dst_refnum, n)) {
				IsOk = trueblnr;
			}
			(void) MyCloseFile(dst_refnum);
		}
		(void) MyCloseFile(src_refnum);
	}

	return IsOk;
}

static blnr rsubstcharwith2_File(CInfoPBRec *cPB, MyDir_R *dst_d, ps3p dst_s)
{
	HParamBlockRec r;
	blnr IsOk = falseblnr;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = dst_d->VRefNum;
	r.fileParam.ioDirID = dst_d->DirId;
	r.fileParam.ioNamePtr = dst_s;
	if (CheckSysErr(PBHCreate(&r, false))) {
		if (rsubstcharwith2_DF(cPB, dst_d, dst_s))
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

static blnr rsubstcharwith2_ProcessOne(MyDir_R *dst_d, ps3p dst_s)
{
	blnr IsOk = falseblnr;

	if (! CatInfoIsFolder(&rsubstcharwith2_gd()->cPB)) {
		if ('TEXT' == rsubstcharwith2_gd()->cPB.hFileInfo.ioFlFndrInfo.fdType) {
			IsOk = rsubstcharwith2_File(&rsubstcharwith2_gd()->cPB, dst_d, dst_s);
		} else {
			IsOk = MyCatInfoCopyFile(&rsubstcharwith2_gd()->cPB, dst_d, dst_s);
		}
	} else {
		blnr FinishOk;
		MyDir_R old_d;
		MyDir_R new_d;
		int index = 1;

		MyCatInfoGetMyDir(&rsubstcharwith2_gd()->cPB, &old_d);
		if (MyMakeNamedDir(dst_d, dst_s, &new_d)) {
			while (MyCatGetNextChild(&rsubstcharwith2_gd()->cPB,
				&old_d, &index, &FinishOk)
				&& rsubstcharwith2_ProcessOne(&new_d,
					rsubstcharwith2_gd()->cPB.hFileInfo.ioNamePtr))
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

GLOBALFUNC blnr rsubstcharwith2_WriteFile(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s,
	ui3r find_c, ui3r subst1_c, ui3r subst2_c)
{
	MyPStr itemName;
	rsubstcharwith2_r r;
	blnr IsOk = falseblnr;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	r.find_c = find_c;
	r.subst1_c = subst1_c;
	r.subst2_c = subst2_c;

	if (MyFileGetCatInfo(src_d, src_s,
		itemName, &r.cPB))
	if (rsubstcharwith2_ProcessOne(dst_d, dst_s))
	{
		(void) MyFileClearInitted(dst_d, dst_s);
		IsOk = trueblnr;
	}

	pDt = r.SavepDt;

	return IsOk;
}
