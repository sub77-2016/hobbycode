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

#ifndef SIMULATION_ENGINE_H
#define SIMULATION_ENGINE_H

// Note: It is possible to use the SimulationEngine without graphics or 
// input (i.e. physics alone).  Simply #define SIMULATION_ENGINE_PHYSICS_ONLY.

#include <opal/opal.h>

#include "PhysicalEntity.h"
#include "SDLViewer.h"
//#include "PhysicalCamera.h"
//#include "Timer.h"

namespace SDLGL { 
	

	/// A general-purpose module for simulating physics (with Opal and ODE), 
	/// handling user input, and drawing 3D graphics (with Ogre).
	class SimulationEngine : public SDLViewer
	{
	public:
		/// Various ways to update the simulation.
		enum UpdateMode
		{
			/// Each update simulates the world ahead by a constant amount of 
			/// time.
			SIMULATE_CONSTANT_CHUNK,

			/// Each update simulates the world ahead by an amount of time 
			/// proportional to the elapsed time since the previous update.
			SIMULATE_REAL_TIME_MULTIPLE
		};

		SimulationEngine(const unsigned int w, const unsigned int h, VideoMode video);

		~SimulationEngine();

		/// Sets up Ogre and Opal.  Returns false if we should quit.
		//bool init(PhysicalCamera::Type cameraType=PhysicalCamera::NON_CLIPPING, 
			//opal::real physicalCameraEyeHeight=2);

		/// Updates input, physics, and graphics.  Returns by reference the 
		/// amount of time by which the world was just simulated (which depends 
		/// on the UpdateType being used) and the actual elapsed (wall clock) 
		/// time since the last update.
		void update(opal::real& elapsedSimTime, opal::real& elapsedRealTime);

		/// Returns true if we should quit the app.  This should be checked 
		/// after each update.
		//bool quitApp();

		/// Returns a pointer to the Opal Simulator.
		opal::Simulator* getSimulator() const;

	#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
		/// Returns a pointer to the Ogre SceneManager.
		//Ogre::SceneManager* getSceneManager()const;

		/// Returns a pointer to the OIS keyboard.  This can be used for 
		/// application-specific user input.
		//OIS::Keyboard* getKeyboard()const;

		/// Returns a pointer to the OIS mouse.  This can be used for 
		/// application-specific user input.
		//OIS::Mouse* getMouse()const;

		/// Returns a pointer to the physical camera.
		//PhysicalCamera* getCamera();

		/// Sets the camera's translation speed.
		//void setCameraMoveSpeed(opal::real speed);

		///// Sets whether frames should be captured after each update and 
		///// saved to files.  This is useful for making videos, for example.  
		///// It is recommended that the update mode be SIMULATE_CONSTANT_CHUNK 
		///// when capturing frames.  Also, note that the runtime performance 
		///// will probably decrease substantially as frames are being captured.
		//void setCaptureFramesEnabled(bool capture);
	#endif

		/// Sets the type of update to use.  The extra real parameter is used 
		/// for the constant amount of time to simulate in seconds (for 
		/// SIMULATE_CONSTANT_CHUNK mode) or for the multiple of the actual 
		/// elapsed time (for SIMULATE_REAL_TIME_MULTIPLE mode).  The constant 
		/// must be greater than zero.
		void setUpdateMode(UpdateMode type, opal::real updateConstant);

		/// Returns the update mode being used.
		UpdateMode getUpdateMode()const;

		/// Returns the update constant being used.
		opal::real getUpdateConstant()const;

		/// Finds a PhysicalEntity pointer by name.  Returns NULL if the 
		/// PhysicalEntity could not be found.
		PhysicalEntity* getPhysicalEntity(const std::string& name)const;

		/// Destroys the given PhysicalEntity.
		void destroyPhysicalEntity(PhysicalEntity* pe);

		/// Destroys all of the PhysicalEntities.
		void destroyAllPhysicalEntities();

		/// Creates and returns a pointer to a PhysicalEntity.  Takes 
		/// the name of the new PhysicalEntity, the material to use, 
		/// and a pointer to an OPAL Solid.  This will automatically 
		/// create primitive visual shapes for all of the Solid's 
		/// physical shapes.
		PhysicalEntity* createPhysicalEntity(const std::string& name, 
			const std::string& materialName, opal::Solid* s);

		///// Creates a PhysicalEntity drawn as a box.  The OPAL Solid can 
		///// be any shape, however.  This is useful for prototyping scenes 
		///// when you don't have a specific visual mesh to use.  If the name 
		///// string is empty, a unique name will be automatically generated; 
		///// otherwise, the given name must be unique.
		//PhysicalEntity* createPhysicalEntityBox(const std::string& name, 
		//	const std::string& materialName, Ogre::Vector3 dimensions, 
		//	opal::Solid* s);

