Open Physics Abstraction Layer
http://opal.sourceforge.net
Copyright (c) 2004-2008
Alan Fischer (http://www.lightningtoads.com)
Andres Reinot (http://www.reinot.com)
Tyler Streeter (http://www.tylerstreeter.net)
Oleksandr Lozitskiy (http://ox.slug.louisville.edu/~o0lozi01/wiki)

OPAL is a high-level interface for low-level physics engines used in games, robotics simulations, and other 3D applications.  Features a simple C++ API, intuitive objects (e.g. Solids, Joints, Motors, Sensors), and XML-based file storage for complex objects.

This file contains basic installation info.  For more documentation, visit the OPAL website.

------------------------------------
Package contents:
------------------------------------
include - OPAL headers
lib - OPAL libraries (debug and release versions of the OPAL dll and import libraries built with single and double floating point precision)
samples - executable versions of sample applications and required dlls
changelog.txt - a list of changes for each released version
license-BSD.txt - BSD Open Source license
license-LGPL.txt - LGPL Open Source license
win32BinaryReadme.txt - you're reading it


------------------------------------
Dependencies:
------------------------------------
None.


------------------------------------
Installation:
------------------------------------
Copy the OPAL headers and import libraries into your include and library path, respectively.  Put the dlls into the same directory as your executable or into the places where Windows looks for dlls (e.g. c:\WINDOWS\SYSTEM32).


------------------------------------
Using OPAL in your application:
------------------------------------
Add #include <opal/opal.h> in your application and link against the OPAL library.
