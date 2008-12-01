/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
*                                                                       *
* This library is free software; you can redistribute it and/or         *
* modify it under the terms of EITHER:                                  *
*   (1) The GNU Lesser General Public License as published by the Free  *
*       Software Foundation; either version 2.1 of the License, or (at  *
*       your option) any later version. The text of the GNU Lesser      *
*       General Public License is included with this library in the     *
*       file license-LGPL.txt.                                          *
*   (2) The BSD-style license that is included with this library in     *
*       the file license-BSD.txt.                                       *
*                                                                       *
* This library is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
* license-LGPL.txt and license-BSD.txt for more details.                *
************************************************************************/

#ifndef OPAL_DEFINES_H
#define OPAL_DEFINES_H

// Put this here for now.  They seem to nail all the really annoying and
// useless VC6 warning: 4786, Debug info too long
#if defined(WIN32) || defined(_WIN32)
	#pragma warning(disable:4786)
#endif

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <assert.h>
#include "OpalMath.h"
#include "Portability.h"
#include "Logger.h"
#include "Vec3r.h"
#include "Point3r.h"

/// The main namespace that contains everything in OPAL.
namespace opal
{
	class Solid;
	class Joint;

	/// Types of Forces.  Used when creating a Force struct to designate
	/// how the Forces should be applied to a Solid.
	enum ForceType
	{
		/// Apply a force in a direction relative to the Solid's local
		/// coordinate system.
		LOCAL_FORCE,

		/// Apply a force in a direction relative to the global
		/// coordinate system.
		GLOBAL_FORCE,

		/// Apply a torque with the axis specified relative to the Solid's
		/// local coordinate system.
		LOCAL_TORQUE,

		/// Apply a torque with the axis specified relative to global
		/// coordinate system.
		GLOBAL_TORQUE,

		/// Apply a force at a position relative to the Solid's local
		/// coordinate system in a direction relative to the Solid's local
		/// coordinate system.
		LOCAL_FORCE_AT_LOCAL_POS,

		/// Apply a force at a position relative to the global
		/// coordinate system in a direction relative to the Solid's local
		/// coordinate system.
		LOCAL_FORCE_AT_GLOBAL_POS,

		/// Apply a force at a position relative to the Solid's local
		/// coordinate system in a direction relative to the global
		/// coordinate system.
		GLOBAL_FORCE_AT_LOCAL_POS,

		/// Apply a force at a position relative to the global
		/// coordinate system in a direction relative to the global
		/// coordinate system.
		GLOBAL_FORCE_AT_GLOBAL_POS
	};

	/// Data structure containing all necessary info for applying a
	/// force/torque to a Solid.
	struct Force
	{
		Force()
		{
			type = LOCAL_FORCE;
			duration = 0;
			singleStep = false;
			// "vec" is initialized in its own constructor.
			// "pos" is initialized in its own constructor.
		}

		/// Specifies the type of force.
		ForceType type;

		/// Specifies how long to apply to force.  This makes it easy to
		/// apply forces independent of the step size.  This will be ignored
		/// if "singleStep" is true.
		real duration;

		/// This is mainly used internally by OPAL.  It specifies that
		/// the force will be applied across a single time step.  If this
		/// parameter is true, "duration" will be ignored.
		bool singleStep;

		/// The force direction or torque axis.  This parameter encodes the
		/// magnitude of the force or torque.
		Vec3r vec;

		/// Only used when the force is applied at an offset (i.e. not
		/// the center of mass).
		Point3r pos;
	};

	/// Data structure describing material properties.  These properties
	/// determine the collision response when two Solids collide.
	struct Material
	{
		Material(real h, real f, real b, real d)
		{
			//assert(h >= 0.0 && h <= 1.0);
			//assert(f >= 0.0 && f <= 1.0);
			//assert(b >= 0.0 && b <= 1.0);
			//assert(d >= 0.0);

			hardness = h;
			friction = f;
			bounciness = b;
			density = d;
		}

		Material()
		{
			hardness = (real)0.5;
			friction = (real)0.5;
			bounciness = (real)0.5;
			density = (real)0.5;
		}

		/// Determines how far Solids can interpenetrate.  This must
		/// be between 0 and 1.
		real hardness;

		/// Simple friction constant.  This must be between 0 and 1.
		real friction;

