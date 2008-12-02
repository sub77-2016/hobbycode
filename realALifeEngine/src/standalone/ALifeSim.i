%module ALifeSim
%include "typemaps.i"

%{
/* Includes the header in the wrapper code */
#include <SDL/SDL.h>
#include <ogre/Ogre.h>

#include <verve/Agent.h>
#include <verve/apps/SimulationEngine.h>
#include <verve/apps/Robot.h>
#include <verve/apps/Car.h>
#include <verve/apps/AgentVisualDebugger.h>
%}

// Agent Layer
%include "Defines.h"
%include "Platform.h"
%include "Observation.h"
%include "AgentDescriptor.h"
%include "Agent.h"

// Application Layer
%include "apps/SimulationEngine.h"
%include "apps/Robot.h"
%include "apps/Car.h"
%include "apps/AgentVisualDebugger.h"
