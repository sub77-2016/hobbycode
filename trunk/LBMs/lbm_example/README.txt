. ---
. Minimal LBM demonstration program by Nils Thuerey 
. See http://www.ntoken.com and http://www.matthiasmueller.info/realtimephysics/ for further info
. All code is released under the GPL, see LICENSE.txt for details
. ---


Usage:
The files are tailored towards a UNIX like system, under windows it's best to
use, e.g., Cygwin (MSVC should, however, also work). Just running 'make' should
give you an executable called 'lbm'. 

By default, executing 'lbm' will run a 32^3 LBM simulation without turbulence
model, and output PPM images of a middle slice of the pressure values after
each 20 steps.  If ImageMagick is installed, the images will be converted to
PNGs. The setup is a lid-driven-cavity, which means that the top layer of the
domain is set to a fixed velocity, so that a large vortex develops in the
middle of the domain.


Turbulence model:
To switch on the Smagorinsky turbulence model, set the SMAGORINSKY bool
variable in lbm.c to true. You can then lower the viscosity (set omega closer
to 2), and increase the velocity (LDC_VELOCITY). When the turbulence model is
activated, try increasing the values, E.g., to omega=1.999 and velocity=0.166.
Hard upper limit for omega is 2, for the velocity around 1/3 (for lower
viscosities, it's closer to 1/6, though).


OpenMP:
The basic version compiles without using OpenMP. To enable it, uncomment the
marked lines in the Makefile. It requires a recent version of GCC (>=4.2)
or MSVC, and you might need to specify the correct compiler version using the
CXX variable.


Debugging:
The total mass, computed for the picture output, should stay constant
throughout the simulation. This is a good start for debugging an LBM code...


Where to go from here:
Some things should be easy to try out, e.g.,
- introduce arbitrary obstacles in the flow by setting flags to NOSLIP.
- manually allocate the cells + flags array, and manually compute offsets
  into it. This should allow for larger grids.
- Combine the stream and collide loops into a single one.
- Add a semi-Lagrange step to trace a density in the flow.



