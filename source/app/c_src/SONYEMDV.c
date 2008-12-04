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
#include "GLOBGLUE.h"
#include "MINEM68K.h"
#endif

#include "SONYEMDV.h"

#if EmVidCard
IMPORTPROC ExtnVideo_Access(CPTR p);
#endif

#define kDSK_Params_Hi 0
#define kDSK_Params_Lo 1
#define kDSK_QuitOnEject 3 /* obsolete */

#define kDSK_numvars 4


enum {
	kExtnFindExtn, /* must be first */

	kExtnDisk,
#if EmVidCard
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
#if EmVidCard
#define kHostVideoExtension 0x163DA5A9
#endif
#if IncludePbufs
#define kHostParamBuffersExtension 0x314C87BF
#endif
#if IncludeHostTextClipExchange
#define kHostClipExchangeExtension 0x27B130CA
#endif

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

#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
EXPORTPROC PowerOff_ChangeNtfy(void);
GLOBALPROC PowerOff_ChangeNtfy(void)
{
	if (! VIA2_iB2) {
		ForceMacOff = trueblnr;
	}
}
#endif

LOCALPROC ExtnFind_Access(CPTR p)
{
	tMacErr result = mnvm_controlErr;

	switch (get_vm_word(p + ExtnDat_commnd)) {
		case kCmndVersion:
			put_vm_word(p + ExtnDat_version, 1);
			result = mnvm_noErr;
			break;
		case kCmndFindExtnFind:
			{
				ui5b extn = get_vm_long(p + kParamFindExtnTheExtn);

				if (extn == kDiskDriverExtension) {
					put_vm_word(p + kParamFindExtnTheId, kExtnDisk);
					result = mnvm_noErr;
				} else
#if EmVidCard
				if (extn == kHostVideoExtension) {
					put_vm_word(p + kParamFindExtnTheId, kExtnVideo);
					result = mnvm_noErr;
				} else
#endif
#if IncludePbufs
				if (extn == kHostParamBuffersExtension) {
					put_vm_word(p + kParamFindExtnTheId, kExtnParamBuffers);
					result = mnvm_noErr;
				} else
#endif
#if IncludeHostTextClipExchange
				if (extn == kHostClipExchangeExtension) {
					put_vm_word(p + kParamFindExtnTheId, kExtnHostTextClipExchange);
					result = mnvm_noErr;
				} else
#endif
				if (extn == kFindExtnExtension) {
					put_vm_word(p + kParamFindExtnTheId, kExtnFindExtn);
					result = mnvm_noErr;
				} else
				{
					/* not found */
				}
			}
			break;
		case kCmndFindExtnId2Code:
			{
				ui4r extn = get_vm_word(p + kParamFindExtnTheId);

				if (extn == kExtnDisk) {
					put_vm_long(p + kParamFindExtnTheExtn, kDiskDriverExtension);
					result = mnvm_noErr;
				} else
#if EmVidCard
				if (extn == kExtnVideo) {
					put_vm_long(p + kParamFindExtnTheExtn, kHostVideoExtension);
					result = mnvm_noErr;
				} else
#endif
#if IncludePbufs
				if (extn == kExtnParamBuffers) {
					put_vm_long(p + kParamFindExtnTheExtn, kHostParamBuffersExtension);
					result = mnvm_noErr;
				} else
#endif
#if IncludeHostTextClipExchange
				if (extn == kExtnHostTextClipExchange) {
					put_vm_long(p + kParamFindExtnTheExtn, kHostClipExchangeExtension);
					result = mnvm_noErr;
				} else
#endif
				if (extn == kExtnFindExtn) {
					put_vm_long(p + kParamFindExtnTheExtn, kFindExtnExtension);
					result = mnvm_noErr;
				} else
				{
					/* not found */
				}
			}
			break;
		case kCmndFindExtnCount:
			put_vm_word(p + kParamFindExtnTheId, kNumExtns);
			result = mnvm_noErr;
			break;
	}

	put_vm_word(p + ExtnDat_result, result);
}

#if IncludePbufs
LOCALPROC ExtnParamBuffers_Access(CPTR p)
{
	tMacErr result = mnvm_controlErr;

	switch (get_vm_word(p + ExtnDat_commnd)) {
		case kCmndVersion:
			put_vm_word(p + ExtnDat_version, 1);
			result = mnvm_noErr;
			break;
		case kCmndPbufFeatures:
			put_vm_long(p + ExtnDat_params + 0, 0);
			result = mnvm_noErr;
			break;
		case kCmndPbufNew:
			{
				tPbuf Pbuf_No;
				ui5b count = get_vm_long(p + ExtnDat_params + 4);
				/* reserved word at offset 2, should be zero */
				result = PbufNew(count, &Pbuf_No);
				put_vm_word(p + ExtnDat_params + 0, Pbuf_No);
			}
			break;
		case kCmndPbufDispose:
			{
				tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);
				/* reserved word at offset 2, should be zero */
				result = CheckPbuf(Pbuf_No);
				if (mnvm_noErr == result) {
					PbufDispose(Pbuf_No);
				}
			}
			break;
		case kCmndPbufGetSize:
			{
				tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);
				/* reserved word at offset 2, should be zero */

				result = CheckPbuf(Pbuf_No);
				if (mnvm_noErr == result) {
					put_vm_long(p + ExtnDat_params + 4, PbufSize[Pbuf_No]);
				}
			}
			break;
		case kCmndPbufTransfer:
			{
				tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);
				/* reserved word at offset 2, should be zero */
				ui5r offset = get_vm_long(p + ExtnDat_params + 4);
				ui5r count = get_vm_long(p + ExtnDat_params + 8);
				CPTR Buffera = get_vm_long(p + ExtnDat_params + 12);
				blnr IsWrite = (get_vm_word(p + ExtnDat_params + 16) != 0);
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

