/**
 * Serial-IO
 * Copyright (C) 2007-2008  Wilfried Holzke
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License only.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <QtGui>

enum e_Action { HightlightBytesInBuffer = 0 };

//! Attributes of an event.
/*! The class is a container for all attributes that are necessary for an event.
*/
class Event
{
public:
	Event();
	virtual ~Event();
	
	void setName(QString N);
	QString getName() const;
	
	void setBuffer(QStringList B);
	QStringList getBuffer() const;
	
	QByteArray getByteBuffer();
	
	void setActive(bool c);
	bool isActive() const;
	
	void setAction(e_Action C);
	e_Action getAction() const;
	
	void setColor(QColor C);
	QColor getColor() const;
	
private:
	QString m_Name;
	QStringList m_Buffer;
 	bool m_Active;
 	e_Action m_Action;
 	QColor m_Color;
};

#endif /*EVENT_H_*/
