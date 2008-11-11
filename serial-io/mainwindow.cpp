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
#include <vector>
#include <QtGui>
#include <QFile> 

#include "mainwindow.h"
#include "DeviceDialog.h"
#include "EditBufferDialog.h"
#include "EditEventDialog.h"
#include "ValueBuffer.h"
#include "Event.h"
#include "OptionsDlg.h"
#include "RxTxValue.h"
#include "SaveBufferDlg.h"
#include "Dialog.h"
#include "EditDialogDialog.h"
#include "VariableDialog.h"
 
using namespace std;


MainWindow::MainWindow() {
	
    setWindowTitle(tr("Serial I/O Manager"));
    
	m_ProjectSaved = true;
	StreamIsOpen = false;
	SendCount = 0;
	ReceivedCount = 0;
	
	m_TxBufferSize = 1024;
	m_RxBufferSize = 1024;
	m_ThreadSleep = 100;
	m_RxTxColumnCount = 8;
	m_RxTxValueRepresentation = 0;
	m_ScrollToBottom = true;
	m_SelectLastValues = false;
	
	m_LastTimestamp = "";
	
	m_InputHasErrors = false;
	
	m_NumberRegExp.setPattern("[0-9a-fA-F x\\-]*");
	
	// read application settings
    readSettings();


    // GUI stuff
    createActions();
    
    QWidget *centralWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    createMenus();
    createToolBars();
    createStatusBar();

	setMenuBar(menuBar);
	addToolBar(fileToolBar);

	createTypeBox();
	mainLayout->addWidget(horizontalGroupBox);
	
	createWidthBox();
	mainLayout->addWidget(horizontalGroupBox3);

	createSignBox();
	mainLayout->addWidget(horizontalGroupBox4);
	
	createByteOrderBox();
	mainLayout->addWidget(horizontalGroupBox2);
	
	createCRCBox();
	mainLayout->addWidget(horizontalGroupBox5);
	
	/*****/
	
	QSplitter *Splitter = new QSplitter(this);
	Splitter->setOrientation(Qt::Vertical);
	mainLayout->addWidget(Splitter,1);
	
	/*****/
	
	QStringList TTHeader;
	TTHeader << "Timestamp";
	for (unsigned int i=0; i<m_RxTxColumnCount; i++) {
		TTHeader << QString("%1").arg(i);
	}
	TTHeader << "Characters";
	
	TranceiverTable = new QTableWidget(0,TTHeader.size(),this);
	TranceiverTable->setHorizontalHeaderLabels(TTHeader);
	TranceiverTable->setGridStyle(Qt::NoPen);
	TranceiverTable->resizeColumnsToContents();
	TranceiverTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	TranceiverTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	Splitter->addWidget(TranceiverTable);
	
	// LineEdit
	
	m_LineEditFrame = new QFrame();
	
	m_LineEditVLayout = new QVBoxLayout();
	m_LineEditFrame->setLayout(m_LineEditVLayout);
	
	m_HistoryCB = new QComboBox();
	m_HistoryCB->addItem("");
	m_HistoryCB->setEditable(true);
	m_HistoryCB->setEnabled(false);
	
	m_LineEditHLayout = new QHBoxLayout();
	m_LineEditVLayout->addLayout(m_LineEditHLayout);
	
	m_LineEdit = m_HistoryCB->lineEdit();
	m_LineEdit->setValidator(new QRegExpValidator(m_NumberRegExp,m_LineEdit));
	connect(m_LineEdit,SIGNAL(returnPressed()),this,SLOT(returnPressed()));
	connect(m_LineEdit,SIGNAL(textEdited(const QString &)),this,SLOT(characterTyped(const QString &)));
	m_LineEditHLayout->addWidget(m_HistoryCB,1);
	
	m_SendButton = new QPushButton("send");
	m_SendButton->setEnabled(false);
	m_LineEditHLayout->addWidget(m_SendButton);
	connect(m_SendButton,SIGNAL(clicked()),this,SLOT(sendClicked()));
	
	QHBoxLayout *m_AppendLayout = new QHBoxLayout();
	m_LineEditVLayout->addLayout(m_AppendLayout);
	
	m_AppendBufferCB = new QCheckBox("Append buffer:");
	m_AppendBufferCB->setEnabled(false);
	connect(m_AppendBufferCB,SIGNAL(stateChanged(int)),this,SLOT(appendBufferChanged(int)));
	m_AppendLayout->addWidget(m_AppendBufferCB);

	
	m_BufferCB = new QComboBox();
	m_BufferCB->setEnabled(false);
	m_BufferCB->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_AppendLayout->addWidget(m_BufferCB);

	m_AppendLayout->addStretch();
	m_LineEditVLayout->addStretch();
	
	m_ErrorLabel = new QLabel();
	m_LineEditVLayout->addWidget(m_ErrorLabel,1);

	// Buffer
	m_BufferFrame = new QFrame();
	m_BufferHLayout = new QHBoxLayout;
	m_BufferFrame->setLayout(m_BufferHLayout);

	QStringList BufferHeader;
	BufferHeader << "Name";
	BufferHeader << "Number of bytes";
	
	m_BufferTable = new QTableWidget(0,BufferHeader.size(),m_BufferFrame);
	m_BufferTable->setHorizontalHeaderLabels(BufferHeader);
	m_BufferTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_BufferTable->setAlternatingRowColors(true);
	m_BufferTable->setShowGrid(false);
	m_BufferTable->resizeColumnsToContents();
	connect(m_BufferTable,SIGNAL(itemSelectionChanged()),this,SLOT(BufferItemSelectionChanged()));
	m_BufferHLayout->addWidget(m_BufferTable);
	
	m_BufferVLayout = new QVBoxLayout();
	m_BufferHLayout->addLayout(m_BufferVLayout);
	
	m_BufferAddButton = new QPushButton("Add");
	connect(m_BufferAddButton,SIGNAL(clicked()),this,SLOT(addBuffer()));
	m_BufferVLayout->addWidget(m_BufferAddButton);

	m_BufferRemoveButton = new QPushButton("Remove");
	m_BufferRemoveButton->setEnabled(false);
	connect(m_BufferRemoveButton,SIGNAL(clicked()),this,SLOT(removeBuffer()));
	m_BufferVLayout->addWidget(m_BufferRemoveButton);

	m_BufferEditButton = new QPushButton("Edit");
	m_BufferEditButton->setEnabled(false);
	connect(m_BufferEditButton,SIGNAL(clicked()),this,SLOT(editBuffer()));
	m_BufferVLayout->addWidget(m_BufferEditButton);

	m_BufferSendButton = new QPushButton("Send");
	m_BufferSendButton->setEnabled(false);
	connect(m_BufferSendButton,SIGNAL(clicked()),this,SLOT(sendBuffer()));
	m_BufferVLayout->addWidget(m_BufferSendButton);

	m_BufferVLayout->addStretch();
	
	// Event
	
	m_EventFrame = new QFrame();
	m_EventHLayout = new QHBoxLayout;
	m_EventFrame->setLayout(m_EventHLayout);

	QStringList EventHeader;
	EventHeader << "Name";
	EventHeader << "State";
	EventHeader << "Action";
	
	m_EventTable = new QTableWidget(0,EventHeader.size(),m_EventFrame);
	m_EventTable->setHorizontalHeaderLabels(EventHeader);
	m_EventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_EventTable->setAlternatingRowColors(true);
	m_EventTable->setShowGrid(false);
	m_EventTable->resizeColumnsToContents();
	connect(m_EventTable,SIGNAL(itemSelectionChanged()),this,SLOT(EventItemSelectionChanged()));
	m_EventHLayout->addWidget(m_EventTable);
	
	m_EventVLayout = new QVBoxLayout();
	m_EventHLayout->addLayout(m_EventVLayout);
	
	m_EventAddButton = new QPushButton("Add");
	connect(m_EventAddButton,SIGNAL(clicked()),this,SLOT(addEvent()));
	m_EventVLayout->addWidget(m_EventAddButton);

	m_EventRemoveButton = new QPushButton("Remove");
	m_EventRemoveButton->setEnabled(false);
	connect(m_EventRemoveButton,SIGNAL(clicked()),this,SLOT(removeEvent()));
	m_EventVLayout->addWidget(m_EventRemoveButton);

	m_EventEditButton = new QPushButton("Edit");
	m_EventEditButton->setEnabled(false);
	connect(m_EventEditButton,SIGNAL(clicked()),this,SLOT(editEvent()));
	m_EventVLayout->addWidget(m_EventEditButton);
	m_EventVLayout->addStretch();

	// Dialogs
	
	m_DialogFrame = new QFrame();
	m_DialogHLayout = new QHBoxLayout;
	m_DialogFrame->setLayout(m_DialogHLayout);

	QStringList DialogHeader;
	DialogHeader << "Name";
	
	m_DialogTable = new QTableWidget(0,DialogHeader.size(),m_DialogFrame);
	m_DialogTable->setHorizontalHeaderLabels(DialogHeader);
	m_DialogTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_DialogTable->setAlternatingRowColors(true);
	m_DialogTable->setShowGrid(false);
	m_DialogTable->resizeColumnsToContents();
	connect(m_DialogTable,SIGNAL(itemSelectionChanged()),this,SLOT(DialogItemSelectionChanged()));
	m_DialogHLayout->addWidget(m_DialogTable);
	
	m_DialogVLayout = new QVBoxLayout();
	m_DialogHLayout->addLayout(m_DialogVLayout);
	
	m_DialogAddButton = new QPushButton("Add");
	connect(m_DialogAddButton,SIGNAL(clicked()),this,SLOT(addDialog()));
	m_DialogVLayout->addWidget(m_DialogAddButton);

	m_DialogRemoveButton = new QPushButton("Remove");
	m_DialogRemoveButton->setEnabled(false);
	connect(m_DialogRemoveButton,SIGNAL(clicked()),this,SLOT(removeDialog()));
	m_DialogVLayout->addWidget(m_DialogRemoveButton);

	m_DialogEditButton = new QPushButton("Edit");
	m_DialogEditButton->setEnabled(false);
	connect(m_DialogEditButton,SIGNAL(clicked()),this,SLOT(editDialog()));
	m_DialogVLayout->addWidget(m_DialogEditButton);

	m_DialogOpenButton = new QPushButton("Open");
	m_DialogOpenButton->setEnabled(false);
	connect(m_DialogOpenButton,SIGNAL(clicked()),this,SLOT(openDialog()));
	m_DialogVLayout->addWidget(m_DialogOpenButton);

	m_DialogVLayout->addStretch();
	
	
	// Tab
	m_Tab = new QTabWidget(this);
	m_Tab->setTabPosition(QTabWidget::South);
	m_Tab->addTab(m_LineEditFrame,"Command Line");
	m_Tab->addTab(m_BufferFrame,"Buffers");
	m_Tab->addTab(m_EventFrame,"Events");
	m_Tab->addTab(m_DialogFrame,"Dialogs");
	
	Splitter->addWidget(m_Tab);

	/****/

	setStatusBar(statusBar);

    viewTools();
    viewSendType();
    viewWidth();
    viewSign();
    viewByteOrder();
    viewCRC();
    
    loadLastConfig();
}

