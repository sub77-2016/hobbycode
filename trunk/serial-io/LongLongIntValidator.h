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

#ifndef LONGLONGINTVALIDATOR_H_
#define LONGLONGINTVALIDATOR_H_

#include <QValidator>

class LongLongIntValidator : public QValidator {
	
	Q_OBJECT
	
public:
	LongLongIntValidator(QObject *parent);
	LongLongIntValidator(long long int minimum, long long int maximum, QObject * parent);
	virtual ~LongLongIntValidator();
	
	void setRange(long long int bottom, long long int top);
	void setBottom(long long int bottom);
	void setTop(long long int top);

	long long int top() const;
	long long int bottom() const;
	
	virtual void fixup ( QString & input ) const;
    virtual QValidator::State validate ( QString & input, int & pos ) const;
	
private:
	long long int m_Bottom;
	long long int m_Top;
};

#endif /*LONGLONGINTVALIDATOR_H_*/
