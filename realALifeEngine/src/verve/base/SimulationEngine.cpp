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

#include "SimulationEngine.h"

SimulationEngine::SimulationEngine()
{
#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	mOgreRoot = NULL;
	mOgreSceneMgr = NULL;
	mOgreWindow = NULL;
	/*
	mInputManager = NULL;
	mMouse = NULL;
	mKeyboard = NULL;
	*/
	mPhysicalCamera = NULL;
	mDrawPickingGraphics = true;
	//mCaptureFramesEnabled = false;
	mFrameListener = NULL;
#endif

	mSimulator = NULL;
#ifdef SIMULATION_ENGINE_PHYSICS_ONLY
	mQuitApp = false;
#endif
	//mPaused = false;
	mUpdateMode = SIMULATE_REAL_TIME_MULTIPLE;
	mUpdateConstant = 1;
}

SimulationEngine::~SimulationEngine()
{
	// This must occur before destroying the Ogre root or the Opal Simulator.
	destroyAllPhysicalEntities();

#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	/*
	if (mInputManager)
	{
		mInputManager->destroyInputObject( mMouse );
		mInputManager->destroyInputObject( mKeyboard );
		OIS::InputManager::destroyInputSystem(mInputManager);
		mInputManager = NULL;
	}
	*/

	// This must occur before destroying the Opal Simulator.
	if (mPhysicalCamera)
	{
		delete mPhysicalCamera;
	}

	if (mOgreWindow)
	{
		mOgreWindow->destroy();
	}

	if (mOgreRoot)
	{
		delete mOgreRoot;
	}
	
	if (mFrameListener)
	{
		delete mFrameListener;
	}
#endif

	if (mSimulator)
	{
		mSimulator->destroy();
	}
}

bool SimulationEngine::init(PhysicalCamera::Type cameraType, 
	opal::real physicalCameraEyeHeight)
{
#ifndef SIMULATION_ENGINE_PHYSICS_ONLY

#if defined (WIN32) || defined(_WIN32)
	// It looks like win32 defines _DEBUG and the rest define DEBUG.
	#if defined(_DEBUG)
		mOgreRoot = new Ogre::Root("ogre_plugins_win32_d.cfg");
	#else
		mOgreRoot = new Ogre::Root("ogre_plugins_win32.cfg");
	#endif
#else
	#if defined(DEBUG)
		mOgreRoot = new Ogre::Root("ogre_plugins_unix_d.cfg");
	#else
		mOgreRoot = new Ogre::Root("ogre_plugins_unix.cfg");
	#endif
#endif

	// Only One Scene Root
	//mOgreRoot = mOgreRoot->getSingletonPtr();
	
	// Load resource paths from config file.  Go through all sections and 
	// settings in the file.
	Ogre::ConfigFile cf;
	cf.load("ogre_resources.cfg");
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}

    // Show the Ogre configuration dialog and initialise the Ogre root.  
	// This can be skipped by using root.restoreConfig() to load 
	// configuration settings, but there must be ones saved in ogre.cfg.
    if(mOgreRoot->showConfigDialog())
    {
        // If we got here, user clicked OK, so initialise the Ogre root.  
		// Here we choose to let the system create a default rendering 
		// window by passing 'true'.
        mOgreWindow = mOgreRoot->initialise(true, "Opal-Ogre Application");
    }
    else
    {
		// The user canceled the configuration dialog, so we need to 
		// quit.
        return false;
    }

	// Create the Ogre SceneManager, in this case a generic one.
	mOgreSceneMgr = mOgreRoot->createSceneManager(Ogre::ST_GENERIC);

    // Create and setup an Ogre camera.
	Ogre::Camera* ogreCam = mOgreSceneMgr->createCamera("camera");
	ogreCam->setNearClipDistance((Ogre::Real)0.5);
	Ogre::SceneNode* camNode = mOgreSceneMgr->getRootSceneNode()->
		createChildSceneNode("cameraNode");
	camNode->attachObject(ogreCam);
    //ogreCam->setPosition(0, 0, 0);
    //ogreCam->lookAt(0, 0, -300);

	// Create a viewport that covers the entire window.
	Ogre::Viewport* vp = mOgreWindow->addViewport(ogreCam);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport.
	ogreCam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / 
		Ogre::Real(vp->getActualHeight()));

	// Set default mipmap level (NB some APIs ignore this).
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Initialise resources (e.g. parse resource scripts.
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	//ExampleFrameListener::init(ExampleApplication::mWindow, 
	//	mOgreCamera);

	/*
	// Get a pointer to the Ogre overlay.  Make it visible by default.
	Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay")->
		show();

	// Setup the input capturing devices.
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;
	mOgreWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	mInputManager = OIS::InputManager::createInputSystem(pl);
	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(
		OIS::OISKeyboard, false));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(
		OIS::OISMouse, false));
	*/

#endif // #ifndef SIMULATION_ENGINE_PHYSICS_ONLY

	// Setup the OPAL simulator.
	mSimulator = opal::createSimulator();

#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	// Create the physical camera.
	mPhysicalCamera = new PhysicalCamera(cameraType, mSimulator, ogreCam, 
		physicalCameraEyeHeight);

	setupDefaultVisualScene();
	createScene();
	createFrameListener();
#endif

	mFrameTimer.reset();

	return true;
}

