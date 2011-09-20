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

	Intel assembler version of MINEM68K.c
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
	my_import_proc M68KITAB_setup
	my_import_proc customreset
	my_import_proc MMDV_Access
	my_import_proc MemAccessNtfy
	my_imports_end


/* dispatch table */

	my_dispatch_table_begin	L647

	my_dispatch_entry	DoIKind_Tst
	my_dispatch_entry	DoIKind_CmpB
	my_dispatch_entry	DoIKind_CmpW
	my_dispatch_entry	DoIKind_CmpL
	my_dispatch_entry	DoIKind_BccB
	my_dispatch_entry	DoIKind_BccW
	my_dispatch_entry	DoIKind_BraB
	my_dispatch_entry	DoIKind_BraW
	my_dispatch_entry	DoIKind_DBcc
	my_dispatch_entry	DoIKind_DBF
	my_dispatch_entry	DoIKind_Swap
	my_dispatch_entry	DoIKind_Move
	my_dispatch_entry	DoIKind_Move
	my_dispatch_entry	DoIKind_Move
	my_dispatch_entry	DoIKind_MoveAL
	my_dispatch_entry	DoIKind_MoveAW
	my_dispatch_entry	DoIKind_MoveQ
	my_dispatch_entry	DoIKind_AddB
	my_dispatch_entry	DoIKind_AddW
	my_dispatch_entry	DoIKind_AddL
	my_dispatch_entry	DoIKind_SubB
	my_dispatch_entry	DoIKind_SubW
	my_dispatch_entry	DoIKind_SubL
	my_dispatch_entry	DoIKind_Lea
	my_dispatch_entry	DoIKind_PEA
	my_dispatch_entry	DoIKind_A
	my_dispatch_entry	DoIKind_BsrB
	my_dispatch_entry	DoIKind_BsrW
	my_dispatch_entry	DoIKind_Jsr
	my_dispatch_entry	DoIKind_LinkA6
	my_dispatch_entry	DoIKind_MOVEMRmML
	my_dispatch_entry	DoIKind_MOVEMApRL
	my_dispatch_entry	DoIKind_UnlkA6
	my_dispatch_entry	DoIKind_Rts
	my_dispatch_entry	DoIKind_Jmp
	my_dispatch_entry	DoIKind_Clr
	my_dispatch_entry	DoIKind_AddA
	my_dispatch_entry	DoIKind_AddQA
	my_dispatch_entry	DoIKind_SubA
	my_dispatch_entry	DoIKind_SubQA
	my_dispatch_entry	DoIKind_CmpA
	my_dispatch_entry	DoIKind_AddXB
	my_dispatch_entry	DoIKind_AddXW
	my_dispatch_entry	DoIKind_AddXL
	my_dispatch_entry	DoIKind_SubXB
	my_dispatch_entry	DoIKind_SubXW
	my_dispatch_entry	DoIKind_SubXL
	my_dispatch_entry	DoIKind_RolopNM
	my_dispatch_entry	DoIKind_RolopND
	my_dispatch_entry	DoIKind_RolopDD
	my_dispatch_entry	DoIKind_BitOpDD
	my_dispatch_entry	DoIKind_BitOpDM
	my_dispatch_entry	DoIKind_BitOpND
	my_dispatch_entry	DoIKind_BitOpNM
	my_dispatch_entry	DoIKind_And
	my_dispatch_entry	DoIKind_And
	my_dispatch_entry	DoIKind_And
	my_dispatch_entry	DoIKind_Or
	my_dispatch_entry	DoIKind_Or
	my_dispatch_entry	DoIKind_Or
	my_dispatch_entry	DoIKind_Eor
	my_dispatch_entry	DoIKind_Eor
	my_dispatch_entry	DoIKind_Not
	my_dispatch_entry	DoIKind_Scc
	my_dispatch_entry	DoIKind_NegXB
	my_dispatch_entry	DoIKind_NegXW
	my_dispatch_entry	DoIKind_NegXL
	my_dispatch_entry	DoIKind_NegB
	my_dispatch_entry	DoIKind_NegW
	my_dispatch_entry	DoIKind_NegL
	my_dispatch_entry	DoIKind_EXTW
	my_dispatch_entry	DoIKind_EXTL
	my_dispatch_entry	DoIKind_MulU
	my_dispatch_entry	DoIKind_MulS
	my_dispatch_entry	DoIKind_DivU
	my_dispatch_entry	DoIKind_DivS
	my_dispatch_entry	DoIKind_Exgdd
	my_dispatch_entry	DoIKind_Exgaa
	my_dispatch_entry	DoIKind_Exgda
	my_dispatch_entry	DoIKind_MoveCCREa
	my_dispatch_entry	DoIKind_MoveEaCCR
	my_dispatch_entry	DoIKind_MoveSREa
	my_dispatch_entry	DoIKind_MoveEaSR
	my_dispatch_entry	DoIKind_BinOpStatusCCR
	my_dispatch_entry	DoIKind_MOVEMApRW
	my_dispatch_entry	DoIKind_MOVEMRmMW
	my_dispatch_entry	DoIKind_MOVEMrm
	my_dispatch_entry	DoIKind_MOVEMmr
	my_dispatch_entry	DoIKind_Abcdr
	my_dispatch_entry	DoIKind_Abcdm
	my_dispatch_entry	DoIKind_Sbcdr
	my_dispatch_entry	DoIKind_Sbcdm
	my_dispatch_entry	DoIKind_Nbcd
	my_dispatch_entry	DoIKind_Rte
	my_dispatch_entry	DoIKind_Nop
	my_dispatch_entry	DoIKind_MoveP
	my_dispatch_entry	DoIKind_Illegal
	my_dispatch_entry	DoIKind_ChkW
	my_dispatch_entry	DoIKind_Trap
	my_dispatch_entry	DoIKind_TrapV
	my_dispatch_entry	DoIKind_Rtr
	my_dispatch_entry	DoIKind_Link
	my_dispatch_entry	DoIKind_Unlk
	my_dispatch_entry	DoIKind_MoveRUSP
	my_dispatch_entry	DoIKind_MoveUSPR
	my_dispatch_entry	DoIKind_Tas
	my_dispatch_entry	DoIKind_F
	my_dispatch_entry	DoIKind_CallMorRtm
	my_dispatch_entry	DoIKind_Stop
	my_dispatch_entry	DoIKind_Reset

L359:
	my_dispatch_entry	SetArgKindDReg
	my_dispatch_entry	SetArgKindAReg
	my_dispatch_entry	SetArgKind2
	my_dispatch_entry	SetArgKindPostInc
	my_dispatch_entry	SetArgKindPreDec
	my_dispatch_entry	SetArgKind5
	my_dispatch_entry	SetArgKind6
	my_dispatch_entry	SetArgKind7
	my_dispatch_entry	SetArgKind8

L374:
	my_dispatch_entry	SetArgKind7_0
	my_dispatch_entry	SetArgKind7_1
	my_dispatch_entry	SetArgKind7_2
	my_dispatch_entry	SetArgKind7_3
	my_dispatch_entry	SetArgKind7_4

movl_table:
	my_dispatch_entry	cctrue_T
	my_dispatch_entry	cctrue_F
	my_dispatch_entry	cctrue_HI
	my_dispatch_entry	cctrue_LS
	my_dispatch_entry	cctrue_CC
	my_dispatch_entry	cctrue_CS
	my_dispatch_entry	cctrue_NE
	my_dispatch_entry	cctrue_EQ
	my_dispatch_entry	cctrue_VC
	my_dispatch_entry	cctrue_VS
	my_dispatch_entry	cctrue_PL
	my_dispatch_entry	cctrue_MI
	my_dispatch_entry	cctrue_GE
	my_dispatch_entry	cctrue_LT
	my_dispatch_entry	cctrue_GT
	my_dispatch_entry	cctrue_LE

L161:
	my_dispatch_entry	L153
	my_dispatch_entry	L154
	my_dispatch_entry	L155
	my_dispatch_entry	L156
	my_dispatch_entry	L157
	my_dispatch_entry	L158
	my_dispatch_entry	L159
	my_dispatch_entry	L160

L_dsp_DecodeSrcDst:
	my_dispatch_entry	DecodeSrcDstAMdReg
	my_dispatch_entry	DecodeSrcDstAMdIndirect
	my_dispatch_entry	DecodeSrcDstAMdAPosIncB
	my_dispatch_entry	DecodeSrcDstAMdAPosIncW
	my_dispatch_entry	DecodeSrcDstAMdAPosIncL
	my_dispatch_entry	DecodeSrcDstAMdAPreDecB
	my_dispatch_entry	DecodeSrcDstAMdAPreDecW
	my_dispatch_entry	DecodeSrcDstAMdAPreDecL
	my_dispatch_entry	DecodeSrcDstAMdADisp
	my_dispatch_entry	DecodeSrcDstAMdAIndex
	my_dispatch_entry	DecodeSrcDstAMdAbsW
	my_dispatch_entry	DecodeSrcDstAMdAbsL
	my_dispatch_entry	DecodeSrcDstAMdPCDisp
	my_dispatch_entry	DecodeSrcDstAMdPCIndex
	my_dispatch_entry	DecodeSrcDstAMdImmedB
	my_dispatch_entry	DecodeSrcDstAMdImmedW
	my_dispatch_entry	DecodeSrcDstAMdImmedL
	my_dispatch_entry	DecodeSrcDstAMdDat4

L_dsp_GetSrcDstValue:
	my_dispatch_entry	GetSrcDstValRegB
	my_dispatch_entry	GetSrcDstValRegW
	my_dispatch_entry	GetSrcDstValRegL
	my_dispatch_entry	_get_byte
	my_dispatch_entry	_get_word
	my_dispatch_entry	_get_long
	my_dispatch_entry	GetSrcDstValCnst

L_dsp_SetDstValue:
	my_dispatch_entry	SetDstValRegB
	my_dispatch_entry	SetDstValRegW
	my_dispatch_entry	SetDstValRegL
	my_dispatch_entry	SetDstValMemB
	my_dispatch_entry	SetDstValMemW
	my_dispatch_entry	SetDstValMemL
	my_dispatch_entry	SetDstValCnst

#if 0
L_dsp_DecodeCompatSrcDst:
	my_dispatch_entry	DecodeCompatSrcDstAMdReg
	my_dispatch_entry	DecodeCompatSrcDstAMdIndirect
	my_dispatch_entry	DecodeCompatSrcDstAMdAPosIncB
	my_dispatch_entry	DecodeCompatSrcDstAMdAPosIncW
	my_dispatch_entry	DecodeCompatSrcDstAMdAPosIncL
	my_dispatch_entry	DecodeCompatSrcDstAMdAPreDecB
	my_dispatch_entry	DecodeCompatSrcDstAMdAPreDecW
	my_dispatch_entry	DecodeCompatSrcDstAMdAPreDecL
	my_dispatch_entry	DecodeCompatSrcDstAMdADisp
	my_dispatch_entry	DecodeCompatSrcDstAMdAIndex
	my_dispatch_entry	DecodeCompatSrcDstAMdAbsW
	my_dispatch_entry	DecodeCompatSrcDstAMdAbsL
	my_dispatch_entry	DecodeCompatSrcDstAMdPCDisp
	my_dispatch_entry	DecodeCompatSrcDstAMdPCIndex
	my_dispatch_entry	DecodeCompatSrcDstAMdImmedB
	my_dispatch_entry	DecodeCompatSrcDstAMdImmedW
	my_dispatch_entry	DecodeCompatSrcDstAMdImmedL
	my_dispatch_entry	DecodeCompatSrcDstAMdDat4
#endif

	my_dispatch_table_end


	my_code_begin


DoIKind_CmpB:
	call	DecodeSrcDstGet
	cmpb	%bl, %al
	jmp	_DoCompare_Vals


DoIKind_CmpL:
	call	DecodeSrcDstGet
_DoCompare_L:
	cmpl	%ebx, %eax
	jmp	_DoCompare_Vals


DoIKind_CmpW:
	call	DecodeSrcDstGet
	cmpw	%bx, %ax
	/* jmp	_DoCompare_Vals */


_DoCompare_Vals:
	seto	_regs+fReg_v
	setc	_regs+fReg_c
	/* jmp	_SetNZandNext */

_SetNZandNext:
	sets	_regs+fReg_n
	sete	_regs+fReg_z
	/* jmp	m68k_NextInstruction */

DoIKind_Nop:
m68k_NextInstruction:
	movl	_regs+fReg_pc_p, %edi
m68k_NextInstruction_di:
	cmpl	$0, _regs+fReg_MaxCyclesToGo
	movzwl	(%edi), %eax
	jle	L544a
L544:
	rolw	$8, %ax
	leal	_regs+fdisp_table(,%eax, 8), %ebx
	movl	%eax, %esi
	movl	(%ebx), %ecx
	movl	4(%ebx), %edx
	add		$2, %edi
	movzwl	%cx, %ebx
	movzwl	%dx, %eax
	movl	L647(,%ebx,4), %ebx
	subl	%eax, _regs+fReg_MaxCyclesToGo
	movl	%edi, _regs+fReg_pc_p
	jmp	*%ebx
m68k_NextInstruction_d:
	movl	%edx, _regs+fReg_pc_p
	movl	%edx, %edi
	jmp	m68k_NextInstruction_di


/*
	_DecodeSrc/_DecodeDst

	%eax - result
	might normally follow with
		movl	%eax, _regs+fReg_ArgAddr
*/

#if 0
_DecodeSrc:
	movl	_regs+fDecOpB, %edx
	movl	%edx, %eax
	shrl	$14, %eax
	andl	$0x7C, %eax
	jmp	*L_dsp_DecodeSrcDst(%eax)
#endif

#if 0
_DecodeDst:
	movl	_regs+fDecOpA, %edx
	movl	%edx, %eax
	jmp	_DecodeDst_0
#endif


DecodeSrcDstAMdReg:
	shrl	$26, %edx
	andl	$0x3C, %edx
	leal	_regs(%edx), %eax
	ret

DecodeSrcDstAMdIndirect:
	shrl	$26, %edx
	andl	$0x3C, %edx
	movl	_regs(%edx), %eax
	ret

DecodeSrcDstAMdAPosIncX:
	shrl	$26, %edx
	andl	$0x3C, %edx
	movl	_regs(%edx), %eax
	addl	%eax, %ecx
	movl	%ecx, _regs(%edx)
	ret

DecodeSrcDstAMdAPosIncB:
	movl	$1, %ecx
	jmp	DecodeSrcDstAMdAPosIncX

DecodeSrcDstAMdAPosIncW:
	movl	$2, %ecx
	jmp	DecodeSrcDstAMdAPosIncX

DecodeSrcDstAMdAPosIncL:
	movl	$4, %ecx
	jmp	DecodeSrcDstAMdAPosIncX

DecodeSrcDstAMdAPreDecX:
	shrl	$26, %edx
	andl	$0x3C, %edx
	leal	_regs(%edx), %edx
	movl	(%edx), %eax
	subl	%ecx, %eax
	movl	%eax, (%edx)
	ret

DecodeSrcDstAMdAPreDecB:
	movl	$1, %ecx
	jmp	DecodeSrcDstAMdAPreDecX

DecodeSrcDstAMdAPreDecW:
	movl	$2, %ecx
	jmp	DecodeSrcDstAMdAPreDecX

DecodeSrcDstAMdAPreDecL:
	movl	$4, %ecx
	jmp	DecodeSrcDstAMdAPreDecX

DecodeSrcDstAMdADisp:
	shrl	$26, %edx
	andl	$0x3C, %edx
	movl	_regs+fReg_pc_p, %ecx
	movl	_regs(%edx), %eax
	movzwl	(%ecx), %edx
	rolw	$8, %dx
	addl	$2, %ecx
	movswl	%dx,%edx
	movl	%ecx, _regs+fReg_pc_p
	addl	%edx, %eax
	ret

DecodeSrcDstAMdAIndex:
	shrl	$26, %edx
	andl	$0x3C, %edx

	pushl	%ebx

	movl	_regs+fReg_pc_p, %eax
	movl	_regs(%edx), %ecx
