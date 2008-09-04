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
#include "ADDRSPAC.h"
#endif

#include "VIDEMDEV.h"

LOCALVAR const ui4b VidDrvr_contents[] = {
0x4C00, 0x0000, 0x0000, 0x0000,
0x002A, 0x0000, 0x0122, 0x01C0,
0x00EC, 0x152E, 0x4469, 0x7370,
0x6C61, 0x795F, 0x5669, 0x6465,
0x6F5F, 0x5361, 0x6D70, 0x6C65,
0x0000, 0x2448, 0x2649, 0x7014,
0xA440, 0x7014, 0xA722, 0x6600,
0x0078, 0x2748, 0x0014, 0xA029,
0x49FA, 0x0072, 0x7010, 0xA71E,
0x6600, 0x0066, 0x317C, 0x0006,
0x0004, 0x214C, 0x0008, 0x214B,
0x000C, 0x7000, 0x102B, 0x0028,
0xA075, 0x664C, 0x226B, 0x0014,
0x2251, 0x2348, 0x000A, 0x554F,
0x2F3C, 0x163D, 0xA5A9, 0x554F,
0x3F3C, 0x0001, 0x4267, 0x3F3C,
0x5B17, 0x2078, 0x0134, 0x2068,
0x0014, 0x208F, 0xDEFC, 0x000C,
0x301F, 0x3340, 0x000E, 0x3F3C,
0x0001, 0x554F, 0x3F3C, 0x0003,
0x3F00, 0x3F3C, 0x5B17, 0x208F,
0xDEFC, 0x000A, 0x7000, 0x6002,
0x70E9, 0x4E75, 0x2F08, 0x554F,
0x3F3C, 0x0004, 0x2069, 0x0014,
0x2050, 0x3F28, 0x000E, 0x3F3C,
0x5B17, 0x2078, 0x0134, 0x2068,
0x0014, 0x208F, 0x504F, 0x2029,
0x002A, 0xE198, 0x0240, 0x000F,
0x2078, 0x0D28, 0x4E90, 0x205F,
0x7001, 0x4E75, 0x2669, 0x0014,
0x2653, 0x4267, 0x554F, 0x3F3C,
0x0003, 0x3F2B, 0x000E, 0x3F3C,
0x5B17, 0x2078, 0x0134, 0x2068,
0x0014, 0x208F, 0xDEFC, 0x000A,
0x206B, 0x000A, 0x2649, 0xA076,
0x206B, 0x0014, 0xA023, 0x7000,
0x4E75, 0x48E7, 0x0888, 0x3028,
0x001A, 0x2468, 0x001C, 0x0C40,
0x0002, 0x6210, 0xE348, 0x303B,
0x0006, 0x4EFB, 0x0002, 0x0014,
0x000A, 0x0048, 0x70EF, 0x6002,
0x7000, 0x4CDF, 0x1110, 0x6000,
0x00E8, 0x2669, 0x0014, 0x2653,
0x554F, 0x554F, 0x3F3C, 0x0007,
0x3F2B, 0x000E, 0x3F3C, 0x5B17,
0x2078, 0x0134, 0x2068, 0x0014,
0x208F, 0x34AF, 0x0008, 0xDEFC,
0x000A, 0x426A, 0x0006, 0x256B,
0x0006, 0x0008, 0x60C2, 0x2669,
0x0014, 0x2653, 0x302A, 0x0006,
0x4A40, 0x66B0, 0x3F12, 0x554F,
0x3F3C, 0x0006, 0x3F2B, 0x000E,
0x3F3C, 0x5B17, 0x2078, 0x0134,
0x2068, 0x0014, 0x208F, 0x4A6F,
0x0008, 0xDEFC, 0x000A, 0x668C,
0x256B, 0x0006, 0x0008, 0x6088,
0x2F08, 0x3028, 0x001A, 0x2468,
0x001C, 0x0C40, 0x0005, 0x6216,
0xE348, 0x303B, 0x0006, 0x4EFB,
0x0002, 0x000C, 0x000C, 0x001A,
0x000C, 0x004E, 0x0056, 0x70EE,
0x6000, 0x0004, 0x7000, 0x205F,
0x6000, 0x0046, 0x2669, 0x0014,
0x2653, 0x554F, 0x554F, 0x3F3C,
0x0005, 0x3F2B, 0x000E, 0x3F3C,
0x5B17, 0x2078, 0x0134, 0x2068,
0x0014, 0x208F, 0x34AF, 0x0008,
0xDEFC, 0x000A, 0x426A, 0x0006,
0x256B, 0x0006, 0x0008, 0x60C4,
0x7001, 0x3540, 0x0006, 0x60BC,
0x2569, 0x002A, 0x0008, 0x60B4,
0x0828, 0x0009, 0x0006, 0x6702,
0x4E75, 0x2078, 0x08FC, 0x4ED0
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
	ui3p pTo_VidDrvrDir;
	ui3p pTo_sMacOS68020;
	ui3p pTo_OneBitMode;
	ui3p pTo_OneVidParams;
#if 0
	ui3p pTo_TwoBitMode;
	ui3p pTo_TwoVidParams;
	ui3p pTo_FourBitMode;
	ui3p pTo_FourVidParams;
	ui3p pTo_EightBitMode;
	ui3p pTo_EightVidParams;
	ui3p pTo_SixteenBitMode;
	ui3p pTo_SixteenVidParams;
	ui3p pTo_ThirtyTwoBitMode;
	ui3p pTo_ThirtyTwoVidParams;
#endif

	pPatch = VidROM;

	pAt_sRsrcDir = pPatch;
	pTo_sRsrc_Board = ReservePatchOSLstEntry();
	pTo_sRsrc_Video = ReservePatchOSLstEntry();
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_sRsrc_Board, 0x01 /* sRsrc_Board */);
	pTo_BoardType = ReservePatchOSLstEntry();
	pTo_BoardName = ReservePatchOSLstEntry();
	PatchADatLstEntry(0x20 /* sRsrcType */, 0x00004232);
		/* use Basilisk II id, since known to work */
	pTo_VenderInfo = ReservePatchOSLstEntry();
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_BoardType, 0x01 /* sRsrcType */);
	PatchAWord(0x0001);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchAWord(0x0000);

	PatchAReservedOSLstEntry(pTo_BoardName, 0x02 /* sRsrcType */);
	/*
		'Sample video card' as ascii c string
		(null terminated), and
		zero padded to end aligned long.
	*/
	PatchALong(0x53616D70);
	PatchALong(0x6C652076);
	PatchALong(0x6964656F);
	PatchALong(0x20636172);
	PatchALong(0x64000000);

	PatchAReservedOSLstEntry(pTo_VenderInfo, 0x24 /* vendorInfo */);

	pTo_VendorID = ReservePatchOSLstEntry();
	pTo_RevLevel = ReservePatchOSLstEntry();
	pTo_PartNum = ReservePatchOSLstEntry();
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_VendorID, 0x01 /* vendorId */);
	/*
		'XYZ Inc.' as ascii c string
		(null terminated), and
		zero padded to end aligned long.
	*/
	PatchALong(0x58595A20);
	PatchALong(0x496E632E);
	PatchALong(0x00000000);

	PatchAReservedOSLstEntry(pTo_RevLevel, 0x03 /* revLevel */);
	/*
		'Beta-7.0' as ascii c string
		(null terminated), and
		zero padded to end aligned long.
	*/
	PatchALong(0x42657461);
	PatchALong(0x2D372E30);
	PatchALong(0x00000000);

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
	pTo_OneBitMode = ReservePatchOSLstEntry();
