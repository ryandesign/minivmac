/*
	MINEM68K.S

	Copyright (C) 2010 Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.

	PowerPC assembler version of MINEM68K.c
*/


#include	"CNFGRASM.i"


/* exports */

	my_export_proc	MINEM68K_Init
	my_export_proc	m68k_go_nCycles
	my_export_proc	GetCyclesRemaining
	my_export_proc	SetCyclesRemaining
	my_export_proc	m68k_IPLchangeNtfy
	my_export_proc	DiskInsertedPsuedoException
	my_export_proc	m68k_reset
	my_export_proc	get_vm_byte
	my_export_proc	get_vm_word
	my_export_proc	get_vm_long
	my_export_proc	put_vm_byte
	my_export_proc	put_vm_word
	my_export_proc	put_vm_long
	my_export_proc	SetHeadATTel
	my_export_proc	FindATTel


/* imports */

	my_imports_begin
	my_import_proc	M68KITAB_setup
	my_import_proc	customreset
	my_import_proc	MMDV_Access
	my_import_proc	MemAccessNtfy
	my_imports_end


/* dispatch table */

	my_dispatch_table_begin	Dispatches

	my_dispatch_entry	.DoIKind_Tst
	my_dispatch_entry	.DoIKind_CmpB
	my_dispatch_entry	.DoIKind_CmpW
	my_dispatch_entry	.DoIKind_CmpL
	my_dispatch_entry	.DoIKind_BccB
	my_dispatch_entry	.DoIKind_BccW
	my_dispatch_entry	.DoIKind_BraB
	my_dispatch_entry	.DoIKind_BraW
	my_dispatch_entry	.DoIKind_DBcc
	my_dispatch_entry	.DoIKind_DBF
	my_dispatch_entry	.DoIKind_Swap
	my_dispatch_entry	.DoMove
		/* .DoIKind_MoveL */
	my_dispatch_entry	.DoMove
		/* .DoIKind_MoveW */
	my_dispatch_entry	.DoMove
		/* .DoIKind_MoveB */
	my_dispatch_entry	.DoIKind_MoveAL
	my_dispatch_entry	.DoIKind_MoveAW
	my_dispatch_entry	.DoIKind_MoveQ
	my_dispatch_entry	.DoIKind_AddB
	my_dispatch_entry	.DoIKind_AddW
	my_dispatch_entry	.DoIKind_AddL
	my_dispatch_entry	.DoIKind_SubB
	my_dispatch_entry	.DoIKind_SubW
	my_dispatch_entry	.DoIKind_SubL
	my_dispatch_entry	.DoIKind_Lea
	my_dispatch_entry	.DoIKind_PEA
	my_dispatch_entry	.DoIKind_A
	my_dispatch_entry	.DoIKind_BsrB
	my_dispatch_entry	.DoIKind_BsrW
	my_dispatch_entry	.DoIKind_Jsr
	my_dispatch_entry	.DoIKind_LinkA6
	my_dispatch_entry	.DoIKind_MOVEMRmML
	my_dispatch_entry	.DoIKind_MOVEMApRL
	my_dispatch_entry	.DoIKind_UnlkA6
	my_dispatch_entry	.DoIKind_Rts
	my_dispatch_entry	.DoIKind_Jmp
	my_dispatch_entry	.DoIKind_Clr
	my_dispatch_entry	.DoIKind_AddA
	my_dispatch_entry	.DoIKind_AddQA
	my_dispatch_entry	.DoIKind_SubA
	my_dispatch_entry	.DoIKind_SubQA
	my_dispatch_entry	.DoIKind_CmpA
	my_dispatch_entry	.DoIKind_AddXB
	my_dispatch_entry	.DoIKind_AddXW
	my_dispatch_entry	.DoIKind_AddXL
	my_dispatch_entry	.DoIKind_SubXB
	my_dispatch_entry	.DoIKind_SubXW
	my_dispatch_entry	.DoIKind_SubXL
	my_dispatch_entry	.DoIKind_RolopNM
	my_dispatch_entry	.DoIKind_RolopND
	my_dispatch_entry	.DoIKind_RolopDD
	my_dispatch_entry	.DoIKind_BitOpDD
	my_dispatch_entry	.DoIKind_BitOpDM
	my_dispatch_entry	.DoIKind_BitOpND
	my_dispatch_entry	.DoIKind_BitOpNM
	my_dispatch_entry	.DoIKind_And
		/* .DoIKind_AndI */
	my_dispatch_entry	.DoIKind_And
		/* .DoIKind_AndEaD */
	my_dispatch_entry	.DoIKind_And
		/* .DoIKind_AndDEa */
	my_dispatch_entry	.DoIKind_Or
		/* .DoIKind_OrI */
	my_dispatch_entry	.DoIKind_Or
		/* .DoIKind_OrDEa */
	my_dispatch_entry	.DoIKind_Or
		/* .DoIKind_OrEaD */
	my_dispatch_entry	.DoIKind_Eor
		/* .DoIKind_Eor */
	my_dispatch_entry	.DoIKind_Eor
		/* .DoIKind_EorI */
	my_dispatch_entry	.DoIKind_Not
	my_dispatch_entry	.DoIKind_Scc
	my_dispatch_entry	.DoIKind_NegXB
	my_dispatch_entry	.DoIKind_NegXW
	my_dispatch_entry	.DoIKind_NegXL
	my_dispatch_entry	.DoIKind_NegB
	my_dispatch_entry	.DoIKind_NegW
	my_dispatch_entry	.DoIKind_NegL
	my_dispatch_entry	.DoIKind_EXTW
	my_dispatch_entry	.DoIKind_EXTL
	my_dispatch_entry	.DoIKind_MulU
	my_dispatch_entry	.DoIKind_MulS
	my_dispatch_entry	.DoIKind_DivU
	my_dispatch_entry	.DoIKind_DivS
	my_dispatch_entry	.DoIKind_Exgdd
	my_dispatch_entry	.DoIKind_Exgaa
	my_dispatch_entry	.DoIKind_Exgda
	my_dispatch_entry	.DoIKind_MoveCCREa
	my_dispatch_entry	.DoIKind_MoveEaCCR
	my_dispatch_entry	.DoIKind_MoveSREa
	my_dispatch_entry	.DoIKind_MoveEaSR
	my_dispatch_entry	.DoIKind_BinOpStatusCCR
	my_dispatch_entry	.DoIKind_MOVEMApRW
	my_dispatch_entry	.DoIKind_MOVEMRmMW
	my_dispatch_entry	.DoIKind_MOVEMrm
	my_dispatch_entry	.DoIKind_MOVEMmr
	my_dispatch_entry	.DoIKind_Abcdr
	my_dispatch_entry	.DoIKind_Abcdm
	my_dispatch_entry	.DoIKind_Sbcdr
	my_dispatch_entry	.DoIKind_Sbcdm
	my_dispatch_entry	.DoIKind_Nbcd
	my_dispatch_entry	.DoIKind_Rte
	my_dispatch_entry	.DoIKind_Nop
	my_dispatch_entry	.DoIKind_MoveP
	my_dispatch_entry	.DoIKind_Illegal
	my_dispatch_entry	.DoIKind_ChkW
	my_dispatch_entry	.DoIKind_Trap
	my_dispatch_entry	.DoIKind_TrapV
	my_dispatch_entry	.DoIKind_Rtr
	my_dispatch_entry	.DoIKind_Link
	my_dispatch_entry	.DoIKind_Unlk
	my_dispatch_entry	.DoIKind_MoveRUSP
	my_dispatch_entry	.DoIKind_MoveUSPR
	my_dispatch_entry	.DoIKind_Tas
	my_dispatch_entry	.DoIKind_F
	my_dispatch_entry	.DoIKind_CallMorRtm
	my_dispatch_entry	.DoIKind_Stop
	my_dispatch_entry	.DoIKind_Reset

	my_dispatch_offset	x_b2SDecodeSrcDst, Dispatches
	my_dispatch_entry	.DecodeSrcDstAMdReg
	my_dispatch_entry	.DecodeSrcDstAMdIndirect
	my_dispatch_entry	.DecodeSrcDstAMdAPosIncB
	my_dispatch_entry	.DecodeSrcDstAMdAPosIncW
	my_dispatch_entry	.DecodeSrcDstAMdAPosIncL
	my_dispatch_entry	.DecodeSrcDstAMdAPreDecB
	my_dispatch_entry	.DecodeSrcDstAMdAPreDecW
	my_dispatch_entry	.DecodeSrcDstAMdAPreDecL
	my_dispatch_entry	.DecodeSrcDstAMdADisp
	my_dispatch_entry	.DecodeSrcDstAMdAIndex
	my_dispatch_entry	.DecodeSrcDstAMdAbsW
	my_dispatch_entry	.DecodeSrcDstAMdAbsL
	my_dispatch_entry	.DecodeSrcDstAMdPCDisp
	my_dispatch_entry	.DecodeSrcDstAMdPCIndex
	my_dispatch_entry	.DecodeSrcDstAMdImmedB
	my_dispatch_entry	.DecodeSrcDstAMdImmedW
	my_dispatch_entry	.DecodeSrcDstAMdImmedL
	my_dispatch_entry	.DecodeSrcDstAMdDat4

	my_dispatch_offset	x_b2SGetSrcDstValue, Dispatches
	my_dispatch_entry	.GetSrcDstValRegB
	my_dispatch_entry	.GetSrcDstValRegW
	my_dispatch_entry	.GetSrcDstValRegL
	my_dispatch_entry	.GetSrcDstValMemB
	my_dispatch_entry	.GetSrcDstValMemW
	my_dispatch_entry	.GetSrcDstValMemL
	my_dispatch_entry	.GetSrcDstValCnst

	my_dispatch_offset	x_b2SetDstValue, Dispatches
	my_dispatch_entry	.SetDstValRegB
	my_dispatch_entry	.SetDstValRegW
	my_dispatch_entry	.SetDstValRegL
	my_dispatch_entry	.SetDstValMemB
	my_dispatch_entry	.SetDstValMemW
	my_dispatch_entry	.SetDstValMemL
	my_dispatch_entry	.SetDstValCnst

	my_dispatch_offset	x_b2S9993, Dispatches
	my_dispatch_entry	.L49
	my_dispatch_entry	.L36
	my_dispatch_entry	.L8
	my_dispatch_entry	.L10
	my_dispatch_entry	.L12
	my_dispatch_entry	.L14
	my_dispatch_entry	.L16
	my_dispatch_entry	.L18
	my_dispatch_entry	.L20
	my_dispatch_entry	.L22
	my_dispatch_entry	.L24
	my_dispatch_entry	.L26
	my_dispatch_entry	.L28
	my_dispatch_entry	.L30
	my_dispatch_entry	.L32
	my_dispatch_entry	.L34

	my_dispatch_offset	x_b2S9987, Dispatches
	my_dispatch_entry	.SetArgKindDReg
	my_dispatch_entry	.SetArgKindAReg
	my_dispatch_entry	.DecodeMode2Reg
	my_dispatch_entry	.DecodeMode3Reg
	my_dispatch_entry	.DecodeMode4Reg
	my_dispatch_entry	.DecodeMode5Reg
	my_dispatch_entry	.DecodeMode6Reg
	my_dispatch_entry	.DecodeMode7Reg
	my_dispatch_entry	.L193
	my_dispatch_entry	.B2end9957

	my_dispatch_offset	x_b2S9986, Dispatches
	my_dispatch_entry	.L170
	my_dispatch_entry	.L172
	my_dispatch_entry	.L174
	my_dispatch_entry	.L176
	my_dispatch_entry	.SetArgKindImmediate
	my_dispatch_entry	.B2end9957
	my_dispatch_entry	.B2end9957
	my_dispatch_entry	.B2end9957

	my_dispatch_offset	x_b2S9983, Dispatches
	my_dispatch_entry	.L380
	my_dispatch_entry	.L423
	my_dispatch_entry	.L458
	my_dispatch_entry	.L494
	my_dispatch_entry	.L552
	my_dispatch_entry	.L623
	my_dispatch_entry	.L521
	my_dispatch_entry	.L583

	my_dispatch_table_end	Dispatches

	my_code_begin


