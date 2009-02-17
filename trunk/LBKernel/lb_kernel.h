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

#ifndef LB_KERNEL_H
#define LB_KERNEL_H

#include <iostream>

namespace TINY_LB 
{

#ifdef LB_SINGLE
	typedef float real;
#else
	typedef double real;
#endif

typedef char int8;
typedef int int16;
typedef long int int32;

typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long int uint32;

#define RANDMAX 2147483647
#define PI 3.14159265358979323846264338327

#define FLUID 0
#define SOLID 1

#define NX nSize[0]
#define NY nSize[1]
#define NZ nSize[2]

#define RANK nRank
#define NDIM nDim
#define NVEL nVel
#define NNODE nNode

#define SIZE_R RANK*sizeof(real)

#ifndef Error
#define Error(Str) Fatal_error(Str)
#define Fatal_error(Str) fprintf(stderr,"%s\n",Str), exit(1)
#endif

	inline real sqr(real x)
	{
		return x * x;
	}

	inline void scpy(real* dst, const real* src,
	             int rank, int stride, int count)
	{
		rank *= sizeof(real);
		for (; count > 0; --count) 
		{
			memcpy(dst, src, rank);
			dst += stride;
			src += stride;
		}
	}

	inline void smove(real* dst, const real* src,
	             int rank, int stride, int count)
	{
		real *_dst = dst;
		const real *_src = src;

		stride *= rank;
		rank *= sizeof(real);

		if (_src < _dst)
		{
			_dst += count;
			_src += count;

			for (; count > 0; --count) 
			{
				memcpy(dst, src, rank);
				dst -= stride;
				src -= stride;
			}
		}
		else if (_dst < _src)
		{
			for (; count > 0; --count) 
			{
				memcpy(dst, src, rank);
				dst += stride;
				src += stride;
			}
		}
	}

	class LBCore
	{
	public:
		LBCore(int ndim, int nvel, int rank = 1);
		virtual ~LBCore(void);		

	protected:
		virtual void stream(void) = 0;
		void initCoreBuffer(void);

		int nVel;
		int nDim;
		int *nSize;
		int nRank;
		int nNode;

		// Core buffers
		real** f_;
		
	};

}

#endif

