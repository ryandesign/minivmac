/*
	VIAEMDEV.c

	Copyright (C) 2004 Philip Cummins, Paul C. Pratt

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
	Versatile Interface Adapter EMulated DEVice

	Emulates the VIA found in the Mac Plus.

	This code adapted from vMac by Philip Cummins.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"

#include "MYOSGLUE.h"
#include "ADDRSPAC.h"
#include "PROGMAIN.h"
#endif

#include "VIAEMDEV.h"

#define VIA_ORA_CanIn 0x80
#define VIA_ORA_CanOut 0x7F

#define VIA_ORA_FloatVal 0xEF
#define VIA_ORB_FloatVal 0xFF

#if CurEmu >= kEmuSE1M
#define VIA_ORB_CanIn 0x09
#define VIA_ORB_CanOut 0xF7
#else
#define VIA_ORB_CanIn 0x79
#define VIA_ORB_CanOut 0x87
#endif

#ifdef VIAiA0_ChangeNtfy
IMPORTPROC VIAiA0_ChangeNtfy(void);
#endif

#ifdef VIAiA1_ChangeNtfy
IMPORTPROC VIAiA1_ChangeNtfy(void);
#endif

#ifdef VIAiA2_ChangeNtfy
IMPORTPROC VIAiA2_ChangeNtfy(void);
#endif

#ifdef VIAiA3_ChangeNtfy
IMPORTPROC VIAiA3_ChangeNtfy(void);
#endif

#ifdef VIAiA4_ChangeNtfy
IMPORTPROC VIAiA4_ChangeNtfy(void);
#endif

#ifdef VIAiA5_ChangeNtfy
IMPORTPROC VIAiA5_ChangeNtfy(void);
#endif

#ifdef VIAiA6_ChangeNtfy
IMPORTPROC VIAiA6_ChangeNtfy(void);
#endif

#ifdef VIAiA7_ChangeNtfy
IMPORTPROC VIAiA7_ChangeNtfy(void);
#endif

#ifdef VIAiB0_ChangeNtfy
IMPORTPROC VIAiB0_ChangeNtfy(void);
#endif

#ifdef VIAiB1_ChangeNtfy
IMPORTPROC VIAiB1_ChangeNtfy(void);
#endif

#ifdef VIAiB2_ChangeNtfy
IMPORTPROC VIAiB2_ChangeNtfy(void);
#endif

#ifdef VIAiB3_ChangeNtfy
IMPORTPROC VIAiB3_ChangeNtfy(void);
#endif

#ifdef VIAiB4_ChangeNtfy
IMPORTPROC VIAiB4_ChangeNtfy(void);
#endif

#ifdef VIAiB5_ChangeNtfy
IMPORTPROC VIAiB5_ChangeNtfy(void);
#endif

#ifdef VIAiB6_ChangeNtfy
IMPORTPROC VIAiB6_ChangeNtfy(void);
#endif

#ifdef VIAiB7_ChangeNtfy
IMPORTPROC VIAiB7_ChangeNtfy(void);
#endif

#ifdef VIAiCB2_ChangeNtfy
IMPORTPROC VIAiCB2_ChangeNtfy(void);
#endif

typedef struct {
	ui5b T1C_F;  /* Timer 1 Counter Fixed Point */
	ui5b T2C_F;  /* Timer 2 Counter Fixed Point */
	ui3b ORB;    /* Buffer B */
	/* ui3b ORA_H;     Buffer A with Handshake */
	ui3b DDR_B;  /* Data Direction Register B */
	ui3b DDR_A;  /* Data Direction Register A */
	ui3b T1L_L;  /* Timer 1 Latch Low */
	ui3b T1L_H;  /* Timer 1 Latch High */
	ui3b T2L_L;  /* Timer 2 Latch Low */
	ui3b SR;     /* Shift Register */
	ui3b ACR;    /* Auxiliary Control Register */
	ui3b PCR;    /* Peripheral Control Register */
	ui3b IFR;    /* Interrupt Flag Register */
	ui3b IER;    /* Interrupt Enable Register */
	ui3b ORA;    /* Buffer A */
} VIA_Ty;

