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

LOCALPROC ChooseModel(void)
{
	if (cur_mdl == kListOptionAuto) {
		cur_mdl = gbk_mdl_Plus;
	}
}

LOCALFUNC tMyErr TryAsModelOptionNot(void)
{
	return FindNamedOption("-m", kNumModels, GetModelName, &cur_mdl);
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

LOCALFUNC tMyErr ChooseMemSiz(void)
{
	if (cur_msz == kListOptionAuto) {
		switch (cur_mdl) {
			case gbk_mdl_128K:
				cur_msz = gbk_msz_128K;
				break;
			case gbk_mdl_512Ke:
				cur_msz = gbk_msz_512K;
				break;
			case gbk_mdl_II:
			case gbk_mdl_IIx:
				cur_msz = gbk_msz_8M;
				break;
			case gbk_mdl_Plus:
			case gbk_mdl_SE:
			case gbk_mdl_SEFDHD:
			case gbk_mdl_Classic:
			case gbk_mdl_PB100:
			default:
				cur_msz = gbk_msz_4M;
				break;
		}
		if (gbk_targfam_lnds == gbo_targfam) {
			if (cur_msz > gbk_msz_2M) {
				cur_msz = gbk_msz_2M;
			}
		}
	} else {
		/* should error check here */
	}
	return noErr;
}

LOCALFUNC tMyErr TryAsMemSizOptionNot(void)
{
	return FindNamedOption("-mem",
		kNumMemSizs, GetMemSizName, &cur_msz);
}

/* memory bank sizes */

LOCALVAR uimr RAMa_Size;
LOCALVAR uimr RAMb_Size;

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

/* option: sound */

LOCALVAR blnr MySoundEnabled;

LOCALPROC ResetSoundOption(void)
{
	MySoundEnabled = nanblnr;
}

LOCALFUNC tMyErr TryAsSoundOptionNot(void)
{
	return BooleanTryAsOptionNot("-sound", &MySoundEnabled);
}

LOCALPROC ChooseSoundEnabled(void)
{
	if (nanblnr == MySoundEnabled) {
		MySoundEnabled = ((gbo_apifam == gbk_apifam_mac)
			|| (gbo_apifam == gbk_apifam_osx)
			|| (gbo_apifam == gbk_apifam_win)
			|| (gbo_apifam == gbk_apifam_sdl)
			|| (gbo_apifam == gbk_apifam_cco)
			|| ((gbo_apifam == gbk_apifam_xwn)
				&& ((gbo_targfam == gbk_targfam_linx)
					|| (gbo_targfam == gbk_targfam_fbsd)
					|| (gbo_targfam == gbk_targfam_nbsd))))
			&& (gbk_mdl_II != cur_mdl) && (gbk_mdl_IIx != cur_mdl);
	}
}

/* option: sound api */

enum {
	gbk_sndapi_none,
	gbk_sndapi_alsa,
	gbk_sndapi_ddsp,
	kNumSndApiLevels
};

LOCALVAR int gbo_sndapi;

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

	if (kListOptionAuto == gbo_sndapi) {
		if (! MySoundEnabled) {
			gbo_sndapi = gbk_sndapi_none;
		} else if (gbo_apifam != gbk_apifam_xwn) {
			gbo_sndapi = gbk_sndapi_none;
		} else if (gbo_targfam == gbk_targfam_linx) {
			gbo_sndapi = gbk_sndapi_alsa;
		} else {
			gbo_sndapi = gbk_sndapi_ddsp;
		}
	}

	return err;
}

/* option: number of drives */

LOCALVAR uimr cur_numdrives;
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
	if (! have_numdrives) {
		if (cur_mdl < gbk_mdl_512Ke) {
			cur_numdrives = 2;
		} else {
			cur_numdrives = 6;
		}
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

/* option: Emulated CPU version */

LOCALVAR uimr em_cpu_vers;
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

LOCALPROC ChooseEmCpuVers(void)
{
	if (! have_em_cpu_vers) {
		if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
			em_cpu_vers = 2;
		} else {
			em_cpu_vers = 0;
		}
		have_em_cpu_vers = trueblnr;
	}
}

