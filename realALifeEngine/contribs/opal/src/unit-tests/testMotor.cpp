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

//! Common motor tests.
namespace testMotor
{
    QT_TEST( using_shapeless_solids )
    {
        Simulator * sim = createSimulator();
        sim->setSolverAccuracy( SOLVER_ACCURACY_VERY_HIGH );
        Solid * shapeless = sim->createSolid();
        Solid * solid = sim->createSolid();

        shapeless->setPosition( 0, 0, 3 );

        solid->setPosition( 0, 0, 0 );
        {
            SphereShapeData data;
            data.radius = 1;
            solid->addShape( data );
        }

        Solid * static_base = sim->createSolid();
        static_base->setPosition( 0, 0, -1 );
        {
            SphereShapeData data;
            data.radius = 1;
            static_base->addShape( data );
            static_base->setStatic( true );
        }

        Joint * j_static = sim->createJoint();
        {
            JointData data;
            data.setType( FIXED_JOINT );
            data.solid0 = static_base;
            data.solid1 = solid;
            data.contactsEnabled = false;
            j_static->init( data );
        }

        Joint * j = sim->createJoint();
        {
            JointData data;
            data.setType( HINGE_JOINT );
            data.anchor = Point3r( 0, 0, 0 );
            data.solid0 = solid;
            data.solid1 = shapeless;
            data.contactsEnabled = false;
            data.axis[ 0 ].limitsEnabled = true;
            data.axis[ 0 ].direction = Vec3r( 0, 1, 0 );
            data.axis[ 0 ].limits.low = -30;
            data.axis[ 0 ].limits.high = 30;
            j->init( data );
        }

        ServoMotor * motor = sim->createServoMotor();
        {
            ServoMotorData data;
            data.jointAxisNum = 0;
            data.desiredAngle = 5;
            data.joint = j;
            data.mode = DESIRED_ANGLE_MODE;
            data.maxTorque = 10;
            data.restoreSpeed = 1;
            motor->init( data );
        }

        sim->simulate( 11 );

        QT_CHECK_CLOSE( j->getAngle( 0 ), 5, 0.001 );

        sim->destroy();
    }
}
