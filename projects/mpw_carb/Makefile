# make PowerPC application with MPW 3.6b2, Universal Interfaces 3.4.1

TheDefaultOutput Ä ":Build"

":bld:program.o" Ä ":src:program.c"
	MrC ":src:program.c" -o ":bld:program.o" ¶
		-proto strict -w 17 ¶
		-i ":::source:build:c_src:" -i ":::source:app:build:"

":bld:main.rsrc" Ä ":src:main.r"
	Rez -t rsrc -c RSED -i "{RIncludes}" ":src:main.r" -o ":bld:main.rsrc"

ObjFiles = ¶
	":bld:program.o" ¶


":Build" Ä {ObjFiles} ":bld:main.rsrc"
	Duplicate -y ":bld:main.rsrc" ":Build"
	PPCLink -m main -t APPL -c GrBl ¶
		-o ":Build" ¶
		"{SharedLibraries}"CarbonLib ¶
		"{PPCLibraries}PPCToolLibs.o" ¶
		"{PPCLibraries}PPCCRuntime.o" ¶
		"{SharedLibraries}StdCLib" ¶
		{ObjFiles}
	SetFile -d . -m . -a B ":Build"

clean Ä
	Delete -i {ObjFiles}
	Delete -i ":Build"
	Delete -i ":bld:main.rsrc"