DecodeSrcDst_get_disp_ea:
	movzwl	(%eax), %edx
	addl	$2, %eax
	movl	%eax, _regs+fReg_pc_p
	rolw	$8, %dx
	movl	%edx, %eax
	shrw	$12, %ax
	andl	$15, %eax
	testb	$8, %dh
	movl	_regs(,%eax,4), %eax
	movsbl	%dl,%edx
	movswl	%ax,%ebx
	cmove	%ebx, %eax
	addl	%ecx, %edx
	addl	%edx, %eax
	popl	%ebx
	ret

DecodeSrcDstAMdPCDisp:
	movl	_regs+fReg_pc_p, %eax
	movzwl	(%eax), %edx
	movl	%eax, %ecx
	addl	$2, %eax
	subl	_regs+fReg_oldp, %ecx
	addl	_regs+fReg_pc, %ecx
	movl	%eax, _regs+fReg_pc_p
	rolw	$8, %dx
	movswl	%dx,%eax
	addl	%ecx, %eax
	ret

DecodeSrcDstAMdPCIndex:
	pushl	%ebx

	movl	_regs+fReg_pc_p, %eax
	movl	%eax, %ecx
	subl	_regs+fReg_oldp, %ecx
	addl	_regs+fReg_pc, %ecx
	jmp	DecodeSrcDst_get_disp_ea

DecodeSrcDstAMdImmedB:
	movl	_regs+fReg_pc_p, %ecx
	movsbl	1(%ecx),%eax
	addl	$2, %ecx
	movl	%ecx, _regs+fReg_pc_p
	ret

DecodeSrcDstAMdAbsW:
DecodeSrcDstAMdImmedW:
	movl	_regs+fReg_pc_p, %ecx
	movzwl	(%ecx), %eax
	addl	$2, %ecx
	movl	%ecx, _regs+fReg_pc_p
	rolw	$8, %ax
	movswl	%ax,%eax
	ret

DecodeSrcDstAMdAbsL:
DecodeSrcDstAMdImmedL:
	movl	_regs+fReg_pc_p, %ecx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%ecx, _regs+fReg_pc_p
	bswap	%eax
	/*
		movl	%edx, %ecx
		shrl	$16, %ecx
		rolw	$8, %dx
		rolw	$8, %cx
		sall	$16, %edx
		movzwl	%cx, %ecx
		orl	%ecx, %edx
	*/
	ret

DecodeSrcDstAMdDat4:
	shrl	$28, %edx
	andl	$0x0F, %edx
	movl	%edx, %eax
	ret

#if 0

_DecodeCompatDst:
	movl	_regs+fDecOpA, %edx
	movl	%edx, %eax
	shrl	$14, %eax
	andl	$0x7C, %eax
	jmp	*L_dsp_DecodeCompatSrcDst(%eax)


DecodeCompatSrcDstAMdReg:
	shrl	$26, %edx
	andl	$0x3C, %edx
	leal	_regs(%edx), %eax
	movl	%eax, _regs+fReg_ArgAddr
	movl	$1, _regs+fReg_ArgKind
	ret

DecodeCompatSrcDstAMdIndirect:
	shrl	$26, %edx
	andl	$0x3C, %edx
	movl	_regs(%edx), %eax
	movl	%eax, _regs+fReg_ArgAddr
	movl	$0, _regs+fReg_ArgKind
	ret

DecodeCompatSrcDstAMdAPosIncX:
	shrl	$26, %edx
	andl	$0x3C, %edx
	movl	_regs(%edx), %eax
	addl	%eax, %ecx
	movl	%ecx, _regs(%edx)
	movl	%eax, _regs+fReg_ArgAddr
	movl	$0, _regs+fReg_ArgKind
	ret

DecodeCompatSrcDstAMdAPosIncB:
	movl	$1, %ecx
	jmp	DecodeCompatSrcDstAMdAPosIncX

DecodeCompatSrcDstAMdAPosIncW:
	movl	$2, %ecx
	jmp	DecodeCompatSrcDstAMdAPosIncX

DecodeCompatSrcDstAMdAPosIncL:
	movl	$4, %ecx
	jmp	DecodeCompatSrcDstAMdAPosIncX

DecodeCompatSrcDstAMdAPreDecX:
	shrl	$26, %edx
	andl	$0x3C, %edx
	leal	_regs(%edx), %edx
	movl	(%edx), %eax
	subl	%ecx, %eax
	movl	%eax, (%edx)
	movl	%eax, _regs+fReg_ArgAddr
	movl	$0, _regs+fReg_ArgKind
	ret

DecodeCompatSrcDstAMdAPreDecB:
	movl	$1, %ecx
	jmp	DecodeCompatSrcDstAMdAPreDecX

DecodeCompatSrcDstAMdAPreDecW:
	movl	$2, %ecx
	jmp	DecodeCompatSrcDstAMdAPreDecX

DecodeCompatSrcDstAMdAPreDecL:
	movl	$4, %ecx
	jmp	DecodeCompatSrcDstAMdAPreDecX

DecodeCompatSrcDstAMdADisp:
	shrl	$26, %edx
	andl	$0x3C, %edx
	movl	_regs(%edx), %eax
	movl	_regs+fReg_pc_p, %edx
	movzwl	(%edx), %ecx
	addl	$2, %edx
	movl	%edx, _regs+fReg_pc_p
	rolw	$8, %cx
	movswl	%cx,%ecx
	addl	%ecx, %eax
	movl	%eax, _regs+fReg_ArgAddr
	movl	$0, _regs+fReg_ArgKind
	ret

DecodeCompatSrcDstAMdAIndex:
	shrl	$26, %edx
	andl	$0x3C, %edx

	pushl	%ebx

	movl	_regs+fReg_pc_p, %eax
	movl	_regs(%edx), %ecx
DecodeCompatSrcDst_get_disp_ea:
	movzwl	(%eax), %edx
	addl	$2, %eax
	movl	%eax, _regs+fReg_pc_p
	rolw	$8, %dx
	movl	%edx, %eax
	shrw	$12, %ax
	andl	$15, %eax
	testb	$8, %dh
	movl	_regs(,%eax,4), %eax
	movsbl	%dl,%edx
	movswl	%ax,%ebx
	cmove	%ebx, %eax
	addl	%ecx, %edx
	addl	%edx, %eax
	popl	%ebx
	movl	%eax, _regs+fReg_ArgAddr
	movl	$0, _regs+fReg_ArgKind
	ret

DecodeCompatSrcDstAMdPCDisp:
	movl	_regs+fReg_pc_p, %eax
	movzwl	(%eax), %edx
	movl	%eax, %ecx
	addl	$2, %eax
	subl	_regs+fReg_oldp, %ecx
	addl	_regs+fReg_pc, %ecx
	movl	%eax, _regs+fReg_pc_p
	rolw	$8, %dx
	movswl	%dx,%eax
	addl	%ecx, %eax
	movl	%eax, _regs+fReg_ArgAddr
	movl	$0, _regs+fReg_ArgKind
	ret

DecodeCompatSrcDstAMdPCIndex:
	pushl	%ebx

	movl	_regs+fReg_pc_p, %eax
	movl	%eax, %ecx
	subl	_regs+fReg_oldp, %ecx
	addl	_regs+fReg_pc, %ecx
	jmp	DecodeCompatSrcDst_get_disp_ea

DecodeCompatSrcDstAMdImmedB:
	movl	_regs+fReg_pc_p, %ecx
	movsbl	1(%ecx),%eax
	addl	$2, %ecx
	movl	%ecx, _regs+fReg_pc_p
	ret

DecodeCompatSrcDstAMdAbsW:
	movl	$0, _regs+fReg_ArgKind
	jmp	DecodeCompatSrcDst_L385
DecodeCompatSrcDstAMdImmedW:
	movl	$2, _regs+fReg_ArgKind
DecodeCompatSrcDst_L385:
	movl	_regs+fReg_pc_p, %ecx
	movzwl	(%ecx), %eax
	addl	$2, %ecx
	movl	%ecx, _regs+fReg_pc_p
	rolw	$8, %ax
	movswl	%ax,%eax
	movl	%eax, _regs+fReg_ArgAddr
	ret

DecodeCompatSrcDstAMdAbsL:
	movl	$0, _regs+fReg_ArgKind
	jmp	DecodeCompatSrcDst_L379
DecodeCompatSrcDstAMdImmedL:
	movl	$2, _regs+fReg_ArgKind
DecodeCompatSrcDst_L379:
	movl	_regs+fReg_pc_p, %ecx
	movl	(%ecx), %eax
	addl	$4, %ecx
	movl	%ecx, _regs+fReg_pc_p
	bswap	%eax
	/*
		movl	%edx, %ecx
		shrl	$16, %ecx
		rolw	$8, %dx
		rolw	$8, %cx
		sall	$16, %edx
		movzwl	%cx, %ecx
		orl	%ecx, %edx
	*/
	movl	%eax, _regs+fReg_ArgAddr
	ret

DecodeCompatSrcDstAMdDat4:
	movl	$2, _regs+fReg_ArgKind
	shrl	$28, %edx
	andl	$0x0F, %edx
	movl	%edx, %eax
	movl	%eax, _regs+fReg_ArgAddr
	ret

#endif


_get_byte:
	movl	%eax, %ecx
	andl	_regs+fReg_MATCrdB_cmpmask, %eax
	cmpl	_regs+fReg_MATCrdB_cmpvalu, %eax
	jne	_get_byte_ext
	andl	_regs+fReg_MATCrdB_usemask, %ecx
	addl	_regs+fReg_MATCrdB_usebase, %ecx
	movsbl	(%ecx),%eax
	ret


_get_word:
	movl	%eax, %ecx
	andl	_regs+fReg_MATCrdW_cmpmask, %eax
	cmpl	_regs+fReg_MATCrdW_cmpvalu, %eax
	jne	_get_word_ext
	andl	_regs+fReg_MATCrdW_usemask, %ecx
	addl	_regs+fReg_MATCrdW_usebase, %ecx
	movzwl	(%ecx), %eax
	rolw	$8, %ax
	cwtl
	ret


_get_long:
	movl	%eax, %ecx
	andl	_regs+fReg_MATCrdW_cmpmask, %eax
	cmpl	_regs+fReg_MATCrdW_cmpvalu, %eax
	jne	_get_long_ext

	leal	2(%ecx), %eax
	testl	_regs+fReg_MATCrdW_usemask, %eax
	je	_get_long_ext
	andl	_regs+fReg_MATCrdW_cmpmask, %eax
	cmpl	_regs+fReg_MATCrdW_cmpvalu, %eax
	jne	_get_long_ext

	andl	_regs+fReg_MATCrdW_usemask, %ecx
	addl	_regs+fReg_MATCrdW_usebase, %ecx
	movl	(%ecx), %eax
	bswap	%eax
	/*
		movl	%eax, %edx
		shrl	$16, %edx
		rolw	$8, %ax
		movl	%eax, %ecx
		rolw	$8, %dx
		sall	$16, %ecx
		movzwl	%dx, %edx
		orl	%edx, %ecx
		movl	%ecx, %eax
	*/
	ret

#if 0
_GetSrcValue:
	movl	_regs+fReg_ArgAddr, %eax
	movl	_regs+fDecOpB, %edx
	shrl	$22, %edx
	andl	$0x3C, %edx
	jmp	*L_dsp_GetSrcDstValue(%edx)
#endif


#if 0
_GetDstValue:
	movl	_regs+fReg_ArgAddr, %eax
	movl	_regs+fDecOpA, %edx
	shrl	$22, %edx
	andl	$0x3C, %edx
	jmp	*L_dsp_GetSrcDstValue(%edx)
#endif


GetSrcDstValRegB:
	movsbl	(%eax), %eax
	ret

GetSrcDstValRegW:
	movswl	(%eax), %eax
	ret

GetSrcDstValRegL:
	movl	(%eax), %eax
	ret

#if 0
GetSrcDstValMemB:
	jmp	_get_byte

GetSrcDstValMemW:
	jmp	_get_word

GetSrcDstValMemL:
	jmp	_get_long
#endif

GetSrcDstValCnst:
	ret


DoIKind_Move:
#if 0
	/* optimized version follows */
	movl	%ecx, _regs+fDecOpA
	movl	%edx, _regs+fDecOpB
	call	_DecodeSrc
	movl	%eax, _regs+fReg_ArgAddr
	call	_GetSrcValue
	movl	%eax, %ebx
	call	_DecodeDst
	movl	%eax, _regs+fReg_ArgAddr
#endif
	/* fDecOpB in edx, fDecOpA in ecx */
	movl	%edx, %eax
	movl	%edx, %ebx
	shrl	$14, %eax
	shrl	$22, %ebx
	andl	$0x7C, %eax
	andl	$0x3C, %ebx
	movl	L_dsp_DecodeSrcDst(%eax), %eax /* DecodeSrc */
	movl	L_dsp_GetSrcDstValue(%ebx), %ebx /* GetSrcValue */

	movl	%ecx, %edi

	call	*%eax /* DecodeSrc */
	call	*%ebx /* GetSrcValue */

	movl	%edi, %edx

	movl	%eax, %ebx

	movl	%edi, %eax
	movl	%edi, %ecx
	shrl	$14, %eax
	shrl	$22, %ecx
	andl	$0x7C, %eax
	andl	$0x3C, %ecx
	movl	L_dsp_DecodeSrcDst(%eax), %eax /* DecodeDst */
	movl	L_dsp_SetDstValue(%ecx), %esi /* SetDstValue */

	call	*%eax /* DecodeDst */

	movl	%ebx, %edx

	testl	%ebx, %ebx
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
	sets	_regs+fReg_n
	sete	_regs+fReg_z

	jmp	*%esi /* SetDstValue */


#if 0
_SetDstValue:
	movl	%eax, %edx
	movl	_regs+fDecOpA, %ecx
	shrl	$22, %ecx
	andl	$0x3C, %ecx
	movl	_regs+fReg_ArgAddr, %eax
	jmp	*L_dsp_SetDstValue(%ecx)
#endif


SetDstValRegB:
	movb	%dl, (%eax)
	jmp	m68k_NextInstruction

SetDstValRegW:
	movw	%dx, (%eax)
	jmp	m68k_NextInstruction

SetDstValRegL:
	movl	%edx, (%eax)
	jmp	m68k_NextInstruction


SetDstValMemB:
	call _put_byte
	jmp	m68k_NextInstruction

_put_byte:
	movl	%eax, %ecx
	andl	_regs+fReg_MATCwrB_cmpmask, %eax
	cmpl	_regs+fReg_MATCwrB_cmpvalu, %eax
	jne	_put_byte_ext
	andl	_regs+fReg_MATCwrB_usemask, %ecx
	addl	_regs+fReg_MATCwrB_usebase, %ecx
	movb	%dl, (%ecx)
	ret


SetDstValMemW:
	call _put_word
	jmp	m68k_NextInstruction

_put_word:
	movl	%eax, %ecx
	andl	_regs+fReg_MATCwrW_cmpmask, %eax
	cmpl	_regs+fReg_MATCwrW_cmpvalu, %eax
	jne	_put_word_ext
	andl	_regs+fReg_MATCwrW_usemask, %ecx
	addl	_regs+fReg_MATCwrW_usebase, %ecx
	rolw	$8, %dx
	movw	%dx, (%ecx)
	ret


SetDstValMemL:
	call _put_long
	jmp	m68k_NextInstruction

_put_long:
	movl	%eax, %ecx
	andl	_regs+fReg_MATCwrW_cmpmask, %eax
	cmpl	_regs+fReg_MATCwrW_cmpvalu, %eax
	jne	_put_long_ext

	leal	2(%ecx), %eax
	testl	_regs+fReg_MATCwrW_usemask, %eax
	je	_put_long_ext
	andl	_regs+fReg_MATCwrW_cmpmask, %eax
	cmpl	_regs+fReg_MATCwrW_cmpvalu, %eax
	jne	_put_long_ext

	andl	_regs+fReg_MATCwrW_usemask, %ecx
	addl	_regs+fReg_MATCwrW_usebase, %ecx
	bswap	%edx
	movl	%edx, (%ecx)
	ret


