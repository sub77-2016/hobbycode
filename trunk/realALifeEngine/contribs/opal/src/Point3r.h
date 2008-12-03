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

#ifndef OPAL_POINT3R_H
#define OPAL_POINT3R_H

// project headers
#include "OpalMath.h"
#include "Vec3r.h"

// systeam headers
#include <cassert>

namespace opal
{
    class Point3r;
    inline Point3r operator+(const Point3r &u, const Vec3r &v);
    inline Point3r operator-(const Point3r &u, const Vec3r &v);
    inline Vec3r operator-(const Point3r &u, const Point3r &v);
    inline Point3r operator*(const Point3r &v, real scalar);
    inline Point3r operator*(real scalar, const Point3r &v);
    inline Point3r operator/(const Point3r &v, real scalar);
    inline Point3r operator-(const Point3r &p);

    /// Returns the Euclidean distance squared between two points.
    inline real distanceSqr(const Point3r& p1, const Point3r& p2);

    /// Returns the Euclidean distance between two points.
    inline real distance(const Point3r& p1, const Point3r& p2);

    /// Prints the Point3r to the given output stream.
    inline std::ostream& operator<<(std::ostream& o, const Point3r& p); 

    class Point3r
    {
        public:
            /// X-coordinate
            real x;

            /// Y-coordinate
            real y;

            /// Z-coordinate
            real z;

            Point3r()
            {
                x = 0;
                y = 0;
                z = 0;
            }

            Point3r(const Point3r & src)
            {
                x = src.x;
                y = src.y;
                z = src.z;
            }

            Point3r(real xx, real yy, real zz)
            {
                x = xx;
                y = yy;
                z = zz;
            }

            Point3r(const real * data)
            {
                x = data[0];
                y = data[1];
                z = data[2];
            }

            inline void set(real xx, real yy, real zz)
            {
                x = xx;
                y = yy;
                z = zz;
            }

            inline void set(real * data)
            {
                x = data[0];
                y = data[1];
                z = data[2];
            }

            inline real & operator[] (unsigned int i)
            {
                switch (i)
                {
                    case 0:
                        return x;
                    case 1:
                        return y;
                    case 2:
                        return z;
                    default:
                        assert(i < 3);
                        return z;
                }
            }

            inline const real & operator[] (unsigned int i) const
            {
                switch (i)
                {
                    case 0:
                        return x;
                    case 1:
                        return y;
                    case 2:
                        return z;
                    default:
                        assert(i < 3);
                        return z;
                }
            }

            inline void operator+=(const Vec3r & v)
            {
                x += v.x;
                y += v.y;
                z += v.z;
            }

            inline void operator-=(const Vec3r & v)
            {
                x -= v.x;
                y -= v.y;
                z -= v.z;
            }

            inline void operator*=(real scalar)
            {
                x *= scalar;
                y *= scalar;
                z *= scalar;
            }

            inline void operator/=(real scalar)
            {
                x /= scalar;
                y /= scalar;
                z /= scalar;
            }

            inline bool operator==(const Point3r & v) const
            {
                return (x == v.x && y == v.y && z == v.z);
            }

            inline bool operator!=(const Point3r & v) const
            {
                return (x != v.x || y != v.y || z != v.z);
            }
    };

    inline Point3r operator+(const Point3r &u, const Vec3r &v)
    {
        return Point3r(u.x + v.x, u.y + v.y, u.z + v.z);
    }

    inline Point3r operator-(const Point3r &u, const Vec3r &v)
    {
        return Point3r(u.x - v.x, u.y - v.y, u.z - v.z);
    }

    inline Vec3r operator-(const Point3r &u, const Point3r &v)
    {
        return Vec3r(u.x - v.x, u.y - v.y, u.z - v.z);
    }

    inline Point3r operator*(const Point3r &v, real scalar)
    {
        return Point3r(scalar * v.x, scalar * v.y, scalar * v.z);
    }

    inline Point3r operator*(real scalar, const Point3r &v)
    {
        return Point3r(scalar * v.x, scalar * v.y, scalar * v.z);
    }

    inline Point3r operator/(const Point3r &v, real scalar)
    {
        return Point3r(v.x / scalar, v.y / scalar, v.z / scalar);
    }

    inline real distanceSqr(const Point3r& p1, const Point3r& p2)
    {
        return pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2) + 
			pow((p2.z - p1.z), 2);
    }

    inline real distance(const Point3r& p1, const Point3r& p2)
    {
        return sqrt(distanceSqr(p1, p2));
    }

    inline Point3r operator-(const Point3r &p)
    {
        return p * -1;
    }

    inline std::ostream& operator<<(std::ostream& o, const Point3r& p)
    {
        return o << "[" << p.x << " " << p.y << " " << p.z << "]";
    }
}

#endif

