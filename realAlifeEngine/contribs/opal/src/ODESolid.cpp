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

// class headers
#include "ODESolid.h"

// project headers
#include "ODESpace.h"
#include "ODETools.h"
#include "BoxShapeData.h"
#include "SphereShapeData.h"
#include "CapsuleShapeData.h"
#include "CylinderShapeData.h"
#include "PlaneShapeData.h"
#include "MeshShapeData.h"
#include "Mass.h"

namespace opal
{
	ODESolid::ODESolid(dWorldID worldID, dSpaceID spaceID)
			: Solid()
	{
		mWorldID = worldID;
		mSpaceID = spaceID;
		mIsPlaceable = true;
		mCollisionCount = 0;
		mNonSymmetricInertia = false;
		mIsFreelySpinning = true;
		mPrevAngVelMagSquared = 0;

		if (!mData.isStatic)
		{
			// Create an ODE body with default ODE mass parameters (total
			// mass = 1).  This should have an initial mass of 0 until shapes
			// are added, but ODE won't allow a mass of 0.  This will be
			// adjusted appropriately when the first shape is added.
			mBodyID = dBodyCreate(mWorldID);
		}

		init(mData);
	}

	ODESolid::~ODESolid()
	{
		destroyGeoms();

		if (!mData.isStatic)
		{
			// This is a dynamic solid, so it has an ODE body that needs
			// to be destroyed.
			dBodyDestroy(mBodyID);
		}
	}

	void ODESolid::clearShapes()
	{
		destroyGeoms();
		resetAABB();
		mData.destroyShapes();
	}

	void ODESolid::destroyGeoms()
	{
		for (size_t i = 0; i < mGeomDataList.size(); ++i)
		{
			if (0 != mGeomDataList[i]->transformID)
			{
				dGeomDestroy(mGeomDataList[i]->transformID);
			}

			if (0 != mGeomDataList[i]->trimeshDataID)
			{
				// This geom uses a trimesh.  Destroy it.  (This does NOT
				// touch the user's mesh data, just internal ODE trimesh
				// data.)
				dGeomTriMeshDataDestroy(mGeomDataList[i]->trimeshDataID);
			}

			// Destroy the ODE geom.
			dGeomDestroy(mGeomDataList[i]->geomID);

			// Delete the geom data object.
			delete mGeomDataList[i];
		}
		mGeomDataList.clear();
	}

	void ODESolid::init(const SolidData& data)
	{
		// The order of function calls here is important.

		// Destroy the old Shapes.
		mData.destroyShapes();

		// Destroy the old ODE geoms.
		destroyGeoms();

		// Set whether the Solid is static.
		setStatic(data.isStatic);

		// Set the new transform.
		setTransform(data.transform);

		// Add the new Shapes.
		for (unsigned int i = 0; i < data.getNumShapes(); ++i)
		{
			addShape(*(data.getShapeData(i)));
		}

		// Set whether the Solid is sleeping.
		setSleeping(data.sleeping);

		// Set the Solid's sleepiness level.
		setSleepiness(data.sleepiness);

		// Set whether the Solid is enabled.
		setEnabled(data.enabled);

		// Set damping levels.
		setLinearDamping(data.linearDamping);
		setAngularDamping(data.angularDamping);

		// Set velocities.
		setGlobalLinearVel(data.globalLinearVel);
		setGlobalAngularVel(data.globalAngularVel);

		// Set the Solid's name.
		setName(data.name);
	}

	void ODESolid::setEnabled(bool e)
	{
		Solid::setEnabled(e);

		if (e)
		{
			// If this Solid is dynamic, enable the ODE body.
			if (!mData.isStatic)
			{
				dBodyEnable(mBodyID);
			}

			// Enable the ODE geoms.
			std::vector<GeomData*>::iterator iter = mGeomDataList.begin();
			std::vector<GeomData*>::iterator end = mGeomDataList.end();
			for (; iter != end; ++iter)
			{
				dGeomEnable((*iter)->geomID);
				if(0 != (*iter)->transformID)
				{
					dGeomEnable((*iter)->transformID);
				}
			}
		}
		else // Disable the Solid.
		{
			// If this Solid is dynamic, disable the ODE body.
			if (!mData.isStatic)
			{
				dBodyDisable(mBodyID);
			}

			// Disable the ODE geoms.
			std::vector<GeomData*>::iterator iter = mGeomDataList.begin();
			std::vector<GeomData*>::iterator end = mGeomDataList.end();
			for (; iter != end; ++iter)
			{
				dGeomDisable((*iter)->geomID);
				if(0 != (*iter)->transformID)
				{
					dGeomDisable((*iter)->transformID);
				}
			}
		}
	}

