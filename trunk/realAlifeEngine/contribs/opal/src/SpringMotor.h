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

#ifndef OPAL_SPRING_MOTOR_H
#define OPAL_SPRING_MOTOR_H

#include "Defines.h"
#include "Matrix44r.h"
#include "Motor.h"
#include "SpringMotorData.h"

namespace opal
{
	/// This is a spring that either 1) operates in 3 dimensions to keep
	/// a Solid in a desired position, or 2) operates in 3 degrees of
	/// rotational motion to keep a Solid in a desired orientation.  It can
	/// also do both modes at once.
	class SpringMotor : public Motor
	{
	public:
		SpringMotor();

		virtual ~SpringMotor();

		/// Initializes the Motor with the given data structure.  If the
		/// Solid pointer in the data are NULL, the Motor will do nothing.
		virtual void OPAL_CALL init(const SpringMotorData& data);

		/// Returns all data describing the Motor.
		virtual const SpringMotorData& OPAL_CALL getData()const;

		virtual MotorType OPAL_CALL getType()const;

		virtual void OPAL_CALL setName(const std::string& name);

		virtual const std::string& OPAL_CALL getName()const;

		virtual bool OPAL_CALL isEnabled()const;

		virtual void OPAL_CALL setEnabled(bool e);

		/// Changes which Solid is controlled by the spring.
		virtual void OPAL_CALL setSolid(Solid* s);

		/// Returns the Solid which is controlled by the spring.
		virtual Solid* OPAL_CALL getSolid()const;

		/// Sets the spring's attach point on the Solid.  This is a local
		/// offset point from the Solid's position.
		virtual void OPAL_CALL setLocalAttachPoint(const Point3r& p);

		/// Returns the spring's attach point on the Solid.  This is a local
		/// offset point from the Solid's position.
		virtual const Point3r& OPAL_CALL getLocalAttachPoint()const;

		/// Sets the spring's attach point on the Solid in global
		/// coordinates.
		virtual void OPAL_CALL setGlobalAttachPoint(const Point3r& p);

		/// Returns the spring's attach point on the Solid in global
		/// coordinates.
		virtual Point3r OPAL_CALL getGlobalAttachPoint()const;

		/// Sets the desired position and orientation.
		virtual void OPAL_CALL setDesiredTransform(const Matrix44r& transform);

		/// Sets the desired position.
		virtual void OPAL_CALL setDesiredPosition(const Point3r& pos);

		/// Sets the desired orientation.
		virtual void OPAL_CALL setDesiredOrientation(const Vec3r& forward,
			const Vec3r& up, const Vec3r& right);

		/// Sets the damping constant for linear mode.
		virtual void OPAL_CALL setLinearKd(real kd);

		/// Sets the spring constant for linear mode.
		virtual void OPAL_CALL setLinearKs(real ks);

		/// Returns the damping constant for linear mode.
		virtual real OPAL_CALL getLinearKd()const;

		/// Returns the spring constant for linear mode.
		virtual real OPAL_CALL getLinearKs()const;

		/// Sets the damping constant for angular mode.
		virtual void OPAL_CALL setAngularKd(real kd);

		/// Sets the spring constant for angular mode.
		virtual void OPAL_CALL setAngularKs(real ks);

		/// Returns the damping constant for angular mode.
		virtual real OPAL_CALL getAngularKd()const;

		/// Returns the spring constant for angular mode.
		virtual real OPAL_CALL getAngularKs()const;

		virtual void internal_update();

		virtual bool internal_dependsOnSolid(Solid* s);

	protected:
		/// Stores data describing the Motor.
		SpringMotorData mData;

	private:
	};
}

#endif