LOCALVAR VIA_Ty VIA;

#define kIntCA2 0 /* One_Second */
#define kIntCA1 1 /* Vertical_Blanking */
#define kIntSR 2 /* Keyboard_Data_Ready */
#define kIntCB2 3 /* Keyboard_Data */
#define kIntCB1 4 /* Keyboard_Clock */
#define kIntT2 5 /* Timer_2 */
#define kIntT1 6 /* Timer_1 */

#define Ui3rPowOf2(p) (1 << (p))
#define Ui3rTestBit(i, p) (((i) & Ui3rPowOf2(p)) != 0)

/* VIA_Get_ORA : VIA Get Port A Data */
/* This function queries VIA Port A interfaced hardware about their status */

LOCALFUNC ui3b VIA_Get_ORA(void)
{
	ui3b Selection = ~ VIA.DDR_A;
	ui3b Value = VIA.ORA & VIA.DDR_A;

#if 0
	if ((Selection & ~ VIA_ORA_CanIn) != 0) {
		ReportAbnormal("Set VIA DDR_A unexpected direction");
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanIn, 7)
	if (Ui3rTestBit(Selection, 7)) {
		Value |= (VIAiA7 << 7);
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanIn, 6)
	if (Ui3rTestBit(Selection, 6)) {
		Value |= (VIAiA6 << 6);
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanIn, 5)
	if (Ui3rTestBit(Selection, 5)) {
		Value |= (VIAiA5 << 5);
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanIn, 4)
	if (Ui3rTestBit(Selection, 4)) {
		Value |= (VIAiA4 << 4);
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanIn, 3)
	if (Ui3rTestBit(Selection, 3)) {
		Value |= (VIAiA3 << 3);
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanIn, 2)
	if (Ui3rTestBit(Selection, 2)) {
		Value |= (VIAiA2 << 2);
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanIn, 1)
	if (Ui3rTestBit(Selection, 1)) {
		Value |= (VIAiA1 << 1);
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanIn, 0)
	if (Ui3rTestBit(Selection, 0)) {
		Value |= (VIAiA0 << 0);
	}
#endif

	VIA.ORA = Value;
	return Value;
}

/* VIA_Get_ORB : VIA Get Port B Data */
/* This function queries VIA Port B interfaced hardware about their status */

LOCALFUNC ui3b VIA_Get_ORB(void)
{
	ui3b Selection = ~ VIA.DDR_B;
	ui3b Value = VIA.ORB & VIA.DDR_B;

#if 0
	if ((Selection & ~ VIA_ORB_CanIn) != 0) {
		ReportAbnormal("Set VIA DDR_A unexpected direction");
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanIn, 7)
	if (Ui3rTestBit(Selection, 7)) {
		Value |= (VIAiB7 << 7);
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanIn, 6)
	if (Ui3rTestBit(Selection, 6)) {
		Value |= (VIAiB6 << 6);
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanIn, 5)
	if (Ui3rTestBit(Selection, 5)) {
		Value |= (VIAiB5 << 5);
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanIn, 4)
	if (Ui3rTestBit(Selection, 4)) {
		Value |= (VIAiB4 << 4);
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanIn, 3)
	if (Ui3rTestBit(Selection, 3)) {
		Value |= (VIAiB3 << 3);
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanIn, 2)
	if (Ui3rTestBit(Selection, 2)) {
		Value |= (VIAiB2 << 2);
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanIn, 1)
	if (Ui3rTestBit(Selection, 1)) {
		Value |= (VIAiB1 << 1);
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanIn, 0)
	if (Ui3rTestBit(Selection, 0)) {
		Value |= (VIAiB0 << 0);
	}
#endif

	VIA.ORB = Value;
	return Value;
}

