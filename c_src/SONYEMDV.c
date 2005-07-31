/*
	SONYEMDV.c

	Copyright (C) 2004 Paul C. Pratt

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
	SONY floppy disk EMulated DeVice

	This is now misnamed. The only code remaining in this file
	implements a fake memory mapped device that is used by
	a replacement for the Sony disk driver. The Sony hardware
	is not emulated.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#include "MYOSGLUE.h"
#include "ENDIANAC.h"
#include "MINEM68K.h"
#include "ADDRSPAC.h"
#endif

#include "SONYEMDV.h"

#define kDSK_Params_Hi 0
#define kDSK_Params_Lo 1
#define kDSK_QuitOnEject 3 /* obsolete */

#define kDSK_numvars 4


enum {
	kExtnFindExtn, /* must be first */

	kExtnDisk,

	kNumExtns
};

#define kcom_checkval 0x841339E2
#define kcom_callcheck 0x5B17

#define kFindExtnExtension 0x64E1F58A
#define kDiskDriverExtension 0x4C9219E6


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


#define MinTicksBetweenInsert 60
	/* if call PostEvent too frequently, insert events seem to get lost */

LOCALVAR ui4b DelayUntilNextInsert;

LOCALVAR ui5b ImageOffset[NumDrives]; /* size of any header in disk image file */

#define checkheaderoffset 1024
#define checkheadersize 128

LOCALVAR CPTR MountCallBack = 0;

/* This checks to see if a disk (image) has been inserted */
GLOBALPROC Sony_Update (void)
{
	if (DelayUntilNextInsert != 0) {
		--DelayUntilNextInsert;
	} else {
		ui5b MountPending = vSonyInsertedMask & (~ vSonyMountedMask);

		if (MountPending != 0) {
			if (MountCallBack != 0) {
				int i;
				ui5b data;
				ui5b Sony_Count;
				ui3b Temp[checkheadersize];
				si4b result;

				for (i = 0; i < NumDrives; ++i) {
					if ((MountPending & ((ui5b)1 << i)) != 0) {
						vSonyMountedMask |= ((ui5b)1 << i);
						ImageOffset[i] = 0;
						Sony_Count = checkheadersize;
						result = vSonyRead((void *)&Temp, i, checkheaderoffset, &Sony_Count);
						if (result == 0) {
							if ((Temp[0] == 0x42) && (Temp[1] == 0x44)) {
								/* hfs */
							} else if ((Temp[0] == 0xD2) && (Temp[1] == 0xD7)) {
								/* mfs */
							} else if ((Temp[84] == 0x42) && (Temp[85] == 0x44)) {
								/* hfs, old style disk image header */
								ImageOffset[i] = 84;
							} else if ((Temp[84] == 0xD2) && (Temp[85] == 0xD7)) {
								/* mfs, old style disk image header */
								ImageOffset[i] = 84;
							} else {
								/* probably not a valid image */
							}
						}

						DelayUntilNextInsert = MinTicksBetweenInsert;
						data = i;

						if ((vSonyWritableMask & ((ui5b)1 << i)) == 0) {
							data |= ((ui5b)0x00FF) << 16;
						}
						DiskInsertedPsuedoException(MountCallBack, data);
						return; /* only one disk at a time */
					}
				}
			}
		}
	}
}

LOCALVAR blnr QuitOnEject = falseblnr;
LOCALVAR ui4b ParamAddrHi;

