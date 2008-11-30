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

namespace testPoint3r
{
    QT_TEST( default_constructor )
    {
        Point3r v;
        QT_CHECK_EQUAL( v.x, 0 );
        QT_CHECK_EQUAL( v.y, 0 );
        QT_CHECK_EQUAL( v.z, 0 );
    }

    QT_TEST( copy_constructor )
    {
        Point3r copy( 1, 2, 3 );
        Point3r v( copy );
        QT_CHECK_EQUAL( v.x, 1 );
        QT_CHECK_EQUAL( v.y, 2 );
        QT_CHECK_EQUAL( v.z, 3 );
    }

    QT_TEST( set_constructor )
    {
        Point3r v( 1, 2, 3 );
        QT_CHECK_EQUAL( v.x, 1 );
        QT_CHECK_EQUAL( v.y, 2 );
        QT_CHECK_EQUAL( v.z, 3 );
    }

    QT_TEST( assign_constructor )
    {
        Point3r v = Point3r( 1, 2, 3 );
        QT_CHECK_EQUAL( v.x, 1 );
        QT_CHECK_EQUAL( v.y, 2 );
        QT_CHECK_EQUAL( v.z, 3 );
    }

    QT_TEST( array_constructor )
    {
        real d[] = {1, 2, 3};
        Point3r v( d );
        QT_CHECK_EQUAL( v.x, 1 );
        QT_CHECK_EQUAL( v.y, 2 );
        QT_CHECK_EQUAL( v.z, 3 );
    }

    QT_TEST( set )
    {
        Point3r v;
        v.set( 1, 2, 3 );
        QT_CHECK_EQUAL( v.x, 1 );
        QT_CHECK_EQUAL( v.y, 2 );
        QT_CHECK_EQUAL( v.z, 3 );
    }

    QT_TEST( set_array )
    {
        real d[] = {1, 2, 3};
        Point3r v;
        v.set( d );
        QT_CHECK_EQUAL( v.x, 1 );
        QT_CHECK_EQUAL( v.y, 2 );
        QT_CHECK_EQUAL( v.z, 3 );
    }

    QT_TEST( operator_sq_brackets )
    {
        Point3r v( 1, 2, 3 );

        QT_CHECK_EQUAL( v[ 0 ], 1 );
        QT_CHECK_EQUAL( v[ 1 ], 2 );
        QT_CHECK_EQUAL( v[ 2 ], 3 );
    }

    QT_TEST( operator_add )
    {
        Point3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        QT_CHECK_EQUAL( v + w, Point3r( 5, 7, 9 ) );
    }

    QT_TEST( operator_add_to )
    {
        Point3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        v += w;
        QT_CHECK_EQUAL( v, Point3r( 5, 7, 9 ) );
    }

    QT_TEST( operator_substract_vector )
    {
        Point3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        QT_CHECK_EQUAL( v - w, Point3r( -3, -3, -3 ) );
    }

    QT_TEST( operator_substract_from_vector )
    {
        Point3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        v -= w;
        QT_CHECK_EQUAL( v, Point3r( -3, -3, -3 ) );
    }

    QT_TEST( operator_substract_point )
    {
        Point3r v( 1, 2, 3 );
        Point3r w( 4, 5, 6 );

        QT_CHECK_EQUAL( v - w, Vec3r( -3, -3, -3 ) );
    }

    QT_TEST( operator_substract_from_point )
    {
        Point3r v( 1, 2, 3 );
        Vec3r w( 4, 5, 6 );

        v -= w;
        QT_CHECK_EQUAL( v, Point3r( -3, -3, -3 ) );
    }

    QT_TEST( operator_mutliply_scalar_after )
    {
        Point3r v( 1, 2, 3 );

        QT_CHECK_EQUAL( v * 3, Point3r( 3, 6, 9 ) );
    }

    QT_TEST( operator_mutliply_scalar_before )
    {
        Point3r v( 1, 2, 3 );

        QT_CHECK_EQUAL( 3 * v, Point3r( 3, 6, 9 ) );
    }

    QT_TEST( operator_divide_vector )
    {
        Point3r v( 1, 2, 3 );
        QT_CHECK_EQUAL( v / 2, Point3r( 1 / 2.0, 2 / 2.0, 3 / 2.0 ) );
    }

    QT_TEST( operator_divide_scalar )
    {
        Point3r v( 1, 2, 3 );

        QT_CHECK_EQUAL( v / 2, Point3r( 0.5, 1, 1.5 ) );
    }

    QT_TEST( operator_equal )
    {
        QT_CHECK_EQUAL( Point3r( 1, 2, 3 ) == Point3r( 1, 2, 3 ), true );
        QT_CHECK_EQUAL( Point3r( 1, 2, 3 ) == Point3r( 4, 2, 3 ), false );
        QT_CHECK_EQUAL( Point3r( 1, 2, 3 ) == Point3r( 1, 4, 3 ), false );
        QT_CHECK_EQUAL( Point3r( 1, 2, 3 ) == Point3r( 1, 2, 4 ), false );
    }

    QT_TEST( operator_not_equal )
    {
        QT_CHECK_EQUAL( Point3r( 1, 2, 3 ) != Point3r( 1, 2, 3 ), false );
        QT_CHECK_EQUAL( Point3r( 1, 2, 3 ) != Point3r( 4, 2, 3 ), true );
        QT_CHECK_EQUAL( Point3r( 1, 2, 3 ) != Point3r( 1, 4, 3 ), true );
        QT_CHECK_EQUAL( Point3r( 1, 2, 3 ) != Point3r( 1, 2, 4 ), true );
    }

    QT_TEST( negate )
    {
        QT_CHECK_EQUAL( -Point3r( 1, 2, 3 ), Point3r( -1, -2, -3 ) );
    }

    QT_TEST( operator_stream )
    {
        {
            std::ostringstream ss;
            ss << Point3r( 1, 1, 1 );
            QT_CHECK_EQUAL( ss.str(), "[1 1 1]" );
        }
        {
            std::ostringstream ss;
            ss << Point3r( 1, 1, 1.5 );
            QT_CHECK_EQUAL( ss.str(), "[1 1 1.5]" );
        }
    }

    QT_TEST( distance )
    {
        QT_CHECK_EQUAL( distance( Point3r( 1, 2, 3 ) , Point3r( 1, 2, 3 ) ), 0 );
        QT_CHECK_EQUAL( distance( Point3r( 1, 0, 1 ) , Point3r( 1, 0, 3 ) ), 2 );
        QT_CHECK_EQUAL( distance( Point3r( 1, 0, 3 ) , Point3r( 1, 0, 1 ) ), 2 );
    }
}
