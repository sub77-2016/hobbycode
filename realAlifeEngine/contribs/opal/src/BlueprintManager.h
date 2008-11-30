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

#ifndef OPAL_BLUEPRINT_MANAGER_H
#define OPAL_BLUEPRINT_MANAGER_H

#include "Defines.h"
#include "Singleton.h"
#include "Blueprint.h"

class TiXmlNode;

namespace opal
{
	class AttractorMotorData;
	class GearedMotorData;
	class ServoMotorData;
	class SpringMotorData;
	class ThrusterMotorData;
	class AccelerationSensorData;
	class InclineSensorData;
	class RaycastSensorData;
	class VolumeSensorData;

	/// A namespace for the BlueprintManager singleton implementation.
	namespace blueprintManagerImpl
	{
		class BlueprintManager
		{
		public:
			BlueprintManager();

			virtual ~BlueprintManager();

			/// Loads a Blueprint from an OPAL XML file.  The file should 
			/// contain a single OpalBlueprint element (additional ones 
			/// will be ignored).  The Blueprint can contain any number 
			/// of Solids, Joints, Motors, and Sensors.  Most of the 
			/// object parameters are optional, in which case the usual 
			/// default values will be used.  The only required 
			/// parameters are object types and references to other objects 
			/// (i.e. the "References" elements in an XML file).  For 
			/// example, a Joint refers to two Solids; if either of those 
			/// Solids are not in the file, the Joint will be ignored.  The 
			/// exception is Sensors' references to Solids; these aren't 
			/// required because Sensors don't have to be attached to a 
			/// Solid.  For a particular element that does exist in the 
			/// XML file, all attributes are always required.  
			/// All object names, if used, must be unique.
			virtual void OPAL_CALL loadFile(Blueprint& bp, 
				const std::string& filename);

			/// Saves a Blueprint to an OPAL XML file.
			//virtual void OPAL_CALL saveFile(const Blueprint& bp, 
			//	const std::string& filename);

		private:
			/// Helper function for parsing Solid XML elements.  Allocates 
			/// and returns a pointer to a new SolidData object.  Returns 
			/// NULL if the element could not be loaded.
			SolidData* loadSolid(const TiXmlNode* nodePtr, 
				const std::string& filename);

			/// Helper function for parsing Joint XML elements.  Allocates 
			/// and returns a pointer to a new JointData object.  Returns 
			/// NULL if the element could not be loaded.
			JointData* loadJoint(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing Motor XML elements.  Allocates 
			/// and returns a pointer to a new MotorData object.  Returns 
			/// NULL if the element could not be loaded.
			MotorData* loadMotor(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing AttractorMotor XML elements.  
			/// Allocates and returns a pointer to a new AttractorMotorData 
			/// object.  Returns NULL if the element could not be loaded.
			AttractorMotorData* loadAttractorMotor(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing GearedMotor XML elements.  
			/// Allocates and returns a pointer to a new GearedMotorData 
			/// object.  Returns NULL if the element could not be loaded.
			GearedMotorData* loadGearedMotor(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing ServoMotor XML elements.  
			/// Allocates and returns a pointer to a new ServoMotorData 
			/// object.  Returns NULL if the element could not be loaded.
			ServoMotorData* loadServoMotor(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing SpringMotor XML elements.  
			/// Allocates and returns a pointer to a new SpringMotorData 
			/// object.  Returns NULL if the element could not be loaded.
			SpringMotorData* loadSpringMotor(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing ThrusterMotor XML elements.  
			/// Allocates and returns a pointer to a new ThrusterMotorData 
			/// object.  Returns NULL if the element could not be loaded.
			ThrusterMotorData* loadThrusterMotor(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing Sensor XML elements.  Allocates 
			/// and returns a pointer to a new SensorData object.  Returns 
			/// NULL if the element could not be loaded.
			SensorData* loadSensor(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing AccelerationSensor XML elements.  
			/// Allocates and returns a pointer to a new 
			/// AccelerationSensorData object.  Returns NULL if the element 
			/// could not be loaded.
			AccelerationSensorData* loadAccelerationSensor(
				const TiXmlNode* nodePtr, const Blueprint& bp, 
				const std::string& filename);

			/// Helper function for parsing InclineSensor XML elements.  
			/// Allocates and returns a pointer to a new InclineSensorData 
			/// object.  Returns NULL if the element could not be loaded.
			InclineSensorData* loadInclineSensor(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing RaycastSensor XML elements.  
			/// Allocates and returns a pointer to a new RaycastSensorData 
			/// object.  Returns NULL if the element could not be loaded.
			RaycastSensorData* loadRaycastSensor(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing VolumeSensor XML elements.  
			/// Allocates and returns a pointer to a new VolumeSensorData 
			/// object.  Returns NULL if the element could not be loaded.
			VolumeSensorData* loadVolumeSensor(const TiXmlNode* nodePtr, 
				const Blueprint& bp, const std::string& filename);

			/// Helper function for parsing Shape XML elements.  Allocates 
			/// and returns a pointer to a new ShapeData object.  Returns 
			/// NULL if the element could not be loaded.
			ShapeData* loadShape(const TiXmlNode* nodePtr, 
				const std::string& filename);

			/// Helper function for parsing offset XML elements.
			void loadOffset(Matrix44r& offset, const TiXmlNode* nodePtr, 
				const std::string& filename);

			/// Helper function; returns 0 if the attribute doesn't 
			/// exist.
			real getAttributeReal(const TiXmlNode* nodePtr, 
				const std::string& name)const;

			/// Helper function; returns an empty string if the attribute 
			/// doesn't exist.
			std::string getAttributeString(const TiXmlNode* nodePtr, 
				const std::string& name)const;
		};
	}
	typedef Singleton<blueprintManagerImpl::BlueprintManager> BlueprintManager;
}

#endif
