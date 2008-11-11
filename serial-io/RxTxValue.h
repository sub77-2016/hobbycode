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

#ifndef RXTXVALUE_H_
#define RXTXVALUE_H_

#include <QString>

enum e_ValueType { UNDEFINED_VALUE, RX_VALUE, TX_VALUE };

using namespace std;

//! A Value of Rx/Tx buffer
class RxTxValue
{
public:
	RxTxValue();
	virtual ~RxTxValue();					
	RxTxValue(const RxTxValue&);			// copy constructor
	RxTxValue& operator=(const RxTxValue&); // assignment operator
	
	void setValues(QString ts, char c, e_ValueType vt, unsigned int row, unsigned int col);
	void decrementRow();
	
	QString Timestamp() const;
	char Value() const;
	e_ValueType Type() const;
	unsigned int Column() const;
	unsigned int Row() const;
	
private:
	QString m_Timestamp;
	char m_Value;
	e_ValueType m_Type;
	unsigned int m_Column;
	unsigned int m_Row;
};

#endif /*RXTXVALUE_H_*/
