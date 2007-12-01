/*
	SPBLDOPT.i
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
	program SPecific BuiLD OPTions
*/

/* option: sound */

LOCALVAR blnr MySoundEnabled;

LOCALPROC ResetSoundOption(void)
{
	MySoundEnabled = nanblnr;
}

LOCALFUNC blnr TryAsSoundOptionNot(void)
{
	return BooleanTryAsOptionNot("-sound", &MySoundEnabled);
}

LOCALPROC ChooseSoundEnabled(void)
{
	if (nanblnr == MySoundEnabled) {
		MySoundEnabled = (gbo_apifam == gbk_apifam_mac) || (gbo_apifam == gbk_apifam_osx) || (gbo_apifam == gbk_apifam_win);
	}
}

/* option: model */

enum {
	gbk_mdl_128K,
	gbk_mdl_512K,
	gbk_mdl_512Ke,
	gbk_mdl_Plus1M,
	gbk_mdl_Plus2M,
	gbk_mdl_Plus2_5M,
	gbk_mdl_Plus,
	gbk_mdl_SE1M,
	gbk_mdl_SE2M,
	gbk_mdl_SE2_5M,
	gbk_mdl_SE,
	kNumModels
};

LOCALVAR int cur_mdl;

LOCALPROC ResetModelOption(void)
{
	cur_mdl = kListOptionAuto;
}

LOCALFUNC char * GetModelName(int i)
{
	char *s;

	switch (i) {
		case gbk_mdl_128K:
			s = "128K";
			break;
		case gbk_mdl_512K:
			s = "512K";
			break;
		case gbk_mdl_512Ke:
			s = "512Ke";
			break;
		case gbk_mdl_Plus1M:
			s = "Plus1M";
			break;
		case gbk_mdl_Plus2M:
			s = "Plus2M";
			break;
		case gbk_mdl_Plus2_5M:
			s = "Plus2_5M";
			break;
		case gbk_mdl_Plus:
			s = "Plus";
			break;
		case gbk_mdl_SE1M:
			s = "SE1M";
			break;
		case gbk_mdl_SE2M:
			s = "SE2M";
			break;
		case gbk_mdl_SE2_5M:
			s = "SE2_5M";
			break;
		case gbk_mdl_SE:
			s = "SE";
			break;
		default:
			s = "(unknown Model)";
			break;
	}
	return s;
}

LOCALPROC ChooseModel(void)
{
	if (cur_mdl == kListOptionAuto) {
		cur_mdl = gbk_mdl_Plus;
	}
}

LOCALFUNC blnr TryAsModelOptionNot(void)
{
	return FindNamedOption("-m", kNumModels, GetModelName, &cur_mdl);
}

/* option: number of drives */

LOCALVAR uimr cur_numdrives;
LOCALVAR blnr have_numdrives;

LOCALPROC ResetNumDrivesOption(void)
{
	have_numdrives = falseblnr;
}

LOCALFUNC blnr TryAsNumDrivesOptionNot(void)
{
	return NumberTryAsOptionNot("-drives", (int *)&cur_numdrives, &have_numdrives);
}

LOCALFUNC blnr ChooseNumDrives(void)
{
	if (! have_numdrives) {
		if (cur_mdl <= gbk_mdl_512Ke) {
			cur_numdrives = 2;
		} else {
			cur_numdrives = 6;
		}
		have_numdrives = trueblnr;
	} else {
		if ((cur_numdrives <= 0) || (cur_numdrives > 32)) {
			ReportParseFailure("-drives must be a number between 1 and 32");
			return falseblnr;
		}
	}
	return trueblnr;
}

/* option: Emulated CPU version */

LOCALVAR uimr em_cpu_vers;
LOCALVAR blnr have_em_cpu_vers;

LOCALPROC ResetEmCpuVersOption(void)
{
	have_em_cpu_vers = falseblnr;
}

LOCALFUNC blnr TryAsEmCpuVersOptionNot(void)
{
	return NumberTryAsOptionNot("-em-cpu", (int *)&em_cpu_vers, &have_em_cpu_vers);
}