	void ODESolid::internal_updateOPALTransform()
	{
		const real * R = (const real*) dBodyGetRotation(mBodyID);
		const real* P = (const real*) dBodyGetPosition(mBodyID);

		mData.transform.set(R[0], R[1], R[2], P[0],
		                     R[4], R[5], R[6], P[1],
		                     R[8], R[9], R[10], P[2],
		                     0, 0, 0, 1);
	}

	void ODESolid::internal_updateEngineTransform()
	{
		dMatrix3 R = {mData.transform[0], mData.transform[4],
		              mData.transform[8], 0, mData.transform[1], mData.transform[5],
		              mData.transform[9], 0, mData.transform[2], mData.transform[6],
		              mData.transform[10], 0};

		if (!mData.isStatic)
		{
			dBodySetRotation(mBodyID, R);
			dBodySetPosition(mBodyID, mData.transform[12],
				mData.transform[13], mData.transform[14]);
		}
		else if (mIsPlaceable)
		{
			std::vector<GeomData*>::iterator iter;
			for (iter = mGeomDataList.begin(); iter != mGeomDataList.end();
			        ++iter)
			{
				GeomData* geomData = (*iter);
				if (0 == geomData->transformID)
				{
					// No geom transform.
					dGeomSetRotation(geomData->geomID, R);
					dGeomSetPosition(geomData->geomID, mData.transform[12],
						mData.transform[13], mData.transform[14]);
				}
				else
				{
					// Using geom transform.
					dGeomSetRotation(geomData->transformID, R);
					dGeomSetPosition(geomData->transformID,
						mData.transform[12], mData.transform[13],
						mData.transform[14]);
				}
			}
		}
	}

	void ODESolid::setSpace(Space* newSpace)
	{
		// update solid's space which will be used for future shapes
		mSpaceID = ((ODESpace*) newSpace)->internal_getSpaceID();

		moveToSpace();
	}

	void ODESolid::setStatic(bool s)
	{
		if (true == mData.isStatic)
		{
			if (false == s)
			{
				// Make this object dynamic.
				mData.isStatic = false;

				// Create a new body.
				mBodyID = dBodyCreate(mWorldID);			

				// Set the ODE sleepiness params using the stored Solid
				// sleepiness param.
				setSleepiness(mData.sleepiness);

				// Set the position of the new body.
				dMatrix3 R =
				    {mData.transform[0], mData.transform[4],
				     mData.transform[8], 0, mData.transform[1],
				     mData.transform[5], mData.transform[9], 0,
				     mData.transform[2], mData.transform[6],
				     mData.transform[10], 0};
				dBodySetRotation(mBodyID, R);
				dBodySetPosition(mBodyID, mData.transform[12],
				                  mData.transform[13], mData.transform[14]);

				// Loop over the geoms.
				std::vector<GeomData*>::iterator iter;
				for (iter = mGeomDataList.begin();
				        iter != mGeomDataList.end(); ++iter)
				{
					dMass newMass;

					// Get a pointer to this geom's ShapeData.
					ShapeData* shapeData = (*iter)->shape;

					// Setup mass.
					switch (shapeData->getType())
					{
						case BOX_SHAPE:
							{
								BoxShapeData * boxData = (BoxShapeData*) shapeData;
								dMassSetBox(&newMass,
								             shapeData->material.density,
								             boxData->dimensions[0],
								             boxData->dimensions[1],
								             boxData->dimensions[2]);
								break;
							}
						case SPHERE_SHAPE:
							{
								SphereShapeData* sphereData =
								    (SphereShapeData*) shapeData;
								dMassSetSphere(&newMass,
								                shapeData->material.density,
								                sphereData->radius);
								break;
							}
						case CAPSULE_SHAPE:
							{
								CapsuleShapeData* capsuleData =
								    (CapsuleShapeData*) shapeData;
								dMassSetCappedCylinder(&newMass,
								                        shapeData->material.density, 3,
								                        capsuleData->radius,
								                        capsuleData->length);
								break;
							}
						case CYLINDER_SHAPE:
							{
								CylinderShapeData* cylinderData =
								    (CylinderShapeData*) shapeData;
								dMassSetCylinder(&newMass,
										  shapeData->material.density, 3,
										  cylinderData->radius,
										  cylinderData->length);
								break;
							}
						case PLANE_SHAPE:
							// Planes have no mass.
							break;
							//case RAY_SHAPE:
							//	// Rays have no mass.
							//	break;
						case MESH_SHAPE:
							dMassSetTrimesh(&newMass, 
								shapeData->material.density, (*iter)->geomID);
							break;
						default:
							assert(false);
					}

					// Setup the new mass.
					if (iter == mGeomDataList.begin())
						//setMass(newMass, shapeData->offset);
						setMass(newMass);
					else
						//addMass(newMass, shapeData->offset);
						addMass(newMass);

					// Add each geom to the new body.
					if (0 == (*iter)->transformID)
					{
						// No geom transform.
						dGeomSetBody((*iter)->geomID, mBodyID);
					}
					else
					{
						// Use geom transform.
						dGeomSetBody((*iter)->transformID, mBodyID);
					}
				}

				moveToSpace();
			}
			else
			{
				// do nothing
			}
		}
		else // this object is not static
		{
			if (true == s)
			{
				// make this object static
				mData.isStatic = true;

				// destroy the body
				dBodyDestroy(mBodyID);

				moveToSpace();
			}
			else
			{
				// do nothing
			}
		}
	}

