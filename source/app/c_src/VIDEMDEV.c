/*
	VIDEMDEV.c

	Copyright (C) 2008 Paul C. Pratt

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
	VIDeo card EMulated DEVice

	Emulation of video card for Macintosh II.

	Written referring to:
		Sample firmware code in "Designing Cards and Drivers
		for Macintosh II and Macintosh SE", Apple Computer,
		page 8-20.

		Basilisk II source code, especially slot_rom.cpp
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#include "MYOSGLUE.h"
#include "ENDIANAC.h"
#include "EMCONFIG.h"
#include "GLOBGLUE.h"
#include "SONYEMDV.h"
#endif

#include "VIDEMDEV.h"

LOCALVAR const ui4b VidDrvr_contents[] = {
0x4C00, 0x0000, 0x0000, 0x0000,
0x002A, 0x0000, 0x011A, 0x0124,
0x00EA, 0x152E, 0x4469, 0x7370,
0x6C61, 0x795F, 0x5669, 0x6465,
0x6F5F, 0x5361, 0x6D70, 0x6C65,
0x0000, 0x2448, 0x2649, 0x700A,
0xA440, 0x700A, 0xA722, 0x6600,
0x007A, 0x2748, 0x0014, 0xA029,
0x49FA, 0x0074, 0x7010, 0xA71E,
0x6600, 0x0068, 0x317C, 0x0006,
0x0004, 0x214C, 0x0008, 0x214B,
0x000C, 0x7000, 0x102B, 0x0028,
0xA075, 0x664E, 0x226B, 0x0014,
0x2251, 0x2288, 0x554F, 0x2F3C,
0x163D, 0xA5A9, 0x554F, 0x3F3C,
0x0001, 0x4267, 0x3F3C, 0x5B17,
0x2078, 0x0134, 0x2068, 0x0014,
0x2348, 0x0004, 0x208F, 0xDEFC,
0x000C, 0x301F, 0x3340, 0x0008,
0x3F3C, 0x0001, 0x554F, 0x3F3C,
0x0003, 0x3F00, 0x3F3C, 0x5B17,
0x208F, 0xDEFC, 0x000A, 0x7000,
0x6002, 0x70E9, 0x4E75, 0x2F08,
0x554F, 0x3F3C, 0x0004, 0x2069,
0x0014, 0x2050, 0x3F28, 0x0008,
0x3F3C, 0x5B17, 0x2068, 0x0004,
0x208F, 0x504F, 0x2029, 0x002A,
0xE198, 0x0240, 0x000F, 0x2078,
0x0D28, 0x4E90, 0x205F, 0x7001,
0x4E75, 0x2669, 0x0014, 0x2653,
0x4267, 0x554F, 0x3F3C, 0x0003,
0x3F2B, 0x0008, 0x3F3C, 0x5B17,
0x206B, 0x0004, 0x208F, 0xDEFC,
0x000A, 0x2053, 0x2649, 0xA076,
0x206B, 0x0014, 0xA023, 0x7000,
0x4E75, 0x2F08, 0x554F, 0x3F3C,
0x0006, 0x6008, 0x2F08, 0x554F,
0x3F3C, 0x0005, 0x2069, 0x0014,
0x2050, 0x3F28, 0x0008, 0x3F3C,
0x5B17, 0x2068, 0x0004, 0x208F,
0x5C4F, 0x301F, 0x205F, 0x0828,
0x0009, 0x0006, 0x6702, 0x4E75,
0x2078, 0x08FC, 0x4ED0
};

LOCALPROC ChecksumSlotROM(void)
{
	/* Calculate CRC */
	/* assuming check sum field initialized to zero */
	int i;
	ui3p p = VidROM;
	ui5b crc = 0;

	for (i = kVidROM_Size; --i >= 0; ) {
		crc = ((crc << 1) | (crc >> 31)) + *p++;
	}
	do_put_mem_long(p - 12, crc);
}

LOCALVAR ui3p pPatch;

LOCALPROC PatchAByte(ui3b v)
{
	*pPatch++ = v;
}

LOCALPROC PatchAWord(ui4r v)
{
	PatchAByte(v >> 8);
	PatchAByte(v & 0x00FF);
}

LOCALPROC PatchALong(ui5r v)
{
	PatchAWord(v >> 16);
	PatchAWord(v & 0x0000FFFF);
}

