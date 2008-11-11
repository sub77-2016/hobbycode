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

#include "Event.h"

Event::Event()
{
	m_Name = "";
	m_Active = false;
 	m_Action = HightlightBytesInBuffer;
}

Event::~Event()
{
}

void Event::setName(QString N) {
	m_Name = N;
}

QString Event::getName() const{
	return m_Name;
}
	
void Event::setBuffer(QStringList B) {
	m_Buffer = B;
}

QStringList Event::getBuffer() const {
	return m_Buffer;
}
	
	
QByteArray Event::getByteBuffer() {
	
	QByteArray bytes;
	
	bool convertOk = false;
	
	int ivalue=0;
	
	char cvalue = 0;
	
	QString strValue = "";
	
	for (int i=0; i<m_Buffer.size(); i++) {

		strValue = m_Buffer.at(i);
		
		if (strValue.length() == 0) continue;
		
		if (strValue.startsWith("0x")) {
			ivalue = strValue.toInt(&convertOk,16);
		} else {
			ivalue = strValue.toInt(&convertOk,10);	
		}
	
		if (convertOk && (ivalue >= 0) && (ivalue <= 255)) { 
			cvalue = ivalue;
		} else {
			cvalue = 0;
		} 
		bytes.push_back(cvalue);
	}
	
	return bytes;
}
	
void Event::setActive(bool c) {
	m_Active = c;
}

bool Event::isActive() const {
	return m_Active;
}
	
void Event::setAction(e_Action A) {
	m_Action = A;
}

e_Action Event::getAction() const {
	return m_Action;
}

void Event::setColor(QColor C) {
	m_Color = C;
}

QColor Event::getColor() const {
	return m_Color;
}
