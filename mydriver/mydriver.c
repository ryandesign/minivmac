/*
	mydriver.c

	Copyright (C) 2004 Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#define EmulatingMac128k 1


typedef unsigned long ui5b;
typedef unsigned short ui4b;
typedef unsigned char ui3b;

typedef long si5b;
typedef short si4b;
typedef char si3b;

typedef short OSErr;
typedef char *Ptr;
typedef long Size;

#define ONEWORDINLINE(trapNum) = trapNum

#pragma parameter __D0 PostEvent(__A0, __D0)
extern pascal OSErr PostEvent(ui4b eventNum, ui5b eventMsg)
	ONEWORDINLINE(0xA02F);

#pragma parameter __A0 NewPtrSysClear(__D0)
extern pascal Ptr NewPtrSysClear(Size byteCount)
	ONEWORDINLINE(0xA71E);

#pragma parameter AddDrive0(__A0, __D0)
extern pascal void AddDrive0(ui5b qEl, ui5b arg2)
	ONEWORDINLINE(0xA04E);

typedef void (* UniversalProcPtr)();

#if EmulatingMac128k
#pragma parameter SetTrapAddress(__A0, __D0)
extern pascal void SetTrapAddress(
	UniversalProcPtr   trapAddr,
	ui4b               trapNum)
	ONEWORDINLINE(0xA047);
#endif

extern pascal void Debugger(void)
	ONEWORDINLINE(0xA9FF);

typedef struct TMTask                   TMTask;
typedef TMTask *                        TMTaskPtr;
typedef pascal void (* TimerUPP)();
struct TMTask {
	TMTaskPtr           qLink;
	short               qType;
	TimerUPP            tmAddr;
	long                tmCount;
};

#pragma parameter InsTime(__A0)
extern pascal void InsTime(TMTaskPtr tmTaskPtr)                                   ONEWORDINLINE(0xA058);

#define CPTR ui5b

#define get_long(x) (*((ui5b *)(x)))
#define get_word(x) (*((ui4b *)(x)))
#define get_byte(x) (*((ui3b *)(x)))
#define put_long(x, v) (*((ui5b *)(x))) = (v)
#define put_word(x, v) (*((ui4b *)(x))) = (v)
#define put_byte(x, v) (*((ui3b *)(x))) = (v)

#define NULL ((void *)0)

/* Sony_Table Indexes */

#define kOpen    0
#define kPrime   1
#define kControl 2
#define kStatus  3
#define kClose   4

/* Sony Variable Drive Offsets */

/* Sony Variable Drive Setting Offsets */

#define kTrack       0 /* Current Track */
#define kWriteProt   2 /* FF if Write Protected, 00 if readable */
#define kDiskInPlace 3 /* 00 = No Disk, 01 = Disk In, 2 = MacOS Read, FC-FF = Just Ejected */
#define kInstalled   4 /* 00 = Unknown, 01 = Installed, FF = Not Installed */
#define kSides       5 /* 00 if Single Sided Drive, FF if Doubled Sided Drive */
#define kQLink       6 /* Link to Next Drive */
#define kQType      10 /* Drive Type (0 = Size Saved, 1 = Very Large) */
#define kQDriveNo   12 /* Drive Number (1 = Internal, 2 = External) */
#define kQRefNum    14 /* Driver Reference Number (-5 for .Sony, FFFB) */
#define kQFSID      16 /* File System ID (0 = MacOS) */
#define kQDrvSz     18 /* size, low-order word */
#define kQDrvSz2    20 /* size, hi-order word */

#define kTwoSideFmt 18 /* FF if double-sided format, 00 if single-sided format */
#define kNewIntf    19 /* FF if new 800K interface or 00 if old 400K interface */
#define kDriveErrs  20 /* Drive Soft Errors */

/* Sony Driver Control Call csCodes */

#define kKillIO             1
#define kVerifyDisk         5
#define kFormatDisk         6
#define kEjectDisk          7
#define kSetTagBuffer       8
#define kTrackCacheControl  9
#define kReturnDriveIcon   21
#define kReturnDriveInfo   23