void MainWindow::loadLastConfig() {
	
	if (m_LoadLastConfigOnStartup) {
		
		if (m_recentFiles.size() > 0) {

			QFileInfo finfo(m_recentFiles.at(0));
			
			if (finfo.exists()) {

				loadProjectFile(m_recentFiles.at(0));
			}
		}
	}
}

void MainWindow::closeEvent(QCloseEvent *event) {
     if (maybeSave()) {
         writeSettings();
         event->accept();
     } else {
         event->ignore();
     }
}

void MainWindow::characterTyped(const QString & text) {

	checkInput(text);
}

void MainWindow::checkInput(QString s) {
	
	QString errorStr = "";
	int errors = 0;

	if (TypeButtonId == 1) {	


		QStringList list = QString(s).split(" ",QString::SkipEmptyParts);

		for (int i=0; i<list.size(); i++) {
			if (!checkNumber(list.at(i))) {

				if (errors > 0) errorStr += ",";
				errorStr += QString().number(i);
				errors++;
			}
		}

		if (errors > 0) {
			if (errors == 1) {
				errorStr = "The value " + errorStr + " exceeds limits!";
			} else {
				errorStr = "The values " + errorStr + " exceeds limits!";
			}
		}
	}
	m_ErrorLabel->setText(errorStr);
	m_SendButton->setEnabled(errors == 0);
	m_InputHasErrors = errors != 0;
}

bool MainWindow::checkNumber(QString s) {
	
	bool result = false;
	
	if (SignButtonId == 0) {

		unsigned long long int v = QString(s).toULongLong(&result,0);
		
		if (result) {
			if (WidthButtonId == 0) {
				if (v > 255) result = false;
			} else if (WidthButtonId == 1) {
				if (v > 65535) result = false;
			} else if (WidthButtonId == 2) {
				if (v > 4294967295LL) result = false;
			}
		}
	} else {
		
		signed long long int v = QString(s).toLongLong(&result,0);

		if (result) {
			if (WidthButtonId == 0) {
				if ((v < -128) || (v > 127)) result = false;
			} else if (WidthButtonId == 1) {
				if ((v < -32768) || (v > 32768)) result = false;
			} else if (WidthButtonId == 2) {
				if ((v < -2147483648LL) || (v > 2147483648LL)) result = false;
			}
		}
	}
	return result;
}

void MainWindow::returnPressed() {
	
	if (!m_InputHasErrors) {
		prepareDataAndSend();
	}
}

void MainWindow::sendClicked() {
	
	prepareDataAndSend();
}

void MainWindow::prepareDataAndSend() {
	
	QStringList SList;
	SList << m_LineEdit->text();

	ValueBuffer CmdBuffer;
	CmdBuffer.setName("Cmd");
	CmdBuffer.setBuffer(SList);

	if (CRCButtonId > 0) {
		CmdBuffer.setCalcChecksum(true);
		if (CRCButtonId == 1) {
			CmdBuffer.setChecksumType(ValueBuffer::SumWithoutCarryMinusOne);
		} else if (CRCButtonId == 2) {
			CmdBuffer.setChecksumType(ValueBuffer::ByteXOR);
		}
	}

	setBufferAttributes(&CmdBuffer);

	if (sendData(CmdBuffer)) {

		if (m_AppendBufferCB->isChecked()) {

			QString BufferName = m_BufferCB->currentText();

			int idx=-1;
			for (int i=0; i<m_BufferList.size(); i++) {

				if (m_BufferList.at(i).getName() == BufferName) {
					idx = i;
				}
			}

			if (idx != -1) {
				if (sendData(m_BufferList.at(idx))) {
				}
			}
		}

		m_LineEdit->setText("");
	}
}

void MainWindow::setBufferAttributes(ValueBuffer *Buffer) {
	
	switch (TypeButtonId) {
	case 0: Buffer->setType(ValueBuffer::Character); 
			break;
	case 1: Buffer->setType(ValueBuffer::Number);
	}
	
	switch (WidthButtonId) {
	case 0: Buffer->setWidth(ValueBuffer::Bits_8);
			break;
	case 1: Buffer->setWidth(ValueBuffer::Bits_16);
			break;
	case 2: Buffer->setWidth(ValueBuffer::Bits_32);
			break;
	}
	
	switch (SignButtonId) {
	case 0: Buffer->setSigned(false);
			break;
	case 1: Buffer->setSigned(true);
			break;
	}
	
	switch (OrderButtonId) {
	case 0: Buffer->setByteOrder(ValueBuffer::LittleEndian);
			break;
	case 1: Buffer->setByteOrder(ValueBuffer::BigEndian);
			break;
	}
}

void MainWindow::appendBufferChanged(int state) {
	
	m_BufferCB->setEnabled(state == Qt::Checked);
}

void MainWindow::appendBufferToTransceiverTable(QString Ts, QByteArray Buffer, e_ValueType Type) {

	unsigned int col = 0;
	unsigned int row = TranceiverTable->rowCount();
	unsigned int newRows = 0;
	
	if (m_SelectLastValues) {
		TranceiverTable->clearSelection();
	}
	
	for (int i=0; i<Buffer.size(); i++) {
		
		if (Type == TX_VALUE) SendCount++;
		else ReceivedCount++;
	
		if ((unsigned int) TranceiverTable->rowCount() < (row+1)) {
			TranceiverTable->insertRow(row);
			
			QTableWidgetItem *item = new QTableWidgetItem();
			QVariant v(Ts);
			item->setData(Qt::DisplayRole,v);
			item->setFlags(item->flags() & (~Qt::ItemIsEditable));
			if (i != 0) {
				QBrush brush;
				brush.setStyle(Qt::SolidPattern);
				brush.setColor(Qt::lightGray);
				item->setForeground(brush);
			}
			TranceiverTable->setItem(row,0,item);
			
			item = new QTableWidgetItem();
			QVariant c = QString("");
			
			item->setData(Qt::DisplayRole,c);
			item->setFlags(item->flags() & (~Qt::ItemIsEditable));
			TranceiverTable->setItem(row,m_RxTxColumnCount+1,item);
			
			newRows++;
		}
	
		unsigned char value = Buffer[i];

		TranceiverTable->setItem(row,col+1,newTableWidgetItem(value,Type));
		
		RxTxValue RTVal;
		RTVal.setValues(Ts,value,Type,row,col+1);
		
		RxTxValueBuffer.push_back(RTVal);

		QTableWidgetItem *item = TranceiverTable->item(row,m_RxTxColumnCount+1);
				
		if (item) {
			QString s = item->data(Qt::DisplayRole).toString();
			s.append(value);
			item->setText(s);
		}
		
		if (col == (m_RxTxColumnCount-1)) {
			col = 0;
			row++;
		} else {
			col++;
		}
		
		if (Type == RX_VALUE) { 
	 		checkEvents();	
		}
	}

	if (m_SelectLastValues) {
		QTableWidgetSelectionRange TSRange(TranceiverTable->rowCount()-newRows, 0, TranceiverTable->rowCount()-1, m_RxTxColumnCount );
	
		TranceiverTable->setRangeSelected(TSRange,true);
	}
	
	if (Type == TX_VALUE) TxBufferDeleteFirstBytes();
	else RxBufferDeleteFirstBytes();
	
	TranceiverTable->resizeColumnsToContents();
	if (m_ScrollToBottom) TranceiverTable->scrollToBottom();
}

QTableWidgetItem *MainWindow::newTableWidgetItem(unsigned int ival, e_ValueType Type) {
	
	QTableWidgetItem *item = new QTableWidgetItem();
	
	QVariant var(getValueRepresentation(ival));
	item->setData(Qt::DisplayRole,var);
	
	QBrush brush;
	brush.setStyle(Qt::SolidPattern);
	if (Type == TX_VALUE) {
		brush.setColor(m_TxColor);
	} else {
		brush.setColor(m_RxColor);
	}
	
	item->setForeground(brush);
	
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));
	
	return item;
}

QString MainWindow::getValueRepresentation(unsigned int ival) {
	QString s;

	if (m_RxTxValueRepresentation == 0) {
		s = QString("%1").arg(ival);
	} else {
		s = QString("%1").arg(ival,2,16);
		if (s[0] == ' ') s[0] = '0';
		s = s.toUpper();
	}
	
	return s;	
}

void MainWindow::TxBufferDeleteFirstBytes() {
	
	unsigned int c=0;
	while (SendCount > m_TxBufferSize) {
	
		if (RxTxValueBuffer.at(c).Type() == TX_VALUE) {
			
			TranceiverTable->setItem(RxTxValueBuffer.at(c).Row(),RxTxValueBuffer.at(c).Column(),0);
			
			if (((RxTxValueBuffer.at(c).Column()-1) == (m_RxTxColumnCount-1)) || 
				(TranceiverTable->item(RxTxValueBuffer.at(c).Row(),RxTxValueBuffer.at(c).Column()+1) == 0)) {
					
				TranceiverTable->removeRow(RxTxValueBuffer.at(c).Row());
				
				for (unsigned int idx=c; idx<RxTxValueBuffer.size(); idx++) {
					RxTxValueBuffer[idx].decrementRow();
				}				
			}
			
			RxTxValueBuffer.erase(RxTxValueBuffer.begin());
			
			SendCount--;
		} else {
			c++;
		}
	}
}

void MainWindow::RxBufferDeleteFirstBytes() {
	
	unsigned int c=0;
	while (ReceivedCount > m_RxBufferSize) {
	
		if (RxTxValueBuffer.at(c).Type() == RX_VALUE) {
			
			TranceiverTable->setItem(RxTxValueBuffer.at(c).Row(),RxTxValueBuffer.at(c).Column(),0);
			
			if (((RxTxValueBuffer.at(c).Column()-1) == (m_RxTxColumnCount-1)) || 
				(TranceiverTable->item(RxTxValueBuffer.at(c).Row(),RxTxValueBuffer.at(c).Column()+1) == 0)) {
					
				TranceiverTable->removeRow(RxTxValueBuffer.at(c).Row());
				
				for (unsigned int idx=c; idx<RxTxValueBuffer.size(); idx++) {
					RxTxValueBuffer[idx].decrementRow();
				}				
			}
			
			RxTxValueBuffer.erase(RxTxValueBuffer.begin());
			
			ReceivedCount--;
		} else {
			c++;
		}
	}
}


bool MainWindow::sendData(ValueBuffer Buffer) {

	QString Timestamp;

	bool stateOk = true;
	
	QByteArray ByteBuffer;

	stateOk = prepareBuffer(Buffer,ByteBuffer);
	
	if (stateOk) {
		
		if (Buffer.getCalcChecksum()) {
			calcChecksum(ByteBuffer,Buffer.getChecksumType());
		} 
		
		Timestamp = getTimestamp();
		stateOk = serialThread->sendData(ByteBuffer);
	}
	
	if (stateOk) {
		appendBufferToTransceiverTable(Timestamp,ByteBuffer,TX_VALUE);
	}

	return stateOk;	
}


