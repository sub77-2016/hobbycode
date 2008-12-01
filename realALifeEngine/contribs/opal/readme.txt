Open Physics Abstraction Layer
http://opal.sourceforge.net
Copyright (c) 2004-2008
Alan Fischer (http://www.lightningtoads.com)
Andres Reinot (http://www.reinot.com)
Tyler Streeter (http://www.tylerstreeter.net)
Oleksandr Lozitskiy (http://ox.slug.louisville.edu/~o0lozi01/wiki)

OPAL is a high-level interface for low-level physics engines used in games, robotics simulations, and other 3D applications.  Features a simple C++ API, intuitive objects (e.g. Solids, Joints, Motors, Sensors), and XML-based file storage for complex objects.

------------------------------------
Source Release Package contents:
------------------------------------
samples - directory containing example applications that show how to use OPAL
build_scripts - build scripts for various build systems (e.g., Visual Studio)
src - directory containing source code for OPAL, external dependencies, and unit tests
tools - miscellaneous utilities (e.g. exporter scripts for 3rd-party content generation software)
changelog.txt - a list of changes for each released version
license-BSD.txt - BSD Open Source license
license-LGPL.txt - LGPL Open Source license
readme.txt - you're reading it
SConstruct - SCons build system


------------------------------------
Dependencies:
------------------------------------
None.  All external dependencies are already included.


------------------------------------
Generic build instructions:
------------------------------------
Use the provided SConstruct (on any platform) to build a shared library for OPAL.  By default, OPAL is built in optimized mode using ODE with single precision reals.

To build OPAL:

	scons

To build and install the OPAL header and library files:

	scons install prefix=/path/to/install/dir

(This put headers in /path/to/install/dir/include/opal and libraries in /path/to/install/dir/lib.  Be aware that any paths used here should be full paths, not relative ones.)


------------------------------------
Visual Studio build instructions:
------------------------------------
Use the included Visual Studio project files to build a shared library (win32 .dll file and .lib import library file containing all the dll symbols) for OPAL.  After building the library, copy the headers and .lib file into your include and library path, respectively.  Put the .dll into the same directory as your executable or into the places where Windows looks for dlls (e.g. c:\WINDOWS\SYSTEM32).


------------------------------------
Using OPAL in your application:
------------------------------------
Include opal.h in your application and link against the OPAL library.  If you built OPAL with double precision reals, you must define OPAL_USE_DOUBLE in your application.
