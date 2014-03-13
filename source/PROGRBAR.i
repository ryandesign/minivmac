/*
	PROGRBAR.i
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
	PROGRess BAR
*/


LOCALVAR uimr ProgressBarMax = 0;
LOCALVAR uimr ProgressBarVal;
LOCALVAR short ProgressBarWidth;

LOCALVAR xbh_r ProgressBar_curtext = xbh_ZapVal;
LOCALVAR xbh_r ProgressBar_newtext = xbh_ZapVal;
LOCALVAR blnr ProgressBar_wantnewtext;

LOCALVAR WindowRef ProgressBar_Wind = NULL;
LOCALVAR Rect ProgressBar_Rect;

GLOBALFUNC tMyErr ProgressBar_Init_v2(WindowRef w, Rect *r)
{
	tMyErr err;

	ProgressBar_Wind = w;
	ProgressBar_Rect = *r;
	if (noErr == (err = xbh_Init_v2(0, &ProgressBar_curtext)))
	if (noErr == (err = xbh_Init_v2(0, &ProgressBar_newtext)))
	{
		/* ok */
	}

	return err;
}

GLOBALPROC ProgressBar_UnInit(void)
{
	xbh_UnInit(&ProgressBar_curtext);
	xbh_UnInit(&ProgressBar_newtext);
	ProgressBar_Wind = NULL;
}

LOCALPROC ProgressBar_SwapText(void)
{
	xbh_r t;

	MyMoveBytes((MyPtr)&ProgressBar_curtext,
		(MyPtr)&t, sizeof(xbh_r));
	MyMoveBytes((MyPtr)&ProgressBar_newtext,
		(MyPtr)&ProgressBar_curtext, sizeof(xbh_r));
	MyMoveBytes((MyPtr)&t,
		(MyPtr)&ProgressBar_newtext, sizeof(xbh_r));
}

GLOBALFUNC tMyErr ProgressBar_SetTextCStr_v2(char *s)
{
	tMyErr err;

	if (noErr == (err = xbh_SetFromCStr(&ProgressBar_newtext, s)))
	{
		ProgressBar_wantnewtext = trueblnr;
	}

	return err;
}

LOCALPROC ProgressBar_DrawText(void)
{
	RgnHandle SaveRgn;
	RgnHandle ClippedRgn;

	if (0 != xbh_GetLen(&ProgressBar_curtext)) {
		SaveRgn = NewRgn();
		if (NULL != SaveRgn) {
			GetClip(SaveRgn);
			ClippedRgn = NewRgn();
			if (NULL != ClippedRgn) {
				RectRgn(ClippedRgn, &ProgressBar_Rect);
				SectRgn(SaveRgn, ClippedRgn, ClippedRgn);
				SetClip(ClippedRgn);

				MoveTo(ProgressBar_Rect.left + 8,
					(ProgressBar_Rect.bottom + ProgressBar_Rect.top)
						/ 2 + 4);
				HLock(xbh_GetH(&ProgressBar_curtext));
				DrawText(*xbh_GetH(&ProgressBar_curtext),
					0, xbh_GetLen(&ProgressBar_curtext));
				HUnlock(xbh_GetH(&ProgressBar_curtext));

				DisposeRgn(ClippedRgn);
			}
			SetClip(SaveRgn);
			DisposeRgn(SaveRgn);
		}
	}
}

GLOBALPROC UpdateProgressBar(void)
{
	short OldWidth;
	blnr wChanged;
	GrafPtr saveport;
	Rect tempRect;

	OldWidth = ProgressBarWidth;
	if (0 == ProgressBarMax) {
		ProgressBarWidth = 0;
	} else {
		fp_extended fV;
		fp_extended fProgressBarMax;
		fp_extended fw;

		fp_fromSi5r(ProgressBarVal, &fV);
		fp_fromSi5r(ProgressBarMax, &fProgressBarMax);
		fp_fromSi5r(ProgressBar_Rect.right - ProgressBar_Rect.left,
			&fw);
		fp_div(&fProgressBarMax, &fV);
		fp_mul(&fw, &fV);
		ProgressBarWidth = fp_toSi5rDown(&fV);
	}
	wChanged = (ProgressBarWidth != OldWidth);

	if (ProgressBar_wantnewtext || wChanged) {
		GetPort(&saveport);
		SetPortWindowPort(ProgressBar_Wind);
		if (ProgressBar_wantnewtext) {
			short SavetxMode = ProgressBar_Wind->txMode;
			TextMode(srcXor);
			TextSize(9);
			ProgressBar_DrawText();
			ProgressBar_SwapText();
			ProgressBar_DrawText();
			TextMode(SavetxMode);
			ProgressBar_wantnewtext = falseblnr;
		}
		if (wChanged) {
			tempRect = ProgressBar_Rect;
			if (ProgressBarWidth < OldWidth) {
				tempRect.right = tempRect.left + OldWidth;
				tempRect.left += ProgressBarWidth;
			} else {
				/* normal case */
				tempRect.right = tempRect.left + ProgressBarWidth;
				tempRect.left += OldWidth;
			}
			InvertRect(&tempRect);
		}
		SetPort(saveport);
	}
}

GLOBALPROC ProgressBar_Draw(void)
{
	GrafPtr saveport;
	Rect tempRect;

	GetPort(&saveport);
	SetPortWindowPort(ProgressBar_Wind);

	EraseRect(&ProgressBar_Rect);

	TextSize(9);
	ProgressBar_DrawText();

	if (ProgressBarWidth != 0) {
		tempRect = ProgressBar_Rect;
		tempRect.right = tempRect.left + ProgressBarWidth;
		InvertRect(&tempRect);
	}

	SetPort(saveport);
}

GLOBALFUNC tMyErr ProgressBar_SetStage_v2(char *s, uimr n)
{
	tMyErr err;

	if (noErr == (err = ProgressBar_SetTextCStr_v2(s)))
	{
		ProgressBarVal = 0;
		ProgressBarMax = n;
	}

	return err;
}

GLOBALPROC ProgressBar_InvertAll(void)
{
	GrafPtr saveport;

	GetPort(&saveport);
	SetPortWindowPort(ProgressBar_Wind);
	InvertRect(&ProgressBar_Rect);
	SetPort(saveport);
}

GLOBALPROC ProgressBar_Move(Rect *r)
{
	ProgressBar_Rect = *r;
}
