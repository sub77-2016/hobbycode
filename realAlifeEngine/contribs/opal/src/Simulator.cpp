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

// class headers
#include "Simulator.h"

// project headers
#include "BlueprintInstance.h"
#include "BoxShapeData.h"
#include "SphereShapeData.h"
#include "CapsuleShapeData.h"
#include "CylinderShapeData.h"
#include "PlaneShapeData.h"
#include "ThrusterMotor.h"
#include "GearedMotor.h"
#include "ServoMotor.h"
#include "AttractorMotor.h"
#include "SpringMotor.h"
#include "AccelerationSensor.h"
#include "InclineSensor.h"
#include "RaycastSensor.h"
#include "VolumeSensor.h"
#include "PostStepEventHandler.h"
#include "VelocityMotor.h"

namespace opal
{
	Simulator::Simulator()
	{
		mTimeBuffer = 0;
		setStepSize(defaults::stepSize);
		//setMaxLinearVel(defaults::maxLinearVel);
		//setMaxAngularVel(defaults::maxAngularVel);
		setMaxCorrectingVel(defaults::maxCorrectingVel);
		setMaxContacts(defaults::maxContacts);
		setUserData(NULL);
		mIsSolidDestructionSafe = true;
		mIsJointDestructionSafe = true;
		mStaticSleepingContactsEnabled =
		    defaults::staticSleepingContactsEnabled;

		int i = 0;
		for (i = 0; i < 32; ++i)
		{
			mContactGroupFlags[i] = defaults::contactGroupFlags;
		}

		mRootSpace = NULL;
	}

	void Simulator::initData(SimulatorData data)
	{
		mData = data;
	}

	Simulator::~Simulator()
	{
		destroyAllSolids();
		destroyAllJoints();
		destroyAllMotors();
		destroyAllSensors();

		while (!mSpaceList.empty())
		{
			mSpaceList.back()->internal_destroy();
			mSpaceList.pop_back();
		}
		mRootSpace->internal_destroy();

		mSolidGarbageList.clear();
		mJointGarbageList.clear();
	}

