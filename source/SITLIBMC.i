/*
	SITLIBMC.i
	Copyright (C) 2009 Paul C. Pratt, Tom Bereiter, Nigel Perry,
		Mark G. Mendel

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
	Stuff IT (uncompressed) format LIBrary for MaCintosh
*/


/*
	adapted from code with this copyright header:

	| sit - Stuffit for UNIX
	|  Puts unix data files into stuffit archive suitable for
	|   downloading to a Mac.
	|   Automatically processes files output from xbin.
	|
	|  Reverse engineered from unsit by Allan G. Weber, which was based
	|  on macput, which was based on ...
	|  Just like unsit this uses the host's version of compress to do
	|  the work.
	|
	| Examples:
	|   1) take collection of UNIX text files and make them LSC text
	|      files when uncompressed on the mac:
	|      sit -u -T TEXT -C KAHL file ...
	|   2) Process output from xbin:
	|      xbin file1 (produces FileOne.{info,rsrc,data})
	|      sit file1
	|
	|  Tom Bereiter
	|   ..!{rutgers,ames}!cs.utexas.edu!halley!rolex!twb
	|
	| This version for CAP aufs files based on info from aufs source
	| + mcvert etc.
	| Aufs version is program is called AUFSNAME (default stuffit)
	|
	| Aug 90. Nigel Perry, np@doc.ic.ac.uk
	|
*/

/*
	which uses updcrc.c code with this copyright header:

	| updcrc(3), crc(1) - calculate crc polynomials
	|
	| Calculate, intelligently, the CRC of a dataset incrementally
	| given a buffer full at a time.
	|
	| Usage:
	|  newcrc = updcrc( oldcrc, bufadr, buflen )
	|    unsigned int oldcrc, buflen;
	|    char_ptr bufadr;
	|
	| Compiling with -DTEST creates a program to print the CRC of
	|  stdin to stdout.
	| Compile with -DMAKETAB to print values for crctab to stdout.
	|  If you change the CRC polynomial parameters, be sure to do
	|  this and change crctab's initial value.
	|
	| Notes:
	|  Regards the data stream as an integer whose MSB is the MSB of
	|  the first byte recieved.  This number is 'divided' (using xor
	|  instead of subtraction) by the crc-polynomial P.
	|  XMODEM does things a little differently, essentially treating
	|  the LSB of the first data byte as the MSB of the integer. Define
	|  SWAPPED to make things behave in this manner.
	|
	| Author: Mark G. Mendel, 7/86
	|  UUCP: ihnp4!umn-cs!hyper!mark, GEnie: mgm
*/


struct sitlib_sitHdr { /* 22 bytes */
	/* big endian format */
	OSType sig1;     /* = 'SIT!' -- for verification */
	ui4b numFiles;   /* number of files in archive */
	ui5b arcLen;     /* length of entire archive incl. */
	OSType sig2;     /* = 'rLau' -- for verification */
	ui3b version;    /* version number */
	ui3b reserved[7];
};

typedef struct sitlib_sitHdr sitlib_sitHdr;

/*
	compression methods
	define noComp  0 (* just read each byte and write it to archive *)
	define repComp 1 (* RLE compression *)
	define lpzComp 2 (* LZW compression *)
	define hufComp 3 (* Huffman compression *)
	all other numbers are reserved
*/

struct sitlib_fileHdr { /* 112 bytes */
	/* big endian format */
	ui3b compRMethod;  /* rsrc fork compression method */
	ui3b compDMethod;  /* data fork compression method */
	ui3b fName[64];    /* a STR63 */
	OSType fType;      /* file type */
	OSType fCreator;   /* creator... */
	ui4b FndrFlags; /* copy of Finder flags */
	ui5b cDate;        /* creation date */
	ui5b mDate;        /* !restored-compat w/backup prgms */
	ui5b rLen;         /* decom rsrc length */
	ui5b dLen;         /* decomp data length */
	ui5b cRLen;        /* compressed lengths */
	ui5b cDLen;
	ui4b rsrcCRC;      /* crc of rsrc fork */
	ui4b dataCRC;      /* crc of data fork */
	ui3b reserved[6];
	ui4b hdrCRC;       /* crc of file header */
};

typedef struct sitlib_fileHdr sitlib_fileHdr;

/*
	file format is:

	sitArchiveHdr
		file1Hdr
			file1RsrcFork
			file1DataFork
		file2Hdr
			file2RsrcFork
			file2DataFork
		.
		.
		.
		fileNHdr
			fileNRsrcFork
			fileNDataFork
*/


static ui4b crctab[1 << 8] = {
	0x0, 0xc0c1, 0xc181, 0x140, 0xc301, 0x3c0, 0x280, 0xc241,
	0xc601, 0x6c0, 0x780, 0xc741, 0x500, 0xc5c1, 0xc481, 0x440,
	0xcc01, 0xcc0, 0xd80, 0xcd41, 0xf00, 0xcfc1, 0xce81, 0xe40,
	0xa00, 0xcac1, 0xcb81, 0xb40, 0xc901, 0x9c0, 0x880, 0xc841,
	0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
	0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
	0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
	0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
	0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
	0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
	0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
	0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
	0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
	0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
	0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
	0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
	0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
	0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
	0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
	0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
	0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
	0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
	0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
	0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
	0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
	0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
	0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
	0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
	0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
	0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
	0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
	0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};

