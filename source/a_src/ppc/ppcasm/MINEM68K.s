;	MINEM68K.s
;
;	Copyright (C) 2004 Paul C. Pratt
;
;	You can redistribute this file and/or modify it under the terms
;	of version 2 of the GNU General Public License as published by
;	the Free Software Foundation.  You should have received a copy
;	of the license along with this file; see the file COPYING.
;
;	This file is distributed in the hope that it will be useful,
;	but WITHOUT ANY WARRANTY; without even the implied warranty of
;	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;	license for more details.

; PowerPC assembler version of MINEM68K.c


; imports

	import	.MM_Access[PR]
	import	MM_Access[DS]
	toc
	tc	MM_Access[TC],MM_Access[DS]

	import	.customreset[PR]
	import	customreset[DS]
	toc
	tc	customreset[TC],customreset[DS]

	import	.get_real_address[PR]
	import	get_real_address[DS]
	toc
	tc	get_real_address[TC],get_real_address[DS]


; exports


	toc
	tc	DiskInsertedPsuedoException[TC],DiskInsertedPsuedoException[DS]
	csect	DiskInsertedPsuedoException[DS]
	dc.l	.DiskInsertedPsuedoException
	dc.l	TOC[tc0]

	toc
	tc	m68k_IPLchangeNtfy[TC],m68k_IPLchangeNtfy[DS]
	csect	m68k_IPLchangeNtfy[DS]
	dc.l	.m68k_IPLchangeNtfy
	dc.l	TOC[tc0]

	toc
	tc	GetInstructionsRemaining[TC],GetInstructionsRemaining[DS]
	csect	GetInstructionsRemaining[DS]
	dc.l	.GetInstructionsRemaining
	dc.l	TOC[tc0]

	toc
	tc	SetInstructionsRemaining[TC],SetInstructionsRemaining[DS]
	csect	SetInstructionsRemaining[DS]
	dc.l	.SetInstructionsRemaining
	dc.l	TOC[tc0]

	toc
	tc	m68k_reset[TC],m68k_reset[DS]
	csect	m68k_reset[DS]
	dc.l	.m68k_reset
	dc.l	TOC[tc0]

	toc
	tc	MINEM68K_Init[TC],MINEM68K_Init[DS]
	csect	MINEM68K_Init[DS]
	dc.l	.MINEM68K_Init
	dc.l	TOC[tc0]

	toc
	tc	m68k_go_nInstructions[TC],m68k_go_nInstructions[DS]
	csect	m68k_go_nInstructions[DS]
	dc.l	.m68k_go_nInstructions
	dc.l	TOC[tc0]


; ---- register usage -----------

; r1 - stack ptr
; r2 - toc
; r3 - return address
; r4:r10 - parameter passing
; r11:r12 - temp

rPC_p	EQU	r13
rOpCode	EQU	r14
rOpSize	EQU	r15
rBankReadAddr	EQU	r16
rBankWritAddr	EQU	r17
rMaxInstrsToGo	EQU	r18
rRegs	EQU	r19
rFlag_c	EQU	r20
rFlag_z	EQU	r21
rFlag_n	EQU	r22
rFlag_v	EQU	r23
rFlag_x	EQU	r24
rDispatches	EQU	r25
rArgKind	EQU	r26
rArgAddr	EQU	r27


; ---- local data -----------

	toc
	tc	.b2regs9999[TC],.b2regs9999[RW]
	csect	.b2regs9999[RW], 3

fReg_D0: equ $-.b2regs9999[RW]
	ds.l	1
fReg_D1: equ $-.b2regs9999[RW]
	ds.l	1
fReg_D2: equ $-.b2regs9999[RW]
	ds.l	1
fReg_D3: equ $-.b2regs9999[RW]
	ds.l	1
fReg_D4: equ $-.b2regs9999[RW]
	ds.l	1
fReg_D5: equ $-.b2regs9999[RW]
	ds.l	1
fReg_D6: equ $-.b2regs9999[RW]
	ds.l	1
fReg_D7: equ $-.b2regs9999[RW]
	ds.l	1
fReg_A0: equ $-.b2regs9999[RW]
	ds.l	1
fReg_A1: equ $-.b2regs9999[RW]
	ds.l	1
fReg_A2: equ $-.b2regs9999[RW]
	ds.l	1
fReg_A3: equ $-.b2regs9999[RW]
	ds.l	1
fReg_A4: equ $-.b2regs9999[RW]
	ds.l	1
fReg_A5: equ $-.b2regs9999[RW]
	ds.l	1
fReg_A6: equ $-.b2regs9999[RW]
	ds.l	1
fReg_A7: equ $-.b2regs9999[RW]
	ds.l	1
fReg_usp: equ $-.b2regs9999[RW]
	ds.l	1
fReg_isp: equ $-.b2regs9999[RW]
	ds.l	1
fReg_pc: equ $-.b2regs9999[RW]
	ds.l	1
fReg_intmask: equ $-.b2regs9999[RW]
	ds.l	1
fReg_t1: equ $-.b2regs9999[RW]
	ds.b	1
fReg_s: equ $-.b2regs9999[RW]
	ds.b	1
fnot_Reg_m: equ $-.b2regs9999[RW]
	; not on 68000
	ds.b	1
fReg_Pad0: equ $-.b2regs9999[RW]
	ds.b	1
fReg_TracePend: equ $-.b2regs9999[RW]
	ds.b	1
fReg_ExtIntPend: equ $-.b2regs9999[RW]
	ds.b	1
fReg_c: equ $-.b2regs9999[RW]
	ds.b	1
fReg_z: equ $-.b2regs9999[RW]
	ds.b	1
fReg_n: equ $-.b2regs9999[RW]
	ds.b	1
fReg_v: equ $-.b2regs9999[RW]
	ds.b	1
fReg_x: equ $-.b2regs9999[RW]
	ds.b	1
fReg_Pad1: equ $-.b2regs9999[RW]
	ds.b	1
fReg_pc_p: equ $-.b2regs9999[RW]
	ds.l	1
fReg_pc_oldp: equ $-.b2regs9999[RW]
	ds.l	1
fReg_MaxInstrs: equ $-.b2regs9999[RW]
	ds.l	1
fReg_MoreInstrs: equ $-.b2regs9999[RW]
	ds.l	1
fReg_RgLstTmp: equ $-.b2regs9999[RW]
	ds.l	1
fBankReadAddr: equ $-.b2regs9999[RW]
	ds.l	1
fBankWritAddr: equ $-.b2regs9999[RW]
	ds.l	1
fIPL: equ $-.b2regs9999[RW]
	ds.l	1


; dispatch table

	toc
	tc	.Dispatches[TC],.Dispatches[RW]
	csect	.Dispatches[RW], 3
	dc.l	.DoCode0
	dc.l	.DoCode1
	dc.l	.DoCode2
	dc.l	.DoCode3
	dc.l	.DoCode4
	dc.l	.DoCode5
	dc.l	.DoCode6
	dc.l	.DoCode7
	dc.l	.DoCode8
	dc.l	.DoCode9
	dc.l	.DoCodeA
	dc.l	.DoCodeB
	dc.l	.DoCodeC
	dc.l	.DoCodeD
	dc.l	.DoCodeE
	dc.l	.DoCodeF

x_b2S9993: equ $-.Dispatches[RW]
	dc.l	.L49
	dc.l	.L36
	dc.l	.L8
	dc.l	.L10
	dc.l	.L12
	dc.l	.L14
	dc.l	.L16
	dc.l	.L18
	dc.l	.L20
	dc.l	.L22
	dc.l	.L24
	dc.l	.L26
	dc.l	.L28
	dc.l	.L30
	dc.l	.L32
	dc.l	.L34

x_b2S9987: equ $-.Dispatches[RW]
	dc.l	.L139
	dc.l	.L141
	dc.l	.L143
	dc.l	.L145
	dc.l	.L154
	dc.l	.L163
	dc.l	.L165
	dc.l	.L167
	dc.l	.L193
	dc.l	.B2end9957

x_b2S9986: equ $-.Dispatches[RW]
	dc.l	.L170
	dc.l	.L172
	dc.l	.L174
	dc.l	.L176
	dc.l	.L178
	dc.l	.B2end9957
	dc.l	.B2end9957
	dc.l	.B2end9957

x_b2S9983: equ $-.Dispatches[RW]
	dc.l	.L314
	dc.l	.L327
	dc.l	.L357
	dc.l	.L340
	dc.l	.L663
	dc.l	.L679
	dc.l	.L376
	dc.l	.L374
	dc.l	.L378
	dc.l	.L380
	dc.l	.L423
	dc.l	.L458
	dc.l	.L494
	dc.l	.L552
	dc.l	.L623
	dc.l	.L521
	dc.l	.L583

x_b2S9982: equ $-.Dispatches[RW]
	dc.l	.L872
	dc.l	.L859
	dc.l	.L842
	dc.l	.L885
	dc.l	.L910

x_b2S9980: equ $-.Dispatches[RW]
	dc.l	.L1124
	dc.l	.L1126
	dc.l	.L1128
	dc.l	.L1130
	dc.l	.L1133
	dc.l	.L1133

x_b2S9979: equ $-.Dispatches[RW]
	dc.l	.L1364
	dc.l	.L1373
	dc.l	.L1382
	dc.l	.L1391
	dc.l	.L1400
	dc.l	.L1438
	dc.l	.L1454
	dc.l	.L1463

x_b2S9978: equ $-.Dispatches[RW]
	dc.l	.L1472
	dc.l	.L1472
	dc.l	.L1474
	dc.l	.L1478
	dc.l	.L1489
	dc.l	.L1498
	dc.l	.L1507
	dc.l	.op_illg

x_b2S9977: equ $-.Dispatches[RW]
	dc.l	.L1510
	dc.l	.m68k_NextInstruction
	dc.l	.L1521
	dc.l	.L1530
	dc.l	.op_illg
	dc.l	.L1541
	dc.l	.L1543
	dc.l	.L1549


; begin code


	export	.m68k_go_nInstructions[PR]
	export	m68k_go_nInstructions[DS]
	csect	.m68k_go_nInstructions[PR], 5
.m68k_go_nInstructions:
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -80(sp)

	bl	.MyEmulatorEntry

	mr	rMaxInstrsToGo, r3
	addi	r12, 0, 0
	stw	r12, fReg_MoreInstrs(rRegs)
.B2LP_9657:

	lbz	r0, fReg_ExtIntPend(rRegs)
	cmpi	cr0, r0, 0
	bne	cr0,.DoCheckExternalInterruptPending
.AfterCheckExternalInterruptPending:
	lbz	r0, fReg_TracePend(rRegs)
	cmpi	cr0, r0, 0
	bne	cr0,.DoPendingTrace
.AfterDoPendingTrace:
	lbz	r0, fReg_t1(rRegs)
	cmpi	cr0, r0, 0
	bne	cr0,.do_trace
.AfterDoTrace:

	addi	rMaxInstrsToGo, rMaxInstrsToGo, 1
	b	.m68k_NextInstruction
.after_m68k_go_MaxInstructions:
	addi	rPC_p, rPC_p, -2

	lwz	r0, fReg_MoreInstrs(rRegs)
	addi	r12, 0, 0
	mr	rMaxInstrsToGo, r0
	cmpi	cr0, r0, 0
	stw	r12, fReg_MoreInstrs(rRegs)
	bne	cr0,.B2LP_9657
	bl	.MyEmulatorExit

	lwz	r0, 88(sp)
	addi	sp, sp, 80
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


.MyEmulatorEntry:
	lwz	rRegs, .b2regs9999[TC](rtoc)
	lwz	rDispatches, .Dispatches[TC](rtoc)

	lwz	rMaxInstrsToGo, fReg_MaxInstrs(rRegs)

	lwz	rPC_p, fReg_pc_p(rRegs)
	lbz	rFlag_c, fReg_c(rRegs)
	lbz	rFlag_z, fReg_z(rRegs)
	lbz	rFlag_n, fReg_n(rRegs)
	lbz	rFlag_v, fReg_v(rRegs)
	lbz	rFlag_x, fReg_x(rRegs)

	lwz	rBankReadAddr, fBankReadAddr(rRegs)
	lwz	rBankWritAddr, fBankWritAddr(rRegs)

	blr


.MyEmulatorExit:
	stb	rFlag_x, fReg_x(rRegs)
	stb	rFlag_v, fReg_v(rRegs)
	stb	rFlag_n, fReg_n(rRegs)
	stb	rFlag_z, fReg_z(rRegs)
	stb	rFlag_c, fReg_c(rRegs)
	stw	rPC_p, fReg_pc_p(rRegs)
	stw	rMaxInstrsToGo, fReg_MaxInstrs(rRegs)

	blr

; variables that may be changed during MM_Access
;	regs.ExternalInterruptPending
;	regs.intmask
;	MoreInstructionsToGo
;	MaxInstructionsToGo
;	VIAInterruptRequest


	align	4
.m68k_NextInstruction:
	lhzu	rOpCode, 2(rPC_p)
	addi	rMaxInstrsToGo, rMaxInstrsToGo, -1
	cmpi	cr0, rMaxInstrsToGo, 0
	beq	cr0,.after_m68k_go_MaxInstructions
	rlwinm	r12, rOpCode, 22, 26, 29
	lwzx	r12, rDispatches, r12
	mtlr	r12
	blr	; this is a switch branch, not a return


.DecodeModeRegister:
	addi	r12, rDispatches, x_b2S9987
	rlwinm	r5, r3, 2, 0, 29
	lwzx	r5, r12, r5
	mtctr	r5
	bcctr	20, 0
.L139:
	addi	rArgKind, 0, 2
	rlwinm	rArgAddr, r4, 2, 0, 29
	add	rArgAddr, rArgAddr, rRegs
	blr
.L141:
	addi	rArgKind, 0, 2
	rlwinm	rArgAddr, r4, 2, 0, 29
	add	rArgAddr, rArgAddr, rRegs
	addi	rArgAddr, rArgAddr, 32
	blr
.L143:
	addi	rArgKind, 0, 0
	rlwinm	r3, r4, 2, 0, 29
	add	r6, r3, rRegs
	lwz	rArgAddr, 32(r6)
	blr
.L145:
	addi	rArgKind, 0, 0
	rlwinm	r3, r4, 2, 0, 29
	addi	r5, r4, 1
	add	r7, r3, rRegs
	rlwinm	r5, r5, 29, 31, 31
	lwz	rArgAddr, 32(r7)
	and	r5, r5, rOpSize
	add	r3, rArgAddr, rOpSize
	add	r3, r3, r5
	stw	r3, 32(r7)
	blr
