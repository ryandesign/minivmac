/*
	VIAEMDEV.c

	Copyright (C) 2004 Philip Cummins, Paul Pratt

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

#include "MINEM68K.h"
#include "ADDRSPAC.h"
#include "PROGMAIN.h"
#include "MYOSGLUE.h"
#endif

#include "VIAEMDEV.h"

IMPORTPROC Keyboard_Put(ui3b in);

IMPORTFUNC ui3b VIA_GORA7(void);  /* SCC */
IMPORTPROC VIA_PORA7(ui3b Data);

IMPORTFUNC ui3b VIA_GORA6(void); /* Main/Alternate Screen Buffer */
IMPORTPROC VIA_PORA6(ui3b Data);

IMPORTFUNC ui3b VIA_GORA5(void); /* Floppy Disk Line SEL */
IMPORTPROC VIA_PORA5(ui3b Data);

IMPORTFUNC ui3b VIA_GORA4(void); /* Overlay/Normal Memory Mapping */
IMPORTPROC VIA_PORA4(ui3b Data);

IMPORTFUNC ui3b VIA_GORA3(void); /* Main/Alternate Sound Buffer */
IMPORTPROC VIA_PORA3(ui3b Data);

IMPORTFUNC ui3b VIA_GORA2(void); /* Sound Volume Bit 2 */
IMPORTPROC VIA_PORA2(ui3b Data);

IMPORTFUNC ui3b VIA_GORA1(void); /* Sound Volume Bit 1 */
IMPORTPROC VIA_PORA1(ui3b Data);

IMPORTFUNC ui3b VIA_GORA0(void); /* Sound Volume Bit 0 */
IMPORTPROC VIA_PORA0(ui3b Data);

IMPORTFUNC ui3b VIA_GORB7(void); /* Sound Enable */
IMPORTPROC VIA_PORB7(ui3b Data);

IMPORTFUNC ui3b VIA_GORB6(void); /* Video Beam in Display */
IMPORTPROC VIA_PORB6(ui3b Data);

IMPORTFUNC ui3b VIA_GORB5(void); /* Mouse Y2 */
IMPORTPROC VIA_PORB5(ui3b Data);

IMPORTFUNC ui3b VIA_GORB4(void); /* Mouse X2 */
IMPORTPROC VIA_PORB4(ui3b Data);

IMPORTFUNC ui3b VIA_GORB3(void); /* Mouse Button */
IMPORTPROC VIA_PORB3(ui3b Data);

IMPORTFUNC ui3b VIA_GORB2(void); /* RTC Enable */
IMPORTPROC VIA_PORB2(ui3b Data);

IMPORTFUNC ui3b VIA_GORB1(void); /* RTC Data Clock */
IMPORTPROC VIA_PORB1(ui3b Data);

IMPORTFUNC ui3b VIA_GORB0(void); /* RTC Data */
IMPORTPROC VIA_PORB0(ui3b Data);

/* VIA_Get_ORA : VIA Get Port A Data */
/* This function queries VIA Port A interfaced hardware about their status */

LOCALFUNC ui3b VIA_Get_ORA(ui3b Selection)
{
	ui3b Value = 0;

	if ((Selection & 0x80) == 0) { /* SCC Wait/Request */
		Value |= (VIA_GORA7() << 7);
	}

	if ((Selection & 0x40) == 0) { /* Main/Alternate Screen Buffer */
		Value |= (VIA_GORA6() << 6);
	}

	if ((Selection & 0x20) == 0) { /* Floppy Disk Line SEL */
		Value |= (VIA_GORA5() << 5);
	}

	if ((Selection & 0x10) == 0) { /* Overlay/Normal Memory Mapping */
		Value |= (VIA_GORA4() << 4);
	}

	if ((Selection & 0x08) == 0) { /* Main/Alternate Sound Buffer */
		Value |= (VIA_GORA3() << 3);
	}

	if ((Selection & 0x04) == 0) { /* Sound Volume Bit 2 */
		Value |= (VIA_GORA2() << 2);
	}

	if ((Selection & 0x02) == 0) { /* Sound Volume Bit 1 */
		Value |= (VIA_GORA1() << 1);
	}

	if ((Selection & 0x01) == 0) { /* Sound Volume Bit 0 */
		Value |= VIA_GORA0();
	}

	return Value;
}

/* VIA_Get_ORB : VIA Get Port B Data */
/* This function queries VIA Port B interfaced hardware about their status */

