/*
	SPBLDOPT.i
	Copyright (C) 2007 Paul Pratt

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

/* option: Alternative Cpu */

LOCALVAR blnr WantAltCpu;

LOCALPROC ResetAltCpu(void)
{
	WantAltCpu = falseblnr;
}

LOCALFUNC blnr TryAsAltCpuNot(void)
{
	return FlagTryAsOptionNot("-alt-cpu", &WantAltCpu);
}

/* option: ide version */

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

/* option: sound */

LOCALVAR blnr WantInitMagnify;

LOCALPROC ResetInitMagnify(void)
{
	WantInitMagnify = nanblnr;
}

LOCALFUNC blnr TryAsInitMagnifyNot(void)
{
	return BooleanTryAsOptionNot("-magnify", &WantInitMagnify);
}

/* ------ */

LOCALPROC SPResetCommandLineParameters(void)
{
	ResetModelOption();
	ResetNumDrivesOption();
	ResetSoundOption();
	ResetAltCpu();
	ResetEmCpuVersOption();
	ResetInitFullScreen();
	ResetInitMagnify();
}

LOCALFUNC blnr TryAsSPOptionNot(void)
{
	if (TryAsModelOptionNot())
	if (TryAsNumDrivesOptionNot())
	if (TryAsSoundOptionNot())
	if (TryAsAltCpuNot())
	if (TryAsEmCpuVersOptionNot())
	if (TryAsInitFullScreenNot())
	if (TryAsInitMagnifyNot())
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
	{
		ChooseEmCpuVers();
		return trueblnr;
	}

	return falseblnr;
}
