/*
	NEWMCAPI.h
	Copyright (C) 2007 Paul C. Pratt

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
	NEW MaCintosh Application Programming Interfaces

	APIs not included in headers from MPW 3
*/


/* from ConditionalMacros.h */

#define FOUR_CHAR_CODE(x)           (x)

/* From MacTypes.h */

typedef unsigned char                   Str32[33];

typedef Str63                           StrFileName;

typedef const unsigned char *           ConstStr255Param;
typedef const unsigned char *           ConstStr32Param;
typedef const unsigned char *           ConstStr31Param;

typedef signed char SInt8;
typedef signed short SInt16;
typedef signed long SInt32;

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned long UInt32;

#define WindowRef WindowPtr

/* Script.h */

enum {
	smSystemScript                = -1,
	smCurrentScript               = -2,
	smAllScripts                  = -3
};

/* from MacErrors.h */

enum {
	gestaltUnknownErr             = -5550,
	gestaltUndefSelectorErr       = -5551,
	gestaltDupSelectorErr         = -5552,
	gestaltLocationErr            = -5553,

	procNotFound = -600
};

/* from Traps.h */

#define _Gestalt 0xA1AD

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
	gestaltDragMgrPresent  = 0,
};

/* From Files.h */

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

typedef pascal Boolean (*AliasFilterProcPtr)(CInfoPBPtr cpbPtr, Boolean *quitFlag, Ptr myDataPtr);

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

/* from Events.h */

enum {
	osEvt = 15,
	kHighLevelEvent               = 23,
	highLevelEventMask            = 0x0400,
};

/* from LowMem.h */

#define LMGetROM85() (*(si4b *)(0x028E))
#define LMGetCurApName() ((StringPtr) 0x0910)
#define LMGetApplLimit() ((Ptr)*((si5p) 0x130))
#define LMGetTicks() (*(ui5b *) 0x016A)
#define LMGetMBarHeight() (*(si4b *)(0x0BAA))
#define LMGetHeapEnd() (*(Ptr *)(0x0114))
#define LMSetHeapEnd(v) (*(Ptr *)(0x0114)) = (v)

#define Have64kROM() (LMGetROM85() < 0)

#define MyGetMBarHeight() (Have64kROM() ? 20 : LMGetMBarHeight())

#ifndef Support64kROM
#define Support64kROM 1
#endif

/* From StandardFile.h */

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
typedef struct StandardFileReply        StandardFileReply;

typedef pascal Boolean (*FileFilterProcPtr)(ParmBlkPtr PB);

pascal void StandardGetFile(FileFilterProcPtr fileFilter,
	short numTypes,
	SFTypeList typeList,
	StandardFileReply *reply)
	= {
		0x3F3C, 0x0006, 0xA9EA
	};

pascal void StandardPutFile(ConstStr255Param prompt,
	ConstStr255Param defaultName,
	StandardFileReply *reply)
	= {
		0x3F3C, 0x0005, 0xA9EA
	};

typedef const OSType *                  ConstSFTypeListPtr;

/* from AppleEvents.h */

#define typeAEList 'list'
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
	errAEEventNotHandled = -1708,
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

typedef ProcPtr EventHandlerProcPtr;

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
	EventHandlerProcPtr handler,
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

typedef pascal Boolean (*AEIdleProcPtr)(EventRecord *theEvent, long *sleepTime, RgnHandle *mouseRgn);
typedef pascal Boolean (*AEFilterProcPtr)(EventRecord *theEvent, long returnID, long transactionID, const AEAddressDesc *sender);

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
	kDragFlavorTypeHFS  = FOUR_CHAR_CODE('hfs '),
};

struct HFSFlavor {
	OSType              fileType;
	OSType              fileCreator;
	UInt16              fdFlags;
	FSSpec              fileSpec;
};
typedef struct HFSFlavor                HFSFlavor;

typedef pascal OSErr (*DragTrackingHandlerProcPtr)(DragTrackingMessage message, WindowRef theWindow, void *handlerRefCon, DragRef theDrag);
typedef pascal OSErr (*DragReceiveHandlerProcPtr)(WindowRef theWindow, void *handlerRefCon, DragRef theDrag);

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
