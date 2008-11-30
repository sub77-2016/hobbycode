/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
* Oleksandr Lozitskiy (http://ox.slug.louisville.edu/~o0lozi01/wiki)    *
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

// class headers
#include "VelocityMotor.h"

// project headers
#include "Simulator.h"

namespace opal
{
    VelocityMotor::VelocityMotor(Simulator * sim)
    {
        mSimulator = sim;
    }

    VelocityMotor::~VelocityMotor()
    {}

    void VelocityMotor::setVelocity(const Vec3r & velocity)
    {
        mData.velocity = velocity;
    }

    const Vec3r & VelocityMotor::getVelocity() const
    {
        return mData.velocity;
    }

    void VelocityMotor::init(const VelocityMotorData & data)
    {
        Motor::init();
        mData = data;
        if (mData.solid)
        {
            mData.solid->setLinearDamping(0);
        }
    }

    void VelocityMotor::setEnabled(bool e)
    {
        mData.enabled = e;
    }

    bool VelocityMotor::isEnabled() const
    {
        return mData.enabled;
    }

    MotorType VelocityMotor::getType() const
    {
        return VELOCITY_MOTOR;
    }

    void VelocityMotor::setName(const std::string& name)
    {
        mData.name = name;
    }

    const std::string & VelocityMotor::getName() const
    {
        return mData.name;
    }

    bool VelocityMotor::internal_dependsOnSolid(Solid * s) const
    {
        if (mData.solid == s)
            return true;
        else
            return false;
    }

    void VelocityMotor::letGravityAffectSolid(bool affect)
    {
        mData.letGravityAffectSolid = affect;
    }

    bool VelocityMotor::doesGravityAffectSolid() const
    {
        return mData.letGravityAffectSolid;
    }

    void VelocityMotor::internal_update()
    {
        // check if we have a solid
        if (mData.solid == NULL || isEnabled() == false) return ;

        Vec3r targetVelocity = mData.velocity;
        Solid * solid = mData.solid;

        Vec3r currentAchievedVelocity = solid->getGlobalLinearVel();

        if (doesGravityAffectSolid())
        {
            Vec3r gravity = mSimulator->getGravity();
            if (gravity.length() > 0)
            {
                Vec3r gravity_velocity = project(gravity, currentAchievedVelocity);
                currentAchievedVelocity -= gravity_velocity;
            }
        }

        Vec3r deltaVelocity = targetVelocity - currentAchievedVelocity;

        Vec3r forceVector = deltaVelocity / mSimulator->getStepSize() * solid->getMass();
        if (!doesGravityAffectSolid())
            forceVector -= mSimulator->getGravity() * solid->getMass();

        if (forceVector.length() > getMaximumForce())
        {
            forceVector.normalize();
            forceVector *= getMaximumForce();
        }

        Force controllingForce;
        controllingForce.duration = 0;
        controllingForce.singleStep = true;
        controllingForce.type = GLOBAL_FORCE;
        controllingForce.vec = forceVector;

        solid->addForce(controllingForce);
    }

    void VelocityMotor::setMaximumForce(real maxForce)
    {
        mData.maxForce = maxForce;
    }

    real VelocityMotor::getMaximumForce() const
    {
        return mData.maxForce;
    }
}