#define ViaORcheckBit(p, x) \
	(Ui3rTestBit(Selection, p) && \
	((v = (Data >> p) & 1) != x))

LOCALPROC VIA_Put_ORA(void)
{
	ui3b v;
	ui3b Data = VIA.ORA;
	ui3b Selection = VIA.DDR_A;

#if Ui3rTestBit(VIA_ORA_CanOut, 7)
	if (ViaORcheckBit(7, VIAiA7)) {
		VIAiA7 = v;
#ifdef VIAiA7_ChangeNtfy
		VIAiA7_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanOut, 6)
	if (ViaORcheckBit(6, VIAiA6)) {
		VIAiA6 = v;
#ifdef VIAiA6_ChangeNtfy
		VIAiA6_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanOut, 5)
	if (ViaORcheckBit(5, VIAiA5)) {
		VIAiA5 = v;
#ifdef VIAiA5_ChangeNtfy
		VIAiA5_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanOut, 4)
	if (ViaORcheckBit(4, VIAiA4)) {
		VIAiA4 = v;
#ifdef VIAiA4_ChangeNtfy
		VIAiA4_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanOut, 3)
	if (ViaORcheckBit(3, VIAiA3)) {
		VIAiA3 = v;
#ifdef VIAiA3_ChangeNtfy
		VIAiA3_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanOut, 2)
	if (ViaORcheckBit(2, VIAiA2)) {
		VIAiA2 = v;
#ifdef VIAiA2_ChangeNtfy
		VIAiA2_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanOut, 1)
	if (ViaORcheckBit(1, VIAiA1)) {
		VIAiA1 = v;
#ifdef VIAiA1_ChangeNtfy
		VIAiA1_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORA_CanOut, 0)
	if (ViaORcheckBit(0, VIAiA0)) {
		VIAiA0 = v;
#ifdef VIAiA0_ChangeNtfy
		VIAiA0_ChangeNtfy();
#endif
	}
#endif
}

LOCALPROC VIA_Put_ORB(void)
{
	ui3b v;
	ui3b Data = VIA.ORB;
	ui3b Selection = VIA.DDR_B;

#if Ui3rTestBit(VIA_ORB_CanOut, 7)
	if (ViaORcheckBit(7, VIAiB7)) {
		VIAiB7 = v;
#ifdef VIAiB7_ChangeNtfy
		VIAiB7_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanOut, 6)
	if (ViaORcheckBit(6, VIAiB6)) {
		VIAiB6 = v;
#ifdef VIAiB6_ChangeNtfy
		VIAiB6_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanOut, 5)
	if (ViaORcheckBit(5, VIAiB5)) {
		VIAiB5 = v;
#ifdef VIAiB5_ChangeNtfy
		VIAiB5_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanOut, 4)
	if (ViaORcheckBit(4, VIAiB4)) {
		VIAiB4 = v;
#ifdef VIAiB4_ChangeNtfy
		VIAiB4_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanOut, 3)
	if (ViaORcheckBit(3, VIAiB3)) {
		VIAiB3 = v;
#ifdef VIAiB3_ChangeNtfy
		VIAiB3_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanOut, 2)
	if (ViaORcheckBit(2, VIAiB2)) {
		VIAiB2 = v;
#ifdef VIAiB2_ChangeNtfy
		VIAiB2_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanOut, 1)
	if (ViaORcheckBit(1, VIAiB1)) {
		VIAiB1 = v;
#ifdef VIAiB1_ChangeNtfy
		VIAiB1_ChangeNtfy();
#endif
	}
#endif

#if Ui3rTestBit(VIA_ORB_CanOut, 0)
	if (ViaORcheckBit(0, VIAiB0)) {
		VIAiB0 = v;
#ifdef VIAiB0_ChangeNtfy
		VIAiB0_ChangeNtfy();
#endif
	}
#endif
}

