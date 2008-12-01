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

#ifndef OPAL_OPAL_MATH_H
#define OPAL_OPAL_MATH_H

// system headers
#include <math.h>

namespace opal
{
#ifdef OPAL_USE_DOUBLE
    typedef double real;
#else
    typedef float real;
#endif

    namespace globals
    {
        const real OPAL_PI = (real) 3.14159265358979323846;
        const real OPAL_HALF_PI = (real) 1.57079632679489661923;
        const real OPAL_ONE_THIRD = (real) 0.33333333333333333333;
        const real OPAL_EPSILON = (real) 0.000001;
    }

    /// Converts a value from degrees to radians.
    inline real degToRad(real deg)
    {
        const real d2r = globals::OPAL_PI / (real) 180.0;
        return (d2r * deg);
    }

    /// Converts a value from radians to degrees.
    inline real radToDeg(real rad)
    {
        const real r2d = (real) 180.0 / globals::OPAL_PI;
        return (r2d * rad);
    }

    /// Returns the absolute value of a real number.
    inline real abs(real value)
    {
        return fabs(value);
    }

    /// Returns true if the two values are equal within some tolerance,
    /// using a combination of absolute and relative (epsilon is scaled
    /// by the magnitudes of the values) tolerance, depending on whether
    /// both values are both less than 1.
    /// See Christer Ericson's GDC 2005 presentation:
    /// http://realtimecollisiondetection.net/pubs/GDC05_Ericson_Numerical_Robustness_for_Geometric_Calculations.ppt
    inline bool areEqual(real x, real y)
    {
        real maxVal = 1;

        if (opal::abs(x) > maxVal)
        {
            maxVal = opal::abs(x);
        }

        if (opal::abs(y) > maxVal)
        {
            maxVal = opal::abs(y);
        }

        if (opal::abs(x - y) <= globals::OPAL_EPSILON * maxVal)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    //! translates degree angle to (-180, 180]
    inline real normalizeDegrees(real degrees)
    {
        while (degrees > 180.0)
        {
            degrees -= 360.0;
        }
        while (degrees <= -180.0)
        {
            degrees += 360.0;
        }
        return degrees;
    }
}

#endif

