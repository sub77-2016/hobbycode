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

#include "Biped.h"

Biped::Biped( opal::Simulator* sim, opal::real size, const opal::Matrix44r& transform )
: Base3DObject()
{
	mSim = sim;
	opal::Space* space = sim->createSpace();

	//calculate body dimensions
	mPelvisDimensions.set(0.3 * size, 0.2 * size, 0.2 * size);
	mUpperLegLength = 0.5 * size;
	mUpperLegRadius = 0.07 * size;
	mLowerLegLength = 0.5 * size;
	mLowerLegRadius = 0.07 * size;
	mFootDimensions.set(0.12 * size, 0.1 * size, 0.3 * size);
	
	mPelvis = sim->createSolid( false, transform, space );
	mPelvis->addBox( mPelvisDimensions );

	//add shapes and joints
	opal::Matrix44r M = transform;
	M.translate( -mPelvisDimensions[0]*0.5, -mUpperLegLength*0.5, 0.0 );
	M.rotate(90.0, 1.0, 0.0, 0.0);
	mLeftUpperLeg = sim->createSolid( false, M, space );
	mLeftUpperLeg->addCapsule( mUpperLegRadius, mUpperLegLength );
	M.rotate(-90.0, 1.0, 0.0, 0.0);
	M.translate( 0.0, mUpperLegLength*0.5, 0.0 );
	mLeftHip = sim->createHingeJoint( mPelvis, mLeftUpperLeg, M.getPosition(), opal::Vec3r(1,0,0) );
	mLeftHip->setLimits( -115.0, 15.0, 0 );

	M = transform;
	M.translate( mPelvisDimensions[0]*0.5, -mUpperLegLength*0.5, 0.0 );
	M.rotate(90.0, 1.0, 0.0, 0.0);
	mRightUpperLeg = sim->createSolid( false, M, space );
	mRightUpperLeg->addCapsule( mUpperLegRadius, mUpperLegLength );
	M.rotate(-90.0, 1.0, 0.0, 0.0);
	M.translate( 0.0, mUpperLegLength*0.5, 0.0 );
	mRightHip = sim->createHingeJoint( mPelvis, mRightUpperLeg, M.getPosition(), opal::Vec3r(1,0,0) );
	mRightHip->setLimits( -115.0, 15.0, 0 );

	M = transform;
	M.translate( -mPelvisDimensions[0]*0.5, -mUpperLegLength - mLowerLegLength*0.5, 0.0 );
	M.rotate(90.0, 1.0, 0.0, 0.0);
	mLeftLowerLeg = sim->createSolid( false, M, space );
	mLeftLowerLeg->addCapsule( mLowerLegRadius, mLowerLegLength );
	M.rotate(-90.0, 1.0, 0.0, 0.0);
	M.translate( 0.0, mLowerLegLength*0.5, 0.0 );
	mLeftKnee = sim->createHingeJoint( mLeftUpperLeg, mLeftLowerLeg, M.getPosition(), 
		opal::Vec3r(1,0,0) );
	mLeftKnee->setLimits(0.0, 100.0, 0 );

	M = transform;
	M.translate( mPelvisDimensions[0]*0.5, -mUpperLegLength - mLowerLegLength*0.5, 0.0 );
	M.rotate(90.0, 1.0, 0.0, 0.0);
	mRightLowerLeg = sim->createSolid( false, M, space );
	mRightLowerLeg->addCapsule( mLowerLegRadius, mLowerLegLength );
	M.rotate(-90.0, 1.0, 0.0, 0.0);
	M.translate( 0.0, mLowerLegLength*0.5, 0.0 );
	mRightKnee = sim->createHingeJoint( mRightUpperLeg, mRightLowerLeg, M.getPosition(), 
		opal::Vec3r(1,0,0) );
	mRightKnee->setLimits(0.0, 100.0, 0 );

	M = transform;
	M.translate( -mPelvisDimensions[0]*0.5, -mUpperLegLength - mLowerLegLength - mLowerLegRadius, 
		-mFootDimensions[2]*0.25 );
	mLeftFoot = sim->createSolid( false, M, space );
	mLeftFoot->addBox( mFootDimensions );
	M.translate( 0.0, 0.0, mFootDimensions[2]*0.25 );
	mLeftAnkle = sim->createHingeJoint( mLeftLowerLeg, mLeftFoot, M.getPosition(), 
		opal::Vec3r(1,0,0) );
	mLeftAnkle->setLimits(-45.0, 25.0, 0);

	M = transform;
	M.translate( mPelvisDimensions[0]*0.5, -mUpperLegLength - mLowerLegLength - mLowerLegRadius, 
		-mFootDimensions[2]*0.25 );
	mRightFoot = sim->createSolid( false, M, space );
	mRightFoot->addBox( mFootDimensions );
	M.translate( 0.0, 0.0, mFootDimensions[2]*0.25 );
	mRightAnkle = sim->createHingeJoint( mRightLowerLeg, mRightFoot, M.getPosition(), 
		opal::Vec3r(1,0,0) );
	mRightAnkle->setLimits(-45.0, 25.0, 0);

	mLeftHipMotor = sim->createServoMotor( mLeftHip, 0, opal::ANGULAR_POS_SERVO, 10.0 );
	mLeftHipMotor->setMaxForce( 1000.0 );
	mRightHipMotor = sim->createServoMotor( mRightHip, 0, opal::ANGULAR_POS_SERVO, 10.0 );
	mRightHipMotor->setMaxForce( 1000.0 );
	mLeftAnkleMotor = sim->createServoMotor( mLeftAnkle, 0, opal::ANGULAR_POS_SERVO, 10.0 );
	mLeftAnkleMotor->setMaxForce( 15.0 );
	mLeftAnkleMotor->setDesiredPos( 0.0 );
	mRightAnkleMotor = sim->createServoMotor( mRightAnkle, 0, opal::ANGULAR_POS_SERVO, 10.0 );
	mRightAnkleMotor->setMaxForce( 15.0 );
	mRightAnkleMotor->setDesiredPos( 0.0 );

	mNetwork = new verve::NeuralNet( 1, 5, 2 );
}

