/*
	SONYEMDV.c
	
	Copyright (C) 2001 Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

/*
	SONY floppy disk EMulated DeVice
	
	This is now misnamed. The only code remaining in this file
	implements a fake memory map deviced that is used by
	a replacement for the Sony disk driver. The Sony hardware
	is not emulated.
*/

#include "SYSDEPNS.h"

#include "SONYEMDV.h"

#include "ENDIANAC.h"
#include "ADDRSPAC.h"
#include "MINEM68K.h"
#include "OSCOMVAR.h"

extern WORD vSonyRead(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count);
extern WORD vSonyWrite(void *Buffer, UWORD Drive_No, ULONG Sony_Start, ULONG *Sony_Count);
extern WORD vSonyEject(UWORD Drive_No);
extern Boolean AnyDiskInserted(void);
extern WORD vSonyVerify(UWORD Drive_No);
extern WORD vSonyFormat(UWORD Drive_No);
extern WORD vSonyGetSize(UWORD Drive_No, ULONG *Sony_Count);
extern Boolean vSonyDiskLocked(UWORD Drive_No);
extern void SetProgramDone(void);

#define kDSK_Block_Base 0x00F40000

#define kDSK_Command 0
#define kDSK_Err 1
#define kDSK_Drive_No 2
#define kDSK_QuitOnEject 3
#define kDSK_Start_Hi 4
#define kDSK_Start_Lo 5
#define kDSK_Count_Hi 6
#define kDSK_Count_Lo 7
#define kDSK_Buffer_Hi 8
#define kDSK_Buffer_Lo 9
#define kDSK_CallBack_Hi 10
#define kDSK_CallBack_Lo 11
#define kDSK_numvars 12

#define kDSKCmdRead 0
#define kDSKCmdWrite 1
#define kDSKCmdEject 2
#define kDSKCmdVerify 3
#define kDSKCmdFormat 4
#define kDSKCmdGetSize 5

#define MinTicksBetweenInsert 60
	/* if call PostEvent too frequently, insert events seems to get lost */

UWORD kDSK_Var[kDSK_numvars];
ULONG MountPending;
UWORD DelayUntilNextInsert;

// This checks every VBL to see if the disk has been read
void Sony_Update (void)
{
	if (DelayUntilNextInsert != 0) {
		--DelayUntilNextInsert;
	} else {
		if (MountPending != 0) {
			CPTR CallBack = do_get_mem_long((ULONG *)&kDSK_Var[kDSK_CallBack_Hi]);

			if (CallBack != 0) {
				int i;
				ULONG data;
				
				for (i = 0; i < kDSK_numvars; ++i) {
					if ((MountPending & ((ULONG)1 << i)) != 0) {
						MountPending &= ~((ULONG)1 << i);
						DelayUntilNextInsert = MinTicksBetweenInsert;
						data = i;
						if (vSonyDiskLocked(i)) {
							data |= (ULONG)0x00FF << 16;
						}
						DiskInsertedPsuedoException(CallBack, data);
						return;
					}
				}

			}    
		}
	}
}

void vSonyInsertNotify (UWORD Drive_No)
{
	MountPending |= ((ULONG)1 << Drive_No);
}

extern ULONG DataBus;
extern Boolean ByteSizeAccess;
extern Boolean WriteMemAccess;

void Sony_Access(CPTR addr)
{
	if (! ByteSizeAccess) {
		if ((addr & 1) == 0) {
			if (addr < (2 * kDSK_numvars)) {
				if (WriteMemAccess) {
					do_put_mem_word(&kDSK_Var[addr >> 1], DataBus);
					if (addr == kDSK_Command) {
						/* this is a command */
						WORD result;
						UWORD Drive_No = do_get_mem_word(&kDSK_Var[kDSK_Drive_No]);

						switch (DataBus) {
							case kDSKCmdRead:
								{
									ULONG Sony_Start = do_get_mem_long((ULONG *)&kDSK_Var[kDSK_Start_Hi]);
									ULONG Sony_Count = do_get_mem_long((ULONG *)&kDSK_Var[kDSK_Count_Hi]);
									CPTR Buffera = do_get_mem_long((ULONG *)&kDSK_Var[kDSK_Buffer_Hi]);
									void *Buffer = (void*)get_real_address(Buffera);
									result = vSonyRead(Buffer, Drive_No, Sony_Start, &Sony_Count);
									do_put_mem_long((ULONG *)&kDSK_Var[kDSK_Count_Hi], Sony_Count);
								}
								break;
							case kDSKCmdWrite:
								{
									ULONG Sony_Start = do_get_mem_long((ULONG *)&kDSK_Var[kDSK_Start_Hi]);
									ULONG Sony_Count = do_get_mem_long((ULONG *)&kDSK_Var[kDSK_Count_Hi]);
									CPTR Buffera = do_get_mem_long((ULONG *)&kDSK_Var[kDSK_Buffer_Hi]);
									void *Buffer = (void*)get_real_address(Buffera);
									result = vSonyWrite(Buffer, Drive_No, Sony_Start, &Sony_Count);
									do_put_mem_long((ULONG *)&kDSK_Var[kDSK_Count_Hi], Sony_Count);
								}
								break;
							case kDSKCmdEject:
								result = vSonyEject(Drive_No);
								if (do_get_mem_word(&kDSK_Var[kDSK_QuitOnEject]) != 0) {
									if (! AnyDiskInserted()) {
										SetProgramDone();
									}
								}
								break;
							case kDSKCmdVerify:
								result = vSonyVerify(Drive_No);
								break;
							case kDSKCmdFormat:
								result = vSonyFormat(Drive_No);
								break;
							case kDSKCmdGetSize:
								{
									ULONG Sony_Count;
									result = vSonyGetSize(Drive_No, &Sony_Count);
									do_put_mem_long((ULONG *)&kDSK_Var[kDSK_Count_Hi], Sony_Count);
								}
								break;
						}
						do_put_mem_word((UWORD *)&kDSK_Var[kDSK_Err], result);
					}
				} else {
					DataBus = do_get_mem_word(&kDSK_Var[addr >> 1]);
				}
			}
		}
	}
}

void Sony_Reset (void)
{
	int i;
	
	for (i = 0; i < kDSK_numvars; ++i) {
		kDSK_Var[i] = 0;
	}
	MountPending = 0;
	DelayUntilNextInsert = 0;
}
