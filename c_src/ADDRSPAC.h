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


#define ln2TotAddrBytes 24

#define ln2BytesPerMemBank 17
#define ln2NumMemBanks (ln2TotAddrBytes - ln2BytesPerMemBank)

#define NumMemBanks (1UL << ln2NumMemBanks)
#define BytesPerMemBank  (1UL << ln2BytesPerMemBank)
#define MemBanksMask (NumMemBanks - 1)
#define MemBankAddrMask (BytesPerMemBank - 1)

#define bankindex(addr) ((((CPTR)(addr)) >> ln2BytesPerMemBank) & MemBanksMask)


EXPORTPROC Memory_Reset(void);

EXPORTPROC MemOverlay_ChangeNtfy(void);

/*
	mapping of address space to real memory
*/

EXPORTFUNC ui3b *get_real_address(ui5b L, blnr WritableMem, CPTR addr);

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

#define get_ram_byte(addr) do_get_mem_byte((addr) + (ui3p)RAM)
#define get_ram_word(addr) do_get_mem_word((addr) + (ui3p)RAM)
#define get_ram_long(addr) do_get_mem_long((addr) + (ui3p)RAM)

#define put_ram_byte(addr, b) do_put_mem_byte((addr) + (ui3p)RAM, (b))
#define put_ram_word(addr, w) do_put_mem_word((addr) + (ui3p)RAM, (w))
#define put_ram_long(addr, l) do_put_mem_long((addr) + (ui3p)RAM, (l))

#define get_ram_address(addr) ((addr) + (ui3p)RAM)

EXPORTFUNC blnr AddrSpac_Init(void);


#ifndef ExtraAbormalReports
#define ExtraAbormalReports DetailedAbormalReport
#endif
	/* Additional reports for situations that
	occur because of bugs in some programs */

#if DetailedAbormalReport
#define ReportAbnormal DoReportAbnormal
EXPORTPROC DoReportAbnormal(char *s);
#else
#define ReportAbnormal(s) DoReportAbnormal()
EXPORTPROC DoReportAbnormal(void);
#endif

EXPORTPROC VIAorSCCinterruptChngNtfy(void);

EXPORTVAR(blnr, InterruptButton)
EXPORTPROC SetInterruptButton(blnr v);

enum {
#if CurEmu <= kEmuPlus
	kICT_Kybd_ReceiveCommand,
	kICT_Kybd_ReceiveEndCommand,
#else
	kICT_ADB_NewState,
#endif
	kICT_VIA_Timer1Check,
	kICT_VIA_Timer2Check,

	kNumICTs
};

EXPORTPROC ICT_add(int taskid, ui5b n);
EXPORTPROC m68k_go_nInstructions_1(ui5b n);

#define iCountt ui5b
EXPORTFUNC iCountt GetCuriCount(void);


/* the Wire variables are 1/0, not true/false */

enum {
	Wire_VIAiA0_SoundVolb0,
#define SoundVolb0 (Wires[Wire_VIAiA0_SoundVolb0])
#define VIAiA0 (Wires[Wire_VIAiA0_SoundVolb0])

	Wire_VIAiA1_SoundVolb1,
#define SoundVolb1 (Wires[Wire_VIAiA1_SoundVolb1])
#define VIAiA1 (Wires[Wire_VIAiA1_SoundVolb1])

	Wire_VIAiA2_SoundVolb2,
#define SoundVolb2 (Wires[Wire_VIAiA2_SoundVolb2])
#define VIAiA2 (Wires[Wire_VIAiA2_SoundVolb2])

#if CurEmu <= kEmuPlus
	Wire_VIAiA3_SoundBuffer,
#define SoundBuffer (Wires[Wire_VIAiA3_SoundBuffer])
#define VIAiA3 (Wires[Wire_VIAiA3_SoundBuffer])
#elif CurEmu <= kEmuClassic
	Wire_VIAiA3_SCCvSync,
#define VIAiA3 (Wires[Wire_VIAiA3_SCCvSync])
#endif

#if CurEmu <= kEmuPlus
	Wire_VIAiA4_MemOverlay,
#define VIAiA4 (Wires[Wire_VIAiA4_MemOverlay])
#define MemOverlay (Wires[Wire_VIAiA4_MemOverlay])
#define VIAiA4_ChangeNtfy MemOverlay_ChangeNtfy
#elif CurEmu <= kEmuClassic
	Wire_VIAiA4_DriveSel,
#define VIAiA4 (Wires[Wire_VIAiA4_DriveSel])
	Wire_MemOverlay,
#define MemOverlay (Wires[Wire_MemOverlay])
#endif

	Wire_VIAiA5_IWMvSel,
#define IWMvSel (Wires[Wire_VIAiA5_IWMvSel])
#define VIAiA5 (Wires[Wire_VIAiA5_IWMvSel])

