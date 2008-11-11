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

#ifndef BYTEBUFFER_H_
#define BYTEBUFFER_H_

#include <QtGui>

//! Attributes of a value buffer.
class ValueBuffer
{
public:
	enum e_Type { None = 0, Character, Number };
	enum e_Width { Bits_8 = 0, Bits_16, Bits_32 };
	enum e_ByteOrder { LittleEndian, BigEndian };
	enum e_ChecksumType { SumWithoutCarryMinusOne = 0, ByteXOR };

	//! The constructor.
	ValueBuffer();
	//! The destructor.
	virtual ~ValueBuffer();
	
	//! The method allows to set the name of the buffer.
	void setName(QString N);
	//! The method returns the currently set name.
	QString getName() const;
	
	//! The method allows to set the type of the buffer.
	void setType(e_Type T);
	//! The methdo returns the currently set type.
	e_Type getType() const;
	
	//! The method allows to set the width of the buffer.
	void setWidth(e_Width W);
	//! The methdo returns the currently set width.
	e_Width getWidth() const;
	
	//! The method allows to set the sign of the buffer.
	void setSigned(bool S);
	//! The methdo returns the currently set sign.
	bool getSigned()  const;

	//! The method allows to set the byte order of the buffer.
	void setByteOrder(e_ByteOrder B);
	//! The methdo returns the currently set byte order.
	e_ByteOrder getByteOrder() const;
	
	//! The method allows to set the buffer.
	void setBuffer(QStringList S);
	//! The methdos returns the buffer.
	QStringList getBuffer() const;
	
	//! The method allows to set the if checksum should be calculated.
	void setCalcChecksum(bool c);
	//! The method returns true if a checksum should be calculated, otherwise false.
	bool getCalcChecksum() const;
	
	//! The method allows to set the type of the checksum.
	void setChecksumType(e_ChecksumType C);
	//! The method returns the type of the checksum.
	e_ChecksumType getChecksumType() const;
	
private:
	//! The name of the buffer.
	QString m_Name;
	//! The type, 0 none, 1 for characters, 2 for numbers
	e_Type m_Type;
	//! The width, 0 for 8 bit, 1 for 16 bit, 2 for 32 bit
	e_Width m_Width;
	//! Signed or unsigned
	bool m_Signed;
	//! Byte order, 0 little endian, 1 big endian
	e_ByteOrder m_ByteOrder;
	//! The buffer itself.
	QStringList m_Buffer;
	//! Calculate a checksum if value is true.
 	bool m_CalcChecksum;
 	//! The checksum type.
 	e_ChecksumType m_ChecksumType;
};

#endif /*BYTEBUFFER_H_*/
