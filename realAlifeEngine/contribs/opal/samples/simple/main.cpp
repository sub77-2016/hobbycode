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

// --------------------------------
// OPAL 'Simple' sample application
// --------------------------------
// This sample application is intended for people who are new to OPAL (or 
// to simulated physics in general).  It starts with a simple scene and 
// allows the user to create new physical objects via keypresses.  This 
// application is very basic and contains extremely verbose comments.  The 
// major steps are numbered for your convenience.  Feel free to use this 
// as a starting point for your own OPAL applications, if you'd like.  
// 
// This application uses OpenGL for rendering and SDL (www.libsdl.org) for 
// cross-platform input handling and window management (note that SDL is 
// required in order to build this sample).  It uses a very minimal set of 
// classes to connect the OPAL Solids with drawable objects ('Entity' is a 
// base class for physically simulated drawable objects, such as the 
// included 'BoxEntity,' 'SphereEntity,' and 'CapsuleEntity' classes.  
// 'Timer' is a simple class used to measure elapsed time between frames.)

#include <time.h>

#include <opal/opal.h>
#include <SDL/SDL.h>

#include "Timer.h"
#include "BoxEntity.h"
#include "SphereEntity.h"
#include "CapsuleEntity.h"

// A global list of all the drawable objects.
std::vector<opalSamples::Entity*> gEntities;

// A global pointer to the OPAL Simulator.
opal::Simulator* gSimulator = NULL;

// Used to determine when to quit.
bool gQuit = false;

// Used to keep track of when the simulation is paused.
bool gPaused = false;

// Dimensions of the ground platform.  Used when creating the ground 
// platform and when determining random positions of new objects.
opal::Vec3r gGroundDimensions((opal::real)2.5, (opal::real)0.5, (opal::real)2.5);

// Creates a window with an OpenGL context.
void setupWindow();

// Destroys the OpenGL window.
void destroyWindow();

// Gets new input from the user.  Returns false if we should quit.
bool processInput();

// Creates a random object at a semi-random location in the scene.
void createRandomObject();

/// Returns a random real value between min and max using a uniform 
/// probability distribution.
opal::real randomRealUniform(opal::real min, opal::real max);

