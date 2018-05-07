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
LOCALVAR ui3r olv_mdl;

LOCALPROC ResetModelOption(void)
{
	cur_mdl = kListOptionAuto;
	olv_mdl = 0;
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
	return FindNamedOption("-m", kNumModels, GetModelName,
		&cur_mdl, &olv_mdl);
}

#define dfo_mdl() gbk_mdl_Plus

LOCALVAR blnr cur_mIIorIIX;

LOCALFUNC tMyErr ChooseModel(void)
{
	if (kListOptionAuto == cur_mdl) {
		cur_mdl = dfo_mdl();
	}

	cur_mIIorIIX = (gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl);

#if 0
	if (cur_mIIorIIX) {
		if (gbk_cpufam_68k == gbo_cpufam) {
			err = ReportParseFailure(
				"Mac II emulation is not supported on Macintosh 680x0");
		}
	}
#endif

	return noErr;
}

LOCALPROC WrtOptModelOption(void)
{
	WrtOptNamedOption("-m", GetModelName, cur_mdl, dfo_mdl());
}


/* option: horizontal resolution */

LOCALVAR uimr cur_hres;
LOCALVAR ui3r olv_hres;

LOCALPROC ResetHResOption(void)
{
	olv_hres = 0;
}

LOCALFUNC tMyErr TryAsHResOptionNot(void)
{
	return NumberTryAsOptionNot("-hres",
		(long *)&cur_hres, &olv_hres);
}

LOCALFUNC uimr dfo_hres(void)
{
	uimr v;

	if (cur_mIIorIIX) {
		v = 640;
	} else if (gbk_mdl_PB100 == cur_mdl) {
		v = 640;
	} else {
		v = 512;
	}

	return v;
}

LOCALFUNC tMyErr ChooseHRes(void)
{
	tMyErr err;
	MyPStr t;
	MyPStr s;

	if (0 == olv_hres) {
		cur_hres = dfo_hres();
		err = noErr;
	} else {
		if ((cur_hres & 0x1F) != 0) {
			PStrFromCStr(t, "-hres must be a multiple of 32."
				" The next lowest multiple is ");
			PStrFromUimr(cur_hres & ~ 0x1F, s);
			PStrAppend(t, s);
			err = ReportParseFailPStr(t);
		} else if (cur_hres < 128) {
			err = ReportParseFailure("-hres must be >= 128");
		} else if (cur_hres >= (uimr)32 * 1024) {
			err = ReportParseFailure("-hres must be < 32k");
		} else {
			err = noErr;
		}
	}

	return err;
}

LOCALPROC WrtOptHResOption(void)
{
	WrtOptNumberOption("-hres", cur_hres, dfo_hres());
}


/* option: vertical resolution */

LOCALVAR uimr cur_vres;
LOCALVAR ui3r olv_vres;

LOCALPROC ResetVResOption(void)
{
	olv_vres = 0;
}

LOCALFUNC tMyErr TryAsVResOptionNot(void)
{
	return NumberTryAsOptionNot("-vres", (long *)&cur_vres, &olv_vres);
}

LOCALFUNC uimr dfo_vres(void)
{
	uimr v;

	if (cur_mIIorIIX) {
		v = 480;
	} else if (gbk_mdl_PB100 == cur_mdl) {
		v = 400;
	} else {
		v = 342;
	}

	return v;
}

LOCALFUNC tMyErr ChooseVRes(void)
{
	tMyErr err;

	if (0 == olv_vres) {
		cur_vres = dfo_vres();
		err = noErr;
	} else {
		if (cur_vres < 128) {
			err = ReportParseFailure("-vres must be >= 128");
		} else if (cur_vres >= (uimr)32 * 1024) {
			err = ReportParseFailure("-vres must be < 32k");
		} else {
			err = noErr;
		}
	}

	return err;
}

LOCALPROC WrtOptVResOption(void)
{
	WrtOptNumberOption("-vres", cur_vres, dfo_vres());
}


/* option: screen depth */

LOCALVAR uimr cur_ScrnDpth;
LOCALVAR ui3r olv_ScrnDpth;

LOCALPROC ResetScrnDpthOption(void)
{
	olv_ScrnDpth = 0;
}

LOCALFUNC tMyErr TryAsScrnDpthOptionNot(void)
{
	return NumberTryAsOptionNot("-depth",
		(long *)&cur_ScrnDpth, &olv_ScrnDpth);
}

LOCALFUNC uimr dfo_ScrnDpth(void)
{
	uimr v;

	if (cur_mIIorIIX) {
		v = 3;
	} else {
		v = 0;
	}

	return v;
}

LOCALFUNC tMyErr ChooseScrnDpth(void)
{
	tMyErr err;

	err = noErr;

	if (0 == olv_ScrnDpth) {
		cur_ScrnDpth = dfo_ScrnDpth();
	} else {
		if (cur_mIIorIIX) {
			if (cur_ScrnDpth > 5) {
				err = ReportParseFailure("-depth must be <= 5");
			}
		} else {
			if (cur_ScrnDpth != 0) {
				err = ReportParseFailure(
					"-depth must be 0 for this model");
			}
		}
	}

	return err;
}

LOCALPROC WrtOptScrnDpth(void)
{
	WrtOptNumberOption("-depth", cur_ScrnDpth, dfo_ScrnDpth());
}


/* option: Initial FullScreen */

LOCALVAR blnr WantInitFullScreen;
LOCALVAR ui3r olv_InitFullScreen;

LOCALPROC ResetInitFullScreen(void)
{
	WantInitFullScreen = nanblnr;
	olv_InitFullScreen = 0;
}

LOCALFUNC tMyErr TryAsInitFullScreenNot(void)
{
	return BooleanTryAsOptionNot("-fullscreen",
		&WantInitFullScreen, &olv_InitFullScreen);
}

LOCALFUNC blnr dfo_InitFullScreen(void)
{
	int v;

	v = gbk_targfam_wnce == gbo_targfam;

	return v;
}

LOCALFUNC tMyErr ChooseInitFullScreen(void)
{
	tMyErr err;

	err = noErr;

	if (nanblnr == WantInitFullScreen) {
		WantInitFullScreen = dfo_InitFullScreen();
	} else {
		if (WantInitFullScreen) {
			if (gbk_targ_wcar == cur_targ) {
				err = ReportParseFailure(
					"-fullscreen 1 is not supported for -t wcar");
			}
		}
	}

	return err;
}

LOCALPROC WrtOptInitFullScreen(void)
{
	WrtOptBooleanOption("-fullscreen",
		WantInitFullScreen, dfo_InitFullScreen());
}


/* option: Variable FullScreen */

LOCALVAR blnr WantVarFullScreen;
LOCALVAR ui3r olv_VarFullScreen;

LOCALPROC ResetVarFullScreen(void)
{
	WantVarFullScreen = nanblnr;
	olv_VarFullScreen = 0;
}

LOCALFUNC tMyErr TryAsVarFullScreenNot(void)
{
	return BooleanTryAsOptionNot("-var-fullscreen",
		&WantVarFullScreen, &olv_VarFullScreen);
}

LOCALFUNC blnr dfo_VarFullScreen(void)
{
	int v;

	if ((gbk_apifam_gtk == gbo_apifam)
		|| (gbk_targfam_wnce == gbo_targfam))
	{
		v = falseblnr;
	} else {
		v = trueblnr;
	}

	return v;
}

LOCALFUNC tMyErr ChooseVarFullScreen(void)
{
	tMyErr err;

	err = noErr;

	if (nanblnr == WantVarFullScreen) {
		WantVarFullScreen = dfo_VarFullScreen();
	} else {
		if (WantVarFullScreen) {
			if (gbk_targ_wcar == cur_targ) {
				err = ReportParseFailure(
					"-var-fullscreen is not supported for -t wcar");
			}
		}
	}

	return err;
}

LOCALPROC WrtOptVarFullScreen(void)
{
	WrtOptBooleanOption("-var-fullscreen",
		WantVarFullScreen, dfo_VarFullScreen());
}


/* option: magnification factor */

LOCALVAR uimr cur_MagFctr;
LOCALVAR ui3r olv_MagFctr;

LOCALPROC ResetMagFctrOption(void)
{
	olv_MagFctr = 0;
}

