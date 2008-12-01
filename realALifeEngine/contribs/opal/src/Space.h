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

#ifndef OPAL_SPACE_H
#define OPAL_SPACE_H

#include "Defines.h"

namespace opal
{
	/// Spaces are used for space-partitioning to speed up collision 
	/// detection.  If a particular application contains a group of 
	/// Solids that will always be in close proximity, it can help to 
	/// create a Space and add those Solids to the Space.
	class Space
	{
	public:
		Space();

		/// Removes the Space from its current parent Space and adds it to 
		/// the new Space.
		virtual void OPAL_CALL setParentSpace(Space* parentSpace) = 0;

		/// Internal function used to destroy this object.
		void internal_destroy();

	protected:
		virtual ~Space();

	private:

	};
}

#endif

