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

#include "Dialog.h"

Dialog::Dialog() {
}

Dialog::~Dialog() {
}

void Dialog::setName(QString name) {
	m_Name = name;
}

QString Dialog::Name() const {
	return m_Name;
}

void Dialog::addVariable(Variable var) {
	m_Variable.push_back(var);
}

Variable Dialog::getVariable(unsigned int i) {
	return m_Variable.at(i);
}

const Variable& Dialog::getVariableRef(unsigned int i) {
	return m_Variable.at(i);
}

unsigned int Dialog::getVariableCount() {
	return m_Variable.size();
}

void Dialog::clearVariables() {
	m_Variable.clear();
}