SetDstValCnst:
	/* shouldn't get here */
	jmp	m68k_NextInstruction


DecodeSrcDstGet:
#if 0
	/* optimized version follows */
	movl	%ecx, _regs+fDecOpA
	movl	%edx, _regs+fDecOpB
	call	_DecodeSrc
	movl	%eax, _regs+fReg_ArgAddr
	call	_GetSrcValue
	movl	%eax, %ebx

	call	_DecodeDst
	movl	%eax, _regs+fReg_ArgAddr
	call	_GetDstValue
#endif
	/* fDecOpB in edx, fDecOpA in ecx */
	movl	%edx, %eax
	movl	%edx, %ebx
	shrl	$14, %eax
	shrl	$22, %ebx
	andl	$0x7C, %eax
	andl	$0x3C, %ebx
	movl	L_dsp_DecodeSrcDst(%eax), %eax /* DecodeSrc */
	movl	L_dsp_GetSrcDstValue(%ebx), %ebx /* GetSrcValue */

	movl	%ecx, %edi

	call	*%eax /* DecodeSrc */
	call	*%ebx /* GetSrcValue */

	movl	%edi, %edx

	movl	%eax, %ebx

	movl	%edi, %eax
	movl	%edi, %ecx
	shrl	$14, %eax
	shrl	$22, %ecx
	andl	$0x7C, %eax
	andl	$0x3C, %ecx
	movl	L_dsp_DecodeSrcDst(%eax), %eax /* DecodeDst */
	movl	L_dsp_GetSrcDstValue(%ecx), %esi /* GetDstValue */
	movl	L_dsp_SetDstValue(%ecx), %edi /* SetDstValue */

	call	*%eax /* DecodeDst */

	movl	%esi, %edx
	movl	%eax, %esi
	jmp	*%edx /* GetDstValue, tail call */


DecodeDstGet:
#if 0
	/* optimized version follows */
	movl	%ecx, _regs+fDecOpA
	call	_DecodeDst
	movl	%eax, _regs+fReg_ArgAddr
	call	_GetDstValue
#endif
	/* fDecOpA in ecx */

	movl	%ecx, %edx

	movl	%ecx, %eax
	shrl	$14, %eax
	shrl	$22, %ecx
	andl	$0x7C, %eax
	andl	$0x3C, %ecx
	movl	L_dsp_DecodeSrcDst(%eax), %eax /* DecodeDst */
	movl	L_dsp_GetSrcDstValue(%ecx), %esi /* GetDstValue */
	movl	L_dsp_SetDstValue(%ecx), %edi /* SetDstValue */

	call	*%eax /* DecodeDst */

	movl	%esi, %edx
	movl	%eax, %esi
	jmp	*%edx /* GetDstValue, tail call */


DoIKind_Tst:
	call	DecodeDstGet
	testl	%eax, %eax
_SetCCRandNext:
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
	jmp	_SetNZandNext


_cctrue:
/* preserve %edx, %ecx */
	movl	%esi, %eax
	shrl	$8, %eax
	andl	$15, %eax
	jmp	*movl_table(,%eax,4)

cctrue_T:
	movb	$1, %al
	ret
cctrue_F:
	xorb	%al, %al
	ret
cctrue_HI:
	movb	_regs+fReg_c, %al
	orb		_regs+fReg_z, %al
	xorb	$1, %al
	ret
cctrue_LS:
	movb	_regs+fReg_c, %al
	orb		_regs+fReg_z, %al
	ret
cctrue_CC:
	movb	_regs+fReg_c, %al
	xorb	$1, %al
	ret
cctrue_CS:
	movb	_regs+fReg_c, %al
	ret
cctrue_NE:
	movb	_regs+fReg_z, %al
	xorb	$1, %al
	ret
cctrue_EQ:
	movb	_regs+fReg_z, %al
	ret
cctrue_VC:
	movb	_regs+fReg_v, %al
	xorb	$1, %al
	ret
cctrue_VS:
	movb	_regs+fReg_v, %al
	ret
cctrue_PL:
	movb	_regs+fReg_n, %al
	xorb	$1, %al
	ret
cctrue_MI:
	movb	_regs+fReg_n, %al
	ret
cctrue_GE:
	movb	_regs+fReg_v, %al
	xorb	_regs+fReg_n, %al
	xorb	$1, %al
	ret
cctrue_LT:
	movb	_regs+fReg_v, %al
	xorb	_regs+fReg_n, %al
	ret
cctrue_GT:
	movb	_regs+fReg_v, %al
	xorb	_regs+fReg_n, %al
	orb	_regs+fReg_z, %al
	xorb	$1, %al
	ret
cctrue_LE:
	movb	_regs+fReg_v, %al
	xorb	_regs+fReg_n, %al
	orb	_regs+fReg_z, %al
	ret


DoIKind_BccB:
	movl	%esi, %eax
	movl	%esi, %edx
	shrl	$6, %eax
	movl	_regs+fReg_pc_p, %ecx
	andl	$0x3C, %eax
	movsbl	%dl, %edx
	movl	movl_table(%eax), %eax
	leal	(%ecx,%edx), %edx
	call	*%eax /* _cctrue */

	cmpb	$0, %al
	je		m68k_NextInstruction

	jmp	m68k_NextInstruction_d


DoIKind_BccW:
	movl	_regs+fReg_pc_p, %ecx
	movl	%esi, %eax
	movzwl	(%ecx), %edx
	shrl	$6, %eax
	andl	$0x3C, %eax
	rolw	$8, %dx
	movl	movl_table(%eax), %eax
	movswl	%dx, %edx
	call	*%eax /* _cctrue */

	cmpb	$0, %al
	leal	(%ecx,%edx), %edx
	je		DoIKind_BraSkip0W
	jmp	m68k_NextInstruction_d
DoIKind_BraSkip0W:
	leal	2(%ecx), %edx
	jmp	m68k_NextInstruction_d


DoIKind_BraB:
	movl	%esi, %eax
	movl	_regs+fReg_pc_p, %ecx
	movsbl	%al, %eax
DoIKind_Bra_offset:
	leal	(%ecx,%eax), %edx
	jmp	m68k_NextInstruction_d


DoIKind_BraW:
	movl	_regs+fReg_pc_p, %ecx
DoIKind_Bra_W:
	movzwl	(%ecx), %eax
	rolw	$8, %ax
	cwtl
	jmp	DoIKind_Bra_offset


DoIKind_BraSkipW:
	movl	_regs+fReg_pc_p, %ecx
	leal	2(%ecx), %edx
	jmp	m68k_NextInstruction_d


DoIKind_DBcc:
	movl	%esi, %eax
	movl	%esi, %edx
	shrl	$6, %eax
	andl	$7, %edx
	andl	$0x3C, %eax
	leal	_regs(,%edx,4), %edx
	movl	movl_table(%eax), %eax
	call	*%eax /* _cctrue */

	cmpb	$0, %al
	jne		DoIKind_BraSkipW
	movw	(%edx), %ax
	movl	_regs+fReg_pc_p, %ecx
	subw	$1, %ax
	movzwl	(%ecx), %ebx
	movw	%ax, (%edx)
	rolw	$8, %bx
	addw	$1, %ax
	movswl	%bx, %ebx
	je	DoIKind_BraSkipW

	leal	(%ecx,%ebx), %edx
	jmp	m68k_NextInstruction_d


DoIKind_DBF:
	movl	%esi, %edx
	andl	$7, %edx
	leal	_regs(,%edx,4), %edx
	movw	(%edx), %ax
	movl	_regs+fReg_pc_p, %ecx
	subw	$1, %ax
	movzwl	(%ecx), %ebx
	movw	%ax, (%edx)
	rolw	$8, %bx
	addw	$1, %ax
	movswl	%bx, %ebx
	je	DoIKind_BraSkipW

	leal	(%ecx,%ebx), %edx
	jmp	m68k_NextInstruction_d


DoIKind_Swap:
	movl	%esi, %ecx
	andl	$7, %ecx
	movl	_regs(,%ecx,4), %edx
	roll	$16, %edx
	movl	%edx, _regs(,%ecx,4)
	testl	%edx, %edx
	jmp	_SetCCRandNext


_DecodeNormModeRegister:
	movl	%esi, %eax
	movl	%eax, %edx
	shrl	$3, %eax
_DecodeMod8ModeRegister:
	andl	$7, %edx
	andl	$7, %eax
_DecodeModeRegister:
	jmp	*L359(,%eax,4)

SetArgKindDReg:
	leal	_regs(,%edx,4), %eax
L382b:
	movl	$1, _regs+fReg_ArgKind
L382a:
	movl	%eax, _regs+fReg_ArgAddr
	ret
SetArgKindAReg:
	leal	_regs+32(,%edx,4), %eax
	jmp	L382b
SetArgKind2:
	movl	$0, _regs+fReg_ArgKind
	movl	_regs+32(,%edx,4), %eax
	jmp	L382a

SetArgKindPostInc:
	cmpl	$7, %edx
	movl	_regs+32(,%edx,4), %eax
	movl	$0, _regs+fReg_ArgKind
	movl	%eax, _regs+fReg_ArgAddr
	jne	L360
	cmpl	$1, _regs+fReg_opsize
	jne	L360
	addl	$2, %eax
	jmp	L360a
L360:
	addl	_regs+fReg_opsize, %eax
L360a:
	movl	%eax, _regs+32(,%edx,4)
	ret

SetArgKindPreDec:
	cmpl	$7, %edx
	movl	_regs+32(,%edx,4), %eax
	movl	$0, _regs+fReg_ArgKind
	jne	L363
	cmpl	$1, _regs+fReg_opsize
	jne	L363
	subl	$2, %eax
	jmp	L363a
L363:
	subl	_regs+fReg_opsize, %eax
L363a:
	movl	%eax, _regs+32(,%edx,4)
	jmp	L382a

SetArgKind5:
	movl	_regs+32(,%edx,4), %eax
	movl	_regs+fReg_pc_p, %edx
	movl	$0, _regs+fReg_ArgKind
	movzwl	(%edx), %ecx
	addl	$2, %edx
	movl	%edx, _regs+fReg_pc_p
	rolw	$8, %cx
	movswl	%cx,%ecx
	addl	%ecx, %eax
	movl	%eax, _regs+fReg_ArgAddr
	ret

SetArgKind6:
	pushl	%ebx

	movl	_regs+fReg_pc_p, %eax
	movl	_regs+32(,%edx,4), %ecx
get_disp_ea:
	movl	$0, _regs+fReg_ArgKind
	movzwl	(%eax), %edx
	addl	$2, %eax
	movl	%eax, _regs+fReg_pc_p
	rolw	$8, %dx
	movl	%edx, %eax
	shrw	$12, %ax
	andl	$15, %eax
	testb	$8, %dh
	movl	_regs(,%eax,4), %eax
	movsbl	%dl,%edx
	movswl	%ax,%ebx
	cmove	%ebx, %eax
	addl	%ecx, %edx
	addl	%eax, %edx
	popl	%ebx
	jmp	L383

SetArgKind7:
	jmp	*L374(,%edx,4)

SetArgKind7_0:
	movl	$0, _regs+fReg_ArgKind
	jmp	L385
SetArgKind7_1:
	movl	$0, _regs+fReg_ArgKind
	jmp	L379
SetArgKind7_2:
	movl	_regs+fReg_pc_p, %eax
	movl	$0, _regs+fReg_ArgKind
	movzwl	(%eax), %edx
	movl	%eax, %ecx
	addl	$2, %eax
	subl	_regs+fReg_oldp, %ecx
	addl	_regs+fReg_pc, %ecx
	movl	%eax, _regs+fReg_pc_p
	rolw	$8, %dx
	movswl	%dx,%edx
	addl	%ecx, %edx
	jmp	L383

SetArgKind7_3:
	pushl	%ebx

	movl	_regs+fReg_pc_p, %eax
	movl	%eax, %ecx
	subl	_regs+fReg_oldp, %ecx
	addl	_regs+fReg_pc, %ecx
	jmp	get_disp_ea

SetArgKind7_4:
	movl	_regs+fReg_opsize, %eax
	movl	$2, _regs+fReg_ArgKind
	cmpl	$2, %eax
	jne	L377

L385:
	movl	_regs+fReg_pc_p, %eax
	movzwl	(%eax), %edx
	addl	$2, %eax
	movl	%eax, _regs+fReg_pc_p
	rolw	$8, %dx
	movswl	%dx,%edx
	jmp	L383

L377:
	jae	L379

	movl	_regs+fReg_pc_p, %eax
	movsbl	1(%eax),%edx
	addl	$2, %eax
	movl	%eax, _regs+fReg_pc_p
	jmp	L383
L379:
	movl	_regs+fReg_pc_p, %eax
	movl	(%eax), %edx
	addl	$4, %eax
	movl	%eax, _regs+fReg_pc_p
	bswap	%edx
	/*
		movl	%edx, %ecx
		shrl	$16, %ecx
		rolw	$8, %dx
		rolw	$8, %cx
		sall	$16, %edx
		movzwl	%cx, %ecx
		orl	%ecx, %edx
	*/
L383:
	movl	%edx, _regs+fReg_ArgAddr
	ret
SetArgKind8:
	movl	$2, _regs+fReg_ArgKind
	jmp	L383


_GetDecodeNormModeRegisterVal:
	call	_DecodeNormModeRegister
_GetArgValue:
	movl	_regs+fReg_ArgKind, %eax
	testl	%eax, %eax
	je	_GetArgValue_Mem
	subl	$1, %eax
	/* decl	%eax */
	je	_GetArgValue_Reg

	movl	_regs+fReg_ArgAddr, %eax
	ret
_GetArgValue_Mem:

	cmpl	$2, _regs+fReg_opsize
	movl	_regs+fReg_ArgAddr, %eax
	je	_get_word
	jae	_get_long
	jmp	_get_byte

_GetArgValue_Reg:
	movl	_regs+fReg_opsize, %eax
	movl	_regs+fReg_ArgAddr, %edx
_GetArgValue_Reg0:
	cmpl	$2, %eax
	jne	L48
	movswl	(%edx), %eax
	ret
L48:
	jae	L49
	movsbl	(%edx), %eax
	ret
L49:
	movl	(%edx), %eax
	ret


DoIKind_MoveAL:
	movl	$4, _regs+fReg_opsize
	jmp	_DoMoveA


DoIKind_MoveAW:
	movl	$2, _regs+fReg_opsize
	/* jmp	_DoMoveA */


_DoMoveA:
	call	_GetDecodeNormModeRegisterVal
	movl	%esi, %edx
	shrl	$9, %edx
	andl	$7, %edx
	movl	%eax, _regs+32(,%edx,4)
	jmp	m68k_NextInstruction


DoIKind_MoveQ:
	movl	%esi, %ecx
	movl	%esi, %eax
	movsbl	%cl,%edx
	shrl	$9, %eax
	andl	$7, %eax
	movl	%edx, _regs(,%eax,4)
	testl	%edx, %edx
	jmp	_SetCCRandNext


DoIKind_AddB:
	call	DecodeSrcDstGet
	addb	%bl, %al
	jmp	_SetAddSubFlagsAndDstValue


DoIKind_AddW:
	call	DecodeSrcDstGet
	addw	%bx, %ax
	jmp	_SetAddSubFlagsAndDstValue


DoIKind_AddL:
	call	DecodeSrcDstGet
	addl	%ebx, %eax
	jmp	_SetAddSubFlagsAndDstValue


DoIKind_SubB:
	call	DecodeSrcDstGet
	subb	%bl, %al
	jmp	_SetAddSubFlagsAndDstValue


DoIKind_SubL:
	call	DecodeSrcDstGet
	subl	%ebx, %eax
	jmp	_SetAddSubFlagsAndDstValue


DoIKind_SubW:
	call	DecodeSrcDstGet
	subw	%bx, %ax
	/* jmp	_SetAddSubFlagsAndDstValue */


_SetAddSubFlagsAndDstValue:
	setc	_regs+fReg_x
	seto	_regs+fReg_v
	setc	_regs+fReg_c
	/* jmp	_SetNZandDstValue */

