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

namespace testVolumeSensor
{
    QT_TEST( cylinder_crash_against_mesh )
    {
        Simulator * sim = createSimulator();
        Solid * obsticle = sim->createSolid();
        obsticle->setName( "obsticle" );

        // 4 vertices
        real vertices[][ 3 ] = {{ -1, 1, -1}, { -1, 0, 1}, {1, 0, 1}, {1, 0, -1}};
        //real vertices[] = { -1, 0, -1, -1, 0, 1, 1, 0, 1, 1, 0, -1 };
        // 2 triangles
        unsigned int triangles[][ 3 ] = {{ 0, 1, 2}, {0, 2, 3}};
        //real triangles[] = { 0, 1, 2, 0, 2, 3 };

        {
            // a square
            MeshShapeData meshData;
            {
                unsigned int vertexCount = 4;
                unsigned int triangleCount = 2;

                meshData.numVertices = vertexCount;
                meshData.numTriangles = triangleCount;

                // Allocate vertex and triangle arrays.
                meshData.vertexArray = new real[ 3 * meshData.numVertices ];
                meshData.triangleArray = new unsigned int[ 3 * meshData.numTriangles ];

                // Copy vertex data.
				unsigned int i;
                for ( i = 0; i < meshData.numVertices; ++i )
                {
                    meshData.vertexArray[ i * 3 ] = vertices[ i ][ 0 ];
                    meshData.vertexArray[ i * 3 + 1 ] = vertices[ i ][ 1 ];
                    meshData.vertexArray[ i * 3 + 2 ] = vertices[ i ][ 2 ];
                }

                // Copy triangle data.
                for ( i = 0; i < meshData.numTriangles; ++i )
                {
                    meshData.triangleArray[ i * 3 ] = triangles[ i ][ 0 ];
                    meshData.triangleArray[ i * 3 + 1 ] = triangles[ i ][ 1 ];
                    meshData.triangleArray[ i * 3 + 2 ] = triangles[ i ][ 2 ];
                }
            }
			
			Mass m;
			//obsticle->setMass( m, Matrix44r() );
			obsticle->setMass(m);

            obsticle->setStatic( true );
            obsticle->addShape( meshData );
        }

        Solid * volume = sim->createSolid();
        volume->setName( "volume" );
        {
            /// @bug capsule crashes for some reason within ODE library
            //CapsuleShapeData data;
            SphereShapeData data;
            //data.length = 10;
            data.radius = 10;
			data.material.density = 1.0;
            volume->addShape( data );
        }

        VolumeSensor * sensor = sim->createVolumeSensor();
        {
            VolumeSensorData data;
            data.name = "baba";
            data.solid = volume;
            sensor->init( data );
        }

		QT_CHECK_EQUAL( sensor->getData().solid, volume );

        obsticle->setPosition( 0, 0, 0 );
        volume->setPosition( 0, 0, 10.99 );

        {
            const VolumeQueryResult & r = sensor->queryVolume( volume );
            QT_CHECK_EQUAL( r.getNumSolids(), 1 );
            QT_CHECK_EQUAL( r.getSolid( 0 ), obsticle );
        }

        volume->setPosition( 0, 0, 11.01 );
        {
            const VolumeQueryResult & r = sensor->queryVolume( volume );
            QT_CHECK_EQUAL( r.getNumSolids(), 0 );
        }

		// clean up mesh
		SolidData solidData = obsticle->getData();
		for (unsigned int i = 0; i < solidData.getNumShapes(); ++i)
		{
			MeshShapeData* meshData = static_cast<MeshShapeData *>( 
				solidData.getShapeData(i) );
			delete [] meshData->vertexArray;
			delete [] meshData->triangleArray;
		}

        sim->destroy();
    }
}
