#ifndef ONE_PASS_LATTICE2D_H
#define ONE_PASS_LATTICE2D_H

#include "lb2d.h"

namespace TINY_LB {

	class LBGK2D : public LB2D
	{
	public:
		LBGK2D(int n, 
			void (*loader)(int x, int y, real_t& rho, real_t& ux, real_t& uy));
		virtual ~LBGK2D();

		virtual const real_t* getData() const;
		virtual void step();

	protected:
		static const int Q = 9;
		static const int DIM = 2;

		int offsets_[Q];

		real_t* f_;
		real_t* accumBuffer_;

		int n_;

		virtual void collide();
		virtual void stream();
	};

}

#endif
