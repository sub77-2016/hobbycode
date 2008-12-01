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

#ifndef ROBOT_H
#define ROBOT_H

#include "../src/LearningEntity.h"
#include "../src/Car.h"

class SimulationEngine;

class Robot : public LearningEntity, public Car
{
public:
	Robot(SimulationEngine& engine, verve::real agentUpdateFrequency);

	~Robot();

	virtual void init(const std::string& blueprintFilename, 
		const std::string& chassisMaterial, opal::real scale=1, 
		const opal::Matrix44r& transform=opal::Matrix44r(), 
		opal::real enginePitchScale=1);

	virtual void randomizeState();

	std::string getCurrentActionName()const;

	/// A place to update any visualization components.  This should be 
	/// called externally once per visual frame.
	void updateVisuals(opal::real dt);

protected:
	virtual void createAgent();

	virtual void loadParts(const std::string& blueprintFilename, 
		const std::string& chassisMaterial, opal::real scale=1, 
		const opal::Matrix44r& transform=opal::Matrix44r());

	void setupVisuals();

	virtual void resetBody();

	virtual void updateObservation();

	virtual void updateEffectors();

	opal::real mMaxRangeFinderDistance;
	opal::real mMaxPerceivedVelocityX;
	opal::real mMaxPerceivedVelocityZ;
	//opal::real mMaxPerceivedAccelX;
	//opal::real mMaxPerceivedAccelZ;

	std::string mCurrentActionName;

	//opal::AccelerationSensor* mAccelSensor;
	std::vector<opal::RaycastSensor*> mRangeFinders;
};

#endif
