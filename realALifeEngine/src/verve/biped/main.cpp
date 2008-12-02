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

/* -- Include the precompiled libraries -- */
#ifdef WIN32
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opal-ode_d.lib")
#pragma comment(lib, "ode_d.lib")
#pragma comment(lib, "verve.lib")
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>

//#include <GL/gl.h>
//#include <GL/glu.h>

#include "Timer.h"
#include "Biped.h"
//#include "CarObject.h"
#include <opal/opal.h>
#include <opal/Matrix44r.h>
#include <SDL/SDL.h>

#define SDL_SYSTEMS_USED SDL_INIT_VIDEO
#define INITIAL_WINDOW_WIDTH 800
#define INITIAL_WINDOW_HEIGHT 600

bool gPaused=false;
int gWindowWidth;
int gWindowHeight;
float gAspectRatio;
float gCameraRotX;
float gCameraRotY;
float gCameraZoom;
float gCameraYPos;
int gOldMousePos[2];
bool gMouseButtonsPressed[2];
opal::Simulator* gSimulator=NULL;
std::vector<Base3DObject*> gObjects;
Timer gTimer;
opal::Solid* gGround=NULL;
opal::real gGroundParams[4] = {0.0, 1.0, 0.0, 0.0};
GLuint gGroundDispList=0;
Biped* gBiped=NULL;
//CarObject* gCar=NULL;
int gCameraMode=0;

void InitSDL();
void ShutdownSDL();
void InitOpenGL();
void InitializeVariables();
void MainLoop();
void Evaluate( Base3DObject* object );

/* Inline Random Functions */
inline bool randbit() {return rand()%2;}
inline int randposneg()
{
  if (randbit()) return 1; 
  else return -1;
}

inline int randint(int x,int y) {return rand()%(y-x+1)+x;}

//Return random value between 0.0 and 1.0.
inline double randfloat() {return (rand())/(RAND_MAX+1.0);}

int _main(int argc, char **argv)
{
	InitializeVariables();
	gCameraRotX = 10.0;
	gCameraRotY = 0.0;
	gCameraZoom = -5.0;
	gCameraYPos = 1.0;
	gSimulator = opal::createSimulator( opal::Vec3r( (opal::real)0.0, (opal::real)-9.81, 
		(opal::real)0.0 ) );
	gSimulator->setAccuracy( opal::ACCURACY_VERY_HIGH );
	//gSimulator->setLinearDamping( 0.0 );
	//gSimulator->setAngularDamping( 0.0 );

	gGround = gSimulator->createSolid( true );
	gGround->addPlane( gGroundParams );

	////create ramp
	//opal::Matrix44r rampTransform;
	//rampTransform.translate( 0.0, 0.0, -60.0 );
	//rampTransform.rotate( -78.0, 1.0, 0.0, 0.0 );
	//BoxObject* box = new BoxObject( gSimulator, true, opal::Vec3r( 6.0, 65.0, 1.0 ), 
	//	opal::defaults::material, rampTransform );
	//gObjects.push_back( box );

	opal::Matrix44r bipedTransform;
	bipedTransform.makeTranslation( 0.0, 1.1, 0.0 );
	gBiped = new Biped( gSimulator, 1.0, bipedTransform );
	gObjects.push_back( gBiped );

	//car 1
	//opal::Matrix44r carTransform;
	//carTransform.makeTranslation( 0.0, 1.8, 10.0 );
	//gCar = new CarObject( gSimulator, 1.0, carTransform );
	//gObjects.push_back( gCar );

	////car 2
	//carTransform.translate( -20.0, 0.0, 0.0 );
	//CarObject* car = new CarObject( gSimulator, 1.0, carTransform );
	//car->getNetwork()->addNoise( 3.0 );
	//gObjects.push_back( car );

	////car 3
	//carTransform.translate( 10.0, 0.0, 0.0 );
	//car = new CarObject( gSimulator, 1.0, carTransform );
	//car->getNetwork()->addNoise( 3.0 );
	//gObjects.push_back( car );

	////car 4
	//carTransform.translate( 20.0, 0.0, 0.0 );
	//car = new CarObject( gSimulator, 1.0, carTransform );
	//car->getNetwork()->addNoise( 3.0 );
	//gObjects.push_back( car );

	////car 5
	//carTransform.translate( 10.0, 0.0, 0.0 );
	//car = new CarObject( gSimulator, 1.0, carTransform );
	//car->getNetwork()->addNoise( 3.0 );
	//gObjects.push_back( car );

	//std::cout << "==============================================" << std::endl;
	//std::cout << "   OPAL Car Test" << std::endl;
	//std::cout << "==============================================" << std::endl;
	//std::cout << "Press 1-9 to create various objects" << std::endl;
	//std::cout << "Use the arrow keys to move" << std::endl;
	//std::cout << "Press 'p' to pause" << std::endl;
	//std::cout << "Press 'q' to quit" << std::endl;
	//std::cout << "==============================================" << std::endl;
	//std::cout << std::endl;

	gTimer.Reset();
	InitSDL();
	MainLoop();
	ShutdownSDL();

	glDeleteLists( gGroundDispList, 1 );

	while (!gObjects.empty())
	{
		delete gObjects.back();
		gObjects.pop_back();
	}

	delete gSimulator;

	return 0;
}