/* Sony Driver Status Call csCodes */

#define kDriveStatus  8

/* Parameter Block Offsets */

#define kqLink         0
#define kqType         4
#define kioTrap        6
#define kioCmdAddr     8
#define kioCompletion 12
#define kioResult     16
#define kioNamePtr    18
#define kioVRefNum    22
#define kioRefNum     24
#define kcsCode       26
#define kcsParam      28
#define kioBuffer     32 /* Buffer to store data into */
#define kioReqCount   36 /* Requested Number of Bytes */
#define kioActCount   40 /* Actual Number of Bytes obtained */
#define kioPosMode    44 /* Positioning Mode */
#define kioPosOffset  46 /* Position Offset */

/* Positioning Modes */

#define kfsAtMark    0 /* At Mark (Ignore PosOffset) */
#define kfsFromStart 1 /* At Start (PosOffset is absolute) */
#define kfsFromLEOF  2 /* From Logical End of File - PosOffset */
#define kfsFromMark  3 /* At Mark + PosOffset */

/* Device Control Entry Offsets */

#define kdCtlPosition 16

#define kDSK_Block_Base 0x00F40000

#define SonyVarsPtr 0x0134

#define FirstDriveVarsOffset 74
#define EachDriveVarsSize 66
#if EmulatingMac128k
#define MinSonVarsSize 0x000000FA
#else
#define MinSonVarsSize 0x00000310
#endif
	/*
		This is probably intended to force it to match
		memory layout of real machine when possible.
		But haven't verified this is true.
	*/


extern void DUpdate(void);
extern void MyAddDrive64k(void);


#define kcom_checkval 0x841339E2
#define kcom_callcheck 0x5B17

#define kDiskDriverExtension 0x4C9219E6

struct MyDriverDat_R {
	ui5b zeroes[4];
	ui5b checkval;
	ui5b pokeaddr;
	ui4b NumDrives;
	ui4b DiskExtn;
	TMTask NullTask;
	/* total size must be <= FirstDriveVarsOffset */
};
typedef struct MyDriverDat_R MyDriverDat_R;

#define DSKDat_checkval 0
#define DSKDat_extension 2
#define DSKDat_commnd 4
#define DSKDat_result 6
#define DSKDat_params 8
#define DSKDat_TotSize 32

#define kCmndFindExtnFind 1

#define kCmndDiskNDrives 1
#define kCmndDiskRead 2
#define kCmndDiskWrite 3
#define kCmndDiskEject 4
#define kCmndDiskGetSize 5
#define kCmndDiskGetCallBack 6
#define kCmndDiskSetCallBack 7
#define kCmndDiskQuitOnEject 8

#define kParamVersion 8

#define kParamFindExtnTheExtn 8
#define kParamFindExtnTheId 12

#define kParamDiskNumDrives 8
#define kParamDiskStart 8
#define kParamDiskCount 12
#define kParamDiskBuffer 16
#define kParamDiskDrive_No 20

static ui5b DriveVarsLocation(ui4b Drive_No)
{
	MyDriverDat_R *SonyVars = (MyDriverDat_R *)get_long(SonyVarsPtr);

	if (Drive_No < SonyVars->NumDrives) {
		return ((CPTR)SonyVars) + FirstDriveVarsOffset + EachDriveVarsSize * Drive_No;
	} else {
		return (ui5b)NULL;
	}
}

