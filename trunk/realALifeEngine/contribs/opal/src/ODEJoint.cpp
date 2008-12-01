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

#include "ODEJoint.h" 

namespace opal
{
    ODEJoint::ODEJoint(dWorldID worldID)
    {
        mJointID = NULL;
        mAMotorID = NULL;
        mWorldID = worldID;
    }

    ODEJoint::~ODEJoint()
    {
        dJointDestroy(mJointID);
        if (mAMotorID)
        {
            dJointDestroy(mAMotorID);
        }
    }

    void ODEJoint::init(const JointData& data)
    {
        if (mInitCalled)
        {
            // If this Joint has already been initialized, destroy the ODE
            // Joint ID.
            dJointDestroy(mJointID);
            if (mAMotorID)
            {
                dJointDestroy(mAMotorID);
            }

            // Reset event handler.
            mJointBreakEventHandler = NULL;
        }

        Joint::init(data);
        mInitCalled = true;

        switch (data.getType())
        {
        case HINGE_JOINT:
            // Create an ODE hinge Joint.
            mJointID = dJointCreateHinge(mWorldID, 0);
            mNumAxes = 1;

            // Set the rotational property of each axis.
            mAxisRotational[0] = true;
            mAxisRotational[1] = false;
            mAxisRotational[2] = false;

            // Set the ODE fudge factor for each axis.
            setJointParam(dParamFudgeFactor,
                           defaults::ode::jointFudgeFactor);
            break;
        case UNIVERSAL_JOINT:
            // Create an ODE universal Joint.
            mJointID = dJointCreateUniversal(mWorldID, 0);
            mNumAxes = 2;

            // Set the rotational property of each axis.
            mAxisRotational[0] = true;
            mAxisRotational[1] = true;
            mAxisRotational[2] = false;

            // Set the ODE fudge factor for each axis.
            setJointParam(dParamFudgeFactor,
                           defaults::ode::jointFudgeFactor);
            setJointParam(dParamFudgeFactor2,
                           defaults::ode::jointFudgeFactor);
            break;
        case BALL_JOINT:
            // Create an ODE ball Joint.
            mJointID = dJointCreateBall(mWorldID, 0);
            mNumAxes = 3;

            // Set the rotational property of each axis.
            mAxisRotational[0] = true;
            mAxisRotational[1] = true;
            mAxisRotational[2] = true;

            // ODE ball Joints need this special "angular motor" to
            // restrain their movement e.g. when limits are used.
            mAMotorID = dJointCreateAMotor(mWorldID, 0);

            // Set the ODE fudge factor for each axis.
            setJointParam(dParamFudgeFactor,
                           defaults::ode::jointFudgeFactor);
            setJointParam(dParamFudgeFactor2,
                           defaults::ode::jointFudgeFactor);
            setJointParam(dParamFudgeFactor3,
                           defaults::ode::jointFudgeFactor);
            break;
        case SLIDER_JOINT:
            // Create an ODE slider Joint.
            mJointID = dJointCreateSlider(mWorldID, 0);
            mNumAxes = 1;

            // Set the rotational property of each axis.
            mAxisRotational[0] = false;
            mAxisRotational[1] = false;
            mAxisRotational[2] = false;

            // Set the ODE fudge factor for each axis.
            setJointParam(dParamFudgeFactor,
                           defaults::ode::jointFudgeFactor);
            break;
        case WHEEL_JOINT:
            // Create an ODE hinge2 Joint.
            mJointID = dJointCreateHinge2(mWorldID, 0);
            mNumAxes = 2;

            // Set the rotational property of each axis.
            mAxisRotational[0] = true;
            mAxisRotational[1] = true;
            mAxisRotational[2] = false;

            // Set the ODE fudge factor for each axis.
            setJointParam(dParamFudgeFactor,
                           defaults::ode::jointFudgeFactor);
            setJointParam(dParamFudgeFactor2,
                           defaults::ode::jointFudgeFactor);
            break;
        case FIXED_JOINT:
            // Create an ODE fixed Joint.
            mJointID = dJointCreateFixed(mWorldID, 0);
            mNumAxes = 0;

            // Set the rotational property of each axis.
            mAxisRotational[0] = false;
            mAxisRotational[1] = false;
            mAxisRotational[2] = false;
            break;
        default:
            assert(false);
            break;
        }

        // Tell ODE about the JointFeedback struct for this Joint.
        // TODO: for ball joints, should this get the joint id or the
        // amotor id?
        mJointFeedback.f1[0] = 0;
        mJointFeedback.f1[1] = 0;
        mJointFeedback.f1[2] = 0;
        mJointFeedback.t1[0] = 0;
        mJointFeedback.t1[1] = 0;
        mJointFeedback.t1[2] = 0;
        mJointFeedback.f2[0] = 0;
        mJointFeedback.f2[1] = 0;
        mJointFeedback.f2[2] = 0;
        mJointFeedback.t2[0] = 0;
        mJointFeedback.t2[1] = 0;
        mJointFeedback.t2[2] = 0;
        dJointSetFeedback(mJointID, &mJointFeedback);

        // Setup the Solids.
        filterSolidForStaticness(data.solid0, data.solid1);

        if (!mData.isBroken)
        {
            // Attach the Joint to the ODE bodies.
            attachODEBodies(mData.solid0, mData.solid1);
        }

        if (BALL_JOINT == data.getType())
        {
			// This "AMotor mode" must be set for ball joints, but 
			// it must happen only after the bodies have been 
			// attached.
            dJointSetAMotorMode(mAMotorID, dAMotorEuler);
		}

        // Setup the Joint's anchor.
        setAnchor(data.anchor);

        // Setup the Joint's axes.
        setAxis(0, data.axis[0]);
        setAxis(1, data.axis[1]);
        setAxis(2, data.axis[2]);

        // Set the ODE joint's userdata pointer.
        if (BALL_JOINT == mData.getType())
        {
            dJointSetData(mAMotorID, this);
        }
        else
        {
            dJointSetData(mJointID, this);
        }
    }

