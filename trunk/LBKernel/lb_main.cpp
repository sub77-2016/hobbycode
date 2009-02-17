/*
 * Created by Waipot Ngamsaad (ngamsaad.waipot@gmail.com)
 * Dated 25-01-2009

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#ifdef USE_GRAPHICS
#include <mgl/mgl_fltk.h>
#endif

#include "lb_d2q9_mix.h"

using namespace TINY_LB;

void loader(int x, int y, real* rho0, real& ux, real& uy)
{
      	rho0[0] = 0. + 0.0001*(1.*rand()/RANDMAX -0.5);
      	rho0[1] = 1. + 0.0001*(1.*rand()/RANDMAX -0.5);

	ux = (real)0.0;
	uy = (real)0.0;
}

int 
main(int argc, char* argv[])
{
	const int nx = 129, ny = 129;

	LBD2Q9Mix lb(nx,ny);
	lb.init(loader);

#ifdef USE_GRAPHICS
	//lb.run();
	return lb.run(argc, argv);
#else
	lb.run(1000);
#endif

	return 0;
}

