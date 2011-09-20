/*
	MACINITS.i
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
	MACintosh INITitializationS
*/


#ifndef ShouldUnloadDataInit
#define ShouldUnloadDataInit 1
#endif

#if ShouldUnloadDataInit
extern void _DataInit(void);
#endif

#if ShouldUnloadDataInit
LOCALPROC MyUnloadDataInit(void)
{
	Handle h;

	UnloadSeg((Ptr)_DataInit);
		/* Unload the linker's initialization segment */
	h = GetNamedResource('CODE', (ps3p)"\p%A5Init");
	if ((h != NULL) && (*h != NULL)) {
		ReleaseResource(h);
	}
}
#endif

#ifndef StackAlloc
#define StackAlloc 0
#endif

LOCALPROC MyMacHeapInit(void)
{
#if ShouldUnloadDataInit
	MyUnloadDataInit();
#endif

#if StackAlloc != 0
	SetApplLimit(LMGetApplLimit() - StackAlloc);
#endif

	if (! Have64kROM()) {
		MaxApplZone();
	}
}

#ifndef NumMoreMasters
#define NumMoreMasters 0
#endif

/*
	Between calling MyMacHeapInit and
	calling MyMacToolBoxInit is the
	appropriate time for any permanent
	large memory allocations.
*/

LOCALPROC MyMacToolBoxInit(void)
{

#if NumMoreMasters != 0
	{
		simr i;

		for (i = NumMoreMasters; --i >= 0; ) {
			MoreMasters();
		}
	}
#endif

	InitGraf((Ptr) &qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();

	FlushEvents(everyEvent, 0);
}

#define Have_MACINITS 1
