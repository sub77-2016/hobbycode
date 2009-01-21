#ifndef LB2D_H
#define LB2D_H

#ifdef LB_SINGLE
typedef float real_t;
#else
typedef double real_t;
#endif

namespace TINY_LB {

	const int e[][2] =
	{
		{ 0,  0},
		{ 1,  0},
		{ 1,  1},
		{ 0,  1},
		{-1,  1},
		{-1,  0},
		{-1, -1},
		{ 0, -1},
		{ 1, -1},
	};

	const real_t w[] =
	{
		4.0f / 9.0f,
		1.0f / 9.0f,  // E
		1.0f / 36.0f, // NE
		1.0f / 9.0f,  // N
		1.0f / 36.0f, // NW
		1.0f / 9.0f,  // W
		1.0f / 36.0f, // SW
		1.0f / 9.0f,  // S
		1.0f / 36.0f, // SE
	};

	class LB2D
	{
	public:
		virtual const real_t* getData() const = 0;
		virtual void step() = 0;
		virtual void collide() = 0;
		virtual void stream() = 0;
	};

}

#endif
