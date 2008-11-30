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

namespace testAccelerationSensor
{
    class Fixture
    {
        public:
            Fixture()
            {
                sim = createSimulator();
                sim->setGravity( Vec3r( 0, -9.81, 0 ) );
                s = sim->createSolid();
                sensor = sim->createAccelerationSensor();
            }

            ~Fixture()
            {
                sim->destroySensor( sensor );
                sim->destroySolid( s );
                sim->destroy();
            }

            Simulator * sim;
            Solid * s;
            AccelerationSensor * sensor;
    };

    QT_TEST( testCreation )
    {
        Fixture f;
        QT_CHECK( f.sensor != NULL );
    }

    QT_TEST( testNaming )
    {
        Fixture f;
        f.sensor->setName( "baba" );
        QT_CHECK_EQUAL( "baba", f.sensor->getName() );
    }

    class SetupFixture : public Fixture
    {
        public:
            SetupFixture()
            {
                AccelerationSensorData data;
                data.solid = s;
                sensor->init( data );
            }
    };

    QT_TEST( testAcceleration )
    {
        SetupFixture f;
        f.sim->simulate( 1 );

        QT_CHECK( f.sensor->getGlobalLinearAccel() [ 1 ] < 0 );
    }

    QT_TEST( testDisabled )
    {
        SetupFixture f;
        f.sensor->setEnabled( false );
        f.sim->simulate( 1 );

        QT_CHECK_CLOSE( f.sensor->getGlobalLinearAccel() [ 1 ] , 0, 0.001 );
    }
}
