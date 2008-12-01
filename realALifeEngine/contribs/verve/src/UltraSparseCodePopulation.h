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

#ifndef VERVE_ULTRA_SPARSE_CODE_POPULATION_H
#define VERVE_ULTRA_SPARSE_CODE_POPULATION_H

#include "Population.h"

namespace verve
{
	/// A Population with only one active Neuron.
	class UltraSparseCodePopulation : public Population
	{
	public:
		VERVE_DECL UltraSparseCodePopulation();

		VERVE_DECL virtual ~UltraSparseCodePopulation();

		/// Overridden to reset the active index.
		VERVE_DECL virtual void VERVE_CALL init(unsigned int numNeurons);

		/// Overridden to reset the active index.
		VERVE_DECL virtual void VERVE_CALL resetShortTermMemory();

		/// Sets the single active Neuron.
		VERVE_DECL virtual void VERVE_CALL setActiveOutput(unsigned int i);

		/// Returns the index of the single active Neuron.  Returns -1 if 
		/// no Neuron has been set active yet.
		VERVE_DECL virtual int VERVE_CALL getActiveOutput()const;

	protected:
		/// The index of the single active Neuron.
		int mActiveIndex;
	};
}

#endif