	put_vm_word(p + ExtnDat_result, result);
}
#endif

#if IncludeHostTextClipExchange
LOCALPROC ExtnHostTextClipExchange_Access(CPTR p)
{
	tMacErr result = mnvm_controlErr;

	switch (get_vm_word(p + ExtnDat_commnd)) {
		case kCmndVersion:
			put_vm_word(p + ExtnDat_version, 1);
			result = mnvm_noErr;
			break;
		case kCmndHTCEFeatures:
			put_vm_long(p + ExtnDat_params + 0, 0);
			result = mnvm_noErr;
			break;
		case kCmndHTCEExport:
			{
				tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 0);

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
				put_vm_word(p + ExtnDat_params + 0, Pbuf_No);
			}
			break;
	}

	put_vm_word(p + ExtnDat_result, result);
}
#endif

LOCALPROC ExtnDisk_Access(CPTR p)
{
	tMacErr result = mnvm_controlErr;

	switch (get_vm_word(p + ExtnDat_commnd)) {
		case kCmndVersion:
			put_vm_word(p + ExtnDat_version, 2);
			result = mnvm_noErr;
			break;
		case kCmndDiskNDrives: /* count drives */
			put_vm_word(p + kParamDiskNumDrives, NumDrives);
			result = mnvm_noErr;
			break;
		case kCmndDiskRead:
			{
				ui5r NewSony_Count = 0;
				tDrive Drive_No = get_vm_word(p + kParamDiskDrive_No);
				result = CheckReadableDrive(Drive_No);
				if (mnvm_noErr == result) {
					ui5r Sony_Start = get_vm_long(p + kParamDiskStart);
					ui5r Sony_Count = get_vm_long(p + kParamDiskCount);
					ui5r Sony_End = Sony_Start + Sony_Count;
					if ((Sony_End < Sony_Start) /* overflow */
						|| (Sony_End > ImageSize[Drive_No]))
					{
						result = mnvm_eofErr;
					} else {
						CPTR Buffera = get_vm_long(p + kParamDiskBuffer);
						ui3p Buffer = get_real_address(Sony_Count, trueblnr, Buffera);
						if (Buffer == nullpr) {
							result = mnvm_miscErr;
						} else {
							result = vSonyRead(Buffer, Drive_No, ImageOffset[Drive_No] + Sony_Start, &Sony_Count);
							NewSony_Count = Sony_Count;
						}
					}
				}
				put_vm_long(p + kParamDiskCount, NewSony_Count);
			}
			break;
		case kCmndDiskWrite:
			{
				ui5r NewSony_Count = 0;
				tDrive Drive_No = get_vm_word(p + kParamDiskDrive_No);
				result = CheckWriteableDrive(Drive_No);
				if (mnvm_noErr == result) {
					ui5r Sony_Start = get_vm_long(p + kParamDiskStart);
					ui5r Sony_Count = get_vm_long(p + kParamDiskCount);
					ui5r Sony_End = Sony_Start + Sony_Count;
					if ((Sony_End < Sony_Start) /* overflow */
						|| (Sony_End > ImageSize[Drive_No]))
					{
						result = mnvm_eofErr;
					} else {
						CPTR Buffera = get_vm_long(p + kParamDiskBuffer);
						ui3p Buffer = get_real_address(Sony_Count, falseblnr, Buffera);
						if (Buffer == nullpr) {
							result = mnvm_miscErr;
						} else {
							result = vSonyWrite(Buffer, Drive_No, ImageOffset[Drive_No] + Sony_Start, &Sony_Count);
							NewSony_Count = Sony_Count;
						}
					}
				}
				put_vm_long(p + kParamDiskCount, NewSony_Count);
			}
			break;
		case kCmndDiskEject:
			{
				tDrive Drive_No = get_vm_word(p + kParamDiskDrive_No);
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
				tDrive Drive_No = get_vm_word(p + kParamDiskDrive_No);

				result = CheckReadableDrive(Drive_No);
				if (mnvm_noErr == result) {
					put_vm_long(p + kParamDiskCount, ImageSize[Drive_No]);
					result = mnvm_noErr;
				}
			}
			break;
		case kCmndDiskGetCallBack:
			put_vm_long(p + kParamDiskBuffer, MountCallBack);
			result = mnvm_noErr;
			break;
		case kCmndDiskSetCallBack:
			MountCallBack = get_vm_long(p + kParamDiskBuffer);
			result = mnvm_noErr;
			break;
		case kCmndDiskQuitOnEject:
			QuitOnEject = trueblnr;
			result = mnvm_noErr;
			break;
		case kCmndDiskFeatures:
			{
				ui5r v = (0
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

				put_vm_long(p + ExtnDat_params + 0, v);
				result = mnvm_noErr;
			}
			break;
		case kCmndDiskNextPendingInsert:
			{
				tDrive i;

				result = vSonyNextPendingInsert(&i);
				if (mnvm_noErr == result) {
					put_vm_word(p + kParamDiskDrive_No, i);
				}
			}
			break;
#if IncludeSonyRawMode
		case kCmndDiskGetRawMode:
			put_vm_word(p + kParamDiskBuffer, vSonyRawMode);
			result = mnvm_noErr;
			break;
		case kCmndDiskSetRawMode:
			vSonyRawMode = get_vm_word(p + kParamDiskBuffer);
			result = mnvm_noErr;
			break;
#endif
#if IncludeSonyNew
		case kCmndDiskNew:
			{
				ui5b count = get_vm_long(p + ExtnDat_params + 0);
				tPbuf Pbuf_No = get_vm_word(p + ExtnDat_params + 4);
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
			put_vm_word(p + kParamDiskBuffer, vSonyNewDiskWanted);
			result = mnvm_noErr;
			break;
		case kCmndDiskEjectDelete:
			{
				tDrive Drive_No = get_vm_word(p + kParamDiskDrive_No);
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
				tDrive Drive_No = get_vm_word(p + ExtnDat_params + 0);
				/* reserved word at offset 2, should be zero */
				result = CheckReadableDrive(Drive_No);
				if (mnvm_noErr == result) {
					tPbuf Pbuf_No;
					result = vSonyGetName(Drive_No, &Pbuf_No);
					put_vm_word(p + ExtnDat_params + 4, Pbuf_No);
				}
			}
			break;
#endif
	}

	put_vm_word(p + ExtnDat_result, result);
}

GLOBALPROC Sony_Access(ui5b Data, CPTR addr)
{
	switch (addr) {
		case kDSK_Params_Hi:
			ParamAddrHi = Data;
			break;
		case kDSK_Params_Lo:
			{
				CPTR p = ParamAddrHi << 16 | Data;

				ParamAddrHi = (ui4b) - 1;
				if (kcom_callcheck == get_vm_word(p + ExtnDat_checkval)) {
					put_vm_word(p + ExtnDat_checkval, 0);

					switch (get_vm_word(p + ExtnDat_extension)) {
						case kExtnFindExtn:
							ExtnFind_Access(p);
							break;
#if EmVidCard
						case kExtnVideo:
							ExtnVideo_Access(p);
							break;
#endif
#if IncludePbufs
						case kExtnParamBuffers:
							ExtnParamBuffers_Access(p);
							break;
#endif
#if IncludeHostTextClipExchange
						case kExtnHostTextClipExchange:
							ExtnHostTextClipExchange_Access(p);
							break;
#endif
						case kExtnDisk:
							ExtnDisk_Access(p);
							break;
						default:
							put_vm_word(p + ExtnDat_result, mnvm_controlErr);
							break;
					}
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
