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

// project headers
#include "ODESimulator.h"
#include "ShapeData.h"

// Check for invalid floating point precision setup between OPAL and ODE.
#if (defined(OPAL_USE_DOUBLE) && defined(dSINGLE)) || (!defined(OPAL_USE_DOUBLE) && defined(dDOUBLE))
	#error Invalid float precision between OPAL and ODE.
#endif

// Ensure that ODE trimesh support is enabled.
#if !defined(dTRIMESH_ENABLED)
	#error ODE trimesh support must be enabled.
#endif

namespace opal
{
	OPAL_EXPORT_FUNCTION opal::Simulator* OPAL_CALL createSimulator()
	{
		// Check to make to ODE's reals are the same size as OPAL's reals.
		if (sizeof(real) != sizeof(dReal))
		{
			OPAL_LOGGER("warning") << "The size of data types (OPAL) real "
			<< "and (ODE) dReal do not match.  Make sure these "
			<< "libraries are built using the same floating point "
			<< "precision (float or double)." << std::endl;
		}

		opal::ODESimulator * sim = new opal::ODESimulator();
		sim->initData(opal::SimulatorData());
		return sim;
	}

	OPAL_EXPORT_FUNCTION opal::Simulator* OPAL_CALL createCustomSimulator(opal::SimulatorData & data)
	{
		// Check to make to ODE's reals are the same size as OPAL's reals.
		if (sizeof(real) != sizeof(dReal))
		{
			OPAL_LOGGER("warning") << "The size of data types (OPAL) real "
			<< "and (ODE) dReal do not match.  Make sure these "
			<< "libraries are built using the same floating point "
			<< "precision (float or double)." << std::endl;
		}

		opal::ODESimulator * sim = new opal::ODESimulator();
		sim->initData(data);
		return sim;
	}

	// Initialize this static member variable.
	unsigned int ODESimulator::mInitCounter = 0;

	void ODESimulator::initData(SimulatorData data)
	{
		Simulator::initData(data);

		// We can only init ODE once.
		if (0 == mInitCounter)
		{
			dInitODE();
		}
		++mInitCounter;

		// Create ODE world.
		mWorldID = dWorldCreate();

		// Set default gravity.
		setGravity(defaults::gravity);

		// Create the root ODE space.
		//mRootSpaceID = dSimpleSpaceCreate(0);
		//dVector3 center = {0, 0, 0};
		//dVector3 extents = {200, 100, 200};
		//mRootSpaceID = dQuadTreeSpaceCreate(0, center, extents, 5);

		if (data.useOctreeInsteadHash)
		{
			dVector3 center = { data.worldCenter[0],
			                    data.worldCenter[1],
			                    data.worldCenter[2] };
			dVector3 extents = { data.worldSize[0],
			                     data.worldSize[1],
			                     data.worldSize[2] };
			mRootSpaceID = dQuadTreeSpaceCreate(0, center, extents, data.octreeDepth);
		}
		else
		{
			mRootSpaceID = dHashSpaceCreate(0);
			dHashSpaceSetLevels(mRootSpaceID, data.hashMinLevel, data.hashMaxLevel);
		}

		mRootSpace = new ODESpace(mRootSpaceID);

		// Create the ODE contact joint group.
		mContactJointGroupID = dJointGroupCreate(0);

		// Set the ODE global CFM value that will be used by all Joints
		// (including contacts).  This affects normal Joint constraint
		// operation and Joint limits.  The user cannot adjust CFM, but
		// they can adjust ERP (a.k.a. bounciness/restitution) for materials
		// (i.e. contact settings) and Joint limits.
		dWorldSetCFM(mWorldID, defaults::ode::globalCFM);

		// Set the ODE global ERP value.  This will only be used for Joints
		// under normal conditions, not at their limits.  Also, it will not
		// affect contacts at all since they use material properties to
		// calculate ERP.
		dWorldSetERP(mWorldID, (real) 0.5 * (defaults::ode::maxERP +
		              defaults::ode::minERP));

		dWorldSetContactSurfaceLayer(mWorldID, defaults::ode::surfaceLayer);

		setSolverAccuracy(defaults::solverAccuracy);
		mCollisionCount = 0;
		// "mRaycastResult" is initialized in its own constructor.
		mSensorSolid = NULL;
		mRayContactGroup = defaults::shape::contactGroup;
	}

	ODESimulator::ODESimulator()
	{
		// empty
	}

	ODESimulator::~ODESimulator()
	{
		// empty
	}

	void ODESimulator::setMaxCorrectingVel(real vel)
	{
		Simulator::setMaxCorrectingVel(vel);
		dWorldSetContactMaxCorrectingVel(mWorldID, vel);
	}

	Solid* ODESimulator::createSolid()
	{
		Solid * newSolid = new ODESolid(mWorldID, mRootSpaceID);
		addSolid(newSolid);
		return newSolid;
	}

	Space* ODESimulator::createSpace()
	{
		ODESpace * newSpace = new ODESpace();

		// Add this new Space as a child of the Simulator's root Space.
		dSpaceAdd(mRootSpaceID, (dGeomID) newSpace->internal_getSpaceID());

		addSpace(newSpace);
		return newSpace;
	}

	Joint* ODESimulator::createJoint()
	{
		Joint * newJoint = new ODEJoint(mWorldID);
		addJoint(newJoint);
		return newJoint;
	}

