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

#ifndef PENDULUM_H
#define PENDULUM_H

#include <opal/opal.h>
#include "../base/LearningEntity.h"

class SimulationEngine;

class Pendulum : public LearningEntity
{
public:
	Pendulum(SimulationEngine& engine, 
		const opal::Matrix44r& transform=opal::Matrix44r());

	~Pendulum();

	virtual void randomizeState();

	/// Returns the current angle between the pendulum and the global 
	/// up direction.  This value will range from -180 to 180 degrees.
	opal::real getAngleFromUp()const;

	///// Gives the pendulum a random shove.  Useful when starting a new 
	///// training episode.
	//void shove();

	///// Returns the current angle between the pendulum and the global 
	///// right direction.  This value will range from -180 to 180 degrees.
	//opal::real getAngleFromRight()const;

	///// Returns the pendulum's angular velocity.
	//opal::real getAngularVelocity()const;

protected:
	virtual void createAgent();

	virtual void resetBody();

	virtual void updateObservation();

	virtual void updateEffectors();

	// Constants.
	const opal::real mMaxAngle;
	const opal::real mMaxVel;

	const opal::real mMaxTorque;

	opal::Solid* mPendulumSolid;
	opal::Joint* mHinge;

	opal::real mLength;
	opal::real mRadius;
	opal::SolidData mInitSolidData;
};

#endif
