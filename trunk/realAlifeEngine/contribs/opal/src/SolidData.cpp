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

#include "SolidData.h"
#include "BoxShapeData.h"
#include "SphereShapeData.h"
#include "CapsuleShapeData.h"
#include "CylinderShapeData.h"
#include "PlaneShapeData.h"
#include "MeshShapeData.h"

namespace opal
{
	SolidData::SolidData()
	{
		enabled = defaults::solid::enabled;
		name = "";
		sleeping = defaults::solid::sleeping;
		sleepiness = defaults::solid::sleepiness;
		isStatic = defaults::solid::isStatic;
		gravityEnabled = defaults::solid::gravityEnabled;
		// Leave the transform as an identity matrix.
		// "globalLinearVel" is already initialized in its constructor.
		// "globalAngularVel" is already initialized in its constructor.
		linearDamping = defaults::solid::linearDamping;
		angularDamping = defaults::solid::angularDamping;
		// The Shape list doesn't need to be initialized.
	}

	SolidData::SolidData(const SolidData& data)
	{
		(*this) = data;
	}

	SolidData::~SolidData()
	{
		destroyShapes();
	}

	void SolidData::addShape(const ShapeData& data)
	{
		ShapeData* newShape = NULL;

		switch(data.getType())
		{
			case BOX_SHAPE:
			{
				newShape = new BoxShapeData((BoxShapeData&)data);
				break;
			}
			case SPHERE_SHAPE:
			{
				newShape = new SphereShapeData((SphereShapeData&)data);
				break;
			}
			case CAPSULE_SHAPE:
			{
				newShape = new CapsuleShapeData((CapsuleShapeData&)data);
				break;
			}
			case CYLINDER_SHAPE:
			{
				newShape = new CylinderShapeData((CylinderShapeData&)data);
				break;
			}
			case PLANE_SHAPE:
			{
				newShape = new PlaneShapeData((PlaneShapeData&)data);
				break;
			}
			//case RAY_SHAPE:
			//{
			//	newShape = new RayShapeData((RayShapeData&)data);
			//	break;
			//}
			case MESH_SHAPE:
			{
				newShape = new MeshShapeData((MeshShapeData&)data);
				break;
			}
			default:
				assert(false);
		}

		mShapes.push_back(newShape);
	}

	unsigned int SolidData::getNumShapes()const
	{
		return (unsigned int)(mShapes.size());
	}

	ShapeData* SolidData::getShapeData(unsigned int i)const
	{
		return mShapes.at(i);
	}

	void SolidData::destroyShapes()
	{
		while (!mShapes.empty())
		{
			delete mShapes.back();
			mShapes.pop_back();
		}
	}

	void SolidData::operator=(const SolidData& data)
	{
		enabled = data.enabled;
		name = data.name;
		sleeping = data.sleeping;
		sleepiness = data.sleepiness;
		isStatic = data.isStatic;
		transform = data.transform;
		globalLinearVel = data.globalLinearVel;
		globalAngularVel = data.globalAngularVel;
		linearDamping = data.linearDamping;
		angularDamping = data.angularDamping;

		// First remove the old Shapes.
		destroyShapes();

		// Allocate copies of the new Shapes.
		for (unsigned int i=0; i<data.mShapes.size(); ++i)
		{
			addShape(*(data.mShapes[i]));
		}
	}
}

