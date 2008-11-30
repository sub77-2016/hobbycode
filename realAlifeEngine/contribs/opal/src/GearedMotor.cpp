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

#include "GearedMotor.h"
#include "Joint.h"

namespace opal
{
	GearedMotor::GearedMotor()
	: Motor()
	{
		// "mData" will be initialized in its own constructor.
	}

	GearedMotor::~GearedMotor()
	{
	}

	//void GearedMotor::init(Joint* joint, int axisNum)
	//{
	//	Motor::init();
	//	assert(joint->isRotational(axisNum));
	//	mJoint = joint;
	//	mJointAxisNum = axisNum;
	//}

	void GearedMotor::init(const GearedMotorData& data)
	{
		assert(data.joint);
		Motor::init();
		assert(data.joint->isRotational(data.jointAxisNum));
		mData = data;
	}

	const GearedMotorData& GearedMotor::getData()const
	{
		return mData;
	}

	MotorType GearedMotor::getType()const
	{
		return mData.getType();
	}

	void GearedMotor::setName(const std::string& name)
	{
		mData.name = name;
	}

	const std::string& GearedMotor::getName()const
	{
		return mData.name;
	}

	bool GearedMotor::isEnabled()const
	{
		return mData.enabled;
	}

	void GearedMotor::setEnabled(bool e)
	{
		//if (!mInitCalled)
		//{
		//	return;
		//}

		mData.enabled = e;
	}

	void GearedMotor::internal_update()
	{
		if (mData.enabled && mData.joint)
		{
			// Global/local direction makes no difference here since 
			// we simply use the joint axis.

			// TODO: Fix the following case: if throttle is zero, there 
			// will be an opposing torque at any velocity.

			real normalizedVel = 
				mData.joint->getVelocity(mData.jointAxisNum) / 
				mData.maxVelocity;
			real magnitude = mData.maxTorque * (mData.throttle - 
				normalizedVel);
			mData.joint->addTorque(mData.jointAxisNum, magnitude, 0, true);
		}
	}

	void GearedMotor::setMaxTorque(real max)
	{
		mData.maxTorque = max;
	}

	real GearedMotor::getMaxTorque()const
	{
		return mData.maxTorque;
	}

	void GearedMotor::setMaxVelocity(real max)
	{
		assert(max != 0);
		mData.maxVelocity = max;
	}

	real GearedMotor::getMaxVelocity()const
	{
		return mData.maxVelocity;
	}

	void GearedMotor::setThrottle(real throttle)
	{
		assert(throttle >= -1.0 && throttle <= 1.0);
		mData.throttle = throttle;
	}

	real GearedMotor::getThrottle()const
	{
		return mData.throttle;
	}

	bool GearedMotor::internal_dependsOnJoint(Joint* j)
	{
		if (j == mData.joint)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