LOCALFUNC tMyErr TryAsMagFctrOptionNot(void)
{
	return NumberTryAsOptionNot("-mf",
		(long *)&cur_MagFctr, &olv_MagFctr);
}

LOCALFUNC uimr dfo_MagFctr(void)
{
	uimr v;

	if (gbk_apifam_gtk == gbo_apifam) {
		/* temporary, until implemented */
		v = 1;
	} else {
		v = 2;
	}

	return v;
}

LOCALFUNC tMyErr ChooseMagFctr(void)
{
	tMyErr err;

	err = noErr;

	if (0 == olv_MagFctr) {
		cur_MagFctr = dfo_MagFctr();
	} else {
		if (cur_MagFctr < 1) {
			err = ReportParseFailure("-mf must be >= 1");
		}
	}

	return err;
}

LOCALPROC WrtOptMagFctrOption(void)
{
	WrtOptNumberOption("-mf", cur_MagFctr, dfo_MagFctr());
}


/* option: Initial Magnify */

LOCALVAR blnr WantInitMagnify;
LOCALVAR ui3r olv_InitMagnify;

LOCALPROC ResetInitMagnify(void)
{
	WantInitMagnify = nanblnr;
	olv_InitMagnify = 0;
}

LOCALFUNC tMyErr TryAsInitMagnifyNot(void)
{
	return BooleanTryAsOptionNot("-magnify",
		&WantInitMagnify, &olv_InitMagnify);
}

#define dfo_InitMagnify() falseblnr

LOCALFUNC tMyErr ChooseInitMagnify(void)
{
	tMyErr err;

	err = noErr;
	if (nanblnr == WantInitMagnify) {
		WantInitMagnify = dfo_InitMagnify();
	} else {
		if (WantInitMagnify && (cur_MagFctr == 1)) {
			err = ReportParseFailure(
				"-magnify 1 does not make sense with -mf 1");
		}
	}

	return err;
}

LOCALPROC WrtOptInitMagnify(void)
{
	WrtOptBooleanOption("-magnify",
		WantInitMagnify, dfo_InitMagnify());
}


/* option: sound */

LOCALVAR blnr MySoundEnabled;
LOCALVAR ui3r olv_SoundEnabled;

LOCALPROC ResetSoundOption(void)
{
	MySoundEnabled = nanblnr;
	olv_SoundEnabled = 0;
}

LOCALFUNC tMyErr TryAsSoundOptionNot(void)
{
	return BooleanTryAsOptionNot("-sound",
		&MySoundEnabled, &olv_SoundEnabled);
}

LOCALFUNC blnr dfo_SoundEnabled(void)
{
	int v;

	v = (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_sdl == gbo_apifam)
		|| (gbk_apifam_sd2 == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
		|| ((gbk_apifam_xwn == gbo_apifam)
			&& ((gbo_targfam == gbk_targfam_linx)
				|| (gbo_targfam == gbk_targfam_fbsd)
				|| (gbo_targfam == gbk_targfam_nbsd)));

	return v;
}

LOCALFUNC tMyErr ChooseSoundEnabled(void)
{
	if (nanblnr == MySoundEnabled) {
		MySoundEnabled = dfo_SoundEnabled();
	}

	return noErr;
}

LOCALPROC WrtOptSoundOption(void)
{
	WrtOptBooleanOption("-sound", MySoundEnabled, dfo_SoundEnabled());
}


/* option: sound api */

enum {
	gbk_sndapi_none,
	gbk_sndapi_alsa,
	gbk_sndapi_ddsp,
	kNumSndApiLevels
};

LOCALVAR int gbo_sndapi;
LOCALVAR ui3r olv_sndapi;

LOCALPROC ResetSndApiOption(void)
{
	gbo_sndapi = kListOptionAuto;
	olv_sndapi = 0;
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
		kNumSndApiLevels, GetSndApiName, &gbo_sndapi, &olv_sndapi);
}

LOCALFUNC int dfo_sndapi(void)
{
	int v;

	if (! MySoundEnabled) {
		v = gbk_sndapi_none;
	} else if (gbk_apifam_xwn != gbo_apifam) {
		v = gbk_sndapi_none;
	} else if (gbo_targfam == gbk_targfam_linx) {
		v = gbk_sndapi_alsa;
	} else {
		v = gbk_sndapi_ddsp;
	}

	return v;
}

LOCALFUNC tMyErr ChooseSndApiOption(void)
{
	if (kListOptionAuto == gbo_sndapi) {
		gbo_sndapi = dfo_sndapi();
	}

	return noErr;
}

LOCALPROC WrtOptSndApiOption(void)
{
	WrtOptNamedOption("-snd-api", GetSndApiName,
		gbo_sndapi, dfo_sndapi());
}


/* option: sound sample size */

LOCALVAR uimr cur_SoundSampSz;
LOCALVAR ui3r olv_SoundSampSz;

LOCALPROC ResetSoundSampSzOption(void)
{
	olv_SoundSampSz = 0;
}

LOCALFUNC tMyErr TryAsSoundSampSzOptionNot(void)
{
	return NumberTryAsOptionNot("-sss",
		(long *)&cur_SoundSampSz, &olv_SoundSampSz);
}

LOCALFUNC uimr dfo_SoundSampSz(void)
{
	uimr v;

	if (gbk_sndapi_ddsp == gbo_sndapi) {
		v = 4;
	} else {
		v = 3;
	}

	return v;
}

LOCALFUNC tMyErr ChooseSoundSampSz(void)
{
	tMyErr err;

	err = noErr;


	if (0 == olv_SoundSampSz) {
		cur_SoundSampSz = dfo_SoundSampSz();
	} else {
		if ((cur_SoundSampSz < 3) || (cur_SoundSampSz > 4)) {
			err = ReportParseFailure(
				"-sss must be 3 or 4");
		}
	}

	return err;
}

LOCALPROC WrtOptSoundSampSzOption(void)
{
	WrtOptNumberOption("-sss", cur_SoundSampSz, dfo_SoundSampSz());
}


/* option: number of drives */

LOCALVAR uimr cur_numdrives;
LOCALVAR ui3r olv_numdrives;

LOCALPROC ResetNumDrivesOption(void)
{
	olv_numdrives = 0;
}

LOCALFUNC tMyErr TryAsNumDrivesOptionNot(void)
{
	return NumberTryAsOptionNot("-drives",
		(long *)&cur_numdrives, &olv_numdrives);
}

LOCALFUNC uimr dfo_numdrives(void)
{
	uimr v;

	if (cur_mdl < gbk_mdl_512Ke) {
		v = 2;
	} else {
		v = 6;
	}

	return v;
}

LOCALFUNC tMyErr ChooseNumDrives(void)
{
	tMyErr err;

	err = noErr;


	if (0 == olv_numdrives) {
		cur_numdrives = dfo_numdrives();
	} else {
		if ((cur_numdrives <= 0) || (cur_numdrives > 32)) {
			err = ReportParseFailure(
				"-drives must be a number between 1 and 32");
		}
	}

	return err;
}

LOCALPROC WrtOptNumDrivesOption(void)
{
	WrtOptNumberOption("-drives", cur_numdrives, dfo_numdrives());
}


/* option: disk driver - support tags */

LOCALVAR blnr SonySupportTags;
LOCALVAR ui3r olv_SonySupportTags;

LOCALPROC ResetSonySupportTags(void)
{
	SonySupportTags = nanblnr;
	olv_SonySupportTags = 0;
}

LOCALFUNC tMyErr TryAsSonySupportTagsNot(void)
{
	return BooleanTryAsOptionNot("-sony-tag",
		&SonySupportTags, &olv_SonySupportTags);
}

#define dfo_SonySupportTags() falseblnr

LOCALFUNC tMyErr ChooseSonySupportTags(void)
{
	if (nanblnr == SonySupportTags) {
		SonySupportTags = dfo_SonySupportTags();
	}

	return noErr;
}

LOCALPROC WrtOptSonySupportTags(void)
{
	WrtOptBooleanOption("-sony-tag",
		SonySupportTags, dfo_SonySupportTags());
}


/* option: disk driver - calculate checksums */

LOCALVAR blnr SonyWantChecksumsUpdated;
LOCALVAR ui3r olv_SonyWantChecksumsUpdated;

