#include <cmath>

#include "lbcolormap.h"


namespace TINY_LB
{

	LBColorMap::LBColorMap(void)
	{
		mNcolors = 0;
		mAllocated = 32;
		mColors = new real[3*mAllocated];
	}

	LBColorMap::~LBColorMap(void)
	{
		delete mColors;
	}

	int
	LBColorMap::num_colors(void) const
	{
		return mNcolors;
	}

	void
	LBColorMap::get_color(int c, real color[3]) const
	{
		c *= 3;

		color[0] = mColors[c++];
		color[1] = mColors[c++];
		color[2] = mColors[c++];
	}

	void
	LBColorMap::set_color(int c, const real color[3])
	{
		c *= 3;

		mColors[c++] = color[0];
		mColors[c++] = color[1];
		mColors[c++] = color[2];
	}

	void
	LBColorMap::append_color(const real color[3])
	{

		if (mNcolors == mAllocated) {
			const int a = mAllocated;

			mAllocated *= 2;

			real* tmp = new real[3*mAllocated];		
			memcpy(tmp, mColors, 3*a*sizeof(real));

			delete[] mColors;
			mColors = tmp;
		}

		mColors[3*mNcolors]     = color[0];
		mColors[3*mNcolors + 1] = color[1];
		mColors[3*mNcolors + 2] = color[2];

		mNcolors += 1;
	}

	void
	LBColorMap::map_value(real v, real color[3]) const
	{
		int bin;

		v = LB_CLAMP(v, 0.0, 1.0);

        	bin = floor(v*(mNcolors - 1));
        	bin = LB_CLAMP(bin, 0, mNcolors - 1);

        	{
                	const real* c0 = mColors + 3*bin;
                	const real* c1 = c0 + 3;
                	const real t = v*(mNcolors - 1) - bin;
                	const real t1 = 1.0 - t;

                	color[0] = c0[0]*t1 + c1[0]*t;
                	color[1] = c0[1]*t1 + c1[1]*t;
                	color[2] = c0[2]*t1 + c1[2]*t;
        	}
	}

}

