/*
	EXTNSLIB.i

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
	mini vmac EXTeNSions LIBrary
*/


#define kCallCheckValFails (-1)


#define SonyVarsPtr 0x0134

#define kcom_checkval 0x841339E2
#define kcom_callcheck 0x5B17

struct MyDriverDat_R {
	ui5b zeroes[4];
	ui5b checkval;
	ui5b pokeaddr;
};
typedef struct MyDriverDat_R MyDriverDat_R;

#define kFindExtnExtension 0x64E1F58A

#define DSKDat_checkval 0
#define DSKDat_extension 2
#define DSKDat_commnd 4
#define DSKDat_result 6
#define DSKDat_params 8
#define DSKDat_TotSize 32

#define kCmndVersion 0

#define kCmndFindExtnFind 1
#define kCmndFindExtnId2Code 2
#define kCmndFindExtnCount 3

#define kParamVersion 8

#define kParamFindExtnTheExtn 8
#define kParamFindExtnTheId 12

#define kParamDiskNumDrives 8
#define kParamDiskStart 8
#define kParamDiskCount 12
#define kParamDiskBuffer 16
#define kParamDiskDrive_No 20


#define get_long(x) (*((ui5b *)(x)))
#define get_word(x) (*((ui4b *)(x)))
#define get_byte(x) (*((ui3b *)(x)))
#define put_long(x, v) (*((ui5b *)(x))) = ((ui5b)(v))
#define put_word(x, v) (*((ui4b *)(x))) = ((ui4b)(v))
#define put_byte(x, v) (*((ui3b *)(x))) = ((ui3b)(v))

LOCALVAR ui5b pokeaddr;
LOCALVAR blnr HavePokeAddr = falseblnr;

LOCALFUNC OSErr GetPokeAddr(void)
{
	MyDriverDat_R *SonyVars;
	OSErr err = -1;

	SonyVars = (MyDriverDat_R *)get_long(SonyVarsPtr);

	if ((SonyVars == NULL)
		|| (SonyVars->zeroes[0] != 0)
		|| (SonyVars->zeroes[1] != 0)
		|| (SonyVars->zeroes[2] != 0)
		/* || (SonyVars->zeroes[3] != 0) */
		|| (SonyVars->checkval != kcom_checkval)
		)
	{
		/* fprintf(stderr, "no Mini vMac extension mechanism\n"); */
	} else {
		pokeaddr = SonyVars->pokeaddr;

		if (pokeaddr == 0) {
			/* fprintf(stderr, "pokeaddr == 0, no Mini vMac extension mechanism\n"); */
		} else {
			err = (OSErr)noErr;
		}
	}

	return err;
}

LOCALPROC InvokeExtension(ui5b pokeaddr, ui5b datp)
/*
	make seperate function to prevent
	invalid compiler optimations
*/
{
	put_long(pokeaddr, datp);
}

LOCALFUNC OSErr InvokeExtension2(ui3b *datp)
{
	OSErr err;

	if (HavePokeAddr || (noErr == (err = GetPokeAddr()))) {
		put_word(datp + DSKDat_checkval, kcom_callcheck);
		InvokeExtension(pokeaddr, (ui5b)datp);
		if (get_word(datp + DSKDat_checkval) != 0) {
			err = (OSErr)kCallCheckValFails;
		} else {
			err = get_word(datp + DSKDat_result);
		}
	}

	return err;
}


/* ---- Host Param Buffers Extension ---- */

#define kHostParamBuffersExtension 0x314C87BF

#define kCmndPbufFeatures 1
#define kCmndPbufNew 2
#define kCmndPbufDispose 3
#define kCmndPbufGetSize 4
#define kCmndPbufTransfer 5

LOCALVAR ui4b PBufExtnId;
LOCALVAR blnr HavePBufExtnId = falseblnr;

LOCALFUNC OSErr GetPBufExtnId(void)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + DSKDat_extension, 0);
	put_word(DskDatV + DSKDat_commnd, kCmndFindExtnFind);
	put_long(DskDatV + kParamFindExtnTheExtn, kHostParamBuffersExtension);

	err = InvokeExtension2(DskDatV);
	if (noErr == err) {
		PBufExtnId = get_word(DskDatV + kParamFindExtnTheId);
		HavePBufExtnId = true;
	}

	return err;
}

GLOBALFUNC blnr HavePBufExtenstion(void)
{
	return HavePBufExtnId || (noErr == GetPBufExtnId());
}

LOCALFUNC blnr InvokePBufExtension(ui3b *datp)
{
	OSErr err;

	if (HavePBufExtnId || (noErr == (err = GetPBufExtnId()))) {
		put_word(datp + DSKDat_extension, PBufExtnId);
		err = InvokeExtension2(datp);
	}

	return CheckSysErr(err);
}