void Sony_Update (ui5b data)
{
	ui3b DskDatV[DSKDat_TotSize];
	si4b i = data & 0x0000FFFF;
	MyDriverDat_R *SonyVars = (MyDriverDat_R *)get_long(SonyVarsPtr);
	CPTR dvl = DriveVarsLocation(i);

	if (get_byte(dvl + kDiskInPlace) == 0x00) {

		put_word(&DskDatV + DSKDat_checkval, kcom_callcheck);
		put_word(&DskDatV + DSKDat_extension, SonyVars->DiskExtn);
		put_word(&DskDatV + DSKDat_commnd, kCmndDiskGetSize);
		put_word(&DskDatV + kParamDiskDrive_No, i);
		put_long(SonyVars->pokeaddr, (ui5b)&DskDatV);

		if ((si4b)get_word(&DskDatV + DSKDat_result) == 0) {
			ui5b L = get_long(&DskDatV + kParamDiskCount);

			L >>= 9; /* bytes to blocks */

			if ((L == 800)
#if ! EmulatingMac128k
				|| (L == 1600)
#endif
			) {
#if EmulatingMac128k
				put_byte(dvl + kTwoSideFmt, 0x00);  /* Drive i Single Format */
				put_byte(dvl + kNewIntf, 0x00);     /* Drive i doesn't use new interface */
#else
				if (L == 800) {
					put_byte(dvl + kTwoSideFmt, 0x00);  /* Drive i Single Format */
				} else {
					put_byte(dvl + kTwoSideFmt, 0xFF);  /* Drive Double Format */
				}
				put_byte(dvl + kNewIntf, 0xFF);     /* Drive i uses new interface */
#endif
				put_word(dvl + kQType, 0x00); /* Drive Type */
				put_word(dvl + kDriveErrs, 0x0000); /* Drive i has no errors */
			} else
			{
				put_word(dvl + kQRefNum, 0xFFFE);  /* Driver */
				put_word(dvl + kQType, 0x01); /* Drive Type */
				put_word(dvl + kQDrvSz , L);
				put_word(dvl + kQDrvSz2, L >> 16);
			}

			put_byte(dvl + kWriteProt, data >> 16);
			put_byte(dvl + kDiskInPlace, 0x01); /* Drive Disk Inserted */
			PostEvent(7 /* Disk Inserted */, i + 1 /* Drive */);
		}
	}
	/* else, if disk already in place, a mistake has been made */
}

static pascal void NullTaskProc(void)
{
}

