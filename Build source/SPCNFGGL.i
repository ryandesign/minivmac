/*
	SPCNFGGL.i
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
	program SPecific CoNFiGuration GLobals
*/

LOCALPROC WriteAppSpecificCNFGGLOBoptions(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define CurEmMd kEmMd_");
	WriteCStrToDestFile(GetModelName(cur_mdl));
	WriteEndDestFileLn();

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

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kTrueROM_Size ");
	switch (cur_mdl) {
		case gbk_mdl_128K:
			WriteCStrToDestFile("0x010000"); /* 64 KB */
			break;
		case gbk_mdl_512Ke:
		case gbk_mdl_Plus:
			WriteCStrToDestFile("0x020000"); /* 128 KB */
			break;
		case gbk_mdl_SE:
		case gbk_mdl_SEFDHD:
		case gbk_mdl_PB100:
		case gbk_mdl_II:
			WriteCStrToDestFile("0x040000"); /* 256 KB */
			break;
		case gbk_mdl_Classic:
		default:
			WriteCStrToDestFile("0x080000"); /* 512 KB */
			break;
	}
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kROM_Size ");
	if (gbk_mdl_128K == cur_mdl) {
		WriteCStrToDestFile("0x020000"); /* 128 KB */
	} else {
		WriteCStrToDestFile("kTrueROM_Size");
	}
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kCheckSumRom_Size ");
	if (gbk_mdl_Classic == cur_mdl) {
		WriteCStrToDestFile("0x040000"); /* 256 KB */
	} else {
		WriteCStrToDestFile("kTrueROM_Size");
	}
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define RomFileName \"");
	switch (cur_mdl) {
		case gbk_mdl_128K:
			WriteCStrToDestFile("Mac128K.ROM");
			break;
		case gbk_mdl_SE:
			WriteCStrToDestFile("MacSE.ROM");
			break;
		case gbk_mdl_SEFDHD:
			WriteCStrToDestFile("SEFDHD.ROM");
			break;
		case gbk_mdl_Classic:
			WriteCStrToDestFile("Classic.ROM");
			break;
		case gbk_mdl_PB100:
			WriteCStrToDestFile("PB100.ROM");
			break;
		case gbk_mdl_II:
			WriteCStrToDestFile("MacII.ROM");
			break;
		case gbk_mdl_512Ke:
		case gbk_mdl_Plus:
		default:
			WriteCStrToDestFile("vMac.ROM");
			break;
	}
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kEmuName \"");
	switch (cur_mdl) {
		case gbk_mdl_128K:
			if (gbk_msz_128K == cur_msz) {
				WriteCStrToDestFile("128K");
			} else {
				WriteCStrToDestFile("512K");
			}
			break;
		case gbk_mdl_512Ke:
			WriteCStrToDestFile("512Ke");
			break;
		default:
			WriteCStrToDestFile(GetModelName(cur_mdl));
			WriteCStrToDestFile(" ");
			WriteCStrToDestFile(GetMemSizName(cur_msz));
			break;
	}
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define NumDrives ");
	WriteUnsignedToOutput(cur_numdrives);
	WriteEndDestFileLn();

	if (gbk_mdl_II == cur_mdl) {
		WriteDestFileLn("#define IncludeVidRom 1");
		WriteDestFileLn("#define kVidROM_Size 0x000800");
	}

	if (NeedVidMem) {
		uimr FrameSize;
		WriteDestFileLn("#define IncludeVidMem 1");
		if (gbk_mdl_II == cur_mdl) {
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

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define vMacScreenHeight ");
	WriteUnsignedToOutput(cur_vres);
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define vMacScreenWidth ");
	WriteUnsignedToOutput(cur_hres);
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define vMacScreenDepth ");
	WriteUnsignedToOutput(cur_ScrnDpth);
	WriteEndDestFileLn();

	if (WantInitFullScreen) {
		WriteDestFileLn("#define WantInitFullScreen 1");
	}

	if (WantInitMagnify) {
		WriteDestFileLn("#define WantInitMagnify 1");
	}

	if (WantInitBackground) {
		WriteDestFileLn("#define WantInitRunInBackground 1");
	}

	if (CurInitSpeed != gbk_speed_8X) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define WantInitSpeedValue ");
		if (gbk_speed_AllOut == CurInitSpeed) {
			WriteCStrToDestFile("-1");
		} else {
			WriteUnsignedToOutput(CurInitSpeed - 1);
		}
		WriteEndDestFileLn();
	}

	if (WantCmndOptSwap) {
		WriteDestFileLn("#define SwapCommandControl 1");
	}

	if (WantMinExtn) {
		WriteDestFileLn("#define IncludePbufs 0");
		WriteDestFileLn("#define IncludeSonyRawMode 0");
		WriteDestFileLn("#define IncludeSonyGetName 0");
		WriteDestFileLn("#define IncludeSonyNew 0");
		WriteDestFileLn("#define IncludeSonyNameNew 0");
		WriteDestFileLn("#define IncludeHostTextClipExchange 0");
	}

	if (gbk_apifam_gtk == gbo_apifam) {
		/* temporary, until implemented */
		WriteDestFileLn("#define EnableMagnify 0");
		WriteDestFileLn("#define EnableFullScreen 0");
		WriteDestFileLn("#define EnableMouseMotion 0");
		WriteDestFileLn("#define IncludeHostTextClipExchange 0");
		WriteDestFileLn("#define IncludeSonyGetName 0");
		WriteDestFileLn("#define IncludeSonyNew 0");
		WriteDestFileLn("#define IncludeSonyNameNew 0");
		WriteDestFileLn("#define IncludePbufs 0");
	}
}
