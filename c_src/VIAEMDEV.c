/*
	VIAEMDEV.c

	Copyright (C) 2002 Philip Cummins, Paul Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with with this file; see the file COPYING.

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

#include "ADDRSPAC.h"
#include "MINEM68K.h"
#endif

#include "VIAEMDEV.h"

IMPORTPROC Keyboard_Put(UBYTE in);
IMPORTPROC Keyboard_Get(void);

IMPORTFUNC UBYTE VIA_GORA7(void);  // SCC
IMPORTPROC VIA_PORA7(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORA6(void); // Main/Alternate Screen Buffer
IMPORTPROC VIA_PORA6(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORA5(void); // Floppy Disk Line SEL
IMPORTPROC VIA_PORA5(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORA4(void); // Overlay/Normal Memory Mapping
IMPORTPROC VIA_PORA4(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORA3(void); // Main/Alternate Sound Buffer
IMPORTPROC VIA_PORA3(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORA2(void); // Sound Volume Bit 2
IMPORTPROC VIA_PORA2(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORA1(void); // Sound Volume Bit 1
IMPORTPROC VIA_PORA1(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORA0(void); // Sound Volume Bit 0
IMPORTPROC VIA_PORA0(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORB7(void); // Sound Enable
IMPORTPROC VIA_PORB7(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORB6(void); // Video Beam in Display
IMPORTPROC VIA_PORB6(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORB5(void); // Mouse Y2
IMPORTPROC VIA_PORB5(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORB4(void); // Mouse X2
IMPORTPROC VIA_PORB4(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORB3(void); // Mouse Button
IMPORTPROC VIA_PORB3(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORB2(void); // RTC Enable
IMPORTPROC VIA_PORB2(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORB1(void); // RTC Data Clock
IMPORTPROC VIA_PORB1(UBYTE Data);

IMPORTFUNC UBYTE VIA_GORB0(void); // RTC Data
IMPORTPROC VIA_PORB0(UBYTE Data);

// VIA_Get_ORA : VIA Get Port A Data
// This function queries VIA Port A interfaced hardware about their status

LOCALFUNC UBYTE VIA_Get_ORA(UBYTE Selection)
{
	UBYTE Value = 0;

	if ((Selection & 0x80) == 0) { // SCC Wait/Request
		Value |= (VIA_GORA7() << 7);
	}

	if ((Selection & 0x40) == 0) { // Main/Alternate Screen Buffer
		Value |= (VIA_GORA6() << 6);
	}

	if ((Selection & 0x20) == 0) { // Floppy Disk Line SEL
		Value |= (VIA_GORA5() << 5);
	}

	if ((Selection & 0x10) == 0) { // Overlay/Normal Memory Mapping
		Value |= (VIA_GORA4() << 4);
	}

	if ((Selection & 0x08) == 0) { // Main/Alternate Sound Buffer
		Value |= (VIA_GORA3() << 3);
	}

	if ((Selection & 0x04) == 0) { // Sound Volume Bit 2
		Value |= (VIA_GORA2() << 2);
	}

	if ((Selection & 0x02) == 0) { // Sound Volume Bit 1
		Value |= (VIA_GORA1() << 1);
	}

	if ((Selection & 0x01) == 0) { // Sound Volume Bit 0
		Value |= VIA_GORA0();
	}

	return Value;
}

// VIA_Get_ORB : VIA Get Port B Data
// This function queries VIA Port B interfaced hardware about their status

LOCALFUNC UBYTE VIA_Get_ORB(UBYTE Selection)
{
	UBYTE Value = 0;

	if ((Selection & 0x80) == 0) { // Sound Enable
		Value |= (VIA_GORB7() << 7);
	}

	if ((Selection & 0x40) == 0) { // Video Beam in Display
		Value |= (VIA_GORB6() << 6);
	}

	if ((Selection & 0x20) == 0) { // Mouse Y2
		Value |= (VIA_GORB5() << 5);
	}

	if ((Selection & 0x10) == 0) { // Mouse X2
		Value |= (VIA_GORB4() << 4);
	}

	if ((Selection & 0x08) == 0) { // Mouse Button
		Value |= (VIA_GORB3() << 3);
	}

	if ((Selection & 0x04) == 0) { // RTC Enable
		Value |= (VIA_GORB2() << 2);
	}

	if ((Selection & 0x02) == 0) { // RTC Data Clock
		Value |= (VIA_GORB1() << 1);
	}

	if ((Selection & 0x01) == 0) { // RTC Data
		Value |= VIA_GORB0();
	}

	return Value;
}

LOCALPROC VIA_Put_ORA(UBYTE Selection, UBYTE Data)
{
	if ((Selection & 0x80) != 0) { // SCC Wait/Request
		VIA_PORA7((Data & 0x80) >> 7);
	}

	if ((Selection & 0x40) != 0) { // Main/Alternate Screen Buffer
		VIA_PORA6((Data & 0x40) >> 6);
	}

	if ((Selection & 0x20) != 0) { // Floppy Disk Line SEL
		VIA_PORA5((Data & 0x20) >> 5);
	}

	if ((Selection & 0x10) != 0) { // Overlay/Normal Memory Mapping
		VIA_PORA4((Data & 0x10) >> 4);
	}

	if ((Selection & 0x08) != 0) { // Main/Alternate Sound Buffer
		VIA_PORA3((Data & 0x08) >> 3);
	}

	if ((Selection & 0x04) != 0) { // Sound Volume Bit 2
		VIA_PORA2((Data & 0x04) >> 2);
	}

	if ((Selection & 0x02) != 0) { // Sound Volume Bit 1
		VIA_PORA1((Data & 0x02) >> 1);
	}

	if ((Selection & 0x01) != 0) { // Sound Volume Bit 0
		VIA_PORA0(Data & 0x01);
	}
}

LOCALPROC VIA_Put_ORB(UBYTE Selection, UBYTE Data)
{
	if ((Selection & 0x80) != 0) { // Sound Enable
		VIA_PORB7((Data & 0x80) >> 7);
	}

	if ((Selection & 0x40) != 0) { // Video Beam in Display
		VIA_PORB6((Data & 0x40) >> 6);
	}

	if ((Selection & 0x20) != 0) { // Mouse Y2
		VIA_PORB5((Data & 0x20) >> 5);
	}

	if ((Selection & 0x10) != 0) { // Mouse X2
		VIA_PORB4((Data & 0x10) >> 4);
	}

	if ((Selection & 0x08) != 0) { // Mouse Button
		VIA_PORB3((Data & 0x08) >> 3);
	}

	if ((Selection & 0x04) != 0) { // RTC Enable
		VIA_PORB2((Data & 0x04) >> 2);
	}

	if ((Selection & 0x02) != 0) { // RTC Data Clock
		VIA_PORB1((Data & 0x02) >> 1);
	}

	if ((Selection & 0x01) != 0) { // RTC Data
		VIA_PORB0(Data & 0x01);
	}
}


typedef struct {
	UBYTE ORB;    // Buffer B
	//UBYTE ORA_H;     Buffer A with Handshake
	UBYTE DDR_B;  // Data Direction Register B
	UBYTE DDR_A;  // Data Direction Register A
	UBYTE T1C_L;  // Timer 1 Counter Low
	UBYTE T1C_H;  // Timer 1 Counter High
	UBYTE T1L_L;  // Timer 1 Latch Low
	UBYTE T1L_H;  // Timer 1 Latch High
	UBYTE T2_L;   // Timer 2 Low
	UBYTE T2_H;   // Timer 2 High
	UBYTE SR;     // Shift Register
	UBYTE ACR;    // Auxiliary Control Register
	UBYTE PCR;    // Peripheral Control Register
	UBYTE IFR;    // Interrupt Flag Register
	UBYTE IER;    // Interrupt Enable Register
	UBYTE ORA;    // Buffer A
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

LOCALVAR UBYTE T1_Active = 0;
LOCALVAR UBYTE T2_Active = 0;
LOCALVAR UBYTE SR_Active = 0;

GLOBALVAR blnr VIAInterruptRequest;

GLOBALPROC VIA_Reset(void)
{
	VIA.ORA   = 0x79; VIA.DDR_A = 0x7F;
	VIA.ORB   = 0x87; VIA.DDR_B = 0x87;
	VIA.T1C_L = VIA.T1C_H = VIA.T1L_L = VIA.T1L_H = 0x00;
	VIA.T2_L  = VIA.T2_H  = VIA.SR    = VIA.ACR   = 0x00;
	VIA.PCR   = VIA.IFR   = VIA.IER   = 0x00;
	T1_Active = T2_Active = SR_Active = 0x00;
	VIAInterruptRequest = 0;
}

LOCALPROC CheckVIAInterruptFlag(void)
{
	blnr NewVIAInterruptRequest = (VIA.IFR & VIA.IER) != 0;

	if (NewVIAInterruptRequest != VIAInterruptRequest) {
		VIAInterruptRequest = NewVIAInterruptRequest;
		if (VIAInterruptRequest) {
			ViaException();
		}
	}
}

LOCALPROC SetVIAInterruptFLag(UBYTE VIA_Int)
{
	VIA.IFR |= ((UBYTE)1 << VIA_Int);
	CheckVIAInterruptFlag();
}

GLOBALPROC GotKeyBoardData(UBYTE v)
{
	VIA.SR = v;
	SetVIAInterruptFLag(2);
}

GLOBALPROC VIA_Access(CPTR addr)
{
	if (ByteSizeAccess) {
		if ((addr & 1) == 0) {
			switch (addr >> 9) {
				case kORB   :
					if ((VIA.PCR & 0xE0) == 0) {
						VIA.IFR &= 0xF7;
						CheckVIAInterruptFlag();
					}
					if (WriteMemAccess) {
						VIA_Put_ORB(VIA.DDR_B,DataBus);
						VIA.ORB = DataBus;
					} else {
						VIA.ORB = (VIA.ORB & VIA.DDR_B) + VIA_Get_ORB(VIA.DDR_B);
						DataBus = VIA.ORB;
					}
					break;
				case kDDR_B :
					if (WriteMemAccess) {
						VIA.DDR_B = DataBus;
					} else {
						DataBus = VIA.DDR_B;
					}
					break;
				case kDDR_A :
					if (WriteMemAccess) {
						VIA.DDR_A = DataBus;
					} else {
						DataBus = VIA.DDR_A;
					}
					break;
				case kT1C_L :
					VIA.IFR &= 0xBF; // Clear T1 Interrupt
					CheckVIAInterruptFlag();
					if (WriteMemAccess) {
						VIA.T1C_L = DataBus;
					} else {
						DataBus = VIA.T1C_L;
					}
					break;
				case kT1C_H :
					if (WriteMemAccess) {
						VIA.T1C_H = DataBus;
					} else {
						DataBus = VIA.T1C_H;
					}
					break;
				case kT1L_L :
					if (WriteMemAccess) {
						VIA.T1L_L = DataBus;
					} else {
						DataBus = VIA.T1L_L;
					}
					break;
				case kT1L_H :
					if (WriteMemAccess) {
						VIA.T1L_H = DataBus;
						VIA.IFR &= 0xBF; // Clear T1 Interrupt
						CheckVIAInterruptFlag();
						VIA.T1C_H = VIA.T1L_H;
						VIA.T1C_L = VIA.T1L_L;
						T1_Active = 1;
					} else {
						DataBus = VIA.T1L_H;
					}
					break;
				case kT2_L  :
					if (WriteMemAccess) {
						VIA.T2_L = DataBus;
					} else {
						VIA.IFR &= 0xDF; // Clear T2 Interrupt
						CheckVIAInterruptFlag();
						DataBus = VIA.T2_L;
					}
					break;
				case kT2_H  :
					if (WriteMemAccess) {
						VIA.T2_H  = DataBus;
						VIA.IFR &= 0xDF; // Clear T2 Interrupt
						CheckVIAInterruptFlag();
						T2_Active = 1;
					} else {
						DataBus = VIA.T2_H;
					}
					break;
				case kSR:
					/* if write access and not SR_Active,
						then VIA.SR won't be changed. is this correct?
					*/
					if (SR_Active == 1) {
						switch ((VIA.ACR & 0x1C) >> 2) {
							case 1 : case 2 : case 3 : // Shifting In
								Keyboard_Get();
								break;
							case 4 : case 5 : case 6 : case 7 : // Shifting Out
								if (WriteMemAccess) {
									VIA.SR = DataBus;
								}
								Keyboard_Put(VIA.SR);
								SetVIAInterruptFLag(2); /* don't wait */
								break;
						}
						SR_Active = 0;
					}
					if (! WriteMemAccess) {
						DataBus = VIA.SR;
					}
					break;
				case kACR:
					if (WriteMemAccess) {
						VIA.ACR = DataBus;
						switch ((VIA.ACR & 0x1C) >> 2) {
							case 0 :
								SR_Active = 0;
								break;
							default :
								SR_Active = 1;
								break;
						}
					} else {
						DataBus = VIA.ACR;
					}
					break;
				case kPCR:
					if (WriteMemAccess) {
						VIA.PCR = DataBus;
					} else {
						DataBus = VIA.PCR;
					}
					break;
				case kIFR:
					if (WriteMemAccess) {
						if ((DataBus & 0x80) == 0) {
							VIA.IFR = VIA.IFR & (0x7F - DataBus); // Clear Enable Bits
						} else {
							VIA.IFR = VIA.IFR | (DataBus & 0x7F); // Set Enable Bits
						}
						CheckVIAInterruptFlag();
					} else {
						DataBus = VIA.IFR;
						if (VIA.IFR != 0) {
							DataBus |= 0x80;
						}
					}
					break;
				case kIER   :
					if (WriteMemAccess) {
						if ((DataBus & 0x80) == 0) {
							VIA.IER = VIA.IER & (0x7F - DataBus); // Clear Enable Bits
						} else {
							VIA.IER = VIA.IER | (DataBus & 0x7F); // Set Enable Bits
						}
						CheckVIAInterruptFlag();
					} else {
						DataBus = VIA.IER;
					}
					break;
				case kORA   :
				case kORA_H :
					if ((VIA.PCR & 0xE) == 0) {
						VIA.IFR &= 0xFE;
						CheckVIAInterruptFlag();
					}
					if (WriteMemAccess) {
						VIA_Put_ORA(VIA.DDR_A, DataBus);
						VIA.ORA = DataBus;
					} else {
						VIA.ORA = (VIA.ORA & VIA.DDR_A) + VIA_Get_ORA(VIA.DDR_A);
						DataBus = VIA.ORA;
					}
					break;
			}
		}
	}
}

