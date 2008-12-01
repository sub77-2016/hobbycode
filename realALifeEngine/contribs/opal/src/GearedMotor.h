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

#ifndef OPAL_GEARED_MOTOR_H
#define OPAL_GEARED_MOTOR_H

#include "Defines.h"
#include "Motor.h"
#include "GearedMotorData.h"

namespace opal
{
	/// The GearedMotor is intended to simulate an automobile engine.  
	/// This Motor only works on rotational Joint axes.  It applies a torque 
	/// to one of its degrees of freedom, but the amount of torque decreases 
	/// linearly as the angular velocity increases, similar to an automobile 
	/// engine.  The Motor's max torque is applied when angular velocity is 
	/// zero.  When the angular velocity reaches the Motor's max velocity, 
	/// zero torque is applied.  Thus, there is always more torque 
	/// available when the Joint axis is at the lower end of its velocity 
	/// range.
	class GearedMotor : public Motor
	{

	public:
		GearedMotor();

		virtual ~GearedMotor();

		/// Sets up the Motor to affect one a Joint's axes.  The axis must 
		/// be rotational.  This will enable the Motor.  Calling this more 
		/// than once will detach the Motor from its Joint and attach it 
		/// to a new Joint.
		//virtual void OPAL_CALL init(Joint* joint, int axisNum);

		/// Initializes the Motor with the given data structure.  Joint 
		/// pointer in the data must be valid.
		virtual void OPAL_CALL init(const GearedMotorData& data);

		/// Returns all data describing the Motor.
		virtual const GearedMotorData& OPAL_CALL getData()const;

		virtual MotorType OPAL_CALL getType()const;

		virtual void OPAL_CALL setName(const std::string& name);

		virtual const std::string& OPAL_CALL getName()const;

		virtual bool OPAL_CALL isEnabled()const;

		virtual void OPAL_CALL setEnabled(bool e);

		/// Sets the max torque parameter.
		virtual void OPAL_CALL setMaxTorque(real max);

		/// Returns the max torque parameter.
		virtual real OPAL_CALL getMaxTorque()const;

		/// Sets the max velocity parameter.  The maximum cannot be set to 
		/// zero.
		virtual void OPAL_CALL setMaxVelocity(real max);

		/// Returns the max velocity parameter.
		virtual real OPAL_CALL getMaxVelocity()const;

		/// Sets the throttle parameter.
		virtual void OPAL_CALL setThrottle(real t);

		/// Returns the throttle parameter.
		virtual real OPAL_CALL getThrottle()const;

		virtual void internal_update();

		virtual bool internal_dependsOnJoint(Joint* j);

	protected:
		/// Stores data describing the Motor.
		GearedMotorData mData;

	private:

	};
}

#endif
