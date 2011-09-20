/*
	EXTNSLIB.i

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
	mini vmac EXTeNSions LIBrary
*/


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

LOCALVAR ui5r pokeaddr;
LOCALVAR blnr HavePokeAddr = falseblnr;

LOCALFUNC tMyErr GetPokeAddr(void)
{
	tMyErr err;
	MyDriverDat_R *SonyVars;

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
		err = kMyErrNoExtn;
	} else {
		pokeaddr = SonyVars->pokeaddr;

		if (pokeaddr == 0) {
			err = kMyErrNoExtn;
			/*
				fprintf(stderr,
					"pokeaddr == 0, no Mini vMac extension mechanism\n"
					);
			*/
		} else {
			err = noErr;
		}
	}

	return err;
}

LOCALPROC InvokeExtension(ui5r pokeaddr, ui5r datp)
/*
	make seperate function to prevent
	invalid compiler optimations
*/
{
	put_long(pokeaddr, datp);
}

LOCALFUNC tMyErr InvokeExtension2(ui3b *datp)
{
	tMyErr err;

	if (HavePokeAddr || (noErr == (err = GetPokeAddr()))) {
		put_word(datp + DSKDat_checkval, kcom_callcheck);
		InvokeExtension(pokeaddr, (ui5b)datp);
		if (0 != get_word(datp + DSKDat_checkval)) {
			err = kMyErrNoExtn;
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

LOCALVAR ui4r PBufExtnId;
LOCALVAR blnr HavePBufExtnId = falseblnr;

LOCALFUNC tMyErr GetPBufExtnId(void)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_extension, 0);
	put_word(DskDatV + DSKDat_commnd, kCmndFindExtnFind);
	put_long(DskDatV + kParamFindExtnTheExtn,
		kHostParamBuffersExtension);

	err = InvokeExtension2(DskDatV);
	if (noErr == err) {
		PBufExtnId = get_word(DskDatV + kParamFindExtnTheId);
		HavePBufExtnId = true;
	}

	return err;
}

GLOBALFUNC tMyErr HavePBufExtenstion_v2(void)
{
	return HavePBufExtnId ? noErr : GetPBufExtnId();
}

LOCALFUNC tMyErr InvokePBufExtension_v2(ui3b *datp)
{
	tMyErr err;

	if (noErr == (err = HavePBufExtenstion_v2())) {
		put_word(datp + DSKDat_extension, PBufExtnId);
		err = InvokeExtension2(datp);
	}

	return err;
}

GLOBALFUNC tMyErr PbufVersion_v2(ui4r *version)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndVersion);
	err = InvokePBufExtension_v2(DskDatV);
	/* if (noErr == err) */ {
		*version = get_word(DskDatV + kParamVersion);
	}

	return err;
}

GLOBALFUNC tMyErr PbufFeatures_v2(ui5r *features)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndPbufFeatures);
	err = InvokePBufExtension_v2(DskDatV);
	/* if (noErr == err) */ {
		*features = get_long(DskDatV + DSKDat_params + 0);
	}

	return err;
}

#define tPbuf ui4r

#define NotAPbuf ((tPbuf)0xFFFF)

GLOBALFUNC tMyErr PbufNew_v2(ui5r count, tPbuf *r)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */
	put_long(DskDatV + DSKDat_params + 4, count);

	put_word(DskDatV + DSKDat_commnd, kCmndPbufNew);
	err = InvokePBufExtension_v2(DskDatV);
	if (noErr == err) {
		*r = get_word(DskDatV + DSKDat_params + 0);
	}

	return err;
}

GLOBALFUNC tMyErr PbufDispose_v2(tPbuf i)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_params + 0, i);
	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */

	put_word(DskDatV + DSKDat_commnd, kCmndPbufDispose);
	return InvokePBufExtension_v2(DskDatV);
}

GLOBALFUNC tMyErr PBufGetSize_v2(tPbuf i, ui5r *count)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_params + 0, i);
	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */

	put_word(DskDatV + DSKDat_commnd, kCmndPbufGetSize);
	err = InvokePBufExtension_v2(DskDatV);
	/* if (noErr == err) */ {
		*count = get_long(DskDatV + DSKDat_params + 4);
	}

	return err;
}

GLOBALFUNC tMyErr PbufTransfer_v2(void *Buffer,
	tPbuf i, ui5r offset, ui5r count, blnr IsWrite)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_params + 0, i);
	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */
	put_long(DskDatV + DSKDat_params + 4, offset);
	put_long(DskDatV + DSKDat_params + 8, count);
	put_long(DskDatV + DSKDat_params + 12, Buffer);
	put_word(DskDatV + DSKDat_params + 16, IsWrite ? 1 : 0);

	put_word(DskDatV + DSKDat_commnd, kCmndPbufTransfer);
	return InvokePBufExtension_v2(DskDatV);
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

