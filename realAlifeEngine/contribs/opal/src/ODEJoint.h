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

#ifndef OPAL_ODE_JOINT_H
#define OPAL_ODE_JOINT_H

#include "external/ode/include/ode/ode.h"

#include "Defines.h"
#include "ODESolid.h"
#include "Joint.h"

namespace opal
{
    /// The ODE implementation of the Joint class.
    class ODEJoint : public Joint
    {
        public:
            ODEJoint(dWorldID worldID);

            virtual ~ODEJoint();

            /// Note: For a perfect save/restore in ODE, the "warm starting"
            /// data should be stored in JointData.  However, there is
            /// currently no easy way to get this data.
            virtual void OPAL_CALL init(const JointData& data);

            virtual real OPAL_CALL getAngle(int axisNum) const;

            virtual real OPAL_CALL getDistance(int axisNum) const;

            virtual real OPAL_CALL getVelocity(int axisNum) const;

            virtual void OPAL_CALL setLimitsEnabled(int axisNum, bool e);

            virtual void OPAL_CALL setEnabled(bool e);

            virtual void OPAL_CALL setLimitRange(int axisNum, real low,
				real high);

            virtual void OPAL_CALL setLimitHardness(int axisNum, real h);

            virtual void OPAL_CALL setLimitBounciness(int axisNum, real b);

            virtual JointAxis OPAL_CALL getAxis(int axisNum) const;

            virtual Point3r OPAL_CALL getAnchor() const;

            virtual void internal_update();

            virtual void internal_setDesiredVel(int axisNum, real value);

            virtual void internal_setMaxTorque(int axisNum, real value);

            /// Returns the ODE joint ID of this ODEJoint.
            dJointID internal_getJointID() const;

        protected:
            virtual void setAxis(int axisNum, const JointAxis& axis);

            virtual void setAnchor(const Point3r& anchor);

            /// Helper function to make it easier to set parameters for
            /// various ODE Joint types.
            void setJointParam(int parameter, dReal value);

            /// Returns the current amount of stress on this Joint.
            virtual real calcStress();

            /// Attaches the ODE Joint to the given ODESolids.
            virtual void attachODEBodies(Solid* s0, Solid* s1);

            //! Static solids are set to NULL internally
            /*!
             * @note this handles the cases when either both are being set
             *       to NULL or both are the same Solid.
             */
            void filterSolidForStaticness(Solid* s0, Solid* s1);

            /// The ODE Joint ID.
            dJointID mJointID;

            /// The ODE AMotor ID, only used for ball Joints.
            dJointID mAMotorID;

            /// The ID of the ODE World containing this Joint.
            dWorldID mWorldID;

            /// An ODE JointFeedback struct that must be allocated somewhere
            /// in order to get the Joint data necessary to calculate Joint
            /// stress.
            dJointFeedback mJointFeedback;

        private:
    };
}

#endif
