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

#include "OptionsDlg.h"

OptionsDlg::OptionsDlg(unsigned int TxBSize, unsigned int RxBSize, 
	unsigned int Timing, unsigned Columns, unsigned int ValueR, bool Scroll, 
	bool Select, QColor RxCol, QColor TxCol, bool LastConfig)
{
	loadConfig();
	
    setWindowTitle(tr("Options"));
	    
	QVBoxLayout *mainLayout = new QVBoxLayout;

	/****/    
    QHBoxLayout *TxBufferLayout = new QHBoxLayout;
	mainLayout->addLayout(TxBufferLayout);
	
	TxBufferLayout->addWidget(new QLabel("Tx Buffersize:"));
	
	m_TxBufferSize = new QSpinBox(this);
    m_TxBufferSize->setMinimum(1);
    m_TxBufferSize->setMaximum(1048576);
    m_TxBufferSize->setValue(TxBSize);
	TxBufferLayout->addWidget(m_TxBufferSize);

	/****/    
    QHBoxLayout *RxBufferLayout = new QHBoxLayout;
	mainLayout->addLayout(RxBufferLayout);
	
	RxBufferLayout->addWidget(new QLabel("Rx Buffersize:"));
	
	m_RxBufferSize = new QSpinBox(this);
    m_RxBufferSize->setMinimum(1);
    m_RxBufferSize->setMaximum(1048576);
    m_RxBufferSize->setValue(RxBSize);
	RxBufferLayout->addWidget(m_RxBufferSize);

	/****/
	QHBoxLayout *TimingLayout = new QHBoxLayout;
	mainLayout->addLayout(TimingLayout);
	
	TimingLayout->addWidget(new QLabel("Thread Timing (ms):"));
	
	m_ThreadSleep = new QSpinBox(this);
    m_ThreadSleep->setMinimum(1);
    m_ThreadSleep->setMaximum(1000);
    m_ThreadSleep->setValue(Timing);
	TimingLayout->addWidget(m_ThreadSleep);

	/****/
	QHBoxLayout *ColumnLayout = new QHBoxLayout;
	mainLayout->addLayout(ColumnLayout);
	
	ColumnLayout->addWidget(new QLabel("Rx/Tx Column Count:"));
	
	m_ColumnCount = new QSpinBox(this);
	m_ColumnCount->setMinimum(1);
	m_ColumnCount->setMaximum(64);
	m_ColumnCount->setValue(Columns);	
	
	ColumnLayout->addWidget(m_ColumnCount);

	/****/
	
	m_ScrollToBottom = new QCheckBox("Rx/Tx-Table scroll to bottom");
	m_ScrollToBottom->setChecked(Scroll);
	
	mainLayout->addWidget(m_ScrollToBottom);
	
	/****/
	
	m_SelectLastValues = new QCheckBox("Select last Rx/Tx values");
	m_SelectLastValues->setChecked(Select);
	
	mainLayout->addWidget(m_SelectLastValues);
	
	/****/
	
	m_RepresentationGroupBox = new QGroupBox("Rx/Tx Value Representation:");
	mainLayout->addWidget(m_RepresentationGroupBox);
	
	QHBoxLayout *DisplayLayout = new QHBoxLayout;

	m_DecRadio0 = new QRadioButton(tr("&Decimal"));
	m_HexRadio1 = new QRadioButton(tr("&Hex"));

	if (ValueR == 0) m_DecRadio0->setChecked(true); 
    else m_HexRadio1->setChecked(true);

    DisplayLayout->addWidget(m_DecRadio0);
    DisplayLayout->addWidget(m_HexRadio1);
    DisplayLayout->addStretch(10);

    m_RepresentationGroupBox->setLayout(DisplayLayout);
    
    m_RepresentationButtonGroup = new QButtonGroup;
    m_RepresentationButtonGroup->addButton(m_DecRadio0,0);
    m_RepresentationButtonGroup->addButton(m_HexRadio1,1);
    
    
    /****/
    
    m_ColorGroupBox = new QGroupBox("Colors:");
    mainLayout->addWidget(m_ColorGroupBox);

    QVBoxLayout *ColorLayout = new QVBoxLayout();
    m_ColorGroupBox->setLayout(ColorLayout);
    
    QHBoxLayout *RxColorLayout = new QHBoxLayout();
    ColorLayout->addLayout(RxColorLayout);

    m_RxColor = RxCol;
    RxColorLayout->addWidget(new QLabel("Receive:"));
    m_RxColorLabel = new QLabel("  ");
	m_RxColorLabel->setPalette(QPalette(m_RxColor));
   	m_RxColorLabel->setAutoFillBackground(true);
    m_RxColorLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    RxColorLayout->addWidget(m_RxColorLabel);
    m_selectRxColor = new QPushButton("choose");
    connect(m_selectRxColor,SIGNAL(clicked()),this,SLOT(selectRxColor()));
    RxColorLayout->addWidget(m_selectRxColor);
    
    QHBoxLayout *TxColorLayout = new QHBoxLayout();
    ColorLayout->addLayout(TxColorLayout);
    
    m_TxColor = TxCol;
    TxColorLayout->addWidget(new QLabel("Transmit:"));
    m_TxColorLabel = new QLabel("  ");
	m_TxColorLabel->setPalette(QPalette(m_TxColor));
   	m_TxColorLabel->setAutoFillBackground(true);
    m_TxColorLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    TxColorLayout->addWidget(m_TxColorLabel);
    m_selectTxColor = new QPushButton("choose");
    connect(m_selectTxColor,SIGNAL(clicked()),this,SLOT(selectTxColor()));
    TxColorLayout->addWidget(m_selectTxColor);
    
    /****/ 
    QHBoxLayout *LoadLayout = new QHBoxLayout();
    mainLayout->addLayout(LoadLayout);
    
    m_LoadLastConfig = new QCheckBox("Load last project on startup");
    m_LoadLastConfig->setChecked(LastConfig);
    LoadLayout->addWidget(m_LoadLastConfig);
    
	/****/	
	m_ButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);

    connect(m_ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_ButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    mainLayout->addWidget(m_ButtonBox);

	setLayout(mainLayout);	
}

