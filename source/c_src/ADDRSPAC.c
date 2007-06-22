/*
	ADDRSPAC.c

	Copyright (C) 2006 Bernd Schmidt, Philip Cummins, Paul C. Pratt

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
	ADDRess SPACe

	Implements the address space of the Mac Plus.

	This code is descended from code in vMac by Philip Cummins, in
	turn descended from code in the Un*x Amiga Emulator by
	Bernd Schmidt.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#include "MYOSGLUE.h"
#include "ENDIANAC.h"
#endif

#include "ADDRSPAC.h"

GLOBALVAR ui3b Wires[kNumWires];

IMPORTPROC EmulatedHardwareZap(void);

IMPORTPROC m68k_IPLchangeNtfy(void);
IMPORTPROC MINEM68K_Init(ui3b **BankReadAddr, ui3b **BankWritAddr,
	ui3b *fIPL);

IMPORTFUNC ui5b GetInstructionsRemaining(void);
IMPORTPROC SetInstructionsRemaining(ui5b n);

IMPORTPROC m68k_go_nInstructions(ui5b n);

IMPORTFUNC ui5b SCSI_Access(ui5b Data, blnr WriteMem, CPTR addr);
IMPORTFUNC ui5b SCC_Access(ui5b Data, blnr WriteMem, CPTR addr);
IMPORTFUNC ui5b IWM_Access(ui5b Data, blnr WriteMem, CPTR addr);
IMPORTFUNC ui5b VIA_Access(ui5b Data, blnr WriteMem, CPTR addr);
IMPORTPROC Sony_Access(ui5b Data, CPTR addr);

LOCALVAR blnr GotOneAbormal = falseblnr;

#ifndef ReportAbnormalInterrupt
#define ReportAbnormalInterrupt 0
#endif

#if DetailedAbormalReport
GLOBALPROC DoReportAbnormal(char *s)
#else
GLOBALPROC DoReportAbnormal(void)
#endif
{
	if (! GotOneAbormal) {
#if DetailedAbormalReport
		WarnMsgAbnormal(s);
#else
		WarnMsgAbnormal();
#endif
#if ReportAbnormalInterrupt
		SetInterruptButton(trueblnr);
#endif
		GotOneAbormal = trueblnr;
	}
}

IMPORTPROC SCC_Reset(void);

/* top 8 bits out of 32 are ignored, so total size of address space is 2 ** 24 bytes */

#define TotAddrBytes (1UL << ln2TotAddrBytes)
#define kAddrMask (TotAddrBytes - 1)

/* map of address space */

#define kROM_Overlay_Base 0x00000000 /* when overlay on */
#define kROM_Overlay_Top  0x00100000

#define kRAM_Base 0x00000000 /* when overlay off */
#define kRAM_Top  0x00400000

#define kROM_Base 0x00400000
#define kROM_Top  0x00500000

#define kSCSI_Block_Base 0x00580000
#define kSCSI_Block_Top  0x00600000

#define kRAM_Overlay_Base 0x00600000 /* when overlay on */
#define kRAM_Overlay_Top  0x00800000

#define kSCCRd_Block_Base 0x00800000
#define kSCCRd_Block_Top  0x00A00000

#define kSCCWr_Block_Base 0x00A00000
#define kSCCWr_Block_Top  0x00C00000

#define kIWM_Block_Base 0x00C00000
#define kIWM_Block_Top  0x00E00000

#define kVIA_Block_Base 0x00E80000
#define kVIA_Block_Top  0x00F00000

#define kDSK_Block_Base 0x00F40000
#define kDSK_Block_Top  0x00F40020

#define kAutoVector_Base 0x00FFFFF0
#define kAutoVector_Top  0x01000000

/* implementation of read/write for everything but RAM and ROM */

#define kSCCRdBase 0x9FFFF8
#define kSCCWrBase 0xBFFFF9

#define kSCC_Mask 0x03

#define kVIA_Mask 0x00000F
#define kVIA_Base 0xEFE1FE

#define kIWM_Mask 0x00000F /* Allocated Memory Bandwidth for IWM */
#define kIWM_Base 0xDFE1FF /* IWM Memory Base */

/* devide address space into banks, some of which are mapped to real memory */

LOCALVAR ui3b *BankReadAddr[NumMemBanks];
LOCALVAR ui3b *BankWritAddr[NumMemBanks];
	/* if BankWritAddr[i] != NULL then BankWritAddr[i] == BankReadAddr[i] */

#define ROMmem_mask (kROM_Size - 1)
#if CurEmu <= kEmu512Ke
#define ROM_CmpZeroMask 0
#elif CurEmu <= kEmuPlus
#define ROM_CmpZeroMask 0x00020000
#elif CurEmu <= kEmuClassic
#define ROM_CmpZeroMask 0
#else
#error "ROM_CmpZeroMask not defined"
#endif

