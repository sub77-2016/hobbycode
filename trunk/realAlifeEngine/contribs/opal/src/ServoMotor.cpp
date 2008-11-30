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

#include "ServoMotor.h"

namespace opal
{
    ServoMotor::ServoMotor()
            : Motor()
    {
        // "mData" will be initialized in its own constructor.
    }

    ServoMotor::~ServoMotor()
    {
        // We must set the Joint's desired vel and max force to 0 since
        // this Motor won't apply to it anymore.  This is only necessary
        // if this Motor is enabled.
        if (mData.enabled && mData.joint)
        {
            mData.joint->internal_setDesiredVel(mData.jointAxisNum, 0);
            mData.joint->internal_setMaxTorque(mData.jointAxisNum, 0);
        }
    }

    void ServoMotor::init(const ServoMotorData& data)
    {
        if (mInitCalled)
        {
            // If the Servo is already in operation, we first need to
            // set the old Joint's desired vel and max force to 0.  The
            // following function call will automatically handle this
            // when set to false.
            setEnabled(false);
        }

        assert(data.joint);
        assert(data.jointAxisNum >= 0 &&
                data.jointAxisNum < data.joint->getNumAxes());
        Motor::init();
        assert(data.joint->isRotational(data.jointAxisNum));
        mData = data;
        setEnabled(mData.enabled);
    }

    const ServoMotorData& ServoMotor::getData() const
    {
        return mData;
    }

    MotorType ServoMotor::getType() const
    {
        return mData.getType();
    }

    void ServoMotor::setName(const std::string& name)
    {
        mData.name = name;
    }

    const std::string& ServoMotor::getName() const
    {
        return mData.name;
    }

    bool ServoMotor::isEnabled() const
    {
        return mData.enabled;
    }

    void ServoMotor::setEnabled(bool e)
    {
        //if (!mInitCalled)
        //{
        //	return;
        //}

        mData.enabled = e;

        if (mData.joint)
        {
            if (e)
            {
                mData.joint->internal_setDesiredVel(mData.jointAxisNum,
                                                     mData.desiredVel);
                mData.joint->internal_setMaxTorque(mData.jointAxisNum,
                                                    mData.maxTorque);
            }
            else
            {
                mData.joint->internal_setDesiredVel(mData.jointAxisNum, 0);
                mData.joint->internal_setMaxTorque(mData.jointAxisNum, 0);
            }
        }
    }

    void ServoMotor::internal_update()
    {
        if (mData.enabled && mData.joint)
        {
            // Make sure both Solids are awake at this point.
            mData.joint->wakeSolids();

            if (DESIRED_ANGLE_MODE == mData.mode)
            {
                // No longer support linear degrees of freedom.
                //if (true == mData.joint->isRotational(mData.jointAxisNum))
                //{
                real velocity = mData.desiredAngle -
                                mData.joint->getAngle(mData.jointAxisNum);
                if (velocity > 180.0)
                    velocity = -360 + velocity;
                if (velocity < -180.0)
                    velocity = 360 + velocity;
                mData.joint->internal_setDesiredVel(mData.jointAxisNum,
                                                     mData.restoreSpeed * velocity);
                //}
                //else
                //{
                //	// This axis must be a linear degree of freedom.
                //	real velocity = mData.desiredPos -
                //		mData.joint->getState(mData.jointAxisNum);
                //	mData.joint->internal_setDesiredVel(mData.jointAxisNum,
                //		mData.restoreSpeed * velocity);
                //}
            }
            else
            {
                // Nothing to do for desired velocity mode; the Joint's
                // desired velocity should already handle this.
            }
        }
    }

    void ServoMotor::setDesiredAngle(real a)
    {
		// this clamping is needed since there are sometimes float point errors
        real low = mData.joint->getLowLimit(mData.jointAxisNum);
        if (a < low)
        {
            a = low;
        }

        real high = mData.joint->getHighLimit(mData.jointAxisNum);
        if (a > high)
        {
            a = high;
        }

        mData.desiredAngle = a;
    }

    void ServoMotor::setDesiredAngleNorm(real a)
    {
        if (a < 0)
            a = 0;
        if (a > 1)
            a = 1;

        real lowLimit = mData.joint->getLowLimit(mData.jointAxisNum);
        real highLimit = mData.joint->getHighLimit(mData.jointAxisNum);

        // map the pos value onto the joint limits
        mData.desiredAngle = a * (highLimit - lowLimit) + lowLimit;

        // Keep desired angle slightly away from the limit to avoid jitter.
        // @todo: fix this; this should just keep the thing away from the
        // limit when it's close, not all the time.
        mData.desiredAngle *= (real) 0.99;
    }

    real ServoMotor::getDesiredAngle() const
    {
        return mData.desiredAngle;
    }

    void ServoMotor::setDesiredVel(real vel)
    {
        mData.desiredVel = vel;
        mData.joint->internal_setDesiredVel(mData.jointAxisNum, vel);
    }

    real ServoMotor::getDesiredVel() const
    {
        return mData.desiredVel;
    }

    void ServoMotor::setMaxTorque(real max)
    {
        mData.maxTorque = max;
        mData.joint->internal_setMaxTorque(mData.jointAxisNum, max);
    }

    real ServoMotor::getMaxTorque() const
    {
        return mData.maxTorque;
    }

    void ServoMotor::setRestoreSpeed(real speed)
    {
        mData.restoreSpeed = speed;
    }

    real ServoMotor::getRestoreSpeed() const
    {
        return mData.restoreSpeed;
    }

    bool ServoMotor::internal_dependsOnJoint(Joint* j)
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
