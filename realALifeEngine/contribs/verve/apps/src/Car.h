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

#ifndef CAR_H
#define CAR_H

#include <opal/opal.h>

class SimulationEngine;

namespace Ogre
{
	class SceneNode;
}

class Car : public opal::CollisionEventHandler
{
public:
	Car(SimulationEngine& engine);

	~Car();

	/// This must be called after the Car is created.
	virtual void init(const std::string& blueprintFilename, 
		const std::string& chassisMaterial, opal::real scale=1, 
		const opal::Matrix44r& transform=opal::Matrix44r(), 
		opal::real enginePitchScale=1);

	void forward();

	void reverse();

	void idle();

	/// Value must be within [-1.0, 1.0].
	void setThrottle(opal::real value);

	void steerLeft();

	void steerRight();

	void setSteering(opal::real value);

	opal::Solid* getChassis();

	opal::GearedMotor* getFLMotor();

	opal::GearedMotor* getFRMotor();

	Ogre::SceneNode* getSceneNode();

	void updateSoundEffects(opal::real dt);

	virtual void OPAL_CALL handleCollisionEvent(
		const opal::CollisionEvent& e);

protected:
	virtual void loadParts(const std::string& blueprintFilename, 
		const std::string& chassisMaterial, opal::real scale=1, 
		const opal::Matrix44r& transform=opal::Matrix44r());

	/// A local copy of the main engine.
	SimulationEngine* mEngine;

	/// Used for the engine noise.
	opal::real mEnginePitchScale;
	opal::real mEngineLevel;
	std::string mEngineSoundName;

	const opal::real mSteeringLimit;
	opal::real mSteeringAngle;

	opal::Solid* mChassis;
	opal::Solid* mFLWheel;
	opal::Solid* mFRWheel;
	opal::Solid* mRLWheel;
	opal::Solid* mRRWheel;
	opal::Joint* mFLJoint;
	opal::Joint* mFRJoint;
	opal::GearedMotor* mFLMotor;
	opal::GearedMotor* mFRMotor;
};

#endif
