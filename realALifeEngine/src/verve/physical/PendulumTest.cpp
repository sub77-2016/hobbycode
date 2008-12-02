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

#include "PendulumTest.h"
#include "Pendulum.h"
#include "../base/AgentVisualDebugger.h"

#include <ogre/Ogre.h>

PendulumTest::PendulumTest(unsigned int numRuns, unsigned int numTrialsPerRun)
: LearningTest(numRuns, numTrialsPerRun)
{
	setTestName("Pendulum");
	mPhysicsStepSize = (verve::real)0.01;
	mCurrentTrialTime = 0;
	mTrialLength = 20;
	mAvgRewardPerStep = 0;
	mPendulum = NULL;
	mAgentDebugger = NULL;
}

PendulumTest::~PendulumTest()
{
	if (mPendulum)
	{
		delete mPendulum;
	}

	if (mAgentDebugger)
	{
		delete mAgentDebugger;
	}
}

void PendulumTest::startTest()
{
	SimulationEngine engine;
	if (!engine.init())
	{
		return;
	}

	// Get a pointer to the trial number overlay and make it visible.
	Ogre::OverlayManager::getSingleton().getByName("Verve/TrialNumber")->show();

	engine.setUpdateMode(SimulationEngine::SIMULATE_REAL_TIME_MULTIPLE, 1);
	engine.setCameraMoveSpeed(5);

	//// Set to capture frames at 29.97 fps.
	//engine.setUpdateMode(SIMULATE_CONSTANT_CHUNK, 0.0333667);

	engine.getSimulator()->setStepSize(mPhysicsStepSize);
	engine.getSimulator()->setGravity(opal::Vec3r(0, -9.81, 0));

	// Make sure we get notified at the end of each step.
	engine.getSimulator()->addPostStepEventHandler(this);

	// Create a static box for a ground plane.
	opal::Solid* boxSolid = engine.getSimulator()->createSolid();
	boxSolid->setStatic(true);
	boxSolid->setPosition(0, -1.5, 0);
	opal::BoxShapeData data;
	data.dimensions.set(6, 3, 6);
	boxSolid->addShape(data);
	engine.createPhysicalEntity("ground", "Plastic/Gray", boxSolid);

	DataFile dataFile(mNumTrialsPerRun);
	opal::Matrix44r initialTransform;
	initialTransform.translate(0, 1.2, 0);
	assert(NULL == mPendulum);
	mPendulum = new Pendulum(engine, initialTransform);

	mAgentDebugger = new AgentVisualDebugger(engine.getSceneManager());

	for (unsigned int run = 0; run < mNumRuns; ++run)
	{
		mPendulum->resetBodyAndCreateNewAgent();
		mAgentDebugger->setAgent(mPendulum->getAgent());

		for (unsigned int trial = 0; trial < mNumTrialsPerRun; ++trial)
		{
			updateOverlayData(trial);
			mPendulum->resetBodyAndSTM();
			mPendulum->randomizeState();
			mAvgRewardPerStep = 0;
			mCurrentTrialTime = 0;

			while (mCurrentTrialTime < mTrialLength)
			{
				Ogre::Real elapsedSimTime = 0;
				Ogre::Real elapsedRealTime = 0;
				engine.update(elapsedSimTime, elapsedRealTime);
				handleInput(elapsedRealTime, engine);
				if (engine.quitApp())
				{
					return;
				}

				mAgentDebugger->updateVisuals();
			}

			mAvgRewardPerStep = mAvgRewardPerStep * mPhysicsStepSize / 
				mCurrentTrialTime;
			dataFile.storeData("trial", trial, (float)trial);
			dataFile.storeData("avg reward per step", trial, 
				mAvgRewardPerStep);
			printTrialAndRunStatus(run, trial, mAvgRewardPerStep);

			//// Print value function data.
			//if (0 == run && 
			//	(trial == 0 || trial == 4 || trial == 19 || trial == 99))
			//{
			//	char fileStr[1024];
			//	sprintf(fileStr, "./results/pendulum-trial%d-value.dat", trial);
			//	mPendulum->getAgent()->saveValueData(400, fileStr);
			//	sprintf(fileStr, "./results/pendulum-trial%d-RBF.dat", trial);
			//	mPendulum->getAgent()->saveStateRBFData(fileStr);
			//}
		}

		std::cout << "Agent age = " << mPendulum->getAgent()->getAgeString() 
			<< std::endl;
	}

	dataFile.printToFile("./results/pendulum-performance.dat");
}

void PendulumTest::handlePostStepEvent()
{
	// If the trial is not finished, update the Pendulum.
	if (mCurrentTrialTime < mTrialLength)
	{
		verve::real reward = computeReward();
		mAvgRewardPerStep += reward;
		mPendulum->update(reward, mPhysicsStepSize);
		mCurrentTrialTime += mPhysicsStepSize;
	}
}

verve::real PendulumTest::computeReward()
{
	assert(mPendulum);
	opal::real absAngle = fabs(mPendulum->getAngleFromUp());
	verve::real reward = 0;

	//// Continuous reward signal.
	//reward = -verve::globals::scaleNegOneToOne(absAngle, 0, 180);

	// Discrete reward signal.  Reward the Agent if the pendulum is 
	// within pi/4 of vertical.
	if (absAngle < 45)
	{
		reward = 1;
	}
	else
	{
		reward = -1;
	}

	return reward;
}

void PendulumTest::updateOverlayData(unsigned int trialNum)
{
	Ogre::OverlayElement* trialElement = 
		Ogre::OverlayManager::getSingleton().getOverlayElement(
		"Verve/TrialNumber/Trial");

	trialElement->setCaption("Trial " + 
		Ogre::StringConverter::toString(trialNum));
}

void PendulumTest::handleInput(opal::real elapsedRealTime, 
	const SimulationEngine& engine)
{
	OIS::Keyboard* keyboard = engine.getKeyboard();

	// This variable can be used to keep keys from repeating too fast.
	static Ogre::Real toggleTimer = 0;
	if (toggleTimer >= 0)
	{
		toggleTimer -= elapsedRealTime;
	}

	// Toggle GUI.
	if (keyboard->isKeyDown(OIS::KC_G) && toggleTimer <= 0)
	{
		Ogre::Overlay* debugOverlay = Ogre::OverlayManager::getSingleton().
			getByName("Verve/TrialNumber");

		if (debugOverlay->isVisible())
        {
			debugOverlay->hide();
			mAgentDebugger->setDisplayEnabled(false);
		}
		else
		{
			debugOverlay->show();
			mAgentDebugger->setDisplayEnabled(true);
		}

		toggleTimer = 0.5;
	}
}