/* Sony_Open */
/*  */
/* This opens up the Sony driver, it allocates a pointer for the variables */
/* Sets SonyVars, then sets up the drive variables with correct values */
/* And then adds the drives into the drive queue */
/*  */
/* Bill Huey comment --- */
/*  */
/* It wasn't obvious initially, but what "Opening" the ".Sony" driver doesn't */
/* mean opening a file, which is pretty stupid of me. I keeped wondering */
/* where and how the drive structures where being set at start up, only to realize */
/* that it's right under my nose in the function Sony_Open. */
/* That's what I get for not paying attention... */
/*  */
si4b Sony_Open(CPTR ParamBlk, CPTR DeviceCtl)
{
#pragma unused(ParamBlk)
#if EmulatingMac128k
#pragma unused(DeviceCtl)
#endif
	si4b i;
	CPTR dvl;
	MyDriverDat_R *SonyVars;
	ui3b DskDatV[DSKDat_TotSize];
	ui4b DistExtnId;
	ui5b pokeaddr = kDSK_Block_Base;
	si4b result = -1;

	put_word(&DskDatV + DSKDat_checkval, kcom_callcheck);
	put_word(&DskDatV + DSKDat_extension, 0);
	put_word(&DskDatV + DSKDat_commnd, kCmndFindExtnFind);
	put_long(&DskDatV + kParamFindExtnTheExtn, kDiskDriverExtension);
	put_long(pokeaddr, (ui5b)&DskDatV);
	DistExtnId = get_word(&DskDatV + 12);

	put_word(&DskDatV + DSKDat_checkval, kcom_callcheck);
	put_word(&DskDatV + DSKDat_extension, DistExtnId);
	put_word(&DskDatV + DSKDat_commnd, kCmndDiskGetCallBack);
	put_long(pokeaddr, (ui5b)&DskDatV);
	if (get_long(&DskDatV + kParamDiskBuffer) != 0) { /* Don't Open Again */
		result = 0x0000; /* No Error */
	} else {

		put_word(&DskDatV + DSKDat_checkval, kcom_callcheck);
		put_word(&DskDatV + DSKDat_extension, DistExtnId);
		put_word(&DskDatV + DSKDat_commnd, kCmndDiskNDrives);
		put_long(pokeaddr, (ui5b)&DskDatV);
		if (get_word(&DskDatV + DSKDat_result) == 0) {
			ui4b NumDrives = get_word(&DskDatV + 8);
			ui5b L = FirstDriveVarsOffset + EachDriveVarsSize * NumDrives;

			if (L < MinSonVarsSize) {
				L = MinSonVarsSize;
			}

			SonyVars = (MyDriverDat_R *)NewPtrSysClear(L);  /* Size of Sony Variables */
			if (SonyVars != NULL) {
				SonyVars->checkval = kcom_checkval;
				SonyVars->pokeaddr = pokeaddr;
				SonyVars->NumDrives = NumDrives;
				SonyVars->DiskExtn = DistExtnId;

				put_long(SonyVarsPtr, (CPTR)SonyVars); /* Save it to SonyVars */
#if 1
#define UTableBase (*(ui5b **)(0x011C))
				UTableBase[1] = UTableBase[4];
#endif
#if EmulatingMac128k
				SetTrapAddress(MyAddDrive64k, 0xA04E);

				put_word(0x308, 0);
				put_long(0x308+2, 0);
				put_long(0x308+6, 0);
#else
				((ui3b *)(DeviceCtl))[7] = 1;

				/*
					There is apparently a bug in the time manager
					in System 6.0.8, which will cause a crash
					if there are no installed time
					tasks. So create a time task, since
					real disk driver does.
				*/
				SonyVars->NullTask.tmAddr = NullTaskProc;
				InsTime(&SonyVars->NullTask);
#endif

				for (i = 0; (dvl = DriveVarsLocation(i)) != (ui5b)NULL; ++i) {
					put_byte(dvl + kDiskInPlace, 0x00); /* Drive i No Disk */
					put_byte(dvl + kInstalled, 0x01);   /* Drive i Installed */
#if EmulatingMac128k
					put_byte(dvl + kSides, 0x00);       /* Drive i Single Sided */
#else
					put_byte(dvl + kSides, 0xFF);       /* Drive i Double Sided */
#endif
					put_word(dvl + kQDriveNo, i + 1);   /* Drive i is Drive 1 */
					put_word(dvl + kQRefNum, 0xFFFB);   /* Drive i uses .Sony */
					AddDrive0(dvl + kQLink, ((ui5b)(i + 1) << 16) + 0xFFFB); /* Drive i (Hi Word), Driver FFFB (Lo Word) */
				}

				put_word(&DskDatV + DSKDat_checkval, kcom_callcheck);
				put_word(&DskDatV + DSKDat_extension, DistExtnId);
				put_word(&DskDatV + DSKDat_commnd, kCmndDiskSetCallBack);
				put_long(&DskDatV + kParamDiskBuffer, (ui5b)&DUpdate);
				put_long(SonyVars->pokeaddr, (ui5b)&DskDatV);
				
				result = 0x0000; /* No Error */
			}
		}
	}
	return result;
}