LOCALVAR ui4r DiskExtnId;
LOCALVAR blnr HaveDiskExtnId = falseblnr;

LOCALFUNC tMyErr GetDiskExtnId(void)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

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

GLOBALFUNC tMyErr HaveDiskExtenstion_v2(void)
{
	return HaveDiskExtnId ? noErr : GetDiskExtnId();
}

LOCALFUNC tMyErr InvokeDiskExtension_v2(ui3b *datp)
{
	tMyErr err;

	if (noErr == (err = HaveDiskExtenstion_v2())) {
		put_word(datp + DSKDat_extension, DiskExtnId);
		err = InvokeExtension2(datp);
	}

	return err;
}

GLOBALFUNC tMyErr DiskVersion_v2(ui4r *version)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndVersion);
	err = InvokeDiskExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*version = get_word(DskDatV + kParamVersion);
	}
	return err;
}

#define kFeatureCmndDisk_RawMode 0
#define kFeatureCmndDisk_New 1
#define kFeatureCmndDisk_NewName 2
#define kFeatureCmndDisk_GetName 3

GLOBALFUNC tMyErr DiskFeatures_v2(ui5r *features)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndDiskFeatures);
	err = InvokeDiskExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*features = get_long(DskDatV + DSKDat_params + 0);
	}
	return err;
}

#define tDrive ui4r

GLOBALFUNC tMyErr DiskRead_v2(void *Buffer, tDrive Drive_No,
	ui5r Sony_Start, ui5r *Sony_Count)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_long(DskDatV + kParamDiskStart, Sony_Start);
	put_long(DskDatV + kParamDiskCount, *Sony_Count);
	put_long(DskDatV + kParamDiskBuffer, (long)Buffer);
	put_word(DskDatV + kParamDiskDrive_No, Drive_No);
	put_word(DskDatV + DSKDat_commnd, kCmndDiskRead);
	err = InvokeDiskExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*Sony_Count = get_long(DskDatV + kParamDiskCount);
	}
	return err;
}

GLOBALFUNC tMyErr DiskWrite_v2(void *Buffer, tDrive Drive_No,
	ui5r Sony_Start, ui5r *Sony_Count)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_long(DskDatV + kParamDiskStart, Sony_Start);
	put_long(DskDatV + kParamDiskCount, *Sony_Count);
	put_long(DskDatV + kParamDiskBuffer, (long)Buffer);
	put_word(DskDatV + kParamDiskDrive_No, Drive_No);
	put_word(DskDatV + DSKDat_commnd, kCmndDiskWrite);
	err = InvokeDiskExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*Sony_Count = get_long(DskDatV + kParamDiskCount);
	}
	return err;
}

GLOBALFUNC tMyErr DiskEject_v2(tDrive Drive_No)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + kParamDiskDrive_No, Drive_No);
	put_word(DskDatV + DSKDat_commnd, kCmndDiskEject);
	return InvokeDiskExtension_v2(DskDatV);
}

GLOBALFUNC tMyErr DiskGetSize_v2(tDrive Drive_No, ui5r *Sony_Count)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + kParamDiskDrive_No, Drive_No);
	put_word(DskDatV + DSKDat_commnd, kCmndDiskGetSize);
	err = InvokeDiskExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*Sony_Count = get_long(DskDatV + kParamDiskCount);
	}
	return err;
}

GLOBALFUNC tMyErr DiskGetCallBack_v2(ui5r *p)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndDiskGetCallBack);
	err = InvokeDiskExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*p = get_long(DskDatV + kParamDiskBuffer);
	}
	return err;
}

GLOBALFUNC tMyErr DiskSetCallBack_v2(ui5r p)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndDiskSetCallBack);
	put_long(DskDatV + kParamDiskBuffer, p);
	return InvokeDiskExtension_v2(DskDatV);
}

GLOBALFUNC tMyErr DiskGetRawMode_v2(ui4r *m)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndDiskGetRawMode);
	err = InvokeDiskExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*m = get_word(DskDatV + kParamDiskBuffer);
	}
	return err;
}

GLOBALFUNC tMyErr DiskSetRawMode_v2(ui4r m)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndDiskSetRawMode);
	put_word(DskDatV + kParamDiskBuffer, m);
	return InvokeDiskExtension_v2(DskDatV);
}

