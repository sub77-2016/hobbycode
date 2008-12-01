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

#ifndef OPAL_SERVO_MOTOR_H
#define OPAL_SERVO_MOTOR_H

#include "Defines.h"
#include "Motor.h"
#include "Joint.h"
#include "ServoMotorData.h"

namespace opal
{
	/// This is a Motor with an internal feedback loop, allowing 
	/// precise positioning control with minimal overshooting.   This 
	/// Motor only works on rotational Joint axes.  It 
	/// controls a single Joint axis.  Depending on the desired mode of 
	/// operation, it tries to achieve a desired angle or angular velocity 
	/// using up to a limited maximum force.
	class ServoMotor : public Motor
	{
	public:
		ServoMotor();

		virtual ~ServoMotor();

		/// Sets up the Motor to affect one a Joint's axes.  This will 
		/// enable the Motor.  Calling this more than once will detach the 
		/// Motor from its Joint and attach it to a new Joint.
		//virtual void OPAL_CALL init(Mode m, Joint* j, int axisNum);

		/// Initializes the Motor with the given data structure.  Joint 
		/// pointer in the data must be valid.
		virtual void OPAL_CALL init(const ServoMotorData& data);

		/// Returns all data describing the Motor.
		virtual const ServoMotorData& OPAL_CALL getData()const;

		virtual MotorType OPAL_CALL getType()const;

		virtual void OPAL_CALL setName(const std::string& name);

		virtual const std::string& OPAL_CALL getName()const;

		virtual bool OPAL_CALL isEnabled()const;

		virtual void OPAL_CALL setEnabled(bool e);

		/// Sets the desired angle to a value between the Joint axis' 
		/// limits.
		/// Clamps given angle into allowed range for the joint.
		virtual void OPAL_CALL setDesiredAngle(real a);

		/// Sets the desired angle to a value between 0.0 and 1.0 which 
		/// will be mapped to the Joint axis' limits.
		/// Clamps given angle into [0,1] range.
		virtual void OPAL_CALL setDesiredAngleNorm(real a);

		/// Gets the desired angle, a value between the Joint axis' 
		/// limits.
		virtual real OPAL_CALL getDesiredAngle()const;

		/// Sets the desired velocity.  
		virtual void OPAL_CALL setDesiredVel(real vel);

		/// Returns the desired velocity.  
		virtual real OPAL_CALL getDesiredVel()const;

		/// Sets the maximum amount of torque this Motor can use.  
		virtual void OPAL_CALL setMaxTorque(real max);

		/// Returns the maximum amount of torque this Motor can use.  
		virtual real OPAL_CALL getMaxTorque()const;

		/// Sets the restore speed, the parameter used to scale how fast 
		/// the Motor will achieve its desired position.  Only used in the 
		/// desired position mode.
		virtual void OPAL_CALL setRestoreSpeed(real speed);

		/// Returns the restore speed.  
		virtual real OPAL_CALL getRestoreSpeed()const;

		virtual void internal_update();

		virtual bool internal_dependsOnJoint(Joint* j);

	protected:
		/// Stores data describing the Motor.
		ServoMotorData mData;

	private:

	};
}

#endif
