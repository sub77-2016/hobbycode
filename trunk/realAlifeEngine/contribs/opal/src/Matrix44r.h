/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
*                                                                       *
* This library is free software; you can redistribute it and/or         *
* modify it under the terms of EITHER:                                  *
*   (1) The GNU Lesser General Public License as published by the Free  *
*       Software Foundation; either version 2.1 of the License, or (at  *
*       your option) any later version. The text of the GNU Lesser      *
*       General Public License is included with this library in the     *
*       file license-LGPL.txt.                                          *
*   (2) The BSD-style license that is included with this library in     *
*       the file license-BSD.txt.                                       *
*                                                                       *
* This library is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
* license-LGPL.txt and license-BSD.txt for more details.                *
************************************************************************/

#ifndef OPAL_MATRIX44R_H
#define OPAL_MATRIX44R_H

// project headers
#include "Quaternion.h"
#include "OpalMath.h"
#include "Point3r.h"
#include "Rayr.h"
#include "Vec3r.h"

namespace opal
{
	// Internally the matrix is column major order (openGL)
	// the accessors are (row, col), but the set(16 x real) function
	// is column, row order because then it is easier to write
	// the 16 real out.

	class Matrix44r;
	inline Matrix44r operator*(const Matrix44r & lhs, const Matrix44r & rhs);
	inline Vec3r operator*(const Matrix44r & m, const Vec3r &v);
	inline Matrix44r operator+(const Matrix44r & lhs, const Matrix44r & rhs);
	inline Matrix44r operator-(const Matrix44r & lhs, const Matrix44r & rhs);
	inline Point3r operator*(const Matrix44r & m, const Point3r &p);
	inline Rayr operator*(const Matrix44r & m, const Rayr &r);

	/// Full matrix inversion.  Assumes the matrix is invertible.
	inline void inverse(Matrix44r& result, const Matrix44r& source);

	/// Fast inverse function for affine transformations.  Assumes the 
	/// matrix is invertible.
	inline void inverseAffine(Matrix44r& result, const Matrix44r& source);

	/// Prints the Matrix44r to the given output stream.
	inline std::ostream& operator<<(std::ostream& o, const Matrix44r& m);

	//! Generic 4 by 4 matrix
	/*!
	 * Such matrix describes translation, rotation and scaling.
	 */
	class Matrix44r
	{
		private:
			real mData[16];

		public:
			inline friend Matrix44r operator*(real scalar, Matrix44r m);

			//! Identity by default
			Matrix44r()
			{
				makeIdentity();
			}

			//! Copy-constructor
			Matrix44r(const Matrix44r & src)
			{
				memcpy(mData, src.mData, 16 * sizeof(real));
			}

			//! set is row-column order
			Matrix44r(real _00, real _10, real _20, real _30,
			           real _01, real _11, real _21, real _31,
			           real _02, real _12, real _22, real _32,
			           real _03, real _13, real _23, real _33)
			{
				mData[0] = _00;
				mData[1] = _01;
				mData[2] = _02;
				mData[3] = _03;

				mData[4] = _10;
				mData[5] = _11;
				mData[6] = _12;
				mData[7] = _13;

				mData[8] = _20;
				mData[9] = _21;
				mData[10] = _22;
				mData[11] = _23;

				mData[12] = _30;
				mData[13] = _31;
				mData[14] = _32;
				mData[15] = _33;
			}

			//! WARNING! this takes in a column-row order matrix
			Matrix44r(const real * data)
			{
				memcpy(mData, data, 16 * sizeof(real));
			}

			//! Resets position.
			inline void setPosition(real x, real y, real z)
			{
				mData[12] = x;
				mData[13] = y;
				mData[14] = z;
			}

			//! Returns current position.
			inline Point3r getPosition() const
			{
				return Point3r(mData[12], mData[13], mData[14]);
			}

			//! Returns current position.
			inline Vec3r getTranslation() const
			{
				return Vec3r(mData[12], mData[13], mData[14]);
			}