	void ODESimulator::destroy()
	{
		// These temporary copies are necessary because the
		// ODESimulator::~ODESimulator call (due to "delete this") will
		// invalidate the data members.
		dSpaceID rootSpaceID = mRootSpaceID;
		dWorldID worldID = mWorldID;
		dJointGroupID contactJointGroupID = mContactJointGroupID;

		delete this;

		// The following must occur after Simulator::~Simulator() is called;
		// otherwise, Simulator::~Simulator() will try to destroy Solids after
		// ODE has closed.
		dSpaceDestroy(rootSpaceID);
		dWorldDestroy(worldID);
		dJointGroupDestroy(contactJointGroupID);

		// We can only close ODE once.
		--mInitCounter;
		if (0 == mInitCounter)
		{
			dCloseODE();
		}
	}

	void ODESimulator::stepPhysics()
	{
		// Apply linear and angular damping; if using the "add opposing
		// forces" method, be sure to do this before calling ODE step
		// function.
		std::vector<Solid*>::iterator iter;
		for (iter = mSolidList.begin(); iter != mSolidList.end(); ++iter)
		{
			ODESolid* solid = (ODESolid*) (*iter);

			if (!solid->isStatic())
			{
				if (solid->isSleeping())
				{
					// Reset velocities, force, & torques of objects that go
					// to sleep; ideally, this should happen in the ODE
					// source only once - when the object initially goes to
					// sleep.

					dBodyID bodyID = ((ODESolid*) solid)->internal_getBodyID();
					dBodySetLinearVel(bodyID, 0, 0, 0);
					dBodySetAngularVel(bodyID, 0, 0, 0);
					dBodySetForce(bodyID, 0, 0, 0);
					dBodySetTorque(bodyID, 0, 0, 0);
				}
				else
				{
					// Dampen Solid motion.  3 possible methods:
					// 1) apply a force/torque in the opposite direction of
					// motion scaled by the body's velocity
					// 2) same as 1, but scale the opposing force by
					// the body's momentum (this automatically handles
					// different mass values)
					// 3) reduce the body's linear and angular velocity by
					// scaling them by 1 - damping * stepsize

					dBodyID bodyID = solid->internal_getBodyID();
					dMass mass;
					dBodyGetMass(bodyID, &mass);

					// Method 1
					//const dReal* l = dBodyGetLinearVel(bodyID);
					//dReal damping = -solid->getLinearDamping();
					//dBodyAddForce(bodyID, damping*l[0], damping*l[1], damping*l[2]);
					//const dReal* a = dBodyGetAngularVel(bodyID);
					//damping = -solid->getAngularDamping();
					//dBodyAddTorque(bodyID, damping*a[0], damping*a[1], damping*a[2]);

					// Method 2
					// Since the ODE mass.I inertia matrix is local, angular
					// velocity and torque also need to be local.

					// Linear damping
					real linearDamping = solid->getLinearDamping();
					if (0 != linearDamping)
					{
						const dReal * lVelGlobal = dBodyGetLinearVel(bodyID);

						// The damping force depends on the damping amount,
						// mass, and velocity (i.e. damping amount and
						// momentum).
						dReal dampingFactor = -linearDamping * mass.mass;
						dVector3 dampingForce = {
						                            dampingFactor * lVelGlobal[0],
						                            dampingFactor * lVelGlobal[1],
						                            dampingFactor * lVelGlobal[2] };

						// Add a global force opposite to the global linear
						// velocity.
						dBodyAddForce(bodyID, dampingForce[0],
						               dampingForce[1], dampingForce[2]);
					}

					// Angular damping
					real angularDamping = solid->getAngularDamping();
					if (0 != angularDamping)
					{
						const dReal * aVelGlobal = dBodyGetAngularVel(bodyID);
						dVector3 aVelLocal;
						dBodyVectorFromWorld(bodyID, aVelGlobal[0],
						                      aVelGlobal[1], aVelGlobal[2], aVelLocal);

						// The damping force depends on the damping amount,
						// mass, and velocity (i.e. damping amount and
						// momentum).
						//dReal dampingFactor = -angularDamping * mass.mass;
						dReal dampingFactor = -angularDamping;
						dVector3 aMomentum;

						// Make adjustments for inertia tensor.
						dMULTIPLYOP0_331(aMomentum, = , mass.I, aVelLocal);
						dVector3 dampingTorque = {
						                             dampingFactor * aMomentum[0],
						                             dampingFactor * aMomentum[1],
						                             dampingFactor * aMomentum[2] };

						// Add a local torque opposite to the local angular
						// velocity.
						dBodyAddRelTorque(bodyID, dampingTorque[0],
						                   dampingTorque[1], dampingTorque[2]);
					}

					//dMass mass;
					//dBodyGetMass(bodyID, &mass);
					//const dReal* l = dBodyGetLinearVel(bodyID);
					//dReal damping = -solid->getLinearDamping() * mass.mass;
					//dBodyAddForce(bodyID, damping*l[0], damping*l[1], damping*l[2]);
					//const dReal* aVelLocal = dBodyGetAngularVel(bodyID);
					////dVector3 aVelLocal;
					////dBodyVectorFromWorld(bodyID, aVelGlobal[0], aVelGlobal[1], aVelGlobal[2], aVelLocal);
					//damping = -solid->getAngularDamping();
					//dVector3 aMomentum;
					//dMULTIPLYOP0_331(aMomentum, =, aVelLocal, mass.I);
					//dBodyAddTorque(bodyID, damping*aMomentum[0], damping*aMomentum[1],
					//  damping*aMomentum[2]);

					// Method 3
					//const dReal* l = dBodyGetLinearVel(bodyID);
					//dReal damping = (real)1.0 - solid->getLinearDamping() * mStepSize;
					//dBodySetLinearVel(bodyID, damping*l[0], damping*l[1], damping*l[2]);
					//const dReal* a = dBodyGetAngularVel(bodyID);
					//damping = (real)1.0 - solid->getAngularDamping() * mStepSize;
					//dBodySetAngularVel(bodyID, damping*a[0], damping*a[1], damping*a[2]);
				}
			}
		}

		// Do collision detection; add contacts to the contact joint group.
		dSpaceCollide(mRootSpaceID, this,
		               &ode_hidden::internal_collisionCallback);

		// Take a simulation step.
		if (SOLVER_QUICKSTEP == mSolverType)
		{
			dWorldQuickStep(mWorldID, mStepSize);
		}
		else
		{
			dWorldStep(mWorldID, mStepSize);
		}

		// Remove all joints from the contact group.
		dJointGroupEmpty(mContactJointGroupID);

		// Fix angular velocities for freely-spinning bodies that have
		// gained angular velocity through explicit integrator inaccuracy.
		for (iter = mSolidList.begin(); iter != mSolidList.end(); ++iter)
		{
			ODESolid* s = (ODESolid*) (*iter);
			if (!s->isSleeping() && !s->isStatic())
			{
				s->internal_doAngularVelFix();
			}
		}
	}

