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

//! Testing generic Joint stuff
namespace testJoint
{
    class TwoSpheresFixture
    {
        public:
            TwoSpheresFixture()
            {
                sim = createSimulator();
                sim->setGravity( Vec3r( 0, -10, 0 ) );

                s0 = sim->createSolid();
                s1 = sim->createSolid();
                {
                    BoxShapeData data;
                    data.dimensions = Vec3r( 1, 1, 1 );
                    s0->addShape( data );
                    s1->addShape( data );
                }

                s0->setPosition( 0, 0, 0 );
                s1->setPosition( 0, 10, 0 );

                j = sim->createJoint();
            }

            ~TwoSpheresFixture()
            {
                sim->destroyJoint( j );
                sim->destroySolid( s0 );
                sim->destroySolid( s1 );
                sim->destroy();
            }

            Simulator * sim;
            Solid * s0;
            Solid * s1;
            Joint * j;
    };

    QT_TEST( wakeSolids_both_nonstatic )
    {
        TwoSpheresFixture f;

        f.s0->setStatic( false );
        f.s1->setStatic( false );
        f.s0->setSleeping( true );
        f.s1->setSleeping( true );

        JointData data;
        data.setType( UNIVERSAL_JOINT );
        data.solid0 = f.s0;
        data.solid1 = f.s1;
        f.j->init( data );

        QT_CHECK_EQUAL( f.j->isEnabled(), true );

        f.j->wakeSolids();
        QT_CHECK_EQUAL( f.s0->isSleeping(), false );
        QT_CHECK_EQUAL( f.s1->isSleeping(), false );
    }

    QT_TEST( wakeSolids_first_static )
    {
        TwoSpheresFixture f;

        f.s0->setStatic( true );
        f.s1->setStatic( false );
        f.s0->setSleeping( true );
        f.s1->setSleeping( true );

        JointData data;
        data.setType( UNIVERSAL_JOINT );
        data.solid0 = f.s0;
        data.solid1 = f.s1;
		data.axis[0].direction.set(0, 1, 0);
		data.axis[1].direction.set(1, 0, 0);
        f.j->init( data );

        QT_CHECK_EQUAL( f.j->isEnabled(), true );

        f.j->wakeSolids();
        QT_CHECK_EQUAL( f.s0->isSleeping(), true );
        QT_CHECK_EQUAL( f.s1->isSleeping(), false );
    }

    QT_TEST( wakeSolids_second_static )
    {
        TwoSpheresFixture f;

        f.s0->setStatic( false );
        f.s1->setStatic( true );
        f.s0->setSleeping( true );
        f.s1->setSleeping( true );

        JointData data;
        data.setType( UNIVERSAL_JOINT );
        data.solid0 = f.s0;
        data.solid1 = f.s1;
        f.j->init( data );

        QT_CHECK_EQUAL( f.j->isEnabled(), true );

        f.j->wakeSolids();
        QT_CHECK_EQUAL( f.s0->isSleeping(), false );
        QT_CHECK_EQUAL( f.s1->isSleeping(), true );
    }

    QT_TEST( wakeSolids_both_static )
    {
        TwoSpheresFixture f;

        f.s0->setStatic( true );
        f.s1->setStatic( true );
        f.s0->setSleeping( true );
        f.s1->setSleeping( true );

        JointData data;
        data.setType( UNIVERSAL_JOINT );
        data.solid0 = f.s0;
        data.solid1 = f.s1;
        f.j->init( data );

        QT_CHECK_EQUAL( f.j->isEnabled(), false );

        f.j->wakeSolids();
        QT_CHECK_EQUAL( f.s0->isSleeping(), true );
        QT_CHECK_EQUAL( f.s1->isSleeping(), true );
    }

    QT_TEST( workingJoint_with_first_static )
    {
        TwoSpheresFixture f;

        f.s0->setStatic( true );
        f.s1->setStatic( false );
        f.s0->setSleeping( false );
        f.s1->setSleeping( false );

        JointData data;
        data.setType( FIXED_JOINT );
        data.solid0 = f.s0;
        data.solid1 = f.s1;
        f.j->init( data );

        f.sim->simulate( 1 );

        QT_CHECK_CLOSE( f.s1->getPosition() [ 1 ], 10, 0.001 );
    }

    QT_TEST( workingJoint_with_second_static )
    {
        TwoSpheresFixture f;

        f.s0->setStatic( false );
        f.s1->setStatic( true );
        f.s0->setSleeping( false );
        f.s1->setSleeping( false );

        JointData data;
        data.setType( FIXED_JOINT );
        data.solid0 = f.s0;
        data.solid1 = f.s1;
        f.j->init( data );

        f.sim->simulate( 1 );

        QT_CHECK_CLOSE( f.s0->getPosition() [ 1 ], 0, 0.001 );
    }

    QT_TEST( setEnabled_checks_solids )
    {
        TwoSpheresFixture f;

        f.s0->setStatic( false );
        f.s1->setStatic( true );
        f.s0->setSleeping( false );
        f.s1->setSleeping( false );

        JointData data;
        data.setType( FIXED_JOINT );
        data.solid0 = f.s0;
        data.solid1 = f.s1;
        f.j->init( data );

        QT_CHECK( f.j->getSolid1() == NULL );

        Solid * temp = f.j->getSolid1();
        temp = f.s1;

        f.j->setEnabled( true );

        QT_CHECK( f.j->getSolid1() == NULL );
    }

    QT_TEST( setContacts )
    {
        Simulator * sim = createSimulator();
        Joint * j = sim->createJoint();
        JointData data;
        data.contactsEnabled = false;

        j->init( data );
        QT_CHECK_EQUAL( j->areContactsEnabled(), false );

        data.contactsEnabled = true;
        j->init( data );
        QT_CHECK_EQUAL( j->areContactsEnabled(), true );

        sim->destroy();
    }

    QT_TEST( break_settings )
    {
        Simulator * sim = createSimulator();
        Joint * j = sim->createJoint();
        JointData data;
        j->init( data );

        QT_CHECK_EQUAL( j->getBreakingMode(), UNBREAKABLE_MODE );

        sim->destroy();
    }

    QT_TEST( breaking_threshhold )
    {
        Simulator * sim = createSimulator();

        Solid * s0 = sim->createSolid();
        s0->setPosition( 0, 0, 0 );
        Solid * s1 = sim->createSolid();
        s1->setPosition( 0, 10, 0 );
        {
            SphereShapeData data;
            data.radius = 1;
            s0->addShape( data );
            s1->addShape( data );
        }

        Joint * j = sim->createJoint();
        {
            JointData data;
            data.setType( FIXED_JOINT );
            data.breakMode = THRESHOLD_MODE;
            data.breakThresh = 100;
            data.solid0 = s0;
            data.solid1 = s1;
            j->init( data );
        }

        sim->simulate( 1 );
        QT_CHECK_EQUAL( j->isBroken(), false );

        // apply a force that breaks the joint
        {
            Force f;
            f.type = GLOBAL_FORCE;
            f.duration = 1;
            f.singleStep = false;
            f.pos = Point3r( 0, 0, 0 );
            f.vec = Vec3r( 0, 100, 0 );
            s1->addForce( f );
        }

        sim->simulate( 1 );
        QT_CHECK_EQUAL( j->isBroken(), true );

        sim->destroy();
    }
}
