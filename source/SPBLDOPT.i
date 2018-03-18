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

/* option: model */

enum {
	gbk_mdl_Twig43,
	gbk_mdl_Twiggy,
	gbk_mdl_128K,
	gbk_mdl_512Ke,
	gbk_mdl_Plus,
	gbk_mdl_SE,
	gbk_mdl_SEFDHD,
	gbk_mdl_Classic,
	gbk_mdl_PB100,
	gbk_mdl_II,
	gbk_mdl_IIx,
	kNumModels
};

LOCALVAR int cur_mdl;
#define dfo_mdl gbk_mdl_Plus

LOCALPROC ResetModelOption(void)
{
	cur_mdl = kListOptionAuto;
}

LOCALFUNC char * GetModelName(int i)
{
	char *s;

	switch (i) {
		case gbk_mdl_Twig43:
			s = "Twig43";
			break;
		case gbk_mdl_Twiggy:
			s = "Twiggy";
			break;
		case gbk_mdl_128K:
			s = "128K";
			break;
		case gbk_mdl_512Ke:
			s = "512Ke";
			break;
		case gbk_mdl_Plus:
			s = "Plus";
			break;
		case gbk_mdl_SE:
			s = "SE";
			break;
		case gbk_mdl_SEFDHD:
			s = "SEFDHD";
			break;
		case gbk_mdl_Classic:
			s = "Classic";
			break;
		case gbk_mdl_PB100:
			s = "PB100";
			break;
		case gbk_mdl_II:
			s = "II";
			break;
		case gbk_mdl_IIx:
			s = "IIx";
			break;
		default:
			s = "(unknown Model)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr TryAsModelOptionNot(void)
{
	return FindNamedOption("-m", kNumModels, GetModelName, &cur_mdl);
}

LOCALFUNC tMyErr ChooseModel(void)
{
	if (kListOptionAuto == cur_mdl) {
		cur_mdl = dfo_mdl;
	}

	return noErr;
}

LOCALPROC WrtOptModelOption(void)
{
	WrtOptNamedOption("-m", GetModelName, cur_mdl, dfo_mdl);
}


/* option: horizontal resolution */

LOCALVAR uimr cur_hres;
LOCALVAR uimr dfo_hres;
LOCALVAR blnr have_hres;

LOCALPROC ResetHResOption(void)
{
	have_hres = falseblnr;
}

LOCALFUNC tMyErr TryAsHResOptionNot(void)
{
	return NumberTryAsOptionNot("-hres",
		(long *)&cur_hres, &have_hres);
}

LOCALFUNC tMyErr ChooseHRes(void)
{
	tMyErr err;

	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		dfo_hres = 640;
	} else if (gbk_mdl_PB100 == cur_mdl) {
		dfo_hres = 640;
	} else {
		dfo_hres = 512;
	}

	if (! have_hres) {
		cur_hres = dfo_hres;
		have_hres = trueblnr;
		err = noErr;
	} else {
		if ((cur_hres & 0x1F) != 0) {
			ReportParseFailure("-hres must be a multiple of 32");
			err = kMyErrReported;
		} else if (cur_hres < 128) {
			ReportParseFailure("-hres must be >= 128");
			err = kMyErrReported;
		} else if (cur_hres >= (uimr)32 * 1024) {
			ReportParseFailure("-hres must be < 32k");
			err = kMyErrReported;
		} else {
			err = noErr;
		}
	}

	return err;
}

LOCALPROC WrtOptHResOption(void)
{
	WrtOptNumberOption("-hres", cur_hres, dfo_hres);
}


/* option: vertical resolution */

LOCALVAR uimr cur_vres;
LOCALVAR uimr dfo_vres;
LOCALVAR blnr have_vres;

LOCALPROC ResetVResOption(void)
{
	have_vres = falseblnr;
}

LOCALFUNC tMyErr TryAsVResOptionNot(void)
{
	return NumberTryAsOptionNot("-vres", (long *)&cur_vres, &have_vres);
}

LOCALFUNC tMyErr ChooseVRes(void)
{
	tMyErr err;

	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		dfo_vres = 480;
	} else if (gbk_mdl_PB100 == cur_mdl) {
		dfo_vres = 400;
	} else {
		dfo_vres = 342;
	}

	if (! have_vres) {
		cur_vres = dfo_vres;
		have_vres = trueblnr;
		err = noErr;
	} else {
		if (cur_vres < 128) {
			ReportParseFailure("-vres must be >= 128");
			err = kMyErrReported;
		} else if (cur_vres >= (uimr)32 * 1024) {
			ReportParseFailure("-vres must be < 32k");
			err = kMyErrReported;
		} else {
			err = noErr;
		}
	}

	return err;
}

LOCALPROC WrtOptVResOption(void)
{
	WrtOptNumberOption("-vres", cur_vres, dfo_vres);
}


/* option: screen depth */

LOCALVAR uimr cur_ScrnDpth;
LOCALVAR uimr dfo_ScrnDpth;
LOCALVAR blnr have_ScrnDpth;

LOCALPROC ResetScrnDpthOption(void)
{
	have_ScrnDpth = falseblnr;
}

LOCALFUNC tMyErr TryAsScrnDpthOptionNot(void)
{
	return NumberTryAsOptionNot("-depth",
		(long *)&cur_ScrnDpth, &have_ScrnDpth);
}

LOCALFUNC tMyErr ChooseScrnDpth(void)
{
	tMyErr err;

	err = noErr;

	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		dfo_ScrnDpth = 3;
	} else {
		dfo_ScrnDpth = 0;
	}

	if (! have_ScrnDpth) {
		cur_ScrnDpth = dfo_ScrnDpth;
		have_ScrnDpth = trueblnr;
	} else {
		if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
			if (cur_ScrnDpth > 5) {
				ReportParseFailure("-depth must be <= 5");
				err = kMyErrReported;
			}
		} else {
			if (cur_ScrnDpth != 0) {
				ReportParseFailure("-depth must be 0 for this model");
				err = kMyErrReported;
			}
		}
	}

	return err;
}

LOCALPROC WrtOptScrnDpth(void)
{
	WrtOptNumberOption("-depth", cur_ScrnDpth, dfo_ScrnDpth);
}


/* option: Initial FullScreen */

LOCALVAR blnr WantInitFullScreen;
LOCALVAR blnr dfo_InitFullScreen;

LOCALPROC ResetInitFullScreen(void)
{
	WantInitFullScreen = nanblnr;
}

LOCALFUNC tMyErr TryAsInitFullScreenNot(void)
{
	return BooleanTryAsOptionNot("-fullscreen", &WantInitFullScreen);
}

LOCALFUNC tMyErr ChooseInitFullScreen(void)
{
	dfo_InitFullScreen = gbk_targfam_wnce == gbo_targfam;

	if (nanblnr == WantInitFullScreen) {
		WantInitFullScreen = dfo_InitFullScreen;
	}

	return noErr;
}

LOCALPROC WrtOptInitFullScreen(void)
{
	WrtOptBooleanOption("-fullscreen",
		WantInitFullScreen, dfo_InitFullScreen);
}


/* option: Variable FullScreen */

LOCALVAR blnr WantVarFullScreen;
LOCALVAR blnr dfo_VarFullScreen;

LOCALPROC ResetVarFullScreen(void)
{
	WantVarFullScreen = nanblnr;
}

LOCALFUNC tMyErr TryAsVarFullScreenNot(void)
{
	return BooleanTryAsOptionNot("-var-fullscreen",
		&WantVarFullScreen);
}

LOCALFUNC tMyErr ChooseVarFullScreen(void)
{
	if ((gbk_apifam_gtk == gbo_apifam)
		|| (gbk_targfam_wnce == gbo_targfam))
	{
		dfo_VarFullScreen = falseblnr;
	} else {
		dfo_VarFullScreen = trueblnr;
	}

	if (nanblnr == WantVarFullScreen) {
		WantVarFullScreen = dfo_VarFullScreen;
	}

	return noErr;
}

LOCALPROC WrtOptVarFullScreen(void)
{
	WrtOptBooleanOption("-var-fullscreen",
		WantVarFullScreen, dfo_VarFullScreen);
}


/* option: magnification factor */

LOCALVAR uimr cur_MagFctr;
LOCALVAR uimr dfo_MagFctr;
LOCALVAR blnr have_MagFctr;

LOCALPROC ResetMagFctrOption(void)
{
	have_MagFctr = falseblnr;
}

LOCALFUNC tMyErr TryAsMagFctrOptionNot(void)
{
	return NumberTryAsOptionNot("-mf",
		(long *)&cur_MagFctr, &have_MagFctr);
}