/// Now become non-Graphics update
#ifdef SIMULATION_ENGINE_PHYSICS_ONLY
void SimulationEngine::update(opal::real& elapsedSimTime, 
	opal::real& elapsedRealTime)
{
	/*
#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	if (mOgreWindow->isClosed())
	{
		mQuitApp = true;
		return;
	}
#endif
	*/

	// Get the elapsed time in seconds since the last time we were here.
	elapsedRealTime = mFrameTimer.getTimeMilliseconds() * (opal::real)0.001;
	mFrameTimer.reset();
	elapsedSimTime = elapsedRealTime;

	/*
#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	if (false == handleInput(elapsedRealTime))
	{
		mQuitApp = true;
		return;
	}
#endif
	*/

	//if (!mPaused)
	//{
		switch(mUpdateMode)
		{
			case SIMULATE_CONSTANT_CHUNK:
				// Simulate constant chunks of time at once.  Keep in 
				// mind that this must finish before continuing, so 
				// if it takes a while to simulate a single chunk 
				// of time, the input handling might become unresponsive.
				elapsedSimTime = mUpdateConstant;
				break;
			case SIMULATE_REAL_TIME_MULTIPLE:
				elapsedSimTime *= mUpdateConstant;
				break;
			default:
				assert(false);
				break;
		}

		mSimulator->simulate(elapsedSimTime);

		size_t size = mPhysicalEntityList.size();
		for(size_t i = 0; i<size; ++i)
		{
			mPhysicalEntityList.at(i)->update(elapsedSimTime);
		}
/*
#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
		mPhysicalCamera->update(elapsedSimTime);
#endif
*/
	//}

/*
#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	updatePickingGraphics();

	// 'renderOneFrame' returns a bool that determines whether we should 
	// quit, but it is only useful when using pre and post frame event 
	// listeners.
	if (!mOgreRoot->renderOneFrame())
		return;

	// Update the stats overlay.
	updateOgreStats();

	//if (mCaptureFramesEnabled)
	//{
	//	captureFrame();
	//}
#endif
*/
}

bool SimulationEngine::quitApp()
{
	return mQuitApp;
}
#endif

opal::Simulator* SimulationEngine::getSimulator()const
{
	return mSimulator;
}

#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
Ogre::SceneManager* SimulationEngine::getSceneManager()const
{
	return mOgreSceneMgr;
}

/*
OIS::Keyboard* SimulationEngine::getKeyboard()const
{
	return mKeyboard;
}

OIS::Mouse* SimulationEngine::getMouse()const
{
	return mMouse;
}
*/

PhysicalCamera* SimulationEngine::getCamera()
{
	return mPhysicalCamera;
}

void SimulationEngine::setCameraMoveSpeed(opal::real speed)
{
	mPhysicalCamera->setTranslateSpeed(speed);
}

//void SimulationEngine::setCaptureFramesEnabled(bool capture)
//{
//	mCaptureFramesEnabled = capture;
//}
#endif

void SimulationEngine::setUpdateMode(UpdateMode mode, opal::real updateConstant)
{
	assert(updateConstant > 0);
	mUpdateMode = mode;
	mUpdateConstant = updateConstant;
}

SimulationEngine::UpdateMode SimulationEngine::getUpdateMode()const
{
	return mUpdateMode;
}

