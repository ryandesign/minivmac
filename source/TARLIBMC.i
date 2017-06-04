/*
	TARLIBMC.i
	Copyright (C) 2009 Paul C. Pratt, John Gilmore

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
	TAR format LIBrary for MaCintosh
*/


/*
	adapted from tar code with this copyright header:

	A tar (tape archiver) program.
	Copyright 1985, 92, 93, 94, 96, 97, 1999
		Free Software Foundation, Inc.
	Written by John Gilmore, on 1985-08-25.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc.,
	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#define ssize_t long int
#define uintmax_t long int

#define gid_t long int
#define uid_t long int

#define major_t long int
#define minor_t long int
#define mode_t long int
#define off_t long int

#ifndef S_ISUID
# define S_ISUID 0004000
#endif
#ifndef S_ISGID
# define S_ISGID 0002000
#endif
#ifndef S_ISVTX
# define S_ISVTX 0001000
#endif
#ifndef S_IRUSR
# define S_IRUSR 0000400
#endif
#ifndef S_IWUSR
# define S_IWUSR 0000200
#endif
#ifndef S_IXUSR
# define S_IXUSR 0000100
#endif
#ifndef S_IRGRP
# define S_IRGRP 0000040
#endif
#ifndef S_IWGRP
# define S_IWGRP 0000020
#endif
#ifndef S_IXGRP
# define S_IXGRP 0000010
#endif
#ifndef S_IROTH
# define S_IROTH 0000004
#endif
#ifndef S_IWOTH
# define S_IWOTH 0000002
#endif
#ifndef S_IXOTH
# define S_IXOTH 0000001
#endif

#define MODE_R   (S_IRUSR | S_IRGRP | S_IROTH)
#define MODE_RW  (S_IWUSR | S_IWGRP | S_IWOTH | MODE_R)
#define MODE_RWX (S_IXUSR | S_IXGRP | S_IXOTH | MODE_RW)
#define MODE_ALL (S_ISUID | S_ISGID | S_ISVTX | MODE_RWX)

/* Common declarations for the tar program. */

/* GNU tar Archive Format description. */

/*
	If OLDGNU_COMPATIBILITY is not zero, tar produces archives which,
	by default, are readable by older versions of GNU tar.  This can be
	overriden by using --posix; in this case, POSIXLY_CORRECT in
	environment may be set for enforcing stricter conformance.  If
	OLDGNU_COMPATIBILITY is zero or undefined, tar will eventually
	produces archives which, by default, POSIX compatible; then either
	using --posix or defining POSIXLY_CORRECT enforces stricter
	conformance.

	This #define will disappear in a few years.  FP, June 1995.
*/
#define OLDGNU_COMPATIBILITY 1

/*
	`tar' Header Block, from POSIX 1003.1-1990.
*/

/* POSIX header. */

struct tarlib_posix_header
{
	/* start */          /* byte offset */
	char name[100];      /*   0 */
	char mode[8];        /* 100 */
	char uid[8];         /* 108 */
	char gid[8];         /* 116 */
	char size[12];       /* 124 */
	char mtime[12];      /* 136 */
	char chksum[8];      /* 148 */
	char typeflag;       /* 156 */
	char linkname[100];  /* 157 */
	char magic[6];       /* 257 */
	char version[2];     /* 263 */
	char uname[32];      /* 265 */
	char gname[32];      /* 297 */
	char devmajor[8];    /* 329 */
	char devminor[8];    /* 337 */
	char prefix[155];    /* 345 */
	/* end */            /* 500 */
};

#define TMAGIC   "ustar" /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"    /* 00 and no null */
#define TVERSLEN 2

/* Values used in typeflag field. */
#define REGTYPE  '0'  /* regular file */
#define AREGTYPE '\0' /* regular file */
#define LNKTYPE  '1'  /* link */
#define SYMTYPE  '2'  /* reserved */
#define CHRTYPE  '3'  /* character special */
#define BLKTYPE  '4'  /* block special */
#define DIRTYPE  '5'  /* directory */
#define FIFOTYPE '6'  /* FIFO special */
#define CONTTYPE '7'  /* reserved */

/* Bits used in the mode field, values in octal. */
#define TSUID    04000 /* set UID on execution */
#define TSGID    02000 /* set GID on execution */
#define TSVTX    01000 /* reserved */

