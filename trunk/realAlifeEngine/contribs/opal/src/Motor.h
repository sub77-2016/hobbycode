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

#ifndef OPAL_MOTOR_H
#define OPAL_MOTOR_H

#include "Defines.h"
#include "MotorData.h"

namespace opal
{
	class Solid;
	class Joint;

	/// A device that can be attached to Solids and/or Joints to affect 
	/// them in a variety of ways.  The point of Motors is to give users 
	/// high-level systems for controlling various parts of the simulation.  
	/// For example, instead of manually applying Forces to a robot arm to 
	/// attain a desired state, users should use a Motor that takes a 
	/// desired position or velocity, automatically applying forces every 
	/// time step to attain that state.  Most Motors remain ineffective 
	/// until they are initialized.
	class Motor
	{
	public:
		Motor();

		/// Sets whether the Motor has any effect.
		virtual void OPAL_CALL setEnabled(bool e) = 0;

		/// Returns true if the Motor is enabled.  
		virtual bool OPAL_CALL isEnabled()const = 0;

		/// Set the user data pointer to some external data.  The user data 
		/// is totally user-managed
		/// (i.e. it is not destroyed when the Motor is destroyed).
		virtual void OPAL_CALL setUserData(void* data);

		/// Returns the user data pointer (NULL if it has not been set).  
		virtual void* OPAL_CALL getUserData();

		/// Returns the Motor type.  
		virtual MotorType OPAL_CALL getType()const = 0;

		/// Sets the Motor's name.
		virtual void OPAL_CALL setName(const std::string& name) = 0;

		/// Returns the Motor's name.
		virtual const std::string& OPAL_CALL getName()const = 0;

		/// Called regularly to update the Motor.  This does nothing if the 
		/// Motor is disabled.
		virtual void internal_update() = 0;

		/// Returns true if this Motor depends on the given Solid.  
		virtual bool internal_dependsOnSolid(Solid* s);

		/// Returns true if this Motor depends on the given Joint.  
		virtual bool internal_dependsOnJoint(Joint* j);

		/// Sets whether this is a custom Motor.  Used internally to 
		/// track custom Motors which must not be destroyed by OPAL.
		//void internal_setCustom(bool c);

		/// Returns true if this is a custom Motor.  
		//bool internal_isCustom();

		/// Internal function used to destroy this object.
		void internal_destroy();

	protected:
		virtual ~Motor();

		/// Called by subclasses when they are initialized.  
		void init();

		/// Pointer to user data.  This is totally user-managed (i.e. OPAL 
		/// will never delete it).  
		void* mUserData;

		/// True if this is a custom Motor.  This is used to ensure that 
		/// custom Motors are not destroyed by OPAL.
		//bool mIsCustom;

		/// True if the Motor has been initialized.  Some Motors use 
		/// this to take special actions when a Motor is reinitialized.
		bool mInitCalled;

	private:

	};
}

#endif
