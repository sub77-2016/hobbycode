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

namespace testVec3r
{
    QT_TEST( default_constructor )
    {
        Vec3r v;
		QT_CHECK_CLOSE( v.x, 0, 0.001);
		QT_CHECK_CLOSE( v.y, 0, 0.001);
		QT_CHECK_CLOSE( v.z, 0, 0.001);
    }

    QT_TEST( copy_constructor )
    {
        Vec3r copy( 1, 2, 3 );
        Vec3r v( copy );
		QT_CHECK_CLOSE( v.x, 1, 0.001 );
		QT_CHECK_CLOSE( v.y, 2, 0.001 );
		QT_CHECK_CLOSE( v.z, 3, 0.001 );
    }

    QT_TEST( set_constructor )
    {
        Vec3r v( 1, 2, 3 );
		QT_CHECK_CLOSE( v.x, 1, 0.001 );
		QT_CHECK_CLOSE( v.y, 2, 0.001 );
		QT_CHECK_CLOSE( v.z, 3, 0.001 );
    }

    QT_TEST( assign_constructor )
    {
        Vec3r v = Vec3r( 1, 2, 3 );
		QT_CHECK_CLOSE( v.x, 1, 0.001 );
		QT_CHECK_CLOSE( v.y, 2, 0.001 );
		QT_CHECK_CLOSE( v.z, 3, 0.001 );
    }

    QT_TEST( array_constructor )
    {
        real d[] = {1, 2, 3};
        Vec3r v( d );
		QT_CHECK_CLOSE( v.x, 1, 0.001 );
		QT_CHECK_CLOSE( v.y, 2, 0.001 );
		QT_CHECK_CLOSE( v.z, 3, 0.001 );
    }

    QT_TEST( set )
    {
        Vec3r v;
        v.set( 1, 2, 3 );
		QT_CHECK_CLOSE( v.x, 1, 0.001 );
		QT_CHECK_CLOSE( v.y, 2, 0.001 );
		QT_CHECK_CLOSE( v.z, 3, 0.001 );
    }

    QT_TEST( set_array )
    {
        real d[] = {1, 2, 3};
        Vec3r v;
        v.set( d );
		QT_CHECK_CLOSE( v.x, 1, 0.001 );
		QT_CHECK_CLOSE( v.y, 2, 0.001 );
		QT_CHECK_CLOSE( v.z, 3, 0.001 );
    }

    QT_TEST( operator_sq_brackets )
    {
        Vec3r v( 1, 2, 3 );

		QT_CHECK_CLOSE( v[ 0 ], 1, 0.001 );
		QT_CHECK_CLOSE( v[ 1 ], 2, 0.001 );
		QT_CHECK_CLOSE( v[ 2 ], 3, 0.001 );
    }

    QT_TEST( lengthSquared )
    {
        Vec3r v( 1, 2, 3 );
		QT_CHECK_CLOSE( v.lengthSquared(), 1 + 4 + 9, 0.001 );
    }

    QT_TEST( lengthSquared_negative )
    {
        Vec3r v( -1, -2, -3 );
		QT_CHECK_CLOSE( v.lengthSquared(), 1 + 4 + 9, 0.001 );
    }

    QT_TEST( length )
    {
        Vec3r v( 1, 2, 3 );
        QT_CHECK_CLOSE( v.length(), sqrt( 1 + 4 + 9.0 ), 0.001 );
    }

    QT_TEST( length_negative )
    {
        Vec3r v( -1, -2, -3 );
		QT_CHECK_CLOSE( v.length(), sqrt( 1 + 4 + 9.0 ), 0.001 );
    }

    QT_TEST( normalize )
    {
        Vec3r v( 2, 0, 0 );
        v.normalize();
        QT_CHECK_EQUAL( v, Vec3r( 1, 0, 0 ) );

        v.set( 0, 2, 0 );
        v.normalize();
        QT_CHECK_EQUAL( v, Vec3r( 0, 1, 0 ) );

        v.set( 0, 0, 2 );
        v.normalize();
        QT_CHECK_EQUAL( v, Vec3r( 0, 0, 1 ) );
    }

