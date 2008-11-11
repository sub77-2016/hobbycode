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

#ifndef INITIALVALUEDIALOG_H_
#define INITIALVALUEDIALOG_H_

#include <QtGui>

//! Initializing of a buffer
class InitialValueDialog : public QDialog
{
	Q_OBJECT
	
public:
	InitialValueDialog();
	virtual ~InitialValueDialog();
	
	int getStartValue();
	int getIncrement();
	
public slots:
	virtual void accept();
	
	protected:
    void closeEvent(QCloseEvent *event);

private:
	//! The method save the dialog config, e.g. position
	void saveConfig();
	//! The method loads the dialog config, e.g. position
	void loadConfig();

	QVBoxLayout *m_MainLayout;
	QHBoxLayout *m_SValueLayout;
	QHBoxLayout *m_IValueLayout;
	
	QSpinBox *m_StartSpinBox;
	QSpinBox *m_IncSpinBox;

	QPushButton *m_AddButton;
	QPushButton *m_CancelButton;
	
	QDialogButtonBox *m_ButtonBox;
};

#endif /*INITIALVALUEDIALOG_H_*/
