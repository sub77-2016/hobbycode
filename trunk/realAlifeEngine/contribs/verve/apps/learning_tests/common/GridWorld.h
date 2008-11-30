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

#ifndef GRID_WORLD_H
#define GRID_WORLD_H

#include <iostream>

/// A discrete 2D grid world containing a single built-in robot.  Has 
/// optional maze functionality.
class GridWorld
{
public:
	/// This constructor should be used when loading map data from a file.
	GridWorld();

	GridWorld(unsigned int gridXSize, unsigned int gridYSize);

	~GridWorld();

	/// Loads map data from a file.  Clears any old map data first.
	void loadMapData(const std::string& mazeDataFilename);

	void moveRobotLeft();

	void moveRobotRight();

	void moveRobotUp();

	void moveRobotDown();

	void resetRobotPosition();

	void randomizeRobotPosition();

	void setRobotPosition(unsigned int x, unsigned int y);

	unsigned int getRobotXPosition()const;

	unsigned int getRobotYPosition()const;

	float getRobotContinuousXPosition()const;

	float getRobotContinuousYPosition()const;

	unsigned int getGoalXPosition()const;

	unsigned int getGoalYPosition()const;

	unsigned int getGridXSize()const;

	unsigned int getGridYSize()const;

protected:
	/// Deallocates dynamic memory.
	void clear();

	void allocateBarrierMaps();

	inline int randomIntUniform(int min, int max)
	{
		// Note: rand() isn't a very good generator, but it's good enough for 
		// simple tasks that only require a few thousand values.

		int range = max - min;
		int randValue = rand();
		int randMaxValue = RAND_MAX;
		while (randMaxValue < range)
		{
			// Increase the random value range until we reach the desired 
			// range.
			randValue = randValue * RAND_MAX;
			randMaxValue = randMaxValue * RAND_MAX;
			if (randValue < randMaxValue)
			{
				randValue += rand();
			}
		}

		// Get a uniform random real value from [0, 1).
		double zeroToOneHalfOpen = randValue / (randMaxValue + 1.0);

		// The [0, 1) value times (max - min + 1) gives each integer within 
		// [0, max - min] an equal chance of being chosen.
		return min + int((range + 1) * zeroToOneHalfOpen);
	}

	/// The current position of the robot.
	unsigned int mRobotPosition[2];

	unsigned int mGridXSize;

	unsigned int mGridYSize;

	/// The robot's default initial position.  Mainly used for mazes.
	unsigned int mRobotStartLocation[2];

	/// The goal location.  Mainly used for mazes.
	unsigned int mGoalLocation[2];

	/// A 2D barrier map.  Barriers exist between the grid spaces.  The 
	/// barrier indices start below and to the right of the spaces.
	bool** mVerticalBarriers;
	bool** mHorizontalBarriers;

private:
};

#endif