LOCALPROC VIA_SetDDR_A(ui3b Data)
{
	ui3b floatbits = VIA.DDR_A & ~ Data;
	ui3b unfloatbits = Data & ~ VIA.DDR_A;

	if (floatbits != 0) {
		VIA.ORA = (VIA.ORA & ~ floatbits)
			| (VIA_ORA_FloatVal & floatbits);
		VIA_Put_ORA();
	}
	VIA.DDR_A = Data;
	if (unfloatbits != 0) {
		VIA_Put_ORA();
	}
	if ((Data & ~ VIA_ORA_CanOut) != 0) {
		ReportAbnormal("Set VIA DDR_A unexpected direction");
	}
}

LOCALPROC VIA_SetDDR_B(ui3b Data)
{
	ui3b floatbits = VIA.DDR_B & ~ Data;
	ui3b unfloatbits = Data & ~ VIA.DDR_B;

	if (floatbits != 0) {
		VIA.ORB = (VIA.ORB & ~ floatbits)
			| (VIA_ORB_FloatVal & floatbits);
		VIA_Put_ORB();
	}
	VIA.DDR_B = Data;
	if (unfloatbits != 0) {
		VIA_Put_ORB();
	}
	if ((Data & ~ VIA_ORB_CanOut) != 0) {
		ReportAbnormal("Set VIA DDR_B unexpected direction");
	}
}


LOCALPROC CheckVIAInterruptFlag(void)
{
	ui3b NewVIAInterruptRequest = ((VIA.IFR & VIA.IER) != 0) ? 1 : 0;

	if (NewVIAInterruptRequest != VIAInterruptRequest) {
		VIAInterruptRequest = NewVIAInterruptRequest;
#ifdef VIAinterruptChngNtfy
		VIAinterruptChngNtfy();
#endif
	}
}


LOCALVAR ui3b T1_Active = 0;
LOCALVAR ui3b T2_Active = 0;

LOCALVAR blnr T1IntReady = falseblnr;

LOCALPROC VIA_Clear(void)
{
	VIA.ORA   = 0; VIA.DDR_A = 0;
	VIA.ORB   = 0; VIA.DDR_B = 0;
	VIA.T1L_L = VIA.T1L_H = 0x00;
	VIA.T2L_L = 0x00;
	VIA.T1C_F = 0;
	VIA.T2C_F = 0;
	VIA.SR = VIA.ACR = 0x00;
	VIA.PCR   = VIA.IFR   = VIA.IER   = 0x00;
	T1_Active = T2_Active = 0x00;
	T1IntReady = falseblnr;
}

GLOBALPROC VIA_Zap(void)
{
	VIA_Clear();
	VIAInterruptRequest = 0;
}

GLOBALPROC VIA_Reset(void)
{
	VIA_SetDDR_A(0);
	VIA_SetDDR_B(0);

	VIA_Clear();

	CheckVIAInterruptFlag();
}

LOCALPROC SetVIAInterruptFlag(ui3b VIA_Int)
{
	VIA.IFR |= ((ui3b)1 << VIA_Int);
	CheckVIAInterruptFlag();
}

LOCALPROC ClrVIAInterruptFlag(ui3b VIA_Int)
{
	VIA.IFR &= ~ ((ui3b)1 << VIA_Int);
	CheckVIAInterruptFlag();
}

#ifdef _VIA_Debug
#include <stdio.h>
#endif

GLOBALPROC VIA_ShiftInData(ui3b v)
{
	ui3b ShiftMode = (VIA.ACR & 0x1C) >> 2;
	if (ShiftMode != 3) {
		if (ShiftMode == 0) {
			/* happens on reset */
		} else {
			ReportAbnormal("VIA Not ready to shift in");
		}
	} else {
		VIA.SR = v;
		SetVIAInterruptFlag(kIntSR);
	}
}