#if 0
LOCALPROC PatchAOSLstEntry0(ui3r Id, ui5r Offset)
{
	PatchALong((Id << 24) | (Offset & 0x00FFFFFF));
}
#endif

LOCALPROC PatchAOSLstEntry(ui3r Id, ui3p Offset)
{
	PatchALong((Id << 24) | ((Offset - pPatch) & 0x00FFFFFF));
}

LOCALFUNC ui3p ReservePatchOSLstEntry(void)
{
	ui3p v = pPatch;
	pPatch += 4;
	return v;
}

LOCALPROC PatchAReservedOSLstEntry(ui3p p, ui3r Id)
{
	ui3p pPatchSave = pPatch;
	pPatch = p;
	PatchAOSLstEntry(Id, pPatchSave);
	pPatch = pPatchSave;
}

LOCALPROC PatchADatLstEntry(ui3r Id, ui5r Data)
{
	PatchALong((Id << 24) | (Data & 0x00FFFFFF));
}

LOCALPROC PatchAnEndOfLst(void)
{
	PatchADatLstEntry(0xFF /* endOfList */, 0x00000000);
}

GLOBALFUNC blnr Vid_Init(void)
{
	int i;
	ui4b *pfrom;
	ui5r UsedSoFar;

	ui3p pAt_sRsrcDir;
	ui3p pTo_sRsrc_Board;
	ui3p pTo_BoardType;
	ui3p pTo_BoardName;
	ui3p pTo_VenderInfo;
	ui3p pTo_VendorID;
	ui3p pTo_RevLevel;
	ui3p pTo_PartNum;
	ui3p pTo_sRsrc_Video;
	ui3p pTo_VideoType;
	ui3p pTo_VideoName;
	ui3p pTo_MinorBase;
	ui3p pTo_MinorLength;
#if 0
	ui3p pTo_MajorBase;
	ui3p pTo_MajorLength;
#endif
	ui3p pTo_VidDrvrDir;
	ui3p pTo_sMacOS68020;
	ui3p pTo_OneBitMode;
	ui3p pTo_OneVidParams;
#if 0 != vMacScreenDepth
	ui3p pTo_ColorBitMode;
	ui3p pTo_ColorVidParams;
#endif

	pPatch = VidROM;

	pAt_sRsrcDir = pPatch;
	pTo_sRsrc_Board = ReservePatchOSLstEntry();
	pTo_sRsrc_Video = ReservePatchOSLstEntry();
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_sRsrc_Board, 0x01 /* sRsrc_Board */);
	pTo_BoardType = ReservePatchOSLstEntry();
	pTo_BoardName = ReservePatchOSLstEntry();
	PatchADatLstEntry(0x20 /* BoardId */, 0x0000764D);
		/* 'vM', for Mini vMac */
	pTo_VenderInfo = ReservePatchOSLstEntry();
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_BoardType, 0x01 /* sRsrcType */);
	PatchAWord(0x0001);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchAWord(0x0000);

	PatchAReservedOSLstEntry(pTo_BoardName, 0x02 /* sRsrcName */);
	/*
		'Mini vMac video card' as ascii c string
		(null terminated), and
		zero padded to end aligned long.
	*/
	PatchALong(0x4D696E69);
	PatchALong(0x20764D61);
	PatchALong(0x63207669);
	PatchALong(0x64656F20);
	PatchALong(0x63617264);
	PatchALong(0x00000000);

	PatchAReservedOSLstEntry(pTo_VenderInfo, 0x24 /* vendorInfo */);

	pTo_VendorID = ReservePatchOSLstEntry();
	pTo_RevLevel = ReservePatchOSLstEntry();
	pTo_PartNum = ReservePatchOSLstEntry();
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_VendorID, 0x01 /* vendorId */);
	/*
		'Paul C. Pratt' as ascii c string
		(null terminated), and
		zero padded to end aligned long.
	*/
	PatchALong(0x5061756C);
	PatchALong(0x20432E20);
	PatchALong(0x50726174);
	PatchALong(0x74000000);

	PatchAReservedOSLstEntry(pTo_RevLevel, 0x03 /* revLevel */);
	/*
		'1.0' as ascii c string
		(null terminated), and
		zero padded to end aligned long.
	*/
	PatchALong(0x312E3000);

	PatchAReservedOSLstEntry(pTo_PartNum, 0x04 /* partNum */);
	/*
		'TFB-1' as ascii c string
		(null terminated), and
		zero padded to end aligned long.
	*/
	PatchALong(0x5446422D);
	PatchALong(0x31000000);

	PatchAReservedOSLstEntry(pTo_sRsrc_Video, 0x80 /* sRsrc_Video */);

	pTo_VideoType = ReservePatchOSLstEntry();
	pTo_VideoName = ReservePatchOSLstEntry();
	pTo_VidDrvrDir = ReservePatchOSLstEntry();
	PatchADatLstEntry(0x08 /* sRsrcHWDevId */, 0x00000001);
	pTo_MinorBase = ReservePatchOSLstEntry();
	pTo_MinorLength = ReservePatchOSLstEntry();
