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

namespace testSolid
{
	class SolidFixture
	{
		public:
			SolidFixture()
			{
				sim = createSimulator();
				sim->setGravity( Vec3r( 0, -9.81, 0 ) );
				s = sim->createSolid();
			}

			~SolidFixture()
			{
				sim->destroySolid( s );
				sim->destroy();
			}

			Simulator * sim;
			Solid * s;
	};

	QT_TEST( testCreation )
	{
		SolidFixture f;
		QT_CHECK( f.s != NULL );
	}

	QT_TEST( testNaming )
	{
		SolidFixture f;
		f.s->setName( "baba" );
		QT_CHECK_EQUAL( "baba", f.s->getName() );

		f.s->setName( "another_baba" );
		QT_CHECK_EQUAL( "another_baba", f.s->getName() );
	}

	class SolidBoxFixture : public SolidFixture
	{
		public:
			SolidBoxFixture() : SolidFixture()
			{
				BoxShapeData data;
				data.dimensions = Vec3r( 1, 1, 1 );
				data.material.density = 1;
				s->addShape( data );
			}
	};

	QT_TEST( same_mass_after_static_change )
	{
		SolidBoxFixture f;

		real before = f.s->getMass();
		f.s->setStatic( true );
		f.s->setStatic( false );
		real after = f.s->getMass();

		QT_CHECK_EQUAL( before, after );
	}

	QT_TEST( testEnablingTrue )
	{
		SolidBoxFixture f;

		f.s->setEnabled( true );
		Point3r p = f.s->getPosition();

		f.sim->simulate( 1 );

		QT_CHECK_LESS( f.s->getPosition() [ 1 ], p[ 1 ] );
	}

	QT_TEST( testEnablingFalse )
	{
		SolidBoxFixture f;

		f.s->setEnabled( false );
		Point3r p = f.s->getPosition();

		f.sim->simulate( 1 );

		QT_CHECK_CLOSE( f.s->getPosition() [ 1 ], p[ 1 ], 0.001 );
	}

	QT_TEST( testStaticTrue )
	{
		SolidBoxFixture f;

		f.s->setStatic( true );
		Point3r p = f.s->getPosition();

		f.sim->simulate( 1 );

		QT_CHECK_CLOSE( f.s->getPosition() [ 1 ], p[ 1 ], 0.001 );
	}

	QT_TEST( testStaticFalse )
	{
		SolidBoxFixture f;

		f.s->setStatic( false );
		Point3r p = f.s->getPosition();

		f.sim->simulate( 1 );

		QT_CHECK_LESS( f.s->getPosition() [ 1 ], p[ 1 ] );
	}

	QT_TEST( testSleepingTrue )
	{
		SolidBoxFixture f;

		f.s->setSleeping( true );
		Point3r p = f.s->getPosition();

		f.sim->simulate( 1 );

		QT_CHECK_CLOSE( f.s->getPosition() [ 1 ], p[ 1 ], 0.001 );
	}

	QT_TEST( testSleepingFalse )
	{
		SolidBoxFixture f;

		f.s->setSleeping( false );
		Point3r p = f.s->getPosition();

		f.sim->simulate( 1 );

		QT_CHECK_LESS( f.s->getPosition() [ 1 ], p[ 1 ] );
	}

	QT_TEST( testAddingForce )
	{
		SolidBoxFixture f;

		Force force;
		force.type = GLOBAL_FORCE;
		force.vec = Vec3r( 0, 200, 0 );
		force.duration = 0.1;
		f.s->addForce( force );

		Point3r p = f.s->getPosition();

		f.sim->simulate( 1 );

		QT_CHECK_GREATER( f.s->getPosition() [ 1 ], p[ 1 ] );
	}

	QT_TEST( testZeroForces )
	{
		SolidBoxFixture f;

		Force force;
		force.type = GLOBAL_FORCE;
		force.vec = Vec3r( 0, 200, 0 );
		force.duration = 0.1;
		f.s->addForce( force );

		f.s->zeroForces();

		Point3r p = f.s->getPosition();

		f.sim->simulate( 1 );

		QT_CHECK_LESS( f.s->getPosition() [ 1 ], p[ 1 ] );
	}

