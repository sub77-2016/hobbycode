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

#ifndef OPAL_SAMPLES_SPHERE_ENTITY_H
#define OPAL_SAMPLES_SPHERE_ENTITY_H

#include "Entity.h"

namespace opalSamples
{
	/// A drawable sphere object that can be attached to an OPAL Solid.
	class SphereEntity : public Entity
	{
	public:
		SphereEntity()
		{
			mRadius = 1;
			mQuadric = gluNewQuadric();
		}

		virtual ~SphereEntity()
		{
			gluDeleteQuadric(mQuadric);
		}

		void setRadius(opal::real radius)
		{
			mRadius = radius;
		}

		virtual void drawGeometry()
		{
			gluSphere(mQuadric, mRadius, 20, 20);
		}

	protected:
		/// The visual sphere's radius.
		opal::real mRadius;

		/// A GLU quadric object used to generate curved meshes.
		GLUquadricObj* mQuadric;
	};
}

#endif
