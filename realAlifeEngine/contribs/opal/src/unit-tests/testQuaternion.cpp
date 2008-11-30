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
#include <sstream>

// project headers
#include "../opal.h"

using namespace opal;

namespace testQuaternion
{
    QT_TEST( default_constructor )
    {
        Quaternion q;
        QT_CHECK_EQUAL( q[ 0 ], 0 );
        QT_CHECK_EQUAL( q[ 1 ], 0 );
        QT_CHECK_EQUAL( q[ 2 ], 0 );
        QT_CHECK_EQUAL( q[ 3 ], 0 );
    }

    QT_TEST( constructor )
    {
        Quaternion q( 1, 2, 3, 4 );
        QT_CHECK_EQUAL( q[ 0 ], 1 );
        QT_CHECK_EQUAL( q[ 1 ], 2 );
        QT_CHECK_EQUAL( q[ 2 ], 3 );
        QT_CHECK_EQUAL( q[ 3 ], 4 );
    }

    QT_TEST( set )
    {
        Quaternion q;
        q.set( 1, 2, 3, 4 );
        QT_CHECK_EQUAL( q[ 0 ], 1 );
        QT_CHECK_EQUAL( q[ 1 ], 2 );
        QT_CHECK_EQUAL( q[ 2 ], 3 );
        QT_CHECK_EQUAL( q[ 3 ], 4 );
    }

    QT_TEST( operator_square_brackets )
    {
        Quaternion q( 1, 2, 3, 4 );
        QT_CHECK_EQUAL( q[ 0 ], 1 );
        QT_CHECK_EQUAL( q[ 1 ], 2 );
        QT_CHECK_EQUAL( q[ 2 ], 3 );
        QT_CHECK_EQUAL( q[ 3 ], 4 );
    }

    QT_TEST( copy_constructor )
    {
        Quaternion q( Quaternion( 1, 2, 3, 4 ) );
        QT_CHECK_EQUAL( q[ 0 ], 1 );
        QT_CHECK_EQUAL( q[ 1 ], 2 );
        QT_CHECK_EQUAL( q[ 2 ], 3 );
        QT_CHECK_EQUAL( q[ 3 ], 4 );
    }

    QT_TEST( operator_equal )
    {
        QT_CHECK_EQUAL( Quaternion( 0, 0, 0, 0 ), Quaternion( 0, 0, 0, 0 ) );
        QT_CHECK_EQUAL( Quaternion( 0, 0, 0, 1 ), Quaternion( 0, 0, 0, 1 ) );
        QT_CHECK_EQUAL( Quaternion( 0, 0, 1, 2 ), Quaternion( 0, 0, 1, 2 ) );
        QT_CHECK_EQUAL( Quaternion( 1, 2, 0, 3 ), Quaternion( 1, 2, 0, 3 ) );
    }

    QT_TEST( operator_not_equal )
    {
        QT_CHECK_NOT_EQUAL( Quaternion( 0, 0, 0, 0 ), Quaternion( 1, 0, 0, 0 ) );
        QT_CHECK_NOT_EQUAL( Quaternion( 0, 0, 0, 1 ), Quaternion( 0, 1, 0, 0 ) );
        QT_CHECK_NOT_EQUAL( Quaternion( 0, 0, 1, 2 ), Quaternion( 4, 5, 0, 2 ) );
        QT_CHECK_NOT_EQUAL( Quaternion( 1, 2, 0, 3 ), Quaternion( 1, 0, 2, 3 ) );
    }

    QT_TEST( operator_summation )
    {
        QT_CHECK_EQUAL( Quaternion( 1, 0, 0, 0 ) + Quaternion( 0, 1, 0, 0 ),
                        Quaternion( 1, 1, 0, 0 ) );
        QT_CHECK_EQUAL( Quaternion( 1, 2, 3, 4 ) + Quaternion( 4, 3, 2, 1 ),
                        Quaternion( 5, 5, 5, 5 ) );
    }

    QT_TEST( operator_substraction )
    {
        QT_CHECK_EQUAL( Quaternion( 1, 0, 0, 0 ) - Quaternion( 0, 1, 0, 0 ),
                        Quaternion( 1, -1, 0, 0 ) );
        QT_CHECK_EQUAL( Quaternion( 1, 2, 3, 4 ) - Quaternion( 4, 3, 2, 1 ),
                        Quaternion( -3, -1, 1, 3 ) );
    }

    QT_TEST( operator_multiply_before_scalar )
    {
        QT_CHECK_EQUAL( Quaternion( 1, 0, 0, 0 ) * 5, Quaternion( 5, 0, 0, 0 ) );
        QT_CHECK_EQUAL( Quaternion( 1, 2, -3, 4 ) * -1, Quaternion( -1, -2, 3, -4 ) );
    }

