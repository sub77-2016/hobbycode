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
#include "BlueprintManager.h"

// project headers
#include "BoxShapeData.h"
#include "SphereShapeData.h"
#include "CapsuleShapeData.h"
#include "CylinderShapeData.h"
#include "PlaneShapeData.h"
#include "MotorData.h"
#include "AttractorMotorData.h"
#include "GearedMotorData.h"
#include "ServoMotorData.h"
#include "ThrusterMotorData.h"
#include "SpringMotorData.h"
#include "SensorData.h"
#include "AccelerationSensorData.h"
#include "InclineSensorData.h"
#include "RaycastSensorData.h"
#include "VolumeSensorData.h"

// system headers
#include "external/tinyxml/tinyxml.h"

using namespace opal;
using namespace std;

namespace opal
{
	OPAL_EXPORT_FUNCTION void OPAL_CALL loadFile(Blueprint& bp,
	        const string& filename)
	{
		BlueprintManager::instance().loadFile(bp, filename);
	}

	namespace blueprintManagerImpl
	{
		BlueprintManager::BlueprintManager()
		{}

		BlueprintManager::~BlueprintManager()
		{}

		void BlueprintManager::loadFile(Blueprint& bp,
		                                 const string& filename)
		{
			// Load the file.
			TiXmlDocument file;
			if (false == file.LoadFile(filename.c_str()))
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadFile: Cannot open " <<
				filename << endl;
				return ;
			}

			// Find the root element (i.e. the 'OpalBlueprint' element).
			TiXmlElement* rootElement = file.RootElement();
			if (NULL == rootElement)
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadFile: Missing root element "
				<< "in " << filename << ". Ignoring file." << endl;
				return ;
			}

			TiXmlNode* nodePtr = NULL;

			// Loop over Solids.
			nodePtr = NULL;
			while ((nodePtr = rootElement->IterateChildren("Solid", nodePtr)) != NULL)
			{
				SolidData * data = loadSolid(nodePtr, filename);
				if (data)
				{
					// This function makes a deep copy, so we need to destroy
					// the data.
					bp.addSolid(data);
					delete data;
				}
			}

			// Loop over Joints.
			nodePtr = NULL;
			while ((nodePtr = rootElement->IterateChildren("Joint", nodePtr)) != NULL)
			{
				JointData * data = loadJoint(nodePtr, bp, filename);
				if (data != NULL)
				{
					// This function makes a deep copy, so we need to destroy
					// the data.
					bp.addJoint(data);
					delete data;
				}
			}

			// Loop over Motors.
			nodePtr = NULL;
			while ((nodePtr = rootElement->IterateChildren("Motor", nodePtr)) != NULL)
			{
				MotorData * data = loadMotor(nodePtr, bp, filename);
				if (data != NULL)
				{
					// This function makes a deep copy, so we need to destroy
					// the data.
					bp.addMotor(data);
					delete data;
				}
			}

			// Loop over Sensors.
			nodePtr = NULL;
			while ((nodePtr = rootElement->IterateChildren(
			                        "Sensor", nodePtr)) != NULL)
			{
				SensorData * data = loadSensor(nodePtr, bp, filename);
				if (data != NULL)
				{
					// This function makes a deep copy, so we need to destroy
					// the data.
					bp.addSensor(data);
					delete data;
				}
			}

