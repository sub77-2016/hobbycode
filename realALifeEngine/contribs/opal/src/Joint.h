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

#ifndef OPAL_JOINT_H
#define OPAL_JOINT_H

#include "Defines.h"
#include "JointBreakEventHandler.h"
#include "JointData.h"

namespace opal
{
    /// A constraint between two Solids or between a Solid and the static
    /// environment.  There is a variety of Joint types, each constraining
    /// Solid motion in different ways.  There are 6 degrees of freedom
    /// for a Solid that can be constrained: 3 rotational and 3 linear.
    /// Each Joint type constrains a different subset of these 6.  When
    /// specifying the two Solids
    /// affected by the Joint, if at least one Solid
    /// is non-NULL, the Joint will be enabled.  If both Solids are
    /// NULL, the Joint will be disabled.  If only one Solid is NULL,
    /// the Joint will attach the other Solid to the static environment.
    /// If both Solids are the same, the Joint's Solids will both be set
    /// to NULL and the Joint will be disabled.  If either Solid is static,
    /// both will be set to NULL, and the Joint will be disabled.
    /// Note that some Joint types
    /// do not use all of the anchor and axis parameters.  An unused anchor
    /// or axis will be ignored (see the JointType description for more
    /// details).  Joints remain ineffective until they are initialized.
    class Joint
    {
        public:
            Joint();

            /// Initializes the Joint with the given data structure.  Calling
            /// this more than once will automatically detach the Joint from
            /// its old Solids first.
            virtual void OPAL_CALL init(const JointData& data);

            /// Returns all data describing the Joint.
            virtual const JointData& OPAL_CALL getData();

            /// Sets the Joint's name.
            virtual void OPAL_CALL setName(const std::string& name);

            /// Returns the Joint's name.
            virtual const std::string& OPAL_CALL getName() const;

            /// Sets whether the Joint's two Solids are constrained by
            /// physical contacts.
            virtual void OPAL_CALL setContactsEnabled(bool e);

            /// Returns whether the Joint's two Solids are constrained by
            /// physical contacts.
            virtual bool OPAL_CALL areContactsEnabled() const;

            /// Returns the Joint type.
            virtual JointType OPAL_CALL getType() const;

            /// Sets the parameters that determine how this Joint will break, if
            /// at all.
            virtual void OPAL_CALL setBreakParams(JointBreakMode mode,
				real breakThresh, real accumThresh = 0);

            //! What is the mode of breaking?
            virtual JointBreakMode OPAL_CALL getBreakingMode() const;

            //! How much damage can a Joint take?
            virtual real OPAL_CALL getBreakThresh() const;

            //! How much damage has a Joint taken so far?
            virtual real OPAL_CALL getAccumulatedDamage() const;

            //! What is minimum amount of damage that will be recorded?
            /*!
             * @note if the damaga is lower than this value, that damage will be ignored
             */
            virtual real OPAL_CALL getAccumulatedThresh() const;

            //! Returns true if the Joint has been broken
            virtual bool OPAL_CALL isBroken() const;

            /// Repairs accumulated damage to breakable Joints in accumulated
            /// damage mode.  This does not reenable the Joint.
            virtual void OPAL_CALL repairAccumDamage();

            /// Sets the Joint's break event handler.
            virtual void OPAL_CALL setJointBreakEventHandler(
                JointBreakEventHandler* eventHandler);

            /// Returns the Joint's break event handler.  If this returns
            /// NULL, the Joint is not using one.
            virtual JointBreakEventHandler* OPAL_CALL
				getJointBreakEventHandler() const;

            /// Enables or disables the given Joint axis' limits.
            virtual void OPAL_CALL setLimitsEnabled(int axisNum, bool e);

            /// Returns true if the given Joint axis' limits are enabled.
            virtual bool OPAL_CALL areLimitsEnabled(int axisNum);

            /// Sets the Joint's limit angles (in degrees for rotational axes,
            /// distance for translational axes).
            /// No limits are applied
            /// if this is not called.  The Wheel Joint does not
            /// use limits for axis 1, so setting this will do nothing.
            virtual void OPAL_CALL setLimitRange(int axisNum, real low,
				real high);

            /// Returns the low limit for a given axis (angle in degrees for
            /// rotational axes, distance for translational axes).
            virtual real OPAL_CALL getLowLimit(int axisNum) const;

            /// Returns the high limit for a given axis (angle in degrees for
            /// rotational axes, distance for translational axes).
            virtual real OPAL_CALL getHighLimit(int axisNum) const;

            /// Sets the hardness for the given axis' limits.  Hardness
            /// represents how "squishy" the limit is.  Hardness must be
            /// between 0 and 1, inclusive.  Setting the hardness for axis 1
            /// of the Wheel Joint will adjust its suspension.
            virtual void OPAL_CALL setLimitHardness(int axisNum, real h);

            /// Returns the hardness for the given axis' limits.
            virtual real OPAL_CALL getLimitHardness(int axisNum) const;