LOCALFUNC ui3b VIA_Get_ORB(ui3b Selection)
{
	ui3b Value = 0;

	if ((Selection & 0x80) == 0) { /* Sound Enable */
		Value |= (VIA_GORB7() << 7);
	}

	if ((Selection & 0x40) == 0) { /* Video Beam in Display */
		Value |= (VIA_GORB6() << 6);
	}

	if ((Selection & 0x20) == 0) { /* Mouse Y2 */
		Value |= (VIA_GORB5() << 5);
	}

	if ((Selection & 0x10) == 0) { /* Mouse X2 */
		Value |= (VIA_GORB4() << 4);
	}

	if ((Selection & 0x08) == 0) { /* Mouse Button */
		Value |= (VIA_GORB3() << 3);
	}

	if ((Selection & 0x04) == 0) { /* RTC Enable */
		Value |= (VIA_GORB2() << 2);
	}

	if ((Selection & 0x02) == 0) { /* RTC Data Clock */
		Value |= (VIA_GORB1() << 1);
	}

	if ((Selection & 0x01) == 0) { /* RTC Data */
		Value |= VIA_GORB0();
	}

	return Value;
}

LOCALPROC VIA_Put_ORA(ui3b Selection, ui3b Data)
{
	if ((Selection & 0x80) != 0) { /* SCC Wait/Request */
		VIA_PORA7((Data & 0x80) >> 7);
	}

	if ((Selection & 0x40) != 0) { /* Main/Alternate Screen Buffer */
		VIA_PORA6((Data & 0x40) >> 6);
	}

	if ((Selection & 0x20) != 0) { /* Floppy Disk Line SEL */
		VIA_PORA5((Data & 0x20) >> 5);
	}

	if ((Selection & 0x10) != 0) { /* Overlay/Normal Memory Mapping */
		VIA_PORA4((Data & 0x10) >> 4);
	}

	if ((Selection & 0x08) != 0) { /* Main/Alternate Sound Buffer */
		VIA_PORA3((Data & 0x08) >> 3);
	}

	if ((Selection & 0x04) != 0) { /* Sound Volume Bit 2 */
		VIA_PORA2((Data & 0x04) >> 2);
	}

	if ((Selection & 0x02) != 0) { /* Sound Volume Bit 1 */
		VIA_PORA1((Data & 0x02) >> 1);
	}

	if ((Selection & 0x01) != 0) { /* Sound Volume Bit 0 */
		VIA_PORA0(Data & 0x01);
	}
}

LOCALPROC VIA_Put_ORB(ui3b Selection, ui3b Data)
{
	if ((Selection & 0x80) != 0) { /* Sound Enable */
		VIA_PORB7((Data & 0x80) >> 7);
	}

	if ((Selection & 0x40) != 0) { /* Video Beam in Display */
		VIA_PORB6((Data & 0x40) >> 6);
	}

	if ((Selection & 0x20) != 0) { /* Mouse Y2 */
		VIA_PORB5((Data & 0x20) >> 5);
	}

	if ((Selection & 0x10) != 0) { /* Mouse X2 */
		VIA_PORB4((Data & 0x10) >> 4);
	}

	if ((Selection & 0x08) != 0) { /* Mouse Button */
		VIA_PORB3((Data & 0x08) >> 3);
	}

	if ((Selection & 0x04) != 0) { /* RTC Enable */
		VIA_PORB2((Data & 0x04) >> 2);
	}

	if ((Selection & 0x02) != 0) { /* RTC Data Clock */
		VIA_PORB1((Data & 0x02) >> 1);
	}

	if ((Selection & 0x01) != 0) { /* RTC Data */
		VIA_PORB0(Data & 0x01);
	}
}


typedef struct {
	ui3b ORB;    /* Buffer B */
	/* ui3b ORA_H;     Buffer A with Handshake */
	ui3b DDR_B;  /* Data Direction Register B */
	ui3b DDR_A;  /* Data Direction Register A */
	ui3b T1C_L;  /* Timer 1 Counter Low */
	ui3b T1C_H;  /* Timer 1 Counter High */
	ui3b T1L_L;  /* Timer 1 Latch Low */
	ui3b T1L_H;  /* Timer 1 Latch High */
	ui3b T2C_L;  /* Timer 2 Counter Low */
	ui3b T2C_H;  /* Timer 2 Counter High */
	ui3b T2L_L;  /* Timer 2 Latch Low */
	ui3b SR;     /* Shift Register */
	ui3b ACR;    /* Auxiliary Control Register */
	ui3b PCR;    /* Peripheral Control Register */
	ui3b IFR;    /* Interrupt Flag Register */
	ui3b IER;    /* Interrupt Enable Register */
	ui3b ORA;    /* Buffer A */
} VIA_Ty;

LOCALVAR VIA_Ty VIA;

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

LOCALVAR ui3b T1_Active = 0;
LOCALVAR ui3b T2_Active = 0;
LOCALVAR blnr WaitingForKeyBoard = falseblnr;
LOCALVAR blnr HaveKeyBoardResult = falseblnr;
LOCALVAR ui3b KeyBoardResult;

