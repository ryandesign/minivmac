/*
	STRMOUTT.i
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
	STREAM to OUTput Text file
*/


LOCALVAR short strmo_bufref = NotAfileRef;
LOCALVAR Handle strmo_bufh = nullpr;
LOCALVAR tMyErr strmo_err = noErr;

#define strmo_buflnsz 14
#define strmo_bufsz PowOf2(strmo_buflnsz)
LOCALVAR uimr strmo_bufpos;

LOCALPROC strmo_write0(MyPtr p, uimr L)
{
	if (noErr == strmo_err) {
		strmo_err = MyWriteBytes_v2(strmo_bufref, p, L);
	}
}

GLOBALFUNC tMyErr strmo_close_v2(void)
{
	tMyErr err = strmo_err;

	strmo_err = noErr;

	if (NotAfileRef != strmo_bufref) {
		if (nullpr != strmo_bufh) {
			uimr n = ModPow2(strmo_bufpos, strmo_buflnsz);
			if (n != 0) {
				strmo_write0((MyPtr)*strmo_bufh, n);
			}
			DisposeHandle(strmo_bufh);
			err = ErrCombine(err, MyOpenFileSetEOF_v2(strmo_bufref,
				strmo_bufpos));

			strmo_bufh = nullpr;
		}

		err = ErrCombine(err, MyCloseFile_v2(strmo_bufref));
		strmo_bufref = NotAfileRef;
	}

	return err;
}

GLOBALFUNC tMyErr strmo_open_v2(MyDir_R *d, StringPtr s)
{
	tMyErr err;

	err = MyFileOpen_v2(d, s,
			(char)fsWrPerm, &strmo_bufref);
	if (fnfErr == err) {
		err = MyOpenNewFile_v3(d, s,
			'MPS ', 'TEXT', &strmo_bufref);
	}

	if (noErr == err) {
		if (noErr == (err = MyHandleNew_v2(strmo_bufsz, &strmo_bufh))) {
			strmo_bufpos = 0;
		}
	}

	if (noErr != err) {
		(void) strmo_close_v2();
			/* ignore any error, since already got one */
	}

	return err;
}

LOCALPROC strmo_writeByte(ui3r v)
{
	uimr bufposmod = ModPow2(strmo_bufpos, strmo_buflnsz);
	((ui3p)*strmo_bufh)[bufposmod] = v;
	if (ModPow2(bufposmod + 1, strmo_buflnsz) == 0) {
		strmo_write0((MyPtr)*strmo_bufh, strmo_bufsz);
	}
	++strmo_bufpos;
}

LOCALPROC strmo_write(MyPtr p, uimr L)
{
	uimr r;
	uimr bufposmod;
	uimr curbufdiv;
	uimr newbufpos = strmo_bufpos + L;
	uimr newbufdiv = FloorDivPow2(newbufpos, strmo_buflnsz);
label_retry:
	curbufdiv = FloorDivPow2(strmo_bufpos, strmo_buflnsz);
	bufposmod = ModPow2(strmo_bufpos, strmo_buflnsz);
	if (newbufdiv != curbufdiv) {
		r = strmo_bufsz - bufposmod;
		MyMoveBytes(p, (MyPtr)*strmo_bufh + bufposmod, r);
		strmo_write0((MyPtr)*strmo_bufh, strmo_bufsz);
		L -= r;
		p += r;
		strmo_bufpos += r;
		goto label_retry;
	}
	MyMoveBytes(p, (MyPtr)*strmo_bufh + bufposmod, L);
	strmo_bufpos = newbufpos;
}

LOCALPROC strmo_writePStr(ps3p s)
{
	strmo_write(PStrToPtr(s), PStrToSize(s));
}

LOCALPROC strmo_writeCStr(char *s)
{
	strmo_write((MyPtr)s, CStrLength(s));
}

LOCALPROC strmo_writeReturn(void)
{
	strmo_writeByte(13);
}

#define strmo_writeChar(x) strmo_writeByte((ui3r)(x))

LOCALPROC strmo_writeln(ps3p s)
{
	strmo_writePStr(s);
	strmo_writeReturn();
}

#if Have_HEXCONVR
LOCALPROC strmo_Uimr2Hex(uimr v)
{
	MyPStr s;

	Uimr2Hex(v, s);
	strmo_writePStr(s);
}
#endif

#if Have_HEXCONVR
LOCALPROC strmo_Simr2Hex(simr v)
{
	if (v < 0) {
		v = - v;
		strmo_writeChar('-');
	}
	strmo_Uimr2Hex(v);
}
#endif

#if Have_HEXCONVR
LOCALPROC strmo_writeHexByte(ui3r x)
{
	strmo_writeChar(hexdv[ModPow2(x >> 4, 4)]);
	strmo_writeChar(hexdv[ModPow2(x, 4)]);
}
#endif

#if Have_HEXCONVR
LOCALPROC strmo_writeHexWord(ui4r x)
{
	strmo_writeHexByte(x >> 8);
	strmo_writeHexByte(x);
}
#endif

#if Have_HEXCONVR
LOCALPROC strmo_writeHexLong(ui4r x)
{
	strmo_writeHexWord(x >> 16);
	strmo_writeHexWord(x);
}
#endif

LOCALPROC strmo_writeNum(simr v)
{
	MyPStr s;

	NumToString(v, s);
	strmo_writePStr(s);
}

LOCALPROC strmo_writeSpace(void)
{
	strmo_writeChar(' ');
}

/* 16 spaces */
LOCALVAR const MyCharR spacesv[] = "                ";

LOCALPROC strmo_writeSpaces(int n)
{
	while (n != 0) {
		int n2 = n > 16 ? 16 : n;
		strmo_write((MyPtr)spacesv, n2);
		n -= n2;
	}
}
