/*
	DUPLIBMC.i
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
	DUPlicate files and folders LIBrary for MaCintosh
*/


/*
	The duplib_r structure is used to minimize the amount of
	stack space used when recursively calling duplib_ProcessOne
	and to hold global information that might be needed at any time.
*/

struct duplib_r
{
	MyPtr SavepDt;
	CInfoPBRec cPB;
};

typedef struct duplib_r duplib_r;

#define duplib_gd() ((duplib_r *)(pDt))


static blnr duplib_ProcessOne(MyDir_R *dst_d, ps3p dst_s)
{
	blnr IsOk = falseblnr;

	if (! CatInfoIsFolder(&duplib_gd()->cPB)) {
		IsOk = MyCatInfoCopyFile(&duplib_gd()->cPB, dst_d, dst_s);
	} else {
		blnr FinishOk;
		MyDir_R old_d;
		MyDir_R new_d;
		int index = 1;

		MyCatInfoGetMyDir(&duplib_gd()->cPB, &old_d);
		if (MyMakeNamedDir(dst_d, dst_s, &new_d)) {
			while (MyCatGetNextChild(&duplib_gd()->cPB,
				&old_d, &index, &FinishOk)
				&& duplib_ProcessOne(&new_d,
					duplib_gd()->cPB.hfileInfo.ioNamePtr))
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

GLOBALFUNC blnr duplib_WriteFile(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s)
{
	MyPStr itemName;
	duplib_r r;
	blnr IsOk = falseblnr;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	if (MyFileGetCatInfo(src_d, src_s,
		itemName, &r.cPB))
	if (duplib_ProcessOne(dst_d, dst_s))
	{
		(void) MyFileClearInitted(dst_d, dst_s);
		IsOk = trueblnr;
	}

	pDt = r.SavepDt;

	return IsOk;
}
