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

#ifndef OPAL_SAMPLES_BASE_OPAL_APP_H
#define OPAL_SAMPLES_BASE_OPAL_APP_H

#include "ExampleApplication.h"
#include "ExampleFrameListener.h"
#include "PhysicalEntity.h"
#include "PhysicalCamera.h"
#include <opal/opal.h>

namespace opalSamples
{
	class BaseOpalApp : public ExampleApplication, public ExampleFrameListener
	{
	public:
		BaseOpalApp()
		: ExampleApplication(), ExampleFrameListener()
		{
			mSimulator = opal::createSimulator();
			mSimulator->setGravity(opal::Vec3r(0, (opal::real)-9.81, 0));
			// The physical camera gets created later after the 
			// ExampleApplication has created its Ogre camera.
			mPhysicalCamera = NULL;
			mPaused = false;
		}

		~BaseOpalApp()
		{
			delete mPhysicalCamera;
			destroyAllPhysicalEntities();
			mSimulator->destroy();
		}

		/// Creates and returns a pointer to a PhysicalEntity.  Takes 
		/// the name of the new PhysicalEntity, the associated Ogre 
		/// SceneNode, and a pointer to an OPAL Solid.  If the name string 
		/// is empty, a unique name will be automatically generated; 
		/// otherwise, the given name must be unique.
		PhysicalEntity* createPhysicalEntity(const std::string& name, 
			Ogre::SceneNode* sn, opal::Solid* s)
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

			// Create a new Physical Entity.
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

		/// Creates a PhysicalEntity drawn as a box.  The OPAL Solid can 
		/// be any shape, however.  This is useful for prototyping scenes 
		/// when you don't have a specific visual mesh to use.  If the name 
		/// string is empty, a unique name will be automatically generated; 
		/// otherwise, the given name must be unique.
		PhysicalEntity* createPhysicalEntityBox(const std::string& name, 
			const std::string& materialName, Ogre::Vector3 dimensions, 
			opal::Solid* s)
		{
			PhysicalEntity* pe = NULL;

			std::string nameStr = name;
			if (nameStr.empty())
			{
				nameStr = generateUniqueName();
			}

			// Create an Ogre SceneNode for the Entity.
			Ogre::SceneNode* sn = mSceneMgr->getRootSceneNode()->
				createChildSceneNode(nameStr);

			// Scale the object according to the given dimensions.
			sn->scale(dimensions[0], dimensions[1], 
				dimensions[2]);

			// Create an Ogre Entity using a cube mesh.  This mesh must be 
			// stored as a box with dimensions 1x1x1.
			Entity* e = mSceneMgr->createEntity(nameStr, "cube.mesh");
			e->setMaterialName(materialName);

			// Keep the normals normalized even after scaling.
			e->setNormaliseNormals(true);

			// Attach the Entity to the SceneNode.
			sn->attachObject(e);

			pe = createPhysicalEntity(nameStr, sn, s);

			return pe;
		}

		/// Creates a PhysicalEntity drawn as a sphere.  The OPAL Solid can 
		/// be any shape, however.  This is useful for prototyping scenes 
		/// when you don't have a specific visual mesh to use.  If the name 
		/// string is empty, a unique name will be automatically generated; 
		/// otherwise, the given name must be unique.
		PhysicalEntity* createPhysicalEntitySphere(const std::string& name, 
			const std::string& materialName, Ogre::Real radius, 
			opal::Solid* s)
		{
			PhysicalEntity* pe = NULL;

			std::string nameStr = name;
			if (nameStr.empty())
			{
				nameStr = generateUniqueName();
			}

			// Create an Ogre SceneNode for the Entity.
			Ogre::SceneNode* sn = mSceneMgr->getRootSceneNode()->
				createChildSceneNode(nameStr);

			// Scale the object according to the given dimensions.
			sn->scale(radius, radius, radius);

			// Create an Ogre Entity using a sphere mesh.  This mesh must be 
			// stored as a sphere with radius 1.
			Entity* e = mSceneMgr->createEntity(nameStr, "sphere.mesh");
			e->setMaterialName(materialName);

			// Keep the normals normalized even after scaling.
			e->setNormaliseNormals(true);

			// Attach the Entity to the SceneNode.
			sn->attachObject(e);

			pe = createPhysicalEntity(nameStr, sn, s);

			return pe;
		}

