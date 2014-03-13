/*
	SPWROASM.i
	Copyright (C) 2010 Paul C. Pratt

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
	program SPecific WRite Optional ASseMbly code
*/

#pragma segment ASMgen

TYPEDEFFUNC char * (*ptMacroParamToName)(uimr i);

LOCALVAR ptMacroParamToName CurMacroParamToName = nullpr;

LOCALPROC WriteBeginMacro(char *s, uimr n, ptMacroParamToName p)
{
	uimr i;

	CurMacroParamToName = p;
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	switch (cur_asm) {
		case gbk_asm_xcdppc:
		case gbk_asm_xcdx86:
			WriteBgnDestFileLn();
			WriteCStrToDestFile("\t.macro ");
			WriteCStrToDestFile(s);
			WriteEndDestFileLn();
			break;

		case gbk_asm_bgcppc:
		case gbk_asm_bgcx86:
		case gbk_asm_dvcx86:
			WriteBgnDestFileLn();
			WriteCStrToDestFile("\t.macro ");
			WriteCStrToDestFile(s);
			if (n != 0) {
				WriteCStrToDestFile(" ");
				WriteCStrToDestFile(p(0));
				for (i = 1; i < n; i++) {
					WriteCStrToDestFile(", ");
					WriteCStrToDestFile(p(i));
				}
			}
			WriteEndDestFileLn();
			break;

		case gbk_asm_mpwppc:
			++DestFileIndent;
			WriteDestFileLn("MACRO");
			WriteBgnDestFileLn();
			WriteCStrToDestFile(s);
			if (n != 0) {
				WriteCStrToDestFile("\t&");
				WriteCStrToDestFile(p(0));
				for (i = 1; i < n; i++) {
					WriteCStrToDestFile(", &");
					WriteCStrToDestFile(p(i));
				}
			}
			WriteEndDestFileLn();
			--DestFileIndent;
			break;
		default:
			break;
	}
}

LOCALPROC WriteEndMacro(void)
{
	WriteBlankLineToDestFile();

	switch (cur_asm) {
		case gbk_asm_xcdppc:
		case gbk_asm_xcdx86:
			WriteDestFileLn("\t.endmacro");
			break;

		case gbk_asm_bgcppc:
		case gbk_asm_bgcx86:
		case gbk_asm_dvcx86:
			WriteDestFileLn("\t.endm");
				/* probably depends on which version */
			break;

		case gbk_asm_mpwppc:
			WriteBlankLineToDestFile();
			++DestFileIndent;
			WriteDestFileLn("ENDM");
			--DestFileIndent;
			break;
		default:
			break;
	}

	CurMacroParamToName = nullpr;
}

LOCALPROC WriteMacroParam(uimr i)
{
	if (CurMacroParamToName != nullpr) {
		switch (cur_asm) {
			case gbk_asm_xcdppc:
			case gbk_asm_xcdx86:
				WriteCStrToDestFile("$");
				WriteUnsignedToOutput(i);
				break;

			case gbk_asm_bgcppc:
			case gbk_asm_bgcx86:
			case gbk_asm_dvcx86:
				/*
					WriteCStrToDestFile("&");
						also works, at least on powerpc,
						but "\\" seems more standard.
				*/
				WriteCStrToDestFile("\\");
				WriteCStrToDestFile(CurMacroParamToName(i));
				break;

			case gbk_asm_mpwppc:
				WriteCStrToDestFile("&");
				WriteCStrToDestFile(CurMacroParamToName(i));
				break;
			default:
				break;
		}
	}
}

LOCALPROC WriteImportsBeginMacro(void)
{
	WriteBeginMacro("my_imports_begin", 0, nullpr);

	switch (cur_asm) {
		case gbk_asm_xcdx86:
			WriteBlankLineToDestFile();
			WriteDestFileLn(
				"\t.section __IMPORT,__jump_table,symbol_stubs,"
				"self_modifying_code+pure_instructions,5");
			break;
		default:
			break;
	}

	WriteEndMacro();
}


LOCALPROC WriteImportsEndMacro(void)
{
	WriteBeginMacro("my_imports_end", 0, nullpr);

	switch (cur_asm) {
		case gbk_asm_xcdx86:
			WriteBlankLineToDestFile();
			WriteDestFileLn("\t.subsections_via_symbols");
			break;
		default:
			break;
	}

	WriteEndMacro();
}

LOCALFUNC char * MacroPListS(uimr i)
{
#pragma unused(i)

	return "s";
}

