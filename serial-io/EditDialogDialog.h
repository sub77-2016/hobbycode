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

#ifndef EDITDIALOGDIALOG_H_
#define EDITDIALOGDIALOG_H_

#include <QtGui>
#include "Dialog.h"

class EditDialogDialog : public QDialog {

	Q_OBJECT
	
public:
	//! The constructor
	EditDialogDialog(Dialog* dlg, bool EditMode,  QList<Dialog> *DialogList);
	//! The destructor
	virtual ~EditDialogDialog();

public slots:
	//! The method is called if the confirms creation or editing of the buffer. 
	virtual void accept();
	//! The method is called if the user clicks on "Add" button
	void addVariable();
	//! The method is called if the user clicks on "Remove" button
	void remVariable();	
	//!
	void itemChanged(QTableWidgetItem *item);
	//! The method is called if the user selects a another variable 
	void selectedVariableChanged();
	//! The method is called if the length of the dialog name is grater zero
	void DialogNameChanged(const QString &text);
	//! The method moves the current variable in the table one up
	void VariableUp();
	//! The method moves the current variable in the table one down
	void VariableDown();
	//! The method is called if the index in m_RepresentCB changes
	void RepresentationChanged(int index);
	//! This method is called if sign or size is changed in integer representation
	void changedLimits(bool tootled);
	//! 
	void changedVariableName(const QString &text);
	
private:
	//!
	void fillRepresentationCB(QString type);
	//!
	void disconnectRepresentationCB();
	//!
	void connectRepresentationCB();
	//! Add a new row to the variables table
	void addTableRow(QString name, QString type);
	//! Inserts a new row into the variables table
	void insertTableRow(unsigned int row, QString name, QString type);
	//! Enables/disables buttons depending on variables/selection in m_LWVariables table 
	void updateButtonStates();
	//! Returns the row number of the selected variable in the table
	bool getSelectedVariableRow(int &idx);
	//! Creates widgets depending on representation type
	void createRepresentationFrameWidgets(QString r);
	void createTextfield();
	void createCheckBox();
	void createRadioButton();
	
	void saveCurrentVariableData();
	void saveVariableData(int idx);
	void restoreVariableData(int idx);
	
	//!  
	bool ValueOk(QString v, long long int min, long long int max);
	//! The method save the dialog config, e.g. position
	void saveConfig();
	//! The method loads the dialog config, e.g. position
	void loadConfig();

	//! An object to store the dialog data.
	Dialog *m_Dialog;
	//! If the value is true, an existing buffer will be edited, otherwise a new one will be created.
	bool m_EditMode;
	//! A pointer to Dialog list
	QList<Dialog> *m_DialogList;
	
	//! A QLineEdit to edit the dialog name
	QLineEdit *m_TFName;
	//! A QListWidget 
	QTableWidget *m_LWVariables;
	//! A list of variables
	QList<Variable*> m_Variables;
	
	//! A QComboxBox for type selection
	QComboBox *m_TypeCB;
	//! A QPushButton for adding a variable
	QPushButton *m_AddTypeButton;
	//! A QPushButton for remving a variable
	QPushButton *m_RemTypeButton;
	
	//! A QPushButton for moving a variable one up
	QPushButton *m_VarUpButton;
	//! A QPushButton for moving a variable one down
	QPushButton *m_VarDownButton;
	
	//! A QComboBox for representation selection
	QComboBox *m_RepresentCB;
	//! A QScrollArea for displaying parameters
	QScrollArea *m_RepresentArea;
	//! A QFrame for displaying parameters
	QFrame *m_RepresentFrame;
	//! 
	QString m_CurrentType;
	
	//! A QPushButton to create the buffer or commit the changes. 
	QPushButton *m_AddButton;
	//! A QPushButton to cancel creation or editing of a buffer.
	QPushButton *m_CancelButton;
	//! A QDialogButtonBox which holds the "Add" and "Cancel" buttons.
	QDialogButtonBox *m_ButtonBox;
	
	int m_lastSelectedVariable;
	
	
	//! Variables for the representation layout
	QLineEdit *m_NameLineEdit;
	QRadioButton *m_ConstRadioButton;
	QLineEdit *m_ConstLineEdit;
	QRadioButton *m_ZeroRadioButton;
	QLineEdit *m_ZeroLineEdit;
	QButtonGroup *m_STypeGroup;
	
	QRadioButton *m_Size0Button;
	QRadioButton *m_Size1Button;
	QRadioButton *m_Size2Button;
	QButtonGroup *m_SizeGroup;
	
	QRadioButton *m_Sign0Button;
	QRadioButton *m_Sign1Button;
	QButtonGroup *m_SignGroup;
	
	QLineEdit *m_LowerLineEdit;
	QLineEdit *m_UpperLineEdit;
	QLabel *m_LowerLabel;
	QLabel *m_UpperLabel;
	
	QRadioButton *m_OffRadioButton;
	QLineEdit *m_OffLineEdit;
	QRadioButton *m_OnRadioButton;
	QLineEdit *m_OnLineEdit;
	
	QList<QLineEdit *> m_BitTextLineEdit;
};
	
#endif /*EDITDIALOGDIALOG_H_*/