_SetNZandDstValue:
	sets	_regs+fReg_n
	sete	_regs+fReg_z
	/* jmp	_SetDstValue */

	movl	%eax, %edx
	movl	%esi, %eax
	jmp	*%edi /* SetDstValue */


_DecodeDst_C:
	movl	%ecx, %eax
	movl	%ecx, %edx
_DecodeDst_0:
	shrl	$14, %eax
	andl	$0x7C, %eax
	jmp	*L_dsp_DecodeSrcDst(%eax)


DoIKind_Lea:
	call	_DecodeDst_C

	movl	%esi, %ebx
	shrl	$9, %ebx
	andl	$7, %ebx
	movl	%eax, _regs+32(,%ebx,4)
	jmp	m68k_NextInstruction


DoIKind_PEA:
	call	_DecodeDst_C

	movl	%eax, %edx
	movl	_regs+fReg_A7, %eax
	subl	$4, %eax
	movl	%eax, _regs+fReg_A7
	call	_put_long
	jmp	m68k_NextInstruction


_m68k_setpc:
	movl	%eax, _regs+fReg_pc
	movl	%eax, %ecx
	movl	%eax, %edx
	andl	_regs+fReg_MATCex_usemask, %eax
	andl	_regs+fReg_MATCex_cmpmask, %edx
	addl	_regs+fReg_MATCex_usebase, %eax
	cmpl	_regs+fReg_MATCex_cmpvalu, %edx
	jne	get0_pc_real_address_ext
_end_get0_pc_real_address:
	movl	%eax, _regs+fReg_oldp
	movl	%eax, _regs+fReg_pc_p
	ret


_m68k_getSR:
	movzbl	_regs+fReg_t1, %eax
	movzbl	_regs+fReg_s, %edx
	sall	$15, %eax
	sall	$13, %edx
	orl	%edx, %eax
	movl	_regs+fReg_intmask, %edx
	sall	$8, %edx
	orl	%edx, %eax
	movzbl	_regs+fReg_x, %edx
	sall	$4, %edx
	orl	%edx, %eax
	movzbl	_regs+fReg_n, %edx
	sall	$3, %edx
	orl	%edx, %eax
	movzbl	_regs+fReg_z, %edx
	sall	$2, %edx
	orl	%edx, %eax
	movzbl	_regs+fReg_v, %edx
	addl	%edx, %edx
	orl	%edx, %eax
	movzbl	_regs+fReg_c, %edx
	orl	%edx, %eax
	ret


_ExceptionTo:
	pushl	%eax
	call	_m68k_getSR
	pushl	%eax
	movzbl	_regs+fReg_s, %edx
	testb	%dl, %dl
	movl	_regs+fReg_A7, %eax
	jne	L498
	movb	$1, _regs+fReg_s
	movl	%eax, _regs+fReg_usp
	movl	_regs+fReg_isp, %eax
L498:
	movl	_regs+fReg_pc_p, %edx
	subl	$4, %eax
	subl	_regs+fReg_oldp, %edx
	addl	_regs+fReg_pc, %edx
	movl	%eax, _regs+fReg_A7
	call	_put_long
	movl	_regs+fReg_A7, %eax
	popl	%edx
	movzwl	%dx, %edx
	subl	$2, %eax
	movl	%eax, _regs+fReg_A7
	call	_put_word
	popl	%eax
	call	_m68k_setpc
	movb	$0, _regs+fReg_t1
	movb	$0, _regs+fReg_TracePend
	ret


_Exception:
	sall	$2, %eax
	call	_get_long
	jmp	_ExceptionTo


DoIKind_A:
	movl	$10, %eax
_BackupExceptionAndNext:
	subl	$2, _regs+fReg_pc_p
_ExceptionAndNext:
	call	_Exception
	jmp	m68k_NextInstruction


DoIKind_BsrB:
	movl	_regs+fReg_pc_p, %edx
	movl	%esi, %eax
	movl	%edx, %ebx
	movsbl	%al, %eax
L816:
	addl	%eax, %ebx
	movl	_regs+fReg_A7, %eax
	subl	_regs+fReg_oldp, %edx
	subl	$4, %eax
	addl	_regs+fReg_pc, %edx
	movl	%eax, _regs+fReg_A7
	call	_put_long
	movl	%ebx, _regs+fReg_pc_p
	jmp	m68k_NextInstruction


DoIKind_BsrW:
	movl	_regs+fReg_pc_p, %edx
	movl	%edx, %ebx
	movzwl	(%edx), %eax
	leal	2(%edx), %edx
	rolw	$8, %ax
	cwtl
	jmp	L816


DoIKind_Jsr:
	call	_DecodeDst_C
	movl	%eax, %esi
	/* call	_DecodeNormModeRegister */
	movl	_regs+fReg_pc_p, %edx
	movl	_regs+fReg_A7, %eax
	subl	_regs+fReg_oldp, %edx
	subl	$4, %eax
	addl	_regs+fReg_pc, %edx
	movl	%eax, _regs+fReg_A7
	call	_put_long
	movl	%esi, %eax
	call	_m68k_setpc
	jmp	m68k_NextInstruction


DoIKind_LinkA6:
	movl	_regs+fReg_A7, %ebx
	movl	_regs+fReg_A6, %edx
	subl	$4, %ebx
	movl	%ebx, %eax
	call	_put_long
	movl	_regs+fReg_pc_p, %edx
	movl	%ebx, _regs+fReg_A6
	movzwl	(%edx), %eax
	addl	$2, %edx
	movl	%edx, _regs+fReg_pc_p
	rolw	$8, %ax
	cwtl
	addl	%eax, %ebx
	movl	%ebx, _regs+fReg_A7
	jmp	m68k_NextInstruction


DoIKind_UnlkA6:
	movl	_regs+fReg_A6, %ebx
	movl	%ebx, %eax
	addl	$4, %ebx
	call	_get_long
	movl	%ebx, _regs+fReg_A7
	movl	%eax, _regs+fReg_A6
	jmp	m68k_NextInstruction


DoIKind_Rts:
	movl	_regs+fReg_A7, %eax
	call	_get_long
	addl	$4, _regs+fReg_A7
	call	_m68k_setpc
	jmp	m68k_NextInstruction


DoIKind_Jmp:
	call	_DecodeDst_C
	/* call	_DecodeNormModeRegister */
	/* movl	_regs+fReg_ArgAddr, %eax */
	call	_m68k_setpc
	jmp	m68k_NextInstruction


DecodeDstForSet:
#if 0
	/* optimized version follows */
	movl	%ecx, _regs+fDecOpA
	call	_DecodeDst
	movl	%eax, _regs+fReg_ArgAddr
#endif
	/* fDecOpA in ecx */

	movl	%ecx, %edx

	movl	%ecx, %eax
	shrl	$14, %eax
	shrl	$22, %ecx
	andl	$0x7C, %eax
	andl	$0x3C, %ecx
	movl	L_dsp_DecodeSrcDst(%eax), %eax /* DecodeDst */
	movl	L_dsp_SetDstValue(%ecx), %edi /* SetDstValue */

	jmp	*%eax /* DecodeDst, tail call */


DoIKind_Clr:
	call	DecodeDstForSet
	xorl	%edx, %edx
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
	movb	$0, _regs+fReg_n
	movb	$1, _regs+fReg_z
	jmp	*%edi /* SetDstValue */



DoIKind_AddA:
	movl	%esi, %ecx
	shrl	$7, %ecx
	andl	$2, %ecx
	addl	$2, %ecx
	movl	%ecx, _regs+fReg_opsize
	call	_GetDecodeNormModeRegisterVal
	movl	%esi, %edx
	shrl	$9, %edx
	andl	$7, %edx
	addl	%eax, _regs+32(,%edx,4)
	jmp	m68k_NextInstruction


DoIKind_AddQA:
	movl	%esi, %eax
	movl	%esi, %edx
	movl	$8, %ecx
	shrl	$9, %edx
	andl	$7, %eax
	andl	$7, %edx
	cmove	%ecx, %edx
	addl	%edx, _regs+32(,%eax,4)
	jmp	m68k_NextInstruction


DoIKind_SubA:
	movl	%esi, %ecx
	shrl	$7, %ecx
	andl	$2, %ecx
	addl	$2, %ecx
	movl	%ecx, _regs+fReg_opsize
	call	_GetDecodeNormModeRegisterVal
	movl	%esi, %edx
	shrl	$9, %edx
	andl	$7, %edx
	subl	%eax, _regs+32(,%edx,4)
	jmp	m68k_NextInstruction


DoIKind_SubQA:
	movl	%esi, %eax
	movl	%esi, %edx
	movl	$8, %ecx
	shrl	$9, %edx
	andl	$7, %eax
	andl	$7, %edx
	cmove	%ecx, %edx
	subl	%edx, _regs+32(,%eax,4)
	jmp	m68k_NextInstruction


DoIKind_CmpA:
	movl	%esi, %ecx
	shrl	$7, %ecx
	andl	$2, %ecx
	addl	$2, %ecx
	movl	%ecx, _regs+fReg_opsize
	call	_GetDecodeNormModeRegisterVal
	movl	%eax, %ebx
	movl	%esi, %edx
	shrl	$9, %edx
	andl	$7, %edx
	movl	_regs+32(,%edx,4), %eax
	jmp		_DoCompare_L


DoIKind_AddXB:
	call	DecodeSrcDstGet
	movb	_regs+fReg_x, %cl
	shrb	$1, %cl
	adcb	%bl, %al
	jmp	_SetAddSubXFlagsAndDstValue


DoIKind_AddXL:
	call	DecodeSrcDstGet
	movb	_regs+fReg_x, %cl
	shrb	$1, %cl
	adcl	%ebx, %eax
	jmp	_SetAddSubXFlagsAndDstValue


DoIKind_AddXW:
	call	DecodeSrcDstGet
	movb	_regs+fReg_x, %cl
	shrb	$1, %cl
		/* load carry flag */
	adcw	%bx, %ax
	/* jmp	_SetAddSubXFlagsAndDstValue */

_SetAddSubXFlagsAndDstValue:
	setc	_regs+fReg_x
	seto	_regs+fReg_v
	setc	_regs+fReg_c
	sets	_regs+fReg_n
	movl	%eax, %edx
	movl	%esi, %eax
	je	SetAddSubXFlagsAndDstValue_L1
	sete	_regs+fReg_z
SetAddSubXFlagsAndDstValue_L1:
	jmp	*%edi /* SetDstValue */


DoIKind_SubXB:
	call	DecodeSrcDstGet
	movb	_regs+fReg_x, %cl
	shrb	$1, %cl
	sbbb	%bl, %al
	jmp	_SetAddSubXFlagsAndDstValue


DoIKind_SubXW:
	call	DecodeSrcDstGet
	movb	_regs+fReg_x, %cl
	shrb	$1, %cl
		/* load carry flag */
	sbbw	%bx, %ax
	jmp	_SetAddSubXFlagsAndDstValue


DoIKind_SubXL:
	call	DecodeSrcDstGet
	movb	_regs+fReg_x, %cl
	shrb	$1, %cl
	sbbl	%ebx, %eax
	jmp	_SetAddSubXFlagsAndDstValue


_SetArgValue:
/*
	exits to m68k_NextInstruction
	entry: %eax = v
	note: unlike in c code, v may often not be sign extended
*/
	movl	%eax, %edx
	cmpl	$1, _regs+fReg_ArgKind
	movl	_regs+fReg_opsize, %eax
	je	_SetArgValue_Reg

	cmpl	$2, %eax
	movl	_regs+fReg_ArgAddr, %eax
	jne	L61
	call _put_word
	jmp	m68k_NextInstruction

L61:
	jae	L63
	call _put_byte
	jmp	m68k_NextInstruction

L63:
	call _put_long
	jmp	m68k_NextInstruction


_SetArgValue_Reg:
	cmpl	$2, %eax
	movl	_regs+fReg_ArgAddr, %eax
	jne	L56

	movw	%dx, (%eax)
	jmp	m68k_NextInstruction
L56:
	jae	L59

	movb	%dl, (%eax)
	jmp	m68k_NextInstruction
L59:
	movl	%edx, (%eax)
	jmp	m68k_NextInstruction


#if 0
_FindOpSizeFromb76b:
/*
	preserve %ecx, %edx, but not %ebx
*/
	movl	%ecx, %ebx
	movl	%esi, %ecx
	movl	$1, %eax
	shrl	$6, %ecx
	andl	$3, %ecx
	sall	%cl, %eax
	movl	%eax, _regs+fReg_opsize
	movl	%ebx, %ecx
	ret
#endif


_FindOpSizeFromb76:
/*
	some callers assume ln2 opsize is in %ecx on exit
	some callers assume opsize is in %eax on exit
*/
	movl	%esi, %ecx
	movl	$1, %eax
	shrl	$6, %ecx
	andl	$3, %ecx
	sall	%cl, %eax
	movl	%eax, _regs+fReg_opsize
	ret


DoIKind_RolopNM:
	movl	$2, _regs+fReg_opsize
	call	_DecodeNormModeRegister
	movl	%esi, %eax
	movl	%eax, %edx
	andl	$256, %eax
	shrl	$8, %edx
	andl	$14, %edx
	cmpl	$1, %eax
	adcl	$0, %edx
	movl	$1, %eax
	jmp	_DoBinOp1


DoIKind_RolopDD:
	call	_FindOpSizeFromb76
	movl	%esi, %ecx
	shrl	$7, %ecx
	andl	$28, %ecx
	leal	_regs(%ecx), %edx
	call	_GetArgValue_Reg0
	movl	%esi, %ecx
	movl	$1, _regs+fReg_ArgKind
	movl	%ecx, %edx
	andl	$7, %edx
	leal	_regs(,%edx,4), %edx
	movl	%edx, _regs+fReg_ArgAddr
	movl	%ecx, %edx
	andl	$256, %ecx
	shrl	$2, %edx
	andl	$6, %edx
	cmpl	$1, %ecx
	adcl	$0, %edx
	jmp	_DoBinOp1


DoIKind_RolopND:
	call	_FindOpSizeFromb76
	movl	%esi, %edx
	movl	$8, %ecx
	movl	%esi, %eax
	andl	$7, %eax
	leal	_regs(,%eax,4), %eax
	movl	%eax, _regs+fReg_ArgAddr
	movl	%esi, %eax
	shrl	$2, %edx
	andl	$256, %eax
	andl	$6, %edx
	cmpl	$1, %eax
	adcl	$0, %edx
	movl	%esi, %eax
	shrl	$9, %eax
	andl	$7, %eax
	cmove	%ecx, %eax
	movl	$1, _regs+fReg_ArgKind
	/* jmp	_DoBinOp1 */


_DoBinOp1:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$72, %esp
	movl	%ebx, -12(%ebp)
	movl	%edx, %ebx
	movl	%esi, -8(%ebp)
	movl	%eax, %esi
	movl	%edi, -4(%ebp)
	andl	$63, %esi
	call	_GetArgValue
	cmpl	$7, %ebx
	movl	%eax, %edx
	ja	L152
	jmp	*L161(,%ebx,4)

L153:
	testl	%esi, %esi
	jne	L162
	movl	%eax, %edi
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
	jmp	L164
L162:
	xorl	%ecx, %ecx
	cmpl	$32, %esi
	ja	L167
	leal	-1(%esi), %ecx
	sall	%cl, %eax
	movl	%eax, %ecx
L167:
	movl	_regs+fReg_opsize, %ebx
	cmpl	$2, %ebx
	jne	L168
	movswl	%cx,%ecx
	jmp	L170
L168:
	movsbl	%cl,%eax
	cmpl	$1, %ebx
	cmovbe	%eax, %ecx
L170:
	movl	%ecx, %eax
	shrl	$31, %eax
	cmpl	$2, %ebx
	movb	%al, _regs+fReg_x
	leal	(%ecx,%ecx), %edi
	movb	%al, _regs+fReg_c
	jne	L172
	movswl	%di,%edi
	jmp	L164
L172:
	movl	%edi, %ecx
	cmpl	$1, %ebx
	movsbl	%cl,%eax
	cmovbe	%eax, %edi
L164:
	testl	%edi, %edi
	jns	L175
	movl	%edi, %eax
	movl	%esi, %ecx
	negl	%eax
	shrl	%cl, %eax
	negl	%eax
	jmp	L177
