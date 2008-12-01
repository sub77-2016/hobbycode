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

#ifndef OPAL_SHAPE_DATA_H
#define OPAL_SHAPE_DATA_H

// project headers
#include "Defines.h"
#include "Matrix44r.h"

namespace opal
{
	/// The types of Shapes currently available.
	enum ShapeType
	{
		BOX_SHAPE,
		SPHERE_SHAPE,
		CAPSULE_SHAPE,
		CYLINDER_SHAPE,
		PLANE_SHAPE,
		//RAY_SHAPE,
		MESH_SHAPE
	};

	/// A data structure describing a Shape.  Note that there is no other
	/// class associated with Shapes besides this one.
	class ShapeData
	{
	public:
		ShapeData()
		{
			// The initial type doesn't matter since the abstract base
			// class will never be instantiated.
			mType = SPHERE_SHAPE;
			// Leave the offset as an identity matrix.
			material = defaults::shape::material;
			contactGroup = defaults::shape::contactGroup;
		}

		virtual ~ShapeData()
		{
		}

		/// Returns the ShapeData's type.
		ShapeType getType()const
		{
			return mType;
		}

		/// Returns the axis-aligned bounding box of the ShapeData relative
		/// to its center.  The 'aabb' array stores data in the following
		/// order: min x, max x, min y, max y, min z, max z.
		virtual void getLocalAABB(real aabb[6]) = 0;

		/// The offset transform from a Solid's transform.
		Matrix44r offset;

		/// The Shape's material.
		Material material;

		/// The Shape's contact group.  If this exceeds 31, bad things
		/// might happen since there are only 32 groups.
		unsigned int contactGroup;

	protected:
		/// The Shape type.
		ShapeType mType;

	private:
	};
}

#endif

