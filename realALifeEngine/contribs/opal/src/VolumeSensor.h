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

#ifndef OPAL_VOLUME_SENSOR_H
#define OPAL_VOLUME_SENSOR_H

#include "Defines.h"
#include "Sensor.h"
#include "VolumeSensorData.h"

namespace opal
{
	class Simulator;

	/// A data structure containing information about a specific 
	/// volume query.
	struct VolumeQueryResult
	{
		OPAL_DECL VolumeQueryResult()
		{
		}

		/// Copy constructor.
		OPAL_DECL VolumeQueryResult(const VolumeQueryResult& result)
		{
			(*this) = result;
		}

		OPAL_DECL ~VolumeQueryResult()
		{
			internal_clearSolids();
		}

		/// Makes a deep copy.
		OPAL_DECL void OPAL_CALL operator=(
			const VolumeQueryResult& result)
		{
			unsigned int i=0;
			for (i=0; i<result.mSolidList.size(); ++i)
			{
				mSolidList.push_back(result.mSolidList[i]);
			}
		}

		/// Adds a Solid pointer to the list of results.
		void internal_addSolid(Solid* s)
		{
			assert(s);
			mSolidList.push_back(s);
		}

		/// Returns the number of Solids in the results.
		OPAL_DECL unsigned int OPAL_CALL getNumSolids()const
		{
			return (unsigned int)(mSolidList.size());
		}

		OPAL_DECL Solid* OPAL_CALL getSolid(unsigned int i)const
		{
			return mSolidList.at(i);
		}

		/// Removes all Solids from the results.
		void internal_clearSolids()
		{
			mSolidList.clear();
		}

		/// Removes a specified Solid from the list of collided Solids.
		void internal_removeSolid(const Solid* s)
		{
			for(size_t i = 0; i<mSolidList.size(); ++i)
			{
				if(mSolidList[i] == s)
				{
					mSolidList[i] = mSolidList.back();
					mSolidList.pop_back();
					return;
				}
			}
		}

	private:
		/// A list of Solids that were found in a volume query.
		std::vector<Solid*> mSolidList;
	};

	/// A Sensor that queries a scene to find which Solids are intersecting 
	/// a specified volume.  Each volume query returns a data structure 
	/// containing a list of those Solids.  This Sensor does not do a volume 
	/// query every time step because that would be a waste of time in 
	/// most cases; it must be queried manually.
	class VolumeSensor : public Sensor
	{
	public:
		VolumeSensor(Simulator* s);

		virtual ~VolumeSensor();

		/// Initializes the Sensor with the given data structure.
		virtual void OPAL_CALL init(const VolumeSensorData& data);

		/// Returns all data describing the Sensor.
		virtual const VolumeSensorData& OPAL_CALL getData()const;

		/// Queries the Sensor's environment with the given "volume Solid," 
		/// returning a list of the Solids that collide with the volume.  
		/// The volume Solid's transform will be totally ignored; use the 
		/// Sensor's transform instead.  Be sure to disable the volume 
		/// Solid first so it doesn't affect other Solids physically.  
		/// If this Sensor is attached to a Solid, that Solid will 
		/// not be added to the results.
		virtual const VolumeQueryResult& OPAL_CALL queryVolume(
			Solid* volume);

		virtual void OPAL_CALL setEnabled(bool e);

		virtual bool OPAL_CALL isEnabled()const;

		virtual void OPAL_CALL setTransform(const Matrix44r& t);

		virtual const Matrix44r& OPAL_CALL getTransform()const;

		virtual SensorType OPAL_CALL getType()const;

		virtual void OPAL_CALL setName(const std::string& name);

		virtual const std::string& OPAL_CALL getName()const;

		virtual void internal_update();

		virtual bool internal_dependsOnSolid(Solid* s);

	protected:
		/// Stores data describing the Sensor.
		VolumeSensorData mData;

		/// Pointer to the Simulator containing this Sensor; used to fire 
		/// rays into the Simulator.
		Simulator* mSim;

	private:
	};
}

#endif

