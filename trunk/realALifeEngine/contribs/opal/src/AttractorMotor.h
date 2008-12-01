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

#ifndef OPAL_ATTRACTOR_MOTOR_H
#define OPAL_ATTRACTOR_MOTOR_H

#include "Defines.h"
#include "Motor.h"
#include "AttractorMotorData.h"

namespace opal
{
	/// This type of Motor attracts two Solids together by applying 
	/// gravitational forces on each Solid every time step.  It uses the 
	/// equation force magnitude = (strength * m0 * m1) / distance^exponent, 
	/// where "strength" and "exponent" are changeable parameters.  This 
	/// can work with dynamic-dynamic and dynamic-static Solid pairs, but 
	/// it will do nothing for static-static Solid pairs.
	class AttractorMotor : public Motor
	{
	public:
		AttractorMotor();

		virtual ~AttractorMotor();

		/// Initializes the Motor with the given data structure.  If the 
		/// Solid pointers in the data are NULL, the Motor will do nothing.
		virtual void OPAL_CALL init(const AttractorMotorData& data);

		/// Returns all data describing the Motor.
		virtual const AttractorMotorData& OPAL_CALL getData()const;

		virtual MotorType OPAL_CALL getType()const;

		virtual void OPAL_CALL setName(const std::string& name);

		virtual const std::string& OPAL_CALL getName()const;

		virtual bool OPAL_CALL isEnabled()const;

		virtual void OPAL_CALL setEnabled(bool e);

		/// Sets the strength parameter.
		virtual void OPAL_CALL setStrength(real s);

		/// Returns the strength parameter.
		virtual real OPAL_CALL getStrength()const;

		/// Sets the exponent parameter.
		virtual void OPAL_CALL setExponent(real e);

		/// Returns the exponent parameter.
		virtual real OPAL_CALL getExponent()const;

		virtual void internal_update();

		virtual bool internal_dependsOnSolid(Solid* s);

	protected:
		/// Stores data describing the Motor.
		AttractorMotorData mData;

		/// Cached copy of Solid 0's mass.
		real mSolid0Mass;

		/// Cached copy of Solid 1's mass.
		real mSolid1Mass;

		/// Cached copy of strength * m0 * m1.   
		real mMassConstant;

	private:

	};
}

#endif
