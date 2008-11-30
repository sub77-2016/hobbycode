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

#include "TDConnection.h"
#include "Neuron.h"

namespace verve
{
	TDConnection::TDConnection(Neuron* preNeuron, Neuron* postNeuron, 
		TDConnectionType type)
	: Connection(preNeuron, postNeuron)
	{
		//mType = TD;
		mETrace = 0;
		mTDConnectionType = type;
		mETraceDecayFactor = 0;
		mTDDiscountFactor = 0;
		mIsInActiveList = false;
	}

	TDConnection::~TDConnection()
	{
	}

	void TDConnection::resetShortTermMemory()
	{
		Connection::resetShortTermMemory();
		mETrace = 0;
		mIsInActiveList = false;
	}

	void TDConnection::setETrace(real trace)
	{
		mETrace = trace;
	}

	real TDConnection::getETrace()const
	{
		return mETrace;
	}

	TDConnectionType TDConnection::getTDConnectionType()
	{
		return mTDConnectionType;
	}

	void TDConnection::setETraceDecayFactor(real value)
	{
		mETraceDecayFactor = value;
	}

	void TDConnection::setTDDiscountFactor(real value)
	{
		mTDDiscountFactor = value;
	}

	void TDConnection::increaseETrace()
	{
		// Here we use different methods to update different types of 
		// TDConnections' eligibility traces.  The authors use different 
		// eligibility trace update methods for the value function and 
		// policy in the following paper: 
		// Barto, Sutton, and Anderson, "Neuronlike Adaptive Elements That 
		// Can Solve Difficult Learning Control Problems," IEEE Trans. 
		// Syst., Man, Cybern., Vol. SMC-13, pp. 834--846, Sept.--Oct. 1983.

		real eligibility = 0;
		switch(mTDConnectionType)
		{
			case VALUE_FUNCTION_TDCONNECTION:
				/// Eligibility depends on the amount of pre-synaptic activity.
				eligibility = mPreNeuron->getFiringRate();
				break;
			case POLICY_TDCONNECTION:
				/// Eligibility depends on both pre- and post-synaptic activity.
				eligibility = mPreNeuron->getFiringRate() * 
					mPostNeuron->getFiringRate();
				break;
			default:
				assert(false);
				break;
		}

		if (eligibility >= mETrace)
		{
			mETrace = eligibility;
		}

		// The following method increases or decreases the trace towards 
		// the current eligibility.
		//mETrace = mETraceDecayFactor * mETrace + 
		//	(1 - mETraceDecayFactor) * eligibility;
	}

	void TDConnection::decayETrace()
	{
		// This decreases the trace towards zero.
		mETrace = mTDDiscountFactor * mETraceDecayFactor * mETrace;
	}

	void TDConnection::applyTDLearning(real learningFactorTimesTDError)
	{
		addToWeight(learningFactorTimesTDError * mETrace);
	}

	void TDConnection::setIsInActiveList(bool inList)
	{
		mIsInActiveList = inList;
	}

	bool TDConnection::isInActiveList()
	{
		return mIsInActiveList;
	}
}
