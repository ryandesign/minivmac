/*
	NEWMCAPI.h
	Copyright (C) 2007 Paul Pratt

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


/* From MacTypes.h */

typedef unsigned char                   Str32[33];

typedef Str63                           StrFileName;

typedef const unsigned char *           ConstStr255Param;
typedef const unsigned char *           ConstStr32Param;
typedef const unsigned char *           ConstStr31Param;

typedef signed char                     SInt8;
typedef signed short                    SInt16;

/* Script.h */

enum {
	smSystemScript                = -1,   /* designates system script.*/
	smCurrentScript               = -2,   /* designates current font script.*/
	smAllScripts                  = -3    /* designates any script*/
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