void MainWindow::calcChecksum(QByteArray &Buffer, ValueBuffer::e_ChecksumType Type) {
	
	unsigned char chksum = 0;
	
	if (Type == ValueBuffer::SumWithoutCarryMinusOne) {
		
		for (int i=0; i<Buffer.size(); i++) {

			chksum += (unsigned char) Buffer[i];		
		}
		chksum -= 1;
		
	} else if (Type == ValueBuffer::ByteXOR) {
		
		for (int i=0; i<Buffer.size(); i++) {

			chksum = chksum ^ ((unsigned char) Buffer[i]);		
		}
	}
	
	Buffer.append(chksum);
}


bool MainWindow::prepareBuffer(ValueBuffer Buffer, QByteArray & ByteBuffer)
{
	bool state=false;
	
	if (Buffer.getType() == ValueBuffer::Character) {

		QStringList BList = Buffer.getBuffer();
		
		for (int i=0; i<BList.size(); i++) {
			ByteBuffer.append(BList.at(i));
		}
		
		state = true;
			
	} else if (Buffer.getType() == ValueBuffer::Number) {
		QStringList List;
		
		QStringList BList = Buffer.getBuffer();
		
		for (int i=0; i<BList.size(); i++) {
			List << splitInput(BList.at(i));
		}
		
		if (Buffer.getSigned() == false) {
			vector<unsigned int> v;
			
			if (Buffer.getWidth() == ValueBuffer::Bits_8) {
				if (unsignedAnalyseInput(List,v)) {
					if (checkUnsignedBounds(255,v)) {
						for (unsigned int i=0; i < v.size(); i++) {
							ByteBuffer.append((unsigned char) v[i]);
						}
						state = true;
					}
				}
			} else if (Buffer.getWidth() == ValueBuffer::Bits_16) {
				if (unsignedAnalyseInput(List,v)) {
					if (checkUnsignedBounds(65535,v)) {
						if (Buffer.getByteOrder() == ValueBuffer::LittleEndian) {
							for (unsigned int i=0; i < v.size(); i++) {
								ByteBuffer.append((unsigned char) (v[i] & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 8) & 0xFF));
							}
						} else {
							// big endian
							for (unsigned int i=0; i < v.size(); i++) {
								ByteBuffer.append((unsigned char) ((v[i] >> 8) & 0xFF));
								ByteBuffer.append((unsigned char) (v[i] & 0xFF));
							}
						}
						state = true;
					}
				}
			} else if (Buffer.getWidth() == ValueBuffer::Bits_32) {
				if (unsignedAnalyseInput(List,v)) {
					if (checkUnsignedBounds(4294967295UL,v)) {
						if (Buffer.getByteOrder() == ValueBuffer::LittleEndian) {
							for (unsigned int i=0; i < v.size(); i++) {
								ByteBuffer.append((unsigned char) (v[i] & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 8) & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 16) & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 24) & 0xFF));
							}
						} else {
							// big endian
							for (unsigned int i=0; i < v.size(); i++) {
								ByteBuffer.append((unsigned char) ((v[i] >> 24) & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 16) & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 8) & 0xFF));
								ByteBuffer.append((unsigned char) (v[i] & 0xFF));
							}
						}
						state = true;
					}
				}
			}
		} else if (Buffer.getSigned() == true) {

			vector<signed int> v;

			if (Buffer.getWidth() == ValueBuffer::Bits_8) {
				
				if (signedAnalyseInput(List,v)) {
					if (checkSignedBounds(-128,127,v)) {
						for (unsigned int i=0; i < v.size(); i++) {
							ByteBuffer.append((signed char) v[i]);
						}
						state = true;
					} 
				}
			} else if (Buffer.getWidth() == ValueBuffer::Bits_16) {
				if (signedAnalyseInput(List,v)) {
					if (checkSignedBounds(-32768,32767,v)) {
						if (Buffer.getByteOrder() == ValueBuffer::LittleEndian) {
							for (unsigned int i=0; i < v.size(); i++) {
								ByteBuffer.append((unsigned char) (((unsigned short int) v[i]) & 0xFF));
								ByteBuffer.append((unsigned char) ((((unsigned short int) v[i]) >> 8) & 0xFF));
							}
						} else {
							// big endian
							for (unsigned int i=0; i < v.size(); i++) {
								ByteBuffer.append((unsigned char) ((((unsigned short int) v[i]) >> 8) & 0xFF));
								ByteBuffer.append((unsigned char) (((unsigned short int) v[i]) & 0xFF));
							}
						}
						state = true;
					}
				}
			} else if (Buffer.getWidth() == ValueBuffer::Bits_32) {
				if (signedAnalyseInput(List,v)) {
					if (checkSignedBounds(0x80000000 /* -2147483648 */,2147483647,v)) {
						if (Buffer.getByteOrder() == ValueBuffer::LittleEndian) {
							for (unsigned int i=0; i < v.size(); i++) {
								ByteBuffer.append((unsigned char) (v[i] & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 8) & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 16) & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 24) & 0xFF));
							}
						} else {
							// big endian
							for (unsigned int i=0; i < v.size(); i++) {
								ByteBuffer.append((unsigned char) ((v[i] >> 24) & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 16) & 0xFF));
								ByteBuffer.append((unsigned char) ((v[i] >> 8) & 0xFF));
								ByteBuffer.append((unsigned char) (v[i] & 0xFF));
							}
						}
						state = true;
					}
				}
			}
		}
	}
	return state; 
}

bool MainWindow::checkUnsignedBounds(unsigned int upperBound, vector<unsigned int> &v)
{
	bool BoundsOk = true;
	
	for (unsigned int i=0; i < v.size(); i++) {
		if (!(v[i] <= upperBound)) {
			BoundsOk = false;
			break;
		}
	}
	
	return BoundsOk;
}

bool MainWindow::checkSignedBounds(signed int lowerBound, signed int upperBound, vector<signed int> &v)
{
	bool BoundsOk = true;
	
	for (unsigned int i=0; i < v.size(); i++) {
		if (!((v[i] >= lowerBound) && (v[i] <= upperBound))) {
			BoundsOk = false;
			break;
		}
	}
	
	return BoundsOk;
}

bool MainWindow::unsignedAnalyseInput(QStringList List, vector<unsigned int> &v)
{
	bool convertOk=true;
	unsigned long int UValue=0;
	
	for (int i=0; i < List.size(); i++) {
				
		UValue = List.at(i).toULong(&convertOk,0);
		if (convertOk) {
			v.push_back(UValue);
		} else {
			break;
		}
	} 
	return convertOk;
}

bool MainWindow::signedAnalyseInput(QStringList List, vector<signed int> &v)
{
	bool convertOk=true;
	signed long int SValue=0;
	
	for (int i=0; i < List.size(); i++) {
		SValue = List.at(i).toLong(&convertOk,0);
		if (convertOk) {
			v.push_back(SValue);
		} else {
			break;
		} 
	}
	
	return convertOk;
}

 
QStringList MainWindow::splitInput(QString S)
{
	QStringList List = S.split(" ",QString::SkipEmptyParts);
	return List;
}
 
void MainWindow::TypeButtonClicked(int id)
{
	TypeButtonId = id;
	radioW0->setEnabled(id != 0);
	radioW1->setEnabled(id != 0);
	radioW2->setEnabled(id != 0);
	radioS0->setEnabled(id != 0);
	radioS1->setEnabled(id != 0);
	radioBo0->setEnabled((id != 0) && (WidthButtonId != 0));
	radioBo1->setEnabled((id != 0) && (WidthButtonId != 0));

	if (id == 0) {
		m_LineEdit->setValidator(0);
	} else {
		m_LineEdit->setValidator(new QRegExpValidator(m_NumberRegExp,m_LineEdit));
	}
	checkInput(m_LineEdit->text());
}
 
void MainWindow::WidthButtonClicked(int id) {
	WidthButtonId = id;
	
	radioBo0->setEnabled(id != 0);
	radioBo1->setEnabled(id != 0);

	checkInput(m_LineEdit->text());
}

void MainWindow::SignButtonClicked(int id) {
	SignButtonId = id;

	checkInput(m_LineEdit->text());
}
 
void MainWindow::OrderButtonClicked(int id) {
	OrderButtonId = id;
} 

void MainWindow::CRCButtonClicked(int id) {
	CRCButtonId = id;
}
 
void MainWindow::openDevice()
{
 	DeviceDialog dialog(BaudrateIndex,FlowControlIndex,ParityIndex,CharSizeIndex,StopBitsIndex,DeviceName);
 	dialog.exec();
 	
 	if (dialog.result() == QDialog::Accepted) {
 		
		BaudrateIndex = dialog.getBaudrate();
		FlowControlIndex = dialog.getFlowControl();
		ParityIndex = dialog.getParity();
		CharSizeIndex = dialog.getCharSize();
		StopBitsIndex = dialog.getStopBits();
		DeviceName = dialog.getDeviceName();
 		
	    SerialStream *serialStream = new SerialStream();
		serialStream->Open(DeviceName.toStdString());

  		if (!serialStream->IsOpen())
  		{
    		QMessageBox::critical(this, tr("Error"),
    				"The device can not be opened!",QMessageBox::Ok);
    		
    		delete serialStream;
  		
  		} else {
  			
			setSerialStreamAttributes(*serialStream,
										BaudrateIndex,
										FlowControlIndex,
										ParityIndex,
										CharSizeIndex,
										StopBitsIndex);

			m_HistoryCB->setEnabled(true);
			closeAct->setEnabled(true);
			openAct->setEnabled(false);
			m_BufferSendButton->setEnabled(true);
			optionAct->setEnabled(false);
			saveBufferAct->setEnabled(false);
			clearBufferAct->setEnabled(false);
			bufferMenu->setEnabled(false);

		    serialThread = new SerialReceiveThread(this);
		    serialThread->startThread(serialStream,m_ThreadSleep); 
		    
		    connect(serialThread, SIGNAL(receivedData(QByteArray)),
             this, SLOT(updateReceiveBuffer(QByteArray)));

  			StreamIsOpen = true;
		    statusBar->showMessage(tr("Connection opened..."));
  		}
 	}  		
}

