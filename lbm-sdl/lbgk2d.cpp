#include "lbgk2d.h"

namespace TINY_LB
{
	LBGK2D::LBGK2D(int n)
	: n_(n), tau(2.0f)
	{
		OMEGA = 1./tau;
	}

	LBGK2D::~LBGK2D()
	{
		delete[] f_;
		delete[] accumBuffer_;
	}

	void LBGK2D::init(void (*loader)(int x, int y, real_t& rho,
				real_t& ux, real_t& uy)) 
	{
		f_ = new real_t[n_ * n_ * Q];
		accumBuffer_ = new real_t[(DIM + 1) * n_ * n_];

		for (int i = 0; i < Q; i++)
			for (int x = 0; x < n_; x++)
				for (int y = 0; y < n_; y++)
				{
					real_t rho, ux, uy, uSqr;
					loader(x, y, rho, ux, uy);
					uSqr = ux * ux + uy * uy;
					f_[x + y * n_ + i * n_ * n_] = feq(i, rho, ux, uy);
					accumBuffer_[3 * (x + n_ * y)] = rho;
					accumBuffer_[3 * (x + n_ * y) + 1] = ux;
					accumBuffer_[3 * (x + n_ * y) + 2] = uy;
				};

		for (int i = 0; i < Q; i++)
			offsets_[i] = 0;
	}

	inline real_t LBGK2D::feq(const int i, real_t rho, real_t ux, real_t uy)
	{
		real_t uSqr = sqr(ux) + sqr(uy);
		real_t dotProd = ux * e[i][0] + uy * e[i][1];
		real_t eqF = w[i] * rho *
			(1.0f + 3.0f * dotProd + 4.5f * dotProd * dotProd - 1.5f * uSqr);
		return eqF;
	}

	void LBGK2D::collide()
	{
		real_t* fCursor[Q];
		real_t* fCursorBounds[Q];
		int blockSize = n_ * n_;

		for (int i = 0; i < Q; i++)
		{
			fCursor[i] = f_ + blockSize * i + offsets_[i];
			fCursorBounds[i] = f_ + blockSize * (i + 1);
		}

		for (int i = 0; i < blockSize; i++)
		{
			real_t rho = *fCursor[0];
			rho += *fCursor[1];

			real_t ux = e[1][0] * (*fCursor[1]);
			real_t uy = e[1][1] * (*fCursor[1]);

			for (int j = 2; j < Q; j++)
			{
				rho += *fCursor[j];
				ux += e[j][0] * (*fCursor[j]);
				uy += e[j][1] * (*fCursor[j]);
			}

			ux /= rho;
			uy /= rho;

			for (int j = 0; j < Q; j++)
				*fCursor[j] = *fCursor[j] * (1 - OMEGA) + 
					OMEGA * feq(j, rho, ux, uy);

			accumBuffer_[3 * i] = rho;
			accumBuffer_[3 * i + 1] = ux;
			accumBuffer_[3 * i + 2] = uy;

			for (int j = 0; j < Q; j++)
				if (++fCursor[j] == fCursorBounds[j])
					fCursor[j] -= blockSize;
		}
	}

	void LBGK2D::stream()
	{
		const int blockSize = n_ * n_;

		for (int i = 0; i < Q; i++)
		{
			offsets_[i] -= e[i][0] + n_ * e[i][1];
			offsets_[i] %= blockSize;
			if (offsets_[i] < 0)
				offsets_[i] += blockSize;
		}
	}

	void LBGK2D::step()
	{
		collide();
		stream();
	}

	const real_t* LBGK2D::getData() const
	{	
		return accumBuffer_;	
	}

}