	bool Simulator::simulate(real dt)
	{
		mTimeBuffer += dt;

		// Use this to return true if at least one step occurred.
		bool stepOccurred = false;
		if (mTimeBuffer >= mStepSize)
		{
			stepOccurred = true;
		}

		//if (mAllowPartialFrames && mTimeBuffer >= (real)defaults::maxStepsPerFrame * mStepSize)
		//{
		//  mTimeBuffer = defaults::maxStepsPerFrame * mStepSize;
		//}

		while (mTimeBuffer >= mStepSize)
		{
			// Update Sensors.
			std::vector<Sensor*>::iterator sensorIter;
			for (sensorIter = mSensorList.begin();
			        sensorIter != mSensorList.end(); ++sensorIter)
			{
				(*sensorIter)->internal_update();
			}

			// Update Motors.
			std::vector<Motor*>::iterator motorIter;
			for (motorIter = mMotorList.begin();
			        motorIter != mMotorList.end(); ++motorIter)
			{
				(*motorIter)->internal_update();
			}

			// Apply forces/torques to Solids.
			std::vector<Solid*>::iterator solidIter;
			for (solidIter = mSolidList.begin();
			        solidIter != mSolidList.end(); ++solidIter)
			{
				(*solidIter)->internal_applyForces(mStepSize);
			}

			// Update Joints.
			mIsJointDestructionSafe = false;
			std::vector<Joint*>::iterator jointIter;
			for (jointIter = mJointList.begin();
			        jointIter != mJointList.end(); ++jointIter)
			{
				// If a Joint gets broken here, it will automatically
				// send a Joint break event to the Joint's
				// JointBreakEventHandler (assuming one exists).
				(*jointIter)->internal_update();
			}
			mIsJointDestructionSafe = true;

			// Now do physics engine-specific stuff; collision events will
			// be sent from here.
			stepPhysics();

			// Loop over Solids again to handle a few more things...
			mIsSolidDestructionSafe = false;
			for (solidIter = mSolidList.begin();
			        solidIter != mSolidList.end(); ++solidIter)
			{
				Solid* solid = *solidIter;

				// Get each dynamic, awake Solid's new transform from the
				// physics engine.
				if (!solid->isStatic() && !solid->isSleeping())
				{
					solid->internal_updateOPALTransform();
					solid->setMovingState(true);
				}

				// Update the sleeping value from the physics engine.  This
				// needs to be updated to keep the Solid's SolidData valid.
				//solid->internal_updateSleeping();
				// Update... Now this gets updated in the Solid::getData
				// function.

				// Update the Solid's CollisionEventHandler if applicable.
				if (solid->getCollisionEventHandler())
				{
					solid->getCollisionEventHandler()->
					internal_handlePendingCollisionEvents();
				}

				if (solid->getMovementEventHandler())
				{
					if (solid->isMoving())
					{
						MovementEvent move(solid);
						solid->getMovementEventHandler()->handleMovementEvent(move);
					}
				}
			}
			mIsSolidDestructionSafe = true;

			// Fire an event to the PostStepEventHandler, if one exists.
			if (getNumPostStepEventHandlers() > 0)
			{
				for (size_t i = 0; i < mPostStepEventHandlers.size(); ++i)
					mPostStepEventHandlers[i]->handlePostStepEvent();
			}

			if (getNumGlobalCollisionEventHandlers() > 0)
			{
				for (size_t i = 0; i < mCollisionEventHandlers.size(); ++i)
					mCollisionEventHandlers[i]->internal_handlePendingCollisionEvents();
			}

			// Destroy garbage now that it's safe.
			destroyGarbage();

			//Decrement the time buffer
			mTimeBuffer -= mStepSize;
		}

		return stepOccurred;
	}

	void Simulator::internal_recordCollision(const CollisionEvent & event)
	{
		if (mCollisionEventHandlers.size() > 0)
		{
			for (size_t i = 0; i < mCollisionEventHandlers.size(); ++i)
			{
				mCollisionEventHandlers[i]->internal_pushCollisionEvent(event);
			}
		}
	}

	void Simulator::setStepSize(real stepSize)
	{
		assert(stepSize > 0);
		mStepSize = stepSize;
	}

	real Simulator::getStepSize()
	{
		return mStepSize;
	}

	void Simulator::addPostStepEventHandler(
	    PostStepEventHandler* eventHandler)
	{
		mPostStepEventHandlers.push_back(eventHandler);
	}

	PostStepEventHandler* Simulator::getPostStepEventHandler(unsigned int id) const
	{
		return mPostStepEventHandlers[id];
	}

	void Simulator::removePostStepEventHandler(
	    PostStepEventHandler * eventHandler)
	{
		for (size_t i = 0; i < mPostStepEventHandlers.size(); ++i)
		{
			if (mPostStepEventHandlers[i] == eventHandler)
			{
				mPostStepEventHandlers.erase(mPostStepEventHandlers.begin() + i);
			}
		}
	}

	size_t Simulator::getNumPostStepEventHandlers() const
	{
		return mPostStepEventHandlers.size();
	}

	void Simulator::addGlobalCollisionEventHandler(
	    CollisionEventHandler* eventHandler)
	{
		mCollisionEventHandlers.push_back(eventHandler);
	}

	CollisionEventHandler* Simulator::getGlobalCollisionEventHandler(unsigned int id) const
	{
		return mCollisionEventHandlers[id];
	}

	void Simulator::removeGlobalCollisionEventHandler(
	    CollisionEventHandler * eventHandler)
	{
		for (size_t i = 0; i < mCollisionEventHandlers.size(); ++i)
		{
			if (mCollisionEventHandlers[i] == eventHandler)
			{
				mCollisionEventHandlers.erase(mCollisionEventHandlers.begin() + i);
				break;
			}
		}
	}

