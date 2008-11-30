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

#ifndef OPAL_PLANE_SHAPE_DATA_H
#define OPAL_PLANE_SHAPE_DATA_H

#include "Defines.h"
#include "ShapeData.h"

namespace opal
{
	/// A data structure describing a plane Shape.
	class PlaneShapeData : public ShapeData
	{
	public:
		PlaneShapeData()
		: ShapeData()
		{
			mType = PLANE_SHAPE;

			for (int i=0; i<4; ++i)
			{
				abcd[i] = defaults::shape::planeABCD[i];
			}
		}

		/// Copy constructor.
		PlaneShapeData(const PlaneShapeData& data)
		{
			(*this) = data;
		}

		virtual ~PlaneShapeData()
		{
		}

		/// Makes a deep copy.
		virtual void operator=(const PlaneShapeData& data)
		{
			mType = data.mType;
			offset = data.offset;
			material = data.material;
			contactGroup = data.contactGroup;

			for (int i=0; i<4; ++i)
			{
				abcd[i] = data.abcd[i];
			}
		}

		/// Planes don't have a bounding box, so this does nothing.
		virtual void getLocalAABB(real aabb[6])
		{
			// Do nothing.
		}

		/// Parameters used to define the plane equation: 
		/// a*x + b*y + c*z = d.
		real abcd[4];

	protected:

	private:
	};
}

#endif
