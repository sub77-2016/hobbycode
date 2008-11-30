%module pyverve
%{
/* Includes the header in the wrapper code */
#include "../../src/Agent.h"
%}

%include "../../src/Defines.h"
%include "../../src/Platform.h"
%include "../../src/Observation.h"
%include "../../src/AgentDescriptor.h"
%include "../../src/Agent.h"