LOCALFUNC ui4b updcrc(ui4b icrc, ui3b *icp, int icnt)
{
	register ui4b crc = icrc;
	register ui3b *cp = icp;
	register int cnt = icnt;

	while (cnt--) {
		crc = (crc >> 8) ^ crctab[(crc & ((1 << 8) - 1)) ^ *cp++];
	}

	return crc;
}


/*
	The sitlib_r structure is used to minimize the amount of
	stack space used when recursively calling sitlib_ProcessOne
	and to hold global information that might be needed at any time.
*/

struct sitlib_r
{
	MyPtr SavepDt;
	CInfoPBRec cPB;

	ui5b uncomp_tot;

	short ofd;
};

typedef struct sitlib_r sitlib_r;

#define sitlib_gd() ((sitlib_r *)(pDt))



LOCALFUNC tMyErr sitlib_copy_v2(short src_refnum, short dst_refnum,
	uimr L, ui4b *crc)
{
	tMyErr err;
	MyPtr p;
	uimr maxn;
	uimr n;

	if (noErr == (err = TempBuffHBeginUse_v2(&p))) {
		maxn = TempBuffSize;

doGetMore:
		if (0 != L) {
			if (L > maxn) {
				n = maxn;
			} else {
				n = L;
			}
			if (noErr == (err = MyReadBytes_v2(src_refnum, p, n))) {
				*crc = updcrc(*crc, p, n);
				if (noErr == (err = MyWriteBytes_v2(dst_refnum, p, n)))
				{
					L -= n;
					goto doGetMore;
				}
			}
		}

		TempBuffHEndUse();
	}

	return err;
}

LOCALFUNC tMyErr sitlib_BeginProcessing_v2(void)
/* write blank header that will rewrite later */
{
	struct sitlib_sitHdr sh;

	sitlib_gd()->uncomp_tot = 0;

	pBytesClear((MyPtr)&sh, sizeof(sh));

	return MyWriteBytes_v2(sitlib_gd()->ofd, (MyPtr)&sh, 22);
}


LOCALFUNC tMyErr sitlib_EndProcessing_v2(ui4r numDocs)
{
	tMyErr err;
	struct sitlib_sitHdr sh;
	uimr fpos2;

	if (noErr == (err = MyOpenFileGetFPos(sitlib_gd()->ofd, &fpos2))) {
		pBytesClear((MyPtr)&sh, sizeof(sh));

		/* header header */
		sh.sig1 = 'SIT!';
		sh.numFiles = numDocs;
		sh.arcLen = fpos2;
		sh.sig2 = 'rLau';
		sh.version = 1;

		err = MyBackWriteBytes_v2(sitlib_gd()->ofd, 0, (MyPtr)&sh, 22);
	}

	return err;
}

LOCALFUNC tMyErr sitlib_ProcessRF_v2(ui4b *f_crc)
{
	tMyErr err;
	short src_refnum;
	uimr n = sitlib_gd()->cPB.hFileInfo.ioFlRLgLen;
	ui4b crc = 0;

	if (0 == n) {
		err = noErr;
	} else if (noErr == (err = CatInfoOpenReadRF_v2(&sitlib_gd()->cPB,
		&src_refnum)))
	{
		if (noErr == (err = sitlib_copy_v2(src_refnum, sitlib_gd()->ofd,
			n, &crc)))
		{
			sitlib_gd()->uncomp_tot += n;
		}
		err = ErrCombine(err, MyCloseFile_v2(src_refnum));
	}

	*f_crc = crc;

	return err;
}

LOCALFUNC tMyErr sitlib_ProcessDF_v2(ui4b *f_crc)
{
	tMyErr err;
	short src_refnum;
	uimr n = sitlib_gd()->cPB.hFileInfo.ioFlLgLen;
	ui4b crc = 0;

	if (0 == n) {
		err = noErr;
	} else if (noErr == (err = CatInfoOpenReadDF_v2(&sitlib_gd()->cPB,
		&src_refnum)))
	{
		if (noErr == (err = sitlib_copy_v2(src_refnum, sitlib_gd()->ofd,
			n, &crc)))
		{
			sitlib_gd()->uncomp_tot += n;
		}
		err = ErrCombine(err, MyCloseFile_v2(src_refnum));
	}

	*f_crc = crc;

	return err;
}