LOCALPROC WriteImportAProcMacro(void)
{
	WriteBeginMacro("my_import_proc", 1, MacroPListS);

	switch (cur_asm) {
		case gbk_asm_xcdppc:
			WriteBlankLineToDestFile();
			WriteDestFileLn(".data");
			WriteDestFileLn(".symbol_stub");

			WriteBgnDestFileLn();
			WriteCStrToDestFile("L_");
			WriteMacroParam(0);
			WriteCStrToDestFile("$stub:");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("\t.indirect_symbol _");
			WriteMacroParam(0);
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("\tlis r11,ha16(L_");
			WriteMacroParam(0);
			WriteCStrToDestFile("$lazy_ptr)");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("\tlwz r12,lo16(L_");
			WriteMacroParam(0);
			WriteCStrToDestFile("$lazy_ptr)(r11)");
			WriteEndDestFileLn();

			WriteDestFileLn("\tmtctr r12");

			WriteBgnDestFileLn();
			WriteCStrToDestFile("\taddi r11,r11,lo16(L_");
			WriteMacroParam(0);
			WriteCStrToDestFile("$lazy_ptr)");
			WriteEndDestFileLn();

			WriteDestFileLn("\tbctr");
			WriteDestFileLn(".data");
			WriteDestFileLn(".lazy_symbol_pointer");

			WriteBgnDestFileLn();
			WriteCStrToDestFile("L_");
			WriteMacroParam(0);
			WriteCStrToDestFile("$lazy_ptr:");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("\t.indirect_symbol _");
			WriteMacroParam(0);
			WriteEndDestFileLn();

			WriteDestFileLn("\t.long dyld_stub_binding_helper");
			break;
		case gbk_asm_xcdx86:
			WriteBlankLineToDestFile();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("L_");
			WriteMacroParam(0);
			WriteCStrToDestFile("$stub:");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("\t.indirect_symbol _");
			WriteMacroParam(0);
			WriteEndDestFileLn();

			WriteDestFileLn("\thlt ; hlt ; hlt ; hlt ; hlt");
			break;

		case gbk_asm_bgcppc:
		case gbk_asm_bgcx86:
		case gbk_asm_dvcx86:
			/* nothing needed */
			break;

		case gbk_asm_mpwppc:
			++DestFileIndent;
			WriteBlankLineToDestFile();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("import\t.");
			WriteMacroParam(0);
			WriteCStrToDestFile("[PR]");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("import\t");
			WriteMacroParam(0);
			WriteCStrToDestFile("[DS]");
			WriteEndDestFileLn();

			WriteDestFileLn("toc");

			WriteBgnDestFileLn();
			WriteCStrToDestFile("tc\t");
			WriteMacroParam(0);
			WriteCStrToDestFile("[TC],");
			WriteMacroParam(0);
			WriteCStrToDestFile("[DS]");
			WriteEndDestFileLn();

			--DestFileIndent;
			break;
		default:
			break;
	}

	WriteEndMacro();
}

LOCALPROC CallImportedProcMacro(void)
{
	WriteBeginMacro("my_call_imported_proc", 1, MacroPListS);

	WriteBlankLineToDestFile();
	++DestFileIndent;
	WriteBgnDestFileLn();
	if (gbo_cpufam == gbk_cpufam_ppc) {
		WriteCStrToDestFile("bl");
	} else {
		WriteCStrToDestFile("call");
	}
	WriteCStrToDestFile("\t");

	switch (cur_asm) {
		case gbk_asm_bgcppc:
			WriteMacroParam(0);
			break;
		case gbk_asm_bgcx86:
			if (gbk_ide_cyg == cur_ide) {
				WriteCStrToDestFile("_");
			}
			WriteMacroParam(0);
			break;
		case gbk_asm_dvcx86:
			WriteCStrToDestFile("_");
			WriteMacroParam(0);
			break;
		case gbk_asm_xcdppc:
		case gbk_asm_xcdx86:
			WriteCStrToDestFile("L_");
			WriteMacroParam(0);
			WriteCStrToDestFile("$stub");
			break;
		case gbk_asm_mpwppc:
			WriteCStrToDestFile(".");
			WriteMacroParam(0);
			WriteCStrToDestFile("[PR]");
			break;
		default:
			break;
	}

	WriteEndDestFileLn();
	switch (cur_asm) {
		case gbk_asm_mpwppc:
			WriteDestFileLn("Nop");
			break;
		default:
			break;
	}
	--DestFileIndent;

	WriteEndMacro();
}

