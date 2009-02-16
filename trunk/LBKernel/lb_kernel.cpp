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

#include "lb_kernel.h"

namespace TINY_LB
{

	LBGKCore::LBGKCore(int ndim, int nvel, int rank)
	{
		nDim = ndim;
		nVel = nvel;
		nRank = rank;

		nSize = new int[nDim];
	}

	LBGKCore::~LBGKCore(void)
	{

		delete[] nSize;

		for (int l = 0; l < nVel; l++)
			delete[] f_[l];
		delete[] f_;

		delete[] dataBuffer_;
	}

	void LBGKCore::initCoreBuffer(void)
	{
		nNode = 1;

		for (int i = 0; i < nDim; i++)
			nNode *= nSize[i] + 2;

		f_ = new real*[nVel];

		// Allocate memory buffers
		for (int l = 0; l < nVel; l++)
			f_[l] = new real[nRank*nNode];

		dataBuffer_ = new real[nNode * (nDim + 1)];

		#ifdef TEST_LB
		std::cout <<"initCoreBuffer: " <<"NNODE = " <<nNode*nRank <<", NX = " <<NX <<", NY = " <<NY <<std::endl;
		#endif

	}

	const real* LBGKCore::getData(void) const
	{
		return dataBuffer_;
	}

}
