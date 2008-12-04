# make 68K application with MPW 3.6b2, Universal Interfaces 3.4.1

TheDefaultOutput Ä ":Build"

":bld:program.o" Ä ":src:program.c"
	sc ":src:program.c" -o ":bld:program.o" ¶
		-proto strict  -w 17  -model far ¶
		-i ":::source:build:c_src:" -i ":::source:app:build:"

":bld:opt.o" Ä ":::source:build:a_src:opt.a"
	asm -case on ":::source:build:a_src:opt.a" -o ":bld:opt.o"

":bld:main.rsrc" Ä ":src:main.r"
	Rez -t rsrc -c RSED -i "{RIncludes}" ":src:main.r" -o ":bld:main.rsrc"

ObjFiles = ¶
	":bld:program.o" ¶
	":bld:opt.o" ¶


":Build" Ä {ObjFiles} ":bld:main.rsrc"
	Duplicate -y ":bld:main.rsrc" ":Build"
	Link -t APPL -c GrBl -model far ¶
		-o ":Build" ¶
		"{Libraries}Interface.o" ¶
		"{Libraries}MacRuntime.o" ¶
		{ObjFiles}
	SetFile -d . -m . -a B ":Build"

clean Ä
	Delete -i {ObjFiles}
	Delete -i ":Build"
	Delete -i ":bld:main.rsrc"