LOCALPROC WriteExportAProcMacro(void)
{
	WriteBeginMacro("my_export_proc", 1, MacroPListS);

	switch (cur_asm) {
		case gbk_asm_mpwppc:
			++DestFileIndent;

			WriteBlankLineToDestFile();

			WriteDestFileLn("toc");

			WriteBgnDestFileLn();
			WriteCStrToDestFile("tc\t");
			WriteMacroParam(0);
			WriteCStrToDestFile("[TC],");
			WriteMacroParam(0);
			WriteCStrToDestFile("[DS]");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("csect\t");
			WriteMacroParam(0);
			WriteCStrToDestFile("[DS]");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("dc.l\t.");
			WriteMacroParam(0);
			WriteEndDestFileLn();

			WriteDestFileLn("dc.l\tTOC[tc0]");

			--DestFileIndent;

			break;
		case gbk_asm_xcdppc:
		case gbk_asm_xcdx86:
		case gbk_asm_dvcx86:
			WriteBlankLineToDestFile();
			WriteBgnDestFileLn();
			WriteCStrToDestFile("\t.globl ");
			WriteCStrToDestFile("_");
			WriteMacroParam(0);
			WriteEndDestFileLn();
			break;

		case gbk_asm_bgcppc:
		case gbk_asm_bgcx86:
			WriteBlankLineToDestFile();
			WriteBgnDestFileLn();
			WriteCStrToDestFile("\t.globl ");
			if (gbk_ide_cyg == cur_ide) {
				WriteCStrToDestFile("_");
			}
			WriteMacroParam(0);
			WriteEndDestFileLn();
			break;

		default:
			break;
	}

	WriteEndMacro();
}

LOCALPROC WriteAsmAlignPow2(uimr i)
{
	WriteBgnDestFileLn();

	switch (cur_asm) {
		case gbk_asm_xcdppc:
		case gbk_asm_xcdx86:
			WriteCStrToDestFile("\t.align ");
			WriteUnsignedToOutput(i);
			break;

		case gbk_asm_bgcppc:
		case gbk_asm_bgcx86:
		case gbk_asm_dvcx86:
			WriteCStrToDestFile("\t.balign ");
			WriteUnsignedToOutput(1 << i);
			break;

		default:
			break;
	}

	WriteEndDestFileLn();
}

LOCALPROC WriteAGlobalLabelMacro(void)
{
	WriteBeginMacro("my_define_exported_proc", 1, MacroPListS);

	if (gbk_asm_mpwppc == cur_asm) {
		++DestFileIndent;

		WriteBlankLineToDestFile();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("export\t.");
		WriteMacroParam(0);
		WriteCStrToDestFile("[PR]");
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("export\t");
		WriteMacroParam(0);
		WriteCStrToDestFile("[DS]");
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("csect\t.");
		WriteMacroParam(0);
		WriteCStrToDestFile("[PR]");
#if 0
		if (5 == align) {
			WriteCStrToDestFile(", 5");
		}
#endif
		WriteEndDestFileLn();

		--DestFileIndent;

		WriteBgnDestFileLn();
		WriteCStrToDestFile(".");
		WriteMacroParam(0);
		WriteCStrToDestFile(":");
		WriteEndDestFileLn();
	} else {
		WriteBlankLineToDestFile();

		if (gbo_cpufam == gbk_cpufam_ppc) {
#if 0
			if (5 == align) {
				WriteAsmAlignPow2(5);
			} else
#endif
			{
				WriteAsmAlignPow2(2);
			}
		}

		WriteBgnDestFileLn();
		if (gbk_ide_cyg == cur_ide) {
			WriteCStrToDestFile("_");
		} else {
			switch (cur_asm) {
				case gbk_asm_xcdppc:
				case gbk_asm_xcdx86:
				case gbk_asm_dvcx86:
					WriteCStrToDestFile("_");
					break;
				default:
					break;
			}
		}
		WriteMacroParam(0);
		WriteCStrToDestFile(":");
		WriteEndDestFileLn();
	}

	WriteEndMacro();
}

LOCALFUNC char * MacroPListRegV(uimr i)
{
	char *s;

	switch (i) {
		case 0:
			s = "reg";
			break;
		case 1:
		default:
			s = "v";
			break;
	}

	return s;
}

