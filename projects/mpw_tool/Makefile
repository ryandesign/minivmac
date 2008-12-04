# make 68K MPW tool for MPW 3.6b2, Universal Interfaces 3.4.1

TheDefaultOutput Ä ":Build"

":bld:program.o" Ä ":src:program.c"
	sc ":src:program.c" -o ":bld:program.o" ¶
		-proto strict -w 17 -model far ¶
		-i ":::source:build:c_src:" -i ":::source:app:build:"

ObjFiles = ¶
	":bld:program.o" ¶


":Build" Ä {ObjFiles}
	Delete -i ":Build"
	Link -t 'MPST' -c 'MPS ' -model far ¶
		-o ":Build" ¶
		"{CLibraries}StdCLib.o" ¶
		"{Libraries}Interface.o" ¶
		"{Libraries}IntEnv.o" ¶
		"{Libraries}MacRuntime.o" ¶
		-sn STDIO=Main ¶
		-sn INTENV=Main ¶
		-sn SADEV=Main ¶
		-sn %A5Init=Main ¶
		{ObjFiles}
	SetFile -d . -m . -a B ":Build"

clean Ä
	Delete -i {ObjFiles}
	Delete -i ":Build"

