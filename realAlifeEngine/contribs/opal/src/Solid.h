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

#ifndef OPAL_SOLID_H
#define OPAL_SOLID_H

// project headers
#include "CollisionEventHandler.h"
#include "Defines.h"
#include "MovementEventHandler.h"
#include "SolidData.h"

namespace opal
{
	class Mass;
	class Simulator;
	class Space;
	class ShapeData;

	/// Solids are the physical objects in a simulation.  Solids can be
	/// static or dynamic: basically, dynamic Solids move, and static Solids
	/// don't move.  (Static Solids can still be positioned manually.)  All
	/// Solids start out enabled, but they don't do much until Shapes are
	/// added.
	class Solid
	{
		public:
			Solid();

			/// Initializes the Solid with the given data structure.  Calling
			/// this more than once will automatically destroy all the old
			/// Shapes before adding new ones.
			virtual void OPAL_CALL init(const SolidData& data) = 0;

			/// Returns all data describing the Solid.
			virtual const SolidData& OPAL_CALL getData();

			/// Sets the Solid's name.
			virtual void OPAL_CALL setName(const std::string& name);

			/// Returns the Solid's name.
			virtual const std::string& OPAL_CALL getName() const;

			/// Returns true if the Solid is enabled.
			virtual bool OPAL_CALL isEnabled() const;

			/// Sets whether the Solid can collide with other Solids and be
			/// physically simulated.  Forces applied to this Solid while
			/// disabled will be ignored.
			virtual void OPAL_CALL setEnabled(bool e);

			/// Returns true if this is a static Solid.
			virtual bool OPAL_CALL isStatic() const;

			/// Sets whether the Solid should be static or dynamic.
			virtual void OPAL_CALL setStatic(bool s) = 0;

			/// Returns whether the Solid is affected by gravity.
			virtual bool OPAL_CALL isGravityEnabled()const;

			/// Sets whether the Solid is affected by gravity.
			virtual void OPAL_CALL setGravityEnabled(bool e) = 0;

			/// Removes the Solid from its current Space and adds it to the new
			/// Space.
			virtual void OPAL_CALL setSpace(Space* newSpace) = 0;

			/// Sets whether the Solid is sleeping (i.e. set this to false to
			/// wake up a sleeping Solid).
			virtual void OPAL_CALL setSleeping(bool sleeping) = 0;

			/// Returns true if the Solid is sleeping.  If the Solid is static,
			/// this will always return true.
			virtual bool OPAL_CALL isSleeping() const = 0;

			/// Sets the Solid's sleepiness level.
			virtual void OPAL_CALL setSleepiness(real s);

			/// Returns the Solid's sleepiness level.
			virtual real OPAL_CALL getSleepiness() const;

			/// Sets the amount of linear damping on this Solid.
			virtual void OPAL_CALL setLinearDamping(real ld);

			/// Returns the amount of linear damping on this Solid.
			virtual real OPAL_CALL getLinearDamping() const;

			/// Sets the amount of angular damping on this Solid.
			virtual void OPAL_CALL setAngularDamping(real ad);

			/// Returns the amount of angular damping on this Solid.
			virtual real OPAL_CALL getAngularDamping() const;

			/// Set this Solid's user data pointer to some external data.  This
			/// can be used to let the Solid point to some user object (e.g. an
			/// object with a visual mesh).  The user data is totally user-managed
			/// (i.e. it is not destroyed when the Solid is destroyed).
			virtual void OPAL_CALL setUserData(void* data);

			/// Returns the user data pointer (NULL if it has not been set).
			virtual void* OPAL_CALL getUserData() const;

			/// Sets this Solid's transform matrix.
			virtual void OPAL_CALL setTransform(const Matrix44r& t);

			/// Returns a constant reference to this Solid's transform.
			virtual const Matrix44r& OPAL_CALL getTransform() const;

			/// Sets the position of this Solid in global coordinates.
			virtual void OPAL_CALL setPosition(real x, real y, real z);

			/// Sets the position of this Solid in global coordinates.
			virtual void OPAL_CALL setPosition(const Point3r & p);

			/// Returns the position of this Solid in global coordinates.
			virtual Point3r OPAL_CALL getPosition() const;

			/// Returns the euler angles of the Solid's orientation.
			virtual Vec3r OPAL_CALL getEulerXYZ() const;

			/// Returns a quaternion representing the Solid's orientation.
			virtual Quaternion OPAL_CALL getQuaternion() const;

			/// Sets a quaternion representing the Solid's orientation.
			virtual void OPAL_CALL setQuaternion(const Quaternion & q);

			/// Sets a quaternion representing the Solid's orientation.
			virtual void OPAL_CALL setQuaternion(real w, real x, real y, real z);

			/// Returns the axis-aligned bounding box for all of the Solid's
			/// shapes relative to the Solid.
			virtual void OPAL_CALL getLocalAABB(real aabb[6]) const;

			/// Returns the axis-aligned bounding box for all of the Solid's
			/// shapes in global coordinates.
			virtual void OPAL_CALL getGlobalAABB(real aabb[6]) const;

			/// Removes all shapes from this Solid.  Resets the Solid's
			/// axis-aligned bounding box.
			virtual void OPAL_CALL clearShapes() = 0;

			/// Adds a Shape to this Solid.  Updates the Solid's axis-aligned
			/// bounding box.
			virtual void OPAL_CALL addShape(ShapeData& data) = 0;

			/// Applies a force/torque to this Solid.  If the Solid is disabled,
			/// the Solid is static, or the magnitude of the force/torque is
			/// zero, this will do nothing.
			virtual void OPAL_CALL addForce(const Force& f);

			/// Removes all forces and torques currently affecting this Solid.
			virtual void OPAL_CALL zeroForces() = 0;