int main(int argc, char **argv)
{
	// 1. Setup a window with an OpenGL context so we can see what's 
	// happening.
	setupWindow();

	// 2. Create a Simulator.  This will enable us to create other OPAL 
	// objects.  As the name implies, it also simulates the interactions 
	// among the various objects it contains.  Also, we set the gravity 
	// vector to be something other than the default (0, 0, 0).
	gSimulator = opal::createSimulator();
	gSimulator->setGravity(opal::Vec3r(0, (opal::real)-9.81, 0));

	// 3. Create a ground platform by making a Solid and an Entity and 
	// connecting them together.  Solids can move around and collide with 
	// other Solids, but they're essentially invisible without some sort 
	// of visual representation.  The Entity class used in this 
	// sample represents drawable objects that can be associated with 
	// Solids, thus making it possible to see how the Solids are moving 
	// and interacting.  (Of course you could run a simulation without 
	// any visuals if you wanted.)  Note that the visual representation 
	// doesn't have to match the physical representation exactly: you 
	// could have an intricate visual mesh for a simulated 
	// television object and use a simple box shape for its physical 
	// representation.  Also note that there is not a strict one-to-one 
	// relationship between Solids and Entities; since a Solid can have 
	// multiple OPAL Shapes attached to it, one Solid could use 
	// multiple Entities: one for each Shape.

	// 3a. Create the Solid for the platform.
	opal::Solid* platformSolid = gSimulator->createSolid();

	// 3b. Make the platform Solid static.  Dynamic Solids can move; static 
	// Solids cannot.
	platformSolid->setStatic(true);

	// 3c. Add a box Shape to the Solid.  Shapes describe a Solid's physical 
	// boundaries.  A Solid can use any number of Shapes, and each Shape 
	// can be offset from the Solid's center.
	opal::BoxShapeData boxShape;
	boxShape.dimensions = gGroundDimensions;
	platformSolid->addShape(boxShape);

	// 3d. Make an Entity for the ground platform, set its visual dimensions 
	// to match the physical box, attach it to the Solid, and add it to our 
	// global list of Entities so it can get drawn.
	opalSamples::BoxEntity* groundEntity = new opalSamples::BoxEntity();
	groundEntity->setDimensions(boxShape.dimensions);
	groundEntity->attachToSolid(platformSolid);
	gEntities.push_back(groundEntity);

	// 4. Make a Timer object to track elapsed time between frames.
	opalSamples::Timer timer;

	// 5. Start the main loop.
	while (!gQuit)
	{
		// 5a. Find out how much time has elapsed since the last time we 
		// were here (i.e. the duration of the previous frame).
		opal::real dt = timer.getTimeMicroseconds() * (opal::real)0.000001;
		timer.reset();

		// 5b. Check for user input.  In this application we only care 
		// about keyboard input for creating new objects, pausing the 
		// simulation, and quitting.
		gQuit = processInput();

		// 5c. Now call 'simulate.'  This will perform collision detection 
		// and advance the simulation ahead by the given amount of time.  
		// We will pass in the duration of the previous frame.  This is 
		// a fairly common way to visualize real-time simulations.  (Note 
		// that internally, the Simulator breaks the given time interval 
		// into small, constant-sized steps and processes them iteratively.  
		// Most real-time physics simulations need constant step sizes in 
		// order to produce stable, repeatable results.)  Note how we can 
		// pause the simulation by simply not telling the Simulator to 
		// advance.
		if (!gPaused)
		{
			gSimulator->simulate(dt);
		}

		// 5d. Clear the OpenGL color and depth buffers, and position/orient 
		// the camera before drawing the Entities.  In this application we 
		// simply set it to the same position/orientation each frame.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		float camPos[3] = {0, 0.5f, 7};
		glTranslatef(-camPos[0], -camPos[1], -camPos[2]);
		glRotatef(10, 1, 0, 0);

		// 5e. Redraw the updated Entities using the latest 
		// positions and orientations of the Solids.
		std::vector<opalSamples::Entity*>::iterator iter;
		for (iter = gEntities.begin(); iter != gEntities.end(); ++iter)
		{
			(*iter)->draw();
		}

		// 5f. Do OpenGL swap buffers (via SDL).
		SDL_GL_SwapBuffers();
	}

	// 6. Close the OpenGL window.
	destroyWindow();

	// 7. Destroy the Entities.
	while (!gEntities.empty())
	{
		delete gEntities.back();
		gEntities.pop_back();
	}

	// 8. Destroy the Simulator.  This will automatically destroy everything 
	// contained within the Simulator.
	gSimulator->destroy();

	return 0;
}

void setupWindow()
{
	// Setup SDL.

	unsigned int windowWidth = 800;
	unsigned int windowHeight = 600;
	float aspectRatio = (float)windowWidth / (float)windowHeight;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL initialization failed: " << SDL_GetError() 
			<< std::endl;
		SDL_Quit();
		return;
	}

	// Enable double buffering.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Surface *SDLSurface = SDL_SetVideoMode(windowWidth, windowHeight, 
		0, SDL_OPENGL|SDL_RESIZABLE);

	if (NULL == SDLSurface)
	{
		fprintf(stderr, "Video mode initialization failed: %s\n", SDL_GetError());
		SDL_Quit();
		return;
	}

	// Set key repeat rate (initial delay and interval between repeats, 
	// both in milliseconds).
	SDL_EnableKeyRepeat(300, 300);

	// Setup OpenGL.

	glClearColor(0.9f, 0.9f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.0f, 0.0f, 0.0f);

	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0f, aspectRatio, 1, 5000.0f);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);

	// Don't draw the back of faces.
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_NORMALIZE);

	// Don't light the back of faces.
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	glEnable(GL_LIGHTING);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Create a light source.
	GLfloat ambient[] = {0.5f, 0.5f, 0.3f, 1.0f};
	GLfloat diffuse[] = {0.9f, 0.9f, 0.9f, 1.0f};
	GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_position[] = {-50, 10, 50, 0.0f};
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void destroyWindow()
{
	if(SDL_WasInit(SDL_INIT_VIDEO))
	{
		SDL_Quit();
	}
}

