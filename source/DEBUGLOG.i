/*
	DEBUGLOG.i
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
	DEBUGging LOG to Text file
*/


#ifndef Support64kROM
#define Support64kROM 1
#endif

#define Have64kROM() (LMGetROM85() < 0)

#define dbglog_buflnsz 14
#define dbglog_bufsz ((uimr)1 << dbglog_buflnsz)
#define dbglog_bszmsk (dbglog_bufsz - 1)

LOCALVAR short dbglog_bufref = -1;
LOCALVAR Handle dbglog_bufh = nullpr;
LOCALVAR tMyErr dbglog_err = noErr;
LOCALVAR uimr dbglog_bufpos;

LOCALPROC dbglog_write0(MyPtr p, uimr L)
{
	ParamBlockRec r;

	if (noErr == dbglog_err) {
		r.ioParam.ioCompletion = NULL;
		r.ioParam.ioRefNum = dbglog_bufref;
		r.ioParam.ioBuffer = (Ptr)p;
		r.ioParam.ioReqCount = L;
		r.ioParam.ioPosMode = (short) fsFromMark;
		r.ioParam.ioPosOffset = 0;

		dbglog_err = PBWriteSync(&r);
	}
}

GLOBALFUNC tMyErr dbglog_close(void)
{
	ParamBlockRec r;
	tMyErr err0;
	tMyErr err = dbglog_err;

	dbglog_err = noErr;

	if (-1 != dbglog_bufref) {
		if (nullpr != dbglog_bufh) {
			uimr n = dbglog_bufpos & dbglog_bszmsk;
			if (n != 0) {
				dbglog_write0((MyPtr)*dbglog_bufh, n);
			}
			DisposeHandle(dbglog_bufh);
			r.ioParam.ioCompletion = NULL;
			r.ioParam.ioRefNum = dbglog_bufref;
			r.ioParam.ioMisc = (Ptr)dbglog_bufpos;

			err0 = PBSetEOFSync(&r);
			if (noErr == err) {
				err = err0;
			}

			dbglog_bufh = nullpr;
		}

		r.ioParam.ioCompletion = NULL;
		r.ioParam.ioRefNum = dbglog_bufref;
		err0 = PBCloseSync(&r);
		if (noErr == err) {
			err = err0;
		}
		dbglog_bufref = -1;
	}

	return err;
}

GLOBALFUNC tMyErr dbglog_open(void)
{
	tMyErr err;
	WDPBRec wr;
	short AppVRefNum;
	long AppDirId;
	HParamBlockRec r;
	const StringPtr log_name = (StringPtr)"\pdbglog";

	wr.ioCompletion = NULL;
	wr.ioNamePtr = NULL;
#if Support64kROM
	if (Have64kROM()) {
		err = PBGetVolSync((ParamBlockRec *)&wr);
		AppVRefNum = wr.ioVRefNum;
		AppDirId = 0;
	} else
#endif
	{
		err = PBHGetVolSync(&wr);
		AppVRefNum = wr.ioWDVRefNum;
		AppDirId = wr.ioWDDirID;
	}

	if (noErr == err) {

l_retry:
		r.ioParam.ioCompletion = NULL;
		r.ioParam.ioNamePtr = log_name;
		r.ioParam.ioVRefNum = AppVRefNum;
		r.ioParam.ioPermssn = (char)fsWrPerm;
		r.ioParam.ioMisc = 0; /* use volume buffer */
		r.ioParam.ioVersNum = 0; /* needed if MFS volume */

#if Support64kROM
		if (Have64kROM()) {
			err = PBOpenSync((ParamBlockRec *)&r);
		} else
#endif
		{
			r.fileParam.ioDirID = AppDirId;
			err = PBHOpenSync(&r);
		}

		if (noErr == err) {
			dbglog_bufref = r.ioParam.ioRefNum;
			/*
				Don't change *refnum unless file opened,
				so can initialize to NotAfileRef, and
				compare later before closing in uninit.
			*/
		}

		if (fnfErr == err) {
			r.fileParam.ioFVersNum = 0; /* needed if MFS volume */

#if Support64kROM
			if (Have64kROM()) {
				err = PBCreateSync((ParamBlockRec *)&r);
			} else
#endif
			{
				r.fileParam.ioDirID = AppDirId;
				err = PBHCreateSync(&r);
			}

			if (noErr == err) {
				r.fileParam.ioFDirIndex = (short)0;

#if Support64kROM
				if (Have64kROM()) {
					err = PBGetFInfoSync((ParamBlockRec *)&r);
				} else
#endif
				{
					r.fileParam.ioDirID = AppDirId;
					err = PBHGetFInfoSync(&r);
				}

				if (noErr == err) {
					r.fileParam.ioFlFndrInfo.fdType = 'TEXT';
					r.fileParam.ioFlFndrInfo.fdCreator = 'MPS ';

#if Support64kROM
					if (Have64kROM()) {
						err = PBSetFInfoSync((ParamBlockRec *)&r);
					} else
#endif
					{
						r.fileParam.ioDirID = AppDirId;
						err = PBHSetFInfoSync(&r);
					}
				}

				goto l_retry;
			}
		}

		if (noErr == err) {
			dbglog_bufh = NewHandle(dbglog_bufsz);
			if (NULL == dbglog_bufh) {
				err = MemError();
				if (noErr == err) {
					err = memFullErr;
				}
			} else {
				dbglog_bufpos = 0;
			}

			if (noErr != err) {
				r.ioParam.ioCompletion = NULL;
				r.ioParam.ioRefNum = dbglog_bufref;
				(void) PBCloseSync((ParamBlockRec *)&r);
					/* ignore any error, since already got one */
				dbglog_bufref = -1;
			}
		}
	}

	return err;
}

