/*
	VIAEMDEV.h

	Copyright (C) 2004 Philip Cummins, Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

#ifdef VIAEMDEV_H
#error "header already included"
#else
#define VIAEMDEV_H
#endif

/* PC - VIA Memory Handlers */

EXPORTPROC VIA_Zap(void);
EXPORTPROC VIA_Reset(void);

EXPORTFUNC ui5b VIA_Access(ui5b Data, blnr WriteMem, CPTR addr);

EXPORTPROC VIA_ExtraTimeBegin(void);
EXPORTPROC VIA_ExtraTimeEnd(void);
EXPORTPROC VIA_Int_Vertical_Blanking(void);
EXPORTPROC VIA_Int_One_Second(void);
EXPORTPROC VIA_DoTimer1Check(void);
EXPORTPROC VIA_DoTimer2Check(void);

EXPORTFUNC ui4b GetSoundInvertTime(void);

EXPORTPROC VIA_ShiftInData(ui3b v);
EXPORTFUNC ui3b VIA_ShiftOutData(void);