GLOBALFUNC tMyErr DiskNextPendingInsert(tDrive *Drive_No)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndDiskNextPendingInsert);
	err = InvokeDiskExtension_v2(DskDatV);

	if (noErr == err) {
		*Drive_No = get_word(DskDatV + kParamDiskDrive_No);
	}
	return err;
}

GLOBALFUNC tMyErr DiskNew_v2(ui5r L, tPbuf Name)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_long(DskDatV + DSKDat_params + 0, L);
	put_word(DskDatV + DSKDat_params + 4, Name);
	put_word(DskDatV + DSKDat_params + 6, 0); /* reserved */
	put_word(DskDatV + DSKDat_commnd, kCmndDiskNew);
	return InvokeDiskExtension_v2(DskDatV);
}

GLOBALFUNC tMyErr DiskGetNewWanted_v2(ui4r *m)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndDiskGetNewWanted);
	err = InvokeDiskExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*m = get_word(DskDatV + kParamDiskBuffer);
	}
	return err;
}

GLOBALFUNC tMyErr DiskGetName_v2(tDrive Drive_No, tPbuf *r)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_params + 0, Drive_No);
	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */
	put_word(DskDatV + DSKDat_commnd, kCmndDiskGetName);
	err = InvokeDiskExtension_v2(DskDatV);

	if (noErr == err) {
		*r = get_word(DskDatV + DSKDat_params + 4);
	}
	return err;
}

GLOBALFUNC tMyErr DiskEjectDelete_v2(tDrive Drive_No)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + kParamDiskDrive_No, Drive_No);
	put_word(DskDatV + DSKDat_commnd, kCmndDiskEjectDelete);
	return InvokeDiskExtension_v2(DskDatV);
}

GLOBALFUNC tMyErr DiskQuitOnEject(void)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndDiskQuitOnEject);
	return InvokeDiskExtension_v2(DskDatV);
}


/* ---- Host Clip Exchange Extension ---- */

#define kHostClipExchangeExtension 0x27B130CA

#define kCmndHTCEFeatures 1
#define kCmndHTCEExport 2
#define kCmndHTCEImport 3

LOCALVAR ui4r HTCExtnId;
LOCALVAR blnr HaveHTCExtnId = falseblnr;

LOCALFUNC tMyErr GetHTCExtnId(void)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_extension, 0);
	put_word(DskDatV + DSKDat_commnd, kCmndFindExtnFind);
	put_long(DskDatV + kParamFindExtnTheExtn,
		kHostClipExchangeExtension);

	err = InvokeExtension2(DskDatV);
	if (noErr == err) {
		HTCExtnId = get_word(DskDatV + kParamFindExtnTheId);
		HaveHTCExtnId = true;
	}

	return err;
}

GLOBALFUNC tMyErr HaveHTCExtenstion_v2(void)
{
	return HaveHTCExtnId ? noErr : GetHTCExtnId();
}

LOCALFUNC tMyErr InvokeHTCExtension_v2(ui3b *datp)
{
	tMyErr err;

	if (noErr == (err = HaveHTCExtenstion_v2())) {
		put_word(datp + DSKDat_extension, HTCExtnId);
		err = InvokeExtension2(datp);
	}

	return err;
}

GLOBALFUNC tMyErr HTCVersion_v2(ui4r *version)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndVersion);
	err = InvokeHTCExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*version = get_word(DskDatV + kParamVersion);
	}
	return err;
}

GLOBALFUNC tMyErr HTCFeatures_v2(ui5r *features)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_commnd, kCmndHTCEFeatures);
	err = InvokeHTCExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*features = get_long(DskDatV + DSKDat_params + 0);
	}
	return err;
}

GLOBALFUNC tMyErr HTCImport_v2(tPbuf *r)
{
	tMyErr err;
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */
	put_word(DskDatV + DSKDat_commnd, kCmndHTCEImport);
	err = InvokeHTCExtension_v2(DskDatV);

	/* if (noErr == err) */ {
		*r = get_word(DskDatV + DSKDat_params + 0);
	}
	return err;
}

GLOBALFUNC tMyErr HTCExport_v2(tPbuf i)
{
	ui3b DskDatV[DSKDat_TotSize];

	put_word(DskDatV + DSKDat_params + 0, i);
	put_word(DskDatV + DSKDat_params + 2, 0); /* reserved */

	put_word(DskDatV + DSKDat_commnd, kCmndHTCEExport);
	return InvokeHTCExtension_v2(DskDatV);
}
