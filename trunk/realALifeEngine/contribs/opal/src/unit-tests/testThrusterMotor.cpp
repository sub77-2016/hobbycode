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

// project headers
#include "../opal.h"

// system headers
#include "../external/quicktest/quicktest.h"

using namespace opal;

namespace testThrusterMotor
{
	QT_TEST( local_forces )
	{
		Simulator * sim = createSimulator();
		Solid * target = sim->createSolid();
		{
			SphereShapeData data;
			data.radius = 0.5;
			data.material.density = 1;
			target->addShape( data );
		}
		target->setPosition( Point3r( 0, 0, 0 ) );

		ThrusterMotor * motor = sim->createThrusterMotor();
		{
			ThrusterMotorData data;
			data.enabled = false;
			data.solid = target;
			{
				Force f;
				f.type = LOCAL_FORCE_AT_LOCAL_POS;
				f.vec = Vec3r( 0, 10, 0 );
				f.singleStep = true;
				f.duration = 0;
				f.pos = Point3r( 0, 0, 0 );
				data.force = f;
			}
			motor->init( data );
		}

		QT_CHECK_EQUAL( target->getGlobalLinearVel(), Vec3r( 0, 0, 0 ) );
		motor->setEnabled( true );

		sim->simulate( 1 );

		QT_CHECK_GREATER( target->getGlobalLinearVel().y, 15.0 );

		target->setGlobalLinearVel( Vec3r( 0, 0, 0 ) );
		motor->setEnabled( false );
		
		Matrix44r m = target->getTransform();
		m.rotate( 90, 1, 0, 0 );
		target->setTransform( m );

		sim->simulate( 1 );
		QT_CHECK_EQUAL( target->getGlobalLinearVel(), Vec3r( 0, 0, 0 ) );

		motor->setEnabled( true );
		sim->simulate( 1 );

		QT_CHECK_GREATER( target->getGlobalLinearVel().z, 15.0 );
	}
}