		/// Creates a PhysicalEntity drawn as a capsule.  The OPAL Solid can 
		/// be any shape, however.  This is useful for prototyping scenes 
		/// when you don't have a specific visual mesh to use.  If the name 
		/// string is empty, a unique name will be automatically generated; 
		/// otherwise, the given name must be unique.
		PhysicalEntity* createPhysicalEntityCapsule(const std::string& name, 
			const std::string& materialName, Ogre::Real radius, 
			Ogre::Real length, opal::Solid* s)
		{
			PhysicalEntity* pe = NULL;

			std::string nameStr = name;
			if (nameStr.empty())
			{
				nameStr = generateUniqueName();
			}

			// Create Ogre Entities using cylinder and sphere meshes.  

			// Create an Ogre SceneNode for the cylinder Entity.
			std::string subObjectName = "cylinder" + nameStr;
			Ogre::SceneNode* cylinderNode = mSceneMgr->getRootSceneNode()->
				createChildSceneNode(subObjectName);

			// Scale the object according to the given dimensions.  This 
			// will also scale the transforms for the child nodes, but 
			// we disable the "inherit scale" option for child nodes 
			// here so the shapes themselves don't get scaled.
			cylinderNode->scale(radius, radius, length);

			// This mesh must be stored as a cylinder with length 1 and 
			// radius 1.
			Entity* e = mSceneMgr->createEntity(subObjectName, 
				"cylinder.mesh");
			e->setMaterialName(materialName);
			e->setNormaliseNormals(true);
			cylinderNode->attachObject(e);

			// The spheres must use separate scene nodes that are offset 
			// from the cylinder's scene node.

			// This mesh must be stored as a sphere with radius 1.
			subObjectName = "sphere0" + nameStr;
			Ogre::SceneNode* sn = 
				cylinderNode->createChildSceneNode(subObjectName);
			sn->setInheritScale(false);
			sn->translate(0, 0, -0.5);
			sn->scale(radius, radius, radius);
			e = mSceneMgr->createEntity(subObjectName, "sphere.mesh");
			e->setMaterialName(materialName);
			e->setNormaliseNormals(true);
			sn->attachObject(e);

			subObjectName = "sphere1" + nameStr;
			sn = cylinderNode->createChildSceneNode(subObjectName);
			sn->setInheritScale(false);
			sn->translate(0, 0, 0.5);
			sn->scale(radius, radius, radius);
			e = mSceneMgr->createEntity(subObjectName, "sphere.mesh");
			e->setMaterialName(materialName);
			e->setNormaliseNormals(true);
			sn->attachObject(e);

			pe = createPhysicalEntity(nameStr, cylinderNode, s);

			return pe;
		}

		/// Creates a PhysicalEntity drawn as a cylinder.  The OPAL Solid can 
		/// be any shape, however.  This is useful for prototyping scenes 
		/// when you don't have a specific visual mesh to use.  If the name 
		/// string is empty, a unique name will be automatically generated; 
		/// otherwise, the given name must be unique.
		PhysicalEntity* createPhysicalEntityCylinder(const std::string& name, 
			const std::string& materialName, Ogre::Real radius, 
			Ogre::Real length, opal::Solid* s)
		{
			PhysicalEntity* pe = NULL;

			std::string nameStr = name;
			if (nameStr.empty())
			{
				nameStr = generateUniqueName();
			}

			// Create Ogre Entities using a cylinder meshes.  

			// Create an Ogre SceneNode for the cylinder Entity.
			std::string subObjectName = "cylinder" + nameStr;
			Ogre::SceneNode* cylinderNode = mSceneMgr->getRootSceneNode()->
				createChildSceneNode(subObjectName);

			// Scale the object according to the given dimensions.  This 
			// will also scale the transforms for the child nodes, but 
			// we disable the "inherit scale" option for child nodes 
			// here so the shapes themselves don't get scaled.
			cylinderNode->scale(radius, radius, length);

			// This mesh must be stored as a cylinder with length 1 and 
			// radius 1.
			Entity* e = mSceneMgr->createEntity(subObjectName, 
				"cylinder.mesh");
			e->setMaterialName(materialName);
			e->setNormaliseNormals(true);
			cylinderNode->attachObject(e);

			pe = createPhysicalEntity(nameStr, cylinderNode, s);

			return pe;
		}

