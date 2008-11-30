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

#ifndef OPAL_SERVO_MOTOR_DATA_H
#define OPAL_SERVO_MOTOR_DATA_H

#include "Defines.h"
#include "MotorData.h"

namespace opal
{
	/// The different ServoMotor modes of operation.  
	enum ServoMotorMode
	{
		/// The ServoMotor tries to achieve a desired angle for 
		/// the Joint axis.
		DESIRED_ANGLE_MODE,

		/// The ServoMotor tries to achieve a desired velocity for 
		/// the Joint axis.
		DESIRED_VEL_MODE
	};

	/// A data structure describing a ServoMotor.
	class ServoMotorData : public MotorData
	{
	public:
		ServoMotorData()
		: MotorData()
		{
			mType = SERVO_MOTOR;
			joint = NULL;
			internal_jointIndex = 0;
			jointBlueprintRefName = "";
			mode = DESIRED_ANGLE_MODE;
			jointAxisNum = 0;
			desiredAngle = 0;
			desiredVel = 0;
			maxTorque = defaults::motor::servo::maxTorque;
			restoreSpeed = defaults::motor::servo::restoreSpeed;
		}

		/// Copy constructor.
		ServoMotorData(const ServoMotorData& data)
		{
			(*this) = data;
		}

		virtual ~ServoMotorData()
		{
		}

		/// Makes a deep copy.
		virtual void operator=(const ServoMotorData& data)
		{
			mType = data.mType;
			enabled = data.enabled;
			name = data.name;
			joint = data.joint;
			internal_jointIndex = data.internal_jointIndex;
			jointBlueprintRefName = data.jointBlueprintRefName;
			mode = data.mode;
			jointAxisNum = data.jointAxisNum;
			desiredAngle = data.desiredAngle;
			desiredVel = data.desiredVel;
			maxTorque = data.maxTorque;
			restoreSpeed = data.restoreSpeed;
		}

		/// Pointer to the Joint affected by this Motor.  
		Joint* joint;

		/// Internal data used for Blueprint instantiation.
		int internal_jointIndex;

		/// String used for for intra-Blueprint references.
		std::string jointBlueprintRefName;

		/// The Motor's mode of operation.
		ServoMotorMode mode;

		/// The index of the Joint axes affected by this Motor.
		int jointAxisNum;

		/// The Motor's desired angle.  This is only used if the 
		/// appropriate mode is set.
		real desiredAngle;

		/// The Motor's desired velocity.  This is only used if the 
		/// appropriate mode is set.		
		real desiredVel;

		/// The maximum amount of torque that can be used to help the 
		/// Joint axis achieve its desired angle or velocity.
		real maxTorque;

		/// A constant used in desired angle mode that scales how fast 
		/// the Joint axis will achieve its desired angle.
		real restoreSpeed;

	protected:

	private:
	};
}

#endif