opal::real SimulationEngine::getUpdateConstant()const
{
	return mUpdateConstant;
}

PhysicalEntity* SimulationEngine::getPhysicalEntity(const std::string& name)const
{
	std::map<std::string, PhysicalEntity*>::const_iterator iter = 
		mPhysicalEntityMap.find(name);

	if (mPhysicalEntityMap.end() == iter)
	{
		// The named PhysicalEntity could not be found in the map.
		return NULL;
	}
	else
	{
		return (*iter).second;
	}
}

void SimulationEngine::destroyPhysicalEntity(PhysicalEntity* pe)
{
	if (!pe)
	{
		return;
	}

	// If the PhysicalEntity has a name, remove it from the 
	// PhysicalEntity map.
	std::string name = pe->getName();
	if (!name.empty())
	{
		mPhysicalEntityMap.erase(name);
	}

	for(size_t i = 0; i<mPhysicalEntityList.size(); ++i)
	{
		if(mPhysicalEntityList[i] == pe)
		{
			// Destroy the PhysicalEntity's OPAL Solid.
			mSimulator->destroySolid(pe->getSolid());

#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
			// Destroy the PhysicalEntity's Ogre SceneNode and 
			// attached Entities.
			Ogre::SceneNode* sn = pe->getSceneNode();

			// Detach and destroy all objects from the SceneNode; 
			// in this case, there should only be a single entity 
			// (which is why we can safely call 'removeEntity' 
			// here).  TODO: We should also check child nodes and 
			// destroy their Entities here; this isn't a huge 
			// issue since Ogre will destroy all the Entities 
			// when the app exits.
			while(sn->numAttachedObjects() > 0)
			{
				Ogre::MovableObject* thisObject = 
					sn->detachObject(static_cast<unsigned short>(0));
				std::string s = thisObject->getName();
				mOgreSceneMgr->destroyEntity(thisObject->getName());
			}

			// Destroy the SceneNodes all of its children.
			sn->removeAndDestroyAllChildren();
			mOgreSceneMgr->getRootSceneNode()->removeAndDestroyChild(
				sn->getName());
			sn = NULL;
#endif

			// Delete the PhysicalEntity.
			delete pe;
			pe = NULL;

			// Copy the last element over this one and pop the last 
			// one off, removing it from the PhysicalEntity list.
			mPhysicalEntityList[i] = mPhysicalEntityList.back();
			mPhysicalEntityList.pop_back();

			// Return early since we've already found the desired 
			// element.
			return;
		}
	}
}

void SimulationEngine::destroyAllPhysicalEntities()
{
	while (!mPhysicalEntityList.empty())
	{
		destroyPhysicalEntity(mPhysicalEntityList.back());
	}

	// Every pointer in the mPhysicalEntityMap is a duplicate of 
	// a pointer in mPhysicalEntityList, so we don't need to 
	// delete the memory they point to.
	mPhysicalEntityMap.clear();
}

PhysicalEntity* SimulationEngine::createPhysicalEntity(
	const std::string& name, const std::string& materialName, opal::Solid* s)
{
	PhysicalEntity* pe = NULL;

	std::string nameStr = name;
	if (nameStr.empty())
	{
		nameStr = generateUniqueName();
	}

#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
	// Create an Ogre SceneNode for the Entity.
	Ogre::SceneNode* sn = mOgreSceneMgr->getRootSceneNode()->
		createChildSceneNode(nameStr);

	for (unsigned int i = 0; i < s->getData().getNumShapes(); ++i)
	{
		char shapeName[512];
		sprintf(shapeName, "%s_shape_%d", nameStr.c_str(), i);
		createChildVisualEntity(sn, s->getData().getShapeData(i), shapeName, 
			materialName);
	}

	pe = createVisualPhysicalEntity(nameStr, sn, s);
#else
	pe = createNonVisualPhysicalEntity(nameStr, s);
#endif

	return pe;
}

PhysicalEntity* SimulationEngine::createNonVisualPhysicalEntity(
	const std::string& name, opal::Solid* s)
{
	if (!s)
	{
		return NULL;
	}

	std::string nameStr = name;
	if (nameStr.empty())
	{
		nameStr = generateUniqueName();
	}

	// Create a new PhysicalEntity without any visual representation.
	PhysicalEntity* pe = new PhysicalEntity(nameStr, s);

	// Store the pointer in the PhysicalEntity list.
	mPhysicalEntityList.push_back(pe);

	// If the name is not empty, also store the pointer in the 
	// map of named PhysicalEntities.
	if (!name.empty())
	{
		mPhysicalEntityMap[nameStr] = pe;
	}

	return pe;
}

