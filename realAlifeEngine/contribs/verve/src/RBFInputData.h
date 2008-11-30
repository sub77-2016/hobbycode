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

#ifndef VERVE_RBF_INPUT_DATA_H
#define VERVE_RBF_INPUT_DATA_H

#include "Globals.h"

namespace verve
{
	class Observation;

	/// A convenient data structure for passing around common sets of data.
	struct RBFInputData
	{
		VERVE_DECL RBFInputData();

		VERVE_DECL ~RBFInputData();

		/// Initializes the RBFInputData with the given data.  Makes a 
		/// deep copy of everything.
		VERVE_DECL void VERVE_CALL init(unsigned int numDiscreteInputs, 
			const unsigned int* discreteNumOptionsData, 
			const unsigned int* discreteInputData, 
			unsigned int numContinuousInputs, 
			unsigned int continuousResolution, 
			const bool* continuousCircularData, 
			const real* continuousInputData);

		/// Initializes the RBFInputData with data from the given 
		/// Observation.  Makes a deep copy of everything.
		VERVE_DECL void VERVE_CALL init(const Observation& obs);

		/// Zeroes all input data.
		VERVE_DECL virtual void VERVE_CALL zeroInputData();

		/// Copies the given input data.
		VERVE_DECL virtual void VERVE_CALL copyInputData(
			const unsigned int* discreteInputData, 
			const real* continuousInputData);

		/// Computes and returns the number of possible unique states 
		/// given the data stored in the RBFInputData object.  This number 
		/// mainly depends on the given continuous resolution and the 
		/// number of options for each discrete input.
		VERVE_DECL unsigned int VERVE_CALL computeNumUniqueStates(
			unsigned int continuousResolution)const;

		/// Sets the input data to that of the state at the given index 
		/// from among all possible states.  This should be used in 
		/// conjunction with 'computeNumUniqueStates'.  This does nothing 
		/// if there are zero discrete and continuous input dimensions or 
		/// if there are more than zero continuous inputs and the given 
		/// continuous resolution is zero.
		VERVE_DECL void VERVE_CALL setToUniqueState(unsigned int index, 
			unsigned int numStates, unsigned int continuousResolution);

		/// The number of discrete inputs.
		unsigned int numDiscInputs;

		/// An array of data encoding the number of options for each 
		/// discrete input.
		unsigned int* discNumOptionsData;

		/// An array of actual discrete input data.
		unsigned int* discInputData;

		/// The number of continuous inputs
		unsigned int numContInputs;

		/// The continuous resolution (i.e. the number of RBFs per 
		/// continuous input).
		unsigned int contResolution;

		/// An array of data encoding which continuous dimensions are 
		/// circular.
		bool* contCircularData;

		/// An array of actual continuous input data.
		real* contInputData;
	};
}

#endif
