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

#ifndef LB_D2Q9_MIX_H
#define LB_D2Q9_MIX_H

#include "lb_d2q9.h"

class mglGraph;

namespace TINY_LB
{
	class LBD2Q9Mix : public LBD2Q9
	{
	public:
		LBD2Q9Mix(int nx, int ny);
		virtual ~LBD2Q9Mix(void);

		virtual void init(void (*loader)(int x, int y, real* rho0, real& ux, real& uy));	
		virtual void step(void);
		virtual void run(int time);
		virtual void writeOutput(void);

	#ifdef USE_GRAPHICS
		virtual void run(void);
		virtual int run(int argc, char **argv);
	#endif

	protected:
		virtual void computeMoments(void);
		virtual void collide(void);
		virtual void multcollide(void);
		virtual void f_eq(const int x, const int y, real f_out[9][2]);

	#ifdef USE_GRAPHICS
		virtual void initGUI(void);
		virtual void reinit(void);

		// GUI stuff
		int mXdim, mYdim;
		int mQuit, mPause, mIterations, mSingleStep, mRepeat;
	#endif

		real *phi;
		real *rho;
		real *U;

		real *omega;

		real C[9][9];
	
	private:
		void (*load_func)(int, int, real*, real&, real&);

	};

}

#endif
