%module ALifeSim
%include "typemaps.i"

%{
/* Includes the header in the wrapper code */
#include "../verve/base/Agent.h"
%}

%include "../verve/base/Defines.h"
%include "../verve/base/Platform.h"
%include "../verve/base/Observation.h"
%include "../verve/base/AgentDescriptor.h"
%include "../verve/base/Agent.h"
