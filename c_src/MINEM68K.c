/*
	MINEM68K.c

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
	MINimum EMulator of 68K cpu

	This code descends from a simple 68000 emulator that I (Paul Pratt)
	wrote long ago. That emulator ran on a 680x0, and used the cpu
	it ran on to do some of the work. This descendent fills
	in those holes with code from the Un*x Amiga Emulator
	by Bernd Schmidt, as found being used in vMac.
	
	This emulator is about 10 times smaller than the UAE,
	at the cost of being 2 to 3 times slower. It also only
	emulates the 68000, not including the emulation of later
	processors or the FPU.
*/

#include "SYSDEPNS.h"

#include "ENDIANAC.h"

#include "ADDRSPAC.h"
#include "GLOBGLUE.h"

#include "MINEM68K.h"

typedef char flagtype;

static struct regstruct 
{
    ULONG regs[16];
    CPTR  usp,isp;
    UWORD sr;
    flagtype t1;
    flagtype t0;
    flagtype s;
    flagtype m;
    flagtype x;
    flagtype stopped;
    int intmask;
    ULONG pc;
 
    ULONG spcflags;
} regs;

#define m68k_dreg(r,num) ((r).regs[(num)])
#define m68k_areg(r,num) ((r).regs[(num)+8])

#ifndef BIGENDIAN 
struct flag_struct {
    unsigned int c;
    unsigned int z;
    unsigned int n;
    unsigned int v; 
    unsigned int x;
};
#else
struct flag_struct {
    unsigned char c;
    unsigned char z;
    unsigned char n;
    unsigned char v;
    unsigned char x;
};
#endif

static struct flag_struct regflags;

#define ZFLG (regflags.z)
#define NFLG (regflags.n)
#define CFLG (regflags.c)
#define VFLG (regflags.v)
#define XFLG (regflags.x)

#ifdef __MWERKS__
#define MayInline __inline__
#else
#define MayInline inline
#endif

#define LOCALPROCUSEDONCE static MayInline void

static MayInline int cctrue(const int cc)
{
    switch(cc){
     case 0: return 1;                       /* T */
     case 1: return 0;                       /* F */
     case 2: return !CFLG && !ZFLG;          /* HI */
     case 3: return CFLG || ZFLG;            /* LS */
     case 4: return !CFLG;                   /* CC */
     case 5: return CFLG;                    /* CS */
     case 6: return !ZFLG;                   /* NE */
     case 7: return ZFLG;                    /* EQ */
     case 8: return !VFLG;                   /* VC */
     case 9: return VFLG;                    /* VS */
     case 10:return !NFLG;                   /* PL */
     case 11:return NFLG;                    /* MI */
     case 12:return NFLG == VFLG;            /* GE */
     case 13:return NFLG != VFLG;            /* LT */
     case 14:return !ZFLG && (NFLG == VFLG); /* GT */
     case 15:return ZFLG || (NFLG != VFLG);  /* LE */
    }
    /* abort(); */
    return 0;
}

static void MakeSR(void)
{
#if 0
    assert((regs.t1 & 1) == regs.t1);
    assert((regs.s & 1) == regs.s);
    assert((regs.m & 1) == regs.m);
    assert((XFLG & 1) == XFLG);
    assert((NFLG & 1) == NFLG);
    assert((ZFLG & 1) == ZFLG);
    assert((VFLG & 1) == VFLG);
    assert((CFLG & 1) == CFLG);
#endif
    regs.sr = ((regs.t1 << 15)
	       | (regs.s << 13) | (regs.m << 12) | (regs.intmask << 8)
	       | (XFLG << 4) | (NFLG << 3) | (ZFLG << 2) | (VFLG << 1) 
	       |  CFLG);
}

#define SPCFLAG_STOP 2
#define SPCFLAG_BUSERR 32
#define SPCFLAG_AUTOVECTOR 64
#define SPCFLAG_DOTRACE 128

void NeedToGetOut(void);

static void MakeFromSR(void)
{
    int oldm = regs.m;
    int olds = regs.s;

    regs.t1 = (regs.sr >> 15) & 1;
    regs.s = (regs.sr >> 13) & 1;
    regs.m = (regs.sr >> 12) & 1;
    regs.intmask = (regs.sr >> 8) & 7;
    XFLG = (regs.sr >> 4) & 1;
    NFLG = (regs.sr >> 3) & 1;
    ZFLG = (regs.sr >> 2) & 1;
    VFLG = (regs.sr >> 1) & 1;
    CFLG = regs.sr & 1;
	if (olds != regs.s) {
 	    if (olds) {
 			regs.isp = m68k_areg(regs, 7);
 	        m68k_areg(regs, 7) = regs.usp;
 	    } else {
 	        regs.usp = m68k_areg(regs, 7);
 	        m68k_areg(regs, 7) = regs.isp;
 	    }
 	}
    
    if (regs.t1) {
    	NeedToGetOut();
	} else {
    	regs.spcflags &= ~(SPCFLAG_DOTRACE);
    }
}

/*
 * This variable was introduced because a program could do a Bcc from
 * whithin chip memory to a location whitin expansion memory. With a
 * pointer variable the program counter would point to the wrong location.
 * With this variable unset the program counter is always correct, but
 * programs will run slower (about 4%).
 * Usually, you'll want to have this defined.
 *
 * vMac REQUIRES this. It allows for fun things like Restart.
 *
 */
#define USE_POINTER

#ifdef USE_POINTER
	static UBYTE *pc_p;
	static UBYTE *pc_oldp;
#endif

// bill comment ---
// Damn Damn Damn Damn !!!!
//
// "pc_p" is defined as a UBYTE, which
// is 8 bits wide.
//
// In the old UAE 0.60 emulator, it was defined
// as UWORD, which is a 16 wide entity.
// Since this version of nextilong() is from the old emulator,
// the assignments in this function were only
// copying 8 bits of data instead of 16...
//
// I must make a cast to a 16 entity, so that upon
// dereferencing, it will copy a 16 bits instead of 8. 
//
// All auto increment operations must also be changed
// to explicit specified human defined constants, because
// the compiler will vary the autoincrement byte size count
// depending on size of the base type. This is what
// led to the incorrect functioning of nextilong(),
// nextibyte(), nextiword()...
//

#ifdef USE_POINTER

static MayInline ULONG nextibyte(void)
{
//    ULONG r = do_get_mem_byte(pc_p+1);
    ULONG r;

//debugout(true, "nextibyte = %02x\n", (UBYTE) pc_p);
    r = do_get_mem_byte(pc_p+1);
    pc_p += 2;
    return r;
}

static MayInline ULONG nextiword(void)
{
//    ULONG r = do_get_mem_word((UWORD *)pc_p);
    ULONG r;

//debugout(true, "nextiword = %04hx\n", (UWORD) pc_p);
    r = do_get_mem_word((UWORD *)pc_p);
    pc_p += 2;
    return r;
}

static MayInline ULONG nextilong(void)
{
//    ULONG r = do_get_mem_long((ULONG *)pc_p);
    ULONG r;
//debugout(true, "nextilong = %08lx\n", (ULONG) pc_p);
    r = do_get_mem_long((ULONG *)pc_p);
    pc_p += 4;
    return r;
}

#else

static MayInline ULONG nextibyte(void)
{
    ULONG r = get_byte(regs.pc+1);
    regs.pc += 2;
    return r;
}

static MayInline ULONG nextiword(void)
{
    ULONG r = get_word(regs.pc);
    regs.pc += 2;
    return r;
}

static MayInline ULONG nextilong(void)
{
    ULONG r = get_long(regs.pc);
    regs.pc += 4;
    return r;
}

#endif

static MayInline void BackupPC(void)
{
#ifdef USE_POINTER
	pc_p -= 2;
#else
	regs.pc -= 2;
#endif
}

#define MakeDumpFile 0

#if MakeDumpFile
extern void DumpAJump(CPTR fromaddr, CPTR toaddr);

static void DumpAJump2(toaddr)
{
	CPTR fromaddr = regs.pc + ((char *)pc_p - (char *)pc_oldp);

	if ((toaddr > fromaddr) || (toaddr < regs.pc)) {
		if ((fromaddr >= 0x00400000) && (fromaddr < 0x00500000)) {
			fromaddr = fromaddr - 0x00400000 + 10000000;
		}
		if ((toaddr >= 0x00400000) && (toaddr < 0x00500000)) {
			toaddr = toaddr - 0x00400000 + 10000000;
		}
		DumpAJump(fromaddr, toaddr);
	}
}

#endif

#ifdef USE_POINTER

static MayInline void m68k_setpc(CPTR newpc)
{
#if MakeDumpFile
	DumpAJump2(newpc);
#endif
#if 0
	if (newpc == 0xBD50/*401AB4*/) {
		Debugger();
	}
#endif
    pc_p = pc_oldp = (UBYTE *) get_real_address(newpc);
    regs.pc = newpc;
}

