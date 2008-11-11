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

#ifndef DIALOG_H_
#define DIALOG_H_

#include <QString>
#include <QList>
#include "Variable.h"

class Dialog {
public:
	Dialog();
	virtual ~Dialog();
	
	void setName(QString name);
	QString Name() const;
	
	void addVariable(Variable var);
	Variable getVariable(unsigned int i );
	const Variable& getVariableRef(unsigned int i );
	unsigned int getVariableCount();
	void clearVariables();
	
private:
	QString m_Name;
	
	QList<Variable> m_Variable;
};

#endif /*DIALOG_H_*/
