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

namespace testBlueprint
{
	QT_TEST( manual_addition_solid )
	{
		// solidmap seems to be corrups sometimes, let's test this
		Simulator * sim = createSimulator();
		Solid * s = sim->createSolid();
		s->setName( "baba" );
		BlueprintInstance * b = new BlueprintInstance();
		b->addSolid( s );

		QT_CHECK_EQUAL( b->getSolid( "baba" ), s );
		QT_CHECK_EQUAL( b->getSolid( 0 ), s );

		delete b;

		sim->destroy();
	}

	QT_TEST( create_and_delete )
	{
		Simulator * sim = createSimulator();

		// Create a SolidData object.
		SolidData solid1Data;
		solid1Data.name = "object";
		BoxShapeData boxData;
		solid1Data.addShape( boxData );

		// Setup a Blueprint.
		Blueprint bp;
		bp.addSolid( &solid1Data );
		bp.finalize();

		sim->simulate( 0.2 );
		QT_CHECK_EQUAL( sim->getNumSolids(), 0 );

		{
			// Instantiate the Blueprint.
			BlueprintInstance * instance = new BlueprintInstance();
			sim->instantiateBlueprint( *instance, bp );
			QT_CHECK_EQUAL( sim->getNumSolids(), 1 );
			unsigned int nsolids = instance->getNumSolids();
			for ( unsigned int i = 0; i < nsolids; ++i )
			{
				Solid * tobedetached = instance->getSolid( i );
				sim->destroySolid( instance->detachSolid( tobedetached->getName() ) );
			}

			delete instance;
		}

		sim->simulate( 0.2 );
		QT_CHECK_EQUAL( sim->getNumSolids(), 0 );

		sim->destroy();
	}

	class MockCollisionHandler : public CollisionEventHandler
	{
		public:
			MockCollisionHandler() {}

			virtual void OPAL_CALL handleCollisionEvent(const CollisionEvent & e)
			{
				// nothing
			}
	};

	QT_TEST( create_and_delete_with_callback )
	{
		Simulator * sim = createSimulator();

		// Create a SolidData object.
		SolidData solid1Data;
		solid1Data.name = "object";
		BoxShapeData boxData;
		solid1Data.addShape( boxData );

		// Setup a Blueprint.
		Blueprint bp;
		bp.addSolid( &solid1Data );
		bp.finalize();

		sim->simulate( 0.2 );
		QT_CHECK_EQUAL( sim->getNumSolids(), 0 );

		{
			// Instantiate the Blueprint.
			BlueprintInstance instance;
			sim->instantiateBlueprint( instance, bp );
			QT_CHECK_EQUAL( sim->getNumSolids(), 1 );

			MockCollisionHandler * callback = new MockCollisionHandler();
			instance.getSolid( "object" ) ->setCollisionEventHandler( callback );

			sim->simulate( 0.2 );

			unsigned int nsolids = instance.getNumSolids();
			for ( unsigned int i = 0; i < nsolids; ++i )
			{
				sim->destroySolid( instance.getSolid( i ) );
			}

			delete callback;
		}

		sim->simulate( 0.2 );
		QT_CHECK_EQUAL( sim->getNumSolids(), 0 );

		sim->destroy();
	}

	//QT_TEST( loading )
	//{
	//	Simulator * sim = createSimulator();
	//	Blueprint example;

	//	// Load the Blueprint from a file.
	//	loadFile( example, "../../../../samples/testing/example.xml" );

	//	// Instantiate the Blueprint.
	//	BlueprintInstance instance;
	//	sim->instantiateBlueprint( instance, example );

	//	sim->destroy();
	//}
}
