/*
 * Created by Waipot Ngamsaad (ngamsaad.waipot@gmail.com)
 * Dated 20-12-2008

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

#ifndef LB_D1Q3_H
#define LB_D1Q3_H

#include "lb_kernel.h"

/*
. D1Q3 Lattice Boltzmann Model
.
.        1--2--0--x
.
*/ 

namespace TINY_LB 
{

	
	#define pos_r(x) ((x))
	#define pos_f(x) (RANK*pos_r((x)))
	#define pos_v(x) (NDIM*pos_r((x)))	

	class LBD1Q3 : public LBCore
	{
	public:
		LBD1Q3(int nx, int nr);
		virtual ~LBD1Q3(){}

	protected:
		virtual void collide(void) = 0;
		virtual void stream(void);

	};


}

#endif