LOCALPROC WriteLoadVarAddrMacro(void)
{
	if (gbo_cpufam == gbk_cpufam_ppc) {
		WriteBeginMacro("my_load_loc_var_addr", 2, MacroPListRegV);

		++DestFileIndent;
		if (gbk_asm_mpwppc == cur_asm) {

			WriteBlankLineToDestFile();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("lwz\t");
			WriteMacroParam(0);
			WriteCStrToDestFile(", .");
			WriteMacroParam(1);
			WriteCStrToDestFile("[TC](rtoc)");
			WriteEndDestFileLn();

		} else {
			WriteBlankLineToDestFile();
			WriteBgnDestFileLn();
			WriteCStrToDestFile("lis");
			WriteCStrToDestFile("\t");
			WriteMacroParam(0);
			WriteCStrToDestFile(", ");
			switch (cur_asm) {
				case gbk_asm_bgcppc:
				case gbk_asm_bgcx86:
					WriteCStrToDestFile(".");
					WriteMacroParam(1);
					WriteCStrToDestFile("@ha");
					break;
				default:
					WriteCStrToDestFile("ha16(.");
					WriteMacroParam(1);
					WriteCStrToDestFile(")");
					break;
			}
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("la");
			WriteCStrToDestFile("\t");
			WriteMacroParam(0);
			WriteCStrToDestFile(", ");
			switch (cur_asm) {
				case gbk_asm_bgcppc:
				case gbk_asm_bgcx86:
					WriteCStrToDestFile(".");
					WriteMacroParam(1);
					WriteCStrToDestFile("@l");
					break;
				default:
					WriteCStrToDestFile("lo16(.");
					WriteMacroParam(1);
					WriteCStrToDestFile(")");
					break;
			}
			WriteCStrToDestFile("(");
			WriteMacroParam(0);
			WriteCStrToDestFile(")");
			WriteEndDestFileLn();
		}

		--DestFileIndent;

		WriteEndMacro();
	}
}

LOCALPROC WriteALocalLabel(uimr i)
{
	WriteBgnDestFileLn();
	if (gbo_cpufam == gbk_cpufam_ppc) {
		WriteCStrToDestFile(".");
	}
	WriteMacroParam(i);
	WriteCStrToDestFile(":");
	WriteEndDestFileLn();
}

LOCALPROC WriteDsptchTabBeginMacro(void)
{
	WriteBeginMacro("my_dispatch_table_begin", 1, MacroPListS);

	if (gbk_asm_mpwppc == cur_asm) {
		++DestFileIndent;

		WriteBlankLineToDestFile();

		WriteDestFileLn("toc");
		WriteDestFileLn("tc\t.&s[TC],.&s[RW]");
		WriteDestFileLn("csect\t.&s[RW], 3");

		--DestFileIndent;

	} else {
		WriteBlankLineToDestFile();
		switch (cur_asm) {
			case gbk_asm_xcdppc:
			case gbk_asm_xcdx86:
				WriteDestFileLn("\t.const");
				break;
			default:
				WriteDestFileLn("\t.section\t.rodata");
				break;
		}
		if (gbo_cpufam == gbk_cpufam_ppc) {
			WriteAsmAlignPow2(2);
		} else {
			WriteAsmAlignPow2(6);
		}
		WriteALocalLabel(0);
	}

	WriteEndMacro();
}

LOCALPROC WriteDsptchTabEndMacro(void)
{
	WriteBeginMacro("my_dispatch_table_end", 1, MacroPListS);
	WriteEndMacro();
}

LOCALPROC DefineDispatchFieldMacro(void)
{
	WriteBeginMacro("my_dispatch_entry", 1, MacroPListS);

	if (gbk_asm_mpwppc == cur_asm) {
		++DestFileIndent;

		WriteBlankLineToDestFile();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("dc.l\t");
		WriteMacroParam(0);
		WriteEndDestFileLn();

		--DestFileIndent;

	} else {
		WriteBlankLineToDestFile();
		++DestFileIndent;
		WriteBgnDestFileLn();
		WriteCStrToDestFile(".long\t");
		WriteMacroParam(0);
		WriteEndDestFileLn();
		--DestFileIndent;
	}

	WriteEndMacro();
}

LOCALFUNC char * MacroPListRegSD(uimr i)
{
	char *s;

	switch (i) {
		case 0:
			s = "s";
			break;
		case 1:
		default:
			s = "d";
			break;
	}

	return s;
}

LOCALPROC DeclareFieldOffsetMacro(void)
{
	if (gbo_cpufam == gbk_cpufam_ppc) {
		WriteBeginMacro("my_dispatch_offset", 2, MacroPListRegSD);

		if (gbk_asm_mpwppc == cur_asm) {
			WriteBlankLineToDestFile();

			WriteBgnDestFileLn();
			WriteMacroParam(0);
			WriteCStrToDestFile(": equ $-.");
			WriteMacroParam(1);
			WriteCStrToDestFile("[RW]");
			WriteEndDestFileLn();
		} else {
			WriteBlankLineToDestFile();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("\t.set ");
			WriteMacroParam(0);
			WriteCStrToDestFile(", .-.");
			WriteMacroParam(1);
			WriteEndDestFileLn();

		}

		WriteEndMacro();
	}
}

