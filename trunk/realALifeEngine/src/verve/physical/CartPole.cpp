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

#include "CartPole.h"
#include "../src/SimulationEngine.h"

CartPole::CartPole(SimulationEngine& engine, const opal::Matrix44r& transform)
: LearningEntity(20),
mMaxCartPos(2.4f),
mMaxCartVel(2.4f),
mMaxPoleAngle(12),
mMaxPoleVel(100),
mMaxForce(10)
{
	opal::Simulator* sim = engine.getSimulator();

	// TESTING
	mAgentUpdatePeriod = 0.05f;

	// We use physical dimensions and density values that produce a cart 
	// mass of ~1, a pole mass of ~0.1, and a pole length of ~1.  This 
	// should make things roughly similar to Barto, Sutton, and Anderson's 
	// 1983 paper, "Neuronlike Adaptive Elements That Can Solve Difficult 
	// Learning Control Problems."

	// Calculate cart-pole dimensions.
	mCartDimensions.set(1, (opal::real)0.15, (opal::real)0.5);

	// Create the cart Solid.
	mCart = sim->createSolid();
	mCart->setSleepiness(0);
	opal::Matrix44r cartTransform = transform;
	cartTransform.translate(0, (opal::real)0.5 * mCartDimensions[1], 
		0);
	mCart->setTransform(cartTransform);
	opal::BoxShapeData box;
	box.dimensions = mCartDimensions;
	box.material.friction = 0;
	box.material.density = (opal::real)13.3;
	mCart->addShape(box);
	engine.createPhysicalEntity("cart", "Plastic/Green", mCart);

	// Create the pole Solid.
	mPole = sim->createSolid();
	mPole->setSleepiness(0);
	//mPole->setLinearDamping(0); // TESTING!
	//mPole->setAngularDamping(0); // TESTING!
	mPoleLength = 1;
	mPoleRadius = (opal::real)0.05;
	opal::Matrix44r poleTransform = transform;
	poleTransform.translate(0, mCartDimensions[1] + 
		(opal::real)0.5 * mPoleLength, 0);
	poleTransform.rotate(90, 1, 0, 0);
	mPole->setTransform(poleTransform);
	opal::CapsuleShapeData capsule;
	capsule.length = mPoleLength;
	capsule.radius = mPoleRadius;
	capsule.material.density = 12.75;
	mPole->addShape(capsule);
	engine.createPhysicalEntity("pole", "Plastic/Green", mPole);

	// Setup the Joint between the cart and pole.
	mHinge = sim->createJoint();
	opal::JointAxis axis;
	axis.direction.set(0, 0, 1);
	axis.limitsEnabled = true;
	axis.limits.low = -90;
	axis.limits.high = 90;
	opal::JointData jointData;
	jointData.setType(opal::HINGE_JOINT);
	jointData.contactsEnabled = false;
	jointData.solid0 = mCart;
	jointData.solid1 = mPole;
	opal::Point3r anchor = transform.getPosition();
	anchor[1] += mCartDimensions[1];
	jointData.anchor = anchor;
	jointData.axis[0] = axis;
	mHinge->init(jointData);

	// Save the initial Solid configurations for resetting.
	mInitCartData = mCart->getData();
	mInitPoleData = mPole->getData();
}

CartPole::~CartPole()
{
}

void CartPole::createAgent()
{
	if (mAgent)
	{
		mAgent->destroy();
	}

	verve::AgentDescriptor desc;
	desc.setArchitecture(verve::RL);
	desc.setDynamicRBFEnabled(true);
	desc.addContinuousSensor();
	desc.addContinuousSensor();
	desc.addContinuousSensor();
	desc.addContinuousSensor();
	desc.setContinuousSensorResolution(8);
	desc.setNumOutputs(3);
	mAgent = verve::createAgent(desc);
	mAgent->setTDLearningRate((verve::real)0.001, (verve::real)50);
	//mAgent->setETraceTimeConstant((verve::real)0.5);
	//mAgent->setTDDiscountTimeConstant((verve::real)5.0);
	mObservation.init(*mAgent);
}

void CartPole::resetBody()
{
	// Reset to the initial configuration.
	mCart->setGlobalLinearVel(opal::Vec3r(0, 0, 0));
	mCart->setGlobalAngularVel(opal::Vec3r(0, 0, 0));
	mCart->zeroForces();
	mPole->setGlobalLinearVel(opal::Vec3r(0, 0, 0));
	mPole->setGlobalAngularVel(opal::Vec3r(0, 0, 0));
	mPole->zeroForces();
	mCart->setTransform(mInitCartData.transform);
	mPole->setTransform(mInitPoleData.transform);
}

