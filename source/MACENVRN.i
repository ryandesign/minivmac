/*
	MACENVRN.i
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

/*
	MACintosh ENViRoNment
*/

#ifndef Support64kROM
#define Support64kROM 1
#endif

#define Have64kROM() (LMGetROM85() < 0)

/* -- basic environment checking -- */

#ifndef DebugTrapAvailableChecks
#define DebugTrapAvailableChecks 0
#endif

LOCALFUNC blnr LowToolTrapAvailable(short trap_num)
{
#if DebugTrapAvailableChecks
	if ((trap_num & 0x0800) == 0) {
		DebugStr("\pOSTrap in LowToolTrapAvailable");
		return falseblnr;
	}
	if ((trap_num & 0x07ff) >= 0x200) {
		DebugStr("\pHiToolTrap in LowToolTrapAvailable");
		return falseblnr;
	}
#endif
	return GetToolTrapAddress(trap_num)
		!= GetToolTrapAddress(_Unimplemented);
}

LOCALFUNC blnr HiToolTrapAvailable(short trap_num)
{
#if DebugTrapAvailableChecks
	if ((trap_num & 0x0800) == 0) {
		DebugStr("\pOSTrap in HiToolTrapAvailable");
		return falseblnr;
	}
	if (((trap_num & 0x07ff) < 0x200)
		|| ((trap_num & 0x07ff) >= 0x400))
	{
		DebugStr("\pLowToolTrap in HiToolTrapAvailable");
		return falseblnr;
	}
#endif
	if (GetToolTrapAddress(_InitGraf)
		== GetToolTrapAddress(0xAA6E))
	{
		return falseblnr;
	} else {
		return GetToolTrapAddress(trap_num)
			!= GetToolTrapAddress(_Unimplemented);
	}
}

LOCALFUNC blnr OSTrapAvailable(short trap_num)
{
#if DebugTrapAvailableChecks
	if ((trap_num & 0x0800) != 0) {
		DebugStr("\pToolTrap in ToolTrapAvailable");
		return falseblnr;
	}
#endif
	return GetOSTrapAddress(trap_num)
		!= GetToolTrapAddress(_Unimplemented);
}

LOCALVAR blnr MyEnvrAttrWaitNextEventAvail;
LOCALVAR blnr HaveEnvrAttrWaitNextEventAvail = falseblnr;

LOCALFUNC blnr HaveWaitNextEventAvail(void)
{
	if (! HaveEnvrAttrWaitNextEventAvail) {
		MyEnvrAttrWaitNextEventAvail =
			LowToolTrapAvailable(_WaitNextEvent);
		HaveEnvrAttrWaitNextEventAvail = trueblnr;
	}
	return MyEnvrAttrWaitNextEventAvail;
}

LOCALVAR blnr MyEnvrAttrGestaltAvail;
LOCALVAR blnr HaveEnvrAttrGestaltAvail = falseblnr;

LOCALFUNC blnr HaveGestaltAvail(void)
{
	if (! HaveEnvrAttrGestaltAvail) {
		MyEnvrAttrGestaltAvail =
			(! Have64kROM()) &&
			OSTrapAvailable(_Gestalt);
			/*
				contrary to all the documentation,
				TrapAvailable check by itself doesn't
				work on 64k ROM, because a tool box trap
				has the same trap number, and both
				GetOSTrapAddress and GetToolTrapAddress
				are the same as GetTrapAddress on a 64k ROM.
			*/
		HaveEnvrAttrGestaltAvail = trueblnr;
	}
	return MyEnvrAttrGestaltAvail;
}

LOCALVAR blnr MyEnvrAttrCustomPutFileAvail;
LOCALVAR blnr HaveEnvrAttrCustomPutFileAvail = falseblnr;

LOCALFUNC blnr HaveCustomPutFileAvail(void)
{
	if (! HaveEnvrAttrCustomPutFileAvail) {
		long reply;

		MyEnvrAttrCustomPutFileAvail =
			HaveGestaltAvail()
			&& (noErr == Gestalt(gestaltStandardFileAttr, &reply))
			&& TestBit(reply, gestaltStandardFile58);
		HaveEnvrAttrCustomPutFileAvail = trueblnr;
	}
	return MyEnvrAttrCustomPutFileAvail;
}

LOCALVAR blnr MyEnvrAttrAliasMgrAvail;
LOCALVAR blnr HaveEnvrAttrAliasMgrAvail = falseblnr;

LOCALFUNC blnr HaveAliasMgrAvail(void)
{
	if (! HaveEnvrAttrAliasMgrAvail) {
		long reply;

		MyEnvrAttrAliasMgrAvail =
			HaveGestaltAvail()
			&& (noErr == Gestalt(gestaltAliasMgrAttr, &reply))
			&& TestBit(reply, gestaltAliasMgrPresent);
		HaveEnvrAttrAliasMgrAvail = trueblnr;
	}
	return MyEnvrAttrAliasMgrAvail;
}

LOCALVAR blnr MyEnvrAttrAppleEvtMgrAvail;
LOCALVAR blnr HaveEnvrAttrAppleEvtMgrAvail = falseblnr;

LOCALFUNC blnr HaveAppleEvtMgrAvail(void)
{
	if (! HaveEnvrAttrAppleEvtMgrAvail) {
		long reply;

		MyEnvrAttrAppleEvtMgrAvail =
			HaveGestaltAvail()
			&& (noErr == Gestalt(gestaltAppleEventsAttr, &reply))
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
			&& (noErr == Gestalt(gestaltDragMgrAttr, &reply))
			&& TestBit(reply, gestaltDragMgrPresent);
		HaveEnvrAttrDragMgrAvail = trueblnr;
	}
	return MyEnvrAttrDragMgrAvail;
}
