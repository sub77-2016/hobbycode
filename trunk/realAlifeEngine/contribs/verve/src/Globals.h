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

#ifndef VERVE_GLOBALS_H
#define VERVE_GLOBALS_H

#include "Defines.h"
#include "RNG.h"

class TiXmlNode;

namespace verve
{
	/// Contains global functions.
	namespace globals
	{
		/// Returns the absolute value of a real number.
		inline real abs(real value)
		{
#ifdef VERVE_USE_DOUBLE
			return ::fabs(value);
#else
			return ::fabsf(value);
#endif
		}

		/// Returns the square root of a real number.
		inline real sqrt(real value)
		{
#ifdef VERVE_USE_DOUBLE
			return ::sqrt(value);
#else
			return ::sqrtf(value);
#endif
		}

		/// Returns the value of e raised to the given exponent.
		inline real exp(real exponent)
		{
#ifdef VERVE_USE_DOUBLE
			return ::exp(exponent);
#else
			return ::expf(exponent);
#endif
		}

		/// Returns the value of the given base raised to the given exponent.
		inline real pow(real base, real exponent)
		{
#ifdef VERVE_USE_DOUBLE
			return ::pow(base, exponent);
#else
			return ::powf(base, exponent);
#endif
		}

		/// Rounds the value to the closest integer.
		inline int roundToInt(real value)
		{
			int nearestInt = (int)value;
			real fraction = value - (real)nearestInt;
			if (value < 0)
			{
				if (fraction <= (real)-0.5)
				{
					--nearestInt;
				}
			}
			else
			{
				if (fraction >= (real)0.5)
				{
					++nearestInt;
				}
			}

			return nearestInt;
		}

		/// Checks whether the given value is within its acceptable range.  
		/// If not, it clamps the value and logs a warning message.  Returns 
		/// the (possibly clamped) value.
		inline real checkParamValid(real value, real rangeMin, 
			real rangeMax, const std::string& callingFunction)
		{
			const real initialValue = value;
			bool invalid = false;

			if (value < rangeMin)
			{
				value = rangeMin;
				invalid = true;
			}
			else if (value > rangeMax)
			{
				value = rangeMax;
				invalid = true;
			}

			if (invalid)
			{
				VERVE_LOGGER("warning") << callingFunction << ": " 
					<< "Actual value " << initialValue << " should be " 
					<< "within [" << rangeMin << ", " << rangeMax << "].  " 
					<< "The value will be clamped." << std::endl;
			}

			return value;
		}

		/// This decay function calculates the decay constant necessary to 
		/// reduce a value across the given dt at a rate of decayPercent 
		/// per decayTime seconds.  For example, to decay some parameter 
		/// p by 10% per 0.2 seconds across a dt of 0.01, simply call 
		/// p *= calcDecayConstant(0.1, 0.2, 0.01).  Here, p will asymptote 
		/// to 0.  Another way to use it is to decay the error between a 
		/// desired/equilibrium value and the actual value: 
		/// p += (1-calcDecayConstant(0.1, 0.2, 0.01)) * (desiredValue - p).  
		/// This reduces the error between p's desired and actual value by 
		/// 10% per 0.2 seconds across a dt of 0.01.  Here, p will asymptote 
		/// to its desired value.  A constant for a faster rate of decay 
		/// can be calculated by using a higher decayPercent and/or a lower 
		/// decayTime (the combination of the two helps achieve a wider 
		/// range of decay constants with limited numerical precision).  
		/// Note that a decayPercent of 1 or a decayTime of 0 will always 
		/// return 0, instantly decaying the parameter to 0.
		inline real calcDecayConstant(real decayPercent, real decayTime, 
			real dt)
		{
			// 'decayPercent' values greater than 1 are bad because they 
			// would present the pow function with a negative base.
			assert(decayPercent <= 1);

			// A 'decayPercent' value of 1 means that we are decaying the 
			// value to 0 in one step, so we just return a constant of 0.  
			// A 'decayTime' value of 0 gives us a divide by zero error 
			// later; since a 'decayTime' of 0 actually means we are 
			// decaying the value instantly, we just return a constant of 0.
			if (1 == decayPercent || 0 == decayTime)
			{
				return 0;
			}

			return globals::pow(1 - decayPercent, dt / decayTime);
		}