/* option: Initial FullScreen */

LOCALVAR blnr WantInitFullScreen;

LOCALPROC ResetInitFullScreen(void)
{
	WantInitFullScreen = nanblnr;
}

LOCALFUNC tMyErr TryAsInitFullScreenNot(void)
{
	return BooleanTryAsOptionNot("-fullscreen", &WantInitFullScreen);
}

LOCALPROC ChooseInitFullScreen(void)
{
	if (nanblnr == WantInitFullScreen) {
		WantInitFullScreen = (gbk_targfam_wnce == gbo_targfam);
	}
}

/* option: Variable FullScreen */

LOCALVAR blnr WantVarFullScreen;

LOCALPROC ResetVarFullScreen(void)
{
	WantVarFullScreen = nanblnr;
}

LOCALFUNC tMyErr TryAsVarFullScreenNot(void)
{
	return BooleanTryAsOptionNot("-var-fullscreen",
		&WantVarFullScreen);
}

LOCALPROC ChooseVarFullScreen(void)
{
	if (nanblnr == WantVarFullScreen) {
		if ((gbk_apifam_gtk == gbo_apifam)
			|| (gbk_targfam_wnce == gbo_targfam))
		{
			WantVarFullScreen = falseblnr;
		} else {
			WantVarFullScreen = trueblnr;
		}
	}
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

/* option: Initial Run In Background */

LOCALVAR blnr WantInitBackground;

LOCALPROC ResetInitBackground(void)
{
	WantInitBackground = nanblnr;
}

LOCALFUNC tMyErr TryAsInitBackgroundNot(void)
{
	return BooleanTryAsOptionNot("-bg", &WantInitBackground);
}

LOCALPROC ChooseInitBackground(void)
{
	if (nanblnr == WantInitBackground) {
		if (WantLocalTalk) {
			WantInitBackground = trueblnr;
		} else {
			WantInitBackground = falseblnr;
		}
	}
}

/* option: Initial AutoSlow */

LOCALVAR blnr WantInitAutoSlow;

LOCALPROC ResetInitAutoSlow(void)
{
	WantInitAutoSlow = nanblnr;
}

LOCALFUNC tMyErr TryAsInitAutoSlowNot(void)
{
	return BooleanTryAsOptionNot("-as", &WantInitAutoSlow);
}

LOCALPROC ChooseInitAutoSlow(void)
{
	if (nanblnr == WantInitAutoSlow) {
		WantInitAutoSlow = (gbk_mdl_II != cur_mdl)
			&& (gbk_mdl_IIx != cur_mdl);
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
		if ((gbk_mdl_II == cur_mdl)
			|| (gbk_mdl_IIx == cur_mdl))
		{
			CurInitSpeed = gbk_speed_4X;
		} else {
			CurInitSpeed = gbk_speed_8X;
		}
	}
}

LOCALFUNC tMyErr TryAsInitSpeedOptionNot(void)
{
	return FindNamedOption("-speed",
		kNumSpeeds, GetInitSpeedName, &CurInitSpeed);
}

/* option: Alternate Keyboard Mode */

LOCALVAR blnr WantMinExtn;

LOCALPROC ResetWantMinExtn(void)
{
	WantMinExtn = falseblnr;
}

LOCALFUNC tMyErr TryAsWantMinExtnNot(void)
{
	return FlagTryAsOptionNot("-min-extn", &WantMinExtn);
}

/* option: disk driver - support tags */

LOCALVAR blnr SonySupportTags;

LOCALPROC ResetSonySupportTags(void)
{
	SonySupportTags = nanblnr;
}

LOCALFUNC tMyErr TryAsSonySupportTagsNot(void)
{
	return BooleanTryAsOptionNot("-sony-tag", &SonySupportTags);
}

LOCALPROC ChooseSonySupportTags(void)
{
	if (nanblnr == SonySupportTags) {
		SonySupportTags = falseblnr;
	}
}

/* option: disk driver - calculate checksums */

LOCALVAR blnr SonyWantChecksumsUpdated;

LOCALPROC ResetSonyWantChecksumsUpdated(void)
{
	SonyWantChecksumsUpdated = nanblnr;
}

LOCALFUNC tMyErr TryAsSonyWantChecksumsUpdatedNot(void)
{
	return BooleanTryAsOptionNot("-sony-sum",
		&SonyWantChecksumsUpdated);
}

LOCALPROC ChooseSonyWantChecksumsUpdated(void)
{
	if (nanblnr == SonyWantChecksumsUpdated) {
		SonyWantChecksumsUpdated = falseblnr;
	}
}

/* option: disk driver - support tags */

LOCALVAR blnr SonySupportDC42;

LOCALPROC ResetSonySupportDC42(void)
{
	SonySupportDC42 = nanblnr;
}

LOCALFUNC tMyErr TryAsSonySupportDC42Not(void)
{
	return BooleanTryAsOptionNot("-sony-dc42", &SonySupportDC42);
}

LOCALPROC ChooseSonySupportDC42(void)
{
	if (nanblnr == SonySupportDC42) {
		SonySupportDC42 = trueblnr;
	}
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

/* option: horizontal resolution */

LOCALVAR uimr cur_hres;
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

LOCALVAR uimr dflt_hres;

LOCALFUNC tMyErr ChooseHRes(void)
{
	tMyErr err;

	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		dflt_hres = 640;
	} else if (gbk_mdl_PB100 == cur_mdl) {
		dflt_hres = 640;
	} else {
		dflt_hres = 512;
	}

	if (! have_hres) {
		cur_hres = dflt_hres;
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

/* option: vertical resolution */

LOCALVAR uimr cur_vres;
LOCALVAR blnr have_vres;

LOCALPROC ResetVResOption(void)
{
	have_vres = falseblnr;
}

LOCALFUNC tMyErr TryAsVResOptionNot(void)
{
	return NumberTryAsOptionNot("-vres", (long *)&cur_vres, &have_vres);
}

LOCALVAR uimr dflt_vres;

LOCALFUNC tMyErr ChooseVRes(void)
{
	tMyErr err;

	if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
		dflt_vres = 480;
	} else if (gbk_mdl_PB100 == cur_mdl) {
		dflt_vres = 400;
	} else {
		dflt_vres = 342;
	}

	if (! have_vres) {
		cur_vres = dflt_vres;
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

/* option: screen depth */

LOCALVAR uimr cur_ScrnDpth;
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
	if (! have_ScrnDpth) {
		if ((gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl)) {
			cur_ScrnDpth = 3;
		} else {
			cur_ScrnDpth = 0;
		}
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

/* option: magnification factor */

LOCALVAR uimr cur_MagFctr;
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
	if (! have_MagFctr) {
		if (gbk_apifam_gtk == gbo_apifam) {
			/* temporary, until implemented */
			cur_MagFctr = 1;
		} else {
			cur_MagFctr = 2;
		}
		have_MagFctr = trueblnr;
	} else {
		if (cur_MagFctr < 1) {
			ReportParseFailure("-mf must be >= 1");
			err = kMyErrReported;
		}
	}

	return err;
}

/* option: Initial Magnify */

LOCALVAR blnr WantInitMagnify;

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
		WantInitMagnify = falseblnr;
	} else {
		if (WantInitMagnify && (cur_MagFctr == 1)) {
			ReportParseFailure(
				"-magnify 1 does not make sense with -mf 1");
			err = kMyErrReported;
		}
	}

	return err;
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
		if (WantScreenVSync && (gbo_apifam != gbk_apifam_osx)) {
			ReportParseFailure(
				"-vsync is so far only implemented for OS X");
			err = kMyErrReported;
		}
	}

	return err;
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
	NeedScrnHack = (cur_hres != dflt_hres)
		|| (cur_vres != dflt_vres);
	NeedVidMem = NeedScrnHack || (gbk_mdl_PB100 == cur_mdl)
		|| (gbk_mdl_II == cur_mdl) || (gbk_mdl_IIx == cur_mdl);

	return err;
}

LOCALVAR blnr EmVidCard;

/* video memory size */

LOCALVAR uimr VidMemSize;

LOCALFUNC tMyErr ChooseVidMemSize(void)
{
	tMyErr err;

	EmVidCard = (cur_mdl == gbk_mdl_II) || (cur_mdl == gbk_mdl_IIx);

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

LOCALPROC ChooseMiscEmHardware(void)
{
	EmClassicKbrd = cur_mdl <= gbk_mdl_Plus;

	if (cur_mdl <= gbk_mdl_Plus) {
		EmADB = falseblnr;
		EmRTC = trueblnr;
		EmPMU = falseblnr;
	} else if ((cur_mdl <= gbk_mdl_Classic)
		|| (cur_mdl == gbk_mdl_II) || (cur_mdl == gbk_mdl_IIx))
	{
		EmADB = trueblnr;
		EmRTC = trueblnr;
		EmPMU = falseblnr;
	} else {
		EmADB = falseblnr;
		EmRTC = falseblnr;
		EmPMU = trueblnr;
	}

	EmVIA2 = (cur_mdl == gbk_mdl_II) || (cur_mdl == gbk_mdl_IIx);
	EmASC = (cur_mdl == gbk_mdl_PB100) || (cur_mdl == gbk_mdl_II)
		|| (cur_mdl == gbk_mdl_IIx);
}

/* option: Want Disassembly */

LOCALVAR blnr WantDisasm;

LOCALPROC ResetWantDisasm(void)
{
	WantDisasm = nanblnr;
}

LOCALFUNC tMyErr TryAsWantDisasmNot(void)
{
	return BooleanTryAsOptionNot("-dis", &WantDisasm);
}

LOCALPROC ChooseWantDisasm(void)
{
	if (nanblnr == WantDisasm) {
		WantDisasm = falseblnr;
	}
}

/* option: dbglog_HAVE */

LOCALVAR blnr DbgLogHAVE;

LOCALPROC ResetDbgLogHAVE(void)
{
	DbgLogHAVE = nanblnr;
}

LOCALFUNC tMyErr TryAsDbgLogHAVENot(void)
{
	return BooleanTryAsOptionNot("-log", &DbgLogHAVE);
}

LOCALPROC ChooseDbgLogHAVE(void)
{
	if (nanblnr == DbgLogHAVE) {
		DbgLogHAVE = WantDisasm;
	}
}

/* option: Timing Accuracy */

LOCALVAR uimr timingacc;
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

LOCALPROC ChooseTimingAccuracy(void)
{
	if (! have_timingacc) {
		timingacc = 1;
		have_timingacc = trueblnr;
	}
}

/* option: MouseMotion */

LOCALVAR blnr MyMouseMotion;

LOCALPROC ResetMouseMotionOption(void)
{
	MyMouseMotion = nanblnr;
}

LOCALFUNC tMyErr TryAsMouseMotionOptionNot(void)
{
	return BooleanTryAsOptionNot("-emm", &MyMouseMotion);
}

LOCALPROC ChooseMouseMotion(void)
{
	if (nanblnr == MyMouseMotion) {
		MyMouseMotion = (gbk_apifam_gtk != gbo_apifam);
	}
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

/* option: ItnlKyBdFix */

LOCALVAR blnr ItnlKyBdFix;

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

	if (nanblnr == ItnlKyBdFix) {
		ItnlKyBdFix = (gbk_apifam_win == gbo_apifam);
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

/* ROM size */

LOCALVAR uimr RomSize;

LOCALPROC ChooseRomSize(void)
{
	switch (cur_mdl) {
		case gbk_mdl_128K:
			RomSize = 0x010000; /* 64 KB */
			break;
		case gbk_mdl_512Ke:
		case gbk_mdl_Plus:
			RomSize = 0x020000; /* 128 KB */
			break;
		case gbk_mdl_SE:
		case gbk_mdl_SEFDHD:
		case gbk_mdl_PB100:
		case gbk_mdl_II:
		case gbk_mdl_IIx:
			RomSize = 0x040000; /* 256 KB */
			break;
		case gbk_mdl_Classic:
		default:
			RomSize = 0x080000; /* 512 KB */
			break;
	}
}

/* total memory size */

#define dbglog_buflnsz 18

#define kLn2SoundBuffers 4 /* kSoundBuffers must be a power of two */
#define kLnOneBuffLen 9
#define kLn2SoundSampSz 3

#define dbhBufferSize (((1UL << kLn2SoundBuffers) + 1UL) \
	<< (kLnOneBuffLen + kLn2SoundSampSz - 3))

#define vMacScreenNumBytes ((((cur_hres * cur_vres) \
	<< cur_ScrnDpth) + 7) >> 3)

