/*
	GLOBGLUE.h

	Copyright (C) 2003 Bernd Schmidt, Philip Cummins, Paul C. Pratt

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

EXPORTFUNC blnr InitEmulation(void);
EXPORTPROC customreset(void);
EXPORTPROC EmulatedHardwareZap(void);
EXPORTPROC SixtiethSecondNotify(void);
EXPORTPROC SubTickNotify(int SubTick);
EXPORTPROC ExtraTimeBeginNotify(void);
EXPORTPROC ExtraTimeEndNotify(void);