	void ODESolid::setGravityEnabled(bool e)
	{
		mData.gravityEnabled = e;

		if (e)
		{
			dBodySetGravityMode(mBodyID, 1);
		}
		else
		{
			dBodySetGravityMode(mBodyID, 0);
		}
	}

	void ODESolid::moveToSpace()
	{
		// remove all current shapes from their spaces and add them to the
		// new one
		std::vector<GeomData*>::iterator iter;
		for (iter = mGeomDataList.begin(); iter != mGeomDataList.end();
		        ++iter)
		{
			if (0 != (*iter)->transformID)
			{
				// This geom uses a transform, so apply the new space only
				// to the transform geom.
				dSpaceRemove((*iter)->spaceID, (*iter)->transformID);
				dSpaceAdd(mSpaceID, (*iter)->transformID);
			}
			else
			{
				// Normal geom with no transform.
				dSpaceRemove((*iter)->spaceID, (*iter)->geomID);
				dSpaceAdd(mSpaceID, (*iter)->geomID);
			}

			(*iter)->spaceID = mSpaceID;
		}
	}

	void ODESolid::setSleeping(bool sleeping)
	{
		if (mData.isStatic)
		{
			return ;
		}

		// Note: mData.sleeping gets updated in Solid::getData.

		if (sleeping)
		{
			dBodyDisable(mBodyID);
		}
		else
		{
			dBodyEnable(mBodyID);
		}
	}