.L154:
	addi	rArgKind, 0, 0
	rlwinm	r3, r4, 2, 0, 29
	addi	r5, r4, 1
	add	r7, r3, rRegs
	rlwinm	r5, r5, 29, 31, 31
	lwz	rArgAddr, 32(r7)
	and	r5, r5, rOpSize
	subf	r3, rOpSize, rArgAddr
	subf	rArgAddr, r5, r3
	stw	rArgAddr, 32(r7)
	blr

.L163:
	addi	rArgKind, 0, 0
	rlwinm	r5, r4, 2, 0, 29
	add	r6, r5, rRegs
	lwz	r12, 32(r6)
	lhzu	r3, 2(rPC_p)
	extsh	r3, r3
	add	rArgAddr, r12, r3
	blr
.L165:
	addi	rArgKind, 0, 0
	rlwinm	r5, r4, 2, 0, 29
	add	r6, r5, rRegs
	lwz	r3, 32(r6)
	b	.get_disp_ea
.L167:
	addi	r12, rDispatches, x_b2S9986
	rlwinm	r3, r4, 2, 0, 29
	lwzx	r3, r12, r3
	mtctr	r3
	bcctr	20, 0
.L170:
	addi	rArgKind, 0, 0
	lhzu	r3, 2(rPC_p)
	extsh	rArgAddr, r3
	blr
.L172:
	addi	rArgKind, 0, 0
	lwz	r3, 2(rPC_p)
	addi	rPC_p, rPC_p, 4
	mr	rArgAddr, r3
	blr
.L174:
	mflr	r9
	addi	rArgKind, 0, 0
	bl	.m68k_getpc
	lhzu	r8, 2(rPC_p)
	mtlr	r9
	extsh	r8, r8
	add	rArgAddr, r8, r3
.B2end9957:
	blr
.L176:
	mflr	r9
	addi	rArgKind, 0, 0
	bl	.m68k_getpc
	mtlr	r9
	b	.get_disp_ea
.L178:
	cmpi	cr0, rOpSize, 2
	addi	rArgKind, 0, 3
	blt	cr0,.L181
	bgt	cr0,.L185
.L183:
	lhzu	r3, 2(rPC_p)
	extsh	rArgAddr, r3
	blr
.L181:
	lbz	r3, 3(rPC_p)
	addi	rPC_p, rPC_p, 2
	extsb	rArgAddr, r3
	blr
.L185:
	lwz	r3, 2(rPC_p)
	addi	rPC_p, rPC_p, 4
	mr	rArgAddr, r3
	blr
.L193:
	addi	rArgKind, 0, 3
	mr	rArgAddr, r4
	blr


.get_disp_ea:
	mr	r8, r3
	lhzu	r3, 2(rPC_p)
	rlwinm	r6, r3, 0, 16, 31
	rlwinm	r4, r3, 22, 26, 29
	lwzx	r9, r4, rRegs
	rlwinm.	r6, r6, 0, 20, 20
	bne	cr0,.L133
	extsh	r9, r9
.L133:
	extsb	r7, r3
	add	r4, r8, r7
	add	rArgAddr, r4, r9
	blr


.get_byte:
	rlwinm	r12, r3, 17, 23, 29
	lwzx	r9, r12, rBankReadAddr
	rlwinm	r5, r3, 0, 15, 31
	cmpi	cr0, r9, 0
	beq	cr0,.get_byte_ext
	lbzx	r3, r5, r9
	extsb	r3, r3
	blr


.get_word:
	rlwinm	r12, r3, 17, 23, 29
	lwzx	r9, r12, rBankReadAddr
	rlwinm	r5, r3, 0, 15, 31
	cmpi	cr0, r9, 0
	beq	cr0,.get_word_ext
	lhzx	r3, r5, r9
	extsh	r3, r3
	blr


.get_long:
	rlwinm	r12, r3, 17, 23, 29
	lwzx	r9, r12, rBankReadAddr
	rlwinm	r5, r3, 0, 15, 31
	cmpi	cr0, r9, 0
	beq	cr0,.get_long_ext
	lwzx	r3, r5, r9
	blr


.put_word:
	rlwinm	r12, r3, 17, 23, 29
	lwzx	r9, r12, rBankWritAddr
	rlwinm	r6, r3, 0, 15, 31
	cmpi	cr0, r9, 0
	beq	cr0,.put_word_ext
	sthx	r4, r6, r9
	blr


.put_long:
	rlwinm	r12, r3, 17, 23, 29
	lwzx	r9, r12, rBankWritAddr
	rlwinm	r6, r3, 0, 15, 31
	cmpi	cr0, r9, 0
	beq	cr0,.put_long_ext
	stwx	r4, r6, r9
	blr


.put_byte:
	rlwinm	r12, r3, 17, 23, 29
	lwzx	r9, r12, rBankWritAddr
	rlwinm	r6, r3, 0, 15, 31
	cmpi	cr0, r9, 0
	beq	cr0,.put_byte_ext
	stbx	r4, r6, r9
	blr


	align	4
.GetArgValue:
	cmpi	cr1, rArgKind, 2
	cmpi	cr0, rOpSize, 2
	beq	cr1, .L220
	bgt	cr1, .L232

	mr	r3, rArgAddr
	blt	cr0,.get_byte
	bgt	cr0,.get_long
	b	.get_word

.L220:
	blt	cr0,.L223
	bgt	cr0,.L224

	lha	r3, 2(rArgAddr)
	; extsh	r3, r3
	blr
.L223:
	lbz	r3, 3(rArgAddr)
	extsb	r3, r3
	blr
.L224:
	lwz	r3, 0(rArgAddr)
	blr

.L232:
	mr	r3, rArgAddr
	blr


	align	4
.SetArgValue:
	cmpi	cr1, rArgKind, 2
	cmpi	cr0, rOpSize, 2
	beq	cr1, .L262
	bgt	cr1, .op_illg ; can't store in const

	mr	r4, r3
	mr	r3, rArgAddr
	blt	cr0,.L253
	bgt	cr0,.L257

.L252:
	bl	.put_word
	b	.m68k_NextInstruction
.L253:
	bl	.put_byte
	b	.m68k_NextInstruction
.L257:
	bl	.put_long
	b	.m68k_NextInstruction

.L262:
	blt	cr0,.L265
	bgt	cr0,.L269

	sth	r3, 2(rArgAddr)
	b	.m68k_NextInstruction
.L265:
	stb	r3, 3(rArgAddr)
	b	.m68k_NextInstruction
.L269:
	stw	r3, 0(rArgAddr)
	b	.m68k_NextInstruction


.cctrue:
	addi	r12, rDispatches, x_b2S9993
	rlwinm	r4, r3, 2, 0, 29
	lwzx	r4, r12, r4
	mtctr	r4
	bcctr	20, 0

.L8:
	or	r3, rFlag_c, rFlag_z
	xori	r3, r3, 1
	blr
.L10:
	or	r3, rFlag_c, rFlag_z
	blr
.L12:
	xori	r3, rFlag_c, 1
	blr
.L14:
	mr	r3, rFlag_c
	blr
.L16:
	xori	r3, rFlag_z, 1
	blr
.L18:
	mr	r3, rFlag_z
	blr
.L20:
	xori	r3, rFlag_v, 1
	blr
.L22:
	mr	r3, rFlag_v
	blr
.L24:
	xori	r3, rFlag_n, 1
	blr
.L26:
	mr	r3, rFlag_n
	blr
.L28:
	xor	r3, rFlag_n, rFlag_v
	xori	r3, r3, 1
	blr
.L30:
	xor	r3, rFlag_n, rFlag_v
	blr
.L32:
	xor	r3, rFlag_n, rFlag_v
	or	r3, r3, rFlag_z
	xori	r3, r3, 1
	blr
.L34:
	xor	r3, rFlag_n, rFlag_v
	or	r3, r3, rFlag_z
	blr

.L36:
	addi	r3, 0, 0
	blr
.L49:
	addi	r3, 0, 1
	blr


.m68k_getSR:
	mr	r3, rFlag_c
	lbz	r6, fReg_t1(rRegs)
	lbz	r0, fReg_s(rRegs)
	lwz	r5, fReg_intmask(rRegs)
;	lbz	r4, fReg_m(rRegs)
	rlwimi	r3, rFlag_x, 4, 27, 27
	rlwimi	r3, rFlag_n, 3, 28, 28
	rlwimi	r3, rFlag_z, 2, 29, 29
	rlwimi	r3, rFlag_v, 1, 30, 30
	rlwimi	r3, r6, 15, 16, 16
	rlwimi	r3, r0, 13, 18, 18
	rlwimi	r3, r5, 8, 21, 23
;	rlwimi	r3, r4, 12, 19, 19
	blr


.m68k_setCR:
	rlwinm	rFlag_x, r3, 28, 31, 31
	rlwinm	rFlag_n, r3, 29, 31, 31
	rlwinm	rFlag_z, r3, 30, 31, 31
	rlwinm	rFlag_v, r3, 31, 31, 31
	rlwinm	rFlag_c, r3, 0, 31, 31
	blr


.NeedToGetOut:
	mr	r12, rMaxInstrsToGo
	cmpi	cr0, r12, 0
	beqlr	cr0
	lwz	r0, fReg_MoreInstrs(rRegs)
	addi	r4, 0, 1
	mr	rMaxInstrsToGo, r4
	add	r3, r0, r12
	addi	r3, r3, -1
	stw	r3, fReg_MoreInstrs(rRegs)
	blr


.SetExternalInterruptPending:
	addi	r3, 0, 1
	stb	r3, fReg_ExtIntPend(rRegs)
	b	.NeedToGetOut


.m68k_setSR:
	mflr	r0
	stmw	r31, -24(sp)
	stw	r0, 8(sp)
	stwu	sp, -80(sp)
	rlwinm	r31, r3, 0, 16, 31
	lbz	r5, fReg_s(rRegs)
	rlwinm	r4, r31, 19, 31, 31
	cmp	cr0, r5, r4
	stb	r4, fReg_s(rRegs)
;	rlwinm	r6, r31, 20, 31, 31
;	stb	r6, fReg_m(rRegs)
	beq	cr0,.L67
	lwz	r8, fReg_A7(rRegs)
	cmpi	cr0, r5, 0
	beq	cr0,.L62
	lwz	r0, fReg_usp(rRegs)
	stw	r8, fReg_isp(rRegs)
	stw	r0, fReg_A7(rRegs)
	b	.L67
.L62:
	lwz	r3, fReg_isp(rRegs)
	stw	r8, fReg_usp(rRegs)
	stw	r3, fReg_A7(rRegs)
.L67:
	mr	r3, r31
	bl	.m68k_setCR
	lwz	r12, fReg_intmask(rRegs)
	rlwinm	r6, r31, 24, 29, 31
	stw	r6, fReg_intmask(rRegs)
	cmp	cr0, r6, r12
	beq	cr0,.L71
	bl	.SetExternalInterruptPending
.L71:
	rlwinm	r6, r31, 17, 31, 31
	stb	r6, fReg_t1(rRegs)
	cmpi	cr0, r6, 0
	beq	cr0,.B2LEP9988
;	bne	cr0,.L75
;	addi	r6, 0, 0
;	stb	r6, fReg_TracePend(rRegs)
;	b	.B2LEP9988
;.L75:
	bl	.NeedToGetOut
.B2LEP9988:
	lwz	r0, 88(sp)
	addi	sp, sp, 80
	mtlr	r0
	lmw	r31, -24(sp)
	blr


;.nextibyte:
;	lbz	r3, 3(rPC_p)
;	addi	rPC_p, rPC_p, 2
;	blr


;.nextiword:
;	lhzu	r3, 2(rPC_p)
;	blr


;.nextilong:
;	lwz	r3, 2(rPC_p)
;	addi	rPC_p, rPC_p, 4
;	blr


;.BackupPC:
;	addi	rPC_p, rPC_p, -2
;	blr


.m68k_getpc:
	; must preserve r9
	lwz	r5, fReg_pc_oldp(rRegs)
	lwz	r7, fReg_pc(rRegs)
	subf	r3, r5, rPC_p
	add	r3, r7, r3
	blr


.m68k_setpc:
	stw	r3, fReg_pc(rRegs)

; get_pc_real_address:
	rlwinm	r12, r3, 17, 23, 29
	lwzx	r12, r12, rBankReadAddr
	cmpi	cr0, r12, 0
	beq	cr0,.get_pc_real_address_ext
	rlwinm	r4, r3, 0, 15, 31
	add	rPC_p, r4, r12
.end_get_pc_real_address:

	addi	rPC_p, rPC_p, -2
	stw	rPC_p, fReg_pc_oldp(rRegs)

	blr


.ExceptionTo:
	mflr	r0
	stmw	r28, -16(sp)
	stw	r0, 8(sp)
	stwu	sp, -80(sp)
	mr	r29, r3
	addi	r30, 0, 0
	bl	.m68k_getSR
	lbz	r0, fReg_s(rRegs)
	cmpi	cr0, r0, 0
	lwz	r9, fReg_A7(rRegs)
	mr	r28, r3
	bne	cr0,.L100
	stw	r9, fReg_usp(rRegs)
	addi	r3, 0, 1
	lwz	r9, fReg_isp(rRegs)
	stb	r3, fReg_s(rRegs)
.L100:
	addi	r8, r9, -4
	stw	r8, fReg_A7(rRegs)
	bl	.m68k_getpc
	mr	r4, r3
	mr	r3, r8
	bl	.put_long
	lwz	r3, fReg_A7(rRegs)
	mr	r4, r28
	addi	r3, r3, -2
	stw	r3, fReg_A7(rRegs)
	bl	.put_word
	mr	r3, r29
	bl	.m68k_setpc
;	stb	r30, fReg_m(rRegs)
	stb	r30, fReg_t1(rRegs)
	stb	r30, fReg_TracePend(rRegs)
	lwz	r0, 88(sp)
	addi	sp, sp, 80
	mtlr	r0
	lmw	r28, -16(sp)
	blr


.Exception:
	mflr	r0
	stw	r0, 8(sp)
	stwu	sp, -64(sp)
	rlwinm	r3, r3, 2, 0, 29
	bl	.get_long
	bl	.ExceptionTo
	lwz	r0, 72(sp)
	addi	sp, sp, 64
	mtlr	r0
	blr


.DoCode3:
	addi	rOpSize, 0, 2
	; fall through