#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
PhysicalEntity* SimulationEngine::createVisualPhysicalEntity(
	const std::string& name, Ogre::SceneNode* sn, opal::Solid* s)
{
	if (!s)
	{
		return NULL;
	}

	std::string nameStr = name;
	if (nameStr.empty())
	{
		nameStr = generateUniqueName();
	}

	// Create a new PhysicalEntity.
	PhysicalEntity* pe = new PhysicalEntity(nameStr, sn, s);

	// Store the pointer in the PhysicalEntity list.
	mPhysicalEntityList.push_back(pe);

	// If the name is not empty, also store the pointer in the 
	// map of named PhysicalEntities.
	if (!name.empty())
	{
		mPhysicalEntityMap[nameStr] = pe;
	}

	return pe;
}

void SimulationEngine::createChildVisualEntity(Ogre::SceneNode* parentNode, 
	const opal::ShapeData* data, const std::string& name, 
	const std::string& materialName)
{
	// Create an Ogre SceneNode for the Entity.
	opal::Point3r offsetPos = data->offset.getPosition();
	Ogre::Vector3 translationOffset(offsetPos[0], offsetPos[1], 
		offsetPos[2]);
	opal::Quaternion offsetQuat = data->offset.getQuaternion();
	Ogre::Quaternion rotationOffset;
	rotationOffset.x = offsetQuat.x;
	rotationOffset.y = offsetQuat.y;
	rotationOffset.z = offsetQuat.z;
	rotationOffset.w = offsetQuat.w;

	Ogre::SceneNode* newChildNode = NULL;
	Ogre::Entity* e = NULL;

	switch(data->getType())
	{
		case opal::BOX_SHAPE:
		{
			newChildNode = parentNode->createChildSceneNode(name, 
				translationOffset, rotationOffset);

			// Scale the object according to the given dimensions.
			opal::Vec3r boxDim = static_cast<const opal::BoxShapeData*>
				(data)->dimensions;
			Ogre::Vector3 dimensions(boxDim[0], boxDim[1], boxDim[2]);
			newChildNode->scale(dimensions[0], dimensions[1], 
				dimensions[2]);

			// Create an Ogre Entity using a cube mesh.  This mesh must be 
			// stored as a box with dimensions 1x1x1.
			e = mOgreSceneMgr->createEntity(name, "cube.mesh");
			e->setMaterialName(materialName);

			// Keep the normals normalized even after scaling.
			//e->setNormaliseNormals(true);

			// Attach the Entity to the SceneNode.
			newChildNode->attachObject(e);
			break;
		}
		case opal::SPHERE_SHAPE:
		{
			newChildNode = parentNode->createChildSceneNode(name, 
				translationOffset, rotationOffset);

			// Scale the object according to the given dimensions.
			Ogre::Real radius = static_cast<const opal::SphereShapeData*>
				(data)->radius;
			newChildNode->scale(radius, radius, radius);

			// Create an Ogre Entity using a sphere mesh.  This mesh must be 
			// stored as a sphere with radius 1.
			e = mOgreSceneMgr->createEntity(name, "sphere.mesh");
			e->setMaterialName(materialName);

			// Keep the normals normalized even after scaling.
			//e->setNormaliseNormals(true);

			// Attach the Entity to the SceneNode.
			newChildNode->attachObject(e);
			break;
		}
		case opal::CAPSULE_SHAPE:
		{
			// Create an Ogre SceneNode for the cylinder Entity.
			std::string subObjectName = "cylinder" + name;
			newChildNode = parentNode->createChildSceneNode(
				subObjectName, translationOffset, rotationOffset);

			// Scale the object according to the given dimensions.  This 
			// will also scale the transforms for the child nodes, but 
			// we disable the "inherit scale" option for child nodes 
			// here so the shapes themselves don't get scaled.
			Ogre::Real radius = static_cast<const opal::CapsuleShapeData*>
				(data)->radius;
			Ogre::Real length = static_cast<const opal::CapsuleShapeData*>
				(data)->length;
			newChildNode->scale(radius, radius, length);

			// This mesh must be stored as a cylinder with length 1 and 
			// radius 1.
			e = mOgreSceneMgr->createEntity(subObjectName, "cylinder.mesh");
			e->setMaterialName(materialName);
			//e->setNormaliseNormals(true);
			newChildNode->attachObject(e);

			// The spheres must use separate scene nodes that are offset 
			// from the cylinder's scene node.

			// This mesh must be stored as a sphere with radius 1.
			subObjectName = "sphere0" + name;
			Ogre::SceneNode* sphereNode = newChildNode->createChildSceneNode(
				subObjectName);
			sphereNode->setInheritScale(false);
			sphereNode->translate(0, 0, -0.5);
			sphereNode->scale(radius, radius, radius);
			e = mOgreSceneMgr->createEntity(subObjectName, "sphere.mesh");
			e->setMaterialName(materialName);
			//e->setNormaliseNormals(true);
			sphereNode->attachObject(e);

			subObjectName = "sphere1" + name;
			sphereNode = newChildNode->createChildSceneNode(subObjectName);
			sphereNode->setInheritScale(false);
			sphereNode->translate(0, 0, 0.5);
			sphereNode->scale(radius, radius, radius);
			e = mOgreSceneMgr->createEntity(subObjectName, "sphere.mesh");
			e->setMaterialName(materialName);
			//e->setNormaliseNormals(true);
			sphereNode->attachObject(e);
			break;
		}
		default:
			assert(false);
			break;
	}
}

