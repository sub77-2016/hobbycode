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

#ifndef OPAL_SENSOR_H
#define OPAL_SENSOR_H

#include "Defines.h"
#include "SensorData.h"

namespace opal
{
	/// A device that records data from a simulation.  Sensors can either be 
	/// attached to Solids or just positioned somewhere within an 
	/// environment.  Each Sensor maintains a transform matrix; depending 
	/// on whether the Sensor is attached to a Solid, the transform is 
	/// relative to the attached Solid or the global origin.  Most Sensors 
	/// remain ineffective until they are initialized.
	class Sensor
	{
	public:
		Sensor();

		/// Sets whether the Sensor can update its measurements.
		virtual void OPAL_CALL setEnabled(bool e) = 0;

		/// Returns true if the Sensor is enabled.  
		virtual bool OPAL_CALL isEnabled()const = 0;

		/// Set the user data pointer to some external data.  The user data 
		/// is totally user-managed
		/// (i.e. it is not destroyed when the Sensor is destroyed).
		virtual void OPAL_CALL setUserData(void* data);

		/// Returns the user data pointer (NULL if it has not been set).  
		virtual void* OPAL_CALL getUserData();

		/// Returns the Sensor type.  
		virtual SensorType OPAL_CALL getType()const = 0;

		/// Sets the Sensor's transform.
		virtual void OPAL_CALL setTransform(const Matrix44r& t) = 0;

		/// Returns the Sensor's transform.
		virtual const Matrix44r& OPAL_CALL getTransform()const = 0;

		/// Sets the Sensor's name.
		virtual void OPAL_CALL setName(const std::string& name) = 0;

		/// Returns the Sensor's name.
		virtual const std::string& OPAL_CALL getName()const = 0;

		/// Called regularly to update the Sensor.  This does nothing if the 
		/// Sensor is disabled.
		virtual void internal_update() = 0;

		/// Returns true if this Sensor depends on the given Solid.  
		virtual bool internal_dependsOnSolid(Solid* s);

		/// Internal function used to destroy this object.
		void internal_destroy();

	protected:
		virtual ~Sensor();

		/// Called by subclasses when they are initialized.  
		void init();

		/// Pointer to user data.  This is totally user-managed (i.e. OPAL 
		/// will never delete it).  
		void* mUserData;

		/// True if the Sensor has been initialized.
		bool mInitCalled;

	private:
	};
}

#endif

