/*
	RTCEMDEV.c
	
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
	Real Time Clock EMulated DEVice
	
	Emulates the RTC found in the Mac Plus.

	This code adapted from "RTC.c" in vMac by Philip Cummins.
*/

#include "SYSDEPNS.h"

#include "RTCEMDEV.h"

#include "OSCOMVAR.h"
#include "OSGLUSTB.h"

typedef struct
{

  // RTC VIA Flags
  UBYTE Enabled;
  UBYTE Clock;
  
  // RTC Data
  UBYTE Mode;
  UBYTE Command;
  UBYTE Data;
  UBYTE Counter;
  
  // RTC Registers
  UBYTE Seconds_1[4]; // Apple Documented Seconds
  UBYTE Seconds_2[4]; // Unknown (apparent) second Seconds backup
  
} RTC_Ty;

RTC_Ty RTC;

// RTC Functions

Boolean RTC_Init (void)
{
	int Counter;
	ULONG secs;
	Boolean IsOk;

	RTC.Enabled = RTC.Clock = RTC.Mode = RTC.Command = RTC.Data = RTC.Counter = 0;  
	for (Counter = 0; Counter < PARAMRAMSize; Counter++) {
		PARAMRAM[Counter] = 0;
	}
	
	secs = GetMacDateInSecond();

	RTC.Seconds_1[0] = secs & 0xFF;
	RTC.Seconds_1[1] = (secs & 0xFF00) >> 8;
	RTC.Seconds_1[2] = (secs & 0xFF0000) >> 16;
	RTC.Seconds_1[3] = (secs & 0xFF000000) >> 24;

	RTC.Seconds_2[0] = RTC.Seconds_1[0];
	RTC.Seconds_2[1] = RTC.Seconds_1[1];
	RTC.Seconds_2[2] = RTC.Seconds_1[2];
	RTC.Seconds_2[3] = RTC.Seconds_1[3];

	PARAMRAM[0]=168;
	PARAMRAM[3]=34;
	PARAMRAM[4]=204;
	PARAMRAM[5]=10;
	PARAMRAM[6]=204;
	PARAMRAM[7]=10;
	PARAMRAM[13]=2;
	PARAMRAM[14]=99;
	PARAMRAM[16]=3;
	PARAMRAM[17]=83;
	PARAMRAM[18]=4;
	PARAMRAM[19]=76;
	
	PARAMRAM[20]=1; /* hog cpu */
	PARAMRAM[21]=0; /* SpeedLimit */

	IsOk = (RTC_Load() == 0);
	
	HogCPU = PARAMRAM[20];
	SpeedLimit = PARAMRAM[21];
	
	return IsOk;
}

void RTC_Interrupt (void)
{
  long Seconds = 0;
  
  Seconds = (RTC.Seconds_1[3] << 24) + (RTC.Seconds_1[2] << 16) + (RTC.Seconds_1[1] << 8) + RTC.Seconds_1[0];
  Seconds++;
  RTC.Seconds_1[0] = Seconds & 0xFF;
  RTC.Seconds_1[1] = (Seconds & 0xFF00) >> 8;
  RTC.Seconds_1[2] = (Seconds & 0xFF0000) >> 16;
  RTC.Seconds_1[3] = (Seconds & 0xFF000000) >> 24;
  
  Seconds = (RTC.Seconds_2[3] << 24) + (RTC.Seconds_2[2] << 16) + (RTC.Seconds_2[1] << 8) + RTC.Seconds_2[0];
  Seconds++;
  RTC.Seconds_2[0] = Seconds & 0xFF;
  RTC.Seconds_2[1] = (Seconds & 0xFF00) >> 8;
  RTC.Seconds_2[2] = (Seconds & 0xFF0000) >> 16;
  RTC.Seconds_2[3] = (Seconds & 0xFF000000) >> 24;
}

void RTC_DoCmd(void);

static void  RTC_Put (UBYTE in)
{
  RTC.Data = (RTC.Data << 1) + in;
  RTC.Counter++;
  if (RTC.Counter == 8)
  {
    RTC_DoCmd();
  }
}

static UBYTE RTC_Get  (void)
{
  return ((RTC.Data >> --RTC.Counter) & 0x01);
}

void RTC_Read_Reg  (void);

void RTC_Write_Reg (void);

void RTC_DoCmd (void)
{
  if (RTC.Mode == 0) // This Byte is a RTC Command
  {
    RTC.Command = RTC.Data;
    RTC.Data = 0;
    if ((RTC.Command & 0x80) == 0x00) // Write Command
    {
      RTC.Mode = 1; // Set to switch to Write Mode
      RTC.Counter = 0;  // Reset the Counter
    }
    else // Read Command
    {
      RTC_Read_Reg(); // Read RTC Register
    }
  }
  else // Data has come in that needs to be written
  {
    RTC_Write_Reg();
  }
}

