
#include "../SimulationEngine.h"
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGGLUTWindow.h>


using namespace SDLGL;

class TestSolid: public SimulationEngine
{
protected:
	osg::TransformPtr transCore;
	
public:
	TestSolid(const unsigned int w, const unsigned int h) : SimulationEngine(w, h) 
	{
		
	}
	~TestSolid(void){}
	
protected:
	void createScene(void)
	{
		mSceneRoot = createScenegraph1();
	}
	
	osg::NodePtr createPhysScene(void)
	{
		osg::NodePtr scene;
		
        setUpdateMode(SimulationEngine::SIMULATE_REAL_TIME_MULTIPLE, 1);

        //// Set to capture frames at 29.97 fps.
        //setUpdateMode(SIMULATE_CONSTANT_CHUNK, 0.0333667);

        // Use feet for this simulation.
        getSimulator()->setGravity(opal::Vec3r(0, -30, 0));
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
        
        //mSceneRoot = createScenegraph2();
        
        opal::ShapeData* sd = boxSolid->getData().getShapeData(0);
        
		opal::Point3r offsetPos = sd->offset.getPosition();
		opal::Quaternion offsetQuat = sd->offset.getQuaternion();
		
		opal::Vec3r boxDim = static_cast<const opal::BoxShapeData*>
					(sd)->dimensions;
		
        //mSceneRoot = osg::makeBox(20,20,20,1,1,1);
        //mSceneRoot = osg::makeBox(
        	//(osg::Real32)boxDim[0],
        	//(osg::Real32)boxDim[1],
        	//(osg::Real32)boxDim[2],10,10,10);
        	
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
        
        transCore = osg::Transform::create();
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
		osg::Matrix m;
    
    	// get the time since the apllication startet
    	osg::Real32 time = (osg::Real32)timeElapsed();    
    	//Uint32 time1 = 100; //timeElapsed();	
    	//std::cout<< "Time 1 = "<< time<< std::endl;
    	
    	//osg::Real32 time2 = glutGet(GLUT_ELAPSED_TIME);
    	//std::cout<< "Time 2 = "<< time2<< std::endl;
    
    	// set the rotation
    	m.setRotate(osg::Quaternion(osg::Vec3f(0,1,0), time/1000.f));
    
    	//apply the new matrix to our transform core
    	osg::beginEditCP(transCore);    	
    		transCore->setMatrix(m);
    	osg::endEditCP(transCore);
    	
    	mMgr->redraw();
    	
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
