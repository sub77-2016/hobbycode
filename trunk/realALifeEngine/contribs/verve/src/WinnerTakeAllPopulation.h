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

#ifndef VERVE_WINNER_TAKE_ALL_POPULATION_H
#define VERVE_WINNER_TAKE_ALL_POPULATION_H

#include "UltraSparseCodePopulation.h"

namespace verve
{
	/// A Population with a single active Neuron.  The active Neuron is 
	/// chosen probabilistically based on the Neurons' firing rates.
	class WinnerTakeAllPopulation : public UltraSparseCodePopulation
	{
	public:
		VERVE_DECL WinnerTakeAllPopulation();

		VERVE_DECL virtual ~WinnerTakeAllPopulation();

		/// Updates all Neuron firing rates and applies the winner-take-all 
		/// method.  Automatically updates the input sums of all 
		/// post-synaptic Neurons.
		VERVE_DECL virtual void VERVE_CALL updateFiringRatesWTA();

	protected:
		/// Runs the winner-take-all method and returns the index of the 
		/// winner.
		unsigned int computeWinnerIndex();
	};
}

#endif