			//! set is row-column order
			inline void set(real _00, real _10, real _20, real _30,
				                 real _01, real _11, real _21, real _31,
				                 real _02, real _12, real _22, real _32,
				                 real _03, real _13, real _23, real _33)
			{
				mData[0] = _00;
				mData[1] = _01;
				mData[2] = _02;
				mData[3] = _03;

				mData[4] = _10;
				mData[5] = _11;
				mData[6] = _12;
				mData[7] = _13;

				mData[8] = _20;
				mData[9] = _21;
				mData[10] = _22;
				mData[11] = _23;

				mData[12] = _30;
				mData[13] = _31;
				mData[14] = _32;
				mData[15] = _33;
			}

			//! WARNING! this takes in a column-row order matrix
			inline void set(const real * data)
			{
				memcpy(mData, data, 16 * sizeof(real));
			}

			//! Entire matrix will be zero'ed.
			inline void makeZero()
			{
				memset(mData, 0, 16 * sizeof(real));
			}

			//! Resets to indentity matrix.
			inline void makeIdentity()
			{
				mData[0] = 1;
				mData[1] = 0;
				mData[2] = 0;
				mData[3] = 0;

				mData[4] = 0;
				mData[5] = 1;
				mData[6] = 0;
				mData[7] = 0;

				mData[8] = 0;
				mData[9] = 0;
				mData[10] = 1;
				mData[11] = 0;

				mData[12] = 0;
				mData[13] = 0;
				mData[14] = 0;
				mData[15] = 1;
			}

			/// Creates a scaling transformation matrix.
			inline void makeScale(real s)
			{
				set(s, 0, 0, 0, 0, s, 0, 0, 0, 0, s, 0, 0, 0, 0, 1);
			}

