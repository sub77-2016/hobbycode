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

#ifndef EDITBUFFERDIALOG_H_
#define EDITBUFFERDIALOG_H_

#include <QtGui>
#include "ValueBuffer.h"

//! Creating or editing a buffer.
/*!
  The class implements dialog which lets the user create or edit a buffer depending
  on the given mode to the constructor. 
*/
class EditBufferDialog : public QDialog
{
	Q_OBJECT
	
public:
	//! The constructor.
	EditBufferDialog(ValueBuffer *Buffer, bool Mode, QList<ValueBuffer> *BufferList);
	//! The destructor.
	virtual ~EditBufferDialog();

public slots:
	//! The method is called if the confirms creation or editing of the buffer. 
	virtual void accept();
	//! The method is called if the name of the buffer is changed.
	void textChanged(const QString & text);
	//! The method is called if the type of the buffer is changed.
	void changedType(int idx);
	//! The method is called if the size of the buffer is changed.
	void SizeChanged(int s);
	//! The method is called if the button "Init" has been clicked.
	void InitBuffer();
	
protected:
	//! The method is called if the dialog should be closed.
    void closeEvent(QCloseEvent *event);

private:
	//! The method save the dialog config, e.g. position
	void saveConfig();
	//! The method loads the dialog config, e.g. position
	void loadConfig();

	//! An object to store the values for a buffer.
	ValueBuffer *m_ValueBuffer;
	//! If the value is true, an existing buffer will be edited, otherwise a new one will be created.
	bool m_EditMode;
	//! A pointer to ValueBuffer list
	QList<ValueBuffer> *m_BufferList;

	//! The main layout for the container.	
	QVBoxLayout *m_MainLayout;
	//! A horizontal layout for the name label and linedit.
	QHBoxLayout *m_NameLayout;
	//! A horizontal layout for the type label and combobox.
	QHBoxLayout *m_TypeLayout;
	//! A horizontal layout for the width label and combobox.
	QHBoxLayout *m_WidthLayout;
	//! A horizontal layout for the sign label and combobox.
	QHBoxLayout *m_SignLayout;
	//! A horizontal layout for the byte order label and combobox.
	QHBoxLayout *m_ByteOrderLayout;
	//! A horizontal layout for the size label and spinbox.
	QHBoxLayout *m_SizeLayout;
	//! A horizontal layout for the checksum label and checkbox.
	QHBoxLayout *m_CheckSumLayout;

	//! A QLineEdit for entering the name of the buffer.	
	QLineEdit *m_NameLineEdit;
	//! A Combobox for choosing the type of the buffer.
	QComboBox *m_TypeCB;
	//! A Combobox for choosing the width of values in the buffer.
	QComboBox *m_WidthCB;
	//! A Combobox for choosing the sign of values in the buffer.
	QComboBox *m_SignCB;
	//! A Combobox for choosing the byte order of values in the buffer.
	QComboBox *m_ByteOrderCB;
	
	//! A QTableWidget for entering the values of the buffer.
	QTableWidget *m_ValueTable;
	//! A QCheckBox to choose if a checksum should be calculated.
	QCheckBox *m_CalcCheckBox;
	//! A QSpinBox to adjust the size of the buffer.
	QSpinBox *m_SizeSpinBox;
	//! A QComboBox for choosing the checksum type.
	QComboBox *m_MethodComboBox;
	
	//! A QPushButton to open a dialog to specify how to initialize the buffer.
	QPushButton *m_InitButton;
	//! A QPushButton to create the buffer or commit the changes. 
	QPushButton *m_AddButton;
	//! A QPushButton to cancel creation or editing of a buffer.
	QPushButton *m_CancelButton;
	//! A QDialogButtonBox which holds the "Add" and "Cancel" buttons.
	QDialogButtonBox *m_ButtonBox;
};

#endif /*EDITBUFFERDIALOG_H_*/
