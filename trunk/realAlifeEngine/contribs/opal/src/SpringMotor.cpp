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

#include "SpringMotor.h"
#include "Solid.h"

namespace opal
{
	SpringMotor::SpringMotor()
	: Motor()
	{
		// "mData" will be initialized in its own constructor.
	}

	SpringMotor::~SpringMotor()
	{
	}

	void SpringMotor::init(const SpringMotorData& data)
	{
		Motor::init();
		mData = data;
	}

	const SpringMotorData& SpringMotor::getData()const
	{
		return mData;
	}

	MotorType SpringMotor::getType()const
	{
		return mData.getType();
	}

	void SpringMotor::setName(const std::string& name)
	{
		mData.name = name;
	}

	const std::string& SpringMotor::getName()const
	{
		return mData.name;
	}

	bool SpringMotor::isEnabled()const
	{
		return mData.enabled;
	}

	void SpringMotor::setEnabled(bool e)
	{
		//if (!mInitCalled)
		//{
		//	return;
		//}

		mData.enabled = e;
	}

	void SpringMotor::setSolid(Solid* s)
	{
		mData.solid = s;
	}

	Solid* SpringMotor::getSolid()const
	{
		return mData.solid;
	}

	void SpringMotor::internal_update()
	{
		// Note: this only applies to global position and orientation.

		// TODO: This should be fixed.  The intent here is to use a PD 
		// controller (see http://en.wikipedia.org/wiki/PID_controller, ignore 
		// the "I" term).  Both linear and angular modes should use a "P" 
		// term based on the immediate error and a "D" term based on the 
		// derivative of the error over time.  The problem here is that we 
		// are looking at the derivative of the position/angle (i.e. linear 
		// and angular velocity) instead of the error derivative, so we 
		// should change this to be a true PD controller by replacing the 
		// velocity terms with error derivative terms.  (The results should be 
		// similar when the position/angle is moving towards its desired state, 
		// but not when it is moving away.)

		if (mData.enabled && mData.solid)
		{
			if (LINEAR_MODE == mData.mode || 
				LINEAR_AND_ANGULAR_MODE == mData.mode)
			{
				Vec3r error = mData.desiredPos - getGlobalAttachPoint();
				Vec3r errorTerm = mData.linearKs * error;
				Vec3r velTerm = mData.linearKd * mData.solid->
					getGlobalLinearVelAtLocalPos(mData.attachOffset);

				Force f;
				f.singleStep = true;
				f.type = GLOBAL_FORCE_AT_LOCAL_POS;
				f.pos = mData.attachOffset;
				f.vec = errorTerm - velTerm;

				// Scale the force vector by the Solid's mass.
				f.vec *= mData.solid->getMass();
				mData.solid->addForce(f);
			}

			if (ANGULAR_MODE == mData.mode || 
				LINEAR_AND_ANGULAR_MODE == mData.mode)
			{
				// Find cross products of actual and desired forward, up, 
				// and right vectors; these represent the orientation error.
				Matrix44r transform = mData.solid->getTransform();
				Vec3r actualForward = transform.getForward();
				Vec3r actualUp = transform.getUp();
				Vec3r actualRight = transform.getRight();
				
				if (0 != actualForward.lengthSquared())
				{
					actualForward.normalize();
				}
				if (0 != actualUp.lengthSquared())
				{
					actualUp.normalize();
				}
				if (0 != actualRight.lengthSquared())
				{
					actualRight.normalize();
				}

				Vec3r forwardError = cross(mData.desiredForward, 
					actualForward);
				Vec3r upError = cross(mData.desiredUp, actualUp);
				Vec3r rightError = cross(mData.desiredRight, actualRight);

				if (0 != forwardError.lengthSquared())
				{
					forwardError.normalize();
				}
				if (0 != upError.lengthSquared())
				{
					upError.normalize();
				}
				if (0 != rightError.lengthSquared())
				{
					rightError.normalize();
				}

				// Scale error vectors by the magnitude of the angles.
				real fangle = angleBetweenPreNorm(mData.desiredForward, 
					actualForward);
				real uangle = angleBetweenPreNorm(mData.desiredUp, 
					actualUp);
				real rangle = angleBetweenPreNorm(mData.desiredRight, 
					actualRight);

				forwardError *= -fangle;
				upError *= -uangle;
				rightError *= -rangle;

				// Average the vectors into one.
				Vec3r errorAxis = (forwardError + upError + rightError) * 
					globals::OPAL_ONE_THIRD;

				Vec3r errorTerm = mData.angularKs * errorAxis;
				Vec3r velTerm = mData.angularKd * mData.solid->
					getGlobalAngularVel();

				// Use the error vector to calculate torque.
				Force f;
				f.singleStep = true;
				f.type = GLOBAL_TORQUE;
				f.vec = errorTerm - velTerm;

				// Scale the torque vector by the Solid's intertia tensor.
				f.vec = mData.solid->getInertiaTensor() * f.vec;
				mData.solid->addForce(f);
			}
		}
	}

