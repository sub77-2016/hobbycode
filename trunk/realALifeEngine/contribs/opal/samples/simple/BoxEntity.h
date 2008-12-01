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

#ifndef OPAL_SAMPLES_BOX_ENTITY_H
#define OPAL_SAMPLES_BOX_ENTITY_H

#include "Entity.h"

namespace opalSamples
{
	/// A drawable box object that can be attached to an OPAL Solid.
	class BoxEntity : public Entity
	{
	public:
		BoxEntity()
		{
			mDimensions.set(1, 1, 1);
		}

		virtual ~BoxEntity()
		{}

		void setDimensions(const opal::Vec3r& dim)
		{
			mDimensions = dim;
		}

		virtual void drawGeometry()
		{
			opal::real halfx = (opal::real)0.5 * mDimensions[0];
			opal::real halfy = (opal::real)0.5 * mDimensions[1];
			opal::real halfz = (opal::real)0.5 * mDimensions[2];

			glBegin(GL_QUADS);
				// Left side.
				glNormal3f(-1, 0, 0);
				glVertex3f(-halfx, -halfy, halfz);
				glVertex3f(-halfx, halfy, halfz);
				glVertex3f(-halfx, halfy, -halfz);
				glVertex3f(-halfx, -halfy, -halfz);

				// Back side.
				glNormal3f(0, 0, -1);
				glVertex3f(-halfx, -halfy, -halfz);
				glVertex3f(-halfx, halfy, -halfz);
				glVertex3f(halfx, halfy, -halfz);
				glVertex3f(halfx, -halfy, -halfz);

				// Right side.
				glNormal3f(1, 0, 0);
				glVertex3f(halfx, -halfy, -halfz);
				glVertex3f(halfx, halfy, -halfz);
				glVertex3f(halfx, halfy, halfz);
				glVertex3f(halfx, -halfy, halfz);

				// Front side.
				glNormal3f(0, 0, 1);
				glVertex3f(halfx, -halfy, halfz);
				glVertex3f(halfx, halfy, halfz);
				glVertex3f(-halfx, halfy, halfz);
				glVertex3f(-halfx, -halfy, halfz);

				// Top side.
				glNormal3f(0, 1, 0);
				glVertex3f(halfx, halfy, halfz);
				glVertex3f(halfx, halfy, -halfz);
				glVertex3f(-halfx, halfy, -halfz);
				glVertex3f(-halfx, halfy, halfz);

				// Bottom side.
				glNormal3f(0, -1, 0);
				glVertex3f(-halfx, -halfy, halfz);
				glVertex3f(-halfx, -halfy, -halfz);
				glVertex3f(halfx, -halfy, -halfz);
				glVertex3f(halfx, -halfy, halfz);
			glEnd();
		}

	protected:
		/// The visual box's dimensions.
		opal::Vec3r mDimensions;
	};
}

#endif