.DoMove:
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r28, r3
	rlwinm	r3, rOpCode, 26, 29, 31
	cmpi	cr1, r3, 1
	beq	cr1,.L289a
	rlwinm	r4, rOpCode, 23, 29, 31
	bl	.DecodeModeRegister
	cntlzw	r5, r28
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_n, r28, 1, 31, 31
	rlwinm	rFlag_z, r5, 27, 5, 31
	mr	r3, r28
	b	.SetArgValue
.L289a:
	rlwinm	r4, rOpCode, 23, 29, 31
	rlwinm	rArgAddr, r4, 2, 0, 29
	addi	rArgAddr, rArgAddr, 32
	stwx	r28, rArgAddr, rRegs
	b	.m68k_NextInstruction


.preDoClr:
	bl	.FindOpSizeFromb76
; DoClr
	rlwinm	r3, rOpCode, 29, 29, 31
	cmpi	cr0, r3, 1
	rlwinm	r4, rOpCode, 0, 29, 31
	beq-	cr0,.op_illg
	bl	.DecodeModeRegister
	addi	r3, 0, 0
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	addi	rFlag_n, 0, 0
	addi	rFlag_z, 0, 1
	b	.SetArgValue


.DoBinOpA:
	mr	r28, r6
	mr	r31, r7
	bl	.DecodeModeRegister
	bl	.GetArgValue

	rlwinm	r0, r28, 2, 0, 29
	mr	r30, r3
	add	r4, r0, rRegs
	cmpi	cr0, r31, 0
	lwz	r3, 32(r4)

	bne	cr0,.L299a
	add	r3, r30, r3
	stw	r3, 32(r4)
	b	.m68k_NextInstruction
.L299a:
	subf	r3, r30, r3
	stw	r3, 32(r4)
	b	.m68k_NextInstruction


.DoBinOpStatusCCR:
	; r29=IsStatus, r7 = binop
	lhzu	r30, 2(rPC_p)
		; sign extension would
		; have no effect.
	mr	r31, r7

	bl	.m68k_getSR

	cmpi	cr0, r31, 6
	beq	cr0,.BinOpStatusCCR_Or
	cmpi	cr0, r31, 8
	beq	cr0,.BinOpStatusCCR_Eor

.BinOpStatusCCR_And:
	and	r3, r30, r3
	b	.BinOpStatusCCR_Store
.BinOpStatusCCR_Eor:
	xor	r3, r30, r3
	b	.BinOpStatusCCR_Store
.BinOpStatusCCR_Or:
	or	r3, r30, r3
.BinOpStatusCCR_Store:
	cmpi	cr0, r29, 0
	bne	cr0,.BinOpStatusCCR_SetSR

	bl	.m68k_setCR
	b	.m68k_NextInstruction

.BinOpStatusCCR_SetSR:
	bl	.m68k_setSR
	b	.m68k_NextInstruction


.DoBinOp1:
	cmpi	cr0, r5, 1
	beq	cr0,.op_illg
.DoBinOp1notA:
	mr	r29, r5
	mr	r28, r6
	mr	r31, r7
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r30, r3
	mr	r4, r28
	mr	r3, r29
	bl	.DecodeModeRegister
	bl	.GetArgValue
	addi	r8, 0, 1
	addi	r28, rDispatches, x_b2S9983
	rlwinm	r4, r31, 2, 0, 29
	lwzx	r4, r28, r4
	mtctr	r4
	bcctr	20, 0
.L314:	; BinOpAdd
	cmpi	cr0, rOpSize, 4
	bne	cr0,.L314a

	addco	r3, r3, r30
	mfxer	r6
	cntlzw	r5, r3
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_c, r6, 3, 31, 31
	rlwinm	rFlag_v, r6, 2, 31, 31
	rlwinm	rFlag_z, r5, 27, 5, 31
	mr	rFlag_x, rFlag_c
	b	.SetArgValue

.L314a:
	rlwinm	r11, r3, 1, 31, 31 ; flgo
	add	r3, r3, r30
.L319a:
	cmpi	cr0, rOpSize, 1
	rlwinm	r12, r3, 1, 31, 31 ; z < 0
	bne	cr0,.L319
	extsb	r3, r3
	b	.L324
.L319:
	extsh	r3, r3
.L324:
	cntlzw	r5, r3
	rlwinm	rFlag_z, r5, 27, 5, 31
	rlwinm	rFlag_n, r3, 1, 31, 31
.L324a:
	rlwinm	r10, r30, 1, 31, 31 ; flgs
	xor	rFlag_v, rFlag_n, r12
	xor	rFlag_c, r10, r11
	xor	rFlag_c, rFlag_c, r12
	mr	rFlag_x, rFlag_c
	b	.SetArgValue

.L327:	; BinOpSub
	cmpi	cr0, rOpSize, 4
	bne	cr0,.L327a

	subfco	r3, r30, r3

	mfxer	r6
	cntlzw	r5, r3
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_c, r6, 3, 31, 31
	rlwinm	rFlag_v, r6, 2, 31, 31
	xori	rFlag_c, rFlag_c, 1
	rlwinm	rFlag_z, r5, 27, 5, 31
	mr	rFlag_x, rFlag_c
	b	.SetArgValue

.L327a:
	rlwinm	r11, r3, 1, 31, 31 ; flgo
	subf	r3, r30, r3
	b	.L319a

.L357:	; BinOpAddX
	cmpi	cr0, rOpSize, 4
	bne	cr0,.L357a

	addic 	r5, rFlag_x, -1	; load carry flag
	addeo	r3, r3, r30

	mfxer	r6
	cntlzw	r5, r3
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_c, r6, 3, 31, 31
	rlwinm	r10, r5, 27, 5, 31
	rlwinm	rFlag_v, r6, 2, 31, 31
	mr	rFlag_x, rFlag_c
	and	rFlag_z, rFlag_z, r10
	b	.SetArgValue

.L357a:
	rlwinm	r11, r3, 1, 31, 31 ; flgo
	add	r3, r3, r30
	add	r3, r3, rFlag_x

.L362a:
	cmpi	cr0, rOpSize, 1
	rlwinm	r12, r3, 1, 31, 31 ; z < 0
	bne	cr0,.L362
	extsb	r3, r3
	b	.L367
.L362:
	extsh	r3, r3
.L367:
	cntlzw	r5, r3
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	r5, r5, 27, 5, 31
	and	rFlag_z, rFlag_z, r5
	b	.L324a

.L340:	; BinOpSubX
	cmpi	cr0, rOpSize, 4
	bne	cr0,.L340a

	xori	rFlag_x, rFlag_x, 1
	addic 	r5, rFlag_x, -1	; load carry flag
	subfeo	r3, r30, r3

	mfxer	r6
	cntlzw	r5, r3
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_c, r6, 3, 31, 31
	rlwinm	r10, r5, 27, 5, 31
	xori	rFlag_c, rFlag_c, 1
	rlwinm	rFlag_v, r6, 2, 31, 31
	mr	rFlag_x, rFlag_c
	and	rFlag_z, rFlag_z, r10
	b	.SetArgValue

.L340a:
	rlwinm	r11, r3, 1, 31, 31 ; flgo
	subf	r3, r30, r3
	subf	r3, rFlag_x, r3
	b	.L362a

.L376:	; BinOpOr
	mfcr	r29
	or	r3, r30, r3
	addi	rFlag_c, 0, 0
	cntlzw	r5, r3
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_z, r5, 27, 5, 31
	b	.L701
.L378:	; BinOpEor
	mfcr	r29
	xor	r3, r30, r3
	addi	rFlag_c, 0, 0
	cntlzw	r4, r3
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31
	b	.L701
.L380:	; BinOpASL
	mfcr	r29
	rlwinm.	r8, r30, 0, 26, 31
	mr	r6, r3
	bne	cr0,.L385
	addi	r4, 0, 0
	mr	rFlag_c, r4
	b	.L413
.L385:
	cmpli	cr0, r8, 32
	bgt	cr0,.B2SEL9898
	addi	r5, r8, -1
	slw	r7, r3, r5
	b	.B2SEL9899
.B2SEL9898:
	addi	r7, 0, 0
.B2SEL9899:
	cmpli	cr6, rOpSize, 1
	bne	cr6,.L397
	extsb	r7, r7
	b	.L402
.L397:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L402
	extsh	r7, r7
.L402:
	rlwinm	r4, r7, 1, 31, 31
	mr	rFlag_x, r4
	rlwinm	r3, r7, 1, 0, 30
	mr	rFlag_c, r4
	bne	cr6,.L406
	extsb	r3, r3
	b	.L413
.L406:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L413
	extsh	r3, r3
.L413:
	cmpi	cr6, r3, 0
	bnl	cr6,.B2SEL9896
	neg	r5, r3
	srw	r4, r5, r8
	neg	r9, r4
	b	.B2SEL9897
.B2SEL9896:
	srw	r9, r3, r8
.B2SEL9897:
	mfcr	r4
	subfc	r7, r9, r6
	mfcr	r5
	addic 	r0, r7, -1
	rlwinm	r4, r4, 27, 31, 31
	mr	rFlag_z, r4
	subfe	rFlag_v, r0, r7
	rlwinm	rFlag_n, r5, 25, 31, 31
	b	.L701
.L423:	; BinOpASR
	mfcr	r29
	addi	r4, 0, 0
	rlwinm	r8, r3, 1, 31, 31
	mr	rFlag_n, r8
	rlwinm.	r8, r30, 0, 26, 31
	mr	rFlag_v, r4
	cmpi	cr6, r3, 0
	bne	cr0,.L428
	addi	r5, 0, 0
	mr	rFlag_c, r5
	b	.L455
.L428:
	bnl	cr6,.L433
	nor	r3, r3, r3
.L433:
	cmpi	cr0, rOpSize, 1
	bne	cr0,.L437
	rlwinm	r3, r3, 0, 24, 31
	b	.L442
.L437:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L442
	rlwinm	r3, r3, 0, 16, 31
.L442:
	cmpli	cr0, r8, 32
	bgt	cr0,.B2SEL9894
	addi	r4, r8, -1
	srw	r8, r3, r4
	b	.B2SEL9895
.B2SEL9894:
	addi	r8, 0, 0
.B2SEL9895:
	rlwinm	r9, r8, 0, 31, 31
	mr	rFlag_c, r9
	rlwinm	r3, r8, 31, 1, 31
	bnl	cr6,.L453
	cntlzw	r5, r9
	nor	r3, r3, r3
	rlwinm	r9, r5, 27, 5, 31
	mr	rFlag_c, r9
.L453:
	mr	rFlag_x, r9
	cmpi	cr6, r3, 0
.L455:
	mfcr	r4
	rlwinm	r4, r4, 27, 31, 31
	mr	rFlag_z, r4
	b	.L701
.L458:	; BinOpLSL
	mfcr	r29
	rlwinm.	r8, r30, 0, 26, 31
	bne	cr0,.L463
	addi	rFlag_c, 0, 0
	b	.L491
.L463:
	cmpli	cr0, r8, 32
	bgt	cr0,.B2SEL9892
	addi	r4, r8, -1
	slw	r8, r3, r4
	b	.B2SEL9893
.B2SEL9892:
	addi	r8, 0, 0
.B2SEL9893:
	cmpli	cr6, rOpSize, 1
	bne	cr6,.L475
	extsb	r8, r8
	b	.L480
.L475:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L480
	extsh	r8, r8
.L480:
	rlwinm	rFlag_c, r8, 1, 31, 31
	mr	rFlag_x, rFlag_c
	rlwinm	r3, r8, 1, 0, 30
	bne	cr6,.L484
	extsb	r3, r3
	b	.L491
.L484:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L491
	extsh	r3, r3
.L491:
	addi	rFlag_v, 0, 0
	cntlzw	r4, r3
	rlwinm	rFlag_z, r4, 27, 5, 31
	rlwinm	rFlag_n, r3, 1, 31, 31
	b	.L701
.L494:	; BinOpLSR
	mfcr	r29
	rlwinm.	r8, r30, 0, 26, 31
	bne	cr0,.L499
	addi	rFlag_c, 0, 0
	b	.L518
.L499:
	cmpi	cr0, rOpSize, 1
	bne	cr0,.L504
	rlwinm	r3, r3, 0, 24, 31
	b	.L509
.L504:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L509
	rlwinm	r3, r3, 0, 16, 31
.L509:
	cmpli	cr0, r8, 32
	bgt	cr0,.B2SEL9890
	addi	r4, r8, -1
	srw	r9, r3, r4
	b	.B2SEL9891
.B2SEL9890:
	addi	r9, 0, 0
.B2SEL9891:
	rlwinm	rFlag_c, r9, 0, 31, 31
	mr	rFlag_x, rFlag_c
	rlwinm	r3, r9, 31, 1, 31
.L518:
	addi	rFlag_v, 0, 0
	cntlzw	r4, r3
	rlwinm	rFlag_z, r4, 27, 5, 31
	rlwinm	rFlag_n, r3, 1, 31, 31
	b	.L701
.L521:	; BinOpROL
	mfcr	r29
	rlwinm.	r9, r30, 0, 26, 31
	bne	cr0,.B2LP_9921
	addi	r7, 0, 0
	mr	rFlag_c, r7
	b	.L549
.B2LP_9921:
	mtctr	r9
	cmpli	cr6, rOpSize, 1
	cmpli	cr7, rOpSize, 2
.B2LP_9863:
	cmpi	cr0, r3, 0
	rlwinm	r9, r3, 1, 0, 30
	rlwinm	r8, r3, 1, 31, 31
	mr	r3, r9
	bnl	cr0,.L534
	ori	r3, r9, 1
.L534:
	bne	cr6,.L538
	extsb	r3, r3
	b	.L543
.L538:
	bne	cr7,.L543
	extsh	r3, r3
.L543:
	bdnz	.B2LP_9863
	mr	rFlag_c, r8
.L549:
	addi	rFlag_v, 0, 0
	cntlzw	r5, r3
	rlwinm	rFlag_z, r5, 27, 5, 31
	rlwinm	rFlag_n, r3, 1, 31, 31
	b	.L701
.L552:	; BinOpRXL
	mfcr	r29
	mr	r9, rFlag_x
	rlwinm.	r8, r30, 0, 26, 31
	bne	cr0,.B2LP_9923
	mr	rFlag_c, r9
	b	.L580
.B2LP_9923:
	mtctr	r8
	cmpli	cr6, rOpSize, 1
	cmpli	cr7, rOpSize, 2
