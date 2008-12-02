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

#ifndef CART_POLE_H
#define CART_POLE_H

#include <opal/opal.h>
#include "../src/LearningEntity.h"

class SimulationEngine;

class CartPole : public LearningEntity
{
public:
	CartPole(SimulationEngine& engine, 
		const opal::Matrix44r& transform=opal::Matrix44r());

	~CartPole();

	virtual void randomizeState();

	/// Returns the x position of the cart.
	opal::real getCartPosition()const;

	/// Returns the current angle between the pole and its initial 
	/// orientation.  This value will range from -180 to 180 degrees.
	opal::real getPoleAngle()const;

	opal::real getMaxCartPos()const;

	opal::real getMaxPoleAngle()const;

	//opal::real getCartLength()const;

private:
	virtual void createAgent();

	virtual void resetBody();

	virtual void updateObservation();

	virtual void updateEffectors();

	// Constants.
	const opal::real mMaxCartPos;
	const opal::real mMaxCartVel;
	const opal::real mMaxPoleAngle;
	const opal::real mMaxPoleVel;

	const opal::real mMaxForce;

	opal::Solid* mCart;
	opal::Solid* mPole;
	opal::Joint* mHinge;

	opal::real mPoleLength;
	opal::real mPoleRadius;
	opal::Vec3r mCartDimensions;
	opal::SolidData mInitCartData;
	opal::SolidData mInitPoleData;
};

#endif