Biped::~Biped()
{
	delete mNetwork;
}

void Biped::moveLeg( bool left )
{
	if (left)
	{
		mLeftHipMotor->setDesiredPos( 13.0 );
		mRightHipMotor->setDesiredPos( -40.0 );
	}
	else
	{
		mLeftHipMotor->setDesiredPos( -40.0 );
		mRightHipMotor->setDesiredPos( 13.0 );
	}
}

void Biped::Update( opal::real dt )
{
	opal::real inputs[1];
	opal::Vec3r angVel = mPelvis->getLocalAngularVel();
	inputs[0] = angVel[0];
	mNetwork->updateInputs( inputs );
	mNetwork->activate( dt );
	//mNetwork->learning();
	const opal::real* outputs = mNetwork->getOutputs();
	mLeftHipMotor->setDesiredPosNorm( outputs[0] );
	mRightHipMotor->setDesiredPosNorm( outputs[1] );
}

void Biped::Draw()
{
	float matAmbientAndDiffuseRed[4] = {.6, .1, .1, 1};
	float matAmbientAndDiffuseBlue[4] = {.3, .3, .9, 1};

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbientAndDiffuseRed);

	glPushMatrix();
		glMultMatrixf(mPelvis->getTransform().getData());
		DrawBox( mPelvisDimensions );
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(mLeftUpperLeg->getTransform().getData());
		DrawCylinder( mUpperLegLength, mUpperLegRadius );
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(mRightUpperLeg->getTransform().getData());
		DrawCylinder( mUpperLegLength, mUpperLegRadius );
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(mLeftLowerLeg->getTransform().getData());
		DrawCylinder( mLowerLegLength, mLowerLegRadius );
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(mRightLowerLeg->getTransform().getData());
		DrawCylinder( mLowerLegLength, mLowerLegRadius );
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(mLeftFoot->getTransform().getData());
		DrawBox( mFootDimensions );
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(mRightFoot->getTransform().getData());
		DrawBox( mFootDimensions );
	glPopMatrix();
}

const opal::Matrix44r& Biped::getTransform()
{
	return mPelvis->getTransform();
}

opal::Solid* Biped::getSolid()const
{
	return mPelvis;
}

verve::NeuralNet* Biped::getNetwork()const
{
	return mNetwork;
}