	namespace ode_hidden
	{
		void internal_collisionCallback(void *data, dGeomID o0, dGeomID o1)
		{
			if (dGeomIsSpace(o0) || dGeomIsSpace(o1))
			{
				// Colliding a space with either a geom or another space.
				dSpaceCollide2(o0, o1, data, &internal_collisionCallback);

				if (dGeomIsSpace(o0))
				{
					// Colliding all geoms internal to the space.
					dSpaceCollide((dSpaceID) o0, data,
					               &internal_collisionCallback);
				}

				if (dGeomIsSpace(o1))
				{
					// Colliding all geoms internal to the space.
					dSpaceCollide((dSpaceID) o1, data,
					               &internal_collisionCallback);
				}
			}
			else
			{
				// Colliding two geoms.

				// The following is a set of special cases where we might
				// want to skip collision detection (but not all are
				// enforced here for various reasons):
				// 1. Two static Solids (neither geom has a body) AND
				//    neither Solid has a CollisionEventHandler AND there are
				//    not global handlers: this is enforced.
				// 2. Two Shapes that are part of the same Solid (they
				//    share a body): this is not enforced because ODE
				//    already takes care of it.
				// 3. Two sleeping Solids (note: both must have bodies to
				//    check this): this is enforced.  (ODE should handle
				//    this, but it doesn't.)
				// 4. Two Solids connected by a fixed Joint: this is
				//    enforced.
				// 5. Two Solids connected by a Joint (besides ODE
				//    contact joints, which should never generate more
				//    contacts) with contacts disabled (note: both must have
				//    bodies to check this): this is enforced.
				// 6. Solid0 is static, Solid1 is dynamic and is sleeping,
				//    static-to-sleeping contacts are ignored by the
				//    Simulator, and neither Solid has a
				//    CollisionEventHandler AND there are no global handlers:
				//    this is enforced.
				// 7. Solid1 is static, Solid0 is dynamic and is sleeping,
				//    static-to-sleeping contacts are ignored by the
				//    Simulator, and neither Solid has a
				//    CollisionEventHandler AND there are no global handlers:
				//    this is enforced.
				// 8. The two Solids' contact groups do not generate
				//    contacts when they collide AND neither Solid has a
				//    CollisionEventHandler AND there are no global handlers.

				// Get the geoms' ODE body IDs.
				dBodyID o0BodyID = dGeomGetBody(o0);
				dBodyID o1BodyID = dGeomGetBody(o1);
				bool solid0Static = (0 == o0BodyID);
				bool solid1Static = (0 == o1BodyID);

				// Check if both Solids are dynamic (i.e. have ODE bodies).
				bool bothHaveBodies = true;
				if (0 == o0BodyID || 0 == o1BodyID)
				{
					bothHaveBodies = false;
				}

				// If the two Solids are connected by a common Joint, get
				// a pointer to that Joint.
				Joint* commonJoint = NULL;
				if (bothHaveBodies && dAreConnectedExcluding(o0BodyID,
				        o1BodyID, dJointTypeContact))
				{
					// This will become non-NULL only if there exists an ODE
					// joint connecting the two bodies.
					commonJoint = internal_getCommonJoint(o0BodyID, o1BodyID);
				}

				// Get pointers to the geoms' GeomData structures.
				GeomData* geomData0 = (GeomData*) dGeomGetData(o0);
				GeomData* geomData1 = ((GeomData*) dGeomGetData(o1));

				// Get pointers to the geoms' ShapeData structures.
				const ShapeData* shape0 = geomData0->shape;
				const ShapeData* shape1 = geomData1->shape;

				// Get a pointer to the ODESimulator.
				ODESimulator* sim = (ODESimulator*)data;

				// Check if the two Solids' contact groups generate contacts
				// when they collide.
				bool makeContacts = sim->groupsMakeContacts(
					shape0->contactGroup, shape1->contactGroup);

				// Find out whether the Simulator has static-to-sleeping
				// contacts disabled.
				bool ignoreStaticSleepingContacts =
				    !sim->areStaticSleepingContactsEnabled();

				// Get pointers to the geoms' Solids.
				Solid* solid0 = geomData0->solid;
				Solid* solid1 = geomData1->solid;

				// Get pointers to the two Solids' CollisionEventHandlers.
				// These will be NULL if the Solids don't use
				// CollisionEventHandlers.
				CollisionEventHandler* handler0 =
				    solid0->getCollisionEventHandler();
				CollisionEventHandler* handler1 =
				    solid1->getCollisionEventHandler();

				bool neitherHasCollisionHandler = !(handler0 || handler1);

				bool noGlobalCollisionHandlers = 
					sim->getNumGlobalCollisionEventHandlers() == 0;

				// Now do the actual tests to see if we should return early.
				// It is important here that we don't call dBodyIsEnabled on
				// a static body because that crashes ODE.

				bool case1 = neitherHasCollisionHandler && 
					noGlobalCollisionHandlers && solid0Static && solid1Static;
				//bool case2= o0BodyID == o1BodyID;
				bool case3 = bothHaveBodies && !dBodyIsEnabled(o0BodyID)
					&& !dBodyIsEnabled(o1BodyID);
				bool case4 = commonJoint &&
					commonJoint->getType() == FIXED_JOINT;
				bool case5 = commonJoint
					&& !commonJoint->areContactsEnabled();
				bool case6 = solid0Static && 0 != o1BodyID
					&& !dBodyIsEnabled(o1BodyID)
					&& ignoreStaticSleepingContacts
					&& neitherHasCollisionHandler && noGlobalCollisionHandlers;
				bool case7 = solid1Static && 0 != o0BodyID
					&& !dBodyIsEnabled(o0BodyID)
					&& ignoreStaticSleepingContacts
					&& neitherHasCollisionHandler && noGlobalCollisionHandlers;
				bool case8 = !makeContacts && neitherHasCollisionHandler
					&& noGlobalCollisionHandlers;

				if (case1 || case3 || case4 || case5 || case6 || case7 || case8)
				{
					return;
				}

				// Now actually test for collision between the two geoms.
				// This is one of the more expensive operations.
				dWorldID theWorldID = sim->internal_getWorldID();
				dJointGroupID theJointGroupID =
				    sim->internal_getJointGroupID();
				dContactGeom contactArray[globals::maxMaxContacts];
				int numContacts = dCollide(o0, o1, sim->getMaxContacts(),
					contactArray, sizeof(dContactGeom));

				// If the two objects didn't make any contacts, they weren't
				// touching, so just return.
				if (0 == numContacts)
				{
					return ;
				}

				// If at least one of the Solids has a CollisionEventHandler,
				// send it a CollisionEvent.
				if (handler0 || handler1 || !noGlobalCollisionHandlers)
				{
					// Call the CollisionEventHandlers.  Note that we only
					// use one contact point per collision: just the first one
					// in the contact array.  The order of the Solids
					// passed to the event handlers is important: the first
					// one should be the one whose event handler is
					// getting called.

					CollisionEvent e;
					e.thisSolid = solid0;
					e.otherSolid = solid1;
					e.pos[0] = (real) contactArray[0].pos[0];
					e.pos[1] = (real) contactArray[0].pos[1];
					e.pos[2] = (real) contactArray[0].pos[2];
					e.normal[0] = (real) contactArray[0].normal[0];
					e.normal[1] = (real) contactArray[0].normal[1];
					e.normal[2] = (real) contactArray[0].normal[2];
					e.depth = (real) contactArray[0].depth;

					if (handler0)
					{
						handler0->internal_pushCollisionEvent(e);
					}

					if (handler1)
					{
						// For the other Solid's CollisionEventHandler, we need
						// to invert the normal and swap the Solid pointers.
						e.normal *= -1;
						e.thisSolid = solid1;
						e.otherSolid = solid0;
						handler1->internal_pushCollisionEvent(e);
					}

					sim->internal_recordCollision(e);
				}

				// Old version...
				//// Early check to save some time.
				//if (solid0->getCollisionEventHandler()
				//  || solid1->getCollisionEventHandler())
				//{
				//  // Call the event handlers.  Note: we only use one
				//  // contact point per collision; just use the first one
				//  // in the contact array.  The order of the Solids
				//  // passed to the event handlers is important: the first
				//  // one should be the one whose event handler is
				//  // getting called.

				//  CollisionEvent e;
				//  e.solid0 = solid0;
				//  e.solid1 = solid1;
				//  e.pos[0] = contactArray[0].pos[0];
				//  e.pos[1] = contactArray[0].pos[1];
				//  e.pos[2] = contactArray[0].pos[2];
				//  e.normal[0] = contactArray[0].normal[0];
				//  e.normal[1] = contactArray[0].normal[1];
				//  e.normal[2] = contactArray[0].normal[2];
				//  e.depth = contactArray[0].depth;

				//  EventHandler* eventHandler =
				//      solid0->getCollisionEventHandler();
				//  if (eventHandler)
				//  {
				//      generateContacts0 =
				//          eventHandler->handleCollisionEvent(e);
				//  }

				//  e.normal *= -1; // Invert normal.
				//  e.solid0 = solid1; // Swap solid pointers.
				//  e.solid1 = solid0;

				//  eventHandler = solid1->getCollisionEventHandler();
				//  if (eventHandler)
				//  {
				//      generateContacts1 =
				//          eventHandler->handleCollisionEvent(e);
				//  }
				//}

				if (makeContacts)
				{
					// Invalidate the "freely-spinning" parameters.
					((ODESolid*)solid0)->internal_setFreelySpinning(false);
					((ODESolid*)solid1)->internal_setFreelySpinning(false);

					for (int i = 0; i < numContacts; ++i)
					{
						const Material* m0 = &(shape0->material);
						const Material* m1 = &(shape1->material);

						dContact tempContact;
						tempContact.surface.mode = dContactBounce |
							dContactSoftERP; // | dContactSoftCFM;

						// Average the hardness of the two materials.
						assert(m0->hardness >= 0 && m0->hardness <= 1
						        && m1->hardness >= 0 && m1->hardness <= 1);
						real hardness = (m0->hardness + m1->hardness) *
						                (real) 0.5;

						// Convert hardness to ERP.  As hardness goes from
						// 0.0 to 1.0, ERP goes from min to max.
						tempContact.surface.soft_erp = hardness *
							(defaults::ode::maxERP - defaults::ode::minERP) +
							defaults::ode::minERP;

						// Don't use contact CFM anymore.  Just let it use
						// the global value set in the ODE world.
						//tempContact.surface.soft_cfm =
						//  defaults::ode::minCFM;

						// As friction goes from 0.0 to 1.0, mu goes from 0.0
						// to max, though it is set to dInfinity when
						// friction == 1.0.
						assert(m0->friction >= 0 && m0->friction <= 1
						        && m1->friction >= 0 && m1->friction <= 1);
						if (1.0 == m0->friction && 1.0 == m1->friction)
						{
							tempContact.surface.mu = dInfinity;
						}
						else
						{
							tempContact.surface.mu =
							    sqrt(m0->friction * m1->friction) *
							    defaults::ode::maxFriction;
						}

						// Average the bounciness of the two materials.
						assert(m0->bounciness >= 0 && m0->bounciness <= 1
							&& m1->bounciness >= 0 && m1->bounciness <= 1);
						real bounciness = (m0->bounciness + m1->bounciness) *
							(real) 0.5;

						// ODE's bounce parameter, a.k.a. restitution.
						tempContact.surface.bounce = bounciness;

						// ODE's bounce_vel parameter is a threshold:
						// the relative velocity of the two objects must be
						// greater than this for bouncing to occur at all.
						tempContact.surface.bounce_vel =
						    defaults::bounceThreshold;

						// Old way to calculate bounce threshold; threshold
						// was scaled by the collision bounciness, but this
						// makes all objects with non-zero bounciness
						// always bounce.  This is undesirable because it
						// takes a while for bouncing to totally diminish.
						//tempContact.surface.bounce_vel =
						//  defaults::ode::maxBounceVel - bounciness *
						//  defaults::ode::maxBounceVel;

						tempContact.geom = contactArray[i];
						dJointID contactJoint = dJointCreateContact(
							theWorldID, theJointGroupID, &tempContact);

						// Note: the following line of code replaces the
						// rest of this function which is commented out.
						// TODO: test this and make sure the mass ratio
						// issues are unimportant and that we never need
						// "one-sided" contacts between two Solids.
						dJointAttach(contactJoint, o0BodyID, o1BodyID);

						//if (!bothHaveBodies)
						//{
						//  // at least one object is static, so just handle
						//  // things like normal
						//  dJointAttach(contactJoint, o0BodyID, o1BodyID);
						//}
						//else
						//{
						//  // TODO: We probably need to remove the following chunk of
						//  // code.  The first case is obsolete since now both sides
						//  // always get contacts, if at all.  (There isn't really a
						//  // good reason to have one side use contacts but not the
						//  // other; the side not wanting contacts would appear to be
						//  // static, so just make it static in the first place.  On
						//  // the other hand, this may end up being desirable if
						//  // an object should be moved by some objects but not
						//  // others, and the "others" *do* collid with the first
						//  // object... but this situation may never come up.  The
						//  // second case, using mass ratios to determine whether two
						//  // objects should collide, might not be an issue.  Mass
						//  // ratios might only be a problem when the two objects are
						//  // constantly connected with a Joint.

						//  // Handle one-sided contacts for two cases: 1) only one of
						//  // the above event handlers actually wants contacts generated,
						//  // 2) if the mass ratio is above some threshold, treat it as
						//  // a one-sided collision solution: treat the more massive
						//  // object as static, calculate the collision like normal
						//  // (with the massive one being static), then also add the
						//  // massive object's velocity to the smaller one (velocity
						//  // calculated at the point of collision).

						//  // calculate mass ratio (use mass1 / mass2); if
						//  // the ratio is too high, mass1 is too large; if
						//  // the ratio is too low, mass2 is too large
						//  real massRatio = 0;
						//  dMass mass0, mass1;
						//  dBodyGetMass(o0BodyID, &mass0);
						//  dBodyGetMass(o1BodyID, &mass1);
						//  massRatio = mass0.mass / mass1.mass;

						//  // here we handle all the different collision
						//  // cases: one solid or the other or both may want
						//  // contacts generated; also, the mass ratio may
						//  // be outside the acceptable range

						//  if (true == generateContacts0 && true ==
						//      generateContacts1)
						//  {
						//      // both want contacts, neither wants to be
						//      // static
						//      if (massRatio > defaults::ode::maxMassRatio)
						//      {
						//          // ratio is too high - mass0 is too large,
						//          // treat solid0 as static
						//          dBodyEnable(o1BodyID);
						//          dJointAttach(contactJoint, 0, o1BodyID);
						//      }
						//      else if (massRatio <
						//          defaults::ode::minMassRatio)
						//      {
						//          // ratio is too low - mass1 is too large,
						//          // treat solid1 as static
						//          dBodyEnable(o0BodyID);
						//          dJointAttach(contactJoint, o0BodyID, 0);
						//      }
						//      else
						//      {
						//          //ratio is good - no static objects
						//          dJointAttach(contactJoint, o0BodyID,
						//              o1BodyID);
						//      }
						//  }
						//  else if (true == generateContacts0)
						//  {
						//      // solid0 wants contacts, solid1 wants to be
						//      // static
						//      if (massRatio > defaults::ode::maxMassRatio)
						//      {
						//          // ratio is too high - mass0 is too large,
						//          // treat solid0 and solid1 as static
						//          // i.e. don't generate a contact joint
						//      }
						//      else
						//      {
						//          // this block handles two cases which have
						//          // the same result:
						//          // 1. ratio is too low - mass1 is too
						//          //  large, treat solid1 as static
						//          // 2. ratio is good - treat solid1 as
						//          //  static
						//          dBodyEnable(o0BodyID);
						//          dJointAttach(contactJoint, o0BodyID, 0);
						//      }
						//  }
						//  else //generateContacts1 must be true
						//  {
						//      // solid1 wants contacts, solid0 wants to be
						//      // static
						//      if (massRatio < defaults::ode::minMassRatio)
						//      {
						//          // ratio is too low - mass1 is too large,
						//          // treat solid0 and solid1 as static
						//          // i.e. don't generate a contact joint
						//      }
						//      else
						//      {
						//          // this block handles two cases which have
						//          // the same result:
						//          // 1. ratio is too high - mass0 is too
						//          //  large, treat solid0 as static
						//          // 2. ratio is good - treat solid0 as
						//          //  static
						//          dBodyEnable(o1BodyID);
						//          dJointAttach(contactJoint, 0, o1BodyID);
						//      }
						//  }
						//}
					}
				}
			}
		}

