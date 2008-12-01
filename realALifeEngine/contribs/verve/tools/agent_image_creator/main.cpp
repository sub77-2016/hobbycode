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

// TODO: convert this old visual neural network code into an image generation tool...

///////////////////////// from Agent.cpp////////////////////////////////
		// Setup the visual positions.

		//// Input layer.
		//mInputPopulation->setVisualNeuronSpacing(1 / 
		//	(real)mInputPopulation->getNumNeurons());
		//mInputPopulation->setVisualPosition((real)-0.5, 0);

		//// Bias/RBF layer.  Space the bias and the RBFs apart by 4 units.
		//real spacing = 1 / (real)(mRBFPopulation->getNumNeurons() + 5);
		//mBiasNeuron->setVisualPosition((real)0.1, (real)0.5 * 
		//	mRBFPopulation->getNumNeurons() * spacing + (real)2.5 * spacing);
		//mRBFPopulation->setVisualPosition(0, (real)-2.5 * spacing);
		//mRBFPopulation->setVisualNeuronSpacing(spacing);

		//// Input RBF/bias layer.  Space each RBFPopulation and the bias 
		//// apart by 4 units.
		//unsigned int numPositions = 0; 
		//unsigned int numInputRBFPopulations = 
		//	(unsigned int)mInputRBFPopulations.size();
		//for (unsigned int i = 0; i < numInputRBFPopulations; ++i)
		//{
		//	numPositions += mInputRBFPopulations[i]->getNumNeurons();
		//}
		//// Add the spaces between each Population and between the 
		//// Populations and the bias; also, add a space for the bias.
		//numPositions += (numInputRBFPopulations * 4 + 1);
		//real spacing = 1 / (real)(numPositions);
		//real currentPos = (real)0.5;

		////mBiasNeuron->setVisualPosition((real)-0.4, (real)0.5 * 
		////	mRBFPopulation->getNumNeurons() * spacing + (real)2.5 * spacing);
		//mBiasNeuron->setVisualPosition((real)-0.4, currentPos);

		//for (unsigned int i = 0; i < numInputRBFPopulations; ++i)
		//{
		//	// Make 4 spaces between successive Populations.
		//	currentPos -= 5 * spacing;

		//	RBFPopulation* pop = mInputRBFPopulations[i];
		//	real halfLength = (real)0.5 * (real)pop->getNumNeurons() * 
		//		spacing;
		//	currentPos -= halfLength;
		//	pop->setVisualPosition((real)-0.5, currentPos);
		//	pop->setVisualNeuronSpacing(spacing);
		//	currentPos -= halfLength;
		//}

		// State representation layer.
		mStatePopulation->setVisualNeuronSpacing(1 / 
			(real)mStatePopulation->getNumNeurons());
		mStatePopulation->setVisualPosition(0, 0);

		// Actor/critic layer.  Space the actor and critic sections apart 
		// by 4 units.
		real spacing = 1 / (real)(mActorPopulation->getNumNeurons() + 5);
		mActorPopulation->setVisualPosition((real)0.5, spacing);
		mActorPopulation->setVisualNeuronSpacing(spacing);
		mCriticNeuron->setVisualPosition((real)0.5, (real)-0.5 * 
			mActorPopulation->getNumNeurons() * spacing - (real)2.5 * 
			spacing);
		//mCriticPopulation->setVisualNeuronSpacing((real)2.5 * spacing);

//////////////////end from Agent.cpp///////////////////////////

////////////////from Population.cpp/////////////////////////////
	void Population::setNeuronsVisualPositions()
	{
		unsigned int numNeurons = (unsigned int)mNeurons.size();
		real baseOffset = (real)-0.5 * (numNeurons - 1) * mVisualSpacing;
		for (unsigned int i = 0; i < numNeurons; ++i)
		{
			real offset = baseOffset + i * mVisualSpacing;
			mNeurons[i]->setVisualPosition(mVisualPosition[0], 
				mVisualPosition[1] + offset);
		}
	}
////////////////end from Population.cpp/////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <corona/corona.h>

void printUsage()
{
	std::cout << "====================================================" << std::endl;
	std::cout << "Verve Value Image Creator generates a png " << std::endl 
		<< "image named valueImage.png from a data file.  This " << std::endl 
		<< "data file must be stored in the following format: " << std::endl 
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
	std::string outputFilename, unsigned int scale)
{
	// Read data from file.
	std::ifstream dataFile(inputFilename.c_str());
	std::vector<unsigned int> input0Data;
	std::vector<unsigned int> input1Data;
	std::vector<float> valueData;

	// Test input file to see how many input columns it has.
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

	unsigned int input = 0;
	float value = 0;
	while (!dataFile.eof())
	{
		unsigned int temp = 0;
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

	// Find the ranges of the data.
	unsigned int maxInput0 = input0Data[0];
	size_t size = input0Data.size();
	for (unsigned int i = 1; i < size; ++i)
	{
		if (input0Data[i] > maxInput0)
		{
			maxInput0 = input0Data[i];
		}
	}

	unsigned int maxInput1 = 0;
	if (2 == numInputColumns)
	{
		maxInput1 = input1Data[0];
		size = input1Data.size();
		for (unsigned int i = 1; i < size; ++i)
		{
			if (input1Data[i] > maxInput1)
			{
				maxInput1 = input1Data[i];
			}
		}
	}

	float minValue = valueData[0];
	float maxValue = valueData[0];
	size = valueData.size();
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

	// Create pixel data.  Assume the x axis uses input0 data and the 
	// y axis uses input1 data (if available).
	unsigned int width = maxInput0 + 1;
	unsigned int height = 1;
	if (2 == numInputColumns)
	{
		height = maxInput1 + 1;
	}
	unsigned int dataSize = (unsigned int)valueData.size();
	unsigned char* pixelData = new unsigned char[3 * width * height];
	for (unsigned int i = 0; i < dataSize; ++i)
	{
		unsigned int x = input0Data[i];
		unsigned int y = 0;
		if (2 == numInputColumns)
		{
			y = input1Data[i];
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

	// Now make a scaled image.
	unsigned int scaledWidth = width * scale;
	unsigned int scaledHeight = height * scale;
	unsigned char* scaledPixelData = 
		new unsigned char[3 * scaledWidth * scaledHeight];
	for (unsigned int y = 0; y < scaledHeight; ++y)
	{
		for (unsigned int x = 0; x < scaledWidth; ++x)
		{
			unsigned int pixelBaseIndex = 
				(((y / scale) * width + (x / scale)) * 3);
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

	corona::Image* image = createValueImage(inputFilename, outputFilename, 
		50);

	return 0;
}
