. ---
. Minimal shallow water demonstration program by Nils Thuerey 
. See http://www.ntoken.com and http://www.matthiasmueller.info/realtimephysics/ for further info
. All code is released under the GPL, see LICENSE.txt for details
. ---


Usage:
The files are tailored towards a UNIX like system, under windows it's best to
use, e.g., Cygwin (MSVC should, however, also work). Just running 'make' should
give you an executable called 'sws'. 

By default, executing 'sws' will run a 100^2 shallow water simulation with
random "drops" being added over time.  The program will output PPM images of
the height field and velocities every two simulation steps. If ImageMagick is
installed, the images will be converted to PNGs. 

Try increasing the SIZE cnostant for more detailed simulations. The 
advection can be disabled by commenting out the three advectArray
calls in the main function.


Where to go from here:
Some things should be easy to try out, e.g.,
- vary the resolution of the simulation with the SIZE constant,
  and change its length with the END_TIME constant
- add a simple GUI to allow "drawing" waves, and render a quad
  mesh using the heigh values from the simulation
- add spherical obstacles that can be dragged through the water