void CartPole::randomizeState()
{
	// Set the 4 state variables to random values selected 
	// uniformly within +/- 20% of their max values.
	opal::real multiplier = (opal::real)0.2;

	// Set random cart position.
	opal::Matrix44r cartTransform = mInitCartData.transform;
	opal::real cartOffset = verve::globals::randomRealUniform(
		-multiplier * mMaxCartPos, multiplier * mMaxCartPos);
	cartTransform.translate(cartOffset, 0, 0);
	mCart->setTransform(cartTransform);

	// Set random cart velocity.
	opal::Vec3r cartVel(verve::globals::randomRealUniform(
		-multiplier * mMaxCartVel, 
		multiplier * mMaxCartVel), 0, 0);
	mCart->setLocalLinearVel(cartVel);

	opal::Matrix44r poleTransform = mInitPoleData.transform;
	poleTransform.translate(cartOffset, 0, 0);

	// Translate to the Joint position (i.e. along the pole's +Z 
	// axis).
	poleTransform.translate(0, 0, (verve::real)0.5 * mPoleLength);

	// Rotate the pole to a random angle.
	poleTransform.rotate(verve::globals::randomRealUniform(
		-multiplier * mMaxPoleAngle, multiplier * mMaxPoleAngle), 
		0, 1, 0);

	// Translate back from the Joint position (i.e. along the 
	// pendulum's -Z axis).
	poleTransform.translate(0, 0, -(verve::real)0.5 * mPoleLength);
	mPole->setTransform(poleTransform);

	// Setup a random pole angular velocity.
	opal::Vec3r poleVel(0, 0, verve::globals::randomRealUniform(
		-multiplier * mMaxPoleVel, 
		multiplier * mMaxPoleVel));
	mPole->setGlobalAngularVel(poleVel);
}

opal::real CartPole::getMaxCartPos()const
{
	return mMaxCartPos;
}

opal::real CartPole::getMaxPoleAngle()const
{
	return mMaxPoleAngle;
}

//opal::real CartPole::getCartLength()const
//{
//	return mCartDimensions[0];
//}

opal::real CartPole::getCartPosition()const
{
	return mCart->getPosition()[0];
}

opal::real CartPole::getPoleAngle()const
{
	// TODO: use InclineSensor for this?

	// Calculate the angle between the pole and the global up vector.  
	// The pole's "up" vector is its initial -Z axis, a.k.a. its 
	// "forward" vector.
	opal::Vec3r poleUp = mPole->getTransform().getForward();
	opal::real poleAngle = opal::angleBetween(poleUp, 
		opal::Vec3r(0, 1, 0));

	if (opal::cross(poleUp, opal::Vec3r(0, 1, 0))[2] < 0)
	{
		poleAngle *= -1;
	}

	return poleAngle;
}

void CartPole::updateObservation()
{
	// Setup sensory inputs.  Scale them all to be between -1 and 1.

	opal::real value = 0;

	// Cart X position.
	value = verve::globals::scaleNegOneToOne(mCart->getPosition()[0], 
		-mMaxCartPos, mMaxCartPos);
	mObservation.setContinuousValue(0, value);

	// Cart X velocity.
	value = verve::globals::scaleNegOneToOne(mCart->getLocalLinearVel()[0], 
		-mMaxCartVel, mMaxCartVel);
	mObservation.setContinuousValue(1, value);

	// Pole angle.
	value = verve::globals::scaleNegOneToOne(getPoleAngle(), 
		-mMaxPoleAngle, mMaxPoleAngle);
	mObservation.setContinuousValue(2, value);

	// Pole angular velocity (i.e. the angular velocity around the 
	// pole's initial Y axis.
	value = verve::globals::scaleNegOneToOne(mPole->getLocalAngularVel()[1], 
		-mMaxPoleVel, mMaxPoleVel);
	mObservation.setContinuousValue(3, value);
}

void CartPole::updateEffectors()
{
	opal::real force = 0;

	switch(mActionIndex)
	{
		case 0:
			force = 1;
			break;
		case 1:
			force = 0;
			break;
		case 2:
			force = -1;
			break;
		default:
			assert(false);
	}

	// Apply forces to the cart.  Scale the output by some constant to 
	// give it more power.
	opal::Force f;
	f.type = opal::LOCAL_FORCE;
	f.singleStep = true;
	f.vec.set(force * mMaxForce, 0, 0);
	mCart->addForce(f);
}
