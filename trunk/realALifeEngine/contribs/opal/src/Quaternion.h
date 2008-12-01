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

#ifndef OPAL_QUATERNION_H
#define OPAL_QUATERNION_H

// project headers
#include "OpalMath.h"
#include "Portability.h"

// system headers
#include <iostream>

namespace opal
{
	class Vec3r;
	
    class Quaternion
    {
        public:
            real w;
            real x;
            real y;
            real z;

            OPAL_DECL Quaternion();

            OPAL_DECL Quaternion(real ww, real xx, real yy, real zz);

            OPAL_DECL Quaternion(const Quaternion& src);

            OPAL_DECL virtual void OPAL_CALL set(real ww, real xx, real yy, real zz);

            OPAL_DECL virtual real OPAL_CALL lengthSquared() const;

            OPAL_DECL virtual real OPAL_CALL length() const;

            //! Length will be one.
            OPAL_DECL virtual void OPAL_CALL normalize();

            //! Returns the equivalent rotation.
            /*!
             * @param angle in degrees
             * @param axis axis of rotation
             *
             * @note Quaternion needs to be normalized first
             */
            OPAL_DECL virtual void OPAL_CALL getAngleAxis(real & angle, Vec3r & axis) const;

            OPAL_DECL virtual real OPAL_CALL getRoll() const;

            OPAL_DECL virtual real OPAL_CALL getPitch() const;

            OPAL_DECL virtual real OPAL_CALL getYaw() const;

            OPAL_DECL virtual real & OPAL_CALL operator[] (unsigned int i);

            OPAL_DECL virtual const real & OPAL_CALL operator[] (unsigned int i) const;
    };

    inline Quaternion operator+(const Quaternion &q1, const Quaternion &q2)
    {
        return Quaternion(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z);
    }

    inline Quaternion operator-(const Quaternion &q1, const Quaternion &q2)
    {
        return Quaternion(q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z);
    }

    inline bool operator==(const Quaternion &q1, const Quaternion &q2)
    {
        if (!areEqual(q1.w, q2.w) || !areEqual(q1.x , q2.x) ||
                !areEqual(q1.y , q2.y) || !areEqual(q1.z , q2.z))
            return false;
        else
            return true;
    }

    inline bool operator!=(const Quaternion &q1, const Quaternion &q2)
    {
        if (!areEqual(q1.w, q2.w) || !areEqual(q1.x , q2.x) ||
                !areEqual(q1.y , q2.y) || !areEqual(q1.z , q2.z))
            return true;
        else
            return false;
    }

    inline Quaternion operator*(const Quaternion& q, real scalar)
    {
        return Quaternion(scalar * q.w, scalar * q.x, scalar * q.y, scalar * q.z);
    }

    inline Quaternion operator*(real scalar, const Quaternion& q)
    {
        return Quaternion(scalar * q.w, scalar * q.x, scalar * q.y, scalar * q.z);
    }

	/// Prints the Quaternion to the given output stream.
    inline std::ostream& operator<<(std::ostream& o, const Quaternion& q)
    {
        return o << "[" << q.w << " " << q.x << " " << q.y << " " << q.z << "]";
    }
}

#endif
