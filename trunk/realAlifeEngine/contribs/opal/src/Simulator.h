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

#ifndef OPAL_SIMULATOR_H
#define OPAL_SIMULATOR_H

// project headers
#include "Defines.h"
#include "Motor.h"
#include "Joint.h"
#include "Solid.h"
#include "Space.h"
#include "Blueprint.h"
#include "Sensor.h"
#include "Vec3r.h"

namespace opal
{
	class AttractorMotor;
	class GearedMotor;
	class ServoMotor;
	class SpringMotor;
	class ThrusterMotor;
	class VelocityMotor;
	class AccelerationSensor;
	class InclineSensor;
	class RaycastSensor;
	class VolumeSensor;
	class BlueprintInstance;
	class PostStepEventHandler;
	struct RaycastResult;
	struct VolumeQueryResult;

	/// Contains various Simulator configuration data.
	class SimulatorData
	{
		public:
			SimulatorData()
			{
				useOctreeInsteadHash = false;

				// octree defaults
				worldSize = Vec3r(1000, 1000, 1000);
				octreeDepth = 7;
				worldCenter = Vec3r(0, 0, 0);

				// hash defaults
				hashMinLevel = -5;
				hashMaxLevel = 9;
			}

			/*
			SimulatorData(SimulatorData & copy)
			{
			    useOctreeInsteadHash = copy.useOctreeInsteadHash;

			    // octree defaults
			    worldSize = copy.worldSize;
			    octreeDepth = copy.octreeDepth;
			    worldCenter = copy.worldCenter;

			    // hash defaults
			    hashMinLevel = copy.hashMinLevel;
			    hashMaxLevel = copy.hashMaxLevel;
			}*/

			/// If true, octree space will be used.  Otherwise, hash space 
			/// will be used.
			bool useOctreeInsteadHash;

			/// used for octree space, default - (1000,1000,1000)
			Vec3r worldSize;

			/// used for octree space, default - (0,0,0)
			Vec3r worldCenter;

			/// used for octree space, default - 7
			int octreeDepth;

			/// for hash space: 2^(this value)
			int hashMinLevel;

			/// for hash space: 2^(this value)
			int hashMaxLevel;
	};

	/// A Simulator is an environment that contains simulated objects.
	/// It performs collision detection and physical simulation.  It is
	/// a factory that creates, maintains, and destroys Solids, Joints,
	/// Motors, and Sensors.
	class Simulator
	{
		public:
			Simulator();

			/// Used for creating custom simulators.  This should not be 
			/// called directly.
			void initData(SimulatorData data);

			/// Deallocates everything within the Simulator.  This should be
			/// called when finished with everything.  Simulator implementations
			/// should call "delete this" within this function.
			virtual void OPAL_CALL destroy() = 0;

			/// This function performs collision detection and simulates
			/// everything ahead by the given dt.  Internally, it steps through
			/// the simulation iteratively using a fixed step size (so you can
			/// pass in whatever dt you want; it will be broken up into fixed
			/// increments automatically).  Any "leftover" time will be saved
			/// until the next time this is called.  The function returns true
			/// if at least one time step has been taken.  During collision
			/// detection, the following cases are ignored when deciding whether
			/// to collide two Solids (ignored both for physical contact
			/// generation and collision event handling):
			/// 1. Two static Solids, each without a CollisionEventHandler.
			/// 2. Two Shapes that are part of the same Solid.
			/// 3. Two sleeping Solids.
			/// 4. Two Solids connected by a fixed Joint.
			/// 5. Two Solids connected by a Joint with contacts disabled.
			/// 6. Solid0 is static, Solid1 dynamic and is sleeping,
			///    static-to-sleeping contacts are ignored by the
			///    Simulator, and neither Solid has a
			///    CollisionEventHandler.
			/// 7. Solid1 is static, Solid0 dynamic and is sleeping,
			///    static-to-sleeping contacts are ignored by the
			///    Simulator, and neither Solid has a
			///    CollisionEventHandler.
			/// 8. The two Solids' contact groups do not generate
			///    contacts when they collide, and neither Solid has a
			///    CollisionEventHandler.
			virtual bool OPAL_CALL simulate(real dt);

			/// Sets the constant step size used in the simulation.  The step
			/// size must be greater than zero.
			virtual void OPAL_CALL setStepSize(real stepSize);

			/// Returns the constant step size used in the simulation.
			virtual real OPAL_CALL getStepSize();

			// post events

			//! Sets the Simulator's post-step event handler.
			/*!
			 * @param eventHandler you need to clean it up yourself
			 */
			virtual void OPAL_CALL addPostStepEventHandler(
			    PostStepEventHandler * eventHandler);