.DoIKind_Nop:
.m68k_NextInstruction:
	lhzu	rOpCode, 2(rPC_p)
	addi	r5, rRegs, fdisp_table
	addi	r3, rRegs, fdisp_table + 4

	rlwinm	r4, rOpCode, 3, 0, 28

	lwzx	rDecOpA, r4, r5
	lwzx	rDecOpB, r4, r3

	cmpi	cr0, rMaxCyclesToGo, 0
	rlwinm	r3, rDecOpA, 2, 14, 29
	rlwinm	r4, rDecOpB, 0, 16, 31

	lwzx	r3, rDispatches, r3
	ble	cr0,.after_m68k_go_MaxCycles
	mtctr	r3

	sub	rMaxCyclesToGo, rMaxCyclesToGo, r4

	bctr
		/* switch branch */


.DecodeSrcDstGet:
#if 0
	/* unoptimized. optimized version follows */
	/*
		in addition optimized version sets rArgKind to dispatch
		address for SetDstValue
	*/
	mflr	r31
	bl	.DecodeSrc
	bl	.GetSrcValue
	mr	r30, r3
	bl	.DecodeDst
	bl	.GetDstValue
	mtlr	r31
	blr
#endif
	mflr	r31

	rlwinm	r6, rDecOpB, 18, 25, 29
	addi	r5, rDispatches, x_b2SDecodeSrcDst
	rlwinm	r7, rDecOpB, 10, 26, 29
	lwzx	r6, r5, r6
	addi	r4, rDispatches, x_b2SGetSrcDstValue
	mtctr	r6
	mr	r8, rDecOpB
	lwzx	r29, r4, r7
	bctrl

	mtctr	r29
	rlwinm	r28, rDecOpA, 18, 25, 29
	bctrl

	addi	r5, rDispatches, x_b2SDecodeSrcDst
	mr	r30, r3
	lwzx	r6, r5, r28
	rlwinm	r7, rDecOpA, 10, 26, 29
	addi	r3, rDispatches, x_b2SGetSrcDstValue
	addi	r4, rDispatches, x_b2SetDstValue
	mtctr	r6
	mr	r8, rDecOpA
	lwzx	r29, r3, r7
	lwzx	rArgKind, r4, r7
	bctrl

	mtctr	r29

	mtlr	r31
		/* following call returns to address in r31 */
	bctr


.DecodeSrcDstAMdReg:
	rlwinm	r4, r8, 6, 26, 29
	/* addi	rArgKind, 0, 2 */
	add	rArgAddr, r4, rRegs
	blr


.GetSrcDstValRegW:
	lha	r3, 2(rArgAddr)
	/* extsh	r3, r3 */
	blr


.SetDstValRegW:
	sth	r3, 2(rArgAddr)
	b	.m68k_NextInstruction


.GetSrcDstValRegL:
	lwz	r3, 0(rArgAddr)
	blr


.SetDstValRegL:
	stw	r3, 0(rArgAddr)
	b	.m68k_NextInstruction


.GetSrcDstValRegB:
	lbz	r3, 3(rArgAddr)
	extsb	r3, r3
	blr


.SetDstValRegB:
	stb	r3, 3(rArgAddr)
	b	.m68k_NextInstruction


.DecodeSrcDstAMdADisp:
	rlwinm	r4, r8, 6, 26, 29
	/* addi	rArgKind, 0, 0 */
	lhau	r3, 2(rPC_p)
	lwzx	r5, rRegs, r4
	add	rArgAddr, r5, r3
	blr


.GetSrcDstValMemW:
	mr	r3, rArgAddr
	/* b	.get_word */


.get_word:
	lwz	r12, fReg_MATCrdW_cmpmask(rRegs)
	lwz	r10, fReg_MATCrdW_cmpvalu(rRegs)
	and	r7, r12, r3
	lwz	r11, fReg_MATCrdW_usemask(rRegs)
	lwz	r9, fReg_MATCrdW_usebase(rRegs)
	cmpw	cr0, r7, r10
	and	r8, r11, r3
	bne	cr0, .get_word_ext
	lhax	r3, r8, r9
	blr


.SetDstValMemW:
	mr	r4, r3
	mr	r3, rArgAddr
	bl	.put_word
	b	.m68k_NextInstruction


.put_word:
	lwz	r12, fReg_MATCwrW_cmpmask(rRegs)
	lwz	r10, fReg_MATCwrW_cmpvalu(rRegs)
	and	r7, r12, r3
	lwz	r11, fReg_MATCwrW_usemask(rRegs)
	lwz	r9, fReg_MATCwrW_usebase(rRegs)
	cmpw	cr0, r7, r10
	and	r8, r11, r3
	bne	cr0, .put_word_ext
	sthx	r4, r8, r9
	blr


.GetSrcDstValMemL:
	mr	r3, rArgAddr
	/* b	.get_long */

.get_long:
	/*
		misalignment so common that it is just
		as fast to not have special case for
		aligned, avoiding unpredictable branch.
	*/
	lwz	r12, fReg_MATCrdW_cmpmask(rRegs)
	lwz	r10, fReg_MATCrdW_cmpvalu(rRegs)

	addi	r5, r3, 2

	and	r7, r12, r3
	cmpw	cr0, r7, r10
	lwz	r11, fReg_MATCrdW_usemask(rRegs)
	lwz	r9, fReg_MATCrdW_usebase(rRegs)
	and	r8, r11, r3
	bne	cr0, .get_long_ext

	and	r7, r12, r5
	cmpw	cr1, r7, r10
	and.	r6, r11, r5
	bne	cr1, .get_long_ext
	beq	cr0, .get_long_ext

	lwzx	r3, r8, r9
	blr


.SetDstValMemL:
	mr	r4, r3
	mr	r3, rArgAddr
	bl	.put_long
	b	.m68k_NextInstruction


.put_long:
	lwz	r12, fReg_MATCwrW_cmpmask(rRegs)
	lwz	r10, fReg_MATCwrW_cmpvalu(rRegs)

	addi	r5, r3, 2

	and	r7, r12, r3
	cmpw	cr0, r7, r10
	lwz	r11, fReg_MATCwrW_usemask(rRegs)
	lwz	r9, fReg_MATCwrW_usebase(rRegs)
	and	r8, r11, r3
	bne	cr0, .put_long_ext

	and	r7, r12, r5
	cmpw	cr1, r7, r10
	and.	r6, r11, r5
	bne	cr1, .put_long_ext
	beq	cr0, .put_long_ext

	stwx	r4, r8, r9
	blr


.GetSrcDstValMemB:
	mr	r3, rArgAddr
	/* b	.get_byte */

.get_byte:
	lwz	r12, fReg_MATCrdB_cmpmask(rRegs)
	lwz	r10, fReg_MATCrdB_cmpvalu(rRegs)
	and	r7, r12, r3
	lwz	r11, fReg_MATCrdB_usemask(rRegs)
	lwz	r9, fReg_MATCrdB_usebase(rRegs)
	cmpw	cr0, r7, r10
	and	r8, r11, r3
	bne	cr0, .get_byte_ext
	lbzx	r3, r8, r9
	extsb	r3, r3
	blr


.SetDstValMemB:
	mr	r4, r3
	mr	r3, rArgAddr
	bl	.put_byte
	b	.m68k_NextInstruction


.put_byte:
	lwz	r12, fReg_MATCwrB_cmpmask(rRegs)
	lwz	r10, fReg_MATCwrB_cmpvalu(rRegs)
	and	r7, r12, r3
	lwz	r11, fReg_MATCwrB_usemask(rRegs)
	lwz	r9, fReg_MATCwrB_usebase(rRegs)
	cmpw	cr0, r7, r10
	and	r8, r11, r3
	bne	cr0, .put_byte_ext
	stbx	r4, r8, r9
	blr


.DecodeSrcDstAMdIndirect:
	rlwinm	r4, r8, 6, 26, 29
	/* addi	rArgKind, 0, 0 */
	lwzx	rArgAddr, rRegs, r4
	blr
.DecodeSrcDstAMdAPosIncB:
	rlwinm	r4, r8, 6, 26, 29
	/* addi	rArgKind, 0, 0 */
	lwzx	rArgAddr, rRegs, r4
	addi	r5, rArgAddr, 1
	stwx	r5, rRegs, r4
	blr
.DecodeSrcDstAMdAPosIncW:
	rlwinm	r4, r8, 6, 26, 29
	/* addi	rArgKind, 0, 0 */
	lwzx	rArgAddr, rRegs, r4
	addi	r5, rArgAddr, 2
	stwx	r5, rRegs, r4
	blr
.DecodeSrcDstAMdAPosIncL:
	rlwinm	r4, r8, 6, 26, 29
	/* addi	rArgKind, 0, 0 */
	lwzx	rArgAddr, rRegs, r4
	addi	r5, rArgAddr, 4
	stwx	r5, rRegs, r4
	blr
.DecodeSrcDstAMdAPreDecB:
	rlwinm	r4, r8, 6, 26, 29
	/* addi	rArgKind, 0, 0 */
	lwzx	rArgAddr, rRegs, r4
	subi	rArgAddr, rArgAddr, 1
	stwx	rArgAddr, rRegs, r4
	blr
.DecodeSrcDstAMdAPreDecW:
	rlwinm	r4, r8, 6, 26, 29
	/* addi	rArgKind, 0, 0 */
	lwzx	rArgAddr, rRegs, r4
	subi	rArgAddr, rArgAddr, 2
	stwx	rArgAddr, rRegs, r4
	blr
.DecodeSrcDstAMdAPreDecL:
	rlwinm	r4, r8, 6, 26, 29
	/* addi	rArgKind, 0, 0 */
	lwzx	rArgAddr, rRegs, r4
	subi	rArgAddr, rArgAddr, 4
	stwx	rArgAddr, rRegs, r4
	blr
.DecodeSrcDstAMdAIndex:
	/* addi	rArgKind, 0, 0 */
	rlwinm	r4, r8, 6, 26, 29
	lhzu	r3, 2(rPC_p)
	lwzx	r5, rRegs, r4
	b	.get_disp_ea_0
.DecodeSrcDstAMdAbsW:
	/* addi	rArgKind, 0, 0 */
	lhau	rArgAddr, 2(rPC_p)
	blr