void RTC_Read_Reg (void)
{
# ifdef _RTC_Debug
  printf("RTC Read Reg %2x\n", RTC.Command);
# endif
  switch ((RTC.Command & 0x7C) >> 2)
  {
    case 0 :
      RTC.Data = RTC.Seconds_1[0]; break;
    case 1 :
      RTC.Data = RTC.Seconds_1[1]; break;
    case 2 :
      RTC.Data = RTC.Seconds_1[2]; break;
    case 3 :
      RTC.Data = RTC.Seconds_1[3]; break;
    case 4 :
	  RTC.Data = RTC.Seconds_2[0]; break; 
	case 5 :
	  RTC.Data = RTC.Seconds_2[1]; break; 
	case 6 :
	  RTC.Data = RTC.Seconds_2[2]; break; 
	case 7 :
	  RTC.Data = RTC.Seconds_2[3]; break; 
    case 8 : case 9 : case 10 : case 11 :
      RTC.Data = PARAMRAM[((RTC.Command & 0x0C) >> 2) + 0x10]; break;
    case 16 : case 17 : case 18 : case 19 :
    case 20 : case 21 : case 22 : case 23 :
    case 24 : case 25 : case 26 : case 27 :
    case 28 : case 29 : case 30 : case 31 :
      RTC.Data = PARAMRAM[((RTC.Command & 0x3C) >> 2)]; break;
    default :
      RTC.Data = 0;
      Debugger();
#     ifdef _RTC_Debug
      printf("RTC Read Reg Unknown, %2x\n", RTC.Command);
#     endif
      break;
  }
  RTC.Command = RTC.Mode = 0;
}

void RTC_Write_Reg (void)
{
# ifdef _RTC_Debug
  printf("RTC Write Reg %2x %2x\n", RTC.Command, RTC.Data);
# endif
  switch ((RTC.Command & 0x7C) >> 2)
  {
    case 0 :
      RTC.Seconds_1[0] = RTC.Data; break;
    case 1 :
      RTC.Seconds_1[1] = RTC.Data; break;
    case 2 :
      RTC.Seconds_1[2] = RTC.Data; break;
    case 3 :
      RTC.Seconds_1[3] = RTC.Data; break;
    case 4 :
      RTC.Seconds_2[0] = RTC.Data; break;
    case 5 :
      RTC.Seconds_2[1] = RTC.Data; break;
    case 6 :
      RTC.Seconds_2[2] = RTC.Data; break;
    case 7 :
      RTC.Seconds_2[3] = RTC.Data; break;
    case 8 : case 9 : case 10 : case 11 :
      PARAMRAM[((RTC.Command & 0x0C) >> 2) + 0x10] = RTC.Data; break;
    case 12 :
      break; // Test Write, do nothing
    case 13 : 
      /* RTC_Save(); */
      /* write to disk when quit, no point in doing it sooner */
      break; // Write_Protect Register
    case 16 : case 17 : case 18 : case 19 :
    case 20 : case 21 : case 22 : case 23 :
    case 24 : case 25 : case 26 : case 27 :
    case 28 : case 29 : case 30 : case 31 :
      PARAMRAM[((RTC.Command & 0x3C) >> 2)] = RTC.Data; break;
    default :
      RTC.Data = 0;
#	ifdef _RTC_Debug
      printf("RTC Write Reg Unknown, %2x\n", RTC.Command);
#	endif
      break;
  }
  RTC.Command = RTC.Data = RTC.Counter = RTC.Mode = 0;
}

void RTC_Save (void)
{

	PARAMRAM[20] = HogCPU;
	PARAMRAM[21] = SpeedLimit;
	RTC_Save0();
}

// VIA Interface Functions

UBYTE VIA_GORB2 (void) // RTC Enable
{
#	ifdef _VIA_Interface_Debug
  	printf("VIA ORB2 attempts to be an input\n");
#	endif
  	return 0;
}

void  VIA_PORB2 (UBYTE Data)
{
  RTC.Enabled = Data;
}

UBYTE VIA_GORB1 (void) // RTC Data Clock
{
#	ifdef _VIA_Interface_Debug
  	printf("VIA ORB1 attempts to be an input\n");
#	endif
	return 0;
}

void  VIA_PORB1 (UBYTE Data)
{
   RTC.Clock = Data;
}

UBYTE VIA_GORB0 (void) // RTC Data
{
  if ((RTC.Enabled == 0) && (RTC.Clock == 1))
    { return RTC_Get(); }
  else
    { return 0; }
}

void  VIA_PORB0 (UBYTE Data)
{
  if ((RTC.Enabled == 0) && (RTC.Clock == 1))
    { RTC_Put(Data); }
}
