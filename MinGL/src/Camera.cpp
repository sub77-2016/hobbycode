/*
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Copyright (C) 2006 by Vinay Pulim.
 * All rights reserved.
 *
 */

#ifdef MACOSX
#include "OpenGL/glu.h"
#else
#include "GL/glu.h"
#endif

#include "Camera.h"

namespace MINGL {
	
	Camera::Camera()
	{
  		reset();
	}

	void Camera::rotate(float x, float y)
	{
  		_rotX += x;
  		if (_rotX < 2) _rotX = 2;
  		if (_rotX > 80) _rotX = 80;
  		_rotY += y;
	}

	void Camera::pan(float x, float y, float z)
	{
  		_panX -= x/2;
  		if (_panX < -500) _panX = -500;
  		if (_panX > 500) _panX = 500;
  		_panY += y/2;
  		if (_panY < 5) _panY = 5;
  		if (_panY > 500) _panY = 500;
  		_panZ += z;
  		if (_panZ < 10) _panZ = 10;
  		if (_panZ > 1000) _panZ = 1000;
	}
 
	void Camera::setModelView()
	{
  		glMatrixMode(GL_MODELVIEW);	
  		glLoadIdentity();
  				    
  		gluLookAt(_panX, _panY, _panZ, _panX, _panY, 0, 0, 1, 0);
  		
  		glRotatef(_rotX, 1, 0, 0);
  		glRotatef(_rotY, 0, 1, 0);
	}

	void Camera::reset()
	{
  		//_rotX = 10;
  		//_rotY = 0;
  		//_panX = 0;
  		//_panY = 40;
  		//_panZ = 100;
  		
  		_rotX = 0;
  		_rotY = 0;
  		
  		_panX = 0;
  		_panY = 1;
  		_panZ = 6;
	}
	
}
