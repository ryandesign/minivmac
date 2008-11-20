# make 68K MPW tool for MPW 3.6b2, Universal Interfaces 3.4.1

TheDefaultOutput Ä "::::Build"

":program.o" Ä ":program.c"
	sc ":program.c" -o ":program.o" ¶
		-proto strict -w 17 -model far ¶
		-i ":::source:" -i "::::source:build:"

ObjFiles = ¶
	":program.o" ¶


"::::Build" Ä {ObjFiles}
	Delete -i "::::Build"
	Link -t 'MPST' -c 'MPS ' -model far ¶
		-o "::::Build" ¶
		"{CLibraries}StdCLib.o" ¶
		"{Libraries}Interface.o" ¶
		"{Libraries}IntEnv.o" ¶
		"{Libraries}MacRuntime.o" ¶
		-sn STDIO=Main ¶
		-sn INTENV=Main ¶
		-sn SADEV=Main ¶
		-sn %A5Init=Main ¶
		{ObjFiles}
	SetFile -d . -m . -a B "::::Build"

clean Ä
	Delete -i {ObjFiles}
	Delete -i "::::Build"

