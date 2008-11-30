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

#ifndef OPAL_SAMPLES_ENTITY_H
#define OPAL_SAMPLES_ENTITY_H

//#include <math.h>
#include <opal/opal.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

namespace opalSamples
{
	/// A drawable object that can be attached to an OPAL Solid.
	class Entity
	{
	public:
		Entity()
		{
			mSolid = NULL;
			mColor[0] = 0.5f;
			mColor[1] = 0.5f;
			mColor[2] = 0.5f;
			mColor[3] = 1.0f;
		}

		virtual ~Entity()
		{}

		/// Draws the object.  Does nothing until after being attached to a 
		/// Solid.
		virtual void draw()
		{
			if (!mSolid)
			{
				// Don't do anything if the Entity hasn't been attached 
				// to a Solid yet.
				return;
			}

			// Set the material color for this object.
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mColor);

			glPushMatrix();
				// Move to the Solids position/orientation.
				glMultMatrixf(mSolid->getTransform().getData());

				// Apply the offset from the Solid's transform.
				glMultMatrixf(mOffset.getData());

				// Draw the actual geometry.
				drawGeometry();
			glPopMatrix();
		}

		/// Attaches the Entity to an OPAL Solid.  The offset can be used to 
		/// draw the Entity away from the Solid's center (e.g. when multiple 
		/// Entities are attached to a single Solid, possibly to represent 
		/// multiple physical Shapes).
		void attachToSolid(opal::Solid* solid, const opal::Matrix44r& offset = 
			opal::Matrix44r())
		{
			mSolid = solid;
			mOffset = offset;
		}

		void setColor(float red, float green, float blue)
		{
			mColor[0] = red;
			mColor[1] = green;
			mColor[2] = blue;
		}

	protected:
		/// Draws the actual geometry used for the Entity's visual 
		/// representation.
		virtual void drawGeometry() = 0;

	private:
		/// Pointer to the Solid to which this Entity is attached.
		opal::Solid* mSolid;

		/// The offset of this Entity from the Solid's base transform.
		opal::Matrix44r mOffset;

		/// The Entity's color.
		float mColor[4];
	};
}

#endif
