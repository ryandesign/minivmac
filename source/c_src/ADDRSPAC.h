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

#if CurEmMd == kEmMd_II
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


/* the Wire variables are 1/0, not true/false */

enum {


#if CurEmMd == kEmMd_II
	Wire_unknown_SoundDisable,
#define SoundDisable (Wires[Wire_unknown_SoundDisable])

	Wire_unknown_SoundVolb0,
#define SoundVolb0 (Wires[Wire_unknown_SoundVolb0])

	Wire_unknown_SoundVolb1,
#define SoundVolb1 (Wires[Wire_unknown_SoundVolb1])

	Wire_unknown_SoundVolb2,
#define SoundVolb2 (Wires[Wire_unknown_SoundVolb2])

	Wire_VIA1_iA0_unknown,
#define VIA1_iA0 (Wires[Wire_VIA1_iA0_unknown])

	Wire_VIA1_iA1_unknown,
#define VIA1_iA1 (Wires[Wire_VIA1_iA1_unknown])

	Wire_VIA1_iA2_unknown,
#define VIA1_iA2 (Wires[Wire_VIA1_iA2_unknown])

	Wire_VIA1_iB7_unknown, /* for compatibility with SoundDisable */
#define VIA1_iB7 (Wires[Wire_VIA1_iB7_unknown])

	Wire_VIA2_InterruptRequest,
#define VIA2_InterruptRequest (Wires[Wire_VIA2_InterruptRequest])
#define VIA2_interruptChngNtfy VIAorSCCinterruptChngNtfy

	Wire_VIA2_iA7_unknown,
#define VIA2_iA7 (Wires[Wire_VIA2_iA7_unknown])

	Wire_VIA2_iA6_unknown,
#define VIA2_iA6 (Wires[Wire_VIA2_iA6_unknown])

	Wire_VIA2_iB7_unknown,
#define VIA2_iB7 (Wires[Wire_VIA2_iB7_unknown])

	Wire_VIA2_iCB2_unknown,
#define VIA2_iCB2 (Wires[Wire_VIA2_iCB2_unknown])

#if CurEmMd == kEmMd_II
	Wire_VIA2_iB2_PowerOff,
#define VIA2_iB2 (Wires[Wire_VIA2_iB2_PowerOff])
#define VIA2_iB2_ChangeNtfy PowerOff_ChangeNtfy
#endif

	Wire_VIA2_iB3_Addr32,
#define VIA2_iB3 (Wires[Wire_VIA2_iB3_Addr32])
#define Addr32 (Wires[Wire_VIA2_iB3_Addr32])
#define VIA2_iB3_ChangeNtfy Addr32_ChangeNtfy
#endif

#if CurEmMd == kEmMd_PB100
	Wire_VIA1_iA0_PmuBus0,
	Wire_VIA1_iA1_PmuBus1,
	Wire_VIA1_iA2_PmuBus2,
	Wire_VIA1_iA3_PmuBus3,
	Wire_VIA1_iA4_PmuBus4,
	Wire_VIA1_iA5_PmuBus5,
	Wire_VIA1_iA6_PmuBus6,
	Wire_VIA1_iA7_PmuBus7,

#define VIA1_iA0 (Wires[Wire_VIA1_iA0_PmuBus0])
#define VIA1_iA1 (Wires[Wire_VIA1_iA1_PmuBus1])
#define VIA1_iA2 (Wires[Wire_VIA1_iA2_PmuBus2])
#define VIA1_iA3 (Wires[Wire_VIA1_iA3_PmuBus3])
#define VIA1_iA4 (Wires[Wire_VIA1_iA4_PmuBus4])
#define VIA1_iA5 (Wires[Wire_VIA1_iA5_PmuBus5])
#define VIA1_iA6 (Wires[Wire_VIA1_iA6_PmuBus6])
#define VIA1_iA7 (Wires[Wire_VIA1_iA7_PmuBus7])

	Wire_VIA1_iB0_PmuToReady,
#define VIA1_iB0 (Wires[Wire_VIA1_iB0_PmuToReady])
#define PmuToReady (Wires[Wire_VIA1_iB0_PmuToReady])
#define VIA1_iB0_ChangeNtfy PmuToReady_ChangeNtfy

	Wire_VIA1_iB1_PmuFromReady,
#define VIA1_iB1 (Wires[Wire_VIA1_iB1_PmuFromReady])
#define PmuFromReady (Wires[Wire_VIA1_iB1_PmuFromReady])