LOCALPROC ChooseEmCpuVers(void)
{
	if (! have_em_cpu_vers) {
		em_cpu_vers = 0;
		have_em_cpu_vers = trueblnr;
	}
}

/* option: sound */

LOCALVAR blnr WantInitFullScreen;

LOCALPROC ResetInitFullScreen(void)
{
	WantInitFullScreen = nanblnr;
}

LOCALFUNC blnr TryAsInitFullScreenNot(void)
{
	return BooleanTryAsOptionNot("-fullscreen", &WantInitFullScreen);
}

LOCALPROC ChooseInitFullScreen(void)
{
	if (nanblnr == WantInitFullScreen) {
		WantInitFullScreen = falseblnr;
	}
}

/* option: Initial Magnify */

LOCALVAR blnr WantInitMagnify;

LOCALPROC ResetInitMagnify(void)
{
	WantInitMagnify = nanblnr;
}

LOCALFUNC blnr TryAsInitMagnifyNot(void)
{
	return BooleanTryAsOptionNot("-magnify", &WantInitMagnify);
}

LOCALPROC ChooseInitMagnify(void)
{
	if (nanblnr == WantInitMagnify) {
		WantInitMagnify = falseblnr;
	}
}

/* option: Alternate Keyboard Mode */

LOCALVAR blnr WantAltKeysMode;

LOCALPROC ResetAltKeysMode(void)
{
	WantAltKeysMode = falseblnr;
}

LOCALFUNC blnr TryAsAltKeysModeNot(void)
{
	return FlagTryAsOptionNot("-akm", &WantAltKeysMode);
}

/* option: Command Option Swap */

LOCALVAR blnr WantCmndOptSwap;

LOCALPROC ResetCmndOptSwap(void)
{
	WantCmndOptSwap = falseblnr;
}

LOCALFUNC blnr TryAsCmndOptSwapNot(void)
{
	return FlagTryAsOptionNot("-ccs", &WantCmndOptSwap);
}

/* option: Initial Run In Background */

LOCALVAR blnr WantInitBackground;

LOCALPROC ResetInitBackground(void)
{
	WantInitBackground = nanblnr;
}

LOCALFUNC blnr TryAsInitBackgroundNot(void)
{
	return BooleanTryAsOptionNot("-bg", &WantInitBackground);
}

LOCALPROC ChooseInitBackground(void)
{
	if (nanblnr == WantInitBackground) {
		WantInitBackground = falseblnr;
	}
}

/* option: initial speed */

enum {
	gbk_speed_AllOut,
	gbk_speed_1X,
	gbk_speed_2X,
	gbk_speed_4X,
	gbk_speed_8X,
	gbk_speed_16X,
	gbk_speed_32X,
	kNumSpeeds
};

LOCALVAR int CurInitSpeed;

LOCALPROC ResetInitSpeedOption(void)
{
	CurInitSpeed = kListOptionAuto;
}

LOCALFUNC char * GetInitSpeedName(int i)
{
	char *s;

	switch (i) {
		case gbk_speed_AllOut:
			s = "a";
			break;
		case gbk_speed_1X:
			s = "z";
			break;
		case gbk_speed_2X:
			s = "1";
			break;
		case gbk_speed_4X:
			s = "2";
			break;
		case gbk_speed_8X:
			s = "3";
			break;
		case gbk_speed_16X:
			s = "4";
			break;
		case gbk_speed_32X:
			s = "5";
			break;
		default:
			s = "(unknown Speed)";
			break;
	}
	return s;
}

LOCALPROC ChooseInitSpeed(void)
{
	if (CurInitSpeed == kListOptionAuto) {
		CurInitSpeed = gbk_speed_8X;
	}
}

LOCALFUNC blnr TryAsInitSpeedOptionNot(void)
{
	return FindNamedOption("-speed", kNumSpeeds, GetInitSpeedName, &CurInitSpeed);
}

/* option: Alternate Keyboard Mode */

LOCALVAR blnr WantMinExtn;

LOCALPROC ResetWantMinExtn(void)
{
	WantMinExtn = falseblnr;
}

LOCALFUNC blnr TryAsWantMinExtnNot(void)
{
	return FlagTryAsOptionNot("-min-extn", &WantMinExtn);
}

