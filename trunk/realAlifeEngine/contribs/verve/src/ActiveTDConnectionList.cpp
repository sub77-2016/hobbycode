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

#include "ActiveTDConnectionList.h"
#include "TDConnection.h"

namespace verve
{
	ActiveTDConnectionList::ActiveTDConnectionList()
	{
	}

	ActiveTDConnectionList::~ActiveTDConnectionList()
	{
		mConnections.clear();
	}

	void ActiveTDConnectionList::clearList()
	{
		while (!mConnections.empty())
		{
			mConnections.back()->setIsInActiveList(false);
			mConnections.pop_back();
		}
	}

	void ActiveTDConnectionList::addNewActiveConnection(TDConnection* c)
	{
		if (c->isInActiveList())
		{
			return;
		}

		// Insert the new element at the end of the list (because new 
		// TDConnections will probably have high eligibility traces, so 
		// this should help with sorting later).  The function 
		// std::list::insert(iterator pos, const T& x) inserts x before 
		// pos in constant time.
		mConnections.insert(mConnections.end(), c);

		c->setIsInActiveList(true);
	}

	void ActiveTDConnectionList::increaseETraces()
	{
		std::list<TDConnection*>::iterator iter;
		for (iter = mConnections.begin(); iter != mConnections.end(); ++iter)
		{
			(*iter)->increaseETrace();
		}
	}

	void ActiveTDConnectionList::decayETraces()
	{
		// After we decay each TDConnection's eligibility trace, we need 
		// to remove any TDConnection whose eligibility trace has fallen 
		// below the active level.  

		std::list<TDConnection*>::iterator iter;
		for (iter = mConnections.begin(); iter != mConnections.end(); )
		{
			(*iter)->decayETrace();

			// If the TDConnection is no longer active, remove it.
			if ((*iter)->getETrace() < defaults::activeETraceThreshold)
			{
				// Let the TDConnection know that it is no longer in 
				// the active list.
				(*iter)->setIsInActiveList(false);

				// Reset the inactive TDConnection's eligibility 
				// trace since it is probably not exactly zero.
				(*iter)->setETrace(0);

				// This returns an iterator to the element after the 
				// one erased.  This operates in constant time.
				iter = mConnections.erase(iter);
			}
			else
			{
				// The TDConnection must still be active.
				++iter;
			}
		}
	}

	void ActiveTDConnectionList::trainConnections(
		real learningFactorTimesTDError)
	{
		std::list<TDConnection*>::iterator iter;
		for (iter = mConnections.begin(); iter != mConnections.end(); ++iter)
		{
			(*iter)->applyTDLearning(learningFactorTimesTDError);
		}
	}
}
