/*
	when compiling with MSVC,
	copy this configuration file into
	the c_src directory, replacing any
	existing file of the same name.
*/

#ifndef _MSC_VER
#error wrong CNFGGLOB.h
#endif
#ifndef _M_IX86
#error wrong CNFGGLOB.h
#endif

/* ignore integer conversion warnings */
#pragma warning(disable : 4244 4761 4018 4245 4024 4305)

/* ignore unused inline warning */
#pragma warning(disable : 4514)

#define MayInline __inline

#define HaveOSTarget 1
#define WinTarget 1
