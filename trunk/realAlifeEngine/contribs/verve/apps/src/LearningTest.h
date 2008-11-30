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

#ifndef LEARNING_TEST_H
#define LEARNING_TEST_H

#include <verve/verve.h>
#include "DataFile.h"

/// A particular test of an Agent's learning abilities.
class LearningTest
{
public:
	/// Runs the test.  Each LearningTest is responsible for gathering 
	/// and saving its own statistics.
	void run();

	/// Deallocates the test.
	void destroy();

	/// Returns the name of the test that gets printed during execution.
	const std::string& getTestName()const;

protected:
	LearningTest(unsigned int numRuns, unsigned int numTrialsPerRun);

	virtual ~LearningTest();

	/// Sets the name of the test that gets printed during execution.
	void setTestName(const std::string& name);

	/// Prints a progress message to standard output.  This should only 
	/// be called by subclasses from within their 'startTest' functions.
	void printTrialAndRunStatus(unsigned int currentRun, 
		unsigned int currentTrial, float trialPerformance);

	/// Prints a progress message to standard output.  This should only 
	/// be called by subclasses from within their 'startTest' functions.
	void printRunStatus(unsigned int currentRun);

	/// Performs learning task-specific functions.  This is the most 
	/// important part of the test.
	virtual void startTest() = 0;

	/// The number of times to repeat the entire learning task.
	unsigned int mNumRuns;

	/// The number of trials executed per run.
	unsigned int mNumTrialsPerRun;

private:
	/// The name of the test that gets printed during execution.  This 
	/// must be set by subclasses.
	std::string mTestName;

	/// The current number of marks printed to the text-based progress bar.
	unsigned int mNumProgressMarksPrinted;
};

#endif
