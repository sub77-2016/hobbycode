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

#ifndef VERVE_OBSERVATION_H
#define VERVE_OBSERVATION_H

#include "Globals.h"

namespace verve
{
	class Agent;

	/// A simple data structure containing arrays of discrete and/or 
	/// continuous sensory input data.  An Observation is specific to a 
	/// particular type of Agent (e.g. its number of inputs), so it must 
	/// be initd to fit an Agent (or another Obsrevation) before use.
	class Observation
	{
	public:
		VERVE_DECL Observation();

		VERVE_DECL virtual ~Observation();

		/// Initializes the Observation to fit the given Agent.
		VERVE_DECL virtual void VERVE_CALL init(const Agent& a);

		/// Initializes the Observation to match the given Observation.  
		/// Also copies the actual input data.
		VERVE_DECL virtual void VERVE_CALL init(const Observation& obs);

		/// Zeroes all input data.
		VERVE_DECL virtual void VERVE_CALL zeroInputData();

		/// Copies the given input data.
		VERVE_DECL virtual void VERVE_CALL copyInputData(
			const Observation& obs);

		/// Returns the number of discrete input dimensions.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getNumDiscreteInputs()const;

		/// Returns the number of continuous input dimensions.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getNumContinuousInputs()const;

		/// Sets the value of the discrete data element at the given 
		/// index.  The value must be within [0, # of possible values - 1].
		VERVE_DECL virtual void VERVE_CALL setDiscreteValue(
			unsigned int index, unsigned int value);

		/// Returns the value of the discrete data element at the given 
		/// index.
		VERVE_DECL virtual unsigned int VERVE_CALL getDiscreteValue(
			unsigned int index)const;

		/// Returns a constant pointer to the array of discrete input data.
		VERVE_DECL virtual const unsigned int* VERVE_CALL 
			getDiscreteInputData()const;

		/// Returns a constant pointer to the array of data encoding the 
		/// number of options for each discrete input.
		VERVE_DECL virtual const unsigned int* VERVE_CALL 
			getDiscreteNumOptionsData()const;

		/// Sets the value of the continuous data element at the given 
		/// index.  The value must be within [-1, 1].  
		VERVE_DECL virtual void VERVE_CALL setContinuousValue(
			unsigned int index, real value);

		/// Returns the value of the continuous data element at the given 
		/// index.
		VERVE_DECL virtual real VERVE_CALL getContinuousValue(
			unsigned int index)const;

		/// Returns a constant pointer to the array of continuous input data.
		VERVE_DECL virtual const real* VERVE_CALL 
			getContinuousInputData()const;

		/// Returns a constant pointer to the array of data encoding which 
		/// continuous dimensions are circular.
		VERVE_DECL virtual const bool* VERVE_CALL 
			getContinuousCircularData()const;

		/// Returns the number of options for the discrete input at the 
		/// given index.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getDiscreteInputNumOptions(unsigned int i)const;

		/// Returns whether the continuous input at the given index 
		/// represents a circular data range.
		VERVE_DECL virtual bool VERVE_CALL 
			getContinuousInputIsCircular(unsigned int i)const;

		/// Returns the continuous sensor resolution.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getContinuousResolution()const;

	protected:
		/// The number of discrete input dimensions.
		unsigned int mNumDiscreteDimensions;

		/// The number of continuous input dimensions.
		unsigned int mNumContinuousDimensions;

		/// An array of discrete input data.
		unsigned int* mDiscreteInputData;

		/// An array of data encoding the number of options for each 
		/// discrete input.
		unsigned int* mDiscreteNumOptionsData;

		/// An array of continuous input data.
		real* mContinuousInputData;

		/// An array of data encoding which continuous dimensions are 
		/// circular.
		bool* mContinuousCircularData;

		/// The resolution used for each continuous input.
		unsigned mContinuousResolution;
	};
}

#endif