		Joint* internal_getCommonJoint(dBodyID body0, dBodyID body1)
		{
			// First we need to find the ODE joint connecting the bodies
			// (it would be ideal if ODE included this functionality...).
			// We only need to check one of the bodies' ODE joints
			// because it is assumed here that the two bodies are
			// connected, thus they should have a common ODE joint.
			int numJoints0 = dBodyGetNumJoints(body0);
			dJointID theJoint = 0;

			// Loop through body0's ODE joints.
			int i = 0;
			for (i = 0; i < numJoints0; ++i)
			{
				dJointID currentJoint = dBodyGetJoint(body0, i);
				dBodyID jointBody0 = dJointGetBody(currentJoint, 0);
				dBodyID jointBody1 = dJointGetBody(currentJoint, 1);

				if ((jointBody0 == body0 && jointBody1 == body1) ||
					(jointBody0 == body1 && jointBody1 == body0))
				{
					// This is the ODE joint connecting the two bodies.
					// Note that if the two bodies are connected by multiple
					// Joints, the behavior is undefined.
					theJoint = currentJoint;
				}
			}

			// Make sure the ODE joint was actually found.  This should
			// be guaranteed.
			assert(theJoint);

			// Now return the associated OPAL Joint.
			return (Joint*) dJointGetData(theJoint);
		}

