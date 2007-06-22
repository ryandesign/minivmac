/*
	DFFILDEF.i
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
	DEFinitions for program SPecific FILe DEFinitions
*/

#define kDepDirCSrc 0
#define kDepDirPlat 1
#define kDepDirLang 2
#define kDepDirCnfg 3

typedef void (*tDoOneExtraHeader)(int DepDir, char *s);

typedef void (*tDoOneDepends)(int DepDir, char *s);

typedef void (*tDoDependsForC)(tDoOneDepends p);

typedef void (*tDoOneCFile)(char *s, long Flgm, tDoDependsForC depends);

typedef void (*tWriteOneExtension)(char *s);

typedef void (*tWriteExtensionList)(tWriteOneExtension p);

typedef void (*tWriteOneDocType)(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList);

#define kCSrcFlagAsmAvail 0
#define kCSrcFlagAltSrc 1

#define kCSrcFlgmNone 0
#define kCSrcFlgmAsmAvail (1 << kCSrcFlagAsmAvail)
#define kCSrcFlgmAltSrc (1 << kCSrcFlagAltSrc)