/* Sony_Prime */
/* Handles I/O to disks */
si4b Sony_Prime(CPTR ParamBlk, CPTR DeviceCtl)
{
	si4b result;
	ui5b Sony_Count;
	ui5b Sony_Start;
	ui4b Drive_No = get_word(ParamBlk + kioVRefNum) - 1;
	ui3b DskDatV[DSKDat_TotSize];
	MyDriverDat_R *SonyVars = (MyDriverDat_R *)get_long(SonyVarsPtr);
	CPTR dvl = DriveVarsLocation(Drive_No);

	if (dvl == (ui5b)NULL) {
		result = 0xFFC8; /* No Such Drive (-56) */
		Sony_Count = 0;
	} else {
		CPTR DiskInPlaceA = dvl + kDiskInPlace;
		ui3b DiskInPlaceV = get_byte(DiskInPlaceA);

		if (DiskInPlaceV != 0x02) {
			if (DiskInPlaceV == 0x01) {
				put_byte(DiskInPlaceA, 0x02); /* Clamp Drive */
			} else {
				result = 0xFFBF; /* Say it's offline (-65) */
				put_word(ParamBlk + kioResult, result);
				put_long(ParamBlk + kioActCount, 0);
				return result;
				/*
					if don't check for this, will go right
					ahead and boot off a disk that hasn't
					been mounted yet by Sony_Update.
					(disks other than the boot disk aren't
					seen unless mounted by Sony_Update)
				*/
			}
		}

		switch (get_word(ParamBlk + kioPosMode) & 0x0F) {
			case kfsFromStart:
				Sony_Start = get_long(ParamBlk + kioPosOffset);
				break;
			case kfsFromLEOF:
				put_word(&DskDatV + DSKDat_checkval, kcom_callcheck);
				put_word(&DskDatV + DSKDat_extension, SonyVars->DiskExtn);
				put_word(&DskDatV + DSKDat_commnd, kCmndDiskGetSize);
				put_word(&DskDatV + kParamDiskDrive_No, Drive_No);
				put_long(SonyVars->pokeaddr, (ui5b)&DskDatV);
				Sony_Start = get_long(&DskDatV + kParamDiskCount);
				result = (si4b)get_word(&DskDatV + DSKDat_result) == 0;
				if (result != 0) {
					return result;
				}
				Sony_Start -= get_long(ParamBlk + kioPosOffset);
				break;
			case kfsFromMark:
				Sony_Start = get_long(ParamBlk + kioPosOffset) +
					get_long(DeviceCtl + kdCtlPosition);
				break;
			case kfsAtMark:
			default:
				Sony_Start = get_long(DeviceCtl + kdCtlPosition);
				break;
		}

		Sony_Count = get_long(ParamBlk + kioReqCount);

		put_word(&DskDatV + DSKDat_checkval, kcom_callcheck);
		put_word(&DskDatV + DSKDat_extension, SonyVars->DiskExtn);
		put_long(&DskDatV + kParamDiskStart, Sony_Start);
		put_long(&DskDatV + kParamDiskCount, Sony_Count);
		put_long(&DskDatV + kParamDiskBuffer, get_long(ParamBlk + kioBuffer));
		put_word(&DskDatV + kParamDiskDrive_No, Drive_No);

		switch (get_word(ParamBlk + kioTrap) & 0xF0FF) {
			case 0xA002: /* Reading */
				put_word(&DskDatV + DSKDat_commnd, kCmndDiskRead);
				put_long(SonyVars->pokeaddr, (ui5b)&DskDatV);
				Sony_Count = get_long(&DskDatV + kParamDiskCount);
				result = (si4b)get_word(&DskDatV + DSKDat_result);
				break;
			case 0xA003: /* Writing */
				if (get_byte(dvl + kWriteProt) != 0) {
					Sony_Count = 0;
					result = 0xFFD4; /* Write Protected (-44) */
				} else {
					put_word(&DskDatV + DSKDat_commnd, kCmndDiskWrite);
					put_long(SonyVars->pokeaddr, (ui5b)&DskDatV);
					Sony_Count = get_long(&DskDatV + kParamDiskCount);
					result = (si4b)get_word(&DskDatV + DSKDat_result);
				}
				break;
			default:
				return 0xFFEF; /* Driver can't respond to this Control call (-17) */
				break;
		}
	}


	put_word(ParamBlk + kioResult, result);
	put_long(ParamBlk + kioActCount, Sony_Count);
	put_long(DeviceCtl + kdCtlPosition, get_long(DeviceCtl + kdCtlPosition) + Sony_Count);

	return result;
}

extern void my_drive_icon(void);

