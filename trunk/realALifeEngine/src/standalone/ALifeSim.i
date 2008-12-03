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
%}


/*
%typemap(in,checkfn="lua_isnumber") opal::real *INPUT(opal::real temp), opal::real &INPUT(opal::real temp)
%{ temp = (opal::real)lua_tonumber(L,$input);
   $1 = &temp; %}

%typemap(in, numinputs=0) opal::real *OUTPUT (opal::real temp)
%{ $1 = &temp; %}

%typemap(argout) opal::real *OUTPUT
%{  lua_pushnumber(L, (double) *$1); SWIG_arg++;%}

%typemap(in) opal::real *INOUT = opal::real *INPUT;
%typemap(argout) opal::real *INOUT = opal::real *OUTPUT;
*/
//SWIG_NUMBER_TYPEMAP(opal::real);
%apply opal::real *INOUT{opal::real *};
%apply opal::real &INPUT{opal::real &};
//%apply Vec3r *INOUT{Vec3r *};
//%apply Vec3r &INPUT{Vec3r &};


// Minimal Opal Wrapping 
%include "opal/Portability.h"
%include "opal/EventHandler.h"
%include "opal/CollisionEventHandler.h"
%include "opal/OpalMath.h"

%ignore opal::real;
%ignore operator<<(std::ostream& o, const Vec3r& v);
//%ignore operator+(const Vec3r &u, const Vec3r &v);
//%rename(__add__) Vec3r::operator+;
%ignore operator<<(std::ostream& o, const Point3r& p);
%ignore operator<<(std::ostream& o, const Rayr& r);
%ignore operator<<(std::ostream& o, const Quaternion& q);
%ignore operator<<(std::ostream& o, const Matrix44r& m);

%include "opal/Vec3r.h"
%include "opal/Point3r.h"
%include "opal/Rayr.h"
%include "opal/Quaternion.h"
%include "opal/Matrix44r.h"

%include "opal/Simulator.h"



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

//%include "verve/apps/AudioManager.h"
%include "verve/apps/DataCollection.h"
%include "verve/apps/DataFile.h"
%include "verve/apps/LearningTest.h"
%include "verve/apps/PhysicalCamera.h"
%include "verve/apps/PhysicalEntity.h"
%include "verve/apps/Timer.h"