L175:
	movl	%edi, %eax
	movl	%esi, %ecx
	shrl	%cl, %eax
L177:
	cmpl	%edx, %eax
	movl	%edi, %eax
	setne	_regs+fReg_v
	testl	%edi, %edi
	sete	_regs+fReg_z
	movl	%edi, %edx
	shrl	$31, %eax
	movb	%al, _regs+fReg_n
	jmp	L152
L154:
	shrl	$31, %eax
	testl	%esi, %esi
	movb	%al, _regs+fReg_n
	movb	$0, _regs+fReg_v
	jne	L178
	movb	$0, _regs+fReg_c
	jmp	L180
L178:
	movl	_regs+fReg_opsize, %ecx
	movl	%edx, %eax
	cmpb	$0, _regs+fReg_n
	notl	%eax
	cmovne	%eax, %edx
	cmpl	$2, %ecx
	jne	L183
	movzwl	%dx, %edx
	jmp	L185
L183:
	movzbl	%dl, %eax
	cmpl	$1, %ecx
	cmovbe	%eax, %edx
L185:
	xorl	%eax, %eax
	cmpl	$32, %esi
	ja	L189
	leal	-1(%esi), %ecx
	movl	%edx, %eax
	shrl	%cl, %eax
L189:
	movl	%eax, %ebx
	movl	%eax, %edx
	andl	$1, %ebx
	shrl	%edx
	cmpb	$0, _regs+fReg_n
	movb	%bl, _regs+fReg_c
	je	L190
	xorl	$1, %ebx
	notl	%edx
	movb	%bl, _regs+fReg_c
L190:
	movzbl	_regs+fReg_c, %eax
	movb	%al, _regs+fReg_x
L180:
	testl	%edx, %edx
	sete	_regs+fReg_z
	jmp	L152
L155:
	testl	%esi, %esi
	je	L278
	xorl	%ecx, %ecx
	cmpl	$32, %esi
	ja	L197
	leal	-1(%esi), %ecx
	sall	%cl, %edx
	movl	%edx, %ecx
L197:
	movl	_regs+fReg_opsize, %ebx
	cmpl	$2, %ebx
	jne	L198
	movswl	%cx,%ecx
	jmp	L200
L198:
	movsbl	%cl,%eax
	cmpl	$1, %ebx
	cmovbe	%eax, %ecx
L200:
	movl	%ecx, %eax
	shrl	$31, %eax
	cmpl	$2, %ebx
	movb	%al, _regs+fReg_x
	leal	(%ecx,%ecx), %edx
	movb	%al, _regs+fReg_c
	je	L276
	movsbl	%dl,%eax
	cmpl	$1, %ebx
	jmp	L275
L156:
	testl	%esi, %esi
	je	L278
	movl	_regs+fReg_opsize, %ecx
	cmpl	$2, %ecx
	jne	L208
	movzwl	%ax, %edx
	jmp	L210
L208:
	movzbl	%al, %eax
	cmpl	$1, %ecx
	cmovbe	%eax, %edx
L210:
	xorl	%ecx, %ecx
	cmpl	$32, %esi
	ja	L214
	leal	-1(%esi), %ecx
	shrl	%cl, %edx
	movl	%edx, %ecx
L214:
	movl	%ecx, %eax
	movl	%ecx, %edx
	andl	$1, %eax
	shrl	%edx
	movb	%al, _regs+fReg_x
	movb	%al, _regs+fReg_c
	jmp	L252
L159:
	testl	%esi, %esi
	je	L278
	movl	_regs+fReg_opsize, %edi
	xorl	%ecx, %ecx
L218:
	movl	%edx, %ebx
	addl	%edx, %edx
	movl	%edx, %eax
	shrl	$31, %ebx
	orl	$1, %eax
	testb	%bl, %bl
	cmovne	%eax, %edx
	cmpl	$2, %edi
	jne	L221
	movswl	%dx,%edx
	jmp	L223
L221:
	movsbl	%dl,%eax
	cmpl	$1, %edi
	cmovbe	%eax, %edx
L223:
	incl	%ecx
	cmpl	%ecx, %esi
	jne	L218
	movb	%bl, _regs+fReg_c
	jmp	L252
L157:
	testl	%esi, %esi
	je	L277
	movl	_regs+fReg_opsize, %eax
	xorl	%ebx, %ebx
	movzbl	_regs+fReg_x, %ecx
	movl	%eax, -28(%ebp)
L229:
	movl	%edx, %eax
	addl	%edx, %edx
	shrl	$31, %eax
	movl	%eax, %edi
	movb	%al, -41(%ebp)
	movl	%edx, %eax
	orl	$1, %eax
	testb	%cl, %cl
	cmovne	%eax, %edx
	cmpl	$2, -28(%ebp)
	jne	L232
	movswl	%dx,%edx
	jmp	L234
L232:
	cmpl	$1, -28(%ebp)
	movsbl	%dl,%eax
	cmovbe	%eax, %edx
L234:
	incl	%ebx
	movl	%edi, %ecx
	cmpl	%ebx, %esi
	jne	L229
	movb	%cl, _regs+fReg_x
	movzbl	-41(%ebp), %ecx
	movb	%cl, _regs+fReg_c
	jmp	L252
L160:
	movl	_regs+fReg_opsize, %eax
	movl	$1, %edi
	leal	-1(,%eax,8), %ecx
	sall	%cl, %edi
	testl	%esi, %esi
	movl	%eax, -32(%ebp)
	jne	L236
L278:
	movb	$0, _regs+fReg_c
	jmp	L252
L236:
	cmpl	$2, -32(%ebp)
	jne	L239
	movzwl	%dx, %edx
	jmp	L241
L239:
	cmpl	$1, -32(%ebp)
	movzbl	%dl, %eax
	cmovbe	%eax, %edx
L241:
	xorl	%ecx, %ecx
L243:
	movl	%edx, %ebx
	shrl	%edx
	andl	$1, %ebx
	movl	%edx, %eax
	orl	%edi, %eax
	testb	%bl, %bl
	cmovne	%eax, %edx
	incl	%ecx
	cmpl	%esi, %ecx
	jne	L243
	cmpl	$2, -32(%ebp)
	movb	%bl, _regs+fReg_c
	je	L276
	cmpl	$1, -32(%ebp)
	movsbl	%dl,%eax
	jmp	L275
L158:
	movl	_regs+fReg_opsize, %ecx
	movl	$1, -36(%ebp)
	movl	%ecx, -40(%ebp)
	leal	-1(,%ecx,8), %ecx
	sall	%cl, -36(%ebp)
	testl	%esi, %esi
	jne	L250
L277:
	movzbl	_regs+fReg_x, %eax
	movb	%al, _regs+fReg_c
	jmp	L252
L250:
	cmpl	$2, -40(%ebp)
	jne	L253
	movzwl	%dx, %edx
	jmp	L255
L253:
	cmpl	$1, -40(%ebp)
	movzbl	%dl, %eax
	cmovbe	%eax, %edx
L255:
	movzbl	_regs+fReg_x, %edi
	xorl	%ebx, %ebx
L257:
	movl	-36(%ebp), %eax
	movl	%edx, %ecx
	shrl	%edx
	andl	$1, %ecx
	orl	%edx, %eax
	movl	%eax, -60(%ebp)
	movl	%edi, %eax
	movl	%ecx, %edi
	testb	%al, %al
	cmovne	-60(%ebp), %edx
	incl	%ebx
	cmpl	%ebx, %esi
	jne	L257
	cmpl	$2, -40(%ebp)
	movb	%cl, _regs+fReg_x
	movb	%cl, _regs+fReg_c
	jne	L261
L276:
	movswl	%dx,%edx
	jmp	L252
L261:
	cmpl	$1, -40(%ebp)
	movsbl	%dl,%eax
L275:
	cmovbe	%eax, %edx
L252:
	testl	%edx, %edx
	movl	%edx, %eax
	sete	_regs+fReg_z
	shrl	$31, %eax
	movb	%al, _regs+fReg_n
	movb	$0, _regs+fReg_v
L152:
	movl	-12(%ebp), %ebx
	movl	%edx, %eax
	movl	-8(%ebp), %esi
	movl	-4(%ebp), %edi
	leave
	jmp	_SetArgValue


DoIKind_BitOpDD:
	movl	%esi, _regs+fReg_opcode
	movl	%esi, %eax
	movl	%esi, %ecx
	andl	$7, %ecx
	shrl	$9, %eax
	andl	$7, %eax
	movsbl	_regs(,%eax,4),%eax
	leal	_regs(,%ecx,4), %edx
	movl	%edx, _regs+fReg_ArgAddr
	movl	$4, _regs+fReg_opsize
	movl	$1, _regs+fReg_ArgKind
	andl	$31, %eax
	jmp	_DoBinBitOp1


DoIKind_BitOpDM:
	movl	%esi, %eax
	movl	%esi, %edx
	shrl	$9, %eax
	andl	$7, %edx
	andl	$7, %eax
	movsbl	_regs(,%eax,4),%ebx
	movl	%esi, %eax
	shrl	$3, %eax
	andl	$7, %eax
	movl	$1, _regs+fReg_opsize
	andl	$7, %ebx
	call	_DecodeModeRegister
	movl	%ebx, %eax
	jmp	_DoBinBitOp1


DoIKind_BitOpND:
	movl	_regs+fReg_pc_p, %ecx
	subl	$2, %ecx
	movzbl	3(%ecx), %eax
	leal	4(%ecx), %edx
	movl	%esi, %ecx
	andl	$7, %ecx
	movl	%edx, _regs+fReg_pc_p
	leal	_regs(,%ecx,4), %edx
	movl	%edx, _regs+fReg_ArgAddr
	movl	$4, _regs+fReg_opsize
	andl	$31, %eax
	movl	$1, _regs+fReg_ArgKind
	jmp	_DoBinBitOp1


DoIKind_BitOpNM:
	movl	_regs+fReg_pc_p, %ecx
	subl	$2, %ecx
	movzbl	3(%ecx), %ebx
	leal	4(%ecx), %edx
	movl	%edx, _regs+fReg_pc_p
	movl	%esi, %edx
	movl	%esi, %eax
	shrl	$3, %eax
	andl	$7, %edx
	andl	$7, %eax
	andl	$7, %ebx
	movl	$1, _regs+fReg_opsize
	call	_DecodeModeRegister
	movl	%ebx, %eax
	jmp	_DoBinBitOp1


_DoBinBitOp1:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	movl	%eax, %ebx
	subl	$4, %esp
	call	_GetArgValue
	movl	%ebx, %ecx
	movl	%eax, %edx
	shrl	%cl, %eax
	xorl	$1, %eax
	andl	$1, %eax
	movb	%al, _regs+fReg_z
	movl	%esi, %eax
	shrl	$6, %eax
	andl	$3, %eax
	je	L286
	cmpl	$2, %eax
	je	L284
	cmpl	$3, %eax
	je	L285
	cmpl	$1, %eax
	jne	L282
	sall	%cl, %eax
	xorl	%eax, %edx
	jmp	L282
L284:
	movl	$-2, %eax
	movl	%ebx, %ecx
	roll	%cl, %eax
	andl	%eax, %edx
	jmp	L282
L285:
	movl	$1, %eax
	movl	%ebx, %ecx
	sall	%cl, %eax
	orl	%eax, %edx
L282:
	addl	$4, %esp
	movl	%edx, %eax
	popl	%ebx
	leave
	jmp	_SetArgValue
L286:
	addl	$4, %esp
	popl	%ebx
	leave
	jmp	m68k_NextInstruction


DoIKind_And:
#if 0
	call	_DecodeDEa_xxxxdddxssmmmrrr
	jmp	_DoBinOpAnd
#endif
	call	DecodeSrcDstGet
	andl	%ebx, %eax
	/* jmp	_SetCCRandDstValue */
_SetCCRandDstValue:
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
	jmp	_SetNZandDstValue


DoIKind_Or:
	call	DecodeSrcDstGet
	orl	%ebx, %eax
	jmp	_SetCCRandDstValue


DoIKind_Eor:
	call	DecodeSrcDstGet
	xorl	%ebx, %eax
	jmp	_SetCCRandDstValue


DoIKind_Not:
	call	DecodeDstGet
	notl	%eax
	/* jmp	_SetCCRandDstValue_eax */

_SetCCRandDstValue_eax:
	testl	%eax, %eax
	jmp	_SetCCRandDstValue


DoIKind_Scc:
	movl	$1, _regs+fReg_opsize
	call	_DecodeNormModeRegister
	call	_cctrue
	decb	%al
	notb	%al
	movzbl	%al, %eax
	jmp	_SetArgValue


DoIKind_EXTL:
	movl	%esi, %ecx
	andl	$7, %ecx
	movswl	_regs(,%ecx,4),%eax
	movl	%eax, _regs(,%ecx,4)
	testl	%eax, %eax
	jmp	_SetCCRandNext


DoIKind_EXTW:
	movl	%esi, %ecx
	andl	$7, %ecx
	movsbl	_regs(,%ecx,4), %eax
	movw	%ax, _regs(,%ecx,4)
	testl	%eax, %eax
	jmp	_SetCCRandNext


DoIKind_NegB:
	call	DecodeDstGet
	negb	%al
	jmp	_SetAddSubFlagsAndDstValue


DoIKind_NegW:
	call	DecodeDstGet
	negw	%ax
	jmp	_SetAddSubFlagsAndDstValue


DoIKind_NegL:
	call	DecodeDstGet
	negl	%eax
	jmp	_SetAddSubFlagsAndDstValue


DoIKind_NegXB:
	call	DecodeDstGet
	movb	_regs+fReg_x, %cl
	movl	%eax, %ebx
	xorl	%eax, %eax
	shrb	$1, %cl
	sbbb	%bl, %al
	jmp	_SetAddSubXFlagsAndDstValue


DoIKind_NegXW:
	call	DecodeDstGet
	movb	_regs+fReg_x, %cl
	movl	%eax, %ebx
	xorl	%eax, %eax
	shrb	$1, %cl
		/* load carry flag */
	sbbw	%bx, %ax
	jmp	_SetAddSubXFlagsAndDstValue


DoIKind_NegXL:
	call	DecodeDstGet
	movb	_regs+fReg_x, %cl
	movl	%eax, %ebx
	xorl	%eax, %eax
	shrb	$1, %cl
	sbbl	%ebx, %eax
	jmp	_SetAddSubXFlagsAndDstValue


DoIKind_MulU:
	movl	$2, _regs+fReg_opsize
	call	_GetDecodeNormModeRegisterVal
	movl	%esi, %ecx
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
	shrl	$9, %ecx
	andl	$7, %ecx
	movzwl	_regs(,%ecx,4), %edx
	movzwl	%ax, %eax
	imull	%eax, %edx
	testl	%edx, %edx
	movl	%edx, %eax
	sete	_regs+fReg_z
	shrl	$31, %eax
	movb	%al, _regs+fReg_n
	movl	%edx, _regs(,%ecx,4)
	jmp	m68k_NextInstruction


DoIKind_MulS:
	movl	$2, _regs+fReg_opsize
	call	_GetDecodeNormModeRegisterVal
	movl	%esi, %ecx
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
	shrl	$9, %ecx
	andl	$7, %ecx
	movswl	_regs(,%ecx,4),%edx
	cwtl
	imull	%eax, %edx
	testl	%edx, %edx
	movl	%edx, %eax
	sete	_regs+fReg_z
	shrl	$31, %eax
	movb	%al, _regs+fReg_n
	movl	%edx, _regs(,%ecx,4)
	jmp	m68k_NextInstruction


DoIKind_DivU:
	movl	$2, _regs+fReg_opsize
	call	_GetDecodeNormModeRegisterVal
	movl	%esi, %edi
	movl	%eax, %ecx
	movl	%edi, %eax
	shrl	$9, %eax
	andl	$7, %eax
	testl	%ecx, %ecx
	movl	_regs(,%eax,4), %ebx
	jne	L778
	movl	$5, %eax
	call	_Exception
	movl	%esi, %edi
	jmp	L780