.DecodeSrcDstAMdAbsL:
	/* addi	rArgKind, 0, 0 */
	lwz	rArgAddr, 2(rPC_p)
	addi	rPC_p, rPC_p, 4
	blr
.DecodeSrcDstAMdPCDisp:
	mflr	r9
	/* addi	rArgKind, 0, 0 */
	bl	.m68k_getpc
	lhau	r4, 2(rPC_p)
	mtlr	r9
	add	rArgAddr, r4, r3
	blr
.DecodeSrcDstAMdPCIndex:
	mflr	r9
	/* addi	rArgKind, 0, 0 */
	bl	.m68k_getpc
	mtlr	r9
	/* b	.get_disp_ea */

.get_disp_ea:
	mr	r5, r3
	lhzu	r3, 2(rPC_p)
.get_disp_ea_0:
	rlwinm	r6, r3, 0, 16, 31
	rlwinm	r4, r3, 22, 26, 29
	rlwinm.	r6, r6, 0, 20, 20
	lwzx	r9, r4, rRegs
	bne	cr0,.L133
	extsh	r9, r9
.L133:
	extsb	r7, r3
	add	r4, r5, r7
	add	rArgAddr, r4, r9
	blr


.DecodeSrcDstAMdDat4:
	/* addi	rArgKind, 0, 3 */
	rlwinm	rArgAddr, r8, 4, 28, 31
	blr

.DecodeSrcDstAMdImmedB:
	/* addi	rArgKind, 0, 3 */
	lbz	r3, 3(rPC_p)
	addi	rPC_p, rPC_p, 2
	extsb	rArgAddr, r3
	blr

.DecodeSrcDstAMdImmedL:
	/* addi	rArgKind, 0, 3 */
	lwz	rArgAddr, 2(rPC_p)
	addi	rPC_p, rPC_p, 4
	blr

.DecodeSrcDstAMdImmedW:
	/* addi	rArgKind, 0, 3 */
	lhau	rArgAddr, 2(rPC_p)
	blr

.GetSrcDstValCnst:
	mr	r3, rArgAddr
	blr

.SetDstValCnst:
	/* shouldn't get here */
	b	.m68k_NextInstruction

#if 0
.DecodeSrc:
	rlwinm	r6, rDecOpB, 18, 25, 29
	addi	r5, rDispatches, x_b2SDecodeSrcDst
	lwzx	r6, r5, r6
	mtctr	r6
	mr	r8, rDecOpB
	bctr
#endif


.DecodeDst:
	rlwinm	r6, rDecOpA, 18, 25, 29
	addi	r5, rDispatches, x_b2SDecodeSrcDst
	lwzx	r6, r5, r6
	mtctr	r6
	mr	r8, rDecOpA
	bctr


#if 0
.GetSrcValue:
	rlwinm	r6, rDecOpB, 10, 26, 29
	addi	r5, rDispatches, x_b2SGetSrcDstValue
	lwzx	r6, r5, r6
	mtctr	r6
	bctr
#endif


#if 0
.GetDstValue:
	rlwinm	r6, rDecOpA, 10, 26, 29
	addi	r5, rDispatches, x_b2SGetSrcDstValue
	lwzx	r6, r5, r6
	mtctr	r6
	bctr
#endif


.SetDstValue:
	rlwinm	r6, rDecOpA, 10, 26, 29
	addi	r5, rDispatches, x_b2SetDstValue
	lwzx	r6, r5, r6
	mtctr	r6
	bctr


.DoMove:
	rlwinm	r6, rDecOpB, 18, 25, 29
	addi	r5, rDispatches, x_b2SDecodeSrcDst
	rlwinm	r7, rDecOpB, 10, 26, 29
	lwzx	r6, r5, r6
	addi	r4, rDispatches, x_b2SGetSrcDstValue
	mtctr	r6
	mr	r8, rDecOpB
	lwzx	r31, r4, r7
	bctrl

	mtctr	r31
	rlwinm	r28, rDecOpA, 18, 25, 29
	bctrl

	addi	r5, rDispatches, x_b2SDecodeSrcDst
	mr	r30, r3
	lwzx	r6, r5, r28
	rlwinm	r7, rDecOpA, 10, 26, 29
	addi	r4, rDispatches, x_b2SetDstValue
	mtctr	r6
	mr	r8, rDecOpA
	lwzx	r31, r4, r7
	bctrl

	mr	r3, r30

	cntlzw	r4, r30
	mtctr	r31
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_n, r30, 1, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31

	bctr


.DoIKind_CmpB:
	bl	.DecodeSrcDstGet
	rlwinm	r3, r3, 24, 0, 7
	rlwinm	r30, r30, 24, 0, 7
	b	.DoCompare_r30r3


.DoIKind_CmpL:
	bl	.DecodeSrcDstGet
	b	.DoCompare_r30r3


.DoIKind_CmpW:
	bl	.DecodeSrcDstGet
	rlwinm	r3, r3, 16, 0, 15
	rlwinm	r30, r30, 16, 0, 15
	/* b	.DoCompare_r30r3 */


.DoCompare_r30r3:

	subfco	r3, r30, r3

	mfxer	r6
	cntlzw	r4, r3
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_c, r6, 3, 31, 31
	rlwinm	rFlag_v, r6, 2, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31
	xori	rFlag_c, rFlag_c, 1
	b	.m68k_NextInstruction


.DecodesDstGet:
#if 0
	/* unoptimized. optimized version follows */
	/*
		in addition optimized version sets rArgKind to dispatch
		address for SetDstValue
	*/
	mflr	r31
	bl	.DecodeDst
	bl	.GetDstValue
	mtlr	r31
	blr
#endif
	mflr	r31

	rlwinm	r6, rDecOpA, 18, 25, 29
	addi	r5, rDispatches, x_b2SDecodeSrcDst
	rlwinm	r7, rDecOpA, 10, 26, 29
	lwzx	r6, r5, r6
	addi	r3, rDispatches, x_b2SGetSrcDstValue
	addi	r4, rDispatches, x_b2SetDstValue
	mtctr	r6
	mr	r8, rDecOpA
	lwzx	r29, r3, r7
	lwzx	rArgKind, r4, r7
	bctrl

	mtctr	r29

	mtlr	r31
		/* following call returns to address in r31 */
	bctr


.DoIKind_Tst:
#if 0
	bl	.FindOpSizeFromb76
	bl	.DecodeNormModeRegister
	bl	.GetArgValue
#endif
	bl	.DecodesDstGet
.SetCCRandNext:
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	cntlzw	r4, r3
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31
	b	.m68k_NextInstruction


.cctrue:
	addi	r12, rDispatches, x_b2S9993
	rlwinm	r4, rOpCode, 26, 26, 29
	lwzx	r4, r12, r4
	mtctr	r4
	bctr

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

.DoIKind_BccB:
	bl	.cctrue
	cmpi	cr1, r3, 0
	beq	cr1, .m68k_NextInstruction
.DoIKind_BraB:
	/*
		bl	.m68k_getpc
		mr	r8, r3
	*/
	extsb	r3, rOpCode
	add	rPC_p, rPC_p, r3
	/*
		bl	.m68k_setpc
		mr	rPC_p, r31
	*/
	b	.m68k_NextInstruction

.DoIKind_BccW:
	bl	.cctrue
	cmpi	cr1, r3, 0
	beq	cr1, .DoIKind_BraSkipW
.DoIKind_BraW:
	/*
		bl	.m68k_getpc
		mr	r8, r3
	*/
	mr	r8, rPC_p
	lhau	r3, 2(rPC_p)
	add	rPC_p, r8, r3
	b	.m68k_NextInstruction


.DoIKind_BraSkipW:
	addi	rPC_p, rPC_p, 2
	b	.m68k_NextInstruction


.DoIKind_DBcc:
	/* bl	.m68k_getpc */
	addi	r12, rDispatches, x_b2S9993
	rlwinm	r4, rOpCode, 26, 26, 29
	mr	r3, rPC_p
	rlwinm	r28, rOpCode, 2, 27, 29

	lwzx	r4, r12, r4
	lhau	r8, 2(rPC_p)
	addi	r28, r28, 2

	mtctr	r4
	add	r29, r8, r3
	lhax	r31, r28, rRegs

	bctrl

	cmpi	cr0, r31, 0
	cmpi	cr1, r3, 0
	addi	r4, r31, -1
	bne	cr1, .m68k_NextInstruction
	sthx	r4, r28, rRegs
	beq	cr0, .m68k_NextInstruction
	/*
		mr	r3, r29
		bl	.m68k_setpc
	*/
	mr	rPC_p, r29
	b	.m68k_NextInstruction


.DoIKind_DBF:
	/* bl	.m68k_getpc */
	rlwinm	r28, rOpCode, 2, 27, 29
	mr	r3, rPC_p
	addi	r28, r28, 2
	lhau	r8, 2(rPC_p)
	lhax	r31, r28, rRegs
	add	r29, r8, r3

	addi	r4, r31, -1
	cmpi	cr0, r31, 0
	sthx	r4, r28, rRegs
	beq	cr0, .m68k_NextInstruction
	mr	rPC_p, r29
	b	.m68k_NextInstruction


.DoIKind_Swap:
	rlwinm	r5, rOpCode, 2, 27, 29
	lwzx	r6, r5, rRegs
	rlwinm	r3, r6, 16, 0, 31
	stwx	r3, r5, rRegs
	b	.SetCCRandNext


.DecodeNormModeRegister:
	rlwinm	r6, rOpCode, 31, 27, 29
	addi	r5, rDispatches, x_b2S9987
	lwzx	r6, r5, r6
	rlwinm	r4, rOpCode, 0, 29, 31
	mtctr	r6
	bctr

.SetArgKindDReg:
	addi	rArgKind, 0, 2
	rlwinm	rArgAddr, r4, 2, 0, 29
	add	rArgAddr, rArgAddr, rRegs
	blr
.SetArgKindAReg:
	addi	rArgKind, 0, 2
	rlwinm	rArgAddr, r4, 2, 0, 29
	add	rArgAddr, rArgAddr, rRegs
	addi	rArgAddr, rArgAddr, 32
	blr
.DecodeMode2Reg:
	addi	rArgKind, 0, 0
	rlwinm	r3, r4, 2, 0, 29
	add	r6, r3, rRegs
	lwz	rArgAddr, 32(r6)
	blr
.DecodeMode3Reg:
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
.DecodeMode4Reg:
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

.DecodeMode5Reg:
	addi	rArgKind, 0, 0
	rlwinm	r5, r4, 2, 0, 29
	add	r6, r5, rRegs
	lwz	r12, 32(r6)
	lhzu	r3, 2(rPC_p)
	extsh	r3, r3
	add	rArgAddr, r12, r3
	blr
.DecodeMode6Reg:
	addi	rArgKind, 0, 0
	rlwinm	r5, r4, 2, 0, 29
	add	r6, r5, rRegs
	lwz	r3, 32(r6)
	b	.get_disp_ea
.DecodeMode7Reg:
	addi	r12, rDispatches, x_b2S9986
	rlwinm	r3, r4, 2, 0, 29
	lwzx	r3, r12, r3
	mtctr	r3
	bctr
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
.SetArgKindImmediate:
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
	/* extsh	r3, r3 */
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


