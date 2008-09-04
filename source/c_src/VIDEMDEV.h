/*
	VIDEMDEV.h

	Copyright (C) 2008 Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef VIDEMDEV_H
#error "header already included"
#else
#define VIDEMDEV_H
#endif

EXPORTFUNC blnr Vid_Init(void);
EXPORTPROC Vid_Update(void);
EXPORTFUNC ui4r Vid_GetMode(void);
EXPORTFUNC tMacErr Vid_SetMode(ui4r v);
EXPORTFUNC ui4r Vid_Reset(void);