L778:
	movzwl	%cx, %ecx
	xorl	%edx, %edx
	movl	%ebx, %eax
	divl	%ecx
	cmpl	$65535, %eax
	movl	%edx, %esi
	movl	%eax, %ecx
	jbe	L781
	movb	$1, _regs+fReg_v
	movb	$0, _regs+fReg_c
	movb	$1, _regs+fReg_n
	jmp	L780
L781:
	testw	%ax, %ax
	sete	_regs+fReg_z
	movzwl	%cx,%ebx
	shrw	$15, %ax
	sall	$16, %esi
	orl	%esi, %ebx
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
	movb	%al, _regs+fReg_n
L780:
	shrl	$9, %edi
	andl	$7, %edi
	movl	%ebx, _regs(,%edi,4)
	jmp	m68k_NextInstruction


DoIKind_DivS:
	movl	%esi, _regs+fReg_opcode
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	%ebx, -12(%ebp)
	movl	%esi, -8(%ebp)
	movl	%edi, -4(%ebp)
	movl	$2, _regs+fReg_opsize
	call	_GetDecodeNormModeRegisterVal
	movl	_regs+fReg_opcode, %ecx
	movl	%eax, %edx
	movl	%ecx, %eax
	shrl	$9, %eax
	andl	$7, %eax
	testl	%edx, %edx
	movl	_regs(,%eax,4), %ebx
	jne	L502
	movl	$5, %eax
	call	_Exception
	movl	_regs+fReg_opcode, %ecx
	jmp	L504
L502:
	movswl	%dx,%esi
	movl	%ebx, %edx
	sarl	$31, %edx
	movl	%ebx, %eax
	idivl	%esi
	movl	%eax, %esi
	andl	$-32768, %eax
	movl	%edx, %edi
	setne	%dl
	cmpl	$-32768, %eax
	setne	%al
	testb	%al, %dl
	je	L505
	movb	$1, _regs+fReg_v
	movb	$0, _regs+fReg_c
	movb	$1, _regs+fReg_n
	jmp	L504
L505:
	movl	%edi, %edx
	movl	%edi, %eax
	shrl	$31, %ebx
	negl	%eax
	shrw	$15, %dx
	cmpw	%bx, %dx
	movl	%esi, %ebx
	cmovne	%eax, %edi
	testw	%si, %si
	movl	%esi, %eax
	sete	_regs+fReg_z
	andl	$65535, %ebx
	shrw	$15, %ax
	movb	%al, _regs+fReg_n
	movl	%edi, %eax
	sall	$16, %eax
	orl	%eax, %ebx
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
L504:
	shrl	$9, %ecx
	movl	-8(%ebp), %esi
	andl	$7, %ecx
	movl	-4(%ebp), %edi
	movl	%ebx, _regs(,%ecx,4)
	movl	-12(%ebp), %ebx
	leave
	jmp	m68k_NextInstruction


DoIKind_Exgdd:
	movl	%esi, %eax
	movl	%esi, %ecx
	shrl	$9, %eax
	andl	$7, %ecx
	andl	$7, %eax
	movl	_regs(,%ecx,4), %edx
	movl	_regs(,%eax,4), %ebx
	movl	%edx, _regs(,%eax,4)
	movl	%ebx, _regs(,%ecx,4)
	jmp	m68k_NextInstruction


DoIKind_Exgaa:
	movl	%esi, %eax
	andl	$7, %esi
	shrl	$9, %eax
	andl	$7, %eax
	addl	$8, %eax
	leal	8(%esi), %ecx
	movl	_regs(,%eax,4), %ebx
	movl	_regs(,%ecx,4), %edx
	movl	%edx, _regs(,%eax,4)
	movl	%ebx, _regs(,%ecx,4)
	jmp	m68k_NextInstruction


DoIKind_Exgda:
	movl	%esi, %edx
	andl	$7, %esi
	shrl	$9, %edx
	andl	$7, %edx
	leal	8(%esi), %ecx
	movl	_regs(,%edx,4), %ebx
	movl	_regs(,%ecx,4), %eax
	movl	%eax, _regs(,%edx,4)
	movl	%ebx, _regs(,%ecx,4)
	jmp	m68k_NextInstruction


_m68k_setCR:
/* preserves %eax */
	movl	%eax, %edx
	shrw	%dx
	andl	$1, %edx
	movb	%dl, _regs+fReg_v
	movl	%eax, %edx
	andl	$1, %edx
	movb	%dl, _regs+fReg_c
	movl	%eax, %edx
	shrw	$3, %dx
	andl	$1, %edx
	movb	%dl, _regs+fReg_n
	movl	%eax, %edx
	shrw	$2, %dx
	andl	$1, %edx
	movb	%dl, _regs+fReg_z
	movl	%eax, %edx
	shrw	$4, %dx
	andl	$1, %edx
	movb	%dl, _regs+fReg_x
	ret

_SetExternalInterruptPending:
	movb	$1, _regs+fReg_ExtIntPend
	/* jmp	_NeedToGetOut */

_NeedToGetOut:
	movl	_regs+fReg_MaxCyclesToGo, %eax
	testl	%eax, %eax
	jle	NeedToGetOut_L0
	addl	%eax, _regs+fReg_MoreCyclesToGo
	movl	$0, _regs+fReg_MaxCyclesToGo
NeedToGetOut_L0:
	ret

_m68k_setSR:
	pushl	%ebp
	movl	$_regs+fReg_isp, %ecx
	cmpb	$0, _regs+fReg_s
	movl	%esp, %ebp
	pushl	%edi
	movl	_regs+fReg_intmask, %edi
	pushl	%esi
	movl	%ecx, %esi
	pushl	%ebx
	movl	$_regs+fReg_usp, %ebx
	cmove	%ebx, %esi

	call	_m68k_setCR

	movl	%eax, %edx
	shrw	$15, %dx
	movb	%dl, _regs+fReg_t1
	movl	%eax, %edx
	movzbl	%ah, %eax
	shrw	$13, %dx
	andl	$7, %eax
	andl	$1, %edx
	testb	%dl, %dl
	movb	%dl, _regs+fReg_s
	movl	%ecx, %edx
	cmove	%ebx, %edx
	cmpl	%edx, %esi
	movl	%eax, _regs+fReg_intmask
	je	L339
	movl	_regs+fReg_A7, %eax
	movl	%eax, (%esi)
	movl	(%edx), %eax
	movl	%eax, _regs+fReg_A7
L339:
	cmpl	%edi, _regs+fReg_intmask
	je	L341
	call	_SetExternalInterruptPending
L341:
	cmpb	$0, _regs+fReg_t1
	je	L347
	call	_NeedToGetOut
L347:
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret

DoIKind_MoveCCREa:
	movl	$2, _regs+fReg_opsize
	call	_DecodeNormModeRegister
	call	_m68k_getSR
	movzbl	%al, %eax
	jmp	_SetArgValue

DoIKind_MoveEaCCR:
	movl	$2, _regs+fReg_opsize
	call	_GetDecodeNormModeRegisterVal
	call	_m68k_setCR
	jmp	m68k_NextInstruction


DoIKind_MoveSREa:
	movl	$2, _regs+fReg_opsize
	call	_DecodeNormModeRegister
	call	_m68k_getSR
	movzwl	%ax, %eax
	jmp	_SetArgValue


DoIKind_MoveEaSR:
	movl	$2, _regs+fReg_opsize
	call	_DecodeNormModeRegister
	call	_GetArgValue
	movzwl	%ax, %eax
	call	_m68k_setSR
	jmp	m68k_NextInstruction


DoIKind_BinOpStatusCCR:
	pushl	%edi
	movl	%esi, %ecx
	shrl	$6, %ecx
	andl	$3, %ecx
	setne	%al
	movzbl	%al, %edi
	movl	$1, %eax
	sall	%cl, %eax
	testl	%edi, %edi
	movl	%eax, _regs+fReg_opsize
	je	L511
	cmpb	$0, _regs+fReg_s
	jne	L511
	subl	$2, _regs+fReg_pc_p
	movl	$8, %eax
	call	_Exception
	jmp	DoIKind_BinOpStatusCCR_exit
L511:
	movl	_regs+fReg_pc_p, %eax
	movl	%esi, %ecx
	shrl	$9, %ecx
	andl	$7, %ecx
	movzwl	(%eax), %edx
	addl	$2, %eax
	movl	%eax, _regs+fReg_pc_p
	rolw	$8, %dx
	movswl	%dx,%ebx
	call	_m68k_getSR
	movl	%eax, %edx

	cmpl	$1, %ecx
	movzwl	%dx, %eax
	je	L516
	jb	L515
	cmpl	$5, %ecx
	jne	L514
	jmp	L522
L515:
	orl	%ebx, %eax
	jmp	L514
L516:
	andl	%ebx, %eax
	jmp	L514
L522:
	xorl	%ebx, %eax
L514:
	testl	%edi, %edi
	je	L518
	movzwl	%ax, %eax
	call	_m68k_setSR
	jmp	DoIKind_BinOpStatusCCR_exit
L518:
	call	_m68k_setCR
DoIKind_BinOpStatusCCR_exit:
	popl	%edi
	jmp	m68k_NextInstruction


DoIKind_MOVEMRmMW:
	movl	%esi, _regs+fReg_opcode
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	movl	$_regs, %esi
	pushl	%ebx
	xorl	%ebx, %ebx
	subl	$28, %esp
	movl	_regs+fReg_pc_p, %eax
	movzwl	(%eax), %edx
	addl	$2, %eax
	movl	%eax, _regs+fReg_pc_p
	movl	_regs+fReg_opcode, %eax
	rolw	$8, %dx
	andl	$7, %eax
	movzwl	%dx, %edx
	movl	_regs+32(,%eax,4), %edi
	movl	%edx, -28(%ebp)
L312:
	movl	$1, %eax
	movl	%ebx, %ecx
	sall	%cl, %eax
	testl	%eax, -28(%ebp)
	je	L313
	movl	60(%esi), %edx
	subl	$2, %edi
	movl	%edi, %eax
	call	_put_word
L313:
	incl	%ebx
	subl	$4, %esi
	cmpl	$16, %ebx
	jne	L312
	movl	_regs+fReg_opcode, %edx
	movl	$_regs, %eax
	andl	$7, %edx
	movl	%edi, 32(%eax,%edx,4)
	addl	$28, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	jmp	m68k_NextInstruction


DoIKind_MOVEMrm:
	movl	%esi, _regs+fReg_opcode
	movl	%esi, %ecx
	xorl	%eax, %eax
	shrl	$3, %esi
	andl	$7, %ecx
	andl	$7, %esi
	movl	%esi, %edx
	jmp	_reglist


DoIKind_MOVEMRmML:
	movl	%esi, _regs+fReg_opcode
	movl	_regs+fReg_pc_p, %ecx
	movl	%ecx, %ebx
	subl	$2, %ecx
	movzwl	(%ebx), %eax
	andl	$7, %esi
	movl	$_regs, %ebx
	movl	_regs+32(,%esi,4), %esi
	leal	4(%ecx), %edx
	xorl	%edi, %edi
	movl	%edx, _regs+fReg_pc_p
	rolw	$8, %ax
	movzwl	%ax, %eax
	movl	%eax, -32(%ebp)
L698:
	movl	$1, %eax
	movl	%edi, %ecx
	sall	%cl, %eax
	testl	%eax, -32(%ebp)
	je	L699
	movl	60(%ebx), %edx
	subl	$4, %esi
	movl	%esi, %eax
	call	_put_long
L699:
	incl	%edi
	subl	$4, %ebx
	cmpl	$16, %edi
	jne	L698
	movl	_regs+fReg_opcode, %eax
	andl	$7, %eax
	movl	%esi, _regs+32(,%eax,4)
	jmp	m68k_NextInstruction


DoIKind_MOVEMApRW:
	movl	%esi, _regs+fReg_opcode
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	movl	$_regs, %esi
	pushl	%ebx
	xorl	%ebx, %ebx
	subl	$28, %esp
	movl	_regs+fReg_pc_p, %eax
	movzwl	(%eax), %edx
	addl	$2, %eax
	movl	%eax, _regs+fReg_pc_p
	movl	_regs+fReg_opcode, %eax
	rolw	$8, %dx
	andl	$7, %eax
	movzwl	%dx, %edx
	movl	_regs+32(,%eax,4), %edi
	movl	%edx, -28(%ebp)
L320:
	movl	$1, %eax
	movl	%ebx, %ecx
	sall	%cl, %eax
	testl	%eax, -28(%ebp)
	je	L321
	movl	%edi, %eax
	addl	$2, %edi
	call	_get_word
	movl	%eax, (%esi)
L321:
	incl	%ebx
	addl	$4, %esi
	cmpl	$16, %ebx
	jne	L320
	movl	_regs+fReg_opcode, %edx
	movl	$_regs, %eax
	andl	$7, %edx
	movl	%edi, 32(%eax,%edx,4)
	addl	$28, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	jmp	m68k_NextInstruction


DoIKind_MOVEMApRL:
	movl	%esi, _regs+fReg_opcode
	movl	_regs+fReg_pc_p, %ecx
	movl	%ecx, %ebx
	subl	$2, %ecx
	movzwl	(%ebx), %eax
	andl	$7, %esi
	xorl	%ebx, %ebx
	leal	4(%ecx), %edx
	movl	_regs+32(,%esi,4), %edi
	movl	$_regs, %esi
	movl	%edx, _regs+fReg_pc_p
	rolw	$8, %ax
	movzwl	%ax, %eax
	movl	%eax, -28(%ebp)
L702:
	movl	$1, %eax
	movl	%ebx, %ecx
	sall	%cl, %eax
	testl	%eax, -28(%ebp)
	je	L703
	movl	%edi, %eax
	addl	$4, %edi
	call	_get_long
	movl	%eax, (%esi)
L703:
	incl	%ebx
	addl	$4, %esi
	cmpl	$16, %ebx
	jne	L702
	movl	_regs+fReg_opcode, %eax
	andl	$7, %eax
	movl	%edi, _regs+32(,%eax,4)
	jmp	m68k_NextInstruction


DoIKind_MOVEMmr:
	movl	%esi, _regs+fReg_opcode
	movl	%esi, %ecx
	movl	$1, %eax
	shrl	$3, %esi
	andl	$7, %ecx
	andl	$7, %esi
	movl	%esi, %edx
	jmp	_reglist


_reglist:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	%eax, %ebx
	subl	$28, %esp
	movl	%edx, %eax
	movl	_regs+fReg_pc_p, %edx
	movl	%ebx, %esi
	movzwl	(%edx), %ebx
	addl	$2, %edx
	movl	%edx, _regs+fReg_pc_p
	movl	_regs+fReg_opcode, %edx
	rolw	$8, %bx
	shrl	$5, %edx
	movzwl	%bx, %ebx
	andl	$6, %edx
	subl	$2, %edx
	movl	%edx, _regs+fReg_opsize
	movl	%ecx, %edx
	movl	%ebx, -28(%ebp)
	call	_DecodeModeRegister
	testw	%si, %si
	movl	_regs+fReg_ArgAddr, %edi
	jne	L420
	cmpl	$2, _regs+fReg_opsize
	jne	L422
	xorl	%esi, %esi
	movl	$_regs, %ebx
L424:
	movl	$1, %eax
	movl	%esi, %ecx
	sall	%cl, %eax
	testl	%eax, -28(%ebp)
	je	L425
	movl	(%ebx), %edx
	movl	%edi, %eax
	addl	$2, %edi
	call	_put_word
L425:
	incl	%esi
	addl	$4, %ebx
	cmpl	$16, %esi
	jne	L424
	jmp	L439
L422:
	xorl	%ebx, %ebx
	movl	$_regs, %esi
L428:
	movl	$1, %eax
	movl	%ebx, %ecx
	sall	%cl, %eax
	testl	%eax, -28(%ebp)
	je	L429
	movl	(%esi), %edx
	movl	%edi, %eax
	addl	$4, %edi
	call	_put_long
L429:
	incl	%ebx
	addl	$4, %esi
	cmpl	$16, %ebx
	jne	L428
	jmp	L439
L420:
	cmpl	$2, _regs+fReg_opsize
	jne	L431
	xorl	%esi, %esi
	movl	$_regs, %ebx
