/*
	MOUSEMDV.h

	Copyright (C) 2002 Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef MOUSEMDV_H
#error "header already included"
#else
#define MOUSEMDV_H
#endif

EXPORTPROC Mouse_Update2(void);

// VIA Interface Headers

EXPORTFUNC ui3b VIA_GORB5 (void); // Mouse Y2
EXPORTPROC VIA_PORB5(ui3b Data);

EXPORTFUNC ui3b VIA_GORB4 (void); // Mouse X2
EXPORTPROC VIA_PORB4(ui3b Data);

EXPORTFUNC ui3b VIA_GORB3 (void); // Mouse Button
EXPORTPROC VIA_PORB3(ui3b Data);
