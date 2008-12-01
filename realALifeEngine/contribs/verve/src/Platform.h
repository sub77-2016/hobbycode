#ifndef VERVE_PORTABILITY_H
#define VERVE_PORTABILITY_H

// General DLL interface notes: 
// 
// In general we can make each member function virtual to avoid exporting 
// each one explicitly (instantiating an object through a factory function 
// is all that is necessary to access the object's functions).  In the 
// Verve DLL we export every member function explicitly to allow us to 
// staticly allocate objects (mainly for the unit tests).  In most 
// applications, developers will only interact with the Agent and 
// Observation classes.  Agents are the only object that can be dynamically 
// allocated, which occurs indirectly through a factory function.

// Use the standard calling convention for Windows DLLs.
#ifndef VERVE_CALL
	#if defined(WIN32) || defined(_WIN32)
		#define VERVE_CALL __stdcall
	#else
		#define VERVE_CALL
	#endif
#endif

// DLL import/export macro.
#ifndef VERVE_DECL
	#if defined(WIN32) || defined(_WIN32)
		// VERVE_DLL_EXPORTING should only be defined when building the 
		// Verve library.
		#ifdef VERVE_DLL_EXPORTING
			#define VERVE_DECL __declspec(dllexport)
		#else
			#define VERVE_DECL __declspec(dllimport)
		#endif
	#else
		#define VERVE_DECL
	#endif
#endif

/// Macro for exporting functions that keeps function names from being 
/// mangled.  All member functions for classes instantiated by Verve 
/// (through factory creation calls) simply need to be virtual; they 
/// don't need to be exported explicitly.  
//#define VERVE_EXPORT_FUNCTION(returnType) extern "C" VERVE_DECL returnType VERVE_CALL
#define VERVE_EXPORT_FUNCTION extern "C" VERVE_DECL

#endif
