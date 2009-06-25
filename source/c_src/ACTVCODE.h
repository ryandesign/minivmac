/*
	ACTVCODE.h

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
	ACTiVation CODE
*/

LOCALFUNC uimr KeyFun0(uimr x, uimr y, uimr m)
{
	uimr r = x + y;

	if ((r >= m) || (r < x)) {
		r -= m;
	}

	return r;
}

LOCALFUNC uimr KeyFun1(uimr x, uimr y, uimr m)
{
	uimr r = 0;
	uimr t = x;
	uimr s = y;

	while (s > 0) {
		if (0 != (s & 1)) {
			r = KeyFun0(r, t, m);
		}
		t = KeyFun0(t, t, m);
		s >>= 1;
	}

	return r;
}

LOCALFUNC uimr KeyFun2(uimr x, uimr y, uimr m)
{
	uimr r = 1;
	uimr t = x;
	uimr s = y;

	while (s > 0) {
		if (0 != (s & 1)) {
			r = KeyFun1(r, t, m);
		}
		t = KeyFun1(t, t, m);
		s >>= 1;
	}

	return r;
}

LOCALFUNC blnr CheckActvCode(ui3p p, blnr *Trial)
{
	blnr IsOk = falseblnr;
	uimr v0 = do_get_mem_long(p);
	uimr v1 = do_get_mem_long(p + 4);

	if (v0 > KeyCon2) {
		/* v0 too big */
	} else if (v1 > KeyCon4) {
		/* v1 too big */
	} else {
		uimr t0 = KeyFun0(v0, KeyCon0, KeyCon2);
		uimr t1 = KeyFun2(KeyCon1, t0, KeyCon2);
		uimr t2 = KeyFun2(v1, KeyCon3, KeyCon4);
		uimr t3 = KeyFun0(t2, KeyCon4 - t1, KeyCon4);
		uimr t4 = KeyFun0(t3, KeyCon4 - KeyCon5, KeyCon4);
		if ((0 == (t4 >> 8)) && (t4 >= KeyCon6)) {
			*Trial = falseblnr;
			IsOk = trueblnr;
		} else if (0 == t4) {
			*Trial = trueblnr;
			IsOk = trueblnr;
		}
	}

	return IsOk;
}

/* user interface */

LOCALFUNC blnr Key2Digit(int key, ui3r *r)
{
	ui3r v;

	switch (key) {
		case MKC_0: v = 0; break;
		case MKC_1: v = 1; break;
		case MKC_2: v = 2; break;
		case MKC_3: v = 3; break;
		case MKC_4: v = 4; break;
		case MKC_5: v = 5; break;
		case MKC_6: v = 6; break;
		case MKC_7: v = 7; break;
		case MKC_8: v = 8; break;
		case MKC_9: v = 9; break;
		default:
			return falseblnr;
			break;
	}

	*r = v;
	return trueblnr;
}

#define ActvCodeMaxLen 20
LOCALVAR ui4r ActvCodeLen = 0;
LOCALVAR ui3b ActvCodeDigits[ActvCodeMaxLen];

#define ActvCodeFileLen 8

FORWARDFUNC tMacErr ActvCodeFileSave(ui3p p);
FORWARDFUNC tMacErr ActvCodeFileLoad(ui3p p);

LOCALVAR ui3b CurActvCode[ActvCodeFileLen];

LOCALPROC DoActvCodeModeKey(int key)
{
	ui3r digit;
	ui3r L;
	int i;
	blnr Trial;

	if (MKC_BackSpace == key) {
		if (ActvCodeLen > 0) {
			--ActvCodeLen;
			NeedWholeScreenDraw = trueblnr;
		}
	} else if (Key2Digit(key, &digit)) {
		if (ActvCodeLen < (ActvCodeMaxLen - 1)) {
			ActvCodeDigits[ActvCodeLen] = digit;
			++ActvCodeLen;
			NeedWholeScreenDraw = trueblnr;
			L = ActvCodeDigits[0] + (1 + 9);
			if (ActvCodeLen == L) {
				uimr v0 = 0;
				uimr v1 = 0;

				for (i = 1; i < (ActvCodeDigits[0] + 1); ++i) {
					v0 = v0 * 10 + ActvCodeDigits[i];
				}
				for (; i < ActvCodeLen; ++i) {
					v1 = v1 * 10 + ActvCodeDigits[i];
				}

				do_put_mem_long(&CurActvCode[0], v0);
				do_put_mem_long(&CurActvCode[4], v1);

				if (CheckActvCode(CurActvCode, &Trial)) {
					SpecialModeClr(SpclModeActvCode);
					NeedWholeScreenDraw = trueblnr;
					if (Trial) {
						MacMsg("Using temporary code.", "Thank you for trying Mini vMac!", falseblnr);
					} else {
						if (mnvm_noErr != ActvCodeFileSave(CurActvCode)) {
							MacMsg("Oops", "I could not save the activation code to disk.", falseblnr);
						} else {
							MacMsg("Activation succeeded.", "Thank you!", falseblnr);
						}
					}
				}
			} else if (ActvCodeLen > L) {
				--ActvCodeLen;
			}
		}
	}
}

