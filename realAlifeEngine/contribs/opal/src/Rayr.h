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

#ifndef OPAL_RAYR_H
#define OPAL_RAYR_H

#include "OpalMath.h"

namespace opal
{
	class Rayr;

	/// Prints the Rayr to the given output stream.
	inline std::ostream& operator<<(std::ostream& o, const Rayr& r);

	class Rayr
	{
	private:
		Vec3r mDir;
		Point3r mOrigin;

	public:
		Rayr()
		{
			mDir.set(0,0,1);
			mOrigin.set(0,0,0);
		}

		Rayr(const Rayr & src)
		{
			mDir = src.mDir;
			mOrigin = src.mOrigin;
		}

		Rayr(const opal::Point3r & origin, const opal::Vec3r & dir)
		{
			mDir = dir;
			mOrigin = origin;
		}

		//Rayr(const opal::Vec3r & dir, const opal::Point3r & origin)
		//{
		//	mDir = dir;
		//	mOrigin = origin;
		//}

		inline void set(const opal::Point3r & origin, const opal::Vec3r & dir)
		{
			mDir = dir;
			mOrigin = origin;
		}

		//inline void set(const opal::Vec3r & dir, const opal::Point3r & origin)
		//{
		//	mDir = dir;
		//	mOrigin = origin;
		//}

		inline Vec3r getDir()const
		{
			return mDir;
		}

		inline Point3r getOrigin()const
		{
			return mOrigin;
		}

		inline real getLength()const
		{
			return mDir.length();
		}

		inline void setOrigin(const Point3r & p)
		{
			mOrigin = p;
		}

		inline void setDir(const Vec3r & d)
		{
			mDir = d;
		}
	};

	inline std::ostream& operator<<(std::ostream& o, const Rayr& r)
	{
		Point3r origin = r.getOrigin();
		Vec3r dir = r.getDir();
		return o << "Origin: [" << origin[0] << " " << origin[1] << " " 
			<< origin[2] << "] Direction: [" << dir[0] << " " << dir[1] 
			<< " " << dir[2] << "]";
	}
}

#endif

