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

#ifndef OPAL_RAYCAST_SENSOR_H
#define OPAL_RAYCAST_SENSOR_H

// project headers
#include "Defines.h"
#include "Sensor.h"
#include "RaycastSensorData.h"

namespace opal
{
    class Simulator;

    /// A data structure containing information about a specific
    /// intersection from a ray cast.
    struct RaycastResult
    {
        OPAL_DECL RaycastResult()
        {
            solid = NULL;
            // "intersection" is initialized in its own constructor.
            // "normal" is initialized in its own constructor.
            distance = 0;
        }

        /// Copy constructor.
        OPAL_DECL RaycastResult(const RaycastResult& result)
        {
            (*this) = result;
        }

        OPAL_DECL ~RaycastResult()
        {}

        /// Makes a deep copy.
        OPAL_DECL void OPAL_CALL operator=(
            const RaycastResult& result)
        {
            solid = result.solid;
            intersection = result.intersection;
            normal = result.normal;
            distance = result.distance;
        }

        /// The first Solid hit by the ray.  This will remain NULL if no
        /// Solid is hit.
        Solid* solid;

        /// The point of intersection.
        Point3r intersection;

        /// The normal vector at the point of intersection.
        Vec3r normal;

        /// The distance from the base of the ray to the intersection
        /// point.
        real distance;
    };

    //! Compares RaycastResults by distance.
    bool operator<(const RaycastResult & l, const RaycastResult & r);

    /// A Sensor that fires a ray into a scene and returns data describing
    /// the closest intersection, if any.  This Sensor does not fire a ray
    /// every time step because that would be a waste of time in most
    /// cases; it must be "fired" manually.
    class RaycastSensor : public Sensor
    {
        public:
            //! Constructor.
            RaycastSensor(Simulator* s);

            //! Destructor.
            virtual ~RaycastSensor();

            /// Initializes the Sensor with the given data structure.  If the
            /// Solid pointer in the data is valid, the Sensor's offset will
            /// be relative to the Solid's transform instead of the global
            /// origin.
            virtual void OPAL_CALL init(const RaycastSensorData & data);

            /// Returns all data describing the Sensor.
            virtual const RaycastSensorData & OPAL_CALL getData() const;

            /// Fires a ray into the Sensor's environment, returning
            /// information about the closest intersection encountered.  The
            /// length of the ray used will be the RaycastSensorData length.
            virtual const RaycastResult & OPAL_CALL fireRay();

            //! Same as fireRay, except that this function returns a set of sorted results.
            /*!
             * @note The length of the ray will be the RaycastSensorData length.
             *
             * @return All intersections, sorted by their distance from the base of the ray.
             */
            virtual const std::vector<RaycastResult> & OPAL_CALL firePiercingRay();

            //! Same as the other fireRay, except that the length is an explicit parameter.
            /*!
             * @param length The maximum distance of the ray cast.
             * 
             * @return The closest intersection that was encountered.
             */
            virtual const RaycastResult& OPAL_CALL fireRay(real length);

            //! Same as the other firePiercingRay, except that the length is an explicit parameter.
            /*!
             * @param length The maximum distance of the ray cast.
             *
             * @return All intersections, sorted by their distance from the base of the ray.
             */
            virtual const std::vector<RaycastResult> & OPAL_CALL firePiercingRay(real length);

            virtual void OPAL_CALL setEnabled(bool e);

            virtual bool OPAL_CALL isEnabled() const;

            /// Sets the Sensor's ray.
            virtual void OPAL_CALL setRay(const Rayr& r);

            /// Returns the Sensor's ray.
            virtual const Rayr& OPAL_CALL getRay() const;

            virtual void OPAL_CALL setTransform(const Matrix44r& t);

            virtual const Matrix44r& OPAL_CALL getTransform() const;

            virtual SensorType OPAL_CALL getType() const;

            virtual void OPAL_CALL setName(const std::string& name);

            virtual const std::string& OPAL_CALL getName() const;

            virtual void internal_update();

            virtual bool internal_dependsOnSolid(Solid* s);

        protected:
            /// Stores data describing the Sensor.
            RaycastSensorData mData;

            /// Pointer to the Simulator containing this Sensor; used to fire
            /// rays into the Simulator.
            Simulator* mSim;

        private:
    };
}

#endif
