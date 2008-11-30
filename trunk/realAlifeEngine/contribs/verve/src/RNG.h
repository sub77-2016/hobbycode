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

#ifndef RNG_H
#define RNG_H

#include <time.h>
#include <math.h>
#include <iostream>

/// Global singleton access macro.  Use it like this: 
///	   double randomValue = RNG.real64(0.0, 1.0);
#define RNG rng::RandomNumberGenerator::instance()

namespace rng
{
	// Verify type sizes with compile-time assertions.
	static char sizeOfULL[sizeof(unsigned long long int) == 8];
	static char sizeOfDouble[sizeof(double) == 8];

	/// 2^64.
	const double twoPower64 = 18446744073709551616.0;

	/// 1 / 2^64.
	const double oneOver2Power64 = 1 / twoPower64;

	/// 1 / (2^64 - 1).
	const double oneOver2Power64Minus1 = 1 / (twoPower64 - 1);

	/// The inverse of the 64-bit floating point value just above 2^64 - 1, 
	/// i.e. 1 / (2^64 - 1 + 2049).
	const double oneOverJustAbove2Power64Minus1 = 1 / (twoPower64 - 1 + 2049);

	/// A pseudo-random number generator with a period of ~8.5 x 10^37 based 
	/// on the Ranq2 algorithm described in Numerical Recipes, 3rd Edition, 
	/// p. 352.  This is probably fine for applications that generate up to 
	/// about 10^30 random calls.  (Ranq2 was chosen over Ran and Ranq1 because 
	/// of its fast execution and sufficiently large period.  The Mersenne 
	/// Twister algorithm has a much larger period (~10^6001), but it 
	/// requires a state array of some length, which can be difficult to 
	/// hold in registers or L1 cache; it doesn't add much benefit beyond 
	/// the simpler Ranq2.)
	/// 
	/// This class is set up as a singleton which can be seeded and accessed 
	/// as a single global instance.  If desired, it can also be used to 
	/// create local instantiations.  No methods here are reentrant, so 
	/// they are not thread-safe.  In order to use this in a thread-safe 
	/// manner, you must provide locks before and after each call.  Another 
	/// option is to give each thread its own instance of the generator; each 
	/// instance automatically gets a unique seed.
	class RandomNumberGenerator
	{
	public:
		/// Returns a reference to the singleton instance.
		static RandomNumberGenerator& instance()
		{
			static RandomNumberGenerator singletonInstance;
			return singletonInstance;
		}

		/// Automatically seeds the generator based on the current time.
		RandomNumberGenerator()
		{
			// Perform various run-time assertions.  Note that we can't do 
			// compile-time assertions on these expressions because they use 
			// non-integral (double) types.

			// Make sure the maximum random 64-bit floating point value in the 
			// range [0, 1] is 1.0.
			if (oneOver2Power64Minus1 * (twoPower64 - 1) != 1.0)
			{
				std::cout << "ERROR: In RandomNumberGenerator constructor: " 
					<< "failed to verify that the max random floating point " 
					<< "value is 1.0" << std::endl;
					::exit(1);
			}

			// Verify the 64-bit floating point value just above 2^64 - 1.  The 
			// literal value here is truncated to double precision.
			if (oneOverJustAbove2Power64Minus1 != 
				5.4210108624275210e-020)
			{
				std::cout << "ERROR: In RandomNumberGenerator constructor: " 
					<< "failed to verify the floating point value just above " 
					<< "2^64 - 1" << std::endl;
					::exit(1);
			}

			// Make sure the 64-bit floating point value just above 2^64 - 1 is 
			// distinguishable from 2^64 - 1.
			if (oneOverJustAbove2Power64Minus1 >= oneOver2Power64Minus1)
			{
				std::cout << "ERROR: In RandomNumberGenerator constructor: " 
					<< "failed to verify that 1/(2^64-1+epsilon) < 1/(2^64-1)" 
					<< std::endl;
					::exit(1);
			}

			// Make sure the maximum random 64-bit floating point value in the 
			// range [0, 1] is < 1.0.
			if (oneOverJustAbove2Power64Minus1 * (twoPower64 - 1) >= 1.0)
			{
				std::cout << "ERROR: In RandomNumberGenerator constructor: " 
					<< "failed to verify that the uniform floating point " 
					<< "generator for [0, 1) never returns 1.0" << std::endl;
					::exit(1);
			}

			// Generate an initial seed based on the current time.  Just in 
			// case there are multiple generator instances, only the first 
			// seed is based directly on the time; we keep a static seed 
			// variable which changes each time so that every instance 
			// gets a unique seed.  (Calling time() several times in 
			// succession will return the same value, so we only use time() 
			// for the first seed.)

			// time() returns a 32-bit or 64-bit int.  Either is fine 
			// because this only determines how many unique random sequences 
			// we can generate.
			static unsigned long long int uniqueSeed = 
				(unsigned long long int)time(NULL);
			seed(uniqueSeed);

			// The period is large enough (> 10^35) that we could safely just 
			// use seed+1, seed+2, etc., but we'll use a random seed just to 
			// be safe.
			uniqueSeed = uint64();
		}

