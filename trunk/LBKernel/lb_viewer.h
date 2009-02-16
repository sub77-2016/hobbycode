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

#ifndef LB_VIEWER_H
#define LB_VIEWER_H

#include "lb_d2q9_mix.h"

struct mglDraw;
class mglGraph;

namespace TINY_LB 
{
	class LBViewer : public mglDraw
	{
	public:
		LBViewer(int nx, int ny);
		~LBViewer(void);

		virtual int Draw(mglGraph *gr);
		virtual void Reload(int next);

		virtual void setData(LBD2Q9Mix* lb, real* phi, real* rho, real* U);

	protected:
		virtual void makeData(void);

		mglData *mPhi, *mRho, *mUx, *mUy;
		int mXdim, mYdim;

	private:
		LBD2Q9Mix* lb_;
		real *phi_, *rho_, *U_;

	};

}

#endif