void SimulationEngine::setupDefaultVisualScene()
{
	if (mOgreSceneMgr->getShadowTechnique() == Ogre::SHADOWTYPE_NONE)
	{
		mOgreSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	}
	else
	{
		mOgreSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
	}

	//mOgreSceneMgr->showBoundingBoxes(true);

	// Set the ambient light level.
	mOgreSceneMgr->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));

	// Create a light source.
	Ogre::Light* light = mOgreSceneMgr->createLight("light0");
	light->setType(Ogre::Light::LT_POINT);
	light->setDiffuseColour(0.5, 0.5, 0.5);
	light->setSpecularColour(0.5, 0.5, 0.5);
	light->setPosition(100.0, 300.0, 100.0);

	// Create another light source.
	light = mOgreSceneMgr->createLight("light1");
	light->setType(Ogre::Light::LT_POINT);
	light->setDiffuseColour(0.5, 0.5, 0.5);
	light->setSpecularColour(0.5, 0.5, 0.5);
	light->setPosition(-100.0, 300.0, 100.0);
	//light->setVisible(false);

	// Setup the initial camera position.
	mPhysicalCamera->setPosition(opal::Point3r(0, 2, 3));
	mPhysicalCamera->lookAt(opal::Point3r(0, 1, 0));

	// Create a skybox.
	mOgreSceneMgr->setSkyBox(true, "Skyboxes/Gray", 5000);

	// Make a sphere used to show the grasp position.
	Ogre::SceneNode* sphereNode = mOgreSceneMgr->getRootSceneNode()->
		createChildSceneNode("pickingSphere0");
	sphereNode->scale(0.05, 0.05, 0.05);
	Ogre::Entity* sphereEntity = mOgreSceneMgr->createEntity("pickingSphere", 
		"sphere.mesh");
	sphereEntity->setMaterialName("Unlit/Yellow");
	sphereEntity->setCastShadows(false);
	sphereEntity->setVisible(false);
	sphereNode->attachObject(sphereEntity);

	// Make a "line" (scaled cylinder) to show the grasping spring.
	Ogre::SceneNode* lineNode = mOgreSceneMgr->getRootSceneNode()->
		createChildSceneNode("springLine");
	Ogre::Entity* lineEntity = mOgreSceneMgr->createEntity("springLine", 
		"cylinder.mesh");
	lineEntity->setMaterialName("Unlit/White");
	lineEntity->setCastShadows(false);
	lineEntity->setVisible(false);
	lineNode->attachObject(lineEntity);

	//// Make a crosshairs for picking.
	/*
	Ogre::Overlay* overlay = 
		OverlayManager::getSingleton().getByName("General/Crosshair");
	overlay->show();
	*/
}

