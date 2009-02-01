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

#include <iostream>

#include "lbd2q9.h"

namespace TINY_LB 
{


	LBD2Q9::LBD2Q9(int nx, int ny, int nr)
	: LBGKCore(2, 9, nr)
	{
		nSize[0] = nx;
		nSize[1] = ny;

		initCoreBuffer();
	}

	inline real*
	LBD2Q9::address(real* pdfs[8], const int info[3])
	{
		return pdfs[info[2]] + pos_f(info[0], info[1]);
	}

	void 
	LBD2Q9::scpy(real* dst, const real* src,
	             int rank, int stride, int count)
	{
		rank *= sizeof(real);
		for (; count > 0; --count) 
		{
			memcpy(dst, src, rank);
			dst += stride;
			src += stride;
		}
	}

	inline void
	LBD2Q9::colscpy(real* pdfs[8],
		        const int info[][6], int ncols)
	{
		const int colsize = RANK*NY*sizeof(real);
		for (; ncols > 0; --ncols, ++info)
			memcpy(address(pdfs, *info),
		       	       address(pdfs, *info + 3),
		       	       colsize);
	}

	inline void
	LBD2Q9::rowscpy(real* pdfs[8],
		  const int info[][6], int nrows)
	{
		for (; nrows > 0; --nrows, ++info)
			scpy(address(pdfs, *info),
			     address(pdfs, *info + 3),
			     RANK, RANK*(NY + 2), NX);
	}

	void
	LBD2Q9::wrap_0_2(void)
	{
		const int todo[6][6] = {
			{NX, 1, 2, 0, 1, 2},
			{NX, 2, 5, 0, 2, 5},
			{NX, 0, 6, 0, 0, 6},
			{1, 1, 0, NX + 1, 1, 0},
			{1, 2, 4, NX + 1, 2, 4},
			{1, 0, 7, NX + 1, 0, 7}
		};

		colscpy(f_, todo, 6);
	}

	void
	LBD2Q9::wrap_1_3(void)
	{
		const int todo[6][6] = {
			{0, NY, 6, 0, 0, 6},
			{1, NY, 3, 1, 0, 3},
			{2, NY, 7, 2, 0, 7},
			{0, 1, 5, 0, NY + 1, 5},
			{1, 1, 1, 1, NY + 1, 1},
			{2, 1, 4, 2, NY + 1, 4}
		};

		rowscpy(f_, todo, 6);
	}

	void
	LBD2Q9::wrap_corners(void)
	{
		const int todo[4][6] = {
			{NX, NY, 6, 0, 0, 6},
			{NX, 1, 5, 0, NY + 1, 5},
			{1, NY, 7, NX + 1, 0, 7},
			{1, 1, 4, NX + 1, NY + 1, 4}
		};

		for (int i = 0; i < 4; ++i)
			memcpy(f_[todo[i][2]] + pos_f(todo[i][0], todo[i][1]),
		       	       f_[todo[i][5]] + pos_f(todo[i][3], todo[i][4]),
		       	       RANK*sizeof(real));
	}

	void LBD2Q9::stream(void)
	{
		const int colsize = RANK*NY*sizeof(real);

		// Stream inner
		for (int x = 1; x <= NX; ++x) 
		{
			memcpy(f_[0] + pos_f(NX - x + 2, 1),
		       	       f_[0] + pos_f(NX - x + 1, 1),
		       		colsize);

			memmove(f_[1] + pos_f(x, 2),
				f_[1] + pos_f(x, 1),
		        	colsize);

			memcpy(f_[2] + pos_f(x - 1, 1),
		       	       f_[2] + pos_f(x, 1),
		       		colsize);

			memmove(f_[3] + pos_f(x, 0),
				f_[3] + pos_f(x, 1),
		        	colsize);

			memcpy(f_[4] + pos_f(NX - x + 2, 2),
		  	       f_[4] + pos_f(NX - x + 1, 1),
		       	       colsize);

			memcpy(f_[5] + pos_f(x - 1, 2),
		       	       f_[5] + pos_f(x, 1),
		       	       colsize);

			memcpy(f_[6] + pos_f(x - 1, 0),
		       	       f_[6] + pos_f(x, 1),
		       	       colsize);

			memcpy(f_[7] + pos_f(NX - x + 2, 0),
		       	       f_[7] + pos_f(NX - x + 1, 1),
		       	       colsize);
		}

		// Periodic wrapping
		wrap_0_2();
		wrap_1_3();
		wrap_corners();
	}


}

