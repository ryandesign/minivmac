# make 68K application with MPW 3.6b2, Universal Interfaces 3.4.1

TheDefaultOutput Ä "::::Build"

":program.o" Ä ":program.c"
	sc ":program.c" -o ":program.o" ¶
		-proto strict  -w 17  -model far ¶
		-i ":::source:" -i "::::source:build:"

":opt.o" Ä ":::source:opt.a"
	asm -case on ":::source:opt.a" -o ":opt.o"

":main.rsrc" Ä ":main.r"
	Rez -t rsrc -c RSED -i "{RIncludes}" ":main.r" -o ":main.rsrc"

ObjFiles = ¶
	":program.o" ¶
	":opt.o" ¶


"::::Build" Ä {ObjFiles} ":main.rsrc"
	Duplicate -y ":main.rsrc" "::::Build"
	Link -t APPL -c GrBl -model far ¶
		-o "::::Build" ¶
		"{Libraries}Interface.o" ¶
		"{Libraries}MacRuntime.o" ¶
		{ObjFiles}
	SetFile -d . -m . -a B "::::Build"

clean Ä
	Delete -i {ObjFiles}
	Delete -i "::::Build"
	Delete -i ":main.rsrc"

