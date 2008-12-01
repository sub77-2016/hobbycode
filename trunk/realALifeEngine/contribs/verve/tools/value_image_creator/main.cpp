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
#include <string>
#include <vector>
#include <math.h>

#include <corona/corona.h>

void printUsage()
{
	std::cout << "====================================================" << std::endl;
	std::cout << "Verve Value Image Creator generates a png " << std::endl 
		<< "image file from a data file.  This data " << std::endl 
		<< "file must be stored in the following format: " << std::endl 
		<< "   # input0_num_distinct_values input1_num_distinct_values" << std::endl 
		<< "   input0_data input1_data valueEstimation0" << std::endl 
		<< "   input0_data input1_data valueEstimation1" << std::endl 
		<< "   ...         ...         ..." << std::endl << std::endl 
		<< "Note that the data file must have either " << std::endl 
		<< "1 or 2 input columns." << std::endl << std::endl;
	std::cout << "Usage: verveVIC dataFile outputFile" << std::endl;
	std::cout << "   Example: verveVIC myValueData.dat myImage.png" << std::endl;
	std::cout << "====================================================" << std::endl;
}

void printInvalidData()
{
	std::cout << "====================================================" << std::endl;
	std::cout << "Invalid data file.  Make sure the file " << std::endl 
		<< "is not empty and that it contains only 1 or 2 " 
		<< "input columns." << std::endl;
	std::cout << "====================================================" << std::endl;
}

unsigned int getNumTokens(char* str)
{
	char* delim = " ";
	unsigned int count = 0;
	char* result = NULL;
	result = strtok(str, delim);

	while(NULL != result)
	{
		++count;
		result = strtok(NULL, delim);
	}

	return count;
}

corona::Image* createValueImage(std::string inputFilename, 
	std::string outputFilename, unsigned int pixelsPerDimension)
{
	// Read data from file.
	std::ifstream dataFile(inputFilename.c_str());
	std::vector<float> input0Data;
	std::vector<float> input1Data;
	std::vector<float> valueData;

	// Test input file to see how many input columns it has.  We check 
	// the first line here which contains the number of distinct values 
	// in each input column.
	char str[1024];
	dataFile.getline(str, 1024);
	unsigned numInputColumns = getNumTokens(str) - 1;

	if (numInputColumns < 1 || numInputColumns > 2)
	{
		printInvalidData();
		exit(1);
	}

	// Move the read pointer back to the beginning of the file.
	dataFile.seekg(0, std::ios_base::beg);

	// Read in the number of distinct points per input dimension.
	unsigned int numPointsDimension0 = 0;
	unsigned int numPointsDimension1 = 0;
	char dummy = '\0';

	// Read in '#' character.
	dataFile >> dummy;

	dataFile >> numPointsDimension0;
	if (2 == numInputColumns)
	{
		dataFile >> numPointsDimension1;
	}

	float input = 0;
	float value = 0;
	while (!dataFile.eof())
	{
		float temp = 0;
		dataFile >> temp;
		if (dataFile.eof())
		{
			// We must have hit an empty line at the end.
			break;
		}

		input = temp;
		input0Data.push_back(input);
		if (2 == numInputColumns)
		{
			dataFile >> input;
			input1Data.push_back(input);
		}
		dataFile >> value;
		valueData.push_back(value);
	}

	dataFile.close();

	if (input0Data.size() == 0)
	{
		printInvalidData();
		exit(1);
	}

	float minValue = valueData[0];
	float maxValue = valueData[0];
	size_t size = valueData.size();
	// We will scale the range [min, max] to [0, 1].
	for (unsigned int i = 0; i < size; ++i)
	{
		if (valueData[i] < minValue)
		{
			minValue = valueData[i];
		}
		else if (valueData[i] > maxValue)
		{
			maxValue = valueData[i];
		}
	}
	float valueRange = maxValue - minValue;
	//// Let maxMag = max(|min|, |max|).  We will scale the range 
	//// [-maxMag, maxMag] to [0, 1].
	//float maxMag = 0;
	//for (unsigned int i = 0; i < size; ++i)
	//{
	//	if (fabs(valueData[i]) > maxMag)
	//	{
	//		maxMag = fabs(valueData[i]);
	//	}
	//}
	//minValue = -maxMag;
	//maxValue = maxMag;
	//float valueRange = maxValue - minValue;

	// Create pixel data.  Assume the x axis uses input0 data and the 
	// y axis uses input1 data (if available).
	unsigned int width = numPointsDimension0;
	unsigned int height = 1;
	if (2 == numInputColumns)
	{
		height = numPointsDimension1;
	}

	//float incrementBetweenPoints = 2 / (float)(numPixelsPerDimension - 1);
	unsigned int dataSize = (unsigned int)valueData.size();
	unsigned char* pixelData = new unsigned char[3 * width * height];
	for (unsigned int i = 0; i < dataSize; ++i)
	{
		unsigned int x = 0;
		unsigned int y = 0;
		if (2 == numInputColumns)
		{
			x = i / numPointsDimension1;
			y = i % numPointsDimension1;
		}
		else
		{
			x = i;
			y = 0;
		}

		float value = valueData[i];

		// Scale the value to [0, 1].
		value -= minValue;
		value = value / valueRange;
		unsigned char pixelValue = 
			(unsigned char)(value * (float)255);

		// Color scheme: low value = black, high value = white.
		pixelData[(y * width + x) * 3] = pixelValue;
		pixelData[(y * width + x) * 3 + 1] = pixelValue;
		pixelData[(y * width + x) * 3 + 2] = pixelValue;
	}

	// Now make a scaled image.  Compute the scale factor necessary to 
	// make each dimension 'pixelsPerDimension' pixels across.
	unsigned int xScale = pixelsPerDimension / numPointsDimension0;
	unsigned int yScale = pixelsPerDimension / 10;
	if (2 == numInputColumns)
	{
		yScale = pixelsPerDimension / numPointsDimension1;
	}

	unsigned int scaledWidth = width * xScale;
	unsigned int scaledHeight = height * yScale;
	unsigned char* scaledPixelData = 
		new unsigned char[3 * scaledWidth * scaledHeight];
	for (unsigned int y = 0; y < scaledHeight; ++y)
	{
		for (unsigned int x = 0; x < scaledWidth; ++x)
		{
			unsigned int pixelBaseIndex = 
				(((y / yScale) * width + (x / xScale)) * 3);
			scaledPixelData[(y * scaledWidth + x) * 3] = 
				pixelData[pixelBaseIndex];
			scaledPixelData[(y * scaledWidth + x) * 3 + 1] = 
				pixelData[pixelBaseIndex + 1];
			scaledPixelData[(y * scaledWidth + x) * 3 + 2] = 
				pixelData[pixelBaseIndex + 2];
		}
	}

	corona::Image* image = corona::CreateImage(scaledWidth, scaledHeight, 
		corona::PF_R8G8B8, scaledPixelData);

	// Flip the image vertically so that the minimum of the value function 
	// coordinates is in the bottom left.
	image = corona::FlipImage(image, corona::CA_X);

	if (!corona::SaveImage(outputFilename.c_str(), corona::FF_PNG, image))
	{
		std::cerr << "Error saving value data to PNG image file." 
			<< std::endl;
	}

	delete [] pixelData;
	delete [] scaledPixelData;

	return image;
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printUsage();
		exit(1);
	}

	std::string inputFilename = argv[1];
	std::string outputFilename = argv[2];
	//unsigned int resolution = 0;
	//sscanf(argv[3], "%d", &resolution);

	corona::Image* image = createValueImage(inputFilename, outputFilename, 
		800);

	return 0;
}
