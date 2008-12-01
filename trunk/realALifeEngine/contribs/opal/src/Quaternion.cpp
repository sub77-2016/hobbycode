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

// class headers
#include "Quaternion.h"

// system headers
#include <cassert>

// project headers
#include "Vec3r.h"

namespace opal
{
    Quaternion::Quaternion()
    {
        w = 0;
        x = 0;
        y = 0;
        z = 0;
    }

    Quaternion::Quaternion(real ww, real xx, real yy, real zz)
    {
        w = ww;
        x = xx;
        y = yy;
        z = zz;
    }

    Quaternion::Quaternion(const Quaternion& src)
    {
        w = src.w;
        x = src.x;
        y = src.y;
        z = src.z;
    }

    void Quaternion::set(real ww, real xx, real yy, real zz	)
    {
        w = ww;
        x = xx;
        y = yy;
        z = zz;
    }

    real Quaternion::lengthSquared() const
    {
        return w * w + x * x + y * y + z * z;
    }

    real Quaternion::length() const
    {
        return sqrt(lengthSquared());
    }

    void Quaternion::normalize()
    {
        real len = length();
        if (len > 0)
        {
            real factor = 1 / len;
            (*this) = (*this) * factor;
        }
    }

    void Quaternion::getAngleAxis(real& angle, Vec3r& axis) const
    {
        real sqrLen = x * x + y * y + z * z;

        if (sqrLen > 0)
        {
            angle = 2 * acos(w);
            real invLen = 1 / sqrt(sqrLen);
            axis.x = x * invLen;
            axis.y = y * invLen;
            axis.z = z * invLen;
        }
        else
        {
            angle = 0;
            axis.x = 1;
            axis.y = 0;
            axis.z = 0;
        }

        // convert to degrees
        angle = radToDeg(angle);
    }

    real Quaternion::getRoll() const
    {
        return radToDeg(atan2(2 * (y * z + w * x),
                                w * w - x * x - y * y +
                                z * z));
    }

    real Quaternion::getPitch() const
    {
        return radToDeg(asin(-2 * (x * z - w * y)));
    }

    real Quaternion::getYaw() const
    {
        return radToDeg(atan2(2 * (x * y + w * z),
                                w * w + x * x - y * y -
                                z * z));
    }

    real & Quaternion::operator[] (unsigned int i)
    {
        switch (i)
        {
            case 0: return w;
            case 1: return x;
            case 2: return y;
            case 3: return z;
        }
        assert(i < 4);
        return w;
    }

    const real & Quaternion::operator[] (unsigned int i) const
    {
        switch (i)
        {
            case 0: return w;
            case 1: return x;
            case 2: return y;
            case 3: return z;
        }
        assert(i < 4);
        return w;
    }
}
