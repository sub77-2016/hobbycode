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

#ifndef REPRESENTATION_H_
#define REPRESENTATION_H_

#include <QString>
#include <QStringList>
#include <QMap>

class Representation {
public:
	enum t_RType { NONE, TEXTFIELD, CHECKBOX, RADIOBUTTON }; 
	
	Representation();
	virtual ~Representation();
	
	void setType(t_RType Type);
	t_RType Type() const;
	
	void setAttribute(QString key, QString value);
	QString Attribute(QString key) const;

	unsigned int AttributeCount() const;
	QStringList AttributeKeys() const;
	
private:
	t_RType m_Type;
	
	QMap<QString, QString> m_Attribute;
};

#endif /*REPRESENTATION_H_*/