.B2LP_9865:
	cmpi	cr0, r9, 0
	mr	r8, r3
	rlwinm	r7, r8, 1, 31, 31
	rlwinm	r3, r8, 1, 0, 30
	beq	cr0,.L565
	ori	r3, r3, 1
.L565:
	bne	cr6,.L569
	extsb	r3, r3
	b	.L574
.L569:
	bne	cr7,.L574
	extsh	r3, r3
.L574:
	rlwinm	r9, r8, 1, 31, 31
	bdnz	.B2LP_9865
	mr	rFlag_c, r7
	mr	rFlag_x, r9
.L580:
	addi	r6, 0, 0
	cntlzw	r4, r3
	mr	rFlag_v, r6
	rlwinm	r4, r4, 27, 5, 31
	mr	rFlag_z, r4
	rlwinm	r5, r3, 1, 31, 31
	mr	rFlag_n, r5
	b	.L701
.L583:	; BinOpROR
	mfcr	r29
	rlwinm.	r7, r30, 0, 26, 31
	bne	cr0,.L588
	addi	r4, 0, 0
	mr	rFlag_c, r4
	b	.L620
.L588:
	cmpli	cr6, rOpSize, 1
	bne	cr6,.L593
	rlwinm	r3, r3, 0, 24, 31
	b	.B2LP_9920
.L593:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.B2LP_9920
	rlwinm	r3, r3, 0, 16, 31
.B2LP_9920:
	mtctr	r7
	rlwinm	r5, rOpSize, 3, 0, 28
	addi	r5, r5, -1
	slw	r8, r8, r5
	b	.B2LP_9862
.B2LP_9878:
	beq	cr0,.B2LP_9862
	or	r3, r8, r3
.B2LP_9862:
	rlwinm	r4, r3, 0, 31, 31
	cmpli	cr0, r4, 0
	addic 	r5, r4, -1
	rlwinm	r3, r3, 31, 1, 31
	subfe	r7, r5, r4
	bdnz	.B2LP_9878
	beq	cr0,.B2L_9875
	or	r3, r8, r3
.B2L_9875:
	mr	rFlag_c, r7
	bne	cr6,.L613
	extsb	r3, r3
	b	.L620
.L613:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L620
	extsh	r3, r3
.L620:
	addi	rFlag_v, 0, 0
	cntlzw	r6, r3
	rlwinm	rFlag_z, r6, 27, 5, 31
	rlwinm	rFlag_n, r3, 1, 31, 31
	b	.L701
.L623:	; BinOpRXR
	mfcr	r29
	rlwinm.	r6, r30, 0, 26, 31
	bne	cr0,.L628
	mr	rFlag_c, rFlag_x
	b	.L660
.L628:
	cmpli	cr6, rOpSize, 1
	bne	cr6,.L633
	rlwinm	r3, r3, 0, 24, 31
	b	.B2LP_9922
.L633:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.B2LP_9922
	rlwinm	r3, r3, 0, 16, 31
.B2LP_9922:
	mtctr	r6
	rlwinm	r4, rOpSize, 3, 0, 28
	mr	r7, rFlag_x
	addi	r4, r4, -1
	slw	r6, r8, r4
	b	.B2LP_9864
.B2LP_9880:
	beq	cr0,.L645
	or	r3, r6, r3
.L645:
	rlwinm.	r5, r8, 0, 31, 31
	addic 	r4, r5, -1
	subfe	r7, r4, r5
.B2LP_9864:
	cmpi	cr0, r7, 0
	mr	r8, r3
	rlwinm	r5, r8, 0, 31, 31
	rlwinm	r3, r8, 31, 1, 31
	addic 	r10, r5, -1
	subfe	r5, r10, r5
	bdnz	.B2LP_9880
	beq	cr0,.B2L_9877
	or	r3, r6, r3
.B2L_9877:
	mr	rFlag_c, r5
	rlwinm.	r4, r8, 0, 31, 31
	addic 	r6, r4, -1
	subfe	rFlag_x, r6, r4
	bne	cr6,.L653
	extsb	r3, r3
	b	.L660
.L653:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L660
	extsh	r3, r3
.L660:
	cntlzw	r7, r3
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_z, r7, 27, 5, 31
	rlwinm	rFlag_n, r3, 1, 31, 31
	b	.L701
.L663:	; BinOpAddBCD
	mfcr	r29
	rlwinm	r6, r30, 0, 28, 31
	cmpi	cr6, r3, 0
	addic 	r11, rFlag_x, -1
	rlwinm	r4, r3, 0, 28, 31
	add	r5, r6, r4
	subfe	r10, r11, rFlag_x
	add	r8, r5, r10
	rlwinm	r7, r3, 0, 24, 27
	rlwinm	r28, r30, 0, 24, 27
	add	r7, r7, r28
	rlwinm	r9, r8, 0, 16, 31
	cmpi	cr0, r9, 10
	blt	cr0,.L668
	addi	r31, r8, 6
	rlwinm	r9, r31, 0, 16, 31
.L668:
	add	r8, r7, r9
	rlwinm	r9, r8, 0, 16, 31
	rlwinm	r4, r9, 0, 23, 27
	cmpi	cr0, r4, 145
	subfic	r5, r4, 144
	rlwinm	rFlag_c, r5, 1, 31, 31
	mr	rFlag_x, rFlag_c
	blt	cr0,.L672
	addi	r6, r8, 96
	rlwinm	r9, r6, 0, 16, 31
.L672:
	extsb.	r3, r9
	mcrf	cr7, cr0
	beq	cr7,.L676
	addi	rFlag_z, 0, 0
.L676:
	mfcr	r9
	rlwinm	r5, r30, 1, 31, 31
	mfcr	r8
	rlwinm	rFlag_n, r9, 29, 31, 31
	rlwinm	r8, r8, 25, 31, 31
	cmp	cr0, r5, r8
	beq	cr0,.L742
	cmp	cr0, rFlag_n, r8
	bne	cr0,.L740
.L742:
	addi	rFlag_v, 0, 0
	b	.L701
.L740:
	addi	rFlag_v, 0, 1
	b	.L701
.L679:	; BinOpSubBCD
	mfcr	r29
	rlwinm	r4, r3, 0, 28, 31
	cmpi	cr6, r3, 0
	addic 	r12, rFlag_x, -1
	rlwinm	r5, r30, 0, 28, 31
	subfe	r11, r12, rFlag_x
	subf	r10, r5, r4
	rlwinm	r31, r3, 0, 24, 27
	subf	r7, r11, r10
	rlwinm	r28, r30, 0, 24, 27
	rlwinm	r8, r7, 0, 16, 31
	cmpi	cr0, r8, 10
	subf	r6, r28, r31
	rlwinm	r9, r6, 0, 16, 31
	blt	cr0,.L684
	addi	r28, r7, -6
	rlwinm	r8, r28, 0, 16, 31
	addi	r3, r6, -16
	rlwinm	r9, r3, 0, 16, 31
.L684:
	rlwinm	r5, r9, 0, 23, 27
	cmpi	cr0, r5, 145
	subfic	r6, r5, 144
	rlwinm	r4, r8, 0, 28, 31
	add	r7, r9, r4
	rlwinm	rFlag_c, r6, 1, 31, 31
	mr	rFlag_x, rFlag_c
	rlwinm	r8, r7, 0, 16, 31
	blt	cr0,.L688
	addi	r10, r7, -96
	rlwinm	r8, r10, 0, 16, 31
.L688:
	extsb.	r3, r8
	mcrf	cr7, cr0
	beq	cr7,.L692
	addi	rFlag_z, 0, 0
.L692:
	mfcr	r9
	rlwinm	r5, r30, 1, 31, 31
	mfcr	r8
	rlwinm	rFlag_n, r9, 29, 31, 31
	rlwinm	r8, r8, 25, 31, 31
	cmp	cr0, r5, r8
	beq	cr0,.L747
	cmp	cr0, rFlag_n, r8
	bne	cr0,.L745
.L747:
	addi	rFlag_v, 0, 0
	b	.L701
.L745:
	addi	rFlag_v, 0, 1
	b	.L701
.L695:
	mtcrf	56, r29
	b	.op_illg
.L374:	; BinOpAnd
	mfcr	r29
	and	r3, r30, r3
	addi	rFlag_c, 0, 0
	cntlzw	r4, r3
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31
.L701:
	mtcrf	56, r29
	b	.SetArgValue


.DoCompare:
	mr	r30, r5
	mr	r29, r6
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r31, r3
	mr	r3, r30
	mr	r4, r29
	bl	.DecodeModeRegister
	bl	.GetArgValue
	cmpi	cr0, rOpSize, 1
	subf	r9, r31, r3
	bne	cr0,.L755
	extsb	r9, r9
	b	.L762
.L755:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L762
	extsh	r9, r9
.L762:
	cntlzw	r5, r9
	rlwinm	rFlag_n, r9, 1, 31, 31
	rlwinm	rFlag_z, r5, 27, 5, 31
	rlwinm	r10, r31, 1, 31, 31 ; flgs
	rlwinm	r11, r3, 1, 31, 31 ; flgo
	xor	r5, r10, r11
	xor	r6, rFlag_n, r11
	and	rFlag_v, r5, r6
	xori	r11, r11, 1
	and	r5, r10, r11
	or	r6, r10, r11
	and	r6, rFlag_n, r6
	or	rFlag_c, r5, r6
	b	.m68k_NextInstruction


.preDoCompareA:
	rlwinm	r3, rOpCode, 25, 30, 30
	addi	rOpSize, r3, 2
	rlwinm	r30, rOpCode, 23, 29, 31
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
; DoCompareA (m2 in r30)
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r28, r3
	rlwinm	r31, r28, 1, 31, 31

	rlwinm	r0, r30, 2, 0, 29
	add	r4, r0, rRegs
	lwz	r3, 32(r4)

	subf	r0, r28, r3
	cntlzw	r10, r0
	cmpi	cr6, r28, 0
	rlwinm	rFlag_n, r0, 1, 31, 31
	rlwinm	rFlag_z, r10, 27, 5, 31
	rlwinm	r9, r3, 1, 31, 31
	cmp	cr0, r31, r9
	beq	cr0,.L792
	cmp	cr0, rFlag_n, r9
	bne	cr0,.L790
.L792:
	addi	rFlag_v, 0, 0
	b	.L791
.L790:
	addi	rFlag_v, 0, 1
.L791:
	bnl	cr6,.L795
	cmpi	cr0, r3, 0
	bnl	cr0,.L793
.L795:
	subf.	r4, r28, r3
	bnl	cr0,.L796
	cmpi	cr0, r3, 0
	bnl	cr0,.L793
	blt	cr6,.L793
.L796:
	addi	rFlag_c, 0, 0
	b	.m68k_NextInstruction
.L793:
	addi	rFlag_c, 0, 1
	b	.m68k_NextInstruction


.preDoBinBitOp1:
	rlwinm	r7, rOpCode, 26, 30, 31
	addi	r7, r7, 17
; DoBinBitOp1 (r2 in opcode)
	cmpi	cr0, r5, 1
	beq	cr0,.op_illg
	addi	rOpSize, 0, 1
	mr	r30, r5
	mr	r29, r7
	bl	.DecodeModeRegister
	bl	.GetArgValue
	cmpli	cr0, r30, 0
	bne	cr0,.L801
	addi	rOpSize, 0, 4
.L801:
	beq	cr0,.B2SEL9854
	rlwinm	r28, r3, 0, 29, 31
	b	.B2SEL9855
.B2SEL9854:
	rlwinm	r28, r3, 0, 27, 31
.B2SEL9855:
	mr	r3, r30
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.DecodeModeRegister
	bl	.GetArgValue
	addi	r30, 0, 1
	slw	r30, r30, r28
	cmpi	cr0, r29, 17
	and	r4, r30, r3
	cntlzw	r5, r4
	rlwinm	rFlag_z, r5, 27, 5, 31
	beq	cr0,.m68k_NextInstruction
	cmpi	cr0, r29, 18
	beq	cr0,.L818
	cmpi	cr0, r29, 19
	beq	cr0,.L820
	cmpi	cr0, r29, 20
	bne-	cr0,.op_illg
	or	r3, r30, r3
	b	.SetArgValue
.L818:
	xor	r3, r30, r3
	b	.SetArgValue
.L820:
	andc	r3, r3, r30
	b	.SetArgValue


.DoUniOp1:
	cmpi	cr0, r3, 1
	mr	r30, r5
	beq-	cr0,.op_illg
	bl	.DecodeModeRegister
	bl	.GetArgValue
	addi	r29, rDispatches, x_b2S9982
	cmpli	cr0, r30, 4
	bgt	cr0,.L915
	rlwinm	r28, r30, 2, 0, 29
	lwzx	r28, r29, r28
	mtctr	r28
	bcctr	20, 0
.L842:
	cmpi	cr7, r3, 0
	addic 	r4, rFlag_x, -1
	cmpi	cr0, rOpSize, 1
	subfe	r0, r4, rFlag_x
	add	r5, r3, r0
	neg	r3, r5
	bne	cr0,.L847
	extsb	r3, r3
	b	.L852
.L847:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L852
	extsh	r3, r3
.L852:
	cmpi	cr6, r3, 0
	beq	cr6,.L856
	addi	rFlag_z, 0, 0
.L856:
	mfcr	r4
	rlwinm	rFlag_n, r4, 25, 31, 31
	bnl	cr7,.L923
	blt	cr6,.L921
.L923:
	addi	rFlag_v, 0, 0
	b	.L922
.L921:
	addi	rFlag_v, 0, 1
.L922:
	blt	cr7,.L924
	blt	cr6,.L924
	addi	r9, 0, 0
	b	.L925
.L924:
	addi	r9, 0, 1
.L925:
	rlwinm	rFlag_c, r9, 0, 24, 31
	mr	rFlag_x, rFlag_c
	b	.L915
.L859:
	mr	r8, r3
	cmpi	cr0, rOpSize, 1
	neg	r3, r3
	bne	cr0,.L864
	extsb	r3, r3
	b	.L869
.L864:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L869
	extsh	r3, r3
.L869:
	cmpi	cr7, r8, 0
	cntlzw	r4, r3
	cmpi	cr6, r3, 0
	rlwinm	rFlag_z, r4, 27, 5, 31
	rlwinm	rFlag_n, r3, 1, 31, 31
	bnl	cr7,.L928
	blt	cr6,.L926
.L928:
	addi	rFlag_v, 0, 0
	b	.L927
.L926:
	addi	rFlag_v, 0, 1
.L927:
	blt	cr7,.L929
	blt	cr6,.L929
	addi	r9, 0, 0
	b	.L930