/* file permissions */
#define TUREAD   00400 /* read by owner */
#define TUWRITE  00200 /* write by owner */
#define TUEXEC   00100 /* execute/search by owner */
#define TGREAD   00040 /* read by group */
#define TGWRITE  00020 /* write by group */
#define TGEXEC   00010 /* execute/search by group */
#define TOREAD   00004 /* read by other */
#define TOWRITE  00002 /* write by other */
#define TOEXEC   00001 /* execute/search by other */

/*
	`tar' Header Block, GNU extensions.
*/

/*
	In GNU tar, SYMTYPE is for to symbolic links, and CONTTYPE is for
	contiguous files, so maybe disobeying the `reserved' comment in
	POSIX header description.  I suspect these were meant to be used
	this way, and should not have really been `reserved' in the
	published standards.
*/

/*
	BEWARE BEWARE BEWARE that the following information is still
	boiling, and may change.  Even if the OLDGNU format description
	should be accurate, the so-called GNU format is not yet fully
	decided.  It is surely meant to use only extensions allowed by
	POSIX, but the sketch below repeats some ugliness from the OLDGNU
	format, which should rather go away.  Sparse files should be saved
	in such a way that they do NOT require two passes at archive
	creation time.  Huge files get some POSIX fields to overflow,
	alternate solutions have to be sought for this.
*/

/*
	OLDGNU_MAGIC uses both magic and version fields, which are
	contiguous. Found in an archive, it indicates an old GNU header
	format, which will be hopefully become obsolescent.  With
	OLDGNU_MAGIC, uname and gname are valid, though the header is not
	truly POSIX conforming.
*/
#define OLDGNU_MAGIC "ustar  "  /* 7 chars and a null */

/*
	The standards committee allows only capital A through capital Z for
	user-defined expansion.
*/

/*
	This is a dir entry that contains the names of files that were in
	the dir at the time the dump was made.
*/
#define GNUTYPE_DUMPDIR 'D'

/* Identifies the *next* file on the tape as having a long linkname. */
#define GNUTYPE_LONGLINK 'K'

/* Identifies the *next* file on the tape as having a long name. */
#define GNUTYPE_LONGNAME 'L'

/* This is the continuation of a file that began on another volume. */
#define GNUTYPE_MULTIVOL 'M'

/* For storing filenames that do not fit into the main header. */
#define GNUTYPE_NAMES 'N'

/* This file is a tape/volume header.  Ignore it on extraction. */
#define GNUTYPE_VOLHDR 'V'

/*
	tar Header Block, overall structure.
*/

/* tar files are made in basic blocks of this size. */
#define tarlib_BLOCKSIZE 512

enum archive_format
{
	DEFAULT_FORMAT,  /* format to be decided later */
	V7_FORMAT,       /* old V7 tar format */
	OLDGNU_FORMAT,   /* GNU format as per before tar 1.12 */
	POSIX_FORMAT,    /* restricted, pure POSIX format */
	GNU_FORMAT       /* POSIX format with GNU extensions */
};

/* End of Format description. */

/* Some various global definitions. */

#define V7_FORMAT     0  /* old V7 tar format */
#define OLDGNU_FORMAT 1  /* GNU format as per before tar 1.12 */
#define POSIX_FORMAT  2  /* restricted, pure POSIX format */
#define GNU_FORMAT    3  /* POSIX format with GNU extensions */

#define archive_format V7_FORMAT

/*
	Convert VALUE (with substitute SUBSTITUTE if VALUE is out of range)
	into a size-SIZE field at WHERE, including a trailing space.  For
	example, 3 for SIZE means two digits and a space.

	We assume the trailing NUL is already there and don't fill it in.
	This fact is used by tarlib_write_file_header_v2,
	so don't change it!
*/

/*
	Output VALUE in octal, using SUBSTITUTE if value won't fit.
	Output to buffer WHERE with size SIZE.
	TYPE is the kind of value being output (useful for diagnostics).
	Prefer SIZE - 1 octal digits (with leading '0's), followed by '\0';
	but if SIZE octal digits would fit, omit the '\0'.
*/