#define CA2 0 /* One_Second */
#define CA1 1 /* Vertical_Blanking */
#define SR 2 /* Keyboard_Data_Ready */
#define CB2 3 /* Keyboard_Data */
#define CB1 4 /* Keyboard_Clock */
#define T2 5 /* Timer_2 */
#define T1 6 /* Timer_1 */

GLOBALPROC VIA_Timer(void)
{
	UWORD Temp = 0;

	if (T1_Active == 1) { // Timer 1 is Active
		Temp = (VIA.T1C_H << 8) + VIA.T1C_L; // Get Timer 1 Counter
		Temp -= 1;
		VIA.T1C_H = (Temp & 0xFF00) >> 8; // Save Counter
		VIA.T1C_L = Temp & 0xFF;
		if (Temp == 0) { // Complete?
			if ((VIA.ACR & 0x40) == 1) { // Free Running?
				VIA.T1C_H = VIA.T1L_H; // Reload Counter from Latches
				VIA.T1C_L = VIA.T1L_L;
			} else {
				T1_Active = 0; // Deactivate Timer 1
			}
			/* SetVIAInterruptFLag(6); */
		}
	}
	if (T2_Active == 1) {
		Temp = (VIA.T2_H << 8) + VIA.T2_L; // Get Timer 1 Counter
		Temp -= 1;
		VIA.T2_H = (Temp & 0xFF00) >> 8; // Save Counter
		VIA.T2_L = Temp & 0xFF;
		if (Temp == 0) {
			T2_Active = 0; // Deactivate Timer 2
			/* SetVIAInterruptFLag(5); */
		}
	}
}

// VIA Interrupt Interface

GLOBALPROC VIA_Int_Vertical_Blanking(void)
{
	SetVIAInterruptFLag(CA1);
}

GLOBALPROC VIA_Int_One_Second(void)
{
	SetVIAInterruptFLag(CA2);
}
