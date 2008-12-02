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

#include "LearningTest.h"

LearningTest::LearningTest(unsigned int numRuns, 
	unsigned int numTrialsPerRun)
{
	mNumRuns = numRuns;
	mNumTrialsPerRun = numTrialsPerRun;
	mNumProgressMarksPrinted = 0;
}

LearningTest::~LearningTest()
{
}

void LearningTest::run()
{
	mNumProgressMarksPrinted = 0;

	std::cout << "-----------------------------------"
		<< "-----------------------------------" << std::endl;
	std::cout << "Beginning test '" << mTestName << "'" << std::endl;
	std::cout << "Performing " << mNumRuns << " runs with " 
		<< mNumTrialsPerRun << " trials per run" << std::endl << std::flush;

	// Start the specific learning test.
	startTest();

	std::cout << "Finished test '" << mTestName << "'" << std::endl;
	std::cout << "-----------------------------------"
		<< "-----------------------------------" << std::endl;
}

void LearningTest::destroy()
{
	delete this;
}

void LearningTest::setTestName(const std::string& name)
{
	mTestName = name;
}

const std::string& LearningTest::getTestName()const
{
	return mTestName;
}

void LearningTest::printTrialAndRunStatus(unsigned int currentRun, 
	unsigned int currentTrial, float trialPerformance)
{
	std::cout << "Trial " << currentTrial << " of run " 
		<< currentRun << ": performance = " << trialPerformance 
		<< std::endl;

	//unsigned int percentDone =  (unsigned int)(100 * (verve::real)
	//	(currentRun * mNumTrialsPerRun + currentTrial) / 
	//	(verve::real)(mNumRuns * mNumTrialsPerRun));

	//// Only print a mark for every 2nd percentile.
	//unsigned int newNumMarks = percentDone / 2;
	//if (newNumMarks > mNumProgressMarksPrinted)
	//{
	//	unsigned int newMarkCount = newNumMarks - mNumProgressMarksPrinted;
	//	for (unsigned int i = 0; i < newMarkCount; ++i)
	//	{
	//		std::cout << "." << std::flush;
	//	}
	//	mNumProgressMarksPrinted = newNumMarks;

	//	//if (50 == percentDone)
	//	//{
	//	//	std::cout << "50" << std::flush;
	//	//}
	//}
}

void LearningTest::printRunStatus(unsigned int currentRun)
{
	std::cout << "Finished run " << currentRun << std::endl;
}