bool processInput()
{
	bool quit = false;

	SDL_Event event;

	// Wandle events until the SDL event queue is empty.
	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_q:
						quit = true;
						break;
					case SDLK_p:
						// Pauses the simulation.
						gPaused = !gPaused;
						break;
					case SDLK_SPACE:
						createRandomObject();
						break;
					default:
						break;
				}
				break;
			}
			case SDL_QUIT:
			{
				quit = true;
				break;
			}
			default:
				break;
		}
	}

	return quit;
}

void createRandomObject()
{
	// Seed the random number generator (if it hasn't already happened) 
	// with the current time.
	static bool seeded = false;
	if (!seeded)
	{
		srand((unsigned int)time(NULL));
		seeded = true;
	}

	// Create a new Solid.
	opal::Solid* solid = gSimulator->createSolid();

	opalSamples::Entity* entity = NULL;

	// Choose a random initial position and orientation.
	opal::Matrix44r transform;
	opal::real xMax = (opal::real)0.5 * gGroundDimensions[0];
	opal::real zMax = (opal::real)0.5 * gGroundDimensions[2];
	transform.translate(randomRealUniform(-xMax, xMax), 
		randomRealUniform((opal::real)1.5, 3), randomRealUniform(-zMax, zMax));
	transform.rotate(randomRealUniform(0, 360), 1, 0, 0);
	transform.rotate(randomRealUniform(0, 360), 0, 1, 0);
	transform.rotate(randomRealUniform(0, 360), 0, 0, 1);
	solid->setTransform(transform);

	// Choose a random type of object.
	unsigned int numObjectTypes = 3;
	int objectType = (int)(numObjectTypes * rand() / (RAND_MAX + 1.0));

	// Choose a random color for the object.
	opal::Vec3r color(randomRealUniform(0, (opal::real)0.9), 
		randomRealUniform(0, (opal::real)0.9), 
		randomRealUniform(0, (opal::real)0.9));

	// Add Shapes to the Solid and attach an Entity.  The Shapes and 
	// visual representation depend on the object type.
	switch(objectType)
	{
		case 0:
		{
			// Simple box.
			opal::BoxShapeData boxShape;
			boxShape.dimensions[0] = randomRealUniform((opal::real)0.1, 1);
			boxShape.dimensions[1] = randomRealUniform((opal::real)0.1, 1);
			boxShape.dimensions[2] = randomRealUniform((opal::real)0.1, 1);
			solid->addShape(boxShape);

			entity = new opalSamples::BoxEntity();
			((opalSamples::BoxEntity*)entity)->setDimensions(
				boxShape.dimensions);
			break;
		}
		case 1:
		{
			// Simple sphere.
			opal::SphereShapeData sphereShape;
			sphereShape.radius = randomRealUniform((opal::real)0.03, 
				(opal::real)0.3);
			solid->addShape(sphereShape);

			entity = new opalSamples::SphereEntity();
			((opalSamples::SphereEntity*)entity)->setRadius(
				sphereShape.radius);
			break;
		}
		case 2:
		{
			// Simple capsule.
			opal::CapsuleShapeData capsuleShape;
			capsuleShape.radius = randomRealUniform((opal::real)0.03, 
				(opal::real)0.3);
			capsuleShape.length = randomRealUniform((opal::real)0.1, 1);
			solid->addShape(capsuleShape);

			entity = new opalSamples::CapsuleEntity();
			((opalSamples::CapsuleEntity*)entity)->setDimensions(
				capsuleShape.radius, capsuleShape.length);
			break;
		}
		default:
			assert(false);
	}

	// Set the new Entity's color.
	entity->setColor(color[0], color[1], color[2]);

	// Attach the new Entity to the Solid.
	entity->attachToSolid(solid);

	// Add the new Entity to the global list so it can get drawn.
	gEntities.push_back(entity);
}

opal::real randomRealUniform(opal::real min, opal::real max)
{
	return (opal::real)rand() / RAND_MAX * (max - min) + min;
}
