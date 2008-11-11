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

#include "Variable.h"

Variable::Variable() {
	m_Type = NONE;
	Representation r;
	r.setType(Representation::NONE);
	m_Representation.push_back(r);
}

Variable::~Variable() {
}

void Variable::setName(QString name) {
	m_Name = name;
}

QString Variable::Name() const {
	return m_Name;	
}

void Variable::setType(t_Variable type) {
	m_Type = type;
}

Variable::t_Variable Variable::Type() const {
	return m_Type;
}

void Variable::setRepresentation(Representation r) {
	if (m_Representation.size() == 0) {
		m_Representation.push_back(r);
	} else {
		m_Representation.replace(0,r);
	}
}

Representation Variable::Represent() const {
	return m_Representation.at(0);
}
