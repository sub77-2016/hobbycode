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

#include "InclineSensor.h"
#include "Simulator.h"

namespace opal
{
	InclineSensor::InclineSensor()
	{
		// "mData" is initialized in its own constructor.
		// "mLocalReferenceVec" is initialized in its own constructor.
		// "mInitGlobalReferenceVec" is initialized in its own constructor.
	}

	InclineSensor::~InclineSensor()
	{
	}

	void InclineSensor::init(const InclineSensorData& data)
	{
		Sensor::init();
		mData = data;

		// Define the current setup as zero degrees.
		setupInternalVectors();
	}

	const InclineSensorData& InclineSensor::getData()const
	{
		return mData;
	}

	real InclineSensor::getAngle()
	{
		if (!mData.enabled || !mData.solid)
		{
			return 0;
		}

		// By this time setupInternalVectors should have been called, so 
		// the mLocalReferenceVec and mInitGlobalReferenceVec vectors are 
		// valid.

		// Let's call the plane orthogonal to the rotation axis the 
		// "plane of rotation."  The local reference vector is currently on 
		// the plane of rotation.  We need to get the initial global 
		// reference vector projected onto the plane of rotation.

		Vec3r currentGlobalReferenceVec = mData.solid->getTransform() * 
			mLocalReferenceVec;

		if (areCollinear(mInitGlobalReferenceVec, currentGlobalReferenceVec))
		{
			// Return zero degrees if the initial global reference vector 
			// is collinear with the current global reference vector.
			return 0;
		}

		Vec3r tempVec = cross(mData.axis, mInitGlobalReferenceVec);
		Vec3r u = cross(mData.axis, tempVec);

		// Now 'u' should be on the plane of rotation.  We just need to 
		// project the initial global reference vector onto it.

		Vec3r projInitGlobalReferenceVec = project(u, 
			mInitGlobalReferenceVec);

		// Now calculate the angle between the projected global reference 
		// vector and the current global reference vector.
		real angle = angleBetween(projInitGlobalReferenceVec, 
			currentGlobalReferenceVec);

		return angle;
	}

	void InclineSensor::setEnabled(bool e)
	{
		//if (!mInitCalled)
		//{
		//	return;
		//}

		mData.enabled = e;
	}

	bool InclineSensor::isEnabled()const
	{
		return mData.enabled;
	}

	void InclineSensor::setAxis(const Vec3r& axis)
	{
		if (!mData.solid)
		{
			return;
		}

		mData.axis = axis;

		// Redefine the current setup as zero degrees.
		setupInternalVectors();
	}

	const Vec3r& InclineSensor::getAxis()
	{
		return mData.axis;
	}

	SensorType InclineSensor::getType()const
	{
		return mData.getType();
	}

	void InclineSensor::setTransform(const Matrix44r& t)
	{
		mData.transform = t;
	}

	const Matrix44r& InclineSensor::getTransform()const
	{
		return mData.transform;
	}

	void InclineSensor::setName(const std::string& name)
	{
		mData.name = name;
	}

	const std::string& InclineSensor::getName()const
	{
		return mData.name;
	}

	void InclineSensor::internal_update()
	{
		if (mData.enabled && mData.solid)
		{
			// Do nothing.
		}
	}

	bool InclineSensor::internal_dependsOnSolid(Solid* s)
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

	void InclineSensor::setupInternalVectors()
	{
		if (!mData.solid)
		{
			return;
		}

		// We need to calculate a local reference vector that's orthogonal 
		// to the rotation axis.

		// This temporary vector can be anything as long as its not 
		// collinear with the rotation axis.
		Vec3r tempVec(1, 0, 0);

		if (areCollinear(mData.axis, tempVec))
		{
			// Pick a new tempVec.
			tempVec.set(0, 1, 0);
		}

		// Now we can get the orthogonal reference vector.
		mLocalReferenceVec = cross(mData.axis, tempVec);

		// Also store a copy of this reference vector in its initial global 
		// representation.
		mInitGlobalReferenceVec = mData.solid->getTransform() * 
			mLocalReferenceVec;
	}
}
