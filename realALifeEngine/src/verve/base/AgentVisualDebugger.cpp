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

#include "AgentVisualDebugger.h"

#include <verve/verve.h>
#include <ogre/Ogre.h>

AgentVisualDebugger::AgentVisualDebugger(Ogre::SceneManager* sceneMgr)
: mNumDataValues(7)
{
	mAgent = NULL;
	mSceneMgr = sceneMgr;
	mAgentData = new verve::real[mNumDataValues];

	Ogre::SceneNode* mainNode = sceneMgr->getSceneNode("cameraNode")->
		createChildSceneNode("agentVisualDebuggerMain");

	for (unsigned int i = 0; i < mNumDataValues; ++i)
	{
		char str[32];
		sprintf(str, "agentVisualDebugger%d", i);
		Ogre::SceneNode* n = mainNode->createChildSceneNode(str);
		Ogre::Entity* e = sceneMgr->createEntity(str, "cylinder.mesh");
		e->setMaterialName("Unlit/Green");
		e->setCastShadows(false);
		n->attachObject(e);
	}
}

AgentVisualDebugger::~AgentVisualDebugger()
{
	delete [] mAgentData;
}

void AgentVisualDebugger::updateVisuals()
{
	if (!mAgent)
	{
		return;
	}

	mAgentData[0] = mAgent->getDebugData().reward;
	mAgentData[1] = mAgent->getDebugData().valueEstimation * 0.1;
	mAgentData[2] = mAgent->getDebugData().TDError;
	mAgentData[3] = mAgent->getDebugData().curiosityReward;
	mAgentData[4] = mAgent->getDebugData().predictionUncertaintyEstimation;
	mAgentData[5] = mAgent->getDebugData().modelPredictionError;
	mAgentData[6] = (verve::real)mAgent->getDebugData().planLength / 
		(verve::real)mAgent->getDescriptor()->getMaxNumPlanningSteps();

	for (unsigned int i = 0; i < mNumDataValues; ++i)
	{
		char str[32];
		sprintf(str, "agentVisualDebugger%d", i);
		Ogre::SceneNode* n = mSceneMgr->getSceneNode(str);
		Ogre::Entity* e = mSceneMgr->getEntity(str);
		Ogre::Real value = mAgentData[i];

		if (value >= 0)
		{
			e->setMaterialName("Unlit/Green");
		}
		else
		{
			value = -value;
			e->setMaterialName("Unlit/Red");
		}

		// Move the visuals to a specific part of the visual field.  Orient 
		// them to be aligned with the camera.
		Ogre::Real distanceIncrement = 0.1;
		n->setPosition(0, 0, 0);
		n->resetOrientation();
		n->translate(-0.7, distanceIncrement * (mNumDataValues / 2) - 
			distanceIncrement * i, -2, Ogre::Node::TS_PARENT);
		n->rotate(Ogre::Vector3(0, 1, 0), Ogre::Radian(Ogre::Degree(90)), 
			Ogre::Node::TS_LOCAL);

		// Now adjust the visuals based on the values they represent.
		Ogre::Real length = 0.005 + 0.3 * value;
		n->translate(0.5 * length, 0, 0, Ogre::Node::TS_PARENT);
		n->setScale(0.03, 0.03, length);
	}
}

void AgentVisualDebugger::setDisplayEnabled(bool e)
{
	mSceneMgr->getSceneNode("agentVisualDebuggerMain")->setVisible(e);
}

void AgentVisualDebugger::setAgent(const verve::Agent* a)
{
	mAgent = a;
}