#define Overlay_ROMmem_mask ROMmem_mask
#if CurEmu <= kEmu512Ke
#define Overlay_ROM_CmpZeroMask 0x00100000
#elif CurEmu <= kEmuPlus
#define Overlay_ROM_CmpZeroMask 0x00020000
#elif CurEmu <= kEmuClassic
#define Overlay_ROM_CmpZeroMask 0x00300000
#else
#error "Overlay_ROM_CmpZeroMask not defined"
#endif

#define RAMmem_mask (kRAM_Size - 1)
#if kRAM_Size >= 0x00200000
#define Overlay_RAMmem_offset 0x00200000
#if kRAM_Size == 0x00280000
#define Overlay_RAMmem_mask (0x00080000 - 1)
#else
#define Overlay_RAMmem_mask (0x00200000 - 1)
#endif
#else
#define Overlay_RAMmem_offset 0
#define Overlay_RAMmem_mask RAMmem_mask
#endif

LOCALPROC SetPtrVecToNULL(ui3b **x, ui5b n)
{
	int i;

	for (i = 0; i < n; i++) {
		*x++ = nullpr;
	}
}

LOCALPROC SetUpMemBanks(void)
{
	SetPtrVecToNULL(BankReadAddr, NumMemBanks);
	SetPtrVecToNULL(BankWritAddr, NumMemBanks);
}

LOCALFUNC blnr GetBankAddr(ui5b bi, blnr WriteMem, ui3b **ba)
{
	ui3b **CurBanks = WriteMem ? BankWritAddr : BankReadAddr;
	ui3b *ba0 = CurBanks[bi];

	if (ba0 == nullpr) {
		ui5b vMask = 0;
		ui3b *RealStart = nullpr;
		ui5b iAddr = bi << ln2BytesPerMemBank;
		if ((iAddr >> 22) == (kRAM_Base >> 22)) {
			if (MemOverlay) {
				if (WriteMem) {
					/* fail */
				} else if ((iAddr & Overlay_ROM_CmpZeroMask) != 0) {
					/* fail */
				} else {
					RealStart = (ui3b *)ROM;
					vMask = Overlay_ROMmem_mask;
				}
			} else {
#if kRAM_Size == 0x00280000
				if (iAddr >= 0x00200000) {
					RealStart = 0x00200000 + (ui3b *)RAM;
					vMask = 0x00080000 - 1;
				} else {
					RealStart = (ui3b *)RAM;
					vMask = 0x00200000 - 1;
				}
#else
				RealStart = (ui3b *)RAM;
				vMask = RAMmem_mask;
#endif
			}
		} else
		if ((iAddr >> 20) == (kROM_Base >> 20)) {
#if CurEmu >= kEmuSE1M
			if (MemOverlay != 0) {
				MemOverlay = 0;
				SetUpMemBanks();
			}
#endif
			if (WriteMem) {
				/* fail */
			} else if ((iAddr & ROM_CmpZeroMask) != 0) {
				/* fail */
			} else {
				RealStart = (ui3b *)ROM;
				vMask = ROMmem_mask;
			}
		} else
		if ((iAddr >> 19) == (kRAM_Overlay_Base >> 19)) {
			if (MemOverlay) {
				RealStart = Overlay_RAMmem_offset + (ui3b *)RAM;
				vMask = Overlay_RAMmem_mask;
			}
		} else
		{
			/* fail */
			/* ReportAbnormal("bad memory access"); */
		}
		if (RealStart == nullpr) {
			return falseblnr;
		} else {
			ba0 = RealStart + (iAddr & vMask);
			CurBanks[bi] = ba0;
		}
	}
	*ba = ba0;
	return trueblnr;
}

