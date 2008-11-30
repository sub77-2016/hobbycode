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

#include "DataFile.h"
#include "DataCollection.h"

#include <fstream>

DataFile::DataFile(unsigned int numEntries)
{
	mNumEntries = numEntries;
}

DataFile::~DataFile()
{
	std::map<std::string, DataCollection*>::iterator iter;
	for (iter = mDataCollections.begin(); iter != mDataCollections.end(); 
		++iter)
	{
		delete (*iter).second;
	}

	mDataCollections.clear();
	mDataArrayNameOrder.clear();
}

//void DataFile::addNewArray(const std::string& name, unsigned int numEntries)
//{
//	// Check if an array of the given name already exists.
//	if (mDataCollections.find(name) != mDataCollections.end())
//	{
//		return;
//	}
//
//	// Create a new DataCollection and store a reference to it in the main 
//	/// DataCollection map.
//	DataCollection* c = new DataCollection(name, numEntries);
//	mDataCollections[name] = c;
//}

void DataFile::storeData(const std::string& arrayName, 
	unsigned int entryIndex, double value)
{
	// If there is not already an array of this name, create one.
	if (mDataCollections.find(arrayName) == mDataCollections.end())
	{
		// Create a new DataCollection and store a reference to it in the main 
		/// DataCollection map.
		DataCollection* c = new DataCollection(arrayName, mNumEntries);
		mDataCollections[arrayName] = c;
		mDataArrayNameOrder.push_back(arrayName);
	}

	// The following call to DataCollection::storeData will do nothing if 
	// the index is invalid.
	mDataCollections[arrayName]->storeData(entryIndex, value);
}

const DataCollection* DataFile::getDataCollection(const std::string& arrayName)
{
	return mDataCollections[arrayName];
}

unsigned int DataFile::getNumEntries()const
{
	return mNumEntries;
}

void DataFile::printToFile(const std::string& filename)
{
	std::ofstream file(filename.c_str());

	// Print a row of comma-separated data array names.
	unsigned int numArrays = (unsigned int)mDataArrayNameOrder.size();
	for (unsigned int i = 0; i < numArrays; ++i)
	{
		if (0 == i)
		{
			// '#' lines are skipped by gnuplot.
			file << "# ";
		}
		else
		{
			file << ", ";
		}

		file << mDataArrayNameOrder[i];
	}

	file << std::endl;

	// Print all data to the file.  We know that all arrays are of the 
	// same length.
	for (unsigned int i = 0; i < mNumEntries; ++i)
	{
		for (unsigned int j = 0; j < numArrays; ++j)
		{
			file << mDataCollections[mDataArrayNameOrder[j]]->
				retrieveData(i) << " ";
		}

		file << std::endl;
	}

	file.close();
}
