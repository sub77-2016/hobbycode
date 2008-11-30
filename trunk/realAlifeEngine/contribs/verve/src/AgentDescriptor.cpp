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

#include "AgentDescriptor.h"

namespace verve
{
	AgentDescriptor::AgentDescriptor()
	{
		mNumOutputs = 0;
		mContinuousSensorResolution = 10;
		mDynamicRBFEnabled = true;
		mAgentArchitecture = defaults::agentArchitecture;
		mMaxNumPlanningSteps = defaults::maxNumPlanningSteps;
		mPlanningUncertaintyThreshold = 
			defaults::planningUncertaintyThreshold;
	}

	AgentDescriptor::AgentDescriptor(const AgentDescriptor& desc)
	{
		// Copy discrete sensors.
		unsigned int numDiscreteSensors = desc.getNumDiscreteSensors();
		for (unsigned int i = 0; i < numDiscreteSensors; ++i)
		{
			DiscreteSensorDescriptor sensorDesc;
			sensorDesc.numOptions = desc.getDiscreteSensorNumOptions(i);
			mDiscreteSensors.push_back(sensorDesc);
		}

		// Copy continuous sensors.
		unsigned int numContinuousSensors = desc.getNumContinuousSensors();
		for (unsigned int i = 0; i < numContinuousSensors; ++i)
		{
			ContinuousSensorDescriptor sensorDesc;
			sensorDesc.isCircular = desc.getContinuousSensorIsCircular(i);
			mContinuousSensors.push_back(sensorDesc);
		}

		mNumOutputs = desc.mNumOutputs;
		mContinuousSensorResolution = desc.mContinuousSensorResolution;
		mDynamicRBFEnabled = desc.mDynamicRBFEnabled;
		mAgentArchitecture = desc.mAgentArchitecture;
		mMaxNumPlanningSteps = desc.mMaxNumPlanningSteps;
		mPlanningUncertaintyThreshold = desc.mPlanningUncertaintyThreshold;
	}

	AgentDescriptor::~AgentDescriptor()
	{
		mDiscreteSensors.clear();
		mContinuousSensors.clear();
	}

	void AgentDescriptor::addDiscreteSensor(unsigned int numOptions)
	{
		assert(numOptions > 0);
		DiscreteSensorDescriptor desc;
		desc.numOptions = numOptions;
		mDiscreteSensors.push_back(desc);
	}

	unsigned int AgentDescriptor::getNumDiscreteSensors()const
	{
		return (unsigned int)mDiscreteSensors.size();
	}

	unsigned int AgentDescriptor::getDiscreteSensorNumOptions(
		unsigned int i)const
	{
		return mDiscreteSensors.at(i).numOptions;
	}

	void AgentDescriptor::addContinuousSensor(bool isCircular)
	{
		ContinuousSensorDescriptor desc;
		desc.isCircular = isCircular;
		mContinuousSensors.push_back(desc);
	}

	unsigned int AgentDescriptor::getNumContinuousSensors()const
	{
		return (unsigned int)mContinuousSensors.size();
	}

	bool AgentDescriptor::getContinuousSensorIsCircular(
		unsigned int i)const
	{
		return mContinuousSensors.at(i).isCircular;
	}

	void AgentDescriptor::setNumOutputs(unsigned int numOutputs)
	{
		mNumOutputs = numOutputs;
	}

	unsigned int AgentDescriptor::getNumOutputs()const
	{
		return mNumOutputs;
	}

	void AgentDescriptor::setContinuousSensorResolution(
		unsigned int resolution)
	{
		assert(resolution > 0);
		mContinuousSensorResolution = resolution;
	}

	unsigned int AgentDescriptor::getContinuousSensorResolution()const
	{
		return mContinuousSensorResolution;
	}

	void AgentDescriptor::setDynamicRBFEnabled(bool enabled)
	{
		mDynamicRBFEnabled = enabled;
	}

	bool  AgentDescriptor::isDynamicRBFEnabled()const
	{
		return mDynamicRBFEnabled;
	}

	void AgentDescriptor::setArchitecture(AgentArchitecture arch)
	{
		mAgentArchitecture = arch;
	}

	AgentArchitecture AgentDescriptor::getArchitecture()const
	{
		return mAgentArchitecture;
	}

	void AgentDescriptor::setMaxNumPlanningSteps(unsigned int steps)
	{
		mMaxNumPlanningSteps = steps;
	}

	unsigned int AgentDescriptor::getMaxNumPlanningSteps()const
	{
		return mMaxNumPlanningSteps;
	}

	void AgentDescriptor::setPlanningUncertaintyThreshold(real threshold)
	{
		mPlanningUncertaintyThreshold = threshold;
	}

	real AgentDescriptor::getPlanningUncertaintyThreshold()const
	{
		return mPlanningUncertaintyThreshold;
	}
}
