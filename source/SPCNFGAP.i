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
	WriteBlankLineToDestFile();
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

	if (DbgLogHAVE) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define dbglog_buflnsz ");
		WriteUnsignedToOutput(dbglog_buflnsz);
		WriteEndDestFileLn();
	}

	if (gbk_targfam_wnce == gbo_targfam) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define EnableShellLinks 0");
		WriteDestFileLn("#define EnableDragDrop 0");
		WriteDestFileLn("#define UseTimerThread 0");
	} else if (gbk_targfam_lnds == gbo_targfam) {
		WriteDestFileLn("#define EnableDragDrop 0");
	} else {
		WriteDestFileLn("#define EnableDragDrop 1");
	}
	WriteCompCondBool("EnableAltKeysMode", WantAltKeysMode);
	WriteCompCondBool("SwapCommandControl", WantCmndOptSwap);

	WriteCompCondBool("VarFullScreen", WantVarFullScreen);
	if (WantVarFullScreen) {
		WriteCompCondBool("WantInitFullScreen", WantInitFullScreen);
	}
	WriteCompCondBool("MayFullScreen",
		WantVarFullScreen || WantInitFullScreen);
	WriteCompCondBool("MayNotFullScreen",
		WantVarFullScreen || ! WantInitFullScreen);

	WriteCompCondBool("WantInitMagnify", WantInitMagnify);

	WriteCompCondBool("EnableMagnify", 1 != cur_MagFctr);
	if (1 != cur_MagFctr) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define MyWindowScale ");
		WriteUnsignedToOutput(cur_MagFctr);
		WriteEndDestFileLn();
	}

	WriteCompCondBool("WantInitRunInBackground", WantInitBackground);
	WriteCompCondBool("WantInitNotAutoSlow", ! WantInitAutoSlow);

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define WantInitSpeedValue ");
	if (gbk_speed_AllOut == CurInitSpeed) {
		WriteCStrToDestFile("-1");
	} else {
		WriteUnsignedToOutput(CurInitSpeed - 1);
	}
	WriteEndDestFileLn();

	if (WantScreenVSync) {
		WriteDestFileLn("#define UseAGLdoublebuff 1");
	}

	WriteCompCondBool("NeedRequestInsertDisk",
		(gbk_apifam_gtk == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
		);
	WriteCompCondBool("NeedDoMoreCommandsMsg",
		(gbk_apifam_gtk == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
		);
	WriteCompCondBool("NeedDoAboutMsg",
		(gbk_apifam_gtk == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
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

	WriteCompCondBool("NeedIntlChars",
		(gbo_lang != gbk_lang_eng) || NeedIntl);
	WriteCDefQuote("kStrAppName", WriteStrAppUnabrevName);
	WriteCDefQuote("kAppVariationStr", WriteAppVariationStr);
	WriteCDefQuote("kStrCopyrightYear", WriteAppCopyrightYearStr);
	WriteCDefQuote("kMaintainerName", WriteMaintainerName);
	WriteCDefQuote("kStrHomePage", WriteHomePage);
	if (NULL != hSponsorName) {
		WriteCDefQuote("kStrSponsorName", WriteSponsorName);
	}
}