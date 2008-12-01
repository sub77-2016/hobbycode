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

#ifndef OPAL_MOTOR_DATA_H
#define OPAL_MOTOR_DATA_H

#include "Defines.h"

namespace opal
{
	/// The types of Motors currently available.
	enum MotorType
	{
		ATTRACTOR_MOTOR,
		GEARED_MOTOR,
		SERVO_MOTOR,
		SPRING_MOTOR,
		THRUSTER_MOTOR,
        VELOCITY_MOTOR
	};

	/// A data structure describing a Motor.
	class MotorData
	{
	public:
		MotorData()
		{
			// The initial type doesn't matter since the abstract base 
			// class will never be instantiated.
			mType = ATTRACTOR_MOTOR;
			enabled = defaults::motor::enabled;
			name = "";
		}

		virtual ~MotorData()
		{
		}

		/// Returns the Motor's type.
		virtual MotorType getType()const
		{
			return mType;
		}

		/// True if the Motor is enabled.
		bool enabled;

		/// An identifier for the Motor.  
		std::string name;

	protected:
		/// The Motor type.
		MotorType mType;

	private:
	};
}

#endif
