%module ALifeSim
%include "typemaps.i"

%{
/* Includes the header in the wrapper code */
#include <SDL/SDL.h>
#include <ogre/Ogre.h>
#include <opal/opal.h>

#include <verve/Agent.h>
#include <verve/apps/SimulationEngine.h>
#include <verve/apps/Robot.h>
#include <verve/apps/Car.h>
#include <verve/apps/AgentVisualDebugger.h>
%}

%init {
using namespace opal;
%}

// Wrapping script 
%include "opal/Portability.h"
%include "opal/EventHandler.h"
%include "opal/CollisionEventHandler.h"

// Agent Layer
%include "verve/Defines.h"
%include "verve/Platform.h"
%include "verve/Observation.h"
%include "verve/AgentDescriptor.h"
%include "verve/Agent.h"

// Application Layer
%include "verve/apps/SimulationEngine.h"
%include "verve/apps/AgentVisualDebugger.h"
%include "verve/apps/LearningEntity.h"
%include "verve/apps/Car.h"
%include "verve/apps/Robot.h"

