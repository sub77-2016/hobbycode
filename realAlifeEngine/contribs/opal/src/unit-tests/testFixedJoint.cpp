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

namespace testFixedJoint
{
    class Fixture
    {
        public:
            Fixture()
            {
                sim = createSimulator();
                sim->setGravity( Vec3r( 0, 0, 0 ) );

                s1 = sim->createSolid();
                s2 = sim->createSolid();
                s1->setPosition( Point3r( 0, 0, 0 ) );
                s2->setPosition( Point3r( 0, 10, 0 ) );

                j = sim->createJoint();
            }

            ~Fixture()
            {
                sim->destroy();
            }

            Simulator * sim;
            Solid * s1;
            Solid * s2;
            Joint * j;
    };

    QT_TEST( no_shapes )
    {
        Fixture f;

        JointData jdata;
        jdata.setType( FIXED_JOINT );
        jdata.solid0 = f.s1;
        jdata.solid1 = f.s2;
        f.j->init( jdata );

        f.sim->simulate( 1 );

        QT_CHECK_CLOSE( f.s1->getPosition().x, 0, 0.001 );
        QT_CHECK_CLOSE( f.s1->getPosition().y, 0, 0.001 );
        QT_CHECK_CLOSE( f.s1->getPosition().z, 0, 0.001 );

        QT_CHECK_CLOSE( f.s2->getPosition().x, 0, 0.001 );
        QT_CHECK_CLOSE( f.s2->getPosition().y, 10, 0.001 );
        QT_CHECK_CLOSE( f.s2->getPosition().z, 0, 0.001 );
    }
    
    QT_TEST( with_shapes )
    {
        Fixture f;

        SphereShapeData data;
        data.radius = 1;
        f.s1->addShape( data );
        f.s2->addShape( data );

        JointData jdata;
        jdata.setType( FIXED_JOINT );
        jdata.solid0 = f.s1;
        jdata.solid1 = f.s2;
        f.j->init( jdata );

        f.sim->simulate( 1 );

        QT_CHECK_CLOSE( f.s1->getPosition().x, 0, 0.001 );
        QT_CHECK_CLOSE( f.s1->getPosition().y, 0, 0.001 );
        QT_CHECK_CLOSE( f.s1->getPosition().z, 0, 0.001 );

        QT_CHECK_CLOSE( f.s2->getPosition().x, 0, 0.001 );
        QT_CHECK_CLOSE( f.s2->getPosition().y, 10, 0.001 );
        QT_CHECK_CLOSE( f.s2->getPosition().z, 0, 0.001 );
    }
}
