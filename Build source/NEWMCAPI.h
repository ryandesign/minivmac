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
typedef unsigned long UInt32;
typedef unsigned short UInt16;

#define WindowRef WindowPtr

/* Script.h */

enum {
	smSystemScript                = -1,
	smCurrentScript               = -2,
	smAllScripts                  = -3
};

/* From Files.h */

struct FSSpec {
	short               vRefNum;
	long                parID;
	StrFileName         name;
};
typedef struct FSSpec                   FSSpec;
typedef FSSpec *                        FSSpecPtr;

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

pascal OSErr
NewAliasMinimalFromFullPath(
	short             fullPathLength,
	const void *      fullPath,
	ConstStr32Param   zoneName,
	ConstStr31Param   serverName,
	AliasHandle *     alias)
	= {
		0x7009, 0xA823
	};

pascal OSErr
ResolveAlias(
	const FSSpec *  fromFile,
	AliasHandle     alias,
	FSSpec *        target,
	Boolean *       wasChanged)
	= {
		0x7003, 0xA823
	};

pascal OSErr
ResolveAliasFile(
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

/* from MacErrors.h */

enum {
	gestaltUnknownErr             = -5550,
	gestaltUndefSelectorErr       = -5551,
	gestaltDupSelectorErr         = -5552,
	gestaltLocationErr            = -5553
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

/* from LowMem.h */

#define LMGetROM85() (*(si4b *)(0x028E))
#define LMGetCurApName() ((StringPtr) 0x0910)
#define LMGetApplLimit() ((Ptr)*((si5p) 0x130))
#define LMGetTicks() (*(ui5b *) 0x016A)
#define LMGetMBarHeight() (*(si4b *)(0x0BAA))

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

#define kCoreEventClass 'aevt'

#define kAEOpenApplication 'oapp'
#define kAEOpenDocuments 'odoc'
#define kAEPrintDocuments 'pdoc'
#define kAEQuitApplication 'quit'

#define keyDirectObject '----'
#define keyMissedKeywordAttr 'miss'

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
typedef AERecord AppleEvent;

enum {
	kAEInteractWithSelf,
	kAEInteractWithLocal,
	kAEInteractWithAll
};
typedef unsigned char AEInteractAllowed;

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

enum {
	kNoProcess                    = 0,
	kSystemProcess                = 1,
	kCurrentProcess               = 2
};

pascal OSErr SetFrontProcess(const ProcessSerialNumber * PSN)
	= {
		0x3F3C, 0x003B, 0xA88F
	};
