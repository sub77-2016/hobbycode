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

#ifndef OPAL_GEARED_MOTOR_DATA_H
#define OPAL_GEARED_MOTOR_DATA_H

#include "Defines.h"
#include "MotorData.h"

namespace opal
{
	/// A data structure describing a GearedMotor.
	class GearedMotorData : public MotorData
	{
	public:
		GearedMotorData()
		: MotorData()
		{
			mType = GEARED_MOTOR;
			joint = NULL;
			internal_jointIndex = 0;
			jointBlueprintRefName = "";
			jointAxisNum = 0;
			maxTorque = defaults::motor::geared::maxTorque;
			maxVelocity = defaults::motor::geared::maxVelocity;
			throttle = 0;
		}

		/// Copy constructor.
		GearedMotorData(const GearedMotorData& data)
		{
			(*this) = data;
		}

		virtual ~GearedMotorData()
		{
		}

		/// Makes a deep copy.
		virtual void operator=(const GearedMotorData& data)
		{
			mType = data.mType;
			enabled = data.enabled;
			name = data.name;
			joint = data.joint;
			internal_jointIndex = data.internal_jointIndex;
			jointBlueprintRefName = data.jointBlueprintRefName;
			jointAxisNum = data.jointAxisNum;
			maxTorque = data.maxTorque;
			maxVelocity = data.maxVelocity;
			throttle = data.throttle;
		}

		/// Pointer to the Joint affected by this Motor.
		Joint* joint;

		/// Internal data used for Blueprint instantiation.
		int internal_jointIndex;

		/// String used for for intra-Blueprint references.
		std::string jointBlueprintRefName;

		/// The index of the specific Joint axis affected by this Motor.
		int jointAxisNum;

		/// The maximum amount of torque that can be applied to the Joint 
		/// axis.
		real maxTorque;

		/// The maximum angular velocity that can be achieved by the Motor.
		real maxVelocity;

		/// The throttle (i.e. the "gas") affects how much of the available 
		/// torque is applied to the Joint axis.
		real throttle;

	protected:

	private:
	};
}

#endif
