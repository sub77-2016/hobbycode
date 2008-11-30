Verve
http://verve-agents.sourceforge.net
Copyright (c) 2004-2008
Tyler Streeter (http://www.tylerstreeter.net)

Verve is a C++ library implementing general purpose reinforcement learning agents. It combines radial basis functions, temporal difference learning, planning, uncertainty estimations, and curiosity. It is intended to be an out-of-the-box solution for roboticists and game developers.

Please visit the project website (http://verve-agents.sourceforge.net) for more information.

=========================================================================
Directory Contents
=========================================================================
apps: A few Verve applications.  See apps/readme.txt for more info.
bindings: Bindings for using Verve in languages other than C++.
build: Used during the build process to store intermediate object files and the resulting libraries.
src: The source code for the Verve library (including all header files), unit tests, and external dependencies.
tools: Miscellaneous tools (e.g. to visualize experimental results).  See tools/readme.txt for more info.
vc8: Visual C++ 8.0 project files.


=========================================================================
Depencencies
=========================================================================
* TinyXML (included)
* SCons (http://www.scons.org) or Microsoft Visual Studio


=========================================================================
Installation
=========================================================================
UNIX: Build the library using SCons with the provided SConstruct file.  Simply typing 'scons' should work.  To install the headers and libraries, type 'scons install prefix=/path/to/install/dir', which will put headers in /path/to/install/dir/include/verve and libraries in /path/to/install/dir/lib.  For more options, type 'scons -h'.

Windows: Use either the provided Visual Studio project files or the SConstruct file to build the library.  (See SCons instructions in the UNIX section.)  Once the library is built, install the header files and import library (.lib file) either by using SCons (e.g. 'scons install prefix=c:\software') or by copying them manually.


=========================================================================
Building and Running an Application with Verve
=========================================================================
Simply include the Verve header file (e.g. #include <verve/verve.h>) in your application, and link with the Verve library.  On Windows, make sure that at runtime the Verve dll is located either in the same directory as your executable or somewhere in your PATH environment variable.