LOCALPROC dbglog_writeByte(ui3r v)
{
	uimr bufposmod = dbglog_bufpos & dbglog_bszmsk;

	((ui3p)*dbglog_bufh)[bufposmod] = v;
	if (0 == ((bufposmod + 1) & dbglog_bszmsk)) {
		dbglog_write0((MyPtr)*dbglog_bufh, dbglog_bufsz);
	}
	++dbglog_bufpos;
}

LOCALPROC dbglog_write(MyPtr p, uimr L)
{
	uimr r;
	uimr bufposmod;
	uimr curbufdiv;
	uimr newbufpos = dbglog_bufpos + L;
	uimr newbufdiv = newbufpos >> dbglog_buflnsz;

label_retry:
	curbufdiv = dbglog_bufpos >> dbglog_buflnsz;
	bufposmod = dbglog_bufpos & dbglog_bszmsk;
	if (newbufdiv != curbufdiv) {
		r = dbglog_bufsz - bufposmod;
		BlockMove((void *)p,
			(void *)((MyPtr)*dbglog_bufh + bufposmod), r);
		dbglog_write0((MyPtr)*dbglog_bufh, dbglog_bufsz);
		L -= r;
		p += r;
		dbglog_bufpos += r;
		goto label_retry;
	}
	BlockMove((void *)p, (void *)((MyPtr)*dbglog_bufh + bufposmod), L);
	dbglog_bufpos = newbufpos;
}

LOCALPROC dbglog_writePStr(ps3p s)
{
	dbglog_write((MyPtr)&s[1], s[0]);
}

LOCALPROC dbglog_writeCStr(char *s)
{
	char *p = s;

	while (*p++ != 0) {
	}

	dbglog_write((MyPtr)s, p - s - 1);
}

LOCALPROC dbglog_writeReturn(void)
{
	dbglog_writeByte(13);
}

#define dbglog_writeChar(x) dbglog_writeByte((ui3r)(x))

LOCALPROC dbglog_writeNum(simr v)
{
	MyPStr s;

	NumToString(v, s);
	dbglog_writePStr(s);
}

LOCALPROC dbglog_writeSpace(void)
{
	dbglog_writeChar(' ');
}

LOCALPROC dbglog_writeln(char *s)
{
	dbglog_writeCStr(s);
	dbglog_writeReturn();
}

LOCALPROC dbglog_writelnNum(char *s, simr v)
{
	dbglog_writeCStr(s);
	dbglog_writeSpace();
	dbglog_writeNum(v);
	dbglog_writeReturn();
}

LOCALPROC dbglog_writeHexNib(ui3r x)
{
	if (x >= 10) {
		dbglog_writeChar(x - 10 + 'A');
	} else {
		dbglog_writeChar(x + '0');
	}
}

LOCALPROC dbglog_writeHexByte(ui3r x)
{
	dbglog_writeHexNib((x >> 4) & 0x0F);
	dbglog_writeHexNib(x & 0x0F);
}

LOCALPROC debug_dump_buffer(char *s, unsigned char *p, uimr n)
{
	uimr i;

	dbglog_writeCStr(s);
	dbglog_writeCStr(" : ");

	for (i = n + 1; 0 != --i; ) {
		dbglog_writeHexByte(*p++);
	}

	dbglog_writeReturn();
}

#define Have_DEBUGLOG 1