		~RandomNumberGenerator()
		{
		}

		/// Restarts the generator with the given seed.
		void seed(unsigned long long int seed)
		{
			mV = 4101842887655102017LL;
			mW = 1;
			mV ^= seed;
			mW = uint64();
			mV = uint64();

			// Warm up the generator.  To avoid high correlation between 
			// random sequences generated from successive seeds (most 
			// obviously, where the first random call returns the same value 
			// for multiple runs of the same program executed in a short 
			// duration), we make a few initial random calls.  This is 
			// probably not crucial because the period is so high, but we'll 
			// do it anyway just to be safe.
			for (size_t i = 0; i < 10; ++i)
			{
				uint64();
			}
		}

		/// Returns a random boolean value with equal probability of being 
		/// true or false.
		inline bool bit()
		{
			return int64(0, 1) == 1;
		}

		/// Returns a uniform random 32-bit signed integer in the range 
		/// [min, max], including both end points.  Maximum range: 
		/// [-2^31, 2^31 - 1], i.e. [-2147483648, 2147483647], including 
		/// both end points.
		inline int int32(int min, int max)
		{
			// A random value from [0, 1) times (max - min + 1) gives each 
			// integer within [0, max - min] equal probability.
			return min + int((max - min + 1) * real64HalfOpen());
		}

		/// Returns a uniform random 64-bit signed integer in the range 
		/// [min, max], including both end points.  Maximum range: 
		/// [-2^63, 2^63 - 1], i.e. [-9223372036854775808, 9223372036854775807], 
		/// including both end points.
		inline long long int int64(long long int min, long long int max)
		{
			// A random value from [0, 1) times (max - min + 1) gives each 
			// integer within [0, max - min] equal probability.
			return min + (long long int)((max - min + 1) * real64HalfOpen());
		}

		/// Returns a uniform random 64-bit floating point value in the range 
		/// [min, max], including both end points.
		inline double uniform(double min, double max)
		{
			return min + (max - min) * real64Closed();
		}

