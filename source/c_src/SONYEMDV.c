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
#include "EMCONFIG.h"
#include "ADDRSPAC.h"
#include "MINEM68K.h"
#endif

#include "SONYEMDV.h"

IMPORTFUNC ui4r Vid_GetMode(void);
IMPORTFUNC tMacErr Vid_SetMode(ui4r v);
IMPORTFUNC ui4r Vid_Reset(void);

#define kDSK_Params_Hi 0
#define kDSK_Params_Lo 1
#define kDSK_QuitOnEject 3 /* obsolete */

#define kDSK_numvars 4


enum {
	kExtnFindExtn, /* must be first */

	kExtnDisk,
#if (CurEmMd == kEmMd_II)
	kExtnVideo,
#endif
#if IncludePbufs
	kExtnParamBuffers,
#endif
#if IncludeHostTextClipExchange
	kExtnHostTextClipExchange,
#endif

	kNumExtns
};

#define kcom_checkval 0x841339E2
#define kcom_callcheck 0x5B17

#define kFindExtnExtension 0x64E1F58A
#define kDiskDriverExtension 0x4C9219E6
#if (CurEmMd == kEmMd_II)
#define kHostVideoExtension 0x163DA5A9
#endif
#if IncludePbufs
#define kHostParamBuffersExtension 0x314C87BF
#endif
#if IncludeHostTextClipExchange
#define kHostClipExchangeExtension 0x27B130CA
#endif

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

#if IncludePbufs
#define kCmndPbufFeatures 1
#define kCmndPbufNew 2
#define kCmndPbufDispose 3
#define kCmndPbufGetSize 4
#define kCmndPbufTransfer 5
#endif

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
#if IncludeSonyRawMode
#define kCmndDiskGetRawMode 11
#define kCmndDiskSetRawMode 12
#endif
#if IncludeSonyNew
#define kCmndDiskNew 13
#define kCmndDiskGetNewWanted 14
#define kCmndDiskEjectDelete 15
#endif
#if IncludeSonyGetName
#define kCmndDiskGetName 16
#endif

#define kFeatureCmndDisk_RawMode 0
#define kFeatureCmndDisk_New 1
#define kFeatureCmndDisk_NewName 2
#define kFeatureCmndDisk_GetName 3

#define kParamVersion 8

#define kParamFindExtnTheExtn 8
#define kParamFindExtnTheId 12

#define kParamDiskNumDrives 8
#define kParamDiskStart 8
#define kParamDiskCount 12
#define kParamDiskBuffer 16
#define kParamDiskDrive_No 20

#if IncludeHostTextClipExchange
#define kCmndHTCEFeatures 1
#define kCmndHTCEExport 2
#define kCmndHTCEImport 3
#endif

#if (CurEmMd == kEmMd_II)
#define kCmndVideoFeatures 1
#define kCmndVideoGetIntEnbl 2
#define kCmndVideoSetIntEnbl 3
#define kCmndVideoClearInt 4
#define kCmndVideoGetMode 5
#define kCmndVideoSetMode 6
#define kCmndVideoReset 7
#endif

#define MinTicksBetweenInsert 60
	/* if call PostEvent too frequently, insert events seem to get lost */

LOCALVAR ui4r DelayUntilNextInsert;

#define vSonyIsLocked(Drive_No) ((vSonyWritableMask & ((ui5b)1 << (Drive_No))) == 0)

LOCALVAR ui5r ImageOffset[NumDrives]; /* size of any header in disk image file */
LOCALVAR ui5r ImageSize[NumDrives]; /* size of disk image file contents */

#define checkheaderoffset 1024
#define checkheadersize 128

LOCALVAR CPTR MountCallBack = 0;

LOCALFUNC tMacErr vSonyNextPendingInsert(tDrive *Drive_No)
{
	ui5b MountPending = vSonyInsertedMask & (~ vSonyMountedMask);
	if (MountPending != 0) {
		tDrive i;
		for (i = 0; i < NumDrives; ++i) {
			if ((MountPending & ((ui5b)1 << i)) != 0) {
				tMacErr result;
				ui5r L;
				ui5r offset = 0;

				result = vSonyGetSize(i, &L);
				if (mnvm_noErr == result) {
#if IncludeSonyRawMode
					if (! vSonyRawMode)
#endif
					{
						ui3b Temp[checkheadersize];
						ui5r Sony_Count = checkheadersize;
						result = vSonyRead(Temp, i, checkheaderoffset, &Sony_Count);
						if (mnvm_noErr == result) {
							if ((Temp[0] == 0x42) && (Temp[1] == 0x44)) {
								/* hfs */
							} else if ((Temp[0] == 0xD2) && (Temp[1] == 0xD7)) {
								/* mfs */
							} else if ((Temp[84] == 0x42) && (Temp[85] == 0x44)) {
								/* hfs, old style disk image header */
								offset = 84;
							} else if ((Temp[84] == 0xD2) && (Temp[85] == 0xD7)) {
								/* mfs, old style disk image header */
								offset = 84;
							} else {
								/* perhaps msdos, or not a valid image */
							}
						}
					}
				}

				if (mnvm_noErr != result) {
					(void) vSonyEject(i);
				} else {
					vSonyMountedMask |= ((ui5b)1 << i);

					ImageOffset[i] = offset;
					ImageSize[i] = L - offset;

					*Drive_No = i;
				}

				return result; /* only one disk at a time */
			}
		}
	}

	return mnvm_nsDrvErr;
}

