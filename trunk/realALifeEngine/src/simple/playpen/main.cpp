/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
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

#include <time.h>
#include "../base/BaseOpalApp.h"

namespace playpen
{
	class PlaypenApp : public opalSamples::BaseOpalApp
	{
	public:
		PlaypenApp();

		~PlaypenApp();

	protected:
		virtual void createScene();

		virtual bool appFrameStarted(opal::real dt);

		virtual bool processUnbufferedKeyInput(Ogre::Real dt);

		virtual bool processUnbufferedMouseInput(Ogre::Real dt);

		/// Creates the initial objects in the world.
		void setupInitialPhysicalEntities();

		/// Builds a wall of boxes centered at the origin.
		void createWall(unsigned int length, unsigned height, 
			const opal::Vec3r& boxDim, 
			const opal::Matrix44r& baseTransform = opal::Matrix44r());

		/// Builds a tower of boxes centered at the origin.  For best 
		/// results, make box length = 2 * box width to helps the 
		/// corners line up correctly.
		void createTower(unsigned int width, unsigned int depth, 
			unsigned height, const opal::Vec3r& boxDim, 
			const opal::Matrix44r& baseTransform = opal::Matrix44r());

	private:
		/// Used to toggle shadows on and off.
		bool mUseShadows;

		/// Point where new objects are created.
		opal::Point3r mCreationPoint;
	};

	PlaypenApp::PlaypenApp()
	: BaseOpalApp()
	{
		mUseShadows = true;
		mCreationPoint.set(-55, 60, -55);
	}

	PlaypenApp::~PlaypenApp()
	{
	}

