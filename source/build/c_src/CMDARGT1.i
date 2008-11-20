/*
	CMDARGT1.i
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

LOCALPROC AdvanceTheArg(void)
{
	++The_argi;
	if (The_argi < The_argc) {
		Cur_args = The_argv[The_argi];
	} else {
		The_arg_end = trueblnr;
	}
}

LOCALFUNC blnr CurArgIsCStr(char *s)
{
	if (The_arg_end) {
		return falseblnr;
	} else {
		return CStrEq(Cur_args, s);
	}
}

LOCALFUNC blnr CurArgIsCStrAdvance(char *s)
{
	if (! CurArgIsCStr(s)) {
		return falseblnr;
	} else {
		AdvanceTheArg();
		return trueblnr;
	}
}

LOCALPROC GetCurArgAsPStr(ps3p s)
{
	PStrFromCStr(s, Cur_args);
}

LOCALFUNC blnr GetCurArgAsHandle(Handle *r)
{
	return CStr2Hand(Cur_args, r);
}

LOCALPROC BeginParseCommandLineArguments(int argc, The_argvt argv)
{
	The_argi = 0;
	The_argc = argc;
	The_argv = argv;
	The_arg_end = falseblnr;
	ParseArgsFailed = falseblnr;
	AdvanceTheArg();
}

LOCALPROC ReportParseFailure(char *s)
{
	fprintf(stderr, "%s\n", s);
	ParseArgsFailed = trueblnr;
}

LOCALPROC DisplayRunErr(char *s)
{
	fprintf(stderr, "%s\n", s);
}

LOCALVAR blnr AbortRequested = falseblnr;
LOCALVAR uimr ProgressBarVal;

#define CheckAbort()