LOCALPROC ResetSonyWantChecksumsUpdated(void)
{
	SonyWantChecksumsUpdated = nanblnr;
	olv_SonyWantChecksumsUpdated = 0;
}

LOCALFUNC tMyErr TryAsSonyWantChecksumsUpdatedNot(void)
{
	return BooleanTryAsOptionNot("-sony-sum",
		&SonyWantChecksumsUpdated, &olv_SonyWantChecksumsUpdated);
}

#define dfo_SonyWantChecksumsUpdated() falseblnr

LOCALFUNC tMyErr ChooseSonyWantChecksumsUpdated(void)
{
	if (nanblnr == SonyWantChecksumsUpdated) {
		SonyWantChecksumsUpdated = dfo_SonyWantChecksumsUpdated();
	}

	return noErr;
}

LOCALPROC WrtOptSonyWantChecksumsUpdated(void)
{
	WrtOptBooleanOption("-sony-sum",
		SonyWantChecksumsUpdated, dfo_SonyWantChecksumsUpdated());
}


/* option: disk driver - support disk copy 4.2 format */

LOCALVAR blnr SonySupportDC42;
LOCALVAR ui3r olv_SonySupportDC42;

LOCALPROC ResetSonySupportDC42(void)
{
	SonySupportDC42 = nanblnr;
	olv_SonySupportDC42 = 0;
}

LOCALFUNC tMyErr TryAsSonySupportDC42Not(void)
{
	return BooleanTryAsOptionNot("-sony-dc42",
		&SonySupportDC42, &olv_SonySupportDC42);
}

#define dfo_SonySupportDC42() trueblnr

LOCALFUNC tMyErr ChooseSonySupportDC42(void)
{
	if (nanblnr == SonySupportDC42) {
		SonySupportDC42 = dfo_SonySupportDC42();
	}

	return noErr;
}

LOCALPROC WrtOptSonySupportDC42(void)
{
	WrtOptBooleanOption("-sony-dc42",
		SonySupportDC42, dfo_SonySupportDC42());
}


/* option: Insert Ith Disk Image */

LOCALVAR blnr WantInsertIthDisk;
LOCALVAR ui3r olv_InsertIthDisk;

LOCALPROC ResetInsertIthDisk(void)
{
	WantInsertIthDisk = nanblnr;
	olv_InsertIthDisk = 0;
}

LOCALFUNC tMyErr TryAsInsertIthDisk(void)
{
	return BooleanTryAsOptionNot("-iid",
		&WantInsertIthDisk, &olv_InsertIthDisk);
}

#define dfo_InsertIthDisk() falseblnr

LOCALFUNC tMyErr ChooseInsertIthDisk(void)
{
	if (nanblnr == WantInsertIthDisk) {
		WantInsertIthDisk = dfo_InsertIthDisk();
	}

	return noErr;
}

LOCALPROC WrtOptInsertIthDisk(void)
{
	WrtOptBooleanOption("-iid",
		WantInsertIthDisk, dfo_InsertIthDisk());
}


/* option: Command Option Swap */

LOCALVAR blnr WantCmndOptSwap;
LOCALVAR ui3r olv_CmndOptSwap;

LOCALPROC ResetCmndOptSwap(void)
{
	WantCmndOptSwap = falseblnr;
	olv_CmndOptSwap = 0;
}

LOCALFUNC tMyErr TryAsCmndOptSwapNot(void)
{
	return FlagTryAsOptionNot("-ccs",
		&WantCmndOptSwap, &olv_CmndOptSwap);
}

LOCALFUNC tMyErr ChooseCmndOptSwap(void)
{
	return noErr;
}

LOCALPROC WrtOptCmndOptSwap(void)
{
	WrtOptFlagOption("-ccs", WantCmndOptSwap);
}


/* option: key mapping */

enum {
	gbk_keynam_Control,
	gbk_keynam_Command,
	gbk_keynam_Option,
	gbk_keynam_Shift,
	gbk_keynam_CapsLock,
	gbk_keynam_F1,
	gbk_keynam_F2,
	gbk_keynam_F3,
	gbk_keynam_F4,
	gbk_keynam_F5,
	gbk_keynam_Escape,
	gbk_keynam_BackSlash,
	gbk_keynam_Slash,
	gbk_keynam_Grave,
	gbk_keynam_Enter,
	gbk_keynam_PageUp,
	gbk_keynam_PageDown,
	gbk_keynam_Home,
	gbk_keynam_End,
	gbk_keynam_Help,
	gbk_keynam_ForwardDel,
	kNumKeyNames
};

enum {
	gbk_keynam_RControl = kNumKeyNames,
	gbk_keynam_RCommand,
	gbk_keynam_ROption,
	gbk_keynam_RShift,
	kNumSrcKeyNames
};

enum {
	gbk_keynam_CM = kNumKeyNames,
	kNumDstKeyNames
};

LOCALVAR ui3b gbo_keymap[kNumSrcKeyNames];
LOCALVAR ui3r olv_keymap[kNumSrcKeyNames];

LOCALPROC ResetKeyMapOption(void)
{
	uimr i;

	for (i = 0; i < kNumSrcKeyNames; ++i) {
		gbo_keymap[i] = 0xFF;
		olv_keymap[i] = 0;
	}
}

LOCALFUNC char * GetKeyMapName(int i)
{
	char *s;

	switch (i) {
		case gbk_keynam_Control:
			s = "Control";
			break;
		case gbk_keynam_Command:
			s = "Command";
			break;
		case gbk_keynam_Option:
			s = "Option";
			break;
		case gbk_keynam_Shift:
			s = "Shift";
			break;
		case gbk_keynam_CapsLock:
			s = "CapsLock";
			break;
		case gbk_keynam_F1:
			s = "F1";
			break;
		case gbk_keynam_F2:
			s = "F2";
			break;
		case gbk_keynam_F3:
			s = "F3";
			break;
		case gbk_keynam_F4:
			s = "F4";
			break;
		case gbk_keynam_F5:
			s = "F5";
			break;
		case gbk_keynam_Escape:
			s = "Escape";
			break;
		case gbk_keynam_BackSlash:
			s = "BackSlash";
			break;
		case gbk_keynam_Slash:
			s = "Slash";
			break;
		case gbk_keynam_Grave:
			s = "Grave";
			break;
		case gbk_keynam_Enter:
			s = "Enter";
			break;
		case gbk_keynam_PageUp:
			s = "PageUp";
			break;
		case gbk_keynam_PageDown:
			s = "PageDown";
			break;
		case gbk_keynam_Home:
			s = "Home";
			break;
		case gbk_keynam_End:
			s = "End";
			break;
		case gbk_keynam_Help:
			s = "Help";
			break;
		case gbk_keynam_ForwardDel:
			s = "ForwardDel";
			break;
		default:
			s = "(unknown key)";
			break;
	}

	return s;
}

LOCALFUNC char * GetSrcKeyMapName(int i)
{
	char *s;

	switch (i) {
		case gbk_keynam_RControl:
			s = "RControl";
			break;
		case gbk_keynam_RCommand:
			s = "RCommand";
			break;
		case gbk_keynam_ROption:
			s = "ROption";
			break;
		case gbk_keynam_RShift:
			s = "RShift";
			break;
		default:
			s = GetKeyMapName(i);
			break;
	}

	return s;
}

LOCALFUNC char * GetDstKeyMapName(int i)
{
	char *s;

	switch (i) {
		case gbk_keynam_CM:
			s = "CM";
			break;
		default:
			s = GetKeyMapName(i);
			break;
	}

	return s;
}

