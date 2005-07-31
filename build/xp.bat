@echo off
setlocal

rem build with XP Command Line

set my_source_d=%cd%

if exist "%my_source_d%\c_src" goto L_end_no_src
	echo The current directory isn't right,
	echo the 'c_src' folder doesn't exist.
	goto L_the_exit
:L_end_no_src

rem ---------------------
rem find output directory

set to_relative=yes
set my_output_d=output
if not "%1"=="-to" goto L_else_to_arg
	set to_relative=no
	set my_output_d=%2
	shift
	shift
	goto L_end_to_arg
:L_else_to_arg
if not "%1"=="-torel" goto L_end_to_arg
	set my_output_d=%2
	shift
	shift
:L_end_to_arg

set fresh_output=no
if not exist "%my_output_d%" set fresh_output=yes

if %fresh_output%==yes mkdir "%my_output_d%"

cd "%my_output_d%"
set my_output_d=%cd%

rem -------------------
rem get setup directory

if not exist "setup" mkdir "setup"
cd "setup"

set my_setup_d=%cd%

echo.>"%my_setup_d%\setupopt.h"
del "%my_setup_d%\setupopt.h"

cd "%my_source_d%"

rem ------------------------
rem parse rest of parameters

set do_compile=yes
set keep_setup=no
set keep_make=no
set use_IDE=auto
set use_Targ=auto
set use_Script=auto
set be_quiet=no

:L_start_param_loop
if "%1"=="" goto L_end_param_loop
	set cur_arg=%1
	shift

	if "%cur_arg%"=="-nocompile" goto L_param_nocompile
	if "%cur_arg%"=="-keepsetup" goto L_param_keepsetup
	if "%cur_arg%"=="-keepmake" goto L_param_keepmake
	if "%cur_arg%"=="-quiet" goto L_param_quiet
	if "%cur_arg%"=="-ds" goto L_param_ds
	if "%cur_arg%"=="-d" goto L_param_d
	if "%cur_arg%"=="-ide" goto L_param_ide
	if "%cur_arg%"=="-targ" goto L_param_targ
	if "%cur_arg%"=="-script" goto L_param_script

	rem default
		echo #define %cur_arg% ^1>>"%my_setup_d%\setupopt.h"
		goto :L_start_param_loop
	:L_param_nocompile
		set do_compile=no
		goto :L_start_param_loop
	:L_param_keepsetup
		set keep_setup=yes
		goto :L_start_param_loop
	:L_param_keepmake
		set keep_make=yes
		goto :L_start_param_loop
	:L_param_quiet
		set be_quiet=yes
		goto :L_start_param_loop
	:L_param_ds
		echo #define %1 "%2">>"%my_setup_d%\setupopt.h"
		shift
		shift
		goto :L_start_param_loop
	:L_param_d
		echo #define %1 %2>>"%my_setup_d%\setupopt.h"
		shift
		shift
		goto :L_start_param_loop
	:L_param_ide
		if not "%use_IDE%"=="auto" goto L_param_ide_dup
			set use_IDE=UseIde%1
			shift
			goto :L_start_param_loop
		:L_param_ide_dup
			echo only one ide parameter allowed
			goto L_the_exit
	:L_param_targ
		if not "%use_Targ%"=="auto" goto L_param_targ_dup
			set use_Targ=UseTarg%1
			shift
			goto :L_start_param_loop
		:L_param_targ_dup
			echo only one targ parameter allowed
			goto L_the_exit
	:L_param_script
		if not "%use_Script%"=="auto" goto L_param_script_dup
			set use_Script=UseScript%1
			shift
			goto :L_start_param_loop
		:L_param_script_dup
			echo only one targ parameter allowed
			goto L_the_exit

:L_end_param_loop

if %be_quiet%==no echo build with XP Command Line
if %be_quiet%==no echo my_source_d = %my_source_d%
if %be_quiet%==no echo my_output_d = %my_output_d%

if "%use_IDE%"=="auto" set use_IDE=UseIdeNMake

if "%use_Targ%"=="auto" set use_Targ=UseTargWinx86

if "%use_Script%"=="auto" set use_Script=UseScriptXPCmdLn

if %to_relative%==yes echo #define TheUseAbsolute ^0>>"%my_setup_d%\setupopt.h"
echo #define %use_Script% ^1>>"%my_setup_d%\setupopt.h"
echo #define %use_IDE% ^1>>"%my_setup_d%\setupopt.h"
echo #define %use_Targ% ^1>>"%my_setup_d%\setupopt.h"

set my_cc=cl.exe /nologo

%my_cc% "%my_source_d%\build\setup.c" /Fo"%my_setup_d%\setup.obj" /Fe"%my_setup_d%\setup.exe" /I"%my_setup_d%" /I"%my_source_d%"

"%my_setup_d%\setup.exe" >"%my_setup_d%\setup.bat"

if not UseScriptXPCmdLn==%use_Script% goto L_end_ScriptXPCmdLn

	if not %to_relative%==yes goto L_not_to_relative
		call "%my_setup_d%\setup.bat" "%my_output_d%"
	goto L_end_to_relative
	:L_not_to_relative
		call "%my_setup_d%\setup.bat" "%my_output_d%" "%my_source_d%"
	:L_end_to_relative

	if %keep_setup%==no rmdir "%my_setup_d%" /s /q

	if not "%use_IDE%"=="UseIdeNMake" goto L_end_IdeNMake
		cd "%my_output_d%"

		set mk_opt=/NOLOGO
		if %be_quiet%==yes set mk_opt=%mk_opt% /S

		if %fresh_output%==no nmake clean %mk_opt%

		if not %do_compile%==yes goto L_end_domake
			if %be_quiet%==no echo Make

			nmake %mk_opt%

			if not %keep_make%==no goto L_end_keepmake
				del "%my_output_d%\MAKEFILE"
				rmdir "%my_output_d%\config" /s /q
				rmdir "%my_output_d%\obj" /s /q
			:L_end_keepmake
		:L_end_domake

		cd "%my_source_d%"
	:L_end_IdeNMake
:L_end_ScriptXPCmdLn

if %be_quiet%==no echo Done

:L_the_exit
endlocal
