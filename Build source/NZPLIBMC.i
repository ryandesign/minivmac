/*
	NZPLIBMC.i
	Copyright (C) 2007 Paul C. Pratt, Mark Adler, and many others

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
	No compression ZiP format LIBrary for MaCintosh
*/


/*
	adapted from zip code with this copyright header:

	Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

	See the accompanying file LICENSE, version 1999-Oct-05 or later
	(the contents of which are also included in zip.h) for terms of use.
	If, for some reason, both of these files are missing, the Info-ZIP license
	also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/


/* -------- from "zip.h" -------- */

/*
This is version 1999-Oct-05 of the Info-ZIP copyright and license.
The definitive version of this document should be available at
ftp://ftp.cdrom.com/pub/infozip/license.html indefinitely.


Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

For the purposes of this copyright and license, "Info-ZIP" is defined as
the following set of individuals:

	Mark Adler, John Bush, Karl Davis, Harald Denker, Jean-Michel Dubois,
	Jean-loup Gailly, Hunter Goatley, Ian Gorman, Chris Herborth, Dirk Haase,
	Greg Hartwig, Robert Heath, Jonathan Hudson, Paul Kienitz, David Kirschbaum,
	Johnny Lee, Onno van der Linden, Igor Mandrichenko, Steve P. Miller,
	Sergio Monesi, Keith Owens, George Petrov, Greg Roelofs, Kai Uwe Rommel,
	Steve Salisbury, Dave Smith, Christian Spieler, Antoine Verheijen,
	Paul von Behren, Rich Wales, Mike White

This software is provided "as is," without warranty of any kind, express
or implied.  In no event shall Info-ZIP or its contributors be held liable
for any direct, indirect, incidental, special or consequential damages
arising out of the use of or inability to use this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

	1. Redistributions of source code must retain the above copyright notice,
		definition, disclaimer, and this list of conditions.

	2. Redistributions in binary form must reproduce the above copyright
		notice, definition, disclaimer, and this list of conditions in
		documentation and/or other materials provided with the distribution.

	3. Altered versions--including, but not limited to, ports to new operating
		systems, existing ports with new graphical interfaces, and dynamic,
		shared, or static library versions--must be plainly marked as such
		and must not be misrepresented as being the original source.  Such
		altered versions also must not be misrepresented as being Info-ZIP
		releases--including, but not limited to, labeling of the altered
		versions with the names "Info-ZIP" (or any variation thereof, including,
		but not limited to, different capitalizations), "Pocket UnZip," "WiZ"
		or "MacZip" without the explicit permission of Info-ZIP.  Such altered
		versions are further prohibited from misrepresentative use of the
		Zip-Bugs or Info-ZIP e-mail addresses or of the Info-ZIP URL(s).

	4. Info-ZIP retains the right to use the names "Info-ZIP," "Zip," "UnZip,"
		"WiZ," "Pocket UnZip," "Pocket Zip," and "MacZip" for its own source and
		binary releases.
*/


/* MSDOS file or directory attributes */
#define MSDOS_HIDDEN_ATTR 0x02
#define MSDOS_DIR_ATTR 0x10


/* internal file attribute */
#define zipatt_UNKNOWN (-1)
#define zipatt_BINARY  0
#define zipatt_ASCII   1
#define zipatt_EBCDIC 2

/* Definitions for extra field handling: */
#define EF_SIZE_MAX  ((unsigned)0xFFFF) /* hard limit of total e.f. length */
#define EB_HEADSIZE       4     /* length of a extra field block header */
#define EB_ID             0     /* offset of block ID in header */
#define EB_LEN            2     /* offset of data length field in header */

#define EB_UX_MINLEN      8     /* minimal "UX" field contains atime, mtime */
#define EB_UX_ATIME       0     /* offset of atime in "UX" extra field data */
#define EB_UX_MTIME       4     /* offset of mtime in "UX" extra field data */

#define EB_UX_FULLSIZE    12    /* full "UX" field (atime, mtime, uid, gid) */
#define EB_UX_UID         8     /* byte offset of UID in "UX" field data */
#define EB_UX_GID         10    /* byte offset of GID in "UX" field data */