LOCALVAR uimr TotMemSize;

LOCALPROC ChooseTotMemSize(void)
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
}

/* Use assembly code for 68k emulation */

LOCALVAR blnr UseAsm68k;

LOCALPROC ChooseUseAsm68k(void)
{
	UseAsm68k = (gbk_asm_none != cur_asm)
		&& (em_cpu_vers == 0) && (timingacc < 2)
		&& ((gbo_cpufam == gbk_cpufam_x86)
			|| (gbo_cpufam == gbk_cpufam_ppc))
		&& (! WantDisasm);
}

/* option: Parameter RAM CaretBlinkTime */
	/* usually in 3 (Fast), 8 (Medium), 15 (Slow) */

LOCALVAR uimr cur_CaretBlinkTime;
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

	err = noErr;
	if (! have_CaretBlinkTime) {
		if ((cur_mdl == gbk_mdl_II) || (cur_mdl == gbk_mdl_IIx)) {
			cur_CaretBlinkTime = 8;
		} else {
			cur_CaretBlinkTime = 3;
		}

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

/* ------ */

LOCALPROC SPResetCommandLineParameters(void)
{
	ResetModelOption();
	ResetMemSizOption();
	ResetNumDrivesOption();
	ResetSoundOption();
	ResetSndApiOption();
	ResetEmCpuVersOption();
	ResetInitFullScreen();
	ResetVarFullScreen();
	ResetInitMagnify();
	ResetAltKeysMode();
	ResetCmndOptSwap();
	ResetLocalTalk();
	ResetInitBackground();
	ResetInitAutoSlow();
	ResetInitSpeedOption();
	ResetWantMinExtn();
	ResetHResOption();
	ResetVResOption();
	ResetScrnDpthOption();
	ResetMagFctrOption();
	ResetScreenVSync();
	ResetSonySupportTags();
	ResetSonyWantChecksumsUpdated();
	ResetSonySupportDC42();
	ResetActvCode();
	ResetWantDisasm();
	ResetDbgLogHAVE();
	ResetTimingAccuracyOption();
	ResetMouseMotionOption();
	ResetNeedIntl();
	ResetItnlKyBdFixOption();
	ResetCaretBlinkTimeOption();
}

LOCALFUNC tMyErr TryAsSPOptionNot(void)
{
	tMyErr err;

	if (kMyErrNoMatch == (err = TryAsModelOptionNot()))
	if (kMyErrNoMatch == (err = TryAsMemSizOptionNot()))
	if (kMyErrNoMatch == (err = TryAsNumDrivesOptionNot()))
	if (kMyErrNoMatch == (err = TryAsSoundOptionNot()))
	if (kMyErrNoMatch == (err = TryAsSndApiOptionNot()))
	if (kMyErrNoMatch == (err = TryAsEmCpuVersOptionNot()))
	if (kMyErrNoMatch == (err = TryAsInitFullScreenNot()))
	if (kMyErrNoMatch == (err = TryAsVarFullScreenNot()))
	if (kMyErrNoMatch == (err = TryAsInitMagnifyNot()))
	if (kMyErrNoMatch == (err = TryAsAltKeysModeNot()))
	if (kMyErrNoMatch == (err = TryAsCmndOptSwapNot()))
	if (kMyErrNoMatch == (err = TryAsLocalTalkNot()))
	if (kMyErrNoMatch == (err = TryAsInitBackgroundNot()))
	if (kMyErrNoMatch == (err = TryAsInitAutoSlowNot()))
	if (kMyErrNoMatch == (err = TryAsInitSpeedOptionNot()))
	if (kMyErrNoMatch == (err = TryAsWantMinExtnNot()))
	if (kMyErrNoMatch == (err = TryAsHResOptionNot()))
	if (kMyErrNoMatch == (err = TryAsVResOptionNot()))
	if (kMyErrNoMatch == (err = TryAsScrnDpthOptionNot()))
	if (kMyErrNoMatch == (err = TryAsMagFctrOptionNot()))
	if (kMyErrNoMatch == (err = TryAsScreenVSyncNot()))
	if (kMyErrNoMatch == (err = TryAsSonySupportTagsNot()))
	if (kMyErrNoMatch == (err = TryAsSonyWantChecksumsUpdatedNot()))
	if (kMyErrNoMatch == (err = TryAsSonySupportDC42Not()))
	if (kMyErrNoMatch == (err = TryAsActvCodeNot()))
	if (kMyErrNoMatch == (err = TryAsWantDisasmNot()))
	if (kMyErrNoMatch == (err = TryAsDbgLogHAVENot()))
	if (kMyErrNoMatch == (err = TryAsTimingAccuracyOptionNot()))
	if (kMyErrNoMatch == (err = TryAsMouseMotionOptionNot()))
	if (kMyErrNoMatch == (err = TryAsNeedIntlNot()))
	if (kMyErrNoMatch == (err = TryAsItnlKyBdFixNot()))
	if (kMyErrNoMatch == (err = TryAsCaretBlinkTimeOptionNot()))
	{
	}

	return err;
}

LOCALFUNC tMyErr AutoChooseSPSettings(void)
{
	tMyErr err;

	ChooseSoundEnabled();
	ChooseModel();
	if (noErr == (err = ChooseMemSiz()))
	if (noErr == (err = ChooseMemBankSizes()))
	if (noErr == (err = ChooseNumDrives()))
	if (noErr == (err = ChooseHRes()))
	if (noErr == (err = ChooseVRes()))
	if (noErr == (err = ChooseScrnDpth()))
	if (noErr == (err = ChooseMagFctr()))
	if (noErr == (err = ChooseInitMagnify()))
	if (noErr == (err = ChooseScreenVSync()))
	if (noErr == (err = ChooseScreenOpts()))
	if (noErr == (err = ChooseVidMemSize()))
	if (noErr == (err = ChooseSndApiOption()))
	if (noErr == (err = ChooseItnlKyBdFix()))
	if (noErr == (err = ChooseCaretBlinkTime()))
	{
		ChooseEmCpuVers();
		ChooseInitFullScreen();
		ChooseVarFullScreen();
		ChooseInitBackground();
		ChooseInitAutoSlow();
		ChooseInitSpeed();
		ChooseMiscEmHardware();
		ChooseSonySupportTags();
		ChooseSonyWantChecksumsUpdated();
		ChooseSonySupportDC42();
		ChooseWantDisasm();
		ChooseDbgLogHAVE();
		ChooseTimingAccuracy();
		ChooseRomSize();
		ChooseTotMemSize();
		ChooseUseAsm68k();
		ChooseMouseMotion();
	}

	return err;
}