GLOBALVAR blnr VIAInterruptRequest;

GLOBALPROC VIA_Reset(void)
{
	VIA.ORA   = 0x79; VIA.DDR_A = 0x7F;
	VIA.ORB   = 0x87; VIA.DDR_B = 0x87;
	VIA.T1C_L = VIA.T1C_H = VIA.T1L_L = VIA.T1L_H = 0x00;
	VIA.T2C_L = VIA.T2C_H = VIA.T2L_L = 0x00;
	VIA.SR = VIA.ACR = 0x00;
	VIA.PCR   = VIA.IFR   = VIA.IER   = 0x00;
	T1_Active = T2_Active = 0x00;
	WaitingForKeyBoard = falseblnr;
	HaveKeyBoardResult = falseblnr;
	VIAInterruptRequest = 0;
}

LOCALPROC CheckVIAInterruptFlag(void)
{
	blnr NewVIAInterruptRequest = (VIA.IFR & VIA.IER) != 0;

	if (NewVIAInterruptRequest != VIAInterruptRequest) {
		VIAInterruptRequest = NewVIAInterruptRequest;
		VIAorSCCinterruptChngNtfy();
	}
}

LOCALPROC SetVIAInterruptFlag(ui3b VIA_Int)
{
	VIA.IFR |= ((ui3b)1 << VIA_Int);
	CheckVIAInterruptFlag();
}