    QT_TEST( operator_multiply_after_scalar )
    {
        QT_CHECK_EQUAL( 5 * Quaternion( 1, 0, 0, 0 ), Quaternion( 5, 0, 0, 0 ) );
        QT_CHECK_EQUAL( -1 * Quaternion( 1, 2, -3, 4 ), Quaternion( -1, -2, 3, -4 ) );
    }

    QT_TEST( operator_string )
    {
		std::ostringstream ss;
        ss << Quaternion( 1, 2, 3, 4 );
        QT_CHECK_EQUAL( ss.str(), "[1 2 3 4]" );
    }

    QT_TEST( lengthSquared )
    {
        QT_CHECK_CLOSE( Quaternion( 0, 0, 0, 0 ).lengthSquared(), 0, 0.001 );
        QT_CHECK_CLOSE( Quaternion( 1, 0, 0, 0 ).lengthSquared(), 1, 0.001 );
        QT_CHECK_CLOSE( Quaternion( 1, 2, 0, 0 ).lengthSquared(), 5, 0.001 );
        QT_CHECK_CLOSE( Quaternion( 1, 2, 3, 0 ).lengthSquared(), 14, 0.001 );
        QT_CHECK_CLOSE( Quaternion( 1, 2, 3, 4 ).lengthSquared(), 30, 0.001 );
    }

    QT_TEST( length )
    {
        QT_CHECK_CLOSE( Quaternion( 0, 0, 0, 0 ).length(), 0, 0.001 );
        QT_CHECK_CLOSE( Quaternion( 1, 0, 0, 0 ).length(), 1, 0.001 );
        QT_CHECK_CLOSE( Quaternion( 1, 2, 0, 0 ).length(), sqrt( 5.0 ), 0.001 );
        QT_CHECK_CLOSE( Quaternion( 1, 2, 3, 0 ).length(), sqrt( 14.0 ), 0.001 );
        QT_CHECK_CLOSE( Quaternion( 1, 2, 3, 4 ).length(), sqrt( 30.0 ), 0.001 );
    }

    QT_TEST( normalize )
    {
        Quaternion q( 0, 0, 0, 0 );
        q.normalize();
        QT_CHECK_EQUAL( q, Quaternion( 0, 0, 0, 0 ) );

        q.set( 0, 0, 0, 1 );
        q.normalize();
        QT_CHECK_EQUAL( q, Quaternion( 0, 0, 0, 1 ) );

        q.set( 1, 0, 0, 0 );
        q.normalize();
        QT_CHECK_EQUAL( q, Quaternion( 1, 0, 0, 0 ) );

        q.set( 1, 0, 0, 1 );
        q.normalize();
		QT_CHECK_EQUAL( q, Quaternion( sqrt( (real)2.0 ) / (real)2.0, 0, 0, 
			sqrt( (real)2.0 ) / (real)2.0 ) );
    }

    QT_TEST( getAngleAxis )
    {
        Quaternion q( 0, 1, 0, 0 );
        real angle;
        Vec3r axis;
        q.getAngleAxis( angle, axis );
        QT_CHECK_EQUAL( angle, 180 );
        QT_CHECK_EQUAL( axis, Vec3r( 1, 0, 0 ) );

        q.set( 0, 0, 1, 0 );
        q.getAngleAxis( angle, axis );
        QT_CHECK_EQUAL( angle, 180 );
        QT_CHECK_EQUAL( axis, Vec3r( 0, 1, 0 ) );

        q.set( 1, 0, 1, 0 );
        q.normalize();
        q.getAngleAxis( angle, axis );
        QT_CHECK_CLOSE( angle, 90, 0.001 );
        QT_CHECK_EQUAL( axis, Vec3r( 0, 1, 0 ) );
    }

    QT_TEST( getRoll_Pitch_Yaw )
    {
        Quaternion q( 0, 0, 1, 0 );
        QT_CHECK_CLOSE( q.getRoll(), 180, 0.001 );
        QT_CHECK_CLOSE( q.getPitch(), 0, 0.001 );
        QT_CHECK_CLOSE( q.getYaw(), 180, 0.001 );

        q.set( 0, 1, 0, 0 );
        QT_CHECK_CLOSE( q.getRoll(), 180, 0.001 );
        QT_CHECK_CLOSE( q.getPitch(), 0, 0.001 );
        QT_CHECK_CLOSE( q.getYaw(), 0, 0.001 );

        q.set( 0, 0, 0, 1 );
        QT_CHECK_CLOSE( q.getRoll(), 0, 0.001 );
        QT_CHECK_CLOSE( q.getPitch(), 0, 0.001 );
        QT_CHECK_CLOSE( q.getYaw(), 180, 0.001 );
    }
}