//void Evaluate( Base3DObject* object )
//{
//	opal::Point3r pos = ((CarObject*)object)->getSolid()->getPosition();
//	opal::real r = pos[2];
//	r = -r;
//	r /= 300.0;
//	object->setReinforcement( r );
//}

void InitializeVariables()
{
	gWindowWidth = INITIAL_WINDOW_WIDTH;
	gWindowHeight = INITIAL_WINDOW_HEIGHT;
	gAspectRatio = (float)gWindowWidth/(float)gWindowHeight;
	gOldMousePos[0] = 0;
	gOldMousePos[1] = 0;
	gMouseButtonsPressed[0] = false;
	gMouseButtonsPressed[1] = false;
}

void InitSDL()
{
	//Initialize SDL
	if (SDL_Init(SDL_SYSTEMS_USED) < 0)
	{
		fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
		SDL_Quit();
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  //0 or 1, enable or disable double buffering 

	InitializeVariables();

	SDL_Surface *SDLSurface = SDL_SetVideoMode(gWindowWidth, gWindowHeight, 0, SDL_OPENGL|SDL_RESIZABLE);

	if (NULL == SDLSurface)
	{
		fprintf(stderr, "Video mode initialization failed: %s\n", SDL_GetError());
		SDL_Quit();
		return;
	}

	InitOpenGL();

	//SDL_ShowCursor(SDL_ENABLE);
	//SDL_SetEventFilter(E_FILTER);
	SDL_EnableKeyRepeat(50,50); //delay, interval; both in milliseconds 
}

void ShutdownSDL()
{
	if(0 != SDL_WasInit(SDL_SYSTEMS_USED))
	{
		SDL_Quit();
	}
}

void InitOpenGL()
{
	//glClearColor(0.9, 0.9, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(4.0);

	glViewport(0, 0, gWindowWidth, gWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, gAspectRatio, 1, 5000.0);
	glMatrixMode(GL_MODELVIEW);

	//glEnable(GL_TEXTURE_2D);	// Turn on texture mapping
	glEnable(GL_DEPTH_TEST);

	glCullFace(GL_BACK); // don't draw the back of faces
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0); //don't light the back of faces
	glEnable(GL_LIGHTING);
	//glEnable(GL_NORMALIZE);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//create light
	GLfloat ambient[] =  {0.5f, 0.5f, 0.3f, 1.0f};
	GLfloat diffuse[] =  {0.9f, 0.9f, 0.9f, 1.0f};
	GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_position[] = {-30, 10, 30, 0.0};

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//create ground display list
	gGroundDispList = glGenLists(1);
	glNewList( gGroundDispList, GL_COMPILE );
		glBegin(GL_QUADS);
			glVertex3f(-2000.0, gGroundParams[3], -2000.0);
			glVertex3f(-2000.0, gGroundParams[3], 2000.0);
			glVertex3f(2000.0, gGroundParams[3], 2000.0);
			glVertex3f(2000.0, gGroundParams[3], -2000.0);
		glEnd();

		float matAmbientAndDiffuseBlue[4] = {.1, .1, .8, 1};
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbientAndDiffuseBlue);

		glBegin(GL_LINES);
			for (int i=-1000; i<1000; i+=10)
			{
				glVertex3f(-1000.0, 0.001, (float)i);
				glVertex3f(1000.0, 0.001, (float)i);
				glVertex3f((float)i, 0.001, -1000.0);
				glVertex3f((float)i, 0.001, 1000.0);
			}
		glEnd();
	glEndList();
}

