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

// system headers
#include "../external/quicktest/quicktest.h"

// project headers
#include "../opal.h"

using namespace opal;

namespace testOpalMath
{
    QT_TEST( normalizeDegrees )
    {
        QT_CHECK_EQUAL( normalizeDegrees( 179.0 ), 179.0 );
        QT_CHECK_EQUAL( normalizeDegrees( 180.0 ) , 180.0 );

        QT_CHECK_EQUAL( normalizeDegrees( -179.0 ) , -179.0 );
        QT_CHECK_EQUAL( normalizeDegrees( -180.0 ) , 180.0 );

        QT_CHECK_EQUAL( normalizeDegrees( 360.0 + 20.0 ) , 20.0 );
        QT_CHECK_EQUAL( normalizeDegrees( 360.0 + -20.0 ) , -20.0 );
        QT_CHECK_EQUAL( normalizeDegrees( 180.0 + 20.0 ) , -160.0 );
    }

    QT_TEST( angleBetween )
    {
        real angle;

        angle = angleBetween( Vec3r( 0, 0, 6 ), Vec3r( 0, 0, 5 ) );
        QT_CHECK_EQUAL( angle, 0.0 );
        
        angle = angleBetween( Vec3r( 0, 0, 6 ), Vec3r( -1, 0, 0 ) );
        QT_CHECK_EQUAL( angle, 90.0 );

        angle = angleBetween( Vec3r( -1, 0, 0 ), Vec3r( 0, 0, 6 ) );
        QT_CHECK_EQUAL( angle, 90.0 );
    }

    QT_TEST( distance )
    {
        QT_CHECK_EQUAL( distance( Point3r( 0, -5, 0 ), Point3r( 0, 5, 0 ) ), 10 );
        QT_CHECK_EQUAL( distance( Point3r( 0, -9, 0 ), Point3r( 0, 1, 0 ) ), 10 );
    }
}
