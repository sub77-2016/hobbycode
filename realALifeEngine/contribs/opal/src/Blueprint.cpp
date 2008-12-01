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

#include "Blueprint.h"
#include "SolidData.h"
#include "JointData.h"
#include "MotorData.h"
#include "AttractorMotorData.h"
#include "GearedMotorData.h"
#include "ServoMotorData.h"
#include "SpringMotorData.h"
#include "ThrusterMotorData.h"
#include "SensorData.h"
#include "AccelerationSensorData.h"
#include "InclineSensorData.h"
#include "RaycastSensorData.h"
#include "VolumeSensorData.h"

namespace opal
{
	Blueprint::Blueprint()
	{
		mFinalized = false;
	}

	Blueprint::~Blueprint()
	{
		while (!mSolidList.empty())
		{
			delete mSolidList.back();
			mSolidList.pop_back();
		}

		while (!mJointList.empty())
		{
			delete mJointList.back();
			mJointList.pop_back();
		}

		while (!mMotorList.empty())
		{
			delete mMotorList.back();
			mMotorList.pop_back();
		}

		while (!mSensorList.empty())
		{
			delete mSensorList.back();
			mSensorList.pop_back();
		}
	}

	void Blueprint::finalize()
	{
		// Setup Joint reference indices.
		size_t i;
		for (i=0; i<mJointList.size();)
		{
			JointData* data = mJointList.at(i);

			// Find the indices of the references.
			int solid0Index = getSolidIndex(data->solid0BlueprintRefName);
			int solid1Index = getSolidIndex(data->solid1BlueprintRefName);

			// Here we allow one of the Solid indices to be -1 in case the 
			// user is attaching the Joint to the static environment.
			if (-1 == solid0Index && -1 == solid1Index)
			{
				OPAL_LOGGER("warning") << 
					"opal::Blueprint::finalize: Invalid references "
					<< "in Joint " << data->name << ". Removing the Joint "
					<< "from the Blueprint." << std::endl;

				delete data;
				mJointList.at(i) = mJointList.back();
				mJointList.pop_back();
			}
			else
			{
				data->internal_solid0Index = solid0Index;
				data->internal_solid1Index = solid1Index;
				++i;
			}
		}

		// Setup Motor reference indices.
		for (i=0; i<mMotorList.size();)
		{
			MotorData* m = mMotorList.at(i);
			bool error = false;

			switch(m->getType())
			{
				case ATTRACTOR_MOTOR:
				{
					AttractorMotorData* data = 
						(AttractorMotorData*)mMotorList.at(i);
					int solid0Index = 
						getSolidIndex(data->solid0BlueprintRefName);
					int solid1Index = 
						getSolidIndex(data->solid1BlueprintRefName);
					if (-1 == solid0Index || -1 == solid1Index)
					{
						error = true;
					}
					else
					{
						data->internal_solid0Index = solid0Index;
						data->internal_solid1Index = solid1Index;
					}
					break;
				}
				case GEARED_MOTOR:
				{
					GearedMotorData* data = 
						(GearedMotorData*)mMotorList.at(i);
					int jointIndex = getJointIndex(data->jointBlueprintRefName);
					if (-1 == jointIndex)
					{
						error = true;
					}
					else
					{
						data->internal_jointIndex = jointIndex;
					}
					break;
				}
				case SERVO_MOTOR:
				{
					ServoMotorData* data = (ServoMotorData*)mMotorList.at(i);
					int jointIndex = getJointIndex(data->jointBlueprintRefName);
					if (-1 == jointIndex)
					{
						error = true;
					}
					else
					{
						data->internal_jointIndex = jointIndex;
					}
					break;
				}
				case SPRING_MOTOR:
				{
					SpringMotorData* data = 
						(SpringMotorData*)mMotorList.at(i);
					int solidIndex = getSolidIndex(data->solidBlueprintRefName);
					if (-1 == solidIndex)
					{
						error = true;
					}
					else
					{
						data->internal_solidIndex = solidIndex;
					}
					break;
				}
				case THRUSTER_MOTOR:
				{
					ThrusterMotorData* data = 
						(ThrusterMotorData*)mMotorList.at(i);
					int solidIndex = getSolidIndex(data->solidBlueprintRefName);
					if (-1 == solidIndex)
					{
						error = true;
					}
					else
					{
						data->internal_solidIndex = solidIndex;
					}
					break;
				}
				default:
					assert(false);
			}

			if (error)
			{
				OPAL_LOGGER("warning") << 
					"opal::Blueprint::finalize: Invalid reference in Motor "
					<< mMotorList.at(i)->name 
					<< ". Removing the Motor from the Blueprint." 
					<< std::endl;

				delete mMotorList.at(i);
				mMotorList.at(i) = mMotorList.back();
				mMotorList.pop_back();
			}
			else
			{
				++i;
			}
		}

		// Setup Sensor reference indices.
		for (i=0; i<mSensorList.size(); ++i)
		{
			SensorData* data = mSensorList.at(i);

			// At this time we have no Sensors with special references 
			// besides the standard Solid reference present in every 
			// Sensor.  Thus we can just treat them all the same.

			// Find the index of the Solid reference.  If the value of 
			// the index is -1, the Sensor must not be attached to any 
			// Solid.
			int solidIndex = getSolidIndex(data->solidBlueprintRefName);
			data->internal_solidIndex = solidIndex;
		}

		mFinalized = true;
	}

