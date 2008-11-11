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

#include <iostream>
#include "EditEventDialog.h"

using namespace std;

EditEventDialog::EditEventDialog(Event *E, bool Mode, QList<Event> *EventList)
 : QDialog()
{
	// init variables
	m_Event = E;
	
	m_EditMode = Mode;
	
	m_EventList = EventList;
	
	m_NameExist = false;
	
	// load dialog configuration
	loadConfig();
	
	// set window title
	if (m_EditMode == true) setWindowTitle(tr("New Event"));
	else setWindowTitle("Edit Event");

	// create main layout
    m_MainLayout = new QVBoxLayout();
    
    // event name
    m_NameLayout = new QHBoxLayout();
    m_MainLayout->addLayout(m_NameLayout);
    
    m_NameLayout->addWidget(new QLabel("Name:"));
    m_NameLineEdit = new QLineEdit();
    m_NameLineEdit->setText(m_Event->getName());
    connect(m_NameLineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(textChanged(const QString &)));
    m_NameLayout->addWidget(m_NameLineEdit);
    
    // event size
    m_SizeLayout = new QHBoxLayout();
    m_MainLayout->addLayout(m_SizeLayout);
    
    m_SizeLayout->addWidget(new QLabel("Size:"));
    m_SizeSpinBox = new QSpinBox(this);
    m_SizeSpinBox->setMinimum(1);
    m_SizeSpinBox->setMaximum(1024);
    m_SizeSpinBox->setValue(m_Event->getBuffer().size());
    connect(m_SizeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(SizeChanged(int)));
    m_SizeLayout->addWidget(m_SizeSpinBox); 
    
    // event values
    QStringList ValueTableHeader;
    ValueTableHeader << "Value";
    
    m_ValueTable = new QTableWidget(m_Event->getBuffer().size(),ValueTableHeader.size());
	m_ValueTable->setHorizontalHeaderLabels(ValueTableHeader);
	m_ValueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_ValueTable->setAlternatingRowColors(true);
	m_ValueTable->setShowGrid(false);
	connect(m_ValueTable,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(TableItemChanged(QTableWidgetItem*)));

	m_ValueTable->resizeColumnsToContents();
    m_MainLayout->addWidget(m_ValueTable);

    // event action
	m_CheckSumLayout = new QHBoxLayout();
	m_MainLayout->addLayout(m_CheckSumLayout);
	
	m_ActiveCheckBox = new QCheckBox("Enable Action:");
	m_ActiveCheckBox->setChecked(m_Event->isActive());
	m_CheckSumLayout->addWidget(m_ActiveCheckBox);
	
	QStringList MethodList;
	
	MethodList << "Highlight Byte Sequence";
	
	m_MethodComboBox = new QComboBox();
	m_MethodComboBox->addItems(MethodList);
	m_MethodComboBox->setCurrentIndex(m_Event->getAction());
	m_MethodComboBox->setEnabled(m_Event->isActive());
	m_CheckSumLayout->addWidget(m_MethodComboBox);
	
	// event action color
	m_ColorLayout = new QHBoxLayout();
	m_MainLayout->addLayout(m_ColorLayout);
	
	m_ColorLayout->addWidget(new QLabel("Color: "));
	
	QColor color;
	if (m_EditMode == false) color = E->getColor();
	else color.setNamedColor("#FFE0E0");
	
	m_ColorLabel = new QLabel("   ");
	m_ColorLabel->setPalette(QPalette(color));
   	m_ColorLabel->setAutoFillBackground(true);
    m_ColorLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	m_ColorLayout->addWidget(m_ColorLabel);
	
	m_ColorButton = new QPushButton("choose");
	m_ColorLayout->addWidget(m_ColorButton);
	m_ColorButton->setEnabled(m_Event->isActive());
	connect(m_ColorButton,SIGNAL(clicked()),this,SLOT(chooseColor()));
	
	m_MainLayout->addSpacing(10);

	// button box
	m_ButtonBox = new QDialogButtonBox();

	QString AddButtonLabel;

	if (m_EditMode == true)  AddButtonLabel = "Add";
	else AddButtonLabel = "Change";

	m_AddButton = new QPushButton(AddButtonLabel);
	m_AddButton->setEnabled(false);
	m_AddButton->setEnabled(m_NameLineEdit->text().length() > 0);
	m_ButtonBox->addButton(m_AddButton,QDialogButtonBox::AcceptRole);
    connect(m_ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	
	m_CancelButton = new QPushButton("Cancel");                       
	m_ButtonBox->addButton(m_CancelButton,QDialogButtonBox::RejectRole);
    connect(m_ButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    m_MainLayout->addWidget(m_ButtonBox);

	// add data here because all widgets are initialized
	for (int i=0; i<m_Event->getBuffer().size(); i++) {

		m_ItemState.append(true);
		
		QTableWidgetItem *item = new QTableWidgetItem(m_Event->getBuffer().at(i));
		
		m_ValueTable->setItem(i,0,item);
	}

	// connect signal here to avoid access to not initialized widgets
	connect(m_ActiveCheckBox,SIGNAL(toggled(bool)),this,SLOT(enableActionChanged(bool)));

	setLayout(m_MainLayout);	
}

EditEventDialog::~EditEventDialog()
{
	saveConfig();
}

void EditEventDialog::accept() {
	
	m_Event->setName(m_NameLineEdit->text());
	m_Event->setActive(m_ActiveCheckBox->isChecked());
	
	QStringList Values;
	int i=0;
	for (i=0; i<m_ValueTable->rowCount(); i++) {
		if (m_ValueTable->item(i,0) != 0) {
			Values << m_ValueTable->item(i,0)->text();
		} else {
			Values << "";
		}
	}
	m_Event->setBuffer(Values);	
	
	switch (m_MethodComboBox->currentIndex()) {
		case 0: m_Event->setAction(HightlightBytesInBuffer);
				break;
		
	}
	
	m_Event->setColor(m_Color);
	
	QDialog::accept();
}

void EditEventDialog::textChanged(const QString & text) {
	
	m_NameExist = false;
	
	for (int i=0; i<m_EventList->size(); i++) {
		if (m_EventList->at(i).getName() == text) {
			m_NameExist = true;
		}
	}
	
	AddButtonState();
} 

void EditEventDialog::AddButtonState() {
	
	int Errors = 0;
	
	for (int i=0; i<m_ItemState.size(); i++) {
		if (!m_ItemState[i]) Errors++;
	}	
	
	m_AddButton->setEnabled((m_NameLineEdit->text().length() > 0) && (Errors == 0) && (!m_NameExist));
}

void EditEventDialog::SizeChanged(int s) {
	
	if (s < m_ValueTable->rowCount()) {
		
		while (s < m_ValueTable->rowCount()) {
			m_ValueTable->removeRow(m_ValueTable->rowCount()-1);
			m_ItemState.removeLast();
		}
		
	} else {
		
		while (s > m_ValueTable->rowCount()) {
			m_ValueTable->insertRow(m_ValueTable->rowCount());
			m_ItemState.append(true);
		}
	}
}

void EditEventDialog::closeEvent(QCloseEvent *event)
{
    if (true) {
        event->accept();
    } else {
        event->ignore();
    }
}

void EditEventDialog::TableItemChanged(QTableWidgetItem *item) {

	QString value = item->text();
	
	if (value.length() > 0) {
	
		bool ok = false;
		int ivalue = 0;
		
		if (value.startsWith("0x")) {
			ivalue = value.toInt(&ok,16);
		} else {
			ivalue = value.toInt(&ok,10);	
		}
	
		QBrush bgbrush; 
		bgbrush.setStyle(Qt::SolidPattern);
			
		if (ok && (ivalue >= 0) && (ivalue <= 255)) { 
			bgbrush.setColor(Qt::white);
			m_ItemState[item->row()] = true;
		} else {
			bgbrush.setColor(QColor(255,224,224));
			item->setSelected(false);
			m_ItemState[item->row()] = false;
		} 
		item->setBackground(bgbrush); 
		AddButtonState();
	}	
}

void EditEventDialog::chooseColor() {
	
	QColor color = QColorDialog::getColor(Qt::green, this);
	
	if (color.isValid()) {
		m_Color = color;
		m_ColorLabel->setPalette(QPalette(color));
       	m_ColorLabel->setAutoFillBackground(true);
   	}
}

void EditEventDialog::enableActionChanged(bool state) {
	
	m_MethodComboBox->setEnabled(state);
	m_ColorButton->setEnabled(state);
}

void EditEventDialog::saveConfig() {
	
	QSettings settings;
	
	settings.beginGroup("EditEventDialog");
	settings.setValue("pos",pos());
	settings.endGroup();
}

void EditEventDialog::loadConfig() {
	
	QSettings settings;
	
	settings.beginGroup("EditEventDialog");
	if (settings.contains("pos")) {
		QPoint pos = settings.value("pos").toPoint();
		move(pos);
	}
	settings.endGroup();
}
