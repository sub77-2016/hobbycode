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

#include "Observation.h"
#include "Agent.h"

namespace verve
{
	Observation::Observation()
	{
		mNumDiscreteDimensions = 0;
		mNumContinuousDimensions = 0;
		mDiscreteInputData = NULL;
		mDiscreteNumOptionsData = NULL;
		mContinuousInputData = NULL;
		mContinuousCircularData = NULL;
		mContinuousResolution = 0;
	}

	Observation::~Observation()
	{
		if (mDiscreteInputData)
		{
			delete [] mDiscreteInputData;
		}

		if (mDiscreteNumOptionsData)
		{
			delete [] mDiscreteNumOptionsData;
		}

		if (mContinuousInputData)
		{
			delete [] mContinuousInputData;
		}

		if (mContinuousCircularData)
		{
			delete [] mContinuousCircularData;
		}
	}

	void Observation::init(const Agent& a)
	{
		const AgentDescriptor* desc = a.getDescriptor();

		mNumDiscreteDimensions = desc->getNumDiscreteSensors();
		mNumContinuousDimensions = desc->getNumContinuousSensors();

		if (0 == mNumDiscreteDimensions)
		{
			mDiscreteInputData = NULL;
			mDiscreteNumOptionsData = NULL;
		}
		else
		{
			mDiscreteInputData = new unsigned int[mNumDiscreteDimensions];
			for (unsigned int i = 0; i < mNumDiscreteDimensions; ++i)
			{
				mDiscreteInputData[i] = 0;
			}

			mDiscreteNumOptionsData = 
				new unsigned int[mNumDiscreteDimensions];
			for (unsigned int i = 0; i < mNumDiscreteDimensions; ++i)
			{
				mDiscreteNumOptionsData[i] = 
					desc->getDiscreteSensorNumOptions(i);
			}
		}

		if (0 == mNumContinuousDimensions)
		{
			mContinuousInputData = NULL;
			mContinuousCircularData = NULL;
		}
		else
		{
			mContinuousInputData = new real[mNumContinuousDimensions];
			for (unsigned int i = 0; i < mNumContinuousDimensions; ++i)
			{
				mContinuousInputData[i] = 0;
			}

			mContinuousCircularData = new bool[mNumContinuousDimensions];
			for (unsigned int i = 0; i < mNumContinuousDimensions; ++i)
			{
				mContinuousCircularData[i] = 
					desc->getContinuousSensorIsCircular(i);
			}
		}

		mContinuousResolution = desc->getContinuousSensorResolution();
	}

	void Observation::init(const Observation& obs)
	{
		mNumDiscreteDimensions = obs.mNumDiscreteDimensions;
		mNumContinuousDimensions = obs.mNumContinuousDimensions;

		if (0 == mNumDiscreteDimensions)
		{
			mDiscreteInputData = NULL;
			mDiscreteNumOptionsData = NULL;
		}
		else
		{
			mDiscreteInputData = new unsigned int[mNumDiscreteDimensions];
			for (unsigned int i = 0; i < mNumDiscreteDimensions; ++i)
			{
				mDiscreteInputData[i] = 0;
			}

			mDiscreteNumOptionsData = new unsigned int[mNumDiscreteDimensions];
			for (unsigned int i = 0; i < mNumDiscreteDimensions; ++i)
			{
				mDiscreteNumOptionsData[i] = 
					obs.mDiscreteNumOptionsData[i];
			}
		}

		if (0 == mNumContinuousDimensions)
		{
			mContinuousInputData = NULL;
			mContinuousCircularData = NULL;
		}
		else
		{
			mContinuousInputData = new real[mNumContinuousDimensions];
			for (unsigned int i = 0; i < mNumContinuousDimensions; ++i)
			{
				mContinuousInputData[i] = 0;
			}

			mContinuousCircularData = new bool[mNumContinuousDimensions];
			for (unsigned int i = 0; i < mNumContinuousDimensions; ++i)
			{
				mContinuousCircularData[i] = obs.mContinuousCircularData[i];
			}
		}

		mContinuousResolution = obs.mContinuousResolution;

		copyInputData(obs);
	}

	void Observation::zeroInputData()
	{
		for (unsigned int i = 0; i < mNumDiscreteDimensions; ++i)
		{
			mDiscreteInputData[i] = 0;
		}

		for (unsigned int i = 0; i < mNumContinuousDimensions; ++i)
		{
			mContinuousInputData[i] = 0;
		}
	}

	void Observation::copyInputData(const Observation& obs)
	{
		for (unsigned int i = 0; i < mNumDiscreteDimensions; ++i)
		{
			mDiscreteInputData[i] = obs.mDiscreteInputData[i];
		}

		for (unsigned int i = 0; i < mNumContinuousDimensions; ++i)
		{
			mContinuousInputData[i] = obs.mContinuousInputData[i];
		}
	}

	unsigned int Observation::getNumDiscreteInputs()const
	{
		return mNumDiscreteDimensions;
	}

	unsigned int Observation::getNumContinuousInputs()const
	{
		return mNumContinuousDimensions;
	}

	void Observation::setDiscreteValue(unsigned int index, 
		unsigned int value)
	{
		assert(value >= 0 && value < mDiscreteNumOptionsData[index]);
		mDiscreteInputData[index] = value;
	}

	unsigned int Observation::getDiscreteValue(unsigned int index)const
	{
		return mDiscreteInputData[index];
	}

	const unsigned int* Observation::getDiscreteInputData()const
	{
		return mDiscreteInputData;
	}

	const unsigned int* Observation::getDiscreteNumOptionsData()const
	{
		return mDiscreteNumOptionsData;
	}

	void Observation::setContinuousValue(unsigned int index, real value)
	{
		assert(value >= -1 && value <= 1);
		mContinuousInputData[index] = value;
	}

	real Observation::getContinuousValue(unsigned int index)const
	{
		return mContinuousInputData[index];
	}

	const real* Observation::getContinuousInputData()const
	{
		return mContinuousInputData;
	}

	const bool* Observation::getContinuousCircularData()const
	{
		return mContinuousCircularData;
	}

	unsigned int Observation::getDiscreteInputNumOptions(unsigned int i)const
	{
		return mDiscreteNumOptionsData[i];
	}

	bool Observation::getContinuousInputIsCircular(unsigned int i)const
	{
		return mContinuousCircularData[i];
	}

	unsigned int Observation::getContinuousResolution()const
	{
		return mContinuousResolution;
	}
}