/* This checks to see if a disk (image) has been inserted */
GLOBALPROC Sony_Update (void)
{
	if (DelayUntilNextInsert != 0) {
		--DelayUntilNextInsert;
	} else {
		if (MountCallBack != 0) {
			tDrive i;

			if (mnvm_noErr == vSonyNextPendingInsert(&i)) {
				ui5b data = i;

				if (vSonyIsLocked(i)) {
					data |= ((ui5b)0x00FF) << 16;
				}

				DiskInsertedPsuedoException(MountCallBack, data);

				DelayUntilNextInsert = MinTicksBetweenInsert;
			}
		}
	}
}

#if IncludePbufs
LOCALFUNC tMacErr CheckPbuf(tPbuf Pbuf_No)
{
	tMacErr result;

	if (Pbuf_No >= NumPbufs) {
		result = mnvm_nsDrvErr;
	} else if (! PbufIsAllocated(Pbuf_No)) {
		result = mnvm_offLinErr;
	} else {
		result = mnvm_noErr;
	}

	return result;
}
#endif

LOCALFUNC tMacErr CheckReadableDrive(tDrive Drive_No)
{
	tMacErr result;

	if (Drive_No >= NumDrives) {
		result = mnvm_nsDrvErr;
	} else if (! vSonyIsMounted(Drive_No)) {
		result = mnvm_offLinErr;
	} else {
		result = mnvm_noErr;
	}

	return result;
}

LOCALFUNC tMacErr CheckWriteableDrive(tDrive Drive_No)
{
	tMacErr result;

	if (Drive_No >= NumDrives) {
		result = mnvm_nsDrvErr;
	} else if (! vSonyIsMounted(Drive_No)) {
		result = mnvm_offLinErr;
	} else if (vSonyIsLocked(Drive_No)) {
		result = mnvm_vLckdErr;
	} else {
		result = mnvm_noErr;
	}

	return result;
}

LOCALVAR blnr QuitOnEject = falseblnr;
LOCALVAR ui4b ParamAddrHi;

