
typedef unsigned long ULONG;
typedef unsigned short UWORD;
typedef unsigned char UBYTE;

typedef long LONG;
typedef short WORD;
typedef char BYTE;

#define Boolean UBYTE
#define true 1
#define false 0

typedef short OSErr;
typedef unsigned short UInt16;
typedef unsigned long UInt32;
typedef UInt16 MacOSEventKind;
typedef char *Ptr;
typedef long Size;

#define ONEWORDINLINE(trapNum) = trapNum

#pragma parameter __D0 PostEvent(__A0, __D0)
extern pascal OSErr PostEvent(MacOSEventKind eventNum, UInt32 eventMsg)
	ONEWORDINLINE(0xA02F);

#pragma parameter __A0 NewPtrSysClear(__D0)
extern pascal Ptr NewPtrSysClear(Size byteCount)
	ONEWORDINLINE(0xA71E);

#pragma parameter AddDrive0(__A0, __D0)
extern pascal void AddDrive0(UInt32 qEl, UInt32 arg2)
	ONEWORDINLINE(0xA04E);

#define CPTR ULONG

#define get_long(x) (*((ULONG *)(x)))
#define get_word(x) (*((UWORD *)(x)))
#define get_byte(x) (*((UBYTE *)(x)))
#define put_long(x, v) (*((ULONG *)(x))) = (v)
#define put_word(x, v) (*((UWORD *)(x))) = (v)
#define put_byte(x, v) (*((UBYTE *)(x))) = (v)

#define NULL ((void *)0)

#define NumDrives 3

// Sony_Table Indexes

#define kOpen    0
#define kPrime   1
#define kControl 2
#define kStatus  3
#define kClose   4

// Sony Variable Drive Offsets

// Sony Variable Drive Setting Offsets

#define kTrack       0 // Current Track
#define kWriteProt   2 // FF if Write Protected, 00 if readable
#define kDiskInPlace 3 // 00 = No Disk, 01 = Disk In, 2 = MacOS Read, FC-FF = Just Ejected
#define kInstalled   4 // 00 = Unknown, 01 = Installed, FF = Not Installed
#define kSides       5 // 00 if Single Sided Drive, FF if Doubled Sided Drive
#define kQLink       6 // Link to Next Drive
#define kQType      10 // Drive Type (0 = Size Saved, 1 = Very Large)
#define kQDriveNo   12 // Drive Number (1 = Internal, 2 = External)
#define kQRefNum    14 // Driver Reference Number (-5 for .Sony, FFFB)
#define kQFSID      16 // File System ID (0 = MacOS)
#define kTwoSideFmt 18 // FF if double-sided format, 00 if single-sided format
#define kNewIntf    19 // FF if new 800K interface or 00 if old 400K interface
#define kDriveErrs  20 // Drive Soft Errors

// Sony Driver Control Call csCodes

#define kKillIO             1
#define kVerifyDisk         5
#define kFormatDisk         6
#define kEjectDisk          7
#define kSetTagBuffer       8
#define kTrackCacheControl  9
#define kReturnDriveInfo   23

// Sony Driver Status Call csCodes

#define kDriveStatus  8

// Parameter Block Offsets

#define kqLink         0
#define kqType         4
#define kioTrap        6
#define kioCmdAddr     8
#define kioCompletion 12
#define kioResult     16
#define kioNamePtr    18
#define kioVRefNum    22
#define kioRefNum     24
#define kcsCode       26
#define kcsParam      28
#define kioBuffer     32 // Buffer to store data into
#define kioReqCount   36 // Requested Number of Bytes
#define kioActCount   40 // Actual Number of Bytes obtained
#define kioPosMode    44 // Positioning Mode
#define kioPosOffset  46 // Position Offset

// Positioning Modes

#define kfsAtMark    0 // At Mark (Ignore PosOffset)
#define kfsFromStart 1 // At Start (PosOffset is absolute)
#define kfsFromLEOF  2 // From Logical End of File - PosOffset
#define kfsFromMark  3 // At Mark + PosOffset

