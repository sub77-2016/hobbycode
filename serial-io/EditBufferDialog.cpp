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
#include "EditBufferDialog.h"
#include "InitialValueDialog.h"

using namespace std;


EditBufferDialog::EditBufferDialog(ValueBuffer *Buffer, bool Mode, QList<ValueBuffer> *BufferList) : QDialog()
{
	m_ValueBuffer = Buffer;
	
	m_EditMode = Mode;
	
	m_BufferList = BufferList;
	
	loadConfig();
	
	if (m_EditMode == true) setWindowTitle(tr("New Buffer"));
	else setWindowTitle(tr("Edit Buffer"));
	
    m_MainLayout = new QVBoxLayout();
	setLayout(m_MainLayout);	

	/***** Name *****/
    m_NameLayout = new QHBoxLayout();
    m_MainLayout->addLayout(m_NameLayout);
    
    m_NameLayout->addWidget(new QLabel("Name:"));
    m_NameLineEdit = new QLineEdit();
    m_NameLineEdit->setText(m_ValueBuffer->getName());
    connect(m_NameLineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(textChanged(const QString &)));
    m_NameLayout->addWidget(m_NameLineEdit);
    
    /***** Type *****/
    m_TypeLayout = new QHBoxLayout(); 
    m_MainLayout->addLayout(m_TypeLayout);
    
    m_TypeLayout->addWidget(new QLabel("Type:"));
    
    QStringList TypeList;
    TypeList << "None";
    TypeList << "Character";
    TypeList << "Number";
    
    m_TypeCB = new QComboBox();
    m_TypeCB->addItems(TypeList);
    m_TypeCB->setCurrentIndex(m_ValueBuffer->getType());
    connect(m_TypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(changedType(int)));
    m_TypeLayout->addWidget(m_TypeCB);
    
    /***** Width *****/
    m_WidthLayout = new QHBoxLayout();
    m_MainLayout->addLayout(m_WidthLayout);

    m_WidthLayout->addWidget(new QLabel("Width:"));
    
    QStringList WidthList;
    WidthList << "8 Bit";
    WidthList << "16 Bit";
    WidthList << "32 Bit";
    
    m_WidthCB = new QComboBox();
    m_WidthCB->addItems(WidthList);
    m_WidthCB->setCurrentIndex(m_ValueBuffer->getWidth());
    m_WidthCB->setEnabled(m_ValueBuffer->getType() == ValueBuffer::Number);
    m_WidthLayout->addWidget(m_WidthCB);
    
    /***** Sign *****/
    m_SignLayout = new QHBoxLayout();
    m_MainLayout->addLayout(m_SignLayout);
    
    m_SignLayout->addWidget(new QLabel("Sign:"));
    
    QStringList SignList;
    SignList << "unsigned";
    SignList << "signed";
    
    m_SignCB = new QComboBox();
    m_SignCB->addItems(SignList);
    m_SignCB->setCurrentIndex(m_ValueBuffer->getByteOrder());
    m_SignCB->setEnabled(m_ValueBuffer->getType() == ValueBuffer::Number);
    m_SignLayout->addWidget(m_SignCB);
    
    /***** Byte Order *****/
    m_ByteOrderLayout = new QHBoxLayout();
    m_MainLayout->addLayout(m_ByteOrderLayout);
    
    m_ByteOrderLayout->addWidget(new QLabel("Byte Order:"));
    
    QStringList ByteOrderList;
    ByteOrderList << "little endian";
    ByteOrderList << "big endian";

    m_ByteOrderCB = new QComboBox();
    m_ByteOrderCB->addItems(ByteOrderList);
    m_ByteOrderCB->setEnabled(m_ValueBuffer->getType() == ValueBuffer::Number);
    m_ByteOrderLayout->addWidget(m_ByteOrderCB);
    
    /***** Size *****/
    m_SizeLayout = new QHBoxLayout();
    m_MainLayout->addLayout(m_SizeLayout);
    
    m_SizeLayout->addWidget(new QLabel("Size:"));
    m_SizeLayout->addStretch();
    m_SizeSpinBox = new QSpinBox(this);
    m_SizeSpinBox->setMinimum(1);
    m_SizeSpinBox->setMaximum(1024);
    m_SizeSpinBox->setValue(m_ValueBuffer->getBuffer().size());
    connect(m_SizeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(SizeChanged(int)));
    m_SizeLayout->addWidget(m_SizeSpinBox); 
    
    m_InitButton = new QPushButton("Init");
    connect(m_InitButton, SIGNAL(clicked()), this, SLOT(InitBuffer()));
    m_SizeLayout->addWidget(m_InitButton,0);
    
    /***** Table *****/
    QStringList ValueTableHeader;
   	ValueTableHeader << "Values";
    
    m_ValueTable = new QTableWidget(m_ValueBuffer->getBuffer().size(),ValueTableHeader.size());
	m_ValueTable->setHorizontalHeaderLabels(ValueTableHeader);
	m_ValueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_ValueTable->setAlternatingRowColors(true);
	m_ValueTable->setShowGrid(false);
	
	for (int i=0; i<m_ValueBuffer->getBuffer().size(); i++) {
		
		QTableWidgetItem *item = new QTableWidgetItem(m_ValueBuffer->getBuffer().at(i));
		
		m_ValueTable->setItem(i,0,item);
	}
	
	m_ValueTable->resizeColumnsToContents(); 
    m_MainLayout->addWidget(m_ValueTable);

    /***** Checksum *****/
	m_CheckSumLayout = new QHBoxLayout();
	m_MainLayout->addLayout(m_CheckSumLayout);
	
	m_CalcCheckBox = new QCheckBox("Calc check sum");
	m_CalcCheckBox->setChecked(m_ValueBuffer->getCalcChecksum());
	m_CheckSumLayout->addWidget(m_CalcCheckBox);
	
	QStringList MethodList;
	
	MethodList << "(Sum without carry)-1";
	MethodList << "XOR";
	
	m_MethodComboBox = new QComboBox();
	m_MethodComboBox->addItems(MethodList);
	m_MethodComboBox->setCurrentIndex(m_ValueBuffer->getChecksumType());
	m_CheckSumLayout->addWidget(m_MethodComboBox);

	m_MainLayout->addSpacing(10);

	/***** Buttons *****/
	m_ButtonBox = new QDialogButtonBox();

	QString AddButtonLabel;
	
	if (m_EditMode == true) AddButtonLabel = "Add";
	else AddButtonLabel = "Change";

	m_AddButton = new QPushButton(AddButtonLabel);
	m_AddButton->setEnabled(m_NameLineEdit->text().length() > 0);
    connect(m_ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	m_ButtonBox->addButton(m_AddButton,QDialogButtonBox::AcceptRole);
	
	m_CancelButton = new QPushButton("Cancel");                       
    connect(m_ButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
	m_ButtonBox->addButton(m_CancelButton,QDialogButtonBox::RejectRole);
    
    m_MainLayout->addWidget(m_ButtonBox);
}


EditBufferDialog::~EditBufferDialog()
{
	saveConfig();
}

void EditBufferDialog::accept() {
	
	m_ValueBuffer->setName(m_NameLineEdit->text());
	m_ValueBuffer->setCalcChecksum(m_CalcCheckBox->isChecked());

	QStringList Values;
	int i=0;
	for (i=0; i<m_ValueTable->rowCount(); i++) {
		if (m_ValueTable->item(i,0) != 0) {
			Values << m_ValueTable->item(i,0)->text();
		} else {
			Values << "";
		}
	}
	m_ValueBuffer->setBuffer(Values);	
	
	switch (m_MethodComboBox->currentIndex()) {
	case 0: m_ValueBuffer->setChecksumType(ValueBuffer::SumWithoutCarryMinusOne);
			break;
	case 1: m_ValueBuffer->setChecksumType(ValueBuffer::ByteXOR);
			break;
	}
	
	switch (m_TypeCB->currentIndex()) {
	case 0: m_ValueBuffer->setType(ValueBuffer::None);
			break;
	case 1: m_ValueBuffer->setType(ValueBuffer::Character);
			break;
	case 2: m_ValueBuffer->setType(ValueBuffer::Number);
			break;
	}

	switch (m_WidthCB->currentIndex()) {
	case 0: m_ValueBuffer->setWidth(ValueBuffer::Bits_8); 
			break;
	case 1: m_ValueBuffer->setWidth(ValueBuffer::Bits_16);
			break;
	case 2: m_ValueBuffer->setWidth(ValueBuffer::Bits_32);
			break;
	}

	switch (m_SignCB->currentIndex()) {
	case 0: m_ValueBuffer->setSigned(false);
			break;
	case 1: m_ValueBuffer->setSigned(true);
			break;
	}

	switch (m_ByteOrderCB->currentIndex()) {
	case 0: m_ValueBuffer->setByteOrder(ValueBuffer::LittleEndian);
			break;
	case 1: m_ValueBuffer->setByteOrder(ValueBuffer::BigEndian);
			break;
	}
	
	QDialog::accept();
}

void EditBufferDialog::textChanged(const QString & text) {
	
	bool NameExist = false;
	
	for (int i=0; i<m_BufferList->size(); i++) {
		if (m_BufferList->at(i).getName() == text) {
			NameExist = true;
			break;
		}
	}
	
	m_AddButton->setEnabled((text.length() > 0) && !NameExist);
} 

void EditBufferDialog::SizeChanged(int s) {
	
	if (s < m_ValueTable->rowCount()) {
		
		while (s < m_ValueTable->rowCount()) {
			m_ValueTable->removeRow(m_ValueTable->rowCount()-1);
		}
		
	} else {
		
		while (s > m_ValueTable->rowCount()) {
			m_ValueTable->insertRow(m_ValueTable->rowCount());
		}
	}
}

void EditBufferDialog::changedType(int idx) {
	
	m_WidthCB->setEnabled(idx == 2);
	m_SignCB->setEnabled(idx == 2);
	m_ByteOrderCB->setEnabled(idx == 2); 
}

void EditBufferDialog::closeEvent(QCloseEvent *event)
{
    if (true) {
        event->accept();
    } else {
        event->ignore();
    }
}

void EditBufferDialog::InitBuffer() {

	InitialValueDialog dialog;
	
	dialog.exec();
	
	if (dialog.result() == QDialog::Accepted) {
		
		int StartValue = dialog.getStartValue();
		int Increment = dialog.getIncrement();
		
		for (int i=0; i<m_ValueTable->rowCount(); i++) {
			
			QTableWidgetItem *item = new QTableWidgetItem(QString("%1").arg(StartValue));
		
			m_ValueTable->setItem(i,0,item);
			
			StartValue += Increment;
		}
	}
}
void EditBufferDialog::saveConfig() {
	
	QSettings settings;
	
	settings.beginGroup("EditBufferDialog");
	settings.setValue("pos",pos());
	settings.endGroup();
}

void EditBufferDialog::loadConfig() {
	
	QSettings settings;
	
	settings.beginGroup("EditBufferDialog");
	if (settings.contains("pos")) {
		QPoint pos = settings.value("pos").toPoint();
		move(pos);
	}
	settings.endGroup();
}
