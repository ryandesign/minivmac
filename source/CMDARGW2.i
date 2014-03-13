/*
	CMDARGW2.i
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
	CoMmandD ARGument Window part 2
*/


LOCALPROC ProgramZapVars(void)
{
}

LOCALPROC ProgramPreInit(void)
{
	OneWindAppPreInit();
}

LOCALFUNC tMyErr ProgramInit(void)
{
	return OneWindAppInit_v2();
}

LOCALPROC ProgramUnInit(void)
{
	OneWindAppUnInit();
}

LOCALPROC ProgramMain(void)
{
	(void) ProgressBar_SetStage_v2(
		"Type build options, then click here", 0);
	do {
		WaitForInput();
		if (GoRequested) {
			if (noErr == ProgressBar_SetStage_v2(
				"Running, type command-period to abort\311", 0))
			{
				BeginParseFromTE();
				(void) CheckSysErr(DoTheCommand());
				EndParseFromTE();
			}
			GoRequested = falseblnr;
			(void) ProgressBar_SetStage_v2(
				"Done, ready for more options\311", 0);
		}
	} while ((! ProgramDone) && (! GoRequested));
}

int main(void)
{
	ProgramZapVars();
	ProgramPreInit();
	if (CheckSysErr(ProgramInit())) {
		ProgramMain();
	}
	ProgramUnInit();

	return 0;
}