	Wire_VIA1_iB2_Unknown,
#define VIA1_iB2 (Wires[Wire_VIA1_iB2_Unknown])

	Wire_VIA1_iB3_Unknown,
#define VIA1_iB3 (Wires[Wire_VIA1_iB3_Unknown])

	Wire_VIA1_iB4_Unknown,
#define VIA1_iB4 (Wires[Wire_VIA1_iB4_Unknown])

	Wire_VIA1_iB5_Unknown,
#define VIA1_iB5 (Wires[Wire_VIA1_iB5_Unknown])

	Wire_VIA1_iB6_Unknown,
#define VIA1_iB6 (Wires[Wire_VIA1_iB6_Unknown])

	Wire_VIA1_iB7_Unknown,
#define VIA1_iB7 (Wires[Wire_VIA1_iB7_Unknown])
#endif

#if CurEmMd <= kEmMd_Classic
	Wire_VIA1_iA0_SoundVolb0,
#define SoundVolb0 (Wires[Wire_VIA1_iA0_SoundVolb0])
#define VIA1_iA0 (Wires[Wire_VIA1_iA0_SoundVolb0])

	Wire_VIA1_iA1_SoundVolb1,
#define SoundVolb1 (Wires[Wire_VIA1_iA1_SoundVolb1])
#define VIA1_iA1 (Wires[Wire_VIA1_iA1_SoundVolb1])

	Wire_VIA1_iA2_SoundVolb2,
#define SoundVolb2 (Wires[Wire_VIA1_iA2_SoundVolb2])
#define VIA1_iA2 (Wires[Wire_VIA1_iA2_SoundVolb2])
#endif

#if CurEmMd <= kEmMd_Plus
	Wire_VIA1_iA3_SoundBuffer,
#define SoundBuffer (Wires[Wire_VIA1_iA3_SoundBuffer])
#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SoundBuffer])
#elif (CurEmMd <= kEmMd_Classic) || (kEmMd_II == CurEmMd)
	Wire_VIA1_iA3_SCCvSync,
#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SCCvSync])
#endif

#if (CurEmMd <= kEmMd_Plus) || (kEmMd_II == CurEmMd)
	Wire_VIA1_iA4_MemOverlay,
#define VIA1_iA4 (Wires[Wire_VIA1_iA4_MemOverlay])
#define MemOverlay (Wires[Wire_VIA1_iA4_MemOverlay])
#define VIA1_iA4_ChangeNtfy MemOverlay_ChangeNtfy
#elif CurEmMd <= kEmMd_II
#if CurEmMd <= kEmMd_Classic
	Wire_VIA1_iA4_DriveSel,
#define VIA1_iA4 (Wires[Wire_VIA1_iA4_DriveSel])
#endif
	Wire_MemOverlay,
#define MemOverlay (Wires[Wire_MemOverlay])
#endif

#if kEmMd_PB100 == CurEmMd
	Wire_SCCwaitrq,
#define SCCwaitrq (Wires[Wire_SCCwaitrq])
#endif

#if (CurEmMd <= kEmMd_Classic) || (kEmMd_II == CurEmMd)
	Wire_VIA1_iA5_IWMvSel,
#define IWMvSel (Wires[Wire_VIA1_iA5_IWMvSel])
#define VIA1_iA5 (Wires[Wire_VIA1_iA5_IWMvSel])
#endif

#if CurEmMd <= kEmMd_Classic
	Wire_VIA1_iA6_SCRNvPage2,
#define SCRNvPage2 (Wires[Wire_VIA1_iA6_SCRNvPage2])
#define VIA1_iA6 (Wires[Wire_VIA1_iA6_SCRNvPage2])
#endif

#if (CurEmMd <= kEmMd_Classic) || (kEmMd_II == CurEmMd)
	Wire_VIA1_iA7_SCCwaitrq,
#define SCCwaitrq (Wires[Wire_VIA1_iA7_SCCwaitrq])
#define VIA1_iA7 (Wires[Wire_VIA1_iA7_SCCwaitrq])
#endif

#if (CurEmMd <= kEmMd_Classic) || (CurEmMd == kEmMd_II)
	Wire_VIA1_iB0_RTCdataLine,
#define RTCdataLine (Wires[Wire_VIA1_iB0_RTCdataLine])
#define VIA1_iB0 (Wires[Wire_VIA1_iB0_RTCdataLine])
#define VIA1_iB0_ChangeNtfy RTCdataLine_ChangeNtfy