// Device Control Entry Offsets

#define kdCtlPosition 16

#define kDSK_Block_Base 0x00F40000

#define kDSK_Command 0
#define kDSK_Err 1
#define kDSK_Drive_No 2

#define kDSK_Start_Hi 4
#define kDSK_Start_Lo 5
#define kDSK_Count_Hi 6
#define kDSK_Count_Lo 7
#define kDSK_Buffer_Hi 8
#define kDSK_Buffer_Lo 9
#define kDSK_CallBack_Hi 10
#define kDSK_CallBack_Lo 11
#define kDSK_numvars 12

#define kDSKCmdRead 0
#define kDSKCmdWrite 1
#define kDSKCmdEject 2
#define kDSKCmdVerify 3
#define kDSKCmdFormat 4
#define kDSKCmdGetSize 5

#define kDSKCmdOpen 8
#define kDSKCmdPrime 9
#define kDSKCmdControl 10
#define kDSKCmdStatus 11
#define kDSKCmdClose 12

#define SonyVarsPtr 0x0134
#define FirstDriveVarsOffset 74
#define EachDriveVarsSize 66

extern void DUpdate(void);

static ULONG DriveVarsLocation(UWORD Drive_No)
{
	if (Drive_No < NumDrives) {
		CPTR SonyVars = get_long(SonyVarsPtr);
		return SonyVars + FirstDriveVarsOffset + EachDriveVarsSize * Drive_No;
	} else {
		return (ULONG)NULL;
	}
}

static void Sony_Eject_Notify (UWORD Drive_No)
{
	CPTR dvl;

	dvl = DriveVarsLocation(Drive_No);
	if (dvl != (ULONG)NULL) {
		put_byte(dvl + kWriteProt, 0x00);   // Drive Writeable
		put_byte(dvl + kDiskInPlace, 0x00); // Drive No Disk
		put_byte(dvl + kTwoSideFmt, 0x00);  // Drive Single Format (Initially)
	}
}

void Sony_Update (ULONG data)
{
	WORD i = data & 0x0000FFFF;
	CPTR dvl = DriveVarsLocation(i);

	if (get_byte(dvl + kDiskInPlace) == 0x00) {
		put_byte(dvl + kWriteProt, data >> 16);
		put_byte(dvl + kDiskInPlace, 0x01); // Drive Disk Inserted
		put_byte(dvl + kTwoSideFmt, 0xFF);  // Drive Double Format

		PostEvent(7 /* Disk Inserted */, i + 1 /* Drive */);
	}
	/* else, if disk already in place, a mistake has been made */
}

// Sony_Open
//
// This opens up the Sony driver, it allocates a pointer for the variables
// Sets SonyVars, then sets up the drive variables with correct values
// And then adds the drives into the drive queue
//
// Bill Huey comment ---
//
// It wasn't obvious initially, but what "Opening" the ".Sony" driver doesn't
// mean opening a file, which is pretty stupid of me. I keeped wondering
// where and how the drive structures where being set at start up, only to realize
// that it's right under my nose in the function Sony_Open.
// That's what I get for not paying attention...
//
WORD Sony_Open(CPTR ParamBlk, CPTR DeviceCtl)
{
#pragma unused(ParamBlk, DeviceCtl)
	WORD result;
	WORD i;
	CPTR dvl;
	CPTR SonyVars;


	if (get_long((CPTR)(kDSK_Block_Base + 2 * kDSK_CallBack_Hi)) != 0) { // Don't Open Again
		result = 0x0000; // No Error
	} else {
		SonyVars = (CPTR)NewPtrSysClear(0x00000310);  // Size of Sony Variables
		if (SonyVars != (CPTR)NULL) {
			put_long(SonyVarsPtr, SonyVars); // Save it to SonyVars

			for (i = 0; (dvl = DriveVarsLocation(i)) != (ULONG)NULL; ++i) {
				put_word(dvl + kTrack, 0x0000);     // Drive i at Track 0
				put_byte(dvl + kWriteProt, 0x00);   // Drive i Writeable
				put_byte(dvl + kDiskInPlace, 0x00); // Drive i No Disk
				put_byte(dvl + kInstalled, 0x01);   // Drive i Installed
				put_byte(dvl + kSides, 0xFF);       // Drive i Double Sided
				put_word(dvl + kQDriveNo, i + 1);   // Drive i is Drive 1
				put_word(dvl + kQRefNum, 0xFFFB);   // Drive i uses .Sony
				put_byte(dvl + kTwoSideFmt, 0x00);  // Drive i Single Format (Initially)
				put_byte(dvl + kNewIntf, 0xFF);     // Drive i uses new interface
				put_word(dvl + kDriveErrs, 0x0000); // Drive i has no errors

				AddDrive0(dvl + kQLink, ((ULONG)(i + 1) << 16) + 0xFFFB); // Drive i (Hi Word), Driver FFFB (Lo Word)
			}

			put_long((CPTR)(kDSK_Block_Base + 2 * kDSK_CallBack_Hi), (ULONG)&DUpdate);
		}

		result = 0x0000; // No Error
	}
	return result;
}

