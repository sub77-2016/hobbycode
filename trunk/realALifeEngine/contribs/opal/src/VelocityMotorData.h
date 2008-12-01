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

#ifndef OPAL_VELOCITY_MOTOR_DATA_H
#define OPAL_VELOCITY_MOTOR_DATA_H

// class headers
#include "MotorData.h"

// project headers
#include "Defines.h"

namespace opal
{
    /// A data structure describing a VelocityMotor.
    class VelocityMotorData : public MotorData
    {
        public:
            VelocityMotorData()
                    : MotorData()
            {
                mType = VELOCITY_MOTOR;
                solid = NULL;
                internal_solidIndex = 0;
                solidBlueprintRefName = "";
                velocity = Vec3r(0, 0, 0);
                maxForce = 1;
                letGravityAffectSolid = false;
            }

            /// Copy constructor.
            VelocityMotorData(const VelocityMotorData& data)
            {
                (*this) = data;
            }

            virtual ~VelocityMotorData()
            {}

            /// Makes a deep copy.
            virtual void operator=(const VelocityMotorData& data)
            {
                mType = data.mType;
                enabled = data.enabled;
                name = data.name;
                solid = data.solid;
                internal_solidIndex = data.internal_solidIndex;
                solidBlueprintRefName = data.solidBlueprintRefName;
                velocity = data.velocity;
                maxForce = data.maxForce;
                letGravityAffectSolid = data.letGravityAffectSolid;
            }

            /// Pointer to the Solid.
            Solid* solid;

            /// Maximum force that can be used to achieve target velocity.
            real maxForce;

            /// if true, the solid will be affected by gravity
            bool letGravityAffectSolid;

            /// Internal data used for Blueprint instantiation.
            int internal_solidIndex;

            /// String used for for intra-Blueprint references.
            std::string solidBlueprintRefName;

            /// The velocity that is to be achieved every frame
            Vec3r velocity;

        protected:

        private:
    };
}

#endif
