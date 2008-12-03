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

#ifndef OPAL_VEC3R_H
#define OPAL_VEC3R_H

// project headers
#include "OpalMath.h"
#include "Portability.h"

// system headers
#include <iostream>

namespace opal
{
    class Vec3r;
    inline Vec3r operator+(const Vec3r &u, const Vec3r &v);
    inline Vec3r operator-(const Vec3r &u, const Vec3r &v);
    inline Vec3r operator*(const Vec3r &v, real scalar);
    inline Vec3r operator*(real scalar, const Vec3r &v);
    inline Vec3r operator/(const Vec3r &v, real scalar);
    inline Vec3r operator%(const Vec3r & a, const Vec3r & b);
    inline Vec3r operator-(const Vec3r &v);
    inline real dot(const Vec3r & u, const Vec3r & v);
    inline Vec3r cross(const Vec3r & u, const Vec3r& v);

    /// Returns the projection of 'v' onto 'u'.
    inline Vec3r project(const Vec3r& u, const Vec3r& v);

    /// Given that the vector 'u' is already a unit vector, returns the
    /// projection of v onto u.
    inline Vec3r projectPreNorm(const Vec3r& u, const Vec3r& v);

    /// Returns a positive angle between 0 and 180 degrees.
    inline real angleBetween(const Vec3r& u, const Vec3r& v);

    /// Given that the vectors 'u' and 'v' are already normalized, returns
    /// a positive angle between 0 and 180 degrees.
    inline real angleBetweenPreNorm(const Vec3r& u, const Vec3r& v);

    /// Returns true if the two vectors are roughly collinear.
    inline bool areCollinear(const Vec3r& u, const Vec3r& v);

    /// Prints the Vec3r to the given output stream.
    inline std::ostream& operator<<(std::ostream& o, const Vec3r& v); 

    class Vec3r
    {
        public:
            /// x-coordinate
            real x;

            /// y-coordinate
            real y;

            /// z-coordinate
            real z;

            /// default - (0,0,0)
            OPAL_DECL Vec3r();

            OPAL_DECL Vec3r(const Vec3r & src);

            OPAL_DECL Vec3r(real xx, real yy, real zz);

            OPAL_DECL Vec3r(const real * data);

            OPAL_DECL virtual void OPAL_CALL set(real xx, real yy, real zz);

            OPAL_DECL virtual void OPAL_CALL set(real * data);

            OPAL_DECL virtual real & OPAL_CALL operator[] (unsigned int i);

            OPAL_DECL virtual const real & OPAL_CALL operator[] (unsigned int i) const;

            OPAL_DECL virtual real OPAL_CALL lengthSquared() const;

            OPAL_DECL virtual real OPAL_CALL length() const;

            OPAL_DECL virtual void OPAL_CALL normalize();

            OPAL_DECL virtual Vec3r OPAL_CALL unit() const;

            OPAL_DECL virtual void OPAL_CALL operator+=(const Vec3r & v);

            OPAL_DECL virtual void OPAL_CALL operator-=(const Vec3r & v);

            OPAL_DECL virtual void OPAL_CALL operator*=(const Vec3r & v);

            OPAL_DECL virtual void OPAL_CALL operator*=(real scalar);

            OPAL_DECL virtual void OPAL_CALL operator/=(real scalar);

            OPAL_DECL virtual void OPAL_CALL operator/=(const Vec3r & v);

            OPAL_DECL virtual bool OPAL_CALL operator==(const Vec3r & v) const;

            OPAL_DECL virtual bool OPAL_CALL operator!=(const Vec3r & v) const;

            OPAL_DECL virtual void OPAL_CALL operator=(const Vec3r & v);
    };

    inline Vec3r operator+(const Vec3r &u, const Vec3r &v)
    {
        return Vec3r(u.x + v.x, u.y + v.y, u.z + v.z);
    }

    inline Vec3r operator-(const Vec3r &u, const Vec3r &v)
    {
        return Vec3r(u.x - v.x, u.y - v.y, u.z - v.z);
    }

    inline Vec3r operator*(const Vec3r &v, real scalar)
    {
        return Vec3r(scalar * v.x, scalar * v.y, scalar * v.z);
    }

    inline Vec3r operator*(real scalar, const Vec3r &v)
    {
        return Vec3r(scalar * v.x, scalar * v.y, scalar * v.z);
    }

    inline Vec3r operator/(const Vec3r &v, real scalar)
    {
        return Vec3r(v.x / scalar, v.y / scalar, v.z / scalar);
    }

    inline Vec3r operator%(const Vec3r & a, const Vec3r & b)
    {
        return Vec3r((a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z),
                      (a.x * b.y) - (a.y * b.x));
    }

    inline real dot(const Vec3r & u, const Vec3r & v)
    {
        return u.x * v.x + u.y * v.y + u.z * v.z;
    }

    inline Vec3r cross(const Vec3r & u, const Vec3r & v)
    {
        return u % v;
    }

    inline Vec3r operator-(const Vec3r &v)
    {
        return v * -1;
    }

    inline Vec3r project(const Vec3r& u, const Vec3r& v)
    {
        Vec3r u2 = u;
        u2.normalize();
        return projectPreNorm(u2, v);
    }

    inline Vec3r projectPreNorm(const Vec3r& u, const Vec3r& v)
    {
        return dot(u, v) * u;
    }

    inline real angleBetween(const Vec3r& u, const Vec3r& v)
    {
        Vec3r u2 = u;
        u2.normalize();
        Vec3r v2 = v;
        v2.normalize();
        return angleBetweenPreNorm(u2, v2);
    }

    inline real angleBetweenPreNorm(const Vec3r& u, const Vec3r& v)
    {
        real val = dot(u, v);

        // Clamp to avoid rounding errors; acos will puke with values outside
        // this range.
        if (val < -1)
        {
            val = -1;
        }
        else if (val > 1)
        {
            val = 1;
        }

        return radToDeg(acos(val));
    }

    inline std::ostream& operator<<(std::ostream& o, const Vec3r& v)
    {
        return o << "[" << v.x << " " << v.y << " " << v.z << "]";
    }	

    inline bool areCollinear(const Vec3r & u, const Vec3r & v)
    {
        Vec3r a = u, b = v;
        a.normalize();
        b.normalize();

        if (areEqual(a.x, b.x) && areEqual(a.y, b.y) && areEqual(a.z, b.z))
            return true;
        if (areEqual(a.x, -b.x) && areEqual(a.y, -b.y) && areEqual(a.z, -b.z))
            return true;

        return false;
    }
}

#endif
