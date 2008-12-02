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

#include "Robot.h"
#include "../base/SimulationEngine.h"
#include "../base/AudioManager.h"

#include <ogre/Ogre.h>

Robot::Robot(SimulationEngine& engine, verve::real agentUpdateFrequency)
: LearningEntity(agentUpdateFrequency), Car(engine), 
mMaxRangeFinderDistance(20), 
mMaxPerceivedVelocityX(1), 
mMaxPerceivedVelocityZ(6)
//mMaxPerceivedAccelX(2), 
//mMaxPerceivedAccelZ(5)
{
	//mAccelSensor = NULL;
	createAgent();
}

Robot::~Robot()
{
	mRangeFinders.clear();
}

void Robot::init(const std::string& blueprintFilename, 
	const std::string& chassisMaterial, opal::real scale, 
	const opal::Matrix44r& transform, opal::real enginePitchScale)
{
	Car::init(blueprintFilename, chassisMaterial, scale, transform, 
		enginePitchScale);

	// Note: this assumes the physics XML file has already been loaded.
	setupVisuals();
}

void Robot::createAgent()
{
	if (mAgent)
	{
		mAgent->destroy();
	}

	verve::AgentDescriptor desc;
	desc.setArchitecture(verve::CURIOUS_MODEL_RL);
	desc.setMaxNumPlanningSteps(5);
	desc.setDynamicRBFEnabled(true);

	// Setup the sensors: 6 distance, 2 acceleration
	desc.addContinuousSensor();
	desc.addContinuousSensor();
	desc.addContinuousSensor();
	desc.addContinuousSensor();
	desc.addContinuousSensor();
	//desc.addContinuousSensor();
	//desc.addContinuousSensor();
	//desc.addContinuousSensor();
	desc.setContinuousSensorResolution(12);

	// Use 6 actions: 
	// 1. forward acceleration
	// 2. backward acceleration
	// 3. idle
	// 4. steer left
	// 5. steer right
	// 6. center steering
	desc.setNumOutputs(6);
	mAgent = verve::createAgent(desc);
	mAgent->setTDLearningRate((verve::real)0.01, 2);
	//mAgent->setModelLearningRate((verve::real)0.001);
	//mAgent->setETraceTimeConstant((verve::real)0.5);
	//mAgent->setTDDiscountTimeConstant((verve::real)1.0);
	mObservation.init(*mAgent);
}

void Robot::loadParts(const std::string& blueprintFilename, 
	const std::string& chassisMaterial, opal::real scale, 
	const opal::Matrix44r& transform)
{
	opal::Blueprint carBlueprint;
	opal::loadFile(carBlueprint, blueprintFilename);
	opal::BlueprintInstance carBPInstance;
	mEngine->getSimulator()->instantiateBlueprint(carBPInstance, carBlueprint, 
		transform, scale);

	mChassis = carBPInstance.getSolid("chassis");
	mFLWheel = carBPInstance.getSolid("FLWheel");
	mFRWheel = carBPInstance.getSolid("FRWheel");
	mRLWheel = carBPInstance.getSolid("RLWheel");
	mRRWheel = carBPInstance.getSolid("RRWheel");
	mFLJoint = carBPInstance.getJoint("FLJoint");
	mFRJoint = carBPInstance.getJoint("FRJoint");
	mFLMotor = static_cast<opal::GearedMotor*>(carBPInstance.getMotor("FLMotor"));
	mFRMotor = static_cast<opal::GearedMotor*>(carBPInstance.getMotor("FRMotor"));
	for (unsigned int i = 0; i < carBPInstance.getNumSensors(); ++i)
	{
		if (carBPInstance.getSensor(i)->getType() == opal::RAYCAST_SENSOR)
		{
			mRangeFinders.push_back(static_cast<opal::RaycastSensor*>(
				carBPInstance.getSensor(i)));
		}
	}

	//// TESTING: accelerometer
	//opal::AccelerationSensorData accelData;
	//accelData.solid = mChassis;
	//mAccelSensor = mEngine->getSimulator()->createAccelerationSensor();
	//mAccelSensor->init(accelData);

	char temp[64];
	static int count = 0;
	sprintf(temp, "_robot%d", count);
	std::string partSuffix = temp;
	++count;

	mEngine->createPhysicalEntity("chassis" + partSuffix, chassisMaterial, mChassis);
	mEngine->createPhysicalEntity("FLWheel" + partSuffix, "Plastic/DarkGray", mFLWheel);
	mEngine->createPhysicalEntity("FRWheel" + partSuffix, "Plastic/DarkGray", mFRWheel);
	mEngine->createPhysicalEntity("RLWheel" + partSuffix, "Plastic/DarkGray", mRLWheel);
	mEngine->createPhysicalEntity("RRWheel" + partSuffix, "Plastic/DarkGray", mRRWheel);
}

