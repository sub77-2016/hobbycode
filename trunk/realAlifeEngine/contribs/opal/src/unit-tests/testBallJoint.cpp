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

namespace testBallJoint
{
	QT_TEST( creation )
	{
		Simulator * sim = createSimulator();
		sim->setGravity( Vec3r( 0, -9.81, 0 ) );
		sim->setStepSize( 0.02 );

		// Solid ball
		Solid * ball = sim->createSolid();
		{
			ball->setPosition( 0, 1, 0 );
			SphereShapeData sphereData;
			sphereData.radius = 2.0;
			sphereData.material = globals::metalMaterial;
			ball->addShape( sphereData );
		}

		//box
		Solid * box = sim->createSolid();
		{
			box->setPosition( 0, 0, 0 );
			BoxShapeData boxData;
			boxData.dimensions.set( 10.0, 1.0, 10.0 );
			boxData.material = globals::metalMaterial;
			box->addShape( boxData );
		}

		//joint create
		Joint* joint = sim->createJoint();
		{
			JointData jointData;
			jointData.setType( BALL_JOINT ); //HINGE_JOINT);
			jointData.solid0 = ball;
			jointData.solid1 = box;
			jointData.anchor = Point3r( 0.0, 0.5, 0.0 );
			jointData.axis[ 0 ].direction = Vec3r( 1.5, 1.5, 0.0 );
			jointData.axis[ 0 ].direction = Vec3r( -10.0, 0.0, 30.0 );
			joint->init( jointData );
		}

		sim->simulate( 1 );
	}
}
