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

#include <time.h>

#include "NArmedBanditTest.h"
#include "HotPlateTest.h"
#include "HotPlate2DTest.h"
#include "MazeTest.h"
#include "SequentialActionsTest.h"
#include "SignaledHotPlateTest.h"
#include "SignaledHotPlate2DTest.h"
#include "CuriosityTest.h"

int main(int argc, char* argv[])
{
	srand((unsigned int)time(NULL));

	std::vector<LearningTest*> tests;
	tests.push_back(new NArmedBanditTest(2000, 501));
	tests.push_back(new SequentialActionsTest(200, 700));
	tests.push_back(new HotPlateTest(300, 31, false));
	tests.push_back(new SignaledHotPlateTest(500, 81, false));
	tests.push_back(new HotPlate2DTest(300, 301, false));
	tests.push_back(new SignaledHotPlate2DTest(300, 1601, false));
	tests.push_back(new MazeTest(10, 51));
	tests.push_back(new CuriosityTest(50, 81));

	std::cout << "===================================" 
		<< "===================================" << std::endl;
	std::cout << "Verve Learning Tests" << std::endl;

	bool finished = false;
	while (!finished)
	{
		std::cout << std::endl;
		std::cout << 0 << ") Run all tests" << std::endl;

		unsigned int size = (unsigned int)tests.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			std::cout << i + 1 << ") " << tests[i]->getTestName() 
				<< std::endl;
		}

		std::cout << size + 1 << ") Quit" << std::endl;
		std::cout << std::endl;
		std::cout << "Choose an option: " << std::flush;

		// Get a choice from the user.
		bool success = false;
		while(!success)
		{
			std::string s;
			std::getline(std::cin, s);

			if (s.length() == 1)
			{
				// Convert string to an ascii char, then to an integer.
				char c = s[0];
				int option = c - '0';

				if (0 == option)
				{
					// Run all tests.
					for (unsigned int i = 0; i < size; ++i)
					{
						tests[i]->run();
					}
					success = true;
				}
				else if (option > 0 && option <= (int)size)
				{
					tests[option - 1]->run();
					success = true;
				}
				else if ((size + 1) == option)
				{
					success = true;
					finished = true;
				}
			}

			if (!success)
			{
				std::cout << "Invalid option '" << s << "'.  " 
					<< "Choose again: " << std::flush;
			}
		}
	}

	while (!tests.empty())
	{
		tests.back()->destroy();
		tests.pop_back();
	}

	return 0;
}
