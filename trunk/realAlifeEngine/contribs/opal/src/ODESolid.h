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

#ifndef OPAL_ODE_SOLID_H
#define OPAL_ODE_SOLID_H

#include "external/ode/include/ode/ode.h"
#include "Defines.h"
#include "Solid.h"
#include "Blueprint.h"

namespace opal
{
	/// A data structure used within ODESolids to store data describing 
	/// ODE geoms.
	struct GeomData
	{
		GeomData()
		{
			solid = NULL;
			shape = NULL;
			geomID = 0;
			spaceID = 0;
			transformID = 0;
			trimeshDataID = 0;
		}

		Solid* solid;
		ShapeData* shape;
		dGeomID geomID;
		dSpaceID spaceID;
		dGeomID transformID;
		dTriMeshDataID trimeshDataID; // only used for Solids with trimeshes
	};

	/// The ODE implementation of the Solid class.  Each ODE geom's user 
	/// data pointer points to its corresponding ShapeData object.
	class ODESolid : public Solid
	{
	public:
		ODESolid(dWorldID worldID, dSpaceID spaceID);

		virtual ~ODESolid();

		virtual void OPAL_CALL init(const SolidData& data);

		virtual void OPAL_CALL setEnabled(bool e);

		virtual void OPAL_CALL setSleeping(bool sleeping);

		virtual void OPAL_CALL setSpace(Space* newSpace);

		virtual bool OPAL_CALL isSleeping()const;

		virtual void OPAL_CALL setSleepiness(real s);

		virtual void OPAL_CALL setStatic(bool s);

		virtual void OPAL_CALL setGravityEnabled(bool e);

		virtual void OPAL_CALL clearShapes();

		virtual void OPAL_CALL addShape(ShapeData& data);

		virtual void OPAL_CALL setLocalLinearVel(const Vec3r& vel);

		virtual Vec3r OPAL_CALL getLocalLinearVel()const;

		virtual Vec3r OPAL_CALL getLocalLinearVelAtLocalPos(
			const Point3r& p)const;

		virtual Vec3r OPAL_CALL getLocalLinearVelAtGlobalPos(
			const Point3r& p)const;

		virtual void OPAL_CALL setLocalAngularVel(const Vec3r& vel);

		virtual Vec3r OPAL_CALL getLocalAngularVel()const;

		virtual void OPAL_CALL setGlobalLinearVel(const Vec3r& vel);

		virtual Vec3r OPAL_CALL getGlobalLinearVel()const;

		virtual Vec3r OPAL_CALL getGlobalLinearVelAtLocalPos(
			const Point3r& p)const;

		virtual Vec3r OPAL_CALL getGlobalLinearVelAtGlobalPos(
			const Point3r& p)const;

		virtual void OPAL_CALL setGlobalAngularVel(const Vec3r& vel);

		virtual Vec3r OPAL_CALL getGlobalAngularVel()const;

		//virtual void OPAL_CALL setMass(const Mass & newmass, const Matrix44r & offset);
		virtual void OPAL_CALL setMass(const Mass & newmass);

		//virtual void OPAL_CALL translateMass(const Vec3r& offset);

		//// call dBodySetFiniteRotationMode and dBodySetFiniteRotationAxis (the 
		//// latter every frame); see ODE docs
		//virtual void OPAL_CALL setFastRotation(bool fast);

		//virtual bool OPAL_CALL getFastRotation()const;

		//virtual void OPAL_CALL setFastRotationAxis(Vec3r axis);

		virtual void OPAL_CALL zeroForces();

		virtual real OPAL_CALL getMass()const;

		virtual Matrix44r OPAL_CALL getInertiaTensor()const;

		virtual void internal_updateOPALTransform();

		virtual void internal_updateEngineTransform();

		/// Returns the ODE body ID of this ODESolid.  
		dBodyID internal_getBodyID()const;

		/// Used internally by volume collision checking.  
		void internal_setCollisionCount(long int count);

		/// Used internally by volume collision checking.  
		long int internal_getCollisionCount()const;

		/// Returns all geom data objects in this ODESolid.  
		const std::vector<GeomData*>* internal_getGeomDataList()const;

		/// Fix angular velocities for freely-spinning bodies that have 
		/// gained angular velocity through explicit integrator inaccuracy.  
		/// This usually only happens for long, thin objects.
		void internal_doAngularVelFix();

		/// Sets whether this object is freely-spinning (i.e. no recent 
		/// physical contacts, no user-generated forces, no user-defined 
		/// velocity changes).
		void internal_setFreelySpinning(bool fs);

	protected:
		/// Adds a new GeomData object to the internal list and sets up the 
		/// ODE geom.
		void setupNewGeom(GeomData* newGeom);

		/// Destroys all of this Solid's ODE geoms.
		void destroyGeoms();

		/// move to geometry to current space
		void moveToSpace();

		virtual void applyForce(const Force& f);

		/// Adds the given mass to this Solid's existing mass.  The offset is 
		/// relative to the Solid's center.  This must be called before 
		/// setupNewGeom is called.
		//void addMass(dMass& newMass, const Matrix44r& offset);
		void addMass(dMass& newMass);

		//! Sets the given mass ignoring previous settings.
		//void setMass(dMass& newMass, const Matrix44r& offset);
		void setMass(dMass& newMass);

		/// Returns true if the given mass has a non-symmetric inertia tensor.
		bool isInertiaNonSymmetric(const dMass& mass)const;

		/// The ODE body ID.  
		dBodyID mBodyID;

		/// The ODE world ID that contains this ODESolid's ODE body, if this 
		/// Solid is dynamic.
		dWorldID mWorldID;

		/// The ODE space ID that contains this ODESolid's geoms.
		dSpaceID mSpaceID;

		/// An internal list of this ODESolid's geom data objects.  
		std::vector<GeomData*> mGeomDataList;

		/// Used internally by volume collision checking.  
		long int mCollisionCount;

		/// True if this ODESolid is "placeable."  This is false only for 
		/// Solids containing planes Shapes.
		bool mIsPlaceable;

		/// True if the ODESolid has a non-symmetric inertia tensor.
		bool mNonSymmetricInertia;

		/// Used to improve ODE's angular velocity calculations for objects 
		/// with non-symmetric inertia tensors.
		bool mIsFreelySpinning;

		/// Used to improve ODE's angular velocity calculations for objects 
		/// with non-symmetric inertia tensors.
		real mPrevAngVelMagSquared;

	private:
	};
}

#endif
