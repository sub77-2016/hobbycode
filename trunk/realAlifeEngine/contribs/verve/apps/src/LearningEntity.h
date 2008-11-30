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

#ifndef VERVE_SAMPLES_LEARNING_ENTITY_H
#define VERVE_SAMPLES_LEARNING_ENTITY_H

#include <verve/verve.h>

/// A base class for an entity that combines a Verve Agent and a 
/// system controlled by the Agent (e.g. a physically simulated 
/// creature).
class LearningEntity
{
public:
	LearningEntity(verve::real agentUpdateFrequency)
	{
		assert(agentUpdateFrequency > 0);
		mAgent = NULL;
		mActionIndex = -1;
		mAgentUpdateTimeBuffer = 0;
		mAgentUpdatePeriod = 1 / agentUpdateFrequency;
	}

	virtual ~LearningEntity()
	{
		if (mAgent)
		{
			mAgent->destroy();
			mAgent = NULL;
		}
	}

	/// Resets the body and the Agent's temporary parameters.
	void resetBodyAndSTM()
	{
		resetBody();
		mAgent->resetShortTermMemory();
		mActionIndex = -1;
		mAgentUpdateTimeBuffer = 0;
	}

	/// Resets the body and creates a totally new Agent.
	void resetBodyAndCreateNewAgent()
	{
		resetBody();
		createAgent();
		mActionIndex = -1;
		mAgentUpdateTimeBuffer = 0;
	}

	/// Puts the body into an initial random state.
	virtual void randomizeState()
	{
	}

	/// Updates the Verve Agent's sensors, steps it ahead by one time 
	/// step (i.e. allowing it to learn from its reinforcement), and 
	/// updates the LearningAgent's effectors.  The given dt should 
	/// be less than the Agent's update period; otherwise, the 
	/// Agent will be updated multiple times without any changes 
	/// in the environment.
	void update(verve::real reward, verve::real dt)
	{
		assert(dt < mAgentUpdatePeriod);

		// We only update the sensors and the Agent itself periodically.  
		// Other things (like the effectors and any visualization) we 
		// update constantly.

		mAgentUpdateTimeBuffer += dt;
		if (mAgentUpdateTimeBuffer >= mAgentUpdatePeriod)
		{
			// Update sensory input.
			updateObservation();

			// Update the Agent by simulating it for the given dt.  This 
			// allows it to choose new actions and learn (if learning is 
			// enabled).
			if (mAgent)
			{
				mActionIndex = mAgent->update(reward, mObservation, 
					mAgentUpdatePeriod);
			}

			mAgentUpdateTimeBuffer -= mAgentUpdatePeriod;
		}
		else if (-1 == mActionIndex)
		{
			// This is the first update.  Go ahead and update the Agent, 
			// even if not enough time has elapsed.

			// Update sensory input.
			updateObservation();

			// Update the Agent by simulating it for the given dt.  This 
			// allows it to choose new actions and learn (if learning is 
			// enabled).
			if (mAgent)
			{
				mActionIndex = mAgent->update(reward, mObservation, 
					mAgentUpdatePeriod);
			}

			mAgentUpdateTimeBuffer = 0;
		}

		// Use outputs to affect the controlled system.
		updateEffectors();
	}

	/// Returns a pointer to the Verve Agent.
	verve::Agent* getAgent()const
	{
		return mAgent;
	}

	/// Returns a pointer to the LearningEntity's current input values.
	const verve::real* getContinuousInputs()const
	{
		return mObservation.getContinuousInputData();
	}

protected:
	/// Resets the physical parts to their initial configuration.
	virtual void resetBody() = 0;

	/// Creates the internal Verve Agent.
	virtual void createAgent() = 0;

	/// Updates the Verve Agent's Observation with data from the physical 
	/// environment.
	virtual void updateObservation() = 0;

	/// Updates the LearningEntity's physical motor outputs from the 
	/// Verve Agent's outputs.
	virtual void updateEffectors() = 0;

	/// Pointer to the Verve Agent used to control this LearningEntity.
	verve::Agent* mAgent;

	/// The verve Observation used to supply sensory inputs.
	verve::Observation mObservation;

	/// The latest action index;
	int mActionIndex;

	/// Used to determine when the Agent needs to be udpated.
	verve::real mAgentUpdateTimeBuffer;

	/// Determines how often to update the Agent.
	verve::real mAgentUpdatePeriod;
};

#endif