static MayInline CPTR m68k_getpc(void)
{
    return regs.pc + ((char *)pc_p - (char *)pc_oldp);
}

#else

static MayInline void m68k_setpc(CPTR newpc)
{
//    regs.pc = newpc;
// bill mod
    regs.pc = newpc & 0x00ffFFFF;
}

static MayInline CPTR m68k_getpc(void)
{
    return regs.pc;
}
#endif

/* Opcode of faulting instruction */
static UWORD last_op_for_exception_3;
/* PC at fault time */
static CPTR last_addr_for_exception_3;
/* Address that generated the exception */
static CPTR last_fault_for_exception_3;

static void Exception(int nr)
{
if(nr != 10)

    MakeSR();
    if (!regs.s) {
		regs.usp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.isp;
		regs.s = 1;
    }
#if 0
    if (/* CPU_LEVEL > 0 */1) {
// bill comment ---
//
// (2)  is for a BUS error...
// (3)  is for an ADDRESS error...
//~(4)  is for an Illegal instruction error, but not action is defined in
//	this function...
//
	if (nr == 2 || nr == 3) {
	    int i;
	    /* @@@ this is probably wrong (?) */
	    for (i = 0 ; i < 12 ; i++) {
		m68k_areg(regs, 7) -= 2;
		put_word (m68k_areg(regs, 7), 0);
	    }
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), 0xa000 + nr * 4);
	} else if (nr ==5 || nr == 6 || nr == 7 || nr == 9) {
//
// (5)  is for a Zero-divide exception...
// (6)  is for a CHK/CHK2 exception...
// (7)  is for cpTRAPcc, TRAPcc, TRAPV exceptions...
//~(8)  is for a Privilege violation exception, but no action is defined in
//	this function.
// (9)  is for a Trace exception...
//~(10) is for an A-line trap exception... 
//~(11) is for an F-line trap exception...
//
 	    m68k_areg(regs, 7) -= 4;
 	    put_long (m68k_areg(regs, 7), oldpc);
 	    m68k_areg(regs, 7) -= 2;
 	    put_word (m68k_areg(regs, 7), 0x2000 + nr * 4);
 	} else if (regs.m && nr >= 24 && nr < 32) {
 	    m68k_areg(regs, 7) -= 2;
 	    put_word (m68k_areg(regs, 7), nr * 4);
 	    m68k_areg(regs, 7) -= 4;
 	    put_long (m68k_areg(regs, 7), m68k_getpc ());
 	    m68k_areg(regs, 7) -= 2;
 	    put_word (m68k_areg(regs, 7), regs.sr);
 	    regs.sr |= (1 << 13);
 	    m68k_areg(regs, 7) = regs.isp;
 	    m68k_areg(regs, 7) -= 2;
 	    put_word (m68k_areg(regs, 7), 0x1000 + nr * 4);
 	} else {
 	    m68k_areg(regs, 7) -= 2;
 	    put_word (m68k_areg(regs, 7), nr * 4);
 	}
    } else {
	if (nr == 2 || nr == 3) {
	    m68k_areg(regs, 7) -= 12;
	    /* ??????? */
	    if (nr == 3) {
		put_long (m68k_areg(regs, 7), last_fault_for_exception_3);
		put_word (m68k_areg(regs, 7)+4, last_op_for_exception_3);
		put_long (m68k_areg(regs, 7)+8, last_addr_for_exception_3);
	    }
#ifdef _MINEM68K_Debug
	    printf("Exception!\n");
#endif
	    goto kludge_me_do;
	}
    }
#endif
    m68k_areg(regs, 7) -= 4;
    put_long (m68k_areg(regs, 7), m68k_getpc ());
kludge_me_do:
    m68k_areg(regs, 7) -= 2;
    put_word (m68k_areg(regs, 7), regs.sr);
    m68k_setpc(get_long(4*nr));
    regs.t1 = regs.m = 0;
    regs.spcflags &= ~(SPCFLAG_DOTRACE);
}

void DiskInsertedPsuedoException(CPTR newpc, ULONG data)
{
    MakeSR();
    if (!regs.s) {
		regs.usp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.isp;
		regs.s = 1;
    }
    m68k_areg(regs, 7) -= 4;
    put_long (m68k_areg(regs, 7), m68k_getpc ());
    m68k_areg(regs, 7) -= 2;
    put_word (m68k_areg(regs, 7), regs.sr);
    m68k_areg(regs, 7) -= 4;
    put_long (m68k_areg(regs, 7), data);
    m68k_setpc(newpc);
    regs.t1 = regs.m = 0;
    regs.spcflags &= ~(SPCFLAG_DOTRACE);
}

Boolean ViaException(void)
{
	if (regs.intmask <= 1) {
		Exception(25);
		return true;
	} else {
		return false;
	}
}

void m68k_reset(void)
{

    customreset();

// Sets the MC68000 reset jump vector...
    m68k_setpc(get_long(0x00000004));

// Sets the initial stack vector...
    m68k_areg(regs, 7) = get_long(0x00000000);

    regs.s = 1;
    regs.m = 0;
    regs.stopped = 0;
    regs.t1 = 0;
    ZFLG = CFLG = NFLG = VFLG = 0;
    regs.spcflags = 0;
    regs.intmask = 7;
}

void MacInterrupt (void)
{	   
//specialflags |= SPCFLAG_DOINT;
//InterruptNo = 4;
//  if (regs.intmask <= 4)
//
// bill comment ---
//
// This statement below could be problematic
// because I took a educated guess for the
// value of the second parameter...
//
    Exception(28);
}

static MayInline ULONG get_disp_ea (ULONG base)
{
    UWORD dp = nextiword();
    int reg = (dp >> 12) & 15;
    LONG regd = regs.regs[reg];
    if ((dp & 0x800) == 0)
	regd = (LONG)(WORD)regd;
    return base + (BYTE)(dp) + regd;
}

static ULONG opsize;

void op_illg(ULONG);

#define AKMemory 0
#define AKAddrReg 1
#define AKDataReg 2
#define AKConstant 3
#define AKCCR 4
#define AKSR 5

static ULONG ArgKind;
static ULONG ArgAddr;

static void DecodeModeRegister(ULONG themode, ULONG thereg)
{
	switch (themode) {
   		case 0 :
			ArgKind = AKDataReg;
			ArgAddr = thereg;
			break;
   		case 1 :
			ArgKind = AKAddrReg;
			ArgAddr = thereg;
			break;
   		case 2 :
			ArgKind = AKMemory;
			ArgAddr = m68k_areg(regs, thereg);
			break;
   		case 3 :
			ArgKind = AKMemory;
			ArgAddr = m68k_areg(regs, thereg);
			if ((thereg == 7) && (opsize == 1)) {
				m68k_areg(regs, thereg) += 2;
			} else {
				m68k_areg(regs, thereg) += opsize;
			}
			break;
   		case 4 :
			ArgKind = AKMemory;
			if ((thereg == 7) && (opsize == 1)) {
				m68k_areg(regs, thereg) -= 2;
			} else {
				m68k_areg(regs, thereg) -= opsize;
			}
			ArgAddr = m68k_areg(regs, thereg);
			break;
   		case 5 :
			ArgKind = AKMemory;
   			ArgAddr = m68k_areg(regs, thereg) + (LONG)(WORD)nextiword();
			break;
   		case 6 :
			ArgKind = AKMemory;
   			ArgAddr = get_disp_ea(m68k_areg(regs, thereg));
			break;
   		case 7 :
 			switch (thereg) {
		   		case 0 :
					ArgKind = AKMemory;
		   			ArgAddr = (LONG)(WORD)nextiword();
					break;
		   		case 1 :
					ArgKind = AKMemory;
		   			ArgAddr = nextilong();
					break;
		   		case 2 :
					ArgKind = AKMemory;
		   			ArgAddr = m68k_getpc();
					ArgAddr += (LONG)(WORD)nextiword();
					break;
		   		case 3 :
					ArgKind = AKMemory;
		   			ArgAddr = get_disp_ea(m68k_getpc());
					break;
		   		case 4 :
					ArgKind = AKConstant;
		  			switch (opsize) {
		 		  		case 1:
							ArgAddr = (LONG)(BYTE)nextibyte();
							break;
		 		  		case 2:
							ArgAddr = (LONG)(WORD)nextiword();
							break;
				  		case 4:
							ArgAddr = nextilong();
							break;
		 			}
					break;
 			}
			break;
   		case 8 :
			ArgKind = AKConstant;
   			ArgAddr = thereg;
			break;
   		case 10 :
			ArgKind = AKCCR;
			break;
		case 11:
			ArgKind = AKSR;
			break;
	}
}