/* option: horizontal resolution */

LOCALVAR uimr cur_hres;
LOCALVAR blnr have_hres;

LOCALPROC ResetHResOption(void)
{
	have_hres = falseblnr;
}

LOCALFUNC blnr TryAsHResOptionNot(void)
{
	return NumberTryAsOptionNot("-hres", (int *)&cur_hres, &have_hres);
}

LOCALVAR uimr dflt_hres;

LOCALFUNC blnr ChooseHRes(void)
{
	dflt_hres = 512;
	if (! have_hres) {
		cur_hres = dflt_hres;
		have_hres = trueblnr;
	} else {
		if ((cur_hres & 0x1F) != 0) {
			ReportParseFailure("-hres must be a multiple of 32");
			return falseblnr;
		} else if (cur_hres < 128) {
			ReportParseFailure("-hres must be >= 128");
			return falseblnr;
		} else if (cur_hres >= (uimr)32 * 1024) {
			ReportParseFailure("-hres must be < 32k");
			return falseblnr;
		}
	}
	return trueblnr;
}

/* option: vertical resolution */

LOCALVAR uimr cur_vres;
LOCALVAR blnr have_vres;

LOCALPROC ResetVResOption(void)
{
	have_vres = falseblnr;
}

LOCALFUNC blnr TryAsVResOptionNot(void)
{
	return NumberTryAsOptionNot("-vres", (int *)&cur_vres, &have_vres);
}

LOCALVAR uimr dflt_vres;

LOCALFUNC blnr ChooseVRes(void)
{
	dflt_vres = 342;
	if (! have_vres) {
		cur_vres = dflt_vres;
		have_vres = trueblnr;
	} else {
		if (cur_vres < 128) {
			ReportParseFailure("-vres must be >= 128");
			return falseblnr;
		} else if (cur_vres >= (uimr)32 * 1024) {
			ReportParseFailure("-vres must be < 32k");
			return falseblnr;
		}
	}
	return trueblnr;
}

/* ------ */

LOCALVAR blnr NeedScrnHack;

LOCALFUNC blnr ChooseScreenOpts(void)
{
	if (cur_hres * cur_vres > (uimr)2 * 1024 * 1024) {
		ReportParseFailure("-hres and -vres multiplied must be < 2M");
		return falseblnr;
	}
	NeedScrnHack = (cur_hres != dflt_hres)
		|| (cur_vres != dflt_vres);
	return trueblnr;
}

/* ------ */

LOCALPROC SPResetCommandLineParameters(void)
{
	ResetModelOption();
	ResetNumDrivesOption();
	ResetSoundOption();
	ResetEmCpuVersOption();
	ResetInitFullScreen();
	ResetInitMagnify();
	ResetAltKeysMode();
	ResetCmndOptSwap();
	ResetInitBackground();
	ResetInitSpeedOption();
	ResetWantMinExtn();
	ResetHResOption();
	ResetVResOption();
}

LOCALFUNC blnr TryAsSPOptionNot(void)
{
	if (TryAsModelOptionNot())
	if (TryAsNumDrivesOptionNot())
	if (TryAsSoundOptionNot())
	if (TryAsEmCpuVersOptionNot())
	if (TryAsInitFullScreenNot())
	if (TryAsInitMagnifyNot())
	if (TryAsAltKeysModeNot())
	if (TryAsCmndOptSwapNot())
	if (TryAsInitBackgroundNot())
	if (TryAsInitSpeedOptionNot())
	if (TryAsWantMinExtnNot())
	if (TryAsHResOptionNot())
	if (TryAsVResOptionNot())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALFUNC blnr AutoChooseSPSettings(void)
{
	ChooseSoundEnabled();
	ChooseModel();
	if (ChooseNumDrives())
	if (ChooseHRes())
	if (ChooseVRes())
	if (ChooseScreenOpts())
	{
		ChooseEmCpuVers();
		ChooseInitFullScreen();
		ChooseInitMagnify();
		ChooseInitBackground();
		ChooseInitSpeed();
		return trueblnr;
	}

	return falseblnr;
}
