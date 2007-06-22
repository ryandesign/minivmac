/*
	MINEM68K.c

	Copyright (C) 2004 Bernd Schmidt, Paul C. Pratt

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
	MINimum EMulator of 68K cpu

	This code descends from a simple 68000 emulator that I (Paul C. Pratt)
	wrote long ago. That emulator ran on a 680x0, and used the cpu
	it ran on to do some of the work. This descendent fills
	in those holes with code from the Un*x Amiga Emulator
	by Bernd Schmidt, as found being used in vMac.

	This emulator is about 10 times smaller than the UAE,
	at the cost of being 2 to 3 times slower. It also only
	emulates the 68000, not including the emulation of later
	processors or the FPU.
*/

#ifndef AllFiles
#include "SYSDEPNS.h"
#endif

#include "ENDIANAC.h"
#include "ADDRSPAC.h"

#include "MINEM68K.h"

IMPORTFUNC ui5b MM_Access(ui5b Data, blnr WriteMem, blnr ByteSize, CPTR addr);
IMPORTFUNC ui3b *get_real_address(ui5b L, blnr WritableMem, CPTR addr);
IMPORTPROC customreset(void);

typedef unsigned char flagtype;

#ifndef Use68020
#define Use68020 0
#endif

#ifndef AccurateIllegal
#define AccurateIllegal 0
#endif

LOCALVAR struct regstruct
{
	ui5b regs[16]; /* Data and Address registers */
	ui5b pc; /* Program Counter */
	CPTR usp; /* User Stack Pointer */
	CPTR isp; /* Interrupt Stack Pointer */
#if Use68020
	CPTR msp; /* Master Stack Pointer */
#endif

	/* Status Register */
	int intmask; /* bits 10-8 : interrupt priority mask */
	flagtype t1; /* bit 15: Trace mode 1 */
#if Use68020
	flagtype t0; /* bit 14: Trace mode 0 */
#endif
	flagtype s; /* bit 13: Supervisor or user privilege level  */
#if Use68020
	flagtype m; /* bit 12: Master or interrupt mode */
#endif
	flagtype x; /* bit 4: eXtend */
	flagtype n; /* bit 3: Negative */
	flagtype z; /* bit 2: Zero */
	flagtype v; /* bit 1: oVerflow */
	flagtype c; /* bit 0: Carry */

#if Use68020
	ui5b sfc; /* Source Function Code register */
	ui5b dfc; /* Destination Function Code register */
	ui5b vbr; /* Vector Base Register */
	ui5b cacr; /* Cache Control Register */
		/*
			bit 0 : Enable Cache
			bit 1 : Freeze Cache
			bit 2 : Clear Entry In Cache (write only)
			bit 3 : Clear Cache (write only)
		*/
	ui5b caar; /* Cache Address Register */
#endif

	flagtype TracePending;
	flagtype ExternalInterruptPending;
	ui3b **fBankReadAddr;
	ui3b **fBankWritAddr;
	ui3b *fIPL;
#if 0
	flagtype ResetPending;
#endif
} regs;

#define m68k_dreg(num) (regs.regs[(num)])
#define m68k_areg(num) (regs.regs[(num) + 8])

LOCALFUNC si5b get_word(CPTR addr)
{
	ui3p ba = regs.fBankReadAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		return (si5b)(si4b)do_get_mem_word(m);
	} else {
		return (si5b)(si4b)(ui4b) MM_Access(0, falseblnr, falseblnr, addr);
	}
}

LOCALFUNC si5b get_byte(CPTR addr)
{
	ui3p ba = regs.fBankReadAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		return (si5b)(si3b)*m;
	} else {
		return (si5b)(si3b)(ui3b) MM_Access(0, falseblnr, trueblnr, addr);
	}
}

LOCALFUNC ui5b get_long(CPTR addr)
{
	ui3p ba = regs.fBankReadAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		return do_get_mem_long(m);
	} else {
		si5b hi = get_word(addr);
		si5b lo = get_word(addr + 2);
		return (ui5b) ((((ui5b)hi) << 16) & 0xFFFF0000)
			| (((ui5b)lo) & 0x0000FFFF);
	}
}

LOCALPROC put_word(CPTR addr, ui5b w)
{
	ui3p ba = regs.fBankWritAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		do_put_mem_word(m, w);
	} else {
		(void) MM_Access(w & 0x0000FFFF, trueblnr, falseblnr, addr);
	}
}

LOCALPROC put_byte(CPTR addr, ui5b b)
{
	ui3p ba = regs.fBankWritAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		*m = b;
	} else {
		(void) MM_Access(b & 0x00FF, trueblnr, trueblnr, addr);
	}
}

LOCALPROC put_long(CPTR addr, ui5b l)
{
	ui3p ba = regs.fBankWritAddr[bankindex(addr)];

	if (ba != nullpr) {
		ui3p m = (addr & MemBankAddrMask) + ba;
		do_put_mem_long(m, l);
	} else {
		put_word(addr, l >> 16);
		put_word(addr + 2, l);
	}
}

LOCALFUNC ui3p get_pc_real_address(CPTR addr)
{
	ui3p ba = regs.fBankReadAddr[bankindex(addr)];

	if (ba != nullpr) {
		return (addr & MemBankAddrMask) + ba;
	} else {
		ba = get_real_address(2, falseblnr, addr);
		if (ba != nullpr) {
			return ba;
		} else {
			/* in trouble if get here */
			return regs.fBankReadAddr[0];
		}
	}
}

#define ZFLG regs.z
#define NFLG regs.n
#define CFLG regs.c
#define VFLG regs.v
#define XFLG regs.x

#define LOCALPROCUSEDONCE LOCALFUNC MayInline void

LOCALFUNC MayInline int cctrue(const int cc)
{
	switch (cc) {
		case 0:  return 1;                          /* T */
		case 1:  return 0;                          /* F */
		case 2:  return (! CFLG) && (! ZFLG);       /* HI */
		case 3:  return CFLG || ZFLG;               /* LS */
		case 4:  return ! CFLG;                     /* CC */
		case 5:  return CFLG;                       /* CS */
		case 6:  return ! ZFLG;                     /* NE */
		case 7:  return ZFLG;                       /* EQ */
		case 8:  return ! VFLG;                     /* VC */
		case 9:  return VFLG;                       /* VS */
		case 10: return ! NFLG;                     /* PL */
		case 11: return NFLG;                       /* MI */
		case 12: return NFLG == VFLG;               /* GE */
		case 13: return NFLG != VFLG;               /* LT */
		case 14: return (! ZFLG) && (NFLG == VFLG); /* GT */
		case 15: return ZFLG || (NFLG != VFLG);     /* LE */
		default: return 0; /* shouldn't get here */
	}
}

LOCALFUNC ui4b m68k_getSR(void)
{
	return (regs.t1 << 15)
#if Use68020
			| (regs.t0 << 14)
#endif
			| (regs.s << 13)
#if Use68020
			| (regs.m << 12)
#endif
			| (regs.intmask << 8)
			| (XFLG << 4) | (NFLG << 3) | (ZFLG << 2) | (VFLG << 1)
			|  CFLG;
}

FORWARDPROC NeedToGetOut(void);

LOCALFUNC MayInline void m68k_setCR(ui4b newcr)
{
	XFLG = (newcr >> 4) & 1;
	NFLG = (newcr >> 3) & 1;
	ZFLG = (newcr >> 2) & 1;
	VFLG = (newcr >> 1) & 1;
	CFLG = newcr & 1;
}

FORWARDPROC SetExternalInterruptPending(void);

LOCALPROC m68k_setSR(ui4b newsr)
{
	CPTR *pnewstk;
	CPTR *poldstk = regs.s ? (
#if Use68020
		regs.m ? &regs.msp :
#endif
		&regs.isp) : &regs.usp;
	int oldintmask = regs.intmask;

	m68k_setCR(newsr);
	regs.t1 = (newsr >> 15) & 1;
#if Use68020
	regs.t0 = (newsr >> 14) & 1;
	if (regs.t0) {
		ReportAbnormal("t0 flag set in m68k_setSR");
	}
#endif
	regs.s = (newsr >> 13) & 1;
#if Use68020
	regs.m = (newsr >> 12) & 1;
	if (regs.m) {
		ReportAbnormal("m flag set in m68k_setSR");
	}
#endif
	regs.intmask = (newsr >> 8) & 7;

	pnewstk = regs.s ? (
#if Use68020
		regs.m ? &regs.msp :
#endif
		&regs.isp) : &regs.usp;

	if (poldstk != pnewstk) {
		*poldstk = m68k_areg(7);
		m68k_areg(7) = *pnewstk;
	}

	if (regs.intmask != oldintmask) {
		SetExternalInterruptPending();
	}

	if (regs.t1) {
		NeedToGetOut();
	} else {
		/* regs.TracePending = falseblnr; */
	}
}

/*
	This variable was introduced because a program could do a Bcc from
	whithin chip memory to a location whitin expansion memory. With a
	pointer variable the program counter would point to the wrong location.
	With this variable unset the program counter is always correct, but
	programs will run slower (about 4%).
	Usually, you'll want to have this defined.

	vMac REQUIRES this. It allows for fun things like Restart.
*/

#ifndef USE_POINTER
#define USE_POINTER 1
#endif

#if USE_POINTER
LOCALVAR ui3p pc_p;
LOCALVAR ui3p pc_oldp;
#endif

#if USE_POINTER

LOCALFUNC MayInline ui5b nextibyte(void)
{
/* ui5b r = do_get_mem_byte(pc_p + 1); */
	ui5b r;

/* debugout(trueblnr, "nextibyte = %02x\n", (ui3b) pc_p); */
	r = do_get_mem_byte(pc_p + 1);
	pc_p += 2;
	return r;
}

LOCALFUNC MayInline ui5b nextiword(void)
{
/* ui5b r = do_get_mem_word(pc_p); */
	ui5b r;

	r = do_get_mem_word(pc_p);
	pc_p += 2;
	return r;
}

LOCALFUNC MayInline ui5b nextilong(void)
{
/* ui5b r = do_get_mem_long(pc_p); */
	ui5b r;

	r = do_get_mem_long(pc_p);
	pc_p += 4;
	return r;
}

#else

LOCALFUNC MayInline ui5b nextibyte(void)
{
	ui5b r = (ui5b)get_byte(regs.pc + 1);
	regs.pc += 2;
	return r & 0x00FF;
}

LOCALFUNC MayInline ui5b nextiword(void)
{
	ui5b r = (ui5b)get_word(regs.pc);
	regs.pc += 2;
	return r & 0x0000FFFF;
}

LOCALFUNC MayInline ui5b nextilong(void)
{
	ui5b r = get_long(regs.pc);
	regs.pc += 4;
	return r;
}

#endif

LOCALFUNC MayInline void BackupPC(void)
{
#if USE_POINTER
	pc_p -= 2;
#else
	regs.pc -= 2;
#endif
}

#define MakeDumpFile 0

