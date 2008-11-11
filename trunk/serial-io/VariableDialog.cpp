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

#include <iostream>
#include "VariableDialog.h"
#include "LongLongIntValidator.h"

using namespace std;

VariableDialog::VariableDialog(const Dialog *dlg, QWidget * parent = 0) : QDialog(parent) {

	m_Dialog = *dlg;
	
	setWindowTitle(dlg->Name());
	
    QVBoxLayout *MainLayout = new QVBoxLayout();
	setLayout(MainLayout);	
	
	// ***** variable structure *****
	
	buildVariableWidgets(MainLayout);
	
	// ***** Buttons *****
	m_ButtonBox = new QDialogButtonBox();

	m_SendButton = new QPushButton("Send");
	m_ButtonBox->addButton(m_SendButton,QDialogButtonBox::ActionRole);
    connect(m_ButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(send(QAbstractButton*)));
	
    MainLayout->addWidget(m_ButtonBox);
}

VariableDialog::~VariableDialog() {
}

void VariableDialog::closeEvent(QCloseEvent *event) {

	emit closeDialog(m_Dialog.Name());
	event->accept();
}

void VariableDialog::buildVariableWidgets(QVBoxLayout *l) {
	
	QGridLayout *gLayout = new QGridLayout();
	l->addLayout(gLayout);
	
	for (unsigned int i=0; i<m_Dialog.getVariableCount(); i++) {

		gLayout->addWidget(new QLabel(m_Dialog.getVariable(i).Name()),i,0);
		
		if (m_Dialog.getVariable(i).Type() == Variable::BOOL) {
			addBool(gLayout,m_Dialog.getVariableRef(i));
			
		} else if (m_Dialog.getVariable(i).Type() == Variable::INTEGER) {
			addInteger(gLayout,m_Dialog.getVariableRef(i));
			
		} else if (m_Dialog.getVariable(i).Type() == Variable::STRING) {
			addString(gLayout,m_Dialog.getVariableRef(i));
		}
	}
}

void VariableDialog::addBool(QGridLayout *l, const Variable &v) {
	
	QList<QWidget*> wList;
	
	int row = l->rowCount()-1;
	
	if (v.Represent().Type() == Representation::TEXTFIELD) {
		
		QLineEdit *le = new QLineEdit();
		le->setMaxLength(1);

		QValidator *validator = new QIntValidator(0, 1, this);
		le->setValidator(validator);
		
		l->addWidget(le,row,1);
		
		l->addWidget(new QLabel("(0,1)"),row,2);		wList.push_back(le);
		
	} else if (v.Represent().Type() == Representation::CHECKBOX) {
		
		QCheckBox *cb = new QCheckBox();
		l->addWidget(cb,row,1);
		
		wList.push_back(cb);
		
	} else if (v.Represent().Type() == Representation::RADIOBUTTON) {
		
		QHBoxLayout *hbl = new QHBoxLayout();
		l->addLayout(hbl,row,1);
		
		QRadioButton *rb0 = new QRadioButton(v.Represent().Attribute("RadioButtonOffText"));
		rb0->setChecked(true);
		QRadioButton *rb1 = new QRadioButton(v.Represent().Attribute("RadioButtonOnText"));

		hbl->addWidget(rb0);
		hbl->addWidget(rb1);
		
		QButtonGroup *bg = new QButtonGroup();
		bg->addButton(rb0,0);
		bg->addButton(rb1,1);
		
		wList.push_back(rb0);
		wList.push_back(rb1);
	}
	
	m_Widgets.push_back(wList);
}

void VariableDialog::addInteger(QGridLayout *l, const Variable &v) {
	
	QList<QWidget*> wList;
	
	int row = l->rowCount()-1;
	
	if (v.Represent().Type() == Representation::TEXTFIELD) {
		
		QLineEdit *le = new QLineEdit();
		le->setMaxLength(11);
		
		bool minOk=false;
		bool maxOk=false;
		
		long long int min = v.Represent().Attribute("LowerLimit").toLongLong(&minOk,0);
		long long int max = v.Represent().Attribute("UpperLimit").toLongLong(&maxOk,0);
		
		if ((!minOk) || (!maxOk)) {
			min = 0;
			max = 0;
		}

		LongLongIntValidator *validator = new LongLongIntValidator(min, max, this);
		le->setValidator(validator);
		
		l->addWidget(le,row,1);
		
		l->addWidget(new QLabel("("+v.Represent().Attribute("LowerLimit")+".."+v.Represent().Attribute("UpperLimit")+")"),row,2);
		
		wList.push_back(le);
		
	} else if (v.Represent().Type() == Representation::CHECKBOX) {
		
		QGridLayout *gl = new QGridLayout();
		l->addLayout(gl,row,1);
		
		bool ok=false;
		unsigned int size = v.Represent().Attribute("Size").toUInt(&ok,0);
		
		if (ok) {
			
			for (unsigned int i=0; i<size; i++) {
			
				QCheckBox *cb = new QCheckBox(v.Represent().Attribute("BitText_"+QString("%1").arg(i)));
				gl->addWidget(cb,i >> 3 ,i & 0x07);
				
				wList.push_back(cb);
			}
			
		} else {
			cout << "VariableDialog::addInteger: wrong size string!" << endl;
		}
	}
	
	m_Widgets.push_back(wList);
}