GLOBALPROC GotKeyBoardData(ui3b v)
{
	if (! WaitingForKeyBoard) {
		HaveKeyBoardResult = trueblnr;
		KeyBoardResult = v;
	} else {
		if (((VIA.ACR & 0x1C) >> 2) == 3) {
			VIA.SR = v;
			SetVIAInterruptFlag(2);
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

#define TimerTicksPerSubTick ((704 * 370 / 20) / kNumSubTicks)

GLOBALFUNC ui5b VIA_Access(ui5b Data, blnr WriteMem, CPTR addr)
{
	switch (addr) {
		case kORB   :
			if ((VIA.PCR & 0xE0) == 0) {
				VIA.IFR &= 0xF7;
				CheckVIAInterruptFlag();
			}
			if (WriteMem) {
				VIA_Put_ORB(VIA.DDR_B, Data);
				VIA.ORB = Data;
			} else {
				VIA.ORB = (VIA.ORB & VIA.DDR_B) + VIA_Get_ORB(VIA.DDR_B);
				Data = VIA.ORB;
			}
			break;
		case kDDR_B :
			if (WriteMem) {
				VIA.DDR_B = Data;
				if ((VIA.DDR_B & 0xFE) != 0x86) {
					ReportAbnormal("Set VIA DDR_B unexpected direction");
				}
			} else {
				Data = VIA.DDR_B;
			}
			break;
		case kDDR_A :
			if (WriteMem) {
				VIA.DDR_A = Data;
				if (VIA.DDR_A != 0x7F) {
					ReportAbnormal("Set VIA DDR_A unexpected direction");
				}
			} else {
				Data = VIA.DDR_A;
			}
			break;
		case kT1C_L :
			if (WriteMem) {
				VIA.T1L_L = Data;
			} else {
				VIA.IFR &= 0xBF; /* Clear T1 Interrupt */
				CheckVIAInterruptFlag();
				Data = VIA.T1C_L;
			}
			break;
		case kT1C_H :
			if (WriteMem) {
				VIA.T1L_H = Data;
				VIA.IFR &= 0xBF; /* Clear T1 Interrupt */
				CheckVIAInterruptFlag();
				VIA.T1C_H = VIA.T1L_H;
				VIA.T1C_L = VIA.T1L_L;
				T1_Active = 1;
			} else {
				Data = VIA.T1C_H;
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
				VIA.IFR &= 0xDF; /* Clear T2 Interrupt */
				CheckVIAInterruptFlag();
				Data = VIA.T2C_L;
			}
			break;
		case kT2_H  :
			if (WriteMem) {
				VIA.T2C_H  = Data;
				VIA.T2C_L  = VIA.T2L_L;
				VIA.IFR &= 0xDF; /* Clear T2 Interrupt */
				CheckVIAInterruptFlag();
				T2_Active = 1;
#if 1
				{
					/*
						Work around for tendency to hang when
						playing sounds in System 7. Not sure if
						this is due to bug in system software,
						or some deeper problem in Mini vMac.
					*/
					ui4b Temp = (VIA.T2C_H << 8) + VIA.T2C_L;
					if ((Temp < /* TimerTicksPerSubTick */128)
						&& (Temp != 0))
					{
						T2_Active = 0; /* Deactivate Timer 2 */
						SetVIAInterruptFlag(5);
						VIA.T2C_H = 0;
						VIA.T2C_L = 0;
					}
				}
#endif
			} else {
				Data = VIA.T2C_H;
			}
			break;
		case kSR:
			if (WriteMem) {
				VIA.SR = Data;
			}
			switch ((VIA.ACR & 0x1C) >> 2) {
				case 3 : /* Shifting In */
					if (HaveKeyBoardResult) {
						VIA.SR = KeyBoardResult;
						HaveKeyBoardResult = falseblnr;
						SetVIAInterruptFlag(2);
					} else {
						WaitingForKeyBoard = trueblnr;
						VIA.IFR &= 0xFB;
						CheckVIAInterruptFlag();
					}
					break;
				case 6 : /* shift out under o2 clock ? */
					if ((! WriteMem) || (VIA.SR != 0)) {
						ReportAbnormal("VIA shift mode 6, non zero");
					}
#if 0 /* possibly should do this. seems not to affect anything. */
					WaitingForKeyBoard = falseblnr;
					HaveKeyBoardResult = falseblnr;
					Keyboard_Put(VIA.SR);
					SetVIAInterruptFlag(2); /* don't wait */
#endif
					break;
				case 7 : /* Shifting Out */
					WaitingForKeyBoard = falseblnr;
					HaveKeyBoardResult = falseblnr;
					Keyboard_Put(VIA.SR);
					SetVIAInterruptFlag(2); /* don't wait */
					break;
			}
			if (! WriteMem) {
				Data = VIA.SR;
			}
			break;
		case kACR:
			if (WriteMem) {
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
				switch ((VIA.ACR & 0x1C) >> 2) {
					case 1:
					case 2:
					case 4:
					case 5:
						ReportAbnormal("Set VIA ACR shift mode 1,2,4,5");
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
					ReportAbnormal("IER ~1");
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
				VIA.IFR &= 0xFE;
				CheckVIAInterruptFlag();
			}
			if (WriteMem) {
				VIA_Put_ORA(VIA.DDR_A, Data);
				VIA.ORA = Data;
			} else {
				VIA.ORA = (VIA.ORA & VIA.DDR_A) + VIA_Get_ORA(VIA.DDR_A);
				Data = VIA.ORA;
			}
			break;
	}
	return Data;
}

#define CA2 0 /* One_Second */
#define CA1 1 /* Vertical_Blanking */
#define SR 2 /* Keyboard_Data_Ready */
#define CB2 3 /* Keyboard_Data */
#define CB1 4 /* Keyboard_Clock */
#define T2 5 /* Timer_2 */
#define T1 6 /* Timer_1 */

GLOBALPROC VIA_Timer(int SubTick)
{
	ui4b Temp;

	UnusedParam(SubTick);
	Temp = (VIA.T1C_H << 8) + VIA.T1C_L; /* Get Timer 1 Counter */
	if ((Temp <= TimerTicksPerSubTick) && (Temp != 0)) {
		if (T1_Active == 1) {
			if ((VIA.ACR & 0x40) != 0) { /* Free Running? */
				/* Reload Counter from Latches */
				ui4b v = (VIA.T1L_H << 8) + VIA.T1L_L;
				if (v != 0) {
					Temp += v * ((TimerTicksPerSubTick - Temp) / v + 1);
				}
			} else {
				T1_Active = 0; /* Deactivate Timer 1 */
			}
			SetVIAInterruptFlag(6); /* */
		}
	}
	Temp -= TimerTicksPerSubTick;
	VIA.T1C_H = (Temp & 0xFF00) >> 8; /* Save Counter */
	VIA.T1C_L = Temp & 0xFF;

	/* if ((VIA.ACR & 0x20) == 0) */
	{ /* Not when pulse counting? */
		Temp = (VIA.T2C_H << 8) + VIA.T2C_L; /* Get Timer 2 Counter */
		if ((Temp <= TimerTicksPerSubTick) && (Temp != 0)) {
			if (T2_Active == 1) {
				T2_Active = 0; /* Deactivate Timer 2 */
				SetVIAInterruptFlag(5);
			}
		}
		Temp -= TimerTicksPerSubTick;
		VIA.T2C_H = (Temp & 0xFF00) >> 8; /* Save Counter */
		VIA.T2C_L = Temp & 0xFF;
	}
}

/* VIA Interrupt Interface */

GLOBALPROC VIA_Int_Vertical_Blanking(void)
{
	SetVIAInterruptFlag(CA1);
}

GLOBALPROC VIA_Int_One_Second(void)
{
	SetVIAInterruptFlag(CA2);
}