void MainWindow::setSerialStreamAttributes(SerialStream &serialStr, int Baudrate, int FlowControl, int Parity, int CharSize, int NumOfStopBits)
{
	switch (Baudrate) {
		case 0: serialStr.SetBaudRate( SerialStreamBuf::BAUD_50 );
				break; 
		case 1: serialStr.SetBaudRate( SerialStreamBuf::BAUD_75 );
				break; 
		case 2: serialStr.SetBaudRate( SerialStreamBuf::BAUD_110 );
				break; 
		case 3: serialStr.SetBaudRate( SerialStreamBuf::BAUD_134 );
				break; 
		case 4: serialStr.SetBaudRate( SerialStreamBuf::BAUD_150 );
				break; 
		case 5: serialStr.SetBaudRate( SerialStreamBuf::BAUD_200 );
				break; 
		case 6: serialStr.SetBaudRate( SerialStreamBuf::BAUD_300 );
				break; 
		case 7: serialStr.SetBaudRate( SerialStreamBuf::BAUD_600 );
				break; 
		case 8: serialStr.SetBaudRate( SerialStreamBuf::BAUD_1200 );
				break; 
		case 9: serialStr.SetBaudRate( SerialStreamBuf::BAUD_1800 );
				break; 
		case 10: serialStr.SetBaudRate( SerialStreamBuf::BAUD_2400 );
				break; 
		case 11: serialStr.SetBaudRate( SerialStreamBuf::BAUD_4800 );
				break; 
		case 12: serialStr.SetBaudRate( SerialStreamBuf::BAUD_9600 );
				break; 
		case 13: serialStr.SetBaudRate( SerialStreamBuf::BAUD_19200 );
				break; 
		case 14: serialStr.SetBaudRate( SerialStreamBuf::BAUD_38400 );
				break; 
		case 15: serialStr.SetBaudRate( SerialStreamBuf::BAUD_57600 );
				break; 
		case 16: serialStr.SetBaudRate( SerialStreamBuf::BAUD_115200 );
				break; 
	}
	switch (FlowControl) {
		case 0: serialStr.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_HARD); 
				break;
		case 1: serialStr.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_SOFT); 
				break;
		case 2: serialStr.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE); 
				break;
	}
	switch (Parity) {
		case 0: serialStr.SetParity( SerialStreamBuf::PARITY_EVEN );
				break;
		case 1: serialStr.SetParity( SerialStreamBuf::PARITY_ODD );
				break;
		case 2: serialStr.SetParity( SerialStreamBuf::PARITY_NONE );
				break;
	}
	switch (CharSize) {
		case 0: serialStr.SetCharSize(SerialStreamBuf::CHAR_SIZE_5);
				break;
		case 1: serialStr.SetCharSize(SerialStreamBuf::CHAR_SIZE_6);
				break;
		case 2: serialStr.SetCharSize(SerialStreamBuf::CHAR_SIZE_7);
				break;
		case 3: serialStr.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
				break;
	}
	switch (NumOfStopBits) {
		case 0: serialStr.SetNumOfStopBits(1);
				break;
		case 1: serialStr.SetNumOfStopBits(2);
				break;
	}  
}	

bool MainWindow::closeCon()
{
 	delete serialThread; 
 	StreamIsOpen = false;
 	 
	m_HistoryCB->setEnabled(false);
	closeAct->setEnabled(false);
	openAct->setEnabled(true);
	m_BufferSendButton->setEnabled(false);
	optionAct->setEnabled(true);
	saveBufferAct->setEnabled(true);
	clearBufferAct->setEnabled(true);
	bufferMenu->setEnabled(true);

    statusBar->showMessage(tr("Connection closed."));
 	 
    return true;
}


void MainWindow::updateReceiveBuffer(QByteArray buffer)
{
	appendBufferToTransceiverTable(getTimestamp(),buffer,RX_VALUE);
}


void MainWindow::checkEvents() {

	QByteArray receivedBytes = getReceivedBytes();
	
	for (int i=0; i<m_EventList.size(); i++) {
	
		if (m_EventList[i].isActive()) {
	
			QByteArray list = m_EventList[i].getByteBuffer();
	
			if ((int) ReceivedCount >= list.size()) {

				if (compareSequence(receivedBytes,list)) {
			
					if (m_EventList[i].getAction() == HightlightBytesInBuffer) { 
						highlightLastBytes(list.size(),m_EventList[i].getColor());
					}
				}	
			}
		}
	}
}

void MainWindow::highlightLastBytes(unsigned int count, QColor color) {
	
	QBrush redbrush;
	redbrush.setStyle(Qt::SolidPattern);
	redbrush.setColor(m_RxColor);
	
	QBrush bgbrush; 
	bgbrush.setColor(color);
	bgbrush.setStyle(Qt::SolidPattern);
	
	QTableWidgetItem *item = 0;
	
	unsigned int i=0;
	unsigned int idx = RxTxValueBuffer.size()-1;
	
	while (i < count) {

		if (RxTxValueBuffer.at(idx).Type() == RX_VALUE) {			

			item = TranceiverTable->item(RxTxValueBuffer.at(idx).Row(),RxTxValueBuffer.at(idx).Column());
			
			item->setBackground(bgbrush);		
			
			i++;	
		} 
		
		idx--;
	}
}

bool MainWindow::compareSequence(QByteArray rList, QByteArray eList) {
	
	bool equal = true;
	
	unsigned int rSize = rList.size();
	unsigned int eSize = eList.size();
	
	for (unsigned int i=0; i<eSize; i++) {

		if (((int) eList.at(i)) != ((int) rList.at(rSize-eSize+i))) {
		
			equal = false;
			break;	
		}
	}	
	
	return equal;
}


QByteArray MainWindow::getReceivedBytes() {
	
	QByteArray bytes;
	
	for (unsigned int i=0; i<RxTxValueBuffer.size(); i++) {

		if (RxTxValueBuffer.at(i).Type() == RX_VALUE) {	

			bytes.push_back(RxTxValueBuffer.at(i).Value());
		}
	}
	
	return bytes;
}



void MainWindow::loadProject() {

	bool loadnew = true;
	
	if (!m_ProjectSaved) {
		switch( QMessageBox::warning(this, tr("Serial-IO"),
                   tr("The project has been modified.\n"
                      "Do you want to load a new one without saving?"),
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::No)) {
                   
    		case QMessageBox::No: 
    							loadnew = false;
        						break;
        	default:
        						break;
    	} 
	}
	
	if (loadnew) {
		QFileDialog fileDialog(this,"Load Project...","./","Serial-IO Project (*.sio)");
		fileDialog.setViewMode(QFileDialog::Detail);
		fileDialog.setFileMode(QFileDialog::ExistingFile);
	
		if (fileDialog.exec()) {
		
			QStringList fileNames;
		
			fileNames = fileDialog.selectedFiles();
			
			loadProjectFile(fileNames.at(0));
		}
	}
}

void MainWindow::loadProjectFile(QString FileName) {
	
	QDomDocument doc("SERIAL-IO-XML");
	
	QFile file(FileName);

	bool error = false;

	if (file.open(QIODevice::ReadOnly)) {
			
			if (!doc.setContent(&file)) {
				error = true;
	 	} 
			file.close();
	} else {
		error = true;
	}
	
		if (!error) {
		
		removeAllBuffers();
		removeAllEvents();
		removeAllDialogs();
		
		if (loadDocument(doc)) {
			m_ProjectSaved = true;	
			BufferListToTable();
			EventListToTable();
			DialogListToTable();
		} else {
			error = true;
			m_BufferList.clear();
			m_EventList.clear();
			m_DialogList.clear();
		}
		
		addRecentFile(FileName);
		addRecentFilesToMenu();
	}
	
	if (error) {
		QMessageBox::critical(this, tr("Serial-IO"),
           tr("An Error occured while loading the file!\n"),
           QMessageBox::Ok,
           QMessageBox::Ok);
	}
}

void MainWindow::BufferListToTable() {
	
	for (int i=0; i<m_BufferList.size(); i++) {
	
		addBufferRow(m_BufferList.at(i));
	}
	rebuildBufferList();
	updateBufferButtonStates();
}

void MainWindow::EventListToTable() {

	for (int i=0; i<m_EventList.size(); i++) {
	
		addEventRow(m_EventList.at(i));
	}
	
	updateEventButtonStates();
}

void MainWindow::DialogListToTable() {
	
	for (int i=0; i<m_DialogList.size(); i++) {
		addDialogRow(m_DialogList.at(i));
	}
	
	updateDialogButtonStates();
}

bool MainWindow::loadDocument(QDomDocument &doc) {

	bool error = false;

	QDomElement root = doc.documentElement();
	
	if (root.tagName() == "SIOP") {

		QDomNode n = root.firstChild();
	
		while( !n.isNull() )
		{
  			QDomElement e = n.toElement();
  	
  			if( !e.isNull() )
  			{
    			if( e.tagName() == "Buffers" ) {
    		
    				error = loadBuffersFromDoc(e.firstChild());
    					
			    } else if (e.tagName() == "Events" ) {	

					error = loadEventsFromDoc(e.firstChild());					
			    } else if (e.tagName() == "Dialogs" ) {
			    	
			    	error = loadDialogsFromDoc(e.firstChild());
			    }
  			}

			if (error) break;
			
  			n = n.nextSibling();
		}

	} else error = true;
	
	return (!error);
}


bool MainWindow::loadBuffersFromDoc(QDomNode buffers) {
	
	bool loadError = false;
	
	while (!buffers.isNull()) {
    					
		QDomElement buffer = buffers.toElement();
    					
		if (buffer.tagName() == "Buffer") { 
    					
    		QString Name = buffer.attribute("name","");
    
    		bool TypeOk;
    		ValueBuffer::e_Type Type = (ValueBuffer::e_Type) buffer.attribute("Type",0).toUInt(&TypeOk,10);
    		
    		bool WidthOk;
    		ValueBuffer::e_Width Width = (ValueBuffer::e_Width) buffer.attribute("Width",0).toUInt(&WidthOk,10);
    		
    		bool SignedOk;
    		bool Signed = (bool) buffer.attribute("Signed",false).toUInt(&SignedOk,10); 
    			
    		bool ByteOrderOk;
    		ValueBuffer::e_ByteOrder ByteOrder = (ValueBuffer::e_ByteOrder) buffer.attribute("ByteOrder",0).toUInt(&ByteOrderOk,10);
    			
			bool calcChkSumOk;
    		bool calcChkSum = buffer.attribute("calcChkSum","0").toUInt(&calcChkSumOk,10);
    
			bool ChkSumOk;
    		ValueBuffer::e_ChecksumType ChkSumType = (ValueBuffer::e_ChecksumType) buffer.attribute("ChkSumType","0").toUInt(&ChkSumOk,10);
    					
    		QDomNode values = buffer.firstChild();
    		
    		bool TagError = false;
    		QStringList sList;
    		
    		while (!values.isNull()) {

				QDomElement value = values.toElement();
							
				if (value.tagName() == "Value") { 

					sList << value.firstChild().nodeValue();

				} else {
					TagError = true;	
					break;
				}
				values = values.nextSibling();
    		}    						
    		
    		if ((Name.length() > 0) && TypeOk && WidthOk && SignedOk && ByteOrderOk && calcChkSumOk && ChkSumOk && (!TagError)) {
    		
    			ValueBuffer VBuffer;
    			
    			VBuffer.setName(Name);
    			VBuffer.setType(Type);
    			VBuffer.setWidth(Width);
    			VBuffer.setSigned(Signed);
    			VBuffer.setByteOrder(ByteOrder);
    			VBuffer.setCalcChecksum(calcChkSum);
    			VBuffer.setChecksumType(ChkSumType);
    			VBuffer.setBuffer(sList);	
    			
    			m_BufferList << VBuffer;
    		} else {
    			loadError = true;
    		}
   		}
    				
   		buffers = buffers.nextSibling();	
	} 
	
	return loadError;
}


