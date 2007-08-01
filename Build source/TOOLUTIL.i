/*
	TOOLUTIL.i
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
	mpw TOOL UTILilities
*/


/*
FSpLocationFromFullPath was originally part of More Files version 1.4.8

More Files fixes many of the broken or underfunctional
parts of the file system.

More Files

A collection of File Manager and related routines

by Jim Luther (Apple Macintosh Developer Technical Support Emeritus)
with significant code contributions by Nitin Ganatra
(Apple Macintosh Developer Technical Support Emeritus)
Copyright  1992-1998 Apple Computer, Inc.
Portions copyright  1995 Jim Luther
All rights reserved.

The Package "More Files" is distributed under the following
license terms:

	"You may incorporate this sample code into your
	applications without restriction, though the
	sample code has been provided "AS IS" and the
	responsibility for its operation is 100% yours.
	However, what you are not permitted to do is to
	redistribute the source as "DSC Sample Code" after
	having made changes. If you're going to
	redistribute the source, we require that you make
	it clear in the source that the code was descended
	from Apple Sample Code, but that you've made
	changes."

More Files can be found on the MetroWerks CD and Developer CD from
Apple. You can also download the latest version from:

	http://members.aol.com/JumpLong/#MoreFiles

Jim Luther's Home-page:
	http://members.aol.com/JumpLong/

*/

/*
	Get a FSSpec from a full pathname.
	The FSpLocationFromFullPath function returns a FSSpec to the object
	specified by full pathname. This function requires the Alias Manager.

	fullPathLength  input:  The number of characters in the full pathname
		of the target.
	fullPath        input:  A pointer to a buffer that contains the full
		pathname of the target. The full pathname
		starts with the name of the volume, includes
		all of the directory names in the path to the
		target, and ends with the target name.
	spec            output: An FSSpec record specifying the object.

	Result Codes
		noErr               0       No error
		nsvErr              -35     The volume is not mounted
		fnfErr              -43     Target not found, but volume and parent
			directory found
		paramErr            -50     Parameter error
		usrCanceledErr      -128    The user canceled the operation

	__________

	See also:   LocationFromFullPath
*/

static OSErr FSpLocationFromFullPath(short fullPathLength,
	const void *fullPath,
	FSSpec *spec)
{
	AliasHandle alias;
	OSErr       result;
	Boolean     wasChanged;
	Str32       nullString;

	/* Create a minimal alias from the full pathname */
	nullString[0] = (char)0;  /* null string to indicate no zone or server name */
	result = NewAliasMinimalFromFullPath(fullPathLength, fullPath, nullString,
		nullString, &alias);

	if (result == noErr) {
		/* Let the Alias Manager resolve the alias. */
		result = ResolveAlias(NULL, alias, spec, &wasChanged);

		DisposHandle((Handle)alias);   /* Free up memory used */
	}

	return result;
}

GLOBALFUNC blnr PathArgToNewFileNameDir(char *path,
	MyDir_R *d, ps3p s)
{
	FSSpec Spec;
	OSErr   err;
	blnr IsOk = falseblnr;

	err = FSpLocationFromFullPath(CStrLength(path), path, &Spec);
	if ((fnfErr == err) || CheckSysErr(err)) {
		d->VRefNum = Spec.vRefNum;
		d->DirId = Spec.parID;
		PStrCopy(s, Spec.name);
		IsOk = trueblnr;
	}
	return IsOk;
}

GLOBALFUNC blnr PathArgToOldFileNameDir(char *path,
	MyDir_R *d, ps3p s)
{
	FSSpec Spec;
	OSErr   err;
	Boolean wasAlias;
	Boolean isFolder;
	blnr IsOk = falseblnr;

	err = FSpLocationFromFullPath(CStrLength(path), path, &Spec);
	if (CheckSysErr(err)) {
		err = ResolveAliasFile(&Spec, true, &isFolder, &wasAlias);
		if (CheckSysErr(err)) {
			d->VRefNum = Spec.vRefNum;
			d->DirId = Spec.parID;
			PStrCopy(s, Spec.name);
			IsOk = trueblnr;
		}
	}
	return IsOk;
}

GLOBALPROC ToolReportAnySavedError(int argc, char **argv)
{
#pragma unused(argc)
	if (SavedSysErr != noErr) {
		char *msg = GetTextForSavedSysErr();

		fprintf(stderr, "\n\nError in %s: %s", argv[0], msg);
	}
}