	bool Blueprint::isFinalized()const
	{
		return mFinalized;
	}

	void Blueprint::addSolid(SolidData* data)
	{
		if (mFinalized)
		{
			printFinalizedMessage();
			return;
		}

		if (solidExists(data->name))
		{
			OPAL_LOGGER("warning") << "opal::Blueprint::addSolid: Solid " 
				<< data->name << " already exists in this Blueprint.  \
Ignoring the new one." << std::endl;
		}
		else
		{
			SolidData* newData = new SolidData(*data);
			mSolidList.push_back(newData);
		}
	}

	void Blueprint::addJoint(JointData* data)
	{
		if (mFinalized)
		{
			printFinalizedMessage();
			return;
		}

		if (jointExists(data->name))
		{
			OPAL_LOGGER("warning") << "opal::Blueprint::addJoint: Joint " 
				<< data->name << " already exists in this Blueprint.  \
Ignoring the new one." << std::endl;
		}
		else
		{
			JointData* newData = new JointData(*data);
			mJointList.push_back(newData);
		}
	}

	void Blueprint::addMotor(MotorData* data)
	{
		if (mFinalized)
		{
			printFinalizedMessage();
			return;
		}

		if (motorExists(data->name))
		{
			OPAL_LOGGER("warning") << "opal::Blueprint::addMotor: Motor " 
				<< data->name << " already exists in this Blueprint.  \
Ignoring the new one." << std::endl;
		}
		else
		{
			MotorData* newData = NULL;

			// Allocate the right type of Motor.
			switch(data->getType())
			{
				case ATTRACTOR_MOTOR:
					newData = 
						new AttractorMotorData(*((AttractorMotorData*)data));
					break;
				case GEARED_MOTOR:
					newData = 
						new GearedMotorData(*((GearedMotorData*)data));
					break;
				case SERVO_MOTOR:
					newData = 
						new ServoMotorData(*((ServoMotorData*)data));
					break;
				case SPRING_MOTOR:
					newData = 
						new SpringMotorData(*((SpringMotorData*)data));
					break;
				case THRUSTER_MOTOR:
					newData = 
						new ThrusterMotorData(*((ThrusterMotorData*)data));
					break;
				default:
					assert(false);
			}

			mMotorList.push_back(newData);
		}
	}