/*
void SimulationEngine::setPickingGraphicsEnabled(bool e)
{
	mDrawPickingGraphics = e;
}

void SimulationEngine::updatePickingGraphics()
{
	if (!mDrawPickingGraphics)
	{
		Ogre::Entity* pickingSphere = 
			mOgreSceneMgr->getEntity("pickingSphere");
		if (pickingSphere->isVisible())
		{
			pickingSphere->setVisible(false);
		}

		Ogre::Entity* springLine = 
			mOgreSceneMgr->getEntity("springLine");
		if (springLine->isVisible())
		{
			springLine->setVisible(false);
		}
		return;
	}

	// Update the grasping spring line.
	if (mPhysicalCamera->isGrasping())
	{
		Ogre::Entity* pickingSphere = 
			mOgreSceneMgr->getEntity("pickingSphere");
		if (!pickingSphere->isVisible())
		{
			pickingSphere->setVisible(true);
		}

		Ogre::Entity* springLine = 
			mOgreSceneMgr->getEntity("springLine");
		if (!springLine->isVisible())
		{
			springLine->setVisible(true);
		}

		opal::Point3r desiredPos = 
			mPhysicalCamera->getGraspGlobalPos();
		Ogre::Vector3 point0(desiredPos[0], desiredPos[1], desiredPos[2]);

		opal::Point3r attachPos = mPhysicalCamera->getAttachGlobalPos();
		Ogre::Vector3 point1(attachPos[0], attachPos[1], attachPos[2]);

		pickingSphere->getParentSceneNode()->setPosition(point1);

		Ogre::Vector3 lineVec = point0 - point1;
		if (!lineVec.isZeroLength())
		{
			Ogre::SceneNode* springLineNode = 
				springLine->getParentSceneNode();
			springLineNode->setPosition(0.5 * (point0 + point1));

			springLineNode->setDirection(lineVec, Ogre::Node::TS_WORLD);
			springLineNode->setScale(0.02, 0.02, lineVec.length());
		}
		else
		{
			springLine->setVisible(false);
		}
	}
	else
	{
		Ogre::Entity* pickingSphere = 
			mOgreSceneMgr->getEntity("pickingSphere");
		if (pickingSphere->isVisible())
		{
			pickingSphere->setVisible(false);
		}

		Ogre::Entity* springLine = 
			mOgreSceneMgr->getEntity("springLine");
		if (springLine->isVisible())
		{
			springLine->setVisible(false);
		}
	}
}
*/

//void SimulationEngine::captureFrame()
//{
//	static unsigned int count = 0;
//	char filename[32];
//	sprintf(filename, "capture%d.png", count);
//	mOgreWindow->writeContentsToFile(filename);
//	++count;
//}