bool MainWindow::loadEventsFromDoc(QDomNode events) {
	
	bool loadError = false;
	
	while (!events.isNull()) {
    					
		QDomElement event = events.toElement();
    					
		if (event.tagName() == "Event") { 
    					
    		QString Name = event.attribute("name","");
    
			bool ActiveOk;
    		bool Active = event.attribute("active","0").toUInt(&ActiveOk,10);
    
			bool ActionOk;
    		e_Action Action = (e_Action) event.attribute("action","0").toUInt(&ActionOk,10);
    					
    		QColor color;
    		color.setNamedColor(event.attribute("color","#FFE0E0"));
    		
    		QDomNode values = event.firstChild();
    		
    		bool TagError = false;
    		QStringList sList;
    		
    		while (!values.isNull()) {

				QDomElement value = values.toElement();
							
				if (value.tagName() == "Value") { 

					sList << value.firstChild().nodeValue();

				} else {
					TagError = true;	
					break;
				}
				values = values.nextSibling();
    		}    						
    		
    		if ((Name.length() > 0) && ActiveOk && ActionOk && (!TagError)) {
    		
    			Event Ev;
    			
    			Ev.setName(Name);
    			Ev.setActive(Active);
    			Ev.setAction(Action);
    			Ev.setBuffer(sList);
    			Ev.setColor(color);
    			
    			m_EventList << Ev;
    		} else {
    			loadError = true;
    		}
   		}

   		events = events.nextSibling();	
	} 
	
	return loadError;
}

bool MainWindow::loadDialogsFromDoc(QDomNode dialogs) {
	
	// TODO: load dialogs

	bool loadError = false;
	
	while (!dialogs.isNull()) {
    					
		QDomElement dialog = dialogs.toElement();
    					
		if (dialog.tagName() == "Dialog") { 
    					
    		QString Name = dialog.attribute("name","");
    
    		QDomNode variables = dialog.firstChild();

    		bool VError=false;
    		QList<Variable> vList;
    		
    		while (!variables.isNull()) {

				QDomElement variable = variables.toElement();
							
				if (variable.tagName() == "Variable") { 

					QString vName = variable.attribute("name","");
					
					bool vTypeOk=false;
					unsigned int vt = variable.attribute("type","0").toUInt(&vTypeOk,10);
					Variable::t_Variable vType = Variable::NONE;
					if (vTypeOk) {
						if (vt <= 3) {
							vType = (Variable::t_Variable) vt;
						} else {
							vTypeOk = false;
						}
					}
					
					if ((vName.length() != 0) && vTypeOk) {
						
						bool RError = false;
						
						QDomNode representations = variable.firstChild();
						
						Representation vRepresentation;
						
						while (!representations.isNull()) {
							
							QDomElement represent = representations.toElement();
							
							if (represent.tagName() == "Representation") {
								
								bool rTypeOk=false;
								Representation::t_RType rType = Representation::NONE;
								unsigned int rt = represent.attribute("type","0").toUInt(&rTypeOk,10);
								if (rTypeOk) {
									if (rt <= 3) {
										rType = (Representation::t_RType) rt;
									} else {
										rTypeOk = false;
									}
								}
								
								if (rTypeOk) {
									
									vRepresentation.setType(rType);
									
									QDomNamedNodeMap map = represent.attributes();
									
									for (int mIdx=0; mIdx<map.size(); mIdx++) {
										
										if (map.item(mIdx).nodeName() != "type") {

											if (map.item(mIdx).nodeName().length() > 0) {
												vRepresentation.setAttribute(map.item(mIdx).nodeName(),map.item(mIdx).nodeValue());
											} 
										}
									}
								} else {
									RError = true;
									break;
								}
							}
							representations = representations.nextSibling();
						}
						
						if ((!RError) && (vRepresentation.Type() != Representation::NONE)) {
							
							Variable va;
							
							va.setName(vName);
							va.setType(vType);
							va.setRepresentation(vRepresentation);
							
							vList << va;
						} else {
							VError = true;
							break;
						}
						
					} else {
						VError = true;
						break;
					}
				} 
				variables = variables.nextSibling(); 
    		}    						
    		
    		if ((Name.length() > 0) && (!VError)) {
    		
    			Dialog Di;
    			
    			Di.setName(Name);
    			for (int i=0; i<vList.size(); i++) {
        			Di.addVariable(vList.at(i));
    			}
    			
    			m_DialogList << Di;
    		} else {
    			loadError = true;
    		} 
   		}

   		dialogs = dialogs.nextSibling();	
	} 
	
	return loadError;
}

void MainWindow::removeAllBuffers() {
	
	m_BufferList.clear();

	while (m_BufferTable->rowCount() > 0) {
		m_BufferTable->removeRow(0);	
	}	
}

void MainWindow::removeAllEvents() {
	
	m_EventList.clear();

	while (m_EventTable->rowCount() > 0) {
		m_EventTable->removeRow(0);
	}
}

void MainWindow::removeAllDialogs() {
	
	m_DialogList.clear();

	while (m_DialogTable->rowCount() > 0) {
		m_DialogTable->removeRow(0);
	}
}

void MainWindow::saveProject() {

	QFileDialog fileDialog(this,"Save Project...","./","Serial-IO Project (*.sio)");
	fileDialog.setViewMode(QFileDialog::Detail);
	fileDialog.setDefaultSuffix("sio");
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setConfirmOverwrite(true);
	
	if (fileDialog.exec()) {
		
		QStringList fileNames;
		fileNames = fileDialog.selectedFiles();
		
		QFileInfo FileInfo(fileNames.at(0));

		QDomDocument doc("SERIAL-IO-XML");
		
		createDocument(doc);
		
		QFile file( fileNames.at(0) );
  	
  		if(file.open( QIODevice::WriteOnly ) ) {

	  		QTextStream ts( &file );
  			ts << doc.toString();

  			file.close();
  			
  			m_ProjectSaved = true;

			addRecentFile(fileNames.at(0));
			addRecentFilesToMenu();
  		} else {

  			QMessageBox::critical(this, tr("Serial-IO"),
                   tr("An Error occured while saving the file!\n"),
                   QMessageBox::Ok,
                   QMessageBox::Ok);
  		}
	}
}


void MainWindow::createDocument(QDomDocument &doc) {
	
	QDomElement root = doc.createElement("SIOP");
  
	doc.appendChild( root );
	
	QDomElement buffers = doc.createElement("Buffers");
	
	root.appendChild(buffers);
	
	for (int i=0; i<m_BufferList.size(); i++) {
	
		QDomElement buffer = doc.createElement("Buffer");
		buffer.setAttribute("name",m_BufferList.at(i).getName());
		buffer.setAttribute("Type",m_BufferList.at(i).getType());
		buffer.setAttribute("Width",m_BufferList.at(i).getWidth());
		buffer.setAttribute("Signed",m_BufferList.at(i).getSigned());
		buffer.setAttribute("ByteOrder",m_BufferList.at(i).getByteOrder());
		buffer.setAttribute("calcChkSum",m_BufferList.at(i).getCalcChecksum());
		buffer.setAttribute("ChkSumType",m_BufferList.at(i).getChecksumType());
		
		buffers.appendChild(buffer);
		
		QStringList list = m_BufferList.at(i).getBuffer();
		
		for (int j=0; j<list.size(); j++) {
			
			QDomElement data = doc.createElement("Value");

			QDomText text = doc.createTextNode(list.at(j));
			data.appendChild(text);
			
			buffer.appendChild(data);
		}
	}
	
	
	QDomElement events = doc.createElement("Events");
	
	root.appendChild(events);
	
	for (int i=0; i<m_EventList.size(); i++) {
		
		QDomElement event = doc.createElement("Event");
		event.setAttribute("name",m_EventList.at(i).getName());
		event.setAttribute("active",m_EventList.at(i).isActive());
		event.setAttribute("action",m_EventList.at(i).getAction());
		event.setAttribute("color",m_EventList.at(i).getColor().name());
		events.appendChild(event);
		
		QStringList list = m_EventList.at(i).getBuffer();
		
		for (int j=0; j<list.size(); j++) {
			
			QDomElement data = doc.createElement("Value");

			QDomText text = doc.createTextNode(list.at(j));
			data.appendChild(text);
			
			event.appendChild(data);
		}
	}
	
	QDomElement dialogs = doc.createElement("Dialogs");
	
	root.appendChild(dialogs);

	for (int i=0; i<m_DialogList.size(); i++) {
		
		QDomElement dialog = doc.createElement("Dialog");
		dialog.setAttribute("name",m_DialogList.at(i).Name());
		dialogs.appendChild(dialog);
		
		Dialog d = m_DialogList.at(i);
				
		for (unsigned int j=0; j<d.getVariableCount(); j++) {

			QDomElement ve = doc.createElement("Variable");
			
			Variable v = d.getVariable(j);
			
			ve.setAttribute("name",v.Name());
			ve.setAttribute("type",v.Type());
			
			Representation r = v.Represent();
			
			QDomElement re = doc.createElement("Representation");
			
			re.setAttribute("type",r.Type());
			QStringList key = r.AttributeKeys();
			for (int k=0; k<key.size(); k++) {
				
				re.setAttribute(key[k],r.Attribute(key[k]));
			}
			
			ve.appendChild(re);
			
			dialog.appendChild(ve);
		}
	}
}


void MainWindow::editOptions() {

	OptionsDlg dlg(m_TxBufferSize,m_RxBufferSize,m_ThreadSleep,
		m_RxTxColumnCount,m_RxTxValueRepresentation,m_ScrollToBottom,
		m_SelectLastValues,m_RxColor,m_TxColor,m_LoadLastConfigOnStartup);

	dlg.exec();	
	
	if (dlg.result() == QDialog::Accepted) {
		
		m_RxColor = dlg.RxColor();
		m_TxColor = dlg.TxColor();
		m_ThreadSleep = dlg.ThreadSleep();
		m_ScrollToBottom = dlg.ScrollToBottom();
		m_SelectLastValues = dlg.SelectLastValues();
		m_LoadLastConfigOnStartup = dlg.LastConfig();
		
		if (m_RxTxColumnCount != dlg.ColumnCount()) {
			m_RxTxValueRepresentation = dlg.ValueRepresentation();
			m_RxTxColumnCount = dlg.ColumnCount();
			readdValues();
		}
		
		if (m_TxBufferSize != dlg.TxBufferSize()) {
			m_TxBufferSize = dlg.TxBufferSize();
			TxBufferDeleteFirstBytes();
		}
		
		if (m_RxBufferSize != dlg.RxBufferSize()) {
			m_RxBufferSize = dlg.RxBufferSize();
			RxBufferDeleteFirstBytes();
		}
		
		if (m_RxTxValueRepresentation != dlg.ValueRepresentation()) {
			
			m_RxTxValueRepresentation = dlg.ValueRepresentation();
			
			toNewRepresentation();
		}
	}
}