LOCALPROC WriteCodeBeginMacro(void)
{
	WriteBeginMacro("my_code_begin", 0, nullpr);

	if (gbk_asm_mpwppc == cur_asm) {
		++DestFileIndent;

		WriteBlankLineToDestFile();

		WriteDestFileLn("csect .BeginLocalCode[PR], 5");

		--DestFileIndent;
	} else {
		WriteBlankLineToDestFile();

		WriteDestFileLn("\t.text");
		WriteBlankLineToDestFile();
		if (gbo_cpufam == gbk_cpufam_ppc) {
			WriteAsmAlignPow2(5);
		} else {
			WriteAsmAlignPow2(6);
		}
	}

	WriteEndMacro();
}

LOCALPROC WriteCodeEndMacro(void)
{
	WriteBeginMacro("my_code_end", 0, nullpr);
	WriteEndMacro();
}

LOCALPROC WriteNormalAsmComment(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ");
#if 0
	switch (cur_asm) {
		case gbk_asm_bgcppc:
		case gbk_asm_bgcx86:
		case gbk_asm_dvcx86:
			WriteCStrToDestFile("# ");
			break;

		default:
			WriteCStrToDestFile("; ");
			break;
	}
#endif
	WriteCStrToDestFile(s);
	WriteEndDestFileLn();
}

LOCALPROC WriteAsmIndentComment(char *s)
{
	++DestFileIndent;
	++DestFileIndent;
	WriteNormalAsmComment(s);
	--DestFileIndent;
	--DestFileIndent;
}

LOCALPROC WriteAsmInstrComment(char *s)
{
	++DestFileIndent;
	WriteNormalAsmComment(s);
	--DestFileIndent;
}

LOCALPROC WriteRegEquate(char *newname, char *oldname)
{
	switch (cur_asm) {
		case gbk_asm_mpwppc:
			WriteBgnDestFileLn();
			WriteCStrToDestFile(newname);
			WriteCStrToDestFile("\tEQU\t");
			WriteCStrToDestFile(oldname);
			WriteEndDestFileLn();
			break;
		case gbk_asm_bgcppc:
		case gbk_asm_bgcx86:
		case gbk_asm_dvcx86:
			WriteBgnDestFileLn();
			WriteCStrToDestFile(newname);
			WriteCStrToDestFile(" = ");
			WriteCStrToDestFile(oldname);
			WriteEndDestFileLn();
			break;
		case gbk_asm_xcdppc:
		case gbk_asm_xcdx86:
			WriteBgnDestFileLn();
			WriteCStrToDestFile("#define ");
			WriteCStrToDestFile(newname);
			WriteCStrToDestFile(" ");
			WriteCStrToDestFile(oldname);
			WriteEndDestFileLn();
			break;


		default:
			break;
	}
}

LOCALVAR int FieldOffsetCounter;

LOCALPROC WriteBeginLocalData(void)
{
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteNormalAsmComment("---- local data -----------");
	WriteBlankLineToDestFile();
	if (gbk_asm_mpwppc == cur_asm) {
		++DestFileIndent;
		WriteDestFileLn("toc");
		WriteDestFileLn("tc\t.b2regs9999[TC],.b2regs9999[RW]");
		WriteDestFileLn("csect\t.b2regs9999[RW], 3");
		--DestFileIndent;
	} else {
		FieldOffsetCounter = 0;
#if 0
		WriteDestFileLn(".data");
		WriteDestFileLn("\t.align 2");
		WriteALocalLabel("b2regs9999");
#endif
	}
	WriteBlankLineToDestFile();
}

LOCALPROC WriteEndLocalData(void)
{
	switch (cur_asm) {
		case gbk_asm_xcdppc:
		case gbk_asm_xcdx86:
			WriteBlankLineToDestFile();
			WriteBgnDestFileLn();
			if (gbo_cpufam == gbk_cpufam_ppc) {
				WriteCStrToDestFile("\t.lcomm\t.b2regs9999, ");
			} else {
				WriteCStrToDestFile("\t.lcomm\t_regs, ");
			}
			WriteUnsignedToOutput(FieldOffsetCounter);
			if (gbo_cpufam == gbk_cpufam_ppc) {
				WriteCStrToDestFile(", 4");
			} else {
				WriteCStrToDestFile(", 6");
			}
			WriteEndDestFileLn();
			break;

		case gbk_asm_bgcppc:
		case gbk_asm_bgcx86:
		case gbk_asm_dvcx86:
			WriteBlankLineToDestFile();
			switch (cur_asm) {
				case gbk_asm_bgcppc:
					WriteDestFileLn("\t.local");
					break;

				default:
					WriteDestFileLn("\t.section .bss");
					break;
			}
			if (gbo_cpufam == gbk_cpufam_ppc) {
				WriteAsmAlignPow2(4);
			} else {
				WriteAsmAlignPow2(6);
			}
			WriteBgnDestFileLn();
			if (gbo_cpufam == gbk_cpufam_ppc) {
				WriteCStrToDestFile("\t.comm\t.b2regs9999, ");
			} else {
				WriteCStrToDestFile("\t.comm\t_regs, ");
			}
			WriteUnsignedToOutput(FieldOffsetCounter);
			WriteEndDestFileLn();
			break;

		default:
			break;
	}
}

