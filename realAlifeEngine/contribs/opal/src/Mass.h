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

#ifndef OPAL_MASS_H
#define OPAL_MASS_H

// project headers
#include "Matrix44r.h"
#include "OpalMath.h"
#include "Vec3r.h"

namespace opal
{
    class Mass
    {
        public:
            /// Default values are set
            Mass()
            {
                mass = 1;
                center = Vec3r(0, 0, 0);
                inertia.makeIdentity();
                inertia[0] = static_cast<real>(1 / 6.0);
				inertia[5] = static_cast<real>(1 / 6.0);
				inertia[10] = static_cast<real>(1 / 6.0);
            }

            /// Total mass of the rigid body
            /// Default is 1
            real mass;

            /// Center of gravity position in body frame (x,y,z).
            /// Default is (0,0,0)
            Vec3r center;

            /// 3x3 inertia tensor in body frame, about POR
            /// Default is inertia of a box
            Matrix44r inertia;
    };
}

#endif
