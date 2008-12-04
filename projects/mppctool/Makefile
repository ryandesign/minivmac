# make PowerPC MPW tool for MPW 3.6b2, Universal Interfaces 3.4.1

TheDefaultOutput Ä ":Build"

":bld:program.o" Ä ":src:program.c"
	MrC ":src:program.c" -o ":bld:program.o" ¶
		-proto strict -w 17 ¶
		-i ":::source:build:c_src:" -i ":::source:app:build:"

ObjFiles = ¶
	":bld:program.o" ¶


":Build" Ä {ObjFiles}
	Delete -i ":Build"
	PPCLink -t 'MPST' -c 'MPS ' ¶
		-o ":Build" ¶
		"{PPCLibraries}PPCCRuntime.o" ¶
		"{PPCLibraries}StdCRuntime.o" ¶
		"{SharedLibraries}InterfaceLib" ¶
		"{SharedLibraries}StdCLib" ¶
		{ObjFiles}
	SetFile -d . -m . -a B ":Build"

clean Ä
	Delete -i {ObjFiles}
	Delete -i ":Build"