			/// Sets the Solid's linear velocity in local coordinates.
			virtual void OPAL_CALL setLocalLinearVel(const Vec3r& vel) = 0;

			/// Returns the Solid's linear velocity in local coordinates.
			virtual Vec3r OPAL_CALL getLocalLinearVel() const = 0;

			/// Given an offset point relative to the Solid's local origin,
			/// returns the linear velocity of the point in local coordinates.
			virtual Vec3r OPAL_CALL getLocalLinearVelAtLocalPos(
			    const Point3r& p) const = 0;

			/// Given an offset point in global coordinates, returns the 
			/// linear velocity of the point in local coordinates.
			virtual Vec3r OPAL_CALL getLocalLinearVelAtGlobalPos(
			    const Point3r& p) const = 0;

			/// Sets the Solid's angular velocity in local coordinates.
			virtual void OPAL_CALL setLocalAngularVel(const Vec3r& vel) = 0;

			/// Returns the Solid's angular velocity in local coordinates.
			virtual Vec3r OPAL_CALL getLocalAngularVel() const = 0;

			/// Sets the Solid's linear velocity in global coordinates.
			virtual void OPAL_CALL setGlobalLinearVel(const Vec3r& vel) = 0;

			/// Returns the Solid's linear velocity in global coordinates.
			virtual Vec3r OPAL_CALL getGlobalLinearVel() const = 0;

			/// Given an offset point relative to the Solid's local origin,
			/// returns the linear velocity of the point in global coordinates.
			virtual Vec3r OPAL_CALL getGlobalLinearVelAtLocalPos(
			    const Point3r& p) const = 0;

			/// Given an offset point in global coordinates, returns the 
			/// linear velocity of the point in global coordinates.
			virtual Vec3r OPAL_CALL getGlobalLinearVelAtGlobalPos(
			    const Point3r& p) const = 0;

			/// Sets the Solid's angular velocity in global coordinates.
			virtual void OPAL_CALL setGlobalAngularVel(const Vec3r& vel) = 0;

			/// Returns the Solid's angular velocity in global coordinates.
			virtual Vec3r OPAL_CALL getGlobalAngularVel() const = 0;

			/// Set a new mass, center, and intertia for the solid.
			//virtual void OPAL_CALL setMass(const Mass & newmass, 
			//	const Matrix44r & offset) = 0;
			virtual void OPAL_CALL setMass(const Mass & newmass) = 0;

			/// Translate the Solid's mass using the given offset vector
			/// specified in global coordinates.
			//virtual void OPAL_CALL translateMass(const Vec3r& offset) = 0;

			/// Sets the Solid's collision event handler.
			virtual void OPAL_CALL setCollisionEventHandler(
			    CollisionEventHandler* eventHandler);

			/// Returns the Solid's collision event handler.  If this returns
			/// NULL, the Solid is not using one.
			virtual CollisionEventHandler* OPAL_CALL
				getCollisionEventHandler() const;

			//// Quickly spinning solids should be set as fast rotating solids to
			//// improve simulation accuracy.
			//virtual void OPAL_CALL setFastRotation(bool fast);

			//virtual bool OPAL_CALL getFastRotation()const;

			//virtual void OPAL_CALL setFastRotationAxis(Vec3r axis);

			/// Returns the Solid's mass.  This will return 0 if the Solid
			/// is static.
			virtual real OPAL_CALL getMass() const = 0;

			/// Returns the Solid's inertia tensor as a 4x4 matrix.  This will
			/// be the identity matrix if the Solid is static.
			virtual Matrix44r OPAL_CALL getInertiaTensor() const = 0;

			/// Returns true if the solid has moved since last call of Solid::isMoving()
			virtual bool OPAL_CALL isMoving() const;

			/// Manual set of a moving state
			virtual void OPAL_CALL setMovingState(bool moving);

			/// Sets the Solid's movement event handler.
			virtual void OPAL_CALL setMovementEventHandler(
			    MovementEventHandler* eventHandler);

			/// Returns the Solid's movement event handler.  If this returns
			/// NULL, the Solid is not using one.
			virtual MovementEventHandler* OPAL_CALL getMovementEventHandler() const;

			/// Update the OPAL transform using the physics engine transform.
			virtual void internal_updateOPALTransform() = 0;

			/// Update the physics engine transform using the OPAL transform.
			virtual void internal_updateEngineTransform() = 0;

			/// Loops over the Solid's list of Forces and applies them.
			void internal_applyForces(real stepSize);

			/// Updates the SolidData sleeping value from the physics engine.
			void internal_updateSleeping();

			/// Internal function used to destroy this object.
			void internal_destroy();

		protected:
			virtual ~Solid();

			/// Physics engine-specific function for applying Forces to
			/// Solids.
			virtual void applyForce(const Force& f) = 0;

			/// Adds the given axis-aligned bounding box to the Solid's AABB.
			void addToLocalAABB(const real aabb[6]);

			/// Resets the Solid's axis-aligned bounding box.
			void resetAABB();

			/// An internal list of this Solid's pending Forces.
			std::vector<Force> mForceList;

			/// Stores data describing the Solid.
			SolidData mData;

			/// The axis-aligned bounding box of all shapes in local
			/// coordinates.  This array stores data in the following order:
			/// min x, max x, min y, max y, min z, max z.  This gets updated
			/// whenever a shape is added or removed.
			real mLocalAABB[6];

			/// Pointer to this Solid's collision event handler.
			CollisionEventHandler* mCollisionEventHandler;

			/// Pointer to user data.  This is totally user-managed (i.e. OPAL
			/// will never delete it).
			void* mUserData;

			/// Data used for
		private:
			mutable bool mIsMoving;
			MovementEventHandler* mMovementEventHandler;
	};
}

#endif