	size_t Simulator::getNumGlobalCollisionEventHandlers() const
	{
		return mCollisionEventHandlers.size();
	}

	void Simulator::instantiateBlueprint(
	    BlueprintInstance& instance, const Blueprint& bp,
	    const Matrix44r& offset, real scale)
	{
		assert(scale > 0);

		if (!bp.isFinalized())
		{
			OPAL_LOGGER("warning")
			<< "opal::Simulator::instantiateBlueprint: Cannot \
			instantiate a Blueprint before it is finalized.  Ignoring \
			the Blueprint." << std::endl;
			return ;
		}

		bool useOffset = false;
		if (Matrix44r() != offset)
		{
			useOffset = true;
		}

		bool useScale = false;
		//Matrix44r scaleMat;
		if (1 != scale)
		{
			useScale = true;
			//scaleMat.makeScale(scale);
		}

		// For each object created here, only the ones with names get added
		// to the BlueprintInstance.

		// Some of the objects depend on the presence of others, so
		// they must be created in a specific order: Solids depend on
		// nothing, Joints depend on Solids, Motors depend on Joints
		// and Solids, and Sensors depend on Joints and Solids.

		// For the objects with references to their dependencies (i.e.
		// Joints, Motors, and Sensors), use the stored indices to
		// find pointers in the appropriate array.

		// Create these temporary arrays of pointers.  Other objects
		// will use these to find pointers to their dependencies.
		std::vector<Solid*> solidList;
		std::vector<Joint*> jointList;
		unsigned int i;

		// Create all Solids in the Blueprint.
		for (i = 0; i < bp.getNumSolids(); ++i)
		{
			Solid* s = createSolid();

			// Make a local copy of the Solid's data, including all of its
			// Shapes.
			SolidData sd = *bp.getSolidData(i);

			if (useScale)
			{
				// Scale the Solid's transform.
				sd.transform[12] *= scale;
				sd.transform[13] *= scale;
				sd.transform[14] *= scale;

				// Scale the Solid's Shape's offsets and dimensions.
				unsigned int i = 0;
				for (i = 0; i < sd.getNumShapes(); ++i)
				{
					ShapeData* shapeData = sd.getShapeData(i);
					shapeData->offset[12] *= scale;
					shapeData->offset[13] *= scale;
					shapeData->offset[14] *= scale;

					switch (shapeData->getType())
					{
						case BOX_SHAPE:
							{
								BoxShapeData * boxData =
								    (BoxShapeData*) shapeData;
								boxData->dimensions =
								    scale * boxData->dimensions;
								break;
							}
						case SPHERE_SHAPE:
							{
								SphereShapeData* sphereData =
								    (SphereShapeData*) shapeData;
								sphereData->radius =
								    scale * sphereData->radius;
								break;
							}
						case CAPSULE_SHAPE:
							{
								CapsuleShapeData* capsuleData =
								    (CapsuleShapeData*) shapeData;
								capsuleData->radius =
								    scale * capsuleData->radius;
								capsuleData->length =
								    scale * capsuleData->length;
								break;
							}
						case CYLINDER_SHAPE:
							{
								CylinderShapeData *cylinderData =
								    (CylinderShapeData*) shapeData;
								cylinderData->radius =
								    scale * cylinderData->radius;
								cylinderData->length =
								    scale * cylinderData->length;
								break;
							}
						case PLANE_SHAPE:
							{
								PlaneShapeData* planeData =
								    (PlaneShapeData*) shapeData;
								planeData->abcd[3] =
								    scale * planeData->abcd[3];
								break;
							}
							//case RAY_SHAPE:
							//{
							//  RayShapeData* rayData =
							//      (RayShapeData*)shapeData;
							//  Point3r origin = rayData->ray.getOrigin();
							//  origin = scale * origin;
							//  rayData->ray.setOrigin(origin);
							//  break;
							//}
						case MESH_SHAPE:
							{
								// Not implemented.  Scaling the Mesh data
								// should be handled elsewhere.
								break;
							}
						default:
							assert(false);
					}
				}
			}

			if (useOffset)
			{
				// Offset the Solid's transform.
				sd.transform = offset * sd.transform;
			}

			s->init(sd);
			solidList.push_back(s);

			// Add the Solid to the BlueprintInstance.
			instance.addSolid(s);
		}

		// Create all Joints in the Blueprint.
		for (i = 0; i < bp.getNumJoints(); ++i)
		{
			Joint* j = createJoint();

			// Create a local copy of the Joint's data.
			JointData jd = *bp.getJointData(i);

			// Setup the Solid reference pointers.  If one of the Solid 
			// indices is -1, the Joint will attach that side to the 
			// static environment automatically.
			if (-1 == jd.internal_solid0Index)
			{
				jd.solid0 = NULL;
			}
			else
			{
				jd.solid0 = solidList.at(jd.internal_solid0Index);
			}

			if (-1 == jd.internal_solid1Index)
			{
				jd.solid1 = NULL;
			}
			else
			{
				jd.solid1 = solidList.at(jd.internal_solid1Index);
			}

			if (useScale)
			{
				// Scale the Joint's anchor.
				jd.anchor = scale * jd.anchor;
			}

			if (useOffset)
			{
				// Offset the Joint's anchor and axes.
				jd.anchor = offset * jd.anchor;
				jd.axis[0].direction = offset * jd.axis[0].direction;
				jd.axis[1].direction = offset * jd.axis[1].direction;
				jd.axis[2].direction = offset * jd.axis[2].direction;
			}

			j->init(jd);
			jointList.push_back(j);

			// Add the Joint to the BlueprintInstance.
			instance.addJoint(j);
		}

		// Create all Motors in the Blueprint.
		for (i = 0; i < bp.getNumMotors(); ++i)
		{
			Motor* m = NULL;
			MotorData* motorData = bp.getMotorData(i);

			// Allocate and setup the right type of Motor.
			switch (bp.getMotorData(i)->getType())
			{
				case ATTRACTOR_MOTOR:
					{
						// Make a local copy of the Motor's data.
						AttractorMotorData data = *(AttractorMotorData*) motorData;

						// Setup the Solid reference pointers.
						data.solid0 = solidList.at(data.internal_solid0Index);
						data.solid1 = solidList.at(data.internal_solid1Index);

						m = createAttractorMotor();
						((AttractorMotor*) m)->init(data);
						break;
					}
				case GEARED_MOTOR:
					{
						// Make a local copy of the Motor's data.
						GearedMotorData data = *(GearedMotorData*) motorData;

						// Setup the Joint reference pointer.
						data.joint = jointList.at(data.internal_jointIndex);

						m = createGearedMotor();
						((GearedMotor*) m)->init(data);
						break;
					}
				case SERVO_MOTOR:
					{
						// Make a local copy of the Motor's data.
						ServoMotorData data = *(ServoMotorData*) motorData;

						// Setup the Joint reference pointer.
						data.joint = jointList.at(data.internal_jointIndex);

						m = createServoMotor();
						((ServoMotor*) m)->init(data);
						break;
					}
				case SPRING_MOTOR:
					{
						// Make a local copy of the Motor's data.
						SpringMotorData data = *(SpringMotorData*) motorData;

						// Setup the Solid reference pointer.
						data.solid = solidList.at(data.internal_solidIndex);

						m = createSpringMotor();
						((SpringMotor*) m)->init(data);
						break;
					}
				case THRUSTER_MOTOR:
					{
						// Make a local copy of the Motor's data.
						ThrusterMotorData data = *(ThrusterMotorData*) motorData;

						// Setup the Solid reference pointer.
						data.solid = solidList.at(data.internal_solidIndex);

						m = createThrusterMotor();
						((ThrusterMotor*) m)->init(data);
						break;
					}
				default:
					assert(false);
			}

			// Add the Motor to the BlueprintInstance.
			instance.addMotor(m);
		}

		// Create all Sensors in the Blueprint.
		for (i = 0; i < bp.getNumSensors(); ++i)
		{
			Sensor* s = NULL;
			SensorData* sensorData = bp.getSensorData(i);

			// These are necessary because we don't want to change teh
			// Blueprint's Sensor data directly; we need a local copy.
			Solid* solidPtr = NULL;
			Matrix44r transform = sensorData->transform;

			// Setup the Solid reference pointer.
			if (-1 == sensorData->internal_solidIndex)
			{
				// This Sensor must not be attached to any Solid.
				solidPtr = NULL;
			}
			else
			{
				// This Sensor is attached to a Solid.
				solidPtr = solidList.at(sensorData->internal_solidIndex);
			}

			if (useScale)
			{
				// Scale the Solid's transform.
				transform[12] *= scale;
				transform[13] *= scale;
				transform[14] *= scale;
			}

			if (useOffset && !solidPtr)
			{
				// If the Sensor is not attached to a Solid, offset the 
				// Sensor's transform.
				transform = offset * transform;
			}

			// Allocate the right type of Sensor.
			switch (sensorData->getType())
			{
				case ACCELERATION_SENSOR:
					{
						// Make a local copy of the Sensor's data.
						AccelerationSensorData data =
						    *(AccelerationSensorData*) sensorData;
						data.solid = solidPtr;
						data.transform = transform;

						s = createAccelerationSensor();
						((AccelerationSensor*) s)->init(data);
						break;
					}
				case INCLINE_SENSOR:
					{
						// Make a local copy of the Sensor's data.
						InclineSensorData data = *(InclineSensorData*) sensorData;
						data.solid = solidPtr;
						data.transform = transform;

						s = createInclineSensor();
						((InclineSensor*) s)->init(data);
						break;
					}
				case RAYCAST_SENSOR:
					{
						// Make a local copy of the Sensor's data.
						RaycastSensorData data = *(RaycastSensorData*) sensorData;
						data.solid = solidPtr;
						data.transform = transform;

						s = createRaycastSensor();
						((RaycastSensor*) s)->init(data);
						break;
					}
				case VOLUME_SENSOR:
					{
						// Make a local copy of the Sensor's data.
						VolumeSensorData data = *(VolumeSensorData*) sensorData;
						data.solid = solidPtr;
						data.transform = transform;

						s = createVolumeSensor();
						((VolumeSensor*) s)->init(data);
						break;
					}
				default:
					assert(false);
			}

			// Add the Sensor to the BlueprintInstance.
			instance.addSensor(s);
		}

		solidList.clear();
		jointList.clear();
	}

