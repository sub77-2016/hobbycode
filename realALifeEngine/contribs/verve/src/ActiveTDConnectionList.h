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

#ifndef VERVE_ACTIVE_TD_CONNECTION_LIST_H
#define VERVE_ACTIVE_TD_CONNECTION_LIST_H

#include "Projection.h"
#include <list>

namespace verve
{
	class Neuron;
	class TDConnection;

	/// A dynamic list of "active" TDConnections (those with a 
	/// non-zero eligibility trace).  This saves a lot of time by 
	/// only looping over the active TDConnections, not all of them.
	class ActiveTDConnectionList
	{
	public:
		VERVE_DECL ActiveTDConnectionList();

		VERVE_DECL virtual ~ActiveTDConnectionList();

		/// Empties the list of active TDConnections and notifies each one 
		/// that it is no longer in an active TDConnection list.
		VERVE_DECL virtual void VERVE_CALL clearList();

		/// Adds a new active TDConnection to the list.  If the 
		/// TDConnection is already in the list, this will do nothing.
		VERVE_DECL virtual void VERVE_CALL addNewActiveConnection(
			TDConnection* c);

		/// Increases the eligibility trace of each TDConnection, if 
		/// appropriate.
		VERVE_DECL virtual void VERVE_CALL increaseETraces();

		/// Decays the eligibility trace of each TDConnection.
		VERVE_DECL virtual void VERVE_CALL decayETraces();

		/// Trains the TDConnections using the given error signal (i.e. 
		/// learning factor * TD error).
		VERVE_DECL virtual void VERVE_CALL trainConnections(
			real learningFactorTimesTDError);

	protected:
		/// A list of active TDConnections.
		std::list<TDConnection*> mConnections;
	};
}

#endif
