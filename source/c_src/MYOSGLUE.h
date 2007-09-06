/*
	MYOSGLUE.h

	Copyright (C) 2006 Philip Cummins, Richard F. Bannister, Paul C. Pratt

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
	MY Operating System GLUE.

	header file for operating system dependent code.
	the same header is used for all platforms.

	This code is descended from Richard F. Bannister's Macintosh
	port of vMac, by Philip Cummins.
*/

#ifdef MYOSGLUE_H
#ifndef AllFiles
#error "header already included"
#endif
#else
#define MYOSGLUE_H
#endif

#define kEmu128K        0
#define kEmu512K        1
#define kEmu512Ke       2
#define kEmuPlus1M      3
#define kEmuPlus2M      4
#define kEmuPlus2_5M    5
#define kEmuPlus        6
#define kEmuSE1M        7
#define kEmuSE2M        8
#define kEmuSE2_5M      9
#define kEmuSE          10
#define kEmuClassic1M   11
#define kEmuClassic2M   12
#define kEmuClassic2_5M 13
#define kEmuClassic     14

EXPORTPROC WarnMsgCorruptedROM(void);
EXPORTPROC WarnMsgUnsupportedROM(void);

#ifndef DetailedAbormalReport
#define DetailedAbormalReport 0
#endif

#if DetailedAbormalReport
EXPORTPROC WarnMsgAbnormal(char *s);
#else
EXPORTPROC WarnMsgAbnormal(void);
#endif

EXPORTPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount);

#ifndef kRAM_Size
#if CurEmu == kEmu128K
#define kRAM_Size 0x00020000
#elif (CurEmu == kEmu512K) || (CurEmu == kEmu512Ke)
#define kRAM_Size 0x00080000
#elif (CurEmu == kEmuPlus1M)
#define kRAM_Size 0x00100000
#elif (CurEmu == kEmuPlus2M)
#define kRAM_Size 0x00200000
#elif (CurEmu == kEmuPlus2_5M)
#define kRAM_Size 0x00280000
#elif (CurEmu == kEmuPlus)
#define kRAM_Size 0x00400000
#elif (CurEmu == kEmuSE1M)
#define kRAM_Size 0x00100000
#elif (CurEmu == kEmuSE2M)
#define kRAM_Size 0x00200000
#elif (CurEmu == kEmuSE2_5M)
#define kRAM_Size 0x00280000
#elif (CurEmu == kEmuSE)
#define kRAM_Size 0x00400000
#elif (CurEmu == kEmuClassic1M)
#define kRAM_Size 0x00100000
#elif (CurEmu == kEmuClassic2M)
#define kRAM_Size 0x00200000
#elif (CurEmu == kEmuClassic2_5M)
#define kRAM_Size 0x00280000
#elif (CurEmu == kEmuClassic)
#define kRAM_Size 0x00400000
#else
#error "kRAM_Size not defined"
#endif
#endif

#define RAMSafetyMarginFudge 4
EXPORTVAR(ui4b, *RAM)
	/*
		allocated by MYOSGLUE to be at least kRAM_Size + RAMSafetyMarginFudge
		bytes. Because of shortcuts taken in ADDRSPAC.c, it is in theory
		possible for the emulator to write up to 3 bytes past kRAM_Size.
	*/


#if CurEmu <= kEmu512K
#define kTrueROM_Size 0x010000 /* ROM size is 64 KB */
#elif CurEmu <= kEmuPlus
#define kTrueROM_Size 0x020000 /* ROM size is 128 KB */
#elif CurEmu <= kEmuSE
#define kTrueROM_Size 0x040000 /* ROM size is 256 KB */
#elif CurEmu <= kEmuClassic
#define kTrueROM_Size 0x080000 /* ROM size is 512 KB */
#else
#error "kTrueROM_Size not defined"
#endif

#if CurEmu <= kEmu512K
#define kROM_Size 0x020000 /* ROM size is 128 KB */
#else
#define kROM_Size kTrueROM_Size
#endif

#ifndef TempDebug /* a way to mark temporary debugging code */
#if (CurEmu >= kEmuSE1M) && (CurEmu <= kEmuClassic)
#define TempDebug 1 /* flag some stuff that needs look at */
#else
#define TempDebug 0
#endif
#endif

EXPORTVAR(ui4b, *ROM)

