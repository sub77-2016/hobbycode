/*
 *  main.cpp
 *
 */

#include "SDLViewer.h"

using namespace SDLGL;

class MyApp : public SDLViewer
{
public:
	MyApp(const unsigned int w, const unsigned int h) : SDLViewer(w, h) 
	{
		//SDLViewer::init();
	}
	~MyApp(void){}
	
protected:
	void createScene(void)
	{
		//gluOrtho2D(-4.0, 4.0, -3.0, 3.0);  
		//SDLViewer::createScene();
		
		mSceneRoot = createScenegraph2();
		
		//mScene = osg::makeTorus(.5, 2, 16, 16);
    	//mMgr->setRoot(mScene);
    	//mMgr->showAll();    	
    	
	}
	
	bool frameStarted(void)
	{
		return true;
	}
	
	bool frameEnded(void)
	{
		/*
		glBegin(GL_QUADS);
    		glColor3f(1,0,0);
    		glVertex3f(2.5,2.5,0);
    		glVertex3f(-2.5,2.5,0);
    		glVertex3f(-2.5,-2.5,0);
    		glVertex3f(2.5,-2.5,0);
		glEnd();
		*/
		
		return true;		
	}
	
	//This function will create our scenegraph
	osg::NodePtr createScenegraph(void)
	{
    	// First we will create all needed geometry
    	// the body of the house
    	osg::NodePtr houseMain = osg::makeBox(20,20,20,1,1,1);
    	// now the roof
    	osg::NodePtr roof = osg::makeBox(14.14, 14.14, 20, 1, 1, 1);
    	// and the chimney - we have the top and sides generated
    	// but we have no need for the bottom (it is inside the house)
    	osg::NodePtr chimney = osg::makeCylinder(10,1,8,true,true,false);
    	// Now we create the root node and attach the geometry nodes to it
    	osg::NodePtr n = osg::Node::create();
    	osg::beginEditCP(n, osg::Node::CoreFieldMask | osg::Node::ChildrenFieldMask);
        	n->setCore(osg::Group::create());
        	n->addChild(houseMain);
        	n->addChild(roof);
        	n->addChild(chimney);
    	osg::endEditCP(n, osg::Node::CoreFieldMask | osg::Node::ChildrenFieldMask);
    	return n;
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

};

int main(int argc, char *argv[])
{
	// OSG init
    osg::osgInit(argc, argv);
    
    //SDLViewer viewer;
    //SDLViewer viewer(640,480);
    
    //SDLViewer viewer2(800,600);
    //SDLViewer viewer3(1024,768);
    
    //SDLViewer viewer(640, 480, SDLviewer::WINDOWED);
    //SDLViewer viewer(640, 480, SDLviewer::FULLSCREEN);
    
    MyApp viewer(640,480);
	
	//viewer = SDLViewer::getInstance();
	
	//viewer.init();
	//viewer.startRendering();
    //viewer.cleanup();
    viewer.run();
    
    return 0;
}