.DoIKind_MoveAL:
	addi	rOpSize, 0, 4
	b	.DoMoveA

.DoIKind_MoveAW:
	addi	rOpSize, 0, 2
	/* fall through */
.DoMoveA:
	bl	.DecodeNormModeRegister
	bl	.GetArgValue
	rlwinm	rArgAddr, rOpCode, 25, 27, 29
	addi	rArgAddr, rArgAddr, 32
	stwx	r3, rArgAddr, rRegs
	b	.m68k_NextInstruction

.DoIKind_MoveQ:
	rlwinm	r3, rOpCode, 0, 24, 31
	rlwinm	r4, rOpCode, 25, 27, 29
	extsb	r3, r3
	stwx	r3, r4, rRegs
	b	.SetCCRandNext


.DoIKind_AddB:
	bl	.DecodeSrcDstGet
	rlwinm	r3, r3, 24, 0, 7
	rlwinm	r30, r30, 24, 0, 7
	addco	r4, r3, r30
	mtctr	rArgKind
	rlwinm	r3, r4, 8, 24, 31
	b	DoIKind_Add_common


.DoIKind_AddL:
	bl	.DecodeSrcDstGet

	addco	r4, r3, r30
	mtctr	rArgKind
	mr	r3, r4
	b	DoIKind_Add_common


.DoIKind_AddW:
	bl	.DecodeSrcDstGet

	rlwinm	r3, r3, 16, 0, 15
	rlwinm	r30, r30, 16, 0, 15

	addco	r4, r3, r30
	mtctr	rArgKind
	rlwinm	r3, r4, 16, 16, 31
	/* b	DoIKind_Add_common */

DoIKind_Add_common:
	mfxer	r6
	cntlzw	r5, r4
	rlwinm	rFlag_n, r4, 1, 31, 31
	rlwinm	rFlag_c, r6, 3, 31, 31
	rlwinm	rFlag_v, r6, 2, 31, 31
	rlwinm	rFlag_z, r5, 27, 5, 31
	mr	rFlag_x, rFlag_c

	bctr


.DoIKind_SubB:
	bl	.DecodeSrcDstGet

	rlwinm	r3, r3, 24, 0, 7
	rlwinm	r30, r30, 24, 0, 7

	subfco	r4, r30, r3

	mtctr	rArgKind
	rlwinm	r3, r4, 8, 24, 31
	b	DoIKind_Sub_common


.DoIKind_SubL:
	bl	.DecodeSrcDstGet

	subfco	r4, r30, r3

	mtctr	rArgKind
	mr	r3, r4
	b	DoIKind_Sub_common


.DoIKind_SubW:
	bl	.DecodeSrcDstGet

	rlwinm	r3, r3, 16, 0, 15
	rlwinm	r30, r30, 16, 0, 15

	subfco	r4, r30, r3

	mtctr	rArgKind
	rlwinm	r3, r4, 16, 16, 31
	/* b	DoIKind_Sub_common */

DoIKind_Sub_common:
	mfxer	r6
	cntlzw	r5, r4
	rlwinm	rFlag_n, r4, 1, 31, 31
	rlwinm	rFlag_c, r6, 3, 31, 31
	rlwinm	rFlag_v, r6, 2, 31, 31
	xori	rFlag_c, rFlag_c, 1
	rlwinm	rFlag_z, r5, 27, 5, 31
	mr	rFlag_x, rFlag_c

	bctr


.DoIKind_Lea:
	rlwinm	r28, rOpCode, 25, 27, 29
	addi	r29, rRegs, 32
	bl	.DecodeDst
	stwx	rArgAddr, r29, r28
	b	.m68k_NextInstruction


.DoIKind_PEA:
	bl	.DecodeDst
	lwz	r3, fReg_A7(rRegs)
	mr	r4, rArgAddr
	addi	r3, r3, -4
	stw	r3, fReg_A7(rRegs)
	bl	.put_long
	b	.m68k_NextInstruction


/* .nextibyte: */
	/*
		lbz	r3, 3(rPC_p)
		addi	rPC_p, rPC_p, 2
		blr
	*/


/* .nextiword: */
	/*
		lhzu	r3, 2(rPC_p)
		blr
	*/


/* .nextilong: */
	/*
		lwz	r3, 2(rPC_p)
		addi	rPC_p, rPC_p, 4
		blr
	*/


/* .BackupPC: */
	/*
		addi	rPC_p, rPC_p, -2
		blr
	*/


.m68k_getpc:
	/* must preserve r9 */
	lwz	r5, fReg_pc_oldp(rRegs)
	lwz	r7, fReg_pc(rRegs)
	subf	r3, r5, rPC_p
	add	r3, r7, r3
	blr


.m68k_setpc:
	stw	r3, fReg_pc(rRegs)

/* get0_pc_real_address: */
	lwz	r12, fReg_MATCex_cmpmask(rRegs)
	lwz	r10, fReg_MATCex_cmpvalu(rRegs)
	and	r7, r12, r3
	lwz	r11, fReg_MATCex_usemask(rRegs)
	lwz	r9, fReg_MATCex_usebase(rRegs)
	cmpw	cr0, r7, r10
	and	r8, r11, r3
	bne	cr0, .get0_pc_real_address_ext
	add	rPC_p, r8, r9
.end_get0_pc_real_address:

	addi	rPC_p, rPC_p, -2
	stw	rPC_p, fReg_pc_oldp(rRegs)

	blr


.m68k_getSR:
	mr	r3, rFlag_c
	lbz	r6, fReg_t1(rRegs)
	lbz	r0, fReg_s(rRegs)
	lwz	r5, fReg_intmask(rRegs)
	/* lbz	r4, fReg_m(rRegs) */
	rlwimi	r3, rFlag_x, 4, 27, 27
	rlwimi	r3, rFlag_n, 3, 28, 28
	rlwimi	r3, rFlag_z, 2, 29, 29
	rlwimi	r3, rFlag_v, 1, 30, 30
	rlwimi	r3, r6, 15, 16, 16
	rlwimi	r3, r0, 13, 18, 18
	rlwimi	r3, r5, 8, 21, 23
	/* rlwimi	r3, r4, 12, 19, 19 */
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
	/* stb	r30, fReg_m(rRegs) */
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


.DoIKind_A:
	addi	r3, 0, 10
.BackupExceptionAndNext:
	addi	rPC_p, rPC_p, -2
	/* b	.ExceptionAndNext */
.ExceptionAndNext:
	rlwinm	r3, r3, 2, 0, 29
	bl	.get_long
	bl	.ExceptionTo
	b	.m68k_NextInstruction


.DoIKind_BsrB:
	mr	r8, rPC_p
	extsb	r4, rOpCode
	b	DoIKind_Bsr_Common


.DoIKind_BsrW:
	mr	r8, rPC_p
	lhau	r4, 2(rPC_p)
	/* b	DoIKind_Bsr_Common */

DoIKind_Bsr_Common:
	lwz	r3, fReg_A7(rRegs)
	add	r31, r8, r4
	addi	r8, r3, -4
	stw	r8, fReg_A7(rRegs)
	bl	.m68k_getpc
	mr	r4, r3
	mr	r3, r8
	bl	.put_long
	/*
		mr	r3, r31
		bl	.m68k_setpc
	*/
	mr	rPC_p, r31
	b	.m68k_NextInstruction


.DoIKind_Jsr:
	bl	.DecodeDst
	lwz	r30, fReg_A7(rRegs)
	bl	.m68k_getpc
	addi	r30, r30, -4
	mr	r4, r3
	stw	r30, fReg_A7(rRegs)
	mr	r3, r30
	bl	.put_long
	mr	r3, rArgAddr
	bl	.m68k_setpc
	b	.m68k_NextInstruction


.DoIKind_LinkA6:
	lwz	r3, fReg_A7(rRegs)
	addi	r3, r3, -4
	mr	r29, r3
	lwz	r4, fReg_A6(rRegs)
	bl	.put_long
	lhau	r3, 2(rPC_p)
	stw	r29, fReg_A6(rRegs)
	add	r0, r29, r3
	stw	r0, fReg_A7(rRegs)
	b	.m68k_NextInstruction


.DoIKind_UnlkA6:
	lwz	r3, fReg_A6(rRegs)
	addi	r29, r3, 4
	bl	.get_long
	stw	r3, fReg_A6(rRegs)
	stw	r29, fReg_A7(rRegs)
	b	.m68k_NextInstruction


.DoIKind_Rts:
	lwz	r3, fReg_A7(rRegs)
	addi	r30, r3, 4
	bl	.get_long
	stw	r30, fReg_A7(rRegs)
	bl	.m68k_setpc
	b	.m68k_NextInstruction


.DoIKind_Jmp:
	bl	.DecodeDst
	mr	r3, rArgAddr
	bl	.m68k_setpc
	b	.m68k_NextInstruction


.DoIKind_Clr:
	bl	.DecodeDst
	addi	r3, 0, 0
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	addi	rFlag_n, 0, 0
	addi	rFlag_z, 0, 1
	bl	.SetDstValue


.DoIKind_AddA:
	rlwinm	r3, rOpCode, 25, 30, 30
	addi	rOpSize, r3, 2
	bl	.DecodeNormModeRegister
	bl	.GetArgValue

	rlwinm	r0, rOpCode, 25, 27, 29
	add	r4, r0, rRegs
	lwz	r5, 32(r4)

	add	r5, r3, r5
	stw	r5, 32(r4)
	b	.m68k_NextInstruction


.octdat:
	/* result in r30, preserve r5-r7 */
	rlwinm	r30, rOpCode, 23, 29, 31
	addi	r3, r30, -1
	rlwimi	r30, r3, 0, 28, 28
	blr


.DoIKind_AddQA:
	rlwinm	r5, rOpCode, 0, 29, 31
	rlwinm	r6, r5, 2, 0, 29
	add	r6, r6, rRegs
	lwz	r7, 32(r6)
	bl	.octdat
	add	r7, r30, r7
	stw	r7, 32(r6)
	b	.m68k_NextInstruction


.DoIKind_SubA:
	rlwinm	rOpSize, rOpCode, 25, 30, 30
	addi	rOpSize, rOpSize, 2
	bl	.DecodeNormModeRegister
	bl	.GetArgValue

	rlwinm	r0, rOpCode, 25, 27, 29
	add	r4, r0, rRegs
	lwz	r5, 32(r4)

	subf	r5, r3, r5
	stw	r5, 32(r4)
	b	.m68k_NextInstruction


.DoIKind_SubQA:
	rlwinm	r5, rOpCode, 0, 29, 31
	rlwinm	r6, r5, 2, 0, 29
	add	r6, r6, rRegs
	lwz	r7, 32(r6)
	bl	.octdat
	subf	r7, r30, r7
	stw	r7, 32(r6)
	b	.m68k_NextInstruction


.DoIKind_CmpA:
	rlwinm	rOpSize, rOpCode, 25, 30, 30
	addi	rOpSize, rOpSize, 2
/* DoCompareA (m2 in r29) */
	bl	.DecodeNormModeRegister
	bl	.GetArgValue
	mr	r30, r3

	rlwinm	r3, rOpCode, 25, 27, 29
	add	r3, r3, rRegs
	lwz	r3, 32(r3)
	b	.DoCompare_r30r3