#ifndef IncludePbufs
#define IncludePbufs 1
#endif

#if IncludePbufs

#ifndef NumPbufs
#define NumPbufs 4
#endif

#define NotAPbuf ((ui4b)0xFFFF)

EXPORTVAR(ui5b, PbufAllocatedMask)
EXPORTVAR(ui5b, PbufSize[NumPbufs])

#define PbufIsAllocated(i) ((PbufAllocatedMask & ((ui5b)1 << (i))) != 0)

EXPORTFUNC si4b PbufNew(ui5b count, ui4b *r);
EXPORTPROC PbufDispose(ui4b i);
EXPORTPROC PbufTransfer(void *Buffer,
	ui4b i, ui5b offset, ui5b count, blnr IsWrite);

#endif

EXPORTVAR(ui5b, vSonyWritableMask)
EXPORTVAR(ui5b, vSonyInsertedMask)
EXPORTVAR(ui5b, vSonyMountedMask)

#define vSonyIsInserted(Drive_No) ((vSonyInsertedMask & ((ui5b)1 << (Drive_No))) != 0)
#define vSonyIsMounted(Drive_No) ((vSonyMountedMask & ((ui5b)1 << (Drive_No))) != 0)

EXPORTFUNC si4b vSonyRead(void *Buffer, ui4b Drive_No, ui5b Sony_Start, ui5b *Sony_Count);
EXPORTFUNC si4b vSonyWrite(void *Buffer, ui4b Drive_No, ui5b Sony_Start, ui5b *Sony_Count);
EXPORTFUNC si4b vSonyEject(ui4b Drive_No);
EXPORTFUNC si4b vSonyGetSize(ui4b Drive_No, ui5b *Sony_Count);

EXPORTFUNC blnr AnyDiskInserted(void);

#ifndef IncludeSonyRawMode
#define IncludeSonyRawMode 1
#endif

#if IncludeSonyRawMode
EXPORTVAR(blnr, vSonyRawMode)
#endif

#ifndef IncludeSonyGetName
#define IncludeSonyGetName 1
#endif

#ifndef IncludeSonyNew
#define IncludeSonyNew 1
#endif

#if IncludeSonyNew
EXPORTVAR(blnr, vSonyNewDiskWanted)
EXPORTVAR(ui5b, vSonyNewDiskSize)
EXPORTFUNC si4b vSonyEjectDelete(ui4b Drive_No);
#endif

#ifndef IncludeSonyNameNew
#define IncludeSonyNameNew 1
#endif

#if IncludeSonyNameNew
EXPORTVAR(ui4b, vSonyNewDiskName)
#endif

#if IncludeSonyGetName
EXPORTFUNC si4b vSonyGetName(ui4b Drive_No, ui4b *r);
#endif

EXPORTVAR(ui5b, CurMacDateInSeconds)
EXPORTVAR(ui5b, CurMacLatitude)
EXPORTVAR(ui5b, CurMacLongitude)
EXPORTVAR(ui5b, CurMacDelta) /* (dlsDelta << 24) | (gmtDelta & 0x00FFFFFF) */

#define vMacScreenHeight 342
#define vMacScreenWidth 512
#define vMacScreenNumBits ((long)vMacScreenHeight * (long)vMacScreenWidth)
#define vMacScreenNumBytes (vMacScreenNumBits / 8)
#define vMacScreenByteWidth (vMacScreenWidth / 8)

EXPORTVAR(char, *screencomparebuff)

EXPORTVAR(blnr, ForceMacOff)

EXPORTVAR(blnr, WantMacInterrupt)

EXPORTVAR(blnr, WantMacReset)

EXPORTFUNC blnr ExtraTimeNotOver(void);

EXPORTVAR(blnr, SpeedLimit)

EXPORTVAR(ui3b, SpeedValue)

EXPORTVAR(ui3b, CurMouseButton)

EXPORTVAR(ui4b, CurMouseV)
EXPORTVAR(ui4b, CurMouseH)

#ifndef EnableMouseMotion
#define EnableMouseMotion 1
#endif

#if EnableMouseMotion
EXPORTVAR(blnr, HaveMouseMotion)
EXPORTVAR(ui4b, MouseMotionV)
EXPORTVAR(ui4b, MouseMotionH)
#endif