    void ODEJoint::filterSolidForStaticness(Solid* s0, Solid* s1)
    {
        Solid * temp0 = s0;
        Solid * temp1 = s1;
        mData.enabled = true;
        if (s0 == s1)
        {
            temp0 = NULL;
            temp1 = NULL;
            mData.enabled = false;
        }
        else
        {
            if (s0 && s0->isStatic())
                temp0 = NULL;
            if (s1 && s1->isStatic())
                temp1 = NULL;

            if (temp0 == NULL && temp1 == NULL)
                mData.enabled = false;
        }

        // now we can safely set
        Joint::setSolids(temp0, temp1);
    }

    real ODEJoint::getAngle(int axisNum) const
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);

        real value = 0;

        switch (mData.getType())
        {
        case HINGE_JOINT:
            value = radToDeg((real) dJointGetHingeAngle(mJointID));
            break;
        case UNIVERSAL_JOINT:
            if (0 == axisNum)
            {
                value =
                    radToDeg((real) dJointGetUniversalAngle1(mJointID));
            }
            else
            {
                value =
                    radToDeg((real) dJointGetUniversalAngle2(mJointID));
            }
            break;
        case BALL_JOINT:
            value =
                radToDeg((real) dJointGetAMotorAngle(mAMotorID, axisNum));
            break;
        case SLIDER_JOINT:
            value = 0;
            break;
        case WHEEL_JOINT:
            if (0 == axisNum)
            {
                value = radToDeg((real) dJointGetHinge2Angle1(mJointID));
            }
            else
            {
                // ODE does not give us axis 1 angle.
                assert(false);
            }
            break;
        case FIXED_JOINT:
            // Fixed Joints don't have any kind of state info.
            assert(false);
            break;
        default:
            assert(false);
            break;
        }

        return value;
    }

    real ODEJoint::getDistance(int axisNum) const
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);

        real value = 0;

        switch (mData.getType())
        {
        case HINGE_JOINT:
            value = 0;
            break;
        case UNIVERSAL_JOINT:
            value = 0;
            break;
        case BALL_JOINT:
            value = 0;
            break;
        case SLIDER_JOINT:
            value = (real) dJointGetSliderPosition(mJointID);
            break;
        case WHEEL_JOINT:
            value = 0;
            break;
        case FIXED_JOINT:
            // Fixed Joints don't have any kind of state info.
            assert(false);
            break;
        default:
            assert(false);
            break;
        }

        return value;
    }

    real ODEJoint::getVelocity(int axisNum) const
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);

        real value = 0;

        switch (mData.getType())
        {
        case HINGE_JOINT:
            value = radToDeg((real) dJointGetHingeAngleRate(mJointID));
            break;
        case UNIVERSAL_JOINT:
            if (0 == axisNum)
            {
                value = radToDeg(
                            (real) dJointGetUniversalAngle1Rate(mJointID));
            }
            else
            {
                value = radToDeg(
                            (real) dJointGetUniversalAngle2Rate(mJointID));
            }
            break;
        case BALL_JOINT:
            value = radToDeg(
                        (real) dJointGetAMotorAngleRate(mAMotorID, axisNum));
            break;
        case SLIDER_JOINT:
            value = (real) dJointGetSliderPositionRate(mJointID);
            break;
        case WHEEL_JOINT:
            if (0 == axisNum)
            {
                value = radToDeg(
                            (real) dJointGetHinge2Angle1Rate(mJointID));
            }
            else
            {
                value = radToDeg(
                            (real) dJointGetHinge2Angle2Rate(mJointID));
            }
            break;
        case FIXED_JOINT:
            // Fixed Joints don't have rates.
            assert(false);
            break;
        default:
            assert(false);
            break;
        }

        return value;
    }

    //void ODEJoint::setLimits(int axisNum, JointLimits l)
    //{

    //}

    //const JointLimits& ODEJoint::getLimits(int axisNum)const
    //{

    //}

    void ODEJoint::setLimitHardness(int axisNum, real h)
    {
        Joint::setLimitHardness(axisNum, h);

        // Calculate the ODE ERP value by scaling hardness by a normalized
        // ERP range.
        real value = h * (defaults::ode::maxERP - defaults::ode::minERP) +
                     defaults::ode::minERP;

        // Set the ODE ERP parameter.
        switch (axisNum)
        {
        case 0:
            setJointParam(dParamStopERP, value);
            break;
        case 1:
            setJointParam(dParamStopERP2, value);
            break;
        case 2:
            setJointParam(dParamStopERP3, value);
            break;
        default:
            assert(false);
            break;
        }

        // ODE's Hinge2 Joint also has a suspension parameter.  Use axis 1
        // for this since axis 1 doesn't use limits anyway.
        if (1 == axisNum && WHEEL_JOINT == mData.getType())
        {
            setJointParam(dParamSuspensionERP, value);
        }
    }

    void ODEJoint::setLimitBounciness(int axisNum, real b)
    {
        Joint::setLimitBounciness(axisNum, b);

        // Calculate the ODE bounce value by scaling hardness by a
        // normalized ERP range.
        real value = b * (defaults::ode::maxERP - defaults::ode::minERP) +
                     defaults::ode::minERP;

        // Set the ODE bounce parameter.
        switch (axisNum)
        {
        case 0:
            setJointParam(dParamBounce, value);
            break;
        case 1:
            setJointParam(dParamBounce2, value);
            break;
        case 2:
            setJointParam(dParamBounce3, value);
            break;
        default:
            assert(false);
            break;
        }
    }

    JointAxis ODEJoint::getAxis(int axisNum) const
    {
        // Silently ignore invalid axes.
        if (axisNum < 0 || axisNum >= mNumAxes)
        {
            return JointAxis();
        }

        // First we need to get an updated direction vector from ODE.
        dVector3 direction;

        switch (mData.getType())
        {
        case HINGE_JOINT:
            dJointGetHingeAxis(mJointID, direction);
            break;
        case UNIVERSAL_JOINT:
            if (0 == axisNum)
            {
                dJointGetUniversalAxis1(mJointID, direction);
            }
            else
            {
                dJointGetUniversalAxis2(mJointID, direction);
            }
            break;
        case BALL_JOINT:
            if (0 == axisNum)
            {
                dJointGetAMotorAxis(mAMotorID, 0, direction);
            }
            else if (1 == axisNum)
            {
                dJointGetAMotorAxis(mAMotorID, 1, direction);
            }
            else
            {
                dJointGetAMotorAxis(mAMotorID, 2, direction);
            }
            break;
        case SLIDER_JOINT:
            dJointGetSliderAxis(mJointID, direction);
            break;
        case WHEEL_JOINT:
            if (0 == axisNum)
            {
                dJointGetHinge2Axis1(mJointID, direction);
            }
            else
            {
                dJointGetHinge2Axis2(mJointID, direction);
            }
            break;
        case FIXED_JOINT:
            // Fixed Joints don't have any axes.
            break;
        default:
            assert(false);
            break;
        }

        JointAxis axis = mData.axis[axisNum];

        // All data in this JointAxis is valid except for the direction
        // vector.
        axis.direction.set((real) direction[0],
                            (real) direction[1],
                            (real) direction[2]);

        return axis;
    }

    Point3r ODEJoint::getAnchor() const
    {
        // We need to get an updated anchor point from ODE.
        dVector3 anchor;

        switch (mData.getType())
        {
        case HINGE_JOINT:
            dJointGetHingeAnchor(mJointID, anchor);
            break;
        case UNIVERSAL_JOINT:
            dJointGetUniversalAnchor(mJointID, anchor);
            break;
        case BALL_JOINT:
            dJointGetBallAnchor(mJointID, anchor);
            break;
        case SLIDER_JOINT:
            // Slider Joints don't have an anchor point.
            break;
        case WHEEL_JOINT:
            dJointGetHinge2Anchor(mJointID, anchor);
            break;
        case FIXED_JOINT:
            // Fixed Joints don't have an anchor point.
            break;
        default:
            assert(false);
            break;
        }

        return Point3r((real) anchor[0], (real) anchor[1], (real) anchor[2]);
    }

    void ODEJoint::internal_update()
    {
        if (mData.enabled)
        {
            // Update the Joint's damage status.
            updateDamage(calcStress());
        }
    }

    void ODEJoint::internal_setDesiredVel(int axisNum, real value)
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);

        switch (axisNum)
        {
        case 0:
            setJointParam(dParamVel, value);
            break;
        case 1:
            setJointParam(dParamVel2, value);
            break;
        case 2:
            setJointParam(dParamVel3, value);
            break;
        default:
            assert(false);
            break;
        }
    }

    void ODEJoint::internal_setMaxTorque(int axisNum, real value)
    {
        assert(axisNum >= 0 && axisNum < mNumAxes);

        switch (axisNum)
        {
        case 0:
            setJointParam(dParamFMax, value);
            break;
        case 1:
            setJointParam(dParamFMax2, value);
            break;
        case 2:
            setJointParam(dParamFMax3, value);
            break;
        default:
            assert(false);
            break;
        }
    }

    void ODEJoint::setAxis(int axisNum, const JointAxis& axis)
    {
        // Silently ignore invalid axes.
        //assert(axisNum >= 0 && axisNum < mNumAxes);
        if (axisNum < 0 || axisNum >= mNumAxes)
        {
            return ;
        }

        // Normalize the direction vector.
        JointAxis normAxis = axis;
        normAxis.direction.normalize();

        // Store the axis (with normalized direction vector).
        Joint::setAxis(axisNum, normAxis);

        // Create an ODE-style vector for the direction vector.
        dVector3 newAxis = {(dReal) normAxis.direction[0],
                            (dReal) normAxis.direction[1], (dReal) normAxis.direction[2] };

        // Set the ODE Joint axis direction vector.
        switch (mData.getType())
        {
        case HINGE_JOINT:
            dJointSetHingeAxis(mJointID, newAxis[0], newAxis[1],
                                newAxis[2]);
            break;
        case UNIVERSAL_JOINT:
            if (0 == axisNum)
            {
                dJointSetUniversalAxis1(mJointID, newAxis[0], newAxis[1],
                                         newAxis[2]);
            }
            else
            {
                dJointSetUniversalAxis2(mJointID, newAxis[0], newAxis[1],
                                         newAxis[2]);
            }
            break;
        case BALL_JOINT:
            // From ODE docs:
            // For dAMotorEuler mode:
            // 1. Only axes 0 and 2 need to be set. Axis 1 will be
            //	determined automatically at each time step.
            // 2. Axes 0 and 2 must be perpendicular to each other.
            // 3. Axis 0 must be anchored to the first body, axis 2 must
            //	be anchored to the second body.

            if (0 == axisNum)
            {
                dJointSetAMotorAxis(mAMotorID, 0, 1, newAxis[0],
                                     newAxis[1], newAxis[2]);
            }
            else if (2 == axisNum)
            {
                dJointSetAMotorAxis(mAMotorID, 2, 2, newAxis[0],
                                     newAxis[1], newAxis[2]);
            }
            break;
        case SLIDER_JOINT:
            dJointSetSliderAxis(mJointID, newAxis[0], newAxis[1],
                                 newAxis[2]);
            break;
        case WHEEL_JOINT:
            if (0 == axisNum)
            {
                dJointSetHinge2Axis1(mJointID, newAxis[0], newAxis[1],
                                      newAxis[2]);
            }
            else
            {
                dJointSetHinge2Axis2(mJointID, newAxis[0], newAxis[1],
                                      newAxis[2]);
            }
            break;
        case FIXED_JOINT:
            // Fixed Joints don't have any axes.
            break;
        default:
            assert(false);
            break;
        }

        // Set whether the axis limits are enabled.
        setLimitsEnabled(axisNum, normAxis.limitsEnabled);

        // Set the axis' limit range.
        setLimitRange(axisNum, normAxis.limits.low, normAxis.limits.high);

        // Set the limits' hardness.
        setLimitHardness(axisNum, normAxis.limits.hardness);

        // Set the limits' bounciness.
        setLimitBounciness(axisNum, normAxis.limits.bounciness);
    }

    void ODEJoint::setAnchor(const Point3r& anchor)
    {
        Joint::setAnchor(anchor);
        dVector3 newAnchor = {(dReal) anchor[0], (dReal) anchor[1],
                              (dReal) anchor[2] };

        switch (mData.getType())
        {
        case HINGE_JOINT:
            dJointSetHingeAnchor(mJointID, newAnchor[0], newAnchor[1],
                                  newAnchor[2]);
            break;
        case UNIVERSAL_JOINT:
            dJointSetUniversalAnchor(mJointID, newAnchor[0],
                                      newAnchor[1], newAnchor[2]);
            break;
        case BALL_JOINT:
            dJointSetBallAnchor(mJointID, newAnchor[0], newAnchor[1],
                                 newAnchor[2]);
            break;
        case SLIDER_JOINT:
            // Slider Joints don't have an anchor point.
            break;
        case WHEEL_JOINT:
            dJointSetHinge2Anchor(mJointID, newAnchor[0], newAnchor[1],
                                   newAnchor[2]);
            break;
        case FIXED_JOINT:
            // Fixed Joints don't have an anchor point.
            break;
        default:
            assert(false);
            break;
        }
    }

    void ODEJoint::setLimitsEnabled(int axisNum, bool e)
    {
        Joint::setLimitsEnabled(axisNum, e);

        if (e)
        {
            real low = mData.axis[axisNum].limits.low;
            real high = mData.axis[axisNum].limits.high;

            if (isRotational(axisNum))
            {
                low = degToRad(low);
                high = degToRad(high);
            }

            switch (axisNum)
            {
            case 0:
                // Both limits must be set twice because of a ODE bug in
                // the limit setting function.
                setJointParam(dParamLoStop, low);
                setJointParam(dParamHiStop, high);
                setJointParam(dParamLoStop, low);
                setJointParam(dParamHiStop, high);
                break;
            case 1:
                // Both limits must be set twice because of a ODE bug in
                // the limit setting function.
                setJointParam(dParamLoStop2, low);
                setJointParam(dParamHiStop2, high);
                setJointParam(dParamLoStop2, low);
                setJointParam(dParamHiStop2, high);
                break;
            case 2:
                // Both limits must be set twice because of a ODE bug in
                // the limit setting function.
                setJointParam(dParamLoStop3, low);
                setJointParam(dParamHiStop3, high);
                setJointParam(dParamLoStop3, low);
                setJointParam(dParamHiStop3, high);
                break;
            default:
                assert(false);
                break;
            }
        }
        else
        {
            switch (axisNum)
            {
            case 0:
                // Both limits must be set twice because of a ODE bug in
                // the limit setting function.
                setJointParam(dParamLoStop, -dInfinity);
                setJointParam(dParamHiStop, dInfinity);
                setJointParam(dParamLoStop, -dInfinity);
                setJointParam(dParamHiStop, dInfinity);
                break;
            case 1:
                // Both limits must be set twice because of a ODE bug in
                // the limit setting function.
                setJointParam(dParamLoStop2, -dInfinity);
                setJointParam(dParamHiStop2, dInfinity);
                setJointParam(dParamLoStop2, -dInfinity);
                setJointParam(dParamHiStop2, dInfinity);
                break;
            case 2:
                // Both limits must be set twice because of a ODE bug in
                // the limit setting function.
                setJointParam(dParamLoStop3, -dInfinity);
                setJointParam(dParamHiStop3, dInfinity);
                setJointParam(dParamLoStop3, -dInfinity);
                setJointParam(dParamHiStop3, dInfinity);
                break;
            default:
                assert(false);
                break;
            }
        }
    }

    void ODEJoint::setEnabled(bool e)
    {
        if (!mInitCalled)
        {
            return ;
        }

        // if are enabling a joint, we need to properly alter the solids
        if (e == true)
            filterSolidForStaticness(mData.solid0, mData.solid1);

        if (NULL == mData.solid0 && NULL == mData.solid1)
        {
            return ;
        }
        else
        {
            Joint::setEnabled(e);

            if (e)
            {
                // Enable the joint.
                attachODEBodies(mData.solid0, mData.solid1);
            }
            else
            {
                // Disable the joint by setting both bodies to 0.
                attachODEBodies(NULL, NULL);
            }
        }
    }

    void ODEJoint::setLimitRange(int axisNum, real low, real high)
    {
        Joint::setLimitRange(axisNum, low, high);

        // If this axis is rotational, convert the limit angles from
        // degrees to radians.
        if (isRotational(axisNum))
        {
            low = degToRad(low);
            high = degToRad(high);
        }

        // Only set the ODE limits if the limits for this particular axis
        // are enabled.
        switch (axisNum)
        {
        case 0:
            if (mData.axis[axisNum].limitsEnabled)
            {
                // Both limits must be set twice because of a ODE bug in
                // the limit setting function.
                setJointParam(dParamLoStop, low);
                setJointParam(dParamHiStop, high);
                setJointParam(dParamLoStop, low);
                setJointParam(dParamHiStop, high);
            }
            break;
        case 1:
            if (mData.axis[axisNum].limitsEnabled)
            {
                // Both limits must be set twice because of a ODE bug in
                // the limit setting function.
                setJointParam(dParamLoStop2, low);
                setJointParam(dParamHiStop2, high);
                setJointParam(dParamLoStop2, low);
                setJointParam(dParamHiStop2, high);
            }
            break;
        case 2:
            if (mData.axis[axisNum].limitsEnabled)
            {
                // Both limits must be set twice because of a ODE bug in
                // the limit setting function.
                setJointParam(dParamLoStop3, low);
                setJointParam(dParamHiStop3, high);
                setJointParam(dParamLoStop3, low);
                setJointParam(dParamHiStop3, high);
            }
            break;
        default:
            assert(false);
            break;
        }
    }

    void ODEJoint::setJointParam(int parameter, dReal value)
    {
        switch (mData.getType())
        {
        case HINGE_JOINT:
            dJointSetHingeParam(mJointID, parameter, value);
            break;
        case UNIVERSAL_JOINT:
            dJointSetUniversalParam(mJointID, parameter, value);
            break;
        case BALL_JOINT:
            dJointSetAMotorParam(mAMotorID, parameter, value);
            break;
        case SLIDER_JOINT:
            dJointSetSliderParam(mJointID, parameter, value);
            break;
        case WHEEL_JOINT:
            dJointSetHinge2Param(mJointID, parameter, value);
            break;
        case FIXED_JOINT:
            assert(false);
            break;
        default:
            assert(false);
            break;
        }
    }

    //real ODEJoint::getJointParam(int parameter)const
    //{
    //	real value = 0;

    //	switch(mData.type)
    //	{
    //		case HINGE_JOINT:
    //			value = (real)dJointGetHingeParam(mJointID, parameter);
    //			break;
    //		case UNIVERSAL_JOINT:
    //			value = (real)dJointGetUniversalParam(mJointID, parameter);
    //			break;
    //		case BALL_JOINT:
    //			value = (real)dJointGetAMotorParam(mAMotorID, parameter);
    //			break;
    //		case SLIDER_JOINT:
    //			value = (real)dJointGetSliderParam(mJointID, parameter);
    //			break;
    //		case WHEEL_JOINT:
    //			value = (real)dJointGetHinge2Param(mJointID, parameter);
    //			break;
    //		case FIXED_JOINT:
    //			assert(false);
    //			break;
    //		default:
    //			assert(false);
    //			break;
    //	}

    //	return value;
    //}

    real ODEJoint::calcStress()
    {
        // TODO: the damage calculations here should probably be sped up
        // somehow.

        real currentStress = 0;

        switch (mData.breakMode)
        {
        case UNBREAKABLE_MODE:
            // Nothing to do.
            break;
        case THRESHOLD_MODE:
            // Fall through...
        case ACCUMULATED_MODE:
            {
                dJointFeedback* jf = dJointGetFeedback(mJointID);
                Vec3r f1((real) jf->f1[0], (real) jf->f1[1], (real) jf->f1[2]);
                Vec3r t1((real) jf->t1[0], (real) jf->t1[1], (real) jf->t1[2]);
                Vec3r f2((real) jf->f2[0], (real) jf->f2[1], (real) jf->f2[2]);
                Vec3r t2((real) jf->t2[0], (real) jf->t2[1], (real) jf->t2[2]);

                f1 -= f2;
                t1 -= t2;

                // This is a simplification, but it should still work.
                currentStress = f1.length() + t1.length();
                break;
            }
        default:
            assert(false);
        }

        return (currentStress);
    }

    dJointID ODEJoint::internal_getJointID() const
    {
        if (BALL_JOINT == mData.getType())
        {
            return mAMotorID;
        }
        else
        {
            return mJointID;
        }
    }

    void ODEJoint::attachODEBodies(Solid* s0, Solid* s1)
    {
        ODESolid * solid0 = (ODESolid*) s0;
        ODESolid* solid1 = (ODESolid*) s1;

        if (NULL == solid0 && NULL == solid1)
        {
            dJointAttach(mJointID, 0, 0);
            if (BALL_JOINT == mData.getType())
            {
                dJointAttach(mAMotorID, 0, 0);
            }
        }
        else
        {
            if (NULL == solid0)
            {
                dJointAttach(mJointID, 0, solid1->internal_getBodyID());
                if (BALL_JOINT == mData.getType())
                {
                    dJointAttach(mAMotorID, 0, solid1->internal_getBodyID());
                }
            }
            else if (NULL == solid1)
            {
                dJointAttach(mJointID, solid0->internal_getBodyID(), 0);
                if (BALL_JOINT == mData.getType())
                {
                    dJointAttach(mAMotorID, solid0->internal_getBodyID(), 0);
                }
            }
            else
            {
                dJointAttach(mJointID, solid0->internal_getBodyID(),
                              solid1->internal_getBodyID());
                if (BALL_JOINT == mData.getType())
                {
                    dJointAttach(mAMotorID, solid0->internal_getBodyID(),
                                  solid1->internal_getBodyID());
                }
            }

            // Special call for fixed Joints so they remember the current
            // relationship between the Solids or between a Solid and the
            // static environment.
            if (FIXED_JOINT == mData.getType())
            {
                dJointSetFixed(mJointID);
            }
        }
    }
}
