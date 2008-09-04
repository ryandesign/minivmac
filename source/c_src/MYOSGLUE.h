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


#define kEmMd_128K        0
#define kEmMd_512Ke       1
#define kEmMd_Plus        2
#define kEmMd_SE          3
#define kEmMd_SEFDHD      4
#define kEmMd_Classic     5
#define kEmMd_PB100       6
#define kEmMd_II          7

EXPORTPROC WarnMsgCorruptedROM(void);
EXPORTPROC WarnMsgUnsupportedROM(void);

#ifndef DetailedAbnormalReport
#define DetailedAbnormalReport 0
#endif

#if DetailedAbnormalReport
EXPORTPROC WarnMsgAbnormal(char *s);
#else
EXPORTPROC WarnMsgAbnormal(void);
#endif

#ifndef MakeDumpFile
#define MakeDumpFile 0
#endif

#if MakeDumpFile
EXPORTPROC DumpACStr(char *s);
EXPORTPROC DumpANewLine(void);
EXPORTPROC DumpAHex(ui5r x);
EXPORTPROC DumpANum(ui5r x);
#endif

EXPORTPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount);

#define RAMSafetyMarginFudge 4

#define kRAM_Size (kRAMa_Size + kRAMb_Size)
EXPORTVAR(ui3p, RAM)
	/*
		allocated by MYOSGLUE to be at least kRAM_Size + RAMSafetyMarginFudge
		bytes. Because of shortcuts taken in ADDRSPAC.c, it is in theory
		possible for the emulator to write up to 3 bytes past kRAM_Size.
	*/


#ifndef TempDebug /* a way to mark temporary debugging code */
#if (CurEmMd >= kEmMd_SE) && (CurEmMd <= kEmMd_II)
#define TempDebug 1 /* flag some stuff that needs look at */
#else
#define TempDebug 0
#endif
#endif

EXPORTVAR(ui3p, ROM)


#define tMacErr ui4r

#define mnvm_noErr      ((tMacErr) 0)   /* 0x0000 - No Error */
#define mnvm_miscErr    ((tMacErr) - 1)  /* 0xFFFF - Should probably replace these */
#define mnvm_controlErr ((tMacErr) - 17) /* 0xFFEF - I/O System Errors */
#define mnvm_eofErr     ((tMacErr) - 39) /* 0xFFD9 - End of file */
#define mnvm_vLckdErr   ((tMacErr) - 46) /* 0xFFD2 - volume is locked */
#define mnvm_nsDrvErr   ((tMacErr) - 56) /* 0xFFC8 - No Such Drive */
#define mnvm_offLinErr  ((tMacErr) - 65) /* 0xFFBF - off-line drive */


#ifndef IncludePbufs
#define IncludePbufs 1
#endif

#if IncludePbufs

#ifndef NumPbufs
#define NumPbufs 4
#endif

#define tPbuf ui4r

#define NotAPbuf ((tPbuf)(-1))

EXPORTVAR(ui5b, PbufAllocatedMask)
EXPORTVAR(ui5b, PbufSize[NumPbufs])

#define PbufIsAllocated(i) ((PbufAllocatedMask & ((ui5b)1 << (i))) != 0)

EXPORTFUNC tMacErr PbufNew(ui5b count, tPbuf *r);
EXPORTPROC PbufDispose(tPbuf i);
EXPORTPROC PbufTransfer(ui3p Buffer,
	tPbuf i, ui5r offset, ui5r count, blnr IsWrite);

#endif

#define tDrive ui4r

EXPORTVAR(ui5b, vSonyWritableMask)
EXPORTVAR(ui5b, vSonyInsertedMask)
EXPORTVAR(ui5b, vSonyMountedMask)

#define vSonyIsInserted(Drive_No) ((vSonyInsertedMask & ((ui5b)1 << (Drive_No))) != 0)
#define vSonyIsMounted(Drive_No) ((vSonyMountedMask & ((ui5b)1 << (Drive_No))) != 0)

EXPORTFUNC tMacErr vSonyRead(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count);
EXPORTFUNC tMacErr vSonyWrite(ui3p Buffer, tDrive Drive_No, ui5r Sony_Start, ui5r *Sony_Count);
EXPORTFUNC tMacErr vSonyEject(tDrive Drive_No);
EXPORTFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count);

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
EXPORTFUNC tMacErr vSonyEjectDelete(tDrive Drive_No);
#endif

#ifndef IncludeSonyNameNew
#define IncludeSonyNameNew 1
#endif

#if IncludeSonyNameNew
EXPORTVAR(tPbuf, vSonyNewDiskName)
#endif

#if IncludeSonyGetName
EXPORTFUNC tMacErr vSonyGetName(tDrive Drive_No, tPbuf *r);
#endif

EXPORTVAR(ui5b, CurMacDateInSeconds)
EXPORTVAR(ui5b, CurMacLatitude)
EXPORTVAR(ui5b, CurMacLongitude)
EXPORTVAR(ui5b, CurMacDelta) /* (dlsDelta << 24) | (gmtDelta & 0x00FFFFFF) */


#define vMacScreenNumBits ((long)vMacScreenHeight * (long)vMacScreenWidth)
#define vMacScreenNumBytes (vMacScreenNumBits / 8)
#define vMacScreenByteWidth (vMacScreenWidth / 8)

EXPORTVAR(char, *screencomparebuff)

#ifndef IncludeVidMem
#define IncludeVidMem 0
#endif

#if IncludeVidMem
EXPORTVAR(ui3p, VidMem)
#endif

#ifndef IncludeVidRom
#define IncludeVidRom 0
#endif

#if IncludeVidRom
EXPORTVAR(ui3p, VidROM)
#endif

#define MyEvtQElKindKey 0
#define MyEvtQElKindMouseButton 1
#define MyEvtQElKindMousePos 2
#define MyEvtQElKindMouseDelta 3

struct MyEvtQEl {
	/* expected size : 8 bytes */
	ui3b kind;
	ui3b pad[3];
	union {
		struct {
			ui3b down;
			ui3b key;
		} press;
		struct {
			ui4b h;
			ui4b v;
		} pos;
	} u;
};
typedef struct MyEvtQEl MyEvtQEl;

#define MyEvtQLg2Sz 4
#define MyEvtQSz (1 << MyEvtQLg2Sz)
#define MyEvtQIMask (MyEvtQSz - 1)

EXPORTVAR(MyEvtQEl, MyEvtQA[MyEvtQSz])
EXPORTVAR(ui4r, MyEvtQIn)
EXPORTVAR(ui4r, MyEvtQOut)


EXPORTVAR(blnr, ForceMacOff)

EXPORTVAR(blnr, WantMacInterrupt)

EXPORTVAR(blnr, WantMacReset)

EXPORTFUNC blnr ExtraTimeNotOver(void);

EXPORTVAR(blnr, SpeedLimit)

EXPORTVAR(ui3b, SpeedValue)

/* where emulated machine thinks mouse is */
EXPORTVAR(ui4b, CurMouseV)
EXPORTVAR(ui4b, CurMouseH)

#ifndef EnableMouseMotion
#define EnableMouseMotion 1
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
EXPORTFUNC tMacErr HTCEexport(tPbuf i);
EXPORTFUNC tMacErr HTCEimport(tPbuf *r);
#endif

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
