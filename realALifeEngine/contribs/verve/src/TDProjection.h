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

#ifndef VERVE_TD_PROJECTION_H
#define VERVE_TD_PROJECTION_H

#include "Projection.h"
#include "TDConnection.h"

namespace verve
{
	class Neuron;

	/// A Projection of TDConnections.
	class TDProjection : public Projection
	{
	public:
		VERVE_DECL TDProjection(TDConnectionType type);

		VERVE_DECL virtual ~TDProjection();

		/// Sets the factor that determines how fast the eligibility trace 
		/// decays for all Connections.
		VERVE_DECL virtual void VERVE_CALL setETraceDecayFactor(real value);

		/// Sets the factor for all Connections that determines how much 
		/// future rewards are discounted.
		VERVE_DECL virtual void VERVE_CALL setTDDiscountFactor(real value);

	protected:
		/// Overridden.  Besides the things described in the base class, 
		/// this function sets the new TDConnection's ETraceUpdateMethod, 
		/// eligibility trace decay factor, and TD discount factor.
		virtual void connectNeurons(Neuron* neuron1, 
			Neuron* neuron2);

		/// The type of TDConnections to use.
		TDConnectionType mTDConnectionType;

		/// Determines how fast eligibility traces decay.
		real mETraceDecayFactor;

		/// Determines how much future rewards are discounted.
		real mTDDiscountFactor;
	};
}

#endif