	QT_TEST( testRotationViaIdentity )
	{
		SolidBoxFixture f;

		Matrix44r m;
		m.makeIdentity();

		f.s->setTransform( m );
		Matrix44r current = f.s->getTransform();

		QT_CHECK_EQUAL( current, m );
	}

	QT_TEST( setMass_default )
	{
		SolidBoxFixture f;
		Matrix44r m = f.s->getInertiaTensor();

		Mass mass;
		//f.s->setMass( mass, Matrix44r() );
		f.s->setMass(mass);

		Matrix44r m_set = f.s->getInertiaTensor();
		QT_CHECK_EQUAL( m, m_set );
		QT_CHECK_CLOSE( f.s->getMass(), mass.mass, 0.001 );

		mass.mass = 2;
		//f.s->setMass( mass, Matrix44r() );
		f.s->setMass(mass);

		QT_CHECK_CLOSE( f.s->getMass(), mass.mass, 0.001 );
	}

	QT_TEST( setQuaternion_Q )
	{
		SolidBoxFixture f;
		Quaternion q = f.s->getQuaternion();
		f.s->setQuaternion( q );
		QT_CHECK_EQUAL( q, f.s->getQuaternion() );

		q.set( 1, 0, 0, 0 );
		f.s->setQuaternion( q );
		QT_CHECK_EQUAL( q, f.s->getQuaternion() );

		q.set( 0, 0, 0, 1 );
		f.s->setQuaternion( q );
		QT_CHECK_EQUAL( q, f.s->getQuaternion() );
	}

	QT_TEST( capsule_ODE_crash )
	{
		SolidFixture f;
		CapsuleShapeData data;
		data.length = 70;
		data.radius = 10;
		f.s->addShape( data );

		f.sim->simulate( 1 );
	}

	class TwoSolidFixture
	{
		public:
			TwoSolidFixture()
			{
				sim = createSimulator();
				s0 = sim->createSolid();
				s1 = sim->createSolid();
			}

			~TwoSolidFixture()
			{
				sim->destroySolid( s0 );
				sim->destroySolid( s1 );
				sim->destroy();
			}

			Simulator * sim;
			Solid * s0;
			Solid * s1;
	};

	class MockCollision : public CollisionEventHandler
	{
		public:
			MockCollision()
			{
				calls = 0;
			}

			void OPAL_CALL handleCollisionEvent( const CollisionEvent & e )
			{
				++calls;
			}

			int calls;
	};

	QT_TEST( same_contact_group_no_collision_but_events )
	{
		TwoSolidFixture f;
		{
			SphereShapeData data;
			data.radius = 1;
			data.contactGroup = 1;
			f.s0->addShape( data );
			f.s1->addShape( data );
		}
		MockCollision * c = new MockCollision( );
		f.s0->setCollisionEventHandler( c );
		f.s1->setCollisionEventHandler( c );

		f.s0->setPosition( 0, 0, 0 );
		f.s1->setPosition( 0, 0, 0 );

		f.sim->setupContactGroups( 1, 1, false );

		c->calls = 0;
		f.sim->simulate( 1 );

		QT_CHECK_EQUAL( f.s0->getPosition(), Point3r( 0, 0, 0 ) );
		QT_CHECK_EQUAL( f.s1->getPosition(), Point3r( 0, 0, 0 ) );

		QT_CHECK_GREATER( c->calls, 0 );
	}

	QT_TEST( same_contact_group_with_collision_and_events )
	{
		TwoSolidFixture f;
		{
			SphereShapeData data;
			data.radius = 1;
			data.contactGroup = 1;
			f.s0->addShape( data );
			f.s1->addShape( data );
		}
		MockCollision * c = new MockCollision( );
		f.s0->setCollisionEventHandler( c );
		f.s1->setCollisionEventHandler( c );

		f.s0->setPosition( 0, 0, 0 );
		f.s1->setPosition( 0, 0, 0 );

		f.sim->setupContactGroups( 1, 1, true );

		c->calls = 0;
		f.sim->simulate( 1 );

		QT_CHECK_NOT_EQUAL( f.s0->getPosition(), Point3r( 0, 0, 0 ) );
		QT_CHECK_NOT_EQUAL( f.s1->getPosition(), Point3r( 0, 0, 0 ) );

		QT_CHECK_GREATER( c->calls, 0 );
	}

