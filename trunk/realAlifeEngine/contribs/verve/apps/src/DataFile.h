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

#ifndef DATA_FILE_H
#define DATA_FILE_H

#include <string>
#include <map>
#include <vector>

class DataCollection;

/// A container for a set of DataCollections.  This class is useful for 
/// gathering multiple arrays of data from an experiment.
class DataFile
{
public:
	/// All DataCollections added later will be of size 'numEntries.'
	DataFile(unsigned int numEntries);

	~DataFile();

	///// Adds a new data array with the given name and number of entries.  
	///// This does nothing if an array of the same name already exists.
	//void addNewArray(const std::string& name);

	/// Sets data for the entry at the given index in the named data 
	/// array.  This creates a new data array if the given named array 
	/// does not yet exist.  If this is called multiple times on the same 
	/// entry in the same array, the values are automatically averaged.  
	/// This does nothing if the entry index is invalid.
	void storeData(const std::string& arrayName, unsigned int entryIndex, 
		double value);

	/// Returns a pointer to the named DataCollection.
	const DataCollection* getDataCollection(const std::string& arrayName);

	/// Returns the number of entries (rows) in the DataFile's arrays.
	unsigned int getNumEntries()const;

	/// Prints all data arrays to a file (in the order that they were 
	/// created).  Note: make sure all repetitions have finished before 
	/// calling this; otherwise, some data entries will represent 
	/// averages of more repetitions than others.
	void printToFile(const std::string& filename);

protected:
	/// The number of entries in all DataCollections.
	unsigned int mNumEntries;

	/// A map of named DataCollections.
	std::map<std::string, DataCollection*> mDataCollections;

	/// A vector of data array names that remembers the order in which 
	/// the arrays were created to make sure they get printed to files 
	/// in the same order.
	std::vector<std::string> mDataArrayNameOrder;

private:
};

#endif