	Wire_VIA1_iB1_RTCclock,
#define RTCclock (Wires[Wire_VIA1_iB1_RTCclock])
#define VIA1_iB1 (Wires[Wire_VIA1_iB1_RTCclock])
#define VIA1_iB1_ChangeNtfy RTCclock_ChangeNtfy

	Wire_VIA1_iB2_RTCunEnabled,
#define RTCunEnabled (Wires[Wire_VIA1_iB2_RTCunEnabled])
#define VIA1_iB2 (Wires[Wire_VIA1_iB2_RTCunEnabled])
#define VIA1_iB2_ChangeNtfy RTCunEnabled_ChangeNtfy
#endif

#if CurEmMd <= kEmMd_Plus
	Wire_VIA1_iB3_MouseBtnUp,
#define MouseBtnUp (Wires[Wire_VIA1_iB3_MouseBtnUp])
#define VIA1_iB3 (Wires[Wire_VIA1_iB3_MouseBtnUp])
#elif (CurEmMd <= kEmMd_Classic) || (CurEmMd == kEmMd_II)
	Wire_VIA1_iB3_ADBint,
#define VIA1_iB3 (Wires[Wire_VIA1_iB3_ADBint])
#define ADB_Int (Wires[Wire_VIA1_iB3_ADBint])
#endif

#if CurEmMd <= kEmMd_Plus
	Wire_VIA1_iB4_MouseX2,
#define MouseX2 (Wires[Wire_VIA1_iB4_MouseX2])
#define VIA1_iB4 (Wires[Wire_VIA1_iB4_MouseX2])
#elif (CurEmMd <= kEmMd_Classic) || (CurEmMd == kEmMd_II)
	Wire_VIA1_iB4_ADB_st0,
#define VIA1_iB4 (Wires[Wire_VIA1_iB4_ADB_st0])
#define ADB_st0 (Wires[Wire_VIA1_iB4_ADB_st0])
#define VIA1_iB4_ChangeNtfy ADBstate_ChangeNtfy
#endif

#if CurEmMd <= kEmMd_Plus
	Wire_VIA1_iB5_MouseY2,
#define MouseY2 (Wires[Wire_VIA1_iB5_MouseY2])
#define VIA1_iB5 (Wires[Wire_VIA1_iB5_MouseY2])
#elif (CurEmMd <= kEmMd_Classic) || (CurEmMd == kEmMd_II)
	Wire_VIA1_iB5_ADB_st1,
#define VIA1_iB5 (Wires[Wire_VIA1_iB5_ADB_st1])
#define ADB_st1 (Wires[Wire_VIA1_iB5_ADB_st1])
#define VIA1_iB5_ChangeNtfy ADBstate_ChangeNtfy
#endif

#if CurEmMd <= kEmMd_Plus
	Wire_VIA1_iB6_SCRNbeamInVid,
#define SCRNbeamInVid (Wires[Wire_VIA1_iB6_SCRNbeamInVid])
#define VIA1_iB6 (Wires[Wire_VIA1_iB6_SCRNbeamInVid])
#elif CurEmMd <= kEmMd_Classic
	Wire_VIA1_iB6_SCSIintenable,
#define VIA1_iB6 (Wires[Wire_VIA1_iB6_SCSIintenable])
#endif

#if CurEmMd <= kEmMd_Classic
	Wire_VIA1_iB7_SoundDisable,
#define SoundDisable (Wires[Wire_VIA1_iB7_SoundDisable])
#define VIA1_iB7 (Wires[Wire_VIA1_iB7_SoundDisable])
#endif

	Wire_VIA1_InterruptRequest,
#define VIA1_InterruptRequest (Wires[Wire_VIA1_InterruptRequest])
#define VIA1_interruptChngNtfy VIAorSCCinterruptChngNtfy

	Wire_SCCInterruptRequest,
#define SCCInterruptRequest (Wires[Wire_SCCInterruptRequest])
#define SCCinterruptChngNtfy VIAorSCCinterruptChngNtfy

