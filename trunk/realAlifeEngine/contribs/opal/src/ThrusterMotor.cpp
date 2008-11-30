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

#include "ThrusterMotor.h"
#include "Solid.h"
#include "Joint.h"

namespace opal
{
	ThrusterMotor::ThrusterMotor()
	: Motor()
	{
		// "mData" will be initialized in its own constructor.
	}

	ThrusterMotor::~ThrusterMotor()
	{
	}

	//void ThrusterMotor::init(Solid* solid)
	//{
	//	Motor::init();
	//	mSolid = solid;
	//}

	void ThrusterMotor::init(const ThrusterMotorData& data)
	{
		Motor::init();
		mData = data;
		mData.force.singleStep = true;
	}

	const ThrusterMotorData& ThrusterMotor::getData()const
	{
		return mData;
	}

	MotorType ThrusterMotor::getType()const
	{
		return mData.getType();
	}

	void ThrusterMotor::setName(const std::string& name)
	{
		mData.name = name;
	}

	const std::string& ThrusterMotor::getName()const
	{
		return mData.name;
	}

	bool ThrusterMotor::isEnabled()const
	{
		return mData.enabled;
	}

	void ThrusterMotor::setEnabled(bool e)
	{
		//if (!mInitCalled)
		//{
		//	return;
		//}

		mData.enabled = e;
	}

	void ThrusterMotor::internal_update()
	{
		if (mData.enabled && mData.solid)
		{
			mData.solid->addForce(mData.force);
		}
	}

	void ThrusterMotor::setForce(const Force& f)
	{
		mData.force = f;
		mData.force.singleStep = true;
	}

	const Force& ThrusterMotor::getForce()const
	{
		return mData.force;
	}

	bool ThrusterMotor::internal_dependsOnSolid(Solid* s)
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
