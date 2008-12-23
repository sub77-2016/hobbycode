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

#ifndef _PHYSICAL_ENTITY_H
#define _PHYSICAL_ENTITY_H

#include <iostream>

namespace osg
{
	class NodePtr;
}

namespace opal
{
	typedef float real; // This is just for forward declaration.
	class Solid;
}

class PhysicalEntity
{
public:
#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	/// Constructor for a PhysicalEntity with a physical and visual 
	/// representation.  This should not be called directly.  Instead, 
	/// use the SimulationEngine function for creating PhysicalEntities.
	PhysicalEntity(const std::string& name, osg::NodePtr node, 
		opal::Solid* s);
#endif

	/// Constructor for a PhysicalEntity with a physical representation 
	/// only.  This should not be called directly.  Instead, 
	/// use the SimulationEngine function for creating PhysicalEntities.
	PhysicalEntity(const std::string& name, opal::Solid* s);

	virtual ~PhysicalEntity();

	/// Returns the PhysicalEntity's name.
	const std::string& getName()const;

	/// Returns whether the PhysicalEntity has a visual representation.
	bool isVisual()const;

	/// Updates the PhysicalEntity.
	virtual void update(opal::real dt);

#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	/// Updates the Ogre SceneNode transform from the OPAL 
	/// Solid's transform.
	virtual void updateOSGSceneNode();

	/// Returns a pointer to the Ogre SceneNode.
	osg::NodePtr getSceneNode()const;
#endif

	/// Returns a pointer to the OPAL Solid.
	opal::Solid* getSolid()const;

protected:
	/// The PhysicalEntity's name.
	std::string mName;

	/// Pointer to the OPAL Solid.
	opal::Solid* mSolid;

#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	/// Pointer to the Ogre SceneNode.
	osg::NodePtr mSceneNode;
#endif

private:
};

#endif