void MainWindow::toNewRepresentation() {
	
	for (unsigned int i=0; i<RxTxValueBuffer.size(); i++) {
		
		QTableWidgetItem* item = TranceiverTable->item(RxTxValueBuffer.at(i).Row(),RxTxValueBuffer.at(i).Column());
		
		QVariant var(getValueRepresentation((unsigned int) RxTxValueBuffer.at(i).Value()));
		item->setData(Qt::DisplayRole,var);
	}
}

void MainWindow::readdValues() {
	
	vector<RxTxValue> lRxTxValueBuffer;
	
	lRxTxValueBuffer = RxTxValueBuffer;
	
	while (TranceiverTable->rowCount() > 0) TranceiverTable->removeRow(0);
	
	QStringList TTHeader;
	TTHeader << "Timestamp";
	for (unsigned int i=0; i<m_RxTxColumnCount; i++) {
		TTHeader << QString("%1").arg(i);
	}	
	TTHeader << "Characters";
	
	TranceiverTable->setColumnCount(TTHeader.size());
	TranceiverTable->setHorizontalHeaderLabels(TTHeader);	
	
	RxTxValueBuffer.clear();
	SendCount = 0;
	ReceivedCount = 0;
	
	QString Timestamp = "";
	e_ValueType Type;
	QByteArray Buffer;
	
	unsigned int idx=0;
	while (idx < lRxTxValueBuffer.size()) {
	
		Buffer.clear();
		
		Timestamp = lRxTxValueBuffer.at(idx).Timestamp();
		Type = lRxTxValueBuffer.at(idx).Type();
		Buffer.append(lRxTxValueBuffer.at(idx).Value());
		
		while (lRxTxValueBuffer.size() > (idx+1)) {

			if ((lRxTxValueBuffer.at(idx+1).Timestamp() == Timestamp) && 
				(lRxTxValueBuffer.at(idx+1).Type() == Type)) {
					
				idx++;
				Buffer.append(lRxTxValueBuffer.at(idx).Value());
			} else {
				break;
			}
		}
		appendBufferToTransceiverTable(Timestamp,Buffer,Type);
		idx++;		
	}
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Serial I/O Manager"),
             tr("A simple application to transmit and "
             	"receive data through serial connections.\n\n"
             	"Version 0.9 beta\n"
             	"Copyright 2007-2008 by W. Holzke\n"
             	"Licence GPL 2"));
}