#ifndef MySoundEnabled
#define MySoundEnabled 0
#endif

#if MySoundEnabled
EXPORTFUNC ui3p GetCurSoundOutBuff(void);

/* Length of the audio buffer */
#define SOUND_LEN 370
#endif

#ifndef IncludeHostTextClipExchange
#define IncludeHostTextClipExchange 1
#endif

#if IncludeHostTextClipExchange
EXPORTFUNC si4b HTCEexport(ui4b i);
EXPORTFUNC si4b HTCEimport(ui4b *r);
#endif

EXPORTVAR(ui5b, theKeys[4])
	/*
		What the emulated keyboard thinks is the
		state of the keyboard.
	*/

#define MKC_A 0x00
#define MKC_B 0x0B
#define MKC_C 0x08
#define MKC_D 0x02
#define MKC_E 0x0E
#define MKC_F 0x03
#define MKC_G 0x05
#define MKC_H 0x04
#define MKC_I 0x22
#define MKC_J 0x26
#define MKC_K 0x28
#define MKC_L 0x25
#define MKC_M 0x2E
#define MKC_N 0x2D
#define MKC_O 0x1F
#define MKC_P 0x23
#define MKC_Q 0x0C
#define MKC_R 0x0F
#define MKC_S 0x01
#define MKC_T 0x11
#define MKC_U 0x20
#define MKC_V 0x09
#define MKC_W 0x0D
#define MKC_X 0x07
#define MKC_Y 0x10
#define MKC_Z 0x06

#define MKC_1 0x12
#define MKC_2 0x13
#define MKC_3 0x14
#define MKC_4 0x15
#define MKC_5 0x17
#define MKC_6 0x16
#define MKC_7 0x1A
#define MKC_8 0x1C
#define MKC_9 0x19
#define MKC_0 0x1D

#define MKC_Command 0x37
#define MKC_Shift 0x38
#define MKC_CapsLock 0x39
#define MKC_Option 0x3A

#define MKC_Space 0x31
#define MKC_Return 0x24
#define MKC_BackSpace 0x33
#define MKC_Tab 0x30

#define MKC_Left /* 0x46 */ 0x7B
#define MKC_Right /* 0x42 */ 0x7C
#define MKC_Down /* 0x48 */ 0x7D
#define MKC_Up /* 0x4D */ 0x7E

#define MKC_Minus 0x1B
#define MKC_Equal 0x18
#define MKC_BackSlash 0x2A
#define MKC_Comma 0x2B
#define MKC_Period 0x2F
#define MKC_Slash 0x2C
#define MKC_SemiColon 0x29
#define MKC_SingleQuote 0x27
#define MKC_LeftBracket 0x21
#define MKC_RightBracket 0x1E
#define MKC_Grave 0x32
#define MKC_Clear 0x47
#define MKC_KPEqual 0x51
#define MKC_KPDevide 0x4B
#define MKC_KPMultiply 0x43
#define MKC_KPSubtract 0x4E
#define MKC_KPAdd 0x45
#define MKC_Enter 0x4C

#define MKC_KP1 0x53
#define MKC_KP2 0x54
#define MKC_KP3 0x55
#define MKC_KP4 0x56
#define MKC_KP5 0x57
#define MKC_KP6 0x58
#define MKC_KP7 0x59
#define MKC_KP8 0x5B
#define MKC_KP9 0x5C
#define MKC_KP0 0x52
#define MKC_Decimal 0x41

/* these aren't on the Mac Plus keyboard */

#define MKC_Control 0x3B
#define MKC_Escape 0x35
#define MKC_F1 0x7a
#define MKC_F2 0x78
#define MKC_F3 0x63
#define MKC_F4 0x76
#define MKC_F5 0x60
#define MKC_F6 0x61
#define MKC_F7 0x62
#define MKC_F8 0x64
#define MKC_F9 0x65
#define MKC_F10 0x6d
#define MKC_F11 0x67
#define MKC_F12 0x6f

#define MKC_Home 0x73
#define MKC_End 0x77
#define MKC_PageUp 0x74
#define MKC_PageDown 0x79
#define MKC_Help 0x72 /* = Insert */
#define MKC_ForwardDel 0x75
#define MKC_Print 0x69
#define MKC_ScrollLock 0x6B
#define MKC_Pause 0x71
