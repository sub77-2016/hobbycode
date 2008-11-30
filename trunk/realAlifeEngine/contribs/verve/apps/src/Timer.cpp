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

#include "Timer.h"

#include <iostream>

Timer::Timer()
{
#ifdef USE_WINDOWS_TIMERS
	QueryPerformanceFrequency(&mClockFrequency);
#endif
	reset();
}

Timer::~Timer()
{
}

void Timer::reset()
{
#ifdef USE_WINDOWS_TIMERS
	QueryPerformanceCounter(&mStartTime);
	mStartTick = GetTickCount();
	mPrevElapsedTime = 0;
#else
	gettimeofday(&mStartTime, NULL);
#endif
}

unsigned long int Timer::getTimeMilliseconds()
{
#ifdef USE_WINDOWS_TIMERS
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	LONGLONG elapsedTime = currentTime.QuadPart - 
		mStartTime.QuadPart;

	// Compute the number of millisecond ticks elapsed.
	unsigned long msecTicks = (unsigned long)(1000 * elapsedTime / 
		mClockFrequency.QuadPart);

	// Check for unexpected leaps in the Win32 performance counter.  
	// (This is caused by unexpected data across the PCI to ISA 
	// bridge, aka south bridge.  See Microsoft KB274323.)
	unsigned long elapsedTicks = GetTickCount() - mStartTick;
	signed long msecOff = (signed long)(msecTicks - elapsedTicks);
	if (msecOff < -100 || msecOff > 100)
	{
		// Adjust the starting time forwards.
		LONGLONG msecAdjustment = (std::min)(msecOff * 
			mClockFrequency.QuadPart / 1000, elapsedTime - 
			mPrevElapsedTime);
		mStartTime.QuadPart += msecAdjustment;
		elapsedTime -= msecAdjustment;

		// Recompute the number of millisecond ticks elapsed.
		msecTicks = (unsigned long)(1000 * elapsedTime / 
			mClockFrequency.QuadPart);
	}

	// Store the current elapsed time for adjustments next time.
	mPrevElapsedTime = elapsedTime;

	return msecTicks;
#else
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return (currentTime.tv_sec - mStartTime.tv_sec) * 1000 + 
		(currentTime.tv_usec - mStartTime.tv_usec) / 1000;
#endif
}

unsigned long int Timer::getTimeMicroseconds()
{
#ifdef USE_WINDOWS_TIMERS
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	LONGLONG elapsedTime = currentTime.QuadPart - 
		mStartTime.QuadPart;

	// Compute the number of millisecond ticks elapsed.
	unsigned long msecTicks = (unsigned long)(1000 * elapsedTime / 
		mClockFrequency.QuadPart);

	// Check for unexpected leaps in the Win32 performance counter.  
	// (This is caused by unexpected data across the PCI to ISA 
	// bridge, aka south bridge.  See Microsoft KB274323.)
	unsigned long elapsedTicks = GetTickCount() - mStartTick;
	signed long msecOff = (signed long)(msecTicks - elapsedTicks);
	if (msecOff < -100 || msecOff > 100)
	{
		// Adjust the starting time forwards.
		LONGLONG msecAdjustment = (std::min)(msecOff * 
			mClockFrequency.QuadPart / 1000, elapsedTime - 
			mPrevElapsedTime);
		mStartTime.QuadPart += msecAdjustment;
		elapsedTime -= msecAdjustment;
	}

	// Store the current elapsed time for adjustments next time.
	mPrevElapsedTime = elapsedTime;

	// Convert to microseconds.
	unsigned long usecTicks = (unsigned long)(1000000 * elapsedTime / 
		mClockFrequency.QuadPart);

	return usecTicks;
#else
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return (currentTime.tv_sec - mStartTime.tv_sec) * 1000000 + 
		(currentTime.tv_usec - mStartTime.tv_usec);
#endif
}
