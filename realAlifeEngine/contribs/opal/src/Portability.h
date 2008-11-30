/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
*                                                                       *
* This library is free software; you can redistribute it and/or         *
* modify it under the terms of EITHER:                                  *
*   (1) The GNU Lesser General Public License as published by the Free  *
*       Software Foundation; either version 2.1 of the License, or (at  *
*       your option) any later version. The text of the GNU Lesser      *
*       General Public License is included with this library in the     *
*       file license-LGPL.txt.                                          *
*   (2) The BSD-style license that is included with this library in     *
*       the file license-BSD.txt.                                       *
*                                                                       *
* This library is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
* license-LGPL.txt and license-BSD.txt for more details.                *
************************************************************************/

#ifndef OPAL_PORTABILITY_H
#define OPAL_PORTABILITY_H

// Use the standard calling convention for Windows DLLs.
#ifndef OPAL_CALL
	#if defined(WIN32) || defined(_WIN32)
		#define OPAL_CALL __stdcall
	#else
		#define OPAL_CALL
	#endif
#endif

// DLL import/export macro.
#ifndef OPAL_DECL
   #if (defined(WIN32) || defined(_WIN32))
      #ifdef OPAL_DLL_EXPORTING
         #define OPAL_DECL __declspec(dllexport)
      #else
            #if !defined(__MINGW32__) && !defined(__CYGWIN__)
                #define OPAL_DECL  __declspec(dllimport)
            #else
                #define OPAL_DECL
            #endif
      #endif
   #else
      #define OPAL_DECL
   #endif
#endif 

/// Macro for exporting functions that keeps function names from being 
/// mangled.  Note:  All class methods for classes instantiated by OPAL 
/// (through factory creation calls) simply need to be virtual.  Classes 
/// instantiated by the user should simply OPAL_DECL to export their 
/// public symbols.  This doesn't allow extern "C" to be used to avoid 
/// name mangling of those class methods, but that isn't possible.
//#define OPAL_EXPORT_FUNCTION(returnType) extern "C" OPAL_DECL returnType OPAL_CALL
#define OPAL_EXPORT_FUNCTION extern "C" OPAL_DECL

#endif
