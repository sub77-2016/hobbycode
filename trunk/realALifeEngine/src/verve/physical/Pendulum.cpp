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

#include "Pendulum.h"
#include "../src/SimulationEngine.h"

Pendulum::Pendulum(SimulationEngine& engine, const opal::Matrix44r& transform)
: LearningEntity(10),
mMaxAngle(180),
mMaxVel(500),
mMaxTorque(2)
{
	createAgent();
	opal::Simulator* sim = engine.getSimulator();

	// Create the pendulum Solid.  We use radius, length, and density 
	// values that produce a pendulum mass of ~1.
	mLength = 1;
	mRadius = (opal::real)0.05;
	mPendulumSolid = sim->createSolid();
	mPendulumSolid->setSleepiness(0);

	// Make the pendulum "point" away from the joint anchor.
	opal::Matrix44r pendulumTransform = transform;
	pendulumTransform.rotate(-90, 1, 0, 0);
	mPendulumSolid->setTransform(pendulumTransform);
	opal::CapsuleShapeData capsule;
	capsule.length = mLength;
	capsule.radius = mRadius;
	capsule.material.density = 127;
	mPendulumSolid->addShape(capsule);
	engine.createPhysicalEntity("pole", "Plastic/Green", mPendulumSolid);

	//// TESTING
	//mPendulumSolid->setLinearDamping(0.1);

	// Setup the Joint that connects the pendulum to the static 
	// environment.
	mHinge = sim->createJoint();
	opal::JointAxis axis;
	axis.direction.set(0, 0, 1);
	axis.limitsEnabled = false;
	opal::JointData jointData;
	jointData.setType(opal::HINGE_JOINT);
	jointData.contactsEnabled = false;
	jointData.solid0 = mPendulumSolid;
	jointData.solid1 = NULL;
	opal::Point3r anchor = transform.getPosition();
	anchor[1] += (opal::real)0.5 * mLength;
	jointData.anchor = anchor;
	jointData.axis[0] = axis;
	mHinge->init(jointData);

	// Save the initial Solid configurations for resetting.
	mInitSolidData = mPendulumSolid->getData();
}

Pendulum::~Pendulum()
{
}

void Pendulum::createAgent()
{
	if (mAgent)
	{
		mAgent->destroy();
	}

	verve::AgentDescriptor desc;
	desc.setArchitecture(verve::RL);
	desc.setDynamicRBFEnabled(true);
	desc.addContinuousSensor(true);
	desc.addContinuousSensor();
	desc.setContinuousSensorResolution(16);
	desc.setNumOutputs(3);
	//desc.setMaxNumPlanningSteps(20);
	//desc.setPlanningUncertaintyThreshold((verve::real)0.01);
	mAgent = verve::createAgent(desc);
	mAgent->setTDLearningRate((verve::real)0.01, 2);
	//mAgent->setModelLearningRate((verve::real)0.001);
	//mAgent->setETraceTimeConstant((verve::real)0.5);
	//mAgent->setTDDiscountTimeConstant((verve::real)1.0);
	mObservation.init(*mAgent);
}

void Pendulum::resetBody()
{
	// Reset to the initial configuration.
	mPendulumSolid->setGlobalLinearVel(opal::Vec3r(0, 0, 0));
	mPendulumSolid->setGlobalAngularVel(opal::Vec3r(0, 0, 0));
	mPendulumSolid->zeroForces();
	mPendulumSolid->setTransform(mInitSolidData.transform);
}

void Pendulum::randomizeState()
{
	// Setup a random angle and angular velocity.

	opal::Matrix44r pendulumTransform = mInitSolidData.transform;

	// Translate to the Joint position (i.e. along the pendulum's +Z 
	// axis).
	pendulumTransform.translate(0, 0, (verve::real)0.5 * mLength);

	// Rotate to a random angle.
	pendulumTransform.rotate(verve::globals::randomRealUniform(0, 360), 
		0, 1, 0);

	// Translate back from the Joint position (i.e. along the 
	// pendulum's -Z axis).
	pendulumTransform.translate(0, 0, (verve::real)-0.5 * mLength);
	mPendulumSolid->setTransform(pendulumTransform);

	opal::Vec3r vel(0, 0, verve::globals::randomRealUniform(-mMaxVel, 
		mMaxVel));
	mPendulumSolid->setGlobalAngularVel(vel);
}

opal::real Pendulum::getAngleFromUp()const
{
	// Calculate the angle between the pendulum's forward vector and the 
	// global up vector.  The pendulum's forward vector is initially 
	// aligned along -Z before any rotations.
	opal::Vec3r pendForward = mPendulumSolid->getTransform().getForward();
	opal::real poleAngle = opal::angleBetween(pendForward, 
		opal::Vec3r(0, 1, 0));

	// Distinguish between positive and negative angles using the cross 
	// product.
	if (opal::cross(pendForward, opal::Vec3r(0, 1, 0))[2] < 0)
	{
		poleAngle *= -1;
	}

	return poleAngle;
}

//opal::real Pendulum::getAngleFromRight()const
//{
//	// Calculate the angle between the pole's up vector and the global 
//	// right vector.  The pole's "up" vector is its initial -Z axis, 
//	// a.k.a. its "forward" vector.
//	opal::Vec3r poleUp = mPendulumSolid->getTransform().getForward();
//	opal::real poleAngle = opal::angleBetween(poleUp, 
//		opal::Vec3r(1, 0, 0));