	void PlaypenApp::createScene()
	{
		// Setup shadows.
		if (mUseShadows)
		{
			mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
		}
		else
		{
			mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
		}

		//mSceneMgr->showBoundingBoxes(true);
		//mSimulator->setStepSize(0.02);
		mSimulator->setGravity(opal::Vec3r(0, -70, 0));

		// Using mesh Shapes requires a higher max contacts.
		mSimulator->setMaxContacts(128);

		// Set the ambient light level.
		mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));

		// Create a light source.
		Ogre::Light* light = mSceneMgr->createLight("light0");
		light->setType(Ogre::Light::LT_POINT);
		light->setDiffuseColour(0.5, 0.5, 0.5);
		light->setSpecularColour(0.5, 0.5, 0.5);
		light->setPosition(100.0, 300.0, 100.0);

		// Create another light source.
		light = mSceneMgr->createLight("light1");
		light->setType(Ogre::Light::LT_POINT);
		light->setDiffuseColour(0.5, 0.5, 0.5);
		light->setSpecularColour(0.5, 0.5, 0.5);
		light->setPosition(-100.0, 300.0, 100.0);
		light->setVisible(false);

		// Setup the initial camera position.
		mPhysicalCamera->setPosition(opal::Point3r(0, 20, 30));
		mPhysicalCamera->lookAt(opal::Point3r(0, 0, 0));

		// Create a skybox.
		mSceneMgr->setSkyBox(true, "Skyboxes/Gray", 5000);

		// Make a sphere used to show the grasp position.
		Ogre::SceneNode* sphereNode = mSceneMgr->getRootSceneNode()->
			createChildSceneNode("pickingSphere0");
		sphereNode->scale(0.3, 0.3, 0.3);
		Entity* sphereEntity = mSceneMgr->createEntity("pickingSphere", 
			"sphere.mesh");
		sphereEntity->setMaterialName("Unlit/Yellow");
		sphereEntity->setCastShadows(false);
		sphereEntity->setVisible(false);
		sphereNode->attachObject(sphereEntity);

		// Make a "line" (scaled cylinder) to show the grasping spring.
		Ogre::SceneNode* lineNode = mSceneMgr->getRootSceneNode()->
			createChildSceneNode("springLine");
		Entity* lineEntity = mSceneMgr->createEntity("springLine", 
			"cylinder.mesh");
		lineEntity->setMaterialName("Unlit/White");
		lineEntity->setCastShadows(false);
		lineEntity->setVisible(false);
		lineNode->attachObject(lineEntity);

		// Make a crosshairs for picking.
		Ogre::Overlay* overlay = 
			OverlayManager::getSingleton().getByName("General/Crosshair");
		overlay->show();

		// Load models, create physical objects, etc. here.

		setupInitialPhysicalEntities();
	}

	bool PlaypenApp::appFrameStarted(opal::real dt)
	{
		// Do per-frame application-specific things here.

		// Update the grasping spring line.
		if (mPhysicalCamera->isGrasping())
		{
			Ogre::Entity* pickingSphere = 
				mSceneMgr->getEntity("pickingSphere");
			if (!pickingSphere->isVisible())
			{
				pickingSphere->setVisible(true);
			}

			Ogre::Entity* springLine = 
				mSceneMgr->getEntity("springLine");
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
				springLineNode->setScale(0.1, 0.1, lineVec.length());
			}
			else
			{
				springLine->setVisible(false);
			}
		}
		else
		{
			Ogre::Entity* pickingSphere = 
				mSceneMgr->getEntity("pickingSphere");
			if (pickingSphere->isVisible())
			{
				pickingSphere->setVisible(false);
			}

			Ogre::Entity* springLine = 
				mSceneMgr->getEntity("springLine");
			if (springLine->isVisible())
			{
				springLine->setVisible(false);
			}
		}

		// Return true to continue looping.
		return true;
	}

	bool PlaypenApp::processUnbufferedKeyInput(Ogre::Real dt)
	{
		// Check if we should quit looping.
		if(mKeyboard->isKeyDown(OIS::KC_ESCAPE) 
			|| mKeyboard->isKeyDown(OIS::KC_Q))
		{
			return false;
		}

		// Check if we should pause physics.
		if(mKeyboard->isKeyDown(OIS::KC_P) && mTimeUntilNextToggle <= 0)
		{
			mPaused = !mPaused;

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.5;
		}

		// Reset the scene.
		if(mKeyboard->isKeyDown(OIS::KC_R))
		{
			// Make sure the PhysicalCamera isn't grabbing anything.
			mPhysicalCamera->release();
			destroyAllPhysicalEntities();
			setupInitialPhysicalEntities();
		}

		// Create various types of objects when the number keys are 
		// pressed.

		// Create a box.
		if(mKeyboard->isKeyDown(OIS::KC_1) && mTimeUntilNextToggle <= 0)
		{
			Ogre::Vector3 boxDim(3, 3, 3);
			opal::Solid* s = mSimulator->createSolid();
			s->setPosition(mCreationPoint);
			opal::BoxShapeData data;
			data.dimensions.set(boxDim[0], boxDim[1], boxDim[2]);
			s->addShape(data);
			createPhysicalEntityBox("", "Plastic/Yellow", boxDim, s);

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.3;
		}

		// Create a sphere.
		if(mKeyboard->isKeyDown(OIS::KC_2) && mTimeUntilNextToggle <= 0)
		{
			Ogre::Real radius = 2;
			opal::Solid* s = mSimulator->createSolid();
			s->setPosition(mCreationPoint);
			opal::SphereShapeData data;
			data.radius = radius;
			s->addShape(data);
			createPhysicalEntitySphere("", "Plastic/Purple", radius, s);

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.3;
		}

		// Create a capsule.
		if(mKeyboard->isKeyDown(OIS::KC_3) && mTimeUntilNextToggle <= 0)
		{
			Ogre::Real radius = 2;
			Ogre::Real length = 5;
			opal::Solid* s = mSimulator->createSolid();
			s->setPosition(mCreationPoint);
			opal::CapsuleShapeData data;
			data.radius = radius;
			data.length = length;
			s->addShape(data);
			createPhysicalEntityCapsule("", "Plastic/Red", radius, length, s);

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.3;
		}

		// Create a cylinder.
		if(mKeyboard->isKeyDown(OIS::KC_4) && mTimeUntilNextToggle <= 0)
		{
			Ogre::Real radius = 3;
			Ogre::Real length = 5;
			opal::Solid* s = mSimulator->createSolid();
			s->setPosition(mCreationPoint);
			opal::CylinderShapeData data;
			data.radius = radius;
			data.length = length;
			s->addShape(data);
			createPhysicalEntityCylinder("", "Plastic/Blue", radius, length, s);

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.3;
		}

		// Create a long box.
		if(mKeyboard->isKeyDown(OIS::KC_5) && mTimeUntilNextToggle <= 0)
		{
			Ogre::Vector3 boxDim(2, 10, 3);
			opal::Solid* s = mSimulator->createSolid();
			s->setPosition(mCreationPoint);
			opal::BoxShapeData data;
			data.dimensions.set(boxDim[0], boxDim[1], boxDim[2]);
			data.material.density = 10;
			s->addShape(data);
			createPhysicalEntityBox("", "Plastic/Green", boxDim, s);

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.3;
		}

		//// Create a log.
		//if(mKeyboard->isKeyDown(OIS::KC_6) && mTimeUntilNextToggle <= 0)
		//{
		//	Ogre::Real logScale = 9;
		//	Ogre::Vector3 boxDim(0.4243, 0.4243, 2);
		//	boxDim *= logScale;
		//	opal::Solid* s = mSimulator->createSolid();
		//	opal::Matrix44r m;
		//	m.rotate(90, 1, 0, 0);
		//	s->setTransform(m);
		//	s->setPosition(mCreationPoint);
		//	opal::BoxShapeData data;
		//	data.dimensions.set(boxDim[0], boxDim[1], boxDim[2]);
		//	s->addShape(data);

		//	std::string name = generateUniqueName();
		//	Ogre::SceneNode* sn = mSceneMgr->getRootSceneNode()->
		//		createChildSceneNode(name);
		//	sn->scale(logScale, logScale, logScale);
		//	Entity* e = mSceneMgr->createEntity(name, "log.mesh");
		//	e->setNormaliseNormals(true);
		//	e->setMaterialName("Textured/Wood");
		//	sn->attachObject(e);
		//	createPhysicalEntity(name, sn, s);

		//	// Reset the timer for toggle keys.
		//	mTimeUntilNextToggle = 0.3;
		//}

		// Create a knot.
		if(mKeyboard->isKeyDown(OIS::KC_6) && mTimeUntilNextToggle <= 0)
		{
			opalSamples::PhysicalEntity* pe = createPhysicalEntityMesh("", 
				"knot.mesh", "Textured/RustedMetal", false, 0.1);
			pe->getSolid()->setPosition(opal::Point3r(0, 40, 0));

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.3;
		}

		// Create a wall.
		if(mKeyboard->isKeyDown(OIS::KC_7) && mTimeUntilNextToggle <= 0)
		{
			opal::Matrix44r m;
			m.rotate(45, 0, 1, 0);
			m.translate(0, 0, -23);
			createWall(6, 8, opal::Vec3r(3, 1.5, 1.5), m);

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.3;
		}

		// Create a tower.
		if(mKeyboard->isKeyDown(OIS::KC_8) && mTimeUntilNextToggle <= 0)
		{
			createTower(2, 2, 15, opal::Vec3r(3, 1.5, 1.5));

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.3;
		}

		// Create a ragdoll.
		if(mKeyboard->isKeyDown(OIS::KC_9) && mTimeUntilNextToggle <= 0)
		{
			opal::Blueprint ragdollBP;
			opal::loadFile(ragdollBP, "../data/blueprints/ragdoll.xml");
			opal::Matrix44r transform;
			transform.translate(mCreationPoint[0], mCreationPoint[1] - 5, 
				mCreationPoint[2]);

			// Instantiate the Blueprint.
			opal::BlueprintInstance instance;
			mSimulator->instantiateBlueprint(instance, ragdollBP, transform, 16);

			unsigned int i=0;
			for (i=0; i<instance.getNumSolids(); ++i)
			{
				opal::Solid* s = instance.getSolid(i);
				const opal::SolidData& data = s->getData();
				unsigned int j=0;
				for (j=0; j<data.getNumShapes(); ++j)
				{
					opal::ShapeData* shapeData = data.getShapeData(j);

					switch(shapeData->getType())
					{
						case opal::BOX_SHAPE:
						{
							opal::Vec3r dim = 
								((opal::BoxShapeData*)shapeData)->dimensions;
							Ogre::Vector3 boxDim(dim[0], dim[1], dim[2]);
							createPhysicalEntityBox("", "Plastic/LightBlue", 
								boxDim, s);
							break;
						}
						case opal::SPHERE_SHAPE:
						{
							opal::real r = 
								((opal::SphereShapeData*)shapeData)->radius;
							createPhysicalEntitySphere("", 
								"Plastic/LightBlue", r, s);
							break;
						}
						case opal::CAPSULE_SHAPE:
						{
							opal::real r = 
								((opal::CapsuleShapeData*)shapeData)->radius;
							opal::real l = 
								((opal::CapsuleShapeData*)shapeData)->length;
							createPhysicalEntityCapsule("", 
								"Plastic/LightBlue", r, l, s);
							break;
						}
						default:
							assert(false);
					}
				}
			}

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.5;
		}

		// The following code updates the camera's position.
		opal::Vec3r cameraDir(0, 0, 0);
		bool cameraMoved = false;

		if (mKeyboard->isKeyDown(OIS::KC_LEFT) 
			|| mKeyboard->isKeyDown(OIS::KC_A))
		{
			// Move camera left.
			cameraDir[0] -= (dt * mMoveSpeed);
			cameraMoved = true;
		}

		if (mKeyboard->isKeyDown(OIS::KC_RIGHT) 
			|| mKeyboard->isKeyDown(OIS::KC_D))
		{
			// Move camera right.
			cameraDir[0] += (dt * mMoveSpeed);
			cameraMoved = true;
		}

		if (mKeyboard->isKeyDown(OIS::KC_UP) 
			|| mKeyboard->isKeyDown(OIS::KC_W))
		{
			// Move camera forward.
			cameraDir[2] -= (dt * mMoveSpeed);
			cameraMoved = true;
		}

		if (mKeyboard->isKeyDown(OIS::KC_DOWN) 
			|| mKeyboard->isKeyDown(OIS::KC_S))
		{
			// Move camera backward.
			cameraDir[2] += (dt * mMoveSpeed);
			cameraMoved = true;
		}

		if (!cameraMoved)
		{
			// Slow physical camera motion if necessary.
		}

		// Use the camera dir vector to translate the camera.
		mPhysicalCamera->moveRelative(cameraDir);

		// Toggle shadows.
		if(mKeyboard->isKeyDown(OIS::KC_H) && mTimeUntilNextToggle <= 0)
		{
			mUseShadows = !mUseShadows;

			if (mUseShadows)
			{
				mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
			}
			else
			{
				mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
			}

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.5;
		}

		// Toggle second light source.
		if(mKeyboard->isKeyDown(OIS::KC_L) && mTimeUntilNextToggle <= 0)
		{
			Ogre::Light* light1 = mSceneMgr->getLight("light1");

			if (light1->isVisible())
			{
				light1->setVisible(false);
			}
			else
			{
				light1->setVisible(true);
			}

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.5;
		}

		// Toggle GUI.
		if (mKeyboard->isKeyDown(OIS::KC_G) && mTimeUntilNextToggle <= 0)
		{
			mStatsOn = !mStatsOn;
			showDebugOverlay(mStatsOn);
			mTimeUntilNextToggle = 1;
		}

		// Handy screenshot saving procedure.
		if (mKeyboard->isKeyDown(OIS::KC_SYSRQ) 
			&& mTimeUntilNextToggle <= 0)
		{
			char tmp[20];
			sprintf(tmp, "screenshot_%d.png", ++mNumScreenShots);
			ExampleApplication::mWindow->writeContentsToFile(tmp);
			mDebugText = String("Wrote ") + tmp;

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.5;
		}

		// Return true to continue looping.
		return true;
	}

	bool PlaypenApp::processUnbufferedMouseInput(Ogre::Real dt)
	{
		// The following code checks how far the mouse has move since 
		// the last poll.  This data can be used to rotate the camera 
		// around its X and Y axes (pitch and yaw, respectively).
		const OIS::MouseState &ms = mMouse->getMouseState();
		Ogre::Degree rotY = -ms.X.rel * mRotateSpeed;
		Ogre::Degree rotX = -ms.Y.rel * mRotateSpeed;

		// Use the relative mouse motion to rotate the camera.
		mPhysicalCamera->yawRelative(rotY.valueDegrees());
		mPhysicalCamera->pitchRelative(rotX.valueDegrees());

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

	void PlaypenApp::setupInitialPhysicalEntities()
	{
		opal::Matrix44r m;

		// Create a static box for a ground plane.
		Ogre::Vector3 boxDim(70, 16, 70);
		opal::Solid* s = mSimulator->createSolid();
		s->setStatic(true);
		s->setPosition(0, -8, 0);
		opal::BoxShapeData data;
		data.dimensions.set(boxDim[0], boxDim[1], boxDim[2]);
		s->addShape(data);
		createPhysicalEntityBox("ground", "Plastic/Gray", boxDim, s);

		// Setup the "object creation tube".
		boxDim.x = 1;
		boxDim.y = 20;
		boxDim.z = 10;
		data.dimensions.set(boxDim[0], boxDim[1], boxDim[2]);

		s = mSimulator->createSolid();
		s->setStatic(true);
		m.makeTranslation(mCreationPoint[0] + 0.5 * boxDim.z, 
			mCreationPoint[1], mCreationPoint[2] + 0.5 * boxDim.x);
		s->setTransform(m);
		s->addShape(data);
		createPhysicalEntityBox("tube0", "Plastic/LightGray", boxDim, s);

		s = mSimulator->createSolid();
		s->setStatic(true);
		m.makeTranslation(mCreationPoint[0] - 0.5 * boxDim.z, 
			mCreationPoint[1], mCreationPoint[2] - 0.5 * boxDim.x);
		s->setTransform(m);
		s->addShape(data);
		createPhysicalEntityBox("tube1", "Plastic/LightGray", boxDim, s);

		s = mSimulator->createSolid();
		s->setStatic(true);
		m.makeRotation(90, 0, 1, 0);
		m.translate(-mCreationPoint[2] + 0.5 * boxDim.z, mCreationPoint[1], 
			mCreationPoint[0] + 0.5 * boxDim.x);
		s->setTransform(m);
		s->addShape(data);
		createPhysicalEntityBox("tube2", "Plastic/LightGray", boxDim, s);

		s = mSimulator->createSolid();
		s->setStatic(true);
		m.makeRotation(90, 0, 1, 0);
		m.translate(-mCreationPoint[2] - 0.5 * boxDim.z, mCreationPoint[1], 
			mCreationPoint[0] - 0.5 * boxDim.x);
		s->setTransform(m);
		s->addShape(data);
		createPhysicalEntityBox("tube3", "Plastic/LightGray", boxDim, s);

		// Make a slide under the object creation tube.
		boxDim.x = 60;
		boxDim.y = 1;
		boxDim.z = 15;
		data.dimensions.set(boxDim[0], boxDim[1], boxDim[2]);
		data.material.friction = 0.00001;
		s = mSimulator->createSolid();
		s->setStatic(true);
		m.makeTranslation(mCreationPoint[0], mCreationPoint[1] - 33, 
			mCreationPoint[2]);
		m.rotate(-45, 0, 1, 0);
		m.translate(0.3 * boxDim.x, 0, 0);
		m.rotate(-30, 0, 0, 1);
		s->setTransform(m);
		s->addShape(data);
		createPhysicalEntityBox("slide", "Plastic/DarkGray", boxDim, s);
	}

	void PlaypenApp::createWall(unsigned int length, unsigned height, 
		const opal::Vec3r& boxDim, const opal::Matrix44r& baseTransform)
	{
		for (unsigned int l=0; l<length; ++l)
		{
			for (unsigned int h=0; h<height; ++h)
			{
				opal::real offset = 0;
				if (h % 2 == 0)
				{
					offset = (opal::real)0.5 * boxDim[0];
				}

				opal::Matrix44r blockTransform = baseTransform;
				blockTransform.translate(l * boxDim[0] + 
					0.5 * boxDim[0] - 0.5 * length * boxDim[0] + 
					offset, h * boxDim[1] + 0.5 * boxDim[1], 0);

				opal::Solid* s = mSimulator->createSolid();
				s->setTransform(blockTransform);
				//s->setSleeping(true);
				opal::BoxShapeData boxData;
				boxData.dimensions = boxDim;
				boxData.material.density = 6;
				boxData.material.hardness = 0.4;
				s->addShape(boxData);
				Ogre::Vector3 boxDimensions(boxDim[0], boxDim[1], boxDim[2]);
				createPhysicalEntityBox("", "Plastic/Orange", boxDimensions, s);
			}
		}
	}

	void PlaypenApp::createTower(unsigned int width, unsigned int depth, 
		unsigned height, const opal::Vec3r& boxDim, 
		const opal::Matrix44r& baseTransform)
	{
		for (unsigned int wall=0; wall<4; ++wall)
		{
			opal::Matrix44r wallBaseTransform = baseTransform;
			unsigned int length = 0;

			switch(wall)
			{
				case 0:
					length = width;
					wallBaseTransform.translate(
						-(opal::real)0.25 * boxDim[0], 0, 
						-(opal::real)0.5 * depth * boxDim[0]);
					break;
				case 1:
					length = width;
					wallBaseTransform.translate(
						(opal::real)0.25 * boxDim[0], 0, 
						(opal::real)0.5 * depth * boxDim[0]);
					wallBaseTransform.rotate(180, 0, 1, 0);
					break;
				case 2:
					length = depth;
					wallBaseTransform.translate(
						-(opal::real)0.5 * width * boxDim[0], 0, 0);
					wallBaseTransform.rotate(90, 0, 1, 0);
					wallBaseTransform.translate(
						-(opal::real)0.25 * boxDim[0], 0, 0);
					break;
				case 3:
					length = depth;
					wallBaseTransform.translate(
						(opal::real)0.5 * width * boxDim[0], 0, 0);
					wallBaseTransform.rotate(-90, 0, 1, 0);
					wallBaseTransform.translate(
						-(opal::real)0.25 * boxDim[0], 0, 0);
					break;
				default:
					assert(false);
					break;
			}

			createWall(length, height, boxDim, wallBaseTransform);
		}
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	playpen::PlaypenApp app;

	try
	{
		app.go();
	}
	catch(Ogre::Exception& e)
	{
		std::cerr << "An exception has occured: " <<
			e.getFullDescription().c_str() << std::endl;
	}

	return 0;
}
