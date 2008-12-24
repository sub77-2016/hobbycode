
#include "../SimulationEngine.h"
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGGLUTWindow.h>


using namespace SDLGL;

class TestSolid: public SimulationEngine
{
protected:
	osg::TransformPtr transCore;
	opal::Solid* mSolid;
	
public:
	TestSolid(const unsigned int w, const unsigned int h) : SimulationEngine(w, h) 
	{
		transCore = osg::Transform::create();
	}
	~TestSolid(void){}
	
protected:
	void createScene(void)
	{
		//mSceneRoot = createScenegraph1();
		//mSceneRoot = createPhysScene2();
		
		createPhysScene1();
	}
	
	void createPhysScene1(void)
	{
		// OPAL physics
        setUpdateMode(SimulationEngine::SIMULATE_REAL_TIME_MULTIPLE, 1);

        //// Set to capture frames at 29.97 fps.
        //setUpdateMode(SIMULATE_CONSTANT_CHUNK, 0.0333667);

        // Use feet for this simulation.
        getSimulator()->setGravity(opal::Vec3r(0, -9.8*0.1, 0));
        getSimulator()->setStepSize(0.01);	
        
        //// TESTING: Simple goal box.
        opal::Solid* boxSolid = getSimulator()->createSolid();
        boxSolid->setStatic(false);
        boxSolid->setSleepiness(0);
        boxSolid->setPosition(15.5, 10, -7);
        opal::BoxShapeData data;
        data.dimensions.set(1.5, 1.5, 1.5);
        data.material.friction = 0.1;
        data.material.density = 0.5;
        boxSolid->addShape(data);
        createPhysicalEntity("goal box", "Plastic/Green", boxSolid);	  	
	}
	
	osg::NodePtr createPhysScene2(void)
	{
		osg::NodePtr scene;
		
        osg::NodePtr transNode = osg::Node::create();		
		//transCore = osg::Transform::create();
				
		// OPAL physics
        setUpdateMode(SimulationEngine::SIMULATE_REAL_TIME_MULTIPLE, 1);

        //// Set to capture frames at 29.97 fps.
        //setUpdateMode(SIMULATE_CONSTANT_CHUNK, 0.0333667);

        // Use feet for this simulation.
        getSimulator()->setGravity(opal::Vec3r(0, -9.8*0.1, 0));
        getSimulator()->setStepSize(0.01);	
        
        //// TESTING: Simple goal box.
        opal::Solid* boxSolid = getSimulator()->createSolid();
        boxSolid->setStatic(false);
        boxSolid->setSleepiness(0);
        boxSolid->setPosition(15.5, 10, -7);
        opal::BoxShapeData data;
        data.dimensions.set(1.5, 1.5, 1.5);
        data.material.friction = 0.1;
        data.material.density = 0.5;
        boxSolid->addShape(data);
        //createPhysicalEntity("goal box", "Plastic/Green", boxSolid);	  	
        mSolid = boxSolid;        
        
        // Exchange data to OSG
        opal::ShapeData* sd = boxSolid->getData().getShapeData(0);
        
		opal::Point3r offsetPos = sd->offset.getPosition();
		std::cout<< "offsetPos = "<< std::endl<< 
		(osg::Real32)offsetPos[0]<< std::endl<< 
		(osg::Real32)offsetPos[1]<< std::endl<< 
		(osg::Real32)offsetPos[2]<< std::endl;
		
		opal::Quaternion offsetQuat = sd->offset.getQuaternion();
		std::cout<< "offsetQuat = "<< std::endl<< 
		(osg::Real32)offsetQuat[0]<< std::endl<< 
		(osg::Real32)offsetQuat[1]<< std::endl<< 
		(osg::Real32)offsetQuat[2]<< std::endl<< 
		(osg::Real32)offsetQuat[3]<< std::endl;
		
		opal::Vec3r boxDim = static_cast<const opal::BoxShapeData*>
					(sd)->dimensions;					
		std::cout<< "BoxDim = "<< std::endl<< 
		(osg::Real32)boxDim[0]<< std::endl<< 
		(osg::Real32)boxDim[1]<< std::endl<< 
		(osg::Real32)boxDim[2]<< std::endl;		
		
		// OSG scene
		osg::Matrix m;
        
        // now provide some data...
        
        // no rotation at the beginning
        m.setIdentity();
        
       	m.setTranslate(
       		(osg::Real32)offsetPos[0], 
       		(osg::Real32)offsetPos[1], 
			(osg::Real32)offsetPos[2]);
			
  		m.setRotate(
  			osg::Quaternion(
  				osg::Vec3f(
  					(osg::Real32)offsetQuat[0],
  					(osg::Real32)offsetQuat[1],
  					(osg::Real32)offsetQuat[2]), 
  				(osg::Real32)offsetQuat[3]));
        
        // set the core to the matrix we created
        osg::beginEditCP(transCore);
            transCore->setMatrix(m);
        osg::endEditCP(transCore);       

		
        //scene = osg::makeBox(20,20,20,1,1,1);
        osg::NodePtr box = osg::makeBox(
        	(osg::Real32)boxDim[0],
        	(osg::Real32)boxDim[1],
        	(osg::Real32)boxDim[2],1,1,1);
        	
        // now "insert" the core into the node
        osg::beginEditCP(transNode);
            transNode->setCore(transCore);
            // add the torus as a child to
            // the transformation node
            transNode->addChild(box);
        osg::endEditCP(transNode);
        
        // Set up initial Solid position
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
    
    	//apply the new matrix to our transform core
    	osg::beginEditCP(transCore);    	
    		transCore->setMatrix(m);
    	osg::endEditCP(transCore);
        	
      	// "declare" the transformation as root
        scene = transNode;
        	       	
        return scene;
	}
	
