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

#ifndef OPAL_SOLID_DATA_H
#define OPAL_SOLID_DATA_H

#include "Defines.h"
#include "ShapeData.h"

namespace opal
{
	/// A data structure describing a Solid.
	class SolidData
	{
	public:
		OPAL_DECL SolidData();

		/// Copy constructor.
		OPAL_DECL SolidData(const SolidData& data);

		OPAL_DECL virtual ~SolidData();

		/// Adds a new Shape to the SolidData.  This automatically 
		/// allocates the right ShapeData type.
		OPAL_DECL virtual void OPAL_CALL addShape(const ShapeData& data);

		/// Returns the number of Shapes in this SolidData.
		OPAL_DECL virtual unsigned int OPAL_CALL getNumShapes()const;

		/// Returns a pointer to the ShapeData at the given index.
		OPAL_DECL virtual ShapeData* OPAL_CALL getShapeData(
			unsigned int i)const;

		/// Destroys all Shapes in the SolidData.
		OPAL_DECL virtual void OPAL_CALL destroyShapes();

		/// Makes a deep copy.
		OPAL_DECL virtual void OPAL_CALL operator=(const SolidData& data);

		/// Determines whether the Solid is enabled.
		bool enabled;

		/// An identifier for the Solid.
		std::string name;

		/// Determines whether the Solid is sleeping.
		bool sleeping;

		/// The Solid's sleepiness level which determines how fast the 
		/// Solid falls asleep.
		real sleepiness;

		/// Determines whether the Solid is static.
		bool isStatic;

		/// Determines whether gravity is enabled for this Solid.
		bool gravityEnabled;

		/// The Solid's transform relative to the global origin.
		Matrix44r transform;

		/// The Solid's linear velocity in global coordinates.
		Vec3r globalLinearVel;

		/// The Solid's angular velocity in global coordinates.
		Vec3r globalAngularVel;

		/// The amount of damping applied to the Solid's linear motion.
		real linearDamping;

		/// The amount of damping applied to the Solid's angular motion.
		real angularDamping;
		
	protected:
		/// Pointers to the Solid's Shape data.
		std::vector<ShapeData*> mShapes;

	private:
	};
}

#endif
