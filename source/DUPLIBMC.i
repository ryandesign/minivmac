/*
	DUPLIBMC.i
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


LOCALFUNC blnr duplib_ProcessOne_v2(MyDir_R *dst_d, ps3p dst_s)
{
	tMyErr err;

	if (! CatInfoIsFolder(&duplib_gd()->cPB)) {
		err = MyCatInfoCopyFile_v2(&duplib_gd()->cPB, dst_d, dst_s);
	} else {
		MyDir_R old_d;
		MyDir_R new_d;
		int index = 1;

		MyCatInfoGetMyDir(&duplib_gd()->cPB, &old_d);
		if (noErr == (err = MyMakeNamedDir_v2(dst_d, dst_s, &new_d))) {
Label_1:
			err = MyCatGetNextChild_v2(&duplib_gd()->cPB,
				&old_d, &index);
			if (noErr == err) {
				if (noErr == (err = duplib_ProcessOne_v2(&new_d,
					duplib_gd()->cPB.hFileInfo.ioNamePtr)))
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

GLOBALFUNC tMyErr duplib_WriteFile_v2(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s)
{
	tMyErr err;
	MyPStr itemName;
	duplib_r r;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	if (noErr == (err = MyFileGetCatInfo_v2(src_d, src_s,
		itemName, &r.cPB)))
	if (noErr == (err = duplib_ProcessOne_v2(dst_d, dst_s)))
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