.DoIKind_AddXB:
	bl	.DecodeSrcDstGet

	addi	r3, r3, 1
	rlwinm	r3, r3, 24, 0, 7
	rlwinm	r30, r30, 24, 0, 7
	addi	r3, r3, -1

	addic	r5, rFlag_x, -1
		/* load carry flag */
	addeo	r4, r3, r30

	mtctr	rArgKind
	rlwinm	r3, r4, 8, 24, 31
	b	DoIKind_AddX_Common


.DoIKind_AddXL:
	bl	.DecodeSrcDstGet

	addic	r5, rFlag_x, -1
		/* load carry flag */
	addeo	r4, r3, r30

	mtctr	rArgKind
	mr	r3, r4

	mfxer	r6
	rlwinm	rFlag_c, r6, 3, 31, 31
	b	DoIKind_AddX_Common


.DoIKind_AddXW:
	bl	.DecodeSrcDstGet

	addi	r3, r3, 1
	rlwinm	r3, r3, 16, 0, 15
	rlwinm	r30, r30, 16, 0, 15
	addi	r3, r3, -1

	addic	r5, rFlag_x, -1
		/* load carry flag */
	addeo	r4, r3, r30

	mtctr	rArgKind
	rlwinm	r3, r4, 16, 16, 31
	/* b	DoIKind_AddX_Common */

DoIKind_AddX_Common:
	mfxer	r6
	cntlzw	r5, r3
	rlwinm	rFlag_n, r4, 1, 31, 31
	rlwinm	rFlag_c, r6, 3, 31, 31
	rlwinm	r10, r5, 27, 5, 31
	rlwinm	rFlag_v, r6, 2, 31, 31
	mr	rFlag_x, rFlag_c
	and	rFlag_z, rFlag_z, r10

	bctr


.DoIKind_SubXB:
	bl	.DecodeSrcDstGet

	rlwinm	r3, r3, 24, 0, 7
	rlwinm	r30, r30, 24, 0, 7

	xori	rFlag_x, rFlag_x, 1
	addic	r5, rFlag_x, -1
		/* load carry flag */
	subfeo	r4, r30, r3

	mtctr	rArgKind
	rlwinm	r3, r4, 8, 24, 31
	b	DoIKind_SubX_Common


.DoIKind_SubXL:
	bl	.DecodeSrcDstGet

	xori	rFlag_x, rFlag_x, 1
	addic	r5, rFlag_x, -1
		/* load carry flag */
	subfeo	r4, r30, r3

	mtctr	rArgKind
	mr	r3, r4
	b	DoIKind_SubX_Common


.DoIKind_SubXW:
	bl	.DecodeSrcDstGet

	rlwinm	r3, r3, 16, 0, 15
	rlwinm	r30, r30, 16, 0, 15

	xori	rFlag_x, rFlag_x, 1
	addic	r5, rFlag_x, -1
		/* load carry flag */
	subfeo	r4, r30, r3

	mtctr	rArgKind
	rlwinm	r3, r4, 16, 16, 31
	/* b	DoIKind_SubX_Common */

DoIKind_SubX_Common:
	mfxer	r6
	cntlzw	r5, r3
	rlwinm	rFlag_n, r4, 1, 31, 31
	rlwinm	rFlag_c, r6, 3, 31, 31
	rlwinm	r10, r5, 27, 5, 31
	xori	rFlag_c, rFlag_c, 1
	rlwinm	rFlag_v, r6, 2, 31, 31
	mr	rFlag_x, rFlag_c
	and	rFlag_z, rFlag_z, r10

	bctr


.FindOpSizeFromb76:
/* must preserve cr1 */
	rlwinm	r12, rOpCode, 26, 30, 31
	addi	rOpSize, 0, 1
	slw	rOpSize, rOpSize, r12
	blr


.SetCCRandArgValue:
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	cntlzw	r4, r3
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31
	/* b	.SetArgValue */
.SetArgValue:
	cmpi	cr1, rArgKind, 2
	cmpi	cr0, rOpSize, 2
	beq	cr1, .L262

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


.DoIKind_RolopNM:
	addi	rOpSize, 0, 2
	addi	r30, 0, 1
	bl	.DecodeNormModeRegister
	rlwinm	r3, rOpCode, 23, 29, 31
	b	.DoBinOp1notA


.DoIKind_RolopDD:
	bl	.FindOpSizeFromb76
	rlwinm	r4, rOpCode, 23, 29, 31
	bl	.SetArgKindDReg
	bl	.GetArgValue
	mr	r30, r3
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.SetArgKindDReg
	rlwinm	r3, rOpCode, 29, 30, 31
	b	.DoBinOp1notA


.DoIKind_RolopND:
	bl	.FindOpSizeFromb76
	bl	.octdat
	rlwinm	r4, rOpCode, 0, 29, 31
	bl	.SetArgKindDReg
	rlwinm	r3, rOpCode, 29, 30, 31
	/* b	.DoBinOp1notA */
.DoBinOp1notA:
	rlwinm	r3, r3, 1, 29, 30
	rlwimi	r3, rOpCode, 24, 31, 31
	xori	r31, r3, 1
	bl	.GetArgValue
	addi	r28, rDispatches, x_b2S9983
	rlwinm	r4, r31, 2, 0, 29
	lwzx	r4, r28, r4
	mtctr	r4
	bctr

.L380:
/* BinOpASL */
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
	addic	r0, r7, -1
	rlwinm	r4, r4, 27, 31, 31
	mr	rFlag_z, r4
	subfe	rFlag_v, r0, r7
	rlwinm	rFlag_n, r5, 25, 31, 31
.L701:
	mtcrf	56, r29
	b	.SetArgValue

.L423:
/* BinOpASR */
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
.L458:
/* BinOpLSL */
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
.L494:
/* BinOpLSR */
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
.L521:
	addi	r8, 0, 1
/* BinOpROL */
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
.L552:
/* BinOpRXL */
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
.L583:
	addi	r8, 0, 1
/* BinOpROR */
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
	addic	r5, r4, -1
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
.L623:
	addi	r8, 0, 1
/* BinOpRXR */
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
	addic	r4, r5, -1
	subfe	r7, r4, r5
.B2LP_9864:
	cmpi	cr0, r7, 0
	mr	r8, r3
	rlwinm	r5, r8, 0, 31, 31
	rlwinm	r3, r8, 31, 1, 31
	addic	r10, r5, -1
	subfe	r5, r10, r5
	bdnz	.B2LP_9880
	beq	cr0,.B2L_9877
	or	r3, r6, r3
.B2L_9877:
	mr	rFlag_c, r5
	rlwinm.	r4, r8, 0, 31, 31
	addic	r6, r4, -1
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


.DecodeModeRegister:
	addi	r5, rDispatches, x_b2S9987
	rlwinm	r6, r3, 2, 0, 29
	lwzx	r6, r5, r6
	mtctr	r6
	bctr


.DoIKind_BitOpDD:
.DoIKind_BitOpDM:
	addi	rOpSize, 0, 1
	rlwinm	r4, rOpCode, 23, 29, 31
	bl	.SetArgKindDReg
	b	.preDoBinBitOp1

.DoIKind_BitOpND:
.DoIKind_BitOpNM:
	addi	rOpSize, 0, 1
	bl	.SetArgKindImmediate
	/* b	.preDoBinBitOp1 */
.preDoBinBitOp1:
	rlwinm	r5, rOpCode, 29, 29, 31
	bl	.GetArgValue
	rlwinm	r7, rOpCode, 26, 30, 31
	addi	r7, r7, 17
/* DoBinBitOp1 (r2 in opcode) */
	mr	r30, r5
	mr	r29, r7
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
	or	r3, r30, r3
	b	.SetArgValue
.L818:
	xor	r3, r30, r3
	b	.SetArgValue
.L820:
	andc	r3, r3, r30
	b	.SetArgValue


.DoIKind_And:
	bl	.DecodeSrcDstGet

	and	r3, r30, r3

.SetCCRandArgDstValue:
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	cntlzw	r4, r3
	mtctr	rArgKind
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31
	bctr


.DoIKind_Or:
	bl	.DecodeSrcDstGet

	or	r3, r30, r3

	b	.SetCCRandArgDstValue


.DoIKind_Eor:
	bl	.DecodeSrcDstGet

	xor	r3, r30, r3

	b	.SetCCRandArgDstValue


.DoIKind_Not:
	bl	.DecodesDstGet

	nor	r3, r3, r3

	b	.SetCCRandArgDstValue


.DoIKind_Scc:
	addi	rOpSize, 0, 1
	bl	.DecodeNormModeRegister
	bl	.cctrue
	mr	r0, r3
	addic	r12, r0, -1
	subfe	r3, r12, r0
	neg	r3, r3
	rlwinm	r3, r3, 0, 24, 31
	b	.SetArgValue


.DoIKind_EXTL:
	rlwinm	r7, rOpCode, 2, 27, 29
	lwzx	r5, r7, rRegs
	extsh	r3, r5
	stwx	r3, r7, rRegs
	b	.SetCCRandNext


.DoIKind_EXTW:
	rlwinm	r7, rOpCode, 2, 27, 29
	lwzx	r5, r7, rRegs
	extsb	r3, r5
	rlwimi	r5, r3, 0, 16, 31
	stwx	r5, r7, rRegs
	b	.SetCCRandNext


.DoIKind_NegB:
	bl	.DecodesDstGet

	rlwinm	r3, r3, 24, 0, 7

	addi	r30, 0, 0
	subfco	r4, r3, r30

	mtctr	rArgKind
	rlwinm	r3, r4, 8, 24, 31
	b	DoIKind_Sub_common


.DoIKind_NegW:
	bl	.DecodesDstGet

	rlwinm	r3, r3, 16, 0, 15

	addi	r30, 0, 0
	subfco	r4, r3, r30

	mtctr	rArgKind
	rlwinm	r3, r4, 16, 16, 31
	b	DoIKind_Sub_common


.DoIKind_NegL:
	bl	.DecodesDstGet

	addi	r30, 0, 0
	subfco	r4, r3, r30

	mtctr	rArgKind
	mr	r3, r4
	b	DoIKind_Sub_common


.DoIKind_NegXB:
	bl	.DecodesDstGet

	rlwinm	r3, r3, 24, 0, 7

	xori	rFlag_x, rFlag_x, 1
	addic	r5, rFlag_x, -1
		/* load carry flag */
	subfzeo	r4, r3

	mtctr	rArgKind
	rlwinm	r3, r4, 8, 24, 31
	b	DoIKind_SubX_Common


.DoIKind_NegXW:
	bl	.DecodesDstGet

	rlwinm	r3, r3, 16, 0, 15

	xori	rFlag_x, rFlag_x, 1
	addic	r5, rFlag_x, -1
		/* load carry flag */
	subfzeo	r4, r3

	mtctr	rArgKind
	rlwinm	r3, r4, 16, 16, 31
	b	DoIKind_SubX_Common


.DoIKind_NegXL:
	bl	.DecodesDstGet

	xori	rFlag_x, rFlag_x, 1
	addic	r5, rFlag_x, -1
		/* load carry flag */
	subfzeo	r4, r3

	mtctr	rArgKind
	mr	r3, r4
	b	DoIKind_SubX_Common


