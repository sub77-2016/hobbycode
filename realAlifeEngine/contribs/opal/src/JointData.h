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

#ifndef OPAL_JOINT_DATA_H
#define OPAL_JOINT_DATA_H

#include "Defines.h"

namespace opal
{
    /// A data structure describing the limits for a single Joint axis.
    struct JointLimits
    {
        JointLimits()
        {
            low = defaults::joint::lowLimit;
            high = defaults::joint::highLimit;
            hardness = defaults::joint::limitHardness;
            bounciness = defaults::joint::limitBounciness;
        }

        /// The limit angle or distance with the smaller value.
        real low;

        /// The limit angle or distance with the higher value.
        real high;

        /// Determines how far a Joint can exceed its limits.  This must
        /// be between 0 and 1.
        real hardness;

        /// Bounciness (i.e. restitution) how much the Joint will bounce
        /// when it hits a limit.
        real bounciness;
    };

    /// A data structure describing a single Joint axis.
    struct JointAxis
    {
        JointAxis()
        {
            direction = defaults::joint::axis0Direction;
            // "limits" is initialized in its own constructor.
            limitsEnabled = defaults::joint::limitsEnabled;
        }

        /// The direction vector of the axis.
        Vec3r direction;

        /// Determines whether Joint limits are enabled for this axis.
        bool limitsEnabled;

        /// The limits of the Joint axis.
        JointLimits limits;
    };

    /// A data structure describing a Joint.
    class JointData
    {
        public:
            JointData()
            {
                mType = defaults::joint::type;
                enabled = defaults::joint::enabled;
                name = "";
                solid0 = NULL;
                solid1 = NULL;
                internal_solid0Index = 0;
                internal_solid1Index = 0;
                solid0BlueprintRefName = "";
                solid1BlueprintRefName = "";
                anchor = defaults::joint::anchor;
                axis[0].direction = defaults::joint::axis0Direction;
                axis[1].direction = defaults::joint::axis1Direction;
                axis[2].direction = defaults::joint::axis2Direction;
                // The rest of the data from the axes are initialized in the
                // JointAxis constructor.
                breakMode = defaults::joint::breakMode;
                breakThresh = defaults::joint::breakThresh;
                accumThresh = defaults::joint::accumThresh;
                accumDamage = 0;
                contactsEnabled = defaults::joint::contactsEnabled;
                isBroken = false;
            }

            /// Copy constructor.
            JointData(const JointData& data)
            {
                (*this) = data;
            }

            virtual ~JointData()
            {}

            /// Makes a deep copy.
            void operator=(const JointData& data)
            {
                mType = data.mType;
                enabled = data.enabled;
                name = data.name;
                solid0 = data.solid0;
                solid1 = data.solid1;
                internal_solid0Index = data.internal_solid0Index;
                internal_solid1Index = data.internal_solid1Index;
                solid0BlueprintRefName = data.solid0BlueprintRefName;
                solid1BlueprintRefName = data.solid1BlueprintRefName;
                anchor = data.anchor;

                for (int i = 0; i < 3; ++i)
                {
                    axis[i] = data.axis[i];
                }

                breakMode = data.breakMode;
                breakThresh = data.breakThresh;
                accumThresh = data.accumThresh;
                accumDamage = data.accumDamage;
                contactsEnabled = data.contactsEnabled;
                isBroken = data.isBroken;
            }

            /// Sets the Joint's type.
            void setType(JointType type)
            {
                mType = type;
            }

            /// Returns the Joint's type.
            JointType getType() const
            {
                return mType;
            }

            /// Determines whether the Joint is enabled.
            bool enabled;

            /// An identifier for the Joint.
            std::string name;

            /// Pointer to the Joint's Solid0.
            Solid* solid0;

            /// Pointer to the Joint's Solid1.
            Solid* solid1;

            /// Internal data used for Blueprint instantiation.
            int internal_solid0Index;

            /// Internal data used for Blueprint instantiation.
            int internal_solid1Index;

            /// String used for for intra-Blueprint references.
            std::string solid0BlueprintRefName;

            /// String used for for intra-Blueprint references.
            std::string solid1BlueprintRefName;

            /// The anchor point for this Joint.  This is not used for every
            /// Joint type.
            Point3r anchor;

            /// The axes that describe the Joint's degrees of freedom.  Not all
            /// axes are used for every Joint type.
            JointAxis axis[3];

            /// Determines how damage is handled.
            JointBreakMode breakMode;

            /// Joint breaks if force/torque exceeds this value.
            real breakThresh;

            /// Force/torque exceeding this adds to accumulated damage.
            real accumThresh;

            /// The amount of accumulated damage.
            real accumDamage;

            /// Determines whether Solids connected by this Joint should make
            /// contacts when they collide.  If multiple Joints connect the same
            /// two Solids, each with different contactsEnabled settings,
            /// the behavior is undefined.
            bool contactsEnabled;

            //! If true, the joint is broken.
            /*!
             * @note the default is false - joint is not broken
             */
            bool isBroken;

        protected:
            /// The Joint type.
            JointType mType;

        private:
    };
}

#endif
