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

#include "VolumeSensor.h"
#include "Simulator.h"

namespace opal
{
	VolumeSensor::VolumeSensor(Simulator* s)
	{
		// "mData" is initialized in its own constructor.
		mSim = s;
	}

	VolumeSensor::~VolumeSensor()
	{
	}

	void VolumeSensor::init(const VolumeSensorData& data)
	{
		Sensor::init();
		mData = data;
	}

	const VolumeSensorData& VolumeSensor::getData()const
	{
		return mData;
	}

	const VolumeQueryResult& VolumeSensor::queryVolume(Solid* volume)
	{
		if (mData.enabled)
		{
			// The volume Solid's transform will be totally ignored.

			// Store the volume Solid's original enabled state and transform.  
			bool originalEnabledState = volume->isEnabled();
			Matrix44r originalVolumeTransform = volume->getTransform();

			// We need to make sure the volume Solid is enabled here 
			// temporarily.
			volume->setEnabled(true);

			// If the Sensor is attached to a Solid, we need to transform 
			// the volume relative to the Solid's transform and the 
			// Sensor's transform.
			if (mData.solid)
			{
				volume->setTransform(mData.solid->getTransform() * 
					mData.transform);
			}
			else
			{
				// If the Sensor is not attached to a Solid, just use the 
				// Sensor's transform as a global transform on the volume.
				volume->setTransform(mData.transform);
			}

			// If this is attached to a Solid, the Simulator volume query 
			// function will automatically ignore intersections between the 
			// volume and that Solid.

			// Query the volume for colliding Solids.
			const VolumeQueryResult& result = 
				mSim->internal_queryVolume(volume, mData.solid);

			// Restore the volume Solid's original transform and enabled 
			// state.
			volume->setTransform(originalVolumeTransform);
			volume->setEnabled(originalEnabledState);

			return result;
		}
		else
		{
			static VolumeQueryResult junkResult;
			return junkResult;
		}
	}

	void VolumeSensor::setEnabled(bool e)
	{
		//if (!mInitCalled)
		//{
		//	return;
		//}

		mData.enabled = e;
	}

	bool VolumeSensor::isEnabled()const
	{
		return mData.enabled;
	}

	SensorType VolumeSensor::getType()const
	{
		return mData.getType();
	}

	void VolumeSensor::setTransform(const Matrix44r& t)
	{
		mData.transform = t;
	}

	const Matrix44r& VolumeSensor::getTransform()const
	{
		return mData.transform;
	}

	void VolumeSensor::setName(const std::string& name)
	{
		mData.name = name;
	}

	const std::string& VolumeSensor::getName()const
	{
		return mData.name;
	}

	void VolumeSensor::internal_update()
	{
		if (mData.enabled && mData.solid)
		{
			// Do nothing.
		}
	}

	bool VolumeSensor::internal_dependsOnSolid(Solid* s)
	{
		if (s == mData.solid)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