LOCALPROC
to_oct (uintmax_t value, uintmax_t substitute, char *where, ui5r size)
{
	uintmax_t v = value;
	ui5r i = size;

# define MAX_OCTAL_VAL_WITH_DIGITS(digits) \
	((digits) * 3 < sizeof (uintmax_t) * CHAR_BIT \
	? ((uintmax_t) 1 << ((digits) * 3)) - 1 \
	: (uintmax_t) -1)

	/* Output a trailing NUL unless the value is too large. */
	/* if (value <= MAX_OCTAL_VAL_WITH_DIGITS (size - 1)) */
		/* this test isn't working */
	where[--i] = (char)'\0';

	/* Produce the digits -- at least one. */

	do {
		where[--i] = (char)('0' + (int) (v & 7)); /* one octal digit */
		v >>= 3;
	} while (i != 0 && v != 0);

	/* Leading zeros, if necessary. */
	while (i != 0) {
		where[--i] = (char)'0';
	}

	if (v != 0) {
		uintmax_t maxval = MAX_OCTAL_VAL_WITH_DIGITS (size);
#if 0
		char buf1[UINTMAX_STRSIZE_BOUND];
		char buf2[UINTMAX_STRSIZE_BOUND];
		char buf3[UINTMAX_STRSIZE_BOUND];
		char *value_string = STRINGIFY_BIGINT (value, buf1);
		char *maxval_string = STRINGIFY_BIGINT (maxval, buf2);
#endif
		if (substitute) {
			substitute &= maxval;
#if 0
			WARN ((0, 0,
				_("%s value %s too large (max=%s); substituting %s"),
				type, value_string, maxval_string,
				STRINGIFY_BIGINT (substitute, buf3)));
#endif
			to_oct (substitute, (uintmax_t) 0, where, size);
		} else {
#if 0
			ERROR ((0, 0, _("%s value %s too large (max=%s)"),
				type, value_string, maxval_string));
#endif
		}
	}
}

#ifndef GID_NOBODY
#define GID_NOBODY 0
#endif

LOCALPROC
gid_to_oct (gid_t v, char *p, ui5r s)
{
	to_oct((uintmax_t) v, (uintmax_t) GID_NOBODY, p, s /* , "gid_t" */);
}

LOCALPROC
major_to_oct (major_t v, char *p, ui5r s)
{
	to_oct((uintmax_t) v, (uintmax_t) 0, p, s /* , "major_t" */);
}

LOCALPROC
minor_to_oct (minor_t v, char *p, ui5r s)
{
	to_oct((uintmax_t) v, (uintmax_t) 0, p, s /* , "minor_t" */);
}

LOCALPROC
mode_to_oct (mode_t v, char *p, ui5r s)
{
	/*
		In the common case where the internal and external mode bits
		are the same, propagate all unknown bits to the external mode.
		This matches historical practice.
		Otherwise, just copy the bits we know about.
	*/
	uintmax_t u =
		((S_ISUID == TSUID && S_ISGID == TSGID && S_ISVTX == TSVTX
			&& S_IRUSR == TUREAD && S_IWUSR == TUWRITE
			&& S_IXUSR == TUEXEC && S_IRGRP == TGREAD
			&& S_IWGRP == TGWRITE && S_IXGRP == TGEXEC
			&& S_IROTH == TOREAD && S_IWOTH == TOWRITE
			&& S_IXOTH == TOEXEC)
		? v
		: ((v & S_ISUID ? TSUID : 0)
			| (v & S_ISGID ? TSGID : 0)
			| (v & S_ISVTX ? TSVTX : 0)
			| (v & S_IRUSR ? TUREAD : 0)
			| (v & S_IWUSR ? TUWRITE : 0)
			| (v & S_IXUSR ? TUEXEC : 0)
			| (v & S_IRGRP ? TGREAD : 0)
			| (v & S_IWGRP ? TGWRITE : 0)
			| (v & S_IXGRP ? TGEXEC : 0)
			| (v & S_IROTH ? TOREAD : 0)
			| (v & S_IWOTH ? TOWRITE : 0)
			| (v & S_IXOTH ? TOEXEC : 0)));
	to_oct(u, (uintmax_t) 0, p, s /* , "mode_t" */);
}

LOCALPROC
off_to_oct (off_t v, char *p, ui5r s)
{
	to_oct((uintmax_t) v, (uintmax_t) 0, p, s /* , "off_t" */);
}

LOCALPROC
size_to_oct (ui5r v, char *p, ui5r s)
{
	to_oct((uintmax_t) v, (uintmax_t) 0, p, s /* , "size_t" */);
}

LOCALPROC
time_to_oct (ui5r v, char *p, ui5r s)
{
	to_oct((uintmax_t) v, (uintmax_t) 0, p, s /* , "time_t" */);
}

#ifndef UID_NOBODY
#define UID_NOBODY 0
#endif

