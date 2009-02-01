#ifndef LBCOLORMAP_H
#define LBCOLORMAP_H

#include "lbkernel.h"

namespace TINY_LB
{ 

#define LB_CLAMP(a, n, x) ((a) < (n) ? (n) : ((a) > (x) ? (x) : (a)))

	class LBColorMap
	{
	public:
		LBColorMap();
		~LBColorMap();

		int num_colors(void) const;
		void get_color(int, double c[3]) const;
		void set_color(int, const double c[3]);
		void append_color(const double c[3]);
		void map_value(double, double c[3]) const;

	private:
		int   mNcolors;
		int   mAllocated;
		real* mColors;
	};

}

#endif 