.DoIKind_MulU:
	addi	rOpSize, 0, 2
	bl	.DecodeNormModeRegister
	bl	.GetArgValue
	mr	r29, r3
	addi	r3, 0, 0
	rlwinm	r4, rOpCode, 23, 29, 31
	bl	.DecodeModeRegister
	bl	.GetArgValue
	rlwinm	r8, r3, 0, 16, 31
	rlwinm	r4, r29, 0, 16, 31
	mullw	r3, r8, r4
.SetFlagsAndArgValL:
	addi	rOpSize, 0, 4
	b	.SetCCRandArgValue

.DoIKind_MulS:
	addi	rOpSize, 0, 2
	bl	.DecodeNormModeRegister
	bl	.GetArgValue
	mr	r29, r3
	addi	r3, 0, 0
	rlwinm	r4, rOpCode, 23, 29, 31
	bl	.DecodeModeRegister
	bl	.GetArgValue
	extsh	r8, r3
	extsh	r4, r29
	mullw	r3, r8, r4
	b	.SetFlagsAndArgValL

.DoIKind_DivU:
.DoIKind_DivS:
	rlwinm	r8, rOpCode, 24, 31, 31
	/* b8 in r8 */
	addic	r7, r8, -1
	rlwinm	r3, rOpCode, 29, 29, 31
	subfe	r31, r7, r8
	rlwinm	r4, rOpCode, 0, 29, 31
/* DoBinOpDiv1 (r2 in opcode, binop in r31) */
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
	b	.ExceptionAndNext

.DoIKind_Exgdd:
	rlwinm	r7, rOpCode, 25, 27, 29
	rlwinm	r9, rOpCode, 2, 27, 29

	lwzx	r0, r7, rRegs
	lwzx	r3, r9, rRegs
	stwx	r3, r7, rRegs
	stwx	r0, r9, rRegs
	b	.m68k_NextInstruction


.DoIKind_Exgaa:
	rlwinm	r7, rOpCode, 25, 27, 29
	add	r6, r7, rRegs
	rlwinm	r9, rOpCode, 2, 27, 29
	add	r5, r9, rRegs

	lwz	r4, 32(r6)
	lwz	r3, 32(r5)
	stw	r3, 32(r6)
	stw	r4, 32(r5)
	b	.m68k_NextInstruction


.DoIKind_Exgda:
	rlwinm	r4, rOpCode, 2, 27, 29
	add	r5, r4, rRegs
	rlwinm	r7, rOpCode, 25, 27, 29
	lwz	r6, 32(r5)
	lwzx	r3, r7, rRegs
	stwx	r6, r7, rRegs
	stw	r3, 32(r5)
	b	.m68k_NextInstruction


.m68k_setCR:
	rlwinm	rFlag_x, r3, 28, 31, 31
	rlwinm	rFlag_n, r3, 29, 31, 31
	rlwinm	rFlag_z, r3, 30, 31, 31
	rlwinm	rFlag_v, r3, 31, 31, 31
	rlwinm	rFlag_c, r3, 0, 31, 31
	blr


.NeedToGetOut:
	cmpi	cr0, rMaxCyclesToGo, 0
	blelr	cr0
	lwz	r0, fReg_MoreCyclesToGo(rRegs)
	add	r3, r0, rMaxCyclesToGo
	li	rMaxCyclesToGo, 0
	stw	r3, fReg_MoreCyclesToGo(rRegs)
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
	/*
		rlwinm	r6, r31, 20, 31, 31
		stb	r6, fReg_m(rRegs)
	*/
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
	/*
		bne	cr0,.L75
		addi	r6, 0, 0
		stb	r6, fReg_TracePend(rRegs)
		b	.B2LEP9988
		.L75:
	*/
	bl	.NeedToGetOut
.B2LEP9988:
	lwz	r0, 88(sp)
	addi	sp, sp, 80
	mtlr	r0
	lmw	r31, -24(sp)
	blr


.DoIKind_MoveCCREa:
	addi	rOpSize, 0, 2
	bl	.DecodeNormModeRegister
	bl	.m68k_getSR
	rlwinm	r3, r3, 0, 24, 31
	b	.SetArgValue


.DoIKind_MoveEaCCR:
	addi	rOpSize, 0, 2
	bl	.DecodeNormModeRegister
	bl	.GetArgValue
	rlwinm	r3, r3, 0, 16, 31
	bl	.m68k_setCR
	b	.m68k_NextInstruction


.DoIKind_MoveSREa:
	addi	rOpSize, 0, 2
	bl	.DecodeNormModeRegister
	bl	.m68k_getSR
	b	.SetArgValue


.DoIKind_MoveEaSR:
	addi	rOpSize, 0, 2
	bl	.DecodeNormModeRegister
	bl	.GetArgValue
	rlwinm	r3, r3, 0, 16, 31
	bl	.m68k_setSR
	b	.m68k_NextInstruction


.DoIKind_BinOpStatusCCR:
	rlwinm.	r29, rOpCode, 26, 30, 31
	beq	cr0,.L1148
	lbz	r28, fReg_s(rRegs)
	cmpi	cr0, r28, 0
	bne	cr0,.L1148
	addi	r3, 0, 8
	b	.BackupExceptionAndNext
.L1148:
	bl	.FindOpSizeFromb76

	/* r29=IsStatus */
	lhzu	r30, 2(rPC_p)
	/*
		sign extension would
		have no effect.
	*/

	bl	.m68k_getSR

	rlwinm	r31, rOpCode, 23, 29, 31
	cmpi	cr0, r31, 1
	bgt	cr0, .BinOpStatusCCR_Eor
		/* r31 must be 5 */
	blt	cr0, .BinOpStatusCCR_Or
		/* r31 must be 0 */

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


.DoIKind_MOVEMRmMW:
.DoIKind_MOVEMrm:
.DoIKind_MOVEMRmML:
	addi	r3, 0, 0
	b	.pre_reglist


.DoIKind_MOVEMApRW:
.DoIKind_MOVEMApRL:
.DoIKind_MOVEMmr:
	addi	r3, 0, 1
	b	.pre_reglist


.pre_reglist:
	rlwinm	r8, rOpCode, 27, 29, 30
	addi	rOpSize, r8, -2
/* .reglist: */
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


.DoIKind_Abcdr:
	addi	rOpSize, 0, 1
	addi	r3, 0, 0
	rlwinm	r4, rOpCode, 0, 29, 31
	addi	r5, 0, 0
	rlwinm	r6, rOpCode, 23, 29, 31
	mr	r29, r5
	mr	r28, r6
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r30, r3
	mr	r4, r28
	mr	r3, r29
	bl	.DecodeModeRegister

	b	.DoBinOpAbcd


.DoIKind_Abcdm:
	addi	rOpSize, 0, 1
	addi	r3, 0, 4
	rlwinm	r4, rOpCode, 0, 29, 31
	addi	r5, 0, 4
	rlwinm	r6, rOpCode, 23, 29, 31
	mr	r29, r5
	mr	r28, r6
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r30, r3
	mr	r4, r28
	mr	r3, r29
	bl	.DecodeModeRegister

	b	.DoBinOpAbcd


.DoBinOpAbcd:
	bl	.GetArgValue
/* BinOpAddBCD */
	mfcr	r29
	rlwinm	r6, r30, 0, 28, 31
	cmpi	cr6, r3, 0
	addic	r11, rFlag_x, -1
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


.DoIKind_Sbcdr:
	addi	rOpSize, 0, 1
	addi	r3, 0, 0
	rlwinm	r4, rOpCode, 0, 29, 31
	addi	r5, 0, 0
	rlwinm	r6, rOpCode, 23, 29, 31
	mr	r29, r5
	mr	r28, r6
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r30, r3
	mr	r4, r28
	mr	r3, r29
	bl	.DecodeModeRegister

	b	.DoBinOpSbcd


.DoIKind_Sbcdm:
	addi	rOpSize, 0, 1
	addi	r3, 0, 4
	rlwinm	r4, rOpCode, 0, 29, 31
	addi	r5, 0, 4
	rlwinm	r6, rOpCode, 23, 29, 31
	mr	r29, r5
	mr	r28, r6
	bl	.DecodeModeRegister
	bl	.GetArgValue
	mr	r30, r3
	mr	r4, r28
	mr	r3, r29
	bl	.DecodeModeRegister

	b	.DoBinOpSbcd


.DoBinOpSbcd:
	bl	.GetArgValue
/* BinOpSubBCD */
	mfcr	r29
	rlwinm	r4, r3, 0, 28, 31
	cmpi	cr6, r3, 0
	addic	r12, rFlag_x, -1
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


.DoIKind_Nbcd:
	addi	rOpSize, 0, 1
	bl	.DecodeNormModeRegister
	bl	.GetArgValue

	rlwinm	r10, r3, 0, 28, 31
	addic	r5, rFlag_x, -1
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
	beq	cr0,.SetArgValue
	addi	rFlag_z, 0, 0
	b	.SetArgValue


.DoIKind_Rte:
	lbz	r30, fReg_s(rRegs)
	cmpi	cr0, r30, 0
	bne	cr0, .L1534
	addi	r3, 0, 8
	b	.BackupExceptionAndNext
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


.DoIKind_MoveP:
	rlwinm	r3, rOpCode, 24, 31, 31
	rlwinm	r5, rOpCode, 29, 29, 31
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


.DoIKind_Illegal:
.DoIKind_CallMorRtm:
.op_illg:
	addi	r3, 0, 4
	b	.BackupExceptionAndNext


.DoIKind_ChkW:
	rlwinm	r8, rOpCode, 26, 30, 31

	/* b76 in r8 */
	subfic	rOpSize, r8, 4
	rlwinm	r3, rOpCode, 29, 29, 31
	rlwinm	r4, rOpCode, 0, 29, 31
/* DoCheck (r2 in opcode) */
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
	b	.ExceptionAndNext


.DoIKind_Trap:
	rlwinm	r3, rOpCode, 0, 28, 31
	addi	r3, r3, 32
	b	.ExceptionAndNext


.DoIKind_TrapV:
	cmpi	cr0, rFlag_v, 0
	beq	cr0, .m68k_NextInstruction
	addi	r3, 0, 7
	b	.ExceptionAndNext


.DoIKind_Rtr:
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


.DoIKind_Link:
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
	lhau	r3, 2(rPC_p)
	add	r0, r30, r3
	stw	r0, fReg_A7(rRegs)
	b	.m68k_NextInstruction


.DoIKind_Unlk:
	rlwinm	r0, rOpCode, 0, 29, 31
	cmpi	cr0, r0, 7
	beq	cr0, .L1483
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


.DoIKind_MoveRUSP:
	lbz	r0, fReg_s(rRegs)
	cmpi	cr0, r0, 0
	bne	cr0, .L1493
	addi	r3, 0, 8
	b	.BackupExceptionAndNext
.L1493:
	rlwinm	r30, rOpCode, 2, 27, 29
	add	r3, r30, rRegs
	lwz	r4, 32(r3)
	stw	r4, fReg_usp(rRegs)
	b	.m68k_NextInstruction