		void internal_volumeCollisionCallback(void* data, dGeomID o0,
		                                       dGeomID o1)
		{
			if (dGeomIsSpace(o0) || dGeomIsSpace(o1))
			{
				// Colliding a space with either a geom or another space.
				dSpaceCollide2(o0, o1, data,
					&internal_volumeCollisionCallback);
			}
			else
			{
				// Colliding two geoms.

				//dBodyID o0BodyID = dGeomGetBody(o0);
				//dBodyID o1BodyID = dGeomGetBody(o1);
				//bool bothHaveBodies = true;
				//if (0 == o0BodyID || 0 == o1BodyID)
				//{
				//  bothHaveBodies = false;
				//}

				// don't do collision detection for the following case:
				// two shapes that are part of the same solid (they share a
				// body)
				// update-> this is already handled by ODE
				//if (bothHaveBodies && o0BodyID == o1BodyID)
				//{
				//  //don't do collision detection
				//  return;
				//}

				// Get a pointer to the ODESimulator.
				ODESimulator* sim = (ODESimulator*) data;

				// Get pointers to the two geoms' GeomData structure.  Both
				// of these should always be non-NULL.
				const GeomData* geomData0 = ((GeomData*) dGeomGetData(o0));
				const GeomData* geomData1 = ((GeomData*) dGeomGetData(o1));
				assert(geomData0);
				assert(geomData1);

				// Get pointers to the geoms' ShapeData structures.
				const ShapeData* shape0 = geomData0->shape;
				const ShapeData* shape1 = geomData1->shape;

				// Check if the two Solids' contact groups generate contacts
				// when they collide.
				bool makeContacts = sim->groupsMakeContacts(
					shape0->contactGroup, shape1->contactGroup);
				if (!makeContacts)
				{
					return ;
				}

				// Now actually test for collision between the two geoms.
				// This is a fairly expensive operation.
				dContactGeom contactArray[1];
				int numContacts = dCollide(o0, o1, 1, contactArray,
					sizeof(dContactGeom));

				if (0 == numContacts)
				{
					return ;
				}
				else
				{
					// These two geoms must be intersecting.

					// Get pointers to the geoms' Solids.
					Solid* solid0 = geomData0->solid;
					Solid* solid1 = geomData1->solid;

					// Not sure at this point if we can know that o1 is the
					// volume object, so we'll just call this twice.  It
					// will automatically keep from adding the same Solid
					// multiple times by using its collision count.  Later,
					// the volume Solid will be removed from this list.
					sim->internal_addCollidedSolid(solid0);
					sim->internal_addCollidedSolid(solid1);
				}
			}
		}

