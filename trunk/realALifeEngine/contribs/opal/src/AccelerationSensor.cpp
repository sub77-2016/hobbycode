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

#include "AccelerationSensor.h"
#include "Simulator.h"

namespace opal
{
	AccelerationSensor::AccelerationSensor(Simulator* s)
	{
		// "mData" is initialized in its own constructor.
		mSim = s;

		// "mCurrentGlobalLinearVel" is initialized in its own constructor.
		// "mCurrentGlobalAngularVel" is initialized in its own constructor.
		// "mCurrentLocalLinearVel" is initialized in its own constructor.
		// "mCurrentLocalAngularVel" is initialized in its own constructor.

		// "mPrevGlobalLinearVel" is initialized in its own constructor.
		// "mPrevGlobalAngularVel" is initialized in its own constructor.
		// "mPrevLocalLinearVel" is initialized in its own constructor.
		// "mPrevLocalAngularVel" is initialized in its own constructor.
	}

	AccelerationSensor::~AccelerationSensor()
	{
	}

	void AccelerationSensor::init(const AccelerationSensorData& data)
	{
		Sensor::init();
		mData = data;
	}

	const AccelerationSensorData& AccelerationSensor::getData()const
	{
		return mData;
	}

	Vec3r AccelerationSensor::getGlobalLinearAccel()const
	{
		if (!mData.enabled || !mData.solid)
		{
			return Vec3r();
		}

		return (mCurrentGlobalLinearVel - mPrevGlobalLinearVel) / 
			mSim->getStepSize();
	}

	Vec3r AccelerationSensor::getGlobalAngularAccel()const
	{
		if (!mData.enabled || !mData.solid)
		{
			return Vec3r();
		}

		return (mCurrentGlobalAngularVel - mPrevGlobalAngularVel) / 
			mSim->getStepSize();
	}

	Vec3r AccelerationSensor::getLocalLinearAccel()const
	{
		if (!mData.enabled || !mData.solid)
		{
			return Vec3r();
		}

		return (mCurrentLocalLinearVel - mPrevLocalLinearVel) / 
			mSim->getStepSize();
	}

	Vec3r AccelerationSensor::getLocalAngularAccel()const
	{
		if (!mData.enabled || !mData.solid)
		{
			return Vec3r();
		}

		return (mCurrentLocalAngularVel - mPrevLocalAngularVel) / 
			mSim->getStepSize();
	}

	void AccelerationSensor::setEnabled(bool e)
	{
		//if (!mInitCalled)
		//{
		//	return;
		//}

		mData.enabled = e;
	}

	bool AccelerationSensor::isEnabled()const
	{
		return mData.enabled;
	}

	SensorType AccelerationSensor::getType()const
	{
		return mData.getType();
	}

	void AccelerationSensor::setTransform(const Matrix44r& t)
	{
		mData.transform = t;
	}

	const Matrix44r& AccelerationSensor::getTransform()const
	{
		return mData.transform;
	}

	void AccelerationSensor::setName(const std::string& name)
	{
		mData.name = name;
	}

	const std::string& AccelerationSensor::getName()const
	{
		return mData.name;
	}

	void AccelerationSensor::internal_update()
	{
		if (mData.enabled && mData.solid)
		{
			mPrevGlobalLinearVel = mCurrentGlobalLinearVel;
			mPrevGlobalAngularVel = mCurrentGlobalAngularVel;
			mPrevLocalLinearVel = mCurrentLocalLinearVel;
			mPrevLocalAngularVel = mCurrentLocalAngularVel;

			mCurrentGlobalLinearVel = 
				mData.solid->getGlobalLinearVel();
			mCurrentGlobalAngularVel = 
				mData.solid->getGlobalAngularVel();
			mCurrentLocalLinearVel = 
				mData.solid->getLocalLinearVel();
			mCurrentLocalAngularVel = 
				mData.solid->getLocalAngularVel();
		}
	}

	bool AccelerationSensor::internal_dependsOnSolid(Solid* s)
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
