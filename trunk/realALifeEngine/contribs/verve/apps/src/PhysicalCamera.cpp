/************************************************************************
* Verve                                                                 *
* http://verve-agents.sourceforge.net                                   *
* Copyright (C) 2004-2008                                               *
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

#ifndef SIMULATION_ENGINE_PHYSICS_ONLY

#include "PhysicalCamera.h"

#include <ogre/Ogre.h>

PhysicalCamera::PhysicalCamera(PhysicalCamera::Type type, 
	opal::Simulator* sim, Ogre::Camera* cam, 
	opal::real physicalEyeHeight)
{
	mType = type;
	mSimulator = sim;
	mSolid = NULL;
	mOgreCamera = cam;
	mTranslateSpeed = 5;
	mRotateSpeed = 0.1;
	mGraspingMotor = sim->createSpringMotor();
	mGraspingSensor = sim->createRaycastSensor();
	mGraspedObject = NULL;
	mPhysicalGraspOffset.set(0, 0, -5);
	mMaxReach = 500;
	mGraspingMotorMode = opal::LINEAR_MODE;
	mGraspingLinearKs = 25;
	mGraspingLinearKd = 2.5;
	mGraspingAngularKs = 5;
	mGraspingAngularKd = 0.3;

	// Setup OPAL Solid, if necessary.
	if (PHYSICAL == type)
	{
		mSolid = sim->createSolid();
		mSolid->setPosition(0, physicalEyeHeight, 0);

		// Keep the camera Solid from falling asleep.
		mSolid->setSleepiness(0);

		// Make the spheres' radii 1/4 * eye height.
		opal::real radius = physicalEyeHeight * (opal::real)0.25;

		// Add a sphere at the eye/Solid position.
		opal::SphereShapeData sphereData;
		sphereData.radius = radius;
		mSolid->addShape(sphereData);

		// Add another sphere down below.
		sphereData.offset.translate(0, -3 * radius, 0);
		mSolid->addShape(sphereData);

		// Attach the ray Sensor to the Solid.
		opal::RaycastSensorData raySensorData;
		raySensorData.solid = mSolid;
		mGraspingSensor->init(raySensorData);
	}
}

PhysicalCamera::~PhysicalCamera()
{
	if (PHYSICAL == mType)
	{
		assert(mSimulator && mGraspingMotor);
		mSimulator->destroySolid(mSolid);
	}

	mSimulator->destroyMotor(mGraspingMotor);
	mSimulator->destroySensor(mGraspingSensor);
}

void PhysicalCamera::update(opal::real dt)
{
	if (PHYSICAL == mType)
	{
		// Keep solid upright by resetting its orientation.  Thus the 
		// camera Solid's orientation is always the same.
		opal::Point3r p = mSolid->getPosition();
		opal::Matrix44r m;
		m.translate(p[0], p[1], p[2]);
		mSolid->setTransform(m);

		// Set the Ogre camera node's position.  It must be at the same 
		// position as the OPAL Solid.
		mOgreCamera->getParentSceneNode()->setPosition((Ogre::Real)p[0], 
			(Ogre::Real)p[1], (Ogre::Real)p[2]);
	}

	// Handle the grasped object if one exists.
	if (mGraspedObject)
	{
		// Keep the object awake.
		mGraspedObject->setSleeping(false);

		// Update the desired orientation.
		Ogre::Matrix4 ogreMat;
		mOgreCamera->getParentSceneNode()->getWorldTransforms(&ogreMat);
		opal::Matrix44r camTransform(
			ogreMat[0][0], ogreMat[1][0], ogreMat[2][0], ogreMat[3][0], 
			ogreMat[0][1], ogreMat[1][1], ogreMat[2][1], ogreMat[3][1], 
			ogreMat[0][2], ogreMat[1][2], ogreMat[2][2], ogreMat[3][2], 
			ogreMat[0][3], ogreMat[1][3], ogreMat[2][3], ogreMat[3][3]);
		opal::Matrix44r desiredObjTransform = mGraspedObjectOffsetTransform * 
			camTransform;
		desiredObjTransform.invert();

		// Set the desired transform for orientation only (the position 
		// part should be overwritten below).
		mGraspingMotor->setDesiredTransform(desiredObjTransform);

		// Set the Motor's global desired position for the Solid 
		// (at its attach position).
		mGraspingMotor->setDesiredPosition(getGraspGlobalPos());

		// Drop object if it gets too far away.
		if (opal::distance(getPosition(), mGraspedObject->getPosition())
			> mMaxReach)
		{
			release();
		}
	}
}

Ogre::Camera* PhysicalCamera::getOgreCamera()const
{
	return mOgreCamera;
}

Ogre::SceneNode* PhysicalCamera::getOgreSceneNode()const
{
	return mOgreCamera->getParentSceneNode();
}

void PhysicalCamera::setPosition(const opal::Point3r& pos)
{
	if (PHYSICAL == mType)
	{
		assert(mSolid);
		mSolid->setPosition(pos);
	}
	else
	{
		mOgreCamera->getParentSceneNode()->setPosition(pos[0], pos[1], 
			pos[2]);
	}
}

opal::Point3r PhysicalCamera::getPosition()const
{
	Ogre::Vector3 ogreCamPos = mOgreCamera->getParentSceneNode()->
		getPosition();
	return opal::Point3r(ogreCamPos[0], ogreCamPos[1], ogreCamPos[2]);
}

void PhysicalCamera::setTranslateSpeed(opal::real speed)
{
	mTranslateSpeed = speed;
}

void PhysicalCamera::setRotateSpeed(opal::real speed)
{
	mRotateSpeed = speed;
}

void PhysicalCamera::lookAt(const opal::Point3r& point)
{
	mOgreCamera->getParentSceneNode()->lookAt(Ogre::Vector3(point.x, 
		point.y, point.z), Ogre::Node::TS_WORLD);
}

void PhysicalCamera::setOrbitCenter(const opal::Point3r& point)
{
	mOrbitCenter = point;
}

void PhysicalCamera::moveRelative(const opal::Vec3r& dir, opal::real dt)
{
	// Construct the actual velocity vector.
	opal::Vec3r velocity = dir;
	if (!opal::areEqual(velocity.lengthSquared(), 0))
	{
		velocity.normalize();
	}
	velocity *= mTranslateSpeed;

	switch(mType)
	{
		case PHYSICAL:
		{
			assert(mSolid);

			// TODO: handle things differently if we're in midair.

			Ogre::Matrix4 camTransform;
			mOgreCamera->getParentSceneNode()->getWorldTransforms(
				&camTransform);
			Ogre::Vector3 localDir(velocity[0], velocity[1], velocity[2]);

			// Convert the local direction vector to a global direction 
			// vector.  Subtract out the camera's position.
			Ogre::Vector3 globalDir = camTransform * localDir;
			globalDir -= camTransform.getTrans();
			opal::Vec3r opalVec(globalDir[0], globalDir[1], globalDir[2]);

			// Keep from flying.
			if (opalVec[1] > 0)
			{
				opalVec[1] = 0;
			}

			// Don't use the dt in this case; let Opal take care of the 
			// velocity.
			mSolid->setGlobalLinearVel(opalVec);
			break;
		}
		case NON_CLIPPING:
		{
			Ogre::Vector3 posChange(velocity[0] * dt, velocity[1] * dt, 
				velocity[2] * dt);
			mOgreCamera->getParentSceneNode()->translate(posChange, 
				Ogre::Node::TS_LOCAL);
			break;
		}
		case NON_CLIPPING_ORBIT:
		{
			Ogre::Vector3 posChange(velocity[0] * dt, velocity[1] * dt, 
				velocity[2] * dt);
			mOgreCamera->getParentSceneNode()->translate(posChange, 
				Ogre::Node::TS_LOCAL);
			lookAt(mOrbitCenter);
			break;
		}
		default:
			assert(false);
	}
}

//void PhysicalCamera::orbitRelative(const opal::Vec3r& globalAxis, 
//	opal::real angleDegrees)
//{
//	if (NON_CLIPPING_ORBIT == mType)
//	{
//
//	}
//}

void PhysicalCamera::grasp()
{
	if (!mGraspedObject)
	{
		// Fire a ray into the scene to find an object to grasp.  If 
		// an object is found, attach it to the grasping Motor and, 
		// if using long range grasping mode, store the intersection 
		// position relative to the camera as the grasp offset.

		// First update the ray casting Sensor's ray.
		Ogre::Vector3 camForward = mOgreCamera->getDerivedDirection();
		if (0 != camForward.squaredLength())
		{
			camForward.normalise();
		}
		opal::Vec3r rayDir(camForward[0], camForward[1], 
			camForward[2]);
		if (PHYSICAL == mType)
		{
			// The ray's origin will be updated automatically since 
			// it is attached to the camera's Solid.  Its direction 
			// should be set manually here because we constantly 
			// reset the camera's orientation.
			opal::Point3r dummyPoint;
			opal::Rayr r(dummyPoint, rayDir);
			mGraspingSensor->setRay(r);
		}
		else
		{
			// The ray should start at the camera's position and fire 
			// straight forward into the scene.
			Ogre::Vector3 ogreCamPos = mOgreCamera->getDerivedPosition();
			opal::Point3r opalCamPos(ogreCamPos[0], ogreCamPos[1], 
				ogreCamPos[2]);
			opal::Rayr r(opalCamPos, rayDir);
			mGraspingSensor->setRay(r);
		}

		// Fire the ray.
		opal::RaycastResult result = 
			mGraspingSensor->fireRay(mMaxReach);

		if (result.solid) // && !result.solid->isStatic())
		{
			// Store the grasped object.
			mGraspedObject = result.solid;

			// Initialize the grasping Motor with the new data.
			opal::SpringMotorData data;
			data.solid = result.solid;
			data.mode = mGraspingMotorMode;
			data.linearKs = mGraspingLinearKs;
			data.linearKd = mGraspingLinearKd;
			data.angularKs = mGraspingAngularKs;
			data.angularKd = mGraspingAngularKd;

			opal::Matrix44r solidTransform = result.solid->getTransform();
			//data.desiredUp = solidTransform.getUp();
			//data.desiredForward = solidTransform.getForward();
			//data.desiredRight = solidTransform.getRight();
			// Desired position will be updated in the "update" 
			// function.
			mGraspingMotor->init(data);

			if (PHYSICAL == mType)
			{
				//// Grab it where the ray intersected it.
				//mGraspingMotor->setGlobalAttachPoint(result.intersection);

				// Just grab it in the middle.
				mGraspingMotor->setGlobalAttachPoint(result.solid->
					getPosition());
			}
			else
			{
				mGraspingMotor->setGlobalAttachPoint(result.intersection);

				// Set the offset (from the camera) to be the same distance 
				// as when it was grasped.
				mPhysicalGraspOffset.set(0, 0, -result.distance);
			}

			// Compute the grasped object's offset transform from the camera 
			// which is only used for desired orientation.
			Ogre::Matrix4 ogreMat;
			mOgreCamera->getParentSceneNode()->getWorldTransforms(&ogreMat);
			opal::Matrix44r camTransform(
				ogreMat[0][0], ogreMat[1][0], ogreMat[2][0], ogreMat[3][0], 
				ogreMat[0][1], ogreMat[1][1], ogreMat[2][1], ogreMat[3][1], 
				ogreMat[0][2], ogreMat[1][2], ogreMat[2][2], ogreMat[3][2], 
				ogreMat[0][3], ogreMat[1][3], ogreMat[2][3], ogreMat[3][3]);
			camTransform.invert();
			opal::Matrix44r objTransInv = mGraspedObject->getTransform();
			objTransInv.invert();
			mGraspedObjectOffsetTransform = objTransInv * camTransform;

			// Set the desired transform for orientation only (the position 
			// part should be overwritten below).
			opal::Matrix44r desiredObjTransform = mGraspedObjectOffsetTransform * 
				camTransform;
			desiredObjTransform.invert();
			mGraspingMotor->setDesiredTransform(desiredObjTransform);

			// Update the the desired position.
			mGraspingMotor->setDesiredPosition(getGraspGlobalPos());

			// Apply extra angular damping.
			result.solid->setAngularDamping(3);
		}
	}
}

void PhysicalCamera::release()
{
	if (mGraspedObject)
	{
		// Make sure the object is awake before releasing it so it 
		// doesn't float in midair.
		mGraspedObject->setAngularDamping(
			opal::defaults::solid::angularDamping);
		mGraspedObject->setSleeping(false);
		mGraspedObject = NULL;
		mGraspingMotor->setEnabled(false);
	}
}

bool PhysicalCamera::isGrasping()
{
	if (mGraspedObject)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void PhysicalCamera::yawRelative(opal::real amount)
{
	// Update the Ogre camera.
	mOgreCamera->getParentSceneNode()->rotate(Ogre::Vector3(0, 1, 0), 
		Ogre::Radian(Ogre::Degree(amount * mRotateSpeed)), 
		Ogre::Node::TS_WORLD);
}

void PhysicalCamera::pitchRelative(opal::real amount)
{
	// Update the Ogre camera.
	mOgreCamera->getParentSceneNode()->rotate(Ogre::Vector3(1, 0, 0), 
		Ogre::Radian(Ogre::Degree(amount * mRotateSpeed)), 
		Ogre::Node::TS_LOCAL);
}

void PhysicalCamera::setPhysicalGraspOffset(const opal::Vec3r& offset)
{
	if (PHYSICAL == mType)
	{
		mPhysicalGraspOffset = offset;
	}
}

const opal::Vec3r& PhysicalCamera::getPhysicalGraspOffset()const
{
	return mPhysicalGraspOffset;
}

void PhysicalCamera::setGraspingConstants(opal::SpringMotorMode mode, 
	opal::real linearKs, opal::real linearKd, opal::real angularKs, 
	opal::real angularKd)
{
	mGraspingMotorMode = mode;
	mGraspingLinearKs = linearKs;
	mGraspingLinearKd = linearKd;
	mGraspingAngularKs = angularKs;
	mGraspingAngularKd = angularKd;
}


opal::Point3r PhysicalCamera::getAttachGlobalPos()const
{
	return mGraspingMotor->getGlobalAttachPoint();
}

opal::Point3r PhysicalCamera::getGraspGlobalPos()const
{
	//Ogre::Matrix4 ogreMat = mOgreCamera->getViewMatrix().inverse();
	Ogre::Matrix4 camTransform;
	mOgreCamera->getParentSceneNode()->getWorldTransforms(&camTransform);

	Ogre::Vector3 graspOffset(mPhysicalGraspOffset[0], 
		mPhysicalGraspOffset[1], mPhysicalGraspOffset[2]);

	// Transform the offset vector (actually representing a point) 
	// to global space.
	graspOffset = camTransform * graspOffset;

	return opal::Point3r(graspOffset[0], graspOffset[1], 
		graspOffset[2]);
}

//opal::Point3r getGlobalPosFromLocalOffset(opal::Vec3r localOffset)
//{
//	Ogre::Matrix4 ogreMat = mOgreCamera->getViewMatrix().inverse();
//	//Ogre::Vector3 camPos = mOgreCamera->getPosition();
//	Ogre::Vector3 offset(localOffset[0], localOffset[1], 
//		localOffset[2]);

//	// Transform the offset vector to global space.
//	offset = ogreMat * offset;

//	// Combine the camera position with the local offset vector.
//	return opal::Point3r(offset[0], offset[1], offset[2]);
//}

void PhysicalCamera::setMaxReach(opal::real r)
{
	mMaxReach = r;
}

opal::real PhysicalCamera::getMaxReach()const
{
	return mMaxReach;
}

#endif // #ifndef SIMULATION_ENGINE_PHYSICS_ONLY
