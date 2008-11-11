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

#include "Representation.h"

Representation::Representation() {
	m_Type = NONE;
}

Representation::~Representation() {
}

void Representation::setType(t_RType Type) {
	m_Type = Type;
}

Representation::t_RType Representation::Type() const {
	return m_Type;
}

void Representation::setAttribute(QString key, QString value) {
	
	m_Attribute[key] = value;
}

QString Representation::Attribute(QString key) const {
	return m_Attribute[key];
}

unsigned int Representation::AttributeCount() const {
	return m_Attribute.size();
}

QStringList Representation::AttributeKeys() const {
	
	QStringList list;
	
	QMapIterator<QString, QString> i(m_Attribute);
	while (i.hasNext()) {
		i.next();
		list << i.key();
	}	
	return list;
}