LOCALFUNC tMyErr TryAsKeyMapOptionNot(void)
{
	tMyErr err;
	MyPStr t;
	int k;
	int j;

	if (! CurArgIsCStr_v2("-km")) {
		err = kMyErrNoMatch;
	} else
	if (kMyErr_noErr != (err = AdvanceTheArg())) {
		/* fail */
	} else
	if (The_arg_end) {
		PStrFromCStr(t,
			"Expecting a src argument for -km when reached end");
		err = ReportParseFailPStr(t);
	} else
	if (! GetCurArgNameIndex(kNumSrcKeyNames, GetSrcKeyMapName, &k)) {
		PStrFromCStr(t, "Unknown source value for -km");
		err = ReportParseFailPStr(t);
	} else
	if (olv_keymap[k] == olv_cur) {
		PStrFromCStr(t,
			"same -km src value has appeared more than once");
		err = ReportParseFailPStr(t);
	} else
	if (kMyErr_noErr != (err = AdvanceTheArg())) {
		/* fail */
	} else
	if (The_arg_end) {
		PStrFromCStr(t,
			"Expecting a dst argument for -km when reached end");
		err = ReportParseFailPStr(t);
	} else
	if (! GetCurArgNameIndex(kNumDstKeyNames, GetDstKeyMapName, &j)) {
		if (CurArgIsCStr_v2("*")) {
			olv_keymap[k] = olv_cur;
			gbo_keymap[k] = 0xFF;
			err = AdvanceTheArg();
		} else
		{
			PStrFromCStr(t, "Unknown dst value for -km");
			err = ReportParseFailPStr(t);
		}
	} else

	{
		olv_keymap[k] = olv_cur;
		gbo_keymap[k] = j;
		err = AdvanceTheArg();
	}

	return err;
}

LOCALPROC dfo_keyset(ui3b *a, uimr i, ui3r v)
{
	a[i] = v;

	if (0xFF == gbo_keymap[i]) {
		gbo_keymap[i] = v;
	}
}

LOCALPROC dfo_keymap(ui3b *a)
{
	uimr i;

	dfo_keyset(a, gbk_keynam_Control,
		WantCmndOptSwap ? gbk_keynam_Command : gbk_keynam_CM);
	dfo_keyset(a, gbk_keynam_Command,
		WantCmndOptSwap ? gbk_keynam_CM : gbk_keynam_Command);

	for (i = gbk_keynam_Option; i <= gbk_keynam_CapsLock; ++i) {
		dfo_keyset(a, i, i);
	}

	dfo_keyset(a, gbk_keynam_F1, gbk_keynam_Option);
	dfo_keyset(a, gbk_keynam_F2, gbk_keynam_Command);

	for (i = gbk_keynam_F3; i <= gbk_keynam_ForwardDel; ++i) {
		dfo_keyset(a, i, i);
	}

	dfo_keyset(a, gbk_keynam_RControl, gbo_keymap[gbk_keynam_Control]);
	dfo_keyset(a, gbk_keynam_RCommand, gbo_keymap[gbk_keynam_Command]);
	dfo_keyset(a, gbk_keynam_ROption, gbo_keymap[gbk_keynam_Option]);
	dfo_keyset(a, gbk_keynam_RShift, gbo_keymap[gbk_keynam_Shift]);
}

LOCALFUNC tMyErr ChooseKeyMap(void)
{
	ui3b a[kNumSrcKeyNames];

	dfo_keymap(a);

	return noErr;
}

LOCALPROC WrtOptKeyMap(void)
{
	ui3b a[kNumSrcKeyNames];
	uimr i;

	dfo_keymap(a);

	for (i = 0; i < kNumSrcKeyNames; ++i) {
		if (gbo_keymap[i] != a[i]) {
			strmo_writeCStr(" -km ");
			strmo_writeCStr(GetSrcKeyMapName(i));
			strmo_writeCStr(" ");
			strmo_writeCStr(GetDstKeyMapName(gbo_keymap[i]));
		}
	}
}


/* option: Alternate Keyboard Mode */

LOCALVAR blnr WantAltKeysMode;
LOCALVAR ui3r olv_WantAltKeysMode;

LOCALPROC ResetAltKeysMode(void)
{
	WantAltKeysMode = falseblnr;
	olv_WantAltKeysMode = 0;
}

LOCALFUNC tMyErr TryAsAltKeysModeNot(void)
{
	return FlagTryAsOptionNot("-akm",
		&WantAltKeysMode, &olv_WantAltKeysMode);
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
LOCALVAR ui3r olv_ItnlKyBdFix;

LOCALPROC ResetItnlKyBdFixOption(void)
{
	ItnlKyBdFix = nanblnr;
}

LOCALFUNC tMyErr TryAsItnlKyBdFixNot(void)
{
	return BooleanTryAsOptionNot("-ikb",
		&ItnlKyBdFix, &olv_ItnlKyBdFix);
}

LOCALFUNC blnr dfo_ItnlKyBdFix(void)
{
	int v;

	v = (gbk_apifam_win == gbo_apifam);

	return v;
}

LOCALFUNC tMyErr ChooseItnlKyBdFix(void)
{
	tMyErr err = noErr;

	if (nanblnr == ItnlKyBdFix) {
		ItnlKyBdFix = dfo_ItnlKyBdFix();
	} else {
		if (ItnlKyBdFix) {
			if (gbk_apifam_win != gbo_apifam) {
				err = ReportParseFailure("-ikb is only for Windows");
			}
		}
	}

	return err;
}

LOCALPROC WrtOptItnlKyBdFix(void)
{
	WrtOptBooleanOption("-ikb", ItnlKyBdFix, dfo_ItnlKyBdFix());
}


/* option: LocalTalk emulation */

LOCALVAR blnr WantLocalTalk;
LOCALVAR ui3r olv_LocalTalk;

LOCALPROC ResetLocalTalk(void)
{
	WantLocalTalk = falseblnr;
	olv_LocalTalk = 0;
}

LOCALFUNC tMyErr TryAsLocalTalkNot(void)
{
	return FlagTryAsOptionNot("-lt", &WantLocalTalk, &olv_LocalTalk);
}

LOCALFUNC tMyErr ChooseLocalTalk(void)
{
	tMyErr err;

	err = noErr;

	if (WantLocalTalk) {
		if ((gbk_apifam_osx != gbo_apifam)
			&& (gbk_apifam_cco != gbo_apifam))
		{
			err = ReportParseFailure(
				"-lt is so far only implemented for OS X");
		}
	}

	return err;
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
LOCALVAR ui3r olv_InitSpeed;

LOCALPROC ResetInitSpeedOption(void)
{
	CurInitSpeed = kListOptionAuto;
	olv_InitSpeed = 0;
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
		kNumSpeeds, GetInitSpeedName, &CurInitSpeed, &olv_InitSpeed);
}

LOCALFUNC int dfo_InitSpeed(void)
{
	int v;

	if (cur_mIIorIIX) {
		v = gbk_speed_4X;
	} else {
		v = gbk_speed_8X;
	}

	return v;
}

LOCALFUNC tMyErr ChooseInitSpeed(void)
{
	if (kListOptionAuto == CurInitSpeed) {
		CurInitSpeed = dfo_InitSpeed();
	}

	return noErr;
}

LOCALPROC WrtOptInitSpeedOption(void)
{
	WrtOptNamedOption("-speed", GetInitSpeedName,
		CurInitSpeed, dfo_InitSpeed());
}


/* option: Initial Run In Background */

LOCALVAR blnr WantInitBackground;
LOCALVAR ui3r olv_InitBackground;

LOCALPROC ResetInitBackground(void)
{
	WantInitBackground = nanblnr;
	olv_InitBackground = 0;
}

LOCALFUNC tMyErr TryAsInitBackgroundNot(void)
{
	return BooleanTryAsOptionNot("-bg",
		&WantInitBackground, &olv_InitBackground);
}

LOCALFUNC blnr dfo_InitBackground(void)
{
	int v;

	if (WantLocalTalk) {
		v = trueblnr;
	} else {
		v = falseblnr;
	}

	return v;
}

LOCALFUNC tMyErr ChooseInitBackground(void)
{
	if (nanblnr == WantInitBackground) {
		WantInitBackground = dfo_InitBackground();
	}

	return noErr;
}

LOCALPROC WrtOptInitBackground(void)
{
	WrtOptBooleanOption("-bg", WantInitBackground,
		dfo_InitBackground());
}


/* option: Initial AutoSlow */

LOCALVAR blnr WantInitAutoSlow;
LOCALVAR ui3r olv_InitAutoSlow;

LOCALPROC ResetInitAutoSlow(void)
{
	WantInitAutoSlow = nanblnr;
	olv_InitAutoSlow = 0;
}

LOCALFUNC tMyErr TryAsInitAutoSlowNot(void)
{
	return BooleanTryAsOptionNot("-as",
		&WantInitAutoSlow, &olv_InitAutoSlow);
}

LOCALFUNC blnr dfo_InitAutoSlow(void)
{
	int v;

	v = ! cur_mIIorIIX;

	return v;
}

LOCALFUNC tMyErr ChooseInitAutoSlow(void)
{
	if (nanblnr == WantInitAutoSlow) {
		WantInitAutoSlow = dfo_InitAutoSlow();
	}

	return noErr;
}

LOCALPROC WrtOptInitAutoSlow(void)
{
	WrtOptBooleanOption("-as", WantInitAutoSlow, dfo_InitAutoSlow());
}


/* option: Timing Accuracy */

LOCALVAR uimr timingacc;
LOCALVAR ui3r olv_timingacc;

LOCALPROC ResetTimingAccuracyOption(void)
{
	olv_timingacc = 0;
}

LOCALFUNC tMyErr TryAsTimingAccuracyOptionNot(void)
{
	return NumberTryAsOptionNot("-ta",
		(long *)&timingacc, &olv_timingacc);
}

#define dfo_timingacc() 1

LOCALFUNC tMyErr ChooseTimingAccuracy(void)
{
	if (0 == olv_timingacc) {
		timingacc = dfo_timingacc();
	}

	return noErr;
}

LOCALPROC WrtOptTimingAccuracy(void)
{
	WrtOptNumberOption("-ta", timingacc, dfo_timingacc());
}


/* option: Emulated CPU version */

LOCALVAR uimr em_cpu_vers;
LOCALVAR ui3r olv_em_cpu_vers;

LOCALPROC ResetEmCpuVersOption(void)
{
	olv_em_cpu_vers = 0;
}

LOCALFUNC tMyErr TryAsEmCpuVersOptionNot(void)
{
	return NumberTryAsOptionNot("-em-cpu",
		(long *)&em_cpu_vers, &olv_em_cpu_vers);
}

LOCALFUNC uimr dfo_em_cpu_vers(void)
{
	uimr v;

	if (cur_mIIorIIX) {
		v = 2;
	} else {
		v = 0;
	}

	return v;
}

LOCALFUNC tMyErr ChooseEmCpuVers(void)
{
	if (0 == olv_em_cpu_vers) {
		em_cpu_vers = dfo_em_cpu_vers();
	}

	return noErr;
}

LOCALPROC WrtOptEmCpuVers(void)
{
	WrtOptNumberOption("-em-cpu", em_cpu_vers, dfo_em_cpu_vers());
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
LOCALVAR ui3r olv_msz;

LOCALPROC ResetMemSizOption(void)
{
	cur_msz = kListOptionAuto;
	olv_msz = 0;
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
		kNumMemSizs, GetMemSizName, &cur_msz, &olv_msz);
}

LOCALFUNC int dfo_msz(void)
{
	int v;

	switch (cur_mdl) {
		case gbk_mdl_Twig43:
		case gbk_mdl_Twiggy:
		case gbk_mdl_128K:
			v = gbk_msz_128K;
			break;
		case gbk_mdl_512Ke:
			v = gbk_msz_512K;
			break;
		case gbk_mdl_II:
		case gbk_mdl_IIx:
			v = gbk_msz_8M;
			break;
		case gbk_mdl_Plus:
		case gbk_mdl_SE:
		case gbk_mdl_SEFDHD:
		case gbk_mdl_Classic:
		case gbk_mdl_PB100:
		default:
			v = gbk_msz_4M;
			break;
	}
	if (gbk_targfam_lnds == gbo_targfam) {
		if (v > gbk_msz_2M) {
			v = gbk_msz_2M;
		}
	}

	return v;
}

LOCALFUNC tMyErr ChooseMemSiz(void)
{
	if (kListOptionAuto == cur_msz) {
		cur_msz = dfo_msz();
	} else {
		/* should error check here */
		/* no, checked in ChooseMemBankSizes */
	}

	return noErr;
}

LOCALPROC WrtOptMemSizOption(void)
{
	WrtOptNamedOption("-mem", GetMemSizName, cur_msz, dfo_msz());
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
		err = ReportParseFailure(
			"memory size (-mem) unsupported for this model (-m)");
	} else {
		err = noErr;
	}

	return err;
}