		/// Creates a PhysicalEntity with a single mesh shape, drawn as a 
		/// mesh.  If the name string is empty, a unique name will be 
		/// automatically generated; otherwise, the given name must be 
		/// unique.
		PhysicalEntity* createPhysicalEntityMesh(const std::string& name, 
			const std::string& meshName, const std::string& materialName, 
			bool isStatic, opal::real scale)
		{
			std::string nameStr = name;
			if (nameStr.empty())
			{
				nameStr = generateUniqueName();
			}

			// Create an Ogre Entity using the given mesh.
			Entity* e = mSceneMgr->createEntity(nameStr, meshName);
			e->setMaterialName(materialName);

			opal::MeshShapeData meshData;

			// We need to get vertex and triangle data from the Ogre 
			// Entity for our OPAL mesh Shape.  Note: The vertex array 
			// returned by the Entity's submesh contains some redundant 
			// vertices.

			// The following code is adapted from code given by DWORD and 
			// KingPrawnVindaloo in the Ogre forums in the thread entitled 
			// "Vertex Data - Possible Bug?"

			// Note that this code assumes there is only one submesh in the 
			// Entity.
			SubMesh* subMesh = e->getMesh()->getSubMesh(0);

			VertexData* vertexData = NULL;
			if (subMesh->useSharedVertices)
			{
				vertexData = e->getMesh()->sharedVertexData;
			}
			else
			{
				vertexData = subMesh->vertexData;
			}

			// Get the vertex and triangle counts.
			meshData.numVertices = (unsigned int)vertexData->vertexCount;
			meshData.numTriangles = 
				(unsigned int)subMesh->indexData->indexCount / 3;

			// Allocate vertex and triangle arrays.
			meshData.vertexArray = 
				new opal::real[3 * meshData.numVertices];
			meshData.triangleArray = 
				new unsigned int[3 * meshData.numTriangles];

			const VertexElement* elem = vertexData->
				vertexDeclaration->findElementBySemantic(VES_POSITION);
			HardwareVertexBufferSharedPtr vbuf = vertexData->
				vertexBufferBinding->getBuffer(elem->getSource());

			// The following buffer might contain normals, textured 
			// coordinates, etc. in addition to vertex positions, so 
			// we need to be careful when iterating through it.
			unsigned char* vertexPointer = static_cast<unsigned char*>
				(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));

			// Loop over the vertices in this submesh.
			size_t vertexSize = vbuf->getVertexSize();
			for (unsigned j = 0; j < meshData.numVertices; ++j)
			{
				// Get a pointer to the base address of the vertex.
				float* tempFloat;
				elem->baseVertexPointerToElement(vertexPointer, &tempFloat);

				meshData.vertexArray[j * 3] = tempFloat[0] * scale;
				meshData.vertexArray[j * 3 + 1] = tempFloat[1] * scale;
				meshData.vertexArray[j * 3 + 2] = tempFloat[2] * scale;

				// Move to the next vertex in the array.
				vertexPointer += vertexSize;
			}

			vbuf->unlock();

			HardwareIndexBufferSharedPtr ibuf = 
				subMesh->indexData->indexBuffer;
			unsigned short* indexData = 
				new unsigned short[meshData.numTriangles * 3];

			ibuf->readData(0, ibuf->getSizeInBytes(), indexData);

			// Copy index data.  'indices' is a 1-dimensional array.
			for (unsigned int j = 0; j < meshData.numTriangles; ++j)
			{
				meshData.triangleArray[j * 3] = indexData[j * 3];
				meshData.triangleArray[j * 3 + 1] = indexData[j * 3 + 1];
				meshData.triangleArray[j * 3 + 2] = indexData[j * 3 + 2];
			}

			delete[] indexData;

			// Create an OPAL Solid for the mesh.
			opal::Solid* s = mSimulator->createSolid();
			s->setStatic(isStatic);

			// Add the mesh Shape to the Solid.
			s->addShape(meshData);

			// Create an Ogre SceneNode for the Entity.
			Ogre::SceneNode* sn = mSceneMgr->getRootSceneNode()->
				createChildSceneNode(nameStr);

			// Scale the object.
			sn->scale(scale, scale, scale);

			// Keep the normals normalized even after scaling.
			e->setNormaliseNormals(true);

			// Attach the Entity to the SceneNode.
			sn->attachObject(e);

			PhysicalEntity* pe = createPhysicalEntity(nameStr, sn, s);

			if (isStatic)
			{
				// The Ogre SceneNode will only get updated for dynamic Solid, 
				// so we need to update it here.
				pe->updateOgreSceneNode();
			}

			return pe;
		}

		/// Finds a PhysicalEntity pointer by name.  Returns NULL if the 
		/// PhysicalEntity could not be found.
		PhysicalEntity* getPhysicalEntity(const std::string& name)const
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

		/// Destroys the given PhysicalEntity.
		void destroyPhysicalEntity(PhysicalEntity* pe)
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
					opal::Solid* s = pe->getSolid();

					// If any OPAL mesh Shapes are being used, we need to 
					// destroy the mesh data here.  Note that this assumes 
					// that no other OPAL Solids with meshes are using this 
					// data.
					opal::SolidData solidData = s->getData();
					for (unsigned int j = 0; j < solidData.getNumShapes(); ++j)
					{
						if (solidData.getShapeData(j)->getType() == 
							opal::MESH_SHAPE)
						{
							opal::MeshShapeData* meshData = 
								(opal::MeshShapeData*)solidData.getShapeData(j);

							// Destroy vertex array.
							//for (unsigned int v = 0; v < meshData->numVertices; 
							//	++v)
							//{
							//	delete [] meshData->vertexArray[v];
							//}
							delete [] meshData->vertexArray;

							// Destroy triangle array.
							//for (unsigned int t = 0; t < meshData->numTriangles; 
							//	++t)
							//{
							//	delete [] meshData->triangleArray[t];
							//}
							delete [] meshData->triangleArray;
						}
					}

