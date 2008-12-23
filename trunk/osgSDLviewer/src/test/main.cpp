/*
 *  main.cpp
 *
 */

#include "main.h"
#include "../SimulationEngine.h"

int main(int argc, char *argv[])
{
	// OSG init
    osg::osgInit(argc, argv);

	// 1. 
	//test_viewer();
	    
	// 2. 
	//test_sim();

	// 3. 
	test_solid();
    
    return 0;
}
