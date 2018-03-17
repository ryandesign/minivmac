/*
	MYMACAPI.i
	Copyright (C) 2010 Paul C. Pratt

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
	MY MACintosh Application Programming Interfaces
*/


/* from ConditionalMacros.h */

#if defined(__SC__) && (defined(MPW_CPLUS) || defined(MPW_C))
/*
	SC[pp] compiler from Apple Computer, Inc.
*/
#define PRAGMA_PARAMETER 1
#elif defined(applec) && (! defined(__SC__)) && (! defined(__GNUC__))
/*
	MPW C compiler from Apple Computer, Inc.
*/
#elif defined(__MWERKS__)
/*
	CodeWarrior compiler from Metrowerks, Inc.
*/

#define ShouldUnloadDataInit 0
#define PRAGMA_PARAMETER 1

#elif defined(SYMANTEC_CPLUS) || defined(SYMANTEC_C)
/*
	C and C++ compiler from Symantec, Inc.
*/
#error "Symantec"
#elif defined(THINK_C)
/*
	THINK C compiler from Symantec, Inc.
*/

#define ShouldUnloadDataInit 0
#define PRAGMA_PARAMETER 1

#elif defined(_MSC_VER) && (! defined(__MWERKS__)) && defined(_M_M68K)
/*
	Visual Studio C/C++ from Microsoft, Inc.
*/
#error "Visual Studio"

#elif defined(__GNUC__)
/*
	gC for MPW from Free Software Foundation, Inc.
*/
#define PRAGMA_PARAMETER 1
#else
#error "unsupported compiler"
#endif

#ifndef PRAGMA_PARAMETER
#define PRAGMA_PARAMETER 0
#endif

#ifndef IsAnApp
#define IsAnApp 1
#endif

#define FOUR_CHAR_CODE(x)           (x)

/* From MacTypes.h */

#ifndef NULL
#if (defined(__SC__) || defined(THINK_C))
/*
	Symantec C compilers (but not C++) want NULL and nil
	to be (void*)0
*/
#define NULL ((void *) 0)
#else
/* in case int is 16-bits, make sure NULL is 32-bits */
#define NULL 0L
#endif
#endif

#ifndef nil
#define nil NULL
#endif

typedef signed char SInt8;
typedef signed short SInt16;
typedef signed long SInt32;

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned long UInt32;

typedef long Fixed;

typedef char * Ptr;
typedef Ptr * Handle;
typedef long Size;

typedef SInt16 OSErr;
typedef SInt32 OSStatus;
typedef UInt32 ByteCount;
typedef unsigned long FourCharCode;
typedef FourCharCode OSType;
typedef FourCharCode ResType;

enum {
	false                         = 0,
	true                          = 1
};
typedef unsigned char Boolean;
typedef long (* ProcPtr)();
typedef ProcPtr UniversalProcPtr;

typedef unsigned char Str255[256];
typedef unsigned char Str63[64];
typedef unsigned char Str32[33];

typedef Str63                           StrFileName;

typedef unsigned char *                 StringPtr;
typedef StringPtr *                     StringHandle;
typedef const unsigned char *           ConstStr255Param;
typedef const unsigned char *           ConstStr63Param;
typedef const unsigned char *           ConstStr32Param;
typedef const unsigned char *           ConstStr31Param;

struct Point {
	short v;
	short h;
};
typedef struct Point Point;

struct Rect {
	short               top;
	short               left;
	short               bottom;
	short               right;
};
typedef struct Rect Rect;

typedef short CharParameter;

enum {
	normal                        = 0,
	bold                          = 1,
	italic                        = 2,
	underline                     = 4,
	outline                       = 8,
	shadow                        = 0x10,
	condense                      = 0x20,
	extend                        = 0x40
};

typedef unsigned char Style;
typedef short StyleParameter;
typedef Style StyleField;

typedef UInt8 Byte;
typedef SInt8 SignedByte;

pascal void
Debugger(void)
	= {
		0xA9FF
	};

pascal void
DebugStr(ConstStr255Param debuggerMsg)
	= {
		0xABFF
	};

pascal void SysBeep(short duration)
	= {
		0xA9C8
	};

/* from Limits */

#define CHAR_BIT 8

#define LONG_MAX 2147483647

/* Script.h */

enum {
	smSystemScript                = -1,
	smCurrentScript               = -2,
	smAllScripts                  = -3
};

/* from MacErrors.h */

enum {
	dirFulErr                     = -33,
	dskFulErr                     = -34,
	nsvErr                        = -35,
	bdNamErr                      = -37,
	eofErr                        = -39,
	mFulErr                       = -41,
	fnfErr                        = -43,
	wPrErr                        = -44,
	fLckdErr                      = -45,
	vLckdErr                      = -46,
	fBsyErr                       = -47,
	dupFNErr                      = -48,
	opWrErr                       = -49,
	paramErr                      = -50,
	rfNumErr                      = -51,
	volOffLinErr                  = -53,
	permErr                       = -54,
	memFullErr                    = -108,
	dirNFErr                      = -120,

	gestaltUnknownErr             = -5550,
	gestaltUndefSelectorErr       = -5551,
	gestaltDupSelectorErr         = -5552,
	gestaltLocationErr            = -5553,

	procNotFound = -600
};

/* from Traps.h */

enum {
	_InitGraf                     = 0xA86E,
	_Gestalt                      = 0xA1AD,
	_WaitNextEvent                = 0xA860,
	_Unimplemented                = 0xA89F
};

/* from Gestalt.h */

pascal OSErr Gestalt(OSType selector, long *response)
	= { /* 0xA1AD, 0x2288 */
		0x225F, /* MOVEA.L    (A7)+,A1 */
		0x201F, /* MOVE.L     (A7)+,D0 */
		0xA1AD, /* DC.W       $A1AD */
		0x2288, /* MOVE.L     A0,(A1) */
		0x3E80  /* MOVE.W     D0,(A7) */
	};

enum {
	gestaltPhysicalRAMSize        = FOUR_CHAR_CODE('ram ')
};

enum {
	gestaltStandardFileAttr = FOUR_CHAR_CODE('stdf'),
	gestaltStandardFile58 = 0,
	gestaltStandardFileTranslationAware = 1,
	gestaltStandardFileHasColorIcons = 2,
	gestaltStandardFileUseGenericIcons = 3,
	gestaltStandardFileHasDynamicVolumeAllocation = 4
};

enum {
	gestaltAppleEventsAttr = FOUR_CHAR_CODE('evnt'),
	gestaltAppleEventsPresent     = 0,
	gestaltScriptingSupport       = 1,
	gestaltOSLInSystem            = 2,
	gestaltSupportsApplicationURL = 4
};

enum {
	gestaltDragMgrAttr = FOUR_CHAR_CODE('drag'),
	gestaltDragMgrPresent  = 0
};

enum {
	gestaltAliasMgrAttr = FOUR_CHAR_CODE('alis'),
	gestaltAliasMgrPresent  = 0
};

/* From MacMemory.h */

pascal OSErr
MemError(void)
	= {
		0x3EB8, 0x0220 /* MOVE.W     $0BAC,(A7) */
	};

