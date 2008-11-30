OPAL Sample Applications

These applications demonstrate some of OPAL's features.  Each application comes with a vc71 project for win32 and an SConstruct for all platforms.  A template OPAL-Ogre application has been provided in the TemplateApp.cpp file for anyone wanting to write their own apps using OPAL with Ogre.

------------------------------------
Available applications:
------------------------------------

Playpen*
-------
An arena where objects can be created and manipulated.  Uses a Raycast Sensor and Spring Motor to allow the user to pick and move any dynamic object in the scene.  Click and drag the left mouse button to move objects, arrow or 'wasd' keys to move the camera, 1-9 to create objects, 'r' to reset the scene, 'q' or 'esc' to quit, 'p' to pause, 'h' to toggle shadows, and 'l' to toggle a second light source.

Simple**
-------
A simple example for beginners.  Keypresses from the user create random primitive objects in random locations.  Press 'space' to create objects, 'q' or 'esc' to quit, and 'p' to pause.


* Uses the Ogre rendering engine (www.ogre3d.org) for graphics.  Requires version 1.0.1 or later.

** Uses SDL (www.libsdl.org) for input handling and window management.  Requires version 1.2.7 or later.