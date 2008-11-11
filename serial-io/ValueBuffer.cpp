/*
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

#include "ValueBuffer.h"

ValueBuffer::ValueBuffer()
{
	m_Name = "";
	m_Type = None;
	m_Width = Bits_8;
	m_Signed = false;
	m_ByteOrder = LittleEndian;
	m_CalcChecksum = false;
	m_ChecksumType = SumWithoutCarryMinusOne;
}

ValueBuffer::~ValueBuffer()
{
}

void ValueBuffer::setName(QString N) {
	m_Name = N;
}

QString ValueBuffer::getName() const {
	return m_Name;
}

void ValueBuffer::setType(e_Type T) {
	m_Type = T;
}

ValueBuffer::e_Type ValueBuffer::getType() const {
	return m_Type;
}
	
void ValueBuffer::setWidth(e_Width W) {
	m_Width = W;
}

ValueBuffer::e_Width ValueBuffer::getWidth() const {
	return m_Width;
}

void ValueBuffer::setSigned(bool S) {
	m_Signed = S;
}

bool ValueBuffer::getSigned() const {
	return m_Signed;
}
	
void ValueBuffer::setByteOrder(e_ByteOrder B) {
	m_ByteOrder = B;
}

ValueBuffer::e_ByteOrder ValueBuffer::getByteOrder() const {
	return m_ByteOrder;
}
	
void ValueBuffer::setBuffer(QStringList S) {
	m_Buffer = S;
}

QStringList ValueBuffer::getBuffer() const {
	return m_Buffer;
}
	
void ValueBuffer::setCalcChecksum(bool c) {
	m_CalcChecksum = c;
}

bool ValueBuffer::getCalcChecksum() const {
	return m_CalcChecksum;
}
	
void ValueBuffer::setChecksumType(e_ChecksumType C) {
	m_ChecksumType = C;
}

ValueBuffer::e_ChecksumType ValueBuffer::getChecksumType() const {
	return m_ChecksumType;
}
