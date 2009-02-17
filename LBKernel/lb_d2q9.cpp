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

#include "lb_d2q9.h"

namespace TINY_LB 
{

	LBD2Q9::LBD2Q9(int nx, int ny, int nr)
	: LBCore(2, 9, nr)
	{
		nSize[0] = nx;
		nSize[1] = ny;

		initCoreBuffer();
	}

	void LBD2Q9::stream(void)
	{
		const int xxsize = (NX*NY - 1)*SIZE_R;
		const int yysize = (NX*NY - NX)*SIZE_R;
		const int xysize = (NX*NY - NX - 1)*SIZE_R;

		real corner0[RANK], corner2[RANK], corner4[RANK], corner5[RANK], corner6[RANK], corner7[RANK];
		real fout_1[RANK*NX], fout_3[RANK*NX];

		// Copy rim rows
		memcpy(&fout_1[0], f_[1] + pos_f(0, NY-1), NX*SIZE_R);
		memcpy(&fout_3[0], f_[3] + pos_f(0, 0), NX*SIZE_R);
		
		// Copy corners 
		memcpy(&corner0[0], f_[0] + pos_f(NX-1, NY-1), SIZE_R);
		memcpy(&corner2[0], f_[2] + pos_f(0, 0), SIZE_R);
		memcpy(&corner4[0], f_[4] + pos_f(NX-1, NY-1), SIZE_R);
  		memcpy(&corner5[0], f_[5] + pos_f(0, NY-1), SIZE_R);
  		memcpy(&corner6[0], f_[6] + pos_f(0, 0), SIZE_R);
  		memcpy(&corner7[0], f_[7] + pos_f(NX-1, 0), SIZE_R);

		// Stream inner PDFs
		memmove(f_[0] + pos_f(1, 0),
		       	f_[0] + pos_f(0, 0), xxsize);

		memmove(f_[1] + pos_f(0, 1),
			f_[1] + pos_f(0, 0), yysize);

		memmove(f_[2] + pos_f(0, 0),
		       	f_[2] + pos_f(1, 0), xxsize);

		memmove(f_[3] + pos_f(0, 0),
			f_[3] + pos_f(0, 1), yysize);

		memmove(f_[4] + pos_f(1, 1),
		  	f_[4] + pos_f(0, 0), xysize);

		memmove(f_[5] + pos_f(1, 0),
		       	f_[5] + pos_f(0, 1), xysize);

		memmove(f_[6] + pos_f(0, 0),
		       	f_[6] + pos_f(1, 1), xysize);

		memmove(f_[7] + pos_f(1, 0),
		       	f_[7] + pos_f(0, 1), xysize);

		// Wrap rim columns
		scpy(f_[0] + pos_f(0, 0),
		     f_[0] + pos_f(0, 1), RANK, NX, NY-1);

		smove(f_[2] + pos_f(NX-1, 1),
		      f_[2] + pos_f(NX-1, 0), RANK, NX, NY-1);

		// Wrap rim rows
		memcpy(f_[1] + pos_f(0, 0),
		       &fout_1[0], NX*SIZE_R);

		memcpy(f_[3] + pos_f(0, NY-1),
		       &fout_3[0], NX*SIZE_R);

		// Wrap coners
		memcpy(f_[0] + pos_f(0, NY-1), &corner0[0], SIZE_R);
  		memcpy(f_[2] + pos_f(NX-1, 0), &corner2[0], SIZE_R);
		memcpy(f_[4] + pos_f(0, 0), &corner4[0], SIZE_R);
  		memcpy(f_[5] + pos_f(NX-1, 0), &corner5[0], SIZE_R);
  		memcpy(f_[6] + pos_f(NX-1, NY-1), &corner6[0], SIZE_R);
  		memcpy(f_[7] + pos_f(0, NY-1), &corner7[0], SIZE_R);

	}

}

