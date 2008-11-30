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

#ifndef OPAL_BLUEPRINT_INSTANCE_H
#define OPAL_BLUEPRINT_INSTANCE_H

#include "Defines.h"

namespace opal
{
    class Solid;
    class Joint;
    class Motor;
    class Sensor;

    /// After instantiating a Blueprint, this container class holds
    /// pointers to the objects in the Blueprint with names.  Pointers
    /// to specific objects can then be requested in two ways: 1) by name,
    /// which is useful for finding a specific object, or 2) by index,
    /// which is useful for interating through every object.
    class BlueprintInstance
    {
        public:
            OPAL_DECL BlueprintInstance();

            OPAL_DECL virtual ~BlueprintInstance();

            /// Finds a Solid by name and returns its pointer.  Returns NULL if
            /// the name is not found.
            OPAL_DECL virtual Solid* OPAL_CALL getSolid(
                const std::string& name) const;

            /// Finds a Joint by name and returns its pointer.  Returns NULL if
            /// the name is not found.
            OPAL_DECL virtual Joint* OPAL_CALL getJoint(
                const std::string& name) const;

            /// Finds a Motor by name and returns its pointer.  Returns NULL if
            /// the name is not found.
            OPAL_DECL virtual Motor* OPAL_CALL getMotor(
                const std::string& name) const;

            /// Finds a Sensor by name and returns its pointer.  Returns NULL if
            /// the name is not found.
            OPAL_DECL virtual Sensor* OPAL_CALL getSensor(
                const std::string& name) const;

            /// Returns the number of Solids in this BlueprintInstance.
            OPAL_DECL virtual unsigned int OPAL_CALL getNumSolids() const;

            /// Returns the number of Joints in this BlueprintInstance.
            OPAL_DECL virtual unsigned int OPAL_CALL getNumJoints() const;

            /// Returns the number of Motors in this BlueprintInstance.
            OPAL_DECL virtual unsigned int OPAL_CALL getNumMotors() const;

            /// Returns the number of Sensors in this BlueprintInstance.
            OPAL_DECL virtual unsigned int OPAL_CALL getNumSensors() const;

            //! Finds a Solid by index and returns its pointer.
            /*!
             * @note this changes the index order
             */
            OPAL_DECL virtual Solid * OPAL_CALL getSolid(unsigned int i) const;

            //! Finds a Joint by index and returns its pointer.
            /*!
             * @note this changes the index order
             */
            OPAL_DECL virtual Joint * OPAL_CALL getJoint(unsigned int i) const;

            //! Finds a Motor by index and returns its pointer.
            /*!
             * @note this changes the index order
             */
            OPAL_DECL virtual Motor * OPAL_CALL getMotor(unsigned int i) const;

            //! Finds a Sensor by index and returns its pointer.
            /*!
             * @note this changes the index order
             */
            OPAL_DECL virtual Sensor * OPAL_CALL getSensor(unsigned int i) const;

            //! Removes but does not delete the solid.
            /*!
             * @note make sure i is in the range of solid list
             */
            OPAL_DECL virtual Solid * OPAL_CALL detachSolid(unsigned int i);

            /// Detachs a Joint by index and returns its pointer.
            OPAL_DECL virtual Joint* OPAL_CALL detachJoint(unsigned int i);

            /// Detachs a Motor by index and returns its pointer.
            OPAL_DECL virtual Motor* OPAL_CALL detachMotor(unsigned int i);

            /// Detachs a Sensor by index and returns its pointer.
            OPAL_DECL virtual Sensor* OPAL_CALL detachSensor(unsigned int i);

            //! Removes but does not delete the solid.
            /*!
             * @note make sure solid is stored here
             */
            OPAL_DECL virtual Solid * OPAL_CALL detachSolid(const std::string & name);

            /// Adds a new Solid pointer to the list.  If the pointer is
            /// non-NULL and the object's name is not an empty string, this
            /// also adds the pointer to the Solid map.
            OPAL_DECL virtual void OPAL_CALL addSolid(Solid* s);

            /// Adds a new Joint pointer.  If the pointer is
            /// non-NULL and the object's name is not an empty string, this
            /// also adds the pointer to the Joint map.
            OPAL_DECL virtual void OPAL_CALL addJoint(Joint* j);

            /// Adds a new Motor pointer.  If the pointer is
            /// non-NULL and the object's name is not an empty string, this
            /// also adds the pointer to the Motor map.
            OPAL_DECL virtual void OPAL_CALL addMotor(Motor* m);

            /// Adds a new Sensor pointer.  If the pointer is
            /// non-NULL and the object's name is not an empty string, this
            /// also adds the pointer to the Sensor map.
            OPAL_DECL virtual void OPAL_CALL addSensor(Sensor* s);

        private:
            /// Map of Solid names to Solid pointers.
            std::map<std::string, Solid*> mSolidMap;

            /// Map of Joint names to Joint pointers.
            std::map<std::string, Joint*> mJointMap;

            /// Map of Motor names to Motor pointers.
            std::map<std::string, Motor*> mMotorMap;

            /// Map of Sensor names to Sensor pointers.
            std::map<std::string, Sensor*> mSensorMap;

            /// List of Solid pointers.
            std::vector<Solid*> mSolidList;

            /// List of Joint pointers.
            std::vector<Joint*> mJointList;

            /// List of Motor pointers.
            std::vector<Motor*> mMotorList;

            /// List of Sensor pointers.
            std::vector<Sensor*> mSensorList;
    };
}

#endif