LOCALPROC DrawCellsActvCodeModeBody(void)
{
	DrawCellsOneLineStr("Please enter your activation code:");
	DrawCellsBlankLine();

	if (0 == ActvCodeLen) {
		DrawCellsOneLineStr("?");
	} else {
		int i;
		ui3r L = ActvCodeDigits[0] + (1 + 9);

		DrawCellsBeginLine();
		for (i = 0; i < L; ++i) {
			if (0 == ((L - i) % 3)) {
				if (0 != i) {
					DrawCellAdvance(kCellSpace);
				}
			}
			if (i < ActvCodeLen) {
				DrawCellAdvance(kCellDigit0 + ActvCodeDigits[i]);
			} else if (i == ActvCodeLen) {
				DrawCellAdvance(kCellQuestion);
			} else {
				DrawCellAdvance(kCellHyphen);
			}
		}
		DrawCellsEndLine();
		if (L == ActvCodeLen) {
			DrawCellsBlankLine();
			DrawCellsOneLineStr("Sorry, this is not a valid activation code.");
		}
	}
	DrawCellsBlankLine();
	DrawCellsOneLineStr("If you haven;}t obtained an activation code yet, here is a temporary one:");
	DrawCellsBlankLine();
	DrawCellsOneLineStr("281 953 822 340");
}

LOCALPROC DrawActvCodeMode(void)
{
	DrawSpclMode0("Activation Code", DrawCellsActvCodeModeBody);
}

LOCALPROC ClStrAppendHexNib(int *L0, ui3b *r, ui3r v)
{
	if (v < 10) {
		ClStrAppendChar(L0, r, kCellDigit0 + v);
	} else {
		ClStrAppendChar(L0, r, kCellUpA + (v - 10));
	}
}

LOCALPROC ClStrAppendHexByte(int *L0, ui3b *r, ui3r v)
{
	ClStrAppendHexNib(L0, r, (v >> 4) & 0x0F);
	ClStrAppendHexNib(L0, r, v & 0x0F);
}

LOCALPROC ClStrAppendHexWord(int *L0, ui3b *r, ui4r v)
{
	ClStrAppendHexByte(L0, r, (v >> 8) & 0xFF);
	ClStrAppendHexByte(L0, r, v & 0xFF);
}

LOCALPROC ClStrAppendHexLong(int *L0, ui3b *r, ui5r v)
{
	ClStrAppendHexWord(L0, r, (v >> 16) & 0xFFFF);
	ClStrAppendHexWord(L0, r, v & 0xFFFF);
}

LOCALPROC CopyActvInfo(void)
{
	int i;
	int L0;
	int L;
	ui3b ps[ClStrMaxLength];
	tPbuf j;
	ui5r sum;

	ClStrFromSubstCStr(&L0, ps, "^v ");

	for (i = 0; i < L0; ++i) {
		ps[i] = Cell2MacAsciiMap[ps[i]];
	}
	L = L0;

	sum = 0;
	for (i = 0; i < L; ++i) {
		sum += ps[i];
		sum = (sum << 5) | ((sum >> (32 - 5)) & 0x1F);
		sum += (sum << 8);
	}

	sum &= 0x1FFFFFFF;

	sum = KeyFun0(sum, do_get_mem_long(&CurActvCode[0]), KeyCon4);

	ClStrAppendHexLong(&L, ps, sum);

	sum = KeyFun0(sum, do_get_mem_long(&CurActvCode[4]), KeyCon4);
	sum = KeyFun2(sum, KeyCon3, KeyCon4);

	ClStrAppendHexLong(&L, ps, sum);

	for (i = L0; i < L; ++i) {
		ps[i] = Cell2MacAsciiMap[ps[i]];
	}

	if (mnvm_noErr == PbufNew(L, &j)) {
		PbufTransfer(ps, j, 0, L, trueblnr);
		HTCEexport(j);
	}
}

LOCALFUNC blnr ActvCodeInit(void)
{
	blnr Trial;

	if ((mnvm_noErr != ActvCodeFileLoad(CurActvCode))
		|| (! CheckActvCode(CurActvCode, &Trial))
		|| Trial
		)
	{
		SpecialModeSet(SpclModeActvCode);
		NeedWholeScreenDraw = trueblnr;
	}

	return trueblnr;
}
