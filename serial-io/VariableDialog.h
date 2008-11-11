/*
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

#ifndef VARIABLEDIALOG_H_
#define VARIABLEDIALOG_H_

#include <QtGui>
#include "Dialog.h"

class VariableDialog : public QDialog {

	Q_OBJECT
	
public:
	//! The constructor
	VariableDialog(const Dialog *dlg, QWidget * parent);
	//! The destructor
	virtual ~VariableDialog();
	
signals:
	void sendData(QByteArray &data);
	void closeDialog(QString name);
	
public slots:
	//! Send the data if the user clicks the button "send"
	void send(QAbstractButton *button);
	
private:
	//! The close event method is called if the window is closed. 
    void closeEvent(QCloseEvent *event);
	
	//! Builds the widgets depending on the defined variables in m_Dialog
	void buildVariableWidgets(QVBoxLayout *l);
	//! Add a bool variable to the dialog
	void addBool(QGridLayout *l, const Variable &v);
	//! Add an integer variable to the dialog
	void addInteger(QGridLayout *l, const Variable &v);
	//! Add a string variable to the dialog
	void addString(QGridLayout *l, const Variable &v);
	
	//! Collects the data from the widgets and tranforms it to byte values
	void getData(QByteArray &ba);
	void getBoolData(int idx, Variable &v, QByteArray &ba);
	void getIntegerData(int idx, Variable &v, QByteArray &ba);
	void convertStringToInt(unsigned int idx, Variable &v, unsigned int byteCount, QByteArray &ba);
	void getStringData(int idx, Variable &v, QByteArray &ba);
	
	//! A QPushButton to create the buffer or commit the changes. 
	QPushButton *m_SendButton;
	//! A QDialogButtonBox which holds the "Add" and "Cancel" buttons.
	QDialogButtonBox *m_ButtonBox;
	
	//! A local copy of the dialog structure
	Dialog m_Dialog;
	
	//! A list to store pointers to the gui widgets
	QList<QList<QWidget*> > m_Widgets;
};

#endif /*VARIABLEDIALOG_H_*/
