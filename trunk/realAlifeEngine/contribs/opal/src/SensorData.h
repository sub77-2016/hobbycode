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

#ifndef OPAL_SENSOR_DATA_H
#define OPAL_SENSOR_DATA_H

// project headers
#include "Defines.h"
#include "Matrix44r.h"

namespace opal
{
	/// The types of Sensors currently available.
	enum SensorType
	{
		ACCELERATION_SENSOR,
		INCLINE_SENSOR,
		RAYCAST_SENSOR,
		VOLUME_SENSOR
	};

	/// A data structure describing a Sensor.
	class SensorData
	{
	public:
		SensorData()
		{
			// The initial type doesn't matter since the abstract base
			// class will never be instantiated.
			mType = ACCELERATION_SENSOR;
			enabled = defaults::sensor::enabled;
			name = "";
			solid = NULL;
			internal_solidIndex = 0;
			solidBlueprintRefName = "";
			// "transform" is initialized in its own constructor.
		}

		virtual ~SensorData()
		{
		}

		/// Returns the Sensor's type.
		SensorType getType()const
		{
			return mType;
		}

		/// True if the Sensor is enabled.
		bool enabled;

		/// An identifier for the Sensor.
		std::string name;

		/// Pointer to the Solid to which this Sensor is attached.  This
		/// will be NULL if the Sensor is not attached to a Solid (i.e.
		/// it is just positioned somewhere within the environment).
		Solid* solid;

		/// Internal data used for Blueprint instantiation.
		int internal_solidIndex;

		/// String used for for intra-Blueprint references.
		std::string solidBlueprintRefName;

		/// If the Sensor is attached to a Solid, this transform is the global
		/// offset from that Solid's transform.  Otherwise, it is just the
		/// Sensor's global transform.
		Matrix44r transform;

	protected:
		/// The Sensor type.
		SensorType mType;

	private:
	};
}

#endif
