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

#ifndef OPAL_BLUEPRINT_H
#define OPAL_BLUEPRINT_H

#include "Defines.h"
#include "Solid.h"
#include "Joint.h"

namespace opal
{
	class SolidData;
	class JointData;
	class MotorData;
	class SensorData;

	/// Blueprints are useful for saving and loading systems of Solids, 
	/// Joints, Motors, and/or Sensors.  They are created by OPAL when 
	/// an OPAL XML file is loaded.  They can also be created by users 
	/// and saved to an OPAL XML file.  When setting up a Blueprint 
	/// manually add all object data to the Blueprint and call finalize
	/// when finished.  Blueprints are not guaranteed to yield 
	/// perfect repeatability after a save/restore sequence.
	class Blueprint
	{
	public:
		OPAL_DECL Blueprint();

		OPAL_DECL virtual ~Blueprint();

		/// Updates references among internal objects.  This must 
		/// be called before the Blueprint can be instantiated.  This 
		/// is automatically called on Blueprints loaded from files.
		OPAL_DECL virtual void OPAL_CALL finalize();

		/// Returns true if this Blueprint has been finalized (via the 
		/// finalize function).
		OPAL_DECL virtual bool OPAL_CALL isFinalized()const;

		/// Adds a new SolidData object to the Blueprint.  Makes a 
		/// deep copy of the given data.
		OPAL_DECL virtual void OPAL_CALL addSolid(SolidData* data);

		/// Adds a new JointData object to the Blueprint.  Makes a 
		/// deep copy of the given data.
		OPAL_DECL virtual void OPAL_CALL addJoint(JointData* data);

		/// Adds a new MotorData object to the Blueprint.  Makes a 
		/// deep copy of the given data.
		OPAL_DECL virtual void OPAL_CALL addMotor(MotorData* data);

		/// Adds a new SensorData object to the Blueprint.  Makes a 
		/// deep copy of the given data.
		OPAL_DECL virtual void OPAL_CALL addSensor(SensorData* data);

		/// Returns the number of SolidData objects.
		OPAL_DECL virtual unsigned int OPAL_CALL getNumSolids()const;

		/// Returns the number of JointData objects.
		OPAL_DECL virtual unsigned int OPAL_CALL getNumJoints()const;

		/// Returns the number of MotorData objects.
		OPAL_DECL virtual unsigned int OPAL_CALL getNumMotors()const;

		/// Returns the number of SensorData objects.
		OPAL_DECL virtual unsigned int OPAL_CALL getNumSensors()const;

		/// Returns a pointer to the SolidData at the given index.
		OPAL_DECL virtual SolidData* OPAL_CALL getSolidData(
			unsigned int i)const;

		/// Returns a pointer to the JointData at the given index.
		OPAL_DECL virtual JointData* OPAL_CALL getJointData(
			unsigned int i)const;

		/// Returns a pointer to the MotorData at the given index.
		OPAL_DECL virtual MotorData* OPAL_CALL getMotorData(
			unsigned int i)const;

		/// Returns a pointer to the SensorData at the given index.
		OPAL_DECL virtual SensorData* OPAL_CALL getSensorData(
			unsigned int i)const;

	private:
		/// Helper function that returns true if a Solid of the given 
		/// name already exists in the Blueprint.  Ignores empty name 
		/// strings.
		bool solidExists(const std::string& name);

		/// Helper function that returns true if a Joint of the given 
		/// name already exists in the Blueprint.  Ignores empty name 
		/// strings.
		bool jointExists(const std::string& name);

		/// Helper function that returns true if a Motor of the given 
		/// name already exists in the Blueprint.  Ignores empty name 
		/// strings.
		bool motorExists(const std::string& name);

		/// Helper function that returns true if a Sensor of the given 
		/// name already exists in the Blueprint.  Ignores empty name 
		/// strings.
		bool sensorExists(const std::string& name);

		/// Returns the index of the given Solid name in the Blueprint's 
		/// list of SolidData.  Returns -1 if the Solid name is not found 
		/// or if the given name is an empty string.
		int getSolidIndex(const std::string& name)const;

		/// Returns the index of the given Joint name in the Blueprint's 
		/// list of JointData.  Returns -1 if the Joint name is not found 
		/// or if the given name is an empty string.
		int getJointIndex(const std::string& name)const;

		/// Prints a warning message saying that the Blueprint has already 
		/// been finalized.
		void printFinalizedMessage();

		/// True if the Blueprint has been finalized.
		bool mFinalized;

		/// The set of all Solid pointers in this Blueprint.
		std::vector<SolidData*> mSolidList;

		/// The set of all Joint pointers in this Blueprint.
		std::vector<JointData*> mJointList;

		/// The set of all Motor pointers in this Blueprint.
		std::vector<MotorData*> mMotorList;

		/// The set of all Sensor pointers in this Blueprint.
		std::vector<SensorData*> mSensorList;
	};
}

#endif