void MainWindow::createActions()
{
    openAct = new QAction(QIcon(":/images/open_con.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open a connection"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openDevice()));

    closeAct = new QAction(QIcon(":/images/close_con.png"), tr("&Close"), this);
    closeAct->setShortcut(tr("Ctrl+C"));
    closeAct->setStatusTip(tr("Close a connection"));
    closeAct->setEnabled(false);
    connect(closeAct, SIGNAL(triggered()), this, SLOT(closeCon()));

	loadAct = new QAction(tr("&Load"),this);
	loadAct->setShortcut(tr("Ctrl+L"));
	loadAct->setStatusTip(tr("Load Buffers and Events"));
	connect(loadAct,SIGNAL(triggered()),this,SLOT(loadProject()));

	saveAct = new QAction(tr("&Save"),this);
	saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setStatusTip(tr("Save Buffers and Events"));
	connect(saveAct,SIGNAL(triggered()),this,SLOT(saveProject()));

	optionAct = new QAction(tr("&Option"),this);
    optionAct->setShortcut(tr("Ctrl+O"));
    optionAct->setStatusTip(tr("Application options"));
    connect(optionAct,SIGNAL(triggered()),this,SLOT(editOptions()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    
    saveBufferAct = new QAction(tr("Sa&ve"),this);
	saveBufferAct->setShortcut(tr("Ctrl+B"));
    saveBufferAct->setStatusTip(tr("Save Tx/Rx Buffer..."));
    connect(saveBufferAct,SIGNAL(triggered()),this,SLOT(saveBuffer()));

	clearBufferAct = new QAction(tr("Clea&r"),this);
	clearBufferAct->setShortcut(tr("Ctrl+R"));
	clearBufferAct->setStatusTip(tr("Clear Rx/Rx Buffer..."));
	connect(clearBufferAct,SIGNAL(triggered()),this,SLOT(clearBuffer()));

	viewToolsAct = new QAction(tr("&Toolbar"),this);
	viewToolsAct->setStatusTip(tr("Show/Hide Toolbar..."));
	viewToolsAct->setCheckable(true);
	viewToolsAct->setChecked(m_viewTools);
	connect(viewToolsAct,SIGNAL(triggered()),this,SLOT(viewTools()));

    viewSendTypeAct = new QAction(tr("Send &Type"),this);
    viewSendTypeAct->setStatusTip(tr("Show/Hide Send Type..."));
    viewSendTypeAct->setCheckable(true);
    viewSendTypeAct->setChecked(m_viewSendType);
	connect(viewSendTypeAct,SIGNAL(triggered()),this,SLOT(viewSendType()));
    
    viewWidthAct  = new QAction(tr("&Width"),this);
    viewWidthAct->setStatusTip(tr("Show/Hide Width..."));
    viewWidthAct->setCheckable(true);
    viewWidthAct->setChecked(m_viewWidth);
	connect(viewWidthAct,SIGNAL(triggered()),this,SLOT(viewWidth()));
    
   	viewSignAct = new QAction(tr("&Sign"),this);
   	viewSignAct->setStatusTip(tr("Show/Hide Sign..."));
   	viewSignAct->setCheckable(true);
   	viewSignAct->setChecked(m_viewSign);
	connect(viewSignAct,SIGNAL(triggered()),this,SLOT(viewSign()));
    
    viewByteOrderAct = new QAction(tr("Byte &Order"),this);
    viewByteOrderAct->setStatusTip(tr("Show/Hide Byte Order..."));
    viewByteOrderAct->setCheckable(true);
    viewByteOrderAct->setChecked(m_viewByteOrder);
	connect(viewByteOrderAct,SIGNAL(triggered()),this,SLOT(viewByteOrder()));

	viewCRCAct = new QAction(tr("Calc Checksum"),this);
	viewCRCAct->setStatusTip(tr("Show/Hide Calc Checksum..."));
	viewCRCAct->setCheckable(true);
	viewCRCAct->setChecked(m_viewCRC);
	connect(viewCRCAct,SIGNAL(triggered()),this,SLOT(viewCRC()));
	
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
	menuBar = new QMenuBar;
	
    fileMenu = new QMenu(tr("&Program"),this);
    fileMenu->addAction(loadAct);
    
    recentMenu = fileMenu->addMenu("Load recent...");
    connect(recentMenu,SIGNAL(triggered(QAction*)),this,SLOT(loadrecentFile(QAction*)));
    addRecentFilesToMenu();

    fileMenu->addAction(saveAct);
    
    fileMenu->addSeparator();
    fileMenu->addAction(optionAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
	menuBar->addMenu(fileMenu);

	menuBar->addSeparator();
	
	conMenu = new QMenu(tr("&Connection"),this);
    conMenu->addAction(openAct);
    conMenu->addAction(closeAct);
    menuBar->addMenu(conMenu);
	
    menuBar->addSeparator();
    
    bufferMenu = new QMenu(tr("&Buffer"),this);
    bufferMenu->addAction(clearBufferAct);
    bufferMenu->addAction(saveBufferAct);
    menuBar->addMenu(bufferMenu);
    
    menuBar->addSeparator();

	viewMenu = new QMenu(tr("&View"),this);
	viewMenu->addAction(viewToolsAct);
	viewMenu->addSeparator();
	viewMenu->addAction(viewSendTypeAct);
	viewMenu->addAction(viewWidthAct);
	viewMenu->addAction(viewSignAct);
	viewMenu->addAction(viewByteOrderAct);
	viewMenu->addAction(viewCRCAct);
	menuBar->addMenu(viewMenu);

    menuBar->addSeparator();

    helpMenu = new QMenu(tr("&Help"),this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
	menuBar->addMenu(helpMenu);
}

void MainWindow::createToolBars()
{
     fileToolBar = new QToolBar(tr("File"));
     fileToolBar->addAction(openAct);
     fileToolBar->addAction(closeAct);
}

void MainWindow::createStatusBar()
{
	statusBar = new QStatusBar;
    statusBar->showMessage(tr("Ready"));
}

void MainWindow::createTypeBox()
{
	horizontalGroupBox = new QGroupBox(tr("Send Type"));
    QHBoxLayout *layout = new QHBoxLayout;

	radio0 = new QRadioButton(tr("&Character"));
	radio1 = new QRadioButton(tr("&Number"));

    radio1->setChecked(true);
	TypeButtonId = 1;

    layout->addWidget(radio0);
    layout->addWidget(radio1);
    layout->addStretch(10);

    horizontalGroupBox->setLayout(layout);
    
    TypeButtonGroup = new QButtonGroup;
    TypeButtonGroup->addButton(radio0,0);
    TypeButtonGroup->addButton(radio1,1);
    connect(TypeButtonGroup, SIGNAL(buttonClicked(int)), 
    		this, SLOT(TypeButtonClicked(int)));
}

void MainWindow::createWidthBox()
{
	horizontalGroupBox3 = new QGroupBox(tr("Width"));
    QHBoxLayout *layout = new QHBoxLayout;

	radioW0 = new QRadioButton(tr("&8 Bit"));
	radioW1 = new QRadioButton(tr("&16 Bit"));
	radioW2 = new QRadioButton(tr("&32 Bit"));

    radioW0->setChecked(true);
	WidthButtonId = 0;

    layout->addWidget(radioW0);
    layout->addWidget(radioW1);
    layout->addWidget(radioW2);
    layout->addStretch(10);

    horizontalGroupBox3->setLayout(layout);
    
    WidthButtonGroup = new QButtonGroup;
    WidthButtonGroup->addButton(radioW0,0);
    WidthButtonGroup->addButton(radioW1,1);
    WidthButtonGroup->addButton(radioW2,2);
    connect(WidthButtonGroup, SIGNAL(buttonClicked(int)), 
    		this, SLOT(WidthButtonClicked(int)));
}

void MainWindow::createSignBox()
{
	horizontalGroupBox4 = new QGroupBox(tr("Sign"));
    QHBoxLayout *layout = new QHBoxLayout;

	radioS0 = new QRadioButton(tr("&unsigned"));
	radioS1 = new QRadioButton(tr("&signed"));

    radioS0->setChecked(true);
	SignButtonId = 0;

    layout->addWidget(radioS0);
    layout->addWidget(radioS1);
    layout->addStretch(10);

    horizontalGroupBox4->setLayout(layout);
    
    SignButtonGroup = new QButtonGroup;
    SignButtonGroup->addButton(radioS0,0);
    SignButtonGroup->addButton(radioS1,1);

    connect(SignButtonGroup, SIGNAL(buttonClicked(int)), 
    		this, SLOT(SignButtonClicked(int)));
}

void MainWindow::createByteOrderBox()
{
	horizontalGroupBox2 = new QGroupBox(tr("Byte Order"));
    QHBoxLayout *layout = new QHBoxLayout;

	radioBo0 = new QRadioButton(tr("little endian"));
	radioBo0->setEnabled(false);
	radioBo1 = new QRadioButton(tr("big endian"));
	radioBo1->setEnabled(false);

    radioBo0->setChecked(true);
    OrderButtonId = 0;

    layout->addWidget(radioBo0);
    layout->addWidget(radioBo1);
    layout->addStretch(10);

    horizontalGroupBox2->setLayout(layout);
    
    OrderButtonGroup = new QButtonGroup;
    OrderButtonGroup->addButton(radioBo0,0);
    OrderButtonGroup->addButton(radioBo1,1);
    connect(OrderButtonGroup, SIGNAL(buttonClicked(int)),
    		this, SLOT(OrderButtonClicked(int)));
}

void MainWindow::createCRCBox() {
	
	horizontalGroupBox5 = new QGroupBox(tr("Calc checksum"));
	QHBoxLayout *layout = new QHBoxLayout;
	
	radioCRC0 = new QRadioButton(tr("None"));
	radioCRC1 = new QRadioButton(tr("(Sum without carry)-1"));
	radioCRC2 = new QRadioButton(tr("XOR"));
	
	radioCRC0->setChecked(true);
	CRCButtonId = 0;
	
	layout->addWidget(radioCRC0);
	layout->addWidget(radioCRC1);
	layout->addWidget(radioCRC2);
	layout->addStretch(10);
	
	horizontalGroupBox5->setLayout(layout);
	
	CRCButtonGroup = new QButtonGroup;
	CRCButtonGroup->addButton(radioCRC0,0);
	CRCButtonGroup->addButton(radioCRC1,1);
	CRCButtonGroup->addButton(radioCRC2,2);
	connect(CRCButtonGroup,SIGNAL(buttonClicked(int)),
			this,SLOT(CRCButtonClicked(int)));	
}

void MainWindow::addRecentFilesToMenu() {
	
	recentMenu->clear();
	
	for (int i=0; i<m_recentFiles.size(); i++) {
	
		QFileInfo fi(m_recentFiles.at(i));
		
		QAction *act = new QAction(fi.fileName(),this);
		act->setData(i);
		recentMenu->addAction(act);
	}
	
    recentMenu->setEnabled(m_recentFiles.size() > 0);
}

void MainWindow::addRecentFile(QString FileName) {

	if (m_recentFiles.size() > 0) {
		for (int i=0; i<m_recentFiles.size(); i++) {
			if (m_recentFiles.at(i).compare(FileName) == 0) {
				m_recentFiles.removeAt(i);
				break;
			}
		}
	}
		
	m_recentFiles.insert(0,FileName);
	if (m_recentFiles.size() > 5) {
		m_recentFiles.erase(m_recentFiles.end());
	}
}

void MainWindow::readSettings()
{
     QSettings settings;
     m_RxColor = settings.value("RxColor",Qt::red).value<QColor>();
     m_TxColor = settings.value("TxColor",Qt::green).value<QColor>();
     m_RxBufferSize = settings.value("RxBufferSize",1024).toInt();
     m_TxBufferSize = settings.value("TxBufferSize",1024).toInt();
     m_ThreadSleep = settings.value("ThreadSleep",100).toInt();
     m_RxTxColumnCount = settings.value("RxTxColumnCount",8).toInt();
     m_RxTxValueRepresentation = settings.value("RxTxValueRepresentation",0).toInt();
     m_ScrollToBottom = settings.value("ScrollToBottom",true).toBool();
     m_SelectLastValues = settings.value("SelectLastValues",false).toBool();
     m_viewTools = settings.value("viewTools",true).toBool();
     m_viewSendType = settings.value("viewSendType",true).toBool();
     m_viewWidth = settings.value("viewWidth",true).toBool();
     m_viewSign = settings.value("viewSign",true).toBool();
     m_viewByteOrder = settings.value("viewByteOrder",true).toBool();
     m_viewCRC = settings.value("viewCRC",true).toBool();
     m_saveRxBuffer = settings.value("saveRxBuffer",true).toBool();
     m_saveTxBuffer = settings.value("saveTxBuffer",false).toBool();
     m_SaveBufferFilename = settings.value("SaveBufferFilename","").toString();
     BaudrateIndex = settings.value("BaudrateIndex", 0).toInt();
     FlowControlIndex = settings.value("FlowControlIndex",0).toInt();
     ParityIndex = settings.value("ParityIndex",0).toInt();
     CharSizeIndex = settings.value("CharSizeIndex",0).toInt();
     StopBitsIndex = settings.value("StopBits",0).toInt();
     DeviceName = settings.value("DeviceName","/dev/ttyS0").toString();
     m_LoadLastConfigOnStartup = settings.value("LoadLastConfigOnStartup",0).toBool();

     int count = settings.beginReadArray("recentFiles");
     for (int i=0; i<count; i++) {
    	 settings.setArrayIndex(i);
    	 m_recentFiles << settings.value("file").toString();
     }
     settings.endArray();

     QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
     QSize size = settings.value("size", QSize(400, 400)).toSize();
     resize(size);
     move(pos);
}

void MainWindow::writeSettings()
{
     QSettings settings;
     settings.setValue("pos", pos());
     settings.setValue("size", size());
     settings.setValue("BaudrateIndex", BaudrateIndex);
     settings.setValue("FlowControlIndex",FlowControlIndex);
     settings.setValue("ParityIndex",ParityIndex);
     settings.setValue("CharSizeIndex",CharSizeIndex);
     settings.setValue("StopBits",StopBitsIndex);
     settings.setValue("DeviceName",DeviceName);
     settings.setValue("RxBufferSize",m_RxBufferSize);
     settings.setValue("TxBufferSize",m_TxBufferSize);
     settings.setValue("ThreadSleep",m_ThreadSleep);
     settings.setValue("RxTxColumnCount",m_RxTxColumnCount);
     settings.setValue("RxTxValueRepresentation",m_RxTxValueRepresentation);
     settings.setValue("ScrollToBottom",m_ScrollToBottom);
     settings.setValue("SelectLastValues",m_SelectLastValues);
     settings.setValue("viewTools",m_viewTools);
     settings.setValue("viewSendType",m_viewSendType);
     settings.setValue("viewWidth",m_viewWidth);
     settings.setValue("viewSign",m_viewSign);
     settings.setValue("viewByteOrder",m_viewByteOrder);
     settings.setValue("viewCRC",m_viewCRC);
     settings.setValue("saveRxBuffer",m_saveRxBuffer);
     settings.setValue("saveTxBuffer",m_saveTxBuffer);
     settings.setValue("SaveBufferFilename",m_SaveBufferFilename);
     settings.setValue("RxColor",m_RxColor);
     settings.setValue("TxColor",m_TxColor);
     settings.setValue("LoadLastConfigOnStartup",m_LoadLastConfigOnStartup);
     
     settings.beginWriteArray("recentFiles");
     for (int i=0; i<m_recentFiles.size(); i++) {
    	 settings.setArrayIndex(i);
    	 settings.setValue("file",m_recentFiles.at(i));
     }
     settings.endArray();
}

bool MainWindow::maybeSave()
{
	bool close = true;
	
    if (!m_ProjectSaved) {
    	
         QMessageBox::StandardButton ret;
         ret = QMessageBox::warning(this, tr("Serial-IO"),
                      tr("The project has been modified!\n"
                         "Exit without saving?"),
                      QMessageBox::Yes | QMessageBox::No);
         if (ret == QMessageBox::Yes)
         	close = true;
         else if (ret == QMessageBox::No)
         	close = false;
    }
     
	if (StreamIsOpen && close) {  
         QMessageBox::StandardButton ret;
         ret = QMessageBox::warning(this, tr("Serial-IO"),
                      tr("A connection is still open.\n"
                         "Close connection and exit?"),
                      QMessageBox::Yes | QMessageBox::No);
         if (ret == QMessageBox::Yes)
         	close = closeCon();
         else if (ret == QMessageBox::No) 
         	close = false;
    }
    
    return close; 
}

void MainWindow::rebuildBufferList() {
	
	QString LastBufferName = m_BufferCB->currentText();
	
	QStringList NameList;
	int idx = -1;
	
	for (int i=0; i<m_BufferList.size(); i++) {
		NameList << m_BufferList.at(i).getName();
		if (m_BufferList.at(i).getName() == LastBufferName) {
			idx = i;
		}
	}
	
	m_BufferCB->clear();
	m_BufferCB->addItems(NameList);
	if (idx != -1) {
		m_BufferCB->setCurrentIndex(idx);
	} else {
		m_AppendBufferCB->setChecked(false);
	}
	m_AppendBufferCB->setEnabled(m_BufferList.size() > 0);
}

void MainWindow::addBuffer() {

	ValueBuffer BBuf;

	QStringList SList;
	
	SList << "";
	
	BBuf.setBuffer(SList);

 	EditBufferDialog dialog(&BBuf,true,&m_BufferList);
 	
 	dialog.exec();
 	
 	if (dialog.result() == QDialog::Accepted) {

		m_BufferList << BBuf;

		addBufferRow(BBuf);
		
		updateBufferButtonStates();
 	
 		m_ProjectSaved = false;
 		
 		rebuildBufferList();
 	}
}

void MainWindow::addBufferRow(ValueBuffer BBuf) {
	
	int count = m_BufferTable->rowCount();
	m_BufferTable->insertRow(count);

	QTableWidgetItem *newItem = 0;
			
	newItem = new QTableWidgetItem(BBuf.getName());
	newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
	m_BufferTable->setItem(count,0,newItem);

	newItem = new QTableWidgetItem(QString("%1").arg(BBuf.getBuffer().size()));
	newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
	m_BufferTable->setItem(count,1,newItem);

	m_BufferTable->resizeColumnsToContents();		
}

void MainWindow::removeBuffer() {
	
	int row = getSelectedBufferRow();
	
	if (row > -1) {
		m_BufferTable->removeRow(row);
		m_BufferList.removeAt(row);
		
		rebuildBufferList();
	}
}

void MainWindow::editBuffer() {

	ValueBuffer BBuf;

	BBuf = m_BufferList.at(getSelectedBufferRow());

 	EditBufferDialog dialog(&BBuf,false,&m_BufferList);
 	
 	dialog.exec();
 	
 	if (dialog.result() == QDialog::Accepted) {

		unsigned int idx = getSelectedBufferRow(); 

		m_BufferList[idx] = BBuf;
		
		QTableWidgetItem *newItem = 0;
			
		newItem = new QTableWidgetItem(BBuf.getName());
		newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
		m_BufferTable->setItem(idx,0,newItem);

		newItem = new QTableWidgetItem(QString("%1").arg(BBuf.getBuffer().size()));
		newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
		m_BufferTable->setItem(idx,1,newItem);

		m_BufferTable->resizeColumnsToContents();		
		
		m_ProjectSaved = false;
		
		rebuildBufferList();
 	}
}

void MainWindow::sendBuffer() {

	if (getSelectedBufferRow() > -1) {

		ValueBuffer Buffer;
		
		Buffer = m_BufferList.at(getSelectedBufferRow());

		if (Buffer.getType() == ValueBuffer::None) {
			setBufferAttributes(&Buffer);
		}
		
		if (sendData(Buffer)) {
		}
	} 
}

int MainWindow::getSelectedBufferRow() {

	QList<QTableWidgetItem *> List = m_BufferTable->selectedItems();

	if (List.size() > 0) {
		return List.at(0)->row();
	} else {
		return (-1);
	} 	
}

void MainWindow::updateBufferButtonStates() {
	QList<QTableWidgetItem *> List = m_BufferTable->selectedItems();
	
	m_BufferRemoveButton->setEnabled(List.size() > 0);
	m_BufferEditButton->setEnabled(List.size() > 0);
	m_BufferSendButton->setEnabled((List.size() > 0) && (StreamIsOpen));
}

void MainWindow::BufferItemSelectionChanged() {
	updateBufferButtonStates();
}

void MainWindow::addEvent() {

	QStringList SList;
	
	SList << "";
	
	Event Ev;
	
	Ev.setBuffer(SList);

	EditEventDialog dialog(&Ev,true,&m_EventList);
 	
 	dialog.exec();
 	
 	if (dialog.result() == QDialog::Accepted) {
 		
 		m_EventList << Ev;
 		
 		addEventRow(Ev);
 		
		updateEventButtonStates();
		
		m_ProjectSaved = false;
 	}
}


void MainWindow::addEventRow(Event Ev) {
	
	int count = m_EventTable->rowCount();
	m_EventTable->insertRow(count);

	QTableWidgetItem *newItem = 0;

	newItem = new QTableWidgetItem(Ev.getName());
	newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
	m_EventTable->setItem(count,0,newItem);

	QString StateStr; 

	if (Ev.isActive()) StateStr = "Enabled";
	else StateStr = "Disabled"; 

	newItem = new QTableWidgetItem(StateStr);
	newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
	m_EventTable->setItem(count,1,newItem);

	QString ActionStr = "Highlight Byte Sequence";
	newItem = new QTableWidgetItem(ActionStr);
	newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
	m_EventTable->setItem(count,2,newItem);

	m_EventTable->resizeColumnsToContents();
}


void MainWindow::removeEvent() {
	
	int row = getSelectedEventRow();
	
	if (row > -1) {
		m_EventTable->removeRow(row);
		m_EventList.removeAt(row);
	}
}

void MainWindow::editEvent() {

	Event Ev;
	
	Ev = m_EventList.at(getSelectedEventRow());

	EditEventDialog dialog(&Ev,false,&m_EventList);
 	
 	dialog.exec();
 	
 	if (dialog.result() == QDialog::Accepted) {
 		
 		
 		unsigned int idx = getSelectedEventRow();
 		
 		m_EventList[idx] = Ev;
 		
		QTableWidgetItem *newItem = 0;
			
		newItem = new QTableWidgetItem(Ev.getName());
		newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
		m_EventTable->setItem(idx,0,newItem);

		QString StateStr; 
		
		if (Ev.isActive()) StateStr = "Enabled";
		else StateStr = "Disabled"; 

		newItem = new QTableWidgetItem(StateStr);
		newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
		m_EventTable->setItem(idx,1,newItem);

		QString ActionStr = "Highlight Byte Sequence";
		newItem = new QTableWidgetItem(ActionStr);
		newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
		m_EventTable->setItem(idx,2,newItem);

		m_EventTable->resizeColumnsToContents();
		updateEventButtonStates();
		
		m_ProjectSaved = false;
 	}
}

int MainWindow::getSelectedEventRow() {

	QList<QTableWidgetItem *> List = m_EventTable->selectedItems();

	return List.at(0)->row(); 	
}

void MainWindow::updateEventButtonStates() {
	QList<QTableWidgetItem *> List = m_EventTable->selectedItems();
	
	m_EventRemoveButton->setEnabled(List.size() > 0);
	m_EventEditButton->setEnabled(List.size() > 0);
}

void MainWindow::EventItemSelectionChanged() {
	updateEventButtonStates();
}

QString MainWindow::getTimestamp() {
	
	QString ts;
	
	QDate date = QDate::currentDate();
	
	ts = date.toString(Qt::ISODate);

	QTime time = QTime::currentTime();
	
	ts += " " + time.toString(Qt::ISODate) +"."+QString("%0").arg(time.msec());

	return ts;
}

void MainWindow::loadrecentFile(QAction * action) {

	loadProjectFile(m_recentFiles.at(action->data().toInt()));
}

void MainWindow::saveBuffer() {
	
	SaveBufferDlg dlg(m_saveTxBuffer,m_saveRxBuffer);
	
	dlg.exec();
	
	m_saveTxBuffer = dlg.saveTxBuffer();
	m_saveRxBuffer = dlg.saveRxBuffer();

	if (dlg.result() == QDialog::Accepted) {
		
		m_SaveBufferFilename = dlg.Filename();
		
		QFile file(m_SaveBufferFilename);
		
		if (file.open(QIODevice::WriteOnly)) {
			
			bool error = false;
			
			for (unsigned int i=0; i<RxTxValueBuffer.size(); i++) {
				
				char c = RxTxValueBuffer.at(i).Value();
				
				if ((RxTxValueBuffer.at(i).Type() == TX_VALUE) && m_saveTxBuffer) {
					if (file.write(&c,1) == -1) {
						error = true;
						break;
					}
				}
				if ((RxTxValueBuffer.at(i).Type() == RX_VALUE) && m_saveRxBuffer) {
					if (file.write(&c,1) == -1) {
						error = true;
						break;
					}
				}
			}
			
			file.close();
			
			if (error) {
				QMessageBox::critical(this, tr("Error"),
    				"An error occured while writing the data!",QMessageBox::Ok);
			}
		} else {
			QMessageBox::critical(this, tr("Error"),
    				"The file could not be opened!",QMessageBox::Ok);
		}
	}
}

void MainWindow::clearBuffer() {

	while (TranceiverTable->rowCount() > 0) {
		TranceiverTable->removeRow(0);
	}
	RxTxValueBuffer.clear();
	SendCount = 0;
	ReceivedCount = 0;
	
	TranceiverTable->resizeColumnsToContents();
}

void MainWindow::addDialog() {
	
	Dialog dlg;

	EditDialogDialog dialog(&dlg,true,&m_DialogList);
 	
 	dialog.exec();
 	
 	if (dialog.result() == QDialog::Accepted) {

 		m_DialogList << dlg;

 		addDialogRow(dlg);
 		
 		updateDialogButtonStates();

		m_ProjectSaved = false;
 	}
}


void MainWindow::editDialog() {

	Dialog Dlg;

	Dlg = m_DialogList.at(getSelectedDialogRow());

	EditDialogDialog dialog(&Dlg,false,&m_DialogList);
 	
 	dialog.exec();
 	
 	if (dialog.result() == QDialog::Accepted) {
 		
 		unsigned int idx = getSelectedDialogRow();
 		
 		m_DialogList[idx] = Dlg;
 		
		QTableWidgetItem *newItem = 0;
			
		newItem = new QTableWidgetItem(Dlg.Name());
		newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
		m_DialogTable->setItem(idx,0,newItem);

		m_DialogTable->resizeColumnsToContents();
		updateDialogButtonStates();
		
		m_ProjectSaved = false;
 	}
}

void MainWindow::removeDialog() {
	
	int row = getSelectedDialogRow();
	
	if (row > -1) {
		m_DialogTable->removeRow(row);
		m_DialogList.removeAt(row);
	}
}

void MainWindow::addDialogRow(Dialog Dlg) {

	int count = m_DialogTable->rowCount();
	m_DialogTable->insertRow(count);

	QTableWidgetItem *newItem = 0;

	newItem = new QTableWidgetItem(Dlg.Name());
	newItem->setFlags(newItem->flags() & (~Qt::ItemIsEditable));
	m_DialogTable->setItem(count,0,newItem);

	m_DialogTable->resizeColumnsToContents();
}

void MainWindow::updateDialogButtonStates() {
	
	QList<QTableWidgetItem *> List = m_DialogTable->selectedItems();
	
	m_DialogRemoveButton->setEnabled(List.size() > 0);
	m_DialogEditButton->setEnabled(List.size() > 0);
	m_DialogOpenButton->setEnabled(List.size() > 0);
}

void MainWindow::DialogItemSelectionChanged() {
	updateDialogButtonStates();
}

void MainWindow::openDialog() {

	QString DlgName = m_DialogList.at(getSelectedDialogRow()).Name();
	
	QMap<QString, QDialog*>::iterator iter = m_DialogMap.find(DlgName);
	
	if (iter == m_DialogMap.end()) {

		VariableDialog *dlg = new VariableDialog(&m_DialogList.at(getSelectedDialogRow()),this);
		dlg->setAttribute(Qt::WA_DeleteOnClose);
		connect(dlg,SIGNAL(closeDialog(QString)),this,SLOT(DialogClosed(QString)));
		connect(dlg,SIGNAL(sendData(QByteArray&)),this,SLOT(sendDialogData(QByteArray&)));
		dlg->show();
		
		m_DialogMap.insert(DlgName,dlg);
	
	} else {
		iter.value()->show();
	}
}

void MainWindow::DialogClosed(QString name) {

	m_DialogMap.remove(name);
}

void MainWindow::sendDialogData(QByteArray &ba) {

	if (StreamIsOpen) {
		bool stateOk = false;

		QString Timestamp = getTimestamp();
		stateOk = serialThread->sendData(ba);

		if (stateOk) {
			appendBufferToTransceiverTable(Timestamp,ba,TX_VALUE);
		}
	}
}

int MainWindow::getSelectedDialogRow() {
	
	QList<QTableWidgetItem *> List = m_DialogTable->selectedItems();

	return List.at(0)->row(); 	
}

void MainWindow::viewTools() {
	if (m_viewTools = viewToolsAct->isChecked()) fileToolBar->show();
	else fileToolBar->hide();
}

void MainWindow::viewSendType() {
	if (m_viewSendType = viewSendTypeAct->isChecked()) horizontalGroupBox->show();
	else horizontalGroupBox->hide();
}

void MainWindow::viewWidth() {
	if (m_viewWidth = viewWidthAct->isChecked()) horizontalGroupBox3->show();
	else horizontalGroupBox3->hide();
}

void MainWindow::viewSign() {
	if (m_viewSign = viewSignAct->isChecked()) horizontalGroupBox4->show();
	else horizontalGroupBox4->hide();
}

void MainWindow::viewByteOrder() {
	if (m_viewByteOrder = viewByteOrderAct->isChecked()) horizontalGroupBox2->show();
	else horizontalGroupBox2->hide();
}

void MainWindow::viewCRC() {
	if (m_viewCRC = viewCRCAct->isChecked()) horizontalGroupBox5->show();
	else horizontalGroupBox5->hide();
}