		void internal_raycastCollisionCallback(void* data, dGeomID o0,
		                                        dGeomID o1)
		{
			if (dGeomIsSpace(o0) || dGeomIsSpace(o1))
			{
				// Colliding a space with either a geom or another space.
				dSpaceCollide2(o0, o1, data,
					&internal_raycastCollisionCallback);
			}
			else
			{
				// Colliding two geoms.

				// Sometimes we get a case where the ray geom is passed in
				// as both objects, which is stupid.
				if (o0 == o1)
				{
					return ;
				}

				// Get a pointer to the ODESimulator.
				ODESimulator* sim = (ODESimulator*) data;

				// Get pointers to the two geoms' GeomData structure.  One
				// of these (the one NOT belonging to the ray geom)
				// will always be non-NULL.
				GeomData* geomData0 = ((GeomData*) dGeomGetData(o0));
				GeomData* geomData1 = ((GeomData*) dGeomGetData(o1));

				// Find the contact group of the collided Solid.
				unsigned int geomContactGroup = defaults::shape::contactGroup;
				if (geomData0)
				{
					geomContactGroup = geomData0->shape->contactGroup;
				}
				else
				{
					geomContactGroup = geomData1->shape->contactGroup;
				}

				// Check if the two Solids' contact groups generate contacts
				// when they collide.
				bool makeContacts = sim->groupsMakeContacts(
					geomContactGroup, sim->internal_getRayContactGroup());
				if (!makeContacts)
				{
					return ;
				}

				// Now actually test for collision between the two geoms.
				// This is a fairly expensive operation.
				dContactGeom contactArray[defaults::ode::maxRaycastContacts];
				int numContacts = dCollide(o0, o1, 
					defaults::ode::maxRaycastContacts, contactArray,
					sizeof(dContactGeom));

				if (0 == numContacts)
				{
					return ;
				}
				else
				{
					// These two geoms must be intersecting.  We will store 
					// only the closest RaycastResult.
					int closest = 0;
					for (int i = 0; i < numContacts; ++i)
					{
						if (contactArray[i].depth < contactArray[closest].depth)
						{
							closest = i;
						}
					}

					// Only one of the geoms will be part of a Solid we 
					// want to store; the other is the ray.
					Solid* solid = NULL;
					if (geomData0)
					{
						solid = geomData0->solid;
					}
					else
					{
						solid = geomData1->solid;
					}

					Point3r intersection((real) contactArray[closest].pos[0],
						(real) contactArray[closest].pos[1],
						(real) contactArray[closest].pos[2]);
					Vec3r normal((real) contactArray[closest].normal[0],
						(real) contactArray[closest].normal[1],
						(real) contactArray[closest].normal[2]);

					sim->internal_addRaycastResult(solid, intersection, 
						normal, (real) contactArray[closest].depth);
				}
			}
		}
	}

