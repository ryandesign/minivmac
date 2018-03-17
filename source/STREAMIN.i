/*
	STREAMIN.i
	Copyright (C) 2008 Paul C. Pratt

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
	STREAM from INput file
*/


LOCALVAR ui3r strmi_peek;
LOCALVAR blnr strmi_neof = falseblnr;

LOCALVAR short strmi_ref = NotAfileRef;
LOCALVAR Handle strmi_h = nullpr;

#define strmi_bufl2sz 14
#define strmi_bufsz PowOf2(strmi_bufl2sz)

LOCALVAR uimr strmi_CL = 0;
LOCALVAR uimr strmi_CU = 0;
LOCALVAR uimr strmi_Mx = 0;
LOCALVAR uimr strmi_I = 0;
LOCALVAR tMyErr strmi_err = noErr;

GLOBALFUNC tMyErr strmi_close_v2(void)
{
	/*
		May be called even if strmi_open
		fails or never called at all.
	*/

	tMyErr err = strmi_err;

	strmi_err = noErr;

	if (NotAfileRef != strmi_ref) {
		err = ErrCombine(err, MyCloseFile_v2(strmi_ref));
		strmi_ref = NotAfileRef;
	}

	err = ErrCombine(err, NHandleClear_v2(&strmi_h));

	strmi_I = 0;
	strmi_CL = 0;
	strmi_CU = 0;
	strmi_Mx = 0;
	strmi_neof = falseblnr;

	return err;
}

LOCALFUNC blnr strmi_loadcontaining(void)
{
	tMyErr err;
	blnr IsOK = falseblnr;

	if ((noErr == strmi_err) && (strmi_I < strmi_Mx)) {
		strmi_CL = FloorPow2Mult(strmi_I, strmi_bufl2sz);
		strmi_CU = strmi_CL + strmi_bufsz;
		if (strmi_CU > strmi_Mx) {
			strmi_CU = strmi_Mx;
		}
		err = MyBackReadBytes_v2(strmi_ref, strmi_CL,
			(MyPtr)*strmi_h, strmi_CU - strmi_CL);
		if (noErr != err) {
			strmi_err = err;
		} else {
			IsOK = trueblnr;
		}
	}

	return IsOK;
}

LOCALPROC strmi_advance(void)
{
	if (strmi_I >= strmi_CU) {
		if (! strmi_loadcontaining()) {
			strmi_neof = falseblnr;
			return;
		}
	}
	strmi_peek = ((ui3b *)*strmi_h)[strmi_I - strmi_CL];
	strmi_I += 1;
}

GLOBALFUNC tMyErr strmi_open_v2(MyDir_R *d, ps3p s, blnr *foundit)
{
	/*
		If file not found, will still return true and
		operate normally, as if reading zero length
		file. But will set *foundit to false, which
		can be ignored if the distinction doesn't matter.
		Caller can set foundit to nullpr when don't care.
		If not *foundit, calling strmi_close is optional.
	*/
	tMyErr err;

	if (NotAfileRef != strmi_ref) {
		/*
			MyAlertFromCStr(
				"program bug - strmi_open may not be nested");
		*/
		err = kMyErrProgramBug;
	} else {
		err = MyFileOpen_v2(d, s, (char)fsRdPerm, &strmi_ref);
		if (fnfErr == err) {
			if (nullpr != foundit) {
				*foundit = falseblnr;
			}
			err = noErr;
		} else if (noErr == err) {
			if (noErr == (err = MyHandleNew_v2(strmi_bufsz, &strmi_h)))
			if (noErr == (err = MyOpenFileGetEOF_v2(strmi_ref,
				&strmi_Mx)))
			{
				strmi_neof = trueblnr;
				strmi_advance();
				err = strmi_err;
			}

			if (noErr != err) {
				(void) strmi_close_v2();
					/* ignore any error, since already got one */
			} else {
				if (nullpr != foundit) {
					*foundit = trueblnr;
				}
			}
		}
	}

	return err;
}
