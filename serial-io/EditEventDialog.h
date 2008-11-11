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

#ifndef EDTEVENTDIALOG_H_
#define EDTEVENTDIALOG_H_

#include <QtGui>
#include "Event.h"

//! Creating or editing an event.
/*!
  The class implements dialog which lets the user create or edit an event depending
  on the given mode to the constructor. 
*/
class EditEventDialog : public QDialog
{
	Q_OBJECT
	
public:
	//! The constructor.
	EditEventDialog(Event *E, bool Mode, QList<Event> *EventList);
	//! The desctructor.
	virtual ~EditEventDialog();
	
public slots:
	//! The method is called if the confirms creation or editing of the buffer. 
	virtual void accept();
	//! The method is called if the name of the buffer is changed.
	void textChanged(const QString & text);
	//! The method is called if the size of the buffer is changed.
	void SizeChanged(int s);
	//! The method is called if an item in the table is changed.
	void TableItemChanged(QTableWidgetItem *item);
	//! Action checkbox toggled
	void enableActionChanged(bool state);
	//! The method is called if the color-choose button is clicked
	void chooseColor();
	
	
protected:
    void closeEvent(QCloseEvent *event);

private:
	//! The method save the dialog config, e.g. position
	void saveConfig();
	//! The method loads the dialog config, e.g. position
	void loadConfig();

	void AddButtonState();

	// state variables
	Event *m_Event;
	bool m_EditMode;
	QList<Event> *m_EventList;
	
	QList<bool> m_ItemState;
	bool m_NameExist;
	
	// gui related variables
	QVBoxLayout *m_MainLayout;
	QHBoxLayout *m_NameLayout;
	QHBoxLayout *m_SizeLayout;
	QHBoxLayout *m_CheckSumLayout;
	QHBoxLayout *m_ColorLayout;
	
	QLineEdit *m_NameLineEdit;
	QTableWidget *m_ValueTable;
	QCheckBox *m_ActiveCheckBox;
	QSpinBox *m_SizeSpinBox;
	QComboBox *m_MethodComboBox;
	QLabel *m_ColorLabel;
	QPushButton *m_ColorButton;
	
	QPushButton *m_AddButton;
	QPushButton *m_CancelButton;
	
	QDialogButtonBox *m_ButtonBox;
	
	QColor m_Color;
};

#endif /*EDTEVENTDIALOG_H_*/