static LONG GetArgValue(void)
{
	LONG v;

	switch (ArgKind) {
   		case AKMemory:
  			switch (opsize) {
 		  		case 1:
					v = (LONG)(BYTE)get_byte(ArgAddr);
					break;
 		  		case 2:
					v = (LONG)(WORD)get_word(ArgAddr);
					break;
		  		case 4:
					v = get_long(ArgAddr);
					break;
 			}
			break;
   		case AKAddrReg:
			v = m68k_areg(regs, ArgAddr);
			break;
   		case AKDataReg:
			v = m68k_dreg(regs, ArgAddr);
  			switch (opsize) {
 		  		case 1:
					v = (LONG)(BYTE)v;
					break;
 		  		case 2:
					v = (LONG)(WORD)v;
					break;
		  		case 4:
					break;
 			}
			break;
		case AKConstant:
			v = ArgAddr;
			break;
		case AKCCR:
			MakeSR();
			v = regs.sr & 0xFF ;
			break;
		case AKSR:
			MakeSR();
			v = regs.sr;
			break;
	}
	return v;
}

static void SetArgValue(LONG v)
{
	switch (ArgKind) {
   		case AKMemory:
			switch (opsize) {
		  		case 1:
					put_byte(ArgAddr, v);
					break;
		  		case 2:
					put_word(ArgAddr, v);
					break;
		  		case 4:
					put_long(ArgAddr, v);
					break;
			}
			break;
  		case AKDataReg:
  			switch (opsize) {
 		  		case 1:
					m68k_dreg(regs, ArgAddr) = (m68k_dreg(regs, ArgAddr) & ~0xff) | ((v) & 0xff);
					break;
 		  		case 2:
					m68k_dreg(regs, ArgAddr) = (m68k_dreg(regs, ArgAddr) & ~0xffff) | ((v) & 0xffff);
					break;
		  		case 4:
					m68k_dreg(regs, ArgAddr) = v;
					break;
 			}
			break;
    	case AKAddrReg:
    		/* stricly speaking, illegal if opsize == 1 */
			m68k_areg(regs, ArgAddr) =  v;
			break;
		case AKCCR:
			regs.sr = (regs.sr & ~0xff) | ((v) & 0xff);
			MakeFromSR();
			break;
		case AKSR:
			regs.sr = v;
			MakeFromSR();
			break;
		defaut:
			op_illg (/* opcode */0);
			break;
	}
}

static ULONG opcode;

static void DoMove(ULONG m1, ULONG r1, ULONG m2,ULONG r2) /* MOVE */
{
	LONG src;

	DecodeModeRegister(m1, r1);
	src = GetArgValue();
	DecodeModeRegister(m2, r2);
	if (ArgKind != AKAddrReg) {
		VFLG = CFLG = 0;
		ZFLG = (src == 0);
		NFLG = (src < 0);
	}
	SetArgValue(src);
}

#define BinOpAdd 0
#define BinOpSub 1
#define BinOpAddX 2
#define BinOpSubX 3
#define BinOpAddBCD 4
#define BinOpSubBCD 5
#define BinOpOr 6
#define BinOpAnd 7
#define BinOpEor 8
#define BinOpASL 9
#define BinOpASR 10
#define BinOpLSL 11
#define BinOpLSR 12
#define BinOpRXL 13
#define BinOpRXR 14
#define BinOpROL 15
#define BinOpROR 16
#define BinOpBTst 17
#define BinOpBChg 18
#define BinOpBClr 19
#define BinOpBSet 20
#define BinOpMulU 21
#define BinOpMulS 22
#define BinOpDivU 23
#define BinOpDivS 24

#define extendopsizedstvalue() if (opsize == 1) {dstvalue = (BYTE)dstvalue;} else if  (opsize == 2) {dstvalue = (WORD)dstvalue;}

#define unextendopsizedstvalue() if (opsize == 1) {dstvalue = (UBYTE)dstvalue;} else if  (opsize == 2) {dstvalue = (UWORD)dstvalue;}

static void DoBinOp1(ULONG m1, ULONG r1, ULONG m2,ULONG r2, ULONG binop)
{
	LONG srcvalue;
	LONG dstvalue;

	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	DecodeModeRegister(m2, r2);
	dstvalue = GetArgValue();
	if (m2 == 1) {
		if (binop == BinOpAdd) {
			dstvalue += srcvalue;
		} else if (binop == BinOpSub) {
			dstvalue -= srcvalue;
		} else {
			op_illg(opcode);
			return;
		}
	} else {
		switch (binop) {
			case BinOpAdd:
				{
					int flgs = (srcvalue < 0);
					int flgo = (dstvalue < 0);
					dstvalue = (dstvalue + srcvalue);
					extendopsizedstvalue();
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = (flgs && flgo && !NFLG) || (!flgs && !flgo && NFLG);
					XFLG = CFLG = (flgs && flgo) || (!NFLG && (flgo || flgs));
				}
				break;
			case BinOpSub:
				{
					int flgs = (srcvalue < 0);
					int flgo = (dstvalue < 0);
					dstvalue = dstvalue - srcvalue;
					extendopsizedstvalue();
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = (flgs != flgo) && (NFLG != flgo);
					XFLG = CFLG = (flgs && !flgo) || (NFLG && (!flgo || flgs));
				}
				break;
			case BinOpSubX:
				{
					int flgs = (srcvalue < 0);
					int flgo = (dstvalue < 0);
					dstvalue = dstvalue - srcvalue - (XFLG ? 1 : 0);
					extendopsizedstvalue();
					if (dstvalue != 0) {
						ZFLG = 0;
					}
					NFLG = (dstvalue < 0);
					VFLG = (!flgs && flgo && !NFLG) || (flgs && !flgo && NFLG);
					XFLG = CFLG = (flgs && !flgo) || (NFLG && (!flgo || flgs));
				}
				break;
			case BinOpAddX:
				{
					int flgs = (srcvalue < 0);
					int flgo = (dstvalue < 0);
					dstvalue += srcvalue + (XFLG ? 1 : 0);
					extendopsizedstvalue();
					if (dstvalue != 0) {
						ZFLG = 0;
					}
					NFLG = (dstvalue < 0);
					XFLG = CFLG = (flgs && flgo) || (!NFLG && (flgo || flgs));
					VFLG = (flgs && flgo && !NFLG) || (!flgs && !flgo && NFLG);
				}
				break;
			case BinOpAnd:
				dstvalue &= srcvalue;
				VFLG = CFLG = 0;
				ZFLG = (dstvalue == 0);
				NFLG = (dstvalue < 0);
				break;
			case BinOpOr:
				dstvalue |= srcvalue;
				VFLG = CFLG = 0;
				ZFLG = (dstvalue == 0);
				NFLG = (dstvalue < 0);
				break;
			case BinOpEor:
				dstvalue ^= srcvalue;
				VFLG = CFLG = 0;
				ZFLG = (dstvalue == 0);
				NFLG = (dstvalue < 0);
				break;
			case BinOpASL:
				{
					ULONG cnt = ((ULONG)srcvalue) & 63;
					LONG dstvalue0 = dstvalue;
					LONG comparevalue;
					if (! cnt) {
						VFLG = 0;
						CFLG = 0;
					} else {
						if (cnt > 32) {
							dstvalue = 0;
						} else {
							dstvalue = (LONG)(((ULONG)dstvalue) << (cnt - 1));
						}
						extendopsizedstvalue();
						CFLG = XFLG = (dstvalue < 0);
						dstvalue = (LONG)(((ULONG)dstvalue) << 1);
						extendopsizedstvalue();
					}
					if (dstvalue < 0) {
						comparevalue = -(LONG)(((ULONG)-dstvalue) >> (cnt));
					} else {
						comparevalue = (LONG)(((ULONG)dstvalue) >> (cnt));
					}
					VFLG = (comparevalue != dstvalue0);
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
				}
				break;
			case BinOpASR:
				{
					ULONG cnt = ((ULONG)srcvalue) & 63;
					NFLG = (dstvalue < 0);
					VFLG = 0;
					if (! cnt) { 
						CFLG = 0;
					} else {
						if (NFLG) {
							dstvalue = (~dstvalue);
						}
						unextendopsizedstvalue();
						if (cnt > 32) {
							dstvalue = 0;
						} else {
							dstvalue = (LONG)(((ULONG)dstvalue) >> (cnt - 1));
						}
						CFLG = ((ULONG)dstvalue & 1);
						dstvalue = (LONG)(((ULONG)dstvalue) >> 1);
						if (NFLG) {
							CFLG = ! CFLG;
							dstvalue = (~dstvalue);
						}
						XFLG = CFLG;
					}
					ZFLG = (dstvalue == 0);
				}
				break;
			case BinOpLSL:
				{
					ULONG cnt = ((ULONG)srcvalue) & 63;
					if (! cnt) {
						CFLG = 0;
					} else {
						if (cnt > 32) {
							dstvalue = 0;
						} else {
							dstvalue = (LONG)(((ULONG)dstvalue) << (cnt - 1));
						}
						extendopsizedstvalue();
						CFLG = XFLG = (dstvalue < 0);
						dstvalue = (LONG)(((ULONG)dstvalue) << 1);
						extendopsizedstvalue();
					}
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = 0;
				}
				break;
			case BinOpLSR:
				{
					ULONG cnt = ((ULONG)srcvalue) & 63;
					if (! cnt) { 
						CFLG = 0;
					} else {
						unextendopsizedstvalue();
						if (cnt > 32) {
							dstvalue = 0;
						} else {
							dstvalue = (LONG)(((ULONG)dstvalue) >> (cnt - 1));
						}
						CFLG = XFLG = ((ULONG)dstvalue & 1);
						dstvalue = (LONG)(((ULONG)dstvalue) >> 1);
					}
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = 0;
				}
				break;
			case BinOpROL:
				{
					ULONG cnt = ((ULONG)srcvalue) & 63;
					if (! cnt) { 
						CFLG = 0;
					} else {
						for (;cnt;--cnt) {
							CFLG = (dstvalue < 0);
							dstvalue = (LONG)(((ULONG)dstvalue) << 1);
							if (CFLG) {
								dstvalue = (LONG)(((ULONG)dstvalue) | 1);
							}
							extendopsizedstvalue();
						}
					}
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = 0;
				}
				break;
			case BinOpRXL:
				{
					ULONG cnt = ((ULONG)srcvalue) & 63;
					if (! cnt) { 
						CFLG = XFLG;
					} else {
						for (;cnt;--cnt) {
							CFLG = (dstvalue < 0);
							dstvalue = (LONG)(((ULONG)dstvalue) << 1);
							if (XFLG) {
								dstvalue = (LONG)(((ULONG)dstvalue) | 1);
							}
							extendopsizedstvalue();
							XFLG = CFLG;
						}
					}
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = 0;
				}
				break;
			case BinOpROR:
				{
					ULONG cnt = ((ULONG)srcvalue) & 63;
					ULONG cmask = (ULONG)1 << (opsize * 8 - 1);
					if (! cnt) { 
						CFLG = 0;
					} else {
						unextendopsizedstvalue();
						for(;cnt;--cnt){
							CFLG = ((((ULONG)dstvalue) & 1) != 0);
							dstvalue = (LONG)(((ULONG)dstvalue) >> 1);
							if (CFLG) {
								dstvalue = (LONG)(((ULONG)dstvalue) | cmask);
							}
						}
						extendopsizedstvalue();
					}
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = 0;
				}
				break;
			case BinOpRXR:
				{
					ULONG cnt = ((ULONG)srcvalue) & 63;
					ULONG cmask = (ULONG)1 << (opsize * 8 - 1);
					if (! cnt) { 
						CFLG = XFLG;
					} else {
						unextendopsizedstvalue();
						for (;cnt;--cnt) {
							CFLG = ((((ULONG)dstvalue) & 1) != 0);
							dstvalue = (LONG)(((ULONG)dstvalue) >> 1);
							if (XFLG) {
								dstvalue = (LONG)(((ULONG)dstvalue) | cmask);
							}
							XFLG = CFLG;
						}
						extendopsizedstvalue();
					}
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = 0;
				}
				break;
			case BinOpAddBCD:
				{
					/* if (opsize != 1) a bug */
					int flgs = (srcvalue < 0);
					int flgo = (dstvalue < 0);
					UWORD newv_lo = (srcvalue & 0xF) + (dstvalue & 0xF) + (XFLG ? 1 : 0);
					UWORD newv_hi = (srcvalue & 0xF0) + (dstvalue & 0xF0);
					UWORD newv;
					
					if (newv_lo > 9) { 
						newv_lo +=6;
					}
					newv = newv_hi + newv_lo;
					CFLG = XFLG = (newv & 0x1F0) > 0x90;
					if (CFLG) {
						newv += 0x60;
					}
					dstvalue = (BYTE)newv;
					if (dstvalue != 0) {
						ZFLG = 0;
					}
					NFLG = (dstvalue < 0);
					VFLG = (flgs != flgo) && (NFLG != flgo);
					/* but according to my reference book, VFLG is Undefined for ABCD */
				}
				break;
			case BinOpSubBCD:
				{
					int flgs = (srcvalue < 0);
					int flgo = (dstvalue < 0);
					UWORD newv_lo = (dstvalue & 0xF) - (srcvalue & 0xF) - (XFLG ? 1 : 0);
					UWORD newv_hi = (dstvalue & 0xF0) - (srcvalue & 0xF0);
					UWORD newv;
					
					if (newv_lo > 9) { 
						newv_lo -= 6;
						newv_hi -= 0x10;
					}
					newv = newv_hi + (newv_lo & 0xF);
					CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
					if (CFLG) {
						newv -= 0x60;
					}
					dstvalue = (BYTE)newv;
					if (dstvalue != 0) {
						ZFLG = 0;
					}
					NFLG = (dstvalue < 0);
					VFLG = (flgs != flgo) && (NFLG != flgo);
					/* but according to my reference book, VFLG is Undefined for SBCD */
				}
				break;
			default:
				op_illg(opcode);
				return;
				break;
		}
	}
	SetArgValue(dstvalue);
}

