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

#include "AttractorMotor.h"
#include "Solid.h"

namespace opal
{
	AttractorMotor::AttractorMotor()
	: Motor()
	{
		// "mData" will be initialized in its own constructor.
		mSolid0Mass = 0;
		mSolid1Mass = 0;
		mMassConstant = 0;
	}

	AttractorMotor::~AttractorMotor()
	{
	}

	//void AttractorMotor::init(Solid* solid0, Solid* solid1)
	//{
	//	Motor::init();
	//	mSolid0 = solid0;
	//	mSolid1 = solid1;
	//	mSolid0Mass = solid0->getMass();
	//	mSolid1Mass = solid1->getMass();

	//	// Update this constant since the masses changed.
	//	mMassConstant = mStrength * mSolid0Mass * mSolid1Mass;
	//}

	void AttractorMotor::init(const AttractorMotorData& data)
	{
		Motor::init();
		mData = data;

		if (data.solid0 && data.solid1)
		{
			mSolid0Mass = data.solid0->getMass();
			mSolid1Mass = data.solid1->getMass();

			// Update this constant since the masses changed.
			mMassConstant = data.strength * mSolid0Mass * mSolid1Mass;
		}
	}

	const AttractorMotorData& AttractorMotor::getData()const
	{
		return mData;
	}

	MotorType AttractorMotor::getType()const
	{
		return mData.getType();
	}

	void AttractorMotor::setName(const std::string& name)
	{
		mData.name = name;
	}

	const std::string& AttractorMotor::getName()const
	{
		return mData.name;
	}

	bool AttractorMotor::isEnabled()const
	{
		return mData.enabled;
	}

	void AttractorMotor::setEnabled(bool e)
	{
		//if (!mInitCalled)
		//{
		//	return;
		//}

		mData.enabled = e;
	}

	void AttractorMotor::internal_update()
	{
		if (mData.enabled && mData.solid0 && mData.solid1)
		{
			Point3r pos1 = mData.solid0->getPosition();
			Point3r pos2 = mData.solid1->getPosition();

			// Create a unit vector pointing from mSolid1 to mSolid0.
			Vec3r direction = pos1 - pos2;
			real distanceSquared = direction.lengthSquared();
			if (0 != distanceSquared)
			{
				direction.normalize();
			}

			Force f;
			f.singleStep = true;
			f.type = GLOBAL_FORCE;
			f.vec = direction;

			// Use force magnitude = (strength * m0 * m1) / distance^exponent.
			if ((real)2.0 == mData.exponent)
			{
				// If we know the exponent is 2, this can speed things up.
				f.vec *= (mMassConstant / distanceSquared);
			}
			else
			{
				f.vec *= (mMassConstant / (pow(direction.length(), 
					mData.exponent)));
			}

			mData.solid1->addForce(f);
			f.vec *= (real)-1.0;
			mData.solid0->addForce(f);
		}
	}

	void AttractorMotor::setStrength(real s)
	{
		mData.strength = s;

		// Update this constant since the strength changed.
		mMassConstant = mData.strength * mSolid0Mass * mSolid1Mass;
	}

	real AttractorMotor::getStrength()const
	{
		return mData.strength;
	}

	void AttractorMotor::setExponent(real e)
	{
		mData.exponent = e;
	}

	real AttractorMotor::getExponent()const
	{
		return mData.exponent;
	}

	bool AttractorMotor::internal_dependsOnSolid(Solid* s)
	{
		if (s == mData.solid0 || s == mData.solid1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
