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

#ifndef VERVE_DEFINES_H
#define VERVE_DEFINES_H

#include <iostream>
#include <vector>
#include <assert.h>
#include <math.h>

#include "Platform.h"
#include "Logger.h"

namespace verve
{
	#ifdef VERVE_USE_DOUBLE
		typedef double real;
	#else
		typedef float real;
	#endif

	const real VERVE_E = (real)2.71828182845904523536;

	/// Various Agent architectures.
	enum AgentArchitecture
	{
		/// Reinforcement learning only.
		RL,

		/// Reinforcement learning with a learned predictive environment 
		/// model.
		MODEL_RL,

		/// Reinforcement learning, a predictive model, and a curiosity 
		/// component that drives the Agent to experience new situations 
		/// (hopefully improving its predictive model faster).
		CURIOUS_MODEL_RL
	};

	/// Contains all default values used to initialize variables.
	namespace defaults
	{
		/// Update step size.
		const real stepSize = (real)0.1;

		// --------------------------------------------------------
		// TIME CONSTANT NOTES
		// 
		// These notes discuss the basics of time constants used in all 
		// kinds of dynamic systems.  The time constant is the amount of 
		// time it takes for the output to decay to 37% of its initial value.
		// 
		// Here, let 'T' represent a time constant.
		// 
		// System response with zero input:
		// x(t) = e^(-t/T) * x(0)
		// The discrete time equation is:
		// x(t + dt) = e^(-dt/T) * x(t)
		// 
		// System response with a constant input:
		// x(t) = e^(-t/T) * x(0) + (1 - e^(-t/T)) * h
		// where h is the resting level/equilibrium point/constant input 
		// value.  The discrete time equation is: 
		// x(t + dt) = e^(-dt/T) * x(t) + (1 - e^(-dt/T) * h 
		// --------------------------------------------------------

		// OLD...
		// Learning rates should be much slower than Neuron firing rate 
		// decays (i.e. membrane time constants) to ensure a 
		// steady state condition.  See Porr and Worgotter, "Isotropic 
		// Sequence Order Learning."  Also, the average reinforcement 
		// rate should change much slower than the learning rate.  See 
		// Nathaniel Daw's PhD dissertation.  So the relative rate of 
		// change 
		// of these parameters is:
		// (slowest)
		// average reinforcement rate
		// learning rate
		// change in membrane potential
		// (fastest)

		// The membrane time constant is a measure of how fast a Neuron's 
		// membrane potential can change.  It ranges from 1-100 ms 
		// (0.001-0.1 s) in animals, according to Koch, 1998, "Biophysics 
		// of Computation."  Firing rates change at roughly the same rate 
		// as membrane potentials.

		/// Determines how fast eligibility traces change.  
		const real eTraceTimeConstant = (real)0.1;

		/// Determines how much future rewards are discounted.  For example, 
		/// a discount time constant of 1 means that rewards received 1 
		/// second in the future are worth only 37% of what they are worth 
		/// right now.
		const real TDDiscountTimeConstant = (real)1.0;

		/// Determines the learning rate for TDConnections in the value 
		/// functions.  This is how long it takes (in seconds) for errors 
		/// to be reduced to 37% of their initial values.
		const real valueFunctionLearningTimeConstant = (real)0.1;

		/// The policy's learning rate is a combination of this multiplier 
		/// and the value function's learning rate.
		const real policyLearningMultiplier = 5;

		/// Determines the learning rate for the predictive model.  This 
		/// is how long it takes (in seconds) for errors to be reduced to 
		/// 37% of their initial values.
		const real modelLearningTimeConstant = (real)0.001;

		/// TDConnections with eligibility traces below this threshold are 
		/// considered inactive, and their eligibility traces get set to 
		/// zero.
		const real activeETraceThreshold = (real)0.01;

		/// The minimum probability of choosing each action.  This should 
		/// be kept above zero to ensure that exploratory actions are never 
		/// totally ignored.  This value times the number of actions MUST 
		/// be < 1.
		const real minActionSelectionProb = (real)0.001;

		/// The default Agent architecture to use.
		const AgentArchitecture agentArchitecture = RL;

		/// The maximum number of steps to take during a planning sequence.
		const unsigned int maxNumPlanningSteps = 10;

		/// The maximum amount of estimated uncertainty to tolerate before 
		/// ending a planning sequence.
		const real planningUncertaintyThreshold = (real)0.1;
	}
}

#endif
