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

#include "PhysicalEntity.h"

namespace OPAL_OSG { 
	
	#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	PhysicalEntity::PhysicalEntity(const std::string& name, 
		osg::NodePtr node, opal::Solid* s)
	{
		mName = name;
		mSceneNode = node;
		mSolid = s;
	
		if (mSolid && mSceneNode)
		{
			updateOSGSceneNode();
		}
	}

	PhysicalEntity::PhysicalEntity(const std::string& name, 
		osg::TransformPtr trans, opal::Solid* s)
	{
		mName = name;
		mTransCore = trans;
		mSolid = s;

		if (mSolid && mTransCore)
		{
			updateOSGSceneNode();
		}
	}
	#endif

	PhysicalEntity::PhysicalEntity(const std::string& name, opal::Solid* s)
	{
		mName = name;
		mSolid = s;

	#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
		//mSceneNode = NULL;
	#endif
	}

	PhysicalEntity::~PhysicalEntity()
	{
		// The OPAL Solid and Ogre SceneNode should be destroyed 
		// externally, not here.
	}

	const std::string& PhysicalEntity::getName()const
	{
		return mName;
	}

	bool PhysicalEntity::isVisual()const
	{
	#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
		if (mSceneNode)
		{
			return true;
		}
		else
		{
			return false;
		}
	#else
		return false;
	#endif
	}

	void PhysicalEntity::update(opal::real dt)
	{
	#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
		if (NULL == mSolid || /*NULL == mSceneNode ||*/ NULL == mTransCore || mSolid->isSleeping())
		{
			return;
		}

		updateOSGSceneNode();
	#endif
	}

	#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	void PhysicalEntity::updateOSGSceneNode()
	{
		osg::Matrix m;
		
		opal::Point3r pos = mSolid->getPosition();
		std::cout<< "Solid Pos = "<< std::endl<< 
			(osg::Real32)pos[0]<< std::endl<< 
			(osg::Real32)pos[1]<< std::endl<< 
			(osg::Real32)pos[2]<< std::endl;
		opal::Quaternion quat = mSolid->getQuaternion();
		std::cout<< "Solid Quat = "<< std::endl<< 
			(osg::Real32)quat[0]<< std::endl<< 
			(osg::Real32)quat[1]<< std::endl<< 
			(osg::Real32)quat[2]<< std::endl<< 
			(osg::Real32)quat[3]<< std::endl;
		//mSceneNode->setPosition((Ogre::Real)pos[0], (Ogre::Real)pos[1], 
			//(Ogre::Real)pos[2]);
		quat.normalize();
		//mSceneNode->setOrientation((Ogre::Real)quat[0], 
			//(Ogre::Real)quat[1], (Ogre::Real)quat[2], 
			//(Ogre::Real)quat[3]);
	    
    	//apply the new matrix to our transform core
    	osg::beginEditCP(mTransCore);    
    		m.setIdentity();
    			
   			m.setTranslate(
   				(osg::Real32)pos[0], 
   				(osg::Real32)pos[1], 
				(osg::Real32)pos[2]);
			
  			m.setRotate(
  				osg::Quaternion(
  					osg::Vec3f(
  						(osg::Real32)quat[1],
  						(osg::Real32)quat[2],
  						(osg::Real32)quat[3]), 
  					(osg::Real32)quat[0]));
  					
    		mTransCore->setMatrix(m);
    	osg::endEditCP(mTransCore);
	}

	osg::NodePtr PhysicalEntity::getSceneNode()const
	{
		return mSceneNode;
	}
	#endif

	opal::Solid* PhysicalEntity::getSolid()const
	{
		return mSolid;
	}
	
}
