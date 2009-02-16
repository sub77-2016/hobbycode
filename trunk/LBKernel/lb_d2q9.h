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

#ifndef LB_D2Q9_H
#define LB_D2Q9_H

#include "lb_kernel.h"

		/*
		. D2Q9 Lattice Boltzmann Model
		.
		.           y
		.           |
		.           1
		.         5 | 4
		.          \|/
		.        2--8--0--x
		.          /|\
		.         6 | 7
		.           3
		*/ 

namespace TINY_LB 
{

	
	#define pos_r(x,y) ((NY + 2)*(x) + (y))
	#define pos_f(x,y) (RANK*pos_r((x),(y)))
	#define pos_v(x,y) (NDIM*pos_r((x),(y)))	
       	#define ff(v,x,y,l) (*(f_[(v)]+pos_f((x),(y))+(l)))

	class LBD2Q9 : public LBGKCore
	{
	public:
		LBD2Q9(int nx, int ny, int nr);
		virtual ~LBD2Q9(){}

		virtual void step(void){}

	protected:
		virtual void stream(void);

	private:
		void wrap_0_2(void);
		void wrap_1_3(void);
		void wrap_corners(void);

		inline void scpy(real* dst, const real* src, int rank, int stride, int count);
		inline real* address(real* pdfs[8], const int info[3]);
		inline void colscpy(real* pdfs[8], const int info[][6], int ncols);
		inline void rowscpy(real* pdfs[8], const int info[][6], int nrows);
	};


}

#endif
