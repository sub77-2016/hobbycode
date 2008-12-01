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

namespace testSimulator
{
	QT_TEST( multi )
	{
		Simulator * sim1 = createSimulator();
		Simulator * sim2 = createSimulator();
		Simulator * sim3 = createSimulator();

		Vec3r g( 0, -10, 0 );
		sim1->setGravity( g );
		sim2->setGravity( g );
		sim3->setGravity( g );

		Solid * s1 = sim1->createSolid();
		Solid * s2 = sim2->createSolid();
		Solid * s3 = sim3->createSolid();

		SphereShapeData data;
		data.radius = 1;
		data.material.density = 1;
		s1->addShape( data );
		s2->addShape( data );
		s3->addShape( data );

		s1->setPosition( 0, 0, 0 );
		s2->setPosition( 0, 100, 0 );
		s3->setPosition( 0, 10000, 0 );

		int i;
		int steps = 100;
		for ( i = 0; i < steps; ++i )
		{
			sim1->simulate( 0.1 );
		}
		QT_CHECK_LESS( s1->getPosition() [ 1 ], 0 );
		QT_CHECK_EQUAL( s2->getPosition() [ 1 ], 100 );
		QT_CHECK_EQUAL( s3->getPosition() [ 1 ], 10000 );

		for ( i = 0; i < steps; ++i )
		{
			sim2->simulate( 0.1 );
		}
		QT_CHECK_LESS( s1->getPosition() [ 1 ], 0 );
		QT_CHECK_LESS( s2->getPosition() [ 1 ], 100 );
		QT_CHECK_EQUAL( s3->getPosition() [ 1 ], 10000 );

		for ( i = 0; i < steps; ++i )
		{
			sim3->simulate( 0.1 );
		}
		QT_CHECK_LESS( s1->getPosition() [ 1 ], 0 );
		QT_CHECK_LESS( s2->getPosition() [ 1 ], 100 );
		QT_CHECK_LESS( s3->getPosition() [ 1 ], 10000 );

		sim1->destroySolid( s1 );
		sim2->destroySolid( s2 );
		sim3->destroySolid( s3 );

		sim1->destroy();
		sim2->destroy();
		sim3->destroy();
	}

	QT_TEST( destroySolid_from_joint )
	{
		Simulator * sim = createSimulator();
		Solid * s0 = sim->createSolid();
		Solid * s1 = sim->createSolid();
		Joint * j = sim->createJoint();
		JointData data;
		data.solid0 = s0;
		data.solid1 = s1;
		data.setType( FIXED_JOINT );
		j->init( data );

		sim->simulate( 1 );

		sim->destroySolid( s0 );

		QT_CHECK_EQUAL( j->isEnabled(), false );

		sim->destroy();
	}

	class PostStep : public PostStepEventHandler
	{
		public:
			PostStep()
			{
				calls = 0;
			}

			void OPAL_CALL handlePostStepEvent()
			{
				++calls;
			}

			unsigned long calls;
	};

	QT_TEST( step_callbacks )
	{
		Simulator * sim = createSimulator();
		sim->setStepSize( 0.01 );

		PostStep step1;
		sim->addPostStepEventHandler( &step1 );
		PostStep step2;
		sim->addPostStepEventHandler( &step2 );

		QT_CHECK_EQUAL( sim->getNumPostStepEventHandlers(), 2 );

		// one call
		sim->simulate( 0.01 );

		QT_CHECK_EQUAL( step1.calls, 1 );
		QT_CHECK_EQUAL( step2.calls, 1 );

		sim->removePostStepEventHandler( &step1 );

		sim->simulate( 0.01 );
		QT_CHECK_EQUAL( step1.calls, 1 );
		QT_CHECK_EQUAL( step2.calls, 2 );

		sim->removePostStepEventHandler( &step2 );

		sim->simulate( 0.01 );
		QT_CHECK_EQUAL( step1.calls, 1 );
		QT_CHECK_EQUAL( step2.calls, 2 );

		sim->destroy();
	}

	class Collision : public CollisionEventHandler
	{
		public:
			Collision()
			{
				calls = 0;
			}