LOCALFUNC tMyErr ChooseMagFctr(void)
{
	tMyErr err;

	err = noErr;

	if (gbk_apifam_gtk == gbo_apifam) {
		/* temporary, until implemented */
		dfo_MagFctr = 1;
	} else {
		dfo_MagFctr = 2;
	}

	if (! have_MagFctr) {
		cur_MagFctr = dfo_MagFctr;
		have_MagFctr = trueblnr;
	} else {
		if (cur_MagFctr < 1) {
			ReportParseFailure("-mf must be >= 1");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptMagFctrOption(void)
{
	WrtOptNumberOption("-mf", cur_MagFctr, dfo_MagFctr);
}


/* option: Initial Magnify */

LOCALVAR blnr WantInitMagnify;
#define dfo_WantInitMagnify falseblnr

LOCALPROC ResetInitMagnify(void)
{
	WantInitMagnify = nanblnr;
}

LOCALFUNC tMyErr TryAsInitMagnifyNot(void)
{
	return BooleanTryAsOptionNot("-magnify", &WantInitMagnify);
}

LOCALFUNC tMyErr ChooseInitMagnify(void)
{
	tMyErr err;

	err = noErr;
	if (nanblnr == WantInitMagnify) {
		WantInitMagnify = dfo_WantInitMagnify;
	} else {
		if (WantInitMagnify && (cur_MagFctr == 1)) {
			ReportParseFailure(
				"-magnify 1 does not make sense with -mf 1");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptInitMagnify(void)
{
	WrtOptBooleanOption("-magnify",
		WantInitMagnify, dfo_WantInitMagnify);
}


/* option: sound */

LOCALVAR blnr MySoundEnabled;
LOCALVAR blnr dfo_SoundEnabled;

LOCALPROC ResetSoundOption(void)
{
	MySoundEnabled = nanblnr;
}

LOCALFUNC tMyErr TryAsSoundOptionNot(void)
{
	return BooleanTryAsOptionNot("-sound", &MySoundEnabled);
}

LOCALFUNC tMyErr ChooseSoundEnabled(void)
{
	dfo_SoundEnabled = (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_sdl == gbo_apifam)
		|| (gbk_apifam_sd2 == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
		|| ((gbk_apifam_xwn == gbo_apifam)
			&& ((gbo_targfam == gbk_targfam_linx)
				|| (gbo_targfam == gbk_targfam_fbsd)
				|| (gbo_targfam == gbk_targfam_nbsd)));

	if (nanblnr == MySoundEnabled) {
		MySoundEnabled = dfo_SoundEnabled;
	}

	return noErr;
}

LOCALPROC WrtOptSoundOption(void)
{
	WrtOptBooleanOption("-sound", MySoundEnabled, dfo_SoundEnabled);
}


/* option: sound api */

enum {
	gbk_sndapi_none,
	gbk_sndapi_alsa,
	gbk_sndapi_ddsp,
	kNumSndApiLevels
};

LOCALVAR int gbo_sndapi;
LOCALVAR int dfo_sndapi;

LOCALPROC ResetSndApiOption(void)
{
	gbo_sndapi = kListOptionAuto;
}

LOCALFUNC char * GetSndApiName(int i)
{
	char *s;

	switch (i) {
		case gbk_sndapi_none:
			s = "none";
			break;
		case gbk_sndapi_alsa:
			s = "alsa";
			break;
		case gbk_sndapi_ddsp:
			s = "ddsp";
			break;
		default:
			s = "(unknown sound api)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr TryAsSndApiOptionNot(void)
{
	return FindNamedOption("-snd-api",
		kNumSndApiLevels, GetSndApiName, &gbo_sndapi);
}

LOCALFUNC tMyErr ChooseSndApiOption(void)
{
	tMyErr err = noErr;

	if (! MySoundEnabled) {
		dfo_sndapi = gbk_sndapi_none;
	} else if (gbk_apifam_xwn != gbo_apifam) {
		dfo_sndapi = gbk_sndapi_none;
	} else if (gbo_targfam == gbk_targfam_linx) {
		dfo_sndapi = gbk_sndapi_alsa;
	} else {
		dfo_sndapi = gbk_sndapi_ddsp;
	}

	if (kListOptionAuto == gbo_sndapi) {
		gbo_sndapi = dfo_sndapi;
	}

	return err;
}

LOCALPROC WrtOptSndApiOption(void)
{
	WrtOptNamedOption("-snd-api", GetSndApiName,
		gbo_sndapi, dfo_sndapi);
}


/* option: sound sample size */

LOCALVAR uimr cur_SoundSampSz;
LOCALVAR uimr dfo_SoundSampSz;
LOCALVAR blnr have_SoundSampSz;

LOCALPROC ResetSoundSampSzOption(void)
{
	have_SoundSampSz = falseblnr;
}

LOCALFUNC tMyErr TryAsSoundSampSzOptionNot(void)
{
	return NumberTryAsOptionNot("-sss",
		(long *)&cur_SoundSampSz, &have_SoundSampSz);
}

LOCALFUNC tMyErr ChooseSoundSampSz(void)
{
	tMyErr err;

	err = noErr;

	if (gbk_sndapi_ddsp == gbo_sndapi) {
		dfo_SoundSampSz = 4;
	} else {
		dfo_SoundSampSz = 3;
	}

	if (! have_SoundSampSz) {
		cur_SoundSampSz = dfo_SoundSampSz;
		have_SoundSampSz = trueblnr;
	} else {
		if ((cur_SoundSampSz < 3) || (cur_SoundSampSz > 4)) {
			ReportParseFailure(
				"-sss must be 3 or 4");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptSoundSampSzOption(void)
{
	WrtOptNumberOption("-sss", cur_SoundSampSz, dfo_SoundSampSz);
}


/* option: number of drives */

LOCALVAR uimr cur_numdrives;
LOCALVAR uimr dfo_numdrives;
LOCALVAR blnr have_numdrives;

LOCALPROC ResetNumDrivesOption(void)
{
	have_numdrives = falseblnr;
}

LOCALFUNC tMyErr TryAsNumDrivesOptionNot(void)
{
	return NumberTryAsOptionNot("-drives",
		(long *)&cur_numdrives, &have_numdrives);
}

LOCALFUNC tMyErr ChooseNumDrives(void)
{
	tMyErr err;

	err = noErr;

	if (cur_mdl < gbk_mdl_512Ke) {
		dfo_numdrives = 2;
	} else {
		dfo_numdrives = 6;
	}

	if (! have_numdrives) {
		cur_numdrives = dfo_numdrives;
		have_numdrives = trueblnr;
	} else {
		if ((cur_numdrives <= 0) || (cur_numdrives > 32)) {
			ReportParseFailure(
				"-drives must be a number between 1 and 32");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptNumDrivesOption(void)
{
	WrtOptNumberOption("-drives", cur_numdrives, dfo_numdrives);
}


/* option: disk driver - support tags */

LOCALVAR blnr SonySupportTags;
#define dfo_SonySupportTags falseblnr

LOCALPROC ResetSonySupportTags(void)
{
	SonySupportTags = nanblnr;
}

LOCALFUNC tMyErr TryAsSonySupportTagsNot(void)
{
	return BooleanTryAsOptionNot("-sony-tag", &SonySupportTags);
}

LOCALFUNC tMyErr ChooseSonySupportTags(void)
{
	if (nanblnr == SonySupportTags) {
		SonySupportTags = dfo_SonySupportTags;
	}

	return noErr;
}

LOCALPROC WrtOptSonySupportTags(void)
{
	WrtOptBooleanOption("-sony-tag",
		SonySupportTags, dfo_SonySupportTags);
}


/* option: disk driver - calculate checksums */

LOCALVAR blnr SonyWantChecksumsUpdated;
#define dfo_SonyWantChecksumsUpdated falseblnr

LOCALPROC ResetSonyWantChecksumsUpdated(void)
{
	SonyWantChecksumsUpdated = nanblnr;
}

LOCALFUNC tMyErr TryAsSonyWantChecksumsUpdatedNot(void)
{
	return BooleanTryAsOptionNot("-sony-sum",
		&SonyWantChecksumsUpdated);
}

LOCALFUNC tMyErr ChooseSonyWantChecksumsUpdated(void)
{
	if (nanblnr == SonyWantChecksumsUpdated) {
		SonyWantChecksumsUpdated = dfo_SonyWantChecksumsUpdated;
	}

	return noErr;
}

LOCALPROC WrtOptSonyWantChecksumsUpdated(void)
{
	WrtOptBooleanOption("-sony-sum",
		SonyWantChecksumsUpdated, dfo_SonyWantChecksumsUpdated);
}


/* option: disk driver - support disk copy 4.2 format */

LOCALVAR blnr SonySupportDC42;
#define dfo_SonySupportDC42 trueblnr

LOCALPROC ResetSonySupportDC42(void)
{
	SonySupportDC42 = nanblnr;
}

LOCALFUNC tMyErr TryAsSonySupportDC42Not(void)
{
	return BooleanTryAsOptionNot("-sony-dc42", &SonySupportDC42);
}

LOCALFUNC tMyErr ChooseSonySupportDC42(void)
{
	if (nanblnr == SonySupportDC42) {
		SonySupportDC42 = dfo_SonySupportDC42;
	}

	return noErr;
}

LOCALPROC WrtOptSonySupportDC42(void)
{
	WrtOptBooleanOption("-sony-dc42",
		SonySupportDC42, dfo_SonySupportDC42);
}


/* option: Insert Ith Disk Image */

LOCALVAR blnr WantInsertIthDisk;
#define dfo_InsertIthDisk falseblnr

LOCALPROC ResetInsertIthDisk(void)
{
	WantInsertIthDisk = nanblnr;
}

LOCALFUNC tMyErr TryAsInsertIthDisk(void)
{
	return BooleanTryAsOptionNot("-iid", &WantInsertIthDisk);
}

LOCALFUNC tMyErr ChooseInsertIthDisk(void)
{
	if (nanblnr == WantInsertIthDisk) {
		WantInsertIthDisk = dfo_InsertIthDisk;
	}

	return noErr;
}

LOCALPROC WrtOptInsertIthDisk(void)
{
	WrtOptBooleanOption("-iid",
		WantInsertIthDisk, dfo_InsertIthDisk);
}


/* option: Command Option Swap */

LOCALVAR blnr WantCmndOptSwap;

LOCALPROC ResetCmndOptSwap(void)
{
	WantCmndOptSwap = falseblnr;
}

LOCALFUNC tMyErr TryAsCmndOptSwapNot(void)
{
	return FlagTryAsOptionNot("-ccs", &WantCmndOptSwap);
}

LOCALFUNC tMyErr ChooseCmndOptSwap(void)
{
	return noErr;
}

LOCALPROC WrtOptCmndOptSwap(void)
{
	WrtOptFlagOption("-ccs", WantCmndOptSwap);
}


/* option: Alternate Keyboard Mode */

LOCALVAR blnr WantAltKeysMode;

LOCALPROC ResetAltKeysMode(void)
{
	WantAltKeysMode = falseblnr;
}

LOCALFUNC tMyErr TryAsAltKeysModeNot(void)
{
	return FlagTryAsOptionNot("-akm", &WantAltKeysMode);
}

LOCALFUNC tMyErr ChooseAltKeysMode(void)
{
	return noErr;
}

LOCALPROC WrtOptAltKeysMode(void)
{
	WrtOptFlagOption("-akm", WantAltKeysMode);
}


/* option: ItnlKyBdFix */

LOCALVAR blnr ItnlKyBdFix;
LOCALVAR blnr dfo_ItnlKyBdFix;

LOCALPROC ResetItnlKyBdFixOption(void)
{
	ItnlKyBdFix = nanblnr;
}

LOCALFUNC tMyErr TryAsItnlKyBdFixNot(void)
{
	return BooleanTryAsOptionNot("-ikb", &ItnlKyBdFix);
}

LOCALFUNC tMyErr ChooseItnlKyBdFix(void)
{
	tMyErr err = noErr;

	dfo_ItnlKyBdFix = (gbk_apifam_win == gbo_apifam);

	if (nanblnr == ItnlKyBdFix) {
		ItnlKyBdFix = dfo_ItnlKyBdFix;
	} else {
		if (ItnlKyBdFix) {
			if (gbk_apifam_win != gbo_apifam) {
				ReportParseFailure("-ikb is only for Windows");
				err = kMyErrReported;
			}
		}
	}

	return err;
}

LOCALPROC WrtOptItnlKyBdFix(void)
{
	WrtOptBooleanOption("-ikb", ItnlKyBdFix, dfo_ItnlKyBdFix);
}


/* option: LocalTalk emulation */

LOCALVAR blnr WantLocalTalk;

LOCALPROC ResetLocalTalk(void)
{
	WantLocalTalk = falseblnr;
}

LOCALFUNC tMyErr TryAsLocalTalkNot(void)
{
	return FlagTryAsOptionNot("-lt", &WantLocalTalk);
}

LOCALFUNC tMyErr ChooseLocalTalk(void)
{
	return noErr;
}

LOCALPROC WrtOptLocalTalk(void)
{
	WrtOptFlagOption("-lt", WantLocalTalk);
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
LOCALVAR int dfo_InitSpeed;

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

LOCALFUNC tMyErr TryAsInitSpeedOptionNot(void)
{
	return FindNamedOption("-speed",
		kNumSpeeds, GetInitSpeedName, &CurInitSpeed);
}

LOCALFUNC tMyErr ChooseInitSpeed(void)
{
	if ((gbk_mdl_II == cur_mdl)
		|| (gbk_mdl_IIx == cur_mdl))
	{
		dfo_InitSpeed = gbk_speed_4X;
	} else {
		dfo_InitSpeed = gbk_speed_8X;
	}

	if (kListOptionAuto == CurInitSpeed) {
		CurInitSpeed = dfo_InitSpeed;
	}

	return noErr;
}

LOCALPROC WrtOptInitSpeedOption(void)
{
	WrtOptNamedOption("-speed", GetInitSpeedName,
		CurInitSpeed, dfo_InitSpeed);
}


/* option: Initial Run In Background */

LOCALVAR blnr WantInitBackground;
LOCALVAR blnr dfo_InitBackground;

LOCALPROC ResetInitBackground(void)
{
	WantInitBackground = nanblnr;
}

LOCALFUNC tMyErr TryAsInitBackgroundNot(void)
{
	return BooleanTryAsOptionNot("-bg", &WantInitBackground);
}

LOCALFUNC tMyErr ChooseInitBackground(void)
{
	if (WantLocalTalk) {
		dfo_InitBackground = trueblnr;
	} else {
		dfo_InitBackground = falseblnr;
	}

	if (nanblnr == WantInitBackground) {
		WantInitBackground = dfo_InitBackground;
	}

	return noErr;
}

LOCALPROC WrtOptInitBackground(void)
{
	WrtOptBooleanOption("-bg", WantInitBackground, dfo_InitBackground);
}


/* option: Initial AutoSlow */

LOCALVAR blnr WantInitAutoSlow;
LOCALVAR blnr dfo_InitAutoSlow;

LOCALPROC ResetInitAutoSlow(void)
{
	WantInitAutoSlow = nanblnr;
}

LOCALFUNC tMyErr TryAsInitAutoSlowNot(void)
{
	return BooleanTryAsOptionNot("-as", &WantInitAutoSlow);
}

LOCALFUNC tMyErr ChooseInitAutoSlow(void)
{
	dfo_InitAutoSlow = (gbk_mdl_II != cur_mdl)
		&& (gbk_mdl_IIx != cur_mdl);

	if (nanblnr == WantInitAutoSlow) {
		WantInitAutoSlow = dfo_InitAutoSlow;
	}

	return noErr;
}

LOCALPROC WrtOptInitAutoSlow(void)
{
	WrtOptBooleanOption("-as", WantInitAutoSlow, dfo_InitAutoSlow);
}


/* option: Timing Accuracy */

LOCALVAR uimr timingacc;
#define dfo_timingacc 1
LOCALVAR blnr have_timingacc;

LOCALPROC ResetTimingAccuracyOption(void)
{
	have_timingacc = falseblnr;
}

LOCALFUNC tMyErr TryAsTimingAccuracyOptionNot(void)
{
	return NumberTryAsOptionNot("-ta",
		(long *)&timingacc, &have_timingacc);
}

LOCALFUNC tMyErr ChooseTimingAccuracy(void)
{
	if (! have_timingacc) {
		timingacc = dfo_timingacc;
		have_timingacc = trueblnr;
	}

	return noErr;
}

LOCALPROC WrtOptTimingAccuracy(void)
{
	WrtOptNumberOption("-ta", timingacc, dfo_timingacc);
}


/* option: Emulated CPU version */

LOCALVAR uimr em_cpu_vers;
LOCALVAR uimr dfo_em_cpu_vers;
LOCALVAR blnr have_em_cpu_vers;

LOCALPROC ResetEmCpuVersOption(void)
{
	have_em_cpu_vers = falseblnr;
}

LOCALFUNC tMyErr TryAsEmCpuVersOptionNot(void)
{
	return NumberTryAsOptionNot("-em-cpu",
		(long *)&em_cpu_vers, &have_em_cpu_vers);
}

LOCALFUNC tMyErr ChooseEmCpuVers(void)
{
	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		dfo_em_cpu_vers = 2;
	} else {
		dfo_em_cpu_vers = 0;
	}

	if (! have_em_cpu_vers) {
		em_cpu_vers = dfo_em_cpu_vers;
		have_em_cpu_vers = trueblnr;
	}

	return noErr;
}

LOCALPROC WrtOptEmCpuVers(void)
{
	WrtOptNumberOption("-em-cpu", em_cpu_vers, dfo_em_cpu_vers);
}


/* option: memory size */

enum {
	gbk_msz_128K,
	gbk_msz_512K,
	gbk_msz_1M,
	gbk_msz_2M,
	gbk_msz_2_5M,
	gbk_msz_4M,
	gbk_msz_5M,
	gbk_msz_8M,
	kNumMemSizs
};

LOCALVAR int cur_msz;
LOCALVAR int dfo_msz;

LOCALPROC ResetMemSizOption(void)
{
	cur_msz = kListOptionAuto;
}

LOCALFUNC char * GetMemSizName(int i)
{
	char *s;

	switch (i) {
		case gbk_msz_128K:
			s = "128K";
			break;
		case gbk_msz_512K:
			s = "512K";
			break;
		case gbk_msz_1M:
			s = "1M";
			break;
		case gbk_msz_2M:
			s = "2M";
			break;
		case gbk_msz_2_5M:
			s = "2.5M";
			break;
		case gbk_msz_4M:
			s = "4M";
			break;
		case gbk_msz_5M:
			s = "5M";
			break;
		case gbk_msz_8M:
			s = "8M";
			break;
		default:
			s = "(unknown Memory Size)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr TryAsMemSizOptionNot(void)
{
	return FindNamedOption("-mem",
		kNumMemSizs, GetMemSizName, &cur_msz);
}

LOCALFUNC tMyErr ChooseMemSiz(void)
{
	switch (cur_mdl) {
		case gbk_mdl_Twig43:
		case gbk_mdl_Twiggy:
		case gbk_mdl_128K:
			dfo_msz = gbk_msz_128K;
			break;
		case gbk_mdl_512Ke:
			dfo_msz = gbk_msz_512K;
			break;
		case gbk_mdl_II:
		case gbk_mdl_IIx:
			dfo_msz = gbk_msz_8M;
			break;
		case gbk_mdl_Plus:
		case gbk_mdl_SE:
		case gbk_mdl_SEFDHD:
		case gbk_mdl_Classic:
		case gbk_mdl_PB100:
		default:
			dfo_msz = gbk_msz_4M;
			break;
	}
	if (gbk_targfam_lnds == gbo_targfam) {
		if (dfo_msz > gbk_msz_2M) {
			dfo_msz = gbk_msz_2M;
		}
	}

	if (kListOptionAuto == cur_msz) {
		cur_msz = dfo_msz;
	} else {
		/* should error check here */
	}

	return noErr;
}

LOCALPROC WrtOptMemSizOption(void)
{
	WrtOptNamedOption("-mem", GetMemSizName, cur_msz, dfo_msz);
}

/* memory bank sizes */

LOCALVAR uimr RAMa_Size;
LOCALVAR uimr RAMb_Size;

#define ln2_msz_64K 16
#define ln2_msz_128K 17
#define ln2_msz_256K 18
#define ln2_msz_512K 19
#define ln2_msz_1M 20
#define ln2_msz_2M 21
#define ln2_msz_4M 22
#define ln2_msz_8M 23

LOCALFUNC tMyErr ChooseMemBankSizes(void)
{
	tMyErr err;
	RAMa_Size = 0;
	RAMb_Size = 0;

	switch (cur_mdl) {
		case gbk_mdl_Twig43:
		case gbk_mdl_Twiggy:
		case gbk_mdl_128K:
		case gbk_mdl_512Ke:
			if (cur_msz == gbk_msz_128K) {
				RAMa_Size = ln2_msz_128K;
			} else
			if (cur_msz == gbk_msz_512K) {
				RAMa_Size = ln2_msz_512K;
			} else
			{
				/* unsupported */
			}
			break;
		case gbk_mdl_Plus:
		case gbk_mdl_SE:
		case gbk_mdl_SEFDHD:
		case gbk_mdl_Classic:
			if (cur_msz == gbk_msz_128K) {
				if (gbk_mdl_Plus == cur_mdl) {
					RAMa_Size = ln2_msz_128K;
				}
			} else
			if (cur_msz == gbk_msz_512K) {
				RAMa_Size = ln2_msz_512K;
			} else
			if (cur_msz == gbk_msz_1M) {
				RAMa_Size = ln2_msz_512K;
				RAMb_Size = ln2_msz_512K;
			} else
			if (cur_msz == gbk_msz_2M) {
				RAMa_Size = ln2_msz_2M;
			} else
			if (cur_msz == gbk_msz_2_5M) {
				RAMa_Size = ln2_msz_2M;
				RAMb_Size = ln2_msz_512K;
			} else
			if (cur_msz == gbk_msz_4M) {
				RAMa_Size = ln2_msz_2M;
				RAMb_Size = ln2_msz_2M;
			} else
			{
				/* unsupported */
			}
			break;
		case gbk_mdl_II:
		case gbk_mdl_IIx:
			if (cur_msz == gbk_msz_1M) {
				RAMa_Size = ln2_msz_1M;
			} else
			if (cur_msz == gbk_msz_2M) {
				RAMa_Size = ln2_msz_1M;
				RAMb_Size = ln2_msz_1M;
			} else
			if (cur_msz == gbk_msz_4M) {
				RAMa_Size = ln2_msz_4M;
			} else
			if (cur_msz == gbk_msz_5M) {
				RAMa_Size = ln2_msz_4M;
				RAMb_Size = ln2_msz_1M;
			} else
			if (cur_msz == gbk_msz_8M) {
				RAMa_Size = ln2_msz_4M;
				RAMb_Size = ln2_msz_4M;
			} else
			{
				/* unsupported */
			}
			break;
		case gbk_mdl_PB100:
			if (cur_msz == gbk_msz_4M) {
				RAMa_Size = ln2_msz_4M;
			} else
			{
				/* unsupported */
			}
		default:
			/* unsupported */
			break;
	}

	if (0 == RAMa_Size) {
		ReportParseFailure(
			"memory size (-mem) unsupported for this model (-m)");
		err = kMyErrReported;
	} else {
		err = noErr;
	}

	return err;
}


/* option: Parameter RAM CaretBlinkTime */
	/* usually in 3 (Fast), 8 (Medium), 15 (Slow) */

LOCALVAR uimr cur_CaretBlinkTime;
LOCALVAR uimr dfo_CaretBlinkTime;
LOCALVAR blnr have_CaretBlinkTime;

LOCALPROC ResetCaretBlinkTimeOption(void)
{
	have_CaretBlinkTime = falseblnr;
}

LOCALFUNC tMyErr TryAsCaretBlinkTimeOptionNot(void)
{
	return NumberTryAsOptionNot("-cbt",
		(long *)&cur_CaretBlinkTime, &have_CaretBlinkTime);
}

LOCALFUNC tMyErr ChooseCaretBlinkTime(void)
{
	tMyErr err;

	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		dfo_CaretBlinkTime = 8;
	} else {
		dfo_CaretBlinkTime = 3;
	}

	err = noErr;
	if (! have_CaretBlinkTime) {
		cur_CaretBlinkTime = dfo_CaretBlinkTime;

		have_CaretBlinkTime = trueblnr;
	} else {
		if ((cur_CaretBlinkTime <= 0) || (cur_CaretBlinkTime > 15)) {
			ReportParseFailure(
				"-cbt must be a number between 1 and 15");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptCaretBlinkTime(void)
{
	WrtOptNumberOption("-cbt", cur_CaretBlinkTime, dfo_CaretBlinkTime);
}


/* option: Parameter RAM DoubleClickTime */
	/* usually in 5 (Fast), 8 (Medium), 12 (Slow) */

LOCALVAR uimr cur_DoubleClickTime;
LOCALVAR uimr dfo_DoubleClickTime;
LOCALVAR blnr have_DoubleClickTime;

LOCALPROC ResetDoubleClickTimeOption(void)
{
	have_DoubleClickTime = falseblnr;
}

LOCALFUNC tMyErr TryAsDoubleClickTimeOptionNot(void)
{
	return NumberTryAsOptionNot("-dct",
		(long *)&cur_DoubleClickTime, &have_DoubleClickTime);
}

LOCALFUNC tMyErr ChooseDoubleClickTime(void)
{
	tMyErr err;

	err = noErr;

	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		dfo_DoubleClickTime = 8;
	} else {
		dfo_DoubleClickTime = 5;
	}

	if (! have_DoubleClickTime) {
		cur_DoubleClickTime = dfo_DoubleClickTime;

		have_DoubleClickTime = trueblnr;
	} else {
		if ((cur_DoubleClickTime <= 0) || (cur_DoubleClickTime > 15)) {
			ReportParseFailure(
				"-dct must be a number between 1 and 15");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptDoubleClickTime(void)
{
	WrtOptNumberOption("-dct",
		cur_DoubleClickTime, dfo_DoubleClickTime);
}


/* option: Parameter RAM MenuBlink */
	/* in 0..3 */

LOCALVAR uimr cur_MenuBlink;
#define dfo_MenuBlink 3
LOCALVAR blnr have_MenuBlink;

LOCALPROC ResetMenuBlinkOption(void)
{
	have_MenuBlink = falseblnr;
}

LOCALFUNC tMyErr TryAsMenuBlinkOptionNot(void)
{
	return NumberTryAsOptionNot("-mnb",
		(long *)&cur_MenuBlink, &have_MenuBlink);
}

LOCALFUNC tMyErr ChooseMenuBlink(void)
{
	tMyErr err;

	err = noErr;
	if (! have_MenuBlink) {
		cur_MenuBlink = dfo_MenuBlink;

		have_MenuBlink = trueblnr;
	} else {
		if ((cur_MenuBlink < 0) || (cur_MenuBlink > 3)) {
			ReportParseFailure(
				"-mnb must be a number between 0 and 3");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptMenuBlink(void)
{
	WrtOptNumberOption("-mnb", cur_MenuBlink, dfo_MenuBlink);
}


/* option: Parameter RAM AutoKeyThresh */
	/* usually in 0 (Off), A (Long), 6, 4, 3 (Short) */

LOCALVAR uimr cur_AutoKeyThresh;
#define dfo_AutoKeyThresh 6
LOCALVAR blnr have_AutoKeyThresh;

LOCALPROC ResetAutoKeyThreshOption(void)
{
	have_AutoKeyThresh = falseblnr;
}

LOCALFUNC tMyErr TryAsAutoKeyThreshOptionNot(void)
{
	return NumberTryAsOptionNot("-kyt",
		(long *)&cur_AutoKeyThresh, &have_AutoKeyThresh);
}

LOCALFUNC tMyErr ChooseAutoKeyThresh(void)
{
	tMyErr err;

	err = noErr;
	if (! have_AutoKeyThresh) {
		cur_AutoKeyThresh = dfo_AutoKeyThresh;

		have_AutoKeyThresh = trueblnr;
	} else {
		if ((cur_AutoKeyThresh < 0) || (cur_AutoKeyThresh > 15)) {
			ReportParseFailure(
				"-kyt must be a number between 0 and 15");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptAutoKeyThresh(void)
{
	WrtOptNumberOption("-kyt", cur_AutoKeyThresh, dfo_AutoKeyThresh);
}


/* option: Parameter RAM AutoKeyRate */
	/* usually in 0 (Slow), 6, 4, 3, 1 (Fast) */

LOCALVAR uimr cur_AutoKeyRate;
#define dfo_AutoKeyRate 3
LOCALVAR blnr have_AutoKeyRate;

LOCALPROC ResetAutoKeyRateOption(void)
{
	have_AutoKeyRate = falseblnr;
}

LOCALFUNC tMyErr TryAsAutoKeyRateOptionNot(void)
{
	return NumberTryAsOptionNot("-kyr",
		(long *)&cur_AutoKeyRate, &have_AutoKeyRate);
}

LOCALFUNC tMyErr ChooseAutoKeyRate(void)
{
	tMyErr err;

	err = noErr;
	if (! have_AutoKeyRate) {
		cur_AutoKeyRate = dfo_AutoKeyRate;

		have_AutoKeyRate = trueblnr;
	} else {
		if ((cur_AutoKeyRate < 0) || (cur_AutoKeyRate > 15)) {
			ReportParseFailure(
				"-kyr must be a number between 0 and 15");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptAutoKeyRate(void)
{
	WrtOptNumberOption("-kyr", cur_AutoKeyRate, dfo_AutoKeyRate);
}


/* option: Speaker Volume */
	/* usually in 3 (Fast), 8 (Medium), 15 (Slow) */

LOCALVAR uimr cur_SpeakerVol;
LOCALVAR uimr dfo_SpeakerVol;
LOCALVAR blnr have_SpeakerVol;

LOCALPROC ResetSpeakerVolOption(void)
{
	have_SpeakerVol = falseblnr;
}

LOCALFUNC tMyErr TryAsSpeakerVolOptionNot(void)
{
	return NumberTryAsOptionNot("-svl",
		(long *)&cur_SpeakerVol, &have_SpeakerVol);
}

LOCALFUNC tMyErr ChooseSpeakerVol(void)
{
	tMyErr err;

	err = noErr;

	if (MySoundEnabled) {
		dfo_SpeakerVol = 7;
	} else {
		dfo_SpeakerVol = 0;
	}

	if (! have_SpeakerVol) {
		cur_SpeakerVol = dfo_SpeakerVol;

		have_SpeakerVol = trueblnr;
	} else {
		if ((cur_SpeakerVol < 0) || (cur_SpeakerVol >= 8)) {
			ReportParseFailure(
				"-svl must be a number between 0 and 7");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptSpeakerVol(void)
{
	WrtOptNumberOption("-svl", cur_SpeakerVol, dfo_SpeakerVol);
}


/* option: Minimum Extension */

LOCALVAR blnr WantMinExtn;

LOCALPROC ResetWantMinExtn(void)
{
	WantMinExtn = falseblnr;
}

LOCALFUNC tMyErr TryAsWantMinExtnNot(void)
{
	return FlagTryAsOptionNot("-min-extn", &WantMinExtn);
}

LOCALFUNC tMyErr ChooseWantMinExtn(void)
{
	return noErr;
}

LOCALPROC WrtOptMinExtn(void)
{
	WrtOptFlagOption("-min-extn", WantMinExtn);
}


/* option: MouseMotion */

LOCALVAR blnr MyMouseMotion;
LOCALVAR blnr dfo_MouseMotion;

LOCALPROC ResetMouseMotionOption(void)
{
	MyMouseMotion = nanblnr;
}

LOCALFUNC tMyErr TryAsMouseMotionOptionNot(void)
{
	return BooleanTryAsOptionNot("-emm", &MyMouseMotion);
}

LOCALFUNC tMyErr ChooseMouseMotion(void)
{
	dfo_MouseMotion = (gbk_apifam_gtk != gbo_apifam);

	if (nanblnr == MyMouseMotion) {
		MyMouseMotion = dfo_MouseMotion;
	}

	return noErr;
}

LOCALPROC WrtOptMouseMotion(void)
{
	WrtOptBooleanOption("-emm", MyMouseMotion, dfo_MouseMotion);
}


/* option: GrabKeysFullScreen */

LOCALVAR blnr WantGrabKeysFS;
#define dfo_GrabKeysFS trueblnr

LOCALPROC ResetGrabKeysFS(void)
{
	WantGrabKeysFS = nanblnr;
}

LOCALFUNC tMyErr TryAsGrabKeysFSNot(void)
{
	return BooleanTryAsOptionNot("-gkf", &WantGrabKeysFS);
}

LOCALFUNC tMyErr ChooseGrabKeysFS(void)
{
	tMyErr err;

	err = noErr;
	if (nanblnr == WantGrabKeysFS) {
		WantGrabKeysFS = dfo_GrabKeysFS;
	}

	return err;
}

LOCALPROC WrtOptGrabKeysFS(void)
{
	WrtOptBooleanOption("-gkf", WantGrabKeysFS, dfo_GrabKeysFS);
}


/* option: Enable Control Interrupt */

LOCALVAR blnr WantEnblCtrlInt;
#define dfo_EnblCtrlInt trueblnr

LOCALPROC ResetEnblCtrlInt(void)
{
	WantEnblCtrlInt = nanblnr;
}

LOCALFUNC tMyErr TryAsEnblCtrlIntNot(void)
{
	return BooleanTryAsOptionNot("-eci", &WantEnblCtrlInt);
}

LOCALFUNC tMyErr ChooseEnblCtrlInt(void)
{
	if (nanblnr == WantEnblCtrlInt) {
		WantEnblCtrlInt = dfo_EnblCtrlInt;
	}

	return noErr;
}

LOCALPROC WrtOptEnblCtrlInt(void)
{
	WrtOptBooleanOption("-eci", WantEnblCtrlInt, dfo_EnblCtrlInt);
}


/* option: Enable Control Reset */

LOCALVAR blnr WantEnblCtrlRst;
#define dfo_EnblCtrlRst trueblnr

LOCALPROC ResetEnblCtrlRst(void)
{
	WantEnblCtrlRst = nanblnr;
}

LOCALFUNC tMyErr TryAsEnblCtrlRstNot(void)
{
	return BooleanTryAsOptionNot("-ecr", &WantEnblCtrlRst);
}

LOCALFUNC tMyErr ChooseEnblCtrlRst(void)
{
	if (nanblnr == WantEnblCtrlRst) {
		WantEnblCtrlRst = dfo_EnblCtrlRst;
	}

	return noErr;
}

LOCALPROC WrtOptEnblCtrlRst(void)
{
	WrtOptBooleanOption("-ecr", WantEnblCtrlRst, dfo_EnblCtrlRst);
}


/* option: Enable Control K (emulated control toggle) */

LOCALVAR blnr WantEnblCtrlKtg;
#define dfo_EnblCtrlKtg trueblnr

LOCALPROC ResetEnblCtrlKtg(void)
{
	WantEnblCtrlKtg = nanblnr;
}

LOCALFUNC tMyErr TryAsEnblCtrlKtgNot(void)
{
	return BooleanTryAsOptionNot("-eck", &WantEnblCtrlKtg);
}

LOCALFUNC tMyErr ChooseEnblCtrlKtg(void)
{
	if (nanblnr == WantEnblCtrlKtg) {
		WantEnblCtrlKtg = dfo_EnblCtrlKtg;
	}

	return noErr;
}

LOCALPROC WrtOptEnblCtrlKtg(void)
{
	WrtOptBooleanOption("-eck", WantEnblCtrlKtg, dfo_EnblCtrlKtg);
}


/* option: Want Color Image */

LOCALVAR blnr WantColorImage;
LOCALVAR blnr dfo_ColorImage;

LOCALPROC ResetWantColorImage(void)
{
	WantColorImage = nanblnr;
}

LOCALFUNC tMyErr TryAsWantColorImageNot(void)
{
	return BooleanTryAsOptionNot("-ci", &WantColorImage);
}

LOCALFUNC tMyErr ChooseWantColorImage(void)
{
	tMyErr err;

	err = noErr;

	if (gbk_apifam_xwn == gbo_apifam) {
		dfo_ColorImage = trueblnr;
	} else {
		/* leave as default */
		dfo_ColorImage = nanblnr;
	}

	if (nanblnr == WantColorImage) {
		WantColorImage = dfo_ColorImage;
	} else {
		if (gbk_apifam_xwn != gbo_apifam) {
			ReportParseFailure(
				"-ci is only for -api xwn");
			err = kMyErrReported;
		} else
		if ((! WantColorImage) && (cur_ScrnDpth != 0)) {
			ReportParseFailure(
				"-ci 0 requires -depth 0");
			err = kMyErrReported;
		} else
		{
			/* ok */
		}
	}

	return err;
}

LOCALPROC WrtOptColorImage(void)
{
	WrtOptBooleanOption("-ci", WantColorImage, dfo_ColorImage);
}


/* option: Alternate Happy Mac Icons */

enum {
	gbk_AHM_none,
	gbk_AHM_aside,
	gbk_AHM_cheese,
	gbk_AHM_evil,
	gbk_AHM_horror,
	gbk_AHM_lady_mac,
	gbk_AHM_moustache,
	gbk_AHM_nerdy,
	gbk_AHM_pirate,
	gbk_AHM_sleepy,
	gbk_AHM_sly,
	gbk_AHM_sunglasses,
	gbk_AHM_surprise,
	gbk_AHM_tongue,
	gbk_AHM_yuck,
	gbk_AHM_zombie,

	kNumAHMs
};

LOCALVAR int cur_AltHappyMac;
#define dfo_AltHappyMac gbk_AHM_none

LOCALPROC ResetAltHappyMacOption(void)
{
	cur_AltHappyMac = kListOptionAuto;
}

LOCALFUNC char * GetAltHappyMacName(int i)
{
	char *s;

	switch (i) {
		case gbk_AHM_none:
			s = "none";
			break;
		case gbk_AHM_aside:
			s = "aside";
			break;
		case gbk_AHM_cheese:
			s = "cheese";
			break;
		case gbk_AHM_evil:
			s = "evil";
			break;
		case gbk_AHM_horror:
			s = "horror";
			break;
		case gbk_AHM_lady_mac:
			s = "lady_mac";
			break;
		case gbk_AHM_moustache:
			s = "moustache";
			break;
		case gbk_AHM_nerdy:
			s = "nerdy";
			break;
		case gbk_AHM_pirate:
			s = "pirate";
			break;
		case gbk_AHM_sleepy:
			s = "sleepy";
			break;
		case gbk_AHM_sly:
			s = "sly";
			break;
		case gbk_AHM_sunglasses:
			s = "sunglasses";
			break;
		case gbk_AHM_surprise:
			s = "surprise";
			break;
		case gbk_AHM_tongue:
			s = "tongue";
			break;
		case gbk_AHM_yuck:
			s = "yuck";
			break;
		case gbk_AHM_zombie:
			s = "zombie";
			break;

		default:
			s = "(unknown Alt Happy Mac Icon)";
			break;
	}
	return s;
}

LOCALFUNC tMyErr ChooseAltHappyMac(void)
{
	if (kListOptionAuto == cur_AltHappyMac) {
		cur_AltHappyMac = dfo_AltHappyMac;
	}

	return noErr;
}

LOCALFUNC tMyErr TryAsAltHappyMacOptionNot(void)
{
	return FindNamedOption("-ahm", kNumAHMs, GetAltHappyMacName,
		&cur_AltHappyMac);
}

LOCALPROC WrtOptAltHappyMac(void)
{
	WrtOptNamedOption("-ahm", GetAltHappyMacName,
		cur_AltHappyMac, dfo_AltHappyMac);
}


/* option: ROM size */

LOCALVAR uimr cur_RomSize;
LOCALVAR uimr dfo_RomSize;
LOCALVAR blnr have_RomSize;

LOCALPROC ResetRomSizeOption(void)
{
	have_RomSize = falseblnr;
}

LOCALFUNC tMyErr TryAsRomSizeOptionNot(void)
{
	return NumberTryAsOptionNot("-rsz",
		(long *)&cur_RomSize, &have_RomSize);
}

LOCALFUNC tMyErr ChooseRomSize(void)
{
	tMyErr err;

	err = noErr;

	switch (cur_mdl) {
		case gbk_mdl_Twig43:
		case gbk_mdl_Twiggy:
		case gbk_mdl_128K:
			dfo_RomSize = ln2_msz_64K; /* 64 KB */
			break;
		case gbk_mdl_512Ke:
		case gbk_mdl_Plus:
			dfo_RomSize = ln2_msz_128K; /* 128 KB */
			break;
		case gbk_mdl_SE:
		case gbk_mdl_SEFDHD:
		case gbk_mdl_PB100:
		case gbk_mdl_II:
		case gbk_mdl_IIx:
			dfo_RomSize = ln2_msz_256K; /* 256 KB */
			break;
		case gbk_mdl_Classic:
		default:
			dfo_RomSize = ln2_msz_512K; /* 512 KB */
			break;
	}

	if (! have_RomSize) {
		cur_RomSize = dfo_RomSize;
		have_RomSize = trueblnr;
	} else {
		if ((cur_RomSize < 16) || (cur_RomSize > 31)) {
			ReportParseFailure(
				"-rsz must be a number between 16 and 31");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptRomSize(void)
{
	WrtOptNumberOption("-rsz", cur_RomSize, dfo_RomSize);
}


/* option: Want Check RomCheck Sum */

LOCALVAR blnr WantCheckRomCheckSum;
#define dfo_CheckRomCheckSum trueblnr

LOCALPROC ResetCheckRomCheckSum(void)
{
	WantCheckRomCheckSum = nanblnr;
}

LOCALFUNC tMyErr TryAsCheckRomCheckSumNot(void)
{
	return BooleanTryAsOptionNot("-chr", &WantCheckRomCheckSum);
}

LOCALFUNC tMyErr ChooseCheckRomCheckSum(void)
{
	if (nanblnr == WantCheckRomCheckSum) {
		WantCheckRomCheckSum = dfo_CheckRomCheckSum;
	}

	return noErr;
}

LOCALPROC WrtOptCheckRomCheckSum(void)
{
	WrtOptBooleanOption("-chr",
		WantCheckRomCheckSum, dfo_CheckRomCheckSum);
}


/* option: Want Disable Rom Check */

LOCALVAR blnr WantDisableRomCheck;
#define dfo_DisableRomCheck trueblnr

LOCALPROC ResetDisableRomCheck(void)
{
	WantDisableRomCheck = nanblnr;
}

LOCALFUNC tMyErr TryAsDisableRomCheckNot(void)
{
	return BooleanTryAsOptionNot("-drc", &WantDisableRomCheck);
}

LOCALFUNC tMyErr ChooseDisableRomCheck(void)
{
	if (nanblnr == WantDisableRomCheck) {
		WantDisableRomCheck = dfo_DisableRomCheck;
	}

	return noErr;
}

LOCALPROC WrtOptDisableRomCheck(void)
{
	WrtOptBooleanOption("-drc",
		WantDisableRomCheck, dfo_DisableRomCheck);
}


/* option: Want Disable Ram Test */

LOCALVAR blnr WantDisableRamTest;
#define dfo_DisableRamTest trueblnr

LOCALPROC ResetDisableRamTest(void)
{
	WantDisableRamTest = nanblnr;
}

LOCALFUNC tMyErr TryAsDisableRamTestNot(void)
{
	return BooleanTryAsOptionNot("-drt", &WantDisableRamTest);
}

LOCALFUNC tMyErr ChooseDisableRamTest(void)
{
	if (nanblnr == WantDisableRamTest) {
		WantDisableRamTest = dfo_DisableRamTest;
	}

	return noErr;
}

LOCALPROC WrtOptDisableRamTest(void)
{
	WrtOptBooleanOption("-drt", WantDisableRamTest, dfo_DisableRamTest);
}


/* option: Want Disassembly */

LOCALVAR blnr WantDisasm;
#define dfo_WantDisasm falseblnr

LOCALPROC ResetWantDisasm(void)
{
	WantDisasm = nanblnr;
}

LOCALFUNC tMyErr TryAsWantDisasmNot(void)
{
	return BooleanTryAsOptionNot("-dis", &WantDisasm);
}

LOCALFUNC tMyErr ChooseWantDisasm(void)
{
	if (nanblnr == WantDisasm) {
		WantDisasm = dfo_WantDisasm;
	}

	return noErr;
}

LOCALPROC WrtOptWantDisasmNot(void)
{
	WrtOptBooleanOption("-dis", WantDisasm, dfo_WantDisasm);
}


/* option: dbglog_HAVE */

LOCALVAR blnr DbgLogHAVE;
LOCALVAR blnr dfo_DbgLogHAVE;

LOCALPROC ResetDbgLogHAVE(void)
{
	DbgLogHAVE = nanblnr;
}

LOCALFUNC tMyErr TryAsDbgLogHAVENot(void)
{
	return BooleanTryAsOptionNot("-log", &DbgLogHAVE);
}

LOCALFUNC tMyErr ChooseDbgLogHAVE(void)
{
	dfo_DbgLogHAVE = WantDisasm;

	if (nanblnr == DbgLogHAVE) {
		DbgLogHAVE = dfo_DbgLogHAVE;
	}

	return noErr;
}

LOCALPROC WrtOptDbgLogHAVE(void)
{
	WrtOptBooleanOption("-log", DbgLogHAVE, dfo_DbgLogHAVE);
}


/* option: Screen VSync */

LOCALVAR blnr WantScreenVSync;

LOCALPROC ResetScreenVSync(void)
{
	WantScreenVSync = nanblnr;
}

LOCALFUNC tMyErr TryAsScreenVSyncNot(void)
{
	return BooleanTryAsOptionNot("-vsync", &WantScreenVSync);
}

LOCALFUNC tMyErr ChooseScreenVSync(void)
{
	tMyErr err;

	err = noErr;
	if (nanblnr == WantScreenVSync) {
		WantScreenVSync = falseblnr;
	} else {
		if (WantScreenVSync && (gbk_apifam_osx != gbo_apifam)) {
			ReportParseFailure(
				"-vsync is so far only implemented for OS X");
			err = kMyErrReported;
		}
	}

	return err;
}

LOCALPROC WrtOptScreenVSync(void)
{
	WrtOptFlagOption("-vsync", WantScreenVSync);
}


/* option: Need International Characters */

LOCALVAR blnr NeedIntl;

LOCALPROC ResetNeedIntl(void)
{
	NeedIntl = falseblnr;
}

LOCALFUNC tMyErr TryAsNeedIntlNot(void)
{
	return FlagTryAsOptionNot("-intl", &NeedIntl);
}

LOCALFUNC tMyErr ChooseNeedIntl(void)
{
	return noErr;
}


/* option: Demo Message */

LOCALVAR blnr WantDemoMsg;

LOCALPROC ResetDemoMsg(void)
{
	WantDemoMsg = nanblnr;
}

LOCALFUNC tMyErr TryAsDemoMsgNot(void)
{
	return BooleanTryAsOptionNot("-dmo", &WantDemoMsg);
}

LOCALFUNC tMyErr ChooseDemoMsg(void)
{
	if (nanblnr == WantDemoMsg) {
		WantDemoMsg = falseblnr;
	}

	return noErr;
}


/* option: Activation Code */

LOCALVAR blnr WantActvCode;
#define NumKeyCon 7
LOCALVAR long KeyCon[NumKeyCon];

LOCALPROC ResetActvCode(void)
{
	WantActvCode = falseblnr;
}

LOCALFUNC tMyErr TryAsActvCodeNot(void)
{
	tMyErr err;
	int i;

	if (! CurArgIsCStr_v2("-act")) {
		err = kMyErrNoMatch;
	} else {
		if (falseblnr != WantActvCode) {
			ReportParseFailure("This option has already been defined");
			err = kMyErrReported;
		} else {
			AdvanceTheArg();
			WantActvCode = trueblnr;
			for (i = 0; i < NumKeyCon; ++i) {
				err = GetCurArgOptionAsNumber(&KeyCon[i]);
				if (noErr != err) {
					goto Label_1;
				}
			}
			err = noErr;
Label_1:
			;
		}
	}

	return err;
}

LOCALFUNC tMyErr ChooseActvCode(void)
{
	return noErr;
}

/* ------ */

LOCALVAR blnr NeedScrnHack;
LOCALVAR blnr NeedVidMem;

LOCALFUNC tMyErr ChooseScreenOpts(void)
{
	tMyErr err;

	err = noErr;
	if ((gbk_mdl_II != cur_mdl) && (gbk_mdl_IIx != cur_mdl)) {
		if (cur_hres * cur_vres > (uimr)2 * 1024 * 1024) {
			ReportParseFailure(
				"-hres and -vres multiplied must be < 2M");
			err = kMyErrReported;
		}
	}

	if ((gbk_mdl_PB100 == cur_mdl)
		|| (gbk_mdl_II == cur_mdl)
		|| (gbk_mdl_IIx == cur_mdl))
	{
		NeedScrnHack = falseblnr;
		NeedVidMem = trueblnr;
	} else {
		NeedScrnHack = (cur_hres != dfo_hres)
			|| (cur_vres != dfo_vres);
		NeedVidMem = NeedScrnHack;
	}

	return err;
}

LOCALVAR blnr EmVidCard;

/* video memory size */

LOCALVAR uimr VidMemSize;

LOCALFUNC tMyErr ChooseVidMemSize(void)
{
	tMyErr err;

	EmVidCard = (gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl);

	VidMemSize = (((cur_hres * cur_vres) << cur_ScrnDpth) + 7) >> 3;

	--VidMemSize;
	VidMemSize |= (VidMemSize >> 1);
	VidMemSize |= (VidMemSize >> 2);
	VidMemSize |= (VidMemSize >> 4);
	VidMemSize |= (VidMemSize >> 8);
	VidMemSize |= (VidMemSize >> 16);
	++VidMemSize;

	err = noErr;
	if (! NeedVidMem) {
		VidMemSize = 0;
	} else if (EmVidCard) {
		if (VidMemSize > 4 * 1024 * 1024) {
			ReportParseFailure(
				"video memory must be <= 4M");
			err = kMyErrReported;
		} else if (VidMemSize <= 0x00008000) {
			VidMemSize = 0x00008000;
		}
	} else if (gbk_mdl_PB100 == cur_mdl) {
		VidMemSize = 0x00008000;
	} else {
		/* VidMemSize = 0x00020000; */

		if (VidMemSize > 256 * 1024) {
			ReportParseFailure(
				"video memory must be <= 4M");
			err = kMyErrReported;
		} else if (VidMemSize <= 0x00004000) {
			VidMemSize = 0x00004000;
		}
	}

	return err;
}

/* figure out what hardware to emulate */

LOCALVAR blnr EmClassicKbrd;
LOCALVAR blnr EmADB;
LOCALVAR blnr EmRTC;
LOCALVAR blnr EmPMU;
LOCALVAR blnr EmVIA2;
LOCALVAR blnr EmASC;

LOCALFUNC tMyErr ChooseMiscEmHardware(void)
{
	EmClassicKbrd = cur_mdl <= gbk_mdl_Plus;

	if (cur_mdl <= gbk_mdl_Plus) {
		EmADB = falseblnr;
		EmRTC = trueblnr;
		EmPMU = falseblnr;
	} else
	if ((cur_mdl <= gbk_mdl_Classic)
		|| (gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl))
	{
		EmADB = trueblnr;
		EmRTC = trueblnr;
		EmPMU = falseblnr;
	} else
	{
		EmADB = falseblnr;
		EmRTC = falseblnr;
		EmPMU = trueblnr;
	}

	EmVIA2 = (gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl);
	EmASC = (gbk_mdl_PB100 == cur_mdl) || (gbk_mdl_II == cur_mdl)
		|| (gbk_mdl_IIx == cur_mdl);

	return noErr;
}

/* total memory size */

#define dbglog_buflnsz 18

#define kLn2SoundBuffers 4 /* kSoundBuffers must be a power of two */
#define kLnOneBuffLen 9

#define dbhBufferSize (((1UL << kLn2SoundBuffers) + 1UL) \
	<< (kLnOneBuffLen + cur_SoundSampSz - 3))

#define vMacScreenNumBytes ((((cur_hres * cur_vres) \
	<< cur_ScrnDpth) + 7) >> 3)

LOCALVAR uimr TotMemSize;

LOCALFUNC tMyErr ChooseTotMemSize(void)
{
	TotMemSize = 0;

	if (DbgLogHAVE) {
		TotMemSize += (1 << dbglog_buflnsz);
	}


	/* CntrlDisplayBuff */
	TotMemSize += vMacScreenNumBytes;

	/* screencomparebuff */
	TotMemSize += vMacScreenNumBytes;

	if (1 != cur_MagFctr) {
		/* ScalingBuff */
		TotMemSize += vMacScreenNumBytes * cur_MagFctr * cur_MagFctr;

		/* ScalingTabl */
		TotMemSize += 256 * cur_MagFctr;
	}

	if (MySoundEnabled) {
		/* TheSoundBuffer */
		TotMemSize += dbhBufferSize;
	}

	TotMemSize += (1 << RAMa_Size);
	if (0 != RAMb_Size) {
		TotMemSize += (1 << RAMb_Size);
	}

	if (EmVidCard) {
		TotMemSize += 0x000800; /* kVidROM_Size */
	}

	if (NeedVidMem) {
		TotMemSize += VidMemSize;
	}

	TotMemSize += 512 * 1024UL;
		/* for M68KITAB */

	return noErr;
}

/* ------ */

LOCALPROC SPResetCommandLineParameters(void)
{
	ResetModelOption();
	ResetHResOption();
	ResetVResOption();
	ResetScrnDpthOption();
	ResetInitFullScreen();
	ResetVarFullScreen();
	ResetMagFctrOption();
	ResetInitMagnify();
	ResetSoundOption();
	ResetSndApiOption();
	ResetSoundSampSzOption();
	ResetNumDrivesOption();
	ResetSonySupportTags();
	ResetSonyWantChecksumsUpdated();
	ResetSonySupportDC42();
	ResetInsertIthDisk();
	ResetCmndOptSwap();
	ResetAltKeysMode();
	ResetItnlKyBdFixOption();
	ResetLocalTalk();
	ResetInitSpeedOption();
	ResetInitBackground();
	ResetInitAutoSlow();
	ResetTimingAccuracyOption();
	ResetEmCpuVersOption();
	ResetMemSizOption();
	ResetCaretBlinkTimeOption();
	ResetDoubleClickTimeOption();
	ResetMenuBlinkOption();
	ResetAutoKeyThreshOption();
	ResetAutoKeyRateOption();
	ResetSpeakerVolOption();
	ResetWantMinExtn();
	ResetMouseMotionOption();
	ResetGrabKeysFS();
	ResetEnblCtrlInt();
	ResetEnblCtrlRst();
	ResetEnblCtrlKtg();
	ResetWantColorImage();
	ResetAltHappyMacOption();
	ResetRomSizeOption();
	ResetCheckRomCheckSum();
	ResetDisableRomCheck();
	ResetDisableRamTest();
	ResetWantDisasm();
	ResetDbgLogHAVE();
	ResetScreenVSync();
	ResetNeedIntl();
	ResetActvCode();
	ResetDemoMsg();
}

LOCALFUNC tMyErr TryAsSPOptionNot(void)
{
	tMyErr err;

	if (kMyErrNoMatch == (err = TryAsModelOptionNot()))
	if (kMyErrNoMatch == (err = TryAsHResOptionNot()))
	if (kMyErrNoMatch == (err = TryAsVResOptionNot()))
	if (kMyErrNoMatch == (err = TryAsScrnDpthOptionNot()))
	if (kMyErrNoMatch == (err = TryAsInitFullScreenNot()))
	if (kMyErrNoMatch == (err = TryAsVarFullScreenNot()))
	if (kMyErrNoMatch == (err = TryAsMagFctrOptionNot()))
	if (kMyErrNoMatch == (err = TryAsInitMagnifyNot()))
	if (kMyErrNoMatch == (err = TryAsSoundOptionNot()))
	if (kMyErrNoMatch == (err = TryAsSndApiOptionNot()))
	if (kMyErrNoMatch == (err = TryAsSoundSampSzOptionNot()))
	if (kMyErrNoMatch == (err = TryAsNumDrivesOptionNot()))
	if (kMyErrNoMatch == (err = TryAsSonySupportTagsNot()))
	if (kMyErrNoMatch == (err = TryAsSonyWantChecksumsUpdatedNot()))
	if (kMyErrNoMatch == (err = TryAsSonySupportDC42Not()))
	if (kMyErrNoMatch == (err = TryAsInsertIthDisk()))
	if (kMyErrNoMatch == (err = TryAsCmndOptSwapNot()))
	if (kMyErrNoMatch == (err = TryAsAltKeysModeNot()))
	if (kMyErrNoMatch == (err = TryAsItnlKyBdFixNot()))
	if (kMyErrNoMatch == (err = TryAsLocalTalkNot()))
	if (kMyErrNoMatch == (err = TryAsInitSpeedOptionNot()))
	if (kMyErrNoMatch == (err = TryAsInitBackgroundNot()))
	if (kMyErrNoMatch == (err = TryAsInitAutoSlowNot()))
	if (kMyErrNoMatch == (err = TryAsTimingAccuracyOptionNot()))
	if (kMyErrNoMatch == (err = TryAsEmCpuVersOptionNot()))
	if (kMyErrNoMatch == (err = TryAsMemSizOptionNot()))
	if (kMyErrNoMatch == (err = TryAsCaretBlinkTimeOptionNot()))
	if (kMyErrNoMatch == (err = TryAsDoubleClickTimeOptionNot()))
	if (kMyErrNoMatch == (err = TryAsMenuBlinkOptionNot()))
	if (kMyErrNoMatch == (err = TryAsAutoKeyThreshOptionNot()))
	if (kMyErrNoMatch == (err = TryAsAutoKeyRateOptionNot()))
	if (kMyErrNoMatch == (err = TryAsSpeakerVolOptionNot()))
	if (kMyErrNoMatch == (err = TryAsWantMinExtnNot()))
	if (kMyErrNoMatch == (err = TryAsMouseMotionOptionNot()))
	if (kMyErrNoMatch == (err = TryAsGrabKeysFSNot()))
	if (kMyErrNoMatch == (err = TryAsEnblCtrlIntNot()))
	if (kMyErrNoMatch == (err = TryAsEnblCtrlRstNot()))
	if (kMyErrNoMatch == (err = TryAsEnblCtrlKtgNot()))
	if (kMyErrNoMatch == (err = TryAsWantColorImageNot()))
	if (kMyErrNoMatch == (err = TryAsAltHappyMacOptionNot()))
	if (kMyErrNoMatch == (err = TryAsRomSizeOptionNot()))
	if (kMyErrNoMatch == (err = TryAsCheckRomCheckSumNot()))
	if (kMyErrNoMatch == (err = TryAsDisableRomCheckNot()))
	if (kMyErrNoMatch == (err = TryAsDisableRamTestNot()))
	if (kMyErrNoMatch == (err = TryAsWantDisasmNot()))
	if (kMyErrNoMatch == (err = TryAsDbgLogHAVENot()))
	if (kMyErrNoMatch == (err = TryAsScreenVSyncNot()))
	if (kMyErrNoMatch == (err = TryAsNeedIntlNot()))
	if (kMyErrNoMatch == (err = TryAsDemoMsgNot()))
	if (kMyErrNoMatch == (err = TryAsActvCodeNot()))
	{
	}

	return err;
}

LOCALFUNC tMyErr AutoChooseSPSettings(void)
{
	tMyErr err;

	if (noErr == (err = ChooseModel()))
	if (noErr == (err = ChooseHRes()))
	if (noErr == (err = ChooseVRes()))
	if (noErr == (err = ChooseScrnDpth()))
	if (noErr == (err = ChooseInitFullScreen()))
	if (noErr == (err = ChooseVarFullScreen()))
	if (noErr == (err = ChooseMagFctr()))
	if (noErr == (err = ChooseInitMagnify()))
	if (noErr == (err = ChooseSoundEnabled()))
	if (noErr == (err = ChooseSndApiOption()))
	if (noErr == (err = ChooseSoundSampSz()))
	if (noErr == (err = ChooseNumDrives()))
	if (noErr == (err = ChooseSonySupportTags()))
	if (noErr == (err = ChooseSonyWantChecksumsUpdated()))
	if (noErr == (err = ChooseSonySupportDC42()))
	if (noErr == (err = ChooseInsertIthDisk()))
	if (noErr == (err = ChooseCmndOptSwap()))
	if (noErr == (err = ChooseAltKeysMode()))
	if (noErr == (err = ChooseItnlKyBdFix()))
	if (noErr == (err = ChooseLocalTalk()))
	if (noErr == (err = ChooseInitSpeed()))
	if (noErr == (err = ChooseInitBackground()))
	if (noErr == (err = ChooseInitAutoSlow()))
	if (noErr == (err = ChooseTimingAccuracy()))
	if (noErr == (err = ChooseEmCpuVers()))
	if (noErr == (err = ChooseMemSiz()))
	if (noErr == (err = ChooseMemBankSizes())) /* derived */
	if (noErr == (err = ChooseCaretBlinkTime()))
	if (noErr == (err = ChooseDoubleClickTime()))
	if (noErr == (err = ChooseMenuBlink()))
	if (noErr == (err = ChooseAutoKeyThresh()))
	if (noErr == (err = ChooseAutoKeyRate()))
	if (noErr == (err = ChooseSpeakerVol()))
	if (noErr == (err = ChooseWantMinExtn()))
	if (noErr == (err = ChooseMouseMotion()))
	if (noErr == (err = ChooseGrabKeysFS()))
	if (noErr == (err = ChooseEnblCtrlInt()))
	if (noErr == (err = ChooseEnblCtrlRst()))
	if (noErr == (err = ChooseEnblCtrlKtg()))
	if (noErr == (err = ChooseWantColorImage()))
	if (noErr == (err = ChooseAltHappyMac()))
	if (noErr == (err = ChooseRomSize()))
	if (noErr == (err = ChooseCheckRomCheckSum()))
	if (noErr == (err = ChooseDisableRomCheck()))
	if (noErr == (err = ChooseDisableRamTest()))

	if (noErr == (err = ChooseWantDisasm()))
	if (noErr == (err = ChooseDbgLogHAVE()))
	if (noErr == (err = ChooseScreenVSync()))

	if (noErr == (err = ChooseNeedIntl()))
	if (noErr == (err = ChooseDemoMsg()))
	if (noErr == (err = ChooseActvCode()))
	if (noErr == (err = ChooseScreenOpts())) /* derived */
	if (noErr == (err = ChooseVidMemSize())) /* derived */
	if (noErr == (err = ChooseMiscEmHardware())) /* derived */
	if (noErr == (err = ChooseTotMemSize())) /* derived */
	{
		err = noErr;
	}

	return err;
}

LOCALPROC WrtOptSPSettings(void)
{
	WrtOptModelOption();
	WrtOptHResOption();
	WrtOptVResOption();
	WrtOptScrnDpth();
	WrtOptInitFullScreen();
	WrtOptVarFullScreen();
	WrtOptMagFctrOption();
	WrtOptInitMagnify();
	WrtOptSoundOption();
	WrtOptSndApiOption();
	WrtOptSoundSampSzOption();
	WrtOptNumDrivesOption();
	WrtOptSonySupportTags();
	WrtOptSonyWantChecksumsUpdated();
	WrtOptSonySupportDC42();
	WrtOptInsertIthDisk();
	WrtOptCmndOptSwap();
	WrtOptAltKeysMode();
	WrtOptItnlKyBdFix();
	WrtOptLocalTalk();
	WrtOptInitSpeedOption();
	WrtOptInitBackground();
	WrtOptInitAutoSlow();
	WrtOptTimingAccuracy();
	WrtOptEmCpuVers();
	WrtOptMemSizOption();
	WrtOptCaretBlinkTime();
	WrtOptDoubleClickTime();
	WrtOptMenuBlink();
	WrtOptAutoKeyThresh();
	WrtOptAutoKeyRate();
	WrtOptSpeakerVol();
	WrtOptMinExtn();
	WrtOptMouseMotion();
	WrtOptGrabKeysFS();
	WrtOptEnblCtrlInt();
	WrtOptEnblCtrlRst();
	WrtOptEnblCtrlKtg();
	WrtOptColorImage();
	WrtOptAltHappyMac();
	WrtOptRomSize();
	WrtOptCheckRomCheckSum();
	WrtOptDisableRomCheck();
	WrtOptDisableRamTest();
	WrtOptWantDisasmNot();
	WrtOptDbgLogHAVE();
	WrtOptScreenVSync();
	/* WantDemoMsg */
	/* WantActvCode */
}