LOCALPROC DeclareFieldLong(char *s, char *record)
{
	if (gbk_asm_mpwppc == cur_asm) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(": equ $-.");
		WriteCStrToDestFile(record);
		WriteCStrToDestFile("[RW]");
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("\tds.l\t1");
		WriteEndDestFileLn();
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("\t.set ");
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(", ");
		WriteUnsignedToOutput(FieldOffsetCounter);
		FieldOffsetCounter += 4;
#if 0
		WriteCStrToDestFile(".-.");
		WriteCStrToDestFile(record);
#endif
		WriteEndDestFileLn();

#if 0
		WriteBgnDestFileLn();
		WriteCStrToDestFile("\t.long 0");
		WriteEndDestFileLn();
#endif
	}
}

LOCALPROC DeclareFieldWord(char *s, char *record)
{
	if (gbk_asm_mpwppc == cur_asm) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(": equ $-.");
		WriteCStrToDestFile(record);
		WriteCStrToDestFile("[RW]");
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("\tds.w\t1");
		WriteEndDestFileLn();
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("\t.set ");
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(", ");
		WriteUnsignedToOutput(FieldOffsetCounter);
		FieldOffsetCounter += 2;
#if 0
		WriteCStrToDestFile(".-.");
		WriteCStrToDestFile(record);
#endif
		WriteEndDestFileLn();

#if 0
		WriteBgnDestFileLn();
		WriteCStrToDestFile("\t.word 0");
		WriteEndDestFileLn();
#endif
	}
}

LOCALPROC DeclareFieldByte(char *s, char *record)
{
	if (gbk_asm_mpwppc == cur_asm) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(": equ $-.");
		WriteCStrToDestFile(record);
		WriteCStrToDestFile("[RW]");
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("\tds.b\t1");
		WriteEndDestFileLn();
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("\t.set ");
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(", ");
		WriteUnsignedToOutput(FieldOffsetCounter);
		if (gbo_cpufam == gbk_cpufam_ppc) {
			FieldOffsetCounter += 2;
		} else {
			FieldOffsetCounter += 1;
		}
#if 0
		WriteCStrToDestFile(".-.");
		WriteCStrToDestFile(record);
#endif
		WriteEndDestFileLn();

#if 0
		WriteBgnDestFileLn();
		WriteCStrToDestFile("\t.byte 0");
		WriteEndDestFileLn();
#endif
	}
}

LOCALPROC DeclareFieldOfSize(char *s, char *record, uimr n)
{
	if (gbk_asm_mpwppc == cur_asm) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(": equ $-.");
		WriteCStrToDestFile(record);
		WriteCStrToDestFile("[RW]");
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("\tds.b\t");
		WriteUnsignedToOutput(n);
		WriteEndDestFileLn();
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("\t.set ");
		WriteCStrToDestFile(s);
		WriteCStrToDestFile(", ");
		WriteUnsignedToOutput(FieldOffsetCounter);
		FieldOffsetCounter += n;
#if 0
		WriteCStrToDestFile(".-.");
		WriteCStrToDestFile(record);
#endif
		WriteEndDestFileLn();

#if 0
		WriteBgnDestFileLn();
		WriteCStrToDestFile("\t.long 0");
		WriteEndDestFileLn();
#endif
	}
}

#pragma segment ASMgen0