#define NotAPbuf ((ui4b)0xFFFF)

GLOBALFUNC blnr PbufVersion(ui4b *version)
{
	ui3b DskDatV[DSKDat_TotSize];
	blnr IsOk;

	put_word(DskDatV + DSKDat_commnd, kCmndVersion);
	IsOk = InvokePBufExtension(DskDatV);
	/* if (IsOk) */ {
		*version = get_word(DskDatV + kParamVersion);
	}
	return IsOk;
}

GLOBALFUNC blnr PbufFeatures(ui5b *features)
{
	ui3b DskDatV[DSKDat_TotSize];
	blnr IsOk;

	put_word(DskDatV + DSKDat_commnd, kCmndPbufFeatures);
	IsOk = InvokePBufExtension(DskDatV);
	/* if (IsOk) */ {
		*features = get_long(DskDatV + DSKDat_params + 0);
	}
	return IsOk;
}

GLOBALFUNC blnr PbufNew(ui5b count, ui4b *r)
{
	ui3b DskDatV[DSKDat_TotSize];
	blnr IsOk;

	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */
	put_long(DskDatV + DSKDat_params + 4, count);

	put_word(DskDatV + DSKDat_commnd, kCmndPbufNew);
	IsOk = InvokePBufExtension(DskDatV);
	/* if (IsOk) */ {
		*r = get_word(DskDatV + DSKDat_params + 0);
	}
	return IsOk;
}

GLOBALFUNC blnr PbufDispose(ui4b i)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_params + 0, i);
	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */

	put_word(DskDatV + DSKDat_commnd, kCmndPbufDispose);
	return InvokePBufExtension(DskDatV);
}

GLOBALFUNC blnr PBufGetSize(ui4b i, ui5b *count)
{
	ui3b DskDatV[DSKDat_TotSize];
	blnr IsOk;

	put_word(DskDatV + DSKDat_params + 0, i);
	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */

	put_word(DskDatV + DSKDat_commnd, kCmndPbufGetSize);
	IsOk = InvokePBufExtension(DskDatV);
	/* if (IsOk) */ {
		*count = get_long(DskDatV + DSKDat_params + 4);
	}
	return IsOk;
}

GLOBALFUNC blnr PbufTransfer(void *Buffer,
	ui4b i, ui5b offset, ui5b count, blnr IsWrite)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_params + 0, i);
	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */
	put_long(DskDatV + DSKDat_params + 4, offset);
	put_long(DskDatV + DSKDat_params + 8, count);
	put_long(DskDatV + DSKDat_params + 12, Buffer);
	put_word(DskDatV + DSKDat_params + 16, IsWrite ? 1 : 0);

	put_word(DskDatV + DSKDat_commnd, kCmndPbufTransfer);
	return InvokePBufExtension(DskDatV);
}


/* ---- Disk Driver Extension ---- */

#define kDiskDriverExtension 0x4C9219E6

#define kCmndDiskNDrives 1
#define kCmndDiskRead 2
#define kCmndDiskWrite 3
#define kCmndDiskEject 4
#define kCmndDiskGetSize 5
#define kCmndDiskGetCallBack 6
#define kCmndDiskSetCallBack 7
#define kCmndDiskQuitOnEject 8
#define kCmndDiskFeatures 9
#define kCmndDiskNextPendingInsert 10
#define kCmndDiskGetRawMode 11
#define kCmndDiskSetRawMode 12
#define kCmndDiskNew 13
#define kCmndDiskGetNewWanted 14
#define kCmndDiskEjectDelete 15
#define kCmndDiskGetName 16

LOCALVAR ui4b DiskExtnId;
LOCALVAR blnr HaveDiskExtnId = falseblnr;

LOCALFUNC OSErr GetDiskExtnId(void)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + DSKDat_extension, 0);
	put_word(DskDatV + DSKDat_commnd, kCmndFindExtnFind);
	put_long(DskDatV + kParamFindExtnTheExtn, kDiskDriverExtension);

	err = InvokeExtension2(DskDatV);
	if (noErr == err) {
		DiskExtnId = get_word(DskDatV + kParamFindExtnTheId);
		HaveDiskExtnId = true;
	}

	return err;
}

GLOBALFUNC blnr HaveDiskExtenstion(void)
{
	return HaveDiskExtnId || (noErr == GetDiskExtnId());
}

