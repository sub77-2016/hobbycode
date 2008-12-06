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

#include <SDL/SDL.h>
#include <OGRE/Ogre.h>

#include "verve/apps/Robot.h"
#include "verve/apps/Car.h"
#include "verve/apps/AgentVisualDebugger.h"
#include "verve/apps/SimulationEngine.h"
#include "verve/apps/SimulationFrameListener.h"

void setupEnvironment();
void mainLoop();
void handleInput(Ogre::Real elapsedRealTime);
void handlePhysicsPostStepEvent();
void updateOverlay();
verve::real computeReward();

unsigned int gNumRuns = 1;
unsigned int gNumTrialsPerRun = 1;
verve::real gPhysicsStepSize = (verve::real)0.01;
//SimulationEngine gEngine;
Robot* gRobot = NULL;
Car* gCar = NULL;
AgentVisualDebugger* gAgentDebugger = NULL;

//opal::VolumeSensor* gGoalSensor = NULL;
//opal::Solid* gGoalSensorVolume = NULL;
SDL_Joystick *gGamePad = NULL;

class PhysicsPostStepEventHandler : public opal::PostStepEventHandler
{
        virtual void OPAL_CALL handlePostStepEvent()
        {
                handlePhysicsPostStepEvent();
        }
};

PhysicsPostStepEventHandler gPostStepEventHandler;

class MyFrameListener : public SimulationFrameListener
{
public:
    MyFrameListener(RenderWindow* win, PhysicalCamera* cam)
        : SimulationFrameListener(win, cam)
    {
		
    } 
    
   ~MyFrameListener()
	{

	}
	
	virtual bool frameStarted(const FrameEvent& evt);
	virtual bool frameEnded(const FrameEvent& evt){}
	
};

bool MyFrameListener::frameStarted(const FrameEvent& evt)
{
	//if( SimulationFrameListener::frameRenderingQueued(evt) == false )
	if( SimulationFrameListener::frameStarted(evt) == false )
		return false;

  	// Call default
	return true;
}

class MyApplication : public SimulationEngine
{
public:
   MyApplication(void)
   	: SimulationEngine()
   {
   	
   }
   virtual ~MyApplication(){}
   
protected:
	/*
	virtual void createFrameListener()
	{
   		MyFrameListener *frmLnr = new MyFrameListener(mOgreWindow, mPhysicalCamera);
		frmLnr->showDebugOverlay(true);
   		mOgreRoot->addFrameListener(frmLnr);
	}
	*/
	
	
   virtual void createScene();
};

