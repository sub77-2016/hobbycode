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

#include "GridWorld.h"

#include <fstream>
#include <assert.h>

GridWorld::GridWorld()
{
	mGridXSize = 10;
	mGridYSize = 10;
	mRobotStartLocation[0] = 0;
	mRobotStartLocation[1] = 0;
	mGoalLocation[0] = 0;
	mGoalLocation[1] = 0;
	allocateBarrierMaps();
	resetRobotPosition();
}

GridWorld::GridWorld(unsigned int gridXSize, unsigned int gridYSize)
{
	mGridXSize = gridXSize;
	mGridYSize = gridYSize;
	mRobotStartLocation[0] = 0;
	mRobotStartLocation[1] = 0;
	mGoalLocation[0] = 0;
	mGoalLocation[1] = 0;
	allocateBarrierMaps();
	resetRobotPosition();
}

GridWorld::~GridWorld()
{
	clear();
}

void GridWorld::loadMapData(const std::string& mazeDataFilename)
{
	clear();

	std::ifstream dataFile(mazeDataFilename.c_str());

	// Get the grid size.  We assume the grid data file represents a 
	// square grid.
	char temp[64];
	dataFile >> temp;
	dataFile >> mGridYSize;
	dataFile >> temp;
	dataFile >> mGridXSize;

	// Read in 2 end-of-line characters.
	dataFile.get();
	dataFile.get();

	allocateBarrierMaps();

	unsigned int row = 0;
	unsigned int column = 0;
	unsigned int totalYSize = 2 * mGridYSize - 1;
	unsigned int totalXSize = 4 * mGridXSize - 3;
	for (unsigned int y = 0; y < totalYSize; ++y)
	{
		if (y % 2 == 0)
		{
			// This is a row with spaces, vertical barriers, 
			// goal states, etc.
			for (unsigned int x = 0; x < totalXSize; ++x)
			{
				char current = 0;
				current = dataFile.get();
				switch(current)
				{
					case 'e':
						// Empty grid space.
						break;
					case '|':
						// Vertical barrier.
						mVerticalBarriers[y / 2][x / 4] = true;
						break;
					case 'S':
						// Start state.
						mRobotStartLocation[0] = x / 4;
						mRobotStartLocation[1] = y / 2;
						break;
					case 'G':
						// Goal state.
						mGoalLocation[0] = x / 4;
						mGoalLocation[1] = y / 2;
						break;
					default:
						// Do nothing.
						break;
				}
			}

			// Read in the end-of-line character.
			dataFile.get();
		}
		else
		{
			// This is an in-between row containing only horizontal 
			// barriers.
			for (unsigned int x = 0; x < totalXSize; ++x)
			{
				unsigned int gridXPos = -1;
				char current = 0;
				current = dataFile.get();
				switch(current)
				{
					case '-':
						// Horizontal barrier.
						mHorizontalBarriers[(y - 1) / 2][x / 4] = true;
						break;
					default:
						// Do nothing.
						break;
				}
			}

			// Read in the end-of-line character.
			dataFile.get();
		}
	}

	dataFile.close();
	resetRobotPosition();
}

void GridWorld::clear()
{
	for (unsigned int i = 0; i < mGridYSize; ++i)
	{
		delete [] mVerticalBarriers[i];
	}
	delete [] mVerticalBarriers;

	for (unsigned int i = 0; i < mGridYSize - 1; ++i)
	{
		delete [] mHorizontalBarriers[i];
	}
	delete [] mHorizontalBarriers;
}

void GridWorld::allocateBarrierMaps()
{
	mVerticalBarriers = new bool*[mGridYSize];
	for (unsigned int row = 0; row < mGridYSize; ++row)
	{
		mVerticalBarriers[row] = new bool[mGridXSize - 1];

		for (unsigned int column = 0; column < mGridXSize - 1; ++column)
		{
			mVerticalBarriers[row][column] = false;
		}
	}

	mHorizontalBarriers = new bool*[mGridYSize - 1];
	for (unsigned int row = 0; row < mGridYSize - 1; ++row)
	{
		mHorizontalBarriers[row] = new bool[mGridXSize];

		for (unsigned int column = 0; column < mGridXSize; ++column)
		{
			mHorizontalBarriers[row][column] = false;
		}
	}
}

void GridWorld::resetRobotPosition()
{
	mRobotPosition[0] = mRobotStartLocation[0];
	mRobotPosition[1] = mRobotStartLocation[1];
}

void GridWorld::moveRobotLeft()
{
	// Only allow the move if there is no edge or barrier in the way.
	if (mRobotPosition[0] > 0 
		&& !mVerticalBarriers[mRobotPosition[1]][mRobotPosition[0] - 1])
	{
		mRobotPosition[0] -= 1;
	}
}

void GridWorld::moveRobotRight()
{
	// Only allow the move if there is no edge or barrier in the way.
	if (mRobotPosition[0] < mGridXSize - 1 
		&& !mVerticalBarriers[mRobotPosition[1]][mRobotPosition[0]])
	{
		mRobotPosition[0] += 1;
	}
}

void GridWorld::moveRobotUp()
{
	// Only allow the move if there is no edge or barrier in the way.
	if (mRobotPosition[1] < mGridYSize - 1 
		&& !mHorizontalBarriers[mRobotPosition[1]][mRobotPosition[0]])
	{
		mRobotPosition[1] += 1;
	}
}

void GridWorld::moveRobotDown()
{
	// Only allow the move if there is no edge or barrier in the way.
	if (mRobotPosition[1] > 0 
		&& !mHorizontalBarriers[mRobotPosition[1] - 1][mRobotPosition[0]])
	{
		mRobotPosition[1] -= 1;
	}
}

void GridWorld::setRobotPosition(unsigned int x, unsigned int y)
{
	mRobotPosition[0] = x;
	mRobotPosition[1] = y;
}

unsigned int GridWorld::getRobotXPosition()const
{
	return mRobotPosition[0];
}

unsigned int GridWorld::getRobotYPosition()const
{
	return mRobotPosition[1];
}

float GridWorld::getRobotContinuousXPosition()const
{
	float increment = (float)2 / (float)(mGridXSize - 1);
	return -1 + (float)mRobotPosition[0] * increment;
}

float GridWorld::getRobotContinuousYPosition()const
{
	float increment = (float)2 / (float)(mGridYSize - 1);
	return -1 + (float)mRobotPosition[1] * increment;
}

unsigned int GridWorld::getGoalXPosition()const
{
	return mGoalLocation[0];
}

unsigned int GridWorld::getGoalYPosition()const
{
	return mGoalLocation[1];
}

unsigned int GridWorld::getGridXSize()const
{
	return mGridXSize;
}

unsigned int GridWorld::getGridYSize()const
{
	return mGridYSize;
}

void GridWorld::randomizeRobotPosition()
{
	mRobotPosition[0] = randomIntUniform(0, mGridXSize - 1);
	mRobotPosition[1] = randomIntUniform(0, mGridYSize - 1);
}
