/*
	MYOSGLUE.h

	Copyright (C) 2004 Philip Cummins, Richard F. Bannister, Paul Pratt

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

#define kEmu128K     0
#define kEmu512K     1
#define kEmu512Ke    2
#define kEmuPlus1M   3
#define kEmuPlus2M   4
#define kEmuPlus2_5M 5
#define kEmuPlus     6

#ifndef CurEmu
#define CurEmu kEmuPlus
#endif

EXPORTPROC MacMsg(char *briefMsg, char *longMsg, blnr fatal);

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
#else
#define kRAM_Size 0x00400000
#endif
#endif

#define RAMSafetyMarginFudge 4
EXPORTVAR(ui4b, *RAM)
	/*
		allocated by MYOSGLUE to be at least kRAM_Size + RAMSafetyMarginFudge
		bytes. Because of shortcuts taken in ADDRSPAC.c, it is in theory
		possible for the emulator to write up to 3 bytes past kRAM_Size.
	*/

#define kROM_Size 0x020000 /* ROM size is 128 KB */

#if CurEmu <= kEmu512K
#define kTrueROM_Size 0x010000 /* ROM size is 64 KB */
#else
#define kTrueROM_Size kROM_Size /* ROM size is 128 KB */
#endif

EXPORTVAR(ui4b, *ROM)

#ifndef NumDrives
#if CurEmu <= kEmu512K
#define NumDrives 2
#else
#define NumDrives 6
#endif
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

EXPORTPROC HaveChangedScreenBuff(si4b top, si4b left, si4b bottom, si4b right);

#ifndef UseControlKeys
#define UseControlKeys 1
#endif

#if UseControlKeys
EXPORTVAR(char, *CntrlDisplayBuff)
#endif

EXPORTVAR(blnr, RequestMacOff)

EXPORTVAR(si3b, TimeAdjust)

EXPORTPROC CheckIntSixtieth(blnr MayWaitForIt);

EXPORTVAR(blnr, SpeedLimit)

#ifndef EnableMagnify
#define EnableMagnify 1
#endif

#ifndef EnableFullScreen
#define EnableFullScreen (1 && UseControlKeys)
#endif

#ifndef EnableMouseMotion
#define EnableMouseMotion (1 && EnableFullScreen)
#endif

#if UseControlKeys
EXPORTVAR(blnr, ControlKeyPressed)
#endif

#if EnableFullScreen
EXPORTVAR(blnr, WantFullScreen)
#endif

#if EnableFullScreen
EXPORTPROC ToggleWantFullScreen(void);
#endif

#if EnableMagnify
EXPORTVAR(blnr, WantMagnify)
#endif

EXPORTVAR(ui3b, CurMouseButton)

EXPORTVAR(ui4b, CurMouseV)
EXPORTVAR(ui4b, CurMouseH)

#if EnableMouseMotion
EXPORTVAR(blnr, HaveMouseMotion)
EXPORTVAR(ui4b, MouseMotionV)
EXPORTVAR(ui4b, MouseMotionH)
#endif

#if EnableMagnify
EXPORTPROC ToggleWantMagnify(void);
#endif

#ifndef MySoundEnabled
#define MySoundEnabled 0
#endif

#if MySoundEnabled
EXPORTFUNC ui3p GetCurSoundOutBuff(void);

/* Length of the audio buffer */
#define SOUND_LEN 370
#endif

EXPORTPROC InsertADisk(void);

EXPORTVAR(ui5b, theKeys[4])

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