		/// Bounciness (i.e. restitution) determines how elastic the
		/// collisions will be between this Material and another.  In other
		/// words, the more bounciness, the farther the Solids will bounce
		/// when they collide (and, in real life, the less energy is lost
		/// due to heat and sound).  This must be between 0 and 1.
		real bounciness;

		/// Density combined with the volume of a Solid's shapes determine
		/// the Solid's mass.  This must be >= 0.
		real density;
	};

	/// Solver accuracy levels determine how the physics engine constraint
	/// solver is used (e.g. the number of iterations in an iterative
	/// solver).
	enum SolverAccuracyLevel
	{
		SOLVER_ACCURACY_VERY_LOW,
		SOLVER_ACCURACY_LOW,
		SOLVER_ACCURACY_MEDIUM,
		SOLVER_ACCURACY_HIGH,
		SOLVER_ACCURACY_VERY_HIGH
	};

	/// Joints use different break modes to determine how they are damaged
	/// from stress.
	enum JointBreakMode
	{
		/// Joint can never break.
		UNBREAKABLE_MODE,

		/// Joint breaks when force/torque exceeds a threshold.
		THRESHOLD_MODE,

		/// Joint breaks when enough damage is accumulated.
		ACCUMULATED_MODE
	};

	/// The types of Joints currently available.
	enum JointType
	{
		/// Anchor: used
		/// Axis 0: rotational
		/// Axis 1: not used
		/// Axis 2: not used
		HINGE_JOINT,

		/// Anchor: used
		/// Axis 0: rotational
		/// Axis 1: rotational
		/// Axis 2: not used
		UNIVERSAL_JOINT,

		/// Anchor: used
		/// Axis 0: rotational
		/// Axis 1: rotational (need not be set; calculated automatically)
		/// Axis 2: rotational
		BALL_JOINT,

		/// Anchor: not used
		/// Axis 0: linear
		/// Axis 1: not used
		/// Axis 2: not used
		SLIDER_JOINT,

		/// Anchor: used
		/// Axis 0: rotational ("steering axis")
		/// Axis 1: rotational ("wheel axis")
		/// Axis 2: not used
		WHEEL_JOINT,

		/// Anchor: not used
		/// Axis 0: not used
		/// Axis 1: not used
		/// Axis 2: not used
		FIXED_JOINT
	};

	/// A namespace containing globally useful parameters.
	namespace globals
	{
		/// The highest value that can be used for the Simulator's
		/// max contacts parameter.
		const unsigned int maxMaxContacts = 128;

		// Pre-defined material settings
		const real metalHardness = (real)1.0;
		const real metalFriction = (real)0.7;
		const real metalBounciness = (real)0.05;
		const real metalDensity = (real)1.0;

		//NOTE: Max and mix friction settings result in faster calculations.
		const real woodHardness = (real)0.8;
		const real woodFriction = (real)1.0;
		const real woodBounciness = (real)0.15;
		const real woodDensity = (real)0.2;

		const real rubberHardness = 0;
		const real rubberFriction = (real)1.0;
		const real rubberBounciness = (real)1.0;
		const real rubberDensity = (real)0.4;

		const real iceHardness = (real)1.0;
		const real iceFriction = 0;
		const real iceBounciness = (real)0.05;
		const real iceDensity = (real)0.25;

		// Global materials
		const Material metalMaterial(metalHardness, metalFriction,
			metalBounciness, metalDensity);
		const Material woodMaterial(woodHardness, woodFriction,
			woodBounciness, woodDensity);
		const Material rubberMaterial(rubberHardness, rubberFriction,
			rubberBounciness, rubberDensity);
		const Material iceMaterial(iceHardness, iceFriction,
			iceBounciness, iceDensity);
	}

	/// A namespace containing all default parameter values used in OPAL.
	namespace defaults
	{
		const real stepSize = (real)0.0167; // ~60 Hz physics update rate.
		const Vec3r gravity(0, 0, 0);
		const real bounceThreshold = (real)1.0;
		const SolverAccuracyLevel solverAccuracy = SOLVER_ACCURACY_MEDIUM;
		//const bool allowPartialFrames=true;
		//const int maxStepsPerFrame=10;
		//const real maxLinearVel = (real)10000.0;
		//const real maxAngularVel = (real)1000.0;
		const bool staticSleepingContactsEnabled = false;
		const real maxCorrectingVel=(real)40.0;
		const int maxContacts=(int)24;

