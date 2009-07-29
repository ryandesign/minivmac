/*
	SPCNFGAP.i
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
	program SPecific CoNFiGuration of API users

	(that is, configuration settings that are used
	only by platform specific code)
*/


LOCALPROC WriteAppSpecificCNFGRAPIoptions(void)
{
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

	if ((gbk_targ_wcar == cur_targ) || (gbk_targ_wc86 == cur_targ)) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define EnableShellLinks 0");
		WriteDestFileLn("#define EnableDragDrop 0");
		WriteDestFileLn("#define UseTimerThread 0");
	} else {
		WriteDestFileLn("#define EnableDragDrop 1");
	}
	WriteCompCondBool("EnableAltKeysMode", WantAltKeysMode);
	WriteCompCondBool("SwapCommandControl", WantCmndOptSwap);
	WriteCompCondBool("WantInitMagnify", WantInitMagnify);
	WriteCompCondBool("WantInitFullScreen", WantInitFullScreen);
	WriteCompCondBool("WantInitRunInBackground", WantInitBackground);

	if (gbk_apifam_gtk == gbo_apifam) {
		/* temporary, until implemented */
		WriteDestFileLn("#define EnableMagnify 0");
		WriteDestFileLn("#define EnableFullScreen 0");
	} else {
		WriteDestFileLn("#define EnableMagnify 1");
		WriteDestFileLn("#define EnableFullScreen 1");
	}

	if (WantScreenVSync) {
		WriteDestFileLn("#define UseAGLdoublebuff 1");
	}

	WriteCompCondBool("NeedRequestInsertDisk",
		(gbk_apifam_gtk == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		);
	WriteCompCondBool("NeedDoMoreCommandsMsg",
		(gbk_apifam_gtk == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		);
	WriteCompCondBool("NeedDoAboutMsg",
		(gbk_apifam_gtk == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		);
	WriteCompCondBool("UseControlKeys", trueblnr);
	WriteCompCondBool("UseActvCode", WantActvCode);

	if (WantActvCode) {
		int i;

		WriteBlankLineToDestFile();
		for (i = 0; i < NumKeyCon; ++i) {
			WriteBgnDestFileLn();
			WriteCStrToDestFile("#define KeyCon");
			WriteUnsignedToOutput(i);
			WriteCStrToDestFile(" ");
			WriteUnsignedToOutput(KeyCon[i]);
			WriteEndDestFileLn();
		}
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("/* version and other info to display to user */");

	WriteBlankLineToDestFile();

	WriteCDefQuote("kStrAppName", WriteStrAppUnabrevName);
	WriteCDefQuote("kAppVariationStr", WriteAppVariationStr);
	WriteCDefQuote("kStrCopyrightYear", WriteAppCopyrightYearStr);
	WriteCDefQuote("kMaintainerName", WriteMaintainerName);
	WriteCDefQuote("kStrHomePage", WriteHomePage);

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
}
