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

namespace testMatrix44r
{
    QT_TEST( setQuaternion_values )
    {
        Matrix44r m;
        m.setQuaternion( 0, 0, 0, 1 );
        QT_CHECK_EQUAL( m.getEulerXYZ(), Vec3r( 0, 0, -180 ) );

		QT_CHECK_EQUAL( m.getQuaternion(), Quaternion( 0, 0, 0, 1 ) );
		QT_CHECK_EQUAL( m.getQuaternion(), Quaternion( 0, 0, 0, 1 ) );
    }

    QT_TEST( setQuaternion_object )
    {
        Matrix44r m;
        m.setQuaternion( Quaternion( 0, 0, 0, 1 ) );
        QT_CHECK_EQUAL( m.getEulerXYZ(), Vec3r( 0, 0, -180 ) );

		QT_CHECK_EQUAL( m.getQuaternion(), Quaternion( 0, 0, 0, 1 ) );
		QT_CHECK_EQUAL( m.getQuaternion(), Quaternion( 0, 0, 0, 1 ) );
    }

	QT_TEST( matrix_point_multiply )
	{
		opal::Matrix44r transform;
		transform.translate(5, 0, 0);
		opal::Point3r p;
		p = transform * p;
		opal::Point3r intendedPosition(5, 0, 0);

		QT_CHECK_EQUAL( opal::distance(p, intendedPosition), 0 );
	}

	QT_TEST(matrix_inverse)
	{
		opal::Matrix44r m1;
		m1.translate(1, 2, 3);
		m1.rotate(48, 1, 0, 0);
		m1.translate(-10, 42, -37);
		m1.rotate(68, 0, 0, 1);
		opal::Matrix44r m2 = m1;
		m2.invert();
		m2.invert();

		for (size_t i = 0; i < 16; ++i)
		{
			QT_CHECK_CLOSE(m1[i], m2[i], 0.001);
		}

		m2 = m1;
		m2.invert();
		opal::Point3r original(-31, 25, 3);
		opal::Point3r transformed = m2 * original;
		opal::Point3r restored = m1 * transformed;

		QT_CHECK_CLOSE(original.x, restored.x, 0.001);
		QT_CHECK_CLOSE(original.y, restored.y, 0.001);
		QT_CHECK_CLOSE(original.z, restored.z, 0.001);
	}
}