	//void Simulator::setMaxLinearVel(real max)
	//{
	//  assert(max >= 0.0);
	//  mMaxLinearVel = max;
	//}

	//real Simulator::getMaxLinearVel()const
	//{
	//  return mMaxLinearVel;
	//}

	//void Simulator::setMaxAngularVel(real max)
	//{
	//  assert(max >= 0.0);
	//  mMaxAngularVel = max;
	//}

	//real Simulator::getMaxAngularVel()const
	//{
	//  return mMaxAngularVel;
	//}

	//void Simulator::setDefaultLinearDamping(real ld)
	//{
	//  assert(ld >= 0.0);
	//  mDefaultLinearDamping = ld;
	//}

	//real Simulator::getDefaultLinearDamping()const
	//{
	//  return mDefaultLinearDamping;
	//}

	//void Simulator::setDefaultAngularDamping(real ad)
	//{
	//  assert(ad >= 0.0);
	//  mDefaultAngularDamping = ad;
	//}

	//real Simulator::getDefaultAngularDamping()const
	//{
	//  return mDefaultAngularDamping;
	//}

	void Simulator::setUserData(void* data)
	{
		mUserData = data;
	}

	void* Simulator::getUserData()
	{
		return mUserData;
	}

	void Simulator::setupContactGroups(unsigned int group0,
	                                    unsigned int group1, bool makeContacts)
	{
		if (group0 > 31)
		{
			OPAL_LOGGER("warning") << "opal::Simulator::setupContactGroups: "
			<< "Invalid contact group " << group0
			<< ". Request will be ignored." << std::endl;
			return ;
		}

		if (group1 > 31)
		{
			OPAL_LOGGER("warning") << "opal::Simulator::setupContactGroups: "
			<< "Invalid contact group " << group1
			<< ". Request will be ignored." << std::endl;
			return ;
		}

		// The interaction always goes both ways, so we need to set the bit
		// flags both ways.

		unsigned long int group0Bit = 1 << group0;
		unsigned long int group1Bit = 1 << group1;

		if (makeContacts)
		{
			// Raise the group1 bit in group0's array.
			mContactGroupFlags[group0] |= group1Bit;

			// Raise the group0 bit in group1's array.
			mContactGroupFlags[group1] |= group0Bit;
		}
		else
		{
			unsigned long int tempMask = 0xFFFFFFFF;
			unsigned long int notGroup0Bit = group0Bit ^ tempMask;
			unsigned long int notGroup1Bit = group1Bit ^ tempMask;

			// Lower the group1 bit in group0's array.
			mContactGroupFlags[group0] &= notGroup1Bit;

			// Lower the group0 bit in group1's array.
			mContactGroupFlags[group1] &= notGroup0Bit;
		}
	}