			void OPAL_CALL handleCollisionEvent( const CollisionEvent & event )
			{
				++calls;
			}

			unsigned long calls;
	};

	QT_TEST( global_collision_callbacks )
	{
		Simulator * sim = createSimulator();
		sim->setStepSize( 0.01 );
		sim->setGravity( Vec3r( 0, -10, 0 ) );

		Collision coll1;
		sim->addGlobalCollisionEventHandler( &coll1 );
		Collision coll2;
		sim->addGlobalCollisionEventHandler( &coll2 );

		QT_CHECK_EQUAL( sim->getNumGlobalCollisionEventHandlers(), 2 );

		// prepare a collision
		Solid * s1 = sim->createSolid();
		Solid * s2 = sim->createSolid();
		{
			BoxShapeData data;
			data.dimensions = Vec3r( 1, 1, 1 );
			data.material.density = 1;
			
			s1->addShape( data );
			s1->setStatic( true );
			
			s2->addShape( data );
		}
		s1->setPosition( 0, 0, 0 );
		s2->setPosition( 0, 1.001, 0 );

		sim->simulate( 0.02 );

		QT_CHECK_EQUAL( coll1.calls, 1 );
		QT_CHECK_EQUAL( coll2.calls, 1 );

		sim->removeGlobalCollisionEventHandler( &coll1 );

		s1->setPosition( 0, 0, 0 );
		s2->setPosition( 0, 1.001, 0 );
		sim->simulate( 0.02 );
		QT_CHECK_EQUAL( coll1.calls, 1 );
		QT_CHECK_EQUAL( coll2.calls, 2 );

		sim->removeGlobalCollisionEventHandler( &coll2 );

		s1->setPosition( 0, 0, 0 );
		s2->setPosition( 0, 1.001, 0 );
		sim->simulate( 0.02 );
		QT_CHECK_EQUAL( coll1.calls, 1 );
		QT_CHECK_EQUAL( coll2.calls, 2 );

		sim->destroySolid( s1 );
		sim->destroySolid( s2 );
		sim->destroy();
	}

	QT_TEST( instantiate_blueprint_sensor_offset )
	{
		opal::Blueprint bp;
		opal::SolidData solidData;
		solidData.name = "testSolid";
		bp.addSolid(&solidData);
		opal::RaycastSensorData raySensor;
		raySensor.transform.translate(5, 0, 0);

		// Add a Sensor that is free-floating (not attached to a Solid).
		bp.addSensor(&raySensor);

		// Add a Sensor that is attached to a Solid.
		raySensor.solidBlueprintRefName = "testSolid";
		bp.addSensor(&raySensor);

		bp.finalize();

		// Instantiate the Blueprint with some initial transform.
		opal::Simulator* sim = opal::createSimulator();
		opal::BlueprintInstance instance;
		opal::Matrix44r bpTransform;
		bpTransform.translate(1, 2, 3);
		bpTransform.rotate(38, 0, 1, 0);
		sim->instantiateBlueprint(instance, bp, bpTransform);

		opal::Solid* solid = instance.getSolid(0);
		opal::Sensor* unattachedSensor = instance.getSensor(0);
		opal::Sensor* attachedSensor = instance.getSensor(1);

		opal::Point3r globalSolidPos = solid->getTransform().getPosition();
		opal::Point3r globalAttachedSensorPos = solid->getTransform() * 
			attachedSensor->getTransform().getPosition();
		opal::Point3r globalUnattachedSensorPos = 
			unattachedSensor->getTransform().getPosition();

		// Both the attached and unattached Solids should be in the same 
		// location, which is 5 units away from the Solid.
		QT_CHECK_CLOSE(opal::distance(globalAttachedSensorPos, 
			globalUnattachedSensorPos), 0, 0.001);
		QT_CHECK_CLOSE(opal::distance(globalSolidPos, 
			globalAttachedSensorPos), 5, 0.001);
		QT_CHECK_CLOSE(opal::distance(globalSolidPos, 
			globalUnattachedSensorPos), 5, 0.001);

		sim->destroy();
	}
}
