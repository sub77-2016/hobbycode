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

#ifndef OPAL_ODETOOLS_H
#define OPAL_ODETOOLS_H

#include "opal.h"

namespace opal
{
	opal::Vec3r toVec3r( dVector3 vector )
	{
		opal::real x, y, z;
		x = static_cast<opal::real>( vector[0] );
		y = static_cast<opal::real>( vector[1] );
		z = static_cast<opal::real>( vector[2] );

		return opal::Vec3r( x, y, z );
	}

	opal::Vec3r toVec3r( const dReal * vector )
	{
		opal::real x, y, z;
		x = static_cast<opal::real>( vector[0] );
		y = static_cast<opal::real>( vector[1] );
		z = static_cast<opal::real>( vector[2] );

		return opal::Vec3r( x, y, z );
	}

	opal::Vec3r toVec3r_RadToDeg( dVector3 vector )
	{
		opal::real x, y, z;
		x = opal::radToDeg( static_cast<opal::real>( vector[0] ) );
		y = opal::radToDeg( static_cast<opal::real>( vector[1] ) );
		z = opal::radToDeg( static_cast<opal::real>( vector[2] ) );

		return opal::Vec3r( x, y, z );
	}

	opal::Vec3r toVec3r_RadToDeg( const dReal * vector )
	{
		opal::real x, y, z;
		x = opal::radToDeg( static_cast<opal::real>( vector[0] ) );
		y = opal::radToDeg( static_cast<opal::real>( vector[1] ) );
		z = opal::radToDeg( static_cast<opal::real>( vector[2] ) );

		return opal::Vec3r( x, y, z );
	}

	opal::Vec3r toVec3r_DegToRad( dVector3 vector )
	{
		opal::real x, y, z;
		x = opal::degToRad( static_cast<opal::real>( vector[0] ) );
		y = opal::degToRad( static_cast<opal::real>( vector[1] ) );
		z = opal::degToRad( static_cast<opal::real>( vector[2] ) );

		return opal::Vec3r( x, y, z );
	}
}

#endif
