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

#include <iostream>
#include <fstream>
#include <math.h>

typedef float real;

//class Vector
//{
//public:
//	Vector()
//	{
//
//	}
//
//	~Vector()
//	{
//		mData.clear();
//	}
//
//	void 
//private:
//	std::vector<real> mData;
//}

int main(int argc, char* argv[])
{
	std::ifstream inputFile("sampleData.dat", std::ios::in);
	std::ofstream outputFile("results.dat", std::ios.out);

	unsigned int n = 1; // step number
	unsigned int k = 1; // number of principal components to extract
	real amnesic = 0; // CCIPCA amnesic parameter

	real data[2] = {0, 0};
	real v[2] = {0, 0};
	real prevEigenValue = 0;
	real prevEigenVector[2] = {0, 0};

	while (!inputFile.eof())
	{
		inputFile >> data[0] >> data[1];

		// Keep rolling mean values and subtract mean from data here.

		unsigned int min = k;
		if (n < k)
		{
			min = n;
		}

		for (unsigned int i = 1; i <= min; ++i)
		{
			if (i == n)
			{
				v[0] = data[0];
				v[1] = data[1];
			}
			else
			{
				real dotProd = data[0] * data[0] + data[1] * data[1];
				v[0] = (((real)n - 1 - amnesic) / (real)n) * v[0] 
					+ ((1 + amnesic) / (real)n) * dotProd * prevEigenVector[0];
				v[1] = (((real)n - 1 - amnesic) / (real)n) * v[1] 
					+ ((1 + amnesic) / (real)n) * dotProd * prevEigenVector[1];
			}

			// The vector v = eigenvalue * eigenvector.
			// Eigenvalue = length of v.
			// Eigenvector = v / length of v.
			prevEigenValue = sqrt(v[0] * v[0] + v[1] * v[1]);
			prevEigenVector[0] = v[0] / prevEigenValue;
			prevEigenVector[1] = v[1] / prevEigenValue;
		}

		// Print the eigenvalue and eigenvector.
		outputFile << "eigenvalue = " << prevEigenValue << ", eigenvector = <" 
			<< prevEigenVector[0] << ", " << prevEigenVector[1] << ">" << std::endl;

		++n;
	}

	inputFile.close();
	outputFile.close();

	return 0;
}