LOCALFUNC tMyErr sitlib_ProcessOne0_v2(uimr fpos1)
{
	tMyErr err;
	struct sitlib_fileHdr fh;

	pBytesClear((MyPtr)&fh, 112);
	MyMoveBytes((MyPtr)&(sitlib_gd()->cPB.hFileInfo.ioNamePtr[0]),
		(MyPtr)fh.fName, 1 + sitlib_gd()->cPB.hFileInfo.ioNamePtr[0]);
	fh.cDate = sitlib_gd()->cPB.hFileInfo.ioFlCrDat;
	fh.mDate = sitlib_gd()->cPB.hFileInfo.ioFlMdDat;
		/* write incomplete header, will seek back and fill in later */
	if (noErr == (err = MyWriteBytes_v2(sitlib_gd()->ofd,
		(MyPtr)&fh, 112)))
	{
		if (CatInfoIsFolder(&sitlib_gd()->cPB)) {
			/* ok */
		} else {
			fh.fType =
				sitlib_gd()->cPB.hFileInfo.ioFlFndrInfo.fdType;
			fh.fCreator =
				sitlib_gd()->cPB.hFileInfo.ioFlFndrInfo.fdCreator;
			fh.FndrFlags =
				sitlib_gd()->cPB.hFileInfo.ioFlFndrInfo.fdFlags;
			fh.rLen = sitlib_gd()->cPB.hFileInfo.ioFlRLgLen;
			fh.dLen = sitlib_gd()->cPB.hFileInfo.ioFlLgLen;
			fh.cRLen = sitlib_gd()->cPB.hFileInfo.ioFlRLgLen;
			fh.cDLen = sitlib_gd()->cPB.hFileInfo.ioFlLgLen;

			if (noErr == (err = sitlib_ProcessRF_v2(&fh.rsrcCRC)))
			if (noErr == (err = sitlib_ProcessDF_v2(&fh.dataCRC)))
			{
				fh.hdrCRC = updcrc(0, (ui3b *)&fh, 110);
				if (noErr == (err = MyBackWriteBytes_v2(
					sitlib_gd()->ofd, fpos1, (MyPtr)&fh, 112)))
				{
					/* ok */
				}
			}
		}
	}

	return err;
}

LOCALFUNC tMyErr sitlib_PostProcessFolder_v2(uimr fpos1,
	ui5b old_uncomp_tot)
{
	tMyErr err;
	uimr fpos2;
	struct sitlib_fileHdr fh;

	if (noErr == (err = MyOpenFileGetFPos(sitlib_gd()->ofd, &fpos2)))
	if (noErr == (err = MyBackReadBytes_v2(sitlib_gd()->ofd, fpos1,
		(MyPtr)&fh, 112)))
	{
		fh.compRMethod = fh.compDMethod = 0x21;
		fh.hdrCRC = updcrc(0, (ui3b *)&fh, 110);
		if (noErr == (err = MyWriteBytes_v2(sitlib_gd()->ofd,
			(MyPtr)&fh, 112)))
		{
			fh.compRMethod = fh.compDMethod = 0x20;
			fh.cDLen = fpos2 - fpos1;
			fh.dLen = sitlib_gd()->uncomp_tot - old_uncomp_tot;
			fh.hdrCRC = updcrc(0, (ui3b *)&fh, 110);
			if (noErr == (err = MyBackWriteBytes_v2(sitlib_gd()->ofd,
				fpos1, (MyPtr)&fh, 112)))
			{
				/* ok */
			}
		}
	}

	return err;
}

LOCALFUNC tMyErr sitlib_ProcessOne_v2(void)
{
	tMyErr err;
	uimr fpos1;

	if (noErr == (err = MyOpenFileGetFPos(sitlib_gd()->ofd, &fpos1)))
	if (noErr == (err = sitlib_ProcessOne0_v2(fpos1)))
	{
		if (! CatInfoIsFolder(&sitlib_gd()->cPB)) {
			/* ok */
		} else {
			MyDir_R old_d;
			ui5b old_uncomp_tot = sitlib_gd()->uncomp_tot;
			int index = 1;

			MyCatInfoGetMyDir(&sitlib_gd()->cPB, &old_d);

Label_1:
			err = MyCatGetNextChild_v2(&sitlib_gd()->cPB,
				&old_d, &index);
			if (noErr == err) {
				if (noErr == (err = sitlib_ProcessOne_v2())) {
					goto Label_1;
				}
			} else if (fnfErr == err) {
				err = sitlib_PostProcessFolder_v2(fpos1,
					old_uncomp_tot);
			}
		}
	}

	return err;
}

GLOBALFUNC tMyErr sitlib_WriteFile_v2(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s)
{
	tMyErr err;
	sitlib_r r;
	MyPStr NameBuffer;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	if (noErr == (err = MyOpenNewFile_v3(dst_d, dst_s,
		'SIT!', 'SIT!', &r.ofd)))
	{
		if (noErr == (err = MyFileGetCatInfo_v2(src_d, src_s,
			NameBuffer, &r.cPB)))
		if (noErr == (err = sitlib_BeginProcessing_v2()))
		if (noErr == (err = sitlib_ProcessOne_v2()))
		if (noErr == (err = sitlib_EndProcessing_v2(1)))
		{
		}

		err = MyCloseNewFile_v3(r.ofd, dst_d, dst_s, err);
	}

	pDt = r.SavepDt;

	return err;
}
