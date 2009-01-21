#include "lbgk2d.h"

namespace TINY_LB
{
	const real_t OMEGA = 0.5f;

	inline real_t sqr(real_t x)
	{
		return x * x;
	}

	inline real_t getFEq(real_t rho, real_t ux, real_t uy, int i)
	{
		real_t uSqr = sqr(ux) + sqr(uy);
		real_t dotProd = ux * e[i][0] + uy * e[i][1];
		real_t eqF = w[i] * rho *
			(1.0f + 3.0f * dotProd + 4.5f * dotProd * dotProd - 1.5f * uSqr);
		return eqF;
	}

	LBGK2D::LBGK2D(int n,
				void (*loader)(int x, int y, real_t& rho,
				real_t& ux, real_t& uy)) 
	: n_(n)
	{
		f_ = new real_t[n * n * Q];
		accumBuffer_ = new real_t[(DIM + 1) * n * n];

		for (int i = 0; i < Q; i++)
			for (int x = 0; x < n; x++)
				for (int y = 0; y < n; y++)
				{
					real_t rho, ux, uy, uSqr;
					loader(x, y, rho, ux, uy);
					uSqr = ux * ux + uy * uy;
					f_[x + y * n + i * n * n] = getFEq(rho, ux, uy, i);
					accumBuffer_[3 * (x + n * y)] = rho;
					accumBuffer_[3 * (x + n * y) + 1] = ux;
					accumBuffer_[3 * (x + n * y) + 2] = uy;
				};

		for (int i = 0; i < Q; i++)
			offsets_[i] = 0;
	}

	LBGK2D::~LBGK2D()
	{
		delete[] f_;
		delete[] accumBuffer_;
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
					OMEGA * getFEq(rho, ux, uy, j);

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
