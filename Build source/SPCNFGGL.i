/*
	SPCNFGGL.i
	Copyright (C) 2007 Paul C. Pratt

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
	program SPecific CoNFiGuration GLobals
*/

LOCALPROC WriteAppSpecificCNFGGLOBoptions(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define CurEmu kEmu");
	WriteCStrToDestFile(GetModelName(cur_mdl));
	WriteEndDestFileLn();

	if (em_cpu_vers >= 2) {
		WriteDestFileLn("#define Use68020 1");
	}

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define NumDrives ");
	WriteUnsignedToOutput(cur_numdrives);
	WriteEndDestFileLn();

	if (WantInitFullScreen) {
		WriteDestFileLn("#define WantInitFullScreen 1");
	}

	if (WantInitMagnify) {
		WriteDestFileLn("#define WantInitMagnify 1");
	}

	if (WantInitBackground) {
		WriteDestFileLn("#define WantInitRunInBackground 1");
	}

	if (CurInitSpeed != gbk_speed_8X) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define WantInitSpeedValue ");
		if (gbk_speed_AllOut == CurInitSpeed) {
			WriteCStrToDestFile("-1");
		} else {
			WriteUnsignedToOutput(CurInitSpeed - 1);
		}
		WriteEndDestFileLn();
	}

	if (WantCmndOptSwap) {
		WriteDestFileLn("#define SwapCommandControl 1");
	}

	if (WantMinExtn) {
		WriteDestFileLn("#define IncludePbufs 0");
		WriteDestFileLn("#define IncludeSonyRawMode 0");
		WriteDestFileLn("#define IncludeSonyGetName 0");
		WriteDestFileLn("#define IncludeSonyNew 0");
		WriteDestFileLn("#define IncludeSonyNameNew 0");
		WriteDestFileLn("#define IncludeHostTextClipExchange 0");
	}
}