/* option: Parameter RAM CaretBlinkTime */
	/* usually in 3 (Fast), 8 (Medium), 15 (Slow) */

LOCALVAR uimr cur_CaretBlinkTime;
LOCALVAR ui3r olv_CaretBlinkTime;

LOCALPROC ResetCaretBlinkTimeOption(void)
{
	olv_CaretBlinkTime = 0;
}

LOCALFUNC tMyErr TryAsCaretBlinkTimeOptionNot(void)
{
	return NumberTryAsOptionNot("-cbt",
		(long *)&cur_CaretBlinkTime, &olv_CaretBlinkTime);
}

LOCALFUNC uimr dfo_CaretBlinkTime(void)
{
	uimr v;

	if (cur_mIIorIIX) {
		v = 8;
	} else {
		v = 3;
	}

	return v;
}

LOCALFUNC tMyErr ChooseCaretBlinkTime(void)
{
	tMyErr err;


	err = noErr;
	if (0 == olv_CaretBlinkTime) {
		cur_CaretBlinkTime = dfo_CaretBlinkTime();
	} else {
		if ((cur_CaretBlinkTime <= 0) || (cur_CaretBlinkTime > 15)) {
			err = ReportParseFailure(
				"-cbt must be a number between 1 and 15");
		}
	}

	return err;
}

LOCALPROC WrtOptCaretBlinkTime(void)
{
	WrtOptNumberOption("-cbt",
		cur_CaretBlinkTime, dfo_CaretBlinkTime());
}


/* option: Parameter RAM DoubleClickTime */
	/* usually in 5 (Fast), 8 (Medium), 12 (Slow) */

LOCALVAR uimr cur_DoubleClickTime;
LOCALVAR ui3r olv_DoubleClickTime;

LOCALPROC ResetDoubleClickTimeOption(void)
{
	olv_DoubleClickTime = 0;
}

LOCALFUNC tMyErr TryAsDoubleClickTimeOptionNot(void)
{
	return NumberTryAsOptionNot("-dct",
		(long *)&cur_DoubleClickTime, &olv_DoubleClickTime);
}

LOCALFUNC uimr dfo_DoubleClickTime(void)
{
	uimr v;

	if (cur_mIIorIIX) {
		v = 8;
	} else {
		v = 5;
	}

	return v;
}

LOCALFUNC tMyErr ChooseDoubleClickTime(void)
{
	tMyErr err;

	err = noErr;


	if (0 == olv_DoubleClickTime) {
		cur_DoubleClickTime = dfo_DoubleClickTime();
	} else {
		if ((cur_DoubleClickTime <= 0) || (cur_DoubleClickTime > 15)) {
			err = ReportParseFailure(
				"-dct must be a number between 1 and 15");
		}
	}

	return err;
}

LOCALPROC WrtOptDoubleClickTime(void)
{
	WrtOptNumberOption("-dct",
		cur_DoubleClickTime, dfo_DoubleClickTime());
}


/* option: Parameter RAM MenuBlink */
	/* in 0..3 */

LOCALVAR uimr cur_MenuBlink;
LOCALVAR ui3r olv_MenuBlink;

LOCALPROC ResetMenuBlinkOption(void)
{
	olv_MenuBlink = 0;
}

LOCALFUNC tMyErr TryAsMenuBlinkOptionNot(void)
{
	return NumberTryAsOptionNot("-mnb",
		(long *)&cur_MenuBlink, &olv_MenuBlink);
}

#define dfo_MenuBlink() 3

LOCALFUNC tMyErr ChooseMenuBlink(void)
{
	tMyErr err;

	err = noErr;
	if (0 == olv_MenuBlink) {
		cur_MenuBlink = dfo_MenuBlink();
	} else {
		if ((cur_MenuBlink < 0) || (cur_MenuBlink > 3)) {
			err = ReportParseFailure(
				"-mnb must be a number between 0 and 3");
		}
	}

	return err;
}

LOCALPROC WrtOptMenuBlink(void)
{
	WrtOptNumberOption("-mnb", cur_MenuBlink, dfo_MenuBlink());
}


/* option: Parameter RAM AutoKeyThresh */
	/* usually in 0 (Off), A (Long), 6, 4, 3 (Short) */

LOCALVAR uimr cur_AutoKeyThresh;
LOCALVAR ui3r olv_AutoKeyThresh;

LOCALPROC ResetAutoKeyThreshOption(void)
{
	olv_AutoKeyThresh = 0;
}