//	if (opal::cross(poleUp, opal::Vec3r(1, 0, 0))[2] < 0)
//	{
//		poleAngle *= -1;
//	}

//	return poleAngle;
//}

//opal::real Pendulum::getAngularVelocity()const
//{
//	return mPendulumSolid->getLocalAngularVel()[1];
//}

void Pendulum::updateObservation()
{
	// Setup sensory inputs.  Scale them all to be between 0 and 1.

	opal::real value = 0;

	// METHOD 1
	// Pole angle as one input.
	value = verve::globals::scaleNegOneToOne(getAngleFromUp(), 
		-mMaxAngle, mMaxAngle);
	mObservation.setContinuousValue(0, value);

	// Pole angular velocity (i.e. the angular velocity around the 
	// pole's initial Y axis) as one input.
	opal::real poleVel = mPendulumSolid->getLocalAngularVel()[1];
	value = verve::globals::scaleNegOneToOne(poleVel, -mMaxVel, 
		mMaxVel);
	mObservation.setContinuousValue(1, value);

	//// METHOD 2
	//// Pole angle, separated into two inputs.
	//value = getAngle();
	//if (value < 0)
	//{
	//	value = verve::globals::scaleZeroToOne(abs(getAngle()), 0, 
	//		mMaxAngle);
	//	mInputs[0] = value;
	//	mInputs[1] = 0;
	//}
	//else
	//{
	//	value = verve::globals::scaleZeroToOne(getAngle(), 0, mMaxAngle);
	//	mInputs[0] = 0;
	//	mInputs[1] = value;
	//}

	//// Pole angular velocity (i.e. the angular velocity around the 
	//// pole's initial Y axis), separated into two inputs.
	//value = -mPendulumSolid->getLocalAngularVel()[1];
	//if (value < 0)
	//{
	//	value = verve::globals::scaleZeroToOne(abs(value), 0, mMaxVel);
	//	mInputs[2] = value;
	//	mInputs[3] = 0;
	//}
	//else
	//{
	//	value = verve::globals::scaleZeroToOne(value, 0, mMaxVel);
	//	mInputs[2] = 0;
	//	mInputs[3] = value;
	//}

	//// METHOD 3
	//// Pole angle, separated into four inputs.  Using sine and cosine 
	//// as suggested in Remi Coulom's thesis.
	//opal::real angle = opal::degToRad(getAngle());
	//value = sin(angle);
	//if (value < 0)
	//{
	//	mInputs[0] = -value;
	//	mInputs[1] = 0;
	//}
	//else
	//{
	//	mInputs[0] = 0;
	//	mInputs[1] = value;
	//}

	//value = cos(angle);
	//if (value < 0)
	//{
	//	mInputs[2] = -value;
	//	mInputs[3] = 0;
	//}
	//else
	//{
	//	mInputs[2] = 0;
	//	mInputs[3] = value;
	//}

	//// Pole angular velocity (i.e. the angular velocity around the 
	//// pole's initial Y axis), separated into two inputs.
	//value = -mPendulumSolid->getLocalAngularVel()[1];
	//if (value < 0)
	//{
	//	value = verve::globals::scaleZeroToOne(abs(value), 0, mMaxVel);
	//	mInputs[4] = value;
	//	mInputs[5] = 0;
	//}
	//else
	//{
	//	value = verve::globals::scaleZeroToOne(value, 0, mMaxVel);
	//	mInputs[4] = 0;
	//	mInputs[5] = value;
	//}

	// This is just temporary to allow the inputs to be accessed 
	// from outside the Pendulum (for debugging).
	//mAgent->setContinuousValue(0, mInputs[0]);
	//mAgent->setContinuousValue(1, mInputs[1]);
	//mAgent->setInput(2, mInputs[2]);
	//mAgent->setInput(3, mInputs[3]);
	//mAgent->setInput(4, mInputs[4]);
	//mAgent->setInput(5, mInputs[5]);
}

void Pendulum::updateEffectors()
{
	opal::real force = 0;

	//// METHOD 1: using a single output Neuron with a real valued output
	//// Get the Agent's outputs.  Scale the output (which is between 0 
	//// and 1) to the range -1 to 1.
	//verve::real output = mAgent->getOutput(0);
	//force = verve::globals::scaleNegOneToOne(output, 
	//	0, 1);

	//// METHOD 2: using two output Neurons with real valued outputs
	//// Get the Agent's outputs (which are already between 0 and 1).  
	//// The two outputs act like two opposing muscles, one pushing 
	//// the opposite direction from the other.  This combined force 
	//// will range from -1 to 1.
	//force = mAgent->getOutput(0) - mAgent->getOutput(1);

	// METHOD 3: using three output Neurons with "winner-take-all" 
	// outputs
	// Get the Agent's outputs (which are already between 0 and 1).  
	//// TESTING BRAKES
	//mPendulumSolid->setAngularDamping(opal::defaults::solid::angularDamping);
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
		//case 3:
		//	// TESTING BRAKES
		//	mPendulumSolid->setAngularDamping(150);
		//	break;
		default:
			assert(false);
	}

	// Apply torque to the pendulum.  Scale the output by some constant 
	// to adjust its power.
	opal::Force f;
	f.type = opal::LOCAL_TORQUE;
	f.singleStep = true;
	f.vec.set(0, force * mMaxTorque, 0);
	mPendulumSolid->addForce(f);
}
