/*
	ENDIANAC.c

	Copyright (C) 2001 Bernd Schmidt, Paul Pratt

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
	ENDIAN ACcess
	
	Deals with endian issues in memory access.
	
	This code is adapted from code in the Un*x Amiga Emulator by
	Bernd Schmidt, as found in vMac by Philip Cummins.
*/

#ifdef ENDIANAC_H
#error "header already included"
#else
#define ENDIANAC_H
#endif

#define BIGENDIAN
#define PPC

#ifndef BIGENDIAN

static __inline__ ULONG do_get_mem_long(ULONG *a)
{
    UBYTE *b = (UBYTE *)a;
    
    return (*b << 24) | (*(b+1) << 16) | (*(b+2) << 8) | (*(b+3));
}

static __inline__ UWORD do_get_mem_word(UWORD *a)
{
    UBYTE *b = (UBYTE *)a;
    
    return (*b << 8) | (*(b+1));
}

static __inline__ UBYTE do_get_mem_byte(UBYTE *a)
{
    return *a;
}

static __inline__ void do_put_mem_long(ULONG *a, ULONG v)
{
    UBYTE *b = (UBYTE *)a;
    
    *b = v >> 24;
    *(b+1) = v >> 16;    
    *(b+2) = v >> 8;
    *(b+3) = v;
}

static __inline__ void do_put_mem_word(UWORD *a, UWORD v)
{
    UBYTE *b = (UBYTE *)a;
    
    *b = v >> 8;
    *(b+1) = v;
}

static __inline__ void do_put_mem_byte(UBYTE *a, UBYTE v)
{
    *a = v;
}

#else /* Big endian memory functions */

#if 0
#define InLineKeyWord __inline__
#else
#define InLineKeyWord inline
#endif

#ifdef PPC
#if 0
static InLineKeyWord ULONG do_get_mem_long(ULONG *a)
{
    return *a;
}
#else
#define do_get_mem_long(a) (*((ULONG *)(a)))
#endif
#else /* fixes IRIX, Solaris, and HP-UX. */
static InLineKeyWord ULONG do_get_mem_long(ULONG *a)
{
    UBYTE *b = (UBYTE *)a;
   
    return (*b << 24) | (*(b+1) << 16) | (*(b+2) << 8) | (*(b+3));
}
#endif

#if 0
static InLineKeyWord UWORD do_get_mem_word(UWORD *a)
{
    return *a;
}
#else
#define do_get_mem_word(a) (*((UWORD *)(a)))
#endif

#if 0
static InLineKeyWord UBYTE do_get_mem_byte(UBYTE *a)
{
    return *a;
}
#else
#define do_get_mem_byte(a) (*((UBYTE *)(a)))
#endif

#ifdef PPC
#if 0
static InLineKeyWord void do_put_mem_long(ULONG *a, ULONG v)
{
    *a = v;
}
#else
#define do_put_mem_long(a, v) ((*((ULONG *)(a))) = (v))
#endif
#else /* fixes Solaris, IRIX, and HP-UX */
static InLineKeyWord void do_put_mem_long(ULONG *a, ULONG v)
{
    UBYTE *b = (UBYTE *)a;
 
    *b = v >> 24;
    *(b+1) = v >> 16;
    *(b+2) = v >> 8;
    *(b+3) = v;
}  
#endif

#if 0
static InLineKeyWord void do_put_mem_word(UWORD *a, UWORD v)
{
    *a = v;
}
#else
#define do_put_mem_word(a, v) ((*((UWORD *)(a))) = (v))
#endif

#if 0
static InLineKeyWord void do_put_mem_byte(UBYTE *a, UBYTE v)
{
    *a = v;
}
#else
#define do_put_mem_byte(a, v) ((*((UBYTE *)(a))) = (v))
#endif

#endif