            /// Sets the bounciness for the given axis' limits.  Bounciness
            /// (i.e. restitution) represents how much the Joint will bounce
            /// when it hits a limit.  Bounciness must be between 0 and 1,
            /// inclusive.
            virtual void OPAL_CALL setLimitBounciness(int axisNum, real b);

            /// Returns the bounciness for the given axis' limits.
            virtual real OPAL_CALL getLimitBounciness(int axisNum) const;

            /// For rotational axes, returns the current angle in degrees
            /// measured from the initial Joint configuration.  For translational
            /// axes, simply returns 0.
            virtual real OPAL_CALL getAngle(int axisNum) const = 0;

            /// For translational axes, returns the distance from the initial
            /// Joint configuration.  For rotational axes, simply returns 0.
            virtual real OPAL_CALL getDistance(int axisNum) const = 0;

            /// Returns the current rate (degrees per second for rotational
            /// axes, distance units per second for translational axes) for a
            /// given axis.
            virtual real OPAL_CALL getVelocity(int axisNum) const = 0;

            /// Applies a force to this Joint's Solid(s).  To be used for
            /// translational axes.  This does nothing if the Joint is disabled.
            virtual void OPAL_CALL addForce(int axisNum, real magnitude,
				real duration, bool singleStep = false);

            /// Applies a torque to this Joint's Solid(s).  To be used for
            /// rotational Joints. This does nothing if the Joint is disabled.
            virtual void OPAL_CALL addTorque(int axisNum, real magnitude,
				real duration, bool singleStep = false);

            /// Wakes up this Joint's two Solids.
            virtual void OPAL_CALL wakeSolids();

            /// Returns a pointer to Solid0.
            virtual Solid* OPAL_CALL getSolid0() const;

            /// Returns a pointer to Solid1.
            virtual Solid* OPAL_CALL getSolid1() const;

            /// Returns the current specified axis in global coordinates.
            /// Passing in an invalid axis number will return invalid data.
            virtual JointAxis OPAL_CALL getAxis(int axisNum) const = 0;

            /// Returns the current anchor point in global coordinates.
            /// Passing in an invalid axis number will return invalid data.
            virtual Point3r OPAL_CALL getAnchor() const = 0;

            /// Returns the number of axes used by this Joint.
            virtual int OPAL_CALL getNumAxes() const;

            /// Returns true if the Joint is enabled.
            virtual bool OPAL_CALL isEnabled() const;

            /// Set whether the Joint can affect its Solids.  If both Solids are
            /// NULL, this will remain disabled.  If the Joint has not yet
            /// been initialized, this will have no effect.
            virtual void OPAL_CALL setEnabled(bool e);

            /// Returns true if the given Joint axis is rotational, false if it
            /// is linear.
            virtual bool OPAL_CALL isRotational(int axisNum) const;

            /// Set the user data pointer to some external data.  The user data
            /// is totally user-managed
            /// (i.e. it is not destroyed when the Joint is destroyed).
            virtual void OPAL_CALL setUserData(void* data);

            /// Returns the user data pointer (NULL if it has not been set).
            virtual void* OPAL_CALL getUserData();

            /// Various things could be updated here, including damage values.
            /// If the Joint breaks during this update, it will automatically
            /// be disabled, and the event handler will be notified.
            virtual void internal_update() = 0;

            /// Set the desired linear or angular velocity for this Joint.
            /// This is to be used internally by Motors.
            virtual void internal_setDesiredVel(int axisNum, real value) = 0;

            /// Set the max force this Joint can use to attain its desired
            /// velocity.  This is to be used internally by Motors.
            virtual void internal_setMaxTorque(int axisNum, real value) = 0;

            //bool internal_isBroken();

            /// Returns true if this Joint depends on the given Solid.
            bool internal_dependsOnSolid(Solid* s);

            /// Internal function used to destroy this object.
            void internal_destroy();

        protected:
            virtual ~Joint();

            /// Sets the Solids constrained by this Joint.
            void setSolids(Solid* s0, Solid* s1);

            /// Sets the anchor point for this Joint.  Both Solids must be
            /// valid (non-NULL) before this is called for it to affect anything.
            /// This Joint's Solids must be positioned and attached
            /// before calling this function.
            virtual void setAnchor(const Point3r& anchor);

            /// Specifies the given axis for this Joint.  Invalid axes numbers
            /// will be silently ignored.  The axis direction vector will be
            /// normalized.  This Joint's Solids must be positioned and
            /// attached before calling this function.
            virtual void setAxis(int axisNum, const JointAxis& axis);

            /// Updates this Joint's current damage status based on the current
            /// amount of stress.
            void updateDamage(real currentStress);

            /// Stores data describing the Joint.
            JointData mData;

            /// A pointer to the Joint's break event handler.
            JointBreakEventHandler* mJointBreakEventHandler;

            /// Pointer to user data.  This is totally user-managed (i.e. OPAL
            /// will never delete it).
            void* mUserData;

            /// This is set to true when the Joint is initialized.
            bool mInitCalled;

            /// The number of axes used by the Joint.
            int mNumAxes;

            // This data stores which axes are rotational, as opposed to
            // translational, degrees of freedom.
            bool mAxisRotational[3];

        private:

    };
}

#endif