#if MakeDumpFile
IMPORTPROC DumpAJump(CPTR fromaddr, CPTR toaddr);

LOCALPROC DumpAJump2(CPTR toaddr)
{
	CPTR fromaddr = regs.pc + (pc_p - pc_oldp);

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

#if USE_POINTER

LOCALFUNC MayInline void m68k_setpc(CPTR newpc)
{
#if MakeDumpFile
	DumpAJump2(newpc);
#endif
#if 0
	if (newpc == 0xBD50 /* 401AB4 */) {
		/* Debugger(); */
		Exception(5); /* try and get macsbug */
	}
#endif
	pc_p = pc_oldp = get_pc_real_address(newpc);
	regs.pc = newpc;
}

LOCALFUNC MayInline CPTR m68k_getpc(void)
{
	return regs.pc + (pc_p - pc_oldp);
}

#else

LOCALFUNC MayInline void m68k_setpc(CPTR newpc)
{
/* regs.pc = newpc; */
/* bill mod */
	regs.pc = newpc & 0x00FFFFFF;
}

LOCALFUNC MayInline CPTR m68k_getpc(void)
{
	return regs.pc;
}
#endif

#ifndef FastRelativeJump
#define FastRelativeJump (1 && USE_POINTER)
#endif

LOCALPROC ExceptionTo(CPTR newpc
#if Use68020
	, int nr
#endif
	)
{
	ui4b saveSR = m68k_getSR();

	if (! regs.s) {
		regs.usp = m68k_areg(7);
		m68k_areg(7) =
#if Use68020
			regs.m ? regs.msp :
#endif
			regs.isp;
		regs.s = 1;
	}
#if Use68020
	switch (nr) {
		case 5: /* Zero Divide */
		case 6: /* CHK, CHK2 */
		case 7: /* cpTRAPcc, TRAPCcc, TRAPv */
		case 9: /* Trace */
			m68k_areg(7) -= 4;
			put_long(m68k_areg(7), m68k_getpc());
			m68k_areg(7) -= 2;
			put_word(m68k_areg(7), 0x2000 + nr * 4);
			break;
		default:
			m68k_areg(7) -= 2;
			put_word(m68k_areg(7), nr * 4);
			break;
	}
	/* if regs.m should make throw away stack frame */
#endif
	m68k_areg(7) -= 4;
	put_long(m68k_areg(7), m68k_getpc());
	m68k_areg(7) -= 2;
	put_word(m68k_areg(7), saveSR);
	m68k_setpc(newpc);
	regs.t1 = 0;
#if Use68020
	regs.t0 = 0;
	regs.m = 0;
#endif
	regs.TracePending = falseblnr;
}

LOCALPROC Exception(int nr)
{
	ExceptionTo(get_long(4 * nr
#if Use68020
		+ regs.vbr
#endif
		)
#if Use68020
		, nr
#endif
		);
}

GLOBALPROC DiskInsertedPsuedoException(CPTR newpc, ui5b data)
{
	ExceptionTo(newpc
#if Use68020
		, 0
#endif
		);
	m68k_areg(7) -= 4;
	put_long(m68k_areg(7), data);
}

LOCALPROC DoCheckExternalInterruptPending(void)
{
	int level = *regs.fIPL;
	if ((level > regs.intmask) || (level == 7)) {
		Exception(24 + level);
		regs.intmask = level;
	}
}

GLOBALPROC m68k_IPLchangeNtfy(void)
{
	int level = *regs.fIPL;
	if ((level > regs.intmask) || (level == 7)) {
		SetExternalInterruptPending();
	}
}

#if 0
LOCALPROC m68k_DoReset(void)
{
	regs.ResetPending = falseblnr;

/* Sets the MC68000 reset jump vector... */
	m68k_setpc(get_long(0x00000004));

/* Sets the initial stack vector... */
	m68k_areg(7) = get_long(0x00000000);

	regs.s = 1;
#if Use68020
	regs.m = 0;
	regs.t0 = 0;
#endif
	regs.t1 = 0;
	ZFLG = CFLG = NFLG = VFLG = 0;
	regs.ExternalInterruptPending = falseblnr;
	regs.TracePending = falseblnr;
	regs.intmask = 7;

#if Use68020
	regs.sfc = 0;
	regs.dfc = 0;
	regs.vbr = 0;
	regs.cacr = 0;
	regs.caar = 0;
#endif
}
#endif

GLOBALPROC m68k_reset(void)
{
#if 0
	regs.ResetPending = trueblnr;
	NeedToGetOut();
#else
/* Sets the MC68000 reset jump vector... */
	m68k_setpc(get_long(0x00000004));

/* Sets the initial stack vector... */
	m68k_areg(7) = get_long(0x00000000);

	regs.s = 1;
#if Use68020
	regs.m = 0;
	regs.t0 = 0;
#endif
	regs.t1 = 0;
	ZFLG = CFLG = NFLG = VFLG = 0;
	regs.ExternalInterruptPending = falseblnr;
	regs.TracePending = falseblnr;
	regs.intmask = 7;

#if Use68020
	regs.sfc = 0;
	regs.dfc = 0;
	regs.vbr = 0;
	regs.cacr = 0;
	regs.caar = 0;
#endif
#endif
}

GLOBALPROC MINEM68K_Init(ui3b **BankReadAddr, ui3b **BankWritAddr,
	ui3b *fIPL)
{
	regs.fBankWritAddr = BankWritAddr;
	regs.fBankReadAddr = BankReadAddr;
	regs.fIPL = fIPL;
}

LOCALFUNC MayInline ui5b get_disp_ea(ui5b base)
{
	ui4b dp = nextiword();
	int reg = (dp >> 12) & 0x0F;
	si5b regd = regs.regs[reg];
	if ((dp & 0x0800) == 0) {
		regd = (si5b)(si4b)regd;
	}
#if Use68020
	regd <<= (dp >> 9) & 3;
#if ExtraAbormalReports
	if (((dp >> 9) & 3) != 0) {
		/* ReportAbnormal("Have scale in Extension Word"); */
		/* apparently can happen in Sys 7.5.5 boot on 68000 */
	}
#endif
	if (dp & 0x0100) {
		if ((dp & 0x80) != 0) {
			base = 0;
			/* ReportAbnormal("Extension Word: suppress base"); */
			/* used by Sys 7.5.5 boot */
		}
		if ((dp & 0x40) != 0) {
			regd = 0;
			ReportAbnormal("Extension Word: suppress regd");
		}

		switch ((dp >> 4) & 0x03) {
			case 0:
				/* reserved */
				ReportAbnormal("Extension Word: dp reserved");
				break;
			case 1:
				/* no displacement */
				/* ReportAbnormal("Extension Word: no displacement"); */
				/* used by Sys 7.5.5 boot */
				break;
			case 2:
				base += (si5b)(si4b)nextiword();
				/* ReportAbnormal("Extension Word: word displacement"); */
				/* used by Sys 7.5.5 boot */
				break;
			case 3:
				base += nextilong();
				ReportAbnormal("Extension Word: long displacement");
				break;
		}

		if ((dp & 0x03) == 0) {
			base += regd;
			if ((dp & 0x04) != 0) {
				ReportAbnormal("Extension Word: reserved dp form");
			}
			/* ReportAbnormal("Extension Word: noindex"); */
			/* used by Sys 7.5.5 boot */
		} else {
			if ((dp & 0x04) != 0) {
				base = get_long(base);
				base += regd;
				/* ReportAbnormal("Extension Word: postindex"); */
				/* used by Sys 7.5.5 boot */
			} else {
				base += regd;
				base = get_long(base);
				/* ReportAbnormal("Extension Word: preindex"); */
				/* used by Sys 7.5.5 boot */
			}
			switch (dp & 0x03) {
				case 1:
					/* null outer displacement */
					/* ReportAbnormal("Extension Word: null outer displacement"); */
					/* used by Sys 7.5.5 boot */
					break;
				case 2:
					base += (si5b)(si4b)nextiword();
					ReportAbnormal("Extension Word: word outer displacement");
					break;
				case 3:
					base += nextilong();
					ReportAbnormal("Extension Word: long outer displacement");
					break;
			}
		}

		return base;
	} else
#endif
	{
		return base + (si3b)(dp) + regd;
	}
}

LOCALVAR ui5b opsize;

LOCALPROC op_illg(void);

#define AKMemory 0
#define AKRegister 1
#define AKConstant 2

LOCALVAR ui5b ArgKind;
LOCALVAR ui5b ArgAddr;

LOCALPROC DecodeModeRegister(ui5b themode, ui5b thereg)
{
	switch (themode) {
		case 0 :
			ArgKind = AKRegister;
			ArgAddr = thereg;
			break;
		case 1 :
			ArgKind = AKRegister;
			ArgAddr = thereg + 8;
			break;
		case 2 :
			ArgKind = AKMemory;
			ArgAddr = m68k_areg(thereg);
			break;
		case 3 :
			ArgKind = AKMemory;
			ArgAddr = m68k_areg(thereg);
			if ((thereg == 7) && (opsize == 1)) {
				m68k_areg(thereg) += 2;
			} else {
				m68k_areg(thereg) += opsize;
			}
			break;
		case 4 :
			ArgKind = AKMemory;
			if ((thereg == 7) && (opsize == 1)) {
				m68k_areg(thereg) -= 2;
			} else {
				m68k_areg(thereg) -= opsize;
			}
			ArgAddr = m68k_areg(thereg);
			break;
		case 5 :
			ArgKind = AKMemory;
			ArgAddr = m68k_areg(thereg) + (si5b)(si4b)nextiword();
			break;
		case 6 :
			ArgKind = AKMemory;
			ArgAddr = get_disp_ea(m68k_areg(thereg));
			break;
		case 7 :
			switch (thereg) {
				case 0 :
					ArgKind = AKMemory;
					ArgAddr = (si5b)(si4b)nextiword();
					break;
				case 1 :
					ArgKind = AKMemory;
					ArgAddr = nextilong();
					break;
				case 2 :
					ArgKind = AKMemory;
					ArgAddr = m68k_getpc();
					ArgAddr += (si5b)(si4b)nextiword();
					break;
				case 3 :
					ArgKind = AKMemory;
					ArgAddr = get_disp_ea(m68k_getpc());
					break;
				case 4 :
					ArgKind = AKConstant;
					switch (opsize) {
						case 1:
							ArgAddr = (si5b)(si3b)nextibyte();
							break;
						case 2:
							ArgAddr = (si5b)(si4b)nextiword();
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
	}
}

LOCALFUNC si5b GetArgValue(void)
{
	si5b v;

	switch (ArgKind) {
		case AKMemory:
			switch (opsize) {
				case 1:
					v = get_byte(ArgAddr);
					break;
				case 2:
					v = get_word(ArgAddr);
					break;
				case 4:
				default: /* for compiler. should be 1, 2, or 4 */
					v = get_long(ArgAddr);
					break;
			}
			break;
		case AKRegister:
			v = regs.regs[ArgAddr];
			switch (opsize) {
				case 1:
					v = (si5b)(si3b)v;
					break;
				case 2:
					v = (si5b)(si4b)v;
					break;
				case 4:
					break;
			}
			break;
		case AKConstant:
		default: /* for compiler. shouldn't be any other cases */
			v = ArgAddr;
			break;
	}
	return v;
}

LOCALPROC SetArgValue(si5b v)
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
		case AKRegister:
			switch (opsize) {
				case 1:
					regs.regs[ArgAddr] = (regs.regs[ArgAddr] & ~ 0xff) | ((v) & 0xff);
					break;
				case 2:
					regs.regs[ArgAddr] = (regs.regs[ArgAddr] & ~ 0xffff) | ((v) & 0xffff);
					break;
				case 4:
					regs.regs[ArgAddr] = v;
					break;
			}
			break;
		default:
			op_illg();
			break;
	}
}

LOCALPROC DoMove(ui5b m1, ui5b r1, ui5b m2, ui5b r2) /* MOVE */
{
	si5b src;

	DecodeModeRegister(m1, r1);
	src = GetArgValue();
	if (m2 == 1) { /* MOVEA */
		m68k_areg(r2) = src;
	} else {
		DecodeModeRegister(m2, r2);
		VFLG = CFLG = 0;
		ZFLG = (src == 0);
		NFLG = (src < 0);
		SetArgValue(src);
	}
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

#define extendopsizedstvalue() \
	if (opsize == 1) {\
		dstvalue = (si3b)dstvalue;\
	} else if (opsize == 2) {\
		dstvalue = (si4b)dstvalue;\
	}

#define unextendopsizedstvalue() \
	if (opsize == 1) {\
		dstvalue = (ui3b)dstvalue;\
	} else if (opsize == 2) {\
		dstvalue = (ui4b)dstvalue;\
	}

LOCALPROC DoBinOp1(ui5b m1, ui5b r1, ui5b m2, ui5b r2, ui5b binop)
{
	if (m2 == 1) {
		op_illg();
	} else {
		si5b srcvalue;
		si5b dstvalue;

		DecodeModeRegister(m1, r1);
		srcvalue = GetArgValue();
		DecodeModeRegister(m2, r2);
		dstvalue = GetArgValue();
		switch (binop) {
			case BinOpAdd:
				{
					int flgs = (srcvalue < 0);
					int flgo = (dstvalue < 0);
					dstvalue = (dstvalue + srcvalue);
					extendopsizedstvalue();
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = (flgs && flgo && ! NFLG) || ((! flgs) && (! flgo) && NFLG);
					XFLG = CFLG = (flgs && flgo) || ((! NFLG) && (flgo || flgs));
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
					XFLG = CFLG = (flgs && ! flgo) || (NFLG && ((! flgo) || flgs));
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
					VFLG = ((! flgs) && flgo && (! NFLG)) || (flgs && (! flgo) && NFLG);
					XFLG = CFLG = (flgs && (! flgo)) || (NFLG && ((! flgo) || flgs));
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
					XFLG = CFLG = (flgs && flgo) || ((! NFLG) && (flgo || flgs));
					VFLG = (flgs && flgo && ! NFLG) || ((! flgs) && (! flgo) && NFLG);
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
					ui5b cnt = ((ui5b)srcvalue) & 63;
					si5b dstvalue0 = dstvalue;
					si5b comparevalue;
					if (! cnt) {
						VFLG = 0;
						CFLG = 0;
					} else {
						if (cnt > 32) {
							dstvalue = 0;
						} else {
							dstvalue = (si5b)(((ui5b)dstvalue) << (cnt - 1));
						}
						extendopsizedstvalue();
						CFLG = XFLG = (dstvalue < 0);
						dstvalue = (si5b)(((ui5b)dstvalue) << 1);
						extendopsizedstvalue();
					}
					if (dstvalue < 0) {
						comparevalue = - (si5b)(((ui5b) - dstvalue) >> (cnt));
					} else {
						comparevalue = (si5b)(((ui5b)dstvalue) >> (cnt));
					}
					VFLG = (comparevalue != dstvalue0);
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
				}
				break;
			case BinOpASR:
				{
					ui5b cnt = ((ui5b)srcvalue) & 63;
					NFLG = (dstvalue < 0);
					VFLG = 0;
					if (! cnt) {
						CFLG = 0;
					} else {
						if (NFLG) {
							dstvalue = (~ dstvalue);
						}
						unextendopsizedstvalue();
						if (cnt > 32) {
							dstvalue = 0;
						} else {
							dstvalue = (si5b)(((ui5b)dstvalue) >> (cnt - 1));
						}
						CFLG = ((ui5b)dstvalue & 1);
						dstvalue = (si5b)(((ui5b)dstvalue) >> 1);
						if (NFLG) {
							CFLG = ! CFLG;
							dstvalue = (~ dstvalue);
						}
						XFLG = CFLG;
					}
					ZFLG = (dstvalue == 0);
				}
				break;
			case BinOpLSL:
				{
					ui5b cnt = ((ui5b)srcvalue) & 63;
					if (! cnt) {
						CFLG = 0;
					} else {
						if (cnt > 32) {
							dstvalue = 0;
						} else {
							dstvalue = (si5b)(((ui5b)dstvalue) << (cnt - 1));
						}
						extendopsizedstvalue();
						CFLG = XFLG = (dstvalue < 0);
						dstvalue = (si5b)(((ui5b)dstvalue) << 1);
						extendopsizedstvalue();
					}
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = 0;
				}
				break;
			case BinOpLSR:
				{
					ui5b cnt = ((ui5b)srcvalue) & 63;
					if (! cnt) {
						CFLG = 0;
					} else {
						unextendopsizedstvalue();
						if (cnt > 32) {
							dstvalue = 0;
						} else {
							dstvalue = (si5b)(((ui5b)dstvalue) >> (cnt - 1));
						}
						CFLG = XFLG = ((ui5b)dstvalue & 1);
						dstvalue = (si5b)(((ui5b)dstvalue) >> 1);
					}
					ZFLG = (dstvalue == 0);
					NFLG = (dstvalue < 0);
					VFLG = 0;
				}
				break;
			case BinOpROL:
				{
					ui5b cnt = ((ui5b)srcvalue) & 63;
					if (! cnt) {
						CFLG = 0;
					} else {
						for (; cnt; --cnt) {
							CFLG = (dstvalue < 0);
							dstvalue = (si5b)(((ui5b)dstvalue) << 1);
							if (CFLG) {
								dstvalue = (si5b)(((ui5b)dstvalue) | 1);
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
					ui5b cnt = ((ui5b)srcvalue) & 63;
					if (! cnt) {
						CFLG = XFLG;
					} else {
						for (; cnt; --cnt) {
							CFLG = (dstvalue < 0);
							dstvalue = (si5b)(((ui5b)dstvalue) << 1);
							if (XFLG) {
								dstvalue = (si5b)(((ui5b)dstvalue) | 1);
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
					ui5b cnt = ((ui5b)srcvalue) & 63;
					ui5b cmask = (ui5b)1 << (opsize * 8 - 1);
					if (! cnt) {
						CFLG = 0;
					} else {
						unextendopsizedstvalue();
						for (; cnt; --cnt) {
							CFLG = ((((ui5b)dstvalue) & 1) != 0);
							dstvalue = (si5b)(((ui5b)dstvalue) >> 1);
							if (CFLG) {
								dstvalue = (si5b)(((ui5b)dstvalue) | cmask);
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
					ui5b cnt = ((ui5b)srcvalue) & 63;
					ui5b cmask = (ui5b)1 << (opsize * 8 - 1);
					if (! cnt) {
						CFLG = XFLG;
					} else {
						unextendopsizedstvalue();
						for (; cnt; --cnt) {
							CFLG = ((((ui5b)dstvalue) & 1) != 0);
							dstvalue = (si5b)(((ui5b)dstvalue) >> 1);
							if (XFLG) {
								dstvalue = (si5b)(((ui5b)dstvalue) | cmask);
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
					ui4b newv_lo = (srcvalue & 0xF) + (dstvalue & 0xF) + (XFLG ? 1 : 0);
					ui4b newv_hi = (srcvalue & 0xF0) + (dstvalue & 0xF0);
					ui4b newv;

					if (newv_lo > 9) {
						newv_lo += 6;
					}
					newv = newv_hi + newv_lo;
					CFLG = XFLG = (newv & 0x1F0) > 0x90;
					if (CFLG) {
						newv += 0x60;
					}
					dstvalue = (si3b)newv;
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
					ui4b newv_lo = (dstvalue & 0xF) - (srcvalue & 0xF) - (XFLG ? 1 : 0);
					ui4b newv_hi = (dstvalue & 0xF0) - (srcvalue & 0xF0);
					ui4b newv;

					if (newv_lo > 9) {
						newv_lo -= 6;
						newv_hi -= 0x10;
					}
					newv = newv_hi + (newv_lo & 0xF);
					CFLG = XFLG = (newv_hi & 0x1F0) > 0x90;
					if (CFLG) {
						newv -= 0x60;
					}
					dstvalue = (si3b)newv;
					if (dstvalue != 0) {
						ZFLG = 0;
					}
					NFLG = (dstvalue < 0);
					VFLG = (flgs != flgo) && (NFLG != flgo);
					/* but according to my reference book, VFLG is Undefined for SBCD */
				}
				break;
			default:
				op_illg();
				return;
				break;
		}
		SetArgValue(dstvalue);
	}
}

LOCALPROC DoBinOpA(ui5b m1, ui5b r1, ui5b r2, blnr IsSub)
{
	si5b srcvalue;
	si5b dstvalue;

	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	dstvalue = m68k_areg(r2);
	if (IsSub) {
		dstvalue -= srcvalue;
	} else {
		dstvalue += srcvalue;
	}
	m68k_areg(r2) = dstvalue;
}

LOCALPROC DoBinOpStatusCCR(blnr IsStatus, ui5b binop)
{
	si5b srcvalue;
	si5b dstvalue;

	srcvalue = (si5b)(si4b)nextiword();
	dstvalue = m68k_getSR();
	switch (binop) {
		case BinOpAnd:
			dstvalue &= srcvalue;
			break;
		case BinOpOr:
			dstvalue |= srcvalue;
			break;
		case BinOpEor:
			dstvalue ^= srcvalue;
			break;
		default:
			op_illg();
			return;
			break;
	}
	if (IsStatus) {
		m68k_setSR(dstvalue);
	} else {
		m68k_setCR(dstvalue);
	}
}

LOCALPROC DoCompare(ui5b m1, ui5b r1, ui5b m2, ui5b r2)
{
	si5b srcvalue;
	si5b dstvalue;

	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	DecodeModeRegister(m2, r2);
	dstvalue = GetArgValue();
	{
		int flgs = (srcvalue < 0);
		int flgo = (dstvalue < 0);
		dstvalue -= srcvalue;
		extendopsizedstvalue();
		ZFLG = (dstvalue == 0);
		NFLG = (dstvalue < 0);
		VFLG = (flgs != flgo) && (NFLG != flgo);
		CFLG = (flgs && ! flgo) || (NFLG && ((! flgo) || flgs));
	}
}

LOCALPROC DoCompareA(ui5b m1, ui5b r1, ui5b r2)
{
	si5b srcvalue;
	si5b dstvalue;

	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();

	dstvalue = m68k_areg(r2);
	{
		int flgs = (srcvalue < 0);
		int flgo = (dstvalue < 0);
		dstvalue -= srcvalue;
		ZFLG = (dstvalue == 0);
		NFLG = (dstvalue < 0);
		VFLG = (flgs != flgo) && (NFLG != flgo);
		CFLG = (flgs && ! flgo) || (NFLG && ((! flgo) || flgs));
	}
}

LOCALPROC DoBinBitOp1(ui5b m1, ui5b r1, ui5b m2, ui5b r2, ui5b binop)
{
	if (m2 != 1) {
		si5b srcvalue;
		si5b dstvalue;

		opsize = 1;
		DecodeModeRegister(m1, r1);
		srcvalue = GetArgValue();
		if (m2 == 0) {
			opsize = 4;
		}
		DecodeModeRegister(m2, r2);
		dstvalue = GetArgValue();

		if (m2 != 0) {
			srcvalue &= 7;
		} else {
			srcvalue &= 31;
		}
		ZFLG = (((ui5b)dstvalue & ((ui5b)1 << srcvalue)) == 0);
		if (binop != BinOpBTst) {
			switch (binop) {
				case BinOpBChg:
					dstvalue ^= (1 << srcvalue);
					break;
				case BinOpBClr:
					dstvalue &= ~ (1 << srcvalue);
					break;
				case BinOpBSet:
					dstvalue |= (1 << srcvalue);
					break;
				default:
					op_illg();
					return;
					break;
			}
			SetArgValue(dstvalue);
		}
	} else {
		op_illg();
	}
}

#define UniOpNot 0
#define UniOpNeg 1
#define UniOpNegX 2
#define UniOpNbcd 3
#define UniOpTAS 4

LOCALPROC DoUniOp1(ui5b m2, ui5b r2, ui5b uniop)
{
	si5b dstvalue;

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
				dstvalue = ~ dstvalue;
				extendopsizedstvalue();
				ZFLG = (dstvalue == 0);
				NFLG = (dstvalue < 0);
				VFLG = CFLG = 0;
			}
			break;
		case UniOpNbcd:
			{
				ui4b newv_lo = - (dstvalue & 0xF) - (XFLG ? 1 : 0);
				ui4b newv_hi = - (dstvalue & 0xF0);
				ui4b newv;

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
	/* DoUniOp(uniop, opsize, dstvalue, CCRregister); */
	SetArgValue(dstvalue);
}

LOCALPROC DoBinOpMul1(ui5b m1, ui5b r1, ui5b r2, ui5b binop)
{
	si5b srcvalue;
	si5b dstvalue;

	opsize = 2;
	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	DecodeModeRegister(0, r2);
	dstvalue = GetArgValue();
	{
		switch (binop) {
			case BinOpMulU:
				dstvalue = (ui5b)(ui4b)dstvalue * (ui5b)(ui4b)srcvalue;
				VFLG = CFLG = 0;
				ZFLG = (dstvalue == 0);
				NFLG = (dstvalue < 0);
				break;
			case BinOpMulS:
				dstvalue = (si5b)(si4b)dstvalue * (si5b)(si4b)srcvalue;
				VFLG = CFLG = 0;
				ZFLG = (dstvalue == 0);
				NFLG = (dstvalue < 0);
				break;
			default:
				op_illg();
				return;
				break;
		}
	}
	opsize = 4;
	SetArgValue(dstvalue);
}

LOCALPROC DoBinOpDiv1(ui5b m1, ui5b r1, ui5b r2, ui5b binop)
{
	si5b srcvalue;
	si5b dstvalue;

	opsize = 2;
	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	opsize = 4;
	DecodeModeRegister(0, r2);
	dstvalue = GetArgValue();
	if (srcvalue == 0) {
		Exception(5);
	} else {
		switch (binop) {
			case BinOpDivU:
				{
					ui5b newv = (ui5b)dstvalue / (ui5b)(ui4b)srcvalue;
					ui5b rem = (ui5b)dstvalue % (ui5b)(ui4b)srcvalue;
					if (newv > 0xffff) {
						VFLG = NFLG = 1;
						CFLG = 0;
					} else {
						VFLG = CFLG = 0;
						ZFLG = ((si4b)(newv)) == 0;
						NFLG = ((si4b)(newv)) < 0;
						newv = (newv & 0xffff) | ((ui5b)rem << 16);
						dstvalue = newv;
					}
				}
				break;
			case BinOpDivS:
				{
					si5b newv = (si5b)dstvalue / (si5b)(si4b)srcvalue;
					ui4b rem = (si5b)dstvalue % (si5b)(si4b)srcvalue;
					if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
						VFLG = NFLG = 1; CFLG = 0;
					} else {
						if (((si4b)rem < 0) != ((si5b)dstvalue < 0)) rem = - rem;
						VFLG = CFLG = 0;
						ZFLG = ((si4b)(newv)) == 0;
						NFLG = ((si4b)(newv)) < 0;
						newv = (newv & 0xffff) | ((ui5b)rem << 16);
						dstvalue = newv;
					}
				}
				break;
			default:
				op_illg();
				return;
				break;
		}
	}
	SetArgValue(dstvalue);
}

LOCALVAR ui5b opcode;

#define b76 ((opcode >> 6) & 3)
#define b8 ((opcode >> 8) & 1)
#define mode ((opcode >> 3) & 7)
#define reg (opcode & 7)
#define md6 ((opcode >> 6) & 7)
#define rg9 ((opcode >> 9) & 7)

LOCALPROC FindOpSizeFromb76(void)
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

LOCALFUNC ui5b bitop(void)
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
			/* Debugger(); */
			return -1;
			break;
	}
}

LOCALFUNC ui5b octdat(ui5b x)
{
	if (x == 0) {
		return 8;
	} else {
		return x;
	}
}

LOCALFUNC blnr GetEffectiveAddress(si5b *v)
{
	if (ArgKind == AKMemory) {
		*v = ArgAddr;
		return trueblnr;
	} else {
		/* Debugger(); */
		return falseblnr;
	}
}

#if Use68020
LOCALPROC DoCHK2orCMP2(void)
{
	si5b regv;
	si5b lower;
	si5b upper;
	ui4b extra = nextiword();

	switch ((opcode >> 9) & 3) {
		case 0:
			opsize = 1;
			break;
		case 1:
			opsize = 2;
			break;
		case 2:
			opsize = 4;
			break;
		default:
			ReportAbnormal("illegal opsize in CHK2 or CMP2");
			break;
	}
	if ((extra & 0x8000) == 0) {
		DecodeModeRegister(0, (extra >> 12) & 0x07);
		regv = GetArgValue();
	} else {
		regv = m68k_areg((extra >> 12) & 0x07);
	}
	DecodeModeRegister(mode, reg);
	if (ArgKind != AKMemory) {
		ReportAbnormal("illegal CHK2 or CMP2 mode");
	} else {
		/* CPTR oldpc = m68k_getpc(); */
		lower = GetArgValue();
		ArgAddr += opsize;
		upper = GetArgValue();

		ZFLG = (upper == regv) || (lower == regv);
		CFLG = (lower <= upper)
				? (regv < lower || regv > upper)
				: (regv > upper || regv < lower);
		if ((extra & 0x800) && CFLG) {
			Exception(6);
		}
	}
}
#endif

#if Use68020
LOCALPROC DoCAS(void)
{
	si5b srcvalue;
	si5b dstvalue;

	ui4b src = nextiword();
	int ru = (src >> 6) & 7;
	int rc = src & 7;

	switch ((opcode >> 9) & 3) {
		case 1 :
			opsize = 1;
			break;
		case 2 :
			opsize = 2;
			break;
		case 3 :
			opsize = 4;
			break;
	}

	DecodeModeRegister(0, rc);
	srcvalue = GetArgValue();
	DecodeModeRegister(mode, reg);
	dstvalue = GetArgValue();
	{
		int flgs = srcvalue < 0;
		int flgo = dstvalue < 0;
		si5b newv = dstvalue - srcvalue;
		if (opsize == 1) {
			newv = (ui3b)newv;
		} else if (opsize == 2) {
			newv = (ui4b)newv;
		}
		ZFLG = (newv == 0);
		NFLG = (newv < 0);
		VFLG = (flgs != flgo) && (NFLG != flgo);
		CFLG = (flgs && ! flgo) || (NFLG && ((! flgo) || flgs));
		if (ZFLG) {
			SetArgValue(m68k_dreg(ru));
		} else {
			DecodeModeRegister(0, rc);
			SetArgValue(dstvalue);
		}
	}
}
#endif

#if Use68020
LOCALPROC DoCAS2(void)
{
	ui5b extra = nextilong();
	int dc2 = extra & 7;
	int du2 = (extra >> 6) & 7;
	int dc1 = (extra >> 16) & 7;
	int du1 = (extra >> 22) & 7;
	CPTR rn1 = regs.regs[(extra >> 28) & 0x0F];
	CPTR rn2 = regs.regs[(extra >> 12) & 0x0F];
	si5b src = m68k_dreg(dc1);
	si5b dst1;
	si5b dst2;
	switch ((opcode >> 9) & 3) {
		case 1 :
			op_illg();
			return;
			break;
		case 2 :
			opsize = 2;
			break;
		case 3 :
			opsize = 4;
			break;
	}
	if (opsize == 2) {
		dst1 = get_word(rn1);
		dst2 = get_word(rn2);
		src = (si5b)(si4b)src;
	} else {
		dst1 = get_long(rn1);
		dst2 = get_long(rn2);
	}
	{
		int flgs = src < 0;
		int flgo = dst1 < 0;
		si5b newv = dst1 - src;
		if (opsize == 2) {
			newv = (ui4b)newv;
		}
		ZFLG = (newv == 0);
		NFLG = (newv < 0);
		VFLG = (flgs != flgo) && (NFLG != flgo);
		CFLG = (flgs && ! flgo) || (NFLG && ((! flgo) || flgs));
		if (ZFLG) {
			src = m68k_dreg(dc2);
			if (opsize == 2) {
				src = (si5b)(si4b)src;
			}
			flgs = src < 0;
			flgo = dst2 < 0;
			newv = dst2 - src;
			if (opsize == 2) {
				newv = (ui4b)newv;
			}
			ZFLG = (newv == 0);
			NFLG = (newv < 0);
			VFLG = (flgs != flgo) && (NFLG != flgo);
			CFLG = (flgs && ! flgo) || (NFLG && ((! flgo) || flgs));
			if (ZFLG) {
				if (opsize == 2) {
					put_word(rn1, m68k_dreg(du1));
					put_word(rn2, m68k_dreg(du2));
				} else {
					put_word(rn1, m68k_dreg(du1));
					put_word(rn2, m68k_dreg(du2));
				}
			}
		}
	}
	if (! ZFLG) {
		if (opsize == 2) {
			m68k_dreg(du1) = (m68k_dreg(du1) & ~ 0xffff) | ((ui5b)dst1 & 0xffff);
			m68k_dreg(du2) = (m68k_dreg(du2) & ~ 0xffff) | ((ui5b)dst2 & 0xffff);
		} else {
			m68k_dreg(du1) = dst1;
			m68k_dreg(du2) = dst2;
		}
	}
}
#endif

#if Use68020
LOCALPROC DoMOVES(void)
{
	if (! regs.s) {
		BackupPC();
		Exception(8);
	} else {
		ui4b extra = nextiword();
		FindOpSizeFromb76();
		DecodeModeRegister(mode, reg);
		if (extra & 0x0800) {
			ui5b src = regs.regs[(extra >> 12) & 0x0F];
			SetArgValue(src);
		} else {
			ui5b rr = (extra >> 12) & 7;
			si5b srcvalue = GetArgValue();
			if (extra & 0x8000) {
				m68k_areg(rr) = srcvalue;
			} else {
				DecodeModeRegister(0, rr);
				SetArgValue(srcvalue);
			}
		}
	}
}
#endif

LOCALPROCUSEDONCE DoCode0(void)
{
	if (b8 == 1) {
		if (mode == 1) {
			/* MoveP 0000ddd1mm001aaa */
			ui5b TheReg = reg;
			ui5b TheRg9 = rg9;
			ui5b Displacement = nextiword();
				/* shouldn't this sign extend ? */
			CPTR memp = m68k_areg(TheReg) + Displacement;
#if 0
			if ((Displacement & 0x00008000) != 0) {
				/***** for testing only *****/
				BackupPC();
				op_illg();
			}
#endif

			switch (b76) {
				case 0:
					{
						ui4b val = ((get_byte(memp) & 0x00FF) << 8)
							| (get_byte(memp + 2) & 0x00FF);
						m68k_dreg(TheRg9) = (m68k_dreg(TheRg9) & ~ 0xffff) | ((val) & 0xffff);
					}
					break;
				case 1:
					{
						ui5b val = ((get_byte(memp) << 24) & 0x00FF)
							| ((get_byte(memp + 2) << 16) & 0x00FF)
							| ((get_byte(memp + 4) << 8) & 0x00FF)
							| (get_byte(memp + 6) & 0x00FF);
						m68k_dreg(TheRg9) = (val);
					}
					break;
				case 2:
					{
						si4b src = m68k_dreg(TheRg9);
						put_byte(memp, src >> 8); put_byte(memp + 2, src);
					}
					break;
				case 3:
					{
						si5b src = m68k_dreg(TheRg9);
						put_byte(memp, src >> 24); put_byte(memp + 2, src >> 16);
						put_byte(memp + 4, src >> 8); put_byte(memp + 6, src);
					}
					break;
			}

		} else {
			/* dynamic bit, Opcode = 0000ddd1ttmmmrrr */
			DoBinBitOp1(0, rg9, mode, reg, bitop());
		}
	} else {
		if (rg9 == 4) {
			/* static bit 00001010ssmmmrrr */
			DoBinBitOp1(7, 4, mode, reg, bitop());
		} else
#if Use68020 || AccurateIllegal
		if (b76 == 3) {
#if Use68020
			if (rg9 < 3) {
				/* CHK2 or CMP2 00000ss011mmmrrr */
				/* ReportAbnormal("CHK2 or CMP2 instruction"); */
				DoCHK2orCMP2();
			} else
			if (rg9 >= 5) {
				if ((mode == 7) && (reg == 4)) {
					/* CAS2 00001ss011111100 */
					ReportAbnormal("DoCAS2 instruction");
					DoCAS2();
				} else {
					/* CAS  00001ss011mmmrrr */
					ReportAbnormal("CAS instruction");
					DoCAS();
				}
			} else
			if (rg9 == 3) {
				/* CALLM or RTM 0000011011mmmrrr */
				ReportAbnormal("CALLM or RTM instruction");
			} else
#endif
			{
				op_illg();
			}
		} else
#endif
		if (rg9 == 6) {
			if (mode != 1) {
				FindOpSizeFromb76();
				DoCompare(7, 4, mode, reg);
			} else {
				op_illg();
			}
		} else if (rg9 == 7) {
#if Use68020
			ReportAbnormal("MoveS instruction");
			/* MoveS 00001110ssmmmrrr */
			DoMOVES();
#else
			op_illg();
#endif
		} else {
			ui5b BinOp;

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
				default: /* for compiler. should be 0, 1, 2, 3, or 5 */
					BinOp = BinOpEor;
					break;
			}
			if (((opcode & 0x0400) == 0) && (mode == 7) && (reg == 4)) {
				if (b76 == 0) {
					DoBinOpStatusCCR(falseblnr, BinOp);
				} else {
					if (! regs.s) {
						BackupPC();
						Exception(8);
					} else {
						DoBinOpStatusCCR(trueblnr, BinOp);
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

LOCALPROCUSEDONCE DoCheck(ui5b m1, ui5b r1, ui5b r2)
{
	si5b srcvalue;
	si5b dstvalue;

	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();
	DecodeModeRegister(0, r2);
	dstvalue = GetArgValue();
	if (dstvalue < 0) {
		NFLG = 1;
		Exception(6);
	} else if (dstvalue > srcvalue) {
		NFLG = 0;
		Exception(6);
	}
}

LOCALPROCUSEDONCE DoLea(ui5b m1, ui5b r1, ui5b r2)
{
	si5b srcvalue;

	DecodeModeRegister(m1, r1);
	if (GetEffectiveAddress(&srcvalue)) {
		m68k_areg(r2) = srcvalue;
	}
}

LOCALPROCUSEDONCE DoTest1(ui5b m1, ui5b r1)
{
	si5b srcvalue;

	DecodeModeRegister(m1, r1);
	srcvalue = GetArgValue();

	VFLG = CFLG = 0;
	ZFLG = (srcvalue == 0);
	NFLG = (srcvalue < 0);
}

LOCALPROC reglist(si4b direction, ui5b m1, ui5b r1)
{
	si4b z;
	si5b p;
	ui5b regmask;

	regmask = nextiword();
	switch (m1) {
		case 3:
			if (direction == 1) {
				p = m68k_areg(r1);
				if (opsize == 2) {
					for (z = 0; z < 16; ++z) {
						if ((regmask & (1 << (z))) != 0) {
							regs.regs[z] = get_word(p);
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
				m68k_areg(r1) = p;
			} else {
#if AccurateIllegal
				op_illg();
#endif
			}
			break;
		case 4:
			if (direction == 0) {
				p = m68k_areg(r1);
#if Use68020
				{
					int n = 0;

					for (z = 0; z < 16; ++z) {
						if ((regmask & (1 << z)) != 0) {
							n++;
						}
					}
					m68k_areg(r1) = p - n * opsize;
				}
#endif

				if (opsize == 2) {
					for (z = 16; --z >= 0; ) {
						if ((regmask & (1 << (15 - z))) != 0) {
							p -= 2;
							put_word(p, regs.regs[z]);
						}
					}
				} else {
					for (z = 16; --z >= 0; ) {
						if ((regmask & (1 << (15 - z))) != 0) {
							p -= 4;
							put_long(p, regs.regs[z]);
						}
					}
				}
#if ! Use68020
				m68k_areg(r1) = p;
#endif
			} else {
#if AccurateIllegal
				op_illg();
#endif
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
								regs.regs[z] = get_word(p);
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

#define ui5b_lo(x) ((x) & 0x0000FFFF)
#define ui5b_hi(x) (((x) >> 16) & 0x0000FFFF)

#if Use68020
struct ui6r {
	ui5b hi;
	ui5b lo;
};
typedef struct ui6r ui6r;
#endif

#if Use68020
LOCALPROC Ui6r_Negate(ui6r *v)
{
	v->hi = ~ v->hi;
	v->lo = - v->lo;
	if (v->lo == 0) {
		v->hi++;
	}
}
#endif

#if Use68020
LOCALFUNC blnr Ui6r_IsZero(ui6r *v)
{
	return (v->hi == 0) && (v->lo == 0);
}
#endif

#if Use68020
LOCALFUNC blnr Ui6r_IsNeg(ui6r *v)
{
	return ((si5b)v->hi) < 0;
}
#endif

#if Use68020
LOCALPROC mul_unsigned(ui5b src1, ui5b src2, ui6r *dst)
{
	ui5b src1_lo = ui5b_lo(src1);
	ui5b src2_lo = ui5b_lo(src2);
	ui5b src1_hi = ui5b_hi(src1);
	ui5b src2_hi = ui5b_hi(src2);

	ui5b r0 = src1_lo * src2_lo;
	ui5b r1 = src1_hi * src2_lo;
	ui5b r2 = src1_lo * src2_hi;
	ui5b r3 = src1_hi * src2_hi;

	ui5b ra1 = ui5b_hi(r0) + ui5b_lo(r1) + ui5b_lo(r2);

	dst->lo = (ui5b_lo(ra1) << 16) | ui5b_lo(r0);
	dst->hi = ui5b_hi(ra1) + ui5b_hi(r1) + ui5b_hi(r2) + r3;
}
#endif

#if Use68020
LOCALFUNC blnr div_unsigned(ui6r *src, ui5b div,
	ui5b *quot, ui5b *rem)
{
	int i;
	ui5b q = 0;
	ui5b cbit = 0;
	ui5b src_hi = src->hi;
	ui5b src_lo = src->lo;

	if (div <= src_hi) {
		return trueblnr;
	}
	for (i = 0 ; i < 32 ; i++) {
		cbit = src_hi & 0x80000000ul;
		src_hi <<= 1;
		if (src_lo & 0x80000000ul) {
			src_hi++;
		}
		src_lo <<= 1;
		q = q << 1;
		if (cbit || div <= src_hi) {
			q |= 1;
			src_hi -= div;
		}
	}
	*quot = q;
	*rem = src_hi;
	return falseblnr;
}
#endif

#if Use68020
LOCALPROC DoMulL(void)
{
	ui6r dst;
	ui5b srcvalue;
	ui4b extra = nextiword();
	ui5b r2 = (extra >> 12) & 7;
	ui5b dstvalue = m68k_dreg(r2);

	DecodeModeRegister(mode, reg);
	srcvalue = (ui5b)GetArgValue();

	if (extra & 0x800) {
		/* MULS.L - signed */

		si5b src1 = (si5b)srcvalue;
		si5b src2 = (si5b)dstvalue;
		flagtype s1 = src1 < 0;
		flagtype s2 = src2 < 0;
		flagtype sr = s1 != s2;

		/* ReportAbnormal("MULS.L"); */
		/* used by Sys 7.5.5 boot extensions */
		if (s1) {
			src1 = - src1;
		}
		if (s2) {
			src2 = - src2;
		}
		mul_unsigned((ui5b)src1, (ui5b)src2, &dst);
		if (sr) {
			Ui6r_Negate(&dst);
		}
		VFLG = CFLG = 0;
		ZFLG = Ui6r_IsZero(&dst);
		NFLG = Ui6r_IsNeg(&dst);
		if (extra & 0x400) {
			m68k_dreg(extra & 7) = dst.hi;
		} else {
			if ((dst.lo & 0x80000000) != 0) {
				if ((dst.hi & 0xffffffff) != 0xffffffff) {
					VFLG = 1;
				}
			} else {
				if (dst.hi != 0) {
					VFLG = 1;
				}
			}
		}
	} else {
		/* MULU.L - unsigned */

		/* ReportAbnormal("MULU.U"); */
		/* Used by various Apps */

		mul_unsigned(srcvalue, dstvalue, &dst);

		VFLG = CFLG = 0;
		ZFLG = Ui6r_IsZero(&dst);
		NFLG = Ui6r_IsNeg(&dst);
		if (extra & 0x400) {
			m68k_dreg(extra & 7) = dst.hi;
		} else {
			if (dst.hi != 0) {
				VFLG = 1;
			}
		}
	}
	m68k_dreg(r2) = dst.lo;
}
#endif

#if Use68020
LOCALPROC DoDivL(void)
{
	ui6r v2;
	ui5b src;
	ui5b quot;
	ui5b rem;
	ui4b extra = nextiword();
	ui5b rDr = extra & 7;
	ui5b rDq = (extra >> 12) & 7;

	DecodeModeRegister(mode, reg);
	src = (ui5b)GetArgValue();

	if (src == 0) {
		Exception(5);
		return;
	}
	if (extra & 0x0800) {
		/* signed variant */
		flagtype sr;
		flagtype s2;
		flagtype s1 = ((si5b)src < 0);

		v2.lo = (si5b)m68k_dreg(rDq);
		if (extra & 0x0400) {
			v2.hi = (si5b)m68k_dreg(rDr);
		} else {
			v2.hi = ((si5b)v2.lo) < 0 ? -1 : 0;
		}
		s2 = Ui6r_IsNeg(&v2);
		sr = (s1 != s2);
		if (s2) {
			Ui6r_Negate(&v2);
		}
		if (s1) {
			src = - src;
		}
		if (div_unsigned(&v2, src, &quot, &rem) ||
			sr ? quot > 0x80000000 : quot > 0x7fffffff) {
			VFLG = NFLG = 1;
			CFLG = 0;
		} else {
			if (sr) {
				quot = - quot;
			}
			if (((si5b)rem < 0) != s2) {
				rem = - rem;
			}
			VFLG = CFLG = 0;
			ZFLG = ((si5b)quot) == 0;
			NFLG = ((si5b)quot) < 0;
			m68k_dreg(rDr) = rem;
			m68k_dreg(rDq) = quot;
		}
	} else {
		/* unsigned */

		v2.lo = (ui5b)m68k_dreg(rDq);
		if (extra & 0x400) {
			v2.hi = (ui5b)m68k_dreg(rDr);
		} else {
			v2.hi = 0;
		}
		if (div_unsigned(&v2, src, &quot, &rem)) {
			VFLG = NFLG = 1;
			CFLG = 0;
		} else {
			VFLG = CFLG = 0;
			ZFLG = ((si5b)quot) == 0;
			NFLG = ((si5b)quot) < 0;
			m68k_dreg(rDr) = rem;
			m68k_dreg(rDq) = quot;
		}
	}
}
#endif

#if Use68020
LOCALPROC DoMoveToControl(void)
{
	if (! regs.s) {
		BackupPC();
		Exception(8);
	} else {
		ui4b src = nextiword();
		int regno = (src >> 12) & 0x0F;
		ui5b v = regs.regs[regno];

		switch (src & 0x0FFF) {
			case 0x0000:
				regs.sfc = v & 7;
				/* ReportAbnormal("DoMoveToControl: sfc"); */
				/* happens on entering macsbug */
				break;
			case 0x0001:
				regs.dfc = v & 7;
				/* ReportAbnormal("DoMoveToControl: dfc"); */
				break;
			case 0x0002:
				regs.cacr = v & 0x3;
				/* ReportAbnormal("DoMoveToControl: cacr"); */
				/* used by Sys 7.5.5 boot */
				break;
			case 0x0800:
				regs.usp = v;
				ReportAbnormal("DoMoveToControl: usp");
				break;
			case 0x0801:
				regs.vbr = v;
				/* ReportAbnormal("DoMoveToControl: vbr"); */
				/* happens on entering macsbug */
				break;
			case 0x0802:
				regs.caar = v &0xfc;
				/* ReportAbnormal("DoMoveToControl: caar"); */
				/* happens on entering macsbug */
				break;
			case 0x0803:
				regs.msp = v;
				if (regs.m == 1) {
					m68k_areg(7) = regs.msp;
				}
				/* ReportAbnormal("DoMoveToControl: msp"); */
				/* happens on entering macsbug */
				break;
			case 0x0804:
				regs.isp = v;
				if (regs.m == 0) {
					m68k_areg(7) = regs.isp;
				}
				ReportAbnormal("DoMoveToControl: isp");
				break;
			default:
				op_illg();
				ReportAbnormal("DoMoveToControl: unknown reg");
				break;
		}
	}
}
#endif

#if Use68020
LOCALPROC DoMoveFromControl(void)
{
	if (! regs.s) {
		BackupPC();
		Exception(8);
	} else {
		ui5b v;
		ui4b src = nextiword();
		int regno = (src >> 12) & 0x0F;

		switch (src & 0x0FFF) {
			case 0x0000:
				v = regs.sfc;
				/* ReportAbnormal("DoMoveFromControl: sfc"); */
				/* happens on entering macsbug */
				break;
			case 0x0001:
				v = regs.dfc;
				/* ReportAbnormal("DoMoveFromControl: dfc"); */
				/* happens on entering macsbug */
				break;
			case 0x0002:
				v = regs.cacr;
				/* ReportAbnormal("DoMoveFromControl: cacr"); */
				/* used by Sys 7.5.5 boot */
				break;
			case 0x0800:
				v = regs.usp;
				ReportAbnormal("DoMoveFromControl: usp");
				break;
			case 0x0801:
				v = regs.vbr;
				/* ReportAbnormal("DoMoveFromControl: vbr"); */
				/* happens on entering macsbug */
				break;
			case 0x0802:
				v = regs.caar;
				/* ReportAbnormal("DoMoveFromControl: caar"); */
				/* happens on entering macsbug */
				break;
			case 0x0803:
				v = (regs.m == 1)
					? m68k_areg(7)
					: regs.msp;
				/* ReportAbnormal("DoMoveFromControl: msp"); */
				/* happens on entering macsbug */
				break;
			case 0x0804:
				v = (regs.m == 0)
					? m68k_areg(7)
					: regs.isp;
				ReportAbnormal("DoMoveFromControl: isp");
				break;
			default:
				ReportAbnormal("DoMoveFromControl: unknown reg");
				op_illg();
				break;
		}
		regs.regs[regno] = v;
	}
}
#endif

LOCALPROC DoRTE(void)
{
	if (! regs.s) {
		BackupPC();
		Exception(8);
	} else {
		ui5b NewPC;
		CPTR stackp = m68k_areg(7);
		ui5b NewSR = (ui5b) get_word(stackp);
		stackp += 2;
		NewPC = get_long(stackp);
		stackp += 4;

#if Use68020
		{
			ui4b format = get_word(stackp);
			stackp += 2;

			switch ((format >> 12) & 0x0F) {
				case 0:
					/* ReportAbnormal("rte stack frame format 0"); */
					break;
				case 1:
					ReportAbnormal("rte stack frame format 1");
					NewPC = m68k_getpc() - 2;
						/* rerun instruction */
					break;
				case 2:
					ReportAbnormal("rte stack frame format 2");
					stackp += 4;
					break;
				case 9:
					ReportAbnormal("rte stack frame format 9");
					stackp += 12;
					break;
				case 10:
					ReportAbnormal("rte stack frame format 10");
					stackp += 24;
					break;
				case 11:
					ReportAbnormal("rte stack frame format 11");
					stackp += 84;
					break;
				default:
					ReportAbnormal("unknown rte stack frame format");
					Exception(14);
					return;
					break;
			}
		}
#endif
		m68k_areg(7) = stackp;
		m68k_setSR(NewSR);
		m68k_setpc(NewPC);
	}
}

FORWARDPROC m68k_setstopped(void);

LOCALPROCUSEDONCE DoCode4(void)
{
	if (mode != 1) {
		if (b8 != 0) {
			switch (b76) {
				case 0 :
#if Use68020
					/* Chk.L 0100ddd100mmmrrr */
					ReportAbnormal("CHK.L instruction");
					opsize = 4;
					DoCheck(mode, reg, rg9);
#else
					op_illg();
#endif
					break;
				case 2 :
					/* Chk.W 0100ddd110mmmrrr */
					opsize = 2;
					DoCheck(mode, reg, rg9);
					break;
				case 1 :
					op_illg();
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
						DoUniOp1(mode, reg, UniOpNegX);
					} else {
#if Use68020
	/* reference seems incorrect to say not for 68000 */
#endif
						/* Move from SR 0100000011mmmrrr */
						opsize = 2;
						DecodeModeRegister(mode, reg);
						SetArgValue(m68k_getSR());
					}
					break;
				case 1 :
					if (b76 != 3) {
						/* Clr 01000010ssmmmrrr */
						FindOpSizeFromb76();
						DoMove(8, 0, mode, reg);
					} else {
#if Use68020
						/* Move from CCR 0100001011mmmrrr */
						ReportAbnormal("Move from CCR");
						opsize = 2;
						DecodeModeRegister(mode, reg);
						SetArgValue(m68k_getSR() & 0xFF);
#else
						op_illg();
#endif
					}
					break;
				case 2 :
					if (b76 != 3) {
						/* Neg 01000100ssmmmrrr */
						FindOpSizeFromb76();
						DoUniOp1(mode, reg, UniOpNeg);
					} else {
						/* 0100010011mmmrrr */
						opsize = 2;
						DecodeModeRegister(mode, reg);
						m68k_setCR(GetArgValue());
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
						DecodeModeRegister(mode, reg);
						m68k_setSR(GetArgValue());
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
								ui5b srcreg = reg;
								ui5b src = (ui5b)m68k_dreg(srcreg);
								si5b dst = (si5b)(((src >> 16) & 0xFFFF) | ((src&0xFFFF) << 16));
								VFLG = CFLG = 0;
								ZFLG = (dst == 0);
								NFLG = (dst < 0);
								m68k_dreg(srcreg) = dst;
							} else {
								/* PEA 0100100001mmmrrr */
								si5b srcvalue;

								opsize = 4;
								DecodeModeRegister(mode, reg);
								if (GetEffectiveAddress(&srcvalue)) {
									m68k_areg(7) -= 4;
									put_long(m68k_areg(7), srcvalue);
								} else {
#if Use68020 || AccurateIllegal
#if Use68020
									if (mode == 1) {
										/* BKPT 0100100001001rrr */
										ReportAbnormal("BKPT instruction");
										op_illg();
									} else
#endif
									{
										op_illg();
									}
#endif
								}
							}
							break;
						case 2 :
						case 3 :
							if (mode == 0) {
								if (b76 == 2) {
									/* EXT.W */
									ui5b srcreg = reg;
									si5b src = m68k_dreg(srcreg);
									ui4b dst = (si4b)(si3b)src;
									VFLG = CFLG = 0;
									ZFLG = ((si4b)(dst)) == 0;
									NFLG = ((si4b)(dst)) < 0;
									m68k_dreg(srcreg) = (m68k_dreg(srcreg) & ~ 0xffff) | ((dst) & 0xffff);
								} else {
									/* EXT.L */
									ui5b srcreg = reg;
									si5b src = m68k_dreg(srcreg);
									ui5b dst = (si5b)(si4b)src;
									VFLG = CFLG = 0;
									ZFLG = ((si5b)(dst)) == 0;
									NFLG = ((si5b)(dst)) < 0;
									m68k_dreg(srcreg) = (dst);
								}
							} else {
								/* MOVEM reg to mem 01001d001ssmmmrrr */
								opsize = 2 * b76 - 2;
								reglist(0, mode, reg);
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
							op_illg(); /* the ILLEGAL instruction */
						} else {
							/* Tas 0100101011mmmrrr */
							opsize = 1;
							DoUniOp1(mode, reg, UniOpTAS);
						}
					}
					break;
				case 6 :
					if (((opcode >> 7) & 1) == 1) {
						/* MOVEM mem to reg 0100110011smmmrrr */
						opsize = 2 * b76 - 2;
						reglist(1, mode, reg);
					} else {
#if Use68020
						opsize = 4;
						if (((opcode >> 7) & 1) == 1) {
							/* DIVU 0100110001mmmrrr 0rrr0s0000000rrr */
							/* DIVS 0100110001mmmrrr 0rrr1s0000000rrr */
							ReportAbnormal("DIVS/DIVU long");
							DoDivL();
						} else {
							/* MULU 0100110000mmmrrr 0rrr0s0000000rrr */
							/* MULS 0100110000mmmrrr 0rrr1s0000000rrr */
							DoMulL();
						}
#else
						op_illg();
#endif
					}
					break;
				case 7 :
					switch (b76) {
						case 0 :
							op_illg();
							break;
						case 1 :
							switch (mode) {
								case 0 :
								case 1 : /* this case actually handled elsewhere */
									/* Trap 010011100100vvvv */
									Exception((opcode & 15) + 32);
									break;
								case 2 :
									/* Link */
									{
										ui5b srcreg = reg;
										CPTR stackp = m68k_areg(7);
										stackp -= 4;
										m68k_areg(7) = stackp; /* only matters if srcreg == 7 */
										put_long(stackp, m68k_areg(srcreg));
										m68k_areg(srcreg) = stackp;
										m68k_areg(7) += (si5b)(si4b)nextiword();
									}
									break;
								case 3 :
									/* Unlk */
									{
										ui5b srcreg = reg;
										if (srcreg != 7) {
											si5b src = m68k_areg(srcreg);
											m68k_areg(srcreg) = get_long(src);
											m68k_areg(7) =  src + 4;
										} else {
											/* wouldn't expect this to happen */
											m68k_areg(7) = get_long(m68k_areg(7)) + 4;
										}
									}
									break;
								case 4 :
									/* MOVE USP 0100111001100aaa */
									if (! regs.s) {
										BackupPC();
										Exception(8);
									} else {
										regs.usp = m68k_areg(reg);
									}
									break;
								case 5 :
									/* MOVE USP 0100111001101aaa */
									if (! regs.s) {
										BackupPC();
										Exception(8);
									} else {
										m68k_areg(reg) = regs.usp;
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
											if (! regs.s) {
												BackupPC();
												Exception(8);
											} else {
												m68k_setSR((si4b)nextiword());
												m68k_setstopped();
											}
											break;
										case 3 :
											/* Rte 0100111001110011 */
											DoRTE();
											break;
										case 4 :
											/* Rtd 0100111001110100 */
#if Use68020
											{
												ui5b NewPC = get_long(m68k_areg(7));
												si5b offs = (si5b)(si4b)nextiword();
												/* ReportAbnormal("RTD"); */
												/* used by Sys 7.5.5 boot */
												m68k_areg(7) += (4 + offs);
												m68k_setpc(NewPC);
											}
#else
											op_illg();
#endif
											break;
										case 5 :
											/* Rts 0100111001110101 */
											{
												ui5b NewPC = get_long(m68k_areg(7));
												m68k_areg(7) += 4;
												m68k_setpc(NewPC);
											}
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
												ui5b NewPC;
												CPTR stackp = m68k_areg(7);
												ui5b NewCR = get_word(stackp);
												stackp += 2;
												NewPC = get_long(stackp);
												stackp += 4;
												m68k_areg(7) = stackp;
												m68k_setCR(NewCR);
												m68k_setpc(NewPC);
											}
											break;
									}
									break;
								case 7 :
#if Use68020
									/* MOVEC 010011100111101m */
									/* ReportAbnormal("MOVEC"); */
									switch (reg) {
										case 2:
											DoMoveFromControl();
											break;
										case 3:
											DoMoveToControl();
											break;
										default:
											op_illg();
											break;
									}
#else
									op_illg();
#endif
									break;
							}
							break;
						case 2 :
							/* Jsr 0100111010mmmrrr */
							{
								si5b srcvalue;

								opsize = 0;
								DecodeModeRegister(mode, reg);
								if (GetEffectiveAddress(&srcvalue)) {
									m68k_areg(7) -= 4;
									put_long(m68k_areg(7), m68k_getpc());
									m68k_setpc(srcvalue);
								}
							}
							break;
						case 3 :
							/* JMP 0100111011mmmrrr */
							{
								si5b srcvalue;

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
	} else {
#if Use68020
		if ((opcode & 0xFFF8) == 0x4808) {
			/* Link.L 0100100000001rrr */

			ui5b srcreg = reg;
			CPTR stackp = m68k_areg(7);

			ReportAbnormal("Link.L");

			stackp -= 4;
			m68k_areg(7) = stackp; /* only matters if srcreg == 7 */
			put_long(stackp, m68k_areg(srcreg));
			m68k_areg(srcreg) = stackp;
			m68k_areg(7) += (si5b)nextilong();
		} else
		if ((opcode & 0xFF00) == 0x4A00) {
			/* Tst  01001010ssmmmrrr */ /* address register allowed */
			FindOpSizeFromb76();
			DoTest1(mode, reg);
		} else
#endif
		if ((opcode & 0xFFF0) == 0x4E40) {
			/* Trap 010011100100vvvv */
			Exception((opcode & 15) + 32);
		} else {
			op_illg();
		}
	}
}

LOCALPROCUSEDONCE DoCode5(void)
{
	if (b76 == 3) {
		ui5b cond = (opcode >> 8) & 15;

		if (mode == 1) {
			/* DBcc 0101cccc11001ddd */
			si5b dstvalue;
#if FastRelativeJump
			ui3p srcvalue = pc_p;
#else
			si5b srcvalue = m68k_getpc();
#endif

			srcvalue += (si5b)(si4b)nextiword();
			if (! cctrue(cond)) {
				dstvalue = (si5b)(si4b)m68k_dreg(reg);
				--dstvalue;
				m68k_dreg(reg) = (m68k_dreg(reg) & ~ 0xffff)
					| ((dstvalue) & 0xffff);
				if (dstvalue != -1) {
#if FastRelativeJump
					pc_p = srcvalue;
#else
					m68k_setpc(srcvalue);
#endif
				}
			}
		} else {
#if Use68020 || AccurateIllegal
			if ((mode == 7) && (reg >= 2)) {
#if Use68020
				/* TRAPcc 0101cccc11111sss */
				/* ReportAbnormal("TRAPcc"); */
				switch (reg) {
					case 2:
						ReportAbnormal("TRAPcc word data");
						(void) nextiword();
						break;
					case 3:
						ReportAbnormal("TRAPcc long data");
						(void) nextilong();
						break;
					case 4:
						/* no optional data */
						break;
					default:
						ReportAbnormal("TRAPcc illegal format");
						op_illg();
						break;
				}
				if (cctrue(cond)) {
					ReportAbnormal("TRAPcc trapping");
					Exception(7);
					/* pc pushed onto stack wrong */
				}
#else
				op_illg();
#endif
			} else
#endif
			{
				/* Scc 0101cccc11mmmrrr */
				opsize = 1;
				DecodeModeRegister(mode, reg);
				SetArgValue(cctrue(cond) ? 0xff : 0);
			}
		}
	} else {
		ui5b BinOp;

		/* 0101nnnossmmmrrr */
		FindOpSizeFromb76();
		if (mode != 1) {
			if (b8 == 0) {
				BinOp = BinOpAdd; /* AddQ 0101nnn0ssmmmrrr */
			} else {
				BinOp = BinOpSub; /* SubQ 0101nnn1ssmmmrrr */
			}
			DoBinOp1(8, octdat(rg9), mode, reg, BinOp);
		} else {
			DoBinOpA(8, octdat(rg9), reg, b8 != 0);
		}
	}
}

LOCALPROCUSEDONCE DoCode6(void)
{
	ui5b cond = (opcode >> 8) & 15;
	ui5b src = ((ui5b)opcode) & 255;
#if FastRelativeJump
	ui3p s = pc_p;
#else
	ui5b s = m68k_getpc();
#endif

	if (src == 0) {
		s += (si5b)(si4b)nextiword();
	} else
#if Use68020
	if (src == 255) {
		s += (si5b)nextilong();
		/* ReportAbnormal("long branch in DoCode6"); */
		/* Used by various Apps */
	} else
#endif
	{
		s += (si5b)(si3b)src;
	}
	if (cond == 1) {
		/* Bsr 01100001nnnnnnnn */
		m68k_areg(7) -= 4;
		put_long(m68k_areg(7), m68k_getpc());
#if FastRelativeJump
		pc_p = s;
#else
		m68k_setpc(s);
#endif
	} else {
		/* Bra/Bcc 0110ccccnnnnnnnn */
		if (cctrue(cond)) {
#if FastRelativeJump
			pc_p = s;
#else
			m68k_setpc(s);
#endif
		}
	}
}

LOCALPROCUSEDONCE DoCode7(void)
{
	/* MoveQ 0111ddd0nnnnnnnn */
	ui5b src = (si5b)(si3b)(opcode & 255);
	ui5b dstreg = rg9;
	VFLG = CFLG = 0;
	ZFLG = ((si5b)(src)) == 0;
	NFLG = ((si5b)(src)) < 0;
	m68k_dreg(dstreg) = (src);
}

#if Use68020
LOCALPROC DoUNPK(void)
{
	si5b val;
	ui5b m1 = ((opcode >> 3) & 1) << 2;
	ui5b srcreg = reg;
	ui5b dstreg = rg9;
	si5b offs = (si5b)(si4b)nextiword();

	opsize = 1;
	DecodeModeRegister(m1, srcreg);
	val = GetArgValue();

	val = (((val & 0xF0) << 4) | (val & 0x0F)) + offs;

	opsize = 2;
	DecodeModeRegister(m1, dstreg);
	SetArgValue(val);
}
#endif

#if Use68020
LOCALPROC DoPACK(void)
{
	si5b val;
	ui5b m1 = ((opcode >> 3) & 1) << 2;
	ui5b srcreg = reg;
	ui5b dstreg = rg9;
	si5b offs = (si5b)(si4b)nextiword();

	opsize = 2;
	DecodeModeRegister(m1, srcreg);
	val = GetArgValue();

	val += offs;
	val = ((val >> 4) & 0xf0) | (val & 0xf);

	opsize = 1;
	DecodeModeRegister(m1, dstreg);
	SetArgValue(val);
}
#endif

LOCALPROCUSEDONCE DoCode8(void)
{
	if (b76 == 3) {
		ui5b BinOp;

		if (b8 == 0) {
			BinOp = BinOpDivU; /* DivU 1000ddd011mmmrrr */
		} else {
			BinOp = BinOpDivS; /* DivS 1000ddd111mmmrrr */
		}
		if (mode != 1) {
			DoBinOpDiv1(mode, reg, rg9, BinOp);
		} else {
			op_illg();
		}
	} else {
		if ((b8 == 1) && (mode < 2)) {
#if Use68020 || AccurateIllegal
			switch (b76) {
				case 0:
#endif
					/* SBCD 1000xxx10000mxxx */
					opsize = 1;
					if (mode == 0) {
						DoBinOp1(0, reg, 0, rg9, BinOpSubBCD);
					} else {
						DoBinOp1(4, reg, 4, rg9, BinOpSubBCD);
					}
#if Use68020 || AccurateIllegal
					break;
#if Use68020
				case 1:
					/* PACK 1000rrr10100mrrr */
					ReportAbnormal("PACK");
					DoPACK();
					break;
				case 2:
					/* UNPK 1000rrr11000mrrr */
					ReportAbnormal("UNPK");
					DoUNPK();
					break;
#endif
				default:
					op_illg();
					break;
			}
#endif /* Use68020 || AccurateIllegal */
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
		/* SUBA 1001dddm11mmmrrr */
		opsize = b8 * 2 + 2;
		DoBinOpA(mode, reg, rg9, trueblnr);
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
		DoCompareA(mode, reg, rg9);
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
		ui5b BinOp;
		if (b8 == 0) {
			BinOp = BinOpMulU;  /* MulU 1100ddd011mmmrrr */
		} else {
			BinOp = BinOpMulS; /* MulS 1100ddd111mmmrrr */
		}
		if (mode != 1) {
			DoBinOpMul1(mode, reg, rg9, BinOp);
		} else {
			op_illg();
		}
	} else if ((mode < 2) && (b8 == 1)) {
		switch (b76) {
			case 0 :
				/* ABCD 1100ddd10000mrrr */
				/* does anyone use this? */
				opsize = 1;
				if (mode == 0) {
					DoBinOp1(0, reg, 0, rg9, BinOpAddBCD);
				} else {
					DoBinOp1(4, reg, 4, rg9, BinOpAddBCD);
				}
				break;
			case 1 :
				/* Exg 1100ddd10100trrr, opsize = 4 */
				if (mode == 0) {
					ui5b srcreg = rg9;
					ui5b dstreg = reg;
					si5b src = m68k_dreg(srcreg);
					si5b dst = m68k_dreg(dstreg);
					m68k_dreg(srcreg) = dst;
					m68k_dreg(dstreg) = src;
				} else {
					ui5b srcreg = rg9;
					ui5b dstreg = reg;
					si5b src = m68k_areg(srcreg);
					si5b dst = m68k_areg(dstreg);
					m68k_areg(srcreg) = dst;
					m68k_areg(dstreg) = src;
				}
				break;
			case 2 :
				{
					/* Exg 1100ddd110001rrr, opsize = 4 */
					ui5b srcreg = rg9;
					ui5b dstreg = reg;
					si5b src = m68k_dreg(srcreg);
					si5b dst = m68k_areg(dstreg);
					m68k_dreg(srcreg) = dst;
					m68k_areg(dstreg) = src;
				}
				break;
		}
	} else {
		/* And 1100dddmssmmmrrr */
		FindOpSizeFromb76();
		if (b8 == 1) {
			DoBinOp1(0, rg9, mode, reg, BinOpAnd);
		} else {
			DoBinOp1(mode, reg, 0, rg9, BinOpAnd);
		}
	}
}

LOCALPROCUSEDONCE DoCodeD(void)
{
	if (b76 == 3) {
		/* ADDA 1101dddm11mmmrrr */
		opsize = b8 * 2 + 2;
		DoBinOpA(mode, reg, rg9, falseblnr);
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

LOCALFUNC ui5b rolops(ui5b x)
{
	ui5b binop;

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
		default: /* for compiler. should be 0, 1, 2, or 3 */
			binop = BinOpROL;
			break;
	}
	if (! b8) {
		binop++; /* 'R' */
	} /* else 'L' */
	return binop;
}

#if Use68020
LOCALPROC DoBitField(void)
{
	ui5b tmp;
	ui5b newtmp;
	si5b dsta;
	ui5b bf0;
	ui5b bf1;
	ui5b dstreg = opcode & 7;
	ui4b extra = nextiword();
	si5b offset = ((extra & 0x0800) != 0)
		? m68k_dreg((extra >> 6) & 7)
		: ((extra >> 6) & 0x1f);
	ui5b width = ((extra & 0x0020) != 0)
		? m68k_dreg(extra & 7)
		: extra;

	width = ((width - 1) & 0x1f) + 1; /* 0 -> 32 */
	if (mode == 0) {
		bf0 = m68k_dreg(dstreg);
		offset &= 0x1f;
		tmp = bf0 << offset;
	} else {
		DecodeModeRegister(mode, reg);
		if (GetEffectiveAddress(&dsta)) {
			dsta += (offset >> 3) | (offset & 0x80000000 ? ~ 0x1fffffff : 0);
			offset &= 7;
			{
				bf0 = get_long(dsta);
				bf1 = get_byte(dsta + 4) & 0xff;
				tmp = (bf0 << offset) | (bf1 >> (8 - offset));
			}
		}
	}

	NFLG = ((si5b)tmp) < 0;
	tmp >>= (32 - width);
	ZFLG = tmp == 0;
	VFLG = 0;
	CFLG = 0;

	newtmp = tmp;

	switch ((opcode >> 8) & 7) {
		case 0: /* BFTST */
			/* do nothing */
			break;
		case 1: /* BFEXTU */
			m68k_dreg((extra >> 12) & 7) = tmp;
			break;
		case 2: /* BFCHG */
			newtmp = (~ newtmp) & ((1 << width) - 1);
			break;
		case 3: /* BFEXTS */
			if (NFLG) {
				m68k_dreg((extra >> 12) & 7) = tmp
					| ((width == 32) ? 0 : (-1 << width));
			} else {
				m68k_dreg((extra >> 12) & 7) = tmp;
			}
			break;
		case 4: /* BFCLR */
			newtmp = 0;
			break;
		case 5: /* BFFFO */
			{
				ui5b mask = 1 << (width - 1);
				si5b i = offset;
				while (mask && ((tmp & mask) != 0)) {
					mask >>= 1;
					i++;
				}
				m68k_dreg((extra >> 12) & 7) = i;
			}
			break;
		case 6: /* BFSET */
			newtmp = (1 << width) - 1;
			break;
		case 7: /* BFINS */
			newtmp = m68k_dreg((extra >> 12) & 7) & ((1 << width) - 1);
			break;
	}

	if (newtmp != tmp) {
		ui5b mask = ~ ((((1 << width) - 1) << (32 - width)) >> offset);

		newtmp <<= (32 - width);
		bf0 = (bf0 & mask) | (newtmp >> offset);
		if (mode == 0) {
			m68k_dreg(dstreg) = bf0;
		} else {
			put_long(dsta, bf0);
			if ((offset + width) > 32) {
				bf1 = (bf1 & (0xff >> (width - 32 + offset)))
					| (newtmp << (8 - offset));
				put_byte(dsta + 4, bf1);
			}
		}
	}
}
#endif

LOCALPROCUSEDONCE DoCodeE(void)
{
	if (b76 == 3) {
#if Use68020 || AccurateIllegal
		if ((opcode & 0x0800) != 0) {
#if Use68020
			/* 11101???11mmmrrr */
			ReportAbnormal("Bit Field operator");
			DoBitField();
#else
			op_illg();
#endif
		} else
#endif
		{
			opsize = 2;
			DoBinOp1(8, 1, mode, reg, rolops(rg9));
		}
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
	/* ReportAbnormal("DoCodeF"); */
	/* op_illg(); */
	BackupPC();
	Exception(0xB);
}

LOCALPROC op_illg(void)
{
	BackupPC();
	Exception(4);
}

LOCALVAR ui5b MaxInstructionsToGo;

LOCALPROC m68k_go_MaxInstructions(void)
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

LOCALVAR ui5b MoreInstructionsToGo;

LOCALPROC NeedToGetOut(void)
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

GLOBALFUNC ui5b GetInstructionsRemaining(void)
{
	return MoreInstructionsToGo + MaxInstructionsToGo;
}

GLOBALPROC SetInstructionsRemaining(ui5b n)
{
	if (MaxInstructionsToGo >= n) {
		MoreInstructionsToGo = 0;
		MaxInstructionsToGo = n;
	} else {
		MoreInstructionsToGo = n - MaxInstructionsToGo;
	}
}

LOCALPROC do_trace(void)
{
	regs.TracePending = trueblnr;
	NeedToGetOut();
}

LOCALPROC SetExternalInterruptPending(void)
{
	regs.ExternalInterruptPending = trueblnr;
	NeedToGetOut();
}

LOCALPROC m68k_setstopped(void)
{
	/* not implemented. doesn't seemed to be used on Mac Plus */
	Exception(4); /* fake an illegal instruction */
}

GLOBALPROC m68k_go_nInstructions(ui5b n)
{
	MaxInstructionsToGo = n;
	MoreInstructionsToGo = 0;
	do {

#if 0
		if (regs.ResetPending) {
			m68k_DoReset();
		}
#endif
		if (regs.TracePending) {
			Exception(9);
		}
		if (regs.ExternalInterruptPending) {
			regs.ExternalInterruptPending = falseblnr;
			DoCheckExternalInterruptPending();
		}
		if (regs.t1) {
			do_trace();
		}
		m68k_go_MaxInstructions();
		MaxInstructionsToGo = MoreInstructionsToGo;
		MoreInstructionsToGo = 0;
	} while (MaxInstructionsToGo != 0);
}
