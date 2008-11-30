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

#ifndef OPAL_INCLINE_SENSOR_H
#define OPAL_INCLINE_SENSOR_H

#include "Defines.h"
#include "Sensor.h"
#include "InclineSensorData.h"

namespace opal
{
	class Simulator;

	/// A Sensor that measures a Solid's "tilt."  This is similar to a real 
	/// inclinometer that monitors how an object is oriented with respect 
	/// to gravity.  This Sensor takes a rotation axis (defined relative to 
	/// the attached Solid's transform) and returns an angle describing how 
	/// far the Solid has rotated around that axis.  The rotation of the 
	/// Solid is defined as zero degrees when the Sensor is initialized or 
	/// when the rotation axis is redefined.  This Sensor does 
	/// nothing if it is not attached to a Solid (i.e. its returned values 
	/// are always zero).
	class InclineSensor : public Sensor
	{
	public:
		InclineSensor();

		virtual ~InclineSensor();

		/// Initializes the Sensor with the given data structure.  This will 
		/// define the rotation angle as zero degrees when called.  The Solid 
		/// pointer should be valid because this Sensor only works when 
		/// attached to something.  This does nothing if the Sensor's Solid 
		/// pointer is NULL.
		virtual void OPAL_CALL init(const InclineSensorData& data);

		/// Returns all data describing the Sensor.
		virtual const InclineSensorData& OPAL_CALL getData()const;

		/// Returns the angle of rotation about the local rotation axis 
		/// relative to the initial angle of rotation.
		virtual real OPAL_CALL getAngle();

		virtual void OPAL_CALL setEnabled(bool e);

		virtual bool OPAL_CALL isEnabled()const;

		/// Sets the local rotation axis around which the angle of rotation 
		/// will be measured.  This will redefine the rotation angle as zero 
		/// degrees when called.  This does nothing if the Sensor's Solid 
		/// pointer is NULL.
		virtual void OPAL_CALL setAxis(const Vec3r& axis);

		/// Returns the local rotation axis.
		virtual const Vec3r& OPAL_CALL getAxis();

		virtual void OPAL_CALL setTransform(const Matrix44r& t);

		virtual const Matrix44r& OPAL_CALL getTransform()const;

		virtual SensorType OPAL_CALL getType()const;

		virtual void OPAL_CALL setName(const std::string& name);

		virtual const std::string& OPAL_CALL getName()const;

		virtual void internal_update();

		virtual bool internal_dependsOnSolid(Solid* s);

	protected:
		/// A helper function that sets up all internal vectors used when 
		/// calculating the angle of rotation.
		void setupInternalVectors();

		/// Stores data describing the Sensor.
		InclineSensorData mData;

		/// A vector used to measure the angle of rotation.
		Vec3r mLocalReferenceVec;

		/// A vector used to measure the angle of rotation.
		Vec3r mInitGlobalReferenceVec;

	private:
	};
}

#endif

