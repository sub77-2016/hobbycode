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

#ifndef OPAL_CYLINDER_SHAPE_DATA_H
#define OPAL_CYLINDER_SHAPE_DATA_H

#include "Defines.h"
#include "ShapeData.h"

namespace opal
{
	/// A data structure describing a cylinder Shape.  Cylinders start out 
	/// aligned along their local Z axis.
	class CylinderShapeData : public ShapeData
	{
	public:
		CylinderShapeData()
		: ShapeData()
		{
			mType = CYLINDER_SHAPE;
			radius = defaults::shape::cylinderRadius;
			length = defaults::shape::cylinderLength;
		}

		/// Copy constructor.
		CylinderShapeData(const CylinderShapeData& data)
		{
			(*this) = data;
		}

		virtual ~CylinderShapeData()
		{
		}

		/// Makes a deep copy.
		virtual void operator=(const CylinderShapeData& data)
		{
			mType = data.mType;
			offset = data.offset;
			material = data.material;
			contactGroup = data.contactGroup;
			radius = data.radius;
			length = data.length;
		}

		virtual void getLocalAABB(real aabb[6])
		{
			// The standard initial cylinder orientation in OPAL is to 
			// align it along the Z axis.
			aabb[0] = -radius;
			aabb[1] = radius;
			aabb[2] = -radius;
			aabb[3] = radius;
			aabb[4] = -(real)0.5 * length;
			aabb[5] = (real)0.5 * length;
		}

		/// The cylinder's radius.
		real radius;

		/// The cylinder's length.
		real length;

	protected:

	private:
	};
}

#endif
 
