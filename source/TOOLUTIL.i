/*
	TOOLUTIL.i
	Copyright (C) 2009 Paul C. Pratt

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
	mpw TOOL UTILilities
*/


#ifdef Have_FILEUTIL
GLOBALFUNC tMyErr PathArgToDirName(char *path,
	MyDir_R *d, ps3p s)
{
	tMyErr err;
	char c;

	PStrClear(s);

label_1:
	c = *path++;
	if (0 == c) {
		/* no ':' anywhere in path */
		err = MyHGetDir_v2(d);
	} else if (':' != c) {
		PStrApndChar(s, c);
		goto label_1;
	} else {
		if (0 == PStrLength(s)) {
			/* path begins with ':' */
			err = MyHGetDir_v2(d);
		} else {
			/* absolute path, begins with volume */
			err = MyGetNamedVolDir(s, d);
			PStrClear(s);
		}
		if (noErr == err) {
			c = *path++;
			if (0 == c) {
				/* done , ok as is */
				err = noErr;
			} else {
label_2:
				if (':' == c) {
					/* double ':', or more */
					if (noErr == (err = MyFindParentDir(d, d))) {
						c = *path++;
						if (0 == c) {
							/* done */
							err = noErr;
						} else {
							goto label_2;
						}
					}
				} else {
label_3:
					PStrApndChar(s, c);
					c = *path++;
					if (0 == c) {
						err = noErr; /* done */
					} else if (':' != c) {
						goto label_3;
					} else {
						c = *path++;
						if (0 == c) {
							/* done , path with one trailing ':' */
							err = noErr;
						} else {
							err = MyResolveNamedChildDir_v2(d, s, d);
							if (noErr == err) {
								PStrClear(s);
								goto label_2;
							}
						}
					}
				}
			}
		}
	}

	return err;
}
#endif

#ifdef Have_FILEUTIL
GLOBALFUNC tMyErr PathArgToOldDir(char *path, MyDir_R *d)
{
	tMyErr err;
	MyPStr s;

	err = PathArgToDirName(path, d, s);
	if (noErr == err) {
		err = MyResolveNamedChildDir_v2(d, s, d);
	}

	return err;
}
#endif

#ifdef Have_FILEUTIL
GLOBALFUNC tMyErr PathArgToOldNameDir_v2(char *path,
	MyDir_R *d, ps3p s)
{
	tMyErr err;

	if (noErr == (err = PathArgToDirName(path, d, s)))
	if (noErr == (err = MyResolveIfAlias(d, s)))
	{
	}

	return err;
}
#endif

GLOBALPROC ToolReportAnySavedError_v2(tMyErr err, int argc, char **argv)
{
#pragma unused(argc)
	if (noErr != err) {
		if (kMyErrReported != err) {
			char *msg = GetTextForSavedSysErr_v2(err);

			fprintf(stderr, "Error in %s: %s\n", argv[0], msg);
		}
	}
}

LOCALVAR blnr AbortRequested = falseblnr;

LOCALFUNC void MySigHandler(int sig)
{
	AbortRequested = trueblnr;
	(void) signal (sig, MySigHandler);
		/* reinstall this signal handler */
}

LOCALVAR void (*SavedSigHandler)(int);

LOCALFUNC unsigned long LastSpinTick = 0;

LOCALPROC OccasionalSpin(void)
{
	unsigned long CurTick = LMGetTicks();

	if (CurTick != LastSpinTick) {
		SpinCursor(1);
		LastSpinTick = CurTick;
	}
}

GLOBALFUNC tMyErr CheckAbortRequested(void)
{
	OccasionalSpin();
	return AbortRequested ? kMyErrUsrAbort : noErr;
}

GLOBALVAR uimr ProgressBarVal;

#define UpdateProgressBar()

#define ProgressBar_SetStage_v2(s, n) noErr

GLOBALPROC MyAlertFromCStr(char *s)
{
	fprintf(stderr, "%s\n", s);
}

GLOBALFUNC blnr InitMyMPWtool(void)
{
	InitCursorCtl(NULL);
	SavedSigHandler = signal(SIGINT, MySigHandler);
	if (SavedSigHandler == SIG_ERR) {
		fprintf(stderr, "error installing signal handler\n");
		return falseblnr;
	} else {
		return trueblnr;
	}
}

GLOBALPROC UninitMyMPWtool(void)
{
	(void) signal(SIGINT, SavedSigHandler);
}
