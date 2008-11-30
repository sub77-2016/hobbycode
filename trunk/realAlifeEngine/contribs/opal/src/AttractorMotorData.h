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

#ifndef OPAL_ATTRACTOR_MOTOR_DATA_H
#define OPAL_ATTRACTOR_MOTOR_DATA_H

#include "Defines.h"
#include "MotorData.h"

namespace opal
{
	/// A data structure describing an AttractorMotor.
	class AttractorMotorData : public MotorData
	{
	public:
		AttractorMotorData()
		: MotorData()
		{
			mType = ATTRACTOR_MOTOR;
			solid0 = NULL;
			solid1 = NULL;
			internal_solid0Index = 0;
			internal_solid1Index = 0;
			solid0BlueprintRefName = "";
			solid1BlueprintRefName = "";
			strength = defaults::motor::attractor::strength;
			exponent = defaults::motor::attractor::exponent;
		}

		/// Copy constructor.
		AttractorMotorData(const AttractorMotorData& data)
		{
			(*this) = data;
		}

		virtual ~AttractorMotorData()
		{
		}

		/// Makes a deep copy.
		virtual void operator=(const AttractorMotorData& data)
		{
			mType = data.mType;
			enabled = data.enabled;
			name = data.name;
			solid0 = data.solid0;
			solid1 = data.solid1;
			internal_solid0Index = data.internal_solid0Index;
			internal_solid1Index = data.internal_solid1Index;
			solid0BlueprintRefName = data.solid0BlueprintRefName;
			solid1BlueprintRefName = data.solid1BlueprintRefName;
			strength = data.strength;
			exponent = data.exponent;
		}

		/// Pointer to Solid 0.
		Solid* solid0;

		/// Pointer to Solid 1.
		Solid* solid1;

		/// Internal data used for Blueprint instantiation.
		int internal_solid0Index;

		/// Internal data used for Blueprint instantiation.
		int internal_solid1Index;

		/// String used for for intra-Blueprint references.
		std::string solid0BlueprintRefName;

		/// String used for for intra-Blueprint references.
		std::string solid1BlueprintRefName;

		/// Constant used to scale attraction (repulsion if strength < 0).  
		real strength;

		/// Determines exponential relationship of attraction/repulsion.  
		real exponent;

	protected:

	private:
	};
}

#endif