	void Simulator::setupContactGroup(unsigned int group,
	                                   bool makeContacts)
	{
		int i = 0;
		for (i = 0; i < 32; ++i)
		{
			setupContactGroups(group, i, makeContacts);
		}
	}

	bool Simulator::groupsMakeContacts(unsigned int group0,
	                                    unsigned int group1)
	{
		// We only need to check for "one side" of the contact groups
		// here because the groups are always setup both ways (i.e.
		// the interaction between object 0's contact group and
		// object 1's contact group is always symmetric).

		unsigned long int group1Bit = 1 << group1;
		if (internal_getContactGroupFlags(group0) & group1Bit)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void Simulator::setStaticSleepingContactsEnabled(bool enable)
	{
		mStaticSleepingContactsEnabled = enable;
	}

	bool Simulator::areStaticSleepingContactsEnabled()
	{
		return mStaticSleepingContactsEnabled;
	}

	unsigned long int Simulator::internal_getContactGroupFlags(
	    unsigned int groupNum) const
	{
		return mContactGroupFlags[groupNum];
	}

	Solid* Simulator::createPlane(real a, real b, real c, real d,
	                               const Material& m)
	{
		// Create the plane's Solid and make it static.
		Solid * plane = createSolid();
		plane->setStatic(true);

		// Setup the plane's Shape data.
		opal::PlaneShapeData planeData;
		planeData.material = m;
		planeData.abcd[0] = a;
		planeData.abcd[1] = b;
		planeData.abcd[2] = c;
		planeData.abcd[3] = d;

		// Add the Shape to the Solid.
		plane->addShape(planeData);

		return plane;
	}

	unsigned int Simulator::getNumSolids() const
	{
		return (unsigned int) (mSolidList.size());
	}

	Solid* Simulator::getSolid(unsigned int i) const
	{
		return mSolidList.at(i);
	}

	void Simulator::destroySolid(Solid* s)
	{
		if (mIsSolidDestructionSafe)
		{
			removeSolid(s);
		}
		else
		{
			mSolidGarbageList.push_back(s);
		}
	}

	void Simulator::destroyAllSolids()
	{
		while (!mSolidList.empty())
		{
			removeSolid(mSolidList.back());
		}
	}

	void Simulator::destroyJoint(Joint* j)
	{
		if (mIsJointDestructionSafe)
		{
			removeJoint(j);
		}
		else
		{
			mJointGarbageList.push_back(j);
		}
	}

	void Simulator::destroyAllJoints()
	{
		while (!mJointList.empty())
		{
			removeJoint(mJointList.back());
		}
	}

	ThrusterMotor* Simulator::createThrusterMotor()
	{
		ThrusterMotor * newMotor = new ThrusterMotor();
		addMotor(newMotor);
		return newMotor;
	}

	VelocityMotor* Simulator::createVelocityMotor()
	{
		VelocityMotor * newMotor = new VelocityMotor(this);
		addMotor(newMotor);
		return newMotor;
	}

	GearedMotor* Simulator::createGearedMotor()
	{
		GearedMotor * newMotor = new GearedMotor();
		addMotor(newMotor);
		return newMotor;
	}

	ServoMotor* Simulator::createServoMotor()
	{
		ServoMotor * newMotor = new ServoMotor();
		addMotor(newMotor);
		return newMotor;
	}

	AttractorMotor* Simulator::createAttractorMotor()
	{
		AttractorMotor * newMotor = new AttractorMotor();
		addMotor((Motor*) newMotor);
		return newMotor;
	}

	SpringMotor* Simulator::createSpringMotor()
	{
		SpringMotor * newMotor = new SpringMotor();
		addMotor((Motor*) newMotor);
		return newMotor;
	}

	//void Simulator::registerCustomMotor(Motor* m)
	//{
	//  m->internal_setCustom(true);
	//  addMotor(m);
	//}

	void Simulator::destroyMotor(Motor* m)
	{
		removeMotor(m);
	}

	void Simulator::destroyAllMotors()
	{
		while (!mMotorList.empty())
		{
			removeMotor(mMotorList.back());
		}
	}

	AccelerationSensor* Simulator::createAccelerationSensor()
	{
		AccelerationSensor * newSensor = new AccelerationSensor(this);
		addSensor(newSensor);
		return newSensor;
	}

	InclineSensor* Simulator::createInclineSensor()
	{
		InclineSensor * newSensor = new InclineSensor();
		addSensor(newSensor);
		return newSensor;
	}

	RaycastSensor* Simulator::createRaycastSensor()
	{
		RaycastSensor * newSensor = new RaycastSensor(this);
		addSensor(newSensor);
		return newSensor;
	}

	VolumeSensor* Simulator::createVolumeSensor()
	{
		VolumeSensor * newSensor = new VolumeSensor(this);
		addSensor(newSensor);
		return newSensor;
	}

	void Simulator::destroySensor(Sensor* s)
	{
		removeSensor(s);
	}

	void Simulator::destroyAllSensors()
	{
		while (!mSensorList.empty())
		{
			removeSensor(mSensorList.back());
		}
	}

	void Simulator::setSolverAccuracy(SolverAccuracyLevel level)
	{
		mSolverAccuracyLevel = level;
	}

	SolverAccuracyLevel Simulator::getSolverAccuracy() const
	{
		return mSolverAccuracyLevel;
	}

	void Simulator::setGlobalStability(real value)
	{
		// Override this.
	}

	void Simulator::setMaxCorrectingVel(real vel)
	{
		assert(vel > 0);
		mMaxCorrectingVel = vel;
	}

	real Simulator::getMaxCorrectingVel() const
	{
		return mMaxCorrectingVel;
	}

	void Simulator::setMaxContacts(unsigned int mc)
	{
		assert(mc <= globals::maxMaxContacts);
		mMaxContacts = mc;
	}

	unsigned int Simulator::getMaxContacts() const
	{
		return mMaxContacts;
	}

	void Simulator::addSolid(Solid* s)
	{
		mSolidList.push_back(s);
	}

	void Simulator::removeSolid(Solid* s)
	{
		// TODO: make this more efficient by not iterating through all Motors,
		// Sensors, and Joints; maybe have the Solid maintain pointers to
		// things that depend on it.

		// Disable Motors that depend on the given Solid.
		std::vector<Motor*>::iterator iter;
		for (iter = mMotorList.begin(); iter != mMotorList.end(); ++iter)
		{
			if ((*iter)->internal_dependsOnSolid(s))
			{
				(*iter)->setEnabled(false);
			}
		}

		// Disable Joints that depend on the given Solid.
		std::vector<Joint*>::iterator iter2;
		for (iter2 = mJointList.begin(); iter2 != mJointList.end(); ++iter2)
		{
			if ((*iter2)->internal_dependsOnSolid(s))
			{
				(*iter2)->setEnabled(false);
			}
		}

		// Disable Sensors that depend on the given Solid.
		std::vector<Sensor*>::iterator iter3;
		for (iter3 = mSensorList.begin(); iter3 != mSensorList.end(); ++iter3)
		{
			if ((*iter3)->internal_dependsOnSolid(s))
			{
				(*iter3)->setEnabled(false);
			}
		}

		// Now remove the Solid.
		for (size_t i = 0; i < mSolidList.size(); ++i)
		{
			if (mSolidList[i] == s)
			{
				s->internal_destroy();
				mSolidList[i] = mSolidList.back();
				mSolidList.pop_back();

				return ;
			}
		}
	}

	void Simulator::addJoint(Joint* j)
	{
		mJointList.push_back(j);
	}

	void Simulator::removeJoint(Joint* j)
	{
		// TODO: make this more efficient by not iterating through all Motors;
		// maybe have the Solid maintain pointers to things that
		// depend on it.

		// Disable Motors that depend on the given Joint.
		std::vector<Motor*>::iterator iter;
		for (iter = mMotorList.begin(); iter != mMotorList.end(); ++iter)
		{
			if ((*iter)->internal_dependsOnJoint(j))
			{
				(*iter)->setEnabled(false);
			}
		}

		// Now remove the Joint.
		for (size_t i = 0; i < mJointList.size(); ++i)
		{
			if (mJointList[i] == j)
			{
				j->internal_destroy();
				mJointList[i] = mJointList.back();
				mJointList.pop_back();

				return ;
			}
		}
	}

	void Simulator::addMotor(Motor* m)
	{
		mMotorList.push_back(m);
	}

	void Simulator::removeMotor(Motor* m)
	{
		for (size_t i = 0; i < mMotorList.size(); ++i)
		{
			if (mMotorList[i] == m)
			{
				//if (!m->internal_isCustom())
				//{
				//  delete m;
				//}
				m->internal_destroy();
				mMotorList[i] = mMotorList.back();
				mMotorList.pop_back();

				return ;
			}
		}
	}

	void Simulator::addSensor(Sensor* s)
	{
		mSensorList.push_back(s);
	}

	void Simulator::removeSensor(Sensor* s)
	{
		for (size_t i = 0; i < mSensorList.size(); ++i)
		{
			if (mSensorList[i] == s)
			{
				s->internal_destroy();
				mSensorList[i] = mSensorList.back();
				mSensorList.pop_back();

				return ;
			}
		}
	}

	void Simulator::addSpace(Space* s)
	{
		mSpaceList.push_back(s);
	}

	Space* Simulator::getRootSpace()
	{
		return mRootSpace;
	}

	void Simulator::destroyGarbage()
	{
		// Destroy garbage Solids.
		while (!mSolidGarbageList.empty())
		{
			destroySolid(mSolidGarbageList.back());
			mSolidGarbageList.pop_back();
		}

		// Destroy garbage Joints.
		while (!mJointGarbageList.empty())
		{
			destroyJoint(mJointGarbageList.back());
			mJointGarbageList.pop_back();
		}
	}
}
