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

#ifndef OPAL_BOX_SHAPE_DATA_H
#define OPAL_BOX_SHAPE_DATA_H

#include "Defines.h"
#include "ShapeData.h"

namespace opal
{
	/// A data structure describing a box Shape.
	class BoxShapeData : public ShapeData
	{
	public:
		BoxShapeData()
		: ShapeData()
		{
			mType = BOX_SHAPE;
			dimensions = defaults::shape::boxDimensions;
		}

		/// Copy constructor.
		BoxShapeData(const BoxShapeData& data)
		{
			(*this) = data;
		}

		virtual ~BoxShapeData()
		{
		}

		/// Makes a deep copy.
		virtual void operator=(const BoxShapeData& data)
		{
			mType = data.mType;
			offset = data.offset;
			material = data.material;
			contactGroup = data.contactGroup;
			dimensions = data.dimensions;
		}

		virtual void getLocalAABB(real aabb[6])
		{
			aabb[0] = -(real)0.5 * dimensions[0];
			aabb[1] = (real)0.5 * dimensions[0];
			aabb[2] = -(real)0.5 * dimensions[1];
			aabb[3] = (real)0.5 * dimensions[1];
			aabb[4] = -(real)0.5 * dimensions[2];
			aabb[5] = (real)0.5 * dimensions[2];
		}

		/// The box's dimensions.
		Vec3r dimensions;

	protected:

	private:
	};
}

#endif
