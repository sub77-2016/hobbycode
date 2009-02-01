#ifndef LBRGB_H
#define LBRGB_H

#include "lbcolormap.h"

namespace TINY_LB
{

	class LBRGB
	{
	public:
		LBRGB(int width, int height);
		~LBRGB(void);

		int width(void) const;
		int height(void) const;
		void fill(const double rgb[3]);
		void get_pixel(int x, int y, double rgb[3]) const;
		void set_pixel(int x, int y, const double rgb[3]);
		void set_pixel_rgba(int, int, const double rgba[4]);
		void save(char*);
		void map_value(const LBColorMap*, int, int, double) const;

	private:
		int mWidth;
		int mHeight;
		int mRowstride;
		real* mPixels;
	};

}

#endif 

