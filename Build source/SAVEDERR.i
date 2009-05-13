/*
	SAVEDERR.i
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
	SAVED os ERRor value
*/


GLOBALVAR OSErr SavedSysErr = noErr;

GLOBALPROC vCheckSysErr(OSErr theErr)
{
	if (noErr != theErr) {
		if (noErr == SavedSysErr) {
#if 0
			Debugger();
#endif
			SavedSysErr = theErr;
		}
	}
}

GLOBALFUNC blnr CheckSysErr(OSErr theErr)
{
	if (noErr == theErr) {
		return trueblnr;
	} else {
		vCheckSysErr(theErr);
		return falseblnr;
	}
}

GLOBALFUNC char * GetTextForSavedSysErr(void)
{
	char *s;

	switch (SavedSysErr) {
		case dskFulErr:
		case dirFulErr:
			s = "The disk is full";
			break;
		case nsvErr:
			s = "No such Volume";
			break;
		case eofErr:
			s = "End of file";
			break;
		case mFulErr:
		case memFullErr:
			s = "There is not enough memory";
			break;
		case fnfErr:
			s = "Directory/File not found";
			break;
		case wPrErr:
			s = "write protected";
			break;
		case fLckdErr:
			s = "The file is locked";
			break;
		case vLckdErr:
			s = "The volume is locked";
			break;
		case fBsyErr:
			s = "The file is busy";
			break;
		case dupFNErr:
			s = "duplicate filename";
			break;
		case bdNamErr:
			s = "Bad Volume Name";
			break;
		case opWrErr:
			s = "File is already open for writing";
			break;
		case volOffLinErr:
			s = "volume not on line";
			break;
		case permErr:
			s = "permissions error";
			break;
		case dirNFErr:
			s = "Directory/File not found or incomplete pathname";
			break;
		default:
			s = "Unknown error";
			break;
	}
	return s;
}
