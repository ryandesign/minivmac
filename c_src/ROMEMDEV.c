/*
	ROMEMDEV.c
	
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
	Read Only Memory EMulated DEVice
	
	Checks the header of the loaded ROM image, and then patches
	the ROM image.

	This code descended from "ROM.c" in vMac by Philip Cummins.
*/

#include "SYSDEPNS.h"

#include "ROMEMDEV.h"

#include "ENDIANAC.h"
#include "OSGLUSTB.h"
#include "OSCOMVAR.h"

static const UWORD sony_driver[] = {	// Replacement for .Sony driver
	0x4F00, 0x0000, 0x0000, 0x0000,
	0x0018, 0x002C, 0x0040, 0x005C,
	0x0092, 0x052E, 0x536F, 0x6E79,
	0x48E7, 0x00C0, 0x48E7, 0x00C0,
	0x6100, 0x0180, 0x504F, 0x4CDF,
	0x0300, 0x4E75, 0x48E7, 0x00C0,
	0x48E7, 0x00C0, 0x6100, 0x0242,
	0x504F, 0x4CDF, 0x0300, 0x602E,
	0x48E7, 0x00C0, 0x48E7, 0x00C0,
	0x6100, 0x03B0, 0x504F, 0x4CDF,
	0x0300, 0x0C68, 0x0001, 0x001A,
	0x6614, 0x4E75, 0x48E7, 0x00C0,
	0x48E7, 0x00C0, 0x6100, 0x0480,
	0x504F, 0x4CDF, 0x0300, 0x3228,
	0x0006, 0x0801, 0x0009, 0x670C,
	0x4A40, 0x6F02, 0x4240, 0x3140,
	0x0010, 0x4E75, 0x4A40, 0x6F04,
	0x4240, 0x4E75, 0x2F38, 0x08FC,
	0x4E75, 0x48E7, 0x00C0, 0x48E7,
	0x00C0, 0x6100, 0x04B6, 0x504F,
	0x4CDF, 0x0300, 0x4E75, 0x48E7,
	0xE0C0, 0x2F2F, 0x0014, 0x6100,
	0x008C, 0x584F, 0x4CDF, 0x0307,
	0x584F, 0x4E73, 0x4E56, 0x0000,
	0x48E7, 0x1300, 0x3E2E, 0x000A,
	0x7000, 0x3007, 0x0C40, 0x0003,
	0x6426, 0x2C38, 0x0134, 0x2006,
	0x724A, 0xD081, 0x7400, 0x3407,
	0x2602, 0xC4FC, 0x0042, 0x4843,
	0xC6FC, 0x0042, 0x4843, 0x4243,
	0xD483, 0xD480, 0x2002, 0x6002,
	0x7000, 0x4CEE, 0x00C8, 0xFFF4,
	0x4E5E, 0x4E75, 0x4E56, 0x0000,
	0x2F07, 0x7000, 0x302E, 0x000A,
	0x2F00, 0x4EBA, 0xFFA8, 0x2E00,
	0x671A, 0x2007, 0x5480, 0x2040,
	0x4210, 0x2007, 0x5680, 0x2040,
	0x4210, 0x2007, 0x7212, 0xD081,
	0x2040, 0x4210, 0x2E2E, 0xFFFC,
	0x4E5E, 0x4E75, 0x4E56, 0x0000,
	0x48E7, 0x0700, 0x2A2E, 0x0008,
	0x2C3C, 0x0000, 0xFFFF, 0xCC85,
	0x48C6, 0x2F06, 0x4EBA, 0xFF66,
	0x2E00, 0x5680, 0x2040, 0x7000,
	0x1010, 0x4A80, 0x584F, 0x6630,
	0x2005, 0x4240, 0x4840, 0x2207,
	0x5481, 0x2041, 0x1080, 0x2007,
	0x5680, 0x2040, 0x10BC, 0x0001,
	0x2007, 0x7212, 0xD081, 0x2040,
	0x10BC, 0x00FF, 0x307C, 0x0007,
	0x48C6, 0x2006, 0x5280, 0xA02F,
	0x4CEE, 0x00E0, 0xFFF4, 0x4E5E,
	0x4E75, 0x4E56, 0x0000, 0x48E7,
	0x0700, 0x4AB9, 0x00F4, 0x0014,
	0x6706, 0x4245, 0x6000, 0x00B6,
	0x203C, 0x0000, 0x0310, 0xA71E,
	0x2C08, 0x6700, 0x00A6, 0x21C6,
	0x0134, 0x4246, 0x6000, 0x0082,
	0x2007, 0x2040, 0x4250, 0x2007,
	0x5480, 0x2040, 0x4210, 0x2007,
	0x5680, 0x2040, 0x4210, 0x2007,
	0x5880, 0x2040, 0x10BC, 0x0001,
	0x2007, 0x5A80, 0x2040, 0x10BC,
	0x00FF, 0x48C6, 0x2006, 0x5280,
	0x2207, 0x740C, 0xD282, 0x2041,
	0x3080, 0x2007, 0x720E, 0xD081,
	0x2040, 0x30BC, 0xFFFB, 0x2007,
	0x7412, 0xD082, 0x2040, 0x4210,
	0x2007, 0x7413, 0xD082, 0x2040,
	0x10BC, 0x00FF, 0x2007, 0x7414,
	0xD082, 0x2040, 0x4250, 0x2007,
	0x5C80, 0x2040, 0x48C6, 0x2006,
	0x5280, 0x4840, 0x4240, 0xD0BC,
	0x0000, 0xFFFB, 0xA04E, 0x5246,
	0x48C6, 0x2F06, 0x4EBA, 0xFE66,
	0x2E00, 0x584F, 0x6600, 0xFF72,
	0x41FA, 0xFE44, 0x23C8, 0x00F4,
	0x0014, 0x4245, 0x3005, 0x4CEE,
	0x00E0, 0xFFF4, 0x4E5E, 0x4E75,
	0x4E56, 0xFFFE, 0x48E7, 0x1F00,
	0x2E2E, 0x0008, 0x2007, 0x7216,
	0xD081, 0x2040, 0x7000, 0x3010,
	0x5380, 0x3D40, 0xFFFE, 0x7000,
	0x302E, 0xFFFE, 0x2F00, 0x4EBA,
	0xFE1C, 0x2C00, 0x584F, 0x672A,
	0x2A06, 0x5685, 0x2045, 0x1C10,
	0x7000, 0x1006, 0x0C40, 0x0002,
	0x6718, 0x7000, 0x1006, 0x0C40,
	0x0001, 0x6608, 0x2045, 0x10BC,
	0x0002, 0x6006, 0x70BF, 0x6000,
	0x0120, 0x2007, 0x722C, 0xD081,
	0x2040, 0x7000, 0x3010, 0x740F,
	0xC440, 0x0C42, 0x0003, 0x6266,
	0xD442, 0x343B, 0x2006, 0x4EFB,
	0x2000, 0x005E, 0x000A, 0x0016,
	0x0046, 0x2007, 0x722E, 0xD081,
	0x2040, 0x2C10, 0x6054, 0x33EE,
	0xFFFE, 0x00F4, 0x0004, 0x33FC,
	0x0005, 0x00F4, 0x0000, 0x2C39,
	0x00F4, 0x000C, 0x3A39, 0x00F4,
	0x0002, 0x6706, 0x3005, 0x6000,
	0x00C8, 0x2007, 0x722E, 0xD081,
	0x2040, 0x9C90, 0x6024, 0x2007,
	0x722E, 0xD081, 0x2040, 0x202E,
	0x000C, 0x7410, 0xD082, 0x2240,
	0x2C11, 0xDC90, 0x600C, 0x202E,
	0x000C, 0x7210, 0xD081, 0x2040,
	0x2C10, 0x2007, 0x7224, 0xD081,
	0x2040, 0x2810, 0x23C6, 0x00F4,
	0x0008, 0x23C4, 0x00F4, 0x000C,
	0x2007, 0x7420, 0xD082, 0x2040,
	0x23D0, 0x00F4, 0x0010, 0x33EE,
	0xFFFE, 0x00F4, 0x0004, 0x2007,
	0x5C80, 0x2040, 0x7000, 0x3010,
	0x363C, 0xF0FF, 0xC640, 0x0443,
	0xA002, 0x6706, 0x5343, 0x670A,
	0x6012, 0x4279, 0x00F4, 0x0000,
	0x600E, 0x33FC, 0x0001, 0x00F4,
	0x0000, 0x6004, 0x70EF, 0x6038,
	0x2839, 0x00F4, 0x000C, 0x3A39,
	0x00F4, 0x0002, 0x2007, 0x7210,
	0xD081, 0x2040, 0x3085, 0x2007,
	0x7428, 0xD082, 0x2040, 0x2084,
	0x202E, 0x000C, 0xD081, 0x2040,
	0x2004, 0xD090, 0x262E, 0x000C,
	0xD681, 0x2043, 0x2080, 0x3005,
	0x4CEE, 0x00F8, 0xFFEA, 0x4E5E,
	0x4E75, 0x4E56, 0x0000, 0x48E7,
	0x0700, 0x2A2E, 0x0008, 0x2005,
	0x721A, 0xD081, 0x2040, 0x7E00,
	0x3E10, 0x2005, 0x7416, 0xD082,
	0x2040, 0x7000, 0x3010, 0x2C00,
	0x5386, 0x2007, 0x5380, 0x6718,
	0x5980, 0x671A, 0x5380, 0x674E,
	0x5380, 0x6728, 0x0480, 0x0000,
	0x0010, 0x6758, 0x6000, 0x008C,
	0x7EFF, 0x6000, 0x0096, 0x33C6,
	0x00F4, 0x0004, 0x33FC, 0x0003,
	0x00F4, 0x0000, 0x3E39, 0x00F4,
	0x0002, 0x607E, 0x7000, 0x3006,
	0x2F00, 0x4EBA, 0xFCA0, 0x33C6,
	0x00F4, 0x0004, 0x33FC, 0x0002,
	0x00F4, 0x0000, 0x3E39, 0x00F4,
	0x0002, 0x584F, 0x605C, 0x33C6,
	0x00F4, 0x0004, 0x33FC, 0x0004,
	0x00F4, 0x0000, 0x3E39, 0x00F4,
	0x0002, 0x6046, 0x7000, 0x3006,
	0x0C40, 0x0003, 0x6504, 0x7EC8,
	0x6038, 0x7000, 0x3006, 0x4A80,
	0x660E, 0x2005, 0x721C, 0xD081,
	0x2040, 0x7003, 0x2080, 0x600E,
	0x2005, 0x721C, 0xD081, 0x2040,
	0x20BC, 0x0000, 0x0903, 0x4247,
	0x6010, 0x7000, 0x3006, 0x0C40,
	0x0003, 0x6504, 0x7EC8, 0x6002,
	0x7EEF, 0x3007, 0x4CEE, 0x00E0,
	0xFFF4, 0x4E5E, 0x4E75, 0x4E56,
	0x0000, 0x48E7, 0x1F00, 0x2A2E,
	0x0008, 0x2005, 0x7216, 0xD081,
	0x2040, 0x7000, 0x3010, 0x2800,
	0x5384, 0x2005, 0x741A, 0xD082,
	0x2040, 0x7000, 0x3010, 0x7608,
	0xB680, 0x6630, 0x7000, 0x3004,
	0x2F00, 0x4EBA, 0xFBA0, 0x2E00,
	0x584F, 0x6704, 0x78C8, 0x601E,
	0x2C05, 0x701C, 0xDC80, 0x7A0B,
	0x600A, 0x2047, 0x2246, 0x3290,
	0x5487, 0x5486, 0x5385, 0x6CF2,
	0x4244, 0x6002, 0x78EE, 0x3004,
	0x4CEE, 0x00F8, 0xFFEC, 0x4E5E,
	0x4E75, 0x4E56, 0x0000, 0x70E8,
	0x4E5E, 0x4E75
};