#if 0
	pTo_MajorBase = ReservePatchOSLstEntry();
	pTo_MajorLength = ReservePatchOSLstEntry();
#endif
	pTo_OneBitMode = ReservePatchOSLstEntry();
#if 0 != vMacScreenDepth
	pTo_ColorBitMode = ReservePatchOSLstEntry();
#endif
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_VideoType, 0x01 /* sRsrcType */);

	PatchAWord(0x0003); /* catDisplay */
	PatchAWord(0x0001); /* typVideo */
	PatchAWord(0x0001); /* drSwApple */
	PatchAWord(0x0001); /* drHwTFB */

	PatchAReservedOSLstEntry(pTo_VideoName, 0x02 /* sRsrcName */);
	/*
		'Display_Video_Apple_TFB' as ascii c string
		(null terminated), and
		zero padded to end aligned long.
	*/
	PatchALong(0x44697370);
	PatchALong(0x6C61795F);
	PatchALong(0x56696465);
	PatchALong(0x6F5F4170);
	PatchALong(0x706C655F);
	PatchALong(0x54464200);

	PatchAReservedOSLstEntry(pTo_MinorBase, 0x0A /* MinorBaseOS */);
	PatchALong(0x00000000);

	PatchAReservedOSLstEntry(pTo_MinorLength, 0x0B /* MinorLength */);
	PatchALong(kVidMemRAM_Size);

#if 0
	PatchAReservedOSLstEntry(pTo_MajorBase, 0x0C /* MinorBaseOS */);
	PatchALong(0x00000000);

	PatchAReservedOSLstEntry(pTo_MajorLength, 0x0D /* MinorLength */);
	PatchALong(kVidMemRAM_Size);
#endif

	PatchAReservedOSLstEntry(pTo_VidDrvrDir, 0x04 /* sRsrcDrvrDir */);
	pTo_sMacOS68020 = ReservePatchOSLstEntry();
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_sMacOS68020, 0x02 /* sMacOS68020 */);

	PatchALong(4 + sizeof(VidDrvr_contents));
	pfrom = (ui4b *)VidDrvr_contents;
	for (i = sizeof(VidDrvr_contents) / 2; --i >= 0; ) {
		PatchAWord(*pfrom++);
	}

	PatchAReservedOSLstEntry(pTo_OneBitMode, 0x80 /* oneBitMode */);
	pTo_OneVidParams = ReservePatchOSLstEntry();
	PatchADatLstEntry(0x03 /* mVidParams */, 0x00000001);
	PatchADatLstEntry(0x04 /* mDevType */, 0x00000000);
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_OneVidParams, 0x01 /* mVidParams */);
	PatchALong(0x0000002E); /* physical Block Size */
	PatchALong(0x00000000); /* defmBaseOffset */
	PatchAWord(vMacScreenWidth / 8); /* (Bounds.R-Bounds.L)*PixelSize/8 */
	PatchAWord(0x0000); /* Bounds.T */
	PatchAWord(0x0000); /* Bounds.L */
	PatchAWord(vMacScreenHeight); /* Bounds.B */
	PatchAWord(vMacScreenWidth); /* Bounds.R */
	PatchAWord(0x0000); /* bmVersion */
	PatchAWord(0x0000); /* packType not used */
	PatchALong(0x00000000); /* packSize not used */
	PatchALong(0x00480000); /* bmHRes */
	PatchALong(0x00480000); /* bmVRes */
	PatchAWord(0x0000); /* bmPixelType */
	PatchAWord(0x0001); /* bmPixelSize */
	PatchAWord(0x0001); /* bmCmpCount */
	PatchAWord(0x0001); /* bmCmpSize */
	PatchALong(0x00000000); /* bmPlaneBytes */