#define EB_UT_MINLEN      1     /* minimal UT field contains Flags byte */
#define EB_UT_FLAGS       0     /* byte offset of Flags field */
#define EB_UT_TIME1       1     /* byte offset of 1st time value */
#define EB_UT_FL_MTIME    (1 << 0)      /* mtime present */
#define EB_UT_FL_ATIME    (1 << 1)      /* atime present */
#define EB_UT_FL_CTIME    (1 << 2)      /* ctime present */
#define EB_UT_LEN(n)      (EB_UT_MINLEN + 4 * (n))

#define EB_UX2_MINLEN     4     /* minimal Ux field contains UID/GID */
#define EB_UX2_UID        0     /* byte offset of UID in "Ux" field data */
#define EB_UX2_GID        2     /* byte offset of GID in "Ux" field data */
#define EB_UX2_VALID      (1 << 8)      /* UID/GID present */

#define zipmeth_STORE 0                 /* Store method */
#define zipmeth_DEFLATE 8               /* Deflation method*/


/* -------- from "crc32.c" -------- */

/*
	crc32.c -- compute the CRC-32 of a data stream
	Copyright (C) 1995 Mark Adler
	For conditions of distribution and use, see copyright notice in zlib.h
*/


/*
	Table of CRC-32's of all single-byte values (made by make_crc_table)
*/
static const ui5b crc_table0[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

/* ========================================================================= */
static ui5r crc32(register ui5r crc, /* crc shift register */
	register const ui3b *buf, /* pointer to bytes to pump through */
	ui5r len) /* number of bytes in buf[] */
/*
	Run a set of bytes through the crc shift register.  If buf is a NULL
	pointer, then initialize the crc shift register contents instead.
	Return the current crc in either case.
*/
{
	register const ui5b *crc_table = crc_table0;

	crc = crc ^ 0xffffffffL;
	if (len) do {
		crc = crc_table[((int)(crc) ^ (*buf++)) & 0xff] ^ ((crc) >> 8);
	} while (--len);
	return crc ^ 0xffffffffL;     /* (instead of ~c for 64-bit machines) */
}


/* -- Structure of a ZIP file -- */

/* Signatures for zip file information headers */
#define ziphdr_LOCSIG     0x04034b50L
#define ziphdr_CENSIG     0x02014b50L
#define ziphdr_ENDSIG     0x06054b50L
#define ziphdr_EXTLOCSIG  0x08074b50L

/* Offsets of values in headers */

#define zip_EXTCRC  0               /* uncompressed crc-32 for file */
#define zip_EXTSIZ  4               /* compressed size in zip file */
#define zip_EXTLEN  8               /* uncompressed size */




struct LSBui5
{
	ui3b v[4];
};

typedef struct LSBui5 LSBui5;

LOCALPROC PutLSBui5(LSBui5 *r, ui5b x)
{
	r->v[0] = (x      ) & 0xFF;
	r->v[1] = (x >>  8) & 0xFF;
	r->v[2] = (x >> 16) & 0xFF;
	r->v[3] = (x >> 24) & 0xFF;
}

struct LSBui4
{
	ui3b v[2];
};

typedef struct LSBui4 LSBui4;

LOCALPROC PutLSBui4(LSBui4 *r, ui4b x)
{
	r->v[0] = (x      ) & 0xFF;
	r->v[1] = (x >>  8) & 0xFF;
}

struct ziploc_r
{
	LSBui5 sig;
	LSBui4 ver; /* 00 version needed to extract */
	LSBui4 flg; /* 02 encrypt, deflate flags */
	LSBui4 how; /* 04 compression method */
	LSBui5 tim; /* 06 last modified file time & date, DOS format */
	LSBui5 crc; /* 10 uncompressed crc-32 for file */
	LSBui5 siz; /* 14 compressed size in zip file */
	LSBui5 len; /* 18 uncompressed size */
	LSBui4 nam; /* 22 length of filename */
	LSBui4 ext; /* 24 length of extra field */
	/* LOCHEAD     26 */
};

typedef struct ziploc_r ziploc_r;

struct zipcen_r
{
	LSBui5 sig;
	LSBui4 vem; /* 00 version made by */
	LSBui4 ver; /* 02 version needed to extract */
	LSBui4 flg; /* 04 encrypt, deflate flags */
	LSBui4 how; /* 06 compression method */
	LSBui5 tim; /* 08 last modified file time & date, DOS format */
	LSBui5 crc; /* 12 uncompressed crc-32 for file */
	LSBui5 siz; /* 16 compressed size in zip file */
	LSBui5 len; /* 20 uncompressed size */
	LSBui4 nam; /* 24 length of filename */
	LSBui4 ext; /* 26 length of extra field */
	LSBui4 com; /* 28 file comment length */
	LSBui4 dsk; /* 30 disk number start */
	LSBui4 att; /* 32 internal file attributes */
	LSBui5 atx; /* 34 external file attributes */
	LSBui5 off; /* 38 relative offset of local header */
	/* CENHEAD     42 */
};

typedef struct zipcen_r zipcen_r;

struct zipend_r
{
	LSBui5 sig;
	LSBui4 dsk; /* 00 number of this disk */
	LSBui4 beg; /* 02 number of the starting disk */
	LSBui4 sub; /* 04 entries on this disk */
	LSBui4 tot; /* 06 total number of entries */
	LSBui5 siz; /* 08 size of entire central directory */
	LSBui5 off; /* 12 offset of central on starting disk */
	LSBui4 com; /* 16 length of zip file comment */
	/* ENDHEAD     18 */
};

typedef struct zipend_r zipend_r;

LOCALFUNC blnr xbh_WriteToFile(xbh_r *r, short refNum)
{
	Handle h = xbh_GetH(r);
	ui5r n = xbh_GetLen(r);
	blnr IsOk;

	HLock(h);
	IsOk = MyWriteBytes(refNum, (MyPtr)*h, n);
	HUnlock(h);

	return IsOk;
}

#define zip_NAME_MAX    1024

struct nzplib_r
{
	MyPtr SavepDt;
	CInfoPBRec cPB;

	short ofd;

	xbh_r CurFileDosPath;
	xbh_r hCentral;
	long CentralCount;
};

typedef struct nzplib_r nzplib_r;

#define nzplib_gd() ((nzplib_r *)(pDt))

#define DOSTIME_MINIMUM         ((ui5r)0x00210000L)
#define DOSTIME_2038_01_18      ((ui5r)0x74320000L)

static ui5r dostime(int y, /* year */
	int n, /* month */
	int d, /* day */
	int h, /* hour */
	int m, /* minute */
	int s) /* second */
/*
	Convert the date y/n/d and time h:m:s to a four byte DOS date and
	time (date in high two bytes, time in low two bytes allowing magnitude
	comparison).
*/
{
	return y < 1980 ? DOSTIME_MINIMUM /* dostime(1980, 1, 1, 0, 0, 0) */ :
		(((ui5r)y - 1980) << 25) | ((ui5r)n << 21) | ((ui5r)d << 16) |
		((ui5r)h << 11) | ((ui5r)m << 5) | ((ui5r)s >> 1);
}

static ui5r MacTime2DosTime(unsigned long ModDate)
{
	DateTimeRec d;

	Secs2Date(ModDate, &d);
	return dostime(d.year, d.month, d.day,
		d.hour, d.minute, d.second);
}

static blnr nzplib_copy(short fdin, short fdout, uimr L, ui5r *crc)
{
	uimr maxn;
	MyPtr p;
	uimr n;
	blnr IsOk = falseblnr;

	if (TempBuffHBeginUse(&p)) {
		maxn = TempBuffSize;

doGetMore:
		if (0 == L) {
			IsOk = trueblnr;
		} else {
			if (L > maxn) {
				n = maxn;
			} else {
				n = L;
			}
			if (MyReadBytes(fdin, p, n)) {
				*crc = crc32(*crc, (ui3b *) p, n);
				if (MyWriteBytes(fdout, p, n)) {
					L -= n;
					goto doGetMore;
				}
			}
		}

		TempBuffHEndUse();
	}

	return IsOk;
}

static blnr nzplib_ProcessDF(ui5r *crc)
{
	short fd;
	uimr n = nzplib_gd()->cPB.hfileInfo.ioFlLgLen;
	blnr IsOk = falseblnr;

	if (0 == n) {
		IsOk = trueblnr;
	} else if (CatInfoOpenReadDF(&nzplib_gd()->cPB, &fd)) {
		if (nzplib_copy(fd, nzplib_gd()->ofd,
			n, crc))
		{
			IsOk = trueblnr;
		}
		(void) MyCloseFile(fd);
	}

	return IsOk;
}

static blnr nzplib_ProcessOne0(void)
{
	ziploc_r locr;

	ui4r l_ver;
	ui4r l_flg;
	ui4r l_how;
	ui5r l_tim;
	ui5r l_nam; /* offset of ext must be >= LOCHEAD */
	ui4r l_att;
	ui4r l_lflg; /* offset of lflg must be >= LOCHEAD */
	ui5r l_atx;
	long l_off;
	ui5r isize; /* input file size. */
	ui5r crc; /* crc on uncompressed file data */
	ui5r s; /* size of compressed data */
	blnr IsOk = falseblnr;

	/*
		Fill in header information and write local header to zip file.
		This header will later be re-written since compressed length and
		crc are not yet known.
	*/

	crc = 0;
	s = 0;
	isize = 0L;

	l_nam = xbh_GetLen(&nzplib_gd()->CurFileDosPath);
	l_atx = 0; /* may be changed by set_extra_field() */


	/*
		The times returned by the Mac file system are in the
		local time zone.  We convert them to GMT so that the
		epoch starts from GMT.  This is also consistent with
		what is returned from "clock seconds".
	*/
	l_tim = MacTime2DosTime(nzplib_gd()->cPB.hfileInfo.ioFlMdDat);

	l_ver = (ui4r)10; /* Need PKUNZIP 2.0 except for store */
	l_flg = 0;  /* to be updated later */
	l_lflg = l_flg;
	l_how = (ui4r)zipmeth_STORE; /* may be changed later */
	l_att = zipatt_BINARY; /* set sensible value in header */
	if (CatInfoIsFolder(&nzplib_gd()->cPB)) {
		l_atx = MSDOS_DIR_ATTR;
	} else {
		l_atx = 0;
	}

	PutLSBui5(&locr.sig, ziphdr_LOCSIG);
	PutLSBui4(&locr.ver, l_ver);
	PutLSBui4(&locr.flg, l_lflg);
	PutLSBui4(&locr.how, l_how);
	PutLSBui5(&locr.tim, l_tim);
	PutLSBui5(&locr.crc, crc);
	PutLSBui5(&locr.siz, s);
	PutLSBui5(&locr.len, isize);
	PutLSBui4(&locr.nam, l_nam);
	PutLSBui4(&locr.ext, 0);

	if (CheckSysErr(GetFPos(nzplib_gd()->ofd, &l_off)))
	if (MyWriteBytes(nzplib_gd()->ofd, (MyPtr)&locr, sizeof(ziploc_r)))
	if (xbh_WriteToFile(&nzplib_gd()->CurFileDosPath, nzplib_gd()->ofd))
	{
		if (CatInfoIsFolder(&nzplib_gd()->cPB)) {
			IsOk = trueblnr;
		} else {
			if (nzplib_ProcessDF(&crc)) {
				l_att = (ui4r)zipatt_UNKNOWN; /* probably shouldn't do this */
				s = nzplib_gd()->cPB.hfileInfo.ioFlLgLen;
				isize = s;

				/* Try to rewrite the local header with correct information */

				PutLSBui4(&locr.flg, l_lflg);
				PutLSBui5(&locr.crc, crc);
				PutLSBui5(&locr.siz, s);
				PutLSBui5(&locr.len, isize);
				/* don't need to write name again */
				IsOk = MyBackWriteBytes(nzplib_gd()->ofd, l_off,
					(MyPtr) &locr, sizeof(ziploc_r));
			}
		}

		if (IsOk) {
			zipcen_r r;

			IsOk = falseblnr;
			PutLSBui5(&r.sig, ziphdr_CENSIG);
			PutLSBui4(&r.vem, (ui4r)(20));
			PutLSBui4(&r.ver, l_ver);
			PutLSBui4(&r.flg, l_flg);
			PutLSBui4(&r.how, l_how);
			PutLSBui5(&r.tim, l_tim);
			PutLSBui5(&r.crc, crc);
			PutLSBui5(&r.siz, s);
			PutLSBui5(&r.len, isize);
			PutLSBui4(&r.nam, l_nam);
			PutLSBui4(&r.ext, 0);
			PutLSBui4(&r.com, 0);
			PutLSBui4(&r.dsk, 0);
			PutLSBui4(&r.att, l_att);
			PutLSBui5(&r.atx, l_atx);
			PutLSBui5(&r.off, l_off);

			++nzplib_gd()->CentralCount;

			if (xbh_AppendPtr(&nzplib_gd()->hCentral, (MyPtr)&r, sizeof(zipcen_r)))
			if (xbh_Append_xbh(&nzplib_gd()->hCentral, &nzplib_gd()->CurFileDosPath))
			{
				IsOk = trueblnr;
			}
		}
	}

	return IsOk;
}

static blnr nzplib_putend(n, s, c)
int n;                  /* number of entries in central directory */
ui5r s;                  /* size of central directory */
ui5r c;                  /* offset of central directory */
/*
	Write the end of central directory data.
*/
{
	zipend_r r;

	PutLSBui5(&r.sig, ziphdr_ENDSIG);
	PutLSBui4(&r.dsk, 0);
	PutLSBui4(&r.beg, 0);
	PutLSBui4(&r.sub, n);
	PutLSBui4(&r.tot, n);
	PutLSBui5(&r.siz, s);
	PutLSBui5(&r.off, c);
	PutLSBui4(&r.com, 0);

	return MyWriteBytes(nzplib_gd()->ofd, (char *)&r, sizeof(zipend_r));
}

static blnr nzplib_EndProcessing(void)
/* Write central directory and end header */
{
	long c; /* start of central directory */
	ui5r n = xbh_GetLen(&nzplib_gd()->hCentral);
	blnr IsOk = falseblnr;

	if (CheckSysErr(GetFPos(nzplib_gd()->ofd, &c))) {
		IsOk = xbh_WriteToFile(&nzplib_gd()->hCentral,
			nzplib_gd()->ofd);
		if (IsOk) {
			IsOk = nzplib_putend(nzplib_gd()->CentralCount, n, c);
		}
	}

	return IsOk;
}

static blnr nzplib_ProcessOne(void)
{
	uimr OldDosPathLen = xbh_GetLen(&nzplib_gd()->CurFileDosPath);
	blnr IsOk = falseblnr;

	xbh_AppendPStr(&nzplib_gd()->CurFileDosPath,
		nzplib_gd()->cPB.hfileInfo.ioNamePtr);
	if (CatInfoIsFolder(&nzplib_gd()->cPB)) {
		xbh_AppendCStr(&nzplib_gd()->CurFileDosPath, "/");
	}
	if (nzplib_ProcessOne0()) {
		if (! CatInfoIsFolder(&nzplib_gd()->cPB)) {
			IsOk = trueblnr;
		} else {
			blnr FinishOk;
			MyDir_R old_d;
			int index = 1;

			MyCatInfoGetMyDir(&nzplib_gd()->cPB, &old_d);
			while (MyCatGetNextChild(&nzplib_gd()->cPB,
				&old_d, &index, &FinishOk)
				&& nzplib_ProcessOne())
			{
			}
			if (FinishOk) {
				IsOk = trueblnr;
			}
		}
	}

	(void) xbh_SetLen(&nzplib_gd()->CurFileDosPath, OldDosPathLen);

	return IsOk;
}


GLOBALFUNC blnr nzplib_WriteFile(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s)
{
	MyPStr itemName;
	nzplib_r r;
	blnr IsOk = falseblnr;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	if (xbh_Init(0, &r.CurFileDosPath)) {
		if (xbh_Init(0, &r.hCentral)) {
			if (MyOpenNewFile(dst_d, dst_s,
				'MPS ', 'TEXT', &r.ofd))
			{
				r.CentralCount = 0;

				if (MyFileGetCatInfo(src_d, src_s,
					itemName, &r.cPB))
				if (nzplib_ProcessOne())
				if (nzplib_EndProcessing())
				{
					IsOk = trueblnr;
				}

				(void) MyCloseFile(r.ofd);
				if (! IsOk) {
					MyDeleteFile(dst_d, dst_s);
				}
			}
			xbh_UnInit(&r.hCentral);
		}
		xbh_UnInit(&r.CurFileDosPath);
	}

	pDt = r.SavepDt;

	return IsOk;
}