L433:
	movl	$1, %eax
	movl	%esi, %ecx
	sall	%cl, %eax
	testl	%eax, -28(%ebp)
	je	L434
	movl	%edi, %eax
	addl	$2, %edi
	call	_get_word
	movl	%eax, (%ebx)
L434:
	incl	%esi
	addl	$4, %ebx
	cmpl	$16, %esi
	jne	L433
	jmp	L439
L431:
	xorl	%ebx, %ebx
	movl	$_regs, %esi
L436:
	movl	$1, %eax
	movl	%ebx, %ecx
	sall	%cl, %eax
	testl	%eax, -28(%ebp)
	je	L437
	movl	%edi, %eax
	addl	$4, %edi
	call	_get_long
	movl	%eax, (%esi)
L437:
	incl	%ebx
	addl	$4, %esi
	cmpl	$16, %ebx
	jne	L436
L439:
	addl	$28, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	jmp	m68k_NextInstruction


DoIKind_Abcdr:
	movl	%esi, %edx
	xorl	%ecx, %ecx
	shrl	$9, %esi
	andl	$7, %edx
	andl	$7, %esi
	xorl	%eax, %eax
	movl	%esi, (%esp)
	jmp	_DoBinOpAbcd


DoIKind_Abcdm:
	movl	$4, %ecx
	movl	%esi, %eax
	movl	%eax, %edx
	shrl	$9, %eax
	andl	$7, %edx
	andl	$7, %eax
	movl	%eax, (%esp)
	movl	$4, %eax
	/* jmp	_DoBinOpAbcd */

_DoBinOpAbcd:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	movl	%ecx, %esi
	pushl	%ebx
	subl	$28, %esp
	movl	4(%ebp), %ebx
	movl	$1, _regs+fReg_opsize
	call	_DecodeModeRegister
	call	_GetArgValue
	movl	%ebx, %edx
	movl	%eax, %edi
	movl	%esi, %eax
	call	_DecodeModeRegister
	movl	%edi, %ebx
	movl	%edi, %esi
	call	_GetArgValue
	movl	%edi, %ecx
	andl	$15, %ecx
	shrl	$31, %ebx
	movl	%eax, %edx
	shrl	$31, %edx
	movl	%edx, -28(%ebp)
	movl	%eax, %edx
	andl	$15, %edx
	addl	%edx, %ecx
	xorl	%edx, %edx
	cmpb	$0, _regs+fReg_x
	setne	%dl
	andw	$240, %ax
	addl	%edx, %ecx
	andw	$240, %si
	leal	(%esi,%eax), %edx
	cmpw	$10, %cx
	leal	6(%ecx), %eax
	cmovae	%eax, %ecx
	addl	%edx, %ecx
	movl	%ecx, %eax
	andl	$496, %eax
	cmpl	$144, %eax
	setg	%al
	testb	%al, %al
	leal	96(%ecx), %edx
	cmove	%ecx, %edx
	movsbl	%dl,%ecx
	movb	%al, _regs+fReg_x
	movb	%al, _regs+fReg_c
	xorl	%eax, %eax
	testb	%dl, %dl
	movzbl	_regs+fReg_z, %edx
	cmove	%edx, %eax
	movb	%al, _regs+fReg_z
	movl	%ecx, %eax
	shrl	$31, %eax
	cmpl	-28(%ebp), %ebx
	movb	%al, _regs+fReg_n
	je	L455
	movzbl	%al, %eax
	movl	$1, %edx
	cmpl	%eax, -28(%ebp)
	jne	L458
L455:
	xorl	%edx, %edx
L458:
	movb	%dl, _regs+fReg_v
	addl	$28, %esp
	movl	%ecx, %eax
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	jmp	_SetArgValue


DoIKind_Sbcdr:
	xorl	%ecx, %ecx
	movl	%esi, %eax
	movl	%eax, %edx
	shrl	$9, %eax
	andl	$7, %edx
	andl	$7, %eax
	movl	%eax, (%esp)
	xorl	%eax, %eax
	jmp	_DoBinOpSbcd


DoIKind_Sbcdm:
	movl	$4, %ecx
	movl	%esi, %eax
	movl	%eax, %edx
	shrl	$9, %eax
	andl	$7, %edx
	andl	$7, %eax
	movl	%eax, (%esp)
	movl	$4, %eax
	/* jmp	_DoBinOpSbcd */


_DoBinOpSbcd:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	movl	%ecx, %esi
	pushl	%ebx
	subl	$28, %esp
	movl	4(%ebp), %ebx
	movl	$1, _regs+fReg_opsize
	call	_DecodeModeRegister
	call	_GetArgValue
	movl	%ebx, %edx
	movl	%eax, %edi
	movl	%esi, %eax
	call	_DecodeModeRegister
	movl	%edi, %ebx
	call	_GetArgValue
	movl	%edi, %edx
	shrl	$31, %edx
	movl	%edx, -28(%ebp)
	movl	%edi, %edx
	andl	$15, %edx
	movl	%eax, %ecx
	movl	%eax, %esi
	andl	$15, %ecx
	subw	%dx, %cx
	xorl	%edx, %edx
	shrl	$31, %esi
	cmpb	$0, _regs+fReg_x
	setne	%dl
	andw	$240, %ax
	andw	$240, %bx
	subw	%dx, %cx
	subw	%bx, %ax
	cmpw	$9, %cx
	jbe	L463
	subl	$6, %ecx
	subl	$16, %eax
L463:
	andl	$15, %ecx
	addl	%eax, %ecx
	andl	$496, %eax
	cmpl	$144, %eax
	setg	%al
	testb	%al, %al
	leal	-96(%ecx), %edx
	cmove	%ecx, %edx
	movsbl	%dl,%ecx
	movb	%al, _regs+fReg_x
	movb	%al, _regs+fReg_c
	xorl	%eax, %eax
	testb	%dl, %dl
	movzbl	_regs+fReg_z, %edx
	cmove	%edx, %eax
	movb	%al, _regs+fReg_z
	movl	%ecx, %eax
	shrl	$31, %eax
	cmpl	%esi, -28(%ebp)
	movb	%al, _regs+fReg_n
	je	L469
	movzbl	%al, %eax
	movl	$1, %edx
	cmpl	%eax, %esi
	jne	L472
L469:
	xorl	%edx, %edx
L472:
	movb	%dl, _regs+fReg_v
	addl	$28, %esp
	movl	%ecx, %eax
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	jmp	_SetArgValue


DoIKind_Nbcd:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	$1, _regs+fReg_opsize
	call	_GetDecodeNormModeRegisterVal
	xorl	%edx, %edx
	movl	%eax, %ecx
	andl	$15, %ecx
	negl	%ecx
	cmpb	$0, _regs+fReg_x
	setne	%dl
	andw	$240, %ax
	subw	%dx, %cx
	negl	%eax
	cmpw	$9, %cx
	jbe	L479
	subl	$6, %ecx
	subl	$16, %eax
L479:
	andl	$15, %ecx
	leal	(%ecx,%eax), %edx
	andl	$496, %eax
	cmpl	$144, %eax
	setg	%al
	testb	%al, %al
	leal	-96(%edx), %ecx
	cmove	%edx, %ecx
	movb	%al, _regs+fReg_x
	movb	%al, _regs+fReg_c
	movsbl	%cl,%eax
	movl	%eax, %edx
	shrl	$31, %edx
	movb	%dl, _regs+fReg_n
	xorl	%edx, %edx
	testb	%cl, %cl
	movzbl	_regs+fReg_z, %ecx
	cmove	%ecx, %edx
	movb	%dl, _regs+fReg_z
	leave
	jmp	_SetArgValue


DoIKind_Rte:
	cmpb	$0, _regs+fReg_s
	movl	_regs+fReg_A7, %ebx
	movl	%ebx, %eax
	je	_BackupException8AndNext
	addl	$2, %ebx
	call	_get_word
	movzwl	%ax, %eax
	pushl	%eax
	movl	%ebx, %eax
	addl	$4, %ebx
	call	_get_long
	movl	%ebx, _regs+fReg_A7
	movl	%eax, %ebx
	popl	%eax
	call	_m68k_setSR
	movl	%ebx, %eax
	call	_m68k_setpc
	jmp	m68k_NextInstruction

_BackupException8AndNext:
	movl	$8, %eax
	jmp	_BackupExceptionAndNext


DoIKind_MoveP:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	%esi, %ecx
	movl	%ebx, -12(%ebp)
	movl	%esi, -8(%ebp)
	movl	%edi, -4(%ebp)
	movl	_regs+fReg_pc_p, %eax
	movzwl	(%eax), %edx
	addl	$2, %eax
	movl	%eax, _regs+fReg_pc_p
	movl	%ecx, %eax
	movl	%ecx, %ebx
	andl	$7, %eax
	shrl	$9, %ebx
	rolw	$8, %dx
	andl	$7, %ebx
	movzwl	%dx, %esi
	addl	_regs+32(,%eax,4), %esi
	movl	%ecx, %eax
	shrl	$6, %eax
	andl	$3, %eax
	cmpl	$1, %eax
	je	L305
	jb	L304
	cmpl	$2, %eax
	je	L306
	cmpl	$3, %eax
	jne	L308
	jmp	L310
L304:
	movl	%esi, %eax
	call	_get_byte
	movl	%eax, %edi
	leal	2(%esi), %eax
	call	_get_byte
	movl	%edi, %edx
	sall	$8, %edx
	andw	$255, %ax
	orl	%eax, %edx
	movw	%dx, _regs(,%ebx,4)
	jmp	L308
L305:
	movl	%esi, %eax
	call	_get_byte
	leal	2(%esi), %eax
	call	_get_byte
	leal	4(%esi), %eax
	call	_get_byte
	leal	6(%esi), %eax
	call	_get_byte
	movzbl	%al, %eax
	movl	%eax, _regs(,%ebx,4)
	jmp	L308
L306:
	movswl	_regs(,%ebx,4),%ebx
	movl	%esi, %eax
	movsbl	%bh, %edx
	call	_put_byte
	leal	2(%esi), %eax
	jmp	L309
L310:
	movl	_regs(,%ebx,4), %ebx
	movl	%esi, %eax
	movl	%ebx, %edx
	sarl	$24, %edx
	call	_put_byte
	movl	%ebx, %edx
	sarl	$16, %edx
	leal	2(%esi), %eax
	call	_put_byte
	movl	%ebx, %edx
	leal	4(%esi), %eax
	sarl	$8, %edx
	call	_put_byte
	leal	6(%esi), %eax
L309:
	movl	%ebx, %edx
	movl	-8(%ebp), %esi
	movl	-12(%ebp), %ebx
	movl	-4(%ebp), %edi
	leave
	call	_put_byte
	jmp	m68k_NextInstruction
L308:
	movl	-12(%ebp), %ebx
	movl	-8(%ebp), %esi
	movl	-4(%ebp), %edi
	leave
	jmp	m68k_NextInstruction


DoIKind_CallMorRtm:
DoIKind_Illegal:
	movl	$4, %eax
	jmp	_BackupExceptionAndNext


DoIKind_ChkW:
	movl	$2, _regs+fReg_opsize
	/* jmp	_DoCheck */

_DoCheck:
	call	_GetDecodeNormModeRegisterVal
	movl	%esi, %edx
	shrl	$9, %edx
	andl	$7, %edx
	movl	%eax, %ebx
	call	SetArgKindDReg
	call	_GetArgValue
	testl	%eax, %eax
	jns	L534
	movb	$1, _regs+fReg_n
	jmp	L539
L534:
	cmpl	%ebx, %eax
	jle	L538
	movb	$0, _regs+fReg_n
L539:
	movl	$6, %eax
	jmp	_ExceptionAndNext
L538:
	jmp	m68k_NextInstruction


DoIKind_Trap:
	andl	$15, %esi
	leal	32(%esi), %eax
	jmp	_ExceptionAndNext


DoIKind_TrapV:
	cmpb	$0, _regs+fReg_v
	je	m68k_NextInstruction
	movl	$7, %eax
	jmp	_ExceptionAndNext


DoIKind_Rtr:
	movl	_regs+fReg_A7, %ebx
	movl	%ebx, %eax
	addl	$2, %ebx
	call	_get_word
	call	_m68k_setCR
	movl	%ebx, %eax
	addl	$4, %ebx
	call	_get_long

	movl	%ebx, _regs+fReg_A7
	call	_m68k_setpc
	jmp	m68k_NextInstruction


DoIKind_MoveRUSP:
	cmpb	$0, _regs+fReg_s
	je	_BackupException8AndNext
	andl	$7, %esi
	movl	_regs+32(,%esi,4), %eax
	movl	%eax, _regs+fReg_usp
	jmp	m68k_NextInstruction


DoIKind_MoveUSPR:
	cmpb	$0, _regs+fReg_s
	movl	_regs+fReg_usp, %eax
	je	_BackupException8AndNext
	andl	$7, %esi
	movl	%eax, _regs+32(,%esi,4)
	jmp	m68k_NextInstruction


DoIKind_Tas:
	movl	$1, _regs+fReg_opsize
	call	_GetDecodeNormModeRegisterVal
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
	testl	%eax, %eax
	movl	%eax, %edx
	sete	_regs+fReg_z
	orb	$-128, %al
	shrl	$31, %edx
	movb	%dl, _regs+fReg_n
	jmp	_SetArgValue


DoIKind_F:
	movl	$11, %eax
	jmp	_BackupExceptionAndNext


DoIKind_Stop:
	cmpb	$0, _regs+fReg_s
	movl	_regs+fReg_pc_p, %ecx
	movl	%ecx, %ebx
	je	_BackupException8AndNext
	subl	$2, %ecx
	movzwl	(%ebx), %eax
	leal	4(%ecx), %edx
	movl	%edx, _regs+fReg_pc_p
	rolw	$8, %ax
	movzwl	%ax, %eax
	call	_m68k_setSR
	movl	$4, %eax
	jmp	_ExceptionAndNext


DoIKind_Link:
	movl	_regs+fReg_A7, %ebx
	andl	$7, %esi
	addl	$8, %esi
	subl	$4, %ebx
	movl	%ebx, _regs+fReg_A7
	movl	_regs(,%esi,4), %edx
	movl	%ebx, %eax
	call	_put_long
	movl	_regs+fReg_pc_p, %edx
	movl	%ebx, _regs(,%esi,4)
	movzwl	(%edx), %eax
	addl	$2, %edx
	movl	%edx, _regs+fReg_pc_p
	rolw	$8, %ax
	cwtl
	addl	_regs+fReg_A7, %eax
	movl	%eax, _regs+fReg_A7
	jmp	m68k_NextInstruction


DoIKind_Unlk:
	movl	%esi, %eax
	andl	$7, %eax
	cmpl	$7, %eax
	je	L784
	leal	8(%eax), %esi
	movl	_regs(,%esi,4), %ebx
	movl	%ebx, %eax
	addl	$4, %ebx
	call	_get_long
	movl	%eax, _regs(,%esi,4)
	movl	%ebx, _regs+fReg_A7
	jmp	m68k_NextInstruction
L784:
	movl	_regs+fReg_A7, %eax
	call	_get_long
	addl	$4, %eax
	movl	%eax, _regs+fReg_A7
	jmp	m68k_NextInstruction


DoIKind_Reset:
	cmpb	$0, _regs+fReg_s
	je	_BackupException8AndNext
	my_call_imported_proc customreset
	jmp	m68k_NextInstruction


_callMemAccessNtfy:
	/*
		%eax - input
		%ecx - preserved
		flags - set according to result of MemAccessNtfy
	*/
	pushl	%ecx

	pushl	%ebp
	movl	%esp, %ebp

	subl	$72, %esp

	andl	$-16, %esp
		/* align stack ? */

	movl	%eax, (%esp)
	my_call_imported_proc MemAccessNtfy
	leave

	popl	%ecx

	testl	%eax, %eax

	ret


