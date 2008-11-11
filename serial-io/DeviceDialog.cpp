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
#include <QtGui>
#include <QRect>
#include "DeviceDialog.h"

using namespace std;


DeviceDialog::DeviceDialog(int BIndex, int FCIndex, int PIndex, int CSIndex, int SBIndex, QString DevName)
{
	BaudrateIndex = BIndex;
	FlowControlIndex = FCIndex;
	ParityIndex = PIndex;
	CharSizeIndex = CSIndex;
	StopBitsIndex = SBIndex;

    loadConfig();
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    
    QHBoxLayout *BaudrateLayout = new QHBoxLayout;
    
    QLabel *Blabel = new QLabel("Baudrate:");
    BaudrateLayout->addWidget(Blabel);
    
    QStringList BaudrateList;
    BaudrateList << "50";
    BaudrateList << "75";
    BaudrateList << "110";
    BaudrateList << "134.5";
    BaudrateList << "150";
    BaudrateList << "200";
    BaudrateList << "300";
    BaudrateList << "600";
    BaudrateList << "1200";
    BaudrateList << "1800";
    BaudrateList << "2400";
    BaudrateList << "4800";
    BaudrateList << "9600";
    BaudrateList << "19200";
    BaudrateList << "38400";
    BaudrateList << "57600";
    BaudrateList << "115200";

	BaudrateCBox = new QComboBox;
    BaudrateCBox->addItems(BaudrateList);
    BaudrateCBox->setCurrentIndex(BaudrateIndex);
    connect(BaudrateCBox,SIGNAL(currentIndexChanged(int)),this,SLOT(BaudrateChanged(int)));
    BaudrateLayout->addWidget(BaudrateCBox);

    mainLayout->addLayout(BaudrateLayout);
    
    QHBoxLayout *FlowControlLayout = new QHBoxLayout;
    
    QLabel *Flabel = new QLabel("Flow Control:");
    FlowControlLayout->addWidget(Flabel);
    
    QStringList FlowControlList;
    FlowControlList << "Hardware";
    FlowControlList << "Software";
    FlowControlList << "No";

    FlowControlCBox = new QComboBox(this);
    FlowControlCBox->addItems(FlowControlList);
    FlowControlCBox->setCurrentIndex(FlowControlIndex);
    connect(FlowControlCBox,SIGNAL(currentIndexChanged(int)),this,SLOT(FlowControlChanged(int)));
    FlowControlLayout->addWidget(FlowControlCBox);

    mainLayout->addLayout(FlowControlLayout);
    
    
    QHBoxLayout *ParityLayout = new QHBoxLayout;
    
    QLabel *ParityLabel = new QLabel("Parity:");
    ParityLayout->addWidget(ParityLabel);
    
    QStringList ParityList;
    ParityList << "Even";
    ParityList << "Odd";
    ParityList << "No";
    
    ParityCBox = new QComboBox(this);
    ParityCBox->addItems(ParityList);
    ParityCBox->setCurrentIndex(ParityIndex);
    connect(ParityCBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ParityChanged(int)));
    ParityLayout->addWidget(ParityCBox);
    
    mainLayout->addLayout(ParityLayout);   
    
    
    QHBoxLayout *StopBitsLayout = new QHBoxLayout;
    
    QLabel *StopBitsLabel = new QLabel("Stop Bits:");
    StopBitsLayout->addWidget(StopBitsLabel);
    
    QStringList StopBitsList;
    StopBitsList << "1";
    StopBitsList << "2";
    
    StopBitsCBox = new QComboBox(this);
    StopBitsCBox->addItems(StopBitsList);
    StopBitsCBox->setCurrentIndex(StopBitsIndex);
    connect(StopBitsCBox,SIGNAL(currentIndexChanged(int)),this,SLOT(StopBitsChanged(int)));
    StopBitsLayout->addWidget(StopBitsCBox);
    
    mainLayout->addLayout(StopBitsLayout);
    
    
    QHBoxLayout *CharSizeLayout = new QHBoxLayout;
    
    QLabel *CharSizeLabel = new QLabel("Character Size:");
    CharSizeLayout->addWidget(CharSizeLabel);
    
    QStringList SizeList;
    SizeList << "5";
    SizeList << "6";
	SizeList << "7";
	SizeList << "8";
	
	CharSizeCBox = new QComboBox(this);
	CharSizeCBox->addItems(SizeList);
	CharSizeCBox->setCurrentIndex(CharSizeIndex);
	connect(CharSizeCBox,SIGNAL(currentIndexChanged(int)),this,SLOT(CharSizeChanged(int)));
	CharSizeLayout->addWidget(CharSizeCBox);
	
	mainLayout->addLayout(CharSizeLayout);
    
    QHBoxLayout *DeviceLayout = new QHBoxLayout;

	QLabel *label = new QLabel("Serial Device:");
	DeviceLayout->addWidget(label);

	QStringList DeviceList;
	DeviceList << "/dev/ttyS0";
	DeviceList << "/dev/ttyS1";
	DeviceList << "/dev/ttyS2";
	DeviceList << "/dev/ttyS3";
	DeviceList << "/dev/ttyUSB0";
	DeviceList << "/dev/ttyUSB1";
	DeviceList << "/dev/ttyUSB2";
	DeviceList << "/dev/ttyUSB3";
	
	DeviceCBox = new QComboBox(this);
	DeviceCBox->addItems(DeviceList);
	DeviceCBox->setEditable(true);
	DeviceCBox->setEditText(DevName);
	DeviceLayout->addWidget(DeviceCBox);
	
	mainLayout->addLayout(DeviceLayout);
	
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Open
                                      | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    mainLayout->addWidget(buttonBox);

	setLayout(mainLayout);	

    setWindowTitle(tr("Open Device"));
}

DeviceDialog::~DeviceDialog() {
	saveConfig();
}

void DeviceDialog::closeEvent(QCloseEvent *event)
{
    if (true) {
        event->accept();
    } else {
        event->ignore();
    }
}

void DeviceDialog::BaudrateChanged(int index)
{
	BaudrateIndex = index;
}

void DeviceDialog::FlowControlChanged(int index)
{
	FlowControlIndex = index;
}

void DeviceDialog::ParityChanged(int index) 
{
	ParityIndex = index;
}

void DeviceDialog::CharSizeChanged(int index)
{
	CharSizeIndex = index;
}

void DeviceDialog::StopBitsChanged(int index) 
{
	StopBitsIndex = index;
}

int DeviceDialog::getBaudrate() const
{
	return BaudrateIndex;
}

int DeviceDialog::getFlowControl() const
{
	return FlowControlIndex;
}

int DeviceDialog::getParity() const
{
	return ParityIndex;
}

int DeviceDialog::getCharSize() const
{
	return CharSizeIndex;
}

int DeviceDialog::getStopBits() const
{
	return StopBitsIndex;
}

QString DeviceDialog::getDeviceName() const
{
	return DeviceCBox->currentText();
}

void DeviceDialog::saveConfig() {
	
	QSettings settings;
	
	settings.beginGroup("DeviceDialog");
	settings.setValue("pos",pos());
	settings.endGroup();
}

void DeviceDialog::loadConfig() {
	
	QSettings settings;
	
	settings.beginGroup("DeviceDialog");
	if (settings.contains("pos")) {
		QPoint pos = settings.value("pos").toPoint();
		move(pos);
	}
	settings.endGroup();
}
