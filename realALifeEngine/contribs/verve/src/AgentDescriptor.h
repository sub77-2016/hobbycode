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

#ifndef VERVE_AGENT_DESCRIPTOR_H
#define VERVE_AGENT_DESCRIPTOR_H

#include "Globals.h"

namespace verve
{
	/// A data structure describing a continuous sensor.
	struct ContinuousSensorDescriptor
	{
		ContinuousSensorDescriptor()
		{
			isCircular = false;
		}

		/// Determines whether the input range is 'circular' (i.e. it's 
		/// value can wrap from -1 to +1).  This is useful, for 
		/// example, when an input represents an angle that can jump 
		/// from -1 to +1.  However, note that circular inputs cannot 
		/// be processed through principal components analysis, so 
		/// having many of them will increase the state space faster.
		bool isCircular;
	};

	/// A data structure describing a discrete sensor.
	struct DiscreteSensorDescriptor
	{
		DiscreteSensorDescriptor()
		{
			numOptions = 0;
		}

		/// Determines the number of distinct values the sensor 
		/// can represent.  Note that this can be used as a binary 
		/// input value if 'numOptions' = 2.
		unsigned int numOptions;
	};

	/// A data structure used for Agent creation.  It contains data that 
	/// must be specified before the Agent is created.
	class AgentDescriptor
	{
	public:
		VERVE_DECL AgentDescriptor();

		VERVE_DECL AgentDescriptor(const AgentDescriptor& desc);
		
		VERVE_DECL virtual ~AgentDescriptor();

		/// Adds a new discrete sensor.  It is the user's 
		/// responsibility to keep track of sensor indices for 
		/// later access (e.g. the first discrete sensor 
		/// added uses the index 0); indices are separate for continuous 
		/// and discrete sensors, however.  The 'numOptions' 
		/// specifies how many possible values this discrete sensor 
		/// can represent.  This can be used as a binary sensor if 
		/// 'numOptions' = 2.  'numOptions' must be at least 1.
		VERVE_DECL virtual void VERVE_CALL addDiscreteSensor(
			unsigned int numOptions);

		/// Returns the number of discrete sensors.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getNumDiscreteSensors()const;

		/// Returns the number of options for the discrete sensor at the 
		/// given index.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getDiscreteSensorNumOptions(unsigned int i)const;

		/// Adds a new continuous sensor.  It is the user's 
		/// responsibility to keep track of sensor indices for later 
		/// access (e.g. the first continuous sensor 
		/// added uses the index 0); indices are separate for continuous 
		/// and discrete sensors, however.  'isCircular' determines whether 
		/// the input range is 'circular' (i.e. it's value can wrap from 
		/// -1 to +1).  This is useful, for example, when an input 
		/// represents an angle that can jump from -1 to +1.
		VERVE_DECL virtual void VERVE_CALL addContinuousSensor(
			bool isCircular = false);

		/// Returns the number of continuous sensors.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getNumContinuousSensors()const;

		/// Returns whether the continuous sensor at the given index 
		/// represents a circular data range.
		VERVE_DECL virtual bool VERVE_CALL 
			getContinuousSensorIsCircular(unsigned int i)const;

		/// Sets the number of outputs the Agent should have.
		VERVE_DECL virtual void VERVE_CALL setNumOutputs(
			unsigned int numOutputs);

		/// Returns the number of outputs the Agent will have.
		VERVE_DECL virtual unsigned int VERVE_CALL getNumOutputs()const;

		/// Sets the resolution of the continuous sensors.  This number 
		/// represents how many radial basis functions are used across 
		/// each dimension.  This must be at least 1.
		VERVE_DECL virtual void VERVE_CALL setContinuousSensorResolution(
			unsigned int resolution);

		/// Returns the resolution of the continuous sensors.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getContinuousSensorResolution()const;

		/// Sets whether the state space is initially filled with 
		/// RBFs or if we start with none and add them dynamically as 
		/// necessary.  This is a highly recommended feature.
		VERVE_DECL virtual void VERVE_CALL setDynamicRBFEnabled(
			bool enabled);

		/// Returns whether the dynamic RBF feature is enabled.
		VERVE_DECL virtual bool VERVE_CALL isDynamicRBFEnabled()const;

		/// Sets the overall Agent architecture to use.
		VERVE_DECL virtual void VERVE_CALL setArchitecture(
			AgentArchitecture arch);

		/// Returns the overall Agent architecture being used.
		VERVE_DECL virtual AgentArchitecture VERVE_CALL getArchitecture()const;

		/// Sets the maximum number of steps to take during a planning 
		/// sequence.
		VERVE_DECL virtual void VERVE_CALL setMaxNumPlanningSteps(
			unsigned int steps);

		/// Returns the maximum number of steps to take during a planning 
		/// sequence.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getMaxNumPlanningSteps()const;

		/// Sets the maximum amount of estimated uncertainty to tolerate 
		/// before ending a planning sequence.
		VERVE_DECL virtual void VERVE_CALL setPlanningUncertaintyThreshold(
			real threshold);

		/// Returns the maximum amount of estimated uncertainty to tolerate 
		/// before ending a planning sequence.
		VERVE_DECL virtual real VERVE_CALL 
			getPlanningUncertaintyThreshold()const;

	private:
		/// A list of the discrete sensor descriptors.
		std::vector<DiscreteSensorDescriptor> mDiscreteSensors;

		/// A list of the continuous sensor descriptors.
		std::vector<ContinuousSensorDescriptor> mContinuousSensors;

		/// The number of outputs the Agent should have.
		unsigned int mNumOutputs;

		/// Determines the resolution of the continuous sensors.
		unsigned int mContinuousSensorResolution;

		/// Determines whether the dynamic RBF feature is enabled.
		bool mDynamicRBFEnabled;

		/// The overall Agent architecture to use.
		AgentArchitecture mAgentArchitecture;

		/// The maximum number of steps to take during a planning sequence.
		unsigned int mMaxNumPlanningSteps;

		/// The maximum amount of estimated uncertainty to tolerate before 
		/// ending a planning sequence.
		real mPlanningUncertaintyThreshold;
	};
}

#endif