_SetUpMATC:
	/*
		%eax - source
		%ecx - input: emulated address, output: real address
		%edx - destination, preserved
	*/

	pushl	%ecx

	movl	4(%eax), %ecx
	movl	%ecx, (%edx)
	movl	8(%eax), %ecx
	movl	%ecx, 4(%edx)
	movl	16(%eax), %ecx
	movl	%ecx, 8(%edx)
	movl	20(%eax), %ecx
	movl	%ecx, 12(%edx)

	popl	%ecx

	andl	16(%eax), %ecx
	addl	20(%eax), %ecx

	ret


_local_FindATTel:
	/*
		%ecx = addr - input (not %eax), preserved

		%edi = prev
		%eax = p - output
	*/

	movl	_regs+fHeadATTel, %eax
	movl	4(%eax), %edx
	andl	%ecx, %edx
	cmpl	8(%eax), %edx
	je	local_FindATTel_L4
	pushl	%edi
local_FindATTel_L2:
	movl	%eax, %edi
	movl	(%eax), %eax
	movl	4(%eax), %edx
	andl	%ecx, %edx
	cmpl	8(%eax), %edx
	jne	local_FindATTel_L2
	movl	(%eax), %edx
	testl	%edx, %edx
	je	local_FindATTel_L3
	movl	%edx, (%edi)
	movl	_regs+fHeadATTel, %edx
	movl	%edx, (%eax)
	movl	%eax, _regs+fHeadATTel
local_FindATTel_L3:
	popl	%edi
local_FindATTel_L4:

	ret


_get_byte_ext:

MM_Access_RdB_retry:
	call	_local_FindATTel
	movl	12(%eax), %edx
	testb	$1, %dl
	je	MM_Access_RdB_L17

	movl	$fReg_MATCrdB_cmpmask+_regs, %edx
	call	_SetUpMATC

	movsbl	(%ecx), %eax
	ret

MM_Access_RdB_L17:
	testb	$4, %dl
	je	MM_Access_RdB_L19

	movw	$0x0100, %dx
	/* jmp	_callMMDV_Access_Rd */

/* _callMMDV_Access_Rd: */
	pushl	%ebp
	movl	%esp, %ebp

	subl	$20, %esp

	andl	$-16, %esp
		/* align stack */

	movl	%eax, (%esp)
	xorl	%eax, %eax
	movl	%eax, 4(%esp)
	movzbl	%dl, %eax
	movl	%eax, 8(%esp)
	movzbl	%dh, %eax
	movl	%eax, 12(%esp)
	movl	%ecx, 16(%esp)
	my_call_imported_proc MMDV_Access
	movsbl	%al,%eax
	leave
	ret

MM_Access_RdB_L19:
	testb	$8, %dl
	jne	MM_Access_RdB_L20
MM_Access_RdB_failure:
	xorl	%eax, %eax
	ret

MM_Access_RdB_L20:

	call _callMemAccessNtfy

	je	MM_Access_RdB_failure
	jmp	MM_Access_RdB_retry


_put_byte_ext:
	pushl	%edx

MM_Access_WrB_retry:
	call	_local_FindATTel
	movl	12(%eax), %edx
	testb	$2, %dl
	je	MM_Access_WrB_L17

	movl	$fReg_MATCwrB_cmpmask+_regs, %edx
	call	_SetUpMATC

	popl	%edx
	movb	%dl, (%ecx)
	ret

MM_Access_WrB_L17:
	testb	$4, %dl
	je	MM_Access_WrB_L19

	popl	%edx
	movzbl	%dl, %edx
	pushl	%edx

	movw	$0x0101, %dx

	jmp	_callMMDV_Access_Wr

MM_Access_WrB_L19:
	testb	$8, %dl
	jne	MM_Access_WrB_L20
MM_Access_WrB_failure:

	popl	%edx

	ret

MM_Access_WrB_L20:

	call _callMemAccessNtfy

	je	MM_Access_WrB_failure
	jmp	MM_Access_WrB_retry


_get_word_ext:
	testl	$1, %ecx
	jne	_get_word_ext_misaligned

MM_Access_RdW_retry:
	call	_local_FindATTel
	movl	12(%eax), %edx
	testb	$1, %dl
	je	MM_Access_RdW_L17

	/* test: jmp	_get_word_ext_misaligned */
	movl	$fReg_MATCrdW_cmpmask+_regs, %edx
	call	_SetUpMATC
	orl	$1, (%edx)

	movzwl	(%ecx), %eax
	rolw	$8, %ax
	cwtl
	ret

MM_Access_RdW_L17:
	testb	$4, %dl
	je	MM_Access_RdW_L19

	movw	$0x0000, %dx
	/* jmp	_callMMDV_Access_Rd */

/* _callMMDV_Access_Rd: */
	pushl	%ebp
	movl	%esp, %ebp

	subl	$20, %esp

	andl	$-16, %esp
		/* align stack */

	movl	%eax, (%esp)
	xorl	%eax, %eax
	movl	%eax, 4(%esp)
	movzbl	%dl, %eax
	movl	%eax, 8(%esp)
	movzbl	%dh, %eax
	movl	%eax, 12(%esp)
	movl	%ecx, 16(%esp)
	my_call_imported_proc MMDV_Access
	cwtl
	leave
	ret

MM_Access_RdW_L19:
	testb	$8, %dl
	jne	MM_Access_RdW_L20
MM_Access_RdW_failure:
	xorl	%eax, %eax
	ret

MM_Access_RdW_L20:

	call _callMemAccessNtfy

	je	MM_Access_RdW_failure
	jmp	MM_Access_RdW_retry

_get_word_ext_misaligned:
	pushl	%ecx
	movl	%ecx, %eax
	call	_get_byte
	popl	%ecx
	pushl	%eax
	leal	1(%ecx), %eax
	call	_get_byte
	popl	%edx
	sall	$8, %edx
	andl	$255, %eax
	orl	%edx, %eax
	ret


_put_word_ext:
	testl	$1, %ecx
	pushl	%edx
	jne	_put_word_ext_misaligned

MM_Access_WrW_retry:
	call	_local_FindATTel
	movl	12(%eax), %edx
	testb	$2, %dl
	je	MM_Access_WrW_L17

	movl	$fReg_MATCwrW_cmpmask+_regs, %edx
	call	_SetUpMATC
	orl	$1, (%edx)
	/* test: orl	$0xFFFFFFFF, (%edx) */

	popl	%edx
	rolw	$8, %dx
	movw	%dx, (%ecx)
	ret

MM_Access_WrW_L17:
	testb	$4, %dl
	je	MM_Access_WrW_L19

	popl	%edx
	movzwl	%dx, %edx
	pushl	%edx

	movw	$0x0001, %dx
	/* jmp	_callMMDV_Access_Wr */

_callMMDV_Access_Wr:
	pushl	%ebp
	movl	%esp, %ebp

	subl	$20, %esp

	andl	$-16, %esp
		/* align stack */

	movl	%eax, (%esp)
	movl	4(%ebp), %eax
		/* from %edx */
	movl	%eax, 4(%esp)
	movzbl	%dl, %eax
	movl	%eax, 8(%esp)
	movzbl	%dh, %eax
	movl	%eax, 12(%esp)
	movl	%ecx, 16(%esp)
	my_call_imported_proc MMDV_Access
	leave
	addl	$4, %esp
		/* no longer need saved %edx */
	ret

MM_Access_WrW_L19:
	testb	$8, %dl
	jne	MM_Access_WrW_L20
MM_Access_WrW_failure:

	popl	%edx

	ret

MM_Access_WrW_L20:

	call _callMemAccessNtfy

	je	MM_Access_WrW_failure
	jmp	MM_Access_WrW_retry

_put_word_ext_misaligned:
	pushl	%ecx
	movl	%ecx, %eax
	shrl	$8, %edx
	call	_put_byte
	popl	%eax
		/* from %ecx */
	popl	%edx
	leal	1(%eax), %eax
	jmp	_put_byte


_get_long_ext:
	pushl	%ecx
	movl	%ecx, %eax
	call	_get_word
	popl	%ecx
	pushl	%eax
	leal	2(%ecx), %eax
	call	_get_word
	popl	%edx
	sall	$16, %edx
	andl	$65535, %eax
	orl	%edx, %eax
	ret


_put_long_ext:
	pushl	%edx
	pushl	%ecx
	movl	%ecx, %eax
	shrl	$16, %edx
	call	_put_word
	popl	%eax
		/* from %ecx */
	popl	%edx
	leal	2(%eax), %eax
	jmp	_put_word


get0_pc_real_address_ext:

get_pc_real_address_retry:
	call	_local_FindATTel
	movl	12(%eax), %edx
	testb	$1, %dl
	je	L51

	movl	$fReg_MATCex_cmpmask+_regs, %edx
	call	_SetUpMATC

	movl	%ecx, %eax

	jmp _end_get0_pc_real_address

L51:
	testb	$8, %dl
	je	L52

	call	_callMemAccessNtfy
	jne	get_pc_real_address_retry
L52:
	/* call	L_DoReportAbnormal$stub */
	/* movl	L_RAM$non_lazy_ptr, %eax */
	movl	$0, _regs+fReg_MATCex_cmpmask
	movl	$-1, _regs+fReg_MATCex_cmpvalu
	movl	$fReg_fakeword+_regs, %eax

	jmp _end_get0_pc_real_address


	my_define_exported_proc m68k_IPLchangeNtfy
	movl	_regs+fIPL, %eax
	pushl	%ebp
	movl	%esp, %ebp
	movzbl	(%eax), %eax
	cmpl	_regs+fReg_intmask, %eax
	seta	%dl
	cmpl	$7, %eax
	sete	%al
	orb	%al, %dl
	je	L331
	call	_SetExternalInterruptPending
L331:
	leave
	ret


	my_define_exported_proc FindATTel
	/*
		%eax = output
	*/

	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %ecx
	call	_local_FindATTel
	leave
	ret


	my_define_exported_proc SetHeadATTel
	pushl	%ebp
	movl	%esp, %ebp

	movl	$0, _regs+fReg_MATCrdB_cmpmask
	movl	$-1, _regs+fReg_MATCrdB_cmpvalu
	movl	$0, _regs+fReg_MATCwrB_cmpmask
	movl	$-1, _regs+fReg_MATCwrB_cmpvalu

	movl	$0, _regs+fReg_MATCrdW_cmpmask
	movl	$-1, _regs+fReg_MATCrdW_cmpvalu
	movl	$0, _regs+fReg_MATCwrW_cmpmask
	movl	$-1, _regs+fReg_MATCwrW_cmpvalu

	movl	$0, _regs+fReg_MATCex_cmpmask
	movl	$-1, _regs+fReg_MATCex_cmpvalu

	movl	8(%ebp), %eax
	movl	%eax, _regs+fHeadATTel
	leave
	ret


	my_define_exported_proc get_vm_byte
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	call	_get_byte
	movzbl	%al, %eax
	leave
	ret


	my_define_exported_proc get_vm_word
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	call	_get_word
	movzwl	%ax, %eax
	leave
	ret


	my_define_exported_proc get_vm_long
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	call	_get_long
	leave
	ret


	my_define_exported_proc put_vm_byte
	pushl	%ebp
	movl	%esp, %ebp
	movl	12(%ebp), %edx
	movsbl	%dl, %edx
	movl	8(%ebp), %eax
	call	_put_byte
	leave
	ret


	my_define_exported_proc put_vm_word
	pushl	%ebp
	movl	%esp, %ebp
	movl	12(%ebp), %edx
	movswl	%dx, %edx
	movl	8(%ebp), %eax
	call	_put_word
	leave
	ret


	my_define_exported_proc put_vm_long
	pushl	%ebp
	movl	%esp, %ebp
	movl	12(%ebp), %edx
	movl	8(%ebp), %eax
	call	_put_long
	leave
	ret


	my_define_exported_proc GetCyclesRemaining
	movl	_regs+fReg_MoreCyclesToGo, %eax
	addl	_regs+fReg_MaxCyclesToGo, %eax
	ret


	my_define_exported_proc SetCyclesRemaining
	pushl	%ebp
	movl	_regs+fReg_MaxCyclesToGo, %edx
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	cmpl	%eax, %edx
	jl	L296
	movl	$0, _regs+fReg_MoreCyclesToGo
	movl	%eax, _regs+fReg_MaxCyclesToGo
	jmp	L299
L296:
	subl	%edx, %eax
	movl	%eax, _regs+fReg_MoreCyclesToGo
L299:
	leave
	ret


	my_define_exported_proc DiskInsertedPsuedoException
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$4, %esp
	movl	8(%ebp), %eax
	movl	12(%ebp), %ebx
	call	_ExceptionTo
	movl	_regs+fReg_A7, %eax
	movl	%ebx, %edx
	subl	$4, %eax
	movl	%eax, _regs+fReg_A7
	addl	$4, %esp
	popl	%ebx
	leave
	jmp	_put_long


	my_define_exported_proc m68k_reset
	movl	$0, _regs+fReg_MoreCyclesToGo
	movl	$0, _regs+fReg_MaxCyclesToGo
	movl	$0, _regs+fReg_ResidualCycles
	movw	$0xFC4A, _regs+fReg_fakeword
	movl	$4, %eax
	call	_get_long
	call	_m68k_setpc
	xorl	%eax, %eax
	call	_get_long
	movl	%eax, _regs+fReg_A7
	movl	$7, _regs+fReg_intmask
	movb	$0, _regs+fReg_v
	movb	$0, _regs+fReg_c
	movb	$0, _regs+fReg_n
	movb	$0, _regs+fReg_z
	movb	$0, _regs+fReg_t1
	movb	$1, _regs+fReg_s
	movb	$0, _regs+fReg_TracePend
	movb	$0, _regs+fReg_ExtIntPend
	ret


	my_define_exported_proc MINEM68K_Init
	pushl	%ebp
	movl	%esp, %ebp

	subl	$4, %esp
	andl	$-16, %esp

#if 0
	movl	8(%ebp), %eax
	movl	$_regs+fReg_MATCrd_cmpmask, (%eax)
	movl	12(%ebp), %eax
	movl	$_regs+fReg_MATCwr_cmpmask, (%eax)
	movl	16(%ebp), %eax
	movl	$_regs+fReg_MATCex_cmpmask, (%eax)
	movl	20(%ebp), %eax
	movl	%eax, _regs+fIPL
#endif
	movl	8(%ebp), %eax
	movl	%eax, _regs+fIPL
	/* movl	$_regs+fdisp_table, 8(%ebp) */
	movl	$_regs+fdisp_table, (%esp)
	my_call_imported_proc M68KITAB_setup
	leave
	/* jmp	L_M68KITAB_setup$stub */
	ret


	my_define_exported_proc m68k_go_nCycles
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
		/* align stack */
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$52, %esp
	movl	8(%ebp), %eax
	addl	_regs+fReg_ResidualCycles, %eax
	jmp	m68k_go_nCycles_L0
L826:
	cmpb	$0, _regs+fReg_TracePend
	je	L827
	movl	$9, %eax
	call	_Exception
L827:
	cmpb	$0, _regs+fReg_ExtIntPend
	je	L829
	movl	_regs+fIPL, %eax
	movb	$0, _regs+fReg_ExtIntPend
	movzbl	(%eax), %ebx
	cmpl	_regs+fReg_intmask, %ebx
	seta	%dl
	cmpl	$7, %ebx
	sete	%al
	orb	%al, %dl
	je	L829
	leal	24(%ebx), %eax
	call	_Exception
	movl	%ebx, _regs+fReg_intmask
L829:
	cmpb	$0, _regs+fReg_t1
	je	L832
	movb	$1, _regs+fReg_TracePend
	call	_NeedToGetOut
L832:
	movl	_regs+fReg_pc_p, %edi
	movzwl	(%edi), %eax
	jmp	L544

L544a:
	movl	_regs+fReg_MoreCyclesToGo, %eax
	movl	$0, _regs+fReg_MoreCyclesToGo
m68k_go_nCycles_L0:
	addl	_regs+fReg_MaxCyclesToGo, %eax
	testl	%eax, %eax
	movl	%eax, _regs+fReg_MaxCyclesToGo
	jg	L826
	movl	%eax, _regs+fReg_ResidualCycles
	movl	$0, _regs+fReg_MaxCyclesToGo
	addl	$52, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret


	my_code_end