    QT_TEST( unit )
    {
        Vec3r v( 2, 0, 0 );
        QT_CHECK_EQUAL( v.unit(), Vec3r( 1, 0, 0 ) );

        v.set( 0, 2, 0 );
        QT_CHECK_EQUAL( v.unit(), Vec3r( 0, 1, 0 ) );

        v.set( 0, 0, 2 );
        QT_CHECK_EQUAL( v.unit(), Vec3r( 0, 0, 1 ) );
    }

    QT_TEST( operator_add )
    {
        Vec3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        QT_CHECK_EQUAL( v + w, Vec3r( 5, 7, 9 ) );
    }

    QT_TEST( operator_add_to )
    {
        Vec3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        v += w;
        QT_CHECK_EQUAL( v, Vec3r( 5, 7, 9 ) );
    }

    QT_TEST( operator_substract )
    {
        Vec3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        QT_CHECK_EQUAL( v - w, Vec3r( -3, -3, -3 ) );
    }

    QT_TEST( operator_substract_from )
    {
        Vec3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        v -= w;
        QT_CHECK_EQUAL( v, Vec3r( -3, -3, -3 ) );
    }

    QT_TEST( operator_mutliply_by_vector )
    {
        Vec3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        v *= w;
        QT_CHECK_EQUAL( v, Vec3r( 4, 10, 18 ) );
    }

    QT_TEST( operator_mutliply_scalar_after )
    {
        Vec3r v( 1, 2, 3 );

        QT_CHECK_EQUAL( v * 3, Vec3r( 3, 6, 9 ) );
    }

    QT_TEST( operator_mutliply_scalar_before )
    {
        Vec3r v( 1, 2, 3 );

        QT_CHECK_EQUAL( 3 * v, Vec3r( 3, 6, 9 ) );
    }

    QT_TEST( operator_divide_vector )
    {
        Vec3r v( 1, 2, 3 );
        QT_CHECK_EQUAL( v / 2, Vec3r( 1 / 2.0, 2 / 2.0, 3 / 2.0 ) );
    }

    QT_TEST( operator_divide_by_vector )
    {
        Vec3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        v /= w;
        QT_CHECK_EQUAL( v, Vec3r( 1 / 4.0, 2 / 5.0, 3 / 6.0 ) );
    }

    QT_TEST( operator_divide_scalar )
    {
        Vec3r v( 1, 2, 3 );

        QT_CHECK_EQUAL( v / 2, Vec3r( 0.5, 1, 1.5 ) );
    }

    QT_TEST( operator_equal )
    {
        QT_CHECK_EQUAL( Vec3r( 1, 2, 3 ) == Vec3r( 1, 2, 3 ), true );
        QT_CHECK_EQUAL( Vec3r( 1, 2, 3 ) == Vec3r( 4, 2, 3 ), false );
        QT_CHECK_EQUAL( Vec3r( 1, 2, 3 ) == Vec3r( 1, 4, 3 ), false );
        QT_CHECK_EQUAL( Vec3r( 1, 2, 3 ) == Vec3r( 1, 2, 4 ), false );
    }

    QT_TEST( operator_not_equal )
    {
        QT_CHECK_EQUAL( Vec3r( 1, 2, 3 ) != Vec3r( 1, 2, 3 ), false );
        QT_CHECK_EQUAL( Vec3r( 1, 2, 3 ) != Vec3r( 4, 2, 3 ), true );
        QT_CHECK_EQUAL( Vec3r( 1, 2, 3 ) != Vec3r( 1, 4, 3 ), true );
        QT_CHECK_EQUAL( Vec3r( 1, 2, 3 ) != Vec3r( 1, 2, 4 ), true );
    }

    QT_TEST( operator_percent )
    {
        QT_CHECK_EQUAL( Vec3r( 1, 2, 3 ) % Vec3r( 3, 2, 1 ), Vec3r( -4, 8, -4 ) );
    }

    QT_TEST( cross )
    {
        QT_CHECK_EQUAL( Vec3r( 1, 2, 3 ) % Vec3r( 3, 2, 1 ), Vec3r( -4, 8, -4 ) );
    }

    QT_TEST( dot )
    {
        QT_CHECK_EQUAL( dot( Vec3r( 1, 2, 3 ) , Vec3r( 3, 2, 1 ) ), 3 + 4 + 3 );
    }

    QT_TEST( negate )
    {
        QT_CHECK_EQUAL( -Vec3r( 1, 2, 3 ), Vec3r( -1, -2, -3 ) );
    }

