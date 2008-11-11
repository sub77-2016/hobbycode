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

#include <iostream>
#include "LongLongIntValidator.h"

LongLongIntValidator::LongLongIntValidator(QObject *parent) : QValidator(parent) {
	m_Bottom = 0;
	m_Top = 0;
}

LongLongIntValidator::LongLongIntValidator(long long int minimum, long long int maximum, QObject * parent) : QValidator(parent) {
	m_Bottom = minimum;
	m_Top = maximum;
}

LongLongIntValidator::~LongLongIntValidator() {
}

void LongLongIntValidator::setRange(long long int bottom, long long int top) {
	
	m_Bottom = bottom;
	m_Top = top;
}

void LongLongIntValidator::setBottom(long long int bottom) {
	m_Bottom = bottom;
}

void LongLongIntValidator::setTop(long long int top) {
	m_Top = top;
}

long long int LongLongIntValidator::top() const {
	return m_Top;
}

long long int LongLongIntValidator::bottom() const {
	return m_Bottom;
}

void LongLongIntValidator::fixup(QString &input) const {

	if ((input.length() == 0) || (input == "-")) {
		input = "0";
	}
}

QValidator::State LongLongIntValidator::validate(QString &input, int &/*pos*/) const {

	bool ok=false;

	int c=0;
	
	while (c < input.length()) {
		
		if ((input[c].isDigit()) || ((c == 0) && (input[c] == '-'))) {
			c++;
		} else {
			input.remove(c,1);
		}
	}
	
	long long int value = input.toLongLong(&ok,10);
	
	if (ok) {
		if ((value >= m_Bottom) && (value <=m_Top)) {
			return QValidator::Acceptable;
		}
	} else if ((input.length() == 0) || (input == "-")) {
		return QValidator::Intermediate;
	}
	return QValidator::Invalid;
}
