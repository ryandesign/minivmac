/*
	VIAEMDEV.c
	
	Copyright (C) 2001 Philip Cummins, Paul Pratt

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

#include "SYSDEPNS.h"

#include "VIAEMDEV.h"

#include "MINEM68K.h"

extern void  Keyboard_Put (UBYTE in);
extern UBYTE Keyboard_Get (void);

extern UBYTE VIA_GORA7(void);  // SCC
extern void  VIA_PORA7(UBYTE Data);

extern UBYTE VIA_GORA6 (void); // Main/Alternate Screen Buffer
extern void  VIA_PORA6 (UBYTE Data);

extern UBYTE VIA_GORA5 (void); // Floppy Disk Line SEL
extern void  VIA_PORA5 (UBYTE Data);

extern UBYTE VIA_GORA4 (void); // Overlay/Normal Memory Mapping
extern void  VIA_PORA4 (UBYTE Data);

extern UBYTE VIA_GORA3 (void); // Main/Alternate Sound Buffer
extern void  VIA_PORA3 (UBYTE Data);

extern UBYTE VIA_GORA2 (void); // Sound Volume Bit 2
extern void  VIA_PORA2 (UBYTE Data);

extern UBYTE VIA_GORA1 (void); // Sound Volume Bit 1
extern void  VIA_PORA1 (UBYTE Data);

extern UBYTE VIA_GORA0 (void); // Sound Volume Bit 0
extern void  VIA_PORA0 (UBYTE Data);

extern UBYTE VIA_GORB7 (void); // Sound Enable
extern void  VIA_PORB7 (UBYTE Data);

extern UBYTE VIA_GORB6 (void); // Video Beam in Display
extern void  VIA_PORB6 (UBYTE Data);

extern UBYTE VIA_GORB5 (void); // Mouse Y2
extern void  VIA_PORB5 (UBYTE Data);

extern UBYTE VIA_GORB4 (void); // Mouse X2
extern void  VIA_PORB4 (UBYTE Data);

extern UBYTE VIA_GORB3 (void); // Mouse Button
extern void  VIA_PORB3 (UBYTE Data);

extern UBYTE VIA_GORB2 (void); // RTC Enable
extern void  VIA_PORB2 (UBYTE Data);

extern UBYTE VIA_GORB1 (void); // RTC Data Clock
extern void  VIA_PORB1 (UBYTE Data);

extern UBYTE VIA_GORB0 (void); // RTC Data
extern void  VIA_PORB0 (UBYTE Data);

// VIA Shift Register Interface

static UBYTE VIA_Get_SR (void)
{
  return Keyboard_Get();
}

static void  VIA_Put_SR (UBYTE Data)
{
  Keyboard_Put(Data);
}

// VIA_Get_ORA : VIA Get Port A Data
// This function queries VIA Port A interfaced hardware about their status

static UBYTE VIA_Get_ORA (UBYTE Selection)
{
  UBYTE Value = 0;
  
  if ((Selection & 0x80) == 0) // SCC Wait/Request
    { Value |= (VIA_GORA7() << 7); }
  
  if ((Selection & 0x40) == 0) // Main/Alternate Screen Buffer
    { Value |= (VIA_GORA6() << 6); }
  
  if ((Selection & 0x20) == 0) // Floppy Disk Line SEL
    { Value |= (VIA_GORA5() << 5); }
  
  if ((Selection & 0x10) == 0) // Overlay/Normal Memory Mapping
    { Value |= (VIA_GORA4() << 4); }
  
  if ((Selection & 0x08) == 0) // Main/Alternate Sound Buffer
    { Value |= (VIA_GORA3() << 3); }
  
  if ((Selection & 0x04) == 0) // Sound Volume Bit 2
    { Value |= (VIA_GORA2() << 2); }
  
  if ((Selection & 0x02) == 0) // Sound Volume Bit 1
    { Value |= (VIA_GORA1() << 1); }
  
  if ((Selection & 0x01) == 0) // Sound Volume Bit 0
    { Value |= VIA_GORA0(); }
  
  return Value;
}

// VIA_Get_ORB : VIA Get Port B Data
// This function queries VIA Port B interfaced hardware about their status

static UBYTE VIA_Get_ORB (UBYTE Selection)
{
  UBYTE Value = 0;
  
  if ((Selection & 0x80) == 0) // Sound Enable
    { Value |= (VIA_GORB7() << 7); }
  
  if ((Selection & 0x40) == 0) // Video Beam in Display
    { Value |= (VIA_GORB6() << 6); }
  
  if ((Selection & 0x20) == 0) // Mouse Y2
    { Value |= (VIA_GORB5() << 5); }
  
  if ((Selection & 0x10) == 0) // Mouse X2
    { Value |= (VIA_GORB4() << 4); }
  
  if ((Selection & 0x08) == 0) // Mouse Button
    { Value |= (VIA_GORB3() << 3); }
  
  if ((Selection & 0x04) == 0) // RTC Enable
    { Value |= (VIA_GORB2() << 2); }
  
  if ((Selection & 0x02) == 0) // RTC Data Clock
    { Value |= (VIA_GORB1() << 1); }
  
  if ((Selection & 0x01) == 0) // RTC Data
    { Value |= VIA_GORB0(); }
  
  return Value;
}

static void  VIA_Put_ORA (UBYTE Selection, UBYTE Data)
{
  if ((Selection & 0x80) != 0) // SCC Wait/Request
    { VIA_PORA7((Data & 0x80) >> 7); }

  if ((Selection & 0x40) != 0) // Main/Alternate Screen Buffer
    { VIA_PORA6((Data & 0x40) >> 6); }
	
  if ((Selection & 0x20) != 0) // Floppy Disk Line SEL
    { VIA_PORA5((Data & 0x20) >> 5); }
	
  if ((Selection & 0x10) != 0) // Overlay/Normal Memory Mapping
    { VIA_PORA4((Data & 0x10) >> 4); }
	
  if ((Selection & 0x08) != 0) // Main/Alternate Sound Buffer
    { VIA_PORA3((Data & 0x08) >> 3); }
	
  if ((Selection & 0x04) != 0) // Sound Volume Bit 2
    { VIA_PORA2((Data & 0x04) >> 2); }
	
  if ((Selection & 0x02) != 0) // Sound Volume Bit 1
    { VIA_PORA1((Data & 0x02) >> 1); }
	
  if ((Selection & 0x01) != 0) // Sound Volume Bit 0
    { VIA_PORA0(Data & 0x01); }
}
	
static void  VIA_Put_ORB (UBYTE Selection, UBYTE Data)
{
  if ((Selection & 0x80) != 0) // Sound Enable
    { VIA_PORB7((Data & 0x80) >> 7); }

  if ((Selection & 0x40) != 0) // Video Beam in Display
    { VIA_PORB6((Data & 0x40) >> 6); }
	
  if ((Selection & 0x20) != 0) // Mouse Y2
    { VIA_PORB5((Data & 0x20) >> 5); }
	
  if ((Selection & 0x10) != 0) // Mouse X2
    { VIA_PORB4((Data & 0x10) >> 4); }
	
  if ((Selection & 0x08) != 0) // Mouse Button
    { VIA_PORB3((Data & 0x08) >> 3); }

  if ((Selection & 0x04) != 0) // RTC Enable
    { VIA_PORB2((Data & 0x04) >> 2); }
	
  if ((Selection & 0x02) != 0) // RTC Data Clock
    { VIA_PORB1((Data & 0x02) >> 1); }
	
  if ((Selection & 0x01) != 0) // RTC Data
    { VIA_PORB0(Data & 0x01); }
}


typedef struct
{

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

static VIA_Ty VIA;

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

#define kSR_Init_Time 0x007F // Was 0x09C9, trying less

UBYTE T1_Active = 0;
UBYTE T2_Active = 0;
UBYTE SR_Active = 0;
UBYTE SR_Timer_Active = 0;

ULONG SR_Time = 0;

void  VIA_Reset (void)
{
  VIA.ORA   = 0x79; VIA.DDR_A = 0x7F;
  VIA.ORB   = 0x87; VIA.DDR_B = 0x87;
  VIA.T1C_L = VIA.T1C_H = VIA.T1L_L = VIA.T1L_H = 0x00;
  VIA.T2_L  = VIA.T2_H  = VIA.SR    = VIA.ACR   = 0x00;
  VIA.PCR   = VIA.IFR   = VIA.IER   = 0x00;
  T1_Active = T2_Active = SR_Active = 0x00;
  SR_Time   = 0x0000;
  SR_Timer_Active = 0x00;
}

extern ULONG DataBus;
extern blnr ByteSizeAccess;
extern blnr WriteMemAccess;

void VIA_Access(CPTR addr)
{
	if (ByteSizeAccess) {
		if ((addr & 1) == 0) {
			switch (addr >> 9) {
				case kORB   :
				    if ((VIA.PCR & 0xE0) == 0) {
				    	VIA.IFR &= 0xF7;
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
					if (WriteMemAccess) {
					    VIA.T1C_L = DataBus;
				    } else {
					    DataBus = VIA.T1C_L;;
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
					    DataBus = VIA.T2_L;
				    }
				    break;
				case kT2_H  :
					if (WriteMemAccess) {
					    VIA.T2_H  = DataBus;
					    VIA.IFR &= 0xDF; // Clear T2 Interrupt
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
					  			VIA.SR = VIA_Get_SR();
					  			break;
					        case 4 : case 5 : case 6 : case 7 : // Shifting Out
								if (WriteMemAccess) {
					     			VIA.SR = DataBus;
					    		}
					    		VIA_Put_SR(VIA.SR);
					     		break;
				   		}
			       		SR_Time = kSR_Init_Time; // Estimated
			     		SR_Timer_Active = 1;
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
				case kIFR   :
					if (WriteMemAccess) {
					    if ((DataBus & 0x80) == 0) {
					    	VIA.IFR = VIA.IFR & (0x7F - DataBus); // Clear Enable Bits
					    } else {
					    	VIA.IFR = VIA.IFR | (DataBus & 0x7F); // Set Enable Bits
					    }
				    } else {
				    	DataBus = VIA.IFR;
				    }
				    break;
				case kIER   :
					if (WriteMemAccess) {
					    if ((DataBus & 0x80) == 0) {
					    	VIA.IER = VIA.IER & (0x7F - DataBus); // Clear Enable Bits
					    } else {
					    	VIA.IER = VIA.IER | (DataBus & 0x7F); // Set Enable Bits
					    }
				    } else {
				    	DataBus = VIA.IER;
				    }
				    break;
				case kORA   :
				case kORA_H :
				    if ((VIA.PCR & 0xE) == 0) {
				    	VIA.IFR &= 0xFE;
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

static void VIA_Interrupt (UBYTE VIA_Int)
{
	if ((VIA.IER & ((UBYTE)1 << VIA_Int)) != 0) {
		VIA.IFR |= (((UBYTE)1 << VIA_Int) | 0x80);
	}
  
	if ((VIA.IFR & 0x80) != 0) {
		(void) ViaException();
	}
}

void VIA_Timer (void)
{
  UWORD Temp = 0;
  
	if (SR_Timer_Active == 1)
	{
		SR_Time -= 1;
		if (SR_Time == 0) {
			//SR_Timer_Active = 0;
			if ((VIA.IER & 0x04) != 0) // Shift Register Interrupts Enabled
				VIA.IFR |= 0x84;

			if ((VIA.IFR & 0x80) != 0)
			if (ViaException())
			{
				SR_Timer_Active = 0;
			}
		}
	}
  
  if (T1_Active == 1) // Timer 1 is Active
  {
    Temp = (VIA.T1C_H << 8) + VIA.T1C_L; // Get Timer 1 Counter
    Temp -= 1;
    VIA.T1C_H = (Temp & 0xFF00) >> 8; // Save Counter
    VIA.T1C_L = Temp & 0xFF;
    if (Temp == 0) // Complete?
    {
      if ((VIA.ACR & 0x40) == 1) // Free Running?
      {
        VIA.T1C_H = VIA.T1L_H; // Reload Counter from Latches
        VIA.T1C_L = VIA.T1L_L;
      }
      else
      {
        T1_Active = 0; // Deactivate Timer 1
      }
      if ((VIA.IER & 0x40) == 1) // Timer 1 Interrupt Enabled
        VIA.IFR |= 0xC0;
    }
  }
	if (T2_Active == 1) {
		Temp = (VIA.T2_H << 8) + VIA.T2_L; // Get Timer 1 Counter
		Temp -= 1;
		VIA.T2_H = (Temp & 0xFF00) >> 8; // Save Counter
		VIA.T2_L = Temp & 0xFF;
		if (Temp == 0) {
			T2_Active = 0; // Deactivate Timer 2
			if ((VIA.IER & 0x20) == 1) // Timer 2 Interrupt Enabled
			VIA.IFR |= 0xA0;
		}
	}
	if ((VIA.IFR & 0x60) != 0)
	{
		(void) ViaException();
	}
}

// VIA Interrupt Interface

void VIA_Int_Vertical_Blanking(void)
{
	VIA_Interrupt(CA1);
}

void VIA_Int_One_Second(void)
{
	VIA_Interrupt(CA2);
}