void Robot::setupVisuals()
{
	// Note: this assumes the physics XML file has already been loaded.

	Ogre::SceneManager* sm = mEngine->getSceneManager();

	for (size_t i=0; i < mRangeFinders.size(); ++i)
	{
		// Create a line to represent the laser.
		char str[32];
		sprintf(str, "rangefinderLine%d", (int)i);
		Ogre::SceneNode* lineNode = sm->getRootSceneNode()->
			createChildSceneNode(str);
		Ogre::Entity* lineEntity = sm->createEntity(str, "cylinder.mesh");
		lineEntity->setMaterialName("Translucent/Red");
		lineEntity->setCastShadows(false);
		lineNode->attachObject(lineEntity);

		// Create a sphere to represent ray intersections.
		sprintf(str, "rangefinderSphere%d", (int)i);
		Ogre::SceneNode* sphereNode = sm->getRootSceneNode()->
			createChildSceneNode(str);
		sphereNode->scale(0.2, 0.2, 0.2);
		Ogre::Entity* sphereEntity = sm->createEntity(str, "sphere.mesh");
		sphereEntity->setMaterialName("Translucent/Red");
		sphereEntity->setCastShadows(false);
		sphereEntity->setVisible(false);
		sphereNode->attachObject(sphereEntity);
	}

	// Setup a visual "curiosity bubble."  Note: This only works for one 
	// robot (the chassis scene node name is hard-coded here).
	Ogre::SceneNode* curiosityBubbleNode = sm->getSceneNode(
		"chassis_robot0")->createChildSceneNode(
		"curiosityBubble", Ogre::Vector3(0, 1, 0));
	curiosityBubbleNode->scale(0.2, 0.2, 0.2);
	Ogre::Entity* curiosityBubbleEntity = sm->createEntity("chassis_robot0", 
		"sphere.mesh");
	Ogre::MaterialPtr curiosityBubbleMaterial = 
		((Ogre::MaterialPtr)Ogre::MaterialManager::
		getSingletonPtr()->getByName("Translucent/Green"))->clone(
		"curiosityBubbleMaterial");
	curiosityBubbleEntity->setMaterialName(curiosityBubbleMaterial->getName());
	curiosityBubbleEntity->setCastShadows(false);
	curiosityBubbleNode->attachObject(curiosityBubbleEntity);
}

void Robot::resetBody()
{
	//// Reset to the initial configuration.
	//mPendulumSolid->setGlobalLinearVel(opal::Vec3r(0, 0, 0));
	//mPendulumSolid->setGlobalAngularVel(opal::Vec3r(0, 0, 0));
	//mPendulumSolid->zeroForces();
	//mPendulumSolid->setTransform(mInitSolidData.transform);
}

void Robot::randomizeState()
{
	//// Setup a random angle and angular velocity.

	//opal::Matrix44r pendulumTransform = mInitSolidData.transform;

	//// Translate to the Joint position (i.e. along the pendulum's +Z 
	//// axis).
	//pendulumTransform.translate(0, 0, (verve::real)0.5 * mLength);

	//// Rotate to a random angle.
	//pendulumTransform.rotate(verve::globals::randomRealUniform(0, 360), 
	//	0, 1, 0);

	//// Translate back from the Joint position (i.e. along the 
	//// pendulum's -Z axis).
	//pendulumTransform.translate(0, 0, (verve::real)-0.5 * mLength);
	//mPendulumSolid->setTransform(pendulumTransform);

	//opal::Vec3r vel(0, 0, verve::globals::randomRealUniform(-mMaxVel, 
	//	mMaxVel));
	//mPendulumSolid->setGlobalAngularVel(vel);
}

