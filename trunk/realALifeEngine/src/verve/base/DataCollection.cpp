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

#include "DataCollection.h"

DataCollection::DataCollection(const std::string& name, 
	unsigned int numEntries)
{
	if (name.empty())
	{
		mName = "unnamed";
	}
	else
	{
		mName = name;
	}

	mNumEntries = numEntries;
	mData = new double[numEntries];
	mNumRepetitions = new unsigned int[numEntries];

	for (unsigned int i = 0; i < mNumEntries; ++i)
	{
		mData[i] = 0;
		mNumRepetitions[i] = 0;
	}
}

DataCollection::~DataCollection()
{
	delete [] mData;
	delete [] mNumRepetitions;
}

void DataCollection::storeData(unsigned int entryIndex, double value)
{
	if (entryIndex >= mNumEntries)
	{
		return;
	}

	mData[entryIndex] = (mData[entryIndex] * mNumRepetitions[entryIndex] 
		+ value) / (mNumRepetitions[entryIndex] + 1);
	++(mNumRepetitions[entryIndex]);
}

double DataCollection::retrieveData(unsigned int entryIndex)const
{
	return mData[entryIndex];
}

unsigned int DataCollection::getNumRepetitions(unsigned int entryIndex)const
{
	return mNumRepetitions[entryIndex];
}

const std::string& DataCollection::getName()const
{
	return mName;
}

unsigned int DataCollection::getNumEntries()const
{
	return mNumEntries;
}

const double* DataCollection::getData()const
{
	return mData;
}

//unsigned int DataCollection::getNumFilledEntries()const
//{
//	unsigned int numFilledEntries = 0;
//
//	for (unsigned int i = 0; i < mNumEntries; ++i)
//	{
//		if (mData[i].numRepetitions > 0)
//		{
//			++numFilledEntries;
//		}
//	}
//
//	return numFilledEntries;
//}
