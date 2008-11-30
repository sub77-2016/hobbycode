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

#ifndef OPAL_THRUSTER_MOTOR_H
#define OPAL_THRUSTER_MOTOR_H

#include "Defines.h"
#include "Motor.h"
#include "ThrusterMotorData.h"

namespace opal
{
	/// This Motor provides a constant force/torque on a Solid.  Its 
	/// purpose is to simulate physical phenomena where a constant 
	/// force is applied to an object, usually found in a rocket 
	/// thruster.
	class ThrusterMotor : public Motor
	{
	public:
		ThrusterMotor();

		virtual ~ThrusterMotor();

		/// Sets up the Motor to affect a Solid.  This will enable the Motor.  
		/// Calling this more than once will detach the Motor from its Solid 
		/// and attach it to a new Solid.
		//virtual void OPAL_CALL init(Solid* solid);

		/// Initializes the Motor with the given data structure.  If the 
		/// Solid pointer in the data are NULL, the Motor will do nothing.  
		/// The Force in this data structure will automatically be set to a 
		/// "single step" Force.
		virtual void OPAL_CALL init(const ThrusterMotorData& data);

		/// Returns all data describing the Motor.
		virtual const ThrusterMotorData& OPAL_CALL getData()const;

		virtual MotorType OPAL_CALL getType()const;

		virtual void OPAL_CALL setName(const std::string& name);

		virtual const std::string& OPAL_CALL getName()const;

		virtual bool OPAL_CALL isEnabled()const;

		virtual void OPAL_CALL setEnabled(bool e);

		/// Sets the Force applied by this Motor every time step.  The 
		/// Force in this data structure will automatically be set to a 
		/// "single step" Force.
		virtual void OPAL_CALL setForce(const Force& f);

		/// Gets the Force applied by this Motor every time step.  
		virtual const Force& OPAL_CALL getForce()const;

		virtual bool internal_dependsOnSolid(Solid* s);

		virtual void internal_update();

	protected:
		/// Stores data describing the Motor.
		ThrusterMotorData mData;

	private:
	};
}

#endif