std::string Robot::getCurrentActionName()const
{
	return mCurrentActionName;
}

void Robot::updateObservation()
{
	// Scale all sensory inputs to be between 0 and 1.

	unsigned int sensorIndex = 0;

	opal::RaycastResult raycastResult;

	// Distance sensor inputs.
	for (unsigned int i=0; i < mRangeFinders.size(); ++i)
	{
		raycastResult = mRangeFinders[i]->fireRay(mMaxRangeFinderDistance);

		// We also update the laser visuals here (maybe move this somewhere 
		// else later...).

		char str[32];
		sprintf(str, "rangefinderLine%d", i);
		Ogre::Entity* lineEntity = mEngine->getSceneManager()->getEntity(str);
		sprintf(str, "rangefinderSphere%d", i);
		Ogre::Entity* sphereEntity = mEngine->getSceneManager()->getEntity(str);

		if (raycastResult.solid && raycastResult.distance == 0)
		{
			// If something was hit, but the intersection distance is zero, 
			// set the Observation value to minimum and don't draw the lasers.
			mObservation.setContinuousValue(sensorIndex, 0);

			lineEntity->setVisible(false);
		}
		else
		{
			opal::Point3r basePos = mRangeFinders[i]->getData().solid->getTransform() * 
				mRangeFinders[i]->getTransform().getPosition();
			opal::Point3r endPos;

			lineEntity->setVisible(true);

			if (!raycastResult.solid)
			{
				// Nothing was hit.  Set the Observation value to maximum, and 
				// draw lasers of maximum range.
				mObservation.setContinuousValue(sensorIndex, 1);

				opal::Vec3r laserDir = (mRangeFinders[i]->getData().solid->getTransform() * 
					mRangeFinders[i]->getTransform() * 
					mRangeFinders[i]->getRay().getDir()).unit();
				laserDir *= mMaxRangeFinderDistance;
				endPos = basePos + laserDir;

				//lineEntity->setMaterialName("Translucent/Blue");
				sphereEntity->setVisible(false);
			}
			else
			{
				// Something was hit.  Set the scaled Observation value, and 
				// draw the appropriate lasers.
				mObservation.setContinuousValue(sensorIndex, raycastResult.distance / 
					mMaxRangeFinderDistance);

				endPos = raycastResult.intersection;

				//lineEntity->setMaterialName("Translucent/Red");

				sphereEntity->setVisible(true);
				Ogre::Vector3 point(endPos[0], endPos[1], endPos[2]);
				sphereEntity->getParentSceneNode()->setPosition(point);
			}

			Ogre::Vector3 point0(basePos[0], basePos[1], basePos[2]);
			Ogre::Vector3 point1(endPos[0], endPos[1], endPos[2]);
			Ogre::Vector3 laserVector = point0 - point1;

			Ogre::SceneNode* node = lineEntity->getParentSceneNode();
			node->setPosition(0.5 * (point0 + point1));
			node->setDirection(laserVector, Ogre::Node::TS_WORLD);
			node->setScale(0.05, 0.05, laserVector.length());

			++sensorIndex;
		}
	}

	

	// Input 6: linear velocity, x axis
	opal::Vec3r localLinearVel = mChassis->getLocalLinearVel();
	opal::real normalizedVel = localLinearVel[0] / mMaxPerceivedVelocityX;
	if (normalizedVel > 1)
	{
		normalizedVel = 1;
	}
	else if (normalizedVel < -1)
	{
		normalizedVel = -1;
	}
	normalizedVel += 1;
	normalizedVel *= (opal::real)0.5;
	mObservation.setContinuousValue(sensorIndex, normalizedVel);
	++sensorIndex;

	// Input 7: linear velocity, z axis
	normalizedVel = -localLinearVel[2] / mMaxPerceivedVelocityZ;
	if (normalizedVel > 1)
	{
		normalizedVel = 1;
	}
	else if (normalizedVel < -1)
	{
		normalizedVel = -1;
	}
	normalizedVel += 1;
	normalizedVel *= (opal::real)0.5;
	mObservation.setContinuousValue(sensorIndex, normalizedVel);
	++sensorIndex;

	//// Input 6: linear acceleration, x axis
	//opal::Vec3r localLinearAccel = mAccelSensor->getLocalLinearAccel();
	//std::cout << localLinearAccel << std::endl;
	//opal::real normalizedAccel = localLinearAccel[0] / mMaxPerceivedAccelX;
	//if (normalizedAccel > 1)
	//{
	//	normalizedAccel = 1;
	//}
	//else if (normalizedAccel < -1)
	//{
	//	normalizedAccel = -1;
	//}
	//normalizedAccel += 1;
	//normalizedAccel *= (opal::real)0.5;
	//mObservation.setContinuousValue(sensorIndex, normalizedAccel);
	//++sensorIndex;

	//// Input 7: linear acceleration, z axis
	//normalizedAccel = -localLinearAccel[2] / mMaxPerceivedAccelZ;
	//if (normalizedAccel > 1)
	//{
	//	normalizedAccel = 1;
	//}
	//else if (normalizedAccel < -1)
	//{
	//	normalizedAccel = -1;
	//}
	//normalizedAccel += 1;
	//normalizedAccel *= (opal::real)0.5;
	//mObservation.setContinuousValue(sensorIndex, normalizedAccel);
	//++sensorIndex;
}

