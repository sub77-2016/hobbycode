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

#ifndef OPAL_SPHERE_SHAPE_DATA_H
#define OPAL_SPHERE_SHAPE_DATA_H

#include "Defines.h"
#include "ShapeData.h"

namespace opal
{
	/// A data structure describing a sphere Shape.
	class SphereShapeData : public ShapeData
	{
	public:
		SphereShapeData()
		: ShapeData()
		{
			mType = SPHERE_SHAPE;
			radius = defaults::shape::sphereRadius;
		}

		/// Copy constructor.
		SphereShapeData(const SphereShapeData& data)
		{
			(*this) = data;
		}

		virtual ~SphereShapeData()
		{
		}

		/// Makes a deep copy.
		virtual void operator=(const SphereShapeData& data)
		{
			mType = data.mType;
			offset = data.offset;
			material = data.material;
			contactGroup = data.contactGroup;
			radius = data.radius;
		}

		virtual void getLocalAABB(real aabb[6])
		{
			aabb[0] = -radius;
			aabb[1] = radius;
			aabb[2] = -radius;
			aabb[3] = radius;
			aabb[4] = -radius;
			aabb[5] = radius;
		}

		/// The sphere's radius.
		real radius;

	protected:

	private:
	};
}

#endif
