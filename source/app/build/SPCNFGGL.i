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
	WriteBlankLineToDestFile();

	WriteCompCondBool("DetailedAbnormalReport", falseblnr);
	WriteCompCondBool("MakeDumpFile", falseblnr);

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define NumDrives ");
	WriteUnsignedToOutput(cur_numdrives);
	WriteEndDestFileLn();

	WriteCompCondBool("IncludeSonyRawMode", (! WantMinExtn));
	WriteCompCondBool("IncludeSonyGetName", (! WantMinExtn) && (gbk_apifam_gtk != gbo_apifam));
	WriteCompCondBool("IncludeSonyNew", (! WantMinExtn) && (gbk_apifam_gtk != gbo_apifam));
	WriteCompCondBool("IncludeSonyNameNew", (! WantMinExtn) && (gbk_apifam_gtk != gbo_apifam));

	WriteBlankLineToDestFile();

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


	WriteBlankLineToDestFile();

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
		case gbk_mdl_IIx:
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
		case gbk_mdl_IIx:
			WriteCStrToDestFile("MacIIx.ROM");
			break;
		case gbk_mdl_512Ke:
		case gbk_mdl_Plus:
		default:
			WriteCStrToDestFile("vMac.ROM");
			break;
	}
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

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


	WriteBlankLineToDestFile();

	WriteCompCondBool("IncludePbufs", (! WantMinExtn) && (gbk_apifam_gtk != gbo_apifam));

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define NumPbufs ");
	WriteUnsignedToOutput(4);
	WriteEndDestFileLn();


	WriteBlankLineToDestFile();

	WriteCompCondBool("EnableMouseMotion", (gbk_apifam_gtk != gbo_apifam));

	WriteBlankLineToDestFile();

	WriteCompCondBool("IncludeHostTextClipExchange", (! WantMinExtn) && (gbk_apifam_gtk != gbo_apifam));

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define WantInitSpeedValue ");
	if (gbk_speed_AllOut == CurInitSpeed) {
		WriteCStrToDestFile("-1");
	} else {
		WriteUnsignedToOutput(CurInitSpeed - 1);
	}
	WriteEndDestFileLn();
}
