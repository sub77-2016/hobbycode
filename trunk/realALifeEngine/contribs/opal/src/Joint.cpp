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

#include "Joint.h"
#include "Solid.h"

namespace opal
{
    Joint::Joint()
    {
        // "mData" is initialized in its own constructor.
        setJointBreakEventHandler(NULL);
        setUserData(NULL);
        mInitCalled = false;
        mNumAxes = 0;
        mAxisRotational[0] = false;
        mAxisRotational[1] = false;
        mAxisRotational[2] = false;
    }

    Joint::~Joint()
    {}

    void Joint::internal_destroy()
    {
        delete this;
    }

    void Joint::init(const JointData& data)
    {
        mData = data;
    }

    const JointData& Joint::getData()
    {
        // Update parameters that don't get updated automatically.
        for (int i = 0; i < mNumAxes; ++i)
        {
            mData.axis[i] = getAxis(i);
        }

        mData.anchor = getAnchor();

        return mData;
    }

    void Joint::setName(const std::string& name)
    {
        mData.name = name;
    }

    const std::string& Joint::getName() const
    {
        return mData.name;
    }

    void Joint::setContactsEnabled(bool e)
    {
        mData.contactsEnabled = e;
    }

    bool Joint::areContactsEnabled() const
    {
        return mData.contactsEnabled;
    }

    JointType Joint::getType() const
    {
        return mData.getType();
    }

    void Joint::setBreakParams(JointBreakMode mode, real breakThresh,
                                real accumThresh)
    {
        mData.breakMode = mode;
        mData.breakThresh = breakThresh;
        mData.accumThresh = accumThresh;
    }

    bool Joint::isBroken() const
    {
        return mData.isBroken;
    }

    JointBreakMode Joint::getBreakingMode() const
    {
        return mData.breakMode;
    }

    real Joint::getBreakThresh() const
    {
        return mData.breakThresh;
    }

    real Joint::getAccumulatedDamage() const
    {
        return mData.accumDamage;
    }

    real Joint::getAccumulatedThresh() const
    {
        return mData.accumThresh;
    }

    void Joint::repairAccumDamage()
    {
        mData.accumDamage = 0;
        //mIsBroken = false;
    }

    //bool Joint::internal_isBroken()
    //{
    //	return mIsBroken;
    //}

    void Joint::setJointBreakEventHandler(
        JointBreakEventHandler* eventHandler)
    {
        mJointBreakEventHandler = eventHandler;
    }

    JointBreakEventHandler* Joint::getJointBreakEventHandler() const
    {
        return mJointBreakEventHandler;
    }