LOCALFUNC OSErr InvokeDiskExtension(ui3b *datp)
{
	OSErr err;

	if (HaveDiskExtnId || (noErr == (err = GetDiskExtnId()))) {
		put_word(datp + DSKDat_extension, DiskExtnId);
		err = InvokeExtension2(datp);
	}

	return err;
}

GLOBALFUNC blnr DiskVersion(ui4b *version)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + DSKDat_commnd, kCmndVersion);
	err = InvokeDiskExtension(DskDatV);
	/* if (noErr == err) */ {
		*version = get_word(DskDatV + kParamVersion);
	}
	return CheckSysErr(err);
}

#define kFeatureCmndDisk_RawMode 0
#define kFeatureCmndDisk_New 1
#define kFeatureCmndDisk_NewName 2
#define kFeatureCmndDisk_GetName 3

GLOBALFUNC blnr DiskFeatures(ui5b *features)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + DSKDat_commnd, kCmndDiskFeatures);
	err = InvokeDiskExtension(DskDatV);
	/* if (noErr == err) */ {
		*features = get_long(DskDatV + DSKDat_params + 0);
	}
	return CheckSysErr(err);
}

GLOBALFUNC blnr DiskRead(void *Buffer, ui4b Drive_No,
	ui5b Sony_Start, ui5b *Sony_Count)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_long(DskDatV + kParamDiskStart, Sony_Start);
	put_long(DskDatV + kParamDiskCount, *Sony_Count);
	put_long(DskDatV + kParamDiskBuffer, (long)Buffer);
	put_word(DskDatV + kParamDiskDrive_No, Drive_No);
	put_word(DskDatV + DSKDat_commnd, kCmndDiskRead);
	err = InvokeDiskExtension(DskDatV);
	/* if (noErr == err) */ {
		*Sony_Count = get_long(DskDatV + kParamDiskCount);
	}
	return CheckSysErr(err);
}

GLOBALFUNC blnr DiskWrite(void *Buffer, ui4b Drive_No,
	ui5b Sony_Start, ui5b *Sony_Count)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_long(DskDatV + kParamDiskStart, Sony_Start);
	put_long(DskDatV + kParamDiskCount, *Sony_Count);
	put_long(DskDatV + kParamDiskBuffer, (long)Buffer);
	put_word(DskDatV + kParamDiskDrive_No, Drive_No);
	put_word(DskDatV + DSKDat_commnd, kCmndDiskWrite);
	err = InvokeDiskExtension(DskDatV);
	/* if (noErr == err) */ {
		*Sony_Count = get_long(DskDatV + kParamDiskCount);
	}
	return CheckSysErr(err);
}

GLOBALFUNC blnr ExtnDiskEject(ui4b Drive_No)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + kParamDiskDrive_No, Drive_No);
	put_word(DskDatV + DSKDat_commnd, kCmndDiskEject);
	return CheckSysErr(InvokeDiskExtension(DskDatV));
}

GLOBALFUNC blnr DiskGetSize(ui4b Drive_No, ui5b *Sony_Count)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + kParamDiskDrive_No, Drive_No);
	put_word(DskDatV + DSKDat_commnd, kCmndDiskGetSize);
	err = InvokeDiskExtension(DskDatV);
	/* if (noErr == err) */ {
		*Sony_Count = get_long(DskDatV + kParamDiskCount);
	}
	return CheckSysErr(err);
}

GLOBALFUNC blnr DiskGetCallBack(ui5b *p)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + DSKDat_commnd, kCmndDiskGetCallBack);
	err = InvokeDiskExtension(DskDatV);
	/* if (noErr == err) */ {
		*p = get_long(DskDatV + kParamDiskBuffer);
	}
	return CheckSysErr(err);
}

GLOBALFUNC blnr DiskSetCallBack(ui5b p)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndDiskSetCallBack);
	put_long(DskDatV + kParamDiskBuffer, p);
	return CheckSysErr(InvokeDiskExtension(DskDatV));
}

GLOBALFUNC blnr DiskGetRawMode(ui4b *m)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + DSKDat_commnd, kCmndDiskGetRawMode);
	err = InvokeDiskExtension(DskDatV);
	/* if (noErr == err) */ {
		*m = get_word(DskDatV + kParamDiskBuffer);
	}
	return CheckSysErr(err);
}

GLOBALFUNC blnr DiskSetRawMode(ui4b m)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndDiskSetRawMode);
	put_word(DskDatV + kParamDiskBuffer, m);
	return CheckSysErr(InvokeDiskExtension(DskDatV));
}

GLOBALFUNC OSErr DiskNextPendingInsert(ui4b *Drive_No)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + DSKDat_commnd, kCmndDiskNextPendingInsert);
	err = InvokeDiskExtension(DskDatV);
	/* if (noErr == err) */ {
		*Drive_No = get_word(DskDatV + kParamDiskDrive_No);
	}
	return err;
}

