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

#ifndef OPAL_OGRE_PHYSICAL_CAMERA_H
#define OPAL_OGRE_PHYSICAL_CAMERA_H

#ifndef SIMULATION_ENGINE_PHYSICS_ONLY

#include <opal/opal.h>

namespace Ogre
{
	class Camera;
	class SceneNode;
}

/// An extended camera class that uses an Ogre Camera internally.  
/// Can be setup as a "normal" camera or a physical camera (using an 
/// OPAL Solid), which is useful for first-person perspective 
/// applications.to be physical by 
/// using an OPAL Solid.
class PhysicalCamera
{
public:
	enum Type
	{
		/// A physically-simulated camera.  Uses two invisible 
		/// spheres to represent a physical character.  Grasped 
		/// objects are pulled close to the camera.
		PHYSICAL,

		/// A camera that can move anywhere and is unaffected by 
		/// gravity.  Translates and rotate in any direction.  
		/// Grasped objects remain in the same relative position 
		/// from the camera as when they were first grasped.
		NON_CLIPPING,

		/// A camera that can move anywhere and is unaffected by 
		/// gravity.  Movement orbits around a focal point, with 
		/// the view always oriented towards that point.  
		/// Grasped objects remain in the same relative position 
		/// from the camera as when they were first grasped.
		NON_CLIPPING_ORBIT
	};

	/// Note: eye height is only used for physical camera types.  
	/// The Ogre Camera should use a parent SceneNode other than 
	/// the root (to allow objects to be attached to the Camera, 
	/// and vice versa).
	PhysicalCamera(PhysicalCamera::Type type, opal::Simulator* sim, 
		Ogre::Camera* cam, opal::real physicalEyeHeight=2);

	~PhysicalCamera();

	/// Gives the camera a chance to update things regularly.
	void update(opal::real dt);

	/// Returns a pointer to the Ogre Camera.
	Ogre::Camera* getOgreCamera()const;

	/// Returns a pointer to the Ogre Camera's parent SceneNode.
	Ogre::SceneNode* getOgreSceneNode()const;

	/// Sets the position of the camera in global coordinates.  This 
	/// refers to the camera's eye position.
	void setPosition(const opal::Point3r& pos);

	/// Returns the position of the camera in global coordinates.  This 
	/// refers to the camera's eye position.
	opal::Point3r getPosition()const;

	/// Sets the camera's translation speed.
	void setTranslateSpeed(opal::real speed);

	/// Sets the camera's rotation speed.
	void setRotateSpeed(opal::real speed);

	/// Orients the camera look at the given point.
	void lookAt(const opal::Point3r& point);

	/// Sets the center point to use for orbit camera types.
	void setOrbitCenter(const opal::Point3r& point);

	/// Moves the camera according to the given direction vector 
	/// relative to its current transform.  This works differently 
	/// depending on the camera type.
	void moveRelative(const opal::Vec3r& dir, opal::real dt);

	/// For the orbit camera type, this orbits the camera around its 
	/// center point.
	//void orbitRelative(const opal::Vec3r& globalAxis, 
	//	opal::real angleDegrees);

	/// Attempts to grasp an object by firing a ray into the scene 
	/// directly in front of the camera and choosing the closest 
	/// intersected object, if any.
	void grasp();

	/// Releases any grasped objects.
	void release();

	/// Returns true if the camera is grasping an object.
	bool isGrasping();

	/// Adjust the camera's pitch by adding an angle to its yaw 
	/// relative to its current yaw.
	void yawRelative(opal::real amount);

	/// Adjust the camera's pitch by adding an angle to its pitch 
	/// relative to its current pitch.
	void pitchRelative(opal::real amount);

	/// Sets the desired position for grasped objects, represented as a 
	/// local offset from the camera's transform.  This only applies 
	/// to physical camera types.  (In the other types, the 
	/// offset is calculated automatically based on a ray cast 
	/// into the scene.)
	void setPhysicalGraspOffset(const opal::Vec3r& offset);

	/// Returns the desired position for grasped objects relative to 
	/// the camera.
	const opal::Vec3r& getPhysicalGraspOffset()const;

	/// Sets the spring and damping constants used for the grasping 
	/// motor.
	void setGraspingConstants(opal::SpringMotorMode mode, 
		opal::real linearKs, opal::real linearKd, 
		opal::real angularKs, opal::real angularKd);

	/// Returns the position of the grasping spring's attach point 
	/// on the grasped object in global coordinates.  This is only 
	/// meaningful when an object is being grasped.
	opal::Point3r getAttachGlobalPos()const;

	/// Returns the desired position for grasped objects in global 
	/// coordinates.
	opal::Point3r getGraspGlobalPos()const;

	/// Sets the maximum reach distance used for grasping.
	void setMaxReach(opal::real r);

	/// Returns the maximum reach distance used for grasping.
	opal::real getMaxReach()const;

private:
	// Old stuff...
	//opal::Point3r calcGraspPos();
	//opal::Matrix44r mGraspOffset;

	//void getOpalMatFromOgreCam(opal::Matrix44r& mat)
	//{
	//	// Make an OPAL matrix copy of the Ogre camera's transform.
	//	Ogre::Matrix4 ogreMat = mOgreCamera->getViewMatrix();
	//	mat.set(
	//		ogreMat[0][0], ogreMat[1][0], ogreMat[2][0], ogreMat[3][0], 
	//		ogreMat[0][1], ogreMat[1][1], ogreMat[2][1], ogreMat[3][1], 
	//		ogreMat[0][2], ogreMat[1][2], ogreMat[2][2], ogreMat[3][2], 
	//		ogreMat[0][3], ogreMat[1][3], ogreMat[2][3], ogreMat[3][3]);
	//}

	/// The camera type to use.
	PhysicalCamera::Type mType;

	/// Pointer to the OPAL Simulator.
	opal::Simulator* mSimulator;

	/// Pointer to the OPAL Solid.
	opal::Solid* mSolid;

	/// Pointer to the Ogre camera.
	Ogre::Camera* mOgreCamera;

	/// Determines how fast the camera can translate.
	opal::real mTranslateSpeed;

	/// Determines how fast the camera can rotate.
	opal::real mRotateSpeed;

	/// The OPAL ray casting Sensor used to find objects to grasp.
	opal::RaycastSensor* mGraspingSensor;

	/// The OPAL Motor used to hold grasped objects in a desired 
	/// position and orientation.
	opal::SpringMotor* mGraspingMotor;

	/// Pointer to the currently grasped object, if one exists.
	opal::Solid* mGraspedObject;

	/// The offset transform of the currently grasped object from the camera.
	opal::Matrix44r mGraspedObjectOffsetTransform;

	/// The desired offset position for grasped objects with physical 
	/// camera types in global coordinates.
	opal::Vec3r mPhysicalGraspOffset;

	/// When the camera attempts to grasp an object by casting a ray, 
	/// this is the length of the ray.  It determines how far away 
	/// objects can be and still be grasped.
	opal::real mMaxReach;

	/// Grasping motor spring and damping constants.
	opal::SpringMotorMode mGraspingMotorMode;
	opal::real mGraspingLinearKs;
	opal::real mGraspingLinearKd;
	opal::real mGraspingAngularKs;
	opal::real mGraspingAngularKd;

	/// The focal point to use for the orbit camera type.
	opal::Point3r mOrbitCenter;
};

#endif // #ifndef SIMULATION_ENGINE_PHYSICS_ONLY

#endif
