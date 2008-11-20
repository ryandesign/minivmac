/*
	ADDRSPAC.h

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

#ifdef ADDRSPAC_H
#error "header already included"
#else
#define ADDRSPAC_H
#endif


/*
	representation of pointer into memory of emulated computer.
*/
typedef ui5b CPTR;


#define ln2TotAddrBytes 24

#if kEmMd_PB100 == CurEmMd
#define ln2BytesPerMemBank 15
#else
#define ln2BytesPerMemBank 17
#endif
#define ln2NumMemBanks (ln2TotAddrBytes - ln2BytesPerMemBank)

#define NumMemBanks (1UL << ln2NumMemBanks)
#define BytesPerMemBank  (1UL << ln2BytesPerMemBank)
#define MemBanksMask (NumMemBanks - 1)
#define MemBankAddrMask (BytesPerMemBank - 1)

#define bankindex(addr) ((((CPTR)(addr)) >> ln2BytesPerMemBank) & MemBanksMask)


EXPORTPROC Memory_Reset(void);

EXPORTPROC MemOverlay_ChangeNtfy(void);

#if (CurEmMd == kEmMd_II) || (CurEmMd == kEmMd_IIx)
EXPORTPROC Addr32_ChangeNtfy(void);
#endif

/*
	mapping of address space to real memory
*/

EXPORTFUNC ui3p get_real_address(ui5b L, blnr WritableMem, CPTR addr);

/*
	accessing addresses that don't map to
	real memory, i.e. memory mapped devices
*/

EXPORTFUNC ui5b MM_Access(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr);

/*
	memory access routines that can use when have address
	that is known to be in RAM (and that is in the first
	copy of the ram, not the duplicates, i.e. < kRAM_Size).
*/

#define get_ram_byte(addr) do_get_mem_byte((addr) + RAM)
#define get_ram_word(addr) do_get_mem_word((addr) + RAM)
#define get_ram_long(addr) do_get_mem_long((addr) + RAM)

#define put_ram_byte(addr, b) do_put_mem_byte((addr) + RAM, (b))
#define put_ram_word(addr, w) do_put_mem_word((addr) + RAM, (w))
#define put_ram_long(addr, l) do_put_mem_long((addr) + RAM, (l))

#define get_ram_address(addr) ((addr) + RAM)

GLOBALFUNC si5b get_vm_word(CPTR addr);
GLOBALFUNC si5b get_vm_byte(CPTR addr);
GLOBALFUNC ui5b get_vm_long(CPTR addr);

GLOBALPROC put_vm_word(CPTR addr, ui5b w);
GLOBALPROC put_vm_byte(CPTR addr, ui5b b);
GLOBALPROC put_vm_long(CPTR addr, ui5b l);

EXPORTFUNC blnr AddrSpac_Init(void);


#ifndef ExtraAbnormalReports
#define ExtraAbnormalReports DetailedAbnormalReport
#endif
	/* Additional reports for situations that
	occur because of bugs in some programs */

#if DetailedAbnormalReport
#define ReportAbnormal DoReportAbnormal
EXPORTPROC DoReportAbnormal(char *s);
#else
#define ReportAbnormal(s) DoReportAbnormal()
EXPORTPROC DoReportAbnormal(void);
#endif

EXPORTPROC VIAorSCCinterruptChngNtfy(void);

EXPORTVAR(blnr, InterruptButton)
EXPORTPROC SetInterruptButton(blnr v);
EXPORTPROC InterruptReset_Update(void);

enum {
#if EmClassicKbrd
	kICT_Kybd_ReceiveCommand,
	kICT_Kybd_ReceiveEndCommand,
#endif
#if EmADB
	kICT_ADB_NewState,
#endif
#if EmPMU
	kICT_PMU_Task,
#endif
	kICT_VIA1_Timer1Check,
	kICT_VIA1_Timer2Check,
#if EmVIA2
	kICT_VIA2_Timer1Check,
	kICT_VIA2_Timer2Check,
#endif

	kNumICTs
};

EXPORTPROC ICT_add(int taskid, ui5b n);
EXPORTPROC m68k_go_nInstructions_1(ui5b n);

#define iCountt ui5b
EXPORTFUNC iCountt GetCuriCount(void);

EXPORTVAR(ui3b, Wires[kNumWires])

#define InstructionsPerTick 12250
	/*
		This a bit too fast on average, but
		if this was much lower, Concertware wouldn't
		work properly with speed limit on. If this was
		much higher, the initial sounds in Dark Castle
		would have static.
		This can only be an approximation, since on
		a real machine the number of instructions
		executed per time can vary by almost a factor
		of two, because different instructions take
		different times.
	*/