LOCALFUNC tMyErr TryAsAutoKeyThreshOptionNot(void)
{
	return NumberTryAsOptionNot("-kyt",
		(long *)&cur_AutoKeyThresh, &olv_AutoKeyThresh);
}

#define dfo_AutoKeyThresh() 6

LOCALFUNC tMyErr ChooseAutoKeyThresh(void)
{
	tMyErr err;

	err = noErr;
	if (0 == olv_AutoKeyThresh) {
		cur_AutoKeyThresh = dfo_AutoKeyThresh();
	} else {
		if ((cur_AutoKeyThresh < 0) || (cur_AutoKeyThresh > 15)) {
			err = ReportParseFailure(
				"-kyt must be a number between 0 and 15");
		}
	}

	return err;
}

LOCALPROC WrtOptAutoKeyThresh(void)
{
	WrtOptNumberOption("-kyt", cur_AutoKeyThresh, dfo_AutoKeyThresh());
}


/* option: Parameter RAM AutoKeyRate */
	/* usually in 0 (Slow), 6, 4, 3, 1 (Fast) */

LOCALVAR uimr cur_AutoKeyRate;
LOCALVAR ui3r olv_AutoKeyRate;

LOCALPROC ResetAutoKeyRateOption(void)
{
	olv_AutoKeyRate = 0;
}

LOCALFUNC tMyErr TryAsAutoKeyRateOptionNot(void)
{
	return NumberTryAsOptionNot("-kyr",
		(long *)&cur_AutoKeyRate, &olv_AutoKeyRate);
}

#define dfo_AutoKeyRate() 3

LOCALFUNC tMyErr ChooseAutoKeyRate(void)
{
	tMyErr err;

	err = noErr;
	if (0 == olv_AutoKeyRate) {
		cur_AutoKeyRate = dfo_AutoKeyRate();
	} else {
		if ((cur_AutoKeyRate < 0) || (cur_AutoKeyRate > 15)) {
			err = ReportParseFailure(
				"-kyr must be a number between 0 and 15");
		}
	}

	return err;
}

LOCALPROC WrtOptAutoKeyRate(void)
{
	WrtOptNumberOption("-kyr", cur_AutoKeyRate, dfo_AutoKeyRate());
}


LOCALFUNC tMyErr ChooseHilColPart(char *s, uimr *cur_HilColV,
	ui3r olv_HilColV, uimr dfo_HilColV)
{
	tMyErr err;
	Str255 t;

	err = noErr;
	if (0 == olv_HilColV) {
		*cur_HilColV = dfo_HilColV;
	} else {
		if (! cur_mIIorIIX) {
			if (0 != *cur_HilColV) {
				PStrFromCStr(t, s);
				PStrApndCStr(t,
					" not allowed for this emulated computer");
				err = ReportParseFailPStr(t);
			}
		} else
		if ((*cur_HilColV < 0) || (*cur_HilColV > 65535)) {
			PStrFromCStr(t, s);
			PStrApndCStr(t, " must be a number between 0 and 65535");
			err = ReportParseFailPStr(t);
		}
	}

	return err;
}

/* option: Parameter RAM HilColRed */

LOCALVAR uimr cur_HilColRed;
LOCALVAR ui3r olv_HilColRed;

LOCALPROC ResetHilColRedOption(void)
{
	olv_HilColRed = 0;
}

LOCALFUNC tMyErr TryAsHilColRedOptionNot(void)
{
	return NumberTryAsOptionNot("-hcr",
		(long *)&cur_HilColRed, &olv_HilColRed);
}

#define dfo_HilColRed() 0

LOCALFUNC tMyErr ChooseHilColRed(void)
{
	return ChooseHilColPart("-hcr", &cur_HilColRed, olv_HilColRed,
		dfo_HilColRed());
}

LOCALPROC WrtOptHilColRed(void)
{
	WrtOptNumberOption("-hcr", cur_HilColRed, dfo_HilColRed());
}


/* option: Parameter RAM HilColGreen */

LOCALVAR uimr cur_HilColGreen;
LOCALVAR ui3r olv_HilColGreen;

LOCALPROC ResetHilColGreenOption(void)
{
	olv_HilColGreen = 0;
}

LOCALFUNC tMyErr TryAsHilColGreenOptionNot(void)
{
	return NumberTryAsOptionNot("-hcg",
		(long *)&cur_HilColGreen, &olv_HilColGreen);
}

#define dfo_HilColGreen() 0

LOCALFUNC tMyErr ChooseHilColGreen(void)
{
	return ChooseHilColPart("-hcg", &cur_HilColGreen, olv_HilColGreen,
		dfo_HilColGreen());
}

LOCALPROC WrtOptHilColGreen(void)
{
	WrtOptNumberOption("-hcg", cur_HilColGreen, dfo_HilColGreen());
}


/* option: Parameter RAM HilColBlue */

LOCALVAR uimr cur_HilColBlue;
LOCALVAR ui3r olv_HilColBlue;

LOCALPROC ResetHilColBlueOption(void)
{
	olv_HilColBlue = 0;
}

LOCALFUNC tMyErr TryAsHilColBlueOptionNot(void)
{
	return NumberTryAsOptionNot("-hcb",
		(long *)&cur_HilColBlue, &olv_HilColBlue);
}

#define dfo_HilColBlue() 0

LOCALFUNC tMyErr ChooseHilColBlue(void)
{
	return ChooseHilColPart("-hcb", &cur_HilColBlue, olv_HilColBlue,
		dfo_HilColBlue());
}

LOCALPROC WrtOptHilColBlue(void)
{
	WrtOptNumberOption("-hcb", cur_HilColBlue, dfo_HilColBlue());
}


/* option: Speaker Volume */
	/* usually in 3 (Fast), 8 (Medium), 15 (Slow) */

LOCALVAR uimr cur_SpeakerVol;
LOCALVAR ui3r olv_SpeakerVol;

LOCALPROC ResetSpeakerVolOption(void)
{
	olv_SpeakerVol = 0;
}

LOCALFUNC tMyErr TryAsSpeakerVolOptionNot(void)
{
	return NumberTryAsOptionNot("-svl",
		(long *)&cur_SpeakerVol, &olv_SpeakerVol);
}

LOCALFUNC uimr dfo_SpeakerVol(void)
{
	uimr v;

	if (MySoundEnabled) {
		v = 7;
	} else {
		v = 0;
	}

	return v;
}

LOCALFUNC tMyErr ChooseSpeakerVol(void)
{
	tMyErr err;

	err = noErr;


	if (0 == olv_SpeakerVol) {
		cur_SpeakerVol = dfo_SpeakerVol();
	} else {
		if ((cur_SpeakerVol < 0) || (cur_SpeakerVol >= 8)) {
			err = ReportParseFailure(
				"-svl must be a number between 0 and 7");
		}
	}

	return err;
}

LOCALPROC WrtOptSpeakerVol(void)
{
	WrtOptNumberOption("-svl", cur_SpeakerVol, dfo_SpeakerVol());
}


/* option: Minimum Extension */

LOCALVAR blnr WantMinExtn;
LOCALVAR ui3r olv_WantMinExtn;

LOCALPROC ResetWantMinExtn(void)
{
	WantMinExtn = falseblnr;
	olv_WantMinExtn = 0;
}