static void DoCompare(ULONG m1, ULONG r1, ULONG m2,ULONG r2)
{
	LONG srcvalue;
	LONG dstvalue;

	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	DecodeModeRegister(m2, r2);
	dstvalue = GetArgValue();
	{
		int flgs = (srcvalue < 0);
		int flgo = (dstvalue < 0);
		dstvalue -= srcvalue;
		if (m2 != 1) {
			extendopsizedstvalue();
		}
		ZFLG = (dstvalue == 0);
		NFLG = (dstvalue < 0);
		VFLG = (flgs != flgo) && (NFLG != flgo);
		CFLG = (flgs && !flgo) || (NFLG && (!flgo || flgs));
	}
}

static void DoCompareA(ULONG m1, ULONG r1, ULONG m2,ULONG r2)
{
	LONG srcvalue;
	LONG dstvalue;

	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	if (m1 == 1) {
		switch (opsize) {
	  		case 1:
				srcvalue = (BYTE)srcvalue;
				break;
	  		case 2:
				srcvalue = (WORD)srcvalue;
				break;
	  		case 4:
				break;
		}
	} /* if m1 != 1 then GetArgValue already has signed extended to 32 bits */
	
	DecodeModeRegister(m2, r2);
	dstvalue = GetArgValue();
	{
		int flgs = (srcvalue < 0);
		int flgo = (dstvalue < 0);
		dstvalue -= srcvalue;
		ZFLG = (dstvalue == 0);
		NFLG = (dstvalue < 0);
		VFLG = (flgs != flgo) && (NFLG != flgo);
		CFLG = (flgs && !flgo) || (NFLG && (!flgo || flgs));
	}
}

static void DoBinBitOp1(ULONG m1, ULONG r1, ULONG m2,ULONG r2, ULONG binop)
{
	LONG srcvalue;
	LONG dstvalue;

	opsize = 1;
	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	if (m2 == 0) {
		opsize = 4;
	}
	DecodeModeRegister(m2, r2);
	dstvalue = GetArgValue();
	if (m2 == 1) {
		/*
			WritelnDebug('**** Operation not allowed on Address Registers');
			emuerror;
		*/
		Debugger();
	} else {
		if (m2 != 0) {
			srcvalue &= 7;
		} else {
			srcvalue &= 31;
		}
		ZFLG = (((ULONG)dstvalue & ((ULONG)1 << srcvalue)) == 0);
		if (binop != BinOpBTst) {
			switch (binop) {
				case BinOpBChg:
					dstvalue ^= (1 << srcvalue);
					break;
				case BinOpBClr:
					dstvalue &= ~(1 << srcvalue);
					break;
				case BinOpBSet:
					dstvalue |= (1 << srcvalue);
					break;
				default:
					op_illg(opcode);
					return;
					break;
			}
			SetArgValue(dstvalue);
		}
	}
}

#define UniOpNot 0
#define UniOpNeg 1
#define UniOpNegX 2
#define UniOpNbcd 3
#define UniOpTAS 4

