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

#include "Motor.h"

namespace opal
{
	Motor::Motor()
	{
		//mIsCustom = false;
		mInitCalled = false;
		setUserData(NULL);
	}

	Motor::~Motor()
	{
	}

	void Motor::internal_destroy()
	{
		delete this;
	}

	void Motor::init()
	{
		mInitCalled = true;
	}

	void Motor::setUserData(void* data)
	{
		mUserData = data;
	}

	void* Motor::getUserData()
	{
		return mUserData;
	}

	bool Motor::internal_dependsOnSolid(Solid* s)
	{
		return false;
	}

	bool Motor::internal_dependsOnJoint(Joint* j)
	{
		return false;
	}

	//void Motor::internal_setCustom(bool c)
	//{
	//	mIsCustom = c;
	//}

	//bool Motor::internal_isCustom()
	//{
	//	return mIsCustom;
	//}
}