    void Joint::setLimitsEnabled(int axisNum, bool e)
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);
        mData.axis[axisNum].limitsEnabled = e;
    }

    bool Joint::areLimitsEnabled(int axisNum)
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);
        return mData.axis[axisNum].limitsEnabled;
    }

    //void Joint::setLimits(int axisNum, JointLimits l)
    //{
    //	assert(axisNum >= 0 && axisNum < mNumAxes);
    //	assert(l.bounciness >= 0 && l.bounciness <= 1);
    //	assert(l.hardness >= 0 && l.hardness <= 1);
    //	mData.axis[axisNum].limits = l;
    //}

    //const Joint::JointLimits& getLimits(int axisNum)const
    //{
    //	assert(axisNum >= 0 && axisNum < mNumAxes);
    //	return mData.axis[axisNum].limits;
    //}

    void Joint::setLimitRange(int axisNum, real low, real high)
    {
        assert(high >= low);
        assert(axisNum >= 0 && axisNum < mNumAxes);
        mData.axis[axisNum].limits.low = low;
        mData.axis[axisNum].limits.high = high;
    }

    real Joint::getLowLimit(int axisNum) const
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);
        return mData.axis[axisNum].limits.low;
    }

    real Joint::getHighLimit(int axisNum) const
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);
        return mData.axis[axisNum].limits.high;
    }

    void Joint::setLimitHardness(int axisNum, real h)
    {
        assert(h >= 0 && h <= 1);
        assert(axisNum >= 0 && axisNum < mNumAxes);
        mData.axis[axisNum].limits.hardness = h;
    }

    real Joint::getLimitHardness(int axisNum) const
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);
        return mData.axis[axisNum].limits.hardness;
    }

    void Joint::setLimitBounciness(int axisNum, real b)
    {
        assert(b >= 0 && b <= 1);
        assert(axisNum >= 0 && axisNum < mNumAxes);
        mData.axis[axisNum].limits.bounciness = b;
    }

    real Joint::getLimitBounciness(int axisNum) const
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);
        return mData.axis[axisNum].limits.bounciness;
    }

    void Joint::addForce(int axisNum, real magnitude, real duration,
                          bool singleStep)
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);

        if (mData.enabled)
        {
            Force f;
            f.singleStep = singleStep;

            // We only care about the duration if this is not a single-step
            // force.
            if (!f.singleStep)
            {
                f.duration = duration;
            }

            f.type = LOCAL_FORCE;
            Vec3r direction = mData.axis[axisNum].direction;
            f.vec = magnitude * direction;

            if (mData.solid0)
            {
                mData.solid0->addForce(f);
            }

            f.vec *= (real) - 1.0;

            if (mData.solid1)
            {
                mData.solid1->addForce(f);
            }
        }
    }

    void Joint::addTorque(int axisNum, real magnitude, real duration,
                           bool singleStep)
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);

        if (mData.enabled)
        {
            Force f;
            f.singleStep = singleStep;

            // We only care about the duration if this is not a single-step
            // force.
            if (!f.singleStep)
            {
                f.duration = duration;
            }

            f.type = LOCAL_TORQUE;
            Vec3r axis = mData.axis[axisNum].direction;
            f.vec = magnitude * axis;

            if (mData.solid0)
            {
                mData.solid0->addForce(f);
            }

            f.vec *= (real) - 1.0;

            if (mData.solid1)
            {
                mData.solid1->addForce(f);
            }
        }
    }

    void Joint::wakeSolids()
    {
        if (mData.solid0)
            mData.solid0->setSleeping(false);
        if (mData.solid1)
            mData.solid1->setSleeping(false);
    }

    void Joint::setSolids(Solid* s0, Solid* s1)
    {
        mData.solid0 = s0;
        mData.solid1 = s1;
    }

    Solid* Joint::getSolid0() const
    {
        return mData.solid0;
    }

    Solid* Joint::getSolid1() const
    {
        return mData.solid1;
    }

    void Joint::setAxis(int axisNum, const JointAxis& axis)
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);
        mData.axis[axisNum] = axis;
    }

    int Joint::getNumAxes() const
    {
        return mNumAxes;
    }

    void Joint::setAnchor(const Point3r& anchor)
    {
        mData.anchor = anchor;
    }

    bool Joint::isEnabled() const
    {
        return mData.enabled;
    }

    void Joint::setEnabled(bool e)
    {
        if (!mInitCalled)
        {
            return ;
        }

        mData.enabled = e;
    }

    bool Joint::isRotational(int axisNum) const
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);
        return mAxisRotational[axisNum];
    }

    void Joint::setUserData(void* data)
    {
        mUserData = data;
    }

    void* Joint::getUserData()
    {
        return mUserData;
    }

    bool Joint::internal_dependsOnSolid(Solid* s)
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

    void Joint::updateDamage(real currentStress)
    {
        switch (mData.breakMode)
        {
            case UNBREAKABLE_MODE:
            //nothing to do
            break;
            case THRESHOLD_MODE:
            {
                if (currentStress >= mData.breakThresh)
                {
                    mData.isBroken = true;
                }
                break;
            }
            case ACCUMULATED_MODE:
            {
                if (currentStress >= mData.accumThresh)
                {
                    mData.accumDamage += currentStress;
                }

                if (mData.accumDamage >= mData.breakThresh)
                {
                    mData.isBroken = true;
                }
                break;
            }
            default:
            assert(false);
        }

        if (mData.isBroken)
        {
            setEnabled(false);
            if (mJointBreakEventHandler)
            {
                mJointBreakEventHandler->handleJointBreakEvent(this);
            }
        }
    }
}