			// Finalized the Blueprint.
			bp.finalize();
		}

		SolidData* BlueprintManager::loadSolid(const TiXmlNode* nodePtr,
		                                        const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			SolidData * data = new SolidData();

			TiXmlNode* paramNodePtr = NULL;

			// Load Name element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Name");
			if (NULL != paramNodePtr)
			{
				data->name = getAttributeString(paramNodePtr, "value");
			}

			// Load Enabled element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Enabled");
			if (NULL != paramNodePtr)
			{
				string value = getAttributeString(paramNodePtr, "value");
				if ("true" == value)
				{
					data->enabled = true;
				}
				else if ("false" == value)
				{
					data->enabled = false;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadSolid: Invalid value "
					<< value << " for Solid Enabled parameter in "
					<< filename << " will be ignored." << endl;
				}
			}

			// Load Static element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Static");
			if (NULL != paramNodePtr)
			{
				string value = getAttributeString(paramNodePtr, "value");
				if ("true" == value)
				{
					data->isStatic = true;
				}
				else if ("false" == value)
				{
					data->isStatic = false;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadSolid: Invalid value "
					<< value << " for Solid Static parameter in "
					<< filename << " will be ignored." << endl;
				}
			}

			// Load Sleeping element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Sleeping");
			if (NULL != paramNodePtr)
			{
				string value = getAttributeString(paramNodePtr, "value");
				if ("true" == value)
				{
					data->sleeping = true;
				}
				else if ("false" == value)
				{
					data->sleeping = false;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadSolid: Invalid value "
					<< value << " for Solid Sleeping parameter in "
					<< filename << " will be ignored." << endl;
				}
			}

			// Load Sleepiness element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Sleepiness");
			if (NULL != paramNodePtr)
			{
				data->sleepiness = getAttributeReal(paramNodePtr, "value");
			}

			// Load LinearDamping element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("LinearDamping");
			if (NULL != paramNodePtr)
			{
				data->linearDamping = getAttributeReal(paramNodePtr,
				                                        "value");
			}

			// Load AngularDamping element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("AngularDamping");
			if (NULL != paramNodePtr)
			{
				data->angularDamping = getAttributeReal(paramNodePtr,
				                       "value");
			}

			// Load LinearVelocity element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("LinearVelocity");
			if (NULL != paramNodePtr)
			{
				data->globalLinearVel[0] =
				    getAttributeReal(paramNodePtr, "x");
				data->globalLinearVel[1] =
				    getAttributeReal(paramNodePtr, "y");
				data->globalLinearVel[2] =
				    getAttributeReal(paramNodePtr, "z");
			}

			// Load AngularVelocity element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("AngularVelocity");
			if (NULL != paramNodePtr)
			{
				data->globalAngularVel[0] =
				    getAttributeReal(paramNodePtr, "x");
				data->globalAngularVel[1] =
				    getAttributeReal(paramNodePtr, "y");
				data->globalAngularVel[2] =
				    getAttributeReal(paramNodePtr, "z");
			}

			// Load Offset element if it exists.
			TiXmlNode* offsetNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Offset");
			if (NULL != offsetNodePtr)
			{
				loadOffset(data->transform, offsetNodePtr, filename);
			}

			// Loop over Shapes if they exist.
			TiXmlNode* shapeNodePtr = NULL;
			while ((shapeNodePtr = const_cast<TiXmlNode*>
			                         (nodePtr)->IterateChildren("Shape", shapeNodePtr)) != NULL)
			{
				ShapeData * shapeData = loadShape(shapeNodePtr, filename);
				if (shapeData != NULL)
				{
					data->addShape(*shapeData);
				}
			}

			return data;
		}

		JointData* BlueprintManager::loadJoint(const TiXmlNode* nodePtr,
		                                        const Blueprint& bp, const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			JointData * data = new JointData();

			TiXmlNode* paramNodePtr = NULL;

			// Load Name element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Name");
			if (NULL != paramNodePtr)
			{
				data->name = getAttributeString(paramNodePtr, "value");
			}

			// Load Type element if it exists.
			string type;
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Type");
			if (NULL != paramNodePtr)
			{
				type = getAttributeString(paramNodePtr, "value");
			}

			if ("hinge" == type)
			{
				data->setType(HINGE_JOINT);
			}
			else if ("universal" == type)
			{
				data->setType(UNIVERSAL_JOINT);
			}
			else if ("ball" == type)
			{
				data->setType(BALL_JOINT);
			}
			else if ("slider" == type)
			{
				data->setType(SLIDER_JOINT);
			}
			else if ("wheel" == type)
			{
				data->setType(WHEEL_JOINT);
			}
			else if ("fixed" == type)
			{
				data->setType(FIXED_JOINT);
			}
			else
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadJoint: Invalid Joint type "
				<< type << " in " << filename
				<< ". Ignoring the Joint." << endl;
				delete data;
				return NULL;
			}

			// Load References element.  If it doesn't exist, ignore this
			// whole Joint.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("References");
			if (NULL != paramNodePtr)
			{
				// Get the Solids' names.
				data->solid0BlueprintRefName = getAttributeString(paramNodePtr,
				                               "solid0");
				data->solid1BlueprintRefName = getAttributeString(paramNodePtr,
				                               "solid1");
			}
			else
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadJoint: Missing \
				References element in " << data->name << " in " <<
				filename << ".  Ignoring the Joint." << endl;
				delete data;
				return NULL;
			}

			// Load Enabled element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Enabled");
			if (NULL != paramNodePtr)
			{
				string value = getAttributeString(paramNodePtr, "value");
				if ("true" == value)
				{
					data->enabled = true;
				}
				else if ("false" == value)
				{
					data->enabled = false;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadJoint: Invalid value "
					<< value << " for Joint Enabled parameter in "
					<< filename << " will be ignored." << endl;
				}
			}

			// Load Anchor element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Anchor");
			if (NULL != paramNodePtr)
			{
				data->anchor[0] = getAttributeReal(paramNodePtr, "x");
				data->anchor[1] = getAttributeReal(paramNodePtr, "y");
				data->anchor[2] = getAttributeReal(paramNodePtr, "z");
			}

			// Load BreakMode element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("BreakMode");
			if (NULL != paramNodePtr)
			{
				string breakMode = getAttributeString(paramNodePtr,
				                                       "value");
				if ("unbreakable" == breakMode)
				{
					data->breakMode = UNBREAKABLE_MODE;
				}
				else if ("threshold" == breakMode)
				{
					data->breakMode = THRESHOLD_MODE;
				}
				else if ("accumulated" == breakMode)
				{
					data->breakMode = ACCUMULATED_MODE;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadShape: Invalid Joint \
					break mode " << breakMode << " in " << filename
					<< " will be ignored." << endl;
				}
			}

			// Load BreakThresh element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("BreakThresh");
			if (NULL != paramNodePtr)
			{
				data->breakThresh = getAttributeReal(paramNodePtr, "value");
			}

			// Load AccumThresh element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("AccumThresh");
			if (NULL != paramNodePtr)
			{
				data->accumThresh = getAttributeReal(paramNodePtr, "value");
			}

			// Load AccumDamage element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("AccumDamage");
			if (NULL != paramNodePtr)
			{
				data->accumDamage = getAttributeReal(paramNodePtr, "value");
			}

			// Load ContactsEnabled element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("ContactsEnabled");
			if (NULL != paramNodePtr)
			{
				string value = getAttributeString(paramNodePtr, "value");
				if ("true" == value)
				{
					data->contactsEnabled = true;
				}
				else if ("false" == value)
				{
					data->contactsEnabled = false;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadJoint: Invalid value "
					<< value << " for Joint ContactsEnabled parameter in "
					<< filename << " will be ignored." << endl;
				}
			}

			// Loop over Axis elements if they exist.
			TiXmlNode* axisNodePtr = NULL;
			while ((axisNodePtr = const_cast<TiXmlNode*>
			                        (nodePtr)->IterateChildren("Axis", axisNodePtr)) != NULL)
			{
				// Load AxisNum element.  If it doesn't exist, default to
				// Axis 0.
				int axisNum = 0;
				TiXmlNode* axisParamNodePtr =
				    const_cast<TiXmlNode*>(axisNodePtr)->
				    FirstChild("AxisNum");
				if (NULL != axisParamNodePtr)
				{
					axisNum =
					    (int) getAttributeReal(axisParamNodePtr, "value");

					if (axisNum < 0 || axisNum > 2)
					{
						OPAL_LOGGER("warning") <<
						"opal::BlueprintManager::loadJoint: Invalid value "
						<< axisNum << " for Joint AxisNum parameter \
						in " << filename << " will be ignored." << endl;
						axisNum = 0;
					}
				}

				// Load Direction element if it exists.
				axisParamNodePtr = const_cast<TiXmlNode*>(axisNodePtr)->
				                   FirstChild("Direction");
				if (NULL != axisParamNodePtr)
				{
					data->axis[axisNum].direction[0] =
					    getAttributeReal(axisParamNodePtr, "x");
					data->axis[axisNum].direction[1] =
					    getAttributeReal(axisParamNodePtr, "y");
					data->axis[axisNum].direction[2] =
					    getAttributeReal(axisParamNodePtr, "z");
				}

				// Load LimitsEnabled element if it exists.
				axisParamNodePtr = const_cast<TiXmlNode*>(axisNodePtr)->
				                   FirstChild("LimitsEnabled");
				if (NULL != axisParamNodePtr)
				{
					string value =
					    getAttributeString(axisParamNodePtr, "value");
					if ("true" == value)
					{
						data->axis[axisNum].limitsEnabled = true;
					}
					else if ("false" == value)
					{
						data->axis[axisNum].limitsEnabled = false;
					}
					else
					{
						OPAL_LOGGER("warning") <<
						"opal::BlueprintManager::loadJoint: Invalid value "
						<< value << " for Joint Axis LimitsEnabled \
						parameter in " << filename << " will be ignored."
						<< endl;
					}
				}

				// Load LimitLow element if it exists.
				axisParamNodePtr = const_cast<TiXmlNode*>(axisNodePtr)->
				                   FirstChild("LimitLow");
				if (NULL != axisParamNodePtr)
				{
					data->axis[axisNum].limits.low =
					    getAttributeReal(axisParamNodePtr, "value");
				}

				// Load LimitHigh element if it exists.
				axisParamNodePtr = const_cast<TiXmlNode*>(axisNodePtr)->
				                   FirstChild("LimitHigh");
				if (NULL != axisParamNodePtr)
				{
					data->axis[axisNum].limits.high =
					    getAttributeReal(axisParamNodePtr, "value");
				}

				// Load LimitHardness element if it exists.
				axisParamNodePtr = const_cast<TiXmlNode*>(axisNodePtr)->
				                   FirstChild("LimitHardness");
				if (NULL != axisParamNodePtr)
				{
					data->axis[axisNum].limits.hardness =
					    getAttributeReal(axisParamNodePtr, "value");
				}

				// Load LimitBounciness element if it exists.
				axisParamNodePtr = const_cast<TiXmlNode*>(axisNodePtr)->
				                   FirstChild("LimitBounciness");
				if (NULL != axisParamNodePtr)
				{
					data->axis[axisNum].limits.bounciness =
					    getAttributeReal(axisParamNodePtr, "value");
				}
			}

			return data;
		}

		MotorData* BlueprintManager::loadMotor(const TiXmlNode* nodePtr,
		                                        const Blueprint& bp, const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			MotorData * data = NULL;

			TiXmlNode* paramNodePtr = NULL;

			// Load Type element if it exists.
			string type;
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Type");
			if (NULL != paramNodePtr)
			{
				type = getAttributeString(paramNodePtr, "value");
			}

			// Load data for the specific type of Motor.
			if ("attractor" == type)
			{
				data = loadAttractorMotor(nodePtr, bp, filename);
			}
			else if ("geared" == type)
			{
				data = loadGearedMotor(nodePtr, bp, filename);
			}
			else if ("servo" == type)
			{
				data = loadServoMotor(nodePtr, bp, filename);
			}
			else if ("spring" == type)
			{
				data = loadSpringMotor(nodePtr, bp, filename);
			}
			else if ("thruster" == type)
			{
				data = loadThrusterMotor(nodePtr, bp, filename);
			}
			else
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadMotor: Invalid Motor type "
				<< type << " in " << filename
				<< ". Ignoring the Motor." << endl;
				return NULL;
			}

			// Load Name element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Name");
			if (NULL != paramNodePtr)
			{
				data->name = getAttributeString(paramNodePtr, "value");
			}

			// Load Enabled element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Enabled");
			if (NULL != paramNodePtr)
			{
				string value = getAttributeString(paramNodePtr, "value");
				if ("true" == value)
				{
					data->enabled = true;
				}
				else if ("false" == value)
				{
					data->enabled = false;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadMotor: Invalid value "
					<< value << " for Motor Enabled parameter in "
					<< filename << " will be ignored." << endl;
				}
			}

			return data;
		}

		AttractorMotorData* BlueprintManager::loadAttractorMotor(
		    const TiXmlNode* nodePtr, const Blueprint& bp,
		    const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			AttractorMotorData * data = new AttractorMotorData();

			TiXmlNode* paramNodePtr = NULL;

			// Load Strength element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Strength");
			if (NULL != paramNodePtr)
			{
				data->strength = getAttributeReal(paramNodePtr, "value");
			}

			// Load Exponent element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Exponent");
			if (NULL != paramNodePtr)
			{
				data->exponent = getAttributeReal(paramNodePtr, "value");
			}

			// Load References element.  If it doesn't exist, or if there
			// are problems finding the references, ignore this whole Motor.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("References");
			if (NULL != paramNodePtr)
			{
				// Get the Solids' names.
				data->solid0BlueprintRefName = getAttributeString(paramNodePtr,
				                               "solid0");
				data->solid1BlueprintRefName = getAttributeString(paramNodePtr,
				                               "solid1");
			}
			else
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadAttractorMotor: Missing "
				<< "References element in " << data->name << " in " <<
				filename << ".  Ignoring the Motor." << endl;
				delete data;
				return NULL;
			}

			return data;
		}

		GearedMotorData* BlueprintManager::loadGearedMotor(
		    const TiXmlNode* nodePtr, const Blueprint& bp,
		    const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			GearedMotorData * data = new GearedMotorData();

			TiXmlNode* paramNodePtr = NULL;

			// Load JointAxisNum element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("JointAxisNum");
			if (NULL != paramNodePtr)
			{
				data->jointAxisNum =
				    (int) getAttributeReal(paramNodePtr, "value");

				if (data->jointAxisNum < 0 || data->jointAxisNum > 2)
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadGearedMotor: Invalid "
					<< "value " << data->jointAxisNum << " for "
					<< "GearedMotor JointAxisNum parameter in "
					<< filename << " will be ignored." << endl;
					data->jointAxisNum = 0;
				}
			}

			// Load MaxTorque element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("MaxTorque");
			if (NULL != paramNodePtr)
			{
				data->maxTorque = getAttributeReal(paramNodePtr, "value");
			}

			// Load MaxVelocity element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("MaxVelocity");
			if (NULL != paramNodePtr)
			{
				data->maxVelocity = getAttributeReal(paramNodePtr, "value");
			}

			// Load Throttle element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Throttle");
			if (NULL != paramNodePtr)
			{
				data->throttle = getAttributeReal(paramNodePtr, "value");
			}

			// Load References element.  If it doesn't exist, or if there
			// are problems finding the references, ignore this whole Motor.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("References");
			if (NULL != paramNodePtr)
			{
				// Get the Joint's name.
				data->jointBlueprintRefName = getAttributeString(paramNodePtr,
				                              "joint");
			}
			else
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadGearedMotor: Missing "
				<< "References element in " << data->name << " in " <<
				filename << ".  Ignoring the Motor." << endl;
				delete data;
				return NULL;
			}

			return data;
		}

		ServoMotorData* BlueprintManager::loadServoMotor(
		    const TiXmlNode* nodePtr, const Blueprint& bp,
		    const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			ServoMotorData * data = new ServoMotorData();

			TiXmlNode* paramNodePtr = NULL;

			// Load JointAxisNum element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("JointAxisNum");
			if (NULL != paramNodePtr)
			{
				data->jointAxisNum =
				    (int) getAttributeReal(paramNodePtr, "value");

				if (data->jointAxisNum < 0 || data->jointAxisNum > 2)
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadGearedMotor: Invalid "
					<< "value " << data->jointAxisNum << " for "
					<< "GearedMotor JointAxisNum parameter in "
					<< filename << " will be ignored." << endl;
					data->jointAxisNum = 0;
				}
			}

			// Load Mode element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Mode");
			if (NULL != paramNodePtr)
			{
				string mode = getAttributeString(paramNodePtr,
				                                  "value");
				if ("desiredAngle" == mode)
				{
					data->mode = DESIRED_ANGLE_MODE;
				}
				else if ("desiredVel" == mode)
				{
					data->mode = DESIRED_VEL_MODE;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadServoMotor: Invalid "
					<< "Mode " << mode << " in " << data->name <<
					" in " << filename << " will be ignored."
					<< endl;
				}
			}

			// Load DesiredAngle element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("DesiredAngle");
			if (NULL != paramNodePtr)
			{
				data->desiredAngle = getAttributeReal(paramNodePtr, "value");
			}

			// Load DesiredVel element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("DesiredVel");
			if (NULL != paramNodePtr)
			{
				data->desiredVel = getAttributeReal(paramNodePtr, "value");
			}

			// Load MaxTorque element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("MaxTorque");
			if (NULL != paramNodePtr)
			{
				data->maxTorque = getAttributeReal(paramNodePtr, "value");
			}

			// Load RestoreSpeed element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("RestoreSpeed");
			if (NULL != paramNodePtr)
			{
				data->restoreSpeed = getAttributeReal(paramNodePtr, "value");
			}

			// Load References element.  If it doesn't exist, or if there
			// are problems finding the references, ignore this whole Motor.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("References");
			if (NULL != paramNodePtr)
			{
				// Get the Joint's name.
				data->jointBlueprintRefName = getAttributeString(paramNodePtr,
				                              "joint");
			}
			else
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadServoMotor: Missing "
				<< "References element in " << data->name << " in " <<
				filename << ".  Ignoring the Motor." << endl;
				delete data;
				return NULL;
			}

			return data;
		}

		SpringMotorData* BlueprintManager::loadSpringMotor(
		    const TiXmlNode* nodePtr, const Blueprint& bp,
		    const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			SpringMotorData * data = new SpringMotorData();

			TiXmlNode* paramNodePtr = NULL;

			// Load Mode element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Mode");
			if (NULL != paramNodePtr)
			{
				string mode = getAttributeString(paramNodePtr,
				                                  "value");
				if ("linear" == mode)
				{
					data->mode = LINEAR_MODE;
				}
				else if ("angular" == mode)
				{
					data->mode = ANGULAR_MODE;
				}
				else if ("linearAndAngular" == mode)
				{
					data->mode = LINEAR_AND_ANGULAR_MODE;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadSpringMotor: Invalid "
					" Mode " << mode << " in " << data->name
					<< " in " << filename << " will be ignored."
					<< endl;
				}
			}

			// Load AttachOffset element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("AttachOffset");
			if (NULL != paramNodePtr)
			{
				data->attachOffset[0] = getAttributeReal(paramNodePtr, "x");
				data->attachOffset[1] = getAttributeReal(paramNodePtr, "y");
				data->attachOffset[2] = getAttributeReal(paramNodePtr, "z");
			}

			// Load DesiredPos element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("DesiredPos");
			if (NULL != paramNodePtr)
			{
				data->desiredPos[0] = getAttributeReal(paramNodePtr, "x");
				data->desiredPos[1] = getAttributeReal(paramNodePtr, "y");
				data->desiredPos[2] = getAttributeReal(paramNodePtr, "z");
			}

			// Load DesiredForward element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("DesiredForward");
			if (NULL != paramNodePtr)
			{
				data->desiredForward[0] = getAttributeReal(paramNodePtr, "x");
				data->desiredForward[1] = getAttributeReal(paramNodePtr, "y");
				data->desiredForward[2] = getAttributeReal(paramNodePtr, "z");
			}

			// Load DesiredUp element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("DesiredUp");
			if (NULL != paramNodePtr)
			{
				data->desiredUp[0] = getAttributeReal(paramNodePtr, "x");
				data->desiredUp[1] = getAttributeReal(paramNodePtr, "y");
				data->desiredUp[2] = getAttributeReal(paramNodePtr, "z");
			}

			// Load DesiredRight element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("DesiredRight");
			if (NULL != paramNodePtr)
			{
				data->desiredRight[0] = getAttributeReal(paramNodePtr, "x");
				data->desiredRight[1] = getAttributeReal(paramNodePtr, "y");
				data->desiredRight[2] = getAttributeReal(paramNodePtr, "z");
			}

			// Load LinearKd element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("LinearKd");
			if (NULL != paramNodePtr)
			{
				data->linearKd = getAttributeReal(paramNodePtr, "value");
			}

			// Load LinearKs element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("LinearKs");
			if (NULL != paramNodePtr)
			{
				data->linearKs = getAttributeReal(paramNodePtr, "value");
			}

			// Load AngularKd element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("AngularKd");
			if (NULL != paramNodePtr)
			{
				data->angularKd = getAttributeReal(paramNodePtr, "value");
			}

			// Load AngularKs element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("AngularKs");
			if (NULL != paramNodePtr)
			{
				data->angularKs = getAttributeReal(paramNodePtr, "value");
			}

			// Load References element.  If it doesn't exist, or if there
			// are problems finding the references, ignore this whole Motor.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("References");
			if (NULL != paramNodePtr)
			{
				// Get the Solid's name.
				data->solidBlueprintRefName = getAttributeString(paramNodePtr,
				                              "solid");
			}
			else
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadSpringMotor: Missing "
				" References element in " << data->name << " in " <<
				filename << ".  Ignoring the Motor." << endl;
				delete data;
				return NULL;
			}

			return data;
		}

		ThrusterMotorData* BlueprintManager::loadThrusterMotor(
		    const TiXmlNode* nodePtr, const Blueprint& bp,
		    const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			ThrusterMotorData * data = new ThrusterMotorData();

			TiXmlNode* paramNodePtr = NULL;

			// Load ForceType element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("ForceType");
			if (NULL != paramNodePtr)
			{
				string forceType = getAttributeString(paramNodePtr,
				                                       "value");
				if ("localForce" == forceType)
				{
					data->force.type = LOCAL_FORCE;
				}
				else if ("globalForce" == forceType)
				{
					data->force.type = GLOBAL_FORCE;
				}
				else if ("localTorque" == forceType)
				{
					data->force.type = LOCAL_TORQUE;
				}
				else if ("globalTorque" == forceType)
				{
					data->force.type = GLOBAL_TORQUE;
				}
				else if ("localForceAtLocalPos" == forceType)
				{
					data->force.type = LOCAL_FORCE_AT_LOCAL_POS;
				}
				else if ("localForceAtGlobalPos" == forceType)
				{
					data->force.type = LOCAL_FORCE_AT_GLOBAL_POS;
				}
				else if ("globalForceAtLocalPos" == forceType)
				{
					data->force.type = GLOBAL_FORCE_AT_LOCAL_POS;
				}
				else if ("globalForceAtGlobalPos" == forceType)
				{
					data->force.type = GLOBAL_FORCE_AT_GLOBAL_POS;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadThrusterMotor: Invalid "
					<< "Force type " << forceType << " in " << data->name
					<< " in " << filename << " will be ignored."
					<< endl;
				}
			}

			// Load ForceVec element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("ForceVec");
			if (NULL != paramNodePtr)
			{
				data->force.vec[0] = getAttributeReal(paramNodePtr, "x");
				data->force.vec[1] = getAttributeReal(paramNodePtr, "y");
				data->force.vec[2] = getAttributeReal(paramNodePtr, "z");
			}

			// Load ForcePos element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("ForcePos");
			if (NULL != paramNodePtr)
			{
				data->force.pos[0] = getAttributeReal(paramNodePtr, "x");
				data->force.pos[1] = getAttributeReal(paramNodePtr, "y");
				data->force.pos[2] = getAttributeReal(paramNodePtr, "z");
			}

			// Load References element.  If it doesn't exist, or if there
			// are problems finding the references, ignore this whole Motor.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("References");
			if (NULL != paramNodePtr)
			{
				// Get the Solid's name.
				data->solidBlueprintRefName = getAttributeString(paramNodePtr,
				                              "solid");
			}
			else
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadSolidMotor: Missing "
				<< "References element in " << data->name << " in " <<
				filename << ".  Ignoring the Motor." << endl;
				delete data;
				return NULL;
			}

			return data;
		}

		SensorData* BlueprintManager::loadSensor(const TiXmlNode* nodePtr,
		        const Blueprint& bp, const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			SensorData * data = NULL;

			TiXmlNode* paramNodePtr = NULL;

			// Load Type element if it exists.
			string type;
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Type");
			if (NULL != paramNodePtr)
			{
				type = getAttributeString(paramNodePtr, "value");
			}

			// Load data for the specific type of Sensor.
			if ("acceleration" == type)
			{
				data = loadAccelerationSensor(nodePtr, bp, filename);
			}
			else if ("incline" == type)
			{
				data = loadInclineSensor(nodePtr, bp, filename);
			}
			else if ("raycast" == type)
			{
				data = loadRaycastSensor(nodePtr, bp, filename);
			}
			else if ("volume" == type)
			{
				data = loadVolumeSensor(nodePtr, bp, filename);
			}
			else
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadSensor: Invalid Sensor \
				type " << type << " in " << filename
				<< ". Ignoring the Sensor." << endl;
				return NULL;
			}

			// Load Name element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Name");
			if (NULL != paramNodePtr)
			{
				data->name = getAttributeString(paramNodePtr, "value");
			}

			// Load Enabled element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Enabled");
			if (NULL != paramNodePtr)
			{
				string value = getAttributeString(paramNodePtr, "value");
				if ("true" == value)
				{
					data->enabled = true;
				}
				else if ("false" == value)
				{
					data->enabled = false;
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadSensor: Invalid value "
					<< value << " for Sensor Enabled parameter in "
					<< filename << " will be ignored." << endl;
				}
			}

			// Load Offset element if it exists.
			TiXmlNode* offsetNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Offset");
			if (NULL != offsetNodePtr)
			{
				loadOffset(data->transform, offsetNodePtr, filename);
			}

			// Load References element.  If it doesn't exist, just assume
			// that this Sensor will not be attached to a Solid.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("References");
			if (NULL != paramNodePtr)
			{
				// Get the Solids' names.
				data->solidBlueprintRefName = getAttributeString(paramNodePtr,
				                              "solid");
			}
			else
			{
				data->solidBlueprintRefName = "";
			}

			return data;
		}

		AccelerationSensorData* BlueprintManager::loadAccelerationSensor(
		    const TiXmlNode* nodePtr, const Blueprint& bp,
		    const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			AccelerationSensorData * data = new AccelerationSensorData();

			// Nothing special to load for AccelerationSensors.

			return data;
		}

		InclineSensorData* BlueprintManager::loadInclineSensor(
		    const TiXmlNode* nodePtr, const Blueprint& bp,
		    const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			InclineSensorData * data = new InclineSensorData();

			TiXmlNode* paramNodePtr = NULL;

			// Load Ray element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Axis");
			if (NULL != paramNodePtr)
			{
				Vec3r axis;
				axis[0] = getAttributeReal(paramNodePtr, "x");
				axis[1] = getAttributeReal(paramNodePtr, "y");
				axis[2] = getAttributeReal(paramNodePtr, "z");
				data->axis = axis;
			}

			return data;
		}

		RaycastSensorData* BlueprintManager::loadRaycastSensor(
		    const TiXmlNode* nodePtr, const Blueprint& bp,
		    const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			RaycastSensorData * data = new RaycastSensorData();

			TiXmlNode* paramNodePtr = NULL;

			// Load Ray element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("Ray");
			if (NULL != paramNodePtr)
			{
				Point3r origin;
				Vec3r dir;
				origin[0] = getAttributeReal(paramNodePtr, "originx");
				origin[1] = getAttributeReal(paramNodePtr, "originy");
				origin[2] = getAttributeReal(paramNodePtr, "originz");
				dir[0] = getAttributeReal(paramNodePtr, "dirx");
				dir[1] = getAttributeReal(paramNodePtr, "diry");
				dir[2] = getAttributeReal(paramNodePtr, "dirz");
				data->ray.setOrigin(origin);
				data->ray.setDir(dir);
			}

			// Load ContactGroup element if it exists.
			paramNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("ContactGroup");
			if (NULL != paramNodePtr)
			{
				data->contactGroup =
				    (unsigned int) getAttributeReal(
				        paramNodePtr, "value");
			}

			return data;
		}

		VolumeSensorData* BlueprintManager::loadVolumeSensor(
		    const TiXmlNode* nodePtr, const Blueprint& bp,
		    const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			VolumeSensorData * data = new VolumeSensorData();

			// Nothing special to load for VolumeSensors.

			return data;
		}

		ShapeData* BlueprintManager::loadShape(const TiXmlNode* nodePtr,
		                                        const string& filename)
		{
			// This data structure automatically gets initialized to
			// default values in its constructor.
			ShapeData * data = NULL;

			string type = getAttributeString(nodePtr, "type");

			// Allocate the specific ShapeData type, then load its
			// dimensions.
			TiXmlNode* dimensionsNodePtr = const_cast<TiXmlNode*>
			                               (nodePtr)->FirstChild("Dimensions");

			if ("box" == type)
			{
				BoxShapeData * boxData = new BoxShapeData();
				data = boxData;
				boxData->dimensions[0] =
				    getAttributeReal(dimensionsNodePtr, "x");
				boxData->dimensions[1] =
				    getAttributeReal(dimensionsNodePtr, "y");
				boxData->dimensions[2] =
				    getAttributeReal(dimensionsNodePtr, "z");
			}
			else if ("sphere" == type)
			{
				SphereShapeData * sphereData = new SphereShapeData();
				data = sphereData;
				sphereData->radius =
				    getAttributeReal(dimensionsNodePtr, "radius");
			}
			else if ("capsule" == type)
			{
				CapsuleShapeData * capsuleData = new CapsuleShapeData();
				data = capsuleData;
				capsuleData->radius =
				    getAttributeReal(dimensionsNodePtr, "radius");
				capsuleData->length =
				    getAttributeReal(dimensionsNodePtr, "length");
			}
			else if ("cylinder" == type)
			{
				CylinderShapeData * cylinderData = new CylinderShapeData();
				data = cylinderData;
				cylinderData->radius =
				    getAttributeReal(dimensionsNodePtr, "radius");
				cylinderData->length =
				    getAttributeReal(dimensionsNodePtr, "length");
			}
			else if ("plane" == type)
			{
				PlaneShapeData * planeData = new PlaneShapeData();
				data = planeData;
				planeData->abcd[0] = getAttributeReal(dimensionsNodePtr, "a");
				planeData->abcd[1] = getAttributeReal(dimensionsNodePtr, "b");
				planeData->abcd[2] = getAttributeReal(dimensionsNodePtr, "c");
				planeData->abcd[3] = getAttributeReal(dimensionsNodePtr, "d");
			}
			//else if ("ray" == type)
			//{
			//  RayShapeData* rayData = new RayShapeData();
			//  data = rayData;
			//  Point3r origin;
			//  origin[0] = getAttributeReal(dimensionsNodePtr, "originx");
			//  origin[1] = getAttributeReal(dimensionsNodePtr, "originy");
			//  origin[2] = getAttributeReal(dimensionsNodePtr, "originz");
			//  rayData->ray.setOrigin(origin);
			//  Vec3r dir;
			//  dir[0] = getAttributeReal(dimensionsNodePtr, "dirx");
			//  dir[1] = getAttributeReal(dimensionsNodePtr, "diry");
			//  dir[2] = getAttributeReal(dimensionsNodePtr, "dirz");
			//  rayData->ray.setDir(dir);
			//}
			// MESH_SHAPE type not supported in OPAL XML.
			else
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::loadShape: Invalid Shape type "
				<< type << " in " << filename << " will be ignored."
				<< endl;
				return NULL;
			}

			// Load Offset element if it exists.
			TiXmlNode* offsetNodePtr = const_cast<TiXmlNode*>
			                           (nodePtr)->FirstChild("Offset");
			if (NULL != offsetNodePtr)
			{
				loadOffset(data->offset, offsetNodePtr, filename);
			}

			// Load Material element if it exists.
			TiXmlNode* materialNodePtr = const_cast<TiXmlNode*>
			                             (nodePtr)->FirstChild("Material");
			if (NULL != materialNodePtr)
			{
				data->material.hardness =
				    getAttributeReal(materialNodePtr, "hardness");
				data->material.friction =
				    getAttributeReal(materialNodePtr, "friction");
				data->material.bounciness =
				    getAttributeReal(materialNodePtr, "bounciness");
				data->material.density =
				    getAttributeReal(materialNodePtr, "density");
			}

			// Load ContactGroup element if it exists.
			TiXmlNode* contactGroupNodePtr =
			    const_cast<TiXmlNode*>(nodePtr)->FirstChild("ContactGroup");
			if (NULL != contactGroupNodePtr)
			{
				data->contactGroup =
				    (unsigned int) getAttributeReal(
				        contactGroupNodePtr, "value");
			}

			return data;
		}

		void BlueprintManager::loadOffset(Matrix44r& offset,
		                                   const TiXmlNode* nodePtr, const string& filename)
		{
			// Loop over all Transform sub-elements.
			TiXmlNode * transformNodePtr = NULL;
			while ((transformNodePtr = const_cast<TiXmlNode*>
			                             (nodePtr)->IterateChildren("Transform", transformNodePtr)) != NULL)
			{
				string type = getAttributeString(transformNodePtr,
				                                  "type");

				if ("translate" == type)
				{
					real x = getAttributeReal(transformNodePtr, "x");
					real y = getAttributeReal(transformNodePtr, "y");
					real z = getAttributeReal(transformNodePtr, "z");
					offset.translate(x, y, z);
				}
				else if ("rotate" == type)
				{
					real angle = getAttributeReal(transformNodePtr, "angle");
					real x = getAttributeReal(transformNodePtr, "x");
					real y = getAttributeReal(transformNodePtr, "y");
					real z = getAttributeReal(transformNodePtr, "z");
					offset.rotate(angle, x, y, z);
				}
				else if ("euler" == type)
				{
					real x = getAttributeReal(transformNodePtr, "x");
					real y = getAttributeReal(transformNodePtr, "y");
					real z = getAttributeReal(transformNodePtr, "z");

					//Vec3r v(x, y, z);
					//real angle = v.length();
					//if (0 != angle)
					//{
					//  v.normalize();
					//  offset.rotate(angle, v[0], v[1], v[2]);
					//}

					offset.rotate(z, 0, 0, 1);
					offset.rotate(y, 0, 1, 0);
					offset.rotate(x, 1, 0, 0);
				}
				else if ("matrix" == type)
				{
					real data[16];
					data[0] = getAttributeReal(transformNodePtr, "_00");
					data[1] = getAttributeReal(transformNodePtr, "_01");
					data[2] = getAttributeReal(transformNodePtr, "_02");
					data[3] = getAttributeReal(transformNodePtr, "_03");
					data[4] = getAttributeReal(transformNodePtr, "_10");
					data[5] = getAttributeReal(transformNodePtr, "_11");
					data[6] = getAttributeReal(transformNodePtr, "_12");
					data[7] = getAttributeReal(transformNodePtr, "_13");
					data[8] = getAttributeReal(transformNodePtr, "_20");
					data[9] = getAttributeReal(transformNodePtr, "_21");
					data[10] = getAttributeReal(transformNodePtr, "_22");
					data[11] = getAttributeReal(transformNodePtr, "_23");
					data[12] = getAttributeReal(transformNodePtr, "_30");
					data[13] = getAttributeReal(transformNodePtr, "_31");
					data[14] = getAttributeReal(transformNodePtr, "_32");
					data[15] = getAttributeReal(transformNodePtr, "_33");
					offset.set(data);
				}
				else
				{
					OPAL_LOGGER("warning") <<
					"opal::BlueprintManager::loadOffset: Invalid "
					<< "transform type " << type << " in " << filename
					<< endl;
				}
			}
		}

		real BlueprintManager::getAttributeReal(const TiXmlNode* nodePtr,
		        const string& name) const
		{
			TiXmlElement * elementPtr =
			    const_cast<TiXmlNode*>(nodePtr)->ToElement();
			double temp = 0;

			if (NULL == elementPtr->Attribute(name.c_str(), &temp))
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::getAttributeReal: Element "
				<< nodePtr->Value() << " is missing attribute '"
				<< name << "'.  Parameter will be set to 0.0."
				<< endl;
				return 0;
			}
			else
			{
				return (real) temp;
			}
		}

		string BlueprintManager::getAttributeString(
		    const TiXmlNode* nodePtr, const string& name) const
		{
			TiXmlElement * elementPtr =
			    const_cast<TiXmlNode*>(nodePtr)->ToElement();
			const char* temp = elementPtr->Attribute(name.c_str());

			if (NULL == temp)
			{
				OPAL_LOGGER("warning") <<
				"opal::BlueprintManager::getAttributeString: Element "
				<< nodePtr->Value() << " is missing attribute '"
				<< name << "'.  Parameter will be set to \"\"."
				<< endl;
				return "";
			}
			else
			{
				return temp;
			}
		}
	}
}
