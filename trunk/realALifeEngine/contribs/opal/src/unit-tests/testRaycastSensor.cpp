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
using namespace std;

namespace testRayCastSensor
{
    class Fixture
    {
        public:
            Fixture()
            {
                sim = createSimulator();
                sim->setGravity( Vec3r( 0, 0, 0 ) );
                caster = sim->createSolid();
                caster->setName( "caster" );
                {
                    SphereShapeData data;
                    data.radius = 1;
                    caster->addShape( data );
                }
                sensor = sim->createRaycastSensor();

                plane = sim->createSolid();
                plane->setName( "plane" );
                plane->setStatic( true );
                {
                    PlaneShapeData data;
                    real plane_params[ 4 ] = {0, 1, 0, 0};
                    for ( int i = 0; i < 4; ++i )
                        data.abcd[ i ] = plane_params[ i ];
                    plane->addShape( data );
                }

                {
                    RaycastSensorData data;
                    data.solid = caster;
                    data.ray.setOrigin( caster->getPosition() );
                    data.ray.setDir( Vec3r( 0, -1, 0 ) );
                    sensor->init( data );
                }
            }

            ~Fixture()
            {
                sim->destroySensor( sensor );
                sim->destroySolid( caster );
                sim->destroy();
            }

            Simulator * sim;
            Solid * caster;
            Solid * plane;
            RaycastSensor * sensor;
    };

    QT_TEST( testRay_on_Plane )
    {
        Fixture f;
        f.caster->setPosition( 0, 10, 0 );

        RaycastResult r = f.sensor->fireRay( 15 );
        Point3r hitPoint = r.intersection;

        QT_CHECK_EQUAL( hitPoint, Point3r( 0, 0, 0 ) );

        real diff = r.distance;

        QT_CHECK_EQUAL( diff, 10 );

        QT_CHECK_EQUAL( r.distance, 10 );
    }

    QT_TEST( testRay_Sphere_before_Plane )
    {
        Fixture f;
        f.caster->setPosition( 0, 10, 0 );

        Solid * intheway = f.sim->createSolid();
        intheway->setPosition( 0, 5, 0 );
        intheway->setName( "in the way" );
        {
            SphereShapeData data;
            data.radius = 1;
            intheway->addShape( data );
        }
        intheway->setStatic( false );

        Rayr ray( Point3r( 0, 0, 0 ), Vec3r( 0, -15, 0 ) );
        f.sensor->setRay( ray );
        
        RaycastResult r = f.sensor->fireRay( );
        Point3r hitPoint = r.intersection;

        real diff = r.distance;

        QT_CHECK_EQUAL( diff, 4 );

        f.sim->destroySolid( intheway );
    }

    QT_TEST( testPiercingRay_default_length )
    {
        Fixture f;
        f.caster->setPosition( 0, 10, 0 );
        
        Solid * intheway = f.sim->createSolid();
        intheway->setPosition( 0, -5, 0 );
        intheway->setName( "in the way" );
        {
            SphereShapeData data;
            data.radius = 1;
            intheway->addShape( data );
        }
        intheway->setStatic( false );

        Rayr ray( Point3r( 0, 0, 0 ), Vec3r( 0, -200, 0 ) );
        f.sensor->setRay( ray );

        vector<RaycastResult> r = f.sensor->firePiercingRay( );

        QT_CHECK_EQUAL( r.size(), 2 );
        QT_CHECK_EQUAL( r[ 0 ].distance, 10 );
        QT_CHECK_EQUAL( r[ 1 ].distance, 14 );
        QT_CHECK_EQUAL( r[ 0 ].solid, f.plane );
        QT_CHECK_EQUAL( r[ 1 ].solid, intheway );

        f.sim->destroySolid( intheway );
    }

    QT_TEST( testPiercingRay )
    {
        Fixture f;
        f.caster->setPosition( 0, 10, 0 );

        Solid * intheway = f.sim->createSolid();
        intheway->setPosition( 0, -5, 0 );
        intheway->setName( "in the way" );
        {
            SphereShapeData data;
            data.radius = 1;
            intheway->addShape( data );
        }
        intheway->setStatic( false );

        vector<RaycastResult> r = f.sensor->firePiercingRay( 15 );

        QT_CHECK_EQUAL( r.size(), 2 );
        QT_CHECK_EQUAL( r[ 0 ].distance, 10 );
        QT_CHECK_EQUAL( r[ 1 ].distance, 14 );
        QT_CHECK_EQUAL( r[ 0 ].solid, f.plane );
        QT_CHECK_EQUAL( r[ 1 ].solid, intheway );

        f.sim->destroySolid( intheway );
    }
}
