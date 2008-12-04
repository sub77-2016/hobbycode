%module al
%include "typemaps.i"

%{
/* Includes the header in the wrapper code */
#include <SDL/SDL.h>
#include <OGRE/Ogre.h>
#include <opal/opal.h>
//#include <audiere.h>

#include <verve/Agent.h>
#include <verve/apps/SimulationEngine.h>
#include <verve/apps/Robot.h>
#include <verve/apps/Car.h>
#include <verve/apps/AgentVisualDebugger.h>
//#include "verve/apps/AudioManager.h"
#include <verve/apps/DataCollection.h>
#include <verve/apps/DataFile.h>
#include <verve/apps/LearningTest.h>
#include <verve/apps/PhysicalCamera.h>
#include <verve/apps/PhysicalEntity.h>
#include <verve/apps/Timer.h>

using namespace opal;
//using namespace Ogre;
%}

%include "min_opal.i"
%include "min_ogre.i"

// Agent Layer
%include "verve/Defines.h"
%include "verve/Platform.h"
%include "verve/Observation.h"
%include "verve/AgentDescriptor.h"
%include "verve/Agent.h"

%ignore verve::real;

// Application Layer
%include "verve/apps/SimulationEngine.h"
%include "verve/apps/AgentVisualDebugger.h"
%include "verve/apps/LearningEntity.h"
%include "verve/apps/Car.h"
%include "verve/apps/Robot.h"

/*
//%include "verve/apps/AudioManager.h"
%include "verve/apps/DataCollection.h"
%include "verve/apps/DataFile.h"
%include "verve/apps/LearningTest.h"
%include "verve/apps/PhysicalCamera.h"
%include "verve/apps/PhysicalEntity.h"
//%include "verve/apps/Timer.h"
*/
