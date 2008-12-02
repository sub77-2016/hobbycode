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

#ifndef BIPED_H
#define BIPED_H

#include "../../src/Base3DObject.h"

class Biped : public Base3DObject
{

public:
	Biped( opal::Simulator* sim, opal::real size, 
		const opal::Matrix44r& transform=opal::Matrix44r() );

	~Biped();

	virtual void Draw();

	virtual void Update( opal::real dt );

	const opal::Matrix44r& getTransform();

	opal::Solid* getSolid()const;

	verve::NeuralNet* getNetwork()const;

	void moveLeg( bool left );

private:
	opal::Simulator* mSim;

	opal::Solid* mPelvis;
	opal::Solid* mLeftUpperLeg;
	opal::Solid* mRightUpperLeg;
	opal::Solid* mLeftLowerLeg;
	opal::Solid* mRightLowerLeg;
	opal::Solid* mLeftFoot;
	opal::Solid* mRightFoot;

	opal::HingeJoint* mLeftHip;
	opal::HingeJoint* mRightHip;
	opal::HingeJoint* mLeftKnee;
	opal::HingeJoint* mRightKnee;
	opal::HingeJoint* mLeftAnkle;
	opal::HingeJoint* mRightAnkle;

	opal::Vec3r mPelvisDimensions;
	opal::real mUpperLegLength;
	opal::real mUpperLegRadius;
	opal::real mLowerLegLength;
	opal::real mLowerLegRadius;
	opal::Vec3r mFootDimensions;

	opal::ServoMotor* mLeftHipMotor;
	opal::ServoMotor* mRightHipMotor;
	opal::ServoMotor* mLeftAnkleMotor;
	opal::ServoMotor* mRightAnkleMotor;

	verve::NeuralNet* mNetwork;
};

#endif
