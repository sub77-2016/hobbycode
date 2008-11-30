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

#ifndef OPAL_ACCELERATION_SENSOR_H
#define OPAL_ACCELERATION_SENSOR_H

#include "Defines.h"
#include "Sensor.h"
#include "AccelerationSensorData.h"

namespace opal
{
	class Simulator;

	/// A Sensor that monitors the linear and angular acceleration of a 
	/// Solid.  Using its transform, it can be set to a desired offset from 
	/// the attached Solid.  This Sensor does nothing if it is not attached 
	/// to a Solid (i.e. its returned values are always zero).
	class AccelerationSensor : public Sensor
	{
	public:
		AccelerationSensor(Simulator* s);

		virtual ~AccelerationSensor();

		/// Initializes the Sensor with the given data structure.  The Solid 
		/// pointer should always be valid because this Sensor only works 
		/// when attached to something.
		virtual void OPAL_CALL init(const AccelerationSensorData& data);

		/// Returns all data describing the Sensor.
		virtual const AccelerationSensorData& OPAL_CALL getData()const;

		/// Returns the Sensor's global linear acceleration.  If the Sensor 
		/// is not attached to a Solid, this returns (0, 0, 0).
		virtual Vec3r OPAL_CALL getGlobalLinearAccel()const;

		/// Returns the Sensor's global angular acceleration.  If the Sensor 
		/// is not attached to a Solid, this returns (0, 0, 0).
		virtual Vec3r OPAL_CALL getGlobalAngularAccel()const;

		/// Returns the Sensor's local linear acceleration.  If the Sensor 
		/// is not attached to a Solid, this returns (0, 0, 0).
		virtual Vec3r OPAL_CALL getLocalLinearAccel()const;

		/// Returns the Sensor's local angular acceleration.  If the Sensor 
		/// is not attached to a Solid, this returns (0, 0, 0).
		virtual Vec3r OPAL_CALL getLocalAngularAccel()const;

		virtual void OPAL_CALL setEnabled(bool e);

		virtual bool OPAL_CALL isEnabled()const;

		virtual void OPAL_CALL setTransform(const Matrix44r& t);

		virtual const Matrix44r& OPAL_CALL getTransform()const;

		virtual SensorType OPAL_CALL getType()const;

		virtual void OPAL_CALL setName(const std::string& name);

		virtual const std::string& OPAL_CALL getName()const;

		virtual void internal_update();

		virtual bool internal_dependsOnSolid(Solid* s);

	protected:
		/// Stores data describing the Sensor.
		AccelerationSensorData mData;

		/// Pointer to the Simulator containing this Sensor.  This is used 
		/// to get an accurate dt value on every update.
		Simulator* mSim;

		/// Stored copy of the current step's velocity data; used for 
		/// acceleration calculations.
		Vec3r mCurrentGlobalLinearVel;

		/// Stored copy of the current step's velocity data; used for 
		/// acceleration calculations.
		Vec3r mCurrentGlobalAngularVel;

		/// Stored copy of the current step's velocity data; used for 
		/// acceleration calculations.
		Vec3r mCurrentLocalLinearVel;

		/// Stored copy of the current step's velocity data; used for 
		/// acceleration calculations.
		Vec3r mCurrentLocalAngularVel;

		/// Stored copy of the previous step's velocity data; used for 
		/// acceleration calculations.
		Vec3r mPrevGlobalLinearVel;

		/// Stored copy of the previous step's velocity data; used for 
		/// acceleration calculations.
		Vec3r mPrevGlobalAngularVel;

		/// Stored copy of the previous step's velocity data; used for 
		/// acceleration calculations.
		Vec3r mPrevLocalLinearVel;

		/// Stored copy of the previous step's velocity data; used for 
		/// acceleration calculations.
		Vec3r mPrevLocalAngularVel;

	private:
	};
}

#endif

