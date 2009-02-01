extern "C" {
#include <png.h>
}

#include "lbrgb.h"

namespace TINY_LB
{

	LBRGB::LBRGB(int width, int height)
	{
		mWidth = width;
		mHeight = height;

		mRowstride = 3*mWidth;
		mPixels = new real[mRowstride*mHeight];
	}

	LBRGB::~LBRGB(void)
	{
		delete[] mPixels;
	}

	int
	LBRGB::width(void) const
	{
		return mWidth;
	}

	int
	LBRGB::height(void) const
	{
		return mHeight;
	}

	void
	LBRGB::fill(const real c[3])
	{
		real* end;
		real* ptr;
		real color[3];

		color[0] = LB_CLAMP(c[0], 0.0, 1.0);
		color[1] = LB_CLAMP(c[1], 0.0, 1.0);
		color[2] = LB_CLAMP(c[2], 0.0, 1.0);

		end = mPixels + mRowstride;

		for (ptr = mPixels; ptr != end; ptr += 3) {
			ptr[0] = color[0];
			ptr[1] = color[1];
			ptr[2] = color[2];
		}

		end = mPixels + mHeight*mRowstride;
		for (; ptr != end; ptr += mRowstride)
			memcpy(ptr, mPixels, mRowstride*sizeof(real));
	}

	void
	LBRGB::get_pixel(int x, int y, real pixel[3]) const
	{
		const real* ptr;

		ptr = mPixels + y*mRowstride + 3*x;

		pixel[0] = ptr[0];
		pixel[1] = ptr[1];
		pixel[2] = ptr[2];
	}

	void
	LBRGB::set_pixel(int x, int y, const real pixel[3])
	{
		real* ptr;

		ptr = mPixels + y*mRowstride + 3*x;

		ptr[0] = LB_CLAMP(pixel[0], 0.0, 1.0);
		ptr[1] = LB_CLAMP(pixel[1], 0.0, 1.0);
		ptr[2] = LB_CLAMP(pixel[2], 0.0, 1.0);
		std::cout <<"c0 = " <<ptr[0] <<", c1 = " <<ptr[1] <<", c2 = " <<ptr[2] <<std::endl; 

	}

	void
	LBRGB::set_pixel_rgba(int x, int y, const real rgba[4])
	{
		real* ptr;

		ptr = mPixels + y*mRowstride + 3*x;

		{
			const real a1 = LB_CLAMP(rgba[3], 0.0, 1.0);
			const real a2 = 1.0 - a1;

			ptr[0] = a2*ptr[0] + a1*LB_CLAMP(rgba[0], 0.0, 1.0);
			ptr[1] = a2*ptr[1] + a1*LB_CLAMP(rgba[1], 0.0, 1.0);
			ptr[2] = a2*ptr[2] + a1*LB_CLAMP(rgba[2], 0.0, 1.0);
		}
	}

	#ifndef png_jmpbuf
	#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
	#endif /* png_jmpbuf */

	void
	LBRGB::save(char* name)
	{
		png_structp png_ptr;
		png_infop info_ptr;
		png_color_8 sig_bit;

		FILE *file = NULL;

		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                          NULL, NULL, NULL);
		if (png_ptr == NULL)
			Fatal_error("png_create_write_struct: error!");

		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			png_destroy_write_struct(&png_ptr, &info_ptr);
			Fatal_error("png_create_info_struct: error!");
		}

		if (setjmp(png_jmpbuf(png_ptr))) {
			png_destroy_write_struct(&png_ptr, &info_ptr);
			Fatal_error("setjmp(png_jmpbuf(png_ptr): error!");
		}

		png_init_io(png_ptr, file);
        	png_set_IHDR(png_ptr, info_ptr, mWidth, mHeight, 8,
                     	PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                     	PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		sig_bit.red = 8;
		sig_bit.green = 8;
		sig_bit.blue = 8;
		sig_bit.alpha = 8;

		png_set_sBIT(png_ptr, info_ptr, &sig_bit);
		png_write_info(png_ptr, info_ptr);
		png_set_shift(png_ptr, &sig_bit);
		png_set_packing(png_ptr);

		{
			const real* ptr = mPixels;
			char* bytes = new char[mRowstride];
			int x, y;

			for (y = 0; y < mHeight; ++y) {
    				png_bytep row_ptr;

				for (x = 0; x < mRowstride; ++x)
					bytes[x] = (unsigned char) (255.0*ptr[x]);

    				row_ptr = (png_bytep) bytes;
				png_write_rows(png_ptr, &row_ptr, 1);
            			ptr += mRowstride;
			}
		}

		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);
	}

	void
	LBRGB::map_value(const LBColorMap* cmap,
		   	int x, int y, real v) const
	{
        	real* ptr;

		ptr = mPixels + y*mRowstride + 3*x;
		cmap->map_value(v, ptr);
	}

}