#if CurEmMd == kEmMd_II
EXPORTPROC PowerOff_ChangeNtfy(void);
GLOBALPROC PowerOff_ChangeNtfy(void)
{
	if (! VIA2_iB2) {
		ForceMacOff = trueblnr;
	}
}
#endif

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
					tMacErr result = mnvm_controlErr;
					ui4b extn_id = do_get_mem_word(p + DSKDat_extension);
					ui4b command = do_get_mem_word(p + DSKDat_commnd);

					switch (extn_id) {
						case kExtnFindExtn:
							switch (command) {
								case kCmndVersion:
									do_put_mem_word(p + kParamVersion, 1);
									result = mnvm_noErr;
									break;
								case kCmndFindExtnFind:
									{
										ui5b extn = do_get_mem_long(p + kParamFindExtnTheExtn);

										if (extn == kDiskDriverExtension) {
											do_put_mem_word(p + kParamFindExtnTheId, kExtnDisk);
											result = mnvm_noErr;
										} else
#if (CurEmMd == kEmMd_II)
										if (extn == kHostVideoExtension) {
											do_put_mem_word(p + kParamFindExtnTheId, kExtnVideo);
											result = mnvm_noErr;
										} else
#endif
#if IncludePbufs
										if (extn == kHostParamBuffersExtension) {
											do_put_mem_word(p + kParamFindExtnTheId, kExtnParamBuffers);
											result = mnvm_noErr;
										} else
#endif
#if IncludeHostTextClipExchange
										if (extn == kHostClipExchangeExtension) {
											do_put_mem_word(p + kParamFindExtnTheId, kExtnHostTextClipExchange);
											result = mnvm_noErr;
										} else
#endif
										if (extn == kFindExtnExtension) {
											do_put_mem_word(p + kParamFindExtnTheId, kExtnFindExtn);
											result = mnvm_noErr;
										} else
										{
											/* not found */
										}
									}
									break;
								case kCmndFindExtnId2Code:
									{
										ui4b extn = do_get_mem_word(p + kParamFindExtnTheId);

										if (extn == kExtnDisk) {
											do_put_mem_long(p + kParamFindExtnTheExtn, kDiskDriverExtension);
											result = mnvm_noErr;
										} else
#if (CurEmMd == kEmMd_II)
										if (extn == kExtnVideo) {
											do_put_mem_long(p + kParamFindExtnTheExtn, kHostVideoExtension);
											result = mnvm_noErr;
										} else
#endif
#if IncludePbufs
										if (extn == kExtnParamBuffers) {
											do_put_mem_long(p + kParamFindExtnTheExtn, kHostParamBuffersExtension);
											result = mnvm_noErr;
										} else
#endif
#if IncludeHostTextClipExchange
										if (extn == kExtnHostTextClipExchange) {
											do_put_mem_long(p + kParamFindExtnTheExtn, kHostClipExchangeExtension);
											result = mnvm_noErr;
										} else
#endif
										if (extn == kExtnFindExtn) {
											do_put_mem_long(p + kParamFindExtnTheExtn, kFindExtnExtension);
											result = mnvm_noErr;
										} else
										{
											/* not found */
										}
									}
									break;
								case kCmndFindExtnCount:
									do_put_mem_word(p + kParamFindExtnTheId, kNumExtns);
									result = mnvm_noErr;
									break;
							}
							break;
#if (CurEmMd == kEmMd_II)
						case kExtnVideo:
							switch (command) {
								case kCmndVersion:
									do_put_mem_word(p + kParamVersion, 1);
									result = mnvm_noErr;
									break;
								case kCmndVideoGetIntEnbl:
									do_put_mem_word(p + 8,
										Vid_VBLintunenbl ? 0 : 1);
									result = mnvm_noErr;
									break;
								case kCmndVideoSetIntEnbl:
									Vid_VBLintunenbl =
										(0 == do_get_mem_word(p + 8))
											? 1 : 0;
									result = mnvm_noErr;
									break;
								case kCmndVideoClearInt:
									Vid_VBLinterrupt = 1;
									result = mnvm_noErr;
									break;
								case kCmndVideoGetMode:
									do_put_mem_word(p + 8,
										Vid_GetMode());
									result = mnvm_noErr;
									break;
								case kCmndVideoSetMode:
									result = Vid_SetMode(do_get_mem_word(p + 8));
									break;
								case kCmndVideoReset:
									do_put_mem_word(p + 8,
										Vid_GetMode());
									result = mnvm_noErr;
									break;
							}
							break;
#endif
#if IncludePbufs
						case kExtnParamBuffers:
							switch (command) {
								case kCmndVersion:
									do_put_mem_word(p + kParamVersion, 1);
									result = mnvm_noErr;
									break;
								case kCmndPbufFeatures:
									do_put_mem_long(p + DSKDat_params + 0, 0);
									result = mnvm_noErr;
									break;
								case kCmndPbufNew:
									{
										tPbuf Pbuf_No;
										ui5b count = do_get_mem_long(p + DSKDat_params + 4);
										/* reserved word at offset 2, should be zero */
										result = PbufNew(count, &Pbuf_No);
										do_put_mem_word(p + DSKDat_params + 0, Pbuf_No);
									}
									break;
								case kCmndPbufDispose:
									{
										tPbuf Pbuf_No = do_get_mem_word(p + DSKDat_params + 0);
										/* reserved word at offset 2, should be zero */
										result = CheckPbuf(Pbuf_No);
										if (mnvm_noErr == result) {
											PbufDispose(Pbuf_No);
										}
									}
									break;
								case kCmndPbufGetSize:
									{
										tPbuf Pbuf_No = do_get_mem_word(p + DSKDat_params + 0);
										/* reserved word at offset 2, should be zero */

										result = CheckPbuf(Pbuf_No);
										if (mnvm_noErr == result) {
											do_put_mem_long(p + DSKDat_params + 4, PbufSize[Pbuf_No]);
										}
									}
									break;
								case kCmndPbufTransfer:
									{
										tPbuf Pbuf_No = do_get_mem_word(p + DSKDat_params + 0);
										/* reserved word at offset 2, should be zero */
										ui5r offset = do_get_mem_long(p + DSKDat_params + 4);
										ui5r count = do_get_mem_long(p + DSKDat_params + 8);
										CPTR Buffera = do_get_mem_long(p + DSKDat_params + 12);
										blnr IsWrite = (do_get_mem_word(p + DSKDat_params + 16) != 0);
										result = CheckPbuf(Pbuf_No);
										if (mnvm_noErr == result) {
											ui5r endoff = offset + count;
											if ((endoff < offset) /* overflow */
												|| (endoff > PbufSize[Pbuf_No]))
											{
												result = mnvm_eofErr;
											} else {
												ui3p Buffer = get_real_address(count, trueblnr, Buffera);
												if (Buffer == nullpr) {
													result = mnvm_miscErr;
												} else {
													PbufTransfer(Buffer, Pbuf_No, offset, count,
														IsWrite);
												}
											}
										}
									}
									break;
							}
							break;
#endif
#if IncludeHostTextClipExchange
						case kExtnHostTextClipExchange:
							switch (command) {
								case kCmndVersion:
									do_put_mem_word(p + kParamVersion, 1);
									result = mnvm_noErr;
									break;
								case kCmndHTCEFeatures:
									do_put_mem_long(p + DSKDat_params + 0, 0);
									result = mnvm_noErr;
									break;
								case kCmndHTCEExport:
									{
										tPbuf Pbuf_No = do_get_mem_word(p + DSKDat_params + 0);

										result = CheckPbuf(Pbuf_No);
										if (mnvm_noErr == result) {
											result = HTCEexport(Pbuf_No);
										}
									}
									break;
								case kCmndHTCEImport:
									{
										tPbuf Pbuf_No;
										result = HTCEimport(&Pbuf_No);
										do_put_mem_word(p + DSKDat_params + 0, Pbuf_No);
									}
									break;
							}
							break;
#endif
						case kExtnDisk:
							switch (command) {
								case kCmndVersion:
									do_put_mem_word(p + kParamVersion, 2);
									result = mnvm_noErr;
									break;
								case kCmndDiskNDrives: /* count drives */
									do_put_mem_word(p + kParamDiskNumDrives, NumDrives);
									result = mnvm_noErr;
									break;
								case kCmndDiskRead:
									{
										ui5r NewSony_Count = 0;
										tDrive Drive_No = do_get_mem_word(p + kParamDiskDrive_No);
										result = CheckReadableDrive(Drive_No);
										if (mnvm_noErr == result) {
											ui5r Sony_Start = do_get_mem_long(p + kParamDiskStart);
											ui5r Sony_Count = do_get_mem_long(p + kParamDiskCount);
											ui5r Sony_End = Sony_Start + Sony_Count;
											if ((Sony_End < Sony_Start) /* overflow */
												|| (Sony_End > ImageSize[Drive_No]))
											{
												result = mnvm_eofErr;
											} else {
												CPTR Buffera = do_get_mem_long(p + kParamDiskBuffer);
												ui3p Buffer = get_real_address(Sony_Count, trueblnr, Buffera);
												if (Buffer == nullpr) {
													result = mnvm_miscErr;
												} else {
													result = vSonyRead(Buffer, Drive_No, ImageOffset[Drive_No] + Sony_Start, &Sony_Count);
													NewSony_Count = Sony_Count;
												}
											}
										}
										do_put_mem_long(p + kParamDiskCount, NewSony_Count);
									}
									break;
								case kCmndDiskWrite:
									{
										ui5r NewSony_Count = 0;
										tDrive Drive_No = do_get_mem_word(p + kParamDiskDrive_No);
										result = CheckWriteableDrive(Drive_No);
										if (mnvm_noErr == result) {
											ui5r Sony_Start = do_get_mem_long(p + kParamDiskStart);
											ui5r Sony_Count = do_get_mem_long(p + kParamDiskCount);
											ui5r Sony_End = Sony_Start + Sony_Count;
											if ((Sony_End < Sony_Start) /* overflow */
												|| (Sony_End > ImageSize[Drive_No]))
											{
												result = mnvm_eofErr;
											} else {
												CPTR Buffera = do_get_mem_long(p + kParamDiskBuffer);
												ui3p Buffer = get_real_address(Sony_Count, falseblnr, Buffera);
												if (Buffer == nullpr) {
													result = mnvm_miscErr;
												} else {
													result = vSonyWrite(Buffer, Drive_No, ImageOffset[Drive_No] + Sony_Start, &Sony_Count);
													NewSony_Count = Sony_Count;
												}
											}
										}
										do_put_mem_long(p + kParamDiskCount, NewSony_Count);
									}
									break;
								case kCmndDiskEject:
									{
										tDrive Drive_No = do_get_mem_word(p + kParamDiskDrive_No);
										result = CheckReadableDrive(Drive_No);
										if (mnvm_noErr == result) {
											result = vSonyEject(Drive_No);
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
										tDrive Drive_No = do_get_mem_word(p + kParamDiskDrive_No);

										result = CheckReadableDrive(Drive_No);
										if (mnvm_noErr == result) {
											do_put_mem_long(p + kParamDiskCount, ImageSize[Drive_No]);
											result = mnvm_noErr;
										}
									}
									break;
								case kCmndDiskGetCallBack:
									do_put_mem_long(p + kParamDiskBuffer, MountCallBack);
									result = mnvm_noErr;
									break;
								case kCmndDiskSetCallBack:
									MountCallBack = do_get_mem_long(p + kParamDiskBuffer);
									result = mnvm_noErr;
									break;
								case kCmndDiskQuitOnEject:
									QuitOnEject = trueblnr;
									result = mnvm_noErr;
									break;
								case kCmndDiskFeatures:
									{
										ui5b v = (0
#if IncludeSonyRawMode
											| ((ui5b)1 << kFeatureCmndDisk_RawMode)
#endif
#if IncludeSonyNew
											| ((ui5b)1 << kFeatureCmndDisk_New)
#endif
#if IncludeSonyNameNew
											| ((ui5b)1 << kFeatureCmndDisk_NewName)
#endif
#if IncludeSonyGetName
											| ((ui5b)1 << kFeatureCmndDisk_GetName)
#endif
											);

										do_put_mem_long(p + DSKDat_params + 0, v);
										result = mnvm_noErr;
									}
									break;
								case kCmndDiskNextPendingInsert:
									{
										tDrive i;

										result = vSonyNextPendingInsert(&i);
										if (mnvm_noErr == result) {
											do_put_mem_word(p + kParamDiskDrive_No, i);
										}
									}
									break;
#if IncludeSonyRawMode
								case kCmndDiskGetRawMode:
									do_put_mem_word(p + kParamDiskBuffer, vSonyRawMode);
									result = mnvm_noErr;
									break;
								case kCmndDiskSetRawMode:
									vSonyRawMode = do_get_mem_word(p + kParamDiskBuffer);
									result = mnvm_noErr;
									break;
#endif
#if IncludeSonyNew
								case kCmndDiskNew:
									{
										ui5b count = do_get_mem_long(p + DSKDat_params + 0);
										tPbuf Pbuf_No = do_get_mem_word(p + DSKDat_params + 4);
										/* reserved word at offset 6, should be zero */

										result = mnvm_noErr;

#if IncludePbufs
										if (Pbuf_No != NotAPbuf) {
											result = CheckPbuf(Pbuf_No);
											if (mnvm_noErr == result) {
												vSonyNewDiskWanted = trueblnr;
												vSonyNewDiskSize = count;
#if IncludeSonyNameNew
												if (vSonyNewDiskName != NotAPbuf) {
													PbufDispose(vSonyNewDiskName);
												}
												vSonyNewDiskName = Pbuf_No;
#else
												PbufDispose(Pbuf_No);
#endif
											}
										} else
#endif
										{
											vSonyNewDiskWanted = trueblnr;
											vSonyNewDiskSize = count;
										}
									}
									break;
								case kCmndDiskGetNewWanted:
									do_put_mem_word(p + kParamDiskBuffer, vSonyNewDiskWanted);
									result = mnvm_noErr;
									break;
								case kCmndDiskEjectDelete:
									{
										tDrive Drive_No = do_get_mem_word(p + kParamDiskDrive_No);
										result = CheckWriteableDrive(Drive_No);
										if (mnvm_noErr == result) {
											result = vSonyEjectDelete(Drive_No);
										}
									}
									break;
#endif
#if IncludeSonyGetName
								case kCmndDiskGetName:
									{
										tDrive Drive_No = do_get_mem_word(p + DSKDat_params + 0);
										/* reserved word at offset 2, should be zero */
										result = CheckReadableDrive(Drive_No);
										if (mnvm_noErr == result) {
											tPbuf Pbuf_No;
											result = vSonyGetName(Drive_No, &Pbuf_No);
											do_put_mem_word(p + DSKDat_params + 4, Pbuf_No);
										}
									}
									break;
#endif
							}
							break;
					}

					do_put_mem_word(p + DSKDat_result, result);
					do_put_mem_word(p + DSKDat_checkval, 0);
					ParamAddrHi = (ui4b) - 1;
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
	ParamAddrHi = (ui4b) - 1;
}
