/*
	SCSIEMDV.c
	
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
	Small Computer System Interface EMulated DeVice
	
	Emulates the SCSI found in the Mac Plus.

	This code adapted from "SCSI.c" in vMac by Philip Cummins.
*/

#include "SYSDEPNS.h"
#include "SCSIEMDV.h"

// Lot's of additions by Michael Hanni (mhanni@sprintmail.com)
   
// PC - SCSI Memory Handlers

// SCSI Transfer Instruction Block

#define kScOpcode 0  // word    operation code
#define kScParam1 2  // long    first parameter
#define kScParam2 6  // long    second parameter

// SCSI Driver Descriptor Record

#define kSbSig 		0   // word    device signature
#define kSbBlkSize 	2   // word    block size of the device
#define kSbBlkCount 	4   // long    number of blocks on the device
#define kSbDevType 	8   // word    reserved
#define kSbDevId 	10  // word    reserved
#define kSbData 	12  // long    reserved
#define kSbDrvrCount 	16  // word    number of driver descriptor entries
#define kDdBlock 	18  // long    first driver's starting block
#define kDdSize 	22  // word    driver's size, in 512-byte blocks
#define kDdType 	24  // word    operating system type (MacOS = 1)
#define kDdPad 		26  // 486 bytes    additional drivers, if any

// SCSI Partition Map Entry Record

#define kPmSig 		0   //   word    partition signature
#define kPmSigPad 	2   //   word    reserved
#define kPmMapBlkCnt 	4   //   long    number of blocks in partition map
#define kPmPyPartStart 	8   //   long    first physical block of partition
#define kPmPartBlkCnt 	12  //   long    number of blocks in partition
#define kPmPartName 	16  //   32 bytes    partition name
#define kPmParType 	48  //   32 bytes    partition type
#define kPmLgDataStart 	80  //   long    first logical block of data area
#define kPmDataCnt 	84  //   long    number of blocks in data area
#define kPmPartStatus 	88  //   long    partition status information
#define kPmLgBootStart 	92  //   long    first logical block of boot code
#define kPmBootSize 	96  //   long    size of boot code, in bytes
#define kPmBootAddr 	100 //   long    boot code load address
#define kPmBootAddr2 	104 //   long    reserved
#define kPmBootEntry 	108 //   long    boot code entry point
#define kPmBootEntry2 	112 //   long    reserved
#define kPmBootCksum 	116 //   long    boot code checksum
#define kPmProcessor 	120 //   16 bytes    processor type
#define kPmPad 		136 //   376 bytes    reserved

// SCSI Errors.

#define kNoErr 		  0   //  No error
#define kScCommErr   	  2   //  Communications error, operation timeout
#define kScArbNBErr    	  3   //  Bus busy, arbitration timeout
#define kScBadParmsErr    4   //  Bad parameter or unrecognized TIB instruction
#define kScPhaseErr    	  5   //  Phase error on the SCSI bus
#define kScCompareErr     6   //  Comparison error from scComp instruction
#define kScMgrBusyErr     7   //  SCSI Manager busy
#define kScSequenceErr    8   //  Attempted operation is out of sequence
#define kScBusTOErr    	  9   //  Bus timeout during blind transfer
#define kScComplPhaseErr  10  //  SCSI bus was not in status phase on entry to SCSIComplete

// SCSI constants.

#define kScInc   	1     // transfer data, increment buffer pointer
#define kScNoInc    	2     // transfer data, don't increment pointer
#define kScAdd    	3     // add long to address
#define kScMove		4     // move long to address
#define kScLoop    	5     // decrement counter and loop if > 0
#define kScNop    	6     // no operation
#define kScStop    	7     // stop TIB execution
#define kScComp    	8     // compare SCSI data with memory

// SCSI signature values.

#define kSbSIGWord    	$4552  // driver descriptor map signature
#define kpMapSIG    	$504D  // partition map signature

void  SCSI_Reset (void)
{
}

extern ULONG DataBus;
extern blnr ByteSizeAccess;
extern blnr WriteMemAccess;

void SCSI_Access(CPTR addr)
{
	UnusedParam(addr);

	if (WriteMemAccess) {
		if (ByteSizeAccess) {
			#ifdef _SCSI_Debug
			printf("SCSI PB %4lx %2x\n", addr, in);
			#endif
			// if (!(addr & 1))
			// {
			//   RAM [addr >> 1] = (RAM [addr >> 1] & 0xff) | (((UWORD) in) << 8);
			// }
			// else
			// {
			//   RAM [addr >> 1] = (RAM [addr >> 1] & 0xff00) | in;
			// }
		} else {
			// RAM [addr >> 1] = in;
		}
	} else {
		if (ByteSizeAccess) {
			#ifdef _SCSI_Debug
			printf("SCSI GB %4lx\n", addr);
			#endif
			// return RAM [addr >> 1] >> (addr & 1 ? 0 : 8);
		} else {
			// return RAM [addr >> 1];
		}
	}
}
