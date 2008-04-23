/*
	SPOTHRCF.i
	Copyright (C) 2008 Paul C. Pratt

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
	program SPecific write OTHer ConFiguration files
*/

LOCALPROC WriteAppSpecificConfigFiles(void)
{
	if (WriteOpenDestFile(&SrcDirR, "EMCONFIG", ".h")) {

	if (cur_mdl <= gbk_mdl_Plus) {
		WriteDestFileLn("#define EmClassicKbrd 1");
	} else {
		WriteDestFileLn("#define EmClassicKbrd 0");
	}

	if (cur_mdl <= gbk_mdl_Plus) {
		WriteDestFileLn("#define EmADB 0");
		WriteDestFileLn("#define EmRTC 1");
		WriteDestFileLn("#define EmPMU 0");
	} else if ((cur_mdl <= gbk_mdl_Classic) || (cur_mdl == gbk_mdl_II)) {
		WriteDestFileLn("#define EmADB 1");
		WriteDestFileLn("#define EmRTC 1");
		WriteDestFileLn("#define EmPMU 0");
	} else {
		WriteDestFileLn("#define EmADB 0");
		WriteDestFileLn("#define EmRTC 0");
		WriteDestFileLn("#define EmPMU 1");
	}

	if (cur_mdl == gbk_mdl_II) {
		WriteDestFileLn("#define EmVIA2 1");
	} else {
		WriteDestFileLn("#define EmVIA2 0");
	}
	if (em_cpu_vers >= 2) {
		WriteDestFileLn("#define Use68020 1");
	}
	WriteDestFileLn("#define EmMMU 0");
	if (cur_mdl == gbk_mdl_II) {
		WriteDestFileLn("#define EmFPU 1");
	} else {
		WriteDestFileLn("#define EmFPU 0");
	}

	WriteCloseDestFile();
	}
}