LOCALPROC WriteAsmRegisterUse(void)
{
	if (gbo_cpufam == gbk_cpufam_ppc) {
		WriteBlankLineToDestFile();
		WriteBlankLineToDestFile();
		WriteNormalAsmComment("---- register usage -----------");

		switch (cur_asm) {
			case gbk_asm_bgcppc:
				{
					int i;

					WriteBlankLineToDestFile();
					for (i = 0; i < 32; i++) {
						WriteBgnDestFileLn();
						WriteCStrToDestFile("r");
						WriteUnsignedToOutput(i);
						WriteCStrToDestFile(" = ");
						WriteUnsignedToOutput(i);
						WriteEndDestFileLn();
					}
				}
				break;
			default:
				break;
		}

		WriteBlankLineToDestFile();
		if (gbk_asm_mpwppc == cur_asm) {
			WriteNormalAsmComment("r1 - stack ptr");
		} else {
			WriteRegEquate("sp", "r1");
		}
		WriteNormalAsmComment("r2 - toc");
		WriteNormalAsmComment("r3 - return address");
		WriteNormalAsmComment("r4:r10 - parameter passing");
		WriteNormalAsmComment("r11:r12 - temp");
		WriteBlankLineToDestFile();
		WriteRegEquate("rPC_p", "r13");
		WriteRegEquate("rOpCode", "r14");
		WriteRegEquate("rOpSize", "r15");
		WriteRegEquate("rDecOpA", "r16");
		WriteRegEquate("rDecOpB", "r17");
		WriteRegEquate("rMaxCyclesToGo", "r18");
		WriteRegEquate("rRegs", "r19");
		WriteRegEquate("rFlag_c", "r20");
		WriteRegEquate("rFlag_z", "r21");
		WriteRegEquate("rFlag_n", "r22");
		WriteRegEquate("rFlag_v", "r23");
		WriteRegEquate("rFlag_x", "r24");
		WriteRegEquate("rDispatches", "r25");
		WriteRegEquate("rArgKind", "r26");
		WriteRegEquate("rArgAddr", "r27");
	}
}

