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

#ifndef OPAL_COLLISION_EVENT_HANDLER_H
#define OPAL_COLLISION_EVENT_HANDLER_H

// project headers
#include "Defines.h"
#include "EventHandler.h"

namespace opal
{
	class Solid;

	/// A data structure containing info about a specific collision event
	/// involving two Solids.
	struct CollisionEvent
	{
		CollisionEvent()
		{
			thisSolid = NULL;
			otherSolid = NULL;
			// "pos" is initialized in its own constructor.
			// "normal" is initialized in its own constructor.
			depth = 0;
		}

		/// The colliding Solid whose CollisionEventHandler gets called.
		Solid* thisSolid;

		/// The Solid that collided with the one owning the
		/// CollisionEventHandler.
		Solid* otherSolid;

		/// The point of collision.
		Point3r pos;

		/// The normal vector at the point of collision.
		Vec3r normal;

		/// The depth of interpenetration.  This may not be very helpful
		/// if the two Solids are allowed to pass through each other (i.e.
		/// their contact groups do not generate contacts).
		real depth;
	};

	/// A listener that gets notified when two Solids touch.  These events
	/// get handled at the end of the time step, which is necessary because
	/// some physics engines cannot update objects in the middle of a time
	/// step (e.g. in the middle of collision detection).
	class CollisionEventHandler : public EventHandler
	{
		public:
			OPAL_DECL CollisionEventHandler();

			OPAL_DECL virtual ~CollisionEventHandler();

			/// Called once for each pending CollisionEvent.  This is always
			/// called at the end of every time step so CollisionEvents
			/// get handled right away.
			OPAL_DECL virtual void OPAL_CALL handleCollisionEvent(
			    const CollisionEvent& e) = 0;

			/// Adds a CollisionEvent to this handler's internal list.  These
			/// will get handled at the end of the current time step.
			void internal_pushCollisionEvent(const CollisionEvent& e);

			/// Loops through the pending CollisionEvents, calling the event
			/// handling function (which is always user-defined) for each.
			void internal_handlePendingCollisionEvents();

		protected:
			/// This list holds pending CollisionEvents.  It allows events to
			/// be queued during collision detection and handled at a safe time.
			std::vector<CollisionEvent> mPendingCollisionEvents;

		private:

	};
}

#endif