// Sony_Prime
// Handles I/O to disks
WORD Sony_Prime(CPTR ParamBlk, CPTR DeviceCtl)
{
	WORD result;
	ULONG Sony_Count;
	ULONG Sony_Start;
	UWORD Drive_No = get_word(ParamBlk + kioVRefNum) - 1;
	CPTR dvl = DriveVarsLocation(Drive_No);

	if (dvl != (ULONG)NULL) {
		CPTR DiskInPlaceA = dvl + kDiskInPlace;
		UBYTE DiskInPlaceV = get_byte(DiskInPlaceA);

		if (DiskInPlaceV != 0x02) {
			if (DiskInPlaceV == 0x01) {
				put_byte(DiskInPlaceA, 0x02); // Clamp Drive
			} else {
				return 0xFFBF; // Say it's offline (-65)
				/*
					if don't check for this, will go right
					ahead and boot off a disk that hasn't
					been mounted yet by Sony_Update.
					(disks other than the boot disk aren't
					seen unless mounted by Sony_Update)
				*/
			}
		}
	}

	switch (get_word(ParamBlk + kioPosMode) & 0x0F) {
		case kfsFromStart:
			Sony_Start = get_long(ParamBlk + kioPosOffset);
			break;
		case kfsFromLEOF:
			put_word(kDSK_Block_Base + 2 * kDSK_Drive_No, Drive_No);
			put_word(kDSK_Block_Base, kDSKCmdGetSize);
			Sony_Start = get_long(kDSK_Block_Base + 2 * kDSK_Count_Hi);
			result = (WORD)get_word(kDSK_Block_Base + 2 * kDSK_Err);
			if (result != 0) {
				return result;
			}
			Sony_Start -= get_long(ParamBlk + kioPosOffset);
			break;
		case kfsFromMark:
			Sony_Start = get_long(ParamBlk + kioPosOffset) +
				get_long(DeviceCtl + kdCtlPosition);
			break;
		case kfsAtMark:
		default:
			Sony_Start = get_long(DeviceCtl + kdCtlPosition);
			break;
	}

	Sony_Count = get_long(ParamBlk + kioReqCount);

	put_long((CPTR)(kDSK_Block_Base + 2 * kDSK_Start_Hi), Sony_Start);
	put_long((CPTR)(kDSK_Block_Base + 2 * kDSK_Count_Hi), Sony_Count);
	put_long((CPTR)(kDSK_Block_Base + 2 * kDSK_Buffer_Hi), get_long(ParamBlk + kioBuffer));
	put_word(kDSK_Block_Base + 2 * kDSK_Drive_No, Drive_No);

	switch (get_word(ParamBlk + kioTrap) & 0xF0FF) {
		case 0xA002: // Reading
			put_word(kDSK_Block_Base, kDSKCmdRead);
			break;
		case 0xA003: // Writing
			put_word(kDSK_Block_Base, kDSKCmdWrite);
			break;
		default:
			return 0xFFEF; // Driver can't respond to this Control call (-17)
			break;
	}

	Sony_Count = get_long(kDSK_Block_Base + 2 * kDSK_Count_Hi);
	result = (WORD)get_word(kDSK_Block_Base + 2 * kDSK_Err);

	put_word(ParamBlk + kioResult, result);
	put_long(ParamBlk + kioActCount, Sony_Count);
	put_long(DeviceCtl + kdCtlPosition, get_long(DeviceCtl + kdCtlPosition) + Sony_Count);

	return result;
}