GLOBALPROC Sony_Access(ui5b Data, CPTR addr)
{
	switch (addr) {
		case kDSK_Params_Hi:
			ParamAddrHi = Data;
			break;
		case kDSK_Params_Lo:
			{
				ui3p p = (void*)get_real_address(64, trueblnr, ParamAddrHi << 16 | Data);

				if (p != nullpr)
				if (do_get_mem_word(p + DSKDat_checkval) == kcom_callcheck)
				{
					ui5b result = 0xFFEF;
					ui4b extn_id = do_get_mem_word(p + DSKDat_extension);
					ui4b command = do_get_mem_word(p + DSKDat_commnd);

					switch (extn_id) {
						case kExtnFindExtn:
							switch (command) {
								case kCmndVersion:
									do_put_mem_word(p + kParamVersion, 1);
									result = 0x0000;
									break;
								case kCmndFindExtnFind:
									{
										ui5b extn = do_get_mem_long(p + kParamFindExtnTheExtn);

										if (extn == kDiskDriverExtension) {
											do_put_mem_word(p + kParamFindExtnTheId, kExtnDisk);
											result = 0x0000;
										} else if (extn == kFindExtnExtension) {
											do_put_mem_word(p + kParamFindExtnTheId, kExtnFindExtn);
											result = 0x0000;
										}
									}
									break;
								case kCmndFindExtnId2Code:
									{
										ui4b extn = do_get_mem_word(p + kParamFindExtnTheId);

										if (extn == kExtnDisk) {
											do_put_mem_long(p + kParamFindExtnTheExtn, kDiskDriverExtension);
											result = 0x0000;
										} else if (extn == kExtnFindExtn) {
											do_put_mem_long(p + kParamFindExtnTheExtn, kFindExtnExtension);
											result = 0x0000;
										}
									}
									break;
								case kCmndFindExtnCount:
									do_put_mem_word(p + kParamFindExtnTheId, kNumExtns);
									result = 0x0000;
									break;
							}
							break;
						case kExtnDisk:
							switch (command) {
								case kCmndVersion:
									do_put_mem_word(p + kParamVersion, 1);
									result = 0x0000;
									break;
								case kCmndDiskNDrives: /* count drives */
									do_put_mem_word(p + kParamDiskNumDrives, NumDrives);
									result = 0x0000;
									break;
								case kCmndDiskRead:
									{
										ui5b NewSony_Count = 0;
										ui4b Drive_No = do_get_mem_word(p + kParamDiskDrive_No);
										if (Drive_No >= NumDrives) {
											result = 0xFFC8; /* No Such Drive (-56) */
										} else if (! vSonyIsMounted(Drive_No)) {
											result = 0xFFBF; /* Say it's offline (-65) */
										} else {
											ui5b Sony_Start = do_get_mem_long(p + kParamDiskStart);
											ui5b Sony_Count = do_get_mem_long(p + kParamDiskCount);
											CPTR Buffera = do_get_mem_long(p + kParamDiskBuffer);
											void *Buffer = (void*)get_real_address(Sony_Count, trueblnr, Buffera);
											if (Buffer == nullpr) {
												result = -1;
											} else {
												result = vSonyRead(Buffer, Drive_No, ImageOffset[Drive_No] + Sony_Start, &Sony_Count);
												NewSony_Count = Sony_Count;
											}
										}
										do_put_mem_long(p + kParamDiskCount, NewSony_Count);
									}
									break;
								case kCmndDiskWrite:
									{
										ui5b NewSony_Count = 0;
										ui4b Drive_No = do_get_mem_word(p + kParamDiskDrive_No);
										if (Drive_No >= NumDrives) {
											result = 0xFFC8; /* No Such Drive (-56) */
										} else if (! vSonyIsMounted(Drive_No)) {
											result = 0xFFBF; /* Say it's offline (-65) */
										} else {
											ui5b Sony_Start = do_get_mem_long(p + kParamDiskStart);
											ui5b Sony_Count = do_get_mem_long(p + kParamDiskCount);
											CPTR Buffera = do_get_mem_long(p + kParamDiskBuffer);
											void *Buffer = (void*)get_real_address(Sony_Count, falseblnr, Buffera);
											if (Buffer == nullpr) {
												result = -1;
											} else {
												result = vSonyWrite(Buffer, Drive_No, ImageOffset[Drive_No] + Sony_Start, &Sony_Count);
												NewSony_Count = Sony_Count;
											}
										}
										do_put_mem_long(p + kParamDiskCount, NewSony_Count);
									}
									break;
								case kCmndDiskEject:
									{
										ui4b Drive_No = do_get_mem_word(p + kParamDiskDrive_No);
										if (Drive_No >= NumDrives) {
											result = 0xFFC8; /* No Such Drive (-56) */
										} else if (! vSonyIsMounted(Drive_No)) {
											result = 0xFFBF; /* Say it's offline (-65) */
										} else {
											result = vSonyEject(Drive_No);
											vSonyWritableMask &= ~ ((ui5b)1 << Drive_No);
											vSonyInsertedMask &= ~ ((ui5b)1 << Drive_No);
											vSonyMountedMask &= ~ ((ui5b)1 << Drive_No);
											if (QuitOnEject != 0) {
												if (! AnyDiskInserted()) {
													ForceMacOff = trueblnr;
												}
											}
										}
									}
									break;
								case kCmndDiskGetSize:
									{
										ui4b Drive_No = do_get_mem_word(p + kParamDiskDrive_No);

										if (Drive_No >= NumDrives) {
											result = 0xFFC8; /* No Such Drive (-56) */
										} else if (! vSonyIsMounted(Drive_No)) {
											result = 0xFFBF; /* Say it's offline (-65) */
										} else {
											ui5b Sony_Count;
											result = vSonyGetSize(Drive_No, &Sony_Count);
											do_put_mem_long(p + kParamDiskCount, Sony_Count - ImageOffset[Drive_No]);
										}
									}
									break;
								case kCmndDiskGetCallBack:
									do_put_mem_long(p + kParamDiskBuffer, MountCallBack);
									result = 0x0000;
									break;
								case kCmndDiskSetCallBack:
									MountCallBack = do_get_mem_long(p + kParamDiskBuffer);
									result = 0x0000;
									break;
								case kCmndDiskQuitOnEject:
									QuitOnEject = trueblnr;
									result = 0x0000;
									break;
							}
							break;
					}

					do_put_mem_word(p + DSKDat_result, result);
					do_put_mem_word(p + DSKDat_checkval, 0);
					ParamAddrHi = -1;
				}
			}
			break;
		case kDSK_QuitOnEject:
			/* obsolete, kept for compatibility */
			QuitOnEject = trueblnr;
			break;
	}
}

GLOBALPROC Sony_Reset(void)
{
	DelayUntilNextInsert = 0;
	QuitOnEject = falseblnr;
	MountCallBack = 0;
	ParamAddrHi = -1;
}