			//! If such handler is stored it will be removed from the list.
			virtual void OPAL_CALL removePostStepEventHandler(
			    PostStepEventHandler * eventHandler);

			//! Number of stored step handlers
			virtual size_t OPAL_CALL getNumPostStepEventHandlers() const;

			/// Returns the Simulator's post-step event handler.  If this
			/// returns NULL, the Simulator is not using one.
			virtual PostStepEventHandler* OPAL_CALL
			getPostStepEventHandler(unsigned int id) const;

			// global collision events

			//! Sets the Simulator's collision event handler.
			/*!
			 * @param eventHandler you need to clean it up yourself
			 */
			virtual void OPAL_CALL addGlobalCollisionEventHandler(
			    CollisionEventHandler * eventHandler);

			//! If such handler is stored it will be removed from the list.
			virtual void OPAL_CALL removeGlobalCollisionEventHandler(
			    CollisionEventHandler * eventHandler);

			//! Number of stored collision handlers
			virtual size_t OPAL_CALL getNumGlobalCollisionEventHandlers() const;

			/// Returns the Simulator's post-step event handler.  If this
			/// returns NULL, the Simulator is not using one.
			virtual CollisionEventHandler* OPAL_CALL
			getGlobalCollisionEventHandler(unsigned int id) const;

			/// Creates instances of all objects in a Blueprint and fills the
			/// given BlueprintInstance with pointers of the named objects.  The
			/// offset transform and scale factor can be used to affect
			/// all instantiated objects at once.  The scale factor must be
			/// greater than zero.
			virtual void OPAL_CALL instantiateBlueprint(
			    BlueprintInstance& instance, const Blueprint& bp,
			    const Matrix44r& offset = Matrix44r(), real scale = 1);

			/// Sets the gravity used in the simulation.
			virtual void OPAL_CALL setGravity(const Vec3r& gravity) = 0;

			/// Returns the gravity used in the simulation.
			virtual Vec3r OPAL_CALL getGravity() const = 0;

			/// Sets the accuracy level used by the physics engine's constraint
			/// solver.
			virtual void OPAL_CALL setSolverAccuracy(SolverAccuracyLevel level);

			/// Returns the accuracy level.
			virtual SolverAccuracyLevel OPAL_CALL getSolverAccuracy() const;

			/// Sets the global stability parameter that is used to avoid 
			/// singularities in simulation.  Lower values are more accurate 
			/// but less stable, while higher values are more stable but 
			/// squishy.  This value can range from 0.0 to 1.0, but it should 
			/// be adjusted in orders of magnitude (i.e. if things appear to 
			/// be unstable, try increasing it by an order of magnitude).
			virtual void OPAL_CALL setGlobalStability(real value);

			/// Sets the maximum correcting velocity for interpenetrating
			/// objects.  The given velocity must be positive.
			virtual void OPAL_CALL setMaxCorrectingVel(real vel);

			/// Returns the maximum correcting velocity for interpenetrating
			/// objects.
			virtual real OPAL_CALL getMaxCorrectingVel() const;

			/// Sets the maximum number of physical contacts generated when
			/// two Solids collide.  This number cannot be larger than the
			/// global "max max contacts" parameter.
			virtual void OPAL_CALL setMaxContacts(unsigned int mc);

			/// Returns the maximum number of physical contacts generated
			/// when two Solids collide.
			virtual unsigned int OPAL_CALL getMaxContacts() const;

			/// Sets the maximum linear velocity for any Solid.  This limits
			/// Solid motion to prevent explosions from numerical innacuracy.
			//virtual void OPAL_CALL setMaxLinearVel(real max);

			/// Returns the maximum linear velocity for any Solid.  This limits
			/// Solid motion to prevent explosions from numerical innacuracy.
			//virtual real OPAL_CALL getMaxLinearVel()const;

			/// Sets the maximum angular velocity for any Solid.  This limits
			/// Solid motion to prevent explosions from numerical innacuracy.
			//virtual void OPAL_CALL setMaxAngularVel(real max);

			/// Returns the maximum angular velocity for any Solid.  This limits
			/// Solid motion to prevent explosions from numerical innacuracy.
			//virtual real OPAL_CALL getMaxAngularVel()const;

			/// Set the user data pointer to some external data.  The user data
			/// is totally user-managed
			/// (i.e. it is not destroyed when the Simulator is destroyed).
			virtual void OPAL_CALL setUserData(void* data);

			/// Returns the user data pointer (NULL if it has not been set).
			virtual void* OPAL_CALL getUserData();

