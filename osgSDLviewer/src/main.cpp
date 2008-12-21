/*
 *  main.cpp
 *
 */

#include "SDLviewer.h"

using namespace SDLGL;

class MyApp : public SDLviewer
{
public:
	MyApp(const unsigned int w, const unsigned int h) : SDLviewer(w, h) 
	{
		SDLviewer::init();
	}
	~MyApp(void){}
	
protected:
	void createScene(void)
	{
		//gluOrtho2D(-4.0, 4.0, -3.0, 3.0);  
		SDLviewer::createScene();
		
		//mScene = osg::makeTorus(.5, 2, 16, 16);
    	//mMgr->setRoot(mScene);
    	//mMgr->showAll();    	
    	
	}
	
	void redraw(void)
	{
		SDLviewer::redraw();
				
		glBegin(GL_QUADS);
    		glColor3f(1,0,0);
    		glVertex3f(2.5,2.5,0);
    		glVertex3f(-2.5,2.5,0);
    		glVertex3f(-2.5,-2.5,0);
    		glVertex3f(2.5,-2.5,0);
		glEnd();		
	}
};

int main(int argc, char *argv[])
{
	// OSG init
    osg::osgInit(argc, argv);
    
    //SDLviewer viewer;
    //SDLviewer viewer(640,480);
    
    MyApp viewer(640,480);
	
	//viewer.init();
	//viewer.startRendering();
    //viewer.cleanup();
    viewer.run();
    
    return 0;
}