			/// Creates a scaling transformation matrix.
			inline void makeScale(real x, real y, real z)
			{
				set(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
			}

			//! sets rotation of the matrix
			/*!
			 * @param degrees angle of rotation
			 * @param x x-size of the axis vector
			 * @param y y-size of the axis vector
			 * @param z z-size of the axis vector
			 */
			inline void makeRotation(real degrees, real x, real y, real z)
			{
				Vec3r axis(x, y, z);
				axis.normalize();
				x = axis[0];
				y = axis[1];
				z = axis[2];

				// Source: 
				// http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix/
				real radians = degToRad(degrees);
				real c = cos(radians);
				real s = sin(radians);
				real t = (real) 1.0 - c;

				(*this)(0, 0) = c + x * x * t;
				(*this)(1, 1) = c + y * y * t;
				(*this)(2, 2) = c + z * z * t;

				real tmp1 = x * y * t;
				real tmp2 = z * s;
				(*this)(1, 0) = tmp1 + tmp2;
				(*this)(0, 1) = tmp1 - tmp2;

				tmp1 = x * z * t;
				tmp2 = y * s;
				(*this)(2, 0) = tmp1 - tmp2;
				(*this)(0, 2) = tmp1 + tmp2;

				tmp1 = y * z * t;
				tmp2 = x * s;
				(*this)(2, 1) = tmp1 + tmp2;
				(*this)(1, 2) = tmp1 - tmp2;

				(*this)(0, 3) = 0;
				(*this)(1, 3) = 0;
				(*this)(2, 3) = 0;

				(*this)(3, 0) = 0;
				(*this)(3, 1) = 0;
				(*this)(3, 2) = 0;
				(*this)(3, 3) = 1;
			}

			/// x, y, z - axis can be zero
			inline void setQuaternion(real w, real x, real y, real z)
			{
				real angle;
				Vec3r axis;
				Quaternion q(w, x, y, z);
				q.getAngleAxis(angle, axis);

				setRotation(angle, axis.x, axis.y, axis.z);
			}

			/// quaternion can have a zero axis
			inline void setQuaternion(const Quaternion & q)
			{
				real angle;
				Vec3r axis;
				q.getAngleAxis(angle, axis);

				setRotation(angle, axis.x, axis.y, axis.z);
			}

			//! overwrites previous rotations of the matrix
			/*!
			 * @param degrees angle of rotation
			 * @param x x-size of the axis vector
			 * @param y y-size of the axis vector
			 * @param z z-size of the axis vector
			 */
			inline void setRotation(real degrees, real x, real y, real z)
			{
				opal::Point3r p = getPosition();
				makeRotation(degrees, x, y, z);
				(*this)(0, 3) = p[0];
				(*this)(1, 3) = p[1];
				(*this)(2, 3) = p[2];
			}

			inline void makeTranslation(real x, real y, real z)
			{
				set(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1);
			}

			//! post multiply transformations, openGL style, for convenience
			inline void scale(real s)
			{
				Matrix44r m(s, 0, 0, 0, 0, s, 0, 0, 0, 0, s, 0, 0, 0, 0, 1);
				(*this) = (*this) * m;
			}

			inline void scale(real x, real y, real z)
			{
				Matrix44r m(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
				(*this) = (*this) * m;
			}

			/// Returns a quaternion representation.
			inline Quaternion getQuaternion() const
			{
				// Source: 
				// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

				Quaternion q;
				real trace = (*this)(0, 0) + (*this)(1, 1) + (*this)(2, 2) + 
					(real)1.0;

				if (!areEqual(trace, 0))
				{
					real s = (real)0.5 / sqrt(trace);
					q[0] = (real)0.25 / s;
					q[1] = ((*this)(2, 1) - (*this)(1, 2)) * s;
					q[2] = ((*this)(0, 2) - (*this)(2, 0)) * s;
					q[3] = ((*this)(1, 0) - (*this)(0, 1)) * s;
				}
				else
				{
					if ((*this)(0, 0) > (*this)(1, 1) && (*this)(0, 0) > 
						(*this)(2, 2))
					{
						real s = 2 * sqrt((real)1.0 + (*this)(0, 0) - 
							(*this)(1, 1) - (*this)(2, 2));
						q[1] = (real)0.25 * s;
						q[2] = ((*this)(0, 1) + (*this)(1, 0)) / s;
						q[3] = ((*this)(0, 2) + (*this)(2, 0)) / s;
						q[0] = ((*this)(1, 2) - (*this)(2, 1)) / s;
					}
					else if ((*this)(1, 1) > (*this)(2, 2))
					{
						real s = 2 * sqrt((real)1.0 + (*this)(1, 1) -
							(*this)(0, 0) - (*this)(2, 2));
						q[1] = ((*this)(0, 1) + (*this)(1, 0)) / s;
						q[2] = (real)0.25 * s;
						q[3] = ((*this)(1, 2) + (*this)(2, 1)) / s;
						q[0] = ((*this)(0, 2) - (*this)(2, 0)) / s;
					}
					else
					{
						real s = 2 * sqrt((real)1.0 + (*this)(2, 2) -
							(*this)(0, 0) - (*this)(1, 1));
						q[1] = ((*this)(0, 2) + (*this)(2, 0)) / s;
						q[2] = ((*this)(1, 2) + (*this)(2, 1)) / s;
						q[3] = (real)0.25 * s;
						q[0] = ((*this)(0, 1) - (*this)(1, 0)) / s;
					}
				}

				return q;
			}

			//! Returns rotations around x-, y-, and z- axis.
			/*!
			 * euler angles are in degrees
			 */
			inline Vec3r getEulerXYZ() const
			{
				Vec3r angles;

				angles[1] = asin((*this)(0, 2));
				if (angles[1] < globals::OPAL_HALF_PI)
				{
					if (angles[1] > -globals::OPAL_HALF_PI)
					{
						angles[0] = atan2(-(*this)(1, 2), (*this)(2, 2));
						angles[2] = atan2(-(*this)(0, 1), (*this)(0, 0));
					}
					else
					{
						// This is not a unique solution.
						real value = atan2((*this)(1, 0), (*this)(1, 1));
						angles[2] = 0;
						angles[0] = angles[2] - value;
					}
				}
				else
				{
					// This is not a unique solution.
					real value = atan2((*this)(1, 0), (*this)(1, 1));
					angles[2] = 0;
					angles[0] = value - angles[2];
				}

				// convert to degrees
				angles[0] = radToDeg(angles[0]);
				angles[1] = radToDeg(angles[1]);
				angles[2] = radToDeg(angles[2]);

				// normalize to (-180,180]
				angles[0] = normalizeDegrees(angles[0]);
				angles[1] = normalizeDegrees(angles[1]);
				angles[2] = normalizeDegrees(angles[2]);

				return angles;
			}

			//! post multiply transformations, openGL style, for convenience
			inline void preScale(real s)
			{
				Matrix44r m(s, 0, 0, 0, 0, s, 0, 0, 0, 0, s, 0, 0, 0, 0, 1);
				(*this) = m * (*this);
			}

			inline void preScale(real x, real y, real z)
			{
				Matrix44r m(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
				(*this) = m * (*this);
			}

			//! rotates the matrix further
			/*!
			 * @param degrees angle of rotation
			 * @param x x-size of the axis vector
			 * @param y y-size of the axis vector
			 * @param z z-size of the axis vector
			 *
			 * @note this operation does not overwrite previous rotations
			 */
			inline void rotate(real degrees, real x, real y, real z)
			{
				Matrix44r m;
				m.makeRotation(degrees, x, y, z);
				(*this) = (*this) * m;
			}

			//! applies this rotation first, then applies old rotations again
			/*!
			 * @param degrees angle of rotation
			 * @param x x-size of the axis vector
			 * @param y y-size of the axis vector
			 * @param z z-size of the axis vector
			 *
			 * @note this operation affects the order of the rotations
			 */
			inline void preRotate(real degrees, real x, real y, real z)
			{
				Matrix44r m;
				m.makeRotation(degrees, x, y, z);
				(*this) = m * (*this);
			}

			inline void translate(real x, real y, real z)
			{
				Matrix44r m(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1);
				(*this) = (*this) * m;
			}

			inline void preTranslate(real x, real y, real z)
			{
				Matrix44r m(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1);
				(*this) = m * (*this);
			}

			inline real * getData()
			{
				return mData;
			}

			inline const real * getData() const
			{
				return mData;
			}

			inline real & operator[] (unsigned int i)
			{
				return mData[i];
			}

			inline const real & operator[] (unsigned int i) const
			{
				return mData[i];
			}

			inline real & operator() (unsigned int row, unsigned int col)
			{
				return mData[4 * col + row];
			}

			inline const real & operator() (unsigned int row, unsigned int col) const
			{
				return mData[4 * col + row];
			}

			inline void operator*=(const Matrix44r &m)
			{
				(*this) = (*this) * m;
			}

			inline bool operator==(const Matrix44r & m) const
			{
				for (int i = 0; i < 16; ++i)
				{
					if (mData[i] != m[i]) return false;
				}

				return true;
			}

			inline bool operator!=(const Matrix44r & m) const
			{
				for (int i = 0; i < 16; ++i)
				{
					if (mData[i] != m[i]) return true;
				}

				return false;
			}

			inline void swap(real & a, real & b)
			{
				real temp = a;
				a = b;
				b = temp;
			}

			/// Transposes the matrix in place.
			inline void transpose()
			{
				swap(mData[1], mData[4]);
				swap(mData[2], mData[8]);
				swap(mData[3], mData[12]);

				swap(mData[6], mData[9]);
				swap(mData[7], mData[13]);
				swap(mData[11], mData[14]);
			}

			/// Determines whether the matrix is affine.
			inline bool isAffine()const
			{
				if (0 == (*this)(3, 0) && 0 == (*this)(3, 1) && 
					0 == (*this)(3, 2) && 1 == (*this)(3, 3))
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			inline real determinant()const
			{
				// Source: 
				// http://www.euclideanspace.com/maths/algebra/matrix/code/index.htm

				real det = 
					(*this)(0, 3) * (*this)(1, 2) * (*this)(2, 1) * (*this)(3, 0) - 
					(*this)(0, 2) * (*this)(1, 3) * (*this)(2, 1) * (*this)(3, 0) - 
					(*this)(0, 3) * (*this)(1, 1) * (*this)(2, 2) * (*this)(3, 0) + 
					(*this)(0, 1) * (*this)(1, 3) * (*this)(2, 2) * (*this)(3, 0) + 

					(*this)(0, 2) * (*this)(1, 1) * (*this)(2, 3) * (*this)(3, 0) - 
					(*this)(0, 1) * (*this)(1, 2) * (*this)(2, 3) * (*this)(3, 0) - 
					(*this)(0, 3) * (*this)(1, 2) * (*this)(2, 0) * (*this)(3, 1) + 
					(*this)(0, 2) * (*this)(1, 3) * (*this)(2, 0) * (*this)(3, 1) + 

					(*this)(0, 3) * (*this)(1, 0) * (*this)(2, 2) * (*this)(3, 1) - 
					(*this)(0, 0) * (*this)(1, 3) * (*this)(2, 2) * (*this)(3, 1) - 
					(*this)(0, 2) * (*this)(1, 0) * (*this)(2, 3) * (*this)(3, 1) + 
					(*this)(0, 0) * (*this)(1, 2) * (*this)(2, 3) * (*this)(3, 1) + 

					(*this)(0, 3) * (*this)(1, 1) * (*this)(2, 0) * (*this)(3, 2) - 
					(*this)(0, 1) * (*this)(1, 3) * (*this)(2, 0) * (*this)(3, 2) - 
					(*this)(0, 3) * (*this)(1, 0) * (*this)(2, 1) * (*this)(3, 2) + 
					(*this)(0, 0) * (*this)(1, 3) * (*this)(2, 1) * (*this)(3, 2) + 

					(*this)(0, 1) * (*this)(1, 0) * (*this)(2, 3) * (*this)(3, 2) - 
					(*this)(0, 0) * (*this)(1, 1) * (*this)(2, 3) * (*this)(3, 2) - 
					(*this)(0, 2) * (*this)(1, 1) * (*this)(2, 0) * (*this)(3, 3) + 
					(*this)(0, 1) * (*this)(1, 2) * (*this)(2, 0) * (*this)(3, 3) + 

					(*this)(0, 2) * (*this)(1, 0) * (*this)(2, 1) * (*this)(3, 3) - 
					(*this)(0, 0) * (*this)(1, 2) * (*this)(2, 1) * (*this)(3, 3) - 
					(*this)(0, 1) * (*this)(1, 0) * (*this)(2, 2) * (*this)(3, 3) + 
					(*this)(0, 0) * (*this)(1, 1) * (*this)(2, 2) * (*this)(3, 3);

				return det;
			}

			inline real determinantAffine()const
			{
				// Source: 
				// http://www.euclideanspace.com/maths/algebra/matrix/code/sftranslation_cpp.htm

				real det = 
					(*this)(0, 0) * ((*this)(1, 1) * (*this)(2, 2) - (*this)(2, 1) * (*this)(1, 2)) - 
					(*this)(0, 1) * ((*this)(1, 0) * (*this)(2, 2) - (*this)(2, 0) * (*this)(1, 2)) + 
					(*this)(0, 2) * ((*this)(1, 0) * (*this)(2, 1) - (*this)(2, 0) * (*this)(1, 1));

				return det;
			}

			inline void invert()
			{
				inverse(*this, *this);
			}

			inline void invertAffine()
			{
				inverseAffine(*this, *this);
			}

			/// Returns the transform's forward direction in the global 
			/// coordinate system.
			inline Vec3r getForward() const
			{
				return (*this) * Vec3r(0, 0, -1);
			}

			/// Returns the transform's up direction in the global 
			/// coordinate system.
			inline Vec3r getUp() const
			{
				return (*this) * Vec3r(0, 1, 0);
			}

			/// Returns the transform's right direction in the global 
			/// coordinate system.
			inline Vec3r getRight() const
			{
				return (*this) * Vec3r(1, 0, 0);
			}
	};

	inline void inverse(Matrix44r& result, const Matrix44r& source)
	{
		// Adapted from: 
		// http://www.euclideanspace.com/maths/algebra/matrix/code/sftranslation_cpp.htm

		// In case &result == &source...
		Matrix44r src = source;

		real det = src.determinant();
		assert(!areEqual(det, 0));
		real detInv = 1 / det;

		result(0, 0) = (src(1, 2) * src(2, 3) * src(3, 1) - src(1, 3) * src(2, 2) * src(3, 1) 
			+ src(1, 3) * src(2, 1) * src(3, 2) - src(1, 1) * src(2, 3) * src(3, 2) 
			- src(1, 2) * src(2, 1) * src(3, 3) + src(1, 1) * src(2, 2) * src(3, 3)) * detInv;
		result(0, 1) = (src(0, 3) * src(2, 2) * src(3, 1) - src(0, 2) * src(2, 3) * src(3, 1) 
			- src(0, 3) * src(2, 1) * src(3, 2) + src(0, 1) * src(2, 3) * src(3, 2) 
			+ src(0, 2) * src(2, 1) * src(3, 3) - src(0, 1) * src(2, 2) * src(3, 3)) * detInv;
		result(0, 2) = (src(0, 2) * src(1, 3) * src(3, 1) - src(0, 3) * src(1, 2) * src(3, 1) 
			+ src(0, 3) * src(1, 1) * src(3, 2) - src(0, 1) * src(1, 3) * src(3, 2) 
			- src(0, 2) * src(1, 1) * src(3, 3) + src(0, 1) * src(1, 2) * src(3, 3)) * detInv;
		result(0, 3) = (src(0, 3) * src(1, 2) * src(2, 1) - src(0, 2) * src(1, 3) * src(2, 1) 
			- src(0, 3) * src(1, 1) * src(2, 2) + src(0, 1) * src(1, 3) * src(2, 2) 
			+ src(0, 2) * src(1, 1) * src(2, 3) - src(0, 1) * src(1, 2) * src(2, 3)) * detInv;
		result(1, 0) = (src(1, 3) * src(2, 2) * src(3, 0) - src(1, 2) * src(2, 3) * src(3, 0) 
			- src(1, 3) * src(2, 0) * src(3, 2) + src(1, 0) * src(2, 3) * src(3, 2) 
			+ src(1, 2) * src(2, 0) * src(3, 3) - src(1, 0) * src(2, 2) * src(3, 3)) * detInv;
		result(1, 1) = (src(0, 2) * src(2, 3) * src(3, 0) - src(0, 3) * src(2, 2) * src(3, 0) 
			+ src(0, 3) * src(2, 0) * src(3, 2) - src(0, 0) * src(2, 3) * src(3, 2) 
			- src(0, 2) * src(2, 0) * src(3, 3) + src(0, 0) * src(2, 2) * src(3, 3)) * detInv;
		result(1, 2) = (src(0, 3) * src(1, 2) * src(3, 0) - src(0, 2) * src(1, 3) * src(3, 0) 
			- src(0, 3) * src(1, 0) * src(3, 2) + src(0, 0) * src(1, 3) * src(3, 2) 
			+ src(0, 2) * src(1, 0) * src(3, 3) - src(0, 0) * src(1, 2) * src(3, 3)) * detInv;
		result(1, 3) = (src(0, 2) * src(1, 3) * src(2, 0) - src(0, 3) * src(1, 2) * src(2, 0) 
			+ src(0, 3) * src(1, 0) * src(2, 2) - src(0, 0) * src(1, 3) * src(2, 2) 
			- src(0, 2) * src(1, 0) * src(2, 3) + src(0, 0) * src(1, 2) * src(2, 3)) * detInv;
		result(2, 0) = (src(1, 1) * src(2, 3) * src(3, 0) - src(1, 3) * src(2, 1) * src(3, 0) 
			+ src(1, 3) * src(2, 0) * src(3, 1) - src(1, 0) * src(2, 3) * src(3, 1) 
			- src(1, 1) * src(2, 0) * src(3, 3) + src(1, 0) * src(2, 1) * src(3, 3)) * detInv;
		result(2, 1) = (src(0, 3) * src(2, 1) * src(3, 0) - src(0, 1) * src(2, 3) * src(3, 0) 
			- src(0, 3) * src(2, 0) * src(3, 1) + src(0, 0) * src(2, 3) * src(3, 1) 
			+ src(0, 1) * src(2, 0) * src(3, 3) - src(0, 0) * src(2, 1) * src(3, 3)) * detInv;
		result(2, 2) = (src(0, 1) * src(1, 3) * src(3, 0) - src(0, 3) * src(1, 1) * src(3, 0) 
			+ src(0, 3) * src(1, 0) * src(3, 1) - src(0, 0) * src(1, 3) * src(3, 1) 
			- src(0, 1) * src(1, 0) * src(3, 3) + src(0, 0) * src(1, 1) * src(3, 3)) * detInv;
		result(2, 3) = (src(0, 3) * src(1, 1) * src(2, 0) - src(0, 1) * src(1, 3) * src(2, 0) 
			- src(0, 3) * src(1, 0) * src(2, 1) + src(0, 0) * src(1, 3) * src(2, 1) 
			+ src(0, 1) * src(1, 0) * src(2, 3) - src(0, 0) * src(1, 1) * src(2, 3)) * detInv;
		result(3, 0) = (src(1, 2) * src(2, 1) * src(3, 0) - src(1, 1) * src(2, 2) * src(3, 0) 
			- src(1, 2) * src(2, 0) * src(3, 1) + src(1, 0) * src(2, 2) * src(3, 1) 
			+ src(1, 1) * src(2, 0) * src(3, 2) - src(1, 0) * src(2, 1) * src(3, 2)) * detInv;
		result(3, 1) = (src(0, 1) * src(2, 2) * src(3, 0) - src(0, 2) * src(2, 1) * src(3, 0) 
			+ src(0, 2) * src(2, 0) * src(3, 1) - src(0, 0) * src(2, 2) * src(3, 1) 
			- src(0, 1) * src(2, 0) * src(3, 2) + src(0, 0) * src(2, 1) * src(3, 2)) * detInv;
		result(3, 2) = (src(0, 2) * src(1, 1) * src(3, 0) - src(0, 1) * src(1, 2) * src(3, 0) 
			- src(0, 2) * src(1, 0) * src(3, 1) + src(0, 0) * src(1, 2) * src(3, 1) 
			+ src(0, 1) * src(1, 0) * src(3, 2) - src(0, 0) * src(1, 1) * src(3, 2)) * detInv;
		result(3, 3) = (src(0, 1) * src(1, 2) * src(2, 0) - src(0, 2) * src(1, 1) * src(2, 0) 
			+ src(0, 2) * src(1, 0) * src(2, 1) - src(0, 0) * src(1, 2) * src(2, 1) 
			- src(0, 1) * src(1, 0) * src(2, 2) + src(0, 0) * src(1, 1) * src(2, 2)) * detInv;
	}

	inline void inverseAffine(Matrix44r& result, const Matrix44r& source)
	{
		// Adapted from: 
		// http://www.euclideanspace.com/maths/algebra/matrix/code/sftranslation_cpp.htm

		assert(source.isAffine());

		// In case &result == &source...
		Matrix44r src = source;

		real det = src.determinantAffine();
		assert(!areEqual(det, 0));
		real detInv = 1 / det;

		result(0, 0) = (src(1, 1) * src(2, 2) - src(1, 2) * src(2, 1)) * detInv;
		result(0, 1) = (src(0, 2) * src(2, 1) - src(0, 1) * src(2, 2)) * detInv;
		result(0, 2) = (src(0, 1) * src(1, 2) - src(0, 2) * src(1, 1)) * detInv;
		result(1, 0) = (src(1, 2) * src(2, 0) - src(1, 0) * src(2, 2)) * detInv;
		result(1, 1) = (src(0, 0) * src(2, 2) - src(0, 2) * src(2, 0)) * detInv;
		result(1, 2) = (src(0, 2) * src(1, 0) - src(0, 0) * src(1, 2)) * detInv;
		result(2, 0) = (src(1, 0) * src(2, 1) - src(1, 1) * src(2, 0)) * detInv;
		result(2, 1) = (src(0, 1) * src(2, 0) - src(0, 0) * src(2, 1)) * detInv;
		result(2, 2) = (src(0, 0) * src(1, 1) - src(0, 1) * src(1, 0)) * detInv;
		result(0, 3) *= -1;
		result(1, 3) *= -1;
		result(2, 3) *= -1;
	}

	inline Matrix44r operator*(real scalar, Matrix44r m)
	{
		for (unsigned int i = 0; i < 16; ++i)
		{
			m.mData[i] *= scalar;
		}

		return m;
	}

	inline Matrix44r operator*(const Matrix44r & lhs, const Matrix44r & rhs)
	{
		Matrix44r res; // prevent aliasing
		res.makeZero();

		// source: p. 150 Numerical Analysis (second ed.)
		// if A is m x p, and B is p x n, then AB is m x n
		// (AB)ij = [k = 1 to p] (a)ik (b)kj (where:  1 <= i <= m, 1 <= j <= n)
		for (unsigned int i = 0; i < 4; ++i)     	// 1 <= i <= m
			for (unsigned int j = 0; j < 4; ++j)     	// 1 <= j <= n
				for (unsigned int k = 0; k < 4; ++k)     	// [k = 1 to p]
					res(i, j) += lhs(i, k) * rhs(k, j);
		return res;
	}

	inline Vec3r operator*(const Matrix44r & m, const Vec3r &v)
	{
		return Vec3r(m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2],
		              m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2],
		              m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2]);
	}