.L929:
	addi	r9, 0, 1
.L930:
	rlwinm	rFlag_c, r9, 0, 24, 31
	mr	rFlag_x, rFlag_c
	b	.L915
.L872:
	nor	r3, r3, r3
	cmpi	cr0, rOpSize, 1
	bne	cr0,.L877
	extsb	r3, r3
	b	.L882
.L877:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L882
	extsh	r3, r3
.L882:
	cntlzw	r4, r3
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31
	b	.L915
.L885:
	rlwinm	r10, r3, 0, 28, 31
	addic 	r5, rFlag_x, -1
	rlwinm	r6, r3, 0, 24, 27
	subfe	r4, r5, rFlag_x
	add	r7, r10, r4
	neg	r9, r6
	neg	r8, r7
	rlwinm	r9, r9, 0, 16, 31
	rlwinm	r8, r8, 0, 16, 31
	cmpi	cr0, r8, 10
	blt	cr0,.L890
	subfic	r12, r7, -6
	subfic	r3, r6, -16
	rlwinm	r8, r12, 0, 16, 31
	rlwinm	r9, r3, 0, 16, 31
.L890:
	rlwinm	r6, r9, 0, 23, 27
	cmpi	cr0, r6, 145
	subfic	r7, r6, 144
	rlwinm	r4, r8, 0, 28, 31
	add	r8, r9, r4
	rlwinm	rFlag_c, r7, 1, 31, 31
	rlwinm	r3, r8, 0, 16, 31
	mr	rFlag_x, rFlag_c
	blt	cr0,.L894
	addi	r10, r8, -96
	rlwinm	r3, r10, 0, 16, 31
.L894:
	cmpi	cr0, rOpSize, 1
	bne	cr0,.L898
	extsb	r3, r3
	b	.L903
.L898:
	cmpi	cr0, rOpSize, 2
	bne	cr0,.L903
	extsh	r3, r3
.L903:
	cmpi	cr0, r3, 0
	rlwinm	rFlag_n, r3, 1, 31, 31
	beq	cr0,.L915
	addi	rFlag_z, 0, 0
	b	.L915
.L910:
	cntlzw	r4, r3
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31
	ori	r3, r3, 128
.L915:
	b	.SetArgValue


.preDoBinOpMul1:
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	cmpi	cr0, r3, 1
	rlwinm	r31, rOpCode, 24, 31, 31
	beq-	cr0,.op_illg
; DoBinOpMul1 (r2 in opcode, binop in r31)
	addi	rOpSize, 0, 2
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r29, r3
	addi	r3, 0, 0
	rlwinm	r4, rOpCode, 23, 29, 31
	bl	.DecodeModeRegister
	bl	.GetArgValue
	cmpi	cr0, r31, 0
	beq	cr0,.L942
	extsh	r8, r3
	extsh	r4, r29
.L950:
	mullw	r3, r8, r4
	cntlzw	r6, r3
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_z, r6, 27, 5, 31
	rlwinm	rFlag_n, r3, 1, 31, 31
	addi	rOpSize, 0, 4
	b	.SetArgValue
.L942:
	rlwinm	r8, r3, 0, 16, 31
	rlwinm	r4, r29, 0, 16, 31
	b	.L950


.preDoBinOpDiv1:
	; b8 in r8
	addic 	r7, r8, -1
	rlwinm	r3, rOpCode, 29, 29, 31
	subfe	r31, r7, r8
	cmpi	cr0, r3, 1
	rlwinm	r4, rOpCode, 0, 29, 31
	beq-	cr0,.op_illg
; DoBinOpDiv1 (r2 in opcode, binop in r31)
	addi	rOpSize, 0, 2
	bl	.DecodeModeRegister
	bl	.GetArgValue
	addi	rOpSize, 0, 4
	mr	r30, r3
	rlwinm	r4, rOpCode, 23, 29, 31
	addi	r3, 0, 0
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r29, r3
	cmpi	cr0, r30, 0
	beq-	cr0,.L963
	cmpi	cr0, r31, 0
	beq	cr0,.L967
	extsh	r11, r30
	divw	r6, r29, r11
	mullw	r10, r30, r6
	subf	r12, r10, r29
	rlwinm	r7, r12, 0, 16, 31
	mullw	r5, r6, r11
	subf	r5, r5, r29
	rlwinm.	r4, r6, 0, 0, 16
	beq	cr0,.L983
	addis	r31, r4, 1
	addi	r31, r31, -32768
	cmpi	cr0, r31, 0
	beq	cr0,.L983
	addi	rFlag_n, 0, 1
	addi	rFlag_v, 0, 1
	addi	rFlag_c, 0, 0
	b	.L997
.L967:
	rlwinm	r7, r30, 0, 16, 31
	addis	r0, 0, 1
	divwu	r6, r29, r7
	cmpl	cr0, r6, r0
	blt	cr0,.L972
	addi	rFlag_n, 0, 1
	addi	rFlag_v, 0, 1
	addi	rFlag_c, 0, 0
	b	.L997
.L972:
	divwu	r5, r29, r7
	mullw	r8, r7, r5
	subf	r7, r8, r29
	b	.L986
.L983:
	extsh	r3, r5
	rlwinm	r4, r29, 1, 31, 31
	rlwinm	r3, r3, 1, 31, 31
	cmp	cr0, r3, r4
	beq	cr0,.L986
	neg	r8, r5
	rlwinm	r7, r8, 0, 16, 31
.L986:
	extsh	r3, r6
	cntlzw	r4, r3
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31
	mr	r29, r6
	rlwimi	r29, r7, 16, 0, 15
.L997:
	mr	r3, r29
	b	.SetArgValue
.L963:
	addi	r3, 0, 5
	bl	.Exception
	b	.m68k_NextInstruction


.FindOpSizeFromb76:
; must preserve cr1
	rlwinm	r12, rOpCode, 26, 30, 31
	addi	rOpSize, 0, 1
	slw	rOpSize, rOpSize, r12
	blr


;.bitop:
;	rlwinm	r3, rOpCode, 26, 30, 31
;	addi	r3, r3, 17
;	blr


.octdat:
	; result in r4, r3 = 8, preserve r5-r7
	rlwinm	r4, rOpCode, 23, 29, 31
	addi	r3, r4, -1
	rlwimi	r4, r3, 0, 28, 28
	addi	r3, 0, 8
	blr


.DoCode0:
	rlwinm	r3, rOpCode, 24, 31, 31
	cmpi	cr0, r3, 1
	bne	cr0,.L1086
	rlwinm	r5, rOpCode, 29, 29, 31
	cmpi	cr0, r5, 1
	bne	cr0,.L1081
	rlwinm.	r9, rOpCode, 26, 30, 31
	beq	cr0,.L1062
	cmpi	cr0, r9, 1
	beq	cr0,.L1066
	cmpi	cr0, r9, 2
	beq	cr0,.L1070
	cmpi	cr0, r9, 3
	beq	cr0,.L1074
	b	.m68k_NextInstruction
.L1062:
	rlwinm	r4, rOpCode, 2, 27, 29
	add	r3, r4, rRegs
	lwz	r30, 32(r3)
	rlwinm	r28, rOpCode, 25, 27, 29
	lhzu	r3, 2(rPC_p)
	add	r30, r3, r30
	mr	r3, r30
	bl	.get_byte
	rlwinm	r3, r3, 0, 24, 31
	mr	r0, r3
	addi	r3, r30, 2
	rlwinm	r30, r0, 8, 0, 23
	bl	.get_byte
	rlwinm	r3, r3, 0, 24, 31
	lwzx	r0, r28, rRegs
	add	r12, r3, r30
	rlwimi	r0, r12, 0, 16, 31
	stwx	r0, r28, rRegs
	b	.m68k_NextInstruction
.L1066:
	rlwinm	r0, rOpCode, 2, 27, 29
	add	r3, r0, rRegs
	lwz	r30, 32(r3)
	rlwinm	r28, rOpCode, 25, 27, 29
	lhzu	r3, 2(rPC_p)
	add	r30, r3, r30
	mr	r3, r30
	bl	.get_byte
	rlwinm	r3, r3, 0, 24, 31
	mr	r0, r3
	addi	r3, r30, 2
	rlwinm	r29, r0, 24, 0, 7
	bl	.get_byte
	rlwinm	r3, r3, 0, 24, 31
	mr	r0, r3
	addi	r3, r30, 4
	rlwinm	r0, r0, 16, 0, 15
	add	r29, r29, r0
	bl	.get_byte
	rlwinm	r3, r3, 0, 24, 31
	mr	r0, r3
	addi	r3, r30, 6
	rlwinm	r0, r0, 8, 0, 23
	add	r30, r29, r0
	bl	.get_byte
	rlwinm	r3, r3, 0, 24, 31
	add	r0, r30, r3
	stwx	r0, r28, rRegs
	b	.m68k_NextInstruction
.L1070:
	rlwinm	r3, rOpCode, 2, 27, 29
	add	r4, r3, rRegs
	rlwinm	r12, rOpCode, 25, 27, 29
	lwz	r28, 32(r4)
	lwzx	r0, r12, rRegs
	extsh	r31, r0
	srawi	r4, r31, 8
	lhzu	r3, 2(rPC_p)
	add	r28, r3, r28
	mr	r3, r28
	bl	.put_byte
	addi	r3, r28, 2
	mr	r4, r31
	bl	.put_byte
	b	.m68k_NextInstruction
.L1074:
	rlwinm	r3, rOpCode, 2, 27, 29
	add	r5, r3, rRegs
	rlwinm	r12, rOpCode, 25, 27, 29
	lwz	r28, 32(r5)
	lwzx	r31, r12, rRegs
	srawi	r4, r31, 24
	lhzu	r3, 2(rPC_p)
	add	r28, r3, r28
	mr	r3, r28
	bl	.put_byte
	addi	r3, r28, 2
	srawi	r4, r31, 16
	bl	.put_byte
	addi	r3, r28, 4
	srawi	r4, r31, 8
	bl	.put_byte
	addi	r3, r28, 6
	mr	r4, r31
	bl	.put_byte
	b	.m68k_NextInstruction
.L1081:
	addi	r3, 0, 0
	rlwinm	r4, rOpCode, 23, 29, 31
	b	.preDoBinBitOp1
.L1086:
	rlwinm	r9, rOpCode, 23, 29, 31
	cmpi	cr0, r9, 4
	bne	cr0,.L1091
	addi	r3, 0, 7
	addi	r4, 0, 4
	rlwinm	r5, rOpCode, 29, 29, 31
	b	.preDoBinBitOp1
.L1091:
	cmpi	cr0, r9, 6
	bne	cr0,.L1095
	bl	.FindOpSizeFromb76
	addi	r3, 0, 7
	rlwinm	r5, rOpCode, 29, 29, 31
	addi	r4, 0, 4
	cmpi	cr0, r5, 1
	rlwinm	r6, rOpCode, 0, 29, 31
	beq-	cr0,.op_illg
	b	.DoCompare
.L1095:
	cmpi	cr0, r9, 7
	beq-	cr0,.op_illg ; MOVES, not for 68000

	bl	.FindOpSizeFromb76
	addi	r6, rDispatches, x_b2S9980
	rlwinm	r5, rOpCode, 23, 29, 31
	cmpli	cr1, r5, 5
	bgt	cr1,.L1133
	rlwinm	r7, r5, 2, 0, 29
	lwzx	r7, r6, r7
	mtctr	r7
	bcctr	20, 0
.L1124:
	addi	r7, 0, 6
	b	.L1136
.L1126:
	addi	r7, 0, 7
	b	.L1136
.L1128:
	addi	r7, 0, 1
	b	.L1136
.L1130:
	addi	r7, 0, 0
	b	.L1136
.L1133:
	addi	r7, 0, 8
.L1136:
	rlwinm.	r9, rOpCode, 0, 21, 21
	rlwinm	r5, rOpCode, 29, 29, 31
	rlwinm	r6, rOpCode, 0, 29, 31
	bne	cr0,.L1154
	cmpi	cr0, r5, 7
	bne	cr0,.L1154
	cmpi	cr0, r6, 4
	bne	cr0,.L1154
	rlwinm.	r3, rOpCode, 26, 30, 31
	bne	cr0,.L1142
	addi	r29, 0, 0
	b	.DoBinOpStatusCCR
.L1142:
	lbz	r28, fReg_s(rRegs)
	cmpi	cr0, r28, 0
	bne	cr0,.L1147
	addi	r3, 0, 8
	addi	rPC_p, rPC_p, -2
	bl	.Exception
	b	.m68k_NextInstruction
.L1147:
	addi	r29, 0, 1
	b	.DoBinOpStatusCCR
.L1154:
	addi	r3, 0, 7
	addi	r4, 0, 4
	b	.DoBinOp1


.DoCode1:
	addi	rOpSize, 0, 1
	b	.DoMove


.DoCode2:
	addi	rOpSize, 0, 4
	b	.DoMove


.preDoCheck:
	; b76 in r8
	rlwinm	r3, rOpCode, 29, 29, 31
	subfic	rOpSize, r8, 4
	cmpi	cr0, r3, 1
	rlwinm	r4, rOpCode, 0, 29, 31
	beq-	cr0,.op_illg
; DoCheck (r2 in opcode)
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r29, r3
	addi	r3, 0, 0
	rlwinm	r4, rOpCode, 23, 29, 31
	bl	.DecodeModeRegister
	bl	.GetArgValue
	cmpi	cr0, r3, 0
	bnl+	cr0,.L1175
	addi	rFlag_n, 0, 1
	b	.B2LEP9821
.L1175:
	cmp	cr0, r3, r29
	bng+	cr0,.m68k_NextInstruction
	addi	rFlag_n, 0, 0
.B2LEP9821:
	addi	r3, 0, 6
	bl	.Exception
	b	.m68k_NextInstruction


.preDoLea:
	addi	rOpSize, 0, 4
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
; .DoLea:
	bl	.DecodeModeRegister
	rlwinm	r28, rOpCode, 25, 27, 29
	cmpi	cr0, rArgKind, 0
	add	r28, r28, rRegs
	bne-	cr0,.m68k_NextInstruction
	stw	rArgAddr, 32(r28)
	b	.m68k_NextInstruction


.preDoTest1:
	bl	.FindOpSizeFromb76
	rlwinm	r3, rOpCode, 29, 29, 31
	cmpi	cr0, r3, 1
	rlwinm	r4, rOpCode, 0, 29, 31
	beq-	cr0,.op_illg