void Robot::updateEffectors()
{
	switch(mActionIndex)
	{
		case 0:
			forward();
			mCurrentActionName = "forward";
			break;
		case 1:
			reverse();
			mCurrentActionName = "reverse";
			break;
		case 2:
			idle();
			mCurrentActionName = "idle";
			break;
		case 3:
			setSteering(-1);
			mCurrentActionName = "steer left";
			break;
		case 4:
			setSteering(1);
			mCurrentActionName = "steer right";
			break;
		case 5:
			setSteering(0);
			mCurrentActionName = "center steering";
			break;
		default:
			assert(false);
	}
}

void Robot::updateVisuals(opal::real dt)
{
	Ogre::SceneManager* sm = mEngine->getSceneManager();

	// We need to make the rangefinder visuals not visible so they 
	// only show up when the sensors are actually updated.
	for (unsigned int i=0; i < mRangeFinders.size(); ++i)
	{
		char str[32];
		sprintf(str, "rangefinderLine%d", i);
		sm->getEntity(str)->setVisible(false);
		sprintf(str, "rangefinderSphere%d", i);
		sm->getEntity(str)->setVisible(false);
	}

	// Update the "curiosity bubble."
	const opal::real timeConstant = (opal::real)0.4;
	const opal::real decayConstant = powf((opal::real)2.7183, 
		dt / -timeConstant);
	static Ogre::Real curiosityLevel = 0;
	opal::real currentActualLevel = mAgent->getDebugData().curiosityReward;
	if (currentActualLevel > 1)
	{
		currentActualLevel = 1;
	}
	curiosityLevel = curiosityLevel + (1 - decayConstant) * 
		(currentActualLevel - curiosityLevel);

	Ogre::SceneNode* curiosityBubbleNode = sm->getSceneNode("curiosityBubble");
	Ogre::Real bubbleSize = 0.5 * curiosityLevel;
	curiosityBubbleNode->setScale(bubbleSize, bubbleSize, bubbleSize);

	Ogre::Pass* curiosityMaterialPass = 
		((Ogre::MaterialPtr)Ogre::MaterialManager::getSingletonPtr()->
		getByName("curiosityBubbleMaterial"))->getTechnique(0)->getPass(0);
	//curiosityMaterialPass->setSelfIllumination(0, 0, 0);
	curiosityMaterialPass->setDiffuse(0, 0, 0, curiosityLevel);
}
