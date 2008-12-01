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

#include "RBFInputData.h"
#include "Observation.h"

namespace verve
{
	RBFInputData::RBFInputData()
	{
		numDiscInputs = 0;
		discNumOptionsData = NULL;
		discInputData = NULL;
		numContInputs = 0;
		contResolution = 0;
		contCircularData = NULL;
		contInputData = NULL;
	}

	RBFInputData::~RBFInputData()
	{
		if (discNumOptionsData)
		{
			delete [] discNumOptionsData;
		}

		if (discInputData)
		{
			delete [] discInputData;
		}

		if (contCircularData)
		{
			delete [] contCircularData;
		}

		if (contInputData)
		{
			delete [] contInputData;
		}
	}

	void RBFInputData::init(unsigned int numDiscreteInputs, 
		const unsigned int* discreteNumOptionsData, 
		const unsigned int* discreteInputData, 
		unsigned int numContinuousInputs, 
		unsigned int continuousResolution, 
		const bool* continuousCircularData, 
		const real* continuousInputData)
	{
		if (discNumOptionsData)
		{
			delete [] discNumOptionsData;
		}

		if (discInputData)
		{
			delete [] discInputData;
		}

		if (contCircularData)
		{
			delete [] contCircularData;
		}

		if (contInputData)
		{
			delete [] contInputData;
		}

		numDiscInputs = numDiscreteInputs;
		discNumOptionsData = new unsigned int[numDiscreteInputs];
		discInputData = new unsigned int[numDiscreteInputs];
		for (unsigned int i = 0; i < numDiscreteInputs; ++i)
		{
			discNumOptionsData[i] = discreteNumOptionsData[i];
			discInputData[i] = discreteInputData[i];
		}

		numContInputs = numContinuousInputs;
		contResolution = continuousResolution;
		contCircularData = new bool[numContinuousInputs];
		contInputData = new real[numContinuousInputs];
		for (unsigned int i = 0; i < numContinuousInputs; ++i)
		{
			contCircularData[i] = continuousCircularData[i];
			contInputData[i] = continuousInputData[i];
		}
	}

	void RBFInputData::init(const Observation& obs)
	{
		if (discNumOptionsData)
		{
			delete [] discNumOptionsData;
		}

		if (discInputData)
		{
			delete [] discInputData;
		}

		if (contCircularData)
		{
			delete [] contCircularData;
		}

		if (contInputData)
		{
			delete [] contInputData;
		}

		numDiscInputs = obs.getNumDiscreteInputs();
		discNumOptionsData = new unsigned int[numDiscInputs];
		discInputData = new unsigned int[numDiscInputs];
		for (unsigned int i = 0; i < numDiscInputs; ++i)
		{
			discNumOptionsData[i] = obs.getDiscreteInputNumOptions(i);
			discInputData[i] = obs.getDiscreteValue(i);
		}

		numContInputs = obs.getNumContinuousInputs();
		contResolution = obs.getContinuousResolution();
		contCircularData = new bool[numContInputs];
		contInputData = new real[numContInputs];
		for (unsigned int i = 0; i < numContInputs; ++i)
		{
			contCircularData[i] = obs.getContinuousInputIsCircular(i);
			contInputData[i] = obs.getContinuousValue(i);
		}
	}

	void RBFInputData::zeroInputData()
	{
		for (unsigned int i = 0; i < numDiscInputs; ++i)
		{
			discInputData[i] = 0;
		}

		for (unsigned int i = 0; i < numContInputs; ++i)
		{
			contInputData[i] = 0;
		}
	}

	void RBFInputData::copyInputData(const unsigned int* discreteInputData, 
		const real* continuousInputData)
	{
		for (unsigned int i = 0; i < numDiscInputs; ++i)
		{
			discInputData[i] = discreteInputData[i];
		}

		for (unsigned int i = 0; i < numContInputs; ++i)
		{
			contInputData[i] = continuousInputData[i];
		}
	}

	unsigned int RBFInputData::computeNumUniqueStates(
		unsigned int continuousResolution)const
	{
		// We use the given resolution here, not the internal one.

		unsigned int numStates = (unsigned int)globals::pow(
			(real)continuousResolution, (real)numContInputs);

		if (0 == continuousResolution)
		{
			// This value should always be at least 1.
			numStates = 1;
		}

		for (unsigned int i = 0; i < numDiscInputs; ++i)
		{
			numStates *= discNumOptionsData[i];
		}

		return numStates;
	}

	void RBFInputData::setToUniqueState(unsigned int index, 
		unsigned int numStates, unsigned int continuousResolution)
	{
		// We use the given resolution here, not the internal one.

		if (0 == numDiscInputs && 0 == numContInputs 
			|| numContInputs > 0 && 0 == continuousResolution)
		{
			return;
		}

		// Here we are basically converting a single number (the 
		// state index) to an n-digit number, where n is the number 
		// of dimensions (including discrete and continuous), and each 
		// dimension can have a different base.

		// Loop over all input dimensions.
		for (unsigned int i = 0; i < numDiscInputs + numContInputs; ++i)
		{
			unsigned int value = 0;

			// Compute the number of "sub-possibilities" (i.e. the number 
			// of possible values for all "lower" dimensions in the 
			// recursive descent).
			if (i < numDiscInputs)
			{
				// We are still handling discrete input dimensions.
				numStates = numStates / discNumOptionsData[i];
			}
			else
			{
				// We have started handling continuous input dimensions.
				numStates = numStates / continuousResolution;
			}

			while (index >= numStates)
			{
				value += 1;
				index -= numStates;
			}

			// Set the value for this dimension.
			if (i < numDiscInputs)
			{
				// We are still handling discrete input dimensions.
				discInputData[i] = value;
			}
			else
			{
				unsigned int continuousIndex = i - numDiscInputs;
				// We have started handling continuous input dimensions.  
				// Handle the special case where the resolution is 1.
				if (1 == continuousResolution)
				{
					contInputData[continuousIndex] = 0;
				}
				else
				{
					// Compute the distance between successive values for 
					// the continuous dimensions, assuming there are 
					// 'continuousResolution' positions for each one.  For 
					// circular inputs, we don't place an RBF at the 
					// extreme ends but 1/2 inter-RBF distance away from 
					// the ends.  For normal continuous inputs we put one 
					// at each end.

					if (contCircularData[continuousIndex])
					{
						real continuousIncrement = 
							2 / (real)continuousResolution;
						contInputData[continuousIndex] = -1 + 
							(verve::real)0.5 * continuousIncrement + 
							value * continuousIncrement;
					}
					else
					{
						real continuousIncrement = 
							2 / (real)(continuousResolution - 1);
						contInputData[continuousIndex] = -1 + 
							+ value * continuousIncrement;
					}
				}
			}
		}
	}
}
