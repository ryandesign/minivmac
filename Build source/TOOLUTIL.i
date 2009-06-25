/*
	TOOLUTIL.i
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
	mpw TOOL UTILilities
*/


GLOBALFUNC OSErr PathArgToDirName(char *path,
	MyDir_R *d, ps3p s)
{
	OSErr err;
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
							err = MyResolveNamedChildDir0(d, s, d);
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

GLOBALFUNC blnr PathArgToNewFileNameDir(char *path,
	MyDir_R *d, ps3p s)
{
	return CheckSysErr(PathArgToDirName(path, d, s));
}

GLOBALFUNC OSErr PathArgToOldDir(char *path, MyDir_R *d)
{
	OSErr err;
	MyPStr s;

	err = PathArgToDirName(path, d, s);
	if (noErr == err) {
		err = MyResolveNamedChildDir0(d, s, d);
	}

	return err;
}

GLOBALFUNC blnr PathArgToOldDir_v0(char *path, MyDir_R *d)
{
	return CheckSysErr(PathArgToOldDir(path, d));
}

GLOBALFUNC OSErr PathArgToOldNameDir_v2(char *path,
	MyDir_R *d, ps3p s)
{
	OSErr err;

	if (noErr == (err = PathArgToDirName(path, d, s)))
	if (noErr == (err = MyResolveIfAlias(d, s)))
	{
	}

	return err;
}

GLOBALFUNC blnr PathArgToOldNameDir(char *path,
	MyDir_R *d, ps3p s)
{
	return CheckSysErr(PathArgToOldNameDir_v2(path, d, s));
}

GLOBALPROC ToolReportAnySavedError(int argc, char **argv)
{
#pragma unused(argc)
	if (SavedSysErr != noErr) {
		char *msg = GetTextForSavedSysErr();

		fprintf(stderr, "Error in %s: %s\n", argv[0], msg);
	}
}
