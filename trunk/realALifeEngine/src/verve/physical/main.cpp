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

#include "PendulumTest.h"
#include "CartPoleTest.h"

int getUserOption(int minAcceptableValue, int maxAcceptableValue)
{
	std::cout << "Choose an option: " << std::flush;

	int option = 0;
	bool success = false;
	while(!success)
	{
		std::string s;
		std::getline(std::cin, s);

		if (s.length() == 1)
		{
			// Convert string to an ascii char, then to an integer.
			char c = s[0];
			option = c - '0';

			if (option >= minAcceptableValue && option <= maxAcceptableValue)
			{
				success = true;
			}
		}

		if (!success)
		{
			std::cout << "Invalid option '" << s << "'.  " 
				<< "Choose again: " << std::flush;
		}
	}

	return option;
}

int main(int argc, char* argv[])
{
	srand((unsigned int)time(NULL));

	std::vector<LearningTest*> tests;
	tests.push_back(new PendulumTest(10, 201));
	tests.push_back(new CartPoleTest(20, 3001));

	std::cout << "===================================" 
		<< "===================================" << std::endl;
	std::cout << "Verve Physical Learning Tests" << std::endl;

	std::cout << std::endl;

	unsigned int size = (unsigned int)tests.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		std::cout << i + 1 << ") " << tests[i]->getTestName() 
			<< std::endl;
	}

	std::cout << size + 1 << ") Quit" << std::endl;
	std::cout << std::endl;

	// Get a learning test choice from the user.
	int option = getUserOption(1, (int)size + 1);
	if (option <= (int)size)
	{
		tests[option - 1]->run();
	}
	// else, quit...

	while (!tests.empty())
	{
		tests.back()->destroy();
		tests.pop_back();
	}

	return 0;
}