	QT_TEST( shapelist_update )
	{
		SolidFixture f;
		f.s->clearShapes();

		{
			SphereShapeData data;
			data.radius = 1;
			data.contactGroup = 1;
			f.s->addShape( data );
		}

		QT_CHECK_EQUAL( f.s->getData().getNumShapes(), 1 );
		QT_CHECK_EQUAL( f.s->getData().getShapeData( 0 ) ->contactGroup, 1 );

		f.s->clearShapes();
		QT_CHECK_EQUAL( f.s->getData().getNumShapes(), 0 );

		{
			SphereShapeData data;
			data.radius = 1;
			data.contactGroup = 2;
			f.s->addShape( data );
		}

		QT_CHECK_EQUAL( f.s->getData().getNumShapes(), 1 );
		QT_CHECK_EQUAL( f.s->getData().getShapeData( 0 ) ->contactGroup, 2 );
	}

	QT_TEST( is_moving )
	{
		Simulator * sim = createSimulator();
		sim->setGravity( Vec3r( 0, -10, 0 ) );

		Solid * box = sim->createSolid();
		{
			BoxShapeData data;
			data.dimensions = Vec3r( 1, 1, 1 );
			data.material.density = 1;
			box->addShape( data );
		}

		// true at first no matter what
		QT_CHECK_EQUAL( box->isMoving(), true );

		// but then turns to false
		QT_CHECK_EQUAL( box->isMoving(), false );

		// wake and dynamic
		sim->simulate( 1 );
		QT_CHECK_EQUAL( box->isMoving(), true );
		QT_CHECK_EQUAL( box->isMoving(), false );

		// static but manual move
		box->setStatic( true );
		QT_CHECK_EQUAL( box->isMoving(), false );
		box->setPosition( 0, 100, 0 );
		QT_CHECK_EQUAL( box->isMoving(), true );
		QT_CHECK_EQUAL( box->isMoving(), false );

		// asleep
		box->setStatic( false );
		box->setSleeping( true );
		sim->simulate( 1 );
		QT_CHECK_EQUAL( box->isMoving(), false );

		sim->destroySolid( box );
		sim->destroy();
	}

	class MockMovement : public MovementEventHandler
	{
		public:
			MockMovement()
			{
				events = 0;
			}

			void OPAL_CALL handleMovementEvent( const MovementEvent & move )
			{
				++events;
				lastSolid = move.solid;
			}

			unsigned long events;
			Solid * lastSolid;
	};

	QT_TEST( is_moving_callback )
	{
		Simulator * sim = createSimulator();
		sim->setStepSize( 0.01 );
		sim->setGravity( Vec3r( 0, -10, 0 ) );

		Solid * box = sim->createSolid();
		{
			BoxShapeData data;
			data.dimensions = Vec3r( 1, 1, 1 );
			data.material.density = 1;
			box->addShape( data );
		}

		auto_ptr< MockMovement > callback( new MockMovement() );
		box->setMovementEventHandler( callback.get() );
		QT_CHECK_EQUAL( box->getMovementEventHandler(), callback.get() );

		// wake and dynamic
		sim->simulate( 0.01 );
		QT_CHECK_EQUAL( callback->events, 1 );
		QT_CHECK_EQUAL( callback->lastSolid, box );

		// static but manual move
		box->setStatic( true );
		sim->simulate( 0.01 );
		QT_CHECK_EQUAL( callback->events, 1 );
		box->setPosition( 0, 100, 0 );
		sim->simulate( 0.01 );
		QT_CHECK_EQUAL( callback->events, 2 );

		// asleep
		box->setStatic( false );
		box->setSleeping( true );
		sim->simulate( 1 );
		QT_CHECK_EQUAL( callback->events, 2 );

		sim->destroy();
	}
}