.DoIKind_MoveUSPR:
	lbz	r5, fReg_s(rRegs)
	cmpi	cr0, r5, 0
	bne	cr0, .L1502
	addi	r3, 0, 8
	b	.BackupExceptionAndNext
.L1502:
	lwz	r4, fReg_usp(rRegs)
	rlwinm	r30, rOpCode, 2, 27, 29
	add	r3, r30, rRegs
	stw	r4, 32(r3)
	b	.m68k_NextInstruction


.DoIKind_Tas:
	addi	rOpSize, 0, 1
	bl	.DecodeNormModeRegister
	bl	.GetArgValue

	cntlzw	r4, r3
	addi	rFlag_c, 0, 0
	addi	rFlag_v, 0, 0
	rlwinm	rFlag_n, r3, 1, 31, 31
	rlwinm	rFlag_z, r4, 27, 5, 31
	ori	r3, r3, 128
	b	.SetArgValue


.DoIKind_F:
	addi	r3, 0, 11
	b	.BackupExceptionAndNext


.DoIKind_Stop:
	lbz	r30, fReg_s(rRegs)
	cmpi	cr0, r30, 0
	bne	cr0, .L1525
	addi	r3, 0, 8
	b	.BackupExceptionAndNext
.L1525:
	lhzu	r3, 2(rPC_p)
	rlwinm	r3, r3, 0, 16, 31
	bl	.m68k_setSR
	b	.m68k_setstopped


.m68k_setstopped:
	addi	r3, 0, 4
	b	.ExceptionAndNext


.ReadInterruptPriorityLevel:
	lwz	r11,fIPL(rRegs)
	lbz	r3,0x0000(r11)
	blr


.DoCheckExternalInterruptPending:
	bl	.ReadInterruptPriorityLevel
	lwz	r5, fReg_intmask(rRegs)
	mr	r31, r3
	cmp	cr1, r31, r5
	bgt	cr1, .L119
	cmpi	cr0, r31, 7
	bne	cr0, .B2end9967
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
	/*
		So MaxCyclesToGo is accessible
		(and even modifiable), by the routine
		we will be calling.
	*/
	stw	rMaxCyclesToGo, fReg_MaxCyclesToGo(rRegs)
	blr


.MyEmulatorTempReturn:
/* must leave r3 unchanged */
	lwz	rMaxCyclesToGo, fReg_MaxCyclesToGo(rRegs)
	blr


.MyEmulatorEntry:
	my_load_loc_var_addr	rRegs, b2regs9999
	my_load_loc_var_addr	rDispatches, Dispatches

	lwz	rMaxCyclesToGo, fReg_MaxCyclesToGo(rRegs)

	lwz	rPC_p, fReg_pc_p(rRegs)
	lbz	rFlag_c, fReg_c(rRegs)
	lbz	rFlag_z, fReg_z(rRegs)
	lbz	rFlag_n, fReg_n(rRegs)
	lbz	rFlag_v, fReg_v(rRegs)
	lbz	rFlag_x, fReg_x(rRegs)

	blr


.MyEmulatorExit:
	stb	rFlag_x, fReg_x(rRegs)
	stb	rFlag_v, fReg_v(rRegs)
	stb	rFlag_n, fReg_n(rRegs)
	stb	rFlag_z, fReg_z(rRegs)
	stb	rFlag_c, fReg_c(rRegs)
	stw	rPC_p, fReg_pc_p(rRegs)
	stw	rMaxCyclesToGo, fReg_MaxCyclesToGo(rRegs)

	blr


.DoIKind_Reset:
	lbz	r10, fReg_s(rRegs)
	cmpi	cr0, r10, 0
	bne	cr0, .L1514
	addi	r3, 0, 8
	b	.BackupExceptionAndNext
.L1514:
	bl	.MyEmulatorExit
	my_call_imported_proc	customreset
	bl	.MyEmulatorEntry
	b	.m68k_NextInstruction

/*
	variables that may be changed during MM_Access
		regs.ExternalInterruptPending
		regs.intmask
		MoreCyclesToGo
		MaxCyclesToGo
		VIAInterruptRequest
*/


.callMemAccessNtfy:
	/*
		r5 - input (ATTep pT)
		r3 - preserved (addr)
		r4 - preserved (Data)
		r12 - preserved (caller's link)
		cr7 - test of result
	*/
	mflr	r0
	stw	r12, -4(sp)
	stw	r3, -8(sp)
	stw	r4, -12(sp)
	stw	r0, 8(sp)
	stwu	sp, -80(sp)

	bl	.MyEmulatorTempLeave
	mr	r3, r5
	my_call_imported_proc	MemAccessNtfy
	bl	.MyEmulatorTempReturn

	cmpwi	cr7,r3,0

	lwz	r0, 88(sp)
	addi	sp, sp, 80
	mtlr	r0
	lwz	r4, -12(sp)
	lwz	r3, -8(sp)
	lwz	r12, -4(sp)
	blr


.callMMDV_Access:
	/*
		r12 - preserved (caller's link)
	*/
	mflr	r0
	stw	r12, -4(sp)
	stw	r0, 8(sp)
	stwu	sp, -80(sp)

	bl	.MyEmulatorTempLeave
	my_call_imported_proc	MMDV_Access
	bl	.MyEmulatorTempReturn

	lwz	r0, 88(sp)
	addi	sp, sp, 80
	mtlr	r0
	lwz	r12, -4(sp)
	blr


.SetUpMATC:
	/*
		r5 - source, preserved
		r6 - destination
		r7 - OR to cmpmask
		r12 - preserved, required by callers
	*/
	lwz r0,4(r5)
	or	r0, r7, r0
	stw r0,0(r6)
	lwz r8,16(r5)
	stw r8,8(r6)
	lwz r0,8(r5)
	stw r0,4(r6)
	lwz r8,20(r5)
	stw r8,12(r6)
	blr


.FindATTel_loc:
	/*
		r3 - input (CPTR addr), preserved
		r5 - output (ATTep p)
		r4 - preserved, as required by callers
		r12 - preserved, as required by callers
	*/
	lwz	r5, fHeadATTel(rRegs)
	lwz	r0, 4(r5)
	lwz	r8, 8(r5)
	and	r0,r3,r0
	cmpw cr7,r0,r8
	beqlr cr7
L9:
	mr	r9, r5
	lwz r5,0(r5)
	lwz r0,4(r5)
	lwz r8,8(r5)
	and r0,r3,r0
	cmpw cr7,r0,r8
	bne cr7,L9
L5:
	lwz r0,0(r5)
	cmpwi cr7,r0,0
	beqlr cr7
	stw r0,0(r9)
	lwz r0,fHeadATTel(rRegs)
	stw r0,0(r5)
	stw r5,fHeadATTel(rRegs)
	blr


.get_byte_ext:
	mflr	r12

get_byte_ext_retry:
	bl	.FindATTel_loc
	lwz	r0, 12(r5)
	andi.	r8,r0,1
	beq	cr0,get_byte_ext_L51

	addi	r6, rRegs, fReg_MATCrdB_cmpmask
	addi	r7, 0, 0
	bl	.SetUpMATC

	lwz	r8, 16(r5)
	lwz	r9, 20(r5)
	and	r8, r3, r8
	lbzx	r3, r9, r8

get_byte_ext_L58:
	mtlr	r12
	extsb	r3, r3
	blr

get_byte_ext_L51:
	andi.	r8,r0,4
	beq	cr0,get_byte_ext_L52

	mr	r7, r3
	mr	r3, r5
	li	r4,0
	li	r5,0
	li	r6,1
	bl	.callMMDV_Access

	b	get_byte_ext_L58
get_byte_ext_L52:
	andi.	r8,r0,8
	beq	cr0, get_byte_ext_L54

	bl	.callMemAccessNtfy
	bne	cr7, get_byte_ext_retry
	/* b	get_byte_ext_L54 */

get_byte_ext_L54:
	li	r3,0
	b	get_byte_ext_L58


.put_byte_ext:
	mflr	r12

put_byte_ext_retry:
	bl	.FindATTel_loc
	lwz	r0, 12(r5)
	andi.	r8,r0,2
	beq	cr0,put_byte_ext_L51

	addi	r6, rRegs, fReg_MATCwrB_cmpmask
	addi	r7, 0, 0
	bl	.SetUpMATC

	lwz	r8, 16(r5)
	lwz	r9, 20(r5)
	and	r8, r3, r8
	stbx	r4, r9, r8

put_byte_ext_L58:
	mtlr	r12
	blr

put_byte_ext_L51:
	andi.	r8,r0,4
	beq	cr0,put_byte_ext_L52

	mr	r7, r3
	mr	r3, r5
	/* r4 is value to put */
	li	r5,1
	li	r6,1
	bl	.callMMDV_Access

	b	put_byte_ext_L58
put_byte_ext_L52:
	andi.	r8,r0,8
	beq	cr0, put_byte_ext_L54

	bl	.callMemAccessNtfy
	bne	cr7, put_byte_ext_retry
	/* b	put_byte_ext_L54 */

put_byte_ext_L54:
	b	put_byte_ext_L58


.get_word_ext:
	andi.	r8,r3,1
	mflr	r12
	bne	cr0, get_word_ext_misaligned

get_word_ext_retry:

	bl	.FindATTel_loc
	lwz	r0, 12(r5)
	andi.	r8,r0,1
	beq	cr0,get_word_ext_L51

	addi	r6, rRegs, fReg_MATCrdW_cmpmask
	addi	r7, 0, 1
	bl	.SetUpMATC

	lwz	r8, 16(r5)
	lwz	r9, 20(r5)
	and	r8, r3, r8
	lhax	r3, r9, r8

get_word_ext_L58:
	mtlr	r12
	blr

get_word_ext_L51:
	andi.	r8,r0,4
	beq	cr0,get_word_ext_L52

	mr	r7, r3
	mr	r3, r5
	li	r4,0
	li	r5,0
	li	r6,0
	bl	.callMMDV_Access

	extsh	r3, r3

	b	get_word_ext_L58
get_word_ext_L52:
	andi.	r8,r0,8
	beq	cr0, get_word_ext_L54

	bl	.callMemAccessNtfy
	bne	cr7, get_word_ext_retry
	/* b	get_word_ext_L54 */

get_word_ext_L54:
	li	r3,0
	b	get_word_ext_L58

get_word_ext_misaligned:
	stmw	r30, -8(sp)
	stw	r12, 8(sp)
	stwu	sp, -80(sp)
	mr	r31, r3
	bl	.get_byte
	mr	r30, r3
	addi	r3, r31, 1
	bl	.get_byte
	rlwimi	r3, r30, 8, 0, 23
	lwz	r12, 88(sp)
	addi	sp, sp, 80
	lmw	r30, -8(sp)
	b	get_word_ext_L58


.put_word_ext:
	andi.	r8,r3,1
	mflr	r12
	bne	cr0, put_word_ext_misaligned

put_word_ext_retry:
	bl	.FindATTel_loc
	lwz	r0, 12(r5)
	andi.	r8,r0,2
	beq	cr0,put_word_ext_L51

	addi	r6, rRegs, fReg_MATCwrW_cmpmask
	addi	r7, 0, 1
	bl	.SetUpMATC

	lwz	r8, 16(r5)
	lwz	r9, 20(r5)
	and	r8, r3, r8
	sthx	r4, r9, r8

