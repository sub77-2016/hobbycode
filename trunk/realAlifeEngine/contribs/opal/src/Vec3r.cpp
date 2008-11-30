/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
* Oleksandr Lozitskiy (http://ox.slug.louisville.edu/~o0lozi01/wiki)    *
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

// class header
#include "Vec3r.h" 
//#pragma implementation

// system headers
#include <cassert>

namespace opal
{
    Vec3r::Vec3r()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    Vec3r::Vec3r(const Vec3r & src)
    {
        x = src.x;
        y = src.y;
        z = src.z;
    }

    Vec3r::Vec3r(real xx, real yy, real zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

    Vec3r::Vec3r(const real * data)
    {
        x = data[0];
        y = data[1];
        z = data[2];
    }

    void Vec3r::set(real xx, real yy, real zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

    void Vec3r::set(real * data)
    {
        x = data[0];
        y = data[1];
        z = data[2];
    }

    real & Vec3r::operator[] (unsigned int i)
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

    const real & Vec3r::operator[] (unsigned int i) const
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

    real Vec3r::lengthSquared() const
    {
        return dot(*this, *this);
    }

    real Vec3r::length() const
    {
        return sqrt(dot(*this, *this));
    }

    void Vec3r::normalize()
    {
        real len = length();
        assert(0 != len);
        real factor = (real) 1.0 / len;
        (*this) *= factor;
    }

    Vec3r Vec3r::unit() const
    {
        return (*this) / length();
    }

    void Vec3r::operator+=(const Vec3r & v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    void Vec3r::operator-=(const Vec3r & v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    void Vec3r::operator*=(const Vec3r & v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
    }

    void Vec3r::operator*=(real scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }

    void Vec3r::operator/=(real scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
    }

    void Vec3r::operator/=(const Vec3r & v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
    }

    bool Vec3r::operator==(const Vec3r & v) const
    {
        return (areEqual(x, v.x) && areEqual(y , v.y) && areEqual(z , v.z));
    }

    void Vec3r::operator=(const Vec3r & v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    bool Vec3r::operator!=(const Vec3r & v) const
    {
        return (!areEqual(x, v.x) || !areEqual(y , v.y) || !areEqual(z , v.z));
    }
}