	void Blueprint::addSensor(SensorData* data)
	{
		if (mFinalized)
		{
			printFinalizedMessage();
			return;
		}

		if (sensorExists(data->name))
		{
			OPAL_LOGGER("warning") << "opal::Blueprint::addSensor: Sensor " 
				<< data->name << " already exists in this Blueprint.  \
Ignoring the new one." << std::endl;
		}
		else
		{
			SensorData* newData = NULL;

			// Allocate the right type of Sensor.
			switch(data->getType())
			{
				case ACCELERATION_SENSOR:
				{
					newData = 
						new AccelerationSensorData(
						*((AccelerationSensorData*)data));
					break;
				}
				case INCLINE_SENSOR:
				{
					newData = 
						new InclineSensorData(*((InclineSensorData*)data));
					break;
				}
				case RAYCAST_SENSOR:
				{
					newData = 
						new RaycastSensorData(*((RaycastSensorData*)data));
					break;
				}
				case VOLUME_SENSOR:
				{
					newData = 
						new VolumeSensorData(*((VolumeSensorData*)data));
					break;
				}
				default:
					assert(false);
			}

			mSensorList.push_back(newData);
		}
	}

	unsigned int Blueprint::getNumSolids()const
	{
		return (int)(mSolidList.size());
	}

	unsigned int Blueprint::getNumJoints()const
	{
		return (int)(mJointList.size());
	}

	unsigned int Blueprint::getNumMotors()const
	{
		return (int)(mMotorList.size());
	}

	unsigned int Blueprint::getNumSensors()const
	{
		return (int)(mSensorList.size());
	}

	SolidData* Blueprint::getSolidData(unsigned int i)const
	{
		return mSolidList.at(i);
	}

	JointData* Blueprint::getJointData(unsigned int i)const
	{
		return mJointList.at(i);
	}

	MotorData* Blueprint::getMotorData(unsigned int i)const
	{
		return mMotorList.at(i);
	}

	SensorData* Blueprint::getSensorData(unsigned int i)const
	{
		return mSensorList.at(i);
	}

	bool Blueprint::solidExists(const std::string& name)
	{
		/// Ignore empty name strings.
		if (name.empty())
		{
			return false;
		}

		std::vector<SolidData*>::iterator iter;
		for (iter = mSolidList.begin(); iter != mSolidList.end(); ++iter)
		{
			if ((*iter)->name == name)
			{
				return true;
			}
		}

		return false;
	}

	bool Blueprint::jointExists(const std::string& name)
	{
		/// Ignore empty name strings.
		if (name.empty())
		{
			return false;
		}

		std::vector<JointData*>::iterator iter;
		for (iter = mJointList.begin(); iter != mJointList.end(); ++iter)
		{
			if ((*iter)->name == name)
			{
				return true;
			}
		}

		return false;
	}

	bool Blueprint::motorExists(const std::string& name)
	{
		/// Ignore empty name strings.
		if (name.empty())
		{
			return false;
		}

		std::vector<MotorData*>::iterator iter;
		for (iter = mMotorList.begin(); iter != mMotorList.end(); ++iter)
		{
			if ((*iter)->name == name)
			{
				return true;
			}
		}

		return false;
	}

	bool Blueprint::sensorExists(const std::string& name)
	{
		/// Ignore empty name strings.
		if (name.empty())
		{
			return false;
		}

		std::vector<SensorData*>::iterator iter;
		for (iter = mSensorList.begin(); iter != mSensorList.end(); ++iter)
		{
			if ((*iter)->name == name)
			{
				return true;
			}
		}

		return false;
	}

	int Blueprint::getSolidIndex(const std::string& name)const
	{
		if (name.empty())
		{
			return -1;
		}

		for (unsigned int i=0; i<mSolidList.size(); ++i)
		{
			if (mSolidList[i]->name == name)
			{
				return (int)i;
			}
		}

		return -1;
	}

	int Blueprint::getJointIndex(const std::string& name)const
	{
		if (name.empty())
		{
			return -1;
		}

		for (unsigned int i=0; i<mJointList.size(); ++i)
		{
			if (mJointList[i]->name == name)
			{
				return (int)i;
			}
		}

		return -1;
	}

	void Blueprint::printFinalizedMessage()
	{
		OPAL_LOGGER("warning") << "Cannot add objects to a finalized \
Blueprint.  The operation will be ignored." << std::endl;
	}
}
