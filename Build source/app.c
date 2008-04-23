/*
	app.c
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

#define IsAnApp 1

#define SupportWinIDE 1
#define support_ide_mw8 1
#define support_ide_mpw 1

#define Support64kROM 0

#if ! IsAnApp
#include "NEWMCAPI.h"
#endif
#include "COREDEFS.i"

#pragma segment Utilities

#include "POW2UTIL.i"
#include "IMATHOPT.i"
#include "STRUTILS.i"
#include "SAVEDERR.i"
#include "FILEUTIL.i"
#include "MYMEMORY.i"
#include "XBUFHAND.i"

#if IsAnApp
#include "CMDARGW1.i"
#else
#include "CMDARGT1.i"
#include "TOOLUTIL.i"
#endif

#include "FILEHUTL.i"
#include "EXTNSLIB.i"
#include "DELLIBMC.i"
#include "SITLIBMC.i"
#if SupportWinIDE
#include "NZPLIBMC.i"
#endif
#include "TARLIBMC.i"
#include "DUPLIBMC.i"
#if SupportWinIDE
#include "RINSCAFT.i"
#endif
#include "RSBSTCLB.i"

#pragma segment Body

#include "BLDUTIL1.i"
#include "SPBASDEF.i"
#include "GNBLDOPT.i"
#ifdef Have_SPBLDOPT
#include "SPBLDOPT.i"
#endif

#include "WRTEXTFL.i"
#include "BLDUTIL3.i"

#include "DFFILDEF.i"
#include "SPFILDEF.i"
#include "USFILDEF.i"

#pragma segment Body2

#include "WRMACRES.i"
#include "WRMPLIST.i"
#ifdef Have_SPCNFGGL
#include "SPCNFGGL.i"
#endif
#include "WRCNFGGL.i"
#ifdef Have_SPCNFGAP
#include "SPCNFGAP.i"
#endif
#include "WRCNFGAP.i"

#include "SPWROASM.i"
#ifdef Have_SPBLDOPT
#include "SPOTHRCF.i"
#endif

#if support_ide_mpw
#include "WRMPWFLS.i"
#endif
#if support_ide_mw8
#include "WRMW8FLS.i"
#endif
#include "WRBGCFLS.i"
#include "WRSNCFLS.i"
#if SupportWinIDE
#include "WRMSCFLS.i"
#include "WRLCCFLS.i"
#include "WRDVCFLS.i"
#endif
#include "WRXCDFLS.i"

#include "BLDUTIL4.i"

#if ! IsAnApp
#include "CMDARGT2.i"
#else
#include "CMDARGW2.i"
#endif