#if 0 != vMacScreenDepth

	PatchAReservedOSLstEntry(pTo_ColorBitMode, 0x81);
	pTo_ColorVidParams = ReservePatchOSLstEntry();
	PatchADatLstEntry(0x03 /* mVidParams */, 0x00000001);
	PatchADatLstEntry(0x04 /* mDevType */,
		(vMacScreenDepth < 4) ? 0x00000000 : 0x00000002);
		/* 2 for direct devices, according to Basilisk II */
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_ColorVidParams, 0x01);
	PatchALong(0x0000002E); /* physical Block Size */
	PatchALong(0x00000000); /* defmBaseOffset */
	PatchAWord(vMacScreenByteWidth);
	PatchAWord(0x0000); /* Bounds.T */
	PatchAWord(0x0000); /* Bounds.L */
	PatchAWord(vMacScreenHeight); /* Bounds.B */
	PatchAWord(vMacScreenWidth); /* Bounds.R */
	PatchAWord(0x0000); /* bmVersion */
	PatchAWord(0x0000); /* packType not used */
	PatchALong(0x00000000); /* packSize not used */
	PatchALong(0x00480000); /* bmHRes */
	PatchALong(0x00480000); /* bmVRes */
	PatchAWord((vMacScreenDepth < 4) ? 0x0000 : 0x0010); /* bmPixelType */
	PatchAWord(1 << vMacScreenDepth); /* bmPixelSize */
	PatchAWord((vMacScreenDepth < 4) ? 0x0001 : 0x0003); /* bmCmpCount */
#if 4 == vMacScreenDepth
	PatchAWord(0x0005); /* bmCmpSize */
#elif 5 == vMacScreenDepth
	PatchAWord(0x0008); /* bmCmpSize */
#else
	PatchAWord(1 << vMacScreenDepth); /* bmCmpSize */
#endif
	PatchALong(0x00000000); /* bmPlaneBytes */

#endif

	UsedSoFar = (pPatch - VidROM) + 20;
	if (UsedSoFar > kVidROM_Size) {
		ReportAbnormal("kVidROM_Size to small");
		return falseblnr;
	}

	for (i = kVidROM_Size - UsedSoFar; --i >= 0; ) {
		PatchAByte(0);
	}

	pPatch = (kVidROM_Size - 20) + VidROM;
	PatchALong((pAt_sRsrcDir - pPatch) & 0x00FFFFFF);
	PatchALong(/* 0x0000041E */ kVidROM_Size);
	PatchALong(0x00000000);
	PatchAByte(0x01);
	PatchAByte(0x01);
	PatchALong(0x5A932BC7);
	PatchAByte(0x00);
	PatchAByte(0x0F);

	ChecksumSlotROM();

#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)
	CLUT_reds[0] = 0xFFFF;
	CLUT_greens[0] = 0xFFFF;
	CLUT_blues[0] = 0xFFFF;
	CLUT_reds[CLUT_size - 1] = 0;
	CLUT_greens[CLUT_size - 1] = 0;
	CLUT_blues[CLUT_size - 1] = 0;
#endif

	return trueblnr;
}

IMPORTPROC Vid_VBLinterrupt_PulseNotify(void);

GLOBALPROC Vid_Update(void)
{
	if (! Vid_VBLintunenbl) {
		Vid_VBLinterrupt = 0;
		Vid_VBLinterrupt_PulseNotify();
	}
}

LOCALFUNC ui4r Vid_GetMode(void)
{
	return
#if 0 != vMacScreenDepth
		UseColorMode ? 129 :
#endif
		128;
}

LOCALFUNC tMacErr Vid_SetMode(ui4r v)
{
#if 0 == vMacScreenDepth
	UnusedParam(v);
#else
	if (UseColorMode != (v != 128)) {
		UseColorMode = ! UseColorMode;
		ColorMappingChanged = trueblnr;
	}
#endif
	return mnvm_noErr;
}