			/// Defines the interaction between two contact groups.  If the
			/// last argument is true, the two groups will generate physical
			/// points when they collide.  Otherwise, they will pass through
			/// each other.  Keep in mind that certain cases are already
			/// ignored when performing collision detection; see comments
			/// on Simulator::simulate for more details.
			/// Note that contact groups do not affect collision events; two
			/// colliding objects might not generate contacts and still
			/// generate collision events sent to their CollisionEventHandlers.
			virtual void OPAL_CALL setupContactGroups(unsigned int group0,
			        unsigned int group1, bool makeContacts);

			/// Similar to setupContactGroups.  Determines how a single contact
			/// group interacts with all other groups.  This is useful when
			/// you want to e.g. disable collisions between a group and
			/// everything else for a "trigger volume."  (See setupContactGroups
			/// for more details).
			virtual void OPAL_CALL setupContactGroup(unsigned int group,
			        bool makeContacts);

			/// Returns true if the two contact groups are setup to generate
			/// contacts when they collide.
			virtual bool OPAL_CALL groupsMakeContacts(unsigned int group0,
			        unsigned int group1);

			/// Sets whether contacts should be generated between static Solids
			/// and sleeping Solids.  Usually this isn't necessary, but
			/// sometimes you might want a static Solid to wake up a sleeping
			/// dynamic Solid by touching it.
			virtual void OPAL_CALL setStaticSleepingContactsEnabled(bool enable);

			/// Returns true if contacts are generated between static Solids
			/// and sleeping Solids.
			virtual bool OPAL_CALL areStaticSleepingContactsEnabled();

			// SOLIDS

			/// Creates and returns a pointer to a Solid.
			virtual Solid* OPAL_CALL createSolid() = 0;

			/// Helper function for creating a static Solid with a Plane Shape.
			virtual Solid* OPAL_CALL createPlane(real a, real b, real c, real d,
			                                      const Material& m = defaults::shape::material);

			/// Returns the number of Solids in the Simulator.
			virtual unsigned int OPAL_CALL getNumSolids() const;

			/// Returns a pointer to the Solid at the given index.
			virtual Solid* OPAL_CALL getSolid(unsigned int i) const;

			/// Immediately destroys the given Solid.  All Joints, Motors, and
			/// Sensors that depend on this Solid will be automatically
			/// disabled.
			virtual void OPAL_CALL destroySolid(Solid* s);

			/// Immediately destroys all Solids in the Simulator.
			virtual void OPAL_CALL destroyAllSolids();

			// JOINTS

			/// Creates and returns a pointer to a Joint.
			virtual Joint* OPAL_CALL createJoint() = 0;

			/// Immediately destroys the given Joint.  All Motors that depend
			/// on this Solid will be automatically disabled.
			virtual void OPAL_CALL destroyJoint(Joint* j);

			/// Immediately destroys all Joints in the Simulator.
			virtual void OPAL_CALL destroyAllJoints();

			// MOTORS

			/// Creates and returns a pointer to a ThrusterMotor.
			virtual ThrusterMotor* OPAL_CALL createThrusterMotor();

			/// Creates and returns a pointer to a VelocityMotor.
			virtual VelocityMotor* OPAL_CALL createVelocityMotor();

			/// Creates and returns a pointer to a GearedMotor.
			virtual GearedMotor* OPAL_CALL createGearedMotor();

			/// Creates and returns a pointer to a ServoMotor.
			virtual ServoMotor* OPAL_CALL createServoMotor();

			/// Creates and returns a pointer to an AttractorMotor.
			virtual AttractorMotor* OPAL_CALL createAttractorMotor();

			/// Creates and returns a pointer to a SpringMotor.
			virtual SpringMotor* OPAL_CALL createSpringMotor();

			/// Inform the Simulator of a custom Motor.  Custom Motors are
			/// allocated and deallocated by the user
			//virtual void OPAL_CALL registerCustomMotor(Motor* m);

			/// Immediately destroys the given Motor.
			virtual void OPAL_CALL destroyMotor(Motor* m);

			/// Immediately destroys all Motors in the Simulator.
			virtual void OPAL_CALL destroyAllMotors();

			// SENSORS

			/// Creates and returns a pointer to an AccelerationSensor.
			virtual AccelerationSensor* OPAL_CALL createAccelerationSensor();

			/// Creates and returns a pointer to an InclineSensor.
			virtual InclineSensor* OPAL_CALL createInclineSensor();

			/// Creates and returns a pointer to a RaycastSensor.
			virtual RaycastSensor* OPAL_CALL createRaycastSensor();

			/// Creates and returns a pointer to a VolumeSensor.
			virtual VolumeSensor* OPAL_CALL createVolumeSensor();

			/// Immediately destroys the given Sensor.
			virtual void OPAL_CALL destroySensor(Sensor* s);

			/// Immediately destroys all Sensors in the Simulator.
			virtual void OPAL_CALL destroyAllSensors();

			// SPACES