GLOBALFUNC ui3b VIA_ShiftOutData(void)
{
	if (((VIA.ACR & 0x1C) >> 2) != 7) {
		ReportAbnormal("VIA Not ready to shift out");
		return 0;
	} else {
		SetVIAInterruptFlag(kIntSR);
		VIAiCB2 = (VIA.SR & 1);
		return VIA.SR;
	}
}

#define TimerTicksPerTick (704UL * 370 / 20)
#define TimerTicksPerSubTick (TimerTicksPerTick / kNumSubTicks)

#define ConvertTimeConst (0x00010000UL * TimerTicksPerTick / InstructionsPerTick)
#define ConvertTimeInv (0x00010000UL * InstructionsPerTick / TimerTicksPerTick)

LOCALVAR blnr T1Running = trueblnr;
LOCALVAR iCountt VIA_T1LastTime = 0;

GLOBALPROC VIA_DoTimer1Check(void)
{
	if (T1Running) {
		iCountt NewTime = GetCuriCount();
		iCountt deltaTime = NewTime - VIA_T1LastTime;
		if (deltaTime != 0) {
			ui5b Temp = VIA.T1C_F; /* Get Timer 1 Counter */
			ui5b deltaTemp = deltaTime * ConvertTimeConst; /* may overflow */
			ui5b NewTemp = Temp - deltaTemp;
			if ((deltaTime > ConvertTimeInv)
				|| ((Temp <= deltaTemp) && (Temp != 0)))
			{
				if ((VIA.ACR & 0x40) != 0) { /* Free Running? */
					/* Reload Counter from Latches */
					ui4b v = (VIA.T1L_H << 8) + VIA.T1L_L;
					ui4b ntrans = 1 + (v == 0) ? 0 :
						(((deltaTemp - Temp) / v) >> 16);
					NewTemp += (((ui5b)v * ntrans) << 16);
					if ((VIA.ACR & 0x80) != 0) { /* invert ? */
						if ((ntrans & 1) != 0) {
							VIAiB7 ^= 1;
#ifdef VIAiB7_ChangeNtfy
							VIAiB7_ChangeNtfy();
#endif
						}
					}
					SetVIAInterruptFlag(kIntT1);
				} else {
					if (T1_Active == 1) {
						T1_Active = 0;
						SetVIAInterruptFlag(kIntT1);
					}
				}
			}

			VIA.T1C_F = NewTemp;
			VIA_T1LastTime = NewTime;
		}

		T1IntReady = falseblnr;
		if ((VIA.IFR & (1 << kIntT1)) == 0) {
			if (((VIA.ACR & 0x40) != 0) || (T1_Active == 1)) {
				ui5b NewTemp = VIA.T1C_F; /* Get Timer 1 Counter */
				ui5b NewTimer;
#ifdef _VIA_Debug
				fprintf(stderr, "posting Timer1Check, %d, %d\n", Temp, GetCuriCount());
#endif
				if (NewTemp == 0) {
					NewTimer = ConvertTimeInv;
				} else {
					NewTimer = (((NewTemp >> 16) * ConvertTimeInv) >> 16) + 1;
				}
				ICT_add(kICT_VIA_Timer1Check, NewTimer);
				T1IntReady = trueblnr;
			}
		}
	}
}

LOCALPROC CheckT1IntReady(void)
{
	if (T1Running) {
		blnr NewT1IntReady = falseblnr;

		if ((VIA.IFR & (1 << kIntT1)) == 0) {
			if (((VIA.ACR & 0x40) != 0) || (T1_Active == 1)) {
				NewT1IntReady = trueblnr;
			}
		}

		if (T1IntReady != NewT1IntReady) {
			T1IntReady = NewT1IntReady;
			if (NewT1IntReady) {
				VIA_DoTimer1Check();
			}
		}
	}
}