OptionsDlg::~OptionsDlg()
{
	saveConfig();
}

void OptionsDlg::closeEvent(QCloseEvent *event)
{
    if (true) {
        event->accept();
    } else {
        event->ignore();
    }
}

unsigned int OptionsDlg::RxBufferSize() {
	return m_RxBufferSize->value();
}

unsigned int OptionsDlg::TxBufferSize() {
	return m_TxBufferSize->value();
}

unsigned int OptionsDlg::ThreadSleep() {
	return m_ThreadSleep->value();
}

unsigned int OptionsDlg::ColumnCount() {
	return m_ColumnCount->value();
}

unsigned int OptionsDlg::ValueRepresentation() {
	if (m_DecRadio0->isChecked()) return (0);
	else return (1); 	
}

bool OptionsDlg::ScrollToBottom() {
	return m_ScrollToBottom->isChecked();
}

bool OptionsDlg::SelectLastValues() {
	return m_SelectLastValues->isChecked();
}

void OptionsDlg::saveConfig() {
	
	QSettings settings;
	
	settings.beginGroup("OptionsDialog");
	settings.setValue("pos",pos());
	settings.endGroup();
}

void OptionsDlg::loadConfig() {
	
	QSettings settings;
	
	settings.beginGroup("OptionsDialog");
	if (settings.contains("pos")) {
		QPoint pos = settings.value("pos").toPoint();
		move(pos);
	}
	settings.endGroup();
}

void OptionsDlg::selectRxColor() {

	QColor color = QColorDialog::getColor(Qt::green, this);
	
	if (color.isValid()) {
		m_RxColor = color;
		m_RxColorLabel->setPalette(QPalette(color));
       	m_RxColorLabel->setAutoFillBackground(true);
	}
}

void OptionsDlg::selectTxColor() {
	
	QColor color = QColorDialog::getColor(Qt::green, this);
	
	if (color.isValid()) {
		m_TxColor = color;
		m_TxColorLabel->setPalette(QPalette(color));
       	m_TxColorLabel->setAutoFillBackground(true);
	}
}

QColor OptionsDlg::TxColor() const {
	return m_TxColor;
}

QColor OptionsDlg::RxColor() const {
	return m_RxColor;
}

bool OptionsDlg::LastConfig() const {
	return m_LoadLastConfig->isChecked();
}