	Wire_VIA1_iCB2_KybdDat,
#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_KybdDat])
#if CurEmMd <= kEmMd_Plus
#define VIA1_iCB2_ChangeNtfy Kybd_DataLineChngNtfy
#elif (CurEmMd <= kEmMd_Classic) || (CurEmMd == kEmMd_II)
#define ADB_Data (Wires[Wire_VIA1_iCB2_KybdDat])
#define VIA1_iCB2_ChangeNtfy ADB_DataLineChngNtfy
#endif

#if CurEmMd <= kEmMd_Plus
#define Mouse_Enabled SCC_InterruptsEnabled
#else
	Wire_ADBMouseDisabled,
#define ADBMouseDisabled (Wires[Wire_ADBMouseDisabled])
#define Mouse_Enabled() (! ADBMouseDisabled)
#endif

#if CurEmMd == kEmMd_II
	Wire_VBLinterrupt,
#define Vid_VBLinterrupt (Wires[Wire_VBLinterrupt])
#define VIA2_iA0 (Wires[Wire_VBLinterrupt])
#endif

#if CurEmMd == kEmMd_II
	Wire_VBLintunenbl,
#define Vid_VBLintunenbl (Wires[Wire_VBLintunenbl])
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

#if CurEmMd <= kEmMd_Classic
#define VIA1_ORA_CanIn 0x80
#define VIA1_ORA_CanOut 0x7F
#elif CurEmMd == kEmMd_PB100
#define VIA1_ORA_CanIn 0xFF
#define VIA1_ORA_CanOut 0xFF
#elif CurEmMd == kEmMd_II
#define VIA1_ORA_CanIn 0x80
#define VIA1_ORA_CanOut 0x3F
#endif

#if CurEmMd == kEmMd_Classic
#define VIA1_ORA_FloatVal 0xF7
#endif

#if CurEmMd == kEmMd_II
#define VIA1_ORA_FloatVal 0xBF
	/* bit 6 used to check version of hardware */
#endif

#if CurEmMd <= kEmMd_Plus
#define VIA1_ORB_CanIn 0x79
#define VIA1_ORB_CanOut 0x87
#elif CurEmMd <= kEmMd_Classic
#define VIA1_ORB_CanIn 0x09
#define VIA1_ORB_CanOut 0xF7
#elif CurEmMd == kEmMd_PB100
#define VIA1_ORB_CanIn 0x02
#define VIA1_ORB_CanOut 0xFD
#elif CurEmMd == kEmMd_II
#define VIA1_ORB_CanIn 0x09
#define VIA1_ORB_CanOut 0xB7
#endif

#if CurEmMd == kEmMd_II
#define VIA2_ORA_CanIn 0x01
#define VIA2_ORA_CanOut 0xC0
#endif

#if CurEmMd == kEmMd_II
#define VIA2_ORB_CanIn 0x00
#define VIA2_ORB_CanOut 0x8C
#endif

#if CurEmMd <= kEmMd_Plus
#define VIA1_IER_Never0 (1 << 1)
#define VIA1_IER_Never1 ((1 << 3) | (1 << 4))
#elif CurEmMd <= kEmMd_Classic
#define VIA1_IER_Never0 0x00
#define VIA1_IER_Never1 ((1 << 3) | (1 << 4))
#elif CurEmMd == kEmMd_PB100
#define VIA1_IER_Never0 0x00
#define VIA1_IER_Never1 0x0C
#elif CurEmMd == kEmMd_II
#define VIA1_IER_Never0 0x00
#define VIA1_IER_Never1 0x58
#endif

#if CurEmMd == kEmMd_II
#define VIA2_IER_Never0 0x00
#define VIA2_IER_Never1 0xED
#endif

#if CurEmMd == kEmMd_PB100
#define VIA1_CB2modesAllowed 0x03
#define VIA1_CA2modesAllowed 0x03
#endif

#define RTC_OneSecond_PulseNtfy VIA1_iCA2_PulseNtfy

#if CurEmMd == kEmMd_II
#define Vid_VBLinterrupt_PulseNotify VIA2_iCA1_PulseNtfy
#endif

#define GetSoundInvertTime VIA1_GetT1InvertTime

#if EmClassicKbrd
#define KYBD_ShiftInData VIA1_ShiftOutData
#define KYBD_ShiftOutData VIA1_ShiftInData
#endif
#if EmADB
#define ADB_ShiftInData VIA1_ShiftOutData
#define ADB_ShiftOutData VIA1_ShiftInData
#endif