GLOBALFUNC ui4b GetSoundInvertTime(void)
{
	ui4b v;

	if ((VIA.ACR & 0xC0) == 0xC0) {
		v = (VIA.T1L_H << 8) + VIA.T1L_L;
	} else {
		v = 0;
	}
	return v;
}

LOCALVAR blnr T2Running = trueblnr;
LOCALVAR blnr T2C_ShortTime = falseblnr;
LOCALVAR iCountt VIA_T2LastTime = 0;

GLOBALPROC VIA_DoTimer2Check(void)
{
	if (T2Running) {
		iCountt NewTime = GetCuriCount();
		ui5b Temp = VIA.T2C_F; /* Get Timer 2 Counter */
		iCountt deltaTime = NewTime - VIA_T2LastTime;
		ui5b deltaTemp = deltaTime * ConvertTimeConst; /* may overflow */
		ui5b NewTemp = Temp - deltaTemp;
		if (T2_Active == 1) {
			if ((deltaTime > ConvertTimeInv)
				|| ((Temp <= deltaTemp) && (Temp != 0)))
			{
				T2C_ShortTime = falseblnr;
				T2_Active = 0;
				SetVIAInterruptFlag(kIntT2);
			} else {
				ui5b NewTimer;
#ifdef _VIA_Debug
				fprintf(stderr, "posting Timer2Check, %d, %d\n", Temp, GetCuriCount());
#endif
				if (NewTemp == 0) {
					NewTimer = ConvertTimeInv;
				} else {
					NewTimer = (((NewTemp >> 16) * ConvertTimeInv) >> 16) + 1;
				}
				ICT_add(kICT_VIA_Timer2Check, NewTimer);
			}
		}
		VIA.T2C_F = NewTemp;
		VIA_T2LastTime = NewTime;
	}
}

#define kORB    0x00
#define kORA_H  0x01
#define kDDR_B  0x02
#define kDDR_A  0x03
#define kT1C_L  0x04
#define kT1C_H  0x05
#define kT1L_L  0x06
#define kT1L_H  0x07
#define kT2_L   0x08
#define kT2_H   0x09
#define kSR     0x0A
#define kACR    0x0B
#define kPCR    0x0C
#define kIFR    0x0D
#define kIER    0x0E
#define kORA    0x0F

