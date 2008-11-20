# make PowerPC MPW tool for MPW 3.6b2, Universal Interfaces 3.4.1

TheDefaultOutput Ä "::::Build"

":program.o" Ä ":program.c"
	MrC ":program.c" -o ":program.o" ¶
		-proto strict -w 17 ¶
		-i ":::source:" -i "::::source:build:"

ObjFiles = ¶
	":program.o" ¶


"::::Build" Ä {ObjFiles}
	Delete -i "::::Build"
	PPCLink -t 'MPST' -c 'MPS ' ¶
		-o "::::Build" ¶
		"{PPCLibraries}PPCCRuntime.o" ¶
		"{PPCLibraries}StdCRuntime.o" ¶
		"{SharedLibraries}InterfaceLib" ¶
		"{SharedLibraries}StdCLib" ¶
		":program.o" 
	SetFile -d . -m . -a B "::::Build"

clean Ä
	Delete -i {ObjFiles}
	Delete -i "::::Build"

