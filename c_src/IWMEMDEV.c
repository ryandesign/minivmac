/*
	IWMEVDEV.c
	
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
	Integrated Woz Macine EMulated DEVice
	
	Emulates the IWM found in the Mac Plus.

	This code adapted from "IWM.c" in vMac by Philip Cummins.
*/

// This is the emulation for the IWM, the Integrated Woz Machine. It's basically
// a serial to parallel converter with some timing in-built into it to perform
// handshaking. Emulation so far just includes Status and Mode Register Accesses.

#include "SYSDEPNS.h"

#include "IWMEMDEV.h"

#define kph0L     0x00 // CA0 off (0)
#define kph0H     0x01 // CA0 on (1)
#define kph1L     0x02 // CA1 off (0)
#define kph1H     0x03 // CA1 on (1)
#define kph2L     0x04 // CA2 off (0)
#define kph2H     0x05 // CA2 on (1)
#define kph3L     0x06 // LSTRB off (low)
#define kph3H     0x07 // LSTRB on (high)
#define kmtrOff   0x08 // disk enable off
#define kmtrOn    0x09 // disk enable on
#define kintDrive 0x0A // select internal drive
#define kextDrive 0x0B // select external drive
#define kq6L      0x0C // Q6 off
#define kq6H      0x0D // Q6 on
#define kq7L      0x0E // Q7 off
#define kq7H      0x0F // Q7 on

#define kph0 0x01
#define kph1 0x02
#define kph2 0x04
#define kph3 0x08
#define kmtr 0x10
#define kdrv 0x20
#define kq6  0x40
#define kq7  0x80

UBYTE vSel; // Selection Line from VIA

typedef struct
{
  UBYTE DataIn;    // Read Data Register
  UBYTE Handshake; // Read Handshake Register
  UBYTE Status;    // Read Status Register
  UBYTE Mode;      // Drive Off : Write Mode Register
                   // Drive On  : Write Data Register
  UBYTE DataOut;   // Write Data Register
  UBYTE Lines;     // Used to Access Disk Drive Registers
} IWM_Ty;

IWM_Ty IWM;

void IWM_Reset(void)
{
	IWM.DataIn = IWM.Handshake = IWM.Status = IWM.Mode = IWM.DataOut = IWM.Lines = 0;
}

typedef enum {On, Off} Mode_Ty;

static void IWM_Set_Lines (UBYTE line, Mode_Ty mode)
{
  if (mode == Off)
    { IWM.Lines &= (0xFF - line); }
  else
    { IWM.Lines |= line; }
}

UBYTE IWM_Read_Reg  (void);
void  IWM_Write_Reg (UBYTE in);

extern ULONG DataBus;
extern blnr ByteSizeAccess;
extern blnr WriteMemAccess;

void IWM_Access(CPTR addr)
{
	if (ByteSizeAccess) {
		if ((addr & 1) == 0) {
			switch (addr >> 9) {
				case kph0L :
					IWM_Set_Lines(kph0, Off);
					break;
				case kph0H :
					IWM_Set_Lines(kph0, On);
					break;
			  	case kph1L :
					IWM_Set_Lines(kph1, Off);
					break;
			 	case kph1H :
			    	IWM_Set_Lines(kph1, On);
			    	break;
			  	case kph2L :
			    	IWM_Set_Lines(kph2, Off);
			    	break;
				case kph2H :
					IWM_Set_Lines(kph2, On);
					break;
			  	case kph3L :
			    	IWM_Set_Lines(kph3, Off);
			    	break;
			  	case kph3H :
			    	IWM_Set_Lines(kph3, On);
			    	break;
			  	case kmtrOff :
					IWM.Status &= 0xDF;
					IWM_Set_Lines(kmtr, Off); 
					break;
				case kmtrOn :
					IWM.Status |= 0x20;
					IWM_Set_Lines(kmtr, On);  
					break;
				case kintDrive :
					IWM_Set_Lines(kdrv, Off);
					break;
			  	case kextDrive :
					IWM_Set_Lines(kdrv, On);
					break;
				case kq6L :
			    	IWM_Set_Lines(kq6, Off);
			    	break;
				case kq6H :
					IWM_Set_Lines(kq6, On);
					break;
				case kq7L :
					if (! WriteMemAccess) {
					    DataBus = IWM_Read_Reg();
					}
					IWM_Set_Lines(kq7, Off);  
					break;
				case kq7H :
					if (WriteMemAccess) {
					    IWM_Write_Reg(DataBus);
				    }
				    IWM_Set_Lines(kq7, On);
				    break;
			}
			return;
		}
	}
}

UBYTE IWM_Read_Reg (void)
{
  switch ((IWM.Lines & (kq6 + kq7)) >> 6)
  {
    case 0 :
	  #ifdef _IWM_Debug
      printf("IWM Data Read\n");
	  #endif
      return IWM.DataIn; break;
    case 1 :
	  #ifdef _IWM_Debug
      printf("IWM Status Read\n");
	  #endif
      return IWM.Status; break;
    case 2 : 
	  #ifdef _IWM_Debug
      printf("IWM Handshake Read\n");
	  #endif
      return IWM.Handshake; break;
    case 3 :
      return 0; break;
  }

//
// bill huey --- to make the compiler GCC happy...
// The "return" is useless because all cases are handled
// the "switch" statement...
//
  return 0;
}

void IWM_Write_Reg (UBYTE in)
{
  if (((IWM.Lines & kmtr) >> 4) == 0)
  {
    #ifdef _IWM_Debug
    printf("IWM Mode Register Write\n");
	#endif
    IWM.Mode = in;
    IWM.Status = ((IWM.Status & 0xE0) + (IWM.Mode & 0x1F));
  }
}

// VIA Interface Headers

UBYTE VIA_GORA5 (void) // Floppy Disk Line SEL
{
  #ifdef _VIA_Interface_Debug
  printf("VIA ORA5 attempts to be an input\n");
  #endif
  return 0;
}

void  VIA_PORA5 (UBYTE Data)
{
  vSel = Data;
}