#if 0
	pTo_TwoBitMode = ReservePatchOSLstEntry();
	pTo_FourBitMode = ReservePatchOSLstEntry();
	pTo_EightBitMode = ReservePatchOSLstEntry();
	pTo_SixteenBitMode = ReservePatchOSLstEntry();
	pTo_ThirtyTwoBitMode = ReservePatchOSLstEntry();
#endif
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_VideoType, 0x01 /* sRsrcType */);

	PatchAWord(0x0003); /* catDisplay */
	PatchAWord(0x0001); /* typVideo */
	PatchAWord(0x0001); /* drSwApple */
	PatchAWord(0x0001); /* drHwTFB */

	PatchAReservedOSLstEntry(pTo_VideoName, 0x02 /* sRsrcType */);
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
	PatchALong(0x00040000);

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
		/* 2 for direct devices, according to Basilisk II */
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

#if 0
	PatchAReservedOSLstEntry(pTo_TwoBitMode, 0x81 /* twoBitMode */);
	pTo_TwoVidParams = ReservePatchOSLstEntry();
	PatchADatLstEntry(0x03, 0x00000001);
	PatchADatLstEntry(0x04, 0x00000000);
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_TwoVidParams, 0x01);
	PatchALong(0x0000002E);
	PatchALong(0x00000000);
	PatchAWord(vMacScreenWidth / 8 * 2);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchAWord(vMacScreenHeight);
	PatchAWord(vMacScreenWidth);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchALong(0x00000000);
	PatchALong(0x00480000);
	PatchALong(0x00480000);
	PatchAWord(0x0000);
	PatchAWord(0x0002);
	PatchAWord(0x0001);
	PatchAWord(0x0002);
	PatchALong(0x00000000);

	PatchAReservedOSLstEntry(pTo_FourBitMode, 0x82 /* fourBitMode */);
	pTo_FourVidParams = ReservePatchOSLstEntry();
	PatchADatLstEntry(0x03, 0x00000001);
	PatchADatLstEntry(0x04, 0x00000000);
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_FourVidParams, 0x01);
	PatchALong(0x0000002E);
	PatchALong(0x00000000);
	PatchAWord(vMacScreenWidth / 8 * 4);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchAWord(vMacScreenHeight);
	PatchAWord(vMacScreenWidth);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchALong(0x00000000);
	PatchALong(0x00480000);
	PatchALong(0x00480000);
	PatchAWord(0x0000);
	PatchAWord(0x0004);
	PatchAWord(0x0001);
	PatchAWord(0x0004);
	PatchALong(0x00000000);

	PatchAReservedOSLstEntry(pTo_EightBitMode, 0x83);
	pTo_EightVidParams = ReservePatchOSLstEntry();
	PatchADatLstEntry(0x03, 0x00000001);
	PatchADatLstEntry(0x04, 0x00000000);
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_EightVidParams, 0x01);
	PatchALong(0x0000002E);
	PatchALong(0x00000000);
	PatchAWord(vMacScreenWidth);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchAWord(vMacScreenHeight);
	PatchAWord(vMacScreenWidth);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchALong(0x00000000);
	PatchALong(0x00480000);
	PatchALong(0x00480000);
	PatchAWord(0x0000);
	PatchAWord(0x0008);
	PatchAWord(0x0001);
	PatchAWord(0x0008);
	PatchALong(0x00000000);

	PatchAReservedOSLstEntry(pTo_SixteenBitMode, 0x84);
	pTo_SixteenVidParams = ReservePatchOSLstEntry();
	PatchADatLstEntry(0x03, 0x00000001);
	PatchADatLstEntry(0x04, 0x00000002);
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_SixteenVidParams, 0x01);
	PatchALong(0x0000002E);
	PatchALong(0x00000000);
	PatchAWord(vMacScreenWidth * 2);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchAWord(vMacScreenHeight);
	PatchAWord(vMacScreenWidth);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchALong(0x00000000);
	PatchALong(0x00480000);
	PatchALong(0x00480000);
	PatchAWord(0x0010);
	PatchAWord(0x0010);
	PatchAWord(0x0003);
	PatchAWord(0x0005);
	PatchALong(0x00000000);

	PatchAReservedOSLstEntry(pTo_ThirtyTwoBitMode, 0x85);
	pTo_ThirtyTwoVidParams = ReservePatchOSLstEntry();
	PatchADatLstEntry(0x03, 0x00000001);
	PatchADatLstEntry(0x04, 0x00000002);
	PatchAnEndOfLst();

	PatchAReservedOSLstEntry(pTo_ThirtyTwoVidParams, 0x01);
	PatchALong(0x0000002E);
	PatchALong(0x00000000);
	PatchAWord(vMacScreenWidth * 4);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchAWord(vMacScreenHeight);
	PatchAWord(vMacScreenWidth);
	PatchAWord(0x0000);
	PatchAWord(0x0000);
	PatchALong(0x00000000);
	PatchALong(0x00480000);
	PatchALong(0x00480000);
	PatchAWord(0x0010);
	PatchAWord(0x0020);
	PatchAWord(0x0003);
	PatchAWord(0x0008);
	PatchALong(0x00000000);
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

LOCALVAR ui4r VidMode = 0;

GLOBALFUNC ui4r Vid_GetMode(void)
{
	return VidMode + 128;
}

GLOBALFUNC tMacErr Vid_SetMode(ui4r v)
{
	VidMode = v - 128;
	return mnvm_noErr;
}

GLOBALFUNC ui4r Vid_Reset(void)
{
	VidMode = 0;
	return VidMode + 128;
}
