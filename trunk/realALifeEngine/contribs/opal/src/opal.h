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

#ifndef OPAL_H
#define OPAL_H

#include "Simulator.h"
#include "Solid.h"
#include "SolidData.h"
#include "ShapeData.h"
#include "BoxShapeData.h"
#include "SphereShapeData.h"
#include "CapsuleShapeData.h"
#include "CylinderShapeData.h"
#include "PlaneShapeData.h"
#include "Mass.h"
#include "MeshShapeData.h"
#include "Motor.h"
#include "MotorData.h"
#include "AttractorMotor.h"
#include "AttractorMotorData.h"
#include "GearedMotor.h"
#include "GearedMotorData.h"
#include "ServoMotor.h"
#include "ServoMotorData.h"
#include "ThrusterMotor.h"
#include "ThrusterMotorData.h"
#include "VelocityMotor.h"
#include "VelocityMotorData.h"
#include "SpringMotor.h"
#include "SpringMotorData.h"
#include "Joint.h"
#include "JointData.h"
#include "Sensor.h"
#include "SensorData.h"
#include "AccelerationSensor.h"
#include "AccelerationSensorData.h"
#include "InclineSensor.h"
#include "InclineSensorData.h"
#include "RaycastSensor.h"
#include "RaycastSensorData.h"
#include "VolumeSensor.h"
#include "VolumeSensorData.h"
#include "EventHandler.h"
#include "JointBreakEventHandler.h"
#include "CollisionEventHandler.h"
#include "PostStepEventHandler.h"
#include "Blueprint.h"
#include "BlueprintInstance.h"
#include "BlueprintManager.h"
#include "Defines.h"

namespace opal
{
	/// Creates a Simulator.
	OPAL_EXPORT_FUNCTION Simulator* OPAL_CALL createSimulator();

    /// Creates a custom Simulator.
    OPAL_EXPORT_FUNCTION Simulator* OPAL_CALL createCustomSimulator(SimulatorData & data);

	/// Loads a Blueprint from an OPAL XML file.
	OPAL_EXPORT_FUNCTION void OPAL_CALL loadFile(Blueprint& bp,
		const std::string& filename);

	/// Saves a Blueprint in an OPAL XML file.
	//OPAL_EXPORT_FUNCTION void OPAL_CALL saveFile(const Blueprint& bp,
	//	const std::string& filename);
}

#endif
