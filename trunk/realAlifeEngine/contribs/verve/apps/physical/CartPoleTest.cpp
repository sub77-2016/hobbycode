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

#include "CartPoleTest.h"
#include "CartPole.h"
#include "../src/AgentVisualDebugger.h"

#include <ogre/Ogre.h>

CartPoleTest::CartPoleTest(unsigned int numRuns, unsigned int numTrialsPerRun)
: LearningTest(numRuns, numTrialsPerRun)
{
	setTestName("Cart-Pole");
	mPhysicsStepSize = (verve::real)0.01;
	mCurrentTrialTime = 0;
	mTrialFinished = false;
	mTaskSolved = false;
	mCartPole = NULL;
	//mTrialOverlay = NULL;
	mAgentDebugger = NULL;
}

CartPoleTest::~CartPoleTest()
{
	if (mCartPole)
	{
		delete mCartPole;
	}

	if (mAgentDebugger)
	{
		delete mAgentDebugger;
	}
}

void CartPoleTest::startTest()
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
	boxSolid->setPosition(0, -1, 0);
	opal::BoxShapeData data;
	data.dimensions.set(4.8, 2, 2);
	boxSolid->addShape(data);
	engine.createPhysicalEntity("ground", "Plastic/Gray", boxSolid);

	opal::Matrix44r initialTransform;
	initialTransform.translate(0, 0, 0);
	assert(NULL == mCartPole);
	mCartPole = new CartPole(engine, initialTransform);

	mAgentDebugger = new AgentVisualDebugger(engine.getSceneManager());

	for (unsigned int run = 0; run < mNumRuns; ++run)
	{
		// Use a new output file per run.
		DataFile dataFile(mNumTrialsPerRun);
		mTaskSolved = false;
		mCartPole->resetBodyAndCreateNewAgent();
		mAgentDebugger->setAgent(mCartPole->getAgent());

		for (unsigned int trial = 0; trial < mNumTrialsPerRun; ++trial)
		{
			updateOverlayData(trial);
			mCartPole->resetBodyAndSTM();
			mCartPole->randomizeState();
			mCurrentTrialTime = 0;
			mTrialFinished = false;

			while (!mTrialFinished)
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

				if (mTaskSolved)
				{
					std::cout << "Task solved!  Pole balanced for 30 minutes." 
						<< std::endl;
					break;
				}
			}

			dataFile.storeData("trial", trial, (float)trial);
			dataFile.storeData("trial length", trial, mCurrentTrialTime);
			printTrialAndRunStatus(run, trial, mCurrentTrialTime);

			if (mTaskSolved)
			{
				break;
			}
		}

		std::cout << "Agent age = " << mCartPole->getAgent()->getAgeString() 
			<< std::endl;

		char fileStr[1024];
		sprintf(fileStr, "./results/cartpole-run%d-performance.dat", run);
		dataFile.printToFile(fileStr);
	}
}

void CartPoleTest::handlePostStepEvent()
{
	assert(mCartPole);

	// If the trial is not finished, update the Pendulum.
	if (!mTrialFinished)
	{
		if (isOutOfBounds())
		{
			// The trial is now finished.  Punish the Agent for failing.
			mTrialFinished = true;
			mCartPole->update(-1, mPhysicsStepSize);
		}
		else
		{
			// The trial is not finished.  Reward the Agent for not 
			// failing.
			mCartPole->update(1, mPhysicsStepSize);
		}

		mCurrentTrialTime += mPhysicsStepSize;

		// If the cart-pole has been balancing for 30 minutes, stop the 
		// experiment early.
		if (mCurrentTrialTime >= 1800)
		{
			mTaskSolved = true;
		}
	}
}

bool CartPoleTest::isOutOfBounds()
{
	assert(mCartPole);

	//opal::real halfCartLength = (verve::real)0.5 * 
	//	mCartPole->getCartLength();
	opal::real cartPos = mCartPole->getCartPosition();
	opal::real maxCartPos = mCartPole->getMaxCartPos();

	//if (cartPos - halfCartLength < -maxCartPos 
	//	|| cartPos + halfCartLength > maxCartPos 
	//	|| fabs(mCartPole->getPoleAngle()) > mCartPole->getMaxPoleAngle())
	if (cartPos <= -maxCartPos 
		|| cartPos >= maxCartPos 
		|| fabs(mCartPole->getPoleAngle()) > mCartPole->getMaxPoleAngle())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CartPoleTest::updateOverlayData(unsigned int trialNum)
{
	Ogre::OverlayElement* trialElement = 
		Ogre::OverlayManager::getSingleton().getOverlayElement(
		"Verve/TrialNumber/Trial");

	trialElement->setCaption("Trial " + 
		Ogre::StringConverter::toString(trialNum));
}

void CartPoleTest::handleInput(opal::real elapsedRealTime, 
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