GLOBALFUNC ui4r Vid_Reset(void)
{
#if 0 != vMacScreenDepth
	UseColorMode = falseblnr;
#endif
	return 128;
}

#define kCmndVideoFeatures 1
#define kCmndVideoGetIntEnbl 2
#define kCmndVideoSetIntEnbl 3
#define kCmndVideoClearInt 4
#define kCmndVideoStatus 5
#define kCmndVideoControl 6

#define CntrlParam_csCode 0x1A /* control/status code [word] */
#define CntrlParam_csParam 0x1C /* operation-defined parameters */

#define VDPageInfo_csMode 0
#define VDPageInfo_csPage 6
#define VDPageInfo_csBaseAddr 8

#define VDSetEntryRecord_csTable 0
#define VDSetEntryRecord_csStart 4
#define VDSetEntryRecord_csCount 6

#define VDGammaRecord_csGTable 0

#define VidBaseAddr 0xF9900000
	/* appears to be completely ignored */

LOCALVAR blnr UseGrayTones = falseblnr;

LOCALPROC FillScreenWithGrayPattern(void)
{
	int i;
	int j;
	ui5b *p1 = (ui5b *)VidMem;

#if 0 != vMacScreenDepth
	if (UseColorMode) {
#if 1 == vMacScreenDepth
		ui5b pat = 0xCCCCCCCC;
#elif 2 == vMacScreenDepth
		ui5b pat = 0xF0F0F0F0;
#elif 3 == vMacScreenDepth
		ui5b pat = 0xFF00FF00;
#elif 4 == vMacScreenDepth
		ui5b pat = 0x00007FFF;
#elif 5 == vMacScreenDepth
		ui5b pat = 0x00000000;
#endif
		for (i = vMacScreenHeight; --i >= 0; ) {
			for (j = vMacScreenByteWidth >> 2; --j >= 0; ) {
				*p1++ = pat;
#if 5 == vMacScreenDepth
				pat = (~ pat) & 0x00FFFFFF;
#endif
			}
			pat = (~ pat)
#if 4 == vMacScreenDepth
				& 0x7FFF7FFF
#elif 5 == vMacScreenDepth
				& 0x00FFFFFF
#endif
				;
		}
	} else
#endif
	{
		ui5b pat = 0xAAAAAAAA;

		for (i = vMacScreenHeight; --i >= 0; ) {
			for (j = vMacScreenMonoByteWidth >> 2; --j >= 0; ) {
				*p1++ = pat;
			}
			pat = ~ pat;
		}
	}
}