		/// Returns a normally-distributed 64-bit floating point value with 
		/// zero mean and the given standard deviation.  Based on the 
		/// ratio-of-uniforms method described in Numerical Recipes, 3rd 
		/// Edition, pp. 367-369.  Costs about 3 uniform deviates plus a 
		/// 1% chance of a logarithm.
		inline double normal(double stdDev)
		{
			// This fast, "un-transcendental" method calculates exact normal 
			// deviates.  From Numerical Recipes, 3rd Edition, p. 369: 
			// 
			// "Only about 1% of the time is it necessary to calculate an 
			// exact boundary (requiring a logarithm)."
			// 
			// "Note that the while clause makes use of C's (and C++'s) 
			// guarantee that logical expressions are evaluated conditionally: 
			// If the first operand is sufficient to determine the outcome, 
			// the second is not evaluated at all.  With these rules, the 
			// logarithm is evaluated only when q is between 0.27597 and 
			// 0.27846.
			// 
			// On average, each normal deviate uses 2.74 uniform deviates.  
			// By the way, even though the various constants are given only 
			// to six digits, the method is exact (to full double precision).  
			// Small perturbations of the bounding curves are of no 
			// consequence.  The accuracy is implicit in the (rare) 
			// evaluations of the exact boundary."

			double u = 0;
			double v = 0;
			double x = 0;
			double y = 0;
			double q = 0;
			do
			{
				u = real64Closed();
				v = 1.7156 * (real64Closed() - 0.5);
				x = u - 0.449871;
				y = ::fabs(v) + 0.386595;
				q = x * x + y * (0.19600 * y - 0.25472 * x);
			} while (q > 0.27597 
				&& (q > 0.27846 || v * v > -4. * ::log(u) * u * u));
			return stdDev * v / u;

			// Note: u will always be > zero, so we don't have to worry about 
			// log(u) or v / u.
		}

		/// Returns an exponentially-distributed 64-bit floating point value 
		/// with the given mean time between events (aka beta).  This 
		/// distribution represents the time (or distance) between 
		/// independent events that occur at a constant average rate (i.e. 
		/// a Poisson process).  The returned random value is thus the 
		/// time/distance until the next event.  The "rate" (aka lambda) 
		/// is 1 / mean.  Costs about one uniform deviate plus one logarithm.
		inline double exponential(double meanTimeBetweenEvents)
		{
			// Use a uniform deviate in the range [0, 1).
			return -::log(1 - real64HalfOpen()) * meanTimeBetweenEvents;

			// Von Neumann's method, which might be faster: 
			// (see http://www.iro.umontreal.ca/~vazquez/SimSpiders/GenerRV/
			//      Distributions/expon.html)
			// 1. Set k=0. 
			// 2. Generate U1 ~ U(0,1) as a uniform variate, set N=1. 
			// 3. Do
			//	     N = N+1, and 
			//	     Generate UN ~ U(0,1) 
			//    Until UN > U1 
			// 4. if N is pair then set Y = U1 
			//    else set k = k+1 and GO TO 3. 
			// 5. Set X = k + Y
		}

	private:
		// Note: if needed, we can add a int8() based on the extension 
		// given on p. 353 of Numerical Recipes, 3rd Edition: 
		// 
		// unsigned long long int breg;
		// int bc; // Initialize to zero in constructor.
		// inline unsigned char int8()
		//{
		//	if (bc--) return (unsigned char)(breg >>= 8);
		//	breg = int64();
		//	bc = 7;
		//	return (unsigned char)breg;
		//}

		/// Returns a uniform random 64-bit integer in the range [0, 2^64-1], 
		/// including both end points.
		inline unsigned long long int uint64()
		{
			mV ^= mV >> 17;
			mV ^= mV << 31;
			mV ^= mV >> 8;
			mW = 4294957665U * (mW & 0xffffffff) + (mW >> 32);
			return mV ^ mW;
		}

		/// Returns a uniform random 64-bit floating point value in the 
		/// range [0, 1], including both end points.
		inline double real64Closed()
		{
			// uint64 returns a value in the range [0, 2^64-1].  We divide 
			// this by 2^64 - 1 to yield a final range of [0, 1].
			return oneOver2Power64Minus1 * uint64();
		}

		/// Returns a uniform random 64-bit floating point value in the 
		/// range [0, 1), not including 1.
		inline double real64HalfOpen()
		{
			// uint64 returns a value in the range [0, 2^64-1].  We divide 
			// this by the value just above 2^64 - 1 to yield a final range 
			// of [0, 1). 
			return oneOverJustAbove2Power64Minus1 * uint64();
		}

		/// The "v" variable in Ranq2.
		unsigned long long int mV;

		/// The "w" variable in Ranq2.
		unsigned long long int mW;
	};
}

#endif
