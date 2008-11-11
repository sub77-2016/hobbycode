/**
 * Serial-IO
 * Copyright (C) 2007  Wilfried Holzke
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

#include "RxTxValue.h"

RxTxValue::RxTxValue()
{
	m_Timestamp = "";
	m_Value = 0;
	m_Type = UNDEFINED_VALUE;
	m_Column = 0;
	m_Row = 0;
}

RxTxValue::~RxTxValue()
{
}

RxTxValue::RxTxValue(const RxTxValue& o) {
	m_Timestamp = o.Timestamp();
	m_Value = o.Value();
	m_Type = o.Type();
	m_Column = o.Column();
	m_Row = o.Row();
}


RxTxValue& RxTxValue::operator=(const RxTxValue& o) {
	m_Timestamp = o.Timestamp();
	m_Value = o.Value();
	m_Type = o.Type();
	m_Column = o.Column();
	m_Row = o.Row();
	return *this;
}

void RxTxValue::setValues(QString ts, char c, e_ValueType vt, unsigned int row, unsigned int col) {
	m_Timestamp = ts;
	m_Value = c;
	m_Type = vt;
	m_Row = row;
	m_Column = col;
}

void RxTxValue::decrementRow() {
	m_Row--;
}

QString RxTxValue::Timestamp() const {
	return m_Timestamp;
}

char RxTxValue::Value() const {
	return m_Value;
}

e_ValueType RxTxValue::Type() const {
	return m_Type;
}

unsigned int RxTxValue::Column() const {
	return m_Column;
}

unsigned int RxTxValue::Row() const {
	return m_Row;
}