GLOBALFUNC ui5b VIA_Access(ui5b Data, blnr WriteMem, CPTR addr)
{
	switch (addr) {
		case kORB   :
			if ((VIA.PCR & 0xE0) == 0) {
				ClrVIAInterruptFlag(kIntCB2);
			}
			if (WriteMem) {
				VIA.ORB = Data;
				VIA_Put_ORB();
			} else {
				Data = VIA_Get_ORB();
			}
			break;
		case kDDR_B :
			if (WriteMem) {
				VIA_SetDDR_B(Data);
			} else {
				Data = VIA.DDR_B;
			}
			break;
		case kDDR_A :
			if (WriteMem) {
				VIA_SetDDR_A(Data);
			} else {
				Data = VIA.DDR_A;
			}
			break;
		case kT1C_L :
			if (WriteMem) {
				VIA.T1L_L = Data;
			} else {
				ClrVIAInterruptFlag(kIntT1);
				VIA_DoTimer1Check();
				Data = (VIA.T1C_F & 0x00FF0000) >> 16;
			}
			break;
		case kT1C_H :
			if (WriteMem) {
				VIA.T1L_H = Data;
				ClrVIAInterruptFlag(kIntT1);
				VIA.T1C_F = (Data << 24) + (VIA.T1L_L << 16);
				if ((VIA.ACR & 0x40) == 0) {
					T1_Active = 1;
				}
				VIA_T1LastTime = GetCuriCount();
				VIA_DoTimer1Check();
			} else {
				VIA_DoTimer1Check();
				Data = (VIA.T1C_F & 0xFF000000) >> 24;
			}
			break;
		case kT1L_L :
			if (WriteMem) {
				VIA.T1L_L = Data;
			} else {
				Data = VIA.T1L_L;
			}
			break;
		case kT1L_H :
			if (WriteMem) {
				VIA.T1L_H = Data;
			} else {
				Data = VIA.T1L_H;
			}
			break;
		case kT2_L  :
			if (WriteMem) {
				VIA.T2L_L = Data;
			} else {
				ClrVIAInterruptFlag(kIntT2);
				VIA_DoTimer2Check();
				Data = (VIA.T2C_F & 0x00FF0000) >> 16;
			}
			break;
		case kT2_H  :
			if (WriteMem) {
				VIA.T2C_F = (Data << 24) + (VIA.T2L_L << 16);
				ClrVIAInterruptFlag(kIntT2);
				T2_Active = 1;

				if ((VIA.T2C_F < (128UL << 16))
					&& (VIA.T2C_F != 0))
				{
					T2C_ShortTime = trueblnr;
					T2Running = trueblnr;
					/*
						Running too many instructions during
						a short timer interval can crash when
						playing sounds in System 7. So
						in this case don't let timer pause.
					*/
				}
				VIA_T2LastTime = GetCuriCount();
				VIA_DoTimer2Check();
			} else {
				VIA_DoTimer2Check();
				Data = (VIA.T2C_F & 0xFF000000) >> 24;
			}
			break;
		case kSR:
#ifdef _VIA_Debug
			fprintf(stderr, "VIA.SR: %d, %d, %d\n", WriteMem, ((VIA.ACR & 0x1C) >> 2), Data);
#endif
			if (WriteMem) {
				VIA.SR = Data;
			}
			switch ((VIA.ACR & 0x1C) >> 2) {
				case 3 : /* Shifting In */
					ClrVIAInterruptFlag(kIntSR);
					break;
				case 6 : /* shift out under o2 clock */
					if ((! WriteMem) || (VIA.SR != 0)) {
						ReportAbnormal("VIA shift mode 6, non zero");
					} else {
#ifdef _VIA_Debug
						fprintf(stderr, "posting Foo2Task\n");
#endif
						if (VIAiCB2 != 0) {
							VIAiCB2 = 0;
#ifdef VIAiCB2_ChangeNtfy
							VIAiCB2_ChangeNtfy();
#endif
						}
					}
#if 0 /* possibly should do this. seems not to affect anything. */
					SetVIAInterruptFlag(kIntSR); /* don't wait */
#endif
					break;
				case 7 : /* Shifting Out */
					ClrVIAInterruptFlag(kIntSR);
					break;
			}
			if (! WriteMem) {
				Data = VIA.SR;
			}
			break;
		case kACR:
			if (WriteMem) {
#if 1
				if ((VIA.ACR & 0x10) != ((ui3b)Data & 0x10)) {
					/* shift direction has changed */
					if ((Data & 0x10) == 0) {
						/* no longer an output,
							set data to float value */
						if (VIAiCB2 == 0) {
							VIAiCB2 = 1;
#ifdef VIAiCB2_ChangeNtfy
							VIAiCB2_ChangeNtfy();
#endif
						}
					}
				}
#endif
				VIA.ACR = Data;
				if ((VIA.ACR & 0x20) != 0) { /* Not pulse counting? */
					ReportAbnormal("Set VIA ACR T2 Timer pulse counting");
				}
				switch ((VIA.ACR & 0xC0) >> 6) {
					/* case 1: happens in early System 6 */
					case 2:
						ReportAbnormal("Set VIA ACR T1 Timer mode 2");
						break;
				}
				CheckT1IntReady();
				switch ((VIA.ACR & 0x1C) >> 2) {
					case 0: /* this isn't sufficient */
						ClrVIAInterruptFlag(kIntSR);
						break;
					case 1:
					case 2:
					case 4:
					case 5:
						ReportAbnormal("Set VIA ACR shift mode 1,2,4,5");
						break;
					default:
						break;
				}
				if ((VIA.ACR & 0x03) != 0) {
					ReportAbnormal("Set VIA ACR T2 Timer latching enabled");
				}
			} else {
				Data = VIA.ACR;
			}
			break;
		case kPCR:
			if (WriteMem) {
				VIA.PCR = Data;
				if ((VIA.PCR & 0xE0) != 0) {
					ReportAbnormal("Set VIA PCR CB2 Control mode?");
				}
				if ((VIA.PCR & 0x10) != 0) {
					ReportAbnormal("Set VIA PCR CB1 INTERRUPT CONTROL?");
				}
				if ((VIA.PCR & 0x0E) != 0) {
					ReportAbnormal("Set VIA PCR CA2 INTERRUPT CONTROL?");
				}
				if ((VIA.PCR & 0x01) != 0) {
					ReportAbnormal("Set VIA PCR CA1 INTERRUPT CONTROL?");
				}
			} else {
				Data = VIA.PCR;
			}
			break;
		case kIFR:
			if (WriteMem) {
				if ((Data & 0x80) == 0) {
					VIA.IFR = VIA.IFR & ((~ Data) & 0x7F); /* Clear Enable Bits */
				} else {
					VIA.IFR = VIA.IFR | (Data & 0x7F); /* Set Enable Bits */
				}
				CheckVIAInterruptFlag();
				CheckT1IntReady();
			} else {
				Data = VIA.IFR;
				if ((VIA.IFR & VIA.IER) != 0) {
					Data |= 0x80;
				}
			}
			break;
		case kIER   :
			if (WriteMem) {
				if ((Data & 0x80) == 0) {
					VIA.IER = VIA.IER & ((~ Data) & 0x7F); /* Clear Enable Bits */
				} else {
					VIA.IER = VIA.IER | (Data & 0x7F); /* Set Enable Bits */
				}
				CheckVIAInterruptFlag();
				if ((VIA.IER & (1 << 1)) == 0) {
#if TempDebug && (CurEmu >= kEmuSE1M)
#else
					ReportAbnormal("IER ~1");
#endif
				}
				if ((VIA.IER & (1 << 3)) != 0) {
					ReportAbnormal("IER 3");
				}
				if ((VIA.IER & (1 << 4)) != 0) {
					ReportAbnormal("IER 4");
				}
			} else {
				Data = VIA.IER | 0x80;
			}
			break;
		case kORA   :
		case kORA_H :
			if ((VIA.PCR & 0xE) == 0) {
				ClrVIAInterruptFlag(kIntCA2);
			}
			if (WriteMem) {
				VIA.ORA = Data;
				VIA_Put_ORA();
			} else {
				Data = VIA_Get_ORA();
			}
			break;
	}
	return Data;
}

GLOBALPROC VIA_ExtraTimeBegin(void)
{
	if (T1Running) {
		VIA_DoTimer1Check(); /* run up to this moment */
		T1Running = falseblnr;
	}
	if (T2Running & (! T2C_ShortTime)) {
		VIA_DoTimer2Check(); /* run up to this moment */
		T2Running = falseblnr;
	}
}

GLOBALPROC VIA_ExtraTimeEnd(void)
{
	if (! T1Running) {
		T1Running = trueblnr;
		VIA_T1LastTime = GetCuriCount();
		VIA_DoTimer1Check();
	}
	if (! T2Running) {
		T2Running = trueblnr;
		VIA_T2LastTime = GetCuriCount();
		VIA_DoTimer2Check();
	}
}

/* VIA Interrupt Interface */

GLOBALPROC VIA_Int_Vertical_Blanking(void)
{
	SetVIAInterruptFlag(kIntCA1);
}

GLOBALPROC VIA_Int_One_Second(void)
{
	SetVIAInterruptFlag(kIntCA2);
}
