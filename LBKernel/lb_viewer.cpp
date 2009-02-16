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

#include <mgl/mgl_fltk.h>

#include "lb_viewer.h"

namespace TINY_LB 
{
	LBViewer::LBViewer(int nx, int ny)
	{
		mXdim = nx;
		mYdim = ny;

		mPhi = new mglData(nx, ny);
		mRho = new mglData(nx, ny);

		mUx = new mglData(nx, ny);
		mUy = new mglData(nx, ny);
	}

	LBViewer::~LBViewer(void)
	{
		delete mPhi;
		delete mRho;
		delete mUx;
		delete mUy;
	}

	void LBViewer::setData(LBD2Q9Mix* lb, real* phi, real* rho, real* U)
	{
		phi_ = phi;
		rho_ = rho;
		U_ = U;

		lb_ = lb;
	}

	void LBViewer::makeData(void)
	{
		for (int x = 1; x < mXdim+1; x++)
		{
    			for (int y = 1; y < mYdim+1; y++)
			{
				mPhi->a[(x-1)+mXdim*(y-1)] = phi_[(mYdim+2)*x+y];
				mRho->a[(x-1)+mXdim*(y-1)] = rho_[(mYdim+2)*x+y];

				mUx->a[(x-1)+mXdim*(y-1)] = U_[2*((mYdim+2)*x+y)+0];
				mUy->a[(x-1)+mXdim*(y-1)] = U_[2*((mYdim+2)*x+y)+1];

				#ifdef TEST_LB
				std::cout <<"makeData: (phi, rho)[" <<x <<"][" <<y <<"] = (" <<phi_[(mYdim+2)*x+y]<<", " <<rho_[(mYdim+2)*x+y]<<")" << std::endl;
				#endif
    			}
  		}
	}

	void LBViewer::Reload(int next)
	{
		for (int i = 0; i < 25; i++)
			lb_->step();

		makeData();
	}

	int LBViewer::Draw(mglGraph *gr)
	{
		//for (int i = 0; i < 500; i++)
			//lb_->step();

		//makeData();

		gr->NewFrame();
		gr->Box();
		gr->Axis("xy");
		gr->VectC(*mUx,*mUy);
		gr->Colorbar();
        	gr->EndFrame();
     	
		gr->NewFrame();
		gr->Box();
		gr->Axis("xy");
		gr->Dens(*mRho);
		gr->Colorbar();
        	gr->EndFrame();

		gr->NewFrame();
		gr->Box();
		gr->Axis("xy");
		gr->Dens(*mPhi);
		gr->Colorbar();
        	gr->EndFrame();

    		return gr->GetNumFrame();
	}

}