static void DoUniOp1(ULONG m2, ULONG r2, ULONG uniop)
{
	LONG dstvalue;

	if (m2 == 1) {
		/*
		WritelnDebug('**** Operation not allowed on Address Registers');
		emuerror;
		*/
		op_illg(opcode);
	} else {
		DecodeModeRegister(m2, r2);
		dstvalue = GetArgValue();

		switch (uniop) {
			case UniOpNegX:
				{
					int flgs = (dstvalue < 0);
					dstvalue = 0 - dstvalue - (XFLG ? 1 : 0);
					extendopsizedstvalue();
					if (dstvalue != 0) {
						ZFLG = 0;
					}
					NFLG = (dstvalue < 0);
					VFLG = (flgs && NFLG);
					XFLG = CFLG = (flgs || NFLG);
				}
				break;
			case UniOpNeg:
				{
					int flgs = (dstvalue < 0);
					dstvalue = 0 - dstvalue;
					extendopsizedstvalue();
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = (flgs && NFLG);
					XFLG = CFLG = (flgs || NFLG);
				}
				break;
			case UniOpNot:
				{
					dstvalue = ~dstvalue;
					extendopsizedstvalue();
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = CFLG = 0;
				}
				break;
			case UniOpNbcd:
				{
					UWORD newv_lo = - (dstvalue & 0xF) - (XFLG ? 1 : 0);
					UWORD newv_hi = - (dstvalue & 0xF0);
					UWORD newv;
					
					if (newv_lo > 9) { 
						newv_lo -= 6;
						newv_hi -= 0x10;
					}
					newv = newv_hi + (newv_lo & 0xF);
					CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
					if (CFLG) {
						newv -= 0x60;
					}
						
					dstvalue = newv;
					extendopsizedstvalue();
					NFLG = (dstvalue < 0);
					if (dstvalue != 0) {
						ZFLG = 0;
					}
				}

				break;
			case UniOpTAS:
				{
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = CFLG = 0;
					dstvalue |= 0x80;
				}
				break;
		}
		/* DoUniOp(uniop,opsize,dstvalue,CCRregister); */
		SetArgValue(dstvalue);
	}
}

static void DoBinOpMul1(ULONG m1, ULONG r1, ULONG m2,ULONG r2, ULONG binop)
{
	LONG srcvalue;
	LONG dstvalue;

	opsize = 2;
	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	DecodeModeRegister(m2, r2);
	dstvalue = GetArgValue();
	if (m2 == 1) {
		op_illg(opcode);
		return;
	} else {
		switch (binop) {
			case BinOpMulU:
				dstvalue = (ULONG)(UWORD)dstvalue * (ULONG)(UWORD)srcvalue;
				VFLG = CFLG = 0;
				ZFLG = (dstvalue == 0);
				NFLG = (dstvalue < 0);
				break;
			case BinOpMulS:
				dstvalue = (LONG)(WORD)dstvalue * (LONG)(WORD)srcvalue;
				VFLG = CFLG = 0;
				ZFLG = (dstvalue == 0);
				NFLG = (dstvalue < 0);
				break;
			default:
				op_illg(opcode);
				return;
				break;
		}
	}
	opsize = 4;
	SetArgValue(dstvalue);
}

static void DoBinOpDiv1(ULONG m1, ULONG r1, ULONG m2,ULONG r2, ULONG binop)
{
	LONG srcvalue;
	LONG dstvalue;

	opsize = 2;
	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	opsize = 4;
	DecodeModeRegister(m2, r2);
	dstvalue = GetArgValue();
	if (m2 == 1) {
		op_illg(opcode);
		return;
	} else if (srcvalue == 0) {
		Exception(5);
	} else {
		switch (binop) {
			case BinOpDivU:
				{
					ULONG newv = (ULONG)dstvalue / (ULONG)(UWORD)srcvalue;
					ULONG rem = (ULONG)dstvalue % (ULONG)(UWORD)srcvalue;
					if (newv > 0xffff) {
						VFLG = NFLG = 1;
						CFLG = 0;
					} else {
						VFLG = CFLG = 0;
						ZFLG = ((WORD)(newv)) == 0;
						NFLG = ((WORD)(newv)) < 0;
						newv = (newv & 0xffff) | ((ULONG)rem << 16);
						dstvalue = newv;
					}
				}
				break;
			case BinOpDivS:
				{
					LONG newv = (LONG)dstvalue / (LONG)(WORD)srcvalue;
					UWORD rem = (LONG)dstvalue % (LONG)(WORD)srcvalue;
					if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
						VFLG = NFLG = 1; CFLG = 0;
					} else {
						if (((WORD)rem < 0) != ((LONG)dstvalue < 0)) rem = -rem;
						VFLG = CFLG = 0;
						ZFLG = ((WORD)(newv)) == 0;
						NFLG = ((WORD)(newv)) < 0;
						newv = (newv & 0xffff) | ((ULONG)rem << 16);
						dstvalue = newv;
					}
				}
				break;
			default:
				op_illg(opcode);
				return;
				break;
		}
	}
	SetArgValue(dstvalue);
}

#define b76 ((opcode >> 6) & 3)
#define b8 ((opcode >> 8) & 1)
#define mode ((opcode >> 3) & 7)
#define reg (opcode & 7)
#define md6 ((opcode >> 6) & 7)
#define rg9 ((opcode >> 9) & 7)

static void FindOpSizeFromb76(void)
{
	switch (b76) {
		case 0 :
			opsize = 1;
			break;
		case 1 :
			opsize = 2;
			break;
		case 2 :
			opsize = 4;
			break;
	}
}

static ULONG bitop(void)
{
	switch (b76) {
		case 0 :
			return BinOpBTst;
			break;
		case 1 :
			return BinOpBChg;
			break;
		case 2 :
			return BinOpBClr;
			break;
		case 3 :
			return BinOpBSet;
			break;
		default :
			Debugger();
			return -1;
			break;
	}
}

static ULONG octdat(ULONG x)
{
	if (x == 0) {
		return 8;
	} else {
		return x;
	}
}

static Boolean GetEffectiveAddress(LONG *v)
{
	if (ArgKind == AKMemory) {
		*v = ArgAddr;
		return true;
	} else {
		Debugger();
		return false;
	}
}