LOCALPROC
uid_to_oct (uid_t v, char *p, ui5r s)
{
	to_oct((uintmax_t) v, (uintmax_t) UID_NOBODY, p, s /* , "uid_t" */);
}

LOCALPROC
uintmax_to_oct(uintmax_t v, char *p, ui5r s)
{
	to_oct(v, (uintmax_t) 0, p, s /* , "uintmax_t" */);
}

#define GID_TO_OCT(val, where) gid_to_oct(val, where, sizeof(where))
#define MAJOR_TO_OCT(val, where) major_to_oct(val, where, sizeof(where))
#define MINOR_TO_OCT(val, where) minor_to_oct(val, where, sizeof(where))
#define MODE_TO_OCT(val, where) mode_to_oct(val, where, sizeof(where))
#define OFF_TO_OCT(val, where) off_to_oct(val, where, sizeof(where))
#define SIZE_TO_OCT(val, where) size_to_oct(val, where, sizeof(where))
#define TIME_TO_OCT(val, where) time_to_oct(val, where, sizeof(where))
#define UID_TO_OCT(val, where) uid_to_oct(val, where, sizeof(where))
#define UINTMAX_TO_OCT(val, where) \
	uintmax_to_oct(val, where, sizeof(where))

/* Header handling. */

/*
	The checksum field is filled with this while the checksum
	is computed.
*/
#define CHKBLANKS "        " /* 8 blanks, no null */

/* Some constants from POSIX are given names. */
#define NAME_FIELD_SIZE   100
#define PREFIX_FIELD_SIZE 155
#define UNAME_FIELD_SIZE   32
#define GNAME_FIELD_SIZE   32

union tarlib_block
{
	ui3b buffer[tarlib_BLOCKSIZE];
	struct tarlib_posix_header header;
};

/*
	The tarlib_r structure is used to minimize the amount of
	stack space used when recursively calling tarlib_ProcessOne
	and to hold global information that might be needed at any time.
*/
struct tarlib_r
{
	MyPtr SavepDt;
	CInfoPBRec cPB;

	int CurFileIsDirectory;

	Str255 CurFileDosPath;

	off_t CurrentFileSize;
	ui5r CurrentFileModTime;
	mode_t CurrentFileMode;

	short ifile;

	short ofd;

	union tarlib_block buffrecord;
};

typedef struct tarlib_r tarlib_r;

#define tarlib_gd() ((tarlib_r *)(pDt))

/* Writing routines. */

/*
	Write the EOT block(s).  We actually zero at least one block,
	through the end of the record.  Old tar, as previous versions
	of GNU tar, writes garbage after two zeroed blocks.
*/

LOCALFUNC tMyErr
tarlib_write_eot_v2(void)
{
	pBytesClear(tarlib_gd()->buffrecord.buffer, tarlib_BLOCKSIZE);
	return MyWriteBytes_v2(tarlib_gd()->ofd,
		tarlib_gd()->buffrecord.buffer,
		tarlib_BLOCKSIZE);
}

/*
	Make a header block for the file name whose stat info is st.  Return
	header pointer for success, NULL if the name is too long.
*/

