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
#include "EMCONFIG.h"
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
IMPORTFUNC ui5b VIA1_Access(ui5b Data, blnr WriteMem, CPTR addr);
#if EmVIA2
IMPORTFUNC ui5b VIA2_Access(ui5b Data, blnr WriteMem, CPTR addr);
#endif
#if (CurEmMd == kEmMd_PB100) || (CurEmMd == kEmMd_II)
IMPORTFUNC ui5b ASC_Access(ui5b Data, blnr WriteMem, CPTR addr);
#endif
IMPORTPROC Sony_Access(ui5b Data, CPTR addr);

LOCALVAR blnr GotOneAbnormal = falseblnr;

#ifndef ReportAbnormalInterrupt
#define ReportAbnormalInterrupt 0
#endif

#if DetailedAbnormalReport
GLOBALPROC DoReportAbnormal(char *s)
#else
GLOBALPROC DoReportAbnormal(void)
#endif
{
	if (! GotOneAbnormal) {
#if DetailedAbnormalReport
		WarnMsgAbnormal(s);
#else
		WarnMsgAbnormal();
#endif
#if ReportAbnormalInterrupt
		SetInterruptButton(trueblnr);
#endif
		GotOneAbnormal = trueblnr;
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
#if (CurEmMd == kEmMd_PB100) || (CurEmMd == kEmMd_II)
#define kRAM_ln2Spc 23
#else
#define kRAM_ln2Spc 22
#endif

#if CurEmMd == kEmMd_PB100
#define kROM_Base 0x00900000
#define kROM_ln2Spc 20
#elif CurEmMd == kEmMd_II
#define kROM_Base 0x00800000
#define kROM_ln2Spc 20
#else
#define kROM_Base 0x00400000
#define kROM_ln2Spc 20
#endif

#if IncludeVidMem
#if CurEmMd == kEmMd_PB100
#define kVidMem_Base 0x00FA0000
#define kVidMem_ln2Spc 16
#else
#define kVidMem_Base 0x00540000
#define kVidMem_ln2Spc 18
#endif
#endif

#if CurEmMd == kEmMd_PB100
#define kSCSI_Block_Base 0x00F90000
#define kSCSI_ln2Spc 16
#else
#define kSCSI_Block_Base 0x00580000
#define kSCSI_ln2Spc 19
#endif

#define kRAM_Overlay_Base 0x00600000 /* when overlay on */
#define kRAM_Overlay_Top  0x00800000

#if CurEmMd == kEmMd_PB100
#define kSCCRd_Block_Base 0x00FD0000
#define kSCC_ln2Spc 16
#else
#define kSCCRd_Block_Base 0x00800000
#define kSCC_ln2Spc 22
#endif

#if CurEmMd != kEmMd_PB100
#define kSCCWr_Block_Base 0x00A00000
#define kSCCWr_Block_Top  0x00C00000
#endif

#if CurEmMd == kEmMd_PB100
#define kIWM_Block_Base 0x00F60000
#define kIWM_ln2Spc 16
#else
#define kIWM_Block_Base 0x00C00000
#define kIWM_ln2Spc 21
#endif

#if CurEmMd == kEmMd_PB100
#define kVIA1_Block_Base 0x00F70000
#define kVIA1_ln2Spc 16
#else
#define kVIA1_Block_Base 0x00E80000
#define kVIA1_ln2Spc 19
#endif

#if CurEmMd == kEmMd_PB100
#define kASC_Block_Base 0x00FB0000
#define kASC_ln2Spc 16
#endif
#define kASC_Mask 0x00000FFF

#define kDSK_Block_Base 0x00F40000
#define kDSK_ln2Spc 5

/* implementation of read/write for everything but RAM and ROM */

#define kSCC_Mask 0x03

#define kVIA1_Mask 0x00000F
#if CurEmMd == kEmMd_II
#define kVIA2_Mask 0x00000F
#endif

#define kIWM_Mask 0x00000F /* Allocated Memory Bandwidth for IWM */

/* devide address space into banks, some of which are mapped to real memory */

LOCALVAR ui3b *BankReadAddr[NumMemBanks];
LOCALVAR ui3b *BankWritAddr[NumMemBanks];
	/* if BankWritAddr[i] != NULL then BankWritAddr[i] == BankReadAddr[i] */

#define ROMmem_mask (kROM_Size - 1)
#if CurEmMd <= kEmMd_512Ke
#define ROM_CmpZeroMask 0
#elif CurEmMd <= kEmMd_Plus
#define ROM_CmpZeroMask 0x00020000
#elif CurEmMd <= kEmMd_PB100
#define ROM_CmpZeroMask 0
#elif CurEmMd <= kEmMd_II
#define ROM_CmpZeroMask 0
#else
#error "ROM_CmpZeroMask not defined"
#endif

#define Overlay_ROMmem_mask ROMmem_mask
#if CurEmMd <= kEmMd_512Ke
#define Overlay_ROM_CmpZeroMask 0x00100000
#elif CurEmMd <= kEmMd_Plus
#define Overlay_ROM_CmpZeroMask 0x00020000
#elif CurEmMd <= kEmMd_Classic
#define Overlay_ROM_CmpZeroMask 0x00300000
#elif CurEmMd <= kEmMd_PB100
#define Overlay_ROM_CmpZeroMask 0
#elif CurEmMd <= kEmMd_II
#define Overlay_ROM_CmpZeroMask 0
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
	ui5b i;

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
	ui3p ba0 = CurBanks[bi];

	if (ba0 == nullpr) {
		ui5b vMask = 0;
		ui3p RealStart = nullpr;
		ui5b iAddr = bi << ln2BytesPerMemBank;
		if ((iAddr >> kRAM_ln2Spc) == (kRAM_Base >> kRAM_ln2Spc)) {
			if (MemOverlay) {
#if CurEmMd == kEmMd_MacII
				ReportAbnormal("Overlay with 24 bit addressing");
#else
				if (WriteMem) {
					/* fail */
				} else if ((iAddr & Overlay_ROM_CmpZeroMask) != 0) {
					/* fail */
				} else {
					RealStart = ROM;
					vMask = Overlay_ROMmem_mask;
				}
#endif
			} else {
#if kRAM_Size == 0x00280000
				if (iAddr >= 0x00200000) {
					RealStart = 0x00200000 + RAM;
					vMask = 0x00080000 - 1;
				} else {
					RealStart = RAM;
					vMask = 0x00200000 - 1;
				}
#else
				RealStart = RAM;
				vMask = RAMmem_mask;
#endif
			}
		} else
#if IncludeVidMem
		if ((iAddr >> kVidMem_ln2Spc) == (kVidMem_Base >> kVidMem_ln2Spc)) {
			RealStart = VidMem;
			vMask = kVidMemRAM_Size - 1;
		} else
#endif
		if ((iAddr >> kROM_ln2Spc) == (kROM_Base >> kROM_ln2Spc)) {
#if CurEmMd == kEmMd_MacII
			if (MemOverlay) {
				ReportAbnormal("Overlay with 24 bit addressing");
			}
#elif CurEmMd >= kEmMd_SE
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
				RealStart = ROM;
				vMask = ROMmem_mask;
			}
		} else
#if CurEmMd != kEmMd_MacII
		if ((iAddr >> 19) == (kRAM_Overlay_Base >> 19)) {
			if (MemOverlay) {
				RealStart = Overlay_RAMmem_offset + RAM;
				vMask = Overlay_RAMmem_mask;
			}
		} else
#endif
		{
			/* fail */
#if CurEmMd == kEmMd_MacII
			ReportAbnormal("bad memory access");
#endif
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

#if CurEmMd == kEmMd_II
LOCALFUNC ui5b MM_IOAccess(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr)
{
	if ((addr >= 0) && (addr < 0x2000)) {
		if (! ByteSize) {
			ReportAbnormal("access VIA1 word");
		} else if ((addr & 1) != 0) {
			ReportAbnormal("access VIA1 odd");
		} else {
			if ((addr & 0x000001FE) != 0x00000000) {
				ReportAbnormal("access VIA1 nonstandard address");
			}
			Data = VIA1_Access(Data, WriteMem, (addr >> 9) & kVIA1_Mask);
		}
	} else
	if ((addr >= 0x2000) && (addr < 0x4000)) {
		if (! ByteSize) {
			if ((! WriteMem) && ((0x3e00 == addr) || (0x3e02 == addr))) {
				/* for weirdness at offset 0x71E in ROM */
				Data = (VIA2_Access(Data, WriteMem, (addr >> 9) & kVIA2_Mask) << 8)
					| VIA2_Access(Data, WriteMem, (addr >> 9) & kVIA2_Mask);

			} else {
				ReportAbnormal("access VIA2 word");
			}
		} else if ((addr & 1) != 0) {
			if (0x3FFF == addr) {
				/* for weirdness at offset 0x7C4 in ROM. looks like bug. */
				Data = VIA2_Access(Data, WriteMem, (addr >> 9) & kVIA2_Mask);
			} else {
				ReportAbnormal("access VIA2 odd");
			}
		} else {
			if ((addr & 0x000001FE) != 0x00000000) {
				ReportAbnormal("access VIA2 nonstandard address");
			}
			Data = VIA2_Access(Data, WriteMem, (addr >> 9) & kVIA2_Mask);
		}
	} else
	if ((addr >= 0x4000) && (addr < 0x6000)) {
		if (! ByteSize) {
			ReportAbnormal("Attemped Phase Adjust");
		} else
		{
			if ((addr & 0x1FF9) != 0x00000000) {
				ReportAbnormal("access SCC nonstandard address");
			}
			Data = SCC_Access(Data, WriteMem, (addr >> 1) & kSCC_Mask);
		}
	} else
	if ((addr >= 0x10000) && (addr < 0x12000)) {
		if (! ByteSize) {
			ReportAbnormal("access SCSI word");
		} else {
			if ((addr & 0x1F8F) != 0x00000000) {
				ReportAbnormal("access SCC nonstandard address");
			}
			Data = SCSI_Access(Data, WriteMem, (addr >> 4) & 0x07);
		}
	} else
	if ((addr >= 0x14000) && (addr < 0x16000)) {
		if (! ByteSize) {
			if (WriteMem) {
				(void) ASC_Access((Data >> 8) & 0x00FF, WriteMem, addr & kASC_Mask);
				Data = ASC_Access((Data) & 0x00FF, WriteMem, (addr + 1) & kASC_Mask);
			} else {
				ReportAbnormal("access ASC word");
			}
		} else {
			Data = ASC_Access(Data, WriteMem, addr & kASC_Mask);
		}
	} else
	if ((addr >= 0x16000) && (addr < 0x18000)) {
		if (! ByteSize) {
			ReportAbnormal("access IWM word");
		} else if ((addr & 1) != 0) {
			ReportAbnormal("access IWM odd");
		} else {
			Data = IWM_Access(Data, WriteMem, (addr >> 9) & kIWM_Mask);
		}
	} else
#if 0
	if ((addr >= 0x1C000) && (addr < 0x1E000)) {
		if (! ByteSize) {
			ReportAbnormal("access SCSI word");
		} else if (WriteMem != ((addr & 1) != 0)) {
			ReportAbnormal("access SCSI even/odd");
		} else {
			/* fail, nothing supposed to be here, but rom accesses it anyway */
			if ((addr != 0x1DA00) && (addr != 0x1DC00)) {
				ReportAbnormal("another unknown access");
			}
		}
	} else
#endif
	{
		ReportAbnormal("MM_Access fails");
	}
	return Data;
}
#endif

#if CurEmMd == kEmMd_II
GLOBALFUNC ui5b MM_Access(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr)
{
	if (Addr32) {
		if ((addr >= 0x50000000) && (addr < 0x51000000)) {
			Data = MM_IOAccess(Data, WriteMem, ByteSize, addr & 0x1FFFF);
		} else
		if ((addr >= 0x58000000) && (addr < 0x58000004)) {
			/* test hardware. fail */
		} else
		if (addr >= 0xF1000000) {
			/* Standard NuBus space */
		} else
		{
			ui3p m;

			if (ByteSize) {
				m = get_real_address(1, WriteMem, addr);
				if (m != nullpr) {
					if (WriteMem) {
						*m = Data;
					} else {
						Data = (si5b)(si3b)*m;
					}
				}
			} else {
				m = get_real_address(2, WriteMem, addr);
				if (m != nullpr) {
					if (WriteMem) {
						do_put_mem_word(m, Data);
					} else {
						Data = (si5b)(si4b)do_get_mem_word(m);
					}
				}
			}
		}
	} else {
	CPTR mAddressBus = addr & kAddrMask;

	if (mAddressBus >= 0x00F00000) {
		if ((mAddressBus >> kDSK_ln2Spc) == (kDSK_Block_Base >> kDSK_ln2Spc)) {
			if (ByteSize) {
				ReportAbnormal("access Sony byte");
			} else if ((mAddressBus & 1) != 0) {
				ReportAbnormal("access Sony odd");
			} else if (! WriteMem) {
				ReportAbnormal("access Sony read");
			} else {
				Sony_Access(Data, (mAddressBus >> 1) & 0x0F);
			}
		} else {
			Data = MM_IOAccess(Data, WriteMem, ByteSize, mAddressBus & 0x1FFFF);
		}
	} else {
		ui3p ba;

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
	}
	return Data;
}
#else
GLOBALFUNC ui5b MM_Access(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr)
{
	CPTR mAddressBus = addr & kAddrMask;

	if ((mAddressBus >> kVIA1_ln2Spc) == (kVIA1_Block_Base >> kVIA1_ln2Spc)) {
		if (! ByteSize) {
			ReportAbnormal("access VIA word");
		} else if ((mAddressBus & 1) != 0) {
			ReportAbnormal("access VIA odd");
		} else {
#if CurEmMd != kEmMd_PB100
			if ((mAddressBus & 0x000FE1FE) != 0x000FE1FE) {
				ReportAbnormal("access VIA nonstandard address");
			}
#endif
			Data = VIA1_Access(Data, WriteMem, (mAddressBus >> 9) & kVIA1_Mask);
		}
	} else
	if ((mAddressBus >> kSCC_ln2Spc) == (kSCCRd_Block_Base >> kSCC_ln2Spc)) {
#if CurEmMd >= kEmMd_SE
		if ((mAddressBus & 0x00100000) == 0) {
			ReportAbnormal("access SCC unassigned address");
		} else
#endif
		if (! ByteSize) {
			ReportAbnormal("Attemped Phase Adjust");
		} else if (WriteMem != ((mAddressBus & 1) != 0)) {
			if (WriteMem) {
#if CurEmMd >= kEmMd_512Ke
#if CurEmMd != kEmMd_PB100
				ReportAbnormal("access SCC even/odd");
				/*
					This happens on boot with 64k ROM.
				*/
#endif
#endif
			} else {
				SCC_Reset();
			}
		} else
#if CurEmMd != kEmMd_PB100
		if (WriteMem != (mAddressBus >= kSCCWr_Block_Base)) {
			ReportAbnormal("access SCC wr/rd base wrong");
		} else
#endif
		{
#if CurEmMd != kEmMd_PB100
			if ((mAddressBus & 0x001FFFF8) != 0x001FFFF8) {
				ReportAbnormal("access SCC nonstandard address");
			}
#endif
			Data = SCC_Access(Data, WriteMem, (mAddressBus >> 1) & kSCC_Mask);
		}
	} else
	if ((mAddressBus >> kDSK_ln2Spc) == (kDSK_Block_Base >> kDSK_ln2Spc)) {
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
#if CurEmMd == kEmMd_PB100
	if ((mAddressBus >> kASC_ln2Spc) == (kASC_Block_Base >> kASC_ln2Spc)) {
		if (! ByteSize) {
			ReportAbnormal("access ASC word");
		} else {
			Data = ASC_Access(Data, WriteMem, mAddressBus & kASC_Mask);
		}
	} else
#endif
	if ((mAddressBus >> kSCSI_ln2Spc) == (kSCSI_Block_Base >> kSCSI_ln2Spc)) {
		if (! ByteSize) {
			ReportAbnormal("access SCSI word");
		} else if (WriteMem != ((mAddressBus & 1) != 0)) {
			ReportAbnormal("access SCSI even/odd");
		} else {
			Data = SCSI_Access(Data, WriteMem, (mAddressBus >> 4) & 0x07);
		}
	} else
	if ((mAddressBus >> kIWM_ln2Spc) == (kIWM_Block_Base >> kIWM_ln2Spc)) {
#if CurEmMd >= kEmMd_SE
		if ((mAddressBus & 0x00100000) == 0) {
			ReportAbnormal("access IWM unassigned address");
		} else
#endif
		if (! ByteSize) {
#if ExtraAbnormalReports
			ReportAbnormal("access IWM word");
			/*
				This happens when quitting 'Glider 3.1.2'.
				perhaps a bad handle is being disposed of.
			*/
#endif
		} else if ((mAddressBus & 1) == 0) {
			ReportAbnormal("access IWM even");
		} else {
#if CurEmMd != kEmMd_PB100
			if ((mAddressBus & 0x001FE1FF) != 0x001FE1FF) {
				ReportAbnormal("access IWM nonstandard address");
			}
#endif
			Data = IWM_Access(Data, WriteMem, (mAddressBus >> 9) & kIWM_Mask);
		}
	} else
	{
		ui3p ba;

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
#endif

GLOBALPROC MemOverlay_ChangeNtfy(void)
{
#if CurEmMd <= kEmMd_Plus
	SetUpMemBanks();
#endif
}

#if CurEmMd == kEmMd_II
GLOBALPROC Addr32_ChangeNtfy(void)
{
	SetUpMemBanks();
}
#endif


/*
	unlike in the real Mac Plus, Mini vMac
	will allow misaligned memory access,
	since it is easier to allow it than
	it is to correctly simulate a bus error
	and back out of the current instruction.
*/

GLOBALFUNC ui3p get_real_address(ui5b L, blnr WritableMem, CPTR addr)
{
#if CurEmMd == kEmMd_II
	if (Addr32) {
		if (addr < 0x40000000) {
			if (MemOverlay) {
				if (WritableMem) {
					/* fail */
					return nullpr;
				} else {
					return ROM + (addr & ROMmem_mask);
				}
			} else {
				ui5r bankbit = 0x00100000 << (((VIA2_iA7 << 1) | VIA2_iA6) << 1);
				if ((addr & bankbit) != 0) {
					return nullpr;
				} else {
					return RAM + (addr & RAMmem_mask);
				}
			}
		} else
		if ((addr >= 0x40000000) && (addr < 0x50000000)) {
			if (WritableMem) {
				/* fail */
				return nullpr;
			} else {
				return ROM + (addr & ROMmem_mask);
			}
		} else
		{
			ReportAbnormal("get_real_address fails");
			return nullpr;
		}
	} else {
#endif
	ui5b bi = bankindex(addr);
	ui3p ba;

	if (! GetBankAddr(bi, WritableMem, &ba)) {
		ReportAbnormal("get_real_address fails");
		return nullpr;
	} else {
		ui5b bankoffset = addr & MemBankAddrMask;
		ui3p p = (ui3p)(bankoffset + ba);
		ui5b bankleft = BytesPerMemBank - bankoffset;
label_1:
		if (bankleft >= L) {
			return p; /* ok */
		} else {
			ui3p bankend = (ui3p)(BytesPerMemBank + ba);

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
#if CurEmMd == kEmMd_II
	}
#endif
}

GLOBALVAR blnr InterruptButton = falseblnr;

GLOBALPROC SetInterruptButton(blnr v)
{
	if (InterruptButton != v) {
		InterruptButton = v;
		VIAorSCCinterruptChngNtfy();
	}
}

IMPORTPROC DoMacReset(void);

GLOBALPROC InterruptReset_Update(void)
{
	SetInterruptButton(falseblnr);
		/*
			in case has been set. so only stays set
			for 60th of a second.
		*/

	if (WantMacInterrupt) {
		SetInterruptButton(trueblnr);
		WantMacInterrupt = falseblnr;
	}
	if (WantMacReset) {
		DoMacReset();
		WantMacReset = falseblnr;
	}
}

LOCALVAR ui3b CurIPL = 0;

GLOBALPROC VIAorSCCinterruptChngNtfy(void)
{
#if CurEmMd == kEmMd_II
	ui3b NewIPL;

	if (InterruptButton) {
		NewIPL = 7;
	} else if (SCCInterruptRequest) {
		NewIPL = 4;
	} else if (VIA2_InterruptRequest) {
		NewIPL = 2;
	} else if (VIA1_InterruptRequest) {
		NewIPL = 1;
	} else {
		NewIPL = 0;
	}
#else
	ui3b VIAandNotSCC = VIA1_InterruptRequest
		& ~ SCCInterruptRequest;
	ui3b NewIPL = VIAandNotSCC
		| (SCCInterruptRequest << 1)
		| (InterruptButton << 2);
#endif
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

#if EmClassicKbrd
IMPORTPROC DoKybd_ReceiveEndCommand(void);
IMPORTPROC DoKybd_ReceiveCommand(void);
#endif
#if EmADB
IMPORTPROC ADB_DoNewState(void);
#endif
#if EmPMU
IMPORTPROC PMU_DoTask(void);
#endif
IMPORTPROC VIA1_DoTimer1Check(void);
IMPORTPROC VIA1_DoTimer2Check(void);
#if EmVIA2
IMPORTPROC VIA2_DoTimer1Check(void);
IMPORTPROC VIA2_DoTimer2Check(void);
#endif

LOCALPROC ICT_DoTask(int taskid)
{
	switch (taskid) {
#if EmClassicKbrd
		case kICT_Kybd_ReceiveEndCommand:
			DoKybd_ReceiveEndCommand();
			break;
		case kICT_Kybd_ReceiveCommand:
			DoKybd_ReceiveCommand();
			break;
#endif
#if EmADB
		case kICT_ADB_NewState:
			ADB_DoNewState();
			break;
#endif
#if EmPMU
		case kICT_PMU_Task:
			PMU_DoTask();
			break;
#endif
		case kICT_VIA1_Timer1Check:
			VIA1_DoTimer1Check();
			break;
		case kICT_VIA1_Timer2Check:
			VIA1_DoTimer2Check();
			break;
#if EmVIA2
		case kICT_VIA2_Timer1Check:
			VIA2_DoTimer1Check();
			break;
		case kICT_VIA2_Timer2Check:
			VIA2_DoTimer2Check();
			break;
#endif
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
