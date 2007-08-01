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

LOCALFUNC blnr ProgramInit(void)
{
	return OneWindAppInit();
}

LOCALPROC ProgramUnInit(void)
{
	OneWindAppUnInit();
}

LOCALPROC ProgramMain(void)
{
	do {
		WaitForInput();
		if (GoRequested) {
			BeginParseFromTE();
			DoTheCommand();
			EndParseFromTE();
			GoRequested = falseblnr;
		}
	} while ((! ProgramDone) && (! GoRequested));
}

int main(void)
{
	ProgramZapVars();
	ProgramPreInit();
	if (ProgramInit()) {
		ProgramMain();
	}
	ProgramUnInit();

	return 0;
}
