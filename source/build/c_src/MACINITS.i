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

	UnloadSeg((Ptr)_DataInit); /* Unload the linker's initialization segment */
	h = GetNamedResource('CODE', (ps3p)"\p%A5Init");
	if ((h != NULL) && (*h != NULL)) {
		ReleaseResource(h);
	}
}
#endif

#ifndef StackAlloc
#define StackAlloc 0
#endif

#if CALL_NOT_IN_CARBON
LOCALPROC MyMacHeapInit(void)
{
#if ShouldUnloadDataInit
	MyUnloadDataInit();
#endif

#if StackAlloc != 0
	SetApplLimit(LMGetApplLimit() - StackAlloc);
#endif

	MaxApplZone();
}
#else
#define MyMacHeapInit()
#endif

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

#if CALL_NOT_IN_CARBON
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
#endif

	InitCursor();

	FlushEvents(everyEvent, 0);
}

/*-- basic environment checking --*/

#if CALL_NOT_IN_CARBON
LOCALFUNC TrapType GetTrapType(short theTrap)
{
	if ((theTrap & 0x0800) != 0) {
		return ToolTrap;
	} else {
		return OSTrap;
	}
}
#endif

#if CALL_NOT_IN_CARBON
LOCALFUNC short NumToolboxTraps(void)
{
	if (NGetTrapAddress(_InitGraf, ToolTrap)
		== NGetTrapAddress(0xAA6E, ToolTrap))
	{
		return 0x200;
	} else {
		return 0x400;
	}
}
#endif

#if CALL_NOT_IN_CARBON
LOCALFUNC blnr TrapAvailable(short trap_num)
{
	TrapType type;

	type = GetTrapType(trap_num);
	if (type == ToolTrap) {
		trap_num &= 0x07ff;
		if (trap_num > NumToolboxTraps()) {
			return falseblnr;
		}
	}
	return NGetTrapAddress(trap_num, type) != NGetTrapAddress(_Unimplemented, ToolTrap);
}
#endif

#if CALL_NOT_IN_CARBON
LOCALVAR blnr MyEnvrAttrWaitNextEventAvail;
LOCALVAR blnr HaveEnvrAttrWaitNextEventAvail = falseblnr;

LOCALFUNC blnr HaveWaitNextEventAvail(void)
{
	if (! HaveEnvrAttrWaitNextEventAvail) {
		MyEnvrAttrWaitNextEventAvail = TrapAvailable(_WaitNextEvent);
		HaveEnvrAttrWaitNextEventAvail = trueblnr;
	}
	return MyEnvrAttrWaitNextEventAvail;
}
#else
#define HaveWaitNextEventAvail() trueblnr
#endif

#if CALL_NOT_IN_CARBON
LOCALVAR blnr MyEnvrAttrGestaltAvail;
LOCALVAR blnr HaveEnvrAttrGestaltAvail = falseblnr;

LOCALFUNC blnr HaveGestaltAvail(void)
{
	if (! HaveEnvrAttrGestaltAvail) {
		MyEnvrAttrGestaltAvail =
			(! Have64kROM()) &&
			TrapAvailable(_Gestalt);
			/*
				contrary to all the documentation,
				TrapAvailable check by itself doesn't
				work on 64k ROM, because a tool box trap
				has the same trap number.
			*/
		HaveEnvrAttrGestaltAvail = trueblnr;
	}
	return MyEnvrAttrGestaltAvail;
}
#else
#define HaveGestaltAvail() trueblnr
#endif

LOCALVAR blnr MyEnvrAttrCustomPutFileAvail;
LOCALVAR blnr HaveEnvrAttrCustomPutFileAvail = falseblnr;

LOCALFUNC blnr HaveCustomPutFileAvail(void)
{
	if (! HaveEnvrAttrCustomPutFileAvail) {
		long reply;

		MyEnvrAttrCustomPutFileAvail =
			HaveGestaltAvail()
			&& (Gestalt(gestaltStandardFileAttr, &reply) == 0)
			&& TestBit(reply, gestaltStandardFile58);
		HaveEnvrAttrCustomPutFileAvail = trueblnr;
	}
	return MyEnvrAttrCustomPutFileAvail;
}

LOCALVAR blnr MyEnvrAttrAppleEvtMgrAvail;
LOCALVAR blnr HaveEnvrAttrAppleEvtMgrAvail = falseblnr;

LOCALFUNC blnr HaveAppleEvtMgrAvail(void)
{
	if (! HaveEnvrAttrAppleEvtMgrAvail) {
		long reply;

		MyEnvrAttrAppleEvtMgrAvail =
			HaveGestaltAvail()
			&& (Gestalt(gestaltAppleEventsAttr, &reply) == 0)
			&& TestBit(reply, gestaltAppleEventsPresent);
		HaveEnvrAttrAppleEvtMgrAvail = trueblnr;
	}
	return MyEnvrAttrAppleEvtMgrAvail;
}

LOCALVAR blnr MyEnvrAttrDragMgrAvail;
LOCALVAR blnr HaveEnvrAttrDragMgrAvail = falseblnr;

LOCALFUNC blnr HaveDragMgrAvail(void)
{
	if (! HaveEnvrAttrDragMgrAvail) {
		long reply;

		MyEnvrAttrDragMgrAvail =
			HaveGestaltAvail()
			&& (Gestalt(gestaltDragMgrAttr, &reply) == 0)
			&& TestBit(reply, gestaltDragMgrPresent);
		HaveEnvrAttrDragMgrAvail = trueblnr;
	}
	return MyEnvrAttrDragMgrAvail;
}

#define Have_MACINITS 1
