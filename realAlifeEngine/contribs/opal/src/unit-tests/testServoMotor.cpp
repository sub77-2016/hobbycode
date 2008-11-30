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

namespace testServoMotor
{
    /**
     * Two ServoMotor's working on different angles of
     * a universal joint.
     */
    QT_TEST( double_on_universal_joint )
    {
        Simulator * sim = createSimulator();

        Solid * base = sim->createSolid();
        base->setPosition( 0, 0.5, 0 );
        //base->setStatic( true );
        Solid * leg = sim->createSolid();
        leg->setPosition( 0, -0.5, 0 );

        {
            SphereShapeData data;
            data.radius = 0.5;
            base->addShape( data );
            leg->addShape( data );
        }

        Joint * universal = sim->createJoint();
        {
            JointData data;
            data.setType( UNIVERSAL_JOINT );
			data.axis[0].direction.set(0, 0, 1);
			data.axis[1].direction.set(1, 0, 0);
            data.anchor = Point3r( 0, 0, 0 );
            data.contactsEnabled = false;
            data.solid0 = base;
            data.solid1 = leg;
            universal->init( data );
        }

        ServoMotor * motorY = sim->createServoMotor();
        {
            ServoMotorData data;
            data.mode = DESIRED_ANGLE_MODE;
            data.joint = universal;
            data.jointAxisNum = 0;
            data.desiredAngle = 10;
            data.enabled = true;
            data.maxTorque = 100;
            motorY->init( data );
        }

        ServoMotor * motorZ = sim->createServoMotor();
        {
            ServoMotorData data;
            data.mode = DESIRED_ANGLE_MODE;
            data.joint = universal;
            data.jointAxisNum = 1;
            data.desiredAngle = 200;
            data.enabled = true;
            data.maxTorque = 10;
            motorZ->init( data );
        }

        QT_CHECK_EQUAL( universal->getAngle( 0 ), 0 );
        QT_CHECK_EQUAL( universal->getAngle( 1 ), 0 );

        sim->simulate( 1 );

        QT_CHECK_CLOSE( universal->getAngle( 0 ), 10, 0.001 );
        QT_CHECK_CLOSE( universal->getAngle( 1 ), -180 + 20, 0.001 );

        sim->destroy();
    }
}