GLOBALFUNC ui5b MM_Access(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr)
{
	CPTR mAddressBus = addr & kAddrMask;

	if ((mAddressBus >> 19) == (kVIA_Block_Base >> 19)) {
		if (! ByteSize) {
			ReportAbnormal("access VIA word");
		} else if ((mAddressBus & 1) != 0) {
			ReportAbnormal("access VIA odd");
		} else {
			if ((mAddressBus & 0x000FE1FE) != 0x000FE1FE) {
				ReportAbnormal("access VIA nonstandard address");
			}
			Data = VIA_Access(Data, WriteMem, (mAddressBus >> 9) & kVIA_Mask);
		}
	} else
	if ((mAddressBus >> 22) == (kSCCRd_Block_Base >> 22)) {
#if CurEmu >= kEmuSE1M
		if ((mAddressBus & 0x00100000) == 0) {
			ReportAbnormal("access SCC unassigned address");
		} else
#endif
		if (! ByteSize) {
			ReportAbnormal("Attemped Phase Adjust");
		} else if (WriteMem != ((mAddressBus & 1) != 0)) {
			if (WriteMem) {
#if CurEmu > kEmu512K
				ReportAbnormal("access SCC even/odd");
				/*
					This happens on boot with 64k ROM.
				*/
#endif
			} else {
				SCC_Reset();
			}
		} else if (WriteMem != (mAddressBus >= kSCCWr_Block_Base)) {
			ReportAbnormal("access SCC wr/rd base wrong");
		} else {
			if ((mAddressBus & 0x001FFFF8) != 0x001FFFF8) {
				ReportAbnormal("access SCC nonstandard address");
			}
			Data = SCC_Access(Data, WriteMem, (mAddressBus >> 1) & kSCC_Mask);
		}
	} else
	if ((mAddressBus >> 5) == (kDSK_Block_Base >> 5)) {
		if (ByteSize) {
			ReportAbnormal("access Sony byte");
		} else if ((mAddressBus & 1) != 0) {
			ReportAbnormal("access Sony odd");
		} else if (! WriteMem) {
			ReportAbnormal("access Sony read");
		} else {
			Sony_Access(Data, (mAddressBus >> 1) & 0x0F);
		}
	} else
	if ((mAddressBus >> 19) == (kSCSI_Block_Base >> 19)) {
		if (! ByteSize) {
			ReportAbnormal("access SCSI word");
		} else if (WriteMem != ((mAddressBus & 1) != 0)) {
			ReportAbnormal("access SCSI even/odd");
		} else {
			Data = SCSI_Access(Data, WriteMem, (mAddressBus >> 4) & 0x07);
		}
	} else
	if ((mAddressBus >> 21) == (kIWM_Block_Base >> 21)) {
#if CurEmu >= kEmuSE1M
		if ((mAddressBus & 0x00100000) == 0) {
			ReportAbnormal("access IWM unassigned address");
		} else
#endif
		if (! ByteSize) {
#if ExtraAbormalReports
			ReportAbnormal("access IWM word");
			/*
				This happens when quitting 'Glider 3.1.2'.
				perhaps a bad handle is being disposed of.
			*/
#endif
		} else if ((mAddressBus & 1) == 0) {
			ReportAbnormal("access IWM even");
		} else {
			if ((mAddressBus & 0x001FE1FF) != 0x001FE1FF) {
				ReportAbnormal("access IWM nonstandard address");
			}
			Data = IWM_Access(Data, WriteMem, (mAddressBus >> 9) & kIWM_Mask);
		}
	} else
	{
		ui3b *ba;

		if (GetBankAddr(bankindex(mAddressBus), WriteMem, &ba)) {
			ui3p m = (mAddressBus & MemBankAddrMask) + ba;
			if (ByteSize) {
				if (WriteMem) {
					*m = Data;
				} else {
					Data = (si5b)(si3b)*m;
				}
			} else {
				if (WriteMem) {
					do_put_mem_word(m, Data);
				} else {
					Data = (si5b)(si4b)do_get_mem_word(m);
				}
			}
		}
	}
	return Data;
}

GLOBALPROC MemOverlay_ChangeNtfy(void)
{
#if CurEmu <= kEmuPlus
	SetUpMemBanks();
#endif
}


/*
	unlike in the real Mac Plus, Mini vMac
	will allow misaligned memory access,
	since it is easier to allow it than
	it is to correctly simulate a bus error
	and back out of the current instruction.
*/

GLOBALFUNC ui3b *get_real_address(ui5b L, blnr WritableMem, CPTR addr)
{
	ui5b bi = bankindex(addr);
	ui3b *ba;

	if (! GetBankAddr(bi, WritableMem, &ba)) {
		ReportAbnormal("get_real_address fails");
		return nullpr;
	} else {
		ui5b bankoffset = addr & MemBankAddrMask;
		ui3b *p = (ui3b *)(bankoffset + ba);
		ui5b bankleft = BytesPerMemBank - bankoffset;
label_1:
		if (bankleft >= L) {
			return p; /* ok */
		} else {
			ui3b *bankend = (ui3b *)(BytesPerMemBank + ba);

			bi = (bi + 1) & MemBanksMask;
			if ((! GetBankAddr(bi, WritableMem, &ba))
				|| (ba != bankend))
			{
				ReportAbnormal("get_real_address falls off");
				return nullpr;
			} else {
				L -= bankleft;
				bankleft = BytesPerMemBank;
				goto label_1;
			}
		}
	}
}

GLOBALVAR blnr InterruptButton = falseblnr;

GLOBALPROC SetInterruptButton(blnr v)
{
	if (InterruptButton != v) {
		InterruptButton = v;
		VIAorSCCinterruptChngNtfy();
	}
}

