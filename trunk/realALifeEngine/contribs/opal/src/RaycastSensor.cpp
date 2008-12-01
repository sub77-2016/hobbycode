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
#include "RaycastSensor.h"
#include "Simulator.h"

// system headers
#include <algorithm>

using namespace std;

namespace opal
{
    RaycastSensor::RaycastSensor(Simulator* s)
    {
        // "mData" is initialized in its own constructor.
        mSim = s;
    }

    RaycastSensor::~RaycastSensor()
    {}

    void RaycastSensor::init(const RaycastSensorData& data)
    {
        Sensor::init();
        mData = data;
    }

    const RaycastSensorData& RaycastSensor::getData() const
    {
        return mData;
    }

    const RaycastResult & RaycastSensor::fireRay()
    {
        return fireRay(mData.ray.getLength());
    }

    const vector<RaycastResult> & RaycastSensor::firePiercingRay()
    {
        return firePiercingRay(mData.ray.getLength());
    }

    const RaycastResult & RaycastSensor::fireRay(real length)
    {
        static RaycastResult junkResult;

        if (mData.enabled)
        {
            Rayr ray = mData.ray;

            // If the Sensor is attached to a Solid, we need to transform
            // the ray relative to that Solid's transform.
            if (mData.solid)
            {
                ray = mData.solid->getTransform() * mData.transform * ray;
            }
			else
			{
				// If the Sensor is not attached to a Solid, just use the 
				// Sensor's transform as a global transform on the ray.
				ray = mData.transform * ray;
			}

            // If this is attached to a Solid, the Simulator raycast function
            // will automatically ignore intersections between the ray and
            // that Solid.

            vector<RaycastResult> & results = mSim->internal_fireRay(
				ray, length, mData.solid, mData.contactGroup);

            if (results.size() == 0)
                return junkResult;

            size_t closest = 0;
            for (size_t i = 1; i < results.size(); ++i)
            {
                if (results[i].distance < results[closest].distance)
                    closest = i;
            }

            return results[closest];
        }
        else
        {
            return junkResult;
        }
    }

    const vector<RaycastResult> & RaycastSensor::firePiercingRay(real length)
    {
        static vector<RaycastResult> empty;

        if (mData.enabled)
        {
            // the original ray shouldn't change
            Rayr ray = mData.ray;

            // If the Sensor is attached to a Solid, we need to transform
            // the ray relative to that Solid's transform.
            if (mData.solid)
            {
				ray = mData.solid->getTransform() * mData.transform * ray;
            }
			else
			{
				// If the Sensor is not attached to a Solid, just use the 
				// Sensor's transform as a global transform on the ray.
				ray = mData.transform * ray;
			}

            // If this is attached to a Solid, the Simulator raycast function
            // will automatically ignore intersections between the ray and
            // that Solid.

            vector<RaycastResult> & results = mSim->internal_fireRay(
				ray, length, mData.solid, mData.contactGroup);
            sort(results.begin(), results.end());

            return results;
        }
        else
        {
            return empty;
        }
    }

    void RaycastSensor::setEnabled(bool e)
    {
        //if (!mInitCalled)
        //{
        //	return;
        //}

        mData.enabled = e;
    }

    bool RaycastSensor::isEnabled() const
    {
        return mData.enabled;
    }

    void RaycastSensor::setRay(const Rayr& r)
    {
        mData.ray = r;
    }

    const Rayr& RaycastSensor::getRay() const
    {
        return mData.ray;
    }

    SensorType RaycastSensor::getType() const
    {
        return mData.getType();
    }

    void RaycastSensor::setTransform(const Matrix44r& t)
    {
        mData.transform = t;
    }

    const Matrix44r& RaycastSensor::getTransform() const
    {
        return mData.transform;
    }

    void RaycastSensor::setName(const std::string& name)
    {
        mData.name = name;
    }

    const std::string& RaycastSensor::getName() const
    {
        return mData.name;
    }

    void RaycastSensor::internal_update()
    {
        if (mData.enabled && mData.solid)
        {
            // Do nothing.
        }
    }

    bool RaycastSensor::internal_dependsOnSolid(Solid* s)
    {
        if (s == mData.solid)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator<(const RaycastResult & l, const RaycastResult & r)
    {
        return l.distance < r.distance;
    }
}