LOCALFUNC tMyErr TryAsWantMinExtnNot(void)
{
	return FlagTryAsOptionNot("-min-extn",
		&WantMinExtn, &olv_WantMinExtn);
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
LOCALVAR ui3r olv_MouseMotion;

LOCALPROC ResetMouseMotionOption(void)
{
	MyMouseMotion = nanblnr;
	olv_MouseMotion = 0;
}

LOCALFUNC tMyErr TryAsMouseMotionOptionNot(void)
{
	return BooleanTryAsOptionNot("-emm",
		&MyMouseMotion, &olv_MouseMotion);
}

LOCALFUNC blnr dfo_MouseMotion(void)
{
	int v;

	v = (gbk_apifam_gtk != gbo_apifam);

	return v;
}

LOCALFUNC tMyErr ChooseMouseMotion(void)
{

	if (nanblnr == MyMouseMotion) {
		MyMouseMotion = dfo_MouseMotion();
	}

	return noErr;
}

LOCALPROC WrtOptMouseMotion(void)
{
	WrtOptBooleanOption("-emm", MyMouseMotion, dfo_MouseMotion());
}


/* option: GrabKeysFullScreen */

LOCALVAR blnr WantGrabKeysFS;
LOCALVAR ui3r olv_GrabKeysFS;

LOCALPROC ResetGrabKeysFS(void)
{
	WantGrabKeysFS = nanblnr;
	olv_GrabKeysFS = 0;
}

LOCALFUNC tMyErr TryAsGrabKeysFSNot(void)
{
	return BooleanTryAsOptionNot("-gkf",
		&WantGrabKeysFS, &olv_GrabKeysFS);
}

#define dfo_GrabKeysFS() trueblnr

LOCALFUNC tMyErr ChooseGrabKeysFS(void)
{
	if (nanblnr == WantGrabKeysFS) {
		WantGrabKeysFS = dfo_GrabKeysFS();
	}

	return noErr;
}

LOCALPROC WrtOptGrabKeysFS(void)
{
	WrtOptBooleanOption("-gkf", WantGrabKeysFS, dfo_GrabKeysFS());
}


/* option: Enable Control Interrupt */

LOCALVAR blnr WantEnblCtrlInt;
LOCALVAR ui3r olv_EnblCtrlInt;

LOCALPROC ResetEnblCtrlInt(void)
{
	WantEnblCtrlInt = nanblnr;
	olv_EnblCtrlInt = 0;
}

LOCALFUNC tMyErr TryAsEnblCtrlIntNot(void)
{
	return BooleanTryAsOptionNot("-eci",
		&WantEnblCtrlInt, &olv_EnblCtrlInt);
}

#define dfo_EnblCtrlInt() trueblnr

LOCALFUNC tMyErr ChooseEnblCtrlInt(void)
{
	if (nanblnr == WantEnblCtrlInt) {
		WantEnblCtrlInt = dfo_EnblCtrlInt();
	}

	return noErr;
}

LOCALPROC WrtOptEnblCtrlInt(void)
{
	WrtOptBooleanOption("-eci", WantEnblCtrlInt, dfo_EnblCtrlInt());
}


/* option: Enable Control Reset */

LOCALVAR blnr WantEnblCtrlRst;
LOCALVAR ui3r olv_EnblCtrlRst;

LOCALPROC ResetEnblCtrlRst(void)
{
	WantEnblCtrlRst = nanblnr;
	olv_EnblCtrlRst = 0;
}

LOCALFUNC tMyErr TryAsEnblCtrlRstNot(void)
{
	return BooleanTryAsOptionNot("-ecr",
		&WantEnblCtrlRst, &olv_EnblCtrlRst);
}

#define dfo_EnblCtrlRst() trueblnr

LOCALFUNC tMyErr ChooseEnblCtrlRst(void)
{
	if (nanblnr == WantEnblCtrlRst) {
		WantEnblCtrlRst = dfo_EnblCtrlRst();
	}

	return noErr;
}

LOCALPROC WrtOptEnblCtrlRst(void)
{
	WrtOptBooleanOption("-ecr", WantEnblCtrlRst, dfo_EnblCtrlRst());
}


/* option: Enable Control K (emulated control toggle) */

LOCALVAR blnr WantEnblCtrlKtg;
LOCALVAR ui3r olv_EnblCtrlKtg;

LOCALPROC ResetEnblCtrlKtg(void)
{
	WantEnblCtrlKtg = nanblnr;
	olv_EnblCtrlKtg = 0;
}

LOCALFUNC tMyErr TryAsEnblCtrlKtgNot(void)
{
	return BooleanTryAsOptionNot("-eck",
		&WantEnblCtrlKtg, &olv_EnblCtrlKtg);
}

#define dfo_EnblCtrlKtg() trueblnr

LOCALFUNC tMyErr ChooseEnblCtrlKtg(void)
{
	if (nanblnr == WantEnblCtrlKtg) {
		WantEnblCtrlKtg = dfo_EnblCtrlKtg();
	}

	return noErr;
}

LOCALPROC WrtOptEnblCtrlKtg(void)
{
	WrtOptBooleanOption("-eck", WantEnblCtrlKtg, dfo_EnblCtrlKtg());
}


/* option: Want Color Image */

LOCALVAR blnr WantColorImage;
LOCALVAR ui3r olv_ColorImage;

LOCALPROC ResetWantColorImage(void)
{
	WantColorImage = nanblnr;
	olv_ColorImage = 0;
}

LOCALFUNC tMyErr TryAsWantColorImageNot(void)
{
	return BooleanTryAsOptionNot("-ci",
		&WantColorImage, &olv_ColorImage);
}

LOCALFUNC blnr dfo_ColorImage(void)
{
	int v;

	if (gbk_apifam_xwn == gbo_apifam) {
		v = trueblnr;
	} else {
		/* leave as default */
		v = nanblnr;
	}

	return v;
}

LOCALFUNC tMyErr ChooseWantColorImage(void)
{
	tMyErr err;

	err = noErr;

	if (nanblnr == WantColorImage) {
		WantColorImage = dfo_ColorImage();
	} else {
		if (gbk_apifam_xwn != gbo_apifam) {
			err = ReportParseFailure(
				"-ci is only for -api xwn");
		} else
		if ((! WantColorImage) && (cur_ScrnDpth != 0)) {
			err = ReportParseFailure(
				"-ci 0 requires -depth 0");
		} else
		{
			/* ok */
		}
	}

	return err;
}

LOCALPROC WrtOptColorImage(void)
{
	WrtOptBooleanOption("-ci", WantColorImage, dfo_ColorImage());
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
LOCALVAR ui3r olv_AltHappyMac;

LOCALPROC ResetAltHappyMacOption(void)
{
	cur_AltHappyMac = kListOptionAuto;
	olv_AltHappyMac = 0;
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

LOCALFUNC tMyErr TryAsAltHappyMacOptionNot(void)
{
	return FindNamedOption("-ahm", kNumAHMs, GetAltHappyMacName,
		&cur_AltHappyMac, &olv_AltHappyMac);
}

#define dfo_AltHappyMac() gbk_AHM_none

LOCALFUNC tMyErr ChooseAltHappyMac(void)
{
	if (kListOptionAuto == cur_AltHappyMac) {
		cur_AltHappyMac = dfo_AltHappyMac();
	}

	return noErr;
}

LOCALPROC WrtOptAltHappyMac(void)
{
	WrtOptNamedOption("-ahm", GetAltHappyMacName,
		cur_AltHappyMac, dfo_AltHappyMac());
}


/* option: ROM size */

LOCALVAR uimr cur_RomSize;
LOCALVAR ui3r olv_RomSize;

LOCALPROC ResetRomSizeOption(void)
{
	olv_RomSize = 0;
}

LOCALFUNC tMyErr TryAsRomSizeOptionNot(void)
{
	return NumberTryAsOptionNot("-rsz",
		(long *)&cur_RomSize, &olv_RomSize);
}

LOCALFUNC uimr dfo_RomSize(void)
{
	uimr v;

	switch (cur_mdl) {
		case gbk_mdl_Twig43:
		case gbk_mdl_Twiggy:
		case gbk_mdl_128K:
			v = ln2_msz_64K; /* 64 KB */
			break;
		case gbk_mdl_512Ke:
		case gbk_mdl_Plus:
			v = ln2_msz_128K; /* 128 KB */
			break;
		case gbk_mdl_SE:
		case gbk_mdl_SEFDHD:
		case gbk_mdl_PB100:
		case gbk_mdl_II:
		case gbk_mdl_IIx:
			v = ln2_msz_256K; /* 256 KB */
			break;
		case gbk_mdl_Classic:
		default:
			v = ln2_msz_512K; /* 512 KB */
			break;
	}

	return v;
}

LOCALFUNC tMyErr ChooseRomSize(void)
{
	tMyErr err;

	err = noErr;


	if (0 == olv_RomSize) {
		cur_RomSize = dfo_RomSize();
	} else {
		if ((cur_RomSize < 16) || (cur_RomSize > 31)) {
			err = ReportParseFailure(
				"-rsz must be a number between 16 and 31");
		}
	}

	return err;
}

LOCALPROC WrtOptRomSize(void)
{
	WrtOptNumberOption("-rsz", cur_RomSize, dfo_RomSize());
}


/* option: Want Check RomCheck Sum */

LOCALVAR blnr WantCheckRomCheckSum;
LOCALVAR ui3r olv_CheckRomCheckSum;

LOCALPROC ResetCheckRomCheckSum(void)
{
	WantCheckRomCheckSum = nanblnr;
	olv_CheckRomCheckSum = 0;
}

LOCALFUNC tMyErr TryAsCheckRomCheckSumNot(void)
{
	return BooleanTryAsOptionNot("-chr",
		&WantCheckRomCheckSum, &olv_CheckRomCheckSum);
}

#define dfo_CheckRomCheckSum() trueblnr

LOCALFUNC tMyErr ChooseCheckRomCheckSum(void)
{
	if (nanblnr == WantCheckRomCheckSum) {
		WantCheckRomCheckSum = dfo_CheckRomCheckSum();
	}

	return noErr;
}

LOCALPROC WrtOptCheckRomCheckSum(void)
{
	WrtOptBooleanOption("-chr",
		WantCheckRomCheckSum, dfo_CheckRomCheckSum());
}


/* option: Want Disable Rom Check */

LOCALVAR blnr WantDisableRomCheck;
LOCALVAR ui3r olv_DisableRomCheck;

LOCALPROC ResetDisableRomCheck(void)
{
	WantDisableRomCheck = nanblnr;
	olv_DisableRomCheck = 0;
}

LOCALFUNC tMyErr TryAsDisableRomCheckNot(void)
{
	return BooleanTryAsOptionNot("-drc",
		&WantDisableRomCheck, &olv_DisableRomCheck);
}

#define dfo_DisableRomCheck() trueblnr

LOCALFUNC tMyErr ChooseDisableRomCheck(void)
{
	if (nanblnr == WantDisableRomCheck) {
		WantDisableRomCheck = dfo_DisableRomCheck();
	}

	return noErr;
}

LOCALPROC WrtOptDisableRomCheck(void)
{
	WrtOptBooleanOption("-drc",
		WantDisableRomCheck, dfo_DisableRomCheck());
}


/* option: Want Disable Ram Test */

LOCALVAR blnr WantDisableRamTest;
LOCALVAR ui3r olv_DisableRamTest;

LOCALPROC ResetDisableRamTest(void)
{
	WantDisableRamTest = nanblnr;
	olv_DisableRamTest = 0;
}

LOCALFUNC tMyErr TryAsDisableRamTestNot(void)
{
	return BooleanTryAsOptionNot("-drt",
		&WantDisableRamTest, &olv_DisableRamTest);
}

#define dfo_DisableRamTest() trueblnr

LOCALFUNC tMyErr ChooseDisableRamTest(void)
{
	if (nanblnr == WantDisableRamTest) {
		WantDisableRamTest = dfo_DisableRamTest();
	}

	return noErr;
}

LOCALPROC WrtOptDisableRamTest(void)
{
	WrtOptBooleanOption("-drt",
		WantDisableRamTest, dfo_DisableRamTest());
}


/* option: Want Disassembly */

LOCALVAR blnr WantDisasm;
LOCALVAR ui3r olv_WantDisasm;

LOCALPROC ResetWantDisasm(void)
{
	WantDisasm = nanblnr;
	olv_WantDisasm = 0;
}

LOCALFUNC tMyErr TryAsWantDisasmNot(void)
{
	return BooleanTryAsOptionNot("-dis",
		&WantDisasm, &olv_WantDisasm);
}

#define dfo_WantDisasm() falseblnr

LOCALFUNC tMyErr ChooseWantDisasm(void)
{
	if (nanblnr == WantDisasm) {
		WantDisasm = dfo_WantDisasm();
	}

	return noErr;
}

LOCALPROC WrtOptWantDisasmNot(void)
{
	WrtOptBooleanOption("-dis", WantDisasm, dfo_WantDisasm());
}


/* option: dbglog_HAVE */

LOCALVAR blnr DbgLogHAVE;
LOCALVAR ui3r olv_DbgLogHAVE;

LOCALPROC ResetDbgLogHAVE(void)
{
	DbgLogHAVE = nanblnr;
	olv_DbgLogHAVE = 0;
}

LOCALFUNC tMyErr TryAsDbgLogHAVENot(void)
{
	return BooleanTryAsOptionNot("-log", &DbgLogHAVE, &olv_DbgLogHAVE);
}

#define dfo_DbgLogHAVE() WantDisasm

LOCALFUNC tMyErr ChooseDbgLogHAVE(void)
{
	if (nanblnr == DbgLogHAVE) {
		DbgLogHAVE = dfo_DbgLogHAVE();
	}

	return noErr;
}

LOCALPROC WrtOptDbgLogHAVE(void)
{
	WrtOptBooleanOption("-log", DbgLogHAVE, dfo_DbgLogHAVE());
}


/* option: Screen VSync */

LOCALVAR blnr WantScreenVSync;
LOCALVAR ui3r olv_ScreenVSync;

LOCALPROC ResetScreenVSync(void)
{
	WantScreenVSync = nanblnr;
	olv_ScreenVSync = 0;
}

LOCALFUNC tMyErr TryAsScreenVSyncNot(void)
{
	return BooleanTryAsOptionNot("-vsync",
		&WantScreenVSync, &olv_ScreenVSync);
}

#define dfo_ScreenVSync() falseblnr

LOCALFUNC tMyErr ChooseScreenVSync(void)
{
	tMyErr err;

	err = noErr;
	if (nanblnr == WantScreenVSync) {
		WantScreenVSync = dfo_ScreenVSync();
	} else {
		if (WantScreenVSync && (gbk_apifam_osx != gbo_apifam)) {
			err = ReportParseFailure(
				"-vsync is so far only implemented for OS X");
		}
	}

	return err;
}

LOCALPROC WrtOptScreenVSync(void)
{
	WrtOptBooleanOption("-vsync", WantScreenVSync, dfo_ScreenVSync());
}

/* ------ */

LOCALVAR blnr NeedScrnHack;
LOCALVAR blnr NeedVidMem;

LOCALFUNC tMyErr ChooseScreenOpts(void)
{
	tMyErr err;

	err = noErr;
	if (! cur_mIIorIIX) {
		if (cur_hres * cur_vres > (uimr)2 * 1024 * 1024) {
			err = ReportParseFailure(
				"-hres and -vres multiplied must be < 2M");
		}
	}

	if ((gbk_mdl_PB100 == cur_mdl)
		|| cur_mIIorIIX)
	{
		NeedScrnHack = falseblnr;
		NeedVidMem = trueblnr;
	} else {
		NeedScrnHack = (cur_hres != dfo_hres())
			|| (cur_vres != dfo_vres());
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

	EmVidCard = cur_mIIorIIX;

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
			err = ReportParseFailure(
				"video memory must be <= 4M");
		} else if (VidMemSize <= 0x00008000) {
			VidMemSize = 0x00008000;
		}
	} else if (gbk_mdl_PB100 == cur_mdl) {
		VidMemSize = 0x00008000;
	} else {
		/* VidMemSize = 0x00020000; */

		if (VidMemSize > 256 * 1024) {
			err = ReportParseFailure(
				"video memory must be <= 256K");
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
		|| cur_mIIorIIX)
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

	EmVIA2 = cur_mIIorIIX;
	EmASC = (gbk_mdl_PB100 == cur_mdl) || cur_mIIorIIX;

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
	ResetKeyMapOption();
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
	ResetHilColRedOption();
	ResetHilColGreenOption();
	ResetHilColBlueOption();
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
	if (kMyErrNoMatch == (err = TryAsKeyMapOptionNot()))
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
	if (kMyErrNoMatch == (err = TryAsHilColRedOptionNot()))
	if (kMyErrNoMatch == (err = TryAsHilColGreenOptionNot()))
	if (kMyErrNoMatch == (err = TryAsHilColBlueOptionNot()))
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
	if (noErr == (err = ChooseKeyMap()))
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
	if (noErr == (err = ChooseHilColRed()))
	if (noErr == (err = ChooseHilColGreen()))
	if (noErr == (err = ChooseHilColBlue()))
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
	WrtOptKeyMap();
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
	WrtOptHilColRed();
	WrtOptHilColGreen();
	WrtOptHilColBlue();
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
}
