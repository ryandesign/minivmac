/*
	CMDARGT2.i
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
	CoMmandD ARGument Tool part 2
*/


int main(int argc, char *argv[])
{
	tMyErr err;
	int return_code = 1;

	if (noErr == (err = MyMemory_Init_v2())) {
		BeginParseCommandLineArguments(argc, argv);

		err = DoTheCommand();

		if (! ParseArgsFailed) {
			return_code = 0;
		}
	}
	MyMemory_UnInit();

	ToolReportAnySavedError_v2(err, argc, argv);

	return return_code;
}