    QT_TEST( project )
    {
        QT_CHECK_EQUAL( project( Vec3r( 2, 0, 0 ), Vec3r( 3, 2, 1 ) ),
                        Vec3r( 3, 0, 0 ) );
    }

    QT_TEST( projectPreNorm )
    {
        QT_CHECK_EQUAL( project( Vec3r( 1, 0, 0 ) , Vec3r( 3, 2, 1 ) ),
                        Vec3r( 3, 0, 0 ) );
    }

    QT_TEST( angleBetween )
    {
        QT_CHECK_CLOSE( angleBetween( Vec3r( 0, 0, 1 ) , Vec3r( 1, 0, 0 ) ),
                        90, 0.001 );
        QT_CHECK_CLOSE( angleBetween( Vec3r( 0, 0, 1 ) , Vec3r( 0.5, 0.5, 0 ) ),
                        90, 0.001 );
        QT_CHECK_CLOSE( angleBetween( Vec3r( 0, 0, 1 ) , Vec3r( 0, 0.5, 0.5 ) ),
                        45, 0.001 );
        QT_CHECK_CLOSE( angleBetween( Vec3r( 0, 0, 1 ) , Vec3r( 0, 0.5, 0.5 ) ),
                        45, 0.001 );
        QT_CHECK_CLOSE( angleBetween( Vec3r( 0, 0, 1 ) , Vec3r( 0, 0.5, -0.5 ) ),
                        90 + 45, 0.001 );
    }

    QT_TEST( angleBetweenPreNorm )
    {
        QT_CHECK_CLOSE( angleBetween( Vec3r( 0, 0, 1 ) , Vec3r( 1, 0, 0 ) ),
                        90, 0.001 );
        real s = sqrt( (real)0.5 );
        QT_CHECK_CLOSE( angleBetween( Vec3r( 0, 0, 1 ) , Vec3r( s, s, 0 ) ),
                        90, 0.001 );
        QT_CHECK_CLOSE( angleBetween( Vec3r( 0, 0, 1 ) , Vec3r( 0, s, s ) ),
                        45, 0.001 );
        QT_CHECK_CLOSE( angleBetween( Vec3r( 0, 0, 1 ) , Vec3r( 0, s, s ) ),
                        45, 0.001 );
        QT_CHECK_CLOSE( angleBetween( Vec3r( 0, 0, 1 ) , Vec3r( 0, s, -s ) ),
                        90 + 45, 0.001 );
    }

    QT_TEST( areCollinear )
    {
        QT_CHECK_EQUAL( areCollinear( Vec3r( 1, 2, 3 ), Vec3r( 2, 4, 6 ) ), true );
        QT_CHECK_EQUAL( areCollinear( Vec3r( 1, 2, 3 ), Vec3r( -2, -4, -6 ) ), true );
        QT_CHECK_EQUAL( areCollinear( Vec3r( 1, 2, 3 ), Vec3r( 2, 4, 5 ) ), false );

        QT_CHECK_EQUAL( areCollinear( Vec3r( 1, 0, 0 ), Vec3r( 2, 0, 0 ) ), true );
        QT_CHECK_EQUAL( areCollinear( Vec3r( 0, 1, 0 ), Vec3r( 0, 2, 0 ) ), true );
        QT_CHECK_EQUAL( areCollinear( Vec3r( 0, 0, 1 ), Vec3r( 0, 0, 2 ) ), true );

        QT_CHECK_EQUAL( areCollinear( Vec3r( 1, 0, 0 ), Vec3r( 2, 0, 1 ) ), false );
        QT_CHECK_EQUAL( areCollinear( Vec3r( 0, 1, 0 ), Vec3r( 1, 2, 0 ) ), false );
        QT_CHECK_EQUAL( areCollinear( Vec3r( 0, 0, 1 ), Vec3r( 0, 1, 2 ) ), false );
    }

    QT_TEST( operator_stream )
    {
        {
            std::ostringstream ss;
            ss << Vec3r( 1, 1, 1 );
            QT_CHECK_EQUAL( ss.str(), "[1 1 1]" );
        }
        {
            std::ostringstream ss;
            ss << Vec3r( 1, 1, 1.5 );
            QT_CHECK_EQUAL( ss.str(), "[1 1 1.5]" );
        }
    }
}