LOCALFUNC tMyErr
tarlib_write_file_header_v2(void)
{
	ui5r i;
	int sum;
	si4r n;
	char *p;

	pBytesClear(tarlib_gd()->buffrecord.buffer,
		sizeof(union tarlib_block));

	n = PStrLength(tarlib_gd()->CurFileDosPath);
	if (n > NAME_FIELD_SIZE) {
		n = (si4r)NAME_FIELD_SIZE;
	}
	MyMoveBytes(&tarlib_gd()->CurFileDosPath[1],
		tarlib_gd()->buffrecord.header.name, n);
	tarlib_gd()->buffrecord.header.name[NAME_FIELD_SIZE - 1]
		= (char)'\0';

	/*
		Paul Eggert tried the trivial test
			($WRITER cf a b; $READER tvf a)
		for a few tars and came up with the following interoperability
		matrix:

		WRITER
		1 2 3 4 5 6 7 8 9   READER
		. . . . . . . . .   1 = SunOS 4.2 tar
		# . . # # . . # #   2 = NEC SVR4.0.2 tar
		. . . # # . . # .   3 = Solaris 2.1 tar
		. . . . . . . . .   4 = GNU tar 1.11.1
		. . . . . . . . .   5 = HP-UX 8.07 tar
		. . . . . . . . .   6 = Ultrix 4.1
		. . . . . . . . .   7 = AIX 3.2
		. . . . . . . . .   8 = Hitachi HI-UX 1.03
		. . . . . . . . .   9 = Omron UNIOS-B 4.3BSD 1.60Beta

		. = works
		# = ``impossible file type''

		The following mask for old archive removes the `#'s in column 4
		above, thus making GNU tar both a universal donor and a
		universal acceptor for Paul's test.
	*/

#if archive_format == V7_FORMAT
	MODE_TO_OCT(tarlib_gd()->CurrentFileMode & MODE_ALL,
		tarlib_gd()->buffrecord.header.mode);
#else
	MODE_TO_OCT(tarlib_gd()->CurrentFileMode,
		tarlib_gd()->buffrecord.header.mode);
#endif

	UID_TO_OCT(/* current_stat.st_uid */ UID_NOBODY,
		tarlib_gd()->buffrecord.header.uid);
	GID_TO_OCT(/* current_stat.st_gid */ GID_NOBODY,
		tarlib_gd()->buffrecord.header.gid);
	OFF_TO_OCT(tarlib_gd()->CurrentFileSize,
		tarlib_gd()->buffrecord.header.size);
	TIME_TO_OCT(tarlib_gd()->CurrentFileModTime,
		tarlib_gd()->buffrecord.header.mtime);

#if archive_format == V7_FORMAT
	/* buffrecord.header.[ug]name are left as the empty string. */
#else
#if archive_format == OLDGNU_FORMAT
	/*
		Overwrite buffrecord.header.magic and header.version
		in one blow.
	*/
	strcpy (tarlib_gd()->buffrecord.header.magic, OLDGNU_MAGIC);
#else /* POSIX_FORMAT and GNU_FORMAT */
	strncpy (tarlib_gd()->buffrecord.header.magic, TMAGIC, TMAGLEN);
	strncpy (tarlib_gd()->buffrecord.header.version, TVERSION,
		TVERSLEN);
#endif
#endif

	if (tarlib_gd()->CurFileIsDirectory) {
		tarlib_gd()->buffrecord.header.typeflag = (char)DIRTYPE;
	} else {
#if archive_format == V7_FORMAT
		tarlib_gd()->buffrecord.header.typeflag = (char)AREGTYPE;
#else
		tarlib_gd()->buffrecord.header.typeflag = (char)REGTYPE;
#endif
	}

	MyMoveBytes((MyPtr)CHKBLANKS,
		(MyPtr)tarlib_gd()->buffrecord.header.chksum,
		sizeof (tarlib_gd()->buffrecord.header.chksum));

	sum = 0;
	p = (char *)tarlib_gd()->buffrecord.buffer;
	for (i = sizeof (tarlib_gd()->buffrecord); i-- != 0; ) {
		/*
			We can't use unsigned char here because of old compilers,
			e.g. V7.
		*/
		sum += 0xFF & *p++;
	}

	/*
		Fill in the checksum field.  It's formatted differently from
		the other fields: it has [6] digits, a null, then a space
		-- rather than digits, then a null.  We use to_oct.
		The final space is already there, from checksumming,
		and to_oct doesn't modify it.

		This is a fast way to do:

		sprintf(tarlib_gd()->buffrecord.header.chksum, "%6o", sum);
	*/
	uintmax_to_oct ((uintmax_t) sum,
		tarlib_gd()->buffrecord.header.chksum, 7);

	return MyWriteBytes_v2(tarlib_gd()->ofd,
		tarlib_gd()->buffrecord.buffer,
		tarlib_BLOCKSIZE);
}

/*
	Mac file times are based on 1904 Jan 1 00:00 local time,
	not 1970 Jan 1 00:00 UTC.
	So we have to convert the time stamps into UNIX UTC
	compatible values.
*/

LOCALFUNC ui5r MacFtime2UnixFtime(unsigned long macftime)
{
	return macftime - (unsigned long)2082844800UL;
}