		///// Creates a PhysicalEntity drawn as a sphere.  The OPAL Solid can 
		///// be any shape, however.  This is useful for prototyping scenes 
		///// when you don't have a specific visual mesh to use.  If the name 
		///// string is empty, a unique name will be automatically generated; 
		///// otherwise, the given name must be unique.
		//PhysicalEntity* createPhysicalEntitySphere(const std::string& name, 
		//	const std::string& materialName, Ogre::Real radius, 
		//	opal::Solid* s);

		///// Creates a PhysicalEntity drawn as a capsule.  The OPAL Solid can 
		///// be any shape, however.  This is useful for prototyping scenes 
		///// when you don't have a specific visual mesh to use.  If the name 
		///// string is empty, a unique name will be automatically generated; 
		///// otherwise, the given name must be unique.
		//PhysicalEntity* createPhysicalEntityCapsule(const std::string& name, 
		//	const std::string& materialName, Ogre::Real radius, 
		//	Ogre::Real length, opal::Solid* s);

	protected:
		/// Creates and returns a pointer to a PhysicalEntity without any 
		/// visual representation.  Takes the name of the new PhysicalEntity 
		/// and a pointer to an OPAL Solid.  If the name string is empty, a 
		/// unique name will be automatically generated; otherwise, the given 
		/// name must be unique.
		PhysicalEntity* createNonVisualPhysicalEntity(const std::string& name, 
			opal::Solid* s);

	#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
		/// Creates and returns a pointer to a PhysicalEntity.  Takes 
		/// the name of the new PhysicalEntity, the associated Ogre 
		/// SceneNode, and a pointer to an OPAL Solid.  If the name string 
		/// is empty, a unique name will be automatically generated; 
		/// otherwise, the given name must be unique.
		//PhysicalEntity* createVisualPhysicalEntity(const std::string& name, 
			//Ogre::SceneNode* sn, opal::Solid* s);

		/// Helper function to create an Ogre Entity using the given Opal shape 
		/// data and attach it to the given SceneNode.
		//void createChildVisualEntity(Ogre::SceneNode* parentNode, 
			//const opal::ShapeData* data, const std::string& name, 
			//const std::string& materialName);

		/// Initial scene setup function.  Create and position lights, setup 
		/// shadows, etc.
		//void setupDefaultVisualScene();

		/// Sets whether the grasping visuals are shown.
		void setPickingGraphicsEnabled(bool e);

		/// Updates the picking graphics.
		//void updatePickingGraphics();

		///// Saves the current screen to a file with a unique name.
		//void captureFrame();

		/// Updates the Ogre stats overlay.
		//void updateOgreStats();
	#endif

		/// Returns a unique name string.  Useful when creating lots of 
		/// anonymous objects.
		std::string generateUniqueName();

	#ifndef SIMULATION_ENGINE_PHYSICS_ONLY
		/// Pointer to the Ogre root object.
		//Ogre::Root* mOgreRoot;

		/// Pointer to the Ogre scene manager.
		//Ogre::SceneManager* mOgreSceneMgr;

		/// Pointer to the Ogre window object.
		//Ogre::RenderWindow* mOgreWindow;

		/// The OIS input manager.
		//OIS::InputManager* mInputManager;

		/// The OIS mouse.
		//OIS::Mouse* mMouse;

		/// The OIS keyboard.
		//OIS::Keyboard* mKeyboard;

		/// Pointer to the PhysicalCamera.
		//PhysicalCamera* mPhysicalCamera;

		/// Determines how fast the camera can rotate.
		//opal::real mCameraRotateSpeed;

		/// Determines whether the grasping visuals are shown.
		bool mDrawPickingGraphics;

		///// Determines whether frames are captured after each update.
		//bool mCaptureFramesEnabled;

		/// Used to print out debug info.
		std::string mDebugText;
	#endif

		/// Pointer to the OPAL Simulator.
		opal::Simulator* mSimulator;

		/// A timer used to measure time differences between frames.
		//::Timer mFrameTimer;

		/// Set to true when the app should quit.
		//bool mQuitApp;

		/// True when the physics simulation is paused.
		//bool mPaused;

		/// The type of update being used.
		UpdateMode mUpdateMode;

		/// A constant value used along with the update mode.
		opal::real mUpdateConstant;

		/// Map of named PhysicalEntities.  This is just used to find a 
		/// PhysicalEntity by name.
		std::vector<PhysicalEntity*> mPhysicalEntityList;

		/// Map of named PhysicalEntities.
		std::map<std::string, PhysicalEntity*> mPhysicalEntityMap;

	private:
	};

}

#endif
