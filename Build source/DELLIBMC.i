/*
	DELLIBMC.i
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
	DELete files and folders LIBrary for MaCintosh
*/


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

static blnr MyCatInfoDelete(CInfoPBRec *cPB)
{
	HParamBlockRec r;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = cPB->hfileInfo.ioVRefNum;
	r.fileParam.ioDirID = cPB->hfileInfo.ioFlParID;
	r.fileParam.ioNamePtr = cPB->hfileInfo.ioNamePtr;
	return CheckSysErr(PBHDelete(&r, false));
}

static blnr dellib_ProcessOne(void)
{
	blnr IsOk = falseblnr;

	if (! CatInfoIsFolder(&dellib_gd()->cPB)) {
		IsOk = MyCatInfoDelete(&dellib_gd()->cPB);
	} else {
		blnr FinishOk;
		MyDir_R old_d;
		int index = 1;

		MyCatInfoGetMyDir(&dellib_gd()->cPB, &old_d);
		while (MyCatGetNextChild(&dellib_gd()->cPB,
			&old_d, &index, &FinishOk)
			&& dellib_ProcessOne())
		{
			index = 1; /* old number 1 now gone */
		}
		if (FinishOk)
		if (MyFileGetCatInfo(&old_d, NULL, NULL,
			&dellib_gd()->cPB))
		{
			IsOk = MyCatInfoDelete(&dellib_gd()->cPB);
		}
	}

	return IsOk;
}

GLOBALFUNC blnr dellib_DeleteOne(
	MyDir_R *d, StringPtr s)
{
	MyPStr itemName;
	dellib_r r;
	blnr IsOk = falseblnr;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	if (MyFileGetCatInfo(d, s,
		itemName, &r.cPB))
	if (dellib_ProcessOne())
	{
		IsOk = trueblnr;
	}

	pDt = r.SavepDt;

	return IsOk;
}