	inline Point3r operator*(const Matrix44r & m, const Point3r &p)
	{
		return Point3r(
		           m(0, 0) * p[0] + m(0, 1) * p[1] + m(0, 2) * p[2] + m(0, 3),
		           m(1, 0) * p[0] + m(1, 1) * p[1] + m(1, 2) * p[2] + m(1, 3),
		           m(2, 0) * p[0] + m(2, 1) * p[1] + m(2, 2) * p[2] + m(2, 3));
	}

	inline Matrix44r operator+(const Matrix44r & lhs, const Matrix44r & rhs)
	{
		Matrix44r res;
		for (unsigned int i = 0; i < 16; ++i)
		{
			res[i] = lhs[i] + rhs[i];
		}
		return res;
	}

	inline Matrix44r operator-(const Matrix44r & lhs, const Matrix44r & rhs)
	{
		Matrix44r res;
		for (unsigned int i = 0; i < 16; ++i)
		{
			res[i] = lhs[i] - rhs[i];
		}
		return res;
	}

	inline Rayr operator*(const Matrix44r & m, const Rayr & r)
	{
		Rayr ray(m * r.getOrigin(), m * r.getDir());
		return ray;
	}

	inline std::ostream& operator<<(std::ostream& o, const Matrix44r& m)
	{
		return o
		       << "[" << m[0] << " " << m[4] << " " << m[8] << " " << m[12]
		       << "]" << std::endl
		       << "[" << m[1] << " " << m[5] << " " << m[9] << " " << m[13]
		       << "]" << std::endl
		       << "[" << m[2] << " " << m[6] << " " << m[10] << " " << m[14]
		       << "]" << std::endl
		       << "[" << m[3] << " " << m[7] << " " << m[11] << " " << m[15]
		       << "]";
	}
}

#endif
