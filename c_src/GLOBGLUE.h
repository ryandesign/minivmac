/*
	GLOBGLUE.h

	Copyright (C) 2002 Bernd Schmidt, Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef GLOBGLUE_H
#error "header already included"
#else
#define GLOBGLUE_H
#endif

EXPORTPROC ZapProgramVars(void);
EXPORTFUNC blnr InitProgram(void);
EXPORTPROC UnInitProgram(void);
EXPORTPROC customreset(void);
EXPORTPROC SixtiethSecondNotify(void);
EXPORTPROC OneSecondNotify(void);