put_word_ext_L58:
	mtlr	r12
	blr

put_word_ext_L51:
	andi.	r8,r0,4
	beq	cr0,put_word_ext_L52

	mr	r7, r3
	mr	r3, r5
	/* r4 is value to put */
	li	r5,1
	li	r6,0
	bl	.callMMDV_Access

	b	put_word_ext_L58
put_word_ext_L52:
	andi.	r8,r0,8
	beq	cr0, put_word_ext_L54

	bl	.callMemAccessNtfy
	bne	cr7, put_word_ext_retry
	/* b	put_word_ext_L54 */

put_word_ext_L54:
	b	put_word_ext_L58

put_word_ext_misaligned:
	stmw	r30, -8(sp)
	stw	r12, 8(sp)
	stwu	sp, -80(sp)
	mr	r30, r4
	mr	r31, r3
	rlwinm	r4, r4, 24, 24, 31
	bl	.put_byte
	addi	r3, r31, 1
	rlwinm	r4, r30, 0, 24, 31
	bl	.put_byte
	lwz	r12, 88(sp)
	addi	sp, sp, 80
	lmw	r30, -8(sp)
	b	put_word_ext_L58


.get_long_ext:
	mflr	r12
	stmw	r30, -8(sp)
	stw	r12, 8(sp)
	stwu	sp, -80(sp)
	mr	r31, r3
	bl	.get_word
	mr	r30, r3
	addi	r3, r31, 2
	bl	.get_word
	rlwimi	r3, r30, 16, 0, 15
	lwz	r12, 88(sp)
	addi	sp, sp, 80
	mtlr	r12
	lmw	r30, -8(sp)
	blr


.put_long_ext:
	mflr	r12
	stmw	r30, -8(sp)
	stw	r12, 8(sp)
	stwu	sp, -80(sp)
	mr	r30, r4
	mr	r31, r3
	rlwinm	r4, r30, 16, 16, 31
	bl	.put_word
	addi	r3, r31, 2
	rlwinm	r4, r30, 0, 16, 31
	bl	.put_word
	lwz	r12, 88(sp)
	addi	sp, sp, 80
	mtlr	r12
	lmw	r30, -8(sp)
	blr


.get0_pc_real_address_ext:
	mflr	r12

L105:
	bl	.FindATTel_loc
	lwz	r0, 12(r5)
	andi. r8,r0,1
	beq cr0, L107

	addi	r6, rRegs, fReg_MATCex_cmpmask
	addi	r7, 0, 0
	bl	.SetUpMATC

	lwz	r0,16(r5)
	lwz	r8,20(r5)
	and	r0,r3,r0
	add	rPC_p,r0,r8

L111:
	mtlr	r12
	b	.end_get0_pc_real_address

L107:
	andi. r8,r0,8
	beq cr0,L109

	bl	.callMemAccessNtfy
	bne cr7, L105
	/* b L109 */

L109:
	addi	rPC_p, rRegs, fReg_fakeword
	li r8,0
	li r0,-1
	stw	r8, fReg_MATCex_cmpmask(rRegs)
	stw r0, fReg_MATCex_cmpvalu(rRegs)
	b L111


	my_define_exported_proc	m68k_IPLchangeNtfy
	my_load_loc_var_addr	r6, b2regs9999
	lwz	r11, fIPL(r6)
	lwz	r4, fReg_intmask(r6)
	lbz	r3, 0x0000(r11)
	cmp	cr1, r3, r4
	bgt	cr1, .L126
	cmpi	cr0, r3, 7
	bnelr	cr0
.L126:
	lwz	r12, fReg_MaxCyclesToGo(r6)
	addi	r3, 0, 1
	stb	r3, fReg_ExtIntPend(r6)
	cmpi	cr0, r12, 0
	beqlr	cr0
	lwz	r3, fReg_MoreCyclesToGo(r6)
	addi	r4, 0, 1
	stw	r4, fReg_MaxCyclesToGo(r6)
	add	r3, r3, r12
	addi	r3, r3, -1
	stw	r3, fReg_MoreCyclesToGo(r6)
	blr


	my_define_exported_proc	GetCyclesRemaining
	my_load_loc_var_addr	r6, b2regs9999
	lwz	r3,fReg_MoreCyclesToGo(r6)
	lwz	r5,fReg_MaxCyclesToGo(r6)
	add	r3,r3,r5
	blr


	my_define_exported_proc	SetCyclesRemaining
	my_load_loc_var_addr	r6, b2regs9999
	lwz	r5,fReg_MaxCyclesToGo(r6)
	cmplw	r5,r3
	blt	.L1_SetCyclesRemaining
	li	r0,0
	stw	r3,fReg_MaxCyclesToGo(r6)
	stw	r0,fReg_MoreCyclesToGo(r6)
	blr
.L1_SetCyclesRemaining:
	sub	r4,r3,r5
	stw	r4,fReg_MoreCyclesToGo(r6)
	blr


	my_define_exported_proc	DiskInsertedPsuedoException
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -144(sp)

	bl	.MyEmulatorEntry

	mr	r31, r4
	bl	.ExceptionTo
	lwz	r3, fReg_A7(rRegs)
	mr	r4, r31
	addi	r3, r3, -4
	stw	r3, fReg_A7(rRegs)
	bl	.put_long
	bl	.MyEmulatorExit

	lwz	r0, 152(sp)
	addi	sp, sp, 144
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	my_define_exported_proc	m68k_reset
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -144(sp)

	bl	.MyEmulatorEntry

	addi	rMaxCyclesToGo, 0, 0
	stw	rMaxCyclesToGo, fReg_MoreCyclesToGo(rRegs)
	stw	rMaxCyclesToGo, fReg_ResidualCycles(rRegs)

	li	r3, 0x4AFC
	sth	r3, fReg_fakeword(rRegs)

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
	/* stb	r30, fReg_m(rRegs) */
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

	lwz	r0, 152(sp)
	addi	sp, sp, 144
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	my_define_exported_proc	MINEM68K_Init
	mflr	r0
	stw	r0, 8(sp)
	stwu	sp, -64(sp)

	my_load_loc_var_addr	r9, b2regs9999

#if 0
	addi	r0, r9, fReg_MATCrd_cmpmask
	stw	r0, 0(r3)
	addi	r0, r9, fReg_MATCwr_cmpmask
	stw	r0, 0(r4)
	addi	r0, r9, fReg_MATCex_cmpmask
	stw	r0, 0(r5)
#endif

	stw	r3, fIPL(r9)

	addi	r3, r9, fdisp_table
	my_call_imported_proc	M68KITAB_setup

	lwz	r0, 72(sp)
	addi	sp, sp, 64
	mtlr	r0
	blr


	my_define_exported_proc	get_vm_byte
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -144(sp)

	bl	.MyEmulatorEntry

	bl	.get_byte
	rlwinm	r3, r3, 0, 0xff

	bl	.MyEmulatorExit

	lwz	r0, 152(sp)
	addi	sp, sp, 144
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	my_define_exported_proc	get_vm_word
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -144(sp)

	bl	.MyEmulatorEntry

	bl	.get_word
	rlwinm	r3,r3,0,0xffff

	bl	.MyEmulatorExit

	lwz	r0, 152(sp)
	addi	sp, sp, 144
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	my_define_exported_proc	get_vm_long
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -144(sp)

	bl	.MyEmulatorEntry

	bl	.get_long

	bl	.MyEmulatorExit

	lwz	r0, 152(sp)
	addi	sp, sp, 144
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	my_define_exported_proc	put_vm_byte
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -144(sp)

	bl	.MyEmulatorEntry

	extsb	r4, r4
	bl	.put_byte

	bl	.MyEmulatorExit

	lwz	r0, 152(sp)
	addi	sp, sp, 144
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	my_define_exported_proc	put_vm_word
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -144(sp)

	bl	.MyEmulatorEntry

	extsh	r4, r4
	bl	.put_word

	bl	.MyEmulatorExit

	lwz	r0, 152(sp)
	addi	sp, sp, 144
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	my_define_exported_proc	put_vm_long
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -144(sp)

	bl	.MyEmulatorEntry

	bl	.put_long

	bl	.MyEmulatorExit

	lwz	r0, 152(sp)
	addi	sp, sp, 144
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	my_define_exported_proc	SetHeadATTel
	my_load_loc_var_addr	r6, b2regs9999

	stw r3,fHeadATTel(r6)

	li r9,0
	li r0,-1

	stw r9,fReg_MATCrdB_cmpmask(r6)
	stw r0,fReg_MATCrdB_cmpvalu(r6)
	stw r9,fReg_MATCwrB_cmpmask(r6)
	stw r0,fReg_MATCwrB_cmpvalu(r6)

	stw r9,fReg_MATCrdW_cmpmask(r6)
	stw r0,fReg_MATCrdW_cmpvalu(r6)
	stw r9,fReg_MATCwrW_cmpmask(r6)
	stw r0,fReg_MATCwrW_cmpvalu(r6)

	stw r9,fReg_MATCex_cmpmask(r6)
	stw r0,fReg_MATCex_cmpvalu(r6)

	blr


	my_define_exported_proc	FindATTel
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -144(sp)

	bl	.MyEmulatorEntry

	bl	.FindATTel_loc
	mr	r3, r5

	bl	.MyEmulatorExit

	lwz	r0, 152(sp)
	addi	sp, sp, 144
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr


	my_define_exported_proc	m68k_go_nCycles
	mflr	r0
	stmw	rPC_p, -76(sp)
	stw	r0, 8(sp)
	stwu	sp, -144(sp)

	bl	.MyEmulatorEntry

	lwz	r4, fReg_ResidualCycles(rRegs)
	add	r3, r4, r3

	b	.m68k_go_nCycles_L0

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

	/* b	.m68k_NextInstruction */

	lhzu	rOpCode, 2(rPC_p)
	addi	r5, rRegs, fdisp_table
	addi	r3, rRegs, fdisp_table + 4

	rlwinm	r4, rOpCode, 3, 0, 28

	lwzx	rDecOpA, r4, r5
	lwzx	rDecOpB, r4, r3

	rlwinm	r3, rDecOpA, 2, 14, 29
	rlwinm	r4, rDecOpB, 0, 16, 31

	lwzx	r3, rDispatches, r3
	mtctr	r3

	sub	rMaxCyclesToGo, rMaxCyclesToGo, r4

	bctr

.after_m68k_go_MaxCycles:
	addi	rPC_p, rPC_p, -2

	addi	r12, 0, 0
	lwz	r3, fReg_MoreCyclesToGo(rRegs)
	stw	r12, fReg_MoreCyclesToGo(rRegs)
.m68k_go_nCycles_L0:
	add	rMaxCyclesToGo, r3, rMaxCyclesToGo
	cmpi	cr0, rMaxCyclesToGo, 0
	bgt	cr0,.B2LP_9657

	stw	rMaxCyclesToGo, fReg_ResidualCycles(rRegs)
	addi	rMaxCyclesToGo, 0, 0

	bl	.MyEmulatorExit

	lwz	r0, 152(sp)
	addi	sp, sp, 144
	mtlr	r0
	lmw	rPC_p, -76(sp)
	blr

	my_code_end
