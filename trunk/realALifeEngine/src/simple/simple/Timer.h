/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
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

#ifndef TIMER_H
#define TIMER_H

#if defined(WIN32) || defined(_WIN32)
	#define USE_WINDOWS_TIMERS
	#include <windows.h>
	#include <time.h>
#else
	#include <sys/time.h>
#endif

namespace opalSamples
{
	/// A simple cross-platform timer class.
	class Timer
	{
	public:
		Timer();

		~Timer();

		/// Resets the initial reference time.
		void reset();

		/// Returns the time in ms since the last call to reset or since 
		/// the Clock was created.
		unsigned long int getTimeMilliseconds();

		/// Returns the time in us since the last call to reset or since 
		/// the Clock was created.
		unsigned long int getTimeMicroseconds();

	private:
	#ifdef USE_WINDOWS_TIMERS
		LARGE_INTEGER mClockFrequency;
		DWORD mStartTick;
		LONGLONG mPrevElapsedTime;
		LARGE_INTEGER mStartTime;
	#else
		struct timeval mStartTime;
	#endif
	};
}

#endif
