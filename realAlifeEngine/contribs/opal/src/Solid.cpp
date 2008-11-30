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

#include "Solid.h"

namespace opal
{
	Solid::Solid()
	{
		// "mData" is initialized in its own constructor.
		setCollisionEventHandler(0);
		setUserData(0);
		resetAABB();

		setMovingState(true);
		setMovementEventHandler(0);
	}

	Solid::~Solid()
	{
		mForceList.clear();
	}

	bool Solid::isMoving() const
	{
		if (mIsMoving)
		{
			mIsMoving = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	void Solid::setMovingState(bool moving)
	{
		mIsMoving = moving;
	}

	void Solid::setMovementEventHandler(MovementEventHandler* eventHandler)
	{
		mMovementEventHandler = eventHandler;
	}

	MovementEventHandler* Solid::getMovementEventHandler() const
	{
		return mMovementEventHandler;
	}

	void Solid::internal_destroy()
	{
		delete this;
	}

	const SolidData& Solid::getData()
	{
		// Update parameters that don't get updated automatically.
		mData.sleeping = isSleeping();

		return mData;
	}

	void Solid::setName(const std::string& name)
	{
		mData.name = name;
	}

	const std::string& Solid::getName() const
	{
		return mData.name;
	}

	bool Solid::isEnabled() const
	{
		return mData.enabled;
	}

	void Solid::setEnabled(bool e)
	{
		mData.enabled = e;
	}

	bool Solid::isStatic() const
	{
		return mData.isStatic;
	}

	bool Solid::isGravityEnabled()const
	{
		return mData.gravityEnabled;
	}

	void Solid::setSleepiness(real s)
	{
		assert(s >= 0.0 && s <= 1.0);
		mData.sleepiness = s;
	}

	real Solid::getSleepiness() const
	{
		return mData.sleepiness;
	}

	void Solid::setLinearDamping(real ld)
	{
		assert(ld >= 0.0);
		mData.linearDamping = ld;
	}

	real Solid::getLinearDamping() const
	{
		return mData.linearDamping;
	}

	void Solid::setAngularDamping(real ad)
	{
		assert(ad >= 0.0);
		mData.angularDamping = ad;
	}

	real Solid::getAngularDamping() const
	{
		return mData.angularDamping;
	}

	void Solid::setUserData(void* data)
	{
		mUserData = data;
	}

	void* Solid::getUserData() const
	{
		return mUserData;
	}

	void Solid::setTransform(const Matrix44r& t)
	{
		mData.transform = t;
		internal_updateEngineTransform();
	}

	const Matrix44r& Solid::getTransform() const
	{
		return mData.transform;
	}

	void Solid::setPosition(real x, real y, real z)
	{
		mIsMoving = true;
		mData.transform.setPosition(x, y, z);
		internal_updateEngineTransform();
	}

	void Solid::setPosition(const Point3r & p)
	{
		setPosition(p[0], p[1], p[2]);
	}

	Point3r Solid::getPosition() const
	{
		return mData.transform.getPosition();
	}

	Vec3r Solid::getEulerXYZ() const
	{
		return mData.transform.getEulerXYZ();
	}

	Quaternion Solid::getQuaternion() const
	{
		return mData.transform.getQuaternion();
	}

	void Solid::setQuaternion(const Quaternion & q)
	{
		setQuaternion(q[0], q[1], q[2], q[3]);
	}

	void Solid::setQuaternion(real w, real x, real y, real z)
	{
		mIsMoving = true;
		mData.transform.setQuaternion(w, x, y, z);
		internal_updateEngineTransform();
	}

	void Solid::getLocalAABB(real aabb[6]) const
	{
		for (unsigned int i = 0; i < 6; ++i)
		{
			aabb[i] = mLocalAABB[i];
		}
	}

	void Solid::getGlobalAABB(real aabb[6]) const
	{
		Point3r minExtents(mLocalAABB[0], mLocalAABB[2], mLocalAABB[4]);
		Point3r maxExtents(mLocalAABB[1], mLocalAABB[3], mLocalAABB[5]);

		// Transform the AABB extents to global coordinates.
		minExtents = mData.transform * minExtents;
		maxExtents = mData.transform * maxExtents;

		aabb[0] = minExtents[0];
		aabb[1] = maxExtents[0];
		aabb[2] = minExtents[1];
		aabb[3] = maxExtents[1];
		aabb[4] = minExtents[2];
		aabb[5] = maxExtents[2];
	}

	//void Solid::addPlane(const Point3r& point, const Vec3r& normal, const Material& m)
	//{
	//	Point3r origin(0, 0, 0);
	//	Vec3r n = normal;
	//	n.normalize();
	//	Vec3r v = point - origin;
	//
	//	real angle = 0;
	//	if (0 != v.length())
	//	{
	//		v.normalize();
	//		angle = acos(dot(n,v)); // note: this only works when the two vectors are normalized
	//	}
	//
	//	real length = distance(point, origin);
	//	real D = length * cos(angle);
	//	real abcd[4] = {n[0], n[1], n[2], D};
	//	addPlane(abcd, m);
	//}

	void Solid::addForce(const Force& f)
	{
		if (mData.enabled && !mData.isStatic &&
		        !areEqual(f.vec.lengthSquared(), 0))
		{
			mForceList.push_back(f);
		}
	}

	void Solid::internal_applyForces(real stepSize)
	{
		if (mData.isStatic)
		{
			return ;
		}

		// If there are Forces to apply and the Solid is asleep, wake it up.
		if (!mForceList.empty() && isSleeping())
		{
			setSleeping(false);
		}

		real invStepSize = 1 / stepSize;

		for (unsigned int i = 0; i < mForceList.size();)
		{
			if (true == mForceList[i].singleStep)
			{
				mForceList[i].duration = stepSize;
			}
			else if (mForceList[i].duration < stepSize)
			{
				// Scale the size of the force/torque.
				mForceList[i].vec *= (mForceList[i].duration * invStepSize);
			}

			// Apply the actual force/torque.
			applyForce(mForceList[i]);

			// The following is ok for all cases (even when duration is
			// < mStepSize).
			mForceList[i].duration -= stepSize;

			if (mForceList[i].duration <= 0)
			{
				// Delete this force.
				mForceList[i] = mForceList.back();
				mForceList.pop_back();
			}
			else
			{
				++i;
			}
		}
	}

	void Solid::setCollisionEventHandler(CollisionEventHandler* eventHandler)
	{
		mCollisionEventHandler = eventHandler;
	}

	CollisionEventHandler* Solid::getCollisionEventHandler() const
	{
		return mCollisionEventHandler;
	}

	void Solid::addToLocalAABB(const real aabb[6])
	{
		// Loop over the 3 dimensions of the AABB's extents.
		for (unsigned int i = 0; i < 3; ++i)
		{
			if (aabb[i * 2] < mLocalAABB[i * 2])
			{
				mLocalAABB[i * 2] = aabb[i * 2];
			}

			if (aabb[i * 2 + 1] > mLocalAABB[i * 2 + 1])
			{
				mLocalAABB[i * 2 + 1] = aabb[i * 2 + 1];
			}
		}
	}

	void Solid::resetAABB()
	{
		for (unsigned int i = 0; i < 6; ++i)
		{
			mLocalAABB[i] = 0;
		}
	}

	//void Solid::internal_updateSleeping()
	//{
	//	mData.sleeping = isSleeping();
	//}

	//// TODO: Quickly spinning solids should be set as fast rotating solids to
	//// improve simulation accuracy.
	//void Solid::setFastRotation(bool fast)
	//{
	//}

	//bool Solid::getFastRotation()const
	//{
	//	return false;
	//}

	//void Solid::setFastRotationAxis(Vec3r axis)
	//{
	//}
}