	bool ODESolid::isSleeping() const
	{
		if (mData.isStatic)
		{
			return true;
		}

		// The ODE body may fall asleep at unknown times, so we need to
		// get the data straight from ODE.
		if (dBodyIsEnabled(mBodyID))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	void ODESolid::setSleepiness(real s)
	{
		Solid::setSleepiness(s);

		/// If this Solid has no ODE body, just return.  The sleepiness
		/// level will still be saved and applied if this Solid ever
		/// becomes dynamic.
		if (mData.isStatic)
		{
			return ;
		}

		if (0 == s)
		{
			// No sleeping at all for the Solid.
			dBodySetAutoDisableFlag(mBodyID, false);
		}
		else
		{
			// Enable sleeping for the Solid..
			dBodySetAutoDisableFlag(mBodyID, true);
		}

		// As value goes from 0.0 to 1.0:
		// AutoDisableLinearThreshold goes from min to max,
		// AutoDisableAngularThreshold goes from min to max,
		// AutoDisableSteps goes from max to min,
		// AutoDisableTime goes from max to min.

		real range = defaults::ode::autoDisableLinearMax -
		             defaults::ode::autoDisableLinearMin;
		dBodySetAutoDisableLinearThreshold(mBodyID, s * range +
		                                    defaults::ode::autoDisableLinearMin);

		range = defaults::ode::autoDisableAngularMax -
		        defaults::ode::autoDisableAngularMin;
		dBodySetAutoDisableAngularThreshold(mBodyID, s * range +
		                                     defaults::ode::autoDisableAngularMin);

		range = (real) (defaults::ode::autoDisableStepsMax -
		                   defaults::ode::autoDisableStepsMin);
		dBodySetAutoDisableSteps(mBodyID,
		                          (int) ((real) defaults::ode::autoDisableStepsMax - s * range));

		range = defaults::ode::autoDisableTimeMax -
		        defaults::ode::autoDisableTimeMin;
		dBodySetAutoDisableTime(mBodyID,
		                         defaults::ode::autoDisableTimeMax - s * range);
	}

/*
	void ODESolid::translateMass(const Vec3r& offset)
	{
		if (!mData.isStatic)
		{
			dMass m;
			dBodyGetMass(mBodyID, &m);
			dMassTranslate(&m, offset[0], offset[1], offset[2]);
			dBodySetMass(mBodyID, &m);

			// Update this since the mass changed.
			mNonSymmetricInertia = isInertiaNonSymmetric(m);
		}
	}
*/

	void ODESolid::addShape(ShapeData& data)
	{
		assert(data.material.density >= 0);

		dGeomID newGeomID = NULL;
		dGeomID newTransformID = NULL;
		dTriMeshDataID newTrimeshDataID = NULL;
		dSpaceID spaceID = NULL;
		dMass newMass;

		if (Matrix44r() == data.offset)
		{
			// No offset transform.
			spaceID = mSpaceID;
			newTransformID = 0;
		}
		else
		{
			// Use ODE's geom transform object.
			spaceID = 0;
			newTransformID = dCreateGeomTransform(mSpaceID);
		}

		// Allocate a new GeomData object.
		GeomData* newGeomData = new GeomData;

		switch (data.getType())
		{
			case BOX_SHAPE:
				{
					BoxShapeData & boxData = (BoxShapeData&) data;
					newGeomID = dCreateBox(spaceID,
					                        (dReal) boxData.dimensions[0],
					                        (dReal) boxData.dimensions[1],
					                        (dReal) boxData.dimensions[2]);
					dMassSetBox(&newMass, (dReal) data.material.density,
					             (dReal) boxData.dimensions[0],
					             (dReal) boxData.dimensions[1],
					             (dReal) boxData.dimensions[2]);
					break;
				}
			case SPHERE_SHAPE:
				{
					SphereShapeData& sphereData = (SphereShapeData&) data;
					newGeomID = dCreateSphere(spaceID,
					                           (dReal) sphereData.radius);
					dMassSetSphere(&newMass, (dReal) data.material.density,
					                (dReal) sphereData.radius);
					break;
				}
			case CAPSULE_SHAPE:
				{
					CapsuleShapeData& capsuleData = (CapsuleShapeData&) data;
					newGeomID = dCreateCCylinder(spaceID,
					                              (dReal) capsuleData.radius, (dReal) capsuleData.length);

					// The "direction" parameter orients the mass along one of the
					// body's local axes; x=1, y=2, z=3.  This axis MUST
					// correspond with the axis along which the capsule is
					// initially aligned, which is the capsule's local Z axis.
					dMassSetCappedCylinder(&newMass, (dReal) data.material.density,
					                        3, (dReal) capsuleData.radius, (dReal) capsuleData.length);
					break;
				}
			case CYLINDER_SHAPE:
				{
					CylinderShapeData& cylinderData = (CylinderShapeData&) data;
					newGeomID = dCreateCylinder(spaceID,
								     (dReal) cylinderData.radius, (dReal) cylinderData.length);

					dMassSetCylinder(&newMass, (dReal) data.material.density,
							  3, (dReal) cylinderData.radius, (dReal) cylinderData.length);

					break;
				}
			case PLANE_SHAPE:
				{
					PlaneShapeData& planeData = (PlaneShapeData&) data;
					if (!mData.isStatic)
					{
						OPAL_LOGGER("warning") << "opal::ODESolid::addPlane: " <<
						"Plane Shape added to a non-static Solid.  " <<
						"The Solid will be made static." << std::endl;

						// ODE planes can't have bodies, so make it static.
						setStatic(true);
					}

					// TODO: make this fail gracefully and print warning: plane
					// offset transform ignored.
					assert(!newTransformID);

					// ODE planes must have their normal vector (abc) normalized.
					Vec3r normal(planeData.abcd[0], planeData.abcd[1],
					              planeData.abcd[2]);
					normal.normalize();

					newGeomID = dCreatePlane(spaceID, (dReal) normal[0],
					                          (dReal) normal[1], (dReal) normal[2],
					                          (dReal) planeData.abcd[3]);

					// Note: ODE planes cannot have mass, but this is already
					// handled since static Solids ignore mass.

					// Solids with planes are the only non-"placeable" Solids.
					mIsPlaceable = false;
					break;
				}
				//case RAY_SHAPE:
				//{
				//	RayShapeData& rayData = (RayShapeData&)data;
				//	newGeomID = dCreateRay(spaceID,
				//		(dReal)rayData.ray.getLength());
				//	Point3r origin = rayData.ray.getOrigin();
				//	Vec3r dir = rayData.ray.getDir();
				//	dGeomRaySet(newGeomID, (dReal)origin[0], (dReal)origin[1],
				//		(dReal)origin[2], (dReal)dir[0], (dReal)dir[1],
				//		(dReal)dir[2]);
				//	// Note: rays don't have mass.
				//	break;
				//}
			case MESH_SHAPE:
				{
					MeshShapeData& meshData = (MeshShapeData&) data;

					// Setup trimesh data pointer.  It is critical that the
					// size of OPAL reals at this point match the size of ODE's
					// dReals.
					newTrimeshDataID = dGeomTriMeshDataCreate();

					// Old way... This is problematic because ODE interprets
					// the vertex array as an array of dVector3s which each
					// have 4 elements.
					//dGeomTriMeshDataBuildSimple(newTrimeshDataID,
					//	(dReal*)meshData.vertexArray, meshData.numVertices,
					//	(int*)meshData.triangleArray, 3 * meshData.numTriangles);

#ifdef dSINGLE
					dGeomTriMeshDataBuildSingle(newTrimeshDataID,
					                             (void*) meshData.vertexArray, 3 * sizeof(real),
					                             meshData.numVertices, (void*) meshData.triangleArray,
					                             3 * meshData.numTriangles, 3 * sizeof(unsigned int));
#else
					dGeomTriMeshDataBuildDouble(newTrimeshDataID,
												 (void*) meshData.vertexArray, 3 * sizeof(real),
												 meshData.numVertices, (void*) meshData.triangleArray,
												 3 * meshData.numTriangles, 3 * sizeof(unsigned int));
#endif

					newGeomID = dCreateTriMesh(spaceID,
					                            newTrimeshDataID, NULL, NULL, NULL);

					dMassSetTrimesh(&newMass, (dReal)data.material.density, 
						newGeomID);

					// ODE requires the mass to be at (0, 0, 0) relative to 
					// the body.
					dMassTranslate(&newMass, -newMass.c[0], -newMass.c[1], 
						-newMass.c[2]);
					break;
				}
			default:
				assert(false);
		}

		// This will do nothing if this is a static Solid.
		//addMass(newMass, data.offset);
		addMass(newMass);

		// Store new Shape.
		mData.addShape(data);

		// Update the Solid's local AABB.  The new shape's local AABB
		// must be transformed using the shape's offset from the Solid.
		real shapeAABB[6] = {0, 0, 0, 0, 0, 0};
		data.getLocalAABB(shapeAABB);
		Point3r minExtents(shapeAABB[0], shapeAABB[2], shapeAABB[4]);
		Point3r maxExtents(shapeAABB[1], shapeAABB[3], shapeAABB[5]);
		minExtents = data.offset * minExtents;
		maxExtents = data.offset * maxExtents;
		shapeAABB[0] = minExtents[0];
		shapeAABB[1] = maxExtents[0];
		shapeAABB[2] = minExtents[1];
		shapeAABB[3] = maxExtents[1];
		shapeAABB[4] = minExtents[2];
		shapeAABB[5] = maxExtents[2];
		addToLocalAABB(shapeAABB);

		// Setup GeomData.
		newGeomData->solid = this;
		newGeomData->shape = mData.getShapeData(mData.getNumShapes() - 1);
		newGeomData->geomID = newGeomID;
		newGeomData->transformID = newTransformID;
		newGeomData->spaceID = mSpaceID;
		newGeomData->trimeshDataID = newTrimeshDataID;

		// Setup the geom.
		setupNewGeom(newGeomData);
	}

	void ODESolid::setLocalLinearVel(const Vec3r& vel)
	{
		if (!mData.isStatic)
		{
			dVector3 worldVel;
			dBodyVectorToWorld(mBodyID, vel[0], vel[1], vel[2], worldVel);
			dBodySetLinearVel(mBodyID, worldVel[0], worldVel[1], worldVel[2]);

			// Invalidate the "freely-spinning" parameter.
			internal_setFreelySpinning(false);
		}
	}

	Vec3r ODESolid::getLocalLinearVel() const
	{
		if (!mData.isStatic)
		{
			const dReal * vel = dBodyGetLinearVel(mBodyID);

			// Convert the vector from global to local coordinates.
			dVector3 localVel;
			dBodyVectorFromWorld(mBodyID, vel[0], vel[1], vel[2], localVel);
			return toVec3r(localVel);
		}
		else
		{
			return Vec3r(0, 0, 0);
		}
	}

	Vec3r ODESolid::getLocalLinearVelAtLocalPos(const Point3r& p) const
	{
		if (!mData.isStatic)
		{
			// First find the global velocity at the given local point.
			dVector3 result;
			dBodyGetRelPointVel(mBodyID, p[0], p[1], p[2], result);

			// Now convert the velocity from global to local coordinates.
			dBodyVectorFromWorld(mBodyID, result[0], result[1], result[2],
			                      result);
			return toVec3r(result);
		}
		else
		{
			return Vec3r(0, 0, 0);
		}
	}

	Vec3r ODESolid::getLocalLinearVelAtGlobalPos(const Point3r& p) const
	{
		if (!mData.isStatic)
		{
			// First find the global velocity at the given global point.
			dVector3 result;
			dBodyGetPointVel(mBodyID, p[0], p[1], p[2], result);

			// Now convert the velocity from global to local coordinates.
			dBodyVectorFromWorld(mBodyID, result[0], result[1], result[2],
			                      result);
			return toVec3r(result);
		}
		else
		{
			return Vec3r(0, 0, 0);
		}
	}

	void ODESolid::setLocalAngularVel(const Vec3r& vel)
	{
		if (!mData.isStatic)
		{
			dVector3 worldVel;
			dBodyVectorToWorld(mBodyID, vel[0], vel[1], vel[2], worldVel);
			Vec3r velRad = toVec3r_DegToRad(worldVel);
			dBodySetAngularVel(mBodyID, velRad[0], velRad[1], velRad[2]);

			// Invalidate the "freely-spinning" parameter.
			internal_setFreelySpinning(false);
		}
	}

	Vec3r ODESolid::getLocalAngularVel() const
	{
		if (!mData.isStatic)
		{
			const dReal * vel = dBodyGetAngularVel(mBodyID);

			// Convert the vector from global to local coordinates.
			dVector3 localVel;
			dBodyVectorFromWorld(mBodyID, vel[0], vel[1], vel[2], localVel);
			return toVec3r_RadToDeg(localVel);
		}
		else
		{
			return Vec3r(0, 0, 0);
		}
	}

	void ODESolid::setGlobalLinearVel(const Vec3r& vel)
	{
		if (!mData.isStatic)
		{
			dBodySetLinearVel(mBodyID, vel[0], vel[1], vel[2]);

			// Invalidate the "freely-spinning" parameter.
			internal_setFreelySpinning(false);
		}
	}

	Vec3r ODESolid::getGlobalLinearVel() const
	{
		if (!mData.isStatic)
		{
			const dReal * vel = dBodyGetLinearVel(mBodyID);
			return toVec3r(vel);
		}
		else
		{
			return Vec3r(0, 0, 0);
		}
	}

	Vec3r ODESolid::getGlobalLinearVelAtLocalPos(const Point3r& p) const
	{
		if (!mData.isStatic)
		{
			// Find the global velocity at the given local point.
			dVector3 result;
			dBodyGetRelPointVel(mBodyID, p[0], p[1], p[2], result);
			return toVec3r(result);
		}
		else
		{
			return Vec3r(0, 0, 0);
		}
	}

	Vec3r ODESolid::getGlobalLinearVelAtGlobalPos(const Point3r& p) const
	{
		if (!mData.isStatic)
		{
			// Find the global velocity at the given global point.
			dVector3 result;
			dBodyGetPointVel(mBodyID, p[0], p[1], p[2], result);
			return toVec3r(result);
		}
		else
		{
			return Vec3r(0, 0, 0);
		}
	}

	void ODESolid::setGlobalAngularVel(const Vec3r& vel)
	{
		if (!mData.isStatic)
		{
			Vec3r velRad(degToRad(vel[0]), degToRad(vel[1]), degToRad(vel[2]));
			dBodySetAngularVel(mBodyID, velRad[0], velRad[1], velRad[2]);

			// Invalidate the "freely-spinning" parameter.
			internal_setFreelySpinning(false);
		}
	}

	Vec3r ODESolid::getGlobalAngularVel() const
	{
		if (!mData.isStatic)
		{
			const dReal * vel = dBodyGetAngularVel(mBodyID);
			return toVec3r_RadToDeg(vel);
		}
		else
		{
			return Vec3r(0, 0, 0);
		}
	}

	dBodyID ODESolid::internal_getBodyID() const
	{
		return mBodyID;
	}

	void ODESolid::internal_setCollisionCount(long int count)
	{
		mCollisionCount = count;
	}

	long int ODESolid::internal_getCollisionCount() const
	{
		return mCollisionCount;
	}

	//void ODESolid::setFastRotation(bool fast)
	//{
	//	if (mStatic)
	//	{
	//		return;
	//	}
	//
	//	if (fast)
	//	{
	//		//if used, call dBodySetFiniteRotationAxis every step; see ODE docs
	//		dBodySetFiniteRotationMode(mBodyID, 1);
	//	}
	//	else
	//	{
	//		dBodySetFiniteRotationMode(mBodyID, 0);
	//	}
	//}

	//bool ODESolid::getFastRotation()const
	//{
	//	if (mStatic)
	//	{
	//		return false;
	//	}
	//
	//	if(1 == dBodyGetFiniteRotationMode(mBodyID))
	//	{
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	//void ODESolid::setFastRotationAxis(Vec3r axis)
	//{
	//	dBodySetFiniteRotationAxis(mBodyID, axis[0], axis[1], axis[2]);
	//}

	void ODESolid::zeroForces()
	{
		if (!mData.isStatic)
		{
			dBodySetForce(mBodyID, 0, 0, 0);
			dBodySetTorque(mBodyID, 0, 0, 0);

			while (!mForceList.empty())
			{
				mForceList.pop_back();
			}
		}
	}

	real ODESolid::getMass() const
	{
		if (mData.isStatic)
		{
			return 0;
		}
		else
		{
			dMass mass;
			dBodyGetMass(mBodyID, &mass);
			return (real) mass.mass;
		}
	}

	Matrix44r ODESolid::getInertiaTensor() const
	{
		Matrix44r m;

		if (mData.isStatic)
		{
			return m;
		}
		else
		{
			dMass mass;
			dBodyGetMass(mBodyID, &mass);
			m.set(
			    (real) mass.I[0], (real) mass.I[1], (real) mass.I[2], 0,
			    (real) mass.I[4], (real) mass.I[5], (real) mass.I[6], 0,
			    (real) mass.I[8], (real) mass.I[9], (real) mass.I[10], 0,
			    0, 0, 0, 1);

			return m;
		}
	}

	void ODESolid::setupNewGeom(GeomData* newGeom)
	{
		if (!mData.isStatic)
		{
			if (0 == newGeom->transformID)
			{
				// No geom transform.
				dGeomSetBody(newGeom->geomID, mBodyID);
			}
			else
			{
				// Use geom transform.
				dGeomSetBody(newGeom->transformID, mBodyID);
			}
		}

		if (0 != newGeom->transformID)
		{
			// Setup geom transform.
			dGeomTransformSetGeom(newGeom->transformID, newGeom->geomID);
			dMatrix3 R =
			    {newGeom->shape->offset[0], newGeom->shape->offset[4],
			     newGeom->shape->offset[8], 0,
			     newGeom->shape->offset[1], newGeom->shape->offset[5],
			     newGeom->shape->offset[9], 0,
			     newGeom->shape->offset[2], newGeom->shape->offset[6],
			     newGeom->shape->offset[10], 0};
			dGeomSetRotation(newGeom->geomID, R);
			dGeomSetPosition(newGeom->geomID, newGeom->shape->offset[12],
			                  newGeom->shape->offset[13],
			                  newGeom->shape->offset[14]);
		}

		// Set the GeomData reference for later use (e.g. in collision
		// handling).
		if (0 == newGeom->transformID)
		{
			// No geom transform.
			dGeomSetData(newGeom->geomID, newGeom);
		}
		else
		{
			// Using geom transform.
			dGeomSetData(newGeom->transformID, newGeom);
		}

		// Store the GeomData pointer.
		mGeomDataList.push_back(newGeom);

		// Make sure the initial transform is setup; this needs to come after
		// the geom data has been added.
		if (mData.isStatic && mIsPlaceable)
		{
			setTransform(mData.transform);
		}
	}

	void ODESolid::applyForce(const Force& f)
	{
		switch (f.type)
		{
			case LOCAL_FORCE:
				dBodyAddRelForce(mBodyID, f.vec[0], f.vec[1], f.vec[2]);
				break;
			case GLOBAL_FORCE:
				dBodyAddForce(mBodyID, f.vec[0], f.vec[1], f.vec[2]);
				break;
			case LOCAL_TORQUE:
				dBodyAddRelTorque(mBodyID, f.vec[0], f.vec[1], f.vec[2]);
				break;
			case GLOBAL_TORQUE:
				dBodyAddTorque(mBodyID, f.vec[0], f.vec[1], f.vec[2]);
				break;
			case LOCAL_FORCE_AT_LOCAL_POS:
				dBodyAddRelForceAtRelPos(mBodyID, f.vec[0], f.vec[1],
				                          f.vec[2], f.pos[0], f.pos[1], f.pos[2]);
				break;
			case LOCAL_FORCE_AT_GLOBAL_POS:
				dBodyAddRelForceAtPos(mBodyID, f.vec[0], f.vec[1], f.vec[2],
				                       f.pos[0], f.pos[1], f.pos[2]);
				break;
			case GLOBAL_FORCE_AT_LOCAL_POS:
				dBodyAddForceAtRelPos(mBodyID, f.vec[0], f.vec[1], f.vec[2],
				                       f.pos[0], f.pos[1], f.pos[2]);
				break;
			case GLOBAL_FORCE_AT_GLOBAL_POS:
				dBodyAddForceAtPos(mBodyID, f.vec[0], f.vec[1], f.vec[2],
				                    f.pos[0], f.pos[1], f.pos[2]);
				break;
		}

		// Invalidate the "freely-spinning" parameter.
		internal_setFreelySpinning(false);
	}

	//void ODESolid::setMass(const Mass & newmass, const Matrix44r & offset)
	void ODESolid::setMass(const Mass & newmass)
	{
		if (mData.isStatic)
		{
			return ;
		}

		// first create mass object
		if (!areEqual(newmass.center.x, 0) || 
			!areEqual(newmass.center.y, 0) || 
			!areEqual(newmass.center.z, 0))
		{
			OPAL_LOGGER("warning") << "Solid mass must have center at (0, 0, 0)" 
				<< std::endl;
		}

		dMass newMass;
		newMass.setParameters(newmass.mass, 0, 0, 0, 
			newmass.inertia[0], newmass.inertia[5], newmass.inertia[10], 
			newmass.inertia[1], newmass.inertia[2], newmass.inertia[6]);

/*
		// First rotate and translate the new mass.
		dMatrix3 R = {offset[0], offset[4], offset[8], 0,
		              offset[1], offset[5], offset[9], 0,
		              offset[2], offset[6], offset[10], 0};

		dMassRotate(&newMass, R);
		dMassTranslate(&newMass, offset[12], offset[13], offset[14]);
*/

		dBodySetMass(mBodyID, &newMass);

		// Update this since the mass changed.
		dMass m;
		dBodyGetMass(mBodyID, &m);
		mNonSymmetricInertia = isInertiaNonSymmetric(m);
	}

	//void ODESolid::addMass(dMass& newMass, const Matrix44r& offset)
	void ODESolid::addMass(dMass& newMass)
	{
		if (mData.isStatic)
		{
			return ;
		}

/*
		// First rotate and translate the new mass.
		dMatrix3 R = {offset[0], offset[4], offset[8], 0,
		              offset[1], offset[5], offset[9], 0,
		              offset[2], offset[6], offset[10], 0};

		dMassRotate(&newMass, R);
		dMassTranslate(&newMass, offset[12], offset[13], offset[14]);
*/

		// If this mass is for the first Shape, just set the Solid's mass
		// equal to the new one.  ODE bodies start with an initial mass
		// already setup, but we want to ignore that, not add to it.
		if (0 == mGeomDataList.size())
		{
			dBodySetMass(mBodyID, &newMass);
		}
		else
		{
			// Add new mass to the Solid's existing mass.  First get the
			// existing mass struct from ODE.
			dMass totalMass;
			dBodyGetMass(mBodyID, &totalMass);
			dMassAdd(&totalMass, &newMass);
			dBodySetMass(mBodyID, &totalMass);
		}

		// Update this since the mass changed.
		dMass m;
		dBodyGetMass(mBodyID, &m);
		mNonSymmetricInertia = isInertiaNonSymmetric(m);
	}

	//void ODESolid::setMass(dMass& newMass, const Matrix44r& offset)
	void ODESolid::setMass(dMass& newMass)
	{
		if (mData.isStatic)
		{
			return ;
		}

/*
		// First rotate and translate the new mass.
		dMatrix3 R = {offset[0], offset[4], offset[8], 0,
		              offset[1], offset[5], offset[9], 0,
		              offset[2], offset[6], offset[10], 0};

		dMassRotate(&newMass, R);
		dMassTranslate(&newMass, offset[12], offset[13], offset[14]);
*/

		dBodySetMass(mBodyID, &newMass);

		// Update this since the mass changed.
		dMass m;
		dBodyGetMass(mBodyID, &m);
		mNonSymmetricInertia = isInertiaNonSymmetric(m);
	}

	const std::vector<GeomData*>* ODESolid::internal_getGeomDataList() const
	{
		return & mGeomDataList;
	}

	void ODESolid::internal_doAngularVelFix()
	{
		if (mNonSymmetricInertia)
		{
			Vec3r vel = getGlobalAngularVel();
			real currentAngVelMagSquared = vel.lengthSquared();

			if (mIsFreelySpinning)
			{
				// If the current angular velocity magnitude is greater than
				// that of the previous step, scale it by that of the previous
				// step; otherwise, update the previous value to that of the
				// current step.  This ensures that angular velocity never
				// increases for freely-spinning objects.

				if (currentAngVelMagSquared > mPrevAngVelMagSquared)
				{
					real currentAngVelMag = sqrt(currentAngVelMagSquared);
					vel = vel / currentAngVelMag;
					// Vel is now a unit vector.  Next, scale this vector
					// by the previous angular velocity magnitude.
					real prevAngVelMag = sqrt(mPrevAngVelMagSquared);
					setGlobalAngularVel(vel * prevAngVelMag);
				}
			}

			mPrevAngVelMagSquared = currentAngVelMagSquared;
		}

		// Reset the "freely-spinning" parameter for the next time step.
		internal_setFreelySpinning(true);
	}

	void ODESolid::internal_setFreelySpinning(bool fs)
	{
		mIsFreelySpinning = fs;
	}

	bool ODESolid::isInertiaNonSymmetric(const dMass& mass) const
	{
		if (!areEqual((real) mass.I[0], (real) mass.I[5])
		        || !areEqual((real) mass.I[5], (real) mass.I[10]))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
