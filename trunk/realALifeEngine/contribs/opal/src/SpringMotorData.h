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

#ifndef OPAL_SPRING_MOTOR_DATA_H
#define OPAL_SPRING_MOTOR_DATA_H

#include "Defines.h"
#include "MotorData.h"

namespace opal
{
	/// The different SpringMotor modes of operation.  
	enum SpringMotorMode
	{
		/// Makes the Motor work to achieve a desired position.  
		LINEAR_MODE,

		/// Makes the Motor work to achieve a desired orientation.  
		ANGULAR_MODE,

		/// Makes the Motor work to achieve a desired position and 
		/// orientation.
		LINEAR_AND_ANGULAR_MODE
	};

	/// A data structure describing a SpringMotor.
	class SpringMotorData : public MotorData
	{
	public:
		SpringMotorData()
		: MotorData()
		{
			mType = SPRING_MOTOR;
			solid = NULL;
			internal_solidIndex = 0;
			solidBlueprintRefName = "";
			mode = LINEAR_MODE;
			// "attachOffset" is initialized in its own constructor.
			// "desiredPos" is initialized in its own constructor.
			desiredForward = defaults::motor::spring::desiredForward;
			desiredUp = defaults::motor::spring::desiredUp;
			desiredRight = defaults::motor::spring::desiredRight;
			linearKd = defaults::motor::spring::linearKd;
			linearKs = defaults::motor::spring::linearKs;
			angularKd = defaults::motor::spring::angularKd;
			angularKs = defaults::motor::spring::angularKs;
		}

		/// Copy constructor.
		SpringMotorData(const SpringMotorData& data)
		{
			(*this) = data;
		}

		virtual ~SpringMotorData()
		{
		}

		/// Makes a deep copy.
		virtual void operator=(const SpringMotorData& data)
		{
			mType = data.mType;
			enabled = data.enabled;
			name = data.name;
			solid = data.solid;
			internal_solidIndex = data.internal_solidIndex;
			solidBlueprintRefName = data.solidBlueprintRefName;
			mode = data.mode;
			attachOffset = data.attachOffset;
			desiredPos = data.desiredPos;
			desiredForward = data.desiredForward;
			desiredUp = data.desiredUp;
			desiredRight = data.desiredRight;
			linearKd = data.linearKd;
			linearKs = data.linearKs;
			angularKd = data.angularKd;
			angularKs = data.angularKs;
		}

		/// Pointer to the Solid affected by this Motor.  
		Solid* solid;

		/// Internal data used for Blueprint instantiation.
		int internal_solidIndex;

		/// String used for for intra-Blueprint references.
		std::string solidBlueprintRefName;

		/// The Motor's mode of operation.  
		SpringMotorMode mode;

		/// Local offset (relative to the Solid's position) that defines 
		/// where the SpringMotor attaches to the Solid.  This is only 
		/// used in linear mode.
		Point3r attachOffset;

		/// The Motor's desired position.  This is only used if the 
		/// appropriate mode is set.
		Point3r desiredPos;

		/// The Motor's desired forward direction, part of the desired 
		/// orientation.  This is only used if the appropriate mode is set.
		Vec3r desiredForward;

		/// The Motor's desired up direction, part of the desired 
		/// orientation.  This is only used if the appropriate mode is set.
		Vec3r desiredUp;

		/// The Motor's desired right direction, part of the desired 
		/// orientation.  This is only used if the appropriate mode is set.
		Vec3r desiredRight;

		/// The damping constant for linear mode.  
		real linearKd;

		/// The spring constant for linear mode.  
		real linearKs;

		/// The damping constant for angular mode.  
		real angularKd;

		/// The spring constant for angular mode.  
		real angularKs;

	protected:

	private:
	};
}

#endif
