/*
	ADDRSPAC.c

	Copyright (C) 2004 Bernd Schmidt, Philip Cummins, Paul Pratt

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
#include "MINEM68K.h"
#endif

#include "ADDRSPAC.h"

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
		MacMsg("Abnormal Situation",
#if DetailedAbormalReport
			s,
#else
			"The emulated machine is attempting an operation that wasn't expected to happen in normal use.",
#endif
			falseblnr);
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

LOCALVAR ui3b vOverlay;

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

GLOBALFUNC ui5b MM_Access(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr)
{
	CPTR mAddressBus = addr & kAddrMask;

	if (mAddressBus < kIWM_Block_Base) {
		if (mAddressBus < kSCCRd_Block_Base) {
#if CurEmu >= kEmuPlus1M
			if ((mAddressBus >= kSCSI_Block_Base) && (mAddressBus < kSCSI_Block_Top)) {
				if (! ByteSize) {
					ReportAbnormal("access SCSI word");
				} else if (WriteMem != ((mAddressBus & 1) != 0)) {
					ReportAbnormal("access SCSI even/odd");
				} else {
					Data = SCSI_Access(Data, WriteMem, (mAddressBus >> 4) & 0x07);
				}
			}
#endif
		} else {
			if (! ByteSize) {
				ReportAbnormal("Attemped Phase Adjust");
			} else if (WriteMem != ((mAddressBus & 1) != 0)) {
				if (WriteMem) {
					ReportAbnormal("access SCC even/odd");
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
		}
	} else {
		if (mAddressBus < kVIA_Block_Base) {
			if (/* (mAddressBus >= kIWM_Block_Base) && */(mAddressBus < kIWM_Block_Top)) {
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
			}
		} else {
			if (/* (mAddressBus >= kVIA_Block_Base) && */(mAddressBus < kVIA_Block_Top)) {
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
			} else {
				if ((mAddressBus >= kDSK_Block_Base) && (mAddressBus < kDSK_Block_Top)) {
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
				if ((mAddressBus >= kAutoVector_Base) && (mAddressBus < kAutoVector_Top)) {
					/* SetAutoVector(); */
					/* Exception(regs.intmask+24, 0); */
				}
			}
		}
	}
	return Data;
}

/* devide address space into banks, some of which are mapped to real memory */

GLOBALVAR ui3b *BankReadAddr[NumMemBanks];
GLOBALVAR ui3b *BankWritAddr[NumMemBanks];

#define ROMmem_mask (kROM_Size - 1)
#if CurEmu >= kEmuPlus1M
#define ROM_CmpZeroMask 0x00020000
#else
#define ROM_CmpZeroMask 0
#endif

#define Overlay_ROMmem_mask ROMmem_mask
#if CurEmu >= kEmuPlus1M
#define Overlay_ROM_CmpZeroMask 0x00020000
#else
#define Overlay_ROM_CmpZeroMask 0x00100000
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

LOCALPROC SetUpBankRange(ui5b StartAddr, ui5b StopAddr, ui3b * RealStart, ui5b vMask, ui5b CmpZeroMask, blnr Writeable)
{
	ui5b i;
	ui5b iAddr;
	ui5b StartBank = StartAddr >> ln2BytesPerMemBank;
	ui5b StopBank = StopAddr >> ln2BytesPerMemBank;

	for (i = StartBank; i < StopBank; i++) {
		iAddr = i << ln2BytesPerMemBank;
		if ((iAddr & CmpZeroMask) == 0) {
			BankReadAddr[i] = RealStart + (iAddr & vMask);
			if (Writeable) {
				BankWritAddr[i] = BankReadAddr[i];
			}
		}
	}
}

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

	SetUpBankRange(kROM_Base, kROM_Top, (ui3b *) ROM, ROMmem_mask, ROM_CmpZeroMask, falseblnr);

	if (vOverlay) {
		SetUpBankRange(kROM_Overlay_Base, kROM_Overlay_Top, (ui3b *)ROM, Overlay_ROMmem_mask,
			Overlay_ROM_CmpZeroMask, falseblnr);
		SetUpBankRange(kRAM_Overlay_Base, kRAM_Overlay_Top,
			Overlay_RAMmem_offset + (ui3b *)RAM, Overlay_RAMmem_mask,
			0, trueblnr);
	} else {
#if kRAM_Size == 0x00280000
		SetUpBankRange(kRAM_Base, 0x00200000, (ui3b *)RAM, 0x00200000 - 1, 0, trueblnr);
		SetUpBankRange(0x00200000, kRAM_Top, 0x00200000 + (ui3b *)RAM, 0x00080000 - 1, 0, trueblnr);
#else
		SetUpBankRange(kRAM_Base, kRAM_Top, (ui3b *)RAM, RAMmem_mask, 0, trueblnr);
#endif
	}
}

GLOBALPROC ZapNMemoryVars(void)
{
	vOverlay = 2; /* MemBanks uninitialized */
}

GLOBALPROC VIA_PORA4(ui3b Data)
{
	if (vOverlay != Data) {
		vOverlay = Data;
		SetUpMemBanks();
	}
}

GLOBALFUNC ui3b VIA_GORA4(void) /* Overlay/Normal Memory Mapping */
{
#ifdef _VIA_Interface_Debug
	printf("VIA ORA4 attempts to be an input\n");
#endif
	return 0;
}

GLOBALPROC Memory_Reset(void)
{
	VIA_PORA4(1);
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
	ui3b **CurBanks = WritableMem ? BankWritAddr : BankReadAddr;
	ui5b bi = bankindex(addr);
	ui3b *ba = CurBanks[bi];

	if (ba == nullpr) {
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
			ba = CurBanks[bi];
			if (ba != bankend) {
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

extern blnr VIAInterruptRequest;
extern blnr SCCInterruptRequest;

LOCALVAR ui3b CurIPL = 0;

GLOBALPROC VIAorSCCinterruptChngNtfy(void)
{
	blnr VIAandNotSCC = VIAInterruptRequest
		&& ! SCCInterruptRequest;
	blnr NewIPL = (VIAandNotSCC ? 1 : 0)
		| (SCCInterruptRequest ? 2 : 0)
		| (InterruptButton? 4 : 0);
	if (NewIPL != CurIPL) {
		CurIPL = NewIPL;
		m68k_IPLchangeNtfy();
	}
}

GLOBALFUNC blnr AddrSpac_Init(void)
{
	Memory_Reset();
	MINEM68K_Init(BankReadAddr, BankWritAddr,
		&CurIPL);
	return trueblnr;
}
