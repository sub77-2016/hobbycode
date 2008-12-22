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
		SDLViewer::createScene();
		
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
		glBegin(GL_QUADS);
    		glColor3f(1,0,0);
    		glVertex3f(2.5,2.5,0);
    		glVertex3f(-2.5,2.5,0);
    		glVertex3f(-2.5,-2.5,0);
    		glVertex3f(2.5,-2.5,0);
		glEnd();
		
		return true;		
	}
};

int main(int argc, char *argv[])
{
	// OSG init
    osg::osgInit(argc, argv);
    
    //SDLViewer viewer;
    SDLViewer viewer(640,480);
    
    //SDLViewer viewer2(800,600);
    //SDLViewer viewer3(1024,768);
    
    //SDLViewer viewer(640, 480, SDLviewer::WINDOWED);
    //SDLViewer viewer(640, 480, SDLviewer::FULLSCREEN);
    
    //MyApp viewer(640,480);
	
	viewer = SDLViewer::getInstance();
	
	//viewer.init();
	//viewer.startRendering();
    //viewer.cleanup();
    viewer.run();
    
    return 0;
}