	void ODESimulator::internal_addCollidedSolid(Solid* solid)
	{
		// If the collided Solid is attached to the Sensor performing the
		// volume query, ignore this intersection.
		if (mSensorSolid == solid)
		{
			return ;
		}

		ODESolid* solidPtr = ((ODESolid*) solid);

		if (solidPtr->internal_getCollisionCount() != mCollisionCount)
		{
			mVolumeQueryResult.internal_addSolid(solid);
			solidPtr->internal_setCollisionCount(mCollisionCount);
		}
	}

	void ODESimulator::internal_addRaycastResult(Solid* solid,
	        const Point3r& intersection, const Vec3r& normal, real depth)
	{
		// If the collided Solid is attached to the Sensor performing the
		// raycast, ignore this intersection.
		if (mSensorSolid == solid)
		{
			return ;
		}

		// Add this RaycastResult to the vector of results.
		RaycastResult result;
		result.solid = solid;
		result.intersection = intersection;
		result.normal = normal;
		result.distance = depth;
		mRaycastResults.push_back(result);
	}

	unsigned int ODESimulator::internal_getRayContactGroup()
	{
		return mRayContactGroup;
	}

	//helper function for collision callback
	//void createOneSidedContact(dJointID contactJoint, dBodyID movingObject,
	//                                       dBodyID staticObject, dVector3 pos)
	//{
	//treat one object as static, but add make sure to add its velocity
	//(at the contact point) to the other

