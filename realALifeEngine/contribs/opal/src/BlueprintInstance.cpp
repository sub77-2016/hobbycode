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
#include "BlueprintInstance.h"

// project headers
#include "Solid.h"
#include "Joint.h"
#include "Motor.h"
#include "Sensor.h"

// system headers
#include <map>
#include <vector>

using namespace std;

namespace opal
{
	BlueprintInstance::BlueprintInstance()
	{
		// nothing to do
	}

	BlueprintInstance::~BlueprintInstance()
	{
		mSolidMap.clear();
		mJointMap.clear();
		mMotorMap.clear();
		mSensorMap.clear();

		mSolidList.clear();
		mJointList.clear();
		mMotorList.clear();
		mSensorList.clear();
	}

	Solid * BlueprintInstance::detachSolid(unsigned int i)
	{
		assert(i < mSolidList.size());
		Solid * detached = mSolidList[i];

		// detach from list
		mSolidList.erase(mSolidList.begin() + i);

		// detach from map
		map<string, Solid*>::iterator iter = 
			mSolidMap.find(detached->getName());
		mSolidMap.erase(iter);

		return detached;
	}

	Joint * BlueprintInstance::detachJoint(unsigned int i)
	{
		assert(i < mJointList.size());
		Joint * detached = mJointList[i];

		// detach from list
		mJointList.erase(mJointList.begin() + i);

		// detach from map
		map<string, Joint*>::iterator iter = 
			mJointMap.find(detached->getName());
		mJointMap.erase(iter);

		return detached;
	}

	Sensor * BlueprintInstance::detachSensor(unsigned int i)
	{
		assert(i < mSensorList.size());
		Sensor * detached = mSensorList[i];

		// detach from list
		mSensorList.erase(mSensorList.begin() + i);

		// detach from map
		map<string, Sensor*>::iterator iter = 
			mSensorMap.find(detached->getName());
		mSensorMap.erase(iter);

		return detached;
	}

	Motor * BlueprintInstance::detachMotor(unsigned int i)
	{
		assert(i < mMotorList.size());
		Motor * detached = mMotorList[i];

		// detach from list
		mMotorList.erase(mMotorList.begin() + i);

		// detach from map
		map<string, Motor*>::iterator iter = 
			mMotorMap.find(detached->getName());
		mMotorMap.erase(iter);

		return detached;
	}

	Solid * BlueprintInstance::detachSolid(const string & name)
	{
		map<string, Solid*>::iterator iter = 
			mSolidMap.find(name);
		Solid * detached = iter->second;

		// detach from list
		vector<Solid *>::iterator listIter;
		for (listIter = mSolidList.begin(); 
			listIter != mSolidList.end(); 
			++listIter)
		{
			if ((*listIter)->getName() == name)
			{
				mSolidList.erase(listIter);
				break;
			}
		}

		// detach from map		
		mSolidMap.erase(iter);

		return detached;
	}

	Solid* BlueprintInstance::getSolid(const string& name)const
	{
		map<string, Solid*>::const_iterator iter = 
			mSolidMap.find(name);
		if (mSolidMap.end() == iter)
		{
			return NULL;
		}
		else
		{
			return (*iter).second;
		}
	}

	Joint* BlueprintInstance::getJoint(const string& name)const
	{
		map<string, Joint*>::const_iterator iter = 
			mJointMap.find(name);
		if (mJointMap.end() == iter)
		{
			return NULL;
		}
		else
		{
			return (*iter).second;
		}
	}

	Motor* BlueprintInstance::getMotor(const string& name)const
	{
		map<string, Motor*>::const_iterator iter = 
			mMotorMap.find(name);
		if (mMotorMap.end() == iter)
		{
			return NULL;
		}
		else
		{
			return (*iter).second;
		}
	}

	Sensor* BlueprintInstance::getSensor(const string& name)const
	{
		map<string, Sensor*>::const_iterator iter = 
			mSensorMap.find(name);
		if (mSensorMap.end() == iter)
		{
			return NULL;
		}
		else
		{
			return (*iter).second;
		}
	}

	unsigned int BlueprintInstance::getNumSolids()const
	{
		return (int)(mSolidList.size());
	}

	unsigned int BlueprintInstance::getNumJoints()const
	{
		return (int)(mJointList.size());
	}

	unsigned int BlueprintInstance::getNumMotors()const
	{
		return (int)(mMotorList.size());
	}

	unsigned int BlueprintInstance::getNumSensors()const
	{
		return (int)(mSensorList.size());
	}

	Solid* BlueprintInstance::getSolid(unsigned int i)const
	{
		return mSolidList.at(i);
	}

	Joint* BlueprintInstance::getJoint(unsigned int i)const
	{
		return mJointList.at(i);
	}

	Motor* BlueprintInstance::getMotor(unsigned int i)const
	{
		return mMotorList.at(i);
	}

	Sensor* BlueprintInstance::getSensor(unsigned int i)const
	{
		return mSensorList.at(i);
	}

	void BlueprintInstance::addSolid(Solid* s)
	{
		mSolidList.push_back(s);

		if (NULL == s)
		{
			return;
		}

		string name = s->getName();

		if (name.empty())
		{
			return;
		}

		mSolidMap[name] = s;
	}

	void BlueprintInstance::addJoint(Joint* j)
	{
		mJointList.push_back(j);

		if (NULL == j)
		{
			return;
		}

		string name = j->getName();

		if (name.empty())
		{
			return;
		}

		mJointMap[name] = j;
	}

	void BlueprintInstance::addMotor(Motor* m)
	{
		mMotorList.push_back(m);

		if (NULL == m)
		{
			return;
		}

		string name = m->getName();

		if (name.empty())
		{
			return;
		}

		mMotorMap[name] = m;
	}

	void BlueprintInstance::addSensor(Sensor* s)
	{
		mSensorList.push_back(s);

		if (NULL == s)
		{
			return;
		}

		string name = s->getName();

		if (name.empty())
		{
			return;
		}

		mSensorMap[name] = s;
	}
}