LOCALPROC WriteAsmLocalData(void)
{
	WriteBeginLocalData();
	DeclareFieldLong("fReg_D0", "b2regs9999");
	DeclareFieldLong("fReg_D1", "b2regs9999");
	DeclareFieldLong("fReg_D2", "b2regs9999");
	DeclareFieldLong("fReg_D3", "b2regs9999");
	DeclareFieldLong("fReg_D4", "b2regs9999");
	DeclareFieldLong("fReg_D5", "b2regs9999");
	DeclareFieldLong("fReg_D6", "b2regs9999");
	DeclareFieldLong("fReg_D7", "b2regs9999");
	DeclareFieldLong("fReg_A0", "b2regs9999");
	DeclareFieldLong("fReg_A1", "b2regs9999");
	DeclareFieldLong("fReg_A2", "b2regs9999");
	DeclareFieldLong("fReg_A3", "b2regs9999");
	DeclareFieldLong("fReg_A4", "b2regs9999");
	DeclareFieldLong("fReg_A5", "b2regs9999");
	DeclareFieldLong("fReg_A6", "b2regs9999");
	DeclareFieldLong("fReg_A7", "b2regs9999");
	if (gbo_cpufam == gbk_cpufam_ppc) {
		DeclareFieldLong("fReg_usp", "b2regs9999");
		DeclareFieldLong("fReg_isp", "b2regs9999");
		DeclareFieldLong("fReg_pc", "b2regs9999");
		DeclareFieldLong("fReg_intmask", "b2regs9999");
		DeclareFieldByte("fReg_t1", "b2regs9999");
		DeclareFieldByte("fReg_s", "b2regs9999");
		DeclareFieldByte("fnot_Reg_m", "b2regs9999");
		WriteAsmInstrComment("not on 68000");
		DeclareFieldByte("fReg_Pad0", "b2regs9999");
		DeclareFieldByte("fReg_TracePend", "b2regs9999");
		DeclareFieldByte("fReg_ExtIntPend", "b2regs9999");
		DeclareFieldByte("fReg_c", "b2regs9999");
		DeclareFieldByte("fReg_z", "b2regs9999");
		DeclareFieldByte("fReg_n", "b2regs9999");
		DeclareFieldByte("fReg_v", "b2regs9999");
		DeclareFieldByte("fReg_x", "b2regs9999");
		DeclareFieldByte("fReg_Pad1", "b2regs9999");
		DeclareFieldLong("fReg_pc_p", "b2regs9999");
		DeclareFieldLong("fReg_pc_oldp", "b2regs9999");
		DeclareFieldLong("fReg_MaxCyclesToGo", "b2regs9999");
		DeclareFieldLong("fReg_MoreCyclesToGo", "b2regs9999");
		DeclareFieldLong("fReg_RgLstTmp", "b2regs9999");
		DeclareFieldLong("fIPL", "b2regs9999");
	} else {
		DeclareFieldLong("fReg_pc_p", "b2regs9999");
		DeclareFieldLong("fReg_MaxCyclesToGo", "b2regs9999");
		DeclareFieldLong("fReg_pc", "b2regs9999");
		DeclareFieldLong("fReg_oldp", "b2regs9999");
		DeclareFieldByte("fReg_v", "b2regs9999");
		DeclareFieldByte("fReg_c", "b2regs9999");
		DeclareFieldByte("fReg_n", "b2regs9999");
		DeclareFieldByte("fReg_z", "b2regs9999");
		DeclareFieldByte("fReg_x", "b2regs9999");
		DeclareFieldByte("fReg_t1", "b2regs9999");
		DeclareFieldByte("fReg_s", "b2regs9999");
		DeclareFieldByte("fReg_TracePend", "b2regs9999");
		DeclareFieldByte("fReg_ExtIntPend", "b2regs9999");
		DeclareFieldByte("fReg_Pad0", "b2regs9999");
		DeclareFieldByte("fReg_Pad1", "b2regs9999");
		DeclareFieldByte("fReg_Pad2", "b2regs9999");
		DeclareFieldLong("fIPL", "b2regs9999");
		DeclareFieldLong("fReg_intmask", "b2regs9999");
		DeclareFieldLong("fReg_usp", "b2regs9999");
		DeclareFieldLong("fReg_isp", "b2regs9999");
		DeclareFieldLong("fReg_MoreCyclesToGo", "b2regs9999");
		DeclareFieldLong("fReg_opsize", "b2regs9999");
		DeclareFieldLong("fReg_ArgKind", "b2regs9999");
		DeclareFieldLong("fReg_ArgAddr", "b2regs9999");
		DeclareFieldLong("fReg_opcode", "b2regs9999");
	}
	DeclareFieldLong("fReg_MATCrdB_cmpmask", "b2regs9999");
	DeclareFieldLong("fReg_MATCrdB_cmpvalu", "b2regs9999");
	DeclareFieldLong("fReg_MATCrdB_usemask", "b2regs9999");
	DeclareFieldLong("fReg_MATCrdB_usebase", "b2regs9999");
	DeclareFieldLong("fReg_MATCwrB_cmpmask", "b2regs9999");
	DeclareFieldLong("fReg_MATCwrB_cmpvalu", "b2regs9999");
	DeclareFieldLong("fReg_MATCwrB_usemask", "b2regs9999");
	DeclareFieldLong("fReg_MATCwrB_usebase", "b2regs9999");

	DeclareFieldLong("fReg_MATCrdW_cmpmask", "b2regs9999");
	DeclareFieldLong("fReg_MATCrdW_cmpvalu", "b2regs9999");
	DeclareFieldLong("fReg_MATCrdW_usemask", "b2regs9999");
	DeclareFieldLong("fReg_MATCrdW_usebase", "b2regs9999");
	DeclareFieldLong("fReg_MATCwrW_cmpmask", "b2regs9999");
	DeclareFieldLong("fReg_MATCwrW_cmpvalu", "b2regs9999");
	DeclareFieldLong("fReg_MATCwrW_usemask", "b2regs9999");
	DeclareFieldLong("fReg_MATCwrW_usebase", "b2regs9999");

	DeclareFieldLong("fReg_MATCex_cmpmask", "b2regs9999");
	DeclareFieldLong("fReg_MATCex_cmpvalu", "b2regs9999");
	DeclareFieldLong("fReg_MATCex_usemask", "b2regs9999");
	DeclareFieldLong("fReg_MATCex_usebase", "b2regs9999");
	DeclareFieldLong("fHeadATTel", "b2regs9999");
	DeclareFieldLong("fReg_ResidualCycles", "b2regs9999");
	if (gbo_cpufam == gbk_cpufam_ppc) {
		DeclareFieldWord("fReg_fakeword", "b2regs9999");
		DeclareFieldOfSize("fReg_Pad3", "b2regs9999", 38);
	} else {
		DeclareFieldLong("fDecOpA", "b2regs9999");
		DeclareFieldLong("fDecOpB", "b2regs9999");
		DeclareFieldWord("fReg_fakeword", "b2regs9999");
		DeclareFieldOfSize("fReg_Pad3", "b2regs9999", 30);
	}
	DeclareFieldOfSize("fdisp_table", "b2regs9999", 65536 * 8);
	WriteEndLocalData();
}

LOCALPROC WriteAppCNFGRASMContents(void)
{
	WriteImportsBeginMacro();
	WriteImportsEndMacro();
	WriteImportAProcMacro();
	CallImportedProcMacro();
	WriteExportAProcMacro();
	WriteAGlobalLabelMacro();
	WriteLoadVarAddrMacro();
	WriteDsptchTabBeginMacro();
	WriteDsptchTabEndMacro();
	DefineDispatchFieldMacro();
	DeclareFieldOffsetMacro();
	WriteCodeBeginMacro();
	WriteCodeEndMacro();

	WriteAsmRegisterUse();
	WriteAsmLocalData();
}

#if AsmSupported
LOCALFUNC tMyErr WriteAppCNFGRASM(void)
{
	tMyErr err;

	if (! HaveAsm) {
		err = noErr;
	} else {
		err = WriteADestFile(&SrcDirR, "CNFGRASM", ".i",
			WriteAppCNFGRASMContents);
	}

	return err;
}
#endif
