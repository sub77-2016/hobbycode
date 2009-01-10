
#include "demo_main.h"

using namespace MINGL;

void test_viewer(void)
{
    SDLViewer viewer;
    SDLViewer viewer1(640,480);        
    //SDLViewer viewer2(800,600);
    //SDLViewer viewer3(1024,768);
    
    //SDLViewer viewer(640, 480, SDLviewer::WINDOWED);
    //SDLViewer viewer(640, 480, SDLviewer::FULLSCREEN);
    	
	viewer = SDLViewer::getInstance();
	
	//viewer.init();
	//viewer.startRendering();
    //viewer.cleanup();
    viewer.run();
    
}