					// Destroy the PhysicalEntity's OPAL Solid.
					mSimulator->destroySolid(s);

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
						MovableObject* thisObject = 
							sn->detachObject(static_cast<unsigned short>(0));
						std::string s = thisObject->getName();
						mSceneMgr->destroyEntity(thisObject->getName());
					}

					// Destroy the SceneNodes all of its children.
					sn->removeAndDestroyAllChildren();
					mSceneMgr->getRootSceneNode()->removeAndDestroyChild(
						sn->getName());
					sn = NULL;

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

		/// Destroys all of the PhysicalEntities.
		void destroyAllPhysicalEntities()
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

	protected:
		/// Create a PhysicalCamera that uses the ExampleApplication's 
		/// Ogre camera.
		virtual void createPhysicalCamera()
		{
			mPhysicalCamera = new PhysicalCamera(mSimulator, 
				ExampleApplication::mCamera, false);
		}

		/// Register the frame listener with the Ogre root so it will 
		/// call the listener at the appropriate time.
		virtual void createFrameListener()
		{
			ExampleFrameListener::init(ExampleApplication::mWindow, 
				ExampleApplication::mCamera);
			mRoot->addFrameListener(this);
		}

		/// Initial scene setup function.  Create and position lights, load 
		/// models, create a camera, etc.
		virtual void createScene() = 0;

		/// Called once per frame after input and physics have been updated.  
		/// This is a place for applications to do application-specific 
		/// updates per frame.  It takes the dt since the last frame as a 
		/// parameter.  This should return false if we should break out of 
		/// the main loop.
		virtual bool appFrameStarted(opal::real dt) = 0;

		/// Called at the start of each frame.  Calculate dt since the last 
		/// frame, update input, update physics, etc.  Returns false if we 
		/// should break out of the main loop.  Applications should not 
		/// override this; instead, they should override 'appFrameStarted'.
		virtual bool frameStarted(const FrameEvent& evt)
		{
			if(ExampleApplication::mWindow->isClosed())
			{
				return false;
			}

			Ogre::Real dt = evt.timeSinceLastFrame;

			if (false == handleInput(dt))
			{
				return false;
			}

			if (!mPaused)
			{
				// Simulate physics.
				mSimulator->simulate(dt);

				// Update the PhysicalEntities.
				size_t size = mPhysicalEntityList.size();
				for(size_t i = 0; i<size; ++i)
				{
					mPhysicalEntityList.at(i)->update(dt);
				}
			}

			// Update the PhysicalCamera.
			mPhysicalCamera->update(dt);

			// Notify the application about the new frame.
			if (false == appFrameStarted((opal::real)dt))
			{
				return false;
			}

			return true;
		}

		/// Returns false if we should break out of the main loop.
		bool handleInput(Ogre::Real dt)
		{
			// This variable can be used to keep keys from repeating too fast.
			if (mTimeUntilNextToggle >= 0)
			{
				mTimeUntilNextToggle -= dt;
			}

			// Acquire data from input devices.
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

		/// Process keyboard input here.  Returns false if we should break out 
		/// of the main loop.
		virtual bool processUnbufferedKeyInput(Ogre::Real dt) = 0;

		/// Process mouse input here.  Returns false if we should break out of 
		/// the main loop.
		virtual bool processUnbufferedMouseInput(Ogre::Real dt) = 0;

		/// Returns a unique name string.  Useful when creating lots of 
		/// anonymous objects.
		std::string generateUniqueName()
		{
			static unsigned int namelessObjectCount = 0;

			// Make a unique name.
			char newName[20];
			sprintf(newName, "object%d", namelessObjectCount);
			std::string nameStr = newName;
			++namelessObjectCount;

			return nameStr;
		}

		/// Pointer to the OPAL Simulator.
		opal::Simulator* mSimulator;

		/// Pointer to the PhysicalCamera.
		PhysicalCamera* mPhysicalCamera;

		/// True when the physics simulation is paused.
		bool mPaused;

		/// Map of named PhysicalEntities.  This is just used to find a 
		/// PhysicalEntity by name.
		std::vector<PhysicalEntity*> mPhysicalEntityList;

		/// Map of named PhysicalEntities.
		std::map<std::string, PhysicalEntity*> mPhysicalEntityMap;

	private:
	};
}

#endif
