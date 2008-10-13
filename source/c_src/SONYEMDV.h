/*
	SONYEMDV.h

	Copyright (C) 2004 Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef SONYEMDV_H
#error "header already included"
#else
#define SONYEMDV_H
#endif

#define ExtnDat_checkval 0
#define ExtnDat_extension 2
#define ExtnDat_commnd 4
#define ExtnDat_result 6
#define ExtnDat_params 8

#define kCmndVersion 0
#define ExtnDat_version 8

EXPORTPROC Sony_Reset(void);

EXPORTPROC Sony_Access(ui5b Data, CPTR addr);

EXPORTPROC Sony_Update(void);
