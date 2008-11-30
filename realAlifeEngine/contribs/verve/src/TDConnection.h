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

#ifndef VERVE_TD_CONNECTION_H
#define VERVE_TD_CONNECTION_H

#include "Connection.h"

namespace verve
{
	class Neuron;

	/// Different types of TDConnections that must be handled differently.
	enum TDConnectionType
	{
		/// A TDConnection used in the value function representation.
		VALUE_FUNCTION_TDCONNECTION,

		/// A TDConnection used in the policy representation.
		POLICY_TDCONNECTION
	};

	/// A Connection that is trainable via temporal difference learning.
	class TDConnection : public Connection
	{
	public:
		VERVE_DECL TDConnection(Neuron* preNeuron, Neuron* postNeuron, 
			TDConnectionType type);

		/// Deallocates any dynamically allocated memory.  This object 
		/// should never be destroyed by the user calling 'delete' because 
		/// memory allocated within the DLL should also be destroyed within 
		/// the DLL.
		VERVE_DECL virtual ~TDConnection();

		/// Returns the type of TDConnection.
		VERVE_DECL virtual TDConnectionType VERVE_CALL getTDConnectionType();

		/// Increases the eligibility trace if the Connection is "active."
		VERVE_DECL virtual void VERVE_CALL increaseETrace();

		/// Decays the eligibility trace according to the decay factor.
		VERVE_DECL virtual void VERVE_CALL decayETrace();

		/// Sets the factor that determines how fast the eligibility trace 
		/// decays.
		VERVE_DECL virtual void VERVE_CALL setETraceDecayFactor(real value);

		/// Sets the factor that determines how much future rewards are 
		/// discounted.
		VERVE_DECL virtual void VERVE_CALL setTDDiscountFactor(real value);

		/// Sets the eligibility trace.
		VERVE_DECL virtual void VERVE_CALL setETrace(real trace);

		/// Returns the current eligibility trace.
		VERVE_DECL virtual real VERVE_CALL getETrace()const;

		/// Overridden to reset the eligibility trace in addition to other 
		/// STM parameters.
		VERVE_DECL virtual void VERVE_CALL resetShortTermMemory();

		/// Allows temporal difference learning to occur.
		VERVE_DECL virtual void VERVE_CALL applyTDLearning(
			real learningFactorTimesTDError);

		/// Sets whether the TDConnection is in the ActiveTDConnectionList.
		VERVE_DECL virtual void VERVE_CALL setIsInActiveList(bool inList);

		/// Returns whether the TDConnection is in the 
		/// ActiveTDConnectionList.
		VERVE_DECL virtual bool VERVE_CALL isInActiveList();

	protected:
		/// The decaying trace of eligibility.  The higher this value, the 
		/// more the weight can be changed.
		real mETrace;

		/// The type of TDConnection.
		TDConnectionType mTDConnectionType;

		/// Determines how fast the eligibility trace decays.
		real mETraceDecayFactor;

		/// Determines how much future rewards are discounted.
		real mTDDiscountFactor;

		/// This is set to true when the TDConnection is added to the 
		/// ActiveTDConnectionList.
		bool mIsInActiveList;
	};
}

#endif