; DoTest1
	bl	.DecodeModeRegister
	bl	.GetArgValue
	cntlzw	r0, r3
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_z, r0, 27, 5, 31
	rlwinm	rFlag_n, r3, 1, 31, 31
	b	.m68k_NextInstruction


.pre_reglist:
	rlwinm	r8, rOpCode, 27, 29, 30
	addi	rOpSize, r8, -2
; .reglist:
	rlwinm	r4, rOpCode, 29, 29, 31
	rlwinm	r5, rOpCode, 0, 29, 31
	extsh	r30, r3
	cmpi	cr0, r4, 3
	lhzu	r3, 2(rPC_p)
	mr	r31, r3
	beq	cr0,.L1193
	cmpi	cr0, r4, 4
	beq	cr0,.L1230
	b	.L1267
.L1193:
	cmpi	cr0, r30, 1
	addi	r28, 0, 1
	bne	cr0,.m68k_NextInstruction
	rlwinm	r6, r5, 2, 0, 29
	add	r12, r6, rRegs
	lwz	r9, 32(r12)
	stw	r12, fReg_RgLstTmp(rRegs)
	cmpi	cr0, rOpSize, 2
	mr	r29, r9
	bne	cr0,.L1211
	addi	r30, 0, 0
.B2LP_9718:
	slw	r8, r28, r30
	and.	r3, r31, r8
	beq	cr0,.L1205
	mr	r3, r29
	bl	.get_word
	rlwinm	r12, r30, 2, 0, 29
	addi	r29, r29, 2
	stwx	r3, r12, rRegs
	mr	r9, r29
.L1205:
	addi	r0, r30, 1
	extsh	r30, r0
	cmpi	cr0, r30, 16
	blt	cr0,.B2LP_9718
	b	.L1226
.L1211:
	addi	r30, 0, 0
.B2LP_9719:
	slw	r3, r28, r30
	and.	r4, r31, r3
	beq	cr0,.L1220
	mr	r3, r29
	bl	.get_long
	rlwinm	r12, r30, 2, 0, 29
	stwx	r3, r12, rRegs
	addi	r29, r29, 4
	mr	r9, r29
.L1220:
	addi	r0, r30, 1
	extsh	r30, r0
	cmpi	cr0, r30, 16
	blt	cr0,.B2LP_9719
.L1226:
	lwz	r12, fReg_RgLstTmp(rRegs)
	stw	r9, 32(r12)
	b	.m68k_NextInstruction
.L1230:
	cmpi	cr0, r30, 0
	bne	cr0,.m68k_NextInstruction
	rlwinm	r3, r5, 2, 0, 29
	add	r12, r3, rRegs
	lwz	r9, 32(r12)
	stw	r12, fReg_RgLstTmp(rRegs)
	cmpi	cr0, rOpSize, 2
	addi	r28, 0, 1
	bne	cr0,.L1248
	addi	r30, 0, 15
.B2LP_9716:
	subfic	r8, r30, 15
	slw	r3, r28, r8
	and.	r4, r31, r3
	beq	cr0,.L1242
	addi	r3, r9, -2
	rlwinm	r5, r30, 2, 0, 29
	mr	r29, r3
	lwzx	r4, r5, rRegs
	bl	.put_word
	mr	r9, r29
.L1242:
	addi	r0, r30, -1
	extsh.	r30, r0
	bnl	cr0,.B2LP_9716
	b	.L1263
.L1248:
	addi	r30, 0, 15
.B2LP_9717:
	subfic	r3, r30, 15
	slw	r4, r28, r3
	and.	r5, r31, r4
	beq	cr0,.L1257
	addi	r3, r9, -4
	rlwinm	r6, r30, 2, 0, 29
	mr	r29, r3
	lwzx	r4, r6, rRegs
	bl	.put_long
	mr	r9, r29
.L1257:
	addi	r0, r30, -1
	extsh.	r30, r0
	bnl	cr0,.B2LP_9717
.L1263:
	lwz	r12, fReg_RgLstTmp(rRegs)
	stw	r9, 32(r12)
	b	.m68k_NextInstruction
.L1267:
	mr	r3, r4
	mr	r4, r5
	bl	.DecodeModeRegister
	cmpi	cr0, rArgKind, 0
	bne-	cr0,.m68k_NextInstruction
	mr	r29, rArgAddr
	cmpi	cr0, r30, 0
	cmpli	cr6, rOpSize, 2
	addi	r28, 0, 1
	bne	cr0,.L1304
	bne	cr6,.L1287
	addi	r30, 0, 0
.B2LP_9712:
	slw	r3, r28, r30
	and.	r4, r31, r3
	beq	cr0,.L1281
	mr	r3, r29
	rlwinm	r5, r30, 2, 0, 29
	lwzx	r4, r5, rRegs
	bl	.put_word
	addi	r29, r29, 2
.L1281:
	addi	r0, r30, 1
	extsh	r30, r0
	cmpi	cr0, r30, 16
	blt	cr0,.B2LP_9712
	b	.m68k_NextInstruction
.L1287:
	addi	r30, 0, 0
.B2LP_9713:
	slw	r3, r28, r30
	and.	r4, r31, r3
	beq	cr0,.L1296
	mr	r3, r29
	rlwinm	r5, r30, 2, 0, 29
	lwzx	r4, r5, rRegs
	bl	.put_long
	addi	r29, r29, 4
.L1296:
	addi	r0, r30, 1
	extsh	r30, r0
	cmpi	cr0, r30, 16
	blt	cr0,.B2LP_9713
	b	.m68k_NextInstruction
.L1304:
	bne	cr6,.L1321
	addi	r30, 0, 0
.B2LP_9714:
	slw	r3, r28, r30
	and.	r4, r31, r3
	beq	cr0,.L1315
	mr	r3, r29
	bl	.get_word
	rlwinm	r12, r30, 2, 0, 29
	addi	r29, r29, 2
	stwx	r3, r12, rRegs
.L1315:
	addi	r0, r30, 1
	extsh	r30, r0
	cmpi	cr0, r30, 16
	blt	cr0,.B2LP_9714
	b	.m68k_NextInstruction
.L1321:
	addi	r30, 0, 0
.B2LP_9715:
	slw	r3, r28, r30
	and.	r4, r31, r3
	beq	cr0,.L1330
	mr	r3, r29
	bl	.get_long
	rlwinm	r12, r30, 2, 0, 29
	stwx	r3, r12, rRegs
	addi	r29, r29, 4
.L1330:
	addi	r0, r30, 1
	extsh	r30, r0
	cmpi	cr0, r30, 16
	blt	cr0,.B2LP_9715
	b	.m68k_NextInstruction


.m68k_setstopped:
	addi	r3, 0, 4
	bl	.Exception
	b	.m68k_NextInstruction


.DoCode4:
	addi	r6, 0, 1
	rlwinm.	r0, rOpCode, 24, 31, 31
	beq	cr0,.L1360
	rlwinm.	r8, rOpCode, 26, 30, 31
	beq	cr0,.preDoCheck
	cmpi	cr0, r8, 1
	beq-	cr0,.op_illg
	cmpi	cr0, r8, 2
	beq	cr0,.preDoCheck
	cmpi	cr0, r8, 3
	beq	cr0,.preDoLea
	b	.m68k_NextInstruction
.L1360:
	addi	r3, rDispatches, x_b2S9979
	rlwinm	r30, rOpCode, 23, 29, 31
	cmpli	cr0, r30, 7
	bgt	cr0,.m68k_NextInstruction
	rlwinm	r12, r30, 2, 0, 29
	lwzx	r12, r3, r12
	mtctr	r12
	bcctr	20, 0
.L1364:
	rlwinm	r0, rOpCode, 26, 30, 31
	cmpi	cr0, r0, 3
	beq	cr0,.L1368
	bl	.FindOpSizeFromb76
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	addi	r5, 0, 2
	b	.DoUniOp1
.L1368:
	addi	rOpSize, 0, 2
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.DecodeModeRegister
	bl	.m68k_getSR
	b	.SetArgValue
.L1373:
	rlwinm	r0, rOpCode, 26, 30, 31
	cmpi	cr0, r0, 3
	bne	cr0,.preDoClr
.L1377:
	addi	rOpSize, 0, 2
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.DecodeModeRegister
	bl	.m68k_getSR
	rlwinm	r3, r3, 0, 24, 31
	b	.SetArgValue
.L1382:
	rlwinm	r31, rOpCode, 26, 30, 31
	cmpi	cr0, r31, 3
	beq	cr0,.L1386
	bl	.FindOpSizeFromb76
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	addi	r5, 0, 1
	b	.DoUniOp1
.L1386:
	addi	rOpSize, 0, 2
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.DecodeModeRegister
	bl	.GetArgValue
	rlwinm	r3, r3, 0, 16, 31
	bl	.m68k_setCR
	b	.m68k_NextInstruction
.L1391:
	rlwinm	r0, rOpCode, 26, 30, 31
	cmpi	cr0, r0, 3
	beq	cr0,.L1395
	bl	.FindOpSizeFromb76
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	addi	r5, 0, 0
	b	.DoUniOp1
.L1395:
	addi	rOpSize, 0, 2
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.DecodeModeRegister
	bl	.GetArgValue
	rlwinm	r3, r3, 0, 16, 31
	bl	.m68k_setSR
	b	.m68k_NextInstruction
.L1400:
	rlwinm.	r8, rOpCode, 26, 30, 31
	beq	cr0,.L1403
	cmpi	cr0, r8, 1
	beq	cr0,.L1405
	cmpi	cr0, r8, 2
	beq	cr0,.L1419
	cmpi	cr0, r8, 3
	beq	cr0,.L1419
	b	.m68k_NextInstruction
.L1403:
	mr	rOpSize, r6
	rlwinm	r3, rOpCode, 29, 29, 31
	addi	r5, 0, 3
	rlwinm	r4, rOpCode, 0, 29, 31
	b	.DoUniOp1
.L1405:
	rlwinm.	r3, rOpCode, 29, 29, 31
	bne	cr0,.L1409
	addi	r0, 0, 0
	rlwinm	r3, rOpCode, 2, 27, 29
	lwzx	r6, r3, rRegs
	mr	rFlag_c, r0
	rlwinm	r7, r6, 16, 0, 31
	mr	rFlag_v, r0
	cntlzw	r8, r7
	stwx	r7, r3, rRegs
	rlwinm	r8, r8, 27, 5, 31
	mr	rFlag_z, r8
	rlwinm	r12, r6, 17, 31, 31
	mr	rFlag_n, r12
	b	.m68k_NextInstruction
.L1409:
	addi	rOpSize, 0, 4
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.DecodeModeRegister
	cmpi	cr0, rArgKind, 0
	bne-	cr0,.m68k_NextInstruction
	lwz	r3, fReg_A7(rRegs)
	mr	r4, rArgAddr
	addi	r3, r3, -4
	stw	r3, fReg_A7(rRegs)
	bl	.put_long
	b	.m68k_NextInstruction
.L1419:
	rlwinm.	r3, rOpCode, 29, 29, 31
	bne	cr0,.L1430
	addi	r0, 0, 0
	rlwinm	r7, rOpCode, 2, 27, 29
	mr	rFlag_c, r0
	cmpi	cr0, r8, 2
	mr	rFlag_v, r0
	add	r6, r7, rRegs
	lwzx	r9, r7, rRegs
	bne	cr0,.L1425
	extsb	r8, r9
	mr	r3, r9
	cntlzw	r5, r8
	rlwinm	r6, r8, 1, 31, 31
	mr	rFlag_n, r6
	rlwinm	r5, r5, 27, 5, 31
	mr	rFlag_z, r5
	rlwimi	r3, r8, 0, 16, 31
	stwx	r3, r7, rRegs
	b	.m68k_NextInstruction
.L1425:
	extsh	r10, r9
	stw	r10, 0(r6)
	cntlzw	r3, r10
	rlwinm	r5, r10, 1, 31, 31
	mr	rFlag_n, r5
	rlwinm	r3, r3, 27, 5, 31
	mr	rFlag_z, r3
	b	.m68k_NextInstruction
.L1430:
	addi	r3, 0, 0
	b	.pre_reglist
.L1438:
	rlwinm	r31, rOpCode, 26, 30, 31
	cmpi	cr0, r31, 3
	bne	cr0,.preDoTest1
	rlwinm	r3, rOpCode, 29, 29, 31
	cmpi	cr0, r3, 7
	rlwinm	r4, rOpCode, 0, 29, 31
	bne	cr0,.L1447
	cmpi	cr0, r4, 0
	bne-	cr0,.op_illg
.L1447:
	mr	rOpSize, r6
	addi	r5, 0, 4
	b	.DoUniOp1
.L1454:
	rlwinm	r31, rOpCode, 25, 31, 31
	cmpi	cr0, r31, 1
	bne-	cr0,.op_illg
	addi	r3, 0, 1
	b	.pre_reglist
.L1463:
	rlwinm.	r8, rOpCode, 26, 30, 31
	beq-	cr0,.op_illg
	cmpi	cr0, r8, 1
	beq	cr0,.L1468
	cmpi	cr0, r8, 2
	beq	cr0,.L1561
	cmpi	cr0, r8, 3
	beq	cr0,.L1569
	b	.m68k_NextInstruction
.L1468:
	addi	r3, rDispatches, x_b2S9978
	rlwinm	r30, rOpCode, 29, 29, 31
	cmpli	cr0, r30, 7
	bgt	cr0,.m68k_NextInstruction
	rlwinm	r4, r30, 2, 0, 29
	lwzx	r4, r3, r4
	mtctr	r4
	bcctr	20, 0
.L1472:
	rlwinm	r3, rOpCode, 0, 28, 31
	addi	r3, r3, 32
	bl	.Exception
	b	.m68k_NextInstruction
.L1474:
	lwz	r3, fReg_A7(rRegs)
	rlwinm	r0, rOpCode, 2, 27, 29
	addi	r3, r3, -4
	stw	r3, fReg_A7(rRegs)
	add	r30, r0, rRegs
	mr	r29, r3
	lwz	r4, 32(r30)
	bl	.put_long
	stw	r29, 32(r30)
	lwz	r30, fReg_A7(rRegs)
	lhzu	r3, 2(rPC_p)
	extsh	r3, r3
	add	r0, r30, r3
	stw	r0, fReg_A7(rRegs)
	b	.m68k_NextInstruction
.L1478:
	rlwinm	r0, rOpCode, 0, 29, 31
	cmpi	cr0, r0, 7
	beq	cr0,.L1483
	rlwinm	r4, rOpCode, 2, 27, 29
	add	r30, r4, rRegs
	lwz	r3, 32(r30)
	addi	r29, r3, 4
	bl	.get_long
	stw	r3, 32(r30)
	stw	r29, fReg_A7(rRegs)
	b	.m68k_NextInstruction
