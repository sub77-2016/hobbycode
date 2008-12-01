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

namespace testVelocityMotor
{
    QT_TEST( constructor )
    {
        Simulator * sim = createSimulator();
        VelocityMotor * motor = sim->createVelocityMotor();
        sim->destroyMotor( motor );
        sim->destroy();
    }

    class Fixture
    {
        public:
            Fixture()
            {
                sim = createSimulator();
                sim->setGravity( Vec3r( 0, -9.81, 0 ) );

                solid = sim->createSolid();
                SphereShapeData data;
                data.radius = 1;
                data.material.density = 1;
                solid->addShape( data );

                motor = sim->createVelocityMotor();
                VelocityMotorData vdata;
                vdata.solid = solid;
                vdata.maxForce = 1000;
                motor->init( vdata );
            }

            ~Fixture()
            {
                sim->destroyMotor( motor );
                sim->destroySolid( solid );
                sim->destroy();
            }

            Simulator * sim;
            Solid * solid;
            VelocityMotor * motor;
    };

    QT_TEST( check_default )
    {
        Fixture f;
        f.sim->simulate(0.2);
        QT_CHECK_CLOSE(f.solid->getGlobalLinearVel().x + f.solid->getGlobalLinearVel().z, 0, 0.001);
    }

    QT_TEST( check_movement )
    {
        Fixture f;
        f.motor->setVelocity( Vec3r( 1, 0, 0 ) );
        f.sim->simulate( 0.2 );
        QT_CHECK( f.solid->getGlobalLinearVel().length() > 0 );
    }

    QT_TEST( check_achieving_velocity )
    {
        Fixture f;
        f.motor->setVelocity( Vec3r( 1, 0, 0 ) );

        for ( int i = 0; i < 5; ++i )
            f.sim->simulate( 0.2 );

        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().x, 1, 0.01 );
        // don't care about .y here
        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().z, 0, 0.01 );
    }

    QT_TEST( check_achieving_velocity_after_changing_it )
    {
        Fixture f;
        f.motor->setVelocity( Vec3r( 1, 0, 0 ) );

		int i;
        for ( i = 0; i < 5; ++i )
            f.sim->simulate( 0.2 );

        f.motor->setVelocity( Vec3r( 0, 0, 1 ) );

        for ( i = 0; i < 5; ++i )
            f.sim->simulate( 0.2 );

        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().x, 0, 0.01 );
        // don't care about .y here
        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().z, 1, 0.01 );
    }

    QT_TEST( check_achieving_velocity_upward )
    {
        Fixture f;
        f.motor->setVelocity( Vec3r( 0, 1, 0 ) );
        f.motor->letGravityAffectSolid( false );

        for ( int i = 0; i < 50; ++i )
            f.sim->simulate( 0.2 );

        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().x, 0, 0.01 );
        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().y, 1, 0.01 );
        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().z, 0, 0.01 );
    }

    QT_TEST( check_achieving_velocity_downward )
    {
        Fixture f;
        f.motor->setVelocity( Vec3r( 0, -1, 0 ) );
        f.motor->letGravityAffectSolid( false );

        for ( int i = 0; i < 5; ++i )
            f.sim->simulate( 0.2 );

        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().x, 0, 0.01 );
        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().y, -1, 0.01 );
        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().z, 0, 0.01 );
    }

    QT_TEST( check_achieving_velocity_while_allowing_gravity )
    {
        Fixture f;
        f.motor->setVelocity( Vec3r( 1, 0, 0 ) );
        f.motor->letGravityAffectSolid( true );

        for ( int i = 0; i < 5; ++i )
            f.sim->simulate( 0.2 );

        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().x, 1, 0.01 );
        QT_CHECK_LESS( f.solid->getGlobalLinearVel().y, -0.1 );
        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().z, 0, 0.01 );
    }

    QT_TEST( enable_disable )
    {
        Fixture f;
        f.motor->setVelocity( Vec3r( 0, 1, 0 ) );
        f.motor->setEnabled( true );
        f.sim->simulate( 0.2 );
        
        QT_CHECK_GREATER( f.solid->getGlobalLinearVel().y, 0 );

        real y = f.solid->getGlobalLinearVel().y;

        f.motor->setEnabled( false );
        f.sim->simulate( 0.2 );

        QT_CHECK_LESS( f.solid->getGlobalLinearVel().y, y );

        y = f.solid->getGlobalLinearVel().y;
        f.motor->setEnabled( true );
        f.sim->simulate( 0.2 );
        
        QT_CHECK_GREATER( f.solid->getGlobalLinearVel().y, y );
    }

    QT_TEST( not_enough_force )
    {
        Fixture f;
        f.motor->setVelocity( Vec3r( 0, 1, 0 ) );
        f.motor->setMaximumForce( 1 );
        f.sim->simulate( 0.2 );

        QT_CHECK_LESS( f.solid->getGlobalLinearVel().y, 0.5 );

        f.motor->setMaximumForce( 1000 );
        f.sim->simulate( 0.2 );

        QT_CHECK_CLOSE( f.solid->getGlobalLinearVel().y, 1, 0.01 );
    }
}