	//dVector3 result;
	//dBodyGetPointVel(staticObject, pos[0], pos[1], pos[2], result);

	////TODO: test this since it might not be totally correct
	//const dReal* linearVel = dBodyGetLinearVel(movingObject);
	//dVector3 newVel = {linearVel[0] + result[0], linearVel[1] + result[1], linearVel[2] + result[2]};
	//dBodySetLinearVel(movingObject, newVel[0], newVel[1], newVel[2]);

	//dBodyEnable(movingObject);
	//dJointAttach(contactJoint, 0, movingObject); //this works when movingObject is body2
	//dJointAttach(contactJoint, movingObject, 0); //this works when movingObject is body1
	//}

	std::vector<RaycastResult> & ODESimulator::internal_fireRay(const Rayr& r,
	        real length, const Solid* attachedSolid,
	        unsigned int rayContactGroup)
	{
		Point3r origin = r.getOrigin();
		Vec3r dir = r.getDir().unit();

		mRaycastResults.clear();
		mSensorSolid = attachedSolid;
		mRayContactGroup = rayContactGroup;

		// Create an ODE ray geom.  Make sure its user data pointer is
		// NULL because this is used in the collision callback to
		// distinguish the ray from other geoms.
		dGeomID rayGeomID = dCreateRay(mRootSpaceID, length);
		dGeomRaySet(rayGeomID, origin[0], origin[1], origin[2], dir[0],
		             dir[1], dir[2]);
		dGeomSetData(rayGeomID, NULL);

		// Check for collisions.  This will fill mRaycastResult with valid
		// data.  Its Solid pointer will remain NULL if nothing was hit.
		dSpaceCollide2(rayGeomID, (dGeomID) mRootSpaceID, this,
		                &ode_hidden::internal_raycastCollisionCallback);

		// Finished with ODE ray, so destroy it.
		dGeomDestroy(rayGeomID);

		return mRaycastResults;
	}

	const VolumeQueryResult& ODESimulator::internal_queryVolume(
	    const Solid* volume, const Solid* attachedSolid)
	{
		mSensorSolid = attachedSolid;
		mCollisionCount++;
		mVolumeQueryResult.internal_clearSolids();
		const std::vector<GeomData*>* geomList =
		    ((ODESolid*) volume)->internal_getGeomDataList();

		// Check for collisions with each of the volume Solid's geoms.
		// This will fill up mVolumeQueryResult with those Solids that
		// collide with these geoms.
		std::vector<GeomData*>::const_iterator iter;
		for (iter = geomList->begin(); iter != geomList->end(); ++iter)
		{
			dGeomID geomID = (*iter)->geomID;
			if ((*iter)->transformID)
			{
				// Using geom transform.
				geomID = (*iter)->transformID;
			}

			dSpaceCollide2(geomID, (dGeomID) mRootSpaceID, this,
				&ode_hidden::internal_volumeCollisionCallback);
		}

		// We don't want the volume Solid to be listed in the results.
		mVolumeQueryResult.internal_removeSolid(volume);

		return mVolumeQueryResult;
	}

	void ODESimulator::setGravity(const Vec3r& gravity)
	{
		dWorldSetGravity(mWorldID, gravity[0], gravity[1], gravity[2]);
	}

	Vec3r ODESimulator::getGravity() const
	{
		dVector3 g;
		dWorldGetGravity(mWorldID, g);
		Vec3r gravity((real) g[0], (real) g[1], (real) g[2]);
		return gravity;
	}

	void ODESimulator::setSolverAccuracy(SolverAccuracyLevel level)
	{
		Simulator::setSolverAccuracy(level);

		switch (level)
		{
			case SOLVER_ACCURACY_VERY_LOW:
				mSolverType = SOLVER_QUICKSTEP;
				dWorldSetQuickStepNumIterations(mWorldID, 5);
				break;
			case SOLVER_ACCURACY_LOW:
				mSolverType = SOLVER_QUICKSTEP;
				dWorldSetQuickStepNumIterations(mWorldID, 10);
				break;
			case SOLVER_ACCURACY_MEDIUM:
				mSolverType = SOLVER_QUICKSTEP;
				// 20 is the default in ODE
				dWorldSetQuickStepNumIterations(mWorldID, 20);
				break;
			case SOLVER_ACCURACY_HIGH:
				mSolverType = SOLVER_QUICKSTEP;
				dWorldSetQuickStepNumIterations(mWorldID, 40);
				break;
			case SOLVER_ACCURACY_VERY_HIGH:
				mSolverType = SOLVER_WORLDSTEP;
				break;
			default:
				assert(false);
				break;
		}
	}

	void ODESimulator::setGlobalStability(real value)
	{
		assert(value >= 0 && value <= 1);
		dWorldSetCFM(mWorldID, value);
	}

	dWorldID ODESimulator::internal_getWorldID() const
	{
		return mWorldID;
	}

	dSpaceID ODESimulator::internal_getSpaceID() const
	{
		return mRootSpaceID;
	}

	dJointGroupID ODESimulator::internal_getJointGroupID() const
	{
		return mContactJointGroupID;
	}
}