.L1483:
	lwz	r3, fReg_A7(rRegs)
	bl	.get_long
	addi	r3, r3, 4
	stw	r3, fReg_A7(rRegs)
	b	.m68k_NextInstruction
.L1489:
	lbz	r0, fReg_s(rRegs)
	cmpi	cr0, r0, 0
	bne	cr0,.L1493
	addi	r3, 0, 8
	addi	rPC_p, rPC_p, -2
	bl	.Exception
	b	.m68k_NextInstruction
.L1493:
	rlwinm	r30, rOpCode, 2, 27, 29
	add	r3, r30, rRegs
	lwz	r4, 32(r3)
	stw	r4, fReg_usp(rRegs)
	b	.m68k_NextInstruction
.L1498:
	lbz	r5, fReg_s(rRegs)
	cmpi	cr0, r5, 0
	bne	cr0,.L1502
	addi	r3, 0, 8
	addi	rPC_p, rPC_p, -2
	bl	.Exception
	b	.m68k_NextInstruction
.L1502:
	lwz	r4, fReg_usp(rRegs)
	rlwinm	r30, rOpCode, 2, 27, 29
	add	r3, r30, rRegs
	stw	r4, 32(r3)
	b	.m68k_NextInstruction
.L1507:
	addi	rOpSize, 0, 0
	rlwinm	r3, rOpCode, 0, 29, 31
	cmpli	cr0, r3, 7
	addi	r4, rDispatches, x_b2S9977
	bgt	cr0,.m68k_NextInstruction
	rlwinm	r8, r3, 2, 0, 29
	lwzx	r8, r4, r8
	mtctr	r8
	bcctr	20, 0
.L1510:
	lbz	r10, fReg_s(rRegs)
	cmpi	cr0, r10, 0
	bne	cr0,.L1514
	addi	r3, 0, 8
	addi	rPC_p, rPC_p, -2
	bl	.Exception
	b	.m68k_NextInstruction
.L1514:
	bl	.MyEmulatorExit
	bl	.customreset[PR]
	Nop
	bl	.MyEmulatorEntry
	b	.m68k_NextInstruction
.L1521:
	lbz	r30, fReg_s(rRegs)
	cmpi	cr0, r30, 0
	bne	cr0,.L1525
	addi	r3, 0, 8
	addi	rPC_p, rPC_p, -2
	bl	.Exception
	b	.m68k_NextInstruction
.L1525:
	lhzu	r3, 2(rPC_p)
	rlwinm	r3, r3, 0, 16, 31
	bl	.m68k_setSR
	b	.m68k_setstopped
.L1530:
	lbz	r30, fReg_s(rRegs)
	cmpi	cr0, r30, 0
	bne	cr0,.L1534
	addi	r3, 0, 8
	addi	rPC_p, rPC_p, -2
	bl	.Exception
	b	.m68k_NextInstruction
.L1534:
	lwz	r3, fReg_A7(rRegs)
	mr	r30, r3
	bl	.get_word
	rlwinm	r3, r3, 0, 16, 31
	bl	.m68k_setSR
	addi	r3, r30, 2
	addi	r30, r30, 6
	bl	.get_long
	bl	.m68k_setpc
	stw	r30, fReg_A7(rRegs)
	b	.m68k_NextInstruction
.L1541:
	lwz	r3, fReg_A7(rRegs)
	bl	.get_long
	bl	.m68k_setpc
	lwz	r12, fReg_A7(rRegs)
	addi	r0, r12, 4
	stw	r0, fReg_A7(rRegs)
	b	.m68k_NextInstruction
.L1543:
	mr	r30, rFlag_v
	cmpi	cr0, r30, 0
	beq	cr0,.m68k_NextInstruction
	addi	r3, 0, 7
	bl	.Exception
	b	.m68k_NextInstruction
.L1549:
	lwz	r3, fReg_A7(rRegs)
	mr	r30, r3
	bl	.get_word
	rlwinm	r3, r3, 0, 16, 31
	bl	.m68k_setCR
	addi	r3, r30, 2
	addi	r30, r30, 6
	bl	.get_long
	bl	.m68k_setpc
	stw	r30, fReg_A7(rRegs)
	b	.m68k_NextInstruction
.L1561:
	addi	rOpSize, 0, 0
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.DecodeModeRegister
	cmpi	cr0, rArgKind, 0
	bne-	cr0,.m68k_NextInstruction
	mr	r29, rArgAddr
	lwz	r3, fReg_A7(rRegs)
	addi	r30, r3, -4
	stw	r30, fReg_A7(rRegs)
	bl	.m68k_getpc
	mr	r4, r3
	mr	r3, r30
	bl	.put_long
	mr	r3, r29
	bl	.m68k_setpc
	b	.m68k_NextInstruction
.L1569:
	addi	rOpSize, 0, 0
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.DecodeModeRegister
	cmpi	cr0, rArgKind, 0
	bne-	cr0,.m68k_NextInstruction
	mr	r3, rArgAddr
	bl	.m68k_setpc
	b	.m68k_NextInstruction


.DoCode5:
	rlwinm	r0, rOpCode, 26, 30, 31
	rlwinm	r3, rOpCode, 29, 29, 31
	cmpi	cr0, r0, 3
	cmpi	cr1, r3, 1
	bne	cr0,.L1604
	rlwinm	r30, rOpCode, 24, 28, 31
	bne	cr1,.L1599

	; bl	.m68k_getpc
	mr	r3, rPC_p
	lhzu	r8, 2(rPC_p)
	extsh	r8, r8
	add	r29, r8, r3

	mr	r3, r30
	bl	.cctrue
	cmpi	cr1, r3, 0
	bne	cr1,.m68k_NextInstruction
	rlwinm	r5, rOpCode, 2, 27, 29
	lwzx	r3, r5, rRegs
	extsh	r31, r3
	addi	r5, r5, 2
	cmpi	cr0, r31, 0
	addi	r3, r31, -1
	sthx	r3, r5, rRegs
	beq	cr0,.m68k_NextInstruction
	; mr	r3, r29
	; bl	.m68k_setpc
	mr	rPC_p, r29
	b	.m68k_NextInstruction

.L1599:
	addi	rOpSize, 0, 1
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.DecodeModeRegister
	mr	r3, r30
	bl	.cctrue
	mr	r0, r3
	addic 	r12, r0, -1
	subfe	r3, r12, r0
	neg	r3, r3
	rlwinm	r3, r3, 0, 24, 31
	b	.SetArgValue
.L1604:
	bl	.FindOpSizeFromb76
	rlwinm	r7, rOpCode, 24, 31, 31
	bl	.octdat
	rlwinm	r6, rOpCode, 0, 29, 31
	beq	cr1,.DoBinOpA
	rlwinm	r5, rOpCode, 29, 29, 31
	b	.DoBinOp1notA


.DoCode6:
	rlwinm.	r9, rOpCode, 0, 24, 31
;	bl	.m68k_getpc
;	mr	r8, r3
	mr	r8, rPC_p
	bne+	cr0,.L1622
	lhzu	r3, 2(rPC_p)
	extsh	r3, r3
	b	.L1630
.L1622:
; not on 68000
;	cmpi	cr0, r9, 255
;	bne	cr0,.L1626
;	bl	.nextilong
;	b	.L1630
;.L1626:
	extsb	r3, r9
.L1630:
	add	r31, r8, r3
	rlwinm	r3, rOpCode, 24, 28, 31
	cmpi	cr0, r3, 1
	bne	cr0,.L1634
	lwz	r3, fReg_A7(rRegs)
	addi	r8, r3, -4
	stw	r8, fReg_A7(rRegs)
	bl	.m68k_getpc
	mr	r4, r3
	mr	r3, r8
	bl	.put_long
;	mr	r3, r31
;	bl	.m68k_setpc
	mr	rPC_p, r31
	b	.m68k_NextInstruction
.L1634:
	bl	.cctrue
	cmpi	cr1, r3, 0
	beq	cr1,.m68k_NextInstruction
;	mr	r3, r31
;	bl	.m68k_setpc
	mr	rPC_p, r31
	b	.m68k_NextInstruction


.DoCode7:
	addi	r3, 0, 0
	rlwinm	r7, rOpCode, 0, 24, 31
	rlwinm	r11, rOpCode, 25, 27, 29
	extsb	r7, r7
	cntlzw	r8, r7
	mr	rFlag_c, r3
	rlwinm	r12, r7, 1, 31, 31
	mr	rFlag_v, r3
	rlwinm	r8, r8, 27, 5, 31
	mr	rFlag_z, r8
	mr	rFlag_n, r12
	stwx	r7, r11, rRegs
	b	.m68k_NextInstruction


.DoCode8:
	rlwinm	r0, rOpCode, 26, 30, 31
	cmpi	cr0, r0, 3
	rlwinm	r8, rOpCode, 24, 31, 31
	beq	cr0,.preDoBinOpDiv1
	cmpi	cr0, r8, 1
	bne	cr0,.L1668
	rlwinm	r3, rOpCode, 29, 29, 31
	cmpli	cr0, r3, 1
	bgt	cr0,.L1668
	addi	rOpSize, 0, 1
	cmpi	cr0, r3, 0
	rlwinm	r6, rOpCode, 23, 29, 31
	rlwinm	r8, rOpCode, 0, 29, 31
	bne	cr0,.L1663
	addi	r3, 0, 0
	mr	r4, r8
	addi	r5, 0, 0
	addi	r7, 0, 5
	b	.DoBinOp1notA
.L1663:
	addi	r3, 0, 4
	mr	r4, r8
	addi	r5, 0, 4
	addi	r7, 0, 5
	b	.DoBinOp1notA
.L1668:
	bl	.FindOpSizeFromb76
	rlwinm	r5, rOpCode, 24, 31, 31
	cmpi	cr1, r5, 1
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r8, rOpCode, 0, 29, 31
	rlwinm	r6, rOpCode, 23, 29, 31
	bne	cr1,.L1673
	addi	r7, 0, 6
	mr	r4, r6
	mr	r5, r3
	addi	r3, 0, 0
	mr	r6, r8
	b	.DoBinOp1notA
.L1673:
	addi	r5, 0, 0
	mr	r4, r8
	addi	r7, 0, 6
	b	.DoBinOp1notA


.DoCode9:
	rlwinm	r0, rOpCode, 26, 30, 31
	cmpi	cr0, r0, 3
	bne	cr0,.L1686
	rlwinm	r3, rOpCode, 25, 30, 30
	addi	rOpSize, r3, 2
	rlwinm	r6, rOpCode, 23, 29, 31
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	addi	r7, 0, 1
	b	.DoBinOpA
.L1686:
	bl	.FindOpSizeFromb76
	rlwinm.	r7, rOpCode, 24, 31, 31
	rlwinm	r5, rOpCode, 29, 29, 31
	rlwinm	r9, rOpCode, 0, 29, 31
	rlwinm	r6, rOpCode, 23, 29, 31
	bne	cr0,.L1691
	addi	r7, 0, 1
	mr	r3, r5
	addi	r5, 0, 0
	mr	r4, r9
	b	.DoBinOp1
.L1691:
	cmpli	cr0, r5, 2
	blt	cr0,.L1695
	addi	r3, 0, 0
	mr	r4, r6
	addi	r7, 0, 1
	mr	r6, r9
	b	.DoBinOp1notA
.L1695:
	cmpi	cr0, r5, 0
	bne	cr0,.L1699
	addi	r3, 0, 0
	mr	r4, r9
	addi	r5, 0, 0
	addi	r7, 0, 3
	b	.DoBinOp1notA
.L1699:
	addi	r3, 0, 4
	mr	r4, r9
	addi	r5, 0, 4
	addi	r7, 0, 3
	b	.DoBinOp1notA


.DoCodeA:
	addi	r3, 0, 10
	addi	rPC_p, rPC_p, -2
	bl	.Exception
	b	.m68k_NextInstruction


.DoCodeB:
	rlwinm	r0, rOpCode, 26, 30, 31
	cmpi	cr0, r0, 3
	beq	cr0,.preDoCompareA

	rlwinm	r0, rOpCode, 24, 31, 31
	cmpi	cr0, r0, 1
	bne	cr0,.L1722
	rlwinm	r3, rOpCode, 29, 29, 31
	cmpi	cr0, r3, 1
	bne	cr0,.L1718
	bl	.FindOpSizeFromb76
	addi	r3, 0, 3
	rlwinm	r4, rOpCode, 0, 29, 31
	rlwinm	r6, rOpCode, 23, 29, 31
	addi	r5, 0, 3
	b	.DoCompare
.L1718:
	addi	r7, 0, 8
	bl	.FindOpSizeFromb76
	rlwinm	r4, rOpCode, 23, 29, 31
	addi	r3, 0, 0
	rlwinm	r5, rOpCode, 29, 29, 31
	rlwinm	r6, rOpCode, 0, 29, 31
	b	.DoBinOp1notA
.L1722:
	addi	r5, 0, 0
	bl	.FindOpSizeFromb76
	rlwinm	r6, rOpCode, 23, 29, 31
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	b	.DoCompare


.DoCodeC:
	addi	r6, 0, 1
	rlwinm	r7, rOpCode, 26, 30, 31
	cmpi	cr0, r7, 3
	beq	cr0,.preDoBinOpMul1
	rlwinm	r3, rOpCode, 29, 29, 31
	cmpli	cr0, r3, 1
	bgt	cr0,.L1770
	rlwinm	r0, rOpCode, 24, 31, 31
	cmpi	cr0, r0, 1
	bne	cr0,.L1770
	cmpi	cr0, r7, 0
	beq	cr0,.L1745
	cmpi	cr0, r7, 1
	beq	cr0,.L1754
	cmpi	cr0, r7, 2
	beq	cr0,.L1763
	b	.m68k_NextInstruction
.L1745:
	cmpi	cr0, r3, 0
	mr	rOpSize, r6
	rlwinm	r6, rOpCode, 23, 29, 31
	rlwinm	r8, rOpCode, 0, 29, 31
	bne	cr0,.L1749
	addi	r3, 0, 0
	mr	r4, r8
	addi	r5, 0, 0
	addi	r7, 0, 4
	b	.DoBinOp1notA
.L1749:
	addi	r3, 0, 4
	mr	r4, r8
	addi	r5, 0, 4
	addi	r7, 0, 4
	b	.DoBinOp1notA