			/// Creates and returns a pointer to a Space which is a child of
			/// the Simulator's root Space.
			virtual Space* OPAL_CALL createSpace() = 0;

			virtual Space* OPAL_CALL getRootSpace();

			/// Helper function used for ray casting.  Immediately fires a ray
			/// into the scene and returns intersections results.  Uses the
			/// ray's contact group parameter to limit collision checks.
			virtual std::vector<RaycastResult> & internal_fireRay(
			    const Rayr& r, real length, const Solid* attachedSolid,
			    unsigned int rayContactGroup) = 0;

			/// Helper function used for volume queries.
			virtual const VolumeQueryResult& internal_queryVolume(
			    const Solid* volume, const Solid* attachedSolid) = 0;

			/// Returns the Simulator's contact group flags.
			unsigned long int OPAL_CALL
				internal_getContactGroupFlags(unsigned int groupNum) const;

			//! Record collisions into global handlers
			void internal_recordCollision(const CollisionEvent & event);

		protected:
			virtual ~Simulator();

			/// This function is physics engine-specific.  It handles collision
			/// detection and steps the simulation ahead by a constant step
			/// size.
			virtual void stepPhysics() = 0;

			/// Adds a Solid to the internal list of Solids.
			void addSolid(Solid* s);

			/// Removes a Solid from the internal list of Solids.
			void removeSolid(Solid* s);

			/// Adds a Joint to the internal list of Joints.
			void addJoint(Joint* j);

			/// Removes a Joint from the internal list of Joints.
			void removeJoint(Joint* j);

			/// Adds a Motor to the internal list of Motors.
			void addMotor(Motor* m);

			/// Removes a Motor from the internal list of Motors.
			void removeMotor(Motor* m);

			/// Adds a Sensor to the internal list of Sensors.
			void addSensor(Sensor* s);

			/// Removes a Sensor from the internal list of Sensors.
			void removeSensor(Sensor* s);

			/// Adds a Space to the internal list of Spaces.
			void addSpace(Space* s);

			/// Destroys all objects marked as garbage.  Useful for destroying
			/// objects at safe times.
			void destroyGarbage();

			/// The constant step size used to break arbitrary simulation dt
			/// values into constant chunks.
			real mStepSize;

			/// Maintains leftover dt from previous simulation steps.  This is
			/// useful when a dt smaller than the step size is requested; the
			/// dt will accumulate until there is enough for a single step.
			real mTimeBuffer;

			/// The accuracy level used internally by the physics engine's
			/// constraint solver.
			SolverAccuracyLevel mSolverAccuracyLevel;

			/// Pointer to user data.  This is totally user-managed (i.e. OPAL
			/// will never delete it).
			void* mUserData;

			/// A limit on Solid motion to protect against simulation explosions
			/// due to numerical inaccuracy.
			//real mMaxLinearVel;

			/// A limit on Solid motion to protect against simulation explosions
			/// due to numerical inaccuracy.
			//real mMaxAngularVel;

			/// An internal list of all Solids.
			std::vector<Solid*> mSolidList;

			/// An internal list of all Joints.
			std::vector<Joint*> mJointList;

			/// An internal list of all Motors.
			std::vector<Motor*> mMotorList;

			/// An internal list of all Sensors.
			std::vector<Sensor*> mSensorList;

			/// True when it is safe to destroy a Solid (e.g. not in the middle
			/// of looping over all the Solid).
			bool mIsSolidDestructionSafe;

			/// True when it is safe to destroy a Joint (e.g. not in the middle
			/// of looping over all the Joint).
			bool mIsJointDestructionSafe;

			/// An internal list of Solids marked as garbage.
			std::vector<Solid*> mSolidGarbageList;

			/// An internal list of Joints marked as garbage.
			std::vector<Joint*> mJointGarbageList;

			/// Spaces are stored here so the user doesn't have to destroy them;
			/// they get destroyed when the Simulator is destroyed.
			std::vector<Space*> mSpaceList;

			/// parent of all spaces
			Space * mRootSpace;

			/// A set of bitfields used to describe how different contact groups
			/// interact.
			unsigned long int mContactGroupFlags[32];

			/// True if contacts are generated between static Solids and
			/// sleeping Solids.
			bool mStaticSleepingContactsEnabled;

			/// Pointer to the Simulator's post-step event handler.
			std::vector<PostStepEventHandler *> mPostStepEventHandlers;

			/// Global collision handlers.
			std::vector<CollisionEventHandler *> mCollisionEventHandlers;

			/// The maximum correcting velocity used when forcing apart
			/// interpenetrating objects.
			real mMaxCorrectingVel;

			/// The maximum number of physical contacts generated when two
			/// Solids collide.
			unsigned int mMaxContacts;

			/// data
			SimulatorData mData;

		private:
	};
}

#endif

