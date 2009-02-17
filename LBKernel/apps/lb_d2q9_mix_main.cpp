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

	std::cout <<std::endl;
	std::cout <<"Binary Fluid Lattice Bolztmann Simualation" <<std::endl;
	std::cout <<"by Waipot Ngamsaad (waipotn@yahoo.com)" <<std::endl; 
	std::cout <<std::endl;

#ifdef USE_GRAPHICS
	int c;
	std::cout <<"Select Graphics GUI" <<std::endl;
	std::cout <<"[1] Wagner" <<std::endl;
	std::cout <<"[2] MathGL" <<std::endl;
	std::cout <<": "; 
	std::cin >> c;

	if (c == 1)
	{
		lb.run();
		
		return 0;
	}
	else if (c == 2)
	{
		return lb.run(argc, argv);
	}
	else
	{
		std::cout <<"GUI not found, Command-line mode will be used!" <<std::endl; 
	}
#endif
	int nt;
	std::cout <<"Enter number of iterations: "; 
	std::cin >> nt;
	lb.run(nt);

	return 0;
}

