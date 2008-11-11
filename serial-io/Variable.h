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

#ifndef VARIABLE_H_
#define VARIABLE_H_

#include <QList>
#include <QString>
#include "Representation.h"

class Variable {
public:
	enum t_Variable { NONE, BOOL, INTEGER, STRING };
	
	Variable();
	virtual ~Variable();
	
	void setName(QString name);
	QString Name() const;
	
	void setType(t_Variable type);
	t_Variable Type() const;
	
	void setRepresentation(Representation r);
	Representation Represent() const; 
	
private:
	QString m_Name;
	
	t_Variable m_Type;
	
	QList<Representation> m_Representation;
};

#endif /*VARIABLE_H_*/
