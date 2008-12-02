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

#ifndef DATA_COLLECTION_H
#define DATA_COLLECTION_H

#include <string>

/// A container for an array of data gathered from an experiment.  Data 
/// from multiple runs/repetitions is automatically averaged.
class DataCollection
{
public:
	/// If the given name is empty, the name will be set to "unnamed."
	DataCollection(const std::string& name, unsigned int numEntries);

	~DataCollection();

	/// Adds data to the entry at the given index.  If this is called 
	/// multiple times on the same entry, the values are automatically 
	/// averaged.  This does nothing if the entry index is beyond the 
	/// valid range.
	void storeData(unsigned int entryIndex, double value);

	/// Returns the data entry at the given index.
	double retrieveData(unsigned int entryIndex)const;

	/// Returns the number of times the data entry at the given index 
	/// has been stored (which reflects how much the data has been 
	/// averaged).
	unsigned int getNumRepetitions(unsigned int entryIndex)const;

	/// Returns the name of the DataCollection.
	const std::string& getName()const;

	/// Returns the total number of data entries.
	unsigned int getNumEntries()const;

	/// Returns a pointer to the internal data array.
	const double* getData()const;

	///// Returns the number of data entries that have been given data.  
	///// This operation is O(n) where n is the number of data entries.
	//unsigned int getNumFilledEntries()const;

protected:
	/// The name of the DataCollection.
	std::string mName;

	/// The actual array of data values.
	double* mData;

	/// An array that tracks the number of times each entry has been 
	/// stored.  Used for averaging.
	unsigned int* mNumRepetitions;

	/// The number of data points in the collection.
	unsigned int mNumEntries;
private:
};

#endif