void MainLoop()
{
	bool quit = false;

	while(false == quit)
	{
		SDL_Event event;

		while (SDL_PollEvent(&event)) //handle events until event queue is empty
		{
			switch(event.type)
			{
				case SDL_MOUSEBUTTONDOWN:
				{
					//std::cout << "Mouse button down" << std::endl;

					int clickPosX = event.button.x;
					int clickPosY = gWindowHeight - event.button.y;

					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
							//if (event.button.state == SDL_PRESSED)
							//{
								gMouseButtonsPressed[0] = true;
								gOldMousePos[0] = clickPosX;
								gOldMousePos[1] = clickPosY;
							//}
							break;
						case SDL_BUTTON_RIGHT:
							//if (event.button.state == SDL_PRESSED)
							//{
								gMouseButtonsPressed[1] = true;
								gOldMousePos[0] = clickPosX;
								gOldMousePos[1] = clickPosY;
							//}
							break;
						case SDL_BUTTON_MIDDLE:
							break;
						default:				
							break;
					}
					break;
				}
				case SDL_MOUSEBUTTONUP:
				{
					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
							gMouseButtonsPressed[0] = false;
							break;
						case SDL_BUTTON_RIGHT:
							gMouseButtonsPressed[1] = false;
							break;
						case SDL_BUTTON_MIDDLE:
						{
							break;
						}
						default:
							break;
					}
					break;
				}
				case SDL_MOUSEMOTION:
				{
					//arguments: x, y
					int clickPosX = event.button.x;
					int clickPosY = gWindowHeight - event.button.y;

					int deltaMouse[2];
					deltaMouse[0] = clickPosX - gOldMousePos[0];
					deltaMouse[1] = clickPosY - gOldMousePos[1];

					gOldMousePos[0] = clickPosX;
					gOldMousePos[1] = clickPosY;

					if (gMouseButtonsPressed[0] == true)
					{
						gCameraRotY += deltaMouse[0]*0.3f;
						gCameraRotX -= deltaMouse[1]*0.3f;
					}
					if (gMouseButtonsPressed[1]==true)
					{
 						gCameraZoom -= deltaMouse[1]*0.3f;
					}

					break;
				}
				case SDL_KEYDOWN:
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE: //fall through
						case SDLK_q:
							quit = true;
							break;
						case SDLK_p:
							gPaused = !gPaused;
							break;
						case SDLK_c:
							if (0 == gCameraMode)
							{
								gCameraMode = 1;
							}
							else
							{
								gCameraMode = 0;
							}
							break;
						case SDLK_f:
							gBiped->getSolid()->addGlobalForce( opal::Vec3r(0.0,30.0,0.0), 0.2 );
							break;
						//case SDLK_n:
						//	gCar->getNetwork()->addNoise( 3.0 );
						//	break;
						case SDLK_LEFT:
							gBiped->moveLeg(true);
							break;
						case SDLK_RIGHT:
							gBiped->moveLeg(false);
							break;
						case SDLK_SPACE:
						{
							break;
						}
						default:
							break;
					}
					break;
				}
				case SDL_KEYUP:
				{
					//switch(event.key.keysym.sym)
					//{
					//	default:
					//		break;
					//}
					break;
				}
				case SDL_QUIT:
				{
					quit = true;
					break;
				}
				case SDL_VIDEORESIZE:
				{
					gWindowWidth = event.resize.w;
					gWindowHeight = event.resize.h;
					gAspectRatio = (float)gWindowWidth/(float)gWindowHeight;

					//SDL_SetVideoMode(gWindowWidth, gWindowHeight, 0, SDL_OPENGL|SDL_RESIZABLE);
					//InitOpenGL();

					glViewport(0, 0, gWindowWidth, gWindowHeight);
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					gluPerspective(40.0, gAspectRatio, 1.0, 5000.0);
					glMatrixMode(GL_MODELVIEW);
					break;
				}
				default:
					break;
			}
		}

		//update physics
		opal::real dt = (opal::real)gTimer.GetElapsedSeconds();
		//opal::real dt = (opal::real)0.0167;
		if (!gPaused)
		{
			gSimulator->simulate( dt );
		}

		//redraw
		glClearColor(0.8f, 0.8f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();

		float matAmbientAndDiffuse[4] = {.5, .5, .5, 1};
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbientAndDiffuse);
		
		//if (0 == gCameraMode)
		//{
			glTranslatef(0.0, -gCameraYPos, gCameraZoom);
			glRotatef(gCameraRotX, 1, 0, 0);
			glRotatef(gCameraRotY, 0, 1, 0);
		//}
		//else
		//{
		//	opal::Matrix44r carTransform = gCar->getTransform();
		//	opal::Matrix44r cameraOffset = carTransform;
		//	cameraOffset.translate( 0.0, 1.0, 10.0 );
		//	
		//	opal::Point3r carPos = carTransform.getPosition();
		//	opal::Point3r cameraPos = cameraOffset.getPosition();
		//	if (cameraPos[1] < 1.0 )
		//	{
		//		cameraPos[1] = 1.0;
		//	}

		//	gluLookAt( cameraPos[0], cameraPos[1], cameraPos[2], carPos[0], carPos[1], carPos[2],
		//		0.0, 1.0, 0.0 );
		//}

		glCallList( gGroundDispList );

		std::vector<Base3DObject*>::iterator iter;
		for (iter = gObjects.begin(); iter != gObjects.end(); ++iter)
		{
			(*iter)->Draw();
			(*iter)->Update(dt);
		}

		//Evaluate( gCar );

		SDL_GL_SwapBuffers();
	}
}
