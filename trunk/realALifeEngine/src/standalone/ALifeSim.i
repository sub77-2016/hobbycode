%module ALifeSim
%include "typemaps.i"

%{
/* Includes the header in the wrapper code */
#include <verve/Agent.h>
%}

%include "Defines.h"
%include "Platform.h"
%include "Observation.h"
%include "AgentDescriptor.h"
%include "Agent.h"