void VariableDialog::addString(QGridLayout *l, const Variable &v) {
	
	QList<QWidget*> wList;
	
	QLineEdit *le = new QLineEdit();
	bool ok=false;
	int length=0;

	if (v.Represent().Attribute("StringType") == "0") {
		
		length = v.Represent().Attribute("ConstLength").toInt(&ok,0);
	} else {
		length = v.Represent().Attribute("ZeroTerminatedMaxLength").toInt(&ok,0);
	}

	if (!ok) {
		length = 0;
	}
	le->setMaxLength(length);

	l->addWidget(le,l->rowCount()-1,1);
	
	wList.push_back(le);
	
	m_Widgets.push_back(wList);
}

void VariableDialog::send(QAbstractButton *button) {
	
	// TODO: allow sending of data only if all is valid
	
	if (button->text() == "Send") {
		
		QByteArray data;
		
		getData(data);
		emit sendData(data);
	}
}

void VariableDialog::getData(QByteArray &ba) {
	
	for (unsigned int i=0; i<m_Dialog.getVariableCount(); i++) {

		Variable v = m_Dialog.getVariable(i);
		
		if (v.Type() == Variable::BOOL) {
			getBoolData(i,v,ba);
			
		} else if (v.Type() == Variable::INTEGER) {
			getIntegerData(i,v,ba);
			
		} else if (v.Type() == Variable::STRING) {
			getStringData(i,v,ba);
		}
	}
}

void VariableDialog::getBoolData(int idx, Variable &v, QByteArray &ba) {
	
	if (v.Represent().Type() == Representation::TEXTFIELD) {

		QLineEdit *le = (QLineEdit*) m_Widgets.at(idx).at(0);
		
		if ((le->text() == "0") || (le->text() == "")) ba.append((char) 0);
		else ba.append((char) 1);
		
	} else if (v.Represent().Type() == Representation::RADIOBUTTON) {
		
		QRadioButton *rb = (QRadioButton*) m_Widgets.at(idx).at(0);
		
		if (rb->isChecked()) ba.append((char) 0);
		else ba.append((char) 1);

	} else if (v.Represent().Type() == Representation::CHECKBOX) {
		
		QCheckBox *cb = (QCheckBox*) m_Widgets.at(idx).at(0);
		
		if (cb->isChecked()) ba.append((char) 1);
		else ba.append((char) 0);
	}
}

void VariableDialog::getIntegerData(int idx, Variable &v, QByteArray &ba) {
	
	if (v.Represent().Type() == Representation::TEXTFIELD) {

		if (v.Represent().Attribute("Size") == "8") {
			convertStringToInt(idx,v,1,ba);
			
		} else if (v.Represent().Attribute("Size") == "16") {
			convertStringToInt(idx,v,2,ba);
			
		} else if (v.Represent().Attribute("Size") == "32") {
			convertStringToInt(idx,v,4,ba);
		}
		
	} else if (v.Represent().Type() == Representation::CHECKBOX) {
		
		bool ok=false;
		unsigned int size = v.Represent().Attribute("Size").toUInt(&ok,0);
		
		if (ok) {
			unsigned int value=0;
			unsigned int bitValue = 1;
			
			for (unsigned int i=0; i<size; i++) {
				
				QCheckBox *cb = (QCheckBox*) m_Widgets.at(idx).at(i);
				
				if (cb->isChecked()) {
					
					value |= bitValue;
					bitValue <<= 1;
				}
			}
			for (unsigned int i=0; i<(size / 8); i++) {
				ba.append((char) (value & 0xff));
				value >>= 1;
			}
		}
	}
}

void VariableDialog::convertStringToInt(unsigned int idx, Variable &v, unsigned int byteCount, QByteArray &ba) {
	
	QLineEdit *le = (QLineEdit*) m_Widgets.at(idx).at(0);
	
	bool ok=false;
	if (v.Represent().Attribute("Signed") == "0") {
		int value = le->text().toInt(&ok,0);
		if (ok) {
			for (unsigned int i=0; i<byteCount; i++) {
				ba.append((char) (value & 0xff));
				value >>= 1;
			}
		}
	} else {
		unsigned int value = le->text().toUInt(&ok,0);
		if (ok) {
			for (unsigned int i=0; i<byteCount; i++) {
				ba.append((char) (value & 0xff));
				value >>= 1;
			}
		}
	}
}

void VariableDialog::getStringData(int idx, Variable &v, QByteArray &ba) {
	
	QLineEdit *le = (QLineEdit*) m_Widgets.at(idx).at(0);
	QString text = le->text();
	
	for (int i=0; i<text.length(); i++) {
		ba.append(text.at(i));
	}
	
	if (v.Represent().Attribute("StringType") == "0") {
		// constant length
		bool ok=false;
		unsigned int clength = v.Represent().Attribute("ConstLength").toUInt(&ok,0);
		
		// fill up with zeros
		unsigned int c = text.length();
		while (c < clength) {
			ba.append((char) 0);
			c++;
		}
		
	} else {
		// zero terminated
		ba.append((char) 0);
	}
}
