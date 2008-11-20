/*
	SPOTHRCF.i
	Copyright (C) 2008 Paul C. Pratt

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
	program SPecific write OTHer ConFiguration files
*/

LOCALPROC WriteAppSpecificConfigFiles(void)
{
	if (WriteOpenDestFile(&SrcDirR, "EMCONFIG", ".h")) {

	WriteDestFileLn("/*");
	++DestFileIndent;
		WriteDestFileLn("configuration options used by platform independent code");
	--DestFileIndent;
	WriteDestFileLn("*/");

	WriteBlankLineToDestFile();

	WriteCompCondBool("EmClassicKbrd", EmClassicKbrd);
	WriteCompCondBool("EmADB", EmADB);
	WriteCompCondBool("EmRTC", EmRTC);
	WriteCompCondBool("EmPMU", EmPMU);
	WriteCompCondBool("EmVIA2", EmVIA2);
	WriteCompCondBool("Use68020", em_cpu_vers >= 2);
	WriteCompCondBool("EmFPU", (gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl));
	WriteCompCondBool("EmMMU", falseblnr);
	WriteCompCondBool("EmASC", EmASC);

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define CurEmMd kEmMd_");
	WriteCStrToDestFile(GetModelName(cur_mdl));
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kRAMa_Size ");
	WriteCStrToDestFile("0x");
	WriteHexLongToOutput(1 << RAMa_Size);
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kRAMb_Size ");
	if (0 == RAMb_Size) {
		WriteCStrToDestFile("0");
	} else {
		WriteCStrToDestFile("0x");
		WriteHexLongToOutput(1 << RAMb_Size);
	}
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteCompCondBool("IncludeVidMem", NeedVidMem);
	if (NeedVidMem) {
		uimr FrameSize;
		if (EmVidCard) {
			FrameSize = ((cur_hres * cur_vres) << cur_ScrnDpth) >> 3;
			--FrameSize;
			FrameSize |= (FrameSize >> 1);
			FrameSize |= (FrameSize >> 2);
			FrameSize |= (FrameSize >> 4);
			FrameSize |= (FrameSize >> 8);
			FrameSize |= (FrameSize >> 16);
			++FrameSize;
			if (FrameSize <= 0x00008000) {
				FrameSize = 0x00008000;
			}
		} else if (gbk_mdl_PB100 == cur_mdl) {
			FrameSize = 0x00008000;
		} else {
			FrameSize = 0x00020000;
		}
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define kVidMemRAM_Size ");
		WriteCStrToDestFile("0x");
		WriteHexLongToOutput(FrameSize);
		WriteEndDestFileLn();
	}

#if 0
	if (NeedScrnHack) {
		WriteDestFileLn("#define IncludeVidMem 1");
		WriteDestFileLn("#define kVidMemRAM_Size 0x00040000");
	}
#endif

	WriteBlankLineToDestFile();

	WriteCompCondBool("EmVidCard", EmVidCard);
	if (EmVidCard) {
		WriteDestFileLn("#define kVidROM_Size 0x000800");
	}

	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();

	WriteDestFileLn("/* the Wire variables are 1/0, not true/false */");
	WriteBlankLineToDestFile();
	WriteDestFileLn("enum {");
	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_unknown_SoundDisable,");
		WriteDestFileLn("#define SoundDisable (Wires[Wire_unknown_SoundDisable])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_unknown_SoundVolb0,");
		WriteDestFileLn("#define SoundVolb0 (Wires[Wire_unknown_SoundVolb0])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_unknown_SoundVolb1,");
		WriteDestFileLn("#define SoundVolb1 (Wires[Wire_unknown_SoundVolb1])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_unknown_SoundVolb2,");
		WriteDestFileLn("#define SoundVolb2 (Wires[Wire_unknown_SoundVolb2])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA0_unknown,");
		WriteDestFileLn("#define VIA1_iA0 (Wires[Wire_VIA1_iA0_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA1_unknown,");
		WriteDestFileLn("#define VIA1_iA1 (Wires[Wire_VIA1_iA1_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA2_unknown,");
		WriteDestFileLn("#define VIA1_iA2 (Wires[Wire_VIA1_iA2_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB7_unknown, /* for compatibility with SoundDisable */");
		WriteDestFileLn("#define VIA1_iB7 (Wires[Wire_VIA1_iB7_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_InterruptRequest,");
		WriteDestFileLn("#define VIA2_InterruptRequest (Wires[Wire_VIA2_InterruptRequest])");
		WriteDestFileLn("#define VIA2_interruptChngNtfy VIAorSCCinterruptChngNtfy");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iA7_unknown,");
		WriteDestFileLn("#define VIA2_iA7 (Wires[Wire_VIA2_iA7_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iA6_unknown,");
		WriteDestFileLn("#define VIA2_iA6 (Wires[Wire_VIA2_iA6_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iB7_unknown,");
		WriteDestFileLn("#define VIA2_iB7 (Wires[Wire_VIA2_iB7_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iCB2_unknown,");
		WriteDestFileLn("#define VIA2_iCB2 (Wires[Wire_VIA2_iCB2_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iB2_PowerOff,");
		WriteDestFileLn("#define VIA2_iB2 (Wires[Wire_VIA2_iB2_PowerOff])");
		WriteDestFileLn("#define VIA2_iB2_ChangeNtfy PowerOff_ChangeNtfy");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iB3_Addr32,");
		WriteDestFileLn("#define VIA2_iB3 (Wires[Wire_VIA2_iB3_Addr32])");
		WriteDestFileLn("#define Addr32 (Wires[Wire_VIA2_iB3_Addr32])");
		WriteDestFileLn("#define VIA2_iB3_ChangeNtfy Addr32_ChangeNtfy");
	}

	if (cur_mdl <= gbk_mdl_Classic) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA0_SoundVolb0,");
		WriteDestFileLn("#define SoundVolb0 (Wires[Wire_VIA1_iA0_SoundVolb0])");
		WriteDestFileLn("#define VIA1_iA0 (Wires[Wire_VIA1_iA0_SoundVolb0])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA1_SoundVolb1,");
		WriteDestFileLn("#define SoundVolb1 (Wires[Wire_VIA1_iA1_SoundVolb1])");
		WriteDestFileLn("#define VIA1_iA1 (Wires[Wire_VIA1_iA1_SoundVolb1])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA2_SoundVolb2,");
		WriteDestFileLn("#define SoundVolb2 (Wires[Wire_VIA1_iA2_SoundVolb2])");
		WriteDestFileLn("#define VIA1_iA2 (Wires[Wire_VIA1_iA2_SoundVolb2])");
	}

	if ((cur_mdl <= gbk_mdl_Plus) || (gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA4_MemOverlay,");
		WriteDestFileLn("#define VIA1_iA4 (Wires[Wire_VIA1_iA4_MemOverlay])");
		WriteDestFileLn("#define MemOverlay (Wires[Wire_VIA1_iA4_MemOverlay])");
		WriteDestFileLn("#define VIA1_iA4_ChangeNtfy MemOverlay_ChangeNtfy");
	} else {
		if (cur_mdl <= gbk_mdl_Classic) {
			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iA4_DriveSel,");
			WriteDestFileLn("#define VIA1_iA4 (Wires[Wire_VIA1_iA4_DriveSel])");
		}
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_MemOverlay,");
		WriteDestFileLn("#define MemOverlay (Wires[Wire_MemOverlay])");
	}

	if (cur_mdl <= gbk_mdl_Classic) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA6_SCRNvPage2,");
		WriteDestFileLn("#define SCRNvPage2 (Wires[Wire_VIA1_iA6_SCRNvPage2])");
		WriteDestFileLn("#define VIA1_iA6 (Wires[Wire_VIA1_iA6_SCRNvPage2])");
	}

	if (gbk_mdl_PB100 == cur_mdl) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_SCCwaitrq,");
		WriteDestFileLn("#define SCCwaitrq (Wires[Wire_SCCwaitrq])");

		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA0_PmuBus0,");
		WriteDestFileLn("\tWire_VIA1_iA1_PmuBus1,");
		WriteDestFileLn("\tWire_VIA1_iA2_PmuBus2,");
		WriteDestFileLn("\tWire_VIA1_iA3_PmuBus3,");
		WriteDestFileLn("\tWire_VIA1_iA4_PmuBus4,");
		WriteDestFileLn("\tWire_VIA1_iA5_PmuBus5,");
		WriteDestFileLn("\tWire_VIA1_iA6_PmuBus6,");
		WriteDestFileLn("\tWire_VIA1_iA7_PmuBus7,");
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define VIA1_iA0 (Wires[Wire_VIA1_iA0_PmuBus0])");
		WriteDestFileLn("#define VIA1_iA1 (Wires[Wire_VIA1_iA1_PmuBus1])");
		WriteDestFileLn("#define VIA1_iA2 (Wires[Wire_VIA1_iA2_PmuBus2])");
		WriteDestFileLn("#define VIA1_iA3 (Wires[Wire_VIA1_iA3_PmuBus3])");
		WriteDestFileLn("#define VIA1_iA4 (Wires[Wire_VIA1_iA4_PmuBus4])");
		WriteDestFileLn("#define VIA1_iA5 (Wires[Wire_VIA1_iA5_PmuBus5])");
		WriteDestFileLn("#define VIA1_iA6 (Wires[Wire_VIA1_iA6_PmuBus6])");
		WriteDestFileLn("#define VIA1_iA7 (Wires[Wire_VIA1_iA7_PmuBus7])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB0_PmuToReady,");
		WriteDestFileLn("#define VIA1_iB0 (Wires[Wire_VIA1_iB0_PmuToReady])");
		WriteDestFileLn("#define PmuToReady (Wires[Wire_VIA1_iB0_PmuToReady])");
		WriteDestFileLn("#define VIA1_iB0_ChangeNtfy PmuToReady_ChangeNtfy");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB1_PmuFromReady,");
		WriteDestFileLn("#define VIA1_iB1 (Wires[Wire_VIA1_iB1_PmuFromReady])");
		WriteDestFileLn("#define PmuFromReady (Wires[Wire_VIA1_iB1_PmuFromReady])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB2_Unknown,");
		WriteDestFileLn("#define VIA1_iB2 (Wires[Wire_VIA1_iB2_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB3_Unknown,");
		WriteDestFileLn("#define VIA1_iB3 (Wires[Wire_VIA1_iB3_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB4_Unknown,");
		WriteDestFileLn("#define VIA1_iB4 (Wires[Wire_VIA1_iB4_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB5_Unknown,");
		WriteDestFileLn("#define VIA1_iB5 (Wires[Wire_VIA1_iB5_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB6_Unknown,");
		WriteDestFileLn("#define VIA1_iB6 (Wires[Wire_VIA1_iB6_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB7_Unknown,");
		WriteDestFileLn("#define VIA1_iB7 (Wires[Wire_VIA1_iB7_Unknown])");
	} else {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA5_IWMvSel,");
		WriteDestFileLn("#define IWMvSel (Wires[Wire_VIA1_iA5_IWMvSel])");
		WriteDestFileLn("#define VIA1_iA5 (Wires[Wire_VIA1_iA5_IWMvSel])");

		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA7_SCCwaitrq,");
		WriteDestFileLn("#define SCCwaitrq (Wires[Wire_VIA1_iA7_SCCwaitrq])");
		WriteDestFileLn("#define VIA1_iA7 (Wires[Wire_VIA1_iA7_SCCwaitrq])");

		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB0_RTCdataLine,");
		WriteDestFileLn("#define RTCdataLine (Wires[Wire_VIA1_iB0_RTCdataLine])");
		WriteDestFileLn("#define VIA1_iB0 (Wires[Wire_VIA1_iB0_RTCdataLine])");
		WriteDestFileLn("#define VIA1_iB0_ChangeNtfy RTCdataLine_ChangeNtfy");

		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB1_RTCclock,");
		WriteDestFileLn("#define RTCclock (Wires[Wire_VIA1_iB1_RTCclock])");
		WriteDestFileLn("#define VIA1_iB1 (Wires[Wire_VIA1_iB1_RTCclock])");
		WriteDestFileLn("#define VIA1_iB1_ChangeNtfy RTCclock_ChangeNtfy");

		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB2_RTCunEnabled,");
		WriteDestFileLn("#define RTCunEnabled (Wires[Wire_VIA1_iB2_RTCunEnabled])");
		WriteDestFileLn("#define VIA1_iB2 (Wires[Wire_VIA1_iB2_RTCunEnabled])");
		WriteDestFileLn("#define VIA1_iB2_ChangeNtfy RTCunEnabled_ChangeNtfy");

		if (cur_mdl <= gbk_mdl_Plus) {
			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iA3_SoundBuffer,");
			WriteDestFileLn("#define SoundBuffer (Wires[Wire_VIA1_iA3_SoundBuffer])");
			WriteDestFileLn("#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SoundBuffer])");

			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iB3_MouseBtnUp,");
			WriteDestFileLn("#define MouseBtnUp (Wires[Wire_VIA1_iB3_MouseBtnUp])");
			WriteDestFileLn("#define VIA1_iB3 (Wires[Wire_VIA1_iB3_MouseBtnUp])");

			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iB4_MouseX2,");
			WriteDestFileLn("#define MouseX2 (Wires[Wire_VIA1_iB4_MouseX2])");
			WriteDestFileLn("#define VIA1_iB4 (Wires[Wire_VIA1_iB4_MouseX2])");

			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iB5_MouseY2,");
			WriteDestFileLn("#define MouseY2 (Wires[Wire_VIA1_iB5_MouseY2])");
			WriteDestFileLn("#define VIA1_iB5 (Wires[Wire_VIA1_iB5_MouseY2])");

			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iCB2_KybdDat,");
			WriteDestFileLn("#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_KybdDat])");
			WriteDestFileLn("#define VIA1_iCB2_ChangeNtfy Kybd_DataLineChngNtfy");
		} else {
			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iA3_SCCvSync,");
			WriteDestFileLn("#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SCCvSync])");

			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iB3_ADBint,");
			WriteDestFileLn("#define VIA1_iB3 (Wires[Wire_VIA1_iB3_ADBint])");
			WriteDestFileLn("#define ADB_Int (Wires[Wire_VIA1_iB3_ADBint])");

			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iB4_ADB_st0,");
			WriteDestFileLn("#define VIA1_iB4 (Wires[Wire_VIA1_iB4_ADB_st0])");
			WriteDestFileLn("#define ADB_st0 (Wires[Wire_VIA1_iB4_ADB_st0])");
			WriteDestFileLn("#define VIA1_iB4_ChangeNtfy ADBstate_ChangeNtfy");

			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iB5_ADB_st1,");
			WriteDestFileLn("#define VIA1_iB5 (Wires[Wire_VIA1_iB5_ADB_st1])");
			WriteDestFileLn("#define ADB_st1 (Wires[Wire_VIA1_iB5_ADB_st1])");
			WriteDestFileLn("#define VIA1_iB5_ChangeNtfy ADBstate_ChangeNtfy");

			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iCB2_KybdDat,");
			WriteDestFileLn("#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_KybdDat])");
			WriteDestFileLn("#define ADB_Data (Wires[Wire_VIA1_iCB2_KybdDat])");
			WriteDestFileLn("#define VIA1_iCB2_ChangeNtfy ADB_DataLineChngNtfy");
		}
	}

	if (cur_mdl <= gbk_mdl_Plus) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB6_SCRNbeamInVid,");
		WriteDestFileLn("#define SCRNbeamInVid (Wires[Wire_VIA1_iB6_SCRNbeamInVid])");
		WriteDestFileLn("#define VIA1_iB6 (Wires[Wire_VIA1_iB6_SCRNbeamInVid])");
	} else if (cur_mdl <= gbk_mdl_Classic) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB6_SCSIintenable,");
		WriteDestFileLn("#define VIA1_iB6 (Wires[Wire_VIA1_iB6_SCSIintenable])");
	}

	if (cur_mdl <= gbk_mdl_Classic) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB7_SoundDisable,");
		WriteDestFileLn("#define SoundDisable (Wires[Wire_VIA1_iB7_SoundDisable])");
		WriteDestFileLn("#define VIA1_iB7 (Wires[Wire_VIA1_iB7_SoundDisable])");
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("\tWire_VIA1_InterruptRequest,");
	WriteDestFileLn("#define VIA1_InterruptRequest (Wires[Wire_VIA1_InterruptRequest])");
	WriteDestFileLn("#define VIA1_interruptChngNtfy VIAorSCCinterruptChngNtfy");

	WriteBlankLineToDestFile();
	WriteDestFileLn("\tWire_SCCInterruptRequest,");
	WriteDestFileLn("#define SCCInterruptRequest (Wires[Wire_SCCInterruptRequest])");
	WriteDestFileLn("#define SCCinterruptChngNtfy VIAorSCCinterruptChngNtfy");

	if (cur_mdl <= gbk_mdl_Plus) {
	} else {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_ADBMouseDisabled,");
		WriteDestFileLn("#define ADBMouseDisabled (Wires[Wire_ADBMouseDisabled])");
	}

	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VBLinterrupt,");
		WriteDestFileLn("#define Vid_VBLinterrupt (Wires[Wire_VBLinterrupt])");
		WriteDestFileLn("#define VIA2_iA0 (Wires[Wire_VBLinterrupt])");

		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VBLintunenbl,");
		WriteDestFileLn("#define Vid_VBLintunenbl (Wires[Wire_VBLintunenbl])");
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("\tkNumWires");
	WriteDestFileLn("};");

	WriteBlankLineToDestFile();


	WriteBlankLineToDestFile();
	WriteDestFileLn("/* VIA configuration */");

	if (gbk_mdl_Classic == cur_mdl) {
		WriteDestFileLn("#define VIA1_ORA_FloatVal 0xF7");
	} else if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		WriteDestFileLn("#define VIA1_ORA_FloatVal 0xBF");
		WriteDestFileLn("\t/* bit 6 used to check version of hardware */");
	} else {
		WriteDestFileLn("#define VIA1_ORA_FloatVal 0xFF");
	}

	WriteDestFileLn("#define VIA1_ORB_FloatVal 0xFF");

	if (cur_mdl <= gbk_mdl_Classic) {
		WriteDestFileLn("#define VIA1_ORA_CanIn 0x80");
		WriteDestFileLn("#define VIA1_ORA_CanOut 0x7F");
	} else if (gbk_mdl_PB100 == cur_mdl) {
		WriteDestFileLn("#define VIA1_ORA_CanIn 0xFF");
		WriteDestFileLn("#define VIA1_ORA_CanOut 0xFF");
	} else if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		WriteDestFileLn("#define VIA1_ORA_CanIn 0x80");
		WriteDestFileLn("#define VIA1_ORA_CanOut 0x3F");
	} else {
		WriteDestFileLn("#define VIA1_ORA_CanIn 0x00");
		WriteDestFileLn("#define VIA1_ORA_CanOut 0x00");
	}

	if (cur_mdl <= gbk_mdl_Plus) {
		WriteDestFileLn("#define VIA1_ORB_CanIn 0x79");
		WriteDestFileLn("#define VIA1_ORB_CanOut 0x87");
	} else if (cur_mdl <= gbk_mdl_Classic) {
		WriteDestFileLn("#define VIA1_ORB_CanIn 0x09");
		WriteDestFileLn("#define VIA1_ORB_CanOut 0xF7");
	} else if (gbk_mdl_PB100 == cur_mdl) {
		WriteDestFileLn("#define VIA1_ORB_CanIn 0x02");
		WriteDestFileLn("#define VIA1_ORB_CanOut 0xFD");
	} else if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		WriteDestFileLn("#define VIA1_ORB_CanIn 0x09");
		WriteDestFileLn("#define VIA1_ORB_CanOut 0xB7");
	} else {
		WriteDestFileLn("#define VIA1_ORB_CanIn 0x00");
		WriteDestFileLn("#define VIA1_ORB_CanOut 0x00");
	}

	if (cur_mdl <= gbk_mdl_Plus) {
		WriteDestFileLn("#define VIA1_IER_Never0 (1 << 1)");
		WriteDestFileLn("#define VIA1_IER_Never1 ((1 << 3) | (1 << 4))");
	} else if (cur_mdl <= gbk_mdl_Classic) {
		WriteDestFileLn("#define VIA1_IER_Never0 0x00");
		WriteDestFileLn("#define VIA1_IER_Never1 ((1 << 3) | (1 << 4))");
	} else if (gbk_mdl_PB100 == cur_mdl) {
		WriteDestFileLn("#define VIA1_IER_Never0 0x00");
		WriteDestFileLn("#define VIA1_IER_Never1 0x0C");
	} else if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		WriteDestFileLn("#define VIA1_IER_Never0 0x00");
		WriteDestFileLn("#define VIA1_IER_Never1 0x58");
	} else {
		WriteDestFileLn("#define VIA1_IER_Never0 0xFF");
		WriteDestFileLn("#define VIA1_IER_Never1 0xFF");
	}

	if (gbk_mdl_PB100 == cur_mdl) {
		WriteDestFileLn("#define VIA1_CB2modesAllowed 0x03");
		WriteDestFileLn("#define VIA1_CA2modesAllowed 0x03");
	} else {
		WriteDestFileLn("#define VIA1_CB2modesAllowed 0x01");
		WriteDestFileLn("#define VIA1_CA2modesAllowed 0x01");
	}


	if (EmVIA2) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("/* VIA 2 configuration */");

		WriteDestFileLn("#define VIA2_ORA_FloatVal 0xFF");
		WriteDestFileLn("#define VIA2_ORB_FloatVal 0xFF");

		if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
			WriteDestFileLn("#define VIA2_ORA_CanIn 0x01");
			WriteDestFileLn("#define VIA2_ORA_CanOut 0xC0");
		} else {
			WriteDestFileLn("#define VIA2_ORA_CanIn 0x00");
			WriteDestFileLn("#define VIA2_ORA_CanOut 0x00");
		}

		if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
			WriteDestFileLn("#define VIA2_ORB_CanIn 0x00");
			WriteDestFileLn("#define VIA2_ORB_CanOut 0x8C");
		} else {
			WriteDestFileLn("#define VIA2_ORB_CanIn 0x00");
			WriteDestFileLn("#define VIA2_ORB_CanOut 0x00");
		}

		if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
			WriteDestFileLn("#define VIA2_IER_Never0 0x00");
			WriteDestFileLn("#define VIA2_IER_Never1 0xED");
		} else {
			WriteDestFileLn("#define VIA2_IER_Never0 0xFF");
			WriteDestFileLn("#define VIA2_IER_Never1 0xFF");
		}

		WriteDestFileLn("#define VIA2_CB2modesAllowed 0x01");
		WriteDestFileLn("#define VIA2_CA2modesAllowed 0x01");
	}

	WriteBlankLineToDestFile();
	if (cur_mdl <= gbk_mdl_Plus) {
		WriteDestFileLn("#define Mouse_Enabled SCC_InterruptsEnabled");
	} else {
		WriteDestFileLn("#define Mouse_Enabled() (! ADBMouseDisabled)");
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("#define RTC_OneSecond_PulseNtfy VIA1_iCA2_PulseNtfy");

	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define Vid_VBLinterrupt_PulseNotify VIA2_iCA1_PulseNtfy");
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("#define GetSoundInvertTime VIA1_GetT1InvertTime");

	if (EmClassicKbrd) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define KYBD_ShiftInData VIA1_ShiftOutData");
		WriteDestFileLn("#define KYBD_ShiftOutData VIA1_ShiftInData");
	}
	if (EmADB) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define ADB_ShiftInData VIA1_ShiftOutData");
		WriteDestFileLn("#define ADB_ShiftOutData VIA1_ShiftInData");
	}

	WriteCloseDestFile();
	}
}