#if PRAGMA_PARAMETER
#pragma parameter __A0 NewPtr(__D0)
#endif
pascal Ptr
NewPtr(Size byteCount)
	= {
#if PRAGMA_PARAMETER
		0xA11E
#else
		0x201F, /* MOVE.L     (A7)+,D0 */
		0xA11E, /* _NewPtr */
		0x2E88, /* MOVE.L     A0,(A7) */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter DisposePtr(__A0)
#endif
pascal void
DisposePtr(Ptr p)
	= {
#if PRAGMA_PARAMETER
		0xA01F
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA01F, /* _DisposPtr */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __A0 NewHandle(__D0)
#endif
pascal Handle
NewHandle(Size byteCount)
	= {
#if PRAGMA_PARAMETER
		0xA122
#else
		0x201F, /* MOVE.L     (A7)+,D0 */
		0xA122, /* _NewHandle */
		0x2E88, /* MOVE.L     A0,(A7) */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter DisposeHandle(__A0)
#endif
pascal void
DisposeHandle(Handle h)
	= {
#if PRAGMA_PARAMETER
		0xA023
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA023, /* _DisposHandle */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter ReallocateHandle(__A0, __D0)
#endif
pascal void
ReallocateHandle(
	Handle   h,
	Size     byteCount)
	= {
#if PRAGMA_PARAMETER
		0xA027
#else
		0x201F, /* MOVE.L     (A7)+,D0 */
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA027, /* _ReallocHandle */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter SetHandleSize(__A0, __D0)
#endif
pascal void
SetHandleSize(
	Handle   h,
	Size     newSize)
	= {
#if PRAGMA_PARAMETER
		0xA024
#else
		0x201F, /* MOVE.L     (A7)+,D0 */
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA024, /* _SetHandleSize */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 InlineGetHandleSize(__A0)
#endif
pascal Size
InlineGetHandleSize(Handle h)
	= {
#if PRAGMA_PARAMETER
		0xA025
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA025, /* _GetHandleSize */
		0x2E80  /* MOVE.L     D0,(A7) */
#endif
	};

pascal Size
GetHandleSize(Handle h)
	= {
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA025, /* _GetHandleSize */
		0x2E80, /* MOVE.L     D0,(A7) */
		0x6A04, /* BPL.S      @2 */
		0x4297, /* CLR.L      (A7) */
		0x6002, /* BRA.S      @1 */
			/* @2: */
		0x7000, /* MOVEQ      #$00,D0 */
			/* @1: */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
	};

#if PRAGMA_PARAMETER
#pragma parameter HLock(__A0)
#endif
pascal void
HLock(Handle h)
	= {
#if PRAGMA_PARAMETER
		0xA029
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA029, /* _HLock */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter HUnlock(__A0)
#endif
pascal void
HUnlock(Handle h)
	= {
#if PRAGMA_PARAMETER
		0xA02A
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA02A, /* _HUnlock */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter HPurge(__A0)
#endif
pascal void
HPurge(Handle h)
	= {
#if PRAGMA_PARAMETER
		0xA049
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA049, /* _HPurge */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter HNoPurge(__A0)
#endif
pascal void
HNoPurge(Handle h)
	= {
#if PRAGMA_PARAMETER
		0xA04A
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA04A, /* _HNoPurge */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter BlockMove(__A0, __A1, __D0)
#endif
pascal void
BlockMove(
	const void *  srcPtr,
	void *        destPtr,
	Size          byteCount)
	= {
#if PRAGMA_PARAMETER
		0xA02E
#else
		0x201F, /* MOVE.L     (A7)+,D0 */
		0x225F, /* MOVEA.L    (A7)+,A1 */
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA02E, /* _BlockMove */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

pascal void
MaxApplZone(void)
	= {
		0xA063
	};

pascal void
MoreMasters(void)
	= {
		0xA036
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 FreeMem
#endif
pascal long
FreeMem(void)
	= {
#if PRAGMA_PARAMETER
		0xA01C
#else
		0xA01C, /* _FreeMem */
		0x2E80  /* MOVE.L     D0,(A7) */
#endif
	};

pascal void
PurgeSpace(
	long *  total,
	long *  contig)
	= {
		0xA162, /* _PurgeSpace */
		0x225F, /* MOVEA.L    (A7)+,A1 */
		0x2288, /* MOVE.L     A0,(A1) */
		0x225F, /* MOVEA.L    (A7)+,A1 */
		0x2280  /* MOVE.L     D0,(A1) */
	};

typedef long (* GrowZoneProcPtr)(Size cbNeeded);
typedef GrowZoneProcPtr GrowZoneUPP;

typedef void (* PurgeProcPtr)(Handle blockToPurge);
typedef PurgeProcPtr PurgeUPP;

struct Zone {
	Ptr                 bkLim;
	Ptr                 purgePtr;
	Ptr                 hFstFree;
	long                zcbFree;
	GrowZoneUPP         gzProc;
	short               moreMast;
	short               flags;
	short               cntRel;
	short               maxRel;
	short               cntNRel;
	SInt8               heapType;
	SInt8               unused;
	short               cntEmpty;
	short               cntHandles;
	long                minCBFree;
	PurgeUPP            purgeProc;
	Ptr                 sparePtr;
	Ptr                 allocPtr;
	short               heapData;
};
typedef struct Zone Zone;
typedef Zone * THz;
typedef THz * THzPtr;

#if PRAGMA_PARAMETER
#pragma parameter __A0 GetZone
#endif
pascal THz
GetZone(void)
	= {
#if PRAGMA_PARAMETER
		0xA11A
#else
		0xA11A, /* _GetZone */
		0x2E88, /* MOVE.L     A0,(A7) */
		0x31C0, 0x0220 /* MOVE.W     D0,$0220 */
#endif
	};

/* From OSUtils.h */

pascal UInt32
TickCount(void)
	= {
		0xA975
	};

#if PRAGMA_PARAMETER
#pragma parameter Delay(__A0, __A1)
#endif
pascal void
Delay(
	unsigned long numTicks,
	unsigned long * finalTicks)
	= {
#if PRAGMA_PARAMETER
		0xA03B, 0x2280
#else
		0x225F, /* MOVEA.L    (A7)+,A1 */
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA03B, /* _Delay */
		0x2280  /* MOVE.L     D0,(A1) */
#endif
	};

struct QElem {
	struct QElem *      qLink;
	short               qType;
	short               qData[1];
};
typedef struct QElem QElem;
typedef QElem * QElemPtr;

struct QHdr {
	volatile short      qFlags;
	volatile QElemPtr   qHead;
	volatile QElemPtr   qTail;
};
typedef struct QHdr QHdr;
typedef QHdr * QHdrPtr;

struct SysEnvRec {
	short               environsVersion;
	short               machineType;
	short               systemVersion;
	short               processor;
	Boolean             hasFPU;
	Boolean             hasColorQD;
	short               keyBoardType;
	short               atDrvrVersNum;
	short               sysVRefNum;
};
typedef struct SysEnvRec SysEnvRec;

#if PRAGMA_PARAMETER
#pragma parameter __D0 SysEnvirons(__D0, __A0)
#endif
pascal OSErr
SysEnvirons(
	short        versionRequested,
	SysEnvRec *  theWorld)
	= {
#if PRAGMA_PARAMETER
		0xA090
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0x301F, /* MOVE.W     (A7)+,D0 */
		0xA090, /* _SysEnvirons */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

/* From Finder.h */

struct FInfo {
	OSType              fdType;
	OSType              fdCreator;
	UInt16              fdFlags;
	Point               fdLocation;
	SInt16              fdFldr;
};
typedef struct FInfo FInfo;

struct FXInfo {
	SInt16              fdIconID;
	SInt16              fdReserved[3];
	SInt8               fdScript;
	SInt8               fdXFlags;
	SInt16              fdComment;
	SInt32              fdPutAway;
};
typedef struct FXInfo FXInfo;

struct DInfo {
	Rect                frRect;
	UInt16              frFlags;
	Point               frLocation;
	SInt16              frView;
};
typedef struct DInfo DInfo;

struct DXInfo {
	Point               frScroll;
	SInt32              frOpenChain;
	SInt8               frScript;
	SInt8               frXFlags;
	SInt16              frComment;
	SInt32              frPutAway;
};
typedef struct DXInfo DXInfo;

/* From Files.h */

enum {
	fsCurPerm                     = 0x00,
	fsRdPerm                      = 0x01,
	fsWrPerm                      = 0x02,
	fsRdWrPerm                    = 0x03
};

enum {
	fsAtMark                      = 0,
	fsFromStart                   = 1,
	fsFromLEOF                    = 2,
	fsFromMark                    = 3
};

enum {
	kioFlAttribLockedBit          = 0,
	kioFlAttribLockedMask         = 0x01,
	kioFlAttribResOpenBit         = 2,
	kioFlAttribResOpenMask        = 0x04,
	kioFlAttribDataOpenBit        = 3,
	kioFlAttribDataOpenMask       = 0x08,
	kioFlAttribDirBit             = 4,
	kioFlAttribDirMask            = 0x10,
	kioFlAttribCopyProtBit        = 6,
	kioFlAttribCopyProtMask       = 0x40,
	kioFlAttribFileOpenBit        = 7,
	kioFlAttribFileOpenMask       = 0x80,
	kioFlAttribInSharedBit        = 2,
	kioFlAttribInSharedMask       = 0x04,
	kioFlAttribMountedBit         = 3,
	kioFlAttribMountedMask        = 0x08,
	kioFlAttribSharePointBit      = 5,
	kioFlAttribSharePointMask     = 0x20
};

typedef union ParamBlockRec ParamBlockRec;

typedef ParamBlockRec * ParmBlkPtr;

typedef pascal void (*IOCompletionProcPtr)(ParmBlkPtr paramBlock);

#define IOCompletionUPP IOCompletionProcPtr

struct IOParam {
	QElemPtr            qLink;
	short               qType;
	short               ioTrap;
	Ptr                 ioCmdAddr;
	IOCompletionUPP     ioCompletion;
	volatile OSErr      ioResult;
	StringPtr           ioNamePtr;
	short               ioVRefNum;
	short               ioRefNum;
	SInt8               ioVersNum;
	SInt8               ioPermssn;
	Ptr                 ioMisc;
	Ptr                 ioBuffer;
	long                ioReqCount;
	long                ioActCount;
	short               ioPosMode;
	long                ioPosOffset;
};
typedef struct IOParam                  IOParam;
typedef IOParam *                       IOParamPtr;

struct FileParam {
	QElemPtr            qLink;
	short               qType;
	short               ioTrap;
	Ptr                 ioCmdAddr;
	IOCompletionUPP     ioCompletion;
	volatile OSErr      ioResult;
	StringPtr           ioNamePtr;
	short               ioVRefNum;
	short               ioFRefNum;
	SInt8               ioFVersNum;
	SInt8               filler1;
	short               ioFDirIndex;
	SInt8               ioFlAttrib;
	SInt8               ioFlVersNum;
	FInfo               ioFlFndrInfo;
	unsigned long       ioFlNum;
	unsigned short      ioFlStBlk;
	long                ioFlLgLen;
	long                ioFlPyLen;
	unsigned short      ioFlRStBlk;
	long                ioFlRLgLen;
	long                ioFlRPyLen;
	unsigned long       ioFlCrDat;
	unsigned long       ioFlMdDat;
};
typedef struct FileParam FileParam;
typedef FileParam * FileParamPtr;

struct VolumeParam {
	QElemPtr            qLink;
	short               qType;
	short               ioTrap;
	Ptr                 ioCmdAddr;
	IOCompletionUPP     ioCompletion;
	volatile OSErr      ioResult;
	StringPtr           ioNamePtr;
	short               ioVRefNum;
	long                filler2;
	short               ioVolIndex;
	unsigned long       ioVCrDate;
	unsigned long       ioVLsBkUp;
	unsigned short      ioVAtrb;
	unsigned short      ioVNmFls;
	unsigned short      ioVDirSt;
	short               ioVBlLn;
	unsigned short      ioVNmAlBlks;
	unsigned long       ioVAlBlkSiz;
	unsigned long       ioVClpSiz;
	unsigned short      ioAlBlSt;
	unsigned long       ioVNxtFNum;
	unsigned short      ioVFrBlk;
};
typedef struct VolumeParam VolumeParam;
typedef VolumeParam * VolumeParamPtr;

union ParamBlockRec {
	IOParam             ioParam;
	FileParam           fileParam;
	VolumeParam         volumeParam;
#if 0
	CntrlParam          cntrlParam;
	SlotDevParam        slotDevParam;
	MultiDevParam       multiDevParam;
#endif
};

struct HFileInfo {
	QElemPtr            qLink;
	short               qType;
	short               ioTrap;
	Ptr                 ioCmdAddr;
	IOCompletionUPP     ioCompletion;
	volatile OSErr      ioResult;
	StringPtr           ioNamePtr;
	short               ioVRefNum;
	short               ioFRefNum;
	SInt8               ioFVersNum;
	SInt8               filler1;
	short               ioFDirIndex;
	SInt8               ioFlAttrib;
	SInt8               ioACUser;
	FInfo               ioFlFndrInfo;
	long                ioDirID;
	unsigned short      ioFlStBlk;
	long                ioFlLgLen;
	long                ioFlPyLen;
	unsigned short      ioFlRStBlk;
	long                ioFlRLgLen;
	long                ioFlRPyLen;
	unsigned long       ioFlCrDat;
	unsigned long       ioFlMdDat;
	unsigned long       ioFlBkDat;
	FXInfo              ioFlXFndrInfo;
	long                ioFlParID;
	long                ioFlClpSiz;
};
typedef struct HFileInfo                HFileInfo;

struct DirInfo {
	QElemPtr            qLink;
	short               qType;
	short               ioTrap;
	Ptr                 ioCmdAddr;
	IOCompletionUPP     ioCompletion;
	volatile OSErr      ioResult;
	StringPtr           ioNamePtr;
	short               ioVRefNum;
	short               ioFRefNum;
	SInt8               ioFVersNum;
	SInt8               filler1;
	short               ioFDirIndex;
	SInt8               ioFlAttrib;
	SInt8               ioACUser;
	DInfo               ioDrUsrWds;
	long                ioDrDirID;
	unsigned short      ioDrNmFls;
	short               filler3[9];
	unsigned long       ioDrCrDat;
	unsigned long       ioDrMdDat;
	unsigned long       ioDrBkDat;
	DXInfo              ioDrFndrInfo;
	long                ioDrParID;
};
typedef struct DirInfo DirInfo;

union CInfoPBRec {
	HFileInfo           hFileInfo;
	DirInfo             dirInfo;
};
typedef union CInfoPBRec CInfoPBRec;
typedef CInfoPBRec * CInfoPBPtr;

struct HIOParam {
	QElemPtr            qLink;
	short               qType;
	short               ioTrap;
	Ptr                 ioCmdAddr;
	IOCompletionUPP     ioCompletion;
	volatile OSErr      ioResult;
	StringPtr           ioNamePtr;
	short               ioVRefNum;
	short               ioRefNum;
	SInt8               ioVersNum;
	SInt8               ioPermssn;
	Ptr                 ioMisc;
	Ptr                 ioBuffer;
	long                ioReqCount;
	long                ioActCount;
	short               ioPosMode;
	long                ioPosOffset;
};
typedef struct HIOParam HIOParam;
typedef HIOParam * HIOParamPtr;

struct HFileParam {
	QElemPtr            qLink;
	short               qType;
	short               ioTrap;
	Ptr                 ioCmdAddr;
	IOCompletionUPP     ioCompletion;
	volatile OSErr      ioResult;
	StringPtr           ioNamePtr;
	short               ioVRefNum;
	short               ioFRefNum;
	SInt8               ioFVersNum;
	SInt8               filler1;
	short               ioFDirIndex;
	SInt8               ioFlAttrib;
	SInt8               ioFlVersNum;
	FInfo               ioFlFndrInfo;
	long                ioDirID;
	unsigned short      ioFlStBlk;
	long                ioFlLgLen;
	long                ioFlPyLen;
	unsigned short      ioFlRStBlk;
	long                ioFlRLgLen;
	long                ioFlRPyLen;
	unsigned long       ioFlCrDat;
	unsigned long       ioFlMdDat;
};
typedef struct HFileParam HFileParam;
typedef HFileParam * HFileParamPtr;

struct HVolumeParam {
	QElemPtr            qLink;
	short               qType;
	short               ioTrap;
	Ptr                 ioCmdAddr;
	IOCompletionUPP     ioCompletion;
	volatile OSErr      ioResult;
	StringPtr           ioNamePtr;
	short               ioVRefNum;
	long                filler2;
	short               ioVolIndex;
	unsigned long       ioVCrDate;
	unsigned long       ioVLsMod;
	short               ioVAtrb;
	unsigned short      ioVNmFls;
	unsigned short      ioVBitMap;
	unsigned short      ioAllocPtr;
	unsigned short      ioVNmAlBlks;
	unsigned long       ioVAlBlkSiz;
	unsigned long       ioVClpSiz;
	unsigned short      ioAlBlSt;
	unsigned long       ioVNxtCNID;
	unsigned short      ioVFrBlk;
	unsigned short      ioVSigWord;
	short               ioVDrvInfo;
	short               ioVDRefNum;
	short               ioVFSID;
	unsigned long       ioVBkUp;
	short               ioVSeqNum;
	unsigned long       ioVWrCnt;
	unsigned long       ioVFilCnt;
	unsigned long       ioVDirCnt;
	long                ioVFndrInfo[8];
};
typedef struct HVolumeParam HVolumeParam;
typedef HVolumeParam * HVolumeParamPtr;

union HParamBlockRec {
	HIOParam            ioParam;
	HFileParam          fileParam;
	HVolumeParam        volumeParam;
#if 0
	AccessParam         accessParam;
	ObjParam            objParam;
	CopyParam           copyParam;
	WDParam             wdParam;
	FIDParam            fidParam;
	CSParam             csParam;
	ForeignPrivParam    foreignPrivParam;
#endif
};
typedef union HParamBlockRec            HParamBlockRec;


typedef HParamBlockRec *                HParmBlkPtr;

struct WDPBRec {
	QElemPtr            qLink;
	short               qType;
	short               ioTrap;
	Ptr                 ioCmdAddr;
	IOCompletionUPP     ioCompletion;
	volatile OSErr      ioResult;
	StringPtr           ioNamePtr;
	short               ioVRefNum;
	short               filler1;
	short               ioWDIndex;
	long                ioWDProcID;
	short               ioWDVRefNum;
	short               filler2[7];
	long                ioWDDirID;
};
typedef struct WDPBRec WDPBRec;
typedef WDPBRec * WDPBPtr;

struct FCBPBRec {
	QElemPtr            qLink;
	short               qType;
	short               ioTrap;
	Ptr                 ioCmdAddr;
	IOCompletionUPP     ioCompletion;
	volatile OSErr      ioResult;
	StringPtr           ioNamePtr;
	short               ioVRefNum;
	short               ioRefNum;
	short               filler;
	short               ioFCBIndx;
	short               filler1;
	long                ioFCBFlNm;
	short               ioFCBFlags;
	unsigned short      ioFCBStBlk;
	long                ioFCBEOF;
	long                ioFCBPLen;
	long                ioFCBCrPs;
	short               ioFCBVRefNum;
	long                ioFCBClpSiz;
	long                ioFCBParID;
};
typedef struct FCBPBRec FCBPBRec;
typedef FCBPBRec * FCBPBPtr;

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBOpenSync(__A0)
#endif
pascal OSErr
PBOpenSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA000
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA000, /* _Open */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBGetVolSync(__A0)
#endif
pascal OSErr
PBGetVolSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA014
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA014, /* _GetVol */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBHGetVolSync(__A0)
#endif
pascal OSErr
PBHGetVolSync(WDPBPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA214
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA214, /* _GetVol    ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBWriteSync(__A0)
#endif
pascal OSErr
PBWriteSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA003
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA003, /* _Write */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBReadSync(__A0)
#endif
pascal OSErr
PBReadSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA002
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA002, /* _Read */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBHOpenSync(__A0)
#endif
pascal OSErr
PBHOpenSync(HParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA200
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA200, /* _Open */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBOpenRFSync(__A0)
#endif
pascal OSErr
PBOpenRFSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA00A
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA00A, /* _OpenRF */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBHOpenRFSync(__A0)
#endif
pascal OSErr
PBHOpenRFSync(HParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA20A
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA20A, /* _OpenRF    ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBCreateSync(__A0)
#endif
pascal OSErr
PBCreateSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA008
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA008, /* _Create */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBHCreateSync(__A0)
#endif
pascal OSErr
PBHCreateSync(HParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA208
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA208, /* _Create    ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBDirCreateSync(__A0)
#endif
pascal OSErr
PBDirCreateSync(HParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0x7006, 0xA260
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0x7006, /* MOVEQ      #$06,D0 */
		0xA260, /* _HFSDispatch  ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBDeleteSync(__A0)
#endif
pascal OSErr
PBDeleteSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA009
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA009, /* _Delete */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBHDeleteSync(__A0)
#endif
pascal OSErr
PBHDeleteSync(HParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA209
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA209, /* _Delete    ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBGetFInfoSync(__A0)
#endif
pascal OSErr
PBGetFInfoSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA00C
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA00C, /* _GetFileInfo */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBHGetFInfoSync(__A0)
#endif
pascal OSErr
PBHGetFInfoSync(HParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA20C
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA20C, /* _GetFileInfo  ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBSetFInfoSync(__A0)
#endif
pascal OSErr
PBSetFInfoSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA00D
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA00D, /* _SetFileInfo */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBHSetFInfoSync(__A0)
#endif
pascal OSErr
PBHSetFInfoSync(HParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA20D
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA20D, /* _SetFileInfo  ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBGetFCBInfoSync(__A0)
#endif
pascal OSErr
PBGetFCBInfoSync(FCBPBPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0x7008, 0xA260
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0x7008, /* MOVEQ      #$08,D0 */
		0xA260, /* _HFSDispatch  ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBGetCatInfoSync(__A0)
#endif
pascal OSErr
PBGetCatInfoSync(CInfoPBPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0x7009, 0xA260
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0x4228, 0x001A, /* CLR.B      $001A(A0) */ /*& ? &*/
		0x7009, /* MOVEQ      #$09,D0 */
		0xA260, /* _HFSDispatch  ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBSetCatInfoSync(__A0)
#endif
pascal OSErr
PBSetCatInfoSync(CInfoPBPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0x700A, 0xA260
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0x700A, /* MOVEQ      #$0A,D0 */
		0xA260, /* _HFSDispatch  ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBGetWDInfoSync(__A0)
#endif
pascal OSErr
PBGetWDInfoSync(WDPBPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0x7007, 0xA260
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0x7007, /* MOVEQ      #$07,D0 */
		0xA260, /* _HFSDispatch  ,Immed */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBGetVInfoSync(__A0)
#endif
pascal OSErr
PBGetVInfoSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA007
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA007, /* _GetVolInfo */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBGetEOFSync(__A0)
#endif
pascal OSErr PBGetEOFSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA011
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA011, /* _GetEOF */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBSetEOFSync(__A0)
#endif
pascal OSErr PBSetEOFSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA012
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA012, /* _SetEOF */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBSetFPosSync(__A0)
#endif
pascal OSErr
PBSetFPosSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA044
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA044, /* _SetFPos */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBGetFPosSync(__A0)
#endif
pascal OSErr
PBGetFPosSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA018
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA018, /* _GetFPos */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBCloseSync(__A0)
#endif
pascal OSErr
PBCloseSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA001
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA001, /* _Close */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

struct FSSpec {
	short               vRefNum;
	long                parID;
	StrFileName         name;
};
typedef struct FSSpec FSSpec;
typedef FSSpec * FSSpecPtr;
typedef FSSpecPtr FSSpecArrayPtr;

pascal OSErr
FSpOpenDF(
	const FSSpec *  spec,
	SInt8           permission,
	short *         refNum)
	= {
		0x7002, 0xAA52
	};

pascal OSErr
FSpOpenRF(
	const FSSpec *  spec,
	SInt8           permission,
	short *         refNum)
	= {
		0x7003, 0xAA52
	};

typedef SInt16 ScriptCode;

pascal OSErr
FSpCreate(
	const FSSpec *  spec,
	OSType          creator,
	OSType          fileType,
	ScriptCode      scriptTag)
	= {
		0x7004, 0xAA52
	};

pascal OSErr
FSpDelete(const FSSpec * spec)
	= {
		0x7006, 0xAA52
	};

pascal OSErr
FSMakeFSSpec(
	short              vRefNum,
	long               dirID,
	ConstStr255Param   fileName,
	FSSpec *           spec)
	= {
		0x7001, 0xAA52
	};

#if PRAGMA_PARAMETER
#pragma parameter __D0 PBFlushVolSync(__A0)
#endif
pascal OSErr PBFlushVolSync(ParmBlkPtr paramBlock)
	= {
#if PRAGMA_PARAMETER
		0xA013
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA013, /* _FlushVol */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

#define LMGetFSFCBLen() (*(SInt16 *)(0x03F6))
#define LMGetFCBSPtr() (*(Ptr *)(0x034E))

static pascal OSErr
GetVRefNum(
	short    fileRefNum,
	short *  vRefNum)
{
	OSErr err;
	UInt16 offset = (UInt16)fileRefNum;
	SInt16 L = LMGetFSFCBLen();
	if (L < 0) {
		L = 30;
	}
	if (2 == (offset % (UInt16)L)) {
		Ptr p = LMGetFCBSPtr();

		if (offset < (*(UInt16 *)p)) {
			p = *(Ptr *)(p + offset + 20); /* volume control block */
			*vRefNum = *(short *)(p + 0x004E);
				/* vcbVRefNum : Volume reference number */
			err = 0;
			goto label_done;
		}
	}

	*vRefNum = 0;
	err = rfNumErr;

label_done:
	return err;
}

/* From Resource.h */

pascal short
HOpenResFile(
	short              vRefNum,
	long               dirID,
	ConstStr255Param   fileName,
	SInt8              permission)
	= {
		0xA81A
	};

pascal void
CloseResFile(short refNum)
	= {
		0xA99A
	};

#define LMGetResErr() (*(SInt16 *)(0x0A60))
	/* same result as ResError */

pascal OSErr
ResError(void)
	= {
		0xA9AF
	};

#define LMGetCurMap() (*(SInt16 *)(0x0A5A))
	/* same result as CurResFile */

pascal short
CurResFile(void)
	= {
		0xA994
	};

pascal void
UseResFile(short refNum)
	= {
		0xA998
	};

pascal Handle
GetNamedResource(
	ResType            theType,
	ConstStr255Param   name)
	= {
		0xA9A1
	};

pascal void
ReleaseResource(Handle theResource)
	= {
		0xA9A3
	};

pascal void
DetachResource(Handle theResource)
	= {
		0xA992
	};

pascal Handle
Get1IndResource(
	ResType   theType,
	short     index)
	= {
		0xA80E
	};

pascal Handle
GetResource(
	ResType   theType,
	short     theID)
	= {
		0xA9A0
	};

/* From Patches.h */

#if PRAGMA_PARAMETER
#pragma parameter __A0 GetOSTrapAddress(__D0)
#endif
pascal UniversalProcPtr
GetOSTrapAddress(UInt16 trapNum)
	= {
#if PRAGMA_PARAMETER
		0xA346
#else
		0x301F, /* MOVE.W     (A7)+,D0 */
		0xA346, /* _GetTrapAddress  ,Immed */
		0x2E88  /* MOVE.L     A0,(A7) */
#endif
	};

#if PRAGMA_PARAMETER
#pragma parameter __A0 GetToolTrapAddress(__D0)
#endif
pascal UniversalProcPtr
GetToolTrapAddress(UInt16 trapNum)
	= {
#if PRAGMA_PARAMETER
		0xA746
#else
		0x301F, /* MOVE.W     (A7)+,D0 */
		0xA746, /* _GetTrapAddress  ,Sys,Immed */
		0x2E88  /* MOVE.L     A0,(A7) */
#endif
	};

enum {
	noErr                         = 0
};

/* From SegLoad.h */

pascal void
	UnloadSeg(void * routineAddr)
	= {
		0xA9F1
	};


struct AppFile {
	short               vRefNum;
	OSType              fType;
	short               versNum;
	Str255              fName;
};
typedef struct AppFile AppFile;

typedef AppFile * AppFileP;

struct AppFilesA {
	short message;
	short count;
	AppFile first;
};
typedef struct AppFilesA AppFilesA;
typedef AppFilesA * AppFilesAP;

static pascal void
CountAppFiles(
	short *  message,
	short *  count)
{
	Handle h = *(Handle *)0x0AEC;
	Size l = InlineGetHandleSize(h);
	if (l <= 0) {
		*count = 0;
	} else {
		AppFilesAP p = (AppFilesAP)*h;
		*message = p->message;
		*count = p->count;
	}
}

static pascal AppFileP FindAppFile(short index)
{
	AppFileP v = NULL;
	Handle h = *(Handle *)0x0AEC;
	Size L = InlineGetHandleSize(h);
	if (L > 0) {
		AppFilesAP p = (AppFilesAP)*h;
		short i = index - 1; /* convert to zero based */
		if ((i < p->count) && (i >= 0)) {
			v = &p->first;

			while (0 != i) {
				Size L2 = 8 + (0xFFFE & (v->fName[0] + 2));
				v = (AppFileP)(L2 + (Ptr)v);
				--i;
			}
		}
	}

	return v;
}

static pascal void
GetAppFiles(
	short      index,
	AppFile *  theFile)
{
	AppFileP p = FindAppFile(index);
	if (NULL != p) {
		Size L = 8 + (0xFFFE & (p->fName[0] + 2));
		BlockMove((void *)p, (void *)theFile, L);
	}
}

static pascal void
ClrAppFiles(short index)
{
	AppFileP p = FindAppFile(index);
	if (NULL != p) {
		p->fType = 0;
	}
}

/* from TextUtils.h */

static pascal void GetIndString(
	Str255   theString,
	short    strListID,
	short    index)
{
	Handle h;

	theString[0] = 0;
	h = GetResource(FOUR_CHAR_CODE('STR#'), strListID);
	if (NULL != h) {
		UInt8 L;
		UInt8 *p = (UInt8 *)*h;
		UInt16 n = *((UInt16 *)p);
		p += 2;
		if ((index <= n) && (index > 0)) {
			while (--index != 0) {
				L = *p++;
				p += L;
			}
			L = *p;
			BlockMove(p, theString, L + 1);
		}
	}
}

/* From DateTimeUtils.h */

#if PRAGMA_PARAMETER
#pragma parameter __D0 ReadDateTime(__A0)
#endif
pascal OSErr
ReadDateTime(unsigned long * time)
	= {
#if PRAGMA_PARAMETER
		0xA039
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA039, /* _ReadDateTime */
		0x3E80  /* MOVE.W     D0,(A7) */
#endif
	};

struct DateTimeRec {
	short year;
	short month;
	short day;
	short hour;
	short minute;
	short second;
	short dayOfWeek;
};
typedef struct DateTimeRec DateTimeRec;

#if PRAGMA_PARAMETER
#pragma parameter SecondsToDate(__D0, __A0)
#endif
pascal void
SecondsToDate(
	unsigned long   secs,
	DateTimeRec *   d)
	= {
#if PRAGMA_PARAMETER
		0xA9C6
#else
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0x201F, /* MOVE.L     (A7)+,D0 */
		0xA9C6  /* _Secs2Date */
#endif
	};

/* From Fonts.h */

pascal void
InitFonts(void)
	= {
		0xA8FE
	};

/* From QuickdrawText.h */

struct FontInfo {
	short               ascent;
	short               descent;
	short               widMax;
	short               leading;
};
typedef struct FontInfo FontInfo;

pascal void
TextFont(short font)
	= {
		0xA887
	};

pascal void
TextFace(StyleParameter face)
	= {
		0xA888
	};

pascal void
TextMode(short mode)
	= {
		0xA889
	};

pascal void
TextSize(short size)
	= {
		0xA88A
	};

pascal void
DrawText(
	const void *  textBuf,
	short         firstByte,
	short         byteCount)
	= {
		0xA885
	};

/* From Quickdraw.h */

enum {
	srcCopy                       = 0,
	srcOr                         = 1,
	srcXor                        = 2,
	srcBic                        = 3,
	notSrcCopy                    = 4,
	notSrcOr                      = 5,
	notSrcXor                     = 6,
	notSrcBic                     = 7,
	patCopy                       = 8,
	patOr                         = 9,
	patXor                        = 10,
	patBic                        = 11,
	notPatCopy                    = 12,
	notPatOr                      = 13,
	notPatXor                     = 14,
	notPatBic                     = 15
};

enum {
	kQDGrafVerbFrame              = 0,
	kQDGrafVerbPaint              = 1,
	kQDGrafVerbErase              = 2,
	kQDGrafVerbInvert             = 3,
	kQDGrafVerbFill               = 4
};
typedef SInt8 GrafVerb;

struct Pattern {
	UInt8               pat[8];
};
typedef struct Pattern Pattern;

struct BitMap {
	Ptr                 baseAddr;
	short               rowBytes;
	Rect                bounds;
};
typedef struct BitMap BitMap;

typedef short Bits16[16];

struct Cursor {
	Bits16              data;
	Bits16              mask;
	Point               hotSpot;
};
typedef struct Cursor Cursor;
typedef Cursor * CursPtr;
typedef CursPtr * CursHandle;

struct Region {
	unsigned short      rgnSize;
	Rect                rgnBBox;
};
typedef struct Region Region;
typedef Region * RgnPtr;
typedef RgnPtr * RgnHandle;

struct Picture {
	short               picSize;
	Rect                picFrame;
};
typedef struct Picture Picture;
typedef Picture * PicPtr;
typedef PicPtr * PicHandle;

struct Polygon {
	short               polySize;
	Rect                polyBBox;
	Point               polyPoints[1];
};
typedef struct Polygon Polygon;
typedef Polygon * PolyPtr;
typedef PolyPtr * PolyHandle;

typedef pascal void (*QDTextProcPtr)(short byteCount,
	const void *textBuf, Point numer, Point denom);
typedef pascal void (*QDLineProcPtr)(Point newPt);
typedef pascal void (*QDRectProcPtr)(GrafVerb verb, const Rect *r);
typedef pascal void (*QDRRectProcPtr)(GrafVerb verb, const Rect *r,
	short ovalWidth, short ovalHeight);
typedef pascal void (*QDOvalProcPtr)(GrafVerb verb, const Rect *r);
typedef pascal void (*QDArcProcPtr)(GrafVerb verb, const Rect *r,
	short startAngle, short arcAngle);
typedef pascal void (*QDPolyProcPtr)(GrafVerb verb, PolyHandle poly);
typedef pascal void (*QDRgnProcPtr)(GrafVerb verb, RgnHandle rgn);
typedef pascal void (*QDBitsProcPtr)(const BitMap *srcBits,
	const Rect *srcRect, const Rect *dstRect, short mode,
	RgnHandle maskRgn);
typedef pascal void (*QDCommentProcPtr)(short kind, short dataSize,
	Handle dataHandle);
typedef pascal short (*QDTxMeasProcPtr)(short byteCount,
	const void *textAddr, Point *numer, Point *denom, FontInfo *info);
typedef pascal void (*QDGetPicProcPtr)(void *dataPtr, short byteCount);
typedef pascal void (*QDPutPicProcPtr)(const void *dataPtr,
	short byteCount);
typedef pascal void (*QDOpcodeProcPtr)(const Rect *fromRect,
	const Rect *toRect, UInt16 opcode, SInt16 version);
typedef OSStatus (*QDStdGlyphsProcPtr)(void *dataStream,
	ByteCount size);
typedef pascal void (*QDJShieldCursorProcPtr)(short left, short top,
	short right, short bottom);

#define QDTextUPP QDTextProcPtr
#define QDLineUPP QDLineProcPtr
#define QDRectUPP QDRectProcPtr
#define QDRRectUPP QDRRectProcPtr
#define QDOvalUPP QDOvalProcPtr
#define QDArcUPP QDArcProcPtr
#define QDPolyUPP QDPolyProcPtr
#define QDRgnUPP QDRgnProcPtr
#define QDBitsUPP QDBitsProcPtr
#define QDCommentUPP QDCommentProcPtr
#define QDTxMeasUPP QDTxMeasProcPtr
#define QDGetPicUPP QDGetPicProcPtr
#define QDPutPicUPP QDPutPicProcPtr
#define QDOpcodeUPP QDOpcodeProcPtr
#define QDStdGlyphsUPP QDStdGlyphsProcPtr
#define QDJShieldCursorUPP QDJShieldCursorProcPtr

struct QDProcs {
	QDTextUPP           textProc;
	QDLineUPP           lineProc;
	QDRectUPP           rectProc;
	QDRRectUPP          rRectProc;
	QDOvalUPP           ovalProc;
	QDArcUPP            arcProc;
	QDPolyUPP           polyProc;
	QDRgnUPP            rgnProc;
	QDBitsUPP           bitsProc;
	QDCommentUPP        commentProc;
	QDTxMeasUPP         txMeasProc;
	QDGetPicUPP         getPicProc;
	QDPutPicUPP         putPicProc;
};
typedef struct QDProcs QDProcs;
typedef QDProcs * QDProcsPtr;

struct GrafPort {
	short               device;
	BitMap              portBits;
	Rect                portRect;
	RgnHandle           visRgn;
	RgnHandle           clipRgn;
	Pattern             bkPat;
	Pattern             fillPat;
	Point               pnLoc;
	Point               pnSize;
	short               pnMode;
	Pattern             pnPat;
	short               pnVis;
	short               txFont;
	StyleField          txFace;
	short               txMode;
	short               txSize;
	Fixed               spExtra;
	long                fgColor;
	long                bkColor;
	short               colrBit;
	short               patStretch;
	Handle              picSave;
	Handle              rgnSave;
	Handle              polySave;
	QDProcsPtr          grafProcs;
};
typedef struct GrafPort                 GrafPort;
typedef GrafPort *                      GrafPtr;

typedef GrafPtr WindowPtr;
typedef WindowPtr DialogPtr;

#define WindowRef WindowPtr

struct QDGlobals {
	char                privates[76];
	long                randSeed;
	BitMap              screenBits;
	Cursor              arrow;
	Pattern             dkGray;
	Pattern             ltGray;
	Pattern             gray;
	Pattern             black;
	Pattern             white;
	GrafPtr             thePort;
};
typedef struct QDGlobals QDGlobals;
static QDGlobals qd;

pascal void
InitGraf(void * globalPtr)
	= {
		0xA86E
	};

pascal void
InitCursor(void)
	= {
		0xA850
	};

pascal void
SetPort(GrafPtr port)
	= {
		0xA873
	};

pascal void
GetPort(GrafPtr * port)
	= {
		0xA874
	};

pascal void
LocalToGlobal(Point * pt)
	= {
		0xA870
	};

pascal void
GlobalToLocal(Point * pt)
	= {
		0xA871
	};

pascal void
SetClip(RgnHandle rgn)
	= {
		0xA879
	};


pascal void
GetClip(RgnHandle rgn)
	= {
		0xA87A
	};

pascal void
ClipRect(const Rect * r)
	= {
		0xA87B
	};

pascal void
SetPt(
	Point *  pt,
	short    h,
	short    v)
	= {
		0xA880
	};

pascal void
SetRect(
	Rect *  r,
	short   left,
	short   top,
	short   right,
	short   bottom)
	= {
		0xA8A7
	};

pascal void
OffsetRect(
	Rect *  r,
	short   dh,
	short   dv)
	= {
		0xA8A8
	};

pascal void
InsetRect(
	Rect *  r,
	short   dh,
	short   dv)
	= {
		0xA8A9
	};

pascal Boolean
SectRect(
	const Rect *  src1,
	const Rect *  src2,
	Rect *        dstRect)
	= {
		0xA8AA
	};

pascal void
UnionRect(
	const Rect *  src1,
	const Rect *  src2,
	Rect *        dstRect)
	= {
		0xA8AB
	};

pascal Boolean
EqualRect(
	const Rect *  rect1,
	const Rect *  rect2)
	= {
		0xA8A6
	};

pascal Boolean
EmptyRect(const Rect * r)
	= {
		0xA8AE
	};

pascal RgnHandle
NewRgn(void)
	= {
		0xA8D8
	};

pascal void
DisposeRgn(RgnHandle rgn)
	= {
		0xA8D9
	};

pascal void
OpenRgn(void)
	= {
		0xA8DA
	};

pascal void
CloseRgn(RgnHandle dstRgn)
	= {
		0xA8DB
	};

pascal void
CopyRgn(
	RgnHandle   srcRgn,
	RgnHandle   dstRgn)
	= {
		0xA8DC
	};

pascal void
SetEmptyRgn(RgnHandle rgn)
	= {
		0xA8DD
	};

pascal void
SetRectRgn(
	RgnHandle   rgn,
	short       left,
	short       top,
	short       right,
	short       bottom)
	= {
		0xA8DE
	};

pascal void
RectRgn(
	RgnHandle     rgn,
	const Rect *  r)
	= {
		0xA8DF
	};

pascal void
OffsetRgn(
	RgnHandle   rgn,
	short       dh,
	short       dv)
	= {
		0xA8E0
	};

pascal void
InsetRgn(
	RgnHandle   rgn,
	short       dh,
	short       dv)
	= {
		0xA8E1
	};

pascal void
SectRgn(
	RgnHandle   srcRgnA,
	RgnHandle   srcRgnB,
	RgnHandle   dstRgn)
	= {
		0xA8E4
	};

pascal void
UnionRgn(
	RgnHandle   srcRgnA,
	RgnHandle   srcRgnB,
	RgnHandle   dstRgn)
	= {
		0xA8E5
	};

pascal void
DiffRgn(
	RgnHandle   srcRgnA,
	RgnHandle   srcRgnB,
	RgnHandle   dstRgn)
	= {
		0xA8E6
	};

pascal void
XorRgn(
	RgnHandle   srcRgnA,
	RgnHandle   srcRgnB,
	RgnHandle   dstRgn)
	= {
		0xA8E7
	};

pascal Boolean
RectInRgn(
	const Rect *  r,
	RgnHandle     rgn)
	= {
		0xA8E9
	};

pascal Boolean
EqualRgn(
	RgnHandle   rgnA,
	RgnHandle   rgnB)
	= {
		0xA8E3
	};

pascal Boolean
EmptyRgn(RgnHandle rgn)
	= {
		0xA8E2
	};

pascal Boolean
PtInRect(
	Point         pt,
	const Rect *  r)
	= {
		0xA8AD
	};

pascal Boolean
PtInRgn(
	Point       pt,
	RgnHandle   rgn)
	= {
		0xA8E8
	};

pascal void
MoveTo(
	short   h,
	short   v)
	= {
		0xA893
	};

pascal void
FrameRect(const Rect * r)
	= {
		0xA8A1
	};

pascal void
PaintRect(const Rect * r)
	= {
		0xA8A2
	};

pascal void
EraseRect(const Rect * r)
	= {
		0xA8A3
	};

pascal void
InvertRect(const Rect * r)
	= {
		0xA8A4
	};

pascal void
FillRect(
	const Rect *     r,
	const Pattern *  pat)
	= {
		0xA8A5
	};

pascal void
FrameRgn(RgnHandle rgn)
	= {
		0xA8D2
	};

pascal void
PaintRgn(RgnHandle rgn)
	= {
		0xA8D3
	};

pascal void
EraseRgn(RgnHandle rgn)
	= {
		0xA8D4
	};

pascal void
InvertRgn(RgnHandle rgn)
	= {
		0xA8D5
	};

pascal void
FillRgn(
	RgnHandle        rgn,
	const Pattern *  pat)
	= {
		0xA8D6
	};

#define GetRegionBounds(region, bounds) *(bounds) = \
	(**(region)).rgnBBox
#define GetPortBounds(port, r) *(r) = ((port)->portRect)

pascal void
HideCursor(void)
	= {
		0xA852
	};

pascal void
ShowCursor(void)
	= {
		0xA853
	};

pascal void
SetCursor(const Cursor * crsr)
	= {
		0xA851
	};


enum {
	iBeamCursor                   = 1,
	crossCursor                   = 2,
	plusCursor                    = 3,
	watchCursor                   = 4
};

pascal CursHandle
GetCursor(short cursorID)
	= {
		0xA9B9
	};

/* From MacWindows.h */

pascal void
InitWindows(void)
	= {
		0xA912
	};

pascal void
DisposeWindow(WindowRef window)
	= {
		0xA914
	};

pascal void
CloseWindow(WindowRef window)
	= {
		0xA92D
	};

enum {
	documentProc                  = 0,
	dBoxProc                      = 1,
	plainDBox                     = 2,
	altDBoxProc                   = 3,
	noGrowDocProc                 = 4,
	movableDBoxProc               = 5,
	zoomDocProc                   = 8,
	zoomNoGrow                    = 12
};

pascal WindowRef
NewWindow(
	void *             wStorage,
	const Rect *       boundsRect,
	ConstStr255Param   title,
	Boolean            visible,
	short              theProc,
	WindowRef          behind,
	Boolean            goAwayFlag,
	long               refCon)
	= {
		0xA913
	};

pascal void
ShowWindow(WindowRef window)
	= {
		0xA915
	};

pascal void
InvalRect(const Rect * badRect)
	= {
		0xA928
	};

pascal void
InvalRgn(RgnHandle badRgn)
	= {
		0xA927
	};

pascal void
BeginUpdate(WindowRef window)
	= {
		0xA922
	};

pascal void
EndUpdate(WindowRef window)
	= {
		0xA923
	};

pascal void
DrawGrowIcon(WindowRef window)
	= {
		0xA904
	};

pascal void
SizeWindow(
	WindowRef   window,
	short       w,
	short       h,
	Boolean     fUpdate)
	= {
		0xA91D
	};

pascal long
GrowWindow(
	WindowRef     window,
	Point         startPt,
	const Rect *  bBox)
	= {
		0xA92B
	};

pascal void
SelectWindow(WindowRef window)
	= {
		0xA91F
	};

typedef SInt16 WindowPartCode;
enum {
	inDesk                        = 0,
	inMenuBar                     = 1,
	inSysWindow                   = 2,
	inContent                     = 3,
	inDrag                        = 4,
	inGrow                        = 5,
	inGoAway                      = 6,
	inZoomIn                      = 7,
	inZoomOut                     = 8
};

pascal WindowPartCode
FindWindow(
	Point        thePoint,
	WindowRef *  window)
	= {
		0xA92C
	};

pascal WindowRef
FrontWindow(void)
	= {
		0xA924
	};

pascal void
DragWindow(
	WindowRef     window,
	Point         startPt,
	const Rect *  boundsRect)
	= {
		0xA925
	};

pascal void
ZoomWindow(
	WindowRef        window,
	WindowPartCode   partCode,
	Boolean          front)
	= {
		0xA83A
	};

pascal Boolean
TrackGoAway(
	WindowRef   window,
	Point       thePt)
	= {
		0xA91E
	};

pascal Boolean
TrackBox(
	WindowRef        window,
	Point            thePt,
	WindowPartCode   partCode)
	= {
		0xA83B
	};

typedef struct WindowRecord WindowRecord;
typedef WindowRecord * WindowPeek;
struct WindowRecord {
	GrafPort            port;
	short               windowKind;
	Boolean             visible;
	Boolean             hilited;
	Boolean             goAwayFlag;
	Boolean             spareFlag;
	RgnHandle           strucRgn;
	RgnHandle           contRgn;
	RgnHandle           updateRgn;
	Handle              windowDefProc;
	Handle              dataHandle;
	StringHandle        titleHandle;
	short               titleWidth;
	Handle              controlList;
	WindowPeek          nextWindow;
	PicHandle           windowPic;
	long                refCon;
};

#define GetGrayRgn() (*(RgnHandle *)(0x9EE))
#define GetWindowPort(window) (window)
#define SetPortWindowPort(w) SetPort(w)
#define GetWindowKind(window) (((WindowPeek)(window))->windowKind)
#define GetWRefCon(window) (((WindowPeek)(window))->refCon)

pascal void
PaintBehind(
	WindowRef   startWindow,
	RgnHandle   clobberedRgn)
	= {
		0xA90D
	};

pascal void
CalcVisBehind(
	WindowRef   startWindow,
	RgnHandle   clobberedRgn)
	= {
		0xA90A
	};

/* from Events.h */

typedef UInt16 EventKind;
typedef UInt16 EventMask;

enum {
	nullEvent                     = 0,
	mouseDown                     = 1,
	mouseUp                       = 2,
	keyDown                       = 3,
	keyUp                         = 4,
	autoKey                       = 5,
	updateEvt                     = 6,
	diskEvt                       = 7,
	activateEvt                   = 8,
	osEvt                         = 15,
	kHighLevelEvent               = 23
};

enum {
	mDownMask                     = 1 << mouseDown,
	mUpMask                       = 1 << mouseUp,
	keyDownMask                   = 1 << keyDown,
	keyUpMask                     = 1 << keyUp,
	autoKeyMask                   = 1 << autoKey,
	updateMask                    = 1 << updateEvt,
	diskMask                      = 1 << diskEvt,
	activMask                     = 1 << activateEvt,
	highLevelEventMask            = 0x0400,
	osMask                        = 1 << osEvt,
	everyEvent                    = 0xFFFF
};

enum {
	charCodeMask                  = 0x000000FF
};

typedef UInt16 EventModifiers;

enum {
	activeFlagBit                 = 0,
	btnStateBit                   = 7,
	cmdKeyBit                     = 8,
	shiftKeyBit                   = 9,
	alphaLockBit                  = 10,
	optionKeyBit                  = 11,
	controlKeyBit                 = 12
};

enum {
	activeFlag                    = 1 << activeFlagBit,
	btnState                      = 1 << btnStateBit,
	cmdKey                        = 1 << cmdKeyBit,
	shiftKey                      = 1 << shiftKeyBit,
	alphaLock                     = 1 << alphaLockBit,
	optionKey                     = 1 << optionKeyBit,
	controlKey                    = 1 << controlKeyBit
};

struct EventRecord {
	EventKind           what;
	UInt32              message;
	UInt32              when;
	Point               where;
	EventModifiers      modifiers;
};
typedef struct EventRecord EventRecord;

struct EvQEl {
	QElemPtr            qLink;
	SInt16              qType;
	EventKind           evtQWhat;
	UInt32              evtQMessage;
	UInt32              evtQWhen;
	Point               evtQWhere;
	EventModifiers      evtQModifiers;
};
typedef struct EvQEl EvQEl;
typedef EvQEl * EvQElPtr;

pascal QHdrPtr
GetEvQHdr(void)
	= {
		0x2EBC, 0x0000, 0x014A
	};

pascal void
GetMouse(Point * mouseLoc)
	= {
		0xA972
	};

pascal Boolean
Button(void)
	= {
		0xA974
	};

pascal Boolean
StillDown(void)
	= {
		0xA973
	};

typedef UInt32 KeyMap[4];

pascal void
GetKeys(KeyMap theKeys)
	= {
		0xA976
	};

pascal UInt32
GetCaretTime(void)
	= {
		0x2EB8, 0x02F4
	};

pascal Boolean
GetNextEvent(
	EventMask      eventMask,
	EventRecord *  theEvent)
	= {
		0xA970
	};

pascal Boolean
WaitNextEvent(
	EventMask      eventMask,
	EventRecord *  theEvent,
	UInt32         sleep,
	RgnHandle      mouseRgn)
	= {
		0xA860
	};

pascal Boolean
EventAvail(
	EventMask      eventMask,
	EventRecord *  theEvent)
	= {
		0xA971
	};

pascal void
FlushEvents(
	EventMask   whichMask,
	EventMask   stopMask)
	= {
		0x201F, /* MOVE.L     (A7)+,D0 */
		0xA032
	};

pascal void
SystemClick(
	const EventRecord *  theEvent,
	WindowRef            theWindow)
	= {
		0xA9B3
	};

pascal void
SystemTask(void)
	= {
		0xA9B4
	};

/* From ControlDefinitions.h */

enum {
	pushButProc                   = 0,
	checkBoxProc                  = 1,
	radioButProc                  = 2,
	scrollBarProc                 = 16
};

enum {
	kControlButtonPart            = 10,
	kControlUpButtonPart          = 20,
	kControlDownButtonPart        = 21,
	kControlPageUpPart            = 22,
	kControlPageDownPart          = 23
};

/* From Controls.h */

typedef SInt16 ControlPartCode;

typedef struct ControlRecord ControlRecord;
typedef ControlRecord * ControlPtr;
typedef ControlPtr * ControlRef;

typedef pascal void (* ControlActionProcPtr)(ControlRef theControl,
	ControlPartCode partCode);
typedef ControlActionProcPtr ControlActionUPP;

struct ControlRecord {
	ControlRef          nextControl;
	WindowRef           contrlOwner;
	Rect                contrlRect;
	UInt8               contrlVis;
	UInt8               contrlHilite;
	SInt16              contrlValue;
	SInt16              contrlMin;
	SInt16              contrlMax;
	Handle              contrlDefProc;
	Handle              contrlData;
	ControlActionUPP    contrlAction;
	SInt32              contrlRfCon;
	Str255              contrlTitle;
};

pascal ControlRef
NewControl(
	WindowRef          owningWindow,
	const Rect *       boundsRect,
	ConstStr255Param   controlTitle,
	Boolean            initiallyVisible,
	SInt16             initialValue,
	SInt16             minimumValue,
	SInt16             maximumValue,
	SInt16             procID,
	SInt32             controlReference)
	= {
		0xA954
	};

pascal void
DisposeControl(ControlRef theControl)
	= {
		0xA955
	};

#define GetControlBounds(control, r) *(r) = (**(control)).contrlRect

pascal SInt16
GetControlMaximum(ControlRef theControl)
	= {
		0xA962
	};

pascal void
SetControlMaximum(
	ControlRef   theControl,
	SInt16       newMaximum)
	= {
		0xA965
	};

pascal SInt16
GetControlValue(ControlRef theControl)
	= {
		0xA960
	};

pascal void
SetControlValue(
	ControlRef   theControl,
	SInt16       newValue)
	= {
		0xA963
	};

enum {
	kControlNoPart                = 0,
	kControlIndicatorPart         = 129,
	kControlDisabledPart          = 254,
	kControlInactivePart          = 255
};

pascal void
ShowControl(ControlRef theControl)
	= {
		0xA957
	};

pascal void
HideControl(ControlRef theControl)
	= {
		0xA958
	};

pascal void
SizeControl(
	ControlRef   theControl,
	SInt16       w,
	SInt16       h)
	= {
		0xA95C
	};

pascal void
MoveControl(
	ControlRef   theControl,
	SInt16       h,
	SInt16       v)
	= {
		0xA959
	};

pascal void
DrawControls(WindowRef theWindow)
	= {
		0xA969
	};

pascal ControlPartCode
TrackControl(
	ControlRef         theControl,
	Point              startPoint,
	ControlActionUPP   actionProc)
	= {
		0xA968
	};

pascal ControlPartCode
FindControl(
	Point         testPoint,
	WindowRef     theWindow,
	ControlRef *  theControl)
	= {
		0xA96C
	};

pascal void
HiliteControl(
	ControlRef theControl,
	ControlPartCode hiliteState)
	= {
		0xA95D
	};

/* From Menus.h */

typedef SInt16 MenuID;

struct MenuInfo {
	MenuID              menuID;
	short               menuWidth;
	short               menuHeight;
	Handle              menuProc;
	long                enableFlags;
	Str255              menuData;
};
typedef struct MenuInfo MenuInfo;
typedef MenuInfo * MenuPtr;
typedef MenuPtr * MenuHandle;

typedef MenuHandle MenuRef;
typedef Handle MenuBarHandle;

pascal void
InitMenus(void)
	= {
		0xA930
	};

pascal void
DrawMenuBar(void)
	= {
		0xA937
	};

pascal MenuRef
NewMenu(
	MenuID             menuID,
	ConstStr255Param   menuTitle)
	= {
		0xA931
	};

pascal MenuBarHandle
GetNewMBar(short menuBarID)
	= {
		0xA9C0
	};

pascal void
SetMenuBar(MenuBarHandle mbar)
	= {
		0xA93C
	};

pascal MenuRef
GetMenuHandle(MenuID menuID)
	= {
		0xA949
	};

pascal void
GetMenuItemText(
	MenuRef   theMenu,
	short     item,
	Str255    itemString)
	= {
		0xA946
	};

pascal void
AppendMenu(
	MenuRef            menu,
	ConstStr255Param   data)
	= {
		0xA933
	};

pascal void
AppendResMenu(
	MenuRef   theMenu,
	ResType   theType)
	= {
		0xA94D
	};

pascal void
InsertMenu(
	MenuRef   theMenu,
	MenuID    beforeID)
	= {
		0xA935
	};

pascal long
MenuKey(CharParameter ch)
	= {
		0xA93E
	};

pascal long
MenuSelect(Point startPt)
	= {
		0xA93D
	};

pascal void
HiliteMenu(MenuID menuID)
	= {
		0xA938
	};

pascal Boolean
SystemEdit(short editCmd)
	= {
		0xA9C2
	};

pascal void
DisableItem(
	MenuRef   theMenu,
	short     item)
	= {
		0xA93A
	};

pascal void
EnableItem(
	MenuRef   theMenu,
	short     item)
	= {
		0xA939
	};

#define DisableMenuItem DisableItem
#define EnableMenuItem EnableItem

/* Scrap.h */

typedef FourCharCode ScrapFlavorType;

pascal long
GetScrap(
	Handle            destination,
	ScrapFlavorType   flavorType,
	SInt32 *          offset)
	= {
		0xA9FD
	};

pascal OSStatus
ZeroScrap(void)
	= {
		0xA9FC
	};

pascal OSStatus
PutScrap(
	SInt32            sourceBufferByteCount,
	ScrapFlavorType   flavorType,
	const void *      sourceBuffer)
	= {
		0xA9FE
	};

/* From TextEdit.h */

typedef ProcPtr WordBreakUPP;
typedef ProcPtr TEClickLoopUPP;
typedef ProcPtr HighHookUPP;
typedef ProcPtr CaretHookUPP;

struct TERec {
	Rect                destRect;
	Rect                viewRect;
	Rect                selRect;
	short               lineHeight;
	short               fontAscent;
	Point               selPoint;
	short               selStart;
	short               selEnd;
	short               active;
	WordBreakUPP        wordBreak;
	TEClickLoopUPP      clickLoop;
	long                clickTime;
	short               clickLoc;
	long                caretTime;
	short               caretState;
	short               just;
	short               teLength;
	Handle              hText;
	long                hDispatchRec;
	short               clikStuff;
	short               crOnly;
	short               txFont;
	StyleField          txFace;
	short               txMode;
	short               txSize;
	GrafPtr             inPort;
	HighHookUPP         highHook;
	CaretHookUPP        caretHook;
	short               nLines;
	short               lineStarts[16001];
};
typedef struct TERec TERec;
typedef TERec * TEPtr;
typedef TEPtr * TEHandle;

typedef char Chars[32001];
typedef char * CharsPtr;
typedef CharsPtr * CharsHandle;

pascal void
TEInit(void)
	= {
		0xA9CC
	};

pascal TEHandle
TENew(
	const Rect *  destRect,
	const Rect *  viewRect)
	= {
		0xA9D2
	};

pascal void
TEDispose(TEHandle hTE)
	= {
		0xA9CD
	};

pascal void
TEUpdate(
	const Rect *  rUpdate,
	TEHandle      hTE)
	= {
		0xA9D3
	};

pascal void
TEScroll(
	short      dh,
	short      dv,
	TEHandle   hTE)
	= {
		0xA9DD
	};

pascal void
TEAutoView(
	Boolean    fAuto,
	TEHandle   hTE)
	= {
		0xA813
	};

pascal void
TECalText(TEHandle hTE)
	= {
		0xA9D0
	};

pascal void
TEClick(
	Point      pt,
	Boolean    fExtend,
	TEHandle   h)
	= {
		0xA9D4
	};

pascal CharsHandle
TEGetText(TEHandle hTE)
	= {
		0xA9CB
	};

pascal void
TEIdle(TEHandle hTE)
	= {
		0xA9DA
	};

pascal void
TESetSelect(
	long       selStart,
	long       selEnd,
	TEHandle   hTE)
	= {
		0xA9D1
	};

pascal void
TEActivate(TEHandle hTE)
	= {
		0xA9D8
	};

pascal void
TEDeactivate(TEHandle hTE)
	= {
		0xA9D9
	};

pascal void
TEKey(
	CharParameter   key,
	TEHandle        hTE)
	= {
		0xA9DC
	};

pascal void
TECut(TEHandle hTE)
	= {
		0xA9D6
	};

pascal void
TECopy(TEHandle hTE)
	= {
		0xA9D5
	};

pascal void
TEPaste(TEHandle hTE)
	= {
		0xA9DB
	};

pascal void
TEDelete(TEHandle hTE)
	= {
		0xA9D7
	};

pascal void
TEInsert(
	const void *  text,
	long          length,
	TEHandle      hTE)
	= {
		0xA9DE
	};

#define LMGetTEScrpLength() (* (UInt16 *) 0x0AB0)
#define LMSetTEScrpLength(v) (* (UInt16 *) 0x0AB0) = (v);

#define LMGetTEScrpHandle() (*(Handle *)(0x0AB4))
#define LMSetTEScrpHandle(v) (*(Handle *)(0x0AB4)) = (v);

#define TEGetScrapLength() ((long) LMGetTEScrpLength())

static pascal OSErr
TEFromScrap(void)
{
	long L;
	SInt32 offset;

	L = GetScrap(NULL,
		FOUR_CHAR_CODE('TEXT'),
		&offset);
	if (L < 0) {
		LMSetTEScrpLength(0);
		return L;
	} else if (L > 0x00007D01) {
		LMSetTEScrpLength(0);
		return (OSErr)0xFE0B;
	} else {
		L = GetScrap(LMGetTEScrpHandle(),
			FOUR_CHAR_CODE('TEXT'),
			&offset);
		if (L < 0) {
			LMSetTEScrpLength(0);
			return L;
		} else {
			LMSetTEScrpLength(L);
			return 0;
		}
	}
}

static pascal OSErr
TEToScrap(void)
{
	OSErr v;
	Handle h = LMGetTEScrpHandle();

	HLock(h);

	v = PutScrap(
			TEGetScrapLength(),
			FOUR_CHAR_CODE('TEXT'),
			*h);

	HUnlock(h);

	return v;
}

/* From Dialogs.h */

typedef DialogPtr DialogRef;

typedef SInt16 DialogItemIndex;
typedef SInt16 DialogItemType;

typedef pascal Boolean (* ModalFilterProcPtr)(DialogRef theDialog,
	EventRecord *theEvent, DialogItemIndex *itemHit);

typedef ModalFilterProcPtr ModalFilterUPP;

pascal void
InitDialogs(void * ignored)
	= {
		0xA97B
	};

pascal void
ParamText(
	ConstStr255Param   param0,
	ConstStr255Param   param1,
	ConstStr255Param   param2,
	ConstStr255Param   param3)
	= {
		0xA98B
	};

pascal void
ResetAlertStage(void)
	= {
		0x4278, 0x0A9A
	};

pascal DialogItemIndex
Alert(
	SInt16           alertID,
	ModalFilterUPP   modalFilter)
	= {
		0xA985
	};

pascal DialogItemIndex
StopAlert(
	SInt16           alertID,
	ModalFilterUPP   modalFilter)
	= {
		0xA986
	};

pascal DialogItemIndex
NoteAlert(
	SInt16           alertID,
	ModalFilterUPP   modalFilter)
	= {
		0xA987
	};

pascal DialogItemIndex
CautionAlert(
	SInt16           alertID,
	ModalFilterUPP   modalFilter)
	= {
		0xA988
	};

pascal void
GetDialogItem(
	DialogRef theDialog,
	DialogItemIndex itemNo,
	DialogItemType * itemType,
	Handle * item,
	Rect * box)
	= {
		0xA98D
	};

/* From Aliases.h */

struct AliasRecord {
	OSType              userType;
	unsigned short      aliasSize;
};
typedef struct AliasRecord              AliasRecord;
typedef AliasRecord *                   AliasPtr;
typedef AliasPtr *                      AliasHandle;

pascal OSErr NewAliasMinimalFromFullPath(
	short             fullPathLength,
	const void *      fullPath,
	ConstStr32Param   zoneName,
	ConstStr31Param   serverName,
	AliasHandle *     alias)
	= {
		0x7009, 0xA823
	};

pascal OSErr NewAliasMinimal(
	const FSSpec * target,
	AliasHandle * alias)
	= {
		0x7008, 0xA823
	};


pascal OSErr ResolveAlias(
	const FSSpec *  fromFile,
	AliasHandle     alias,
	FSSpec *        target,
	Boolean *       wasChanged)
	= {
		0x7003, 0xA823
	};

pascal OSErr UpdateAlias(
	const FSSpec *  fromFile,
	const FSSpec *  target,
	AliasHandle     alias,
	Boolean *       wasChanged)
	= {
		0x7006, 0xA823
	};

typedef pascal Boolean (*AliasFilterProcPtr)(CInfoPBPtr cpbPtr,
	Boolean *quitFlag, Ptr myDataPtr);

enum {
	rAliasType = 'alis'
};

enum {
	kARMMountVol                  = 0x00000001,
	kARMNoUI                      = 0x00000002,
	kARMMultVols                  = 0x00000008,
	kARMSearch                    = 0x00000100,
	kARMSearchMore                = 0x00000200,
	kARMSearchRelFirst            = 0x00000400
};

pascal OSErr MatchAlias(
	const FSSpec *   fromFile,
	unsigned long    rulesMask,
	AliasHandle      alias,
	short *          aliasCount,
	FSSpecArrayPtr   aliasList,
	Boolean *        needsUpdate,
	AliasFilterProcPtr aliasFilter,
	void *           yourDataPtr)
	= {
		0x7005, 0xA823
	};

pascal OSErr ResolveAliasFile(
	FSSpec *   theSpec,
	Boolean    resolveAliasChains,
	Boolean *  targetIsFolder,
	Boolean *  wasAliased)
	= {
		0x700C, 0xA823
	};

/* ToolUtils.h */

#define HiWord(x) ((short)((long)(x) >> 16))
#define LoWord(x) ((short)(x))

/* from LowMem.h */

#define LMGetROM85() (*(si4b *)(0x028E))
#define LMGetCurApName() ((StringPtr) 0x0910)
#define LMGetApplLimit() ((Ptr)*((si5p) 0x130))
#define LMGetTicks() (*(volatile ui5b *) 0x016A)
#define LMGetMBarHeight() (*(si4b *)(0x0BAA))
#define LMSetMBarHeight(v) (*(si4b *)(0x0BAA)) = (v)
#define LMGetHeapEnd() (*(Ptr *)(0x0114))
#define LMSetHeapEnd(v) (*(Ptr *)(0x0114)) = (v)

#define LMGetMouseButtonState() (*(volatile UInt8 *)(0x0172))

#define LMGetWindowList() (*(WindowRef *)(0x09D6))

#define MyGetMBarHeight() (Have64kROM() ? 20 : LMGetMBarHeight())

#define LMGetSFSaveDisk() (*(SInt16 *)(0x0214))
#define LMSetSFSaveDisk(v) (*(SInt16 *)(0x0214)) = (v)

#define LMGetCurDirStore() (*(SInt32 *)(0x0398))
#define LMSetCurDirStore(v) (*(SInt32 *)(0x0398)) = (v)

#if IsAnApp

/* From StandardFile.h */

struct SFReply {
	Boolean             good;
	Boolean             copy;
	OSType              fType;
	short               vRefNum;
	short               version;
	StrFileName         fName;
};
typedef struct SFReply SFReply;

struct StandardFileReply {
	Boolean             sfGood;
	Boolean             sfReplacing;
	OSType              sfType;
	FSSpec              sfFile;
	ScriptCode          sfScript;
	short               sfFlags;
	Boolean             sfIsFolder;
	Boolean             sfIsVolume;
	long                sfReserved1;
	short               sfReserved2;
};
typedef struct StandardFileReply StandardFileReply;

typedef OSType SFTypeList[4];
typedef const OSType *                  ConstSFTypeListPtr;

typedef pascal short (*DlgHookProcPtr)(short item, DialogRef theDialog);

typedef DlgHookProcPtr DlgHookUPP;

typedef pascal Boolean (* FileFilterProcPtr)(CInfoPBPtr pb);

typedef FileFilterProcPtr FileFilterUPP;

pascal void
	SFPutFile(
		Point              where,
		ConstStr255Param   prompt,
		ConstStr255Param   origName,
		DlgHookUPP         dlgHook,
		SFReply *          reply)
	= {
		0x3F3C, 0x0001, 0xA9EA
	};


pascal void
	SFGetFile(
		Point                where,
		ConstStr255Param     prompt,
		FileFilterUPP        fileFilter,
		short                numTypes,
		ConstSFTypeListPtr   typeList,
		DlgHookUPP           dlgHook,
		SFReply *            reply)
	= {
		0x3F3C, 0x0002, 0xA9EA
	};


enum {
	putSave = 1,
	putCancel = 2,
	putEject = 5,
	putDrive = 6,
	putName = 7,
	getOpen = 1,
	getCancel = 3,
	getEject = 5,
	getDrive = 6,
	getNmList = 7,
	getScroll = 8
};

pascal void
	SFPPutFile(
		Point              where,
		ConstStr255Param   prompt,
		ConstStr255Param   origName,
		DlgHookUPP         dlgHook,
		SFReply *          reply,
		short              dlgID,
		ModalFilterUPP     filterProc)
	= {
		0x3F3C, 0x0003, 0xA9EA
	};

pascal void
	SFPGetFile(
		Point                where,
		ConstStr255Param     prompt,
		FileFilterUPP        fileFilter,
		short                numTypes,
		ConstSFTypeListPtr   typeList,
		DlgHookUPP           dlgHook,
		SFReply *            reply,
		short                dlgID,
		ModalFilterUPP       filterProc)
	= {
		0x3F3C, 0x0004, 0xA9EA
	};

pascal void StandardPutFile(ConstStr255Param prompt,
	ConstStr255Param defaultName,
	StandardFileReply *reply)
	= {
		0x3F3C, 0x0005, 0xA9EA
	};

pascal void StandardGetFile(FileFilterUPP fileFilter,
	short numTypes,
	ConstSFTypeListPtr typeList,
	StandardFileReply *reply)
	= {
		0x3F3C, 0x0006, 0xA9EA
	};


enum {
	sfItemOpenButton = 1,
	sfItemCancelButton = 2,
	sfItemBalloonHelp = 3,
	sfItemVolumeUser = 4,
	sfItemEjectButton = 5,
	sfItemDesktopButton = 6,
	sfItemFileListUser = 7,
	sfItemPopUpMenuUser = 8,
	sfItemDividerLinePict = 9,
	sfItemFileNameTextEdit = 10,
	sfItemPromptStaticText = 11,
	sfItemNewFolderUser = 12
};

#define sfMainDialogRefCon 'stdf'
#define sfNewFolderDialogRefCon 'nfdr'
#define sfReplaceDialogRefCon 'rplc'
#define sfStatWarnDialogRefCon 'stat'
#define sfErrorDialogRefCon 'err '
#define sfLockWarnDialogRefCon 'lock'

typedef pascal Boolean (* FileFilterYDProcPtr)(CInfoPBPtr pb,
	void *yourDataPtr);
typedef FileFilterYDProcPtr FileFilterYDUPP;

typedef pascal short (* DlgHookYDProcPtr)(short item,
	DialogRef theDialog, void *yourDataPtr);
typedef DlgHookYDProcPtr DlgHookYDUPP;

typedef pascal Boolean (* ModalFilterYDProcPtr)(DialogRef theDialog,
	EventRecord *theEvent, short *itemHit, void *yourDataPtr);
typedef ModalFilterYDProcPtr ModalFilterYDUPP;

typedef const short * ActivationOrderListPtr;

typedef pascal void (* ActivateYDProcPtr)(DialogRef theDialog,
	short itemNo, Boolean activating, void *yourDataPtr);
typedef ActivateYDProcPtr ActivateYDUPP;

pascal void CustomPutFile(
	ConstStr255Param prompt,
	ConstStr255Param defaultName,
	StandardFileReply *reply,
	short dlgID,
	Point where,
	DlgHookYDUPP dlgHook,
	ModalFilterYDUPP filterProc,
	ActivationOrderListPtr activeList,
	ActivateYDUPP activate,
	void * yourDataPtr)
	= {
		0x3F3C, 0x0007, 0xA9EA
	};

pascal void
CustomGetFile(
	FileFilterYDUPP fileFilter,
	short numTypes,
	ConstSFTypeListPtr typeList,
	StandardFileReply *reply,
	short dlgID,
	Point where,
	DlgHookYDUPP dlgHook,
	ModalFilterYDUPP filterProc,
	ActivationOrderListPtr activeList,
	ActivateYDUPP activate,
	void * yourDataPtr)
	= {
		0x3F3C, 0x0008, 0xA9EA
	};

#endif

/* from NumberFormatting.h */

pascal void
StringToNum(
	ConstStr255Param   theString,
	long *             theNum)
	= {
		0x206F, 0x0004, /* MOVEA.L    $0004(A7),A0 */
		0x3F3C, 0x0001, /* MOVE.W     #$0001,-(A7) */
		0xA9EE, /* _Pack7 */
		0x2057, /* MOVEA.L    (A7),A0 */
		0x2080, /* MOVE.L     D0,(A0) */
		0x504F  /* ADDQ.W     #$8,A7 */
	};

pascal void
NumToString(
	long     theNum,
	Str255   theString)
	= {
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0x201F, /* MOVE.L     (A7)+,D0 */
		0x4267, /* CLR.W      -(A7) */
		0xA9EE  /* _Pack7 */
	};

/* from Devices.h */

pascal short
OpenDeskAcc(ConstStr255Param deskAccName)
	= {
		0xA9B6
	};

pascal void
CloseDeskAcc(short refNum)
	= {
		0xA9B7
	};

/* from DiskInit.h */

pascal short
DIBadMount(
	Point    where,
	UInt32   evtMessage)
	= {
		0x7000, 0x3F00, 0xA9E9
	};

/* from AppleEvents.h */

#define typeAEList 'list'
#define typeAlias 'alis'
#define typeAppParameters 'appa'
#define typeFSS 'fss '
#define typeWildCard '****'
#define typeApplSignature 'sign'
#define typeNull 'null'
#define typeSInt32 'long'
#define typeLongInteger typeSInt32
#define typeProcessSerialNumber 'psn '

#define kCoreEventClass 'aevt'

#define kAEOpenApplication 'oapp'
#define kAEOpenDocuments 'odoc'
#define kAEPrintDocuments 'pdoc'
#define kAEQuitApplication 'quit'
#define kAEApplicationDied 'obit'

#define keyDirectObject '----'
#define keyErrorNumber 'errn'
#define keyMissedKeywordAttr 'miss'
#define keyProcessSerialNumber 'psn '

enum {
	errAEDescNotFound = -1701,
	errAENotAppleEvent = -1707,
	errAEEventNotHandled = -1708
};

typedef unsigned long AEKeyword;
typedef unsigned long AEEventClass;
typedef unsigned long AEEventID;
typedef ResType DescType;

struct AEDesc {
	DescType descriptorType;
	Handle dataHandle;
};

typedef struct AEDesc AEDesc;

typedef AEDesc AEDescList;
typedef AEDescList AERecord;
typedef AEDesc AEAddressDesc;
typedef AERecord AppleEvent;
typedef SInt16 AEReturnID;
typedef SInt32 AETransactionID;

enum {
	kAEInteractWithSelf,
	kAEInteractWithLocal,
	kAEInteractWithAll
};
typedef unsigned char AEInteractAllowed;

enum {
	kAutoGenerateReturnID = -1,
	kAnyTransactionID     = 0
};

typedef pascal OSErr (*AEEventHandlerProcPtr)(
	const AppleEvent *theAppleEvent, AppleEvent *reply,
	long handlerRefcon);

#define AEEventHandlerUPP AEEventHandlerProcPtr

pascal OSErr AECoerceDesc(
	const AEDesc *  theAEDesc,
	DescType        toType,
	AEDesc *        result)
	= {
		0x303C, 0x0603, 0xA816
	};

pascal OSErr AEGetAttributePtr(const AppleEvent *theAppleEvent,
	AEKeyword theAEKeyword,
	DescType desiredType,
	DescType *typeCode,
	Ptr dataPtr,
	Size maximumSize,
	Size *actualSize)
	= {
		0x303C, 0x0E15, 0xA816
	};

pascal OSErr AEPutParamDesc(
	AppleEvent *    theAppleEvent,
	AEKeyword       theAEKeyword,
	const AEDesc *  theAEDesc)
	= {
		0x303C, 0x0610, 0xA816
	};

pascal OSErr AEGetParamPtr(const AppleEvent *theAppleEvent,
	AEKeyword theAEKeyword,
	DescType desiredType,
	DescType *typeCode,
	Ptr dataPtr,
	Size maximumSize,
	Size *actualSize)
	= {
		0x303C, 0x0E11, 0xA816
	};

pascal OSErr AEGetParamDesc(const AppleEvent *theAppleEvent,
	AEKeyword theAEKeyword,
	DescType desiredType,
	AEDesc *result)
	= {
		0x303C, 0x0812, 0xA816
	};

pascal OSErr AEDisposeDesc(AEDesc *theAEDesc)
	= {
		0x303C, 0x0204, 0xA816
	};

pascal OSErr AECountItems(const AEDescList *theAEDescList,
	long *theCount)
	= {
		0x303C, 0x0407, 0xA816
	};

pascal OSErr AEPutDesc(
	AEDescList *    theAEDescList,
	long            index,
	const AEDesc *  theAEDesc)
	= {
		0x303C, 0x0609, 0xA816
	};

pascal OSErr AEGetNthPtr(const AEDescList *theAEDescList,
	long index,
	DescType desiredType,
	AEKeyword *theAEKeyword,
	DescType *typeCode,
	Ptr dataPtr,
	Size maximumSize,
	Size *actualSize)
	= {
		0x303C, 0x100A, 0xA816
	};

pascal OSErr AESetInteractionAllowed(AEInteractAllowed level)
	= {
		0x303C, 0x011E, 0xA816
	};

pascal OSErr AEInstallEventHandler(AEEventClass theAEEventClass,
	AEEventID theAEEventID,
	AEEventHandlerUPP handler,
	long handlerRefcon,
	Boolean isSysHandler)
	= {
		0x303C, 0x091F, 0xA816
	};

pascal OSErr AEProcessAppleEvent(const EventRecord *theEventRecord)
	= {
		0x303C, 0x021B, 0xA816
	};

pascal OSErr AECreateDesc(
	DescType      typeCode,
	const void *  dataPtr,
	Size          dataSize,
	AEDesc *      result)
	= {
		0x303C, 0x0825, 0xA816
	};

pascal OSErr AECreateList(
	const void *  factoringPtr,
	Size          factoredSize,
	Boolean       isRecord,
	AEDescList *  resultList)
	= {
		0x303C, 0x0706, 0xA816
	};

pascal OSErr AECreateAppleEvent(
	AEEventClass           theAEEventClass,
	AEEventID              theAEEventID,
	const AEAddressDesc *  target,
	AEReturnID             returnID,
	AETransactionID        transactionID,
	AppleEvent *           result)
	= {
		0x303C, 0x0B14, 0xA816
	};

typedef SInt32 AESendMode;
enum {
	kAENoReply                    = 0x00000001,
	kAEQueueReply                 = 0x00000002,
	kAEWaitReply                  = 0x00000003,
	kAEDontReconnect              = 0x00000080,
	kAEWantReceipt                = 0x00000200,
	kAENeverInteract              = 0x00000010,
	kAECanInteract                = 0x00000020,
	kAEAlwaysInteract             = 0x00000030,
	kAECanSwitchLayer             = 0x00000040,
	kAEDontRecord                 = 0x00001000,
	kAEDontExecute                = 0x00002000,
	kAEProcessNonReplyEvents      = 0x00008000
};

enum {
	kAEDefaultTimeout             = -1,
	kNoTimeOut                    = -2
};

typedef SInt16 AESendPriority;
enum {
	kAENormalPriority             = 0x00000000,
	kAEHighPriority               = 0x00000001
};

typedef pascal Boolean (*AEIdleProcPtr)(EventRecord *theEvent,
	long *sleepTime, RgnHandle *mouseRgn);
typedef pascal Boolean (*AEFilterProcPtr)(EventRecord *theEvent,
	long returnID, long transactionID, const AEAddressDesc *sender);

pascal OSErr AESend(
	const AppleEvent *  theAppleEvent,
	AppleEvent *        reply,
	AESendMode          sendMode,
	AESendPriority      sendPriority,
	long                timeOutInTicks,
	AEIdleProcPtr       idleProc,
	AEFilterProcPtr     filterProc)
	= {
		0x303C, 0x0D17, 0xA816
	};

/* from Drag.h */

typedef struct OpaqueDragRef* DragRef;
typedef UInt32 DragItemRef;
typedef OSType FlavorType;

typedef SInt16 DragTrackingMessage;
enum {
	kDragTrackingEnterHandler     = 1,
	kDragTrackingEnterWindow      = 2,
	kDragTrackingInWindow         = 3,
	kDragTrackingLeaveWindow      = 4,
	kDragTrackingLeaveHandler     = 5
};

enum {
	kDragFlavorTypeHFS  = FOUR_CHAR_CODE('hfs ')
};

struct HFSFlavor {
	OSType              fileType;
	OSType              fileCreator;
	UInt16              fdFlags;
	FSSpec              fileSpec;
};
typedef struct HFSFlavor                HFSFlavor;

typedef pascal OSErr (*DragTrackingHandlerProcPtr)(
	DragTrackingMessage message, WindowRef theWindow,
	void *handlerRefCon, DragRef theDrag);
typedef pascal OSErr (*DragReceiveHandlerProcPtr)(
	WindowRef theWindow, void *handlerRefCon, DragRef theDrag);

pascal OSErr InstallTrackingHandler(
	DragTrackingHandlerProcPtr   trackingHandler,
	WindowRef                theWindow,
	void *                   handlerRefCon)
	= {
		0x7001, 0xABED
	};

pascal OSErr InstallReceiveHandler(
	DragReceiveHandlerProcPtr   receiveHandler,
	WindowRef               theWindow,
	void *                  handlerRefCon)
	= {
		0x7002, 0xABED
	};

pascal OSErr RemoveTrackingHandler(
	DragTrackingHandlerProcPtr   trackingHandler,
	WindowRef                theWindow)
	= {
		0x7003, 0xABED
	};

pascal OSErr RemoveReceiveHandler(
	DragReceiveHandlerProcPtr   receiveHandler,
	WindowRef               theWindow)
	= {
		0x7004, 0xABED
	};

pascal OSErr ShowDragHilite(
	DragRef     theDrag,
	RgnHandle   hiliteFrame,
	Boolean     inside)
	= {
		0x701E, 0xABED
	};

pascal OSErr HideDragHilite(DragRef theDrag)
	= {
		0x701F, 0xABED
	};

pascal OSErr CountDragItems(
	DragRef   theDrag,
	UInt16 *  numItems)
	= {
		0x700E, 0xABED
	};

pascal OSErr GetDragItemReferenceNumber(
	DragRef        theDrag,
	UInt16         index,
	DragItemRef *  theItemRef)
	= {
		0x700F, 0xABED
	};

pascal OSErr GetFlavorDataSize(
	DragRef       theDrag,
	DragItemRef   theItemRef,
	FlavorType    theType,
	Size *        dataSize)
	= {
		0x7013, 0xABED
	};

pascal OSErr GetFlavorData(
	DragRef       theDrag,
	DragItemRef   theItemRef,
	FlavorType    theType,
	void *        dataPtr,
	Size *        dataSize,
	UInt32        dataOffset)
	= {
		0x7014, 0xABED
	};

/* from Processes.h */

struct ProcessSerialNumber {
	unsigned long       highLongOfPSN;
	unsigned long       lowLongOfPSN;
};
typedef struct ProcessSerialNumber ProcessSerialNumber;

struct ProcessInfoRec {
	unsigned long       processInfoLength;
	StringPtr           processName;
	ProcessSerialNumber processNumber;
	unsigned long       processType;
	OSType              processSignature;
	unsigned long       processMode;
	Ptr                 processLocation;
	unsigned long       processSize;
	unsigned long       processFreeMem;
	ProcessSerialNumber processLauncher;
	unsigned long       processLaunchDate;
	unsigned long       processActiveTime;
	FSSpecPtr           processAppSpec;
};
typedef struct ProcessInfoRec ProcessInfoRec;
typedef ProcessInfoRec * ProcessInfoRecPtr;

enum {
	kNoProcess                    = 0,
	kSystemProcess                = 1,
	kCurrentProcess               = 2
};

pascal OSErr GetNextProcess(ProcessSerialNumber * PSN)
	= {
		0x3F3C, 0x0038, 0xA88F
	};

pascal OSErr GetProcessInformation(
	const ProcessSerialNumber * PSN,
	ProcessInfoRec * info)
	= {
		0x3F3C, 0x003A, 0xA88F
	};

pascal OSErr SetFrontProcess(const ProcessSerialNumber * PSN)
	= {
		0x3F3C, 0x003B, 0xA88F
	};

pascal OSErr SameProcess(
	const ProcessSerialNumber * PSN1,
	const ProcessSerialNumber * PSN2,
	Boolean *result)
	= {
		0x3F3C, 0x003D, 0xA88F
	};

typedef unsigned short LaunchFlags;

enum {
	launchContinue = 0x4000,
	launchNoFileFlags = 0x0800,
	launchUseMinimum = 0x0400,
	launchDontSwitch = 0x0200,
	launchAllow24Bit = 0x0100,
	launchInhibitDaemon = 0x0080
};

struct AppParameters {
	EventRecord   theMsgEvent;
	unsigned long eventRefCon;
	unsigned long messageLength;
};
typedef struct AppParameters AppParameters;

typedef AppParameters * AppParametersPtr;
struct LaunchParamBlockRec {
	unsigned long       reserved1;
	unsigned short      reserved2;
	unsigned short      launchBlockID;
	unsigned long       launchEPBLength;
	unsigned short      launchFileFlags;
	LaunchFlags         launchControlFlags;
	FSSpecPtr           launchAppSpec;
	ProcessSerialNumber launchProcessSN;
	unsigned long       launchPreferredSize;
	unsigned long       launchMinimumSize;
	unsigned long       launchAvailableSize;
	AppParametersPtr    launchAppParameters;
};
typedef struct LaunchParamBlockRec LaunchParamBlockRec;
typedef LaunchParamBlockRec * LaunchPBPtr;

enum {
	extendedBlock = 0x4C43, /* 'LC' */
	extendedBlockLen = sizeof(LaunchParamBlockRec) - 12
};

pascal OSErr LaunchApplication(LaunchPBPtr LaunchParams)
	= {
		0x205F, /* MOVEA.L    (A7)+,A0 */
		0xA9F2, /* DC.W       $A9F2    ; _Launch */
		0x3E80  /* MOVE.W     D0,(A7) */
	};

pascal void
ExitToShell(void)
	= {
		0xA9F4
	};

/* folder manager */

enum {
	kOnSystemDisk      = -32768L,
	kOnAppropriateDisk = -32767,
	kSystemDomain      = -32766,
	kLocalDomain       = -32765,
	kNetworkDomain     = -32764,
	kUserDomain        = -32763,
	kClassicDomain     = -32762
};

enum {
	kCreateFolder     = true,
	kDontCreateFolder = false
};

enum {
	kSystemFolderType             = FOUR_CHAR_CODE('macs'),
	kDesktopFolderType            = FOUR_CHAR_CODE('desk'),
	kSystemDesktopFolderType      = FOUR_CHAR_CODE('sdsk'),
	kTrashFolderType              = FOUR_CHAR_CODE('trsh'),
	kSystemTrashFolderType        = FOUR_CHAR_CODE('strs'),
	kWhereToEmptyTrashFolderType  = FOUR_CHAR_CODE('empt'),
	kPrintMonitorDocsFolderType   = FOUR_CHAR_CODE('prnt'),
	kStartupFolderType            = FOUR_CHAR_CODE('strt'),
	kShutdownFolderType           = FOUR_CHAR_CODE('shdf'),
	kAppleMenuFolderType          = FOUR_CHAR_CODE('amnu'),
	kControlPanelFolderType       = FOUR_CHAR_CODE('ctrl'),
	kSystemControlPanelFolderType = FOUR_CHAR_CODE('sctl'),
	kExtensionFolderType          = FOUR_CHAR_CODE('extn'),
	kFontsFolderType              = FOUR_CHAR_CODE('font'),
	kPreferencesFolderType        = FOUR_CHAR_CODE('pref'),
	kSystemPreferencesFolderType  = FOUR_CHAR_CODE('sprf'),
	kTemporaryFolderType          = FOUR_CHAR_CODE('temp')
};

enum {
	gestaltFindFolderAttr = FOUR_CHAR_CODE('fold'),
	gestaltFindFolderPresent = 0,
	gestaltFolderDescSupport = 1,
	gestaltFolderMgrFollowsAliasesWhenResolving = 2,
	gestaltFolderMgrSupportsExtendedCalls = 3,
	gestaltFolderMgrSupportsDomains = 4,
	gestaltFolderMgrSupportsFSCalls = 5
};

pascal OSErr FindFolder(
	short vRefNum,
	OSType folderType,
	Boolean createFolder,
	short * foundVRefNum,
	long * foundDirID)
	= {
		0x7000, 0xA823
	};