GLOBALPROC ExtnVideo_Access(CPTR p)
{
	tMacErr result = mnvm_controlErr;

	switch (get_vm_word(p + ExtnDat_commnd)) {
		case kCmndVersion:
			put_vm_word(p + ExtnDat_version, 1);
			result = mnvm_noErr;
			break;
		case kCmndVideoGetIntEnbl:
			put_vm_word(p + 8,
				Vid_VBLintunenbl ? 0 : 1);
			result = mnvm_noErr;
			break;
		case kCmndVideoSetIntEnbl:
			Vid_VBLintunenbl =
				(0 == get_vm_word(p + 8))
					? 1 : 0;
			result = mnvm_noErr;
			break;
		case kCmndVideoClearInt:
			Vid_VBLinterrupt = 1;
			result = mnvm_noErr;
			break;
		case kCmndVideoControl:
			{
				CPTR CntrlParams = get_vm_long(p + 8);
				CPTR csParam = get_vm_long(CntrlParams + CntrlParam_csParam);
				ui4r csCode = get_vm_word(CntrlParams + CntrlParam_csCode);

				switch (csCode) {
					case 0: /* VidReset */
						put_vm_word(csParam + VDPageInfo_csMode, Vid_GetMode());
						put_vm_word(csParam + VDPageInfo_csPage, 0); /* page is always 0 */
						put_vm_long(csParam + VDPageInfo_csBaseAddr, VidBaseAddr);

						result = mnvm_noErr;
						break;
					case 1: /* KillIO */
						result = mnvm_noErr;
						break;
					case 2: /* SetVidMode */
						if (0 != get_vm_word(csParam + VDPageInfo_csPage)) {
							/* return mnvm_controlErr, page must be 0 */
						} else {
							result = Vid_SetMode(get_vm_word(csParam + VDPageInfo_csMode));
							put_vm_long(csParam + VDPageInfo_csBaseAddr, VidBaseAddr);
						}
						break;
					case 3: /* SetEntries */
#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)
						if (UseColorMode) {
							CPTR csTable = get_vm_long(csParam + VDSetEntryRecord_csTable);
							ui4r csStart = get_vm_word(csParam + VDSetEntryRecord_csStart);
							ui4r csCount = 1 + get_vm_word(csParam + VDSetEntryRecord_csCount);
							if (((ui4r) 0xFFFF) ==  csStart) {
								ReportAbnormal("Indexed SetEntries not implemented");
							} else if (csStart + csCount > CLUT_size) {
								result = mnvm_paramErr;
							} else {
								int i;

								for (i = 0; i < csCount; ++i) {
									int j = i + csStart;
									if (j == 0) {
										/* ignore input, leave white */
									} else if (j == CLUT_size - 1) {
										/* ignore input, leave black */
									} else {
										ui4r r = get_vm_word(csTable + 2);
										ui4r g = get_vm_word(csTable + 4);
										ui4r b = get_vm_word(csTable + 6);
										CLUT_reds[j] = r;
										CLUT_greens[j] = g;
										CLUT_blues[j] = b;
									}
									csTable += 8;
								}
								ColorMappingChanged = trueblnr;
							}
						} else
#endif
						{
							/* not implemented */
						}
						break;
					case 4: /* SetGamma */
						{
#if 0
							CPTR csTable = get_vm_long(csParam + VDGammaRecord_csGTable);
							/* not implemented */
#endif
						}
						break;
					case 5: /* GrayScreen */
						{
#if 0
							ui4r csPage = get_vm_word(csParam + VDPageInfo_csPage);
							/* not implemented */
#endif
							FillScreenWithGrayPattern();
							result = mnvm_noErr;
						}
						break;
					case 6: /* SetGray */
						{
							ui3r csMode = get_vm_byte(csParam + VDPageInfo_csMode);
								/*
									"Designing Cards and Drivers" book says
									this is a word, but it seems to be a byte.
								*/

							UseGrayTones = (csMode != 0);
							result = mnvm_noErr;
						}
						break;
				}
			}
			break;
		case kCmndVideoStatus:
			{
				CPTR CntrlParams = get_vm_long(p + 8);
				CPTR csParam = get_vm_long(CntrlParams + CntrlParam_csParam);
				ui4r csCode = get_vm_word(CntrlParams + CntrlParam_csCode);

				result = mnvm_statusErr;
				switch (csCode) {
					case 2: /* GetMode */
						put_vm_word(csParam + VDPageInfo_csMode, Vid_GetMode());
						put_vm_word(csParam + VDPageInfo_csPage, 0); /* page is always 0 */
						put_vm_long(csParam + VDPageInfo_csBaseAddr, VidBaseAddr);
						result = mnvm_noErr;
						break;
					case 3: /* GetEntries */
						{
#if 0
							CPTR csTable = get_vm_long(csParam + VDSetEntryRecord_csTable);
							put_vm_word(csParam + VDSetEntryRecord_csStart, csStart);
							put_vm_word(csParam + VDSetEntryRecord_csCount, csCount);
#endif
							ReportAbnormal("GetEntries not implemented");
						}
						break;
					case 4: /* GetPages */
						put_vm_word(csParam + VDPageInfo_csPage, 1); /* always 1 page */
						result = mnvm_noErr;
						break;
					case 5: /* GetPageAddr */
						{
							ui4r csPage = get_vm_word(csParam + VDPageInfo_csPage);
							if (0 != csPage) {
								/* return mnvm_statusErr, page must be 0 */
							} else {
								put_vm_long(csParam + VDPageInfo_csBaseAddr, VidBaseAddr);
								result = mnvm_noErr;
							}
						}
						break;
					case 6: /* GetGray */
						{
							put_vm_word(csParam + VDPageInfo_csMode, UseGrayTones ? 0x0100 : 0);
								/*
									"Designing Cards and Drivers" book says
									this is a word, but it seems to be a byte.
								*/
							result = mnvm_noErr;
						}
						break;
				}
			}
			break;
	}

	put_vm_word(p + ExtnDat_result, result);
}