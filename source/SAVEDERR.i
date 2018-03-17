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


#define kMyErr_noErr 0 /* no error */

#define kMyErrUsrCancel 1024 /* such as choose cancel in dialog */
#define kMyErrUsrAbort 1025 /* such as choose command-period */
#define kMyErrSysUnknown 1026
#define kMyErrParamErr 1027
#define kMyErrNoExtn 1028 /* extension not available */
#define kMyErrReported 1029
	/* already notified user, no further report needed */
#define kMyErrNoMatch 1030
	/* (so try something else) always should be handled, not reported */
#define kMyErrProgramBug 1031
	/* found situation that shouldn't occur unless bug in program */
#define kMyErrSyntaxErr 1032

#define kMyErr_failr 2048 /* unspecified failure */
#define kMyErr_lowmem 2049 /* out of memory */
#define kMyErr_lowstk 2050 /* out of stack */
#define kMyErr_lowdsk 2051 /* out of disk space */
#define kMyErr_lckdsk 2052 /* locked disk */
#define kMyErr_lckfil 2053 /* locked file */
#define kMyErr_eofile 2054 /* file shorter than expected */
#define kMyErr_toomof 2055 /* too many open files */
#define kMyErr_crptfl 2056 /* Corrupted file */
#define kMyErr_flnfnd 2057 /* file not found */
#define kMyErr_dupfil 2058 /* duplicate file */
#define kMyErr_finuse 2059 /* file in use */
#define kMyErr_iofail 2060 /* disk io failure */
#define kMyErr_toonew 2061 /* need newer version of program */
#define kMyErr_toomni 2062 /* too many indices */

#ifndef DebugCheck
#ifdef Have_DEBUGLOG
#define DebugCheck 1
#else
#define DebugCheck 0
#endif
#endif

GLOBALFUNC tMyErr ErrCombine(tMyErr err, tMyErr err2)
{
	if (noErr == err) {
		err = err2;
	}

	return err;
}

GLOBALFUNC char * GetTextForSavedSysErr_v2(tMyErr err)
{
	char *s;

	switch (err) {
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
			s = "duplicate file name";
			break;
		case bdNamErr:
			s = "Bad file name (too long or invalid characters)";
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

#if DebugCheck
LOCALFUNC tMyErr ErrReportStack0(tMyErr err, char *s)
{
	dbglog_writeCStr("in ");
	dbglog_writeCStr(s);
	dbglog_writeReturn();

	return err;
}
#endif

#if DebugCheck
#define ErrReportStack(err, s) \
	((kMyErr_noErr != (err)) ? ErrReportStack0(err, s) : (err))
#else
#define ErrReportStack(err, s) err
#endif
