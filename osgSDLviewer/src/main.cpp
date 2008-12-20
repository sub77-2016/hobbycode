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
		
	}
	~MyApp(void){}
	
protected:
	void createScene(void)
	{
		mScene = osg::makeTorus(.5, 3, 16, 16);
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