		/// Another method of calculating decay constants that uses time 
		/// constants instead of "decay percents" and "decay times."
		inline real calcDecayConstant(real timeConstant, real dt)
		{
			return calcDecayConstant(1 - VERVE_E, -timeConstant, dt);
		}

		// Note: this decay method avoids using the pow function, but it 
		// is too unstable when decayRate is 0 or >= 1
		//inline real calcDecayConstant2(real decayRate, real dt)
		//{
		//	return (1 - dt/(1-decayRate));
		//}

		/// Returns a random boolean value with equal probability of being 
		/// true or false.
		inline bool randomBool()
		{
			return RNG.bit();
		}

		/// Returns a uniform random 32-bit signed integer in the range 
		/// [min, max], including both end points.  Not thread-safe; uses a 
		/// global RNG singleton.
		inline int randomIntUniform(int min, int max)
		{
			return RNG.int32(min, max);
		}

		/// Returns a uniform random real value in the range [min, max], 
		/// including both end points.  Not thread-safe; uses a global RNG 
		/// singleton.
		inline real randomRealUniform(real min, real max)
		{
#ifdef VERVE_USE_DOUBLE
			return RNG.uniform(min, max);
#else
			return (float)RNG.uniform((double)min, (double)max);
#endif
		}

		/// Returns a normally-distributed real value with zero mean and the 
		/// given standard deviation.  Not thread-safe; uses a global RNG 
		/// singleton.
		inline real randomRealNormal(real stdDev)
		{
#ifdef VERVE_USE_DOUBLE
			return RNG.normal(stdDev);
#else
			return (float)RNG.normal((double)stdDev);
#endif
		}

		/// Scales a real value linearly to the interval [0, 1], given 
		/// some range of possible values.  If the range is zero, returns 
		/// zero.  Values outside the given range will be clamped.
		inline real scaleZeroToOne(real value, real rangeMin, 
			real rangeMax)
		{
			assert(rangeMax >= rangeMin);

			if (rangeMax == rangeMin)
			{
				return 0;
			}

			if (value >= rangeMax)
			{
				return 1;
			}

			if (value <= rangeMin)
			{
				return 0;
			}

			// Calculate the range, which will always be positive.
			real range = rangeMax - rangeMin;

			// Shift the value to be correctly positioned within the new 
			// range.
			value -= rangeMin;

			// Normalize the value to be between 0 and 1.
			value = value / range;

			return value;
		}

		/// Scales a real value linearly to the interval [-1, 1], given 
		/// some range of possible values.  If the range is zero, returns 
		/// zero.  Values outside the given range will be clamped.
		inline real scaleNegOneToOne(real value, real rangeMin, 
			real rangeMax)
		{
			if (rangeMax == rangeMin)
			{
				return 0;
			}

			value = scaleZeroToOne(value, rangeMin, rangeMax);

			// Map the normalized value to [-1, 1].
			value = (2 * value) - 1;

			return value;
		}

		/// Returns the real attribute value of the given name from the 
		/// given XML node.  Returns 0 and prints an error message if the 
		/// attribute doesn't exist.
		real VERVE_CALL getAttributeReal(const TiXmlNode* nodePtr, 
			const std::string& name);

		/// Returns the integer attribute value of the given name from the 
		/// given XML node.  Returns 0 and prints an error message if the 
		/// attribute doesn't exist.
		int VERVE_CALL getAttributeInt(const TiXmlNode* nodePtr, 
			const std::string& name);

		/// Returns the boolean attribute value of the given name from the 
		/// given XML node.  Valid string values are "true", "false", "1", 
		/// and "0".  Returns false and prints an error message if the 
		/// attribute doesn't exist or if its value is not a valid string.
		bool VERVE_CALL getAttributeBool(const TiXmlNode* nodePtr, 
			const std::string& name);

		/// Returns the string attribute value of the given name from the 
		/// given XML node.  Returns an empty string and prints an error 
		/// message if the attribute doesn't exist.
		std::string VERVE_CALL getAttributeString(const TiXmlNode* nodePtr, 
			const std::string& name);
	}
}

#endif
