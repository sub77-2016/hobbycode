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

#include "Car.h"
#include "SimulationEngine.h"
#include "AudioManager.h"

Car::Car(SimulationEngine& engine)
: mSteeringLimit(16)
{
	mEngine = &engine;
	mEnginePitchScale = 1;
	mEngineLevel = 0;
	mSteeringAngle = 0;
	mChassis = NULL;
	mFLWheel = NULL;
	mFRWheel = NULL;
	mRLWheel = NULL;
	mRRWheel = NULL;
	mFLJoint = NULL;
	mFRJoint = NULL;
	mFLMotor = NULL;
	mFRMotor = NULL;

	engine.getSimulator()->addGlobalCollisionEventHandler(this);
}

Car::~Car()
{
}

void Car::init(const std::string& blueprintFilename, 
	const std::string& chassisMaterial, opal::real scale, 
	const opal::Matrix44r& transform, opal::real enginePitchScale)
{
	loadParts(blueprintFilename, chassisMaterial, scale, transform);

	mEnginePitchScale = enginePitchScale;

	AUDIO.loadMultiSound("hit1", "../data/sounds/hit1.wav");
	AUDIO.loadMultiSound("hit2", "../data/sounds/hit2.wav");
	AUDIO.loadMultiSound("rubberHit", "../data/sounds/rubberHit.wav");

	char temp[64];
	static int count = 0;
	sprintf(temp, "servoLoop%d", count);
	mEngineSoundName = temp;
	++count;

	AUDIO.loadSound(mEngineSoundName, "../data/sounds/servoLoop.wav");

	AUDIO.getSound(mEngineSoundName)->setRepeat(true);
	AUDIO.getSound(mEngineSoundName)->setVolume(0);
	AUDIO.getSound(mEngineSoundName)->play();
}

void Car::loadParts(const std::string& blueprintFilename, 
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

	char temp[64];
	static int count = 0;
	sprintf(temp, "_car%d", count);
	std::string partSuffix = temp;
	++count;

	mEngine->createPhysicalEntity("chassis" + partSuffix, chassisMaterial, mChassis);
	mEngine->createPhysicalEntity("FLWheel" + partSuffix, "Plastic/DarkGray", mFLWheel);
	mEngine->createPhysicalEntity("FRWheel" + partSuffix, "Plastic/DarkGray", mFRWheel);
	mEngine->createPhysicalEntity("RLWheel" + partSuffix, "Plastic/DarkGray", mRLWheel);
	mEngine->createPhysicalEntity("RRWheel" + partSuffix, "Plastic/DarkGray", mRRWheel);
}

void Car::forward()
{
	setThrottle(1);
}

void Car::reverse()
{
	setThrottle(-1);
}

void Car::idle()
{
	setThrottle(0);
}

void Car::setThrottle(opal::real value)
{
	assert(value >= -1 && value <= 1);
	mFLMotor->setThrottle(value);
	mFRMotor->setThrottle(value);
}

void Car::steerLeft()
{
	setSteering(-1);
}

void Car::steerRight()
{
	setSteering(1);
}

void Car::setSteering(opal::real value)
{
	assert(value >= -1 && value <= 1);
	mSteeringAngle = value * mSteeringLimit;
	mFLJoint->setLimitRange(0, mSteeringAngle, mSteeringAngle);
	mFRJoint->setLimitRange(0, mSteeringAngle, mSteeringAngle);
}

opal::Solid* Car::getChassis()
{
	return mChassis;
}

opal::GearedMotor* Car::getFLMotor()
{
	return mFLMotor;
}

opal::GearedMotor* Car::getFRMotor()
{
	return mFRMotor;
}

Ogre::SceneNode* Car::getSceneNode()
{
	return mEngine->getPhysicalEntity("chassis")->getSceneNode();
}

void Car::updateSoundEffects(opal::real dt)
{
	const opal::real timeConstant = (opal::real)0.4;
	const opal::real decayConstant = powf((opal::real)2.7183, 
		dt / -timeConstant);

	mEngineLevel = mEngineLevel + (1 - decayConstant) * 
		(mFLMotor->getThrottle() - mEngineLevel);

	AUDIO.getSound(mEngineSoundName)->setPitchShift(0.2f + 
		fabs(mEngineLevel) * mEnginePitchScale * 0.4f);
	AUDIO.getSound(mEngineSoundName)->setVolume(0.01f + 
		fabs(mEngineLevel) * 0.4f);
}

void Car::handleCollisionEvent(const opal::CollisionEvent& e)
{
	// OLD WAY
	//const opal::real impactConstant = 30;
	//opal::real impact = impactConstant * e.depth;

	// To get the impact magnitude to use for the sound volume, we take
	// the magnitude of the relative velocity of the two solids at the
	// contact point dotted with the collision normal vector.
	opal::real impact = opal::abs(opal::dot(
		e.thisSolid->getGlobalLinearVelAtGlobalPos(e.pos) -
		e.otherSolid->getGlobalLinearVelAtGlobalPos(e.pos), e.normal));

	if (e.thisSolid == mChassis || e.otherSolid == mChassis)
	{
		// Handle collisions involving the car chassis.

		const opal::real volumeScalar = (opal::real)0.1;

		if (impact > 0.3f)
		{
			if (impact < 3.0f)
			{
				AUDIO.getMultiSound("hit1")->setVolume(volumeScalar * impact);
				AUDIO.getMultiSound("hit1")->play();
			}
			else
			{
				AUDIO.getMultiSound("hit2")->setVolume(volumeScalar * impact);
				AUDIO.getMultiSound("hit2")->play();
			}
		}
	}
	else if (e.thisSolid == mFLWheel || e.otherSolid == mFLWheel 
		|| e.thisSolid == mFRWheel || e.otherSolid == mFRWheel 
		|| e.thisSolid == mRLWheel || e.otherSolid == mRLWheel 
		|| e.thisSolid == mRRWheel || e.otherSolid == mRRWheel)
	{
		// Handle collisions involving the car wheels.

		if (impact > 5.0f)
		{
			const opal::real volumeScalar = (opal::real)0.05;
			AUDIO.getMultiSound("rubberHit")->setVolume(volumeScalar * impact);
			AUDIO.getMultiSound("rubberHit")->play();
		}
	}
	else
	{
		// Handle all other collisions.

		const opal::real volumeScalar = (opal::real)0.05;

		if (impact > 5.0f)
		{
			AUDIO.getMultiSound("hit1")->setVolume(volumeScalar * impact);
			AUDIO.getMultiSound("hit1")->play();
		}
	}
}