// Sony_Control
// Implements control csCodes for the Sony driver
WORD Sony_Control (CPTR ParamBlk, CPTR DeviceCtl)
{
#pragma unused(DeviceCtl)
	WORD result;
	ULONG OpCode = get_word(ParamBlk + kcsCode);
	UWORD Drive_No = get_word(ParamBlk + kioVRefNum) - 1;

	switch (OpCode) {
		case kKillIO :
			result = 0xFFFF;
			break;
		case kVerifyDisk :
			put_word(kDSK_Block_Base + 2 * kDSK_Drive_No, Drive_No);
			put_word(kDSK_Block_Base, kDSKCmdVerify);
			result = (WORD)get_word(kDSK_Block_Base + 2 * kDSK_Err);
			break;
		case kEjectDisk :
			Sony_Eject_Notify(Drive_No);
			put_word(kDSK_Block_Base + 2 * kDSK_Drive_No, Drive_No);
			put_word(kDSK_Block_Base, kDSKCmdEject);
			result = (WORD)get_word(kDSK_Block_Base + 2 * kDSK_Err);
			break;
		case kFormatDisk :
			put_word(kDSK_Block_Base + 2 * kDSK_Drive_No, Drive_No);
			put_word(kDSK_Block_Base, kDSKCmdFormat);
			result = (WORD)get_word(kDSK_Block_Base + 2 * kDSK_Err);
			break;
		case kReturnDriveInfo :
			if (Drive_No >= NumDrives) {
				result = 0xFFC8; // No Such Drive (-56)
			} else {
				if (Drive_No == 0) {
					put_long(ParamBlk + kcsParam, 0x00000003); // 800K, Primary Internal Drive
				} else {
					put_long(ParamBlk + kcsParam, 0x00000903); // 800K, Secondary External Drive
						/* or is it 0x00000093 ?? */
				}
				result = 0x0000; // No error (0)
			}
			break;
		default :
		{
			if (Drive_No >= NumDrives) {
				result = 0xFFC8; // No Such Drive (-56)
			} else {
				result = 0xFFEF; // Driver can't respond to this Control call (-17)
			}
			break;
		}
	}
	return result;
}

// Sony_Status
// Handles the DriveStatus call
WORD Sony_Status (CPTR ParamBlk, CPTR DeviceCtl)
{
#pragma unused(DeviceCtl)
	CPTR Src;
	CPTR Dst;
	WORD result;
	int Counter;
	UWORD Drive_No = get_word(ParamBlk + kioVRefNum) - 1;

	if (get_word(ParamBlk + kcsCode) == kDriveStatus) { // Drive Status Call
		Src = DriveVarsLocation(Drive_No);
		if (Src != (ULONG)NULL) {
			result = 0xFFC8; // No Such Drive (-56)
		} else {
			Dst = ParamBlk + kcsParam;
			for (Counter = 11; --Counter >=0; ) {
				put_word(Dst, get_word(Src));
				Src += 2; Dst += 2;
			}
			result = 0x0000;
		}
	} else {
		result = 0xFFEE; // Can't Respond to Status Call (-18)
	}
	return result;
}

// Sony_Close
// Returns immediate error
WORD Sony_Close(CPTR ParamBlk, CPTR DeviceCtl)
{
#pragma unused(ParamBlk, DeviceCtl)
	return 0xFFE8; // Can't Close Driver (-24)
}