	osg::NodePtr createScenegraph1(void)
	{
		// That will be our whole scene for now : an incredible Torus
        
        osg::NodePtr scene;
        
        // create all that stuff we will need:
        //one geometry and one transform node
        
        osg::NodePtr torus = osg::makeTorus(.5, 2, 16, 16);
        osg::NodePtr transNode = osg::Node::create();
        
        //transCore = osg::Transform::create();
        osg::Matrix m;
        
        // now provide some data...
        
        // no rotation at the beginning
        m.setIdentity();
        
        // set the core to the matrix we created
        osg::beginEditCP(transCore);
            transCore->setMatrix(m);
        osg::endEditCP(transCore);

        
        // now "insert" the core into the node
        osg::beginEditCP(transNode);
            transNode->setCore(transCore);
            // add the torus as a child to
            // the transformation node
            transNode->addChild(torus);
        osg::endEditCP(transNode);

        
        // "declare" the transformation as root
        scene = transNode;
        
        return scene;
	}
	
		//This function will create our scenegraph
	osg::NodePtr createScenegraph2(void)
	{
    	// we will use the variable to set our transform matrices
    	osg::Matrix m;
    	// First we will create all needed geometry
    	// the body of the house
    	osg::NodePtr houseMain = osg::makeBox(20,20,20,1,1,1);
    	
    	// now the roof
  		osg::NodePtr roof = osg::makeBox(14.14, 14.14, 20, 1, 1, 1);
  		// we translate the roof to the correct position
 	 	osg::TransformPtr tRoof = osg::Transform::create();
  		osg::beginEditCP(tRoof, osg::Transform::MatrixFieldMask);
      		m.setIdentity();
      		m.setTranslate(0,10,0);
      		m.setRotate(osg::Quaternion(osg::Vec3f(0,0,1), 3.14159/4));
      		tRoof->setMatrix(m);
  		osg::endEditCP(tRoof, osg::Transform::MatrixFieldMask);
  		osg::NodePtr roofTrans = osg::Node::create();
  		osg::beginEditCP(roofTrans, osg::Node::CoreFieldMask | osg::Node::ChildrenFieldMask);
      		roofTrans->setCore(tRoof);
      		roofTrans->addChild(roof);
  		osg::endEditCP(roofTrans, osg::Node::CoreFieldMask | osg::Node::ChildrenFieldMask);
  		// and the chimney - we have the top and sides generated
  		// but we have no need for the bottom (it is inside the house)
  		osg::NodePtr chimney = osg::makeCylinder(10,1,8,true,true,false);
  		//now we translate the chimney
  		//create the transform core
  		osg::TransformPtr tChimney = osg::Transform::create();
  		osg::beginEditCP(tChimney, osg::Transform::MatrixFieldMask);
      		m.setIdentity();
      		// -5 along the x-axis and 2.5 along the z axis
      		// translates the chimney away from the center
      		// 15 along the y-axis translates the chimney to fit on top
      		// of the big box (have a look at the figure above2,5
      		m.setTranslate(-5,15,2.5);
      		tChimney->setMatrix(m);
  		osg::endEditCP(tChimney, osg::Transform::MatrixFieldMask);
  		//insert the transform core into the node
  		osg::NodePtr chimneyTrans = osg::Node::create();
  		osg::beginEditCP(chimneyTrans, osg::Node::CoreFieldMask | osg::Node::ChildrenFieldMask);
      		chimneyTrans->setCore(tChimney);
      		chimneyTrans->addChild(chimney);
  		osg::endEditCP(chimneyTrans, osg::Node::CoreFieldMask | osg::Node::ChildrenFieldMask);
  		// Now we create the root node and attach the geometry nodes to it
  		osg::NodePtr n = osg::Node::create();
  		osg::beginEditCP(n, osg::Node::CoreFieldMask | osg::Node::ChildrenFieldMask);
      		n->setCore(osg::Group::create());
      		n->addChild(houseMain);
      		n->addChild(roofTrans);
      		n->addChild(chimneyTrans);
  		osg::endEditCP(n, osg::Node::CoreFieldMask | osg::Node::ChildrenFieldMask);
  		
  		return n;	
	}
	
	bool frameStarted(void)
	{
		/*
		osg::Matrix m;
    
    	// get the time since the apllication startet
    	//osg::Real32 time = (osg::Real32)timeElapsed();    
    	//Uint32 time1 = 100; //timeElapsed();	
    	//std::cout<< "Time 1 = "<< time<< std::endl;
    	
    	//osg::Real32 time2 = glutGet(GLUT_ELAPSED_TIME);
    	//std::cout<< "Time 2 = "<< time2<< std::endl;
    	
    	// set the rotation
    	//m.setRotate(osg::Quaternion(osg::Vec3f(0,1,0), time/1000.f));
    	
    	mSimulator->simulate(0.01);
    	
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
    
    	//apply the new matrix to our transform core
    	osg::beginEditCP(transCore);    	
    		transCore->setMatrix(m);
    	osg::endEditCP(transCore);
    	*/
    	
		return true;
	}
	
	bool frameEnded(void)
	{

    	
		return true;		
	}

};


void test_solid(void)
{
	TestSolid app(800,600);
	
	//app.init();
	
    app.run();
    
}