	Wire_VIAiA6_SCRNvPage2,
#define SCRNvPage2 (Wires[Wire_VIAiA6_SCRNvPage2])
#define VIAiA6 (Wires[Wire_VIAiA6_SCRNvPage2])

	Wire_VIAiA7_SCCwaitrq,
#define SCCwaitrq (Wires[Wire_VIAiA7_SCCwaitrq])
#define VIAiA7 (Wires[Wire_VIAiA7_SCCwaitrq])

	Wire_VIAiB0_RTCdataLine,
#define RTCdataLine (Wires[Wire_VIAiB0_RTCdataLine])
#define VIAiB0 (Wires[Wire_VIAiB0_RTCdataLine])
#define VIAiB0_ChangeNtfy RTCdataLine_ChangeNtfy

	Wire_VIAiB1_RTCclock,
#define RTCclock (Wires[Wire_VIAiB1_RTCclock])
#define VIAiB1 (Wires[Wire_VIAiB1_RTCclock])
#define VIAiB1_ChangeNtfy RTCclock_ChangeNtfy

	Wire_VIAiB2_RTCunEnabled,
#define RTCunEnabled (Wires[Wire_VIAiB2_RTCunEnabled])
#define VIAiB2 (Wires[Wire_VIAiB2_RTCunEnabled])
#define VIAiB2_ChangeNtfy RTCunEnabled_ChangeNtfy

#if CurEmu <= kEmuPlus
	Wire_VIAiB3_MouseBtnUp,
#define MouseBtnUp (Wires[Wire_VIAiB3_MouseBtnUp])
#define VIAiB3 (Wires[Wire_VIAiB3_MouseBtnUp])
#elif CurEmu <= kEmuClassic
	Wire_VIAiB3_ADBint,
#define VIAiB3 (Wires[Wire_VIAiB3_ADBint])
#endif

#if CurEmu <= kEmuPlus
	Wire_VIAiB4_MouseX2,
#define MouseX2 (Wires[Wire_VIAiB4_MouseX2])
#define VIAiB4 (Wires[Wire_VIAiB4_MouseX2])
#elif CurEmu <= kEmuClassic
	Wire_VIAiB4_ADBst0,
#define VIAiB4 (Wires[Wire_VIAiB4_ADBst0])
#define ADBst0 (Wires[Wire_VIAiB4_ADBst0])
#define VIAiB4_ChangeNtfy ADBstate_ChangeNtfy
#endif

#if CurEmu <= kEmuPlus
	Wire_VIAiB5_MouseY2,
#define MouseY2 (Wires[Wire_VIAiB5_MouseY2])
#define VIAiB5 (Wires[Wire_VIAiB5_MouseY2])
#elif CurEmu <= kEmuClassic
	Wire_VIAiB5_ADBst1,
#define VIAiB5 (Wires[Wire_VIAiB5_ADBst1])
#define ADBst1 (Wires[Wire_VIAiB5_ADBst1])
#define VIAiB5_ChangeNtfy ADBstate_ChangeNtfy
#endif

#if CurEmu <= kEmuPlus
	Wire_VIAiB6_SCRNbeamInVid,
#define SCRNbeamInVid (Wires[Wire_VIAiB6_SCRNbeamInVid])
#define VIAiB6 (Wires[Wire_VIAiB6_SCRNbeamInVid])
#elif CurEmu <= kEmuClassic
	Wire_VIAiB6_SCSIintenable,
#define VIAiB6 (Wires[Wire_VIAiB6_SCSIintenable])
#endif

	Wire_VIAiB7_SoundDisable,
#define SoundDisable (Wires[Wire_VIAiB7_SoundDisable])
#define VIAiB7 (Wires[Wire_VIAiB7_SoundDisable])

	Wire_VIAInterruptRequest,
#define VIAInterruptRequest (Wires[Wire_VIAInterruptRequest])
#define VIAinterruptChngNtfy VIAorSCCinterruptChngNtfy

	Wire_SCCInterruptRequest,
#define SCCInterruptRequest (Wires[Wire_SCCInterruptRequest])
#define SCCinterruptChngNtfy VIAorSCCinterruptChngNtfy

	Wire_VIAiCB2_KybdDat,
#define VIAiCB2 (Wires[Wire_VIAiCB2_KybdDat])
#if CurEmu <= kEmuPlus
#define VIAiCB2_ChangeNtfy Kybd_DataLineChngNtfy
#else
#define VIAiCB2_ChangeNtfy ADB_DataLineChngNtfy
#endif

#if CurEmu <= kEmuPlus
#define Mouse_Enabled SCC_InterruptsEnabled
#else
	Wire_ADBMouseDisabled,
#define ADBMouseDisabled (Wires[Wire_ADBMouseDisabled])
#define Mouse_Enabled() (! ADBMouseDisabled)
#endif

	kNumWires
};

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