LOCALPROCUSEDONCE DoCode0(void)
{
	if (b8 == 1) {
		if (mode == 1) {
			/* MoveP 0000ddd1mm001aaa */

						
			switch (b76) {
				case 0:
					{
						ULONG srcreg = reg;
						ULONG dstreg = rg9;
						CPTR memp = m68k_areg(regs, srcreg) + nextiword();
						UWORD val = (get_byte(memp) << 8) + get_byte(memp + 2);
						m68k_dreg(regs, dstreg) = (m68k_dreg(regs, dstreg) & ~0xffff) | ((val) & 0xffff);
					}
					break;
				case 1:
					{
						ULONG srcreg = reg;
						ULONG dstreg = rg9;
						CPTR memp = m68k_areg(regs, srcreg) + nextiword();
						ULONG val = (get_byte(memp) << 24) + (get_byte(memp + 2) << 16)
					              + (get_byte(memp + 4) << 8) + get_byte(memp + 6);
						m68k_dreg(regs, dstreg) = (val);
					}
					break;
				case 2:
					{
						ULONG srcreg = rg9;
						ULONG dstreg = reg;
						WORD src = m68k_dreg(regs, srcreg);
						CPTR memp = m68k_areg(regs, dstreg) + nextiword();
						put_byte(memp, src >> 8); put_byte(memp + 2, src);
					}
					break;
				case 3:
					{
						ULONG srcreg = rg9;
						ULONG dstreg = reg;
						LONG src = m68k_dreg(regs, srcreg);
						CPTR memp = m68k_areg(regs, dstreg) + nextiword();
						put_byte(memp, src >> 24); put_byte(memp + 2, src >> 16);
						put_byte(memp + 4, src >> 8); put_byte(memp + 6, src);
					}
					break;
			}

		/*
			opsize := BAND(b76, 1) * 2 + 2;
			argn := 2;
			IF b76 < 2 THEN BEGIN
				adec(1, 5, reg, data);
				adec(2, 0, reg, alter);
			END ELSE BEGIN
				adec(1, 0, reg, data);
				adec(2, 5, reg, alter);
			END;
		*/
			/* notimplemented; */
		} else {
			/* dynamic bit, Opcode = 0000ddd1ttmmmrrr */
			DoBinBitOp1(0, rg9, mode, reg, bitop());
		}
	} else {
		if (rg9 == 4) {
			/* static bit 00001010ssmmmrrr */
			DoBinBitOp1(7 ,4, mode, reg, bitop());
		} else if (rg9 == 6) {
			FindOpSizeFromb76();
			DoCompare(7, 4, mode, reg);
		} else if (rg9 == 7) {
			Debugger();
			/* MoveS */

			if (!regs.s) {
				BackupPC();
				Exception(8);
			} else {
				ULONG dstreg = reg;
				WORD extra = nextiword();
				if (extra & 0x800) {
					ULONG src = regs.regs[(extra >> 12) & 15];
					CPTR dsta = m68k_areg(regs, dstreg);
					put_byte(dsta,src);
				}else{
					CPTR srca = m68k_areg(regs, dstreg);
					BYTE src = get_byte(srca);
					if (extra & 0x8000) {
						m68k_areg(regs, (extra >> 12) & 7) = (LONG)(BYTE)src;
					} else {
						m68k_dreg(regs, (extra >> 12) & 7) = (m68k_dreg(regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
					}
				}
			}

		} else {
			ULONG BinOp;

			FindOpSizeFromb76();
			switch (rg9) {
				case 0 :
					BinOp = BinOpOr;
					break;
				case 1 :
					BinOp = BinOpAnd;
					break;
				case 2 :
					BinOp = BinOpSub;
					break;
				case 3 :
					BinOp = BinOpAdd;
					break;
				case 5 :
					BinOp = BinOpEor;
					break;
			}
			if (((opcode & 0x0400) == 0) && (mode == 7) && (reg == 4)) {
				if (b76 == 0) {					
					DoBinOp1(7, 4, 10, reg, BinOp);
				} else {
					if (!regs.s) {
						BackupPC();
						Exception(8);
					} else {
						DoBinOp1(7, 4, 11, reg, BinOp);
					}
				}
			} else {
				DoBinOp1(7, 4, mode, reg, BinOp);
			}
		}
	}
}

LOCALPROCUSEDONCE DoCode1(void)
{
	opsize = 1;
	DoMove(mode, reg, md6, rg9);
}

LOCALPROCUSEDONCE DoCode2(void)
{
	opsize = 4;
	DoMove(mode, reg, md6, rg9);
}

LOCALPROCUSEDONCE DoCode3(void)
{
	opsize = 2;
	DoMove(mode, reg, md6, rg9);
}

LOCALPROCUSEDONCE DoCheck(ULONG m1, ULONG r1, ULONG m2, ULONG r2)
{
	LONG srcvalue;
	LONG dstvalue;

	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	DecodeModeRegister(m2, r2);
	dstvalue = GetArgValue();
	if (dstvalue < 0) {
		NFLG=1;
		Exception(6);
	} else if (dstvalue > srcvalue) {
		NFLG=0;
		Exception(6);
	}
}

LOCALPROCUSEDONCE DoLea(ULONG m1, ULONG r1, ULONG r2)
{
	LONG srcvalue;

	DecodeModeRegister(m1, r1);
	if (GetEffectiveAddress(&srcvalue)) {
		m68k_areg(regs, r2) = srcvalue;
	}
}

LOCALPROCUSEDONCE DoTest1(ULONG m1, ULONG r1)
{
	LONG srcvalue;

	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();

	VFLG = CFLG = 0;
	ZFLG = (srcvalue == 0);
	NFLG = (srcvalue < 0);
}

static void reglist (WORD direction, ULONG m1, ULONG r1)
{
	WORD z;
	LONG p;
	ULONG regmask;

	regmask = nextiword();
	switch (m1) {
		case 3:
			if (direction == 1) {
				p = m68k_areg(regs, r1);
				if (opsize == 2) {
					for (z = 0; z < 16; ++z) {
						if ((regmask & (1 << (z))) != 0) {
							regs.regs[z] = (LONG)(WORD)get_word(p);
							p += 2;
						}
					}
					m68k_areg(regs, r1) = (LONG)p;
				} else {
					for (z = 0; z < 16; ++z) {
						if ((regmask & (1 << (z))) != 0) {
							regs.regs[z] = get_long(p);
							p += 4;
						}
					}
				}
				m68k_areg(regs, r1) = p;
			}
			break;
		case 4:
			if (direction == 0) {
				p = m68k_areg(regs, r1);
				if (opsize == 2) {
					for (z = 16; --z >=0; ) {
						if ((regmask & (1 << (15-z))) != 0) {
							p -= 2;
							put_word(p, regs.regs[z]);
						}
					}
				} else {
					for (z = 16; --z >=0; ) {
						if ((regmask & (1 << (15-z))) != 0) {
							p -= 4;
							put_long(p, regs.regs[z]);
						}
					}
				}
				m68k_areg(regs, r1) = p;
			}
			break;
		default:
			DecodeModeRegister(m1, r1);
			if (GetEffectiveAddress(&p)) {
				if (direction == 0) {
					if (opsize == 2) {
						for (z = 0; z < 16; ++z) {
							if ((regmask & (1 << (z))) != 0) {
								put_word(p, regs.regs[z]);
								p += 2;
							}
						}
					} else {
						for (z = 0; z < 16; ++z) {
							if ((regmask & (1 << (z))) != 0) {
								put_long(p, regs.regs[z]);
								p += 4;
							}
						}
					}
				} else {
					if (opsize == 2) {
						for (z = 0; z < 16; ++z) {
							if ((regmask & (1 << (z))) != 0) {
								regs.regs[z] = (LONG)(WORD)get_word(p);
								p += 2;
							}
						}
					} else {
						for (z = 0; z < 16; ++z) {
							if ((regmask & (1 << (z))) != 0) {
								regs.regs[z] = get_long(p);
								p += 4;
							}
						}
					}
				}
			}
			break;
	}
}

void m68k_setstopped(void);

LOCALPROCUSEDONCE DoCode4(void)
{
	if (b8 != 0) {
		switch (b76) {
			case 0 :
			case 2 :
				/* Chk 0100ddd1s0mmmrrr */
				opsize = 4 - b76;
				DoCheck(mode, reg, 0, rg9);
				break;
			case 1 :
				op_illg(opcode);
				break;
			case 3 :
				/* Lea 0100aaa111mmmrrr */
				opsize = 4;
				DoLea(mode, reg, rg9);
				break;
		}
	} else {
		switch (rg9) {
			case 0 :
				if (b76 != 3) {
					/* NegX 01000000ssmmmrrr */
					FindOpSizeFromb76();
					DoUniOp1(mode,reg, UniOpNegX);
				} else {
					opsize = 2;
					DoMove(11,0, mode, reg);
				}
				break;
			case 1 :
				if (b76 != 3) {
					/* Clr 01000010ssmmmrrr */
					FindOpSizeFromb76();
					DoMove(8, 0, mode, reg);
				} else {
					/* 0100001011mmmrrr */
					opsize = 2;
					DoMove(10, 0, mode, reg);
				}
				break;
			case 2 :
				if (b76 != 3) {
					/* Neg 01000100ssmmmrrr */
					FindOpSizeFromb76();
					DoUniOp1(mode,reg, UniOpNeg);
				} else {
					/* 0100010011mmmrrr */
					opsize = 2;
					DoMove(mode, reg, 10, 0);
				}
				break;
			case 3 :
				if (b76 != 3) {
					/* Not 01000110ssmmmrrr */
					FindOpSizeFromb76();
					DoUniOp1(mode, reg, UniOpNot);
				} else {
					/* 0100011011mmmrrr */
					opsize = 2;
					DoMove(mode, reg, 11, 0);
				}
				break;
			case 4 :
				switch (b76) {
					case 0 :
						/* Nbcd 0100100000mmmrrr */
						opsize = 1;
						DoUniOp1(mode, reg, UniOpNbcd);
						break;
					case 1 :
						if (mode == 0) {
							/* Swap 0100100001000rrr */
							ULONG srcreg = reg;
							ULONG src = (ULONG)m68k_dreg(regs, srcreg);
							LONG dst = (LONG)(((src >> 16)&0xFFFF) | ((src&0xFFFF)<<16));
							VFLG = CFLG = 0;
							ZFLG = (dst == 0);
							NFLG = (dst < 0);
							m68k_dreg(regs, srcreg) = dst;
						} else {
							LONG srcvalue;

							opsize = 4;
							DecodeModeRegister(mode, reg);
							if (GetEffectiveAddress(&srcvalue)) {
								m68k_areg(regs, 7) -= 4;
								put_long(m68k_areg(regs, 7), srcvalue);
							}
						}
						break;
					case 2 :
					case 3 :
						if (mode == 0) {
							if (b76 == 2) {
								/* EXT.W */
								ULONG srcreg = reg;
								LONG src = m68k_dreg(regs, srcreg);
								UWORD dst = (WORD)(BYTE)src;
								VFLG = CFLG = 0;
								ZFLG = ((WORD)(dst)) == 0;
								NFLG = ((WORD)(dst)) < 0;
								m68k_dreg(regs, srcreg) = (m68k_dreg(regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
							} else {
								/* EXT.L */
								ULONG srcreg = reg;
								LONG src = m68k_dreg(regs, srcreg);
								ULONG dst = (LONG)(WORD)src;
								VFLG = CFLG = 0;
								ZFLG = ((LONG)(dst)) == 0;
								NFLG = ((LONG)(dst)) < 0;
								m68k_dreg(regs, srcreg) = (dst);
							}
						} else {
							opsize = 2 * b76 - 2;
							reglist (0,mode,reg);
						}
						break;
				}
				break;
			case 5 :
				if (b76 != 3) {
					/* Tst  01001010ssmmmrrr */
					FindOpSizeFromb76();
					DoTest1(mode, reg);
				} else {
					if ((mode == 7) && (reg)) {
						op_illg(opcode); /* the ILLEGAL instruction */
					} else {
						/* Tas 0100101011mmmrrr */
						opsize = 1;
						DoUniOp1(mode, reg, UniOpTAS);
					}
				}
				break;
			case 6 :
				if (((opcode >> 7) & 1) == 1) {
					opsize = 2 * b76 - 2;
					reglist (1,mode,reg);
				} else {
					/* MULS/MULU/DIVS/DIVU long operations for MC68020 */
					op_illg(opcode);
				}
				break;
			case 7 :
				switch (b76) {
					case 0 :
						op_illg(opcode);
						break;
					case 1 :
						switch (mode) {
							case 0 :
							case 1 :
								/* Trap 010011100100vvvv */
								Exception((opcode & 15)+32);
								break;
							case 2 :
								/* Link */
								{
									ULONG srcreg = reg;
									CPTR stackp = m68k_areg(regs, 7);
									stackp -= 4;
									m68k_areg(regs, 7) = stackp; /* only matters if srcreg == 7 */
									put_long(stackp, m68k_areg(regs, srcreg));
									m68k_areg(regs, srcreg) = stackp;
									m68k_areg(regs, 7) += (LONG)(WORD)nextiword();
								}
								break;
							case 3 :
								/* Unlk */
								{
									ULONG srcreg = reg;
									if (srcreg != 7) {
										LONG src = m68k_areg(regs, srcreg);
										m68k_areg(regs, srcreg) = get_long(src);
										m68k_areg(regs, 7) =  src + 4;
									} else {
										/* wouldn't expect this to happen */
										m68k_areg(regs, 7) = get_long(m68k_areg(regs, 7)) + 4;
									}
								}
								break;
							case 4 :
								/* MOVE USP 0100111001100aaa */
								if (! regs.s) {
									BackupPC();
									Exception(8);
								} else {
									regs.usp = m68k_areg(regs, reg);
								}
								break;
							case 5 :
								/* MOVE USP 0100111001101aaa */
								if (! regs.s) {
									BackupPC();
									Exception(8);
								} else {
									m68k_areg(regs, reg) = regs.usp;
								}
								break;
							case 6 :
								opsize = 0;
								switch (reg) {
									case 0 :
										/* Reset 0100111001100000 */
										if (! regs.s) {
											BackupPC();
											Exception(8);
										} else {
											customreset();
										}
										break;
									case 1 :
										/* Nop Opcode = 0100111001110001 */
										break;
									case 2 :
										/* Stop 0100111001110010 */
										if (!regs.s) {
											BackupPC();
											Exception(8);
										} else {
											regs.sr = (WORD)nextiword();
											MakeFromSR();
											m68k_setstopped();
										}
										break;
									case 3 :
										/* Rte 0100111001110011 */
										if (! regs.s) { 
											BackupPC();
											Exception(8);
										} else {
											CPTR stackp = m68k_areg(regs, 7);
											regs.sr = get_word(stackp);
											MakeFromSR();
											stackp += 2;
											m68k_setpc(get_long(stackp));
											stackp += 4;
											m68k_areg(regs, 7) = stackp;
											#if 0
											WORD format;
											
											for (;;) {
												CPTR stackp = m68k_areg(regs, 7);
												
												regs.sr = get_word(stackp);
												MakeFromSR();
												stackp += 2;
												m68k_setpc(get_long(stackp));
												stackp += 4;
												format = get_word(stackp);
												stackp += 2;
												m68k_areg(regs, 7) = stackp;
												if ((format & 0xF000) == 0x0000) { 
													break;
												} else if ((format & 0xF000) == 0x1000) { 
												} else if ((format & 0xF000) == 0x2000) {
													m68k_areg(regs, 7) += 4;
													break;
												} else {
													Exception(14);
													break;
												}
											
											}
											#endif
										}
										break;
									case 4 :
										/* Rtd 0100111001110100 */
										op_illg(opcode);;
										break;
									case 5 :
										/* Rts 0100111001110101 */
									    m68k_setpc(get_long(m68k_areg(regs, 7)));
									    m68k_areg(regs, 7) += 4;
										break;
									case 6 :
										/* TrapV 0100111001110110 */
										if(VFLG) {
											Exception(7);
										}
										break;
									case 7 :
										/* Rtr 0100111001110111 */
										{
											WORD sr;
											
											CPTR stackp = m68k_areg(regs, 7);
											MakeSR();
											sr = get_word(stackp);
											regs.sr = (regs.sr & 0xFF00) | (sr & 0xFF);
											MakeFromSR();
											stackp += 2;
											m68k_setpc(get_long(stackp));
											stackp += 4;
											m68k_areg(regs, 7) = stackp;
										}
										break;
								}
								break;
							case 7 :
								op_illg(opcode);
								break;
						}
						break;
					case 2 :
						/* Jsr 0100111010mmmrrr */
						{
							LONG srcvalue;

							opsize = 0;
							DecodeModeRegister(mode, reg);
							if (GetEffectiveAddress(&srcvalue)) {
								m68k_areg(regs, 7) -= 4;
								put_long(m68k_areg(regs, 7), m68k_getpc());
								m68k_setpc(srcvalue);
							}
						}
						break;
					case 3 :
						/* JMP 0100111011mmmrrr */
						{
							LONG srcvalue;

							opsize = 0;
							DecodeModeRegister(mode, reg);
							if (GetEffectiveAddress(&srcvalue)) {
								m68k_setpc(srcvalue);
							}
						}
						break;
				}
				break;
		}
	}
}

LOCALPROCUSEDONCE DoCode5(void)
{
	if (b76 == 3) {
		ULONG cond = (opcode >> 8) & 15;
		
		if (mode == 1) {
			/* DBcc 0101cccc11001ddd */
			LONG srcvalue;
			LONG dstvalue;
			opsize = 2;
			DecodeModeRegister(7, 2);
			srcvalue = ArgAddr;
			if (! cctrue(cond)) {
				DecodeModeRegister(0, reg);
				dstvalue = GetArgValue();
				--dstvalue;
				SetArgValue(dstvalue);
				if (dstvalue != -1) {
					m68k_setpc(srcvalue);
				}
			}
		} else {
			/* Scc 0101cccc11mmmrrr */
			opsize = 1;
			DecodeModeRegister(mode, reg);
			SetArgValue(cctrue(cond) ? 0xff : 0);
		}
	} else {
		ULONG BinOp;

		/* 0101nnnossmmmrrr */
		FindOpSizeFromb76();
		if (b8 == 0) {
			BinOp = BinOpAdd; /* DivU 0101nnn0ssmmmrrr */
		} else {
			BinOp = BinOpSub; /* DivS 0101nnn1ssmmmrrr */
		}
		DoBinOp1(8, octdat(rg9), mode,reg, BinOp);
	}
}

LOCALPROCUSEDONCE DoCode6(void)
{
	ULONG cond = (opcode >> 8) & 15;
	ULONG src = ((ULONG)opcode) & 255;
 	ULONG s = m68k_getpc();
 	
	if (src == 0) {
		s += (LONG)(WORD)nextiword();
	} else if (src == 255) {
		s += (LONG)nextilong();
	} else {
		s += (LONG)(BYTE)src;
	}
	if (cond == 1) {
		/* Bsr 01100001nnnnnnnn */
	    m68k_areg(regs, 7) -= 4;
	    put_long(m68k_areg(regs, 7), m68k_getpc());
	  	m68k_setpc(s);
	} else {
		/* Bra/Bcc 0110ccccnnnnnnnn */
		if (cctrue(cond)) {
			m68k_setpc(s);
		}
	}
}

LOCALPROCUSEDONCE DoCode7(void)
{
	/* MoveQ 0111ddd0nnnnnnnn */
	ULONG src = (LONG)(BYTE)(opcode & 255);
	ULONG dstreg = rg9;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	m68k_dreg(regs, dstreg) = (src);
}

LOCALPROCUSEDONCE DoCode8(void)
{
	if (b76 == 3) {
		ULONG BinOp;

		if (b8 == 0) {
			BinOp = BinOpDivU; /* DivU 1000ddd011mmmrrr */
		} else {
			BinOp = BinOpDivS; /* DivS 1000ddd111mmmrrr */
		}
		DoBinOpDiv1(mode, reg, 0, rg9, BinOp);
	} else {
		if ((b8 == 1) && (mode < 2)) {
			/* SBCD 1000xxx10000mxxx */
			opsize = 1;
			if (mode == 0) {
				DoBinOp1(0, reg, 0, rg9, BinOpSubBCD);
			} else {
				DoBinOp1(4, reg, 4, rg9, BinOpSubBCD);
			}
		} else {
			/* OR 1000dddmssmmmrrr */
			FindOpSizeFromb76();
			if (b8 == 1) {
				DoBinOp1(0, rg9, mode, reg, BinOpOr);
			} else {
				DoBinOp1(mode, reg, 0, rg9, BinOpOr);
			}
		}
	}
}

LOCALPROCUSEDONCE DoCode9(void)
{
	if (b76 == 3) {
		/* SUBA 1t01dddm11mmmrrr */
		opsize = b8 * 2 + 2;
		DoBinOp1(mode, reg, 1, rg9, BinOpSub);
	} else {
		FindOpSizeFromb76();
		if (b8 == 0) {
			DoBinOp1(mode, reg, 0, rg9, BinOpSub);
		} else if (mode >= 2) {
			DoBinOp1(0, rg9, mode, reg, BinOpSub);
		} else if (mode == 0) {
			DoBinOp1(0, reg, 0, rg9, BinOpSubX);
		} else {
			DoBinOp1(4, reg, 4, rg9, BinOpSubX);
		}
	}
}

LOCALPROCUSEDONCE DoCodeA(void)
{
	BackupPC();
	Exception(0xA);
}

LOCALPROCUSEDONCE DoCodeB(void)
{
	if (b76 == 3) {
		opsize = b8 * 2 + 2;
		DoCompareA(mode, reg, 1, rg9);
	} else if ((b8 == 1) && (mode == 1)) {
		/* CmpM 1011ddd1ss001rrr */
		FindOpSizeFromb76();
		DoCompare(3, reg, 3, rg9);
	} else if (b8 == 1) {
		/* Eor 1011ddd1ssmmmrrr */
		FindOpSizeFromb76();
		DoBinOp1(0, rg9, mode, reg, BinOpEor);
	} else {
		/* Cmp 1011ddd0ssmmmrrr */
		FindOpSizeFromb76();
		DoCompare(mode, reg, 0, rg9);
	}
}

LOCALPROCUSEDONCE DoCodeC(void)
{
	if (b76 == 3) {
		ULONG BinOp;
		if (b8 == 0) {
			BinOp = BinOpMulU;  /* MulU 1100ddd011mmmrrr */
		} else {
			BinOp = BinOpMulS; /* MulS 1100ddd111mmmrrr */
		}
		DoBinOpMul1(mode, reg,0,rg9, BinOp);
	} else if ((mode < 2) && (b8 == 1)) {
		switch (b76) {
    		case 0 :
				/* ABCD 1100ddd10000mrrr */
				/* Debugger(); */ /* does anyone use this? */
				opsize = 1;
				if (mode == 0) {
					DoBinOp1(0,reg,0,rg9,BinOpAddBCD);
				} else {
					DoBinOp1(4,reg,4,rg9,BinOpAddBCD);
				}
				break;
    		case 1 :
				/* Exg 1100ddd10100trrr, opsize = 4 */
				if (mode == 0) {
					ULONG srcreg = rg9;
					ULONG dstreg = reg;
					LONG src = m68k_dreg(regs, srcreg);
					LONG dst = m68k_dreg(regs, dstreg);
					m68k_dreg(regs, srcreg) = dst;
					m68k_dreg(regs, dstreg) = src;
				} else {
					ULONG srcreg = rg9;
					ULONG dstreg = reg;
					LONG src = m68k_areg(regs, srcreg);
					LONG dst = m68k_areg(regs, dstreg);
					m68k_areg(regs, srcreg) = dst;
					m68k_areg(regs, dstreg) = src;
				}
				break;
    		case 2 :
    			{
					/* Exg 1100ddd110001rrr, opsize = 4 */
					ULONG srcreg = rg9;
					ULONG dstreg = reg;
					LONG src = m68k_dreg(regs, srcreg);
					LONG dst = m68k_areg(regs, dstreg);
					m68k_dreg(regs, srcreg) = dst;
					m68k_areg(regs, dstreg) = src;
				}
				break;
		}
	} else {
		/* And 1100dddmssmmmrrr */
		FindOpSizeFromb76();
		if (b8 == 1) {
			DoBinOp1(0,rg9,mode,reg,BinOpAnd);
		} else {
			DoBinOp1(mode,reg,0,rg9,BinOpAnd);
		}
	}
}

LOCALPROCUSEDONCE DoCodeD(void)
{
	if (b76 == 3) {
		/* ADDA 1t01dddm11mmmrrr */
		opsize = b8 * 2 + 2;
		DoBinOp1(mode, reg, 1, rg9, BinOpAdd);
	} else {
		FindOpSizeFromb76();
		if (b8 == 0) {
			DoBinOp1(mode, reg, 0, rg9, BinOpAdd);
		} else if (mode >= 2) {
			DoBinOp1(0, rg9, mode, reg, BinOpAdd);
		} else if (mode == 0) {
			DoBinOp1(0, reg, 0, rg9, BinOpAddX);
		} else {
			DoBinOp1(4, reg, 4, rg9, BinOpAddX);
		}
	}
}

static ULONG rolops(ULONG x)
{
	ULONG binop;

	switch (x) {
		case 0 :
			binop = BinOpASL;
			break;
		case 1 :
			binop = BinOpLSL;
			break;
		case 2 :
			binop = BinOpRXL;
			break;
		case 3 :
			binop = BinOpROL;
			break;
	}
	if (! b8) {
		binop++; /* 'R' */
	} /* else 'L' */
	return binop;
}

LOCALPROCUSEDONCE DoCodeE(void)
{
	if (b76 == 3) {
		opsize = 2;
		DoBinOp1(8, 1, mode,reg, rolops(rg9));
	} else {
		FindOpSizeFromb76();
		if (mode < 4) {
			/* 1110cccdss0ttddd */
			DoBinOp1(8, octdat(rg9), 0, reg, rolops(mode & 3));
		} else {
			/* 1110rrrdss1ttddd */
			DoBinOp1(0, rg9, 0, reg, rolops(mode & 3));
		}
	}
}

LOCALPROCUSEDONCE DoCodeF(void)
{
	op_illg(opcode);
}

void op_illg(ULONG opcode)
{
	UnusedParam(opcode);
	BackupPC();
 	Exception(4);
}

static ULONG MaxInstructionsToGo;

static void m68k_go_MaxInstructions(void)
{
	/* MaxInstructionsToGo must be >= 1 on entry */
	do {
		opcode = nextiword();

		switch (opcode >> 12) {
	   		case 0x0 :
	   			DoCode0();
				break;
	   		case 0x1 :
	 			DoCode1();
				break;
	   		case 0x2 :
				DoCode2();
				break;
	   		case 0x3 :
				DoCode3();
				break;
	   		case 0x4 :
	   			DoCode4();
				break;
	   		case 0x5 :
	   			DoCode5();
				break;
	   		case 0x6 :
	   			DoCode6();
				break;
			case 0x7:
				DoCode7();
				break;
			case 0x8:
				DoCode8();
				break;
			case 0x9:
				DoCode9();
				break;
	   		case 0xA :
	   			DoCodeA();
				break;
	   		case 0xB :
	   			DoCodeB();
				break;
	   		case 0xC :
	   			DoCodeC();
				break;
			case 0xD:
				DoCodeD();
				break;
	   		case 0xE :
	   			DoCodeE();
				break;
	   		case 0xF :
	   			DoCodeF();
				break;
		}

    } while (--MaxInstructionsToGo != 0);
}

static ULONG MoreInstructionsToGo;

void NeedToGetOut(void)
{
	if (MaxInstructionsToGo == 0) {
		/*
			already have gotten out, and exception processing has
			caused another exception, such as because a bad
			stack pointer pointing to a memory mapped device.
		*/
	} else {
		MoreInstructionsToGo += (MaxInstructionsToGo - 1);
			/* not counting the current instruction */
		MaxInstructionsToGo = 1;
	}
}

static void do_trace(void)
{
    regs.spcflags |= SPCFLAG_DOTRACE;
    NeedToGetOut();
}

void SetBusError(CPTR addr)
{
    regs.spcflags |= SPCFLAG_BUSERR;
	last_fault_for_exception_3 = addr;
	last_addr_for_exception_3 = m68k_getpc();
	NeedToGetOut();
}

void SetAutoVector(void)
{
    regs.spcflags |= SPCFLAG_AUTOVECTOR;
	NeedToGetOut();
}

void m68k_setstopped(void)
{
    regs.stopped = 1;
	regs.spcflags |= SPCFLAG_STOP;
	NeedToGetOut();
	MoreInstructionsToGo = 0;
}

void m68k_go_nInstructions(ULONG n)
{
	MaxInstructionsToGo = n;
	do {
		MoreInstructionsToGo = 0;
		if (regs.t1) {
			do_trace();
		}
		m68k_go_MaxInstructions();
		if (regs.spcflags) {
			if (regs.spcflags & SPCFLAG_BUSERR) {
			    last_op_for_exception_3 = opcode;
			    Exception(3);
			    regs.spcflags &= ~SPCFLAG_BUSERR;
			}
			if (regs.spcflags & SPCFLAG_AUTOVECTOR) {
				Exception(regs.intmask+24);
			    regs.spcflags &= ~SPCFLAG_AUTOVECTOR;
			}

		    if (regs.spcflags & SPCFLAG_DOTRACE) {
				Exception(9);
		    }
		    while (regs.spcflags & SPCFLAG_STOP) {
		    }
		}
		MaxInstructionsToGo = MoreInstructionsToGo;
	} while (MaxInstructionsToGo != 0);
}