/*
bool SimulationEngine::handleInput(opal::real dt)
{
	//// This variable can be used to keep keys from repeating too fast.
	//if (mTimeUntilNextToggle >= 0)
	//{
	//	mTimeUntilNextToggle -= dt;
	//}

	// Get fresh input data.
	mKeyboard->capture();
	mMouse->capture();

	// Handle keyboard input.
	if (processUnbufferedKeyInput(dt) == false)
	{
		return false;
	}

	// Handle mouse input.
	if (processUnbufferedMouseInput(dt) == false)
	{
		return false;
	}

	return true;
}

bool SimulationEngine::processUnbufferedKeyInput(opal::real dt)
{
	// This variable can be used to keep keys from repeating too fast.
	static Ogre::Real toggleTimer = 0;
	if (toggleTimer >= 0)
	{
		toggleTimer -= dt;
	}

	// Check if we should quit looping.
	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE) 
		|| mKeyboard->isKeyDown(OIS::KC_Q))
	{
		return false;
	}

	// Check if we should pause the simulation.
	if(mKeyboard->isKeyDown(OIS::KC_P) && toggleTimer <= 0)
	{
		mPaused = !mPaused;

		// Reset the timer for toggle keys.
		toggleTimer = 0.5;
	}

	//// Check if we should start/stop video frame capturing.
	//if(mKeyboard->isKeyDown(OIS::KC_V) && toggleTimer <= 0)
	//{
	//	if (mCaptureFramesEnabled)
	//	{
	//		setCaptureFramesEnabled(false);
	//	}
	//	else
	//	{
	//		setCaptureFramesEnabled(true);
	//	}

	//	toggleTimer = 0.5;
	//}

	// Check if we should save the Agent to an XML file.
	if(mKeyboard->isKeyDown(OIS::KC_F2) && toggleTimer <= 0)
	{
		std::cout << "Verve::Agent::save not implemented yet!" 
			<< std::endl;

		toggleTimer = 0.5;
	}

	// Simulation modes.
	if(mKeyboard->isKeyDown(OIS::KC_1))
	{
		mUpdateMode = SIMULATE_CONSTANT_CHUNK;
		mUpdateConstant = 3;
	}
	else if(mKeyboard->isKeyDown(OIS::KC_2))
	{
		mUpdateMode = SIMULATE_REAL_TIME_MULTIPLE;
		mUpdateConstant = 1;
	}
	else if(mKeyboard->isKeyDown(OIS::KC_3))
	{
		mUpdateMode = SIMULATE_REAL_TIME_MULTIPLE;
		mUpdateConstant = 0.5;
	}
	else if(mKeyboard->isKeyDown(OIS::KC_4))
	{
		mUpdateMode = SIMULATE_REAL_TIME_MULTIPLE;
		mUpdateConstant = 0.25;
	}
	else if(mKeyboard->isKeyDown(OIS::KC_5))
	{
		mUpdateMode = SIMULATE_REAL_TIME_MULTIPLE;
		mUpdateConstant = 0.125;
	}
	else if(mKeyboard->isKeyDown(OIS::KC_6))
	{
		mUpdateMode = SIMULATE_REAL_TIME_MULTIPLE;
		mUpdateConstant = 2;
	}
	else if(mKeyboard->isKeyDown(OIS::KC_7))
	{
		mUpdateMode = SIMULATE_REAL_TIME_MULTIPLE;
		mUpdateConstant = 4;
	}
	else if(mKeyboard->isKeyDown(OIS::KC_8))
	{
		mUpdateMode = SIMULATE_REAL_TIME_MULTIPLE;
		mUpdateConstant = 8;
	}

	// The following code updates the camera's position.
	opal::Vec3r cameraDir;
	bool cameraMoved = false;

	if (mKeyboard->isKeyDown(OIS::KC_LEFT))
	{
		// Move camera left.
		cameraDir[0] -= 1;
		cameraMoved = true;
	}

	if (mKeyboard->isKeyDown(OIS::KC_RIGHT))
	{
		// Move camera right.
		cameraDir[0] += 1;
		cameraMoved = true;
	}

	if (mKeyboard->isKeyDown(OIS::KC_UP))
	{
		// Move camera forward.
		cameraDir[2] -= 1;
		cameraMoved = true;
	}

	if (mKeyboard->isKeyDown(OIS::KC_DOWN))
	{
		// Move camera backward.
		cameraDir[2] += 1;
		cameraMoved = true;
	}

	if (!cameraMoved)
	{
		// Slow physical camera motion if necessary.
	}

	// Use the camera dir vector to translate the camera.
	mPhysicalCamera->moveRelative(cameraDir, dt);

	// Toggle shadows.
	if(mKeyboard->isKeyDown(OIS::KC_H) && toggleTimer <= 0)
	{
		if (mOgreSceneMgr->getShadowTechnique() == Ogre::SHADOWTYPE_NONE)
		{
			mOgreSceneMgr->setShadowTechnique(
				Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		}
		else
		{
			mOgreSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
		}

		toggleTimer = 0.5;
	}

	// Toggle second light source.
	if(mKeyboard->isKeyDown(OIS::KC_L) && toggleTimer <= 0)
	{
		Ogre::Light* light1 = mOgreSceneMgr->getLight("light1");

		if (light1->isVisible())
		{
			light1->setVisible(false);
		}
		else
		{
			light1->setVisible(true);
		}

		toggleTimer = 0.5;
	}

	// Toggle GUI.
	if (mKeyboard->isKeyDown(OIS::KC_G) && toggleTimer <= 0)
	{
		Ogre::Overlay* debugOverlay = Ogre::OverlayManager::getSingleton().
			getByName("Core/DebugOverlay");

		if (debugOverlay->isVisible())
        {
			debugOverlay->hide();
		}
		else
		{
			debugOverlay->show();
		}

		toggleTimer = 0.5;
	}

	// Handy screenshot saving procedure.
	if (mKeyboard->isKeyDown(OIS::KC_SYSRQ) && toggleTimer <= 0)
	{
		static unsigned int count = 0;
		char tmp[20];
		sprintf(tmp, "screenshot_%d.png", count);
		mOgreWindow->writeContentsToFile(tmp);
		mDebugText = Ogre::String("Wrote ") + tmp;
		++count;

		toggleTimer = 0.5;
	}

	return true;
}

bool SimulationEngine::processUnbufferedMouseInput(opal::real dt)
{
	const OIS::MouseState &ms = mMouse->getMouseState();

	// The following code checks how far the mouse has move since 
	// the last poll.  This data can be used to rotate the camera 
	// around its X and Y axes (pitch and yaw, respectively).
	opal::real rotY = -ms.X.rel;
	opal::real rotX = -ms.Y.rel;

	// Use the relative mouse motion to rotate the camera.
	mPhysicalCamera->yawRelative(rotY);
	mPhysicalCamera->pitchRelative(rotX);

	// Check mouse button states.
	if (ms.buttonDown(OIS::MB_Left))
	{
		// The left mouse button is down.
		mPhysicalCamera->grasp();
	}
	else
	{
		// The left mouse button is up.
		mPhysicalCamera->release();
	}

	// Return true to continue looping.
	return true;
}

void SimulationEngine::updateOgreStats()
{
	static Ogre::String currFps = "Current FPS: ";
	static Ogre::String avgFps = "Average FPS: ";
	static Ogre::String bestFps = "Best FPS: ";
	static Ogre::String worstFps = "Worst FPS: ";
	static Ogre::String tris = "Triangle Count: ";

	Ogre::OverlayElement* guiAvg = 
		Ogre::OverlayManager::getSingleton().getOverlayElement(
		"Core/AverageFps");
	Ogre::OverlayElement* guiCurr = 
		Ogre::OverlayManager::getSingleton().getOverlayElement(
		"Core/CurrFps");
	Ogre::OverlayElement* guiBest = 
		Ogre::OverlayManager::getSingleton().getOverlayElement(
		"Core/BestFps");
	Ogre::OverlayElement* guiWorst = 
		Ogre::OverlayManager::getSingleton().getOverlayElement(
		"Core/WorstFps");

	const Ogre::RenderTarget::FrameStats& stats = 
		mOgreWindow->getStatistics();

	guiAvg->setCaption(avgFps + 
		Ogre::StringConverter::toString(stats.avgFPS));
	guiCurr->setCaption(currFps + 
		Ogre::StringConverter::toString(stats.lastFPS));
	guiBest->setCaption(bestFps + 
		Ogre::StringConverter::toString((int)stats.bestFPS) + 
		" " + Ogre::StringConverter::toString((int)stats.bestFrameTime) + 
		" ms");
	guiWorst->setCaption(worstFps + 
		Ogre::StringConverter::toString((int)stats.worstFPS) + 
		" " + Ogre::StringConverter::toString((int)stats.worstFrameTime) + 
		" ms");

	Ogre::OverlayElement* guiTris = 
		Ogre::OverlayManager::getSingleton().getOverlayElement(
		"Core/NumTris");
	guiTris->setCaption(tris + 
		Ogre::StringConverter::toString((unsigned int)stats.triangleCount));

	Ogre::OverlayElement* guiDbg = 
		Ogre::OverlayManager::getSingleton().getOverlayElement(
		"Core/DebugText");
	guiDbg->setCaption(mDebugText);
}
*/

void SimulationEngine::run()
{
	//if (!init())
	//	return;
		
	mOgreRoot->startRendering();

	// clean up
    destroyScene();
}

void SimulationEngine::createFrameListener()
{
 	mFrameListener = new SimulationFrameListener(
 							mOgreWindow, mPhysicalCamera);
   	mFrameListener->showDebugOverlay(true);
   	
   	mFrameListener->hook_simulation(mPhysicalEntityList,
				mSimulator, mOgreSceneMgr, mUpdateConstant, (int)mUpdateMode);

   	mOgreRoot->addFrameListener(mFrameListener);
}

#endif

std::string SimulationEngine::generateUniqueName()
{
	static unsigned int namelessObjectCount = 0;

	// Make a unique name.
	char newName[20];
	sprintf(newName, "object%d", namelessObjectCount);
	std::string nameStr = newName;
	++namelessObjectCount;

	return nameStr;
}
