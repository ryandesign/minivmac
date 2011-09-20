/*
	RINSCAFT.i
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
	Recursively INSert Character AFTer character
*/


LOCALPROC rinscharafter_ptr(ui3b *inbuff, ui3b *outbuff,
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


LOCALFUNC tMyErr rinscharafter_refnum_v2(short src_refnum,
	short dst_refnum, uimr L)
{
	tMyErr err;
	uimr maxn;
	uimr n;
	MyPtr p;
	MyPtr pDst;
	uimr outputsize;

	if (noErr == (err = TempBuffHBeginUse_v2(&p))) {
		maxn = TempBuffSize >> 2;
		pDst = p + maxn;

doGetMore:
		if (0 != L) {
			if (L > maxn) {
				n = maxn;
			} else {
				n = L;
			}
			if (noErr == (err = MyReadBytes_v2(src_refnum, p, n))) {
				rinscharafter_ptr((ui3b *)p, (ui3b *)pDst,
					n, &outputsize,
					rinscharafter_gd()->after_c,
					rinscharafter_gd()->insert_c);
				if (noErr == (err = MyWriteBytes_v2(dst_refnum,
					pDst, outputsize)))
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

LOCALFUNC tMyErr rinscharafter_DF_v2(CInfoPBRec *cPB,
	MyDir_R *dst_d, StringPtr dst_s)
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
			if (noErr == (err = rinscharafter_refnum_v2(src_refnum,
				dst_refnum, n))) {
			}
			err = CombineErr(err, MyCloseFile_v2(dst_refnum));
		}
		err = CombineErr(err, MyCloseFile_v2(src_refnum));
	}

	return err;
}

LOCALFUNC tMyErr rinscharafter_File_v2(CInfoPBRec *cPB,
	MyDir_R *dst_d, ps3p dst_s)
{
	tMyErr err;

	if (noErr == (err = MyCreateFile_v2(dst_d, dst_s))) {
		if (noErr == (err = rinscharafter_DF_v2(cPB, dst_d, dst_s)))
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

LOCALFUNC tMyErr rinscharafter_ProcessOne_v2(
	MyDir_R *dst_d, ps3p dst_s)
{
	tMyErr err;

	if (! CatInfoIsFolder(&rinscharafter_gd()->cPB)) {
		if ('TEXT'
			== rinscharafter_gd()->cPB.hFileInfo.ioFlFndrInfo.fdType)
		{
			err = rinscharafter_File_v2(&rinscharafter_gd()->cPB,
				dst_d, dst_s);
		} else {
			err = MyCatInfoCopyFile_v2(&rinscharafter_gd()->cPB,
				dst_d, dst_s);
		}
	} else {
		MyDir_R old_d;
		MyDir_R new_d;
		int index = 1;

		MyCatInfoGetMyDir(&rinscharafter_gd()->cPB, &old_d);
		if (noErr == (err = MyMakeNamedDir_v2(dst_d, dst_s, &new_d))) {

Label_1:
			err = MyCatGetNextChild_v2(&rinscharafter_gd()->cPB,
				&old_d, &index);
			if (noErr == err) {
				if (noErr == (err = rinscharafter_ProcessOne_v2(&new_d,
					rinscharafter_gd()->cPB.hFileInfo.ioNamePtr)))
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

GLOBALFUNC tMyErr rinscharafter_WriteFile_v2(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s,
	ui3r after_c, ui3r insert_c)
{
	tMyErr err;
	MyPStr itemName;
	rinscharafter_r r;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	r.after_c = after_c;
	r.insert_c = insert_c;

	if (noErr == (err = MyFileGetCatInfo_v2(src_d, src_s,
		itemName, &r.cPB)))
	if (noErr == (err = rinscharafter_ProcessOne_v2(dst_d, dst_s)))
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