.L1754:
	cmpi	cr0, r3, 0
	rlwinm	r7, rOpCode, 25, 27, 29
	add	r6, r7, rRegs
	rlwinm	r9, rOpCode, 2, 27, 29
	add	r5, r9, rRegs
	bne	cr0,.L1758
	lwzx	r0, r7, rRegs
	lwzx	r3, r9, rRegs
	stwx	r3, r7, rRegs
	stwx	r0, r9, rRegs
	b	.m68k_NextInstruction
.L1758:
	lwz	r4, 32(r6)
	lwz	r3, 32(r5)
	stw	r3, 32(r6)
	stw	r4, 32(r5)
	b	.m68k_NextInstruction
.L1763:
	rlwinm	r4, rOpCode, 2, 27, 29
	add	r5, r4, rRegs
	rlwinm	r7, rOpCode, 25, 27, 29
	lwz	r6, 32(r5)
	lwzx	r3, r7, rRegs
	stwx	r6, r7, rRegs
	stw	r3, 32(r5)
	b	.m68k_NextInstruction
.L1770:
	bl	.FindOpSizeFromb76
	rlwinm	r9, rOpCode, 24, 31, 31
	cmpi	cr1, r9, 1
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r8, rOpCode, 0, 29, 31
	rlwinm	r6, rOpCode, 23, 29, 31
	bne	cr1,.L1775
	addi	r7, 0, 7
	mr	r4, r6
	mr	r5, r3
	addi	r3, 0, 0
	mr	r6, r8
	b	.DoBinOp1
.L1775:
	addi	r5, 0, 0
	mr	r4, r8
	addi	r7, 0, 7
	b	.DoBinOp1


.DoCodeD:
	rlwinm	r0, rOpCode, 26, 30, 31
	cmpi	cr0, r0, 3
	bne	cr0,.L1787
	rlwinm	r3, rOpCode, 25, 30, 30
	addi	rOpSize, r3, 2
	rlwinm	r6, rOpCode, 23, 29, 31
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
	addi	r7, 0, 0
	b	.DoBinOpA
.L1787:
	bl	.FindOpSizeFromb76
	rlwinm.	r7, rOpCode, 24, 31, 31
	rlwinm	r5, rOpCode, 29, 29, 31
	rlwinm	r9, rOpCode, 0, 29, 31
	rlwinm	r6, rOpCode, 23, 29, 31
	bne	cr0,.L1792
	addi	r7, 0, 0
	mr	r3, r5
	addi	r5, 0, 0
	mr	r4, r9
	b	.DoBinOp1
.L1792:
	cmpli	cr0, r5, 2
	blt	cr0,.L1796
	addi	r3, 0, 0
	mr	r4, r6
	addi	r7, 0, 0
	mr	r6, r9
	b	.DoBinOp1notA
.L1796:
	cmpi	cr0, r5, 0
	bne	cr0,.L1800
	addi	r3, 0, 0
	mr	r4, r9
	addi	r5, 0, 0
	addi	r7, 0, 2
	b	.DoBinOp1notA
.L1800:
	addi	r3, 0, 4
	mr	r4, r9
	addi	r5, 0, 4
	addi	r7, 0, 2
	b	.DoBinOp1notA


.rolops:
	rlwinm	r3, r3, 1, 29, 30
	rlwimi	r3, rOpCode, 24, 31, 31
	xori	r3, r3, 1
	addi	r3, r3, 9
	blr


.DoCodeE:
	rlwinm	r0, rOpCode, 26, 30, 31
	cmpi	cr0, r0, 3
	bne	cr0,.L1833
	addi	rOpSize, 0, 2
	rlwinm	r3, rOpCode, 23, 29, 31
	rlwinm	r6, rOpCode, 0, 29, 31
	bl	.rolops
	addi	r4, 0, 1
	mr	r7, r3
	addi	r3, 0, 8
	rlwinm	r5, rOpCode, 29, 29, 31
	b	.DoBinOp1
.L1833:
	bl	.FindOpSizeFromb76
	rlwinm	r6, rOpCode, 29, 3, 31
	rlwinm	r0, r6, 0, 29, 31
	cmpli	cr1, r0, 3
	bgt	cr1,.L1838
	rlwinm	r3, rOpCode, 29, 30, 31
	rlwinm	r6, rOpCode, 0, 29, 31
	bl	.rolops
	mr	r7, r3
	bl	.octdat
	addi	r5, 0, 0
	b	.DoBinOp1notA
.L1838:
	rlwinm	r3, r6, 0, 30, 31
	rlwinm	r6, rOpCode, 0, 29, 31
	bl	.rolops
	addi	r5, 0, 0
	mr	r7, r3
	addi	r3, 0, 0
	rlwinm	r4, rOpCode, 23, 29, 31
	b	.DoBinOp1notA


.DoCodeF:
	addi	r3, 0, 11
	addi	rPC_p, rPC_p, -2
	bl	.Exception
	b	.m68k_NextInstruction


.op_illg:
	addi	r3, 0, 4
	addi	rPC_p, rPC_p, -2
	bl	.Exception
	b	.m68k_NextInstruction


.ReadInterruptPriorityLevel:
	lwz       r11,fIPL(rRegs)
	lbz       r3,0x0000(r11)
	blr


.DoCheckExternalInterruptPending:
	bl	.ReadInterruptPriorityLevel
	lwz	r5, fReg_intmask(rRegs)
	mr	r31, r3
	cmp	cr1, r31, r5
	bgt	cr1,.L119
	cmpi	cr0, r31, 7
	bne	cr0,.B2end9967
.L119:
	addi	r3, r31, 24
	bl	.Exception
	stw	r31, fReg_intmask(rRegs)
.B2end9967:
	addi	r12, 0, 0
	stb	r12, fReg_ExtIntPend(rRegs)
	b	.AfterCheckExternalInterruptPending


.do_trace:
	addi	r3, 0, 1
	stb	r3, fReg_TracePend(rRegs)
	bl	.NeedToGetOut
	b	.AfterDoTrace


.DoPendingTrace:
	addi	r3, 0, 9
	bl	.Exception
	b	.AfterDoPendingTrace


.MyEmulatorTempLeave:
	stw	rMaxInstrsToGo, fReg_MaxInstrs(rRegs)
	blr


.MyEmulatorTempReturn:
; must leave r3 unchanged
	lwz	rMaxInstrsToGo, fReg_MaxInstrs(rRegs)
	blr


.get_byte_ext:
	mflr	r0
	stw	r0, 8(sp)
	stwu	sp, -64(sp)
	bl	.MyEmulatorTempLeave

	mr	r6, r3		; addr
	addi	r5, 0, 1	; ByteSize
	addi	r4, 0, 0	; WriteMem
	addi	r3, 0, 0	; Data
	bl	.MM_Access[PR]
	Nop
	rlwinm	r3, r3, 0, 24, 31

	bl	.MyEmulatorTempReturn
	lwz	r0, 72(sp)
	addi	sp, sp, 64
	mtlr	r0
	extsb	r3, r3
	blr


.put_byte_ext:
	mflr	r0
	stw	r0, 8(sp)
	stwu	sp, -64(sp)
	bl	.MyEmulatorTempLeave

	mr	r6, r3		; addr
	addi	r5, 0, 1	; ByteSize
	rlwinm	r3, r4, 0, 24, 31 ; Data
	addi	r4, 0, 1	; WriteMem
	bl	.MM_Access[PR]
	Nop
	bl	.MyEmulatorTempReturn
	lwz	r0, 72(sp)
	addi	sp, sp, 64
	mtlr	r0
	blr


.get_word_ext:
	mflr	r0
	stw	r0, 8(sp)
	stwu	sp, -64(sp)
	bl	.MyEmulatorTempLeave

	mr	r6, r3		; addr
	addi	r5, 0, 0	; ByteSize
	addi	r4, 0, 0	; WriteMem
	addi	r3, 0, 0	; Data
	bl	.MM_Access[PR]
	Nop
	rlwinm	r3, r3, 0, 16, 31
	extsh	r3, r3

	bl	.MyEmulatorTempReturn
	lwz	r0, 72(sp)
	addi	sp, sp, 64
	mtlr	r0
	blr


.put_word_ext:
	mflr	r0
	stw	r0, 8(sp)
	stwu	sp, -64(sp)
	bl	.MyEmulatorTempLeave

	mr	r6, r3		; addr
	addi	r5, 0, 0	; ByteSize
	rlwinm	r3, r4, 0, 16, 31 ; Data
	addi	r4, 0, 1	; WriteMem
	bl	.MM_Access[PR]
	Nop

	bl	.MyEmulatorTempReturn
	lwz	r0, 72(sp)
	addi	sp, sp, 64
	mtlr	r0
	blr


.get_long_ext:
	mflr	r0
	stmw	r30, -8(sp)
	stw	r0, 8(sp)
	stwu	sp, -64(sp)
	mr	r31, r3
	bl	.get_word
	mr	r30, r3
	addi	r3, r31, 2
	bl	.get_word
	rlwimi	r3, r30, 16, 0, 15
	lwz	r0, 72(sp)
	addi	sp, sp, 64
	mtlr	r0
	lmw	r30, -8(sp)
	blr


.put_long_ext:
	mflr	r0
	stmw	r30, -8(sp)
	stw	r0, 8(sp)
	stwu	sp, -64(sp)
	mr	r30, r4
	mr	r31, r3
	rlwinm	r4, r30, 16, 16, 31
	bl	.put_word
	addi	r3, r31, 2
	rlwinm	r4, r30, 0, 16, 31
	bl	.put_word
	lwz	r0, 72(sp)
	addi	sp, sp, 64
	mtlr	r0
	lmw	r30, -8(sp)
	blr


.get_pc_real_address_ext:
	mflr	r0
	stw	r0, 8(sp)
	stwu	sp, -64(sp)
	bl	.MyEmulatorTempLeave

	mr	r5, r3		; addr
	addi	r4, 0, 0	; WritableMem
	addi	r3, 0, 2	; L

	bl	.get_real_address[PR]
	Nop
	mr	rPC_p, r3	; addr

	bl	.MyEmulatorTempReturn
	lwz	r0, 72(sp)
	addi	sp, sp, 64
	mtlr	r0

	cmpi	cr0, rPC_p, 0
	bne+	cr0,.end_get_pc_real_address
	lwz	rPC_p, 0(rBankReadAddr)
	b	.end_get_pc_real_address


	export	.m68k_IPLchangeNtfy[PR]
	export	m68k_IPLchangeNtfy[DS]
	csect	.m68k_IPLchangeNtfy[PR]
.m68k_IPLchangeNtfy:
	lwz	r6, .b2regs9999[TC](rtoc)
	lwz	r11, fIPL(r6)
	lwz	r4, fReg_intmask(r6)
	lbz	r3, 0x0000(r11)
	cmp	cr1, r3, r4
	bgt	cr1,.L126
	cmpi	cr0, r3, 7
	bnelr	cr0
.L126:
	lwz	r12, fReg_MaxInstrs(r6)
	addi	r3, 0, 1
	stb	r3, fReg_ExtIntPend(r6)
	cmpi	cr0, r12, 0
	beqlr	cr0
	lwz	r3, fReg_MoreInstrs(r6)
	addi	r4, 0, 1
	stw	r4, fReg_MaxInstrs(r6)
	add	r3, r3, r12
	addi	r3, r3, -1
	stw	r3, fReg_MoreInstrs(r6)
	blr


	export	.GetInstructionsRemaining[PR]
	export	GetInstructionsRemaining[DS]
	csect	.GetInstructionsRemaining[PR]
.GetInstructionsRemaining:
	lwz	r6, .b2regs9999[TC](rtoc)
	lwz	r3,fReg_MoreInstrs(r6)
	lwz	r5,fReg_MaxInstrs(r6)
	add	r3,r3,r5
	blr


	export	.SetInstructionsRemaining[PR]
	export	SetInstructionsRemaining[DS]
	csect	.SetInstructionsRemaining[PR]
.SetInstructionsRemaining:
	lwz	r6, .b2regs9999[TC](rtoc)
	lwz	r5,fReg_MaxInstrs(r6)
	cmplw	r5,r3
	blt	.L1_SetInstructionsRemaining
	li	r0,0
	stw	r3,fReg_MaxInstrs(r6)
	stw	r0,fReg_MoreInstrs(r6)
	blr
.L1_SetInstructionsRemaining:
	sub	r4,r3,r5
	stw	r4,fReg_MoreInstrs(r6)
	blr


	export	.DiskInsertedPsuedoException[PR]
	export	DiskInsertedPsuedoException[DS]
	csect	.DiskInsertedPsuedoException[PR]
.DiskInsertedPsuedoException:
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -80(sp)

	bl	.MyEmulatorEntry

	mr	r31, r4
	bl	.ExceptionTo
	lwz	r3, fReg_A7(rRegs)
	mr	r4, r31
	addi	r3, r3, -4
	stw	r3, fReg_A7(rRegs)
	bl	.put_long
	bl	.MyEmulatorExit

	lwz	r0, 88(sp)
	addi	sp, sp, 80
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	export	.m68k_reset[PR]
	export	m68k_reset[DS]
	csect	.m68k_reset[PR]
.m68k_reset:
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -80(sp)

	bl	.MyEmulatorEntry

	addi	r30, 0, 0
	addi	r29, 0, 7
	addi	r28, 0, 1
	addi	r3, 0, 4
	bl	.get_long
	bl	.m68k_setpc
	addi	r3, 0, 0
	bl	.get_long
	stw	r3, fReg_A7(rRegs)
	stb	r28, fReg_s(rRegs)
;	stb	r30, fReg_m(rRegs)
	stb	r30, fReg_t1(rRegs)
	mr	rFlag_v, r30
	mr	rFlag_n, r30
	mr	rFlag_c, r30
	mr	rFlag_z, r30
	stb	r30, fReg_ExtIntPend(rRegs)
	stb	r30, fReg_TracePend(rRegs)
	stw	r29, fReg_intmask(rRegs)
.B2end9963:
.B2LEP9962:
	bl	.MyEmulatorExit

	lwz	r0, 88(sp)
	addi	sp, sp, 80
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	export	.MINEM68K_Init[PR]
	export	MINEM68K_Init[DS]
	csect	.MINEM68K_Init[PR]
.MINEM68K_Init:
	lwz	r9, .b2regs9999[TC](rtoc)
	stw	r3, fBankReadAddr(r9)
	stw	r4, fBankWritAddr(r9)
	stw	r5, fIPL(r9)
	blr