LOCALFUNC tMyErr
tarlib_ProcessOne0_v2(void)
{
	tMyErr err;

	/*
		fprintf(stderr, "CurFileDosPath %P\n",
			tarlib_gd()->CurFileDosPath);
	*/

	/*
		The times returned by the Mac file system are in the
		local time zone.  We convert them to GMT so that the
		epoch starts from GMT.  This is also consistent with
		what is returned from "clock seconds".
	*/
	if (! tarlib_gd()->CurFileIsDirectory) {
		tarlib_gd()->CurrentFileModTime =
			MacFtime2UnixFtime(tarlib_gd()->cPB.hFileInfo.ioFlMdDat);
		tarlib_gd()->CurrentFileSize =
			tarlib_gd()->cPB.hFileInfo.ioFlLgLen;
		tarlib_gd()->CurrentFileMode = 0666;
	} else {
		tarlib_gd()->CurrentFileModTime =
			MacFtime2UnixFtime(tarlib_gd()->cPB.dirInfo.ioDrMdDat);
		tarlib_gd()->CurrentFileSize = 0;
		tarlib_gd()->CurrentFileMode = 0777;
	}

	if (noErr == (err = tarlib_write_file_header_v2())) {
		if (tarlib_gd()->CurFileIsDirectory) {
			/* ok */
		} else {
			ui5r bufsize;
			off_t sizeleft = tarlib_gd()->cPB.hFileInfo.ioFlLgLen;

			if (noErr == (err = CatInfoOpenReadDF_v2(&tarlib_gd()->cPB,
				&tarlib_gd()->ifile)))
			{
doGetMore:
				if (0 != sizeleft) {
					if (sizeleft < tarlib_BLOCKSIZE) {
						/* Last read -- zero out area beyond. */

						bufsize = sizeleft;
						pBytesClear(
							tarlib_gd()->buffrecord.buffer + sizeleft,
							(ui5r) (tarlib_BLOCKSIZE - sizeleft));
					} else {
						bufsize = tarlib_BLOCKSIZE;
					}
					if (noErr == (err = MyReadBytes_v2(
						tarlib_gd()->ifile,
						tarlib_gd()->buffrecord.buffer,
						bufsize)))
					if (noErr == (err = MyWriteBytes_v2(
						tarlib_gd()->ofd,
						tarlib_gd()->buffrecord.buffer,
						tarlib_BLOCKSIZE)))
					{
						sizeleft -= bufsize;
						goto doGetMore;
					}
				}
				err = CombineErr(err,
					MyCloseFile_v2(tarlib_gd()->ifile));
			}
		}
	}

	return err;
}

LOCALFUNC tMyErr tarlib_ProcessOne_v2(void)
{
	tMyErr err;
	int OldDosPathLen = PStrLength(tarlib_gd()->CurFileDosPath);

	tarlib_gd()->CurFileIsDirectory =
		CatInfoIsFolder(&tarlib_gd()->cPB);
	PStrAppend(tarlib_gd()->CurFileDosPath,
		tarlib_gd()->cPB.hFileInfo.ioNamePtr);
	if (tarlib_gd()->CurFileIsDirectory) {
		PStrApndChar(tarlib_gd()->CurFileDosPath, '/');
	}

	if (noErr == (err = tarlib_ProcessOne0_v2())) {
		if (! tarlib_gd()->CurFileIsDirectory) {
			/* ok */
		} else {
			MyDir_R old_d;
			int index = 1;

			MyCatInfoGetMyDir(&tarlib_gd()->cPB, &old_d);

Label_1:
			err = MyCatGetNextChild_v2(&tarlib_gd()->cPB,
				&old_d, &index);
			if (noErr == err) {
				if (noErr == (err = tarlib_ProcessOne_v2())) {
					goto Label_1;
				}
			} else if (fnfErr == err) {
				err = noErr;
			}

		} /* CurFileIsDirectory */
	}

	tarlib_gd()->CurFileDosPath[0] = (char)OldDosPathLen;

	return err;
}

/* ****************************************************************** */

GLOBALFUNC tMyErr tarlib_WriteTarFile2_v2(
	MyDir_R *src_d, StringPtr src_s,
	MyDir_R *dst_d, StringPtr dst_s)
{
	tMyErr err;
	tarlib_r r;
	MyPStr NameBuffer;

	r.SavepDt = pDt;
	pDt = (MyPtr)&r;

	if (noErr == (err = MyOpenNewFile_v3(dst_d, dst_s,
		'MPS ', 'TEXT', &r.ofd)))
	{
		r.CurFileDosPath[0] = (char)0;

		if (noErr == (err = MyFileGetCatInfo_v2(src_d, src_s,
			NameBuffer, &r.cPB)))
		if (noErr == (err = tarlib_ProcessOne_v2()))
		if (noErr == (err = tarlib_write_eot_v2()))
		{
		}

		err = MyCloseNewFile_v3(r.ofd, dst_d, dst_s, err);
	}

	pDt = r.SavepDt;

	return err;
}
