/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
* Oleksandr Lozitskiy (http://ox.slug.louisville.edu/~o0lozi01/wiki)    *
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

#ifndef OPALVELOCITYMOTOR_H
#define OPALVELOCITYMOTOR_H

// class headers
#include "Motor.h"

// project headers
#include "Portability.h"
#include "VelocityMotorData.h"

namespace opal
{
    class Simulator;
    class VelocityMotorData;

    /**
     * Tries to achieve given velocity in one time step of the simulation.
     * 
     * @author Oleksandr Lozitskiy 
     */
    class VelocityMotor : public Motor
    {
        public:
            //! constructor
            /*!
             * @param sim physical simulator
             */
            VelocityMotor(Simulator * sim);

            //! destructor
            virtual ~VelocityMotor();

            /// Initializes the Motor with the given data structure.
            virtual void OPAL_CALL init(const VelocityMotorData & data);

            /// Sets whether the Motor has any effect.
            virtual void OPAL_CALL setEnabled(bool e);

            /// Returns true if the Motor is enabled.
            virtual bool OPAL_CALL isEnabled() const;

            /// Returns the Motor type.
            virtual MotorType OPAL_CALL getType() const;

            /// Sets the Motor's name.
            virtual void OPAL_CALL setName(const std::string & name);

            /// Returns the Motor's name.
            virtual const std::string & OPAL_CALL getName() const;

            /// Set target velocity.
            virtual void OPAL_CALL setVelocity(const Vec3r & velocity);

            /// Return target velocity.
            virtual const Vec3r & OPAL_CALL getVelocity() const;

            /// Called regularly to update the Motor.  This does nothing if the
            /// Motor is disabled.
            virtual void internal_update();

            //! Returns true if motors controls this solid.
            virtual bool internal_dependsOnSolid(Solid * s) const;

            //! against gravity or not?
            /*!
             * @param affect if true, motor will NOT work against gravity,
             *               so solid will be affected by gravity
             */
            virtual void OPAL_CALL letGravityAffectSolid(bool affect);

            //! Returns true if gravity is being ignored by the motor
            virtual bool OPAL_CALL doesGravityAffectSolid() const;

            //! Maximum allowed force for the motor to use.
            virtual void OPAL_CALL setMaximumForce(real maxForce);

            //! What is the maximum allowed force for this motor?
            virtual real OPAL_CALL getMaximumForce() const;

        protected:
            /// Stores data describing the Motor.
            VelocityMotorData mData;

        private:
            Simulator * mSimulator;
    };
}

#endif
