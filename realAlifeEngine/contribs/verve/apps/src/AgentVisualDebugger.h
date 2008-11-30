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

#ifndef AGENT_VISUAL_DEBUGGER_H
#define AGENT_VISUAL_DEBUGGER_H

namespace verve
{
	typedef float real; // This is just for forward declaration.
	class Agent;
}

namespace Ogre
{
	class SceneManager;
	class SceneNode;
}

/// Displays a visual representation of a Verve Agent's important data.
class AgentVisualDebugger
{
public:
	AgentVisualDebugger(Ogre::SceneManager* sceneMgr);

	~AgentVisualDebugger();

	/// Updates the visual representation based on the Agent's current data.
	void updateVisuals();

	/// Sets whether the visuals are displayed.
	void setDisplayEnabled(bool e);

	/// Sets the Agent to use.
	void setAgent(const verve::Agent* a);

protected:
	/// A pointer to the Verve Agent.
	const verve::Agent* mAgent;

	/// A pointer to the Ogre SceneManager.
	Ogre::SceneManager* mSceneMgr;

	///// A pointer to the Ogre Camera's parent node.
	//Ogre::SceneNode* mOgreCameraNode;

	/// The number of data values to represent visually.
	const unsigned int mNumDataValues;

	/// An array to store the Agent's debug data values.
	verve::real* mAgentData;

private:
};

#endif