		/// All groups make contacts with all other groups by default.
		const unsigned long int contactGroupFlags = 0xFFFFFFFF;

		/// Default parameters used in Solid creation.
		namespace solid
		{
			const bool enabled = true;
			const bool sleeping = true;
			const real sleepiness = (real)0.5;
			const bool isStatic = false;
			const bool gravityEnabled = true;
			const real linearDamping = (real)0.15;
			const real angularDamping = (real)0.15;
		}

		/// Default parameters used in Shape creation.
		namespace shape
		{
			const Material material = globals::woodMaterial;
			const unsigned int contactGroup = 0;
			const Vec3r boxDimensions = Vec3r(1, 1, 1);
			const real sphereRadius = 1;
			const real capsuleRadius = 1;
			const real capsuleLength = 1;
			const real cylinderRadius = 1;
			const real cylinderLength = 1;
			const real planeABCD[4] = {0, 1, 0, 0};
		}

		/// Default parameters used in Joint creation.
		namespace joint
		{
			const JointType type = HINGE_JOINT;
			const bool enabled = true;
			const real lowLimit = 0;
			const real highLimit = 0;
			const real limitHardness = (real)0.5;
			const real limitBounciness = (real)0.5;
			const Vec3r axis0Direction = Vec3r(1, 0, 0);
			const Vec3r axis1Direction = Vec3r(0, 1, 0);
			const Vec3r axis2Direction = Vec3r(0, 0, 1);
			const bool limitsEnabled = false;

			const Point3r anchor = Point3r(0, 0, 0);
			const JointBreakMode breakMode = UNBREAKABLE_MODE;
			const real breakThresh = (real)100.0;
			const real accumThresh = (real)1000.0;
			const bool contactsEnabled = false;
		}

		/// Default parameters used in Motor creation.
		namespace motor
		{
			const bool enabled = true;

			/// Default parameters used in AttractorMotor creation.
			namespace attractor
			{
				const real strength = (real)1.0;
				const real exponent = (real)1.0;
			}

			/// Default parameters used in GearedMotor creation.
			namespace geared
			{
				const real maxTorque = (real)10.0;
				const real maxVelocity = (real)10.0;
			}

			/// Default parameters used in ServoMotor creation.
			namespace servo
			{
				const real maxTorque = (real)10.0;
				const real restoreSpeed = (real)1.0;
			}

			/// Default parameters used in SpringMotor creation.
			namespace spring
			{
				const real linearKd = (real)0.1;
				const real linearKs = (real)1.0;
				const real angularKd = (real)0.1;
				const real angularKs = (real)1.0;
				const Vec3r desiredForward = Vec3r(0, 0, -1);
				const Vec3r desiredUp = Vec3r(0, 1, 0);
				const Vec3r desiredRight = Vec3r(1, 0, 0);
			}
		}

		/// Default parameters used in Sensor creation.
		namespace sensor
		{
			const bool enabled = true;

			/// Default parameters used in InclineSensor creation.
			namespace incline
			{
				const Vec3r axis = Vec3r(1, 0, 0);
			}
		}

		/// Default parameters specific to the ODE physics engine.
		namespace ode
		{
			// ODE-specific defaults
			const real autoDisableLinearMin=0;
			const real autoDisableLinearMax=(real)0.2;
			const real autoDisableAngularMin=0;
			const real autoDisableAngularMax=(real)0.2;
			const int autoDisableStepsMin=(int)1;
			const int autoDisableStepsMax=(int)60;
			const real autoDisableTimeMin=0;
			const real autoDisableTimeMax=(real)0.4;
			const real minMassRatio=(real)0.001;

			//note: max and min mass ratios must be the inverse of each other
			//const real maxMassRatio=(real)1000.0;
			const real minERP=(real)0.01;
			const real maxERP=(real)0.99;
			const real globalCFM=(real)1e-5;
			const real jointFudgeFactor=(real)0.1;
			const real maxFriction=(real)1000.0;
			const real surfaceLayer=(real)0.001;
			const int maxRaycastContacts=10;
		}
	}
}

#endif
