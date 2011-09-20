/*
	DELLIBMC.i
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
	DELete files and folders LIBrary for MaCintosh
*/


LOCALFUNC tMyErr MyCatInfoDelete_v2(CInfoPBRec *cPB)
{
	HParamBlockRec r;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = cPB->hFileInfo.ioVRefNum;
	r.fileParam.ioDirID = cPB->hFileInfo.ioFlParID;
	r.fileParam.ioNamePtr = cPB->hFileInfo.ioNamePtr;
	return PBHDeleteSync(&r);
}

/*
	The dellib_r structure is used to minimize the amount of
	stack space used when recursively calling dellib_ProcessOne
	and to hold global information that might be needed at any time.
*/

struct dellib_r
{
	MyPtr SavepDt;
	CInfoPBRec cPB;
};

typedef struct dellib_r dellib_r;

#define dellib_gd() ((dellib_r *)(pDt))

LOCALFUNC tMyErr dellib_ProcessOne_v2(void)
{
	tMyErr err;

	if (! CatInfoIsFolder(&dellib_gd()->cPB)) {
		err = MyCatInfoDelete_v2(&dellib_gd()->cPB);
	} else {
		MyDir_R old_d;
		int index = 1;

		MyCatInfoGetMyDir(&dellib_gd()->cPB, &old_d);
Label_1:
		err = MyCatGetNextChild_v2(&dellib_gd()->cPB, &old_d, &index);
		if (noErr == err) {
			/* still has a child */
			if (noErr == (err = dellib_ProcessOne_v2())) {
				index = 1; /* old number 1 now gone */
				goto Label_1;
			}
		} else if (fnfErr == err) {
			/* folder is now empty */
			if (noErr == (err = MyFileGetCatInfo_v2(&old_d,
				NULL, NULL, &dellib_gd()->cPB)))
			{
				err = MyCatInfoDelete_v2(&dellib_gd()->cPB);
			}
		}
	}

	return err;
}

GLOBALFUNC tMyErr dellib_DeleteOne_v2(
	MyDir_R *d, StringPtr s)
{
	tMyErr err;
	MyPStr itemName;
	dellib_r r;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	if (noErr == (err = MyFileGetCatInfo_v2(d, s,
		itemName, &r.cPB)))
	if (noErr == (err = dellib_ProcessOne_v2()))
	{
	}

	pDt = r.SavepDt;

	return err;
}
