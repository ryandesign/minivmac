/*
	CMDARGT1.i
	Copyright (C) 2009 Paul C. Pratt

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
	CoMmandD ARGument Tool part 1

	allows code used with CMDARGW1 to also be
	used to make an MPW tool.
*/


LOCALVAR int The_argc;
LOCALVAR int The_argi;
typedef char **The_argvt;
LOCALVAR The_argvt The_argv;
LOCALVAR char *Cur_args;
LOCALVAR blnr The_arg_end;
LOCALVAR blnr ParseArgsFailed;

GLOBALPROC AdvanceTheArg(void)
{
	++The_argi;
	if (The_argi < The_argc) {
		Cur_args = The_argv[The_argi];
	} else {
		The_arg_end = trueblnr;
	}
}

GLOBALFUNC blnr CurArgIsCStr_v2(char *s)
{
	/* warning : assumes (! The_arg_end) */
	return CStrEq(Cur_args, s);
}

GLOBALFUNC blnr CurArgIsCStrAdvance_v2(char *s)
{
	/* warning : assumes (! The_arg_end) */
	if (! CurArgIsCStr_v2(s)) {
		return falseblnr;
	} else {
		AdvanceTheArg();
		return trueblnr;
	}
}

GLOBALPROC GetCurArgAsCStr(char *s, uimr MaxN)
{
	/* warning : assumes (! The_arg_end) */
	if (CStrLength(Cur_args) > MaxN) {
		s[0] = 0;
	} else {
		CStrCopy(s, Cur_args);
	}
}

GLOBALPROC GetCurArgAsPStr(ps3p s)
{
	/* warning : assumes (! The_arg_end) */
	PStrFromCStr(s, Cur_args);
}

GLOBALFUNC tMyErr GetCurArgAsHandle_v2(Handle *r)
{
	/* warning : assumes (! The_arg_end) */
	return CStr2Hand_v2(Cur_args, r);
}

GLOBALPROC BeginParseCommandLineArguments(int argc, The_argvt argv)
{
	The_argi = 0;
	The_argc = argc;
	The_argv = argv;
	The_arg_end = falseblnr;
	ParseArgsFailed = falseblnr;
	AdvanceTheArg();
}

GLOBALPROC ReportParseFailure(char *s)
{
	fprintf(stderr, "%s\n", s);
	ParseArgsFailed = trueblnr;
}

GLOBALPROC DisplayRunErr(char *s)
{
	fprintf(stderr, "%s\n", s);
}

GLOBALVAR blnr AbortRequested = falseblnr;
GLOBALVAR uimr ProgressBarVal;

#define CheckAbort()

GLOBALFUNC tMyErr CheckAbortRequested(void)
{
	CheckAbort();
	return AbortRequested ? kMyErrUsrAbort : noErr;
}