/* Sony_Control */
/* Implements control csCodes for the Sony driver */
si4b Sony_Control (CPTR ParamBlk, CPTR DeviceCtl)
{
#pragma unused(DeviceCtl)
	si4b result;
	ui3b DskDatV[DSKDat_TotSize];
	ui5b OpCode = get_word(ParamBlk + kcsCode);
	ui4b Drive_No = get_word(ParamBlk + kioVRefNum) - 1;
	MyDriverDat_R *SonyVars = (MyDriverDat_R *)get_long(SonyVarsPtr);
	CPTR dvl = DriveVarsLocation(Drive_No);

	if (dvl == (ui5b)NULL) {
		result = 0xFFC8; /* No Such Drive (-56) */
	} else if (get_byte(dvl + kDiskInPlace) == 0) {
		result = 0xFFBF; /* Say it's offline (-65) */
	} else {
		put_word(&DskDatV + DSKDat_checkval, kcom_callcheck);
		put_word(&DskDatV + DSKDat_extension, SonyVars->DiskExtn);
		put_word(&DskDatV + kParamDiskDrive_No, Drive_No);
		switch (OpCode) {
			case kKillIO :
				result = 0xFFFF;
				break;
			case kVerifyDisk :
				result = 0x0000;
				break;
			case kEjectDisk :
				put_byte(dvl + kWriteProt, 0x00);   /* Drive Writeable */
				put_byte(dvl + kDiskInPlace, 0x00); /* Drive No Disk */
#if 0
				put_byte(dvl + kTwoSideFmt, 0x00);  /* Drive Single Format (Initially) */
#endif
				put_word(dvl + kQRefNum, 0xFFFB);   /* Drive i uses .Sony */

				put_word(&DskDatV + DSKDat_commnd, kCmndDiskEject);
				put_long(SonyVars->pokeaddr, (ui5b)&DskDatV);
				result = (si4b)get_word(&DskDatV + DSKDat_result);
				break;
			case kFormatDisk :
				result = 0x0000;
				break;
			case kReturnDriveIcon :
				if (get_word(dvl + kQType) != 0) {
					put_long(ParamBlk + kcsParam, (ui5b)my_drive_icon);
					result = 0x0000; /* No error (0) */
				} else {
					result = 0xFFEF; /* Driver can't respond to this Control call (-17) */
				}
				break;
			case kReturnDriveInfo :
				{
					ui5b v;

					if (get_word(dvl + kQType) != 0) {
						v = 0x00000001; /* unspecified drive */
					} else {
#if EmulatingMac128k
						v = 0x00000002; /* 400K Drive */
#else
						v = 0x00000003; /* 800K Drive */
#endif
					}
					if (Drive_No != 0) {
						v += 0x00000900; /* Secondary External Drive */
					}
					put_long(ParamBlk + kcsParam, v);
					result = 0x0000; /* No error (0) */
				}
				break;
			default :
				result = 0xFFEF; /* Driver can't respond to this Control call (-17) */
				break;
		}
	}
	return result;
}

/* Sony_Status */
/* Handles the DriveStatus call */
si4b Sony_Status (CPTR ParamBlk, CPTR DeviceCtl)
{
#pragma unused(DeviceCtl)
	CPTR Src;
	CPTR Dst;
	si4b result;
	int Counter;
	ui4b Drive_No = get_word(ParamBlk + kioVRefNum) - 1;

	if (get_word(ParamBlk + kcsCode) == kDriveStatus) { /* Drive Status Call */
		Src = DriveVarsLocation(Drive_No);
		if (Src == (ui5b)NULL) {
			result = 0xFFC8; /* No Such Drive (-56) */
		} else {
			Dst = ParamBlk + kcsParam;
			for (Counter = 11; --Counter >=0; ) {
				put_word(Dst, get_word(Src));
				Src += 2; Dst += 2;
			}
			result = 0x0000;
		}
	} else {
		result = 0xFFEE; /* Can't Respond to Status Call (-18) */
	}
	return result;
}

/* Sony_Close */
/* Returns immediate error */
si4b Sony_Close(CPTR ParamBlk, CPTR DeviceCtl)
{
#pragma unused(ParamBlk, DeviceCtl)
	return 0xFFE8; /* Can't Close Driver (-24) */
}