void MyApplication::createScene()
{
        setUpdateMode(SimulationEngine::SIMULATE_REAL_TIME_MULTIPLE, 1);

        //// Set to capture frames at 29.97 fps.
        //setUpdateMode(SIMULATE_CONSTANT_CHUNK, 0.0333667);

        // Use feet for this simulation.
        getSimulator()->setGravity(opal::Vec3r(0, -30, 0));
        getSimulator()->setStepSize(gPhysicsStepSize);

        // Make sure we get notified at the end of each step.
        getSimulator()->addPostStepEventHandler(&gPostStepEventHandler);

        Ogre::OverlayManager::getSingleton().getByName("Verve/Debug")->hide();
        Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay")->hide();

        // Setup camera.
        getCamera()->setPosition(opal::Point3r(0, 25, 25));
        getCamera()->lookAt(opal::Point3r(0, (opal::real)0.1, 0));
        setCameraMoveSpeed(1);
       
        // Main static arena.
        opal::Blueprint arenaBlueprint;
        opal::loadFile(arenaBlueprint, "../data/blueprints/arena1.xml");
        opal::BlueprintInstance arenaBPInstance;
        getSimulator()->instantiateBlueprint(arenaBPInstance,
                arenaBlueprint, opal::Matrix44r(), 1);
        createPhysicalEntity("staticEnvironment", "Plastic/Gray",
                arenaBPInstance.getSolid("staticEnvironment"));
        createPhysicalEntity("toy1", "Plastic/Green",
                arenaBPInstance.getSolid("toy1"));
        createPhysicalEntity("toy2", "Plastic/Green",
                arenaBPInstance.getSolid("toy2"));
        createPhysicalEntity("toy3", "Plastic/Green",
                arenaBPInstance.getSolid("toy3"));

        // Seesaw.
        opal::Blueprint seesawBP;
        opal::loadFile(seesawBP, "../data/blueprints/seesaw.xml");
        opal::BlueprintInstance seesawBPInstance;
        opal::Matrix44r seesawTransform;
        seesawTransform.translate(8, 0, 0);
        getSimulator()->instantiateBlueprint(seesawBPInstance,
                seesawBP, seesawTransform, 1);
        createPhysicalEntity("seesawSupport", "Plastic/Black",
                seesawBPInstance.getSolid("seesawSupport"));
        createPhysicalEntity("seesawPanel", "Plastic/Orange",
                seesawBPInstance.getSolid("seesawPanel"));

        // Add an initial torque to bring one end of the seesaw to the
        // ground.
        seesawBPInstance.getJoint("seesawHinge")->addTorque(0, 100, 0, true);

        // Merry-go-round.
        createPhysicalEntity("merryGoRound", "Plastic/Yellow",
                arenaBPInstance.getSolid("merryGoRound"));

        // Curtains.
        opal::Blueprint curtainsBP;
        opal::loadFile(curtainsBP, "../data/blueprints/blockCurtain.xml");
        opal::BlueprintInstance curtainsBPInstance;
        opal::Matrix44r curtainsTransform;
        curtainsTransform.rotate(45, 0, 1, 0);
        curtainsTransform.translate(-10, 0, 0);
        getSimulator()->instantiateBlueprint(curtainsBPInstance,
                curtainsBP, curtainsTransform, 1);
        createPhysicalEntity("curtainBase", "Plastic/Red",
                curtainsBPInstance.getSolid("curtainBase"));
        createPhysicalEntity("curtainPiece0", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece0"));
        createPhysicalEntity("curtainPiece1", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece1"));
        createPhysicalEntity("curtainPiece2", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece2"));
        createPhysicalEntity("curtainPiece3", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece3"));
        createPhysicalEntity("curtainPiece4", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece4"));
        createPhysicalEntity("curtainPiece5", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece5"));

        //// Ragdoll.
        //opal::Blueprint ragdollBP;
        //opal::loadFile(ragdollBP, "../data/blueprints/ragdoll.xml");
        //opal::BlueprintInstance ragdollBPInstance;
        //opal::Matrix44r ragdollTransform;
        //ragdollTransform.translate(10, 5, 0);
        //getSimulator()->instantiateBlueprint(ragdollBPInstance,
        //      ragdollBP, ragdollTransform, 2);
        //for (unsigned int i = 0; i < ragdollBPInstance.getNumSolids(); ++i)
        //{
        //      opal::Solid* s = ragdollBPInstance.getSolid(i);
        //      createPhysicalEntity(s->getName(), "Plastic/Red", s);
        //}

        //// TESTING: Simple goal box.
        //opal::Solid* boxSolid = getSimulator()->createSolid();
        //boxSolid->setStatic(false);
        //boxSolid->setSleepiness(0);
        //boxSolid->setPosition(15.5, 10, -7);
        //opal::BoxShapeData data;
        //data.dimensions.set(1.5, 1.5, 1.5);
        //data.material.friction = 0.1;
        //data.material.density = 0.5;
        //boxSolid->addShape(data);
        //createPhysicalEntity("goal box", "Plastic/Green", boxSolid);

        //// TESTING: Make a volume sensor to detect the goal.
        //opal::VolumeSensorData goalSensorData;
        //goalSensorData.solid = gRobot->getChassis();
        //goalSensorData.transform.makeTranslation(0, 0, -2);
        //gGoalSensor = getSimulator()->createVolumeSensor();
        //gGoalSensor->init(goalSensorData);

        //gGoalSensorVolume = getSimulator()->createSolid();
        //gGoalSensorVolume->setStatic(true);
        ////opal::Matrix44r m = gRobot->getChassis()->getTransform();
        ////m.translate(0, 0, -2);
        //opal::Matrix44r m;
        //m.translate(0, 100, 0);
        //gGoalSensorVolume->setTransform(m);
        //opal::BoxShapeData sensorVolumeShape;
        //sensorVolumeShape.dimensions.set(2, 1, 2);
        ////getSimulator()->setupContactGroup(5, false);
        ////sensorVolumeShape.contactGroup = 5;
        //sensorVolumeShape.material.density = 0.01;
        //gGoalSensorVolume->addShape(sensorVolumeShape);
        ////createPhysicalEntityBox("goal sensor", "Translucent/Blue", boxDim,
        ////    gGoalSensorVolume);


        //opal::JointData fixedJointData;
        //fixedJointData.setType(opal::FIXED_JOINT);
        //fixedJointData.solid0 = gRobot->getChassis();
        //fixedJointData.solid1 = gGoalSensorVolume;
        //opal::Joint* fixedJoint = getSimulator()->createJoint();
        //fixedJoint->init(fixedJointData);
         
}

class Sim
{
public:
	Sim(MyApplication &a){
		mEngine = a;
	}
	
	void init()
	{    
		mEngine.init();
		//mEngine = new SimulationEngine();	
		//mEngine = &engine;
		
		// Create the robot.
		opal::Matrix44r robotTransform;
   		robotTransform.translate(0, 1, 0);
   		mRobot = new Robot(mEngine, 5);
   		mRobot->init("../data/blueprints/robot1.xml", "Plastic/LightBlue",
                0.5, robotTransform, 2);
   		mRobot->resetBodyAndCreateNewAgent();
   		mRobot->resetBodyAndSTM();
   		mRobot->randomizeState();
   		mRobot->getFLMotor()->setMaxTorque((opal::real)2);
   		mRobot->getFRMotor()->setMaxTorque((opal::real)2);
   		mRobot->getFLMotor()->setMaxVelocity(1000);
   		mRobot->getFRMotor()->setMaxVelocity(1000);

  		// Create the car.
   		opal::Matrix44r carTransform;
   		carTransform.translate(-12, 2, 4);
   		mCar = new Car(mEngine);
   		mCar->init("../data/blueprints/car1.xml", "Plastic/Blue", 1,
                carTransform, 1);
       
		//DataFile dataFile(mNumTrialsPerRun);
  		//updateOverlayData(trial);
   		//mAvgRewardPerStep = 0;
   		//mCurrentTrialTime = 0;
       
 		mAgentDebugger = new AgentVisualDebugger(mEngine.getSceneManager());
   		mAgentDebugger->setAgent(mRobot->getAgent());
   		mAgentDebugger->setDisplayEnabled(false);  
	}
   
   ~Sim()
	{
		delete mRobot;
   		delete mCar;
  		delete mAgentDebugger;
  		delete mPostStepEventHandler;
	}
	
	void run()
	{
		mEngine.run();
	}
	
	void evolve()
	{
		
	}
	
	void handleInput(Ogre::Real elapsedRealTime)
	{
		
	}
	
protected:    
	MyApplication mEngine;
		
	Robot* mRobot;
	Car* mCar;
	AgentVisualDebugger* mAgentDebugger;
	PhysicsPostStepEventHandler *mPostStepEventHandler;
	
	unsigned int mNumRuns;
	unsigned int mNumTrialsPerRun;
	verve::real mPhysicsStepSize;
};

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
	srand((unsigned int)time(NULL));
	
    // Create application object
    MyApplication app;
    //app.init();
    
    //Sim sim(app);
    //sim.init();
    
    if (!app.init())
    {
     	return 0;
   	}
   	
   	
        // Create the robot.
        opal::Matrix44r robotTransform;
        robotTransform.translate(0, 1, 0);
        gRobot = new Robot(app, 5);
        gRobot->init("../data/blueprints/robot1.xml", "Plastic/LightBlue",
                0.5, robotTransform, 2);
        gRobot->resetBodyAndCreateNewAgent();
        gRobot->resetBodyAndSTM();
        gRobot->randomizeState();
        gRobot->getFLMotor()->setMaxTorque((opal::real)2);
        gRobot->getFRMotor()->setMaxTorque((opal::real)2);
        gRobot->getFLMotor()->setMaxVelocity(1000);
        gRobot->getFRMotor()->setMaxVelocity(1000);


        // Create the car.
        opal::Matrix44r carTransform;
        carTransform.translate(-12, 2, 4);
        gCar = new Car(app);
        gCar->init("../data/blueprints/car1.xml", "Plastic/Blue", 1,
                carTransform, 1);
       

        //DataFile dataFile(mNumTrialsPerRun);
        //updateOverlayData(trial);
        //mAvgRewardPerStep = 0;
        //mCurrentTrialTime = 0;
       
        gAgentDebugger = new AgentVisualDebugger(app.getSceneManager());
        gAgentDebugger->setAgent(gRobot->getAgent());
        gAgentDebugger->setDisplayEnabled(false);
        

    try {
    	app.run();
    } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL );
#else
        std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
    }
    
	delete gRobot;
   	delete gCar;
   	delete gAgentDebugger;

    return 0;
}
#ifdef __cplusplus
}
#endif

MyApplication gEngine;

int _main(int argc, char* argv[])
{

       
        srand((unsigned int)time(NULL));

        SDL_Init(SDL_INIT_JOYSTICK);
        if(SDL_NumJoysticks() > 0)
        {
                // Setup the joystick.
                std::cout << "========================================" << std::endl;
                std::cout << "Initializing game controller: " << SDL_JoystickName(0)
                        << std::endl;
                gGamePad = SDL_JoystickOpen(0);
                std::cout << SDL_JoystickNumAxes(gGamePad) << " axes" << std::endl;
                std::cout << SDL_JoystickNumBalls(gGamePad) << " trackballs" << std::endl;
                std::cout << SDL_JoystickNumHats(gGamePad) << " hats" << std::endl;
                std::cout << SDL_JoystickNumButtons(gGamePad) << " buttons" << std::endl;
                std::cout << "========================================" << std::endl;
        }
        else
        {
                std::cout << "========================================" << std::endl;
                std::cout << "No game controller detected" << std::endl;
                std::cout << "========================================" << std::endl;
        }

        //Ogre::Overlay* trialOverlay;

        ///// The current amount of elapsed time within a trial.
        //Ogre::Real mCurrentTrialTime;

        ///// The length of each trial in seconds.
        //Ogre::Real mTrialLength;

        ///// The rewards received during a single trial, in rewards per step.
        //verve::real mAvgRewardPerStep;

       
        if (!gEngine.init())
        {
                return 0;
        }
       

        /*
        gEngine.setUpdateMode(SimulationEngine::SIMULATE_REAL_TIME_MULTIPLE, 1);
        */

        //// Set to capture frames at 29.97 fps.
        //engine.setUpdateMode(SIMULATE_CONSTANT_CHUNK, 0.0333667);

        /*
        // Use feet for this simulation.
        gEngine.getSimulator()->setGravity(opal::Vec3r(0, -30, 0));
        gEngine.getSimulator()->setStepSize(gPhysicsStepSize);

        // Make sure we get notified at the end of each step.
        gEngine.getSimulator()->addPostStepEventHandler(&gPostStepEventHandler);

        */
       
       
        // Create the robot.
        opal::Matrix44r robotTransform;
        robotTransform.translate(0, 1, 0);
        gRobot = new Robot(gEngine, 5);
        gRobot->init("../data/blueprints/robot1.xml", "Plastic/LightBlue",
                0.5, robotTransform, 2);
        gRobot->resetBodyAndCreateNewAgent();
        gRobot->resetBodyAndSTM();
        gRobot->randomizeState();
        gRobot->getFLMotor()->setMaxTorque((opal::real)2);
        gRobot->getFRMotor()->setMaxTorque((opal::real)2);
        gRobot->getFLMotor()->setMaxVelocity(1000);
        gRobot->getFRMotor()->setMaxVelocity(1000);


        // Create the car.
        opal::Matrix44r carTransform;
        carTransform.translate(-12, 2, 4);
        gCar = new Car(gEngine);
        gCar->init("../data/blueprints/car1.xml", "Plastic/Blue", 1,
                carTransform, 1);
       

        //DataFile dataFile(mNumTrialsPerRun);
        //updateOverlayData(trial);
        //mAvgRewardPerStep = 0;
        //mCurrentTrialTime = 0;
       
        gAgentDebugger = new AgentVisualDebugger(gEngine.getSceneManager());
        gAgentDebugger->setAgent(gRobot->getAgent());
        gAgentDebugger->setDisplayEnabled(false);
       
       
        /*
        Ogre::OverlayManager::getSingleton().getByName("Verve/Debug")->hide();
        Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay")->hide();

        // Setup camera.
        gEngine.getCamera()->setPosition(opal::Point3r(0, 25, 25));
        gEngine.getCamera()->lookAt(opal::Point3r(0, (opal::real)0.1, 0));
        gEngine.setCameraMoveSpeed(15);
        */

        //setupEnvironment();
       
        gEngine.run();

        //mainLoop();

        delete gRobot;
        delete gCar;
        delete gAgentDebugger;
        return 0;
}

/*
void setupEnvironment()
{
        // Main static arena.
        opal::Blueprint arenaBlueprint;
        opal::loadFile(arenaBlueprint, "../data/blueprints/arena1.xml");
        opal::BlueprintInstance arenaBPInstance;
        gEngine.getSimulator()->instantiateBlueprint(arenaBPInstance,
                arenaBlueprint, opal::Matrix44r(), 1);
        gEngine.createPhysicalEntity("staticEnvironment", "Plastic/Gray",
                arenaBPInstance.getSolid("staticEnvironment"));
        gEngine.createPhysicalEntity("toy1", "Plastic/Green",
                arenaBPInstance.getSolid("toy1"));
        gEngine.createPhysicalEntity("toy2", "Plastic/Green",
                arenaBPInstance.getSolid("toy2"));
        gEngine.createPhysicalEntity("toy3", "Plastic/Green",
                arenaBPInstance.getSolid("toy3"));

        // Seesaw.
        opal::Blueprint seesawBP;
        opal::loadFile(seesawBP, "../data/blueprints/seesaw.xml");
        opal::BlueprintInstance seesawBPInstance;
        opal::Matrix44r seesawTransform;
        seesawTransform.translate(8, 0, 0);
        gEngine.getSimulator()->instantiateBlueprint(seesawBPInstance,
                seesawBP, seesawTransform, 1);
        gEngine.createPhysicalEntity("seesawSupport", "Plastic/Black",
                seesawBPInstance.getSolid("seesawSupport"));
        gEngine.createPhysicalEntity("seesawPanel", "Plastic/Orange",
                seesawBPInstance.getSolid("seesawPanel"));

        // Add an initial torque to bring one end of the seesaw to the
        // ground.
        seesawBPInstance.getJoint("seesawHinge")->addTorque(0, 100, 0, true);

        // Merry-go-round.
        gEngine.createPhysicalEntity("merryGoRound", "Plastic/Yellow",
                arenaBPInstance.getSolid("merryGoRound"));

        // Curtains.
        opal::Blueprint curtainsBP;
        opal::loadFile(curtainsBP, "../data/blueprints/blockCurtain.xml");
        opal::BlueprintInstance curtainsBPInstance;
        opal::Matrix44r curtainsTransform;
        curtainsTransform.rotate(45, 0, 1, 0);
        curtainsTransform.translate(-10, 0, 0);
        gEngine.getSimulator()->instantiateBlueprint(curtainsBPInstance,
                curtainsBP, curtainsTransform, 1);
        gEngine.createPhysicalEntity("curtainBase", "Plastic/Red",
                curtainsBPInstance.getSolid("curtainBase"));
        gEngine.createPhysicalEntity("curtainPiece0", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece0"));
        gEngine.createPhysicalEntity("curtainPiece1", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece1"));
        gEngine.createPhysicalEntity("curtainPiece2", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece2"));
        gEngine.createPhysicalEntity("curtainPiece3", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece3"));
        gEngine.createPhysicalEntity("curtainPiece4", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece4"));
        gEngine.createPhysicalEntity("curtainPiece5", "Plastic/Black",
                curtainsBPInstance.getSolid("curtainPiece5"));

        //// Ragdoll.
        //opal::Blueprint ragdollBP;
        //opal::loadFile(ragdollBP, "../data/blueprints/ragdoll.xml");
        //opal::BlueprintInstance ragdollBPInstance;
        //opal::Matrix44r ragdollTransform;
        //ragdollTransform.translate(10, 5, 0);
        //gEngine.getSimulator()->instantiateBlueprint(ragdollBPInstance,
        //      ragdollBP, ragdollTransform, 2);
        //for (unsigned int i = 0; i < ragdollBPInstance.getNumSolids(); ++i)
        //{
        //      opal::Solid* s = ragdollBPInstance.getSolid(i);
        //      gEngine.createPhysicalEntity(s->getName(), "Plastic/Red", s);
        //}

        //// TESTING: Simple goal box.
        //opal::Solid* boxSolid = gEngine.getSimulator()->createSolid();
        //boxSolid->setStatic(false);
        //boxSolid->setSleepiness(0);
        //boxSolid->setPosition(15.5, 10, -7);
        //opal::BoxShapeData data;
        //data.dimensions.set(1.5, 1.5, 1.5);
        //data.material.friction = 0.1;
        //data.material.density = 0.5;
        //boxSolid->addShape(data);
        //gEngine.createPhysicalEntity("goal box", "Plastic/Green", boxSolid);

        //// TESTING: Make a volume sensor to detect the goal.
        //opal::VolumeSensorData goalSensorData;
        //goalSensorData.solid = gRobot->getChassis();
        //goalSensorData.transform.makeTranslation(0, 0, -2);
        //gGoalSensor = gEngine.getSimulator()->createVolumeSensor();
        //gGoalSensor->init(goalSensorData);

        //gGoalSensorVolume = gEngine.getSimulator()->createSolid();
        //gGoalSensorVolume->setStatic(true);
        ////opal::Matrix44r m = gRobot->getChassis()->getTransform();
        ////m.translate(0, 0, -2);
        //opal::Matrix44r m;
        //m.translate(0, 100, 0);
        //gGoalSensorVolume->setTransform(m);
        //opal::BoxShapeData sensorVolumeShape;
        //sensorVolumeShape.dimensions.set(2, 1, 2);
        ////gEngine.getSimulator()->setupContactGroup(5, false);
        ////sensorVolumeShape.contactGroup = 5;
        //sensorVolumeShape.material.density = 0.01;
        //gGoalSensorVolume->addShape(sensorVolumeShape);
        ////gEngine.createPhysicalEntityBox("goal sensor", "Translucent/Blue", boxDim,
        ////    gGoalSensorVolume);

        //opal::JointData fixedJointData;
        //fixedJointData.setType(opal::FIXED_JOINT);
        //fixedJointData.solid0 = gRobot->getChassis();
        //fixedJointData.solid1 = gGoalSensorVolume;
        //opal::Joint* fixedJoint = gEngine.getSimulator()->createJoint();
        //fixedJoint->init(fixedJointData);
}
*/

void mainLoop()
{
        //while (mCurrentTrialTime < mTrialLength)

       
        while (true)
        {
                Ogre::Real elapsedSimTime = 0;
                Ogre::Real elapsedRealTime = 0;
                //gEngine.update(elapsedSimTime, elapsedRealTime);
                handleInput(elapsedRealTime);
                //if (gEngine.quitApp())
                //{
                        //return;
                //}

                // Update sound effects at 50 Hz.
                const Ogre::Real soundUpdatePeriod = 0.02;
                static Ogre::Real soundUpdateTimer = 0;
                soundUpdateTimer -= elapsedSimTime;
                if (soundUpdateTimer <= 0)
                {
                        gRobot->updateSoundEffects(soundUpdatePeriod);
                        gCar->updateSoundEffects(soundUpdatePeriod);
                        soundUpdateTimer = soundUpdatePeriod;
                }

                gRobot->updateVisuals(elapsedSimTime);
                updateOverlay();
                gAgentDebugger->updateVisuals();
        }

        //mAvgRewardPerStep = mAvgRewardPerStep * mPhysicsStepSize /
        //      mCurrentTrialTime;
        //dataFile.storeData("trial", trial, (double)trial);
        //dataFile.storeData("avg reward per step", trial,
        //      mAvgRewardPerStep);
        //printTrialAndRunStatus(run, trial, mAvgRewardPerStep);

        //std::cout << "Agent age = " << mPendulum->getAgent()->getAgeString()
        //      << std::endl;


        //dataFile.printToFile("./results/pendulum-performance.dat");
}

void handleInput(Ogre::Real elapsedRealTime)
{
        // This variable can be used to keep keys from repeating too fast.
        static Ogre::Real toggleTimer = 0;
        if (toggleTimer >= 0)
        {
                toggleTimer -= elapsedRealTime;
        }

        OIS::Keyboard* keyboard ; //= gEngine.getKeyboard();

        if (keyboard->isKeyDown(OIS::KC_W))
        {
                gCar->forward();
        }
        else if (keyboard->isKeyDown(OIS::KC_S))
        {
                gCar->reverse();
        }
        else
        {
                gCar->idle();
        }

        if (keyboard->isKeyDown(OIS::KC_A))
        {
                gCar->setSteering(-1);
        }
        else if (keyboard->isKeyDown(OIS::KC_D))
        {
                gCar->setSteering(1);
        }
        else
        {
                gCar->setSteering(0);
        }

        // If available, get data from the game controller.
        if (gGamePad)
        {
                // Update the game controller state.
                SDL_JoystickUpdate();

                Ogre::Real joy0X = (Ogre::Real)SDL_JoystickGetAxis(gGamePad, 0) /
                        (Ogre::Real)32768;
                Ogre::Real joy0Y = (Ogre::Real)SDL_JoystickGetAxis(gGamePad, 1) /
                        (Ogre::Real)32768;
                Ogre::Real joy1X = (Ogre::Real)SDL_JoystickGetAxis(gGamePad, 4) /
                        (Ogre::Real)32768;
                Ogre::Real joy1Y = (Ogre::Real)SDL_JoystickGetAxis(gGamePad, 3) /
                        (Ogre::Real)32768;

                if (fabs(joy0Y) > 0.1)
                {
                        gCar->setThrottle(-joy0Y);
                }
                else
                {
                        gCar->idle();
                }

                if (fabs(joy0X) > 0.1)
                {
                        gCar->setSteering(joy0X);
                }
                else
                {
                        gCar->setSteering(0);
                }

                if (joy1X > 0.2 || joy1X < -0.2)
                {
                        Ogre::Degree rotAroundY = -Ogre::Degree(joy1X);
                        gEngine.getCamera()->yawRelative(rotAroundY.valueDegrees());
                }

                if (joy1Y > 0.2 || joy1Y < -0.2)
                {
                        Ogre::Degree rotAroundX = -Ogre::Degree(joy1Y);
                        gEngine.getCamera()->pitchRelative(rotAroundX.valueDegrees());
                }
        }

        // Toggle GUI.
        if (keyboard->isKeyDown(OIS::KC_G) && toggleTimer <= 0)
        {
                Ogre::Overlay* debugOverlay = Ogre::OverlayManager::getSingleton().
                        getByName("Verve/Debug");

                if (debugOverlay->isVisible())
        {
                        debugOverlay->hide();
                        gAgentDebugger->setDisplayEnabled(false);
                }
                else
                {
                        debugOverlay->show();
                        gAgentDebugger->setDisplayEnabled(true);
                }

                toggleTimer = 0.5;
        }
}

void handlePhysicsPostStepEvent()
{
        // At the end of each physics step, update the robot.
        verve::real reward = computeReward();
        //mAvgRewardPerStep += reward;
        gRobot->update(reward, gPhysicsStepSize);
        //mCurrentTrialTime += gPhysicsStepSize;
}

void updateOverlay()
{
        const verve::Agent* a = gRobot->getAgent();

        Ogre::OverlayElement* overlayElement =
                Ogre::OverlayManager::getSingleton().getOverlayElement(
                "Verve/Debug/PrimaryReward");
        overlayElement->setCaption("Primary Reward: " +
                Ogre::StringConverter::toString(a->getDebugData().reward, 4));

        overlayElement = Ogre::OverlayManager::getSingleton().getOverlayElement(
                "Verve/Debug/ValueEstimation");
        overlayElement->setCaption("Value Estimation: " +
                Ogre::StringConverter::toString(a->getDebugData().valueEstimation, 4));

        overlayElement = Ogre::OverlayManager::getSingleton().getOverlayElement(
                "Verve/Debug/TDError");
        overlayElement->setCaption("TD Error: " +
                Ogre::StringConverter::toString(a->getDebugData().TDError, 4));

        overlayElement = Ogre::OverlayManager::getSingleton().getOverlayElement(
                "Verve/Debug/CuriosityReward");
        overlayElement->setCaption("Curiosity Reward: " +
                Ogre::StringConverter::toString(a->getDebugData().curiosityReward, 4));

        overlayElement = Ogre::OverlayManager::getSingleton().getOverlayElement(
                "Verve/Debug/Uncertainty");
        overlayElement->setCaption("Uncertainty Estimation: " +
                Ogre::StringConverter::toString(
                a->getDebugData().predictionUncertaintyEstimation, 4));

        overlayElement = Ogre::OverlayManager::getSingleton().getOverlayElement(
                "Verve/Debug/ModelError");
        overlayElement->setCaption("Model Error: " +
                Ogre::StringConverter::toString(a->getDebugData().modelPredictionError, 4));

        overlayElement = Ogre::OverlayManager::getSingleton().getOverlayElement(
                "Verve/Debug/PlanLength");
        overlayElement->setCaption("Plan Length: " +
                Ogre::StringConverter::toString(a->getDebugData().planLength));

        overlayElement = Ogre::OverlayManager::getSingleton().getOverlayElement(
                "Verve/Debug/Age");
        overlayElement->setCaption("Age: " + a->getAgeString());

        overlayElement = Ogre::OverlayManager::getSingleton().getOverlayElement(
                "Verve/Debug/Action");
        overlayElement->setCaption("Action: " + gRobot->getCurrentActionName());

        for (unsigned int i = 0; i < a->getNumContinuousSensors(); ++i)
        {
                Ogre::String sensorNumStr = Ogre::StringConverter::toString(i);
                Ogre::String label = "Sensor " + sensorNumStr;
                Ogre::OverlayElement* sensorElement =
                        Ogre::OverlayManager::getSingleton().getOverlayElement(
                        "Verve/Debug/Sensor" + sensorNumStr);
                const verve::real* sensorData = gRobot->getContinuousInputs();
                sensorElement->setCaption(label + ": " +
                        Ogre::StringConverter::toString(sensorData[i], 4));
        }
}

verve::real computeReward()
{
        verve::real reward = 0;

        //opal::VolumeQueryResult result;
        //result = gGoalSensor->queryVolume(gGoalSensorVolume);

        ////std::cout << (gRobot->getChassis()->getTransform() * gGoalSensor->getTransform()).getPosition()
        ////    << std::endl;

        //for (unsigned int i = 0; i < result.getNumSolids(); ++i)
        //{
        //      if (result.getSolid(i) == gEngine.getPhysicalEntity("goal box")->getSolid())
        //      {
        //              reward = 1;
        //              break;
        //      }
        //      else
        //      {
        //              reward = 0;
        //      }
        //}


        return reward;
}

