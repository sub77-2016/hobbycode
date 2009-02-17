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

#include "lb_d1q3.h"

namespace TINY_LB 
{

	LBD1Q3::LBD1Q3(int nx, int nr)
	: LBCore(1, 3, nr)
	{
		nSize[0] = nx;

		initCoreBuffer();
	}

	void LBD1Q3::stream(void)
	{
		const int xxsize = (NX-1)*SIZE_R;

		real corner0[RANK], corner1[RANK];
		
		// Copy corners 
		memcpy(&corner0[0], f_[0] + pos_f(NX-1), SIZE_R);
		memcpy(&corner1[0], f_[1] + pos_f(0), SIZE_R);

		// Stream inner PDFs
		memmove(f_[0] + pos_f(1),
		       	f_[0] + pos_f(0), xxsize);

		memmove(f_[1] + pos_f(0),
		       	f_[1] + pos_f(1), xxsize);

		// Wrap coners
		memcpy(f_[0] + pos_f(0), &corner0[0], SIZE_R);
  		memcpy(f_[1] + pos_f(NX-1), &corner1[0], SIZE_R);

	}

}