GLOBALFUNC blnr DiskNew(ui5b L, ui4b Name)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_long(DskDatV + DSKDat_params + 0, L);
	put_word(DskDatV + DSKDat_params + 4, Name);
	put_word(DskDatV + DSKDat_params + 6, 0); /* reserved */
	put_word(DskDatV + DSKDat_commnd, kCmndDiskNew);
	return CheckSysErr(InvokeDiskExtension(DskDatV));
}

GLOBALFUNC blnr DiskGetNewWanted(ui4b *m)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + DSKDat_commnd, kCmndDiskGetNewWanted);
	err = InvokeDiskExtension(DskDatV);
	/* if (noErr == err) */ {
		*m = get_word(DskDatV + kParamDiskBuffer);
	}
	return CheckSysErr(err);
}

GLOBALFUNC blnr DiskGetName(ui4b Drive_No, ui4b *r)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + DSKDat_params + 0, Drive_No);
	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */

	put_word(DskDatV + DSKDat_commnd, kCmndDiskGetName);
	err = InvokeDiskExtension(DskDatV);
	/* if (noErr == err) */ {
		*r = get_word(DskDatV + DSKDat_params + 4);
	}
	return CheckSysErr(err);
}

GLOBALFUNC blnr DiskEjectDelete(ui4b Drive_No)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + kParamDiskDrive_No, Drive_No);
	put_word(DskDatV + DSKDat_commnd, kCmndDiskEjectDelete);
	return CheckSysErr(InvokeDiskExtension(DskDatV));
}


/* ---- Host Clip Exchange Extension ---- */

#define kHostClipExchangeExtension 0x27B130CA

#define kCmndHTCEFeatures 1
#define kCmndHTCEExport 2
#define kCmndHTCEImport 3

LOCALVAR ui4b HTCExtnId;
LOCALVAR blnr HaveHTCExtnId = falseblnr;

LOCALFUNC OSErr GetHTCExtnId(void)
{
	ui3b DskDatV[DSKDat_TotSize];
	OSErr err;

	put_word(DskDatV + DSKDat_extension, 0);
	put_word(DskDatV + DSKDat_commnd, kCmndFindExtnFind);
	put_long(DskDatV + kParamFindExtnTheExtn, kHostClipExchangeExtension);

	err = InvokeExtension2(DskDatV);
	if (noErr == err) {
		HTCExtnId = get_word(DskDatV + kParamFindExtnTheId);
		HaveHTCExtnId = true;
	}

	return err;
}

GLOBALFUNC blnr HaveHTCExtenstion(void)
{
	return HaveHTCExtnId || (noErr == GetHTCExtnId());
}

LOCALFUNC blnr InvokeHTCExtension(ui3b *datp)
{
	OSErr err;

	if (HaveHTCExtnId || (noErr == (err = GetHTCExtnId()))) {
		put_word(datp + DSKDat_extension, HTCExtnId);
		err = InvokeExtension2(datp);
	}

	return CheckSysErr(err);
}

GLOBALFUNC blnr HTCVersion(ui4b *version)
{
	ui3b DskDatV[DSKDat_TotSize];
	blnr IsOk;

	put_word(DskDatV + DSKDat_commnd, kCmndVersion);
	IsOk = InvokeHTCExtension(DskDatV);
	/* if (IsOk) */ {
		*version = get_word(DskDatV + kParamVersion);
	}
	return IsOk;
}

GLOBALFUNC blnr HTCFeatures(ui5b *features)
{
	ui3b DskDatV[DSKDat_TotSize];
	blnr IsOk;

	put_word(DskDatV + DSKDat_commnd, kCmndHTCEFeatures);
	IsOk = InvokeHTCExtension(DskDatV);
	/* if (IsOk) */ {
		*features = get_long(DskDatV + DSKDat_params + 0);
	}
	return IsOk;
}

GLOBALFUNC blnr HTCImport(ui4b *r)
{
	ui3b DskDatV[DSKDat_TotSize];
	blnr IsOk;

	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */

	put_word(DskDatV + DSKDat_commnd, kCmndHTCEImport);
	IsOk = InvokeHTCExtension(DskDatV);
	/* if (IsOk) */ {
		*r = get_word(DskDatV + DSKDat_params + 0);
	}
	return IsOk;
}

GLOBALFUNC blnr HTCExport(ui4b i)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_params + 0, i);
	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */

	put_word(DskDatV + DSKDat_commnd, kCmndHTCEExport);
	return InvokeHTCExtension(DskDatV);
}
