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

#ifndef VERVE_H
#define VERVE_H

#include "Agent.h"
#include "Observation.h"
#include "Globals.h"

/// The main namespace that contains everything.
namespace verve
{
	/// Dynamically creates a new Agent using the given AgentDescriptor.  
	/// Always use this instead of "new Agent" to ensure that memory is 
	/// allocated from the correct heap.
	VERVE_EXPORT_FUNCTION Agent* VERVE_CALL createAgent(
		const AgentDescriptor& desc);

	///// Loads an Agent from a file.
	//VERVE_EXPORT_FUNCTION Agent* VERVE_CALL loadAgent(
	//	const std::string& filename);
}

#endif
