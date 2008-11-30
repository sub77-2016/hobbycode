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

#ifndef OPAL_ODE_SIMULATOR_H
#define OPAL_ODE_SIMULATOR_H

// project headers
#include "Defines.h"
#include "Simulator.h"
#include "AccelerationSensor.h"
#include "InclineSensor.h"
#include "RaycastSensor.h"
#include "VolumeSensor.h"
#include "ODESolid.h"
#include "ODEJoint.h"
#include "ODESpace.h"

// system headers
#include "external/ode/include/ode/ode.h"
#include <vector>

namespace opal
{
    /// The ODE implementation of the Simulator class.
    class ODESimulator : public Simulator
    {
        public:
            /// The types of ODE constraint solvers.
            enum SolverType
            {
                /// Time complexity: O(m^3), space complexity O(m^2), where
                /// m = # of constraints.
                SOLVER_WORLDSTEP,

                /// Time complexity: O(m*N), space complexity O(m), where
                /// m = # of constraints and N = # of iterations.
                SOLVER_QUICKSTEP };

            /// use initData to complete
            ODESimulator();

            virtual ~ODESimulator();

			/// Used for creating custom simulators.  This should not be 
			/// called directly.
            void initData(SimulatorData data);

            virtual void OPAL_CALL destroy();

            virtual void OPAL_CALL setMaxCorrectingVel(real vel);

            // SOLIDS

            virtual Solid* OPAL_CALL createSolid();

            // JOINTS

            virtual Joint* OPAL_CALL createJoint();

            // MOTORS

            // No ODE-specific Motor implementations at this time.

            // SENSORS

            // No ODE-specific Sensor implementations at this time.

            // SPACES

            virtual Space* OPAL_CALL createSpace();

            virtual void OPAL_CALL setGravity(const Vec3r& gravity);

            virtual Vec3r OPAL_CALL getGravity() const;

            virtual void OPAL_CALL setSolverAccuracy(SolverAccuracyLevel level);

		    virtual void OPAL_CALL setGlobalStability(real value);

            dWorldID internal_getWorldID() const;

            dSpaceID internal_getSpaceID() const;

            dJointGroupID internal_getJointGroupID() const;

            std::vector<RaycastResult> & internal_fireRay(
                const Rayr& r, real length, const Solid* attachedSolid,
                unsigned int rayContactGroup);

            const VolumeQueryResult& internal_queryVolume(
                const Solid* volume, const Solid* attachedSolid);

            //! Helper function used for ray casting which stores RaycastResults.
            /*!
             * @param depth How far the ray went into the object.
             */
            void internal_addRaycastResult(Solid* solid,
                    const Point3r& intersection, const Vec3r& normal, real depth);

            /// Helper function for volume collision checking.
            void internal_addCollidedSolid(Solid* solid);

            /// Helper function used for ray casting.
            unsigned int internal_getRayContactGroup();

        protected:
            virtual void stepPhysics();

            /// The ODE world ID used by this Simulator.
            dWorldID mWorldID;

            /// The root of the ODE collision detection hierarchy.
            dSpaceID mRootSpaceID;

            /// The ODE joint constraint group.
            dJointGroupID mContactJointGroupID;

            /// The type of constraint solver to use.
            SolverType mSolverType;

            /// Used for volume collision checks.
            long int mCollisionCount;

            /// Temporary list of Solids that collided in a volume collision
            /// check.
            VolumeQueryResult mVolumeQueryResult;

            /// Used for ray casting and volume queries.  If a RaycastSensor or
            /// VolumeSensor is attached to a Solid, this pointer will point to
            /// that Solid.  It is used to make sure the raycasts and collision
            /// query doesn't collide with the attached Solid.
            const Solid* mSensorSolid;

            /// A temporary variable that lets rays to use contact groups.  This
            /// allows them to limit which Shapes they collide with.
            unsigned int mRayContactGroup;

            /// A vector of RaycastResults returned from a ray cast.
            std::vector<RaycastResult> mRaycastResults;

			/// Keeps track of how many times ODESimulator was initialized.  
			/// (ODESimulator::initData increments, and ODESimulator::destroy 
			/// decrements.  dInitODE and dCloseODE are called only if 
			/// mInitCounter == 0.)  This is necessary for multiple simulator 
			/// instances.
			static unsigned int mInitCounter;
    };

    /// A namespace containing internal ODE functions.
    namespace ode_hidden
    {
        // These function don't use the OPAL_CALL calling convention
        // because they are functors passed to ODE, which expects a different
        // calling convention.

        /// Main collision callback functor.
        void internal_collisionCallback(void* data, dGeomID o0, dGeomID o1);

        /// Assuming the two ODE bodies are connected by an ODE joint, this
        /// function returns the OPAL Joint connecting the two bodies'
        /// Solids.
        Joint* internal_getCommonJoint(dBodyID body0, dBodyID body1);

        /// Special collision callback functor for volume collision
        /// checking.
        void internal_volumeCollisionCallback(void* data, dGeomID o0,
			dGeomID o1);

        /// Collision callback functor for ray casting.  Stores results 
		/// in an unsorted array.
        void internal_raycastCollisionCallback(void* data, dGeomID o0,
			dGeomID o1);

        //helper function for collision callback
        //void createOneSidedContact(dJointID contactJoint,
        //	dBodyID movingObject, dBodyID staticObject, dVector3 pos);
    }
}

#endif
