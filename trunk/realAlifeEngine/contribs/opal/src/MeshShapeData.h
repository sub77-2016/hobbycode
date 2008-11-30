/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
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

#ifndef OPAL_MESH_SHAPE_DATA_H
#define OPAL_MESH_SHAPE_DATA_H

#include "Defines.h"
#include "ShapeData.h"

namespace opal
{
	/// A data structure describing a mesh Shape.  A mesh is represented 
	/// as an array of vertices and an array of triangles (the triangle 
	/// array contains indices referring to vertices in the vertex 
	/// array).  The data pointers to these arrays must remain valid; OPAL 
	/// simply stores pointers to the user-supplied arrays.  Additionally, 
	/// the arrays must be destroyed by the user when finished.  It is 
	/// critical that the size of the data type used in these arrays 
	/// (i.e. OPAL real) matches the size of the data type expected by 
	/// the underlying physics engine (don't mix floats with doubles).  
	/// One limitation is that other Shapes can only collide with a mesh's 
	/// surface.  There is no concept of "inside" or "outside" a mesh (like 
	/// there is with the primitive Shapes); an object that is "inside" a 
	/// closed mesh surface will not be detected as a collision.
	class MeshShapeData : public ShapeData
	{
	public:
		MeshShapeData()
		: ShapeData()
		{
			mType = MESH_SHAPE;
			vertexArray = NULL;
			numVertices = 0;
			triangleArray = NULL;
			numTriangles = 0;

			for (unsigned int i = 0; i < 6; ++i)
			{
				localAABB[i] = 0;
			}
		}

		/// Copy constructor.
		MeshShapeData(const MeshShapeData& data)
		{
			(*this) = data;
		}

		virtual ~MeshShapeData()
		{
		}

		/// Makes a shallow copy, simply copying data pointers.
		virtual void operator=(const MeshShapeData& data)
		{
			mType = data.mType;
			offset = data.offset;
			material = data.material;
			contactGroup = data.contactGroup;
			vertexArray = data.vertexArray;
			numVertices = data.numVertices;
			triangleArray = data.triangleArray;
			numTriangles = data.numTriangles;

			for (unsigned int i = 0; i < 6; ++i)
			{
				localAABB[i] = data.localAABB[i];
			}
		}

		/// This recomputes the axis-aligned bounding box from the vertex 
		/// data, so it should not be called very often.
		virtual void getLocalAABB(real aabb[6])
		{
			// Compute the AABB from the vertex data.
			if (vertexArray)
			{
				for (unsigned int i = 0; i < numVertices; ++i)
				{
					unsigned int vertexIndex = i * 3;

					// Check x component.
					if (vertexArray[vertexIndex] < localAABB[0])
					{
						localAABB[0] = vertexArray[vertexIndex];
					}
					else if (vertexArray[vertexIndex] > localAABB[1])
					{
						localAABB[1] = vertexArray[vertexIndex];
					}

					// Check y component.
					if (vertexArray[vertexIndex + 1] < localAABB[2])
					{
						localAABB[2] = vertexArray[vertexIndex + 1];
					}
					else if (vertexArray[vertexIndex + 1] > localAABB[3])
					{
						localAABB[3] = vertexArray[vertexIndex + 1];
					}

					// Check z component.
					if (vertexArray[vertexIndex + 2] < localAABB[4])
					{
						localAABB[4] = vertexArray[vertexIndex + 2];
					}
					else if (vertexArray[vertexIndex + 2] > localAABB[5])
					{
						localAABB[5] = vertexArray[vertexIndex+ 2];
					}
				}
			}

			for (unsigned int i = 0; i < 6; ++i)
			{
				aabb[i] = localAABB[i];
			}
		}

		/// Pointer to a 1-dimensional array of vertices.  This data must 
		/// be allocated/deallocated by the user.  The size of this array 
		/// must be 3 * the number of vertices because each vertex uses 3 
		/// elements in the array.  To access the ith vertex, for example, 
		/// vertexArray[i * 3] is the x component, vertexArray[i* 3 + 1] 
		/// is the y component, and vertexArray[i * 3 + 2] is the z 
		/// component.
		real* vertexArray;

		/// The number of vertices in the mesh.
		unsigned int numVertices;

		/// Pointer to a 1-dimensional array of indexed triangles.  This 
		/// data must be allocated/deallocated by the user.  The size of 
		/// this array must be 3 * the number of triangles because each 
		/// triangle uses 3 elements in the array.  Each element is an 
		/// index into the vertex array.  To access the ith triangle's 
		/// vertices, for example, triangleArray[i * 3] is the index 
		/// for the 1st vertex, triangleArray[i* 3 + 1] is the index for 
		/// the 2nd vertex, and triangleArray[i * 3 + 2] is the index 
		/// for the 3rd vertex.
		unsigned int* triangleArray;

		/// The number of triangles in the mesh.
		unsigned int numTriangles;

	protected:
		/// The mesh's local axis-aligned bounding box.  This array stores 
		/// data in the following order: min x, max x, min y, max y, min z, 
		/// max z.  This will be updated automatically when the mesh is 
		/// added to a Solid.
		real localAABB[6];

	private:
	};
}

#endif
