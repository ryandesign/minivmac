/*
	app.c
	Copyright (C) 2009 Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/


#include "MYMACAPI.i"

#define Support64kROM 0

#include "COREDEFS.i"

#pragma segment Utilities

#include "POW2UTIL.i"
#include "IMATHOPT.i"
#include "STRUTILS.i"
#include "MACENVRN.i"
#if IsAnApp
#include "MACINITS.i"
#endif
#include "SAVEDERR.i"
#include "FILEUTIL.i"
#include "MYMEMORY.i"
#include "XBUFHAND.i"
#include "MYFPMATH.i"
#include "PROGRBAR.i"

#define WantRealInputFile 1

#if IsAnApp
#include "CMDARGW1.i"
#else
#include "CMDARGT1.i"
#include "TOOLUTIL.i"
#endif

#include "STREAMIN.i"
#include "STRMOUTT.i"

#include "FILEHUTL.i"
#include "EXTNSLIB.i"
#include "DELLIBMC.i"
#include "DUPLIBMC.i"
#include "RINSCAFT.i"
#include "RSBSTCLB.i"
#include "SITLIBMC.i"
#include "NZPLIBMC.i"
#include "TARLIBMC.i"

#pragma segment Body

#define ide_mpw_Supported 1
#define ide_bgc_Supported 1
#define ide_xcd_Supported 1
#define ide_mw8_Supported 1
#define ide_snc_Supported 1
#define ide_msv_Supported 1
#define ide_lcc_Supported 1
#define ide_dvc_Supported 1
#define ide_dmc_Supported 1
#define ide_plc_Supported 1
#define ide_ccc_Supported 1

#include "EXTNUTIL.i"
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
#include "WRCNFGGL.i"
#include "WRCNFGAP.i"

#if ide_mpw_Supported
#include "WRMPWFLS.i"
#endif
#if ide_mw8_Supported
#include "WRMW8FLS.i"
#endif
#if ide_bgc_Supported
#include "WRBGCFLS.i"
#endif
#if ide_snc_Supported
#include "WRSNCFLS.i"
#endif
#if ide_msv_Supported
#include "WRMSCFLS.i"
#endif
#if ide_lcc_Supported
#include "WRLCCFLS.i"
#endif
#if ide_dvc_Supported
#include "WRDVCFLS.i"
#endif
#if ide_xcd_Supported
#include "WRXCDFLS.i"
#endif
#if ide_dmc_Supported
#include "WRDMCFLS.i"
#endif
#if ide_plc_Supported
#include "WRPLCFLS.i"
#endif
#if ide_ccc_Supported
#include "WRCCCFLS.i"
#endif

#ifdef Have_SPCNFGGL
#include "SPCNFGGL.i"
#endif
#ifdef Have_SPCNFGAP
#include "SPCNFGAP.i"
#endif
#if AsmSupported
#include "SPWROASM.i"
#endif
#include "SPOTHRCF.i"

#include "BLDUTIL4.i"

#if ! IsAnApp
#include "CMDARGT2.i"
#else
#include "CMDARGW2.i"
#endif