	void SpringMotor::setLocalAttachPoint(const Point3r& p)
	{
		mData.attachOffset = p;
	}

	const Point3r& SpringMotor::getLocalAttachPoint()const
	{
		return mData.attachOffset;
	}

	void SpringMotor::setGlobalAttachPoint(const Point3r& p)
	{
		if (!mData.solid)
		{
			OPAL_LOGGER("warning") << 
				"opal::SpringMotor::setGlobalAttachPoint: Solid pointer is \
invalid.  Ignoring request." << std::endl;
			return;
		}

		// Convert the global point to a local point offset from the Solid's 
		// transform.
		Matrix44r inv = mData.solid->getTransform();
		inv.invert();
		mData.attachOffset = inv * p;
	}

	Point3r SpringMotor::getGlobalAttachPoint()const
	{
		if (!mData.solid)
		{
			OPAL_LOGGER("warning") << 
				"opal::SpringMotor::getGlobalAttachPoint: Solid pointer is \
invalid.  Returning (0,0,0)." << std::endl;
			return Point3r();
		}

		// The global position is a combination of the Solid's global 
		// transform and the spring's local offset from the Solid's 
		// transform.
		Point3r localPos(mData.attachOffset[0], mData.attachOffset[1], 
			mData.attachOffset[2]);
		Point3r globalPos = mData.solid->getTransform() * localPos;
		
		return globalPos;
	}

	void SpringMotor::setDesiredTransform(const Matrix44r& transform)
	{
		mData.desiredPos = transform.getPosition();

		mData.desiredForward = transform.getForward();
		if (0 != mData.desiredForward.lengthSquared())
		{
			mData.desiredForward.normalize();
		}

		mData.desiredUp = transform.getUp();
		if (0 != mData.desiredUp.lengthSquared())
		{
			mData.desiredUp.normalize();
		}

		mData.desiredRight = transform.getRight();
		if (0 != mData.desiredRight.lengthSquared())
		{
			mData.desiredRight.normalize();
		}
	}

	void SpringMotor::setDesiredPosition(const Point3r& pos)
	{
		mData.desiredPos = pos;
	}

	void SpringMotor::setDesiredOrientation(const Vec3r& forward, 
		const Vec3r& up, const Vec3r& right)
	{
		mData.desiredForward = forward;
		if (0 != mData.desiredForward.lengthSquared())
		{
			mData.desiredForward.normalize();
		}

		mData.desiredUp = up;
		if (0 != mData.desiredUp.lengthSquared())
		{
			mData.desiredUp.normalize();
		}

		mData.desiredRight = right;
		if (0 != mData.desiredRight.lengthSquared())
		{
			mData.desiredRight.normalize();
		}
	}

	void SpringMotor::setLinearKd(real kd)
	{
		mData.linearKd = kd;
	}

	void SpringMotor::setLinearKs(real ks)
	{
		mData.linearKs = ks;
	}

	real SpringMotor::getLinearKd()const
	{
		return mData.linearKd;
	}

	real SpringMotor::getLinearKs()const
	{
		return mData.linearKs;
	}

	void SpringMotor::setAngularKd(real kd)
	{
		mData.angularKd = kd;
	}

	void SpringMotor::setAngularKs(real ks)
	{
		mData.angularKs = ks;
	}

	real SpringMotor::getAngularKd()const
	{
		return mData.angularKd;
	}

	real SpringMotor::getAngularKs()const
	{
		return mData.angularKs;
	}

	bool SpringMotor::internal_dependsOnSolid(Solid* s)
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