static void Sony_Install (void)
{
	int i;
	UWORD *pto = (UWORD *)(0x17D30 + (ULONG)ROM);
	UWORD *pfrom = (UWORD *)sony_driver;

	for (i = sizeof(sony_driver) / 2; --i >=0; ) {
		do_put_mem_word(pto, *pfrom++);
		pto++;
	}
}

Boolean ROM_Init(void)
{
  if ((do_get_mem_word(&ROM[0]) == 0x4D1E)
		&& (do_get_mem_word(&ROM[1]) == 0xEEE1))
  {
    //printf("vMac: vMac.ROM loaded successfully, Mac Plus ROM v 1, 'Lonely Hearts'.\n");
  } else if ((do_get_mem_word(&ROM[0]) == 0x4D1E)
		&& (do_get_mem_word(&ROM[1]) == 0xEAE1))
  {
    //printf("vMac: vMac.ROM loaded successfully, Mac Plus ROM v 2, 'Lonely Heifers'.\n");
  } else if ((do_get_mem_word(&ROM[0]) == 0x4D1F)
		&& (do_get_mem_word(&ROM[1]) == 0x8172))
  {
    //printf("vMac: vMac.ROM loaded successfully, Mac Plus ROM v 3, 'Loud Harmonicas'.\n");
  } else {
	//printf("vMac: vMac.ROM loaded successfully, but we don't support this ROM yet.\n");
	  MacMsg("\pUnsupported ROM.","\pvMac.ROM loaded successfully, but we don't support this ROM yet.",true);
	  return false;
  }
  do_put_mem_word(&ROM[3450/2],0x6022); /* skip the rom checksum */
#if 1
  do_put_mem_word(&ROM[3752/2],0x4E71); /* shorten the ram check read */
  do_put_mem_word(&ROM[3728/2],0x4E71); /* shorten the ram check write*/
#endif
  /* do_put_mem_word(&ROM[862/2],0x4E71); */ /* shorten set memory*/
  Sony_Install();
  return true;
}