LOCALVAR ui3b CurIPL = 0;

GLOBALPROC VIAorSCCinterruptChngNtfy(void)
{
	ui3b VIAandNotSCC = VIAInterruptRequest
		& ~ SCCInterruptRequest;
	ui3b NewIPL = VIAandNotSCC
		| (SCCInterruptRequest << 1)
		| (InterruptButton << 2);
	if (NewIPL != CurIPL) {
		CurIPL = NewIPL;
		m68k_IPLchangeNtfy();
	}
}

GLOBALFUNC blnr AddrSpac_Init(void)
{
	int i;

	for (i = 0; i < kNumWires; i++) {
		Wires[i] = 1;
	}

	MINEM68K_Init(BankReadAddr, BankWritAddr,
		&CurIPL);
	EmulatedHardwareZap();
	return trueblnr;
}

#if CurEmu <= kEmuPlus
IMPORTPROC DoKybd_ReceiveEndCommand(void);
IMPORTPROC DoKybd_ReceiveCommand(void);
#else
IMPORTPROC ADB_DoNewState(void);
#endif
IMPORTPROC VIA_DoTimer1Check(void);
IMPORTPROC VIA_DoTimer2Check(void);

LOCALPROC ICT_DoTask(int taskid)
{
	switch (taskid) {
#if CurEmu <= kEmuPlus
		case kICT_Kybd_ReceiveEndCommand:
			DoKybd_ReceiveEndCommand();
			break;
		case kICT_Kybd_ReceiveCommand:
			DoKybd_ReceiveCommand();
			break;
#else
		case kICT_ADB_NewState:
			ADB_DoNewState();
			break;
#endif
		case kICT_VIA_Timer1Check:
			VIA_DoTimer1Check();
			break;
		case kICT_VIA_Timer2Check:
			VIA_DoTimer2Check();
			break;
		default:
			ReportAbnormal("unknown taskid in ICT_DoTask");
			break;
	}
}

#ifdef _VIA_Debug
#include <stdio.h>
#endif

LOCALVAR blnr ICTactive[kNumICTs];
LOCALVAR iCountt ICTwhen[kNumICTs];

LOCALPROC ICT_Zap(void)
{
	int i;

	for (i = 0; i < kNumICTs; i++) {
		ICTactive[i] = falseblnr;
	}
}

LOCALPROC InsertICT(int taskid, iCountt when)
{
	ICTwhen[taskid] = when;
	ICTactive[taskid] = trueblnr;
}

LOCALVAR iCountt NextiCount = 0;

GLOBALFUNC iCountt GetCuriCount(void)
{
	return NextiCount - GetInstructionsRemaining();
}

GLOBALPROC ICT_add(int taskid, ui5b n)
{
	/* n must be > 0 */
	ui5b x = GetInstructionsRemaining();
	ui5b when = NextiCount - x + n;

#ifdef _VIA_Debug
	fprintf(stderr, "ICT_add: %d, %d, %d\n", when, taskid, n);
#endif
	InsertICT(taskid, when);

	if (x > n) {
		SetInstructionsRemaining(n);
		NextiCount = when;
	}
}

LOCALPROC ICT_DoCurrentTasks(void)
{
	int i;

	for (i = 0; i < kNumICTs; i++) {
		if (ICTactive[i]) {
			if (ICTwhen[i] == NextiCount) {
				ICTactive[i] = falseblnr;
#ifdef _VIA_Debug
				fprintf(stderr, "doing task %d, %d\n", NextiCount, i);
#endif
				ICT_DoTask(i);

				/*
					A Task may set the time of
					any task, including itself.
					But it cannot set any task
					to execute immediately, so
					one pass is sufficient.
				*/
			}
		}
	}
}

LOCALFUNC ui5b ICT_DoGetNext(ui5b maxn)
{
	int i;
	ui5b v = maxn;

	for (i = 0; i < kNumICTs; i++) {
		if (ICTactive[i]) {
			ui5b d = ICTwhen[i] - NextiCount;
			/* at this point d must be > 0 */
			if (d < v) {
#ifdef _VIA_Debug
				fprintf(stderr, "coming task %d, %d, %d\n", NextiCount, i, d);
#endif
				v = d;
			}
		}
	}
	return v;
}

GLOBALPROC m68k_go_nInstructions_1(ui5b n)
{
	ui5b n2;
	ui5b StopiCount = NextiCount + n;
	do {
		ICT_DoCurrentTasks();
		n2 = ICT_DoGetNext(n);
		NextiCount += n2;
		m68k_go_nInstructions(n2);
		n = StopiCount - NextiCount;
	} while (n != 0);
}

GLOBALPROC Memory_Reset(void)
{
	MemOverlay = 1;
	SetUpMemBanks();
	ICT_Zap();
}
