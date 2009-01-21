#ifndef LBGK2D_H
#define LBGK2D_H

#ifdef LB_SINGLE
typedef float real_t;
#else
typedef double real_t;
#endif

namespace TINY_LB 
{
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

	inline real_t sqr(real_t x)
	{
		return x * x;
	}

	class LBGK2D //: public LB2D
	{
	public:
		LBGK2D(int n);
		virtual ~LBGK2D();

		virtual const real_t* getData() const;
		virtual void init(void (*loader)(int x, int y, real_t& rho, real_t& ux, real_t& uy));	
		virtual void step();

		void setTau(const real_t t){tau = t;}
		real_t getTau(void){return tau;}

	protected:
		virtual void collide();
		virtual void stream();
		inline virtual real_t feq(const int i, real_t rho, real_t ux, real_t uy);

	private:
		static const int Q = 9;
		static const int DIM = 2;

		int offsets_[Q];

		real_t* f_;
		real_t* accumBuffer_;

		int n_;

		real_t tau, OMEGA;
	};

}

#endif
