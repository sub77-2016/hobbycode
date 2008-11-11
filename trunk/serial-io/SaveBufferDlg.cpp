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

#include "SaveBufferDlg.h"

SaveBufferDlg::SaveBufferDlg(bool saveTxData, bool saveRxData)
{
	loadConfig();
	
	setWindowTitle(tr("Save Buffer"));
	    
	QVBoxLayout *mainLayout = new QVBoxLayout;

	/****/    
	
	QHBoxLayout *FNameLayout = new QHBoxLayout();
	mainLayout->addLayout(FNameLayout);
	
	FNameLayout->addWidget(new QLabel("Filename:"));

	m_LineEdit = new QLineEdit();
	m_LineEdit->setText(m_Filename);
	FNameLayout->addWidget(m_LineEdit,1);
	
	QPixmap saveImage(":/images/save.png");
	QIcon saveIcon;
	saveIcon.addPixmap(saveImage);
	
	m_openButton = new QPushButton(this);
	m_openButton->setIcon(saveIcon);
	m_openButton->setIconSize(QSize(24,24));
	connect(m_openButton,SIGNAL(clicked()),this,SLOT(selectFilename()));
	
	FNameLayout->addWidget(m_openButton);
	
	/****/
	
	m_SaveGroupBox = new QGroupBox("Save:");
	mainLayout->addWidget(m_SaveGroupBox);
	
	QHBoxLayout *HSLayout = new QHBoxLayout;
	QVBoxLayout *SaveLayout = new QVBoxLayout;
	HSLayout->addLayout(SaveLayout);

	m_TxCBox = new QCheckBox(tr("&Transmitted Data"));
	m_TxCBox->setChecked(saveTxData);
	m_RxCBox = new QCheckBox(tr("&Received Data"));
	m_RxCBox->setChecked(saveRxData);

    SaveLayout->addWidget(m_TxCBox);
    SaveLayout->addWidget(m_RxCBox);
    SaveLayout->addStretch();

    m_SaveGroupBox->setLayout(HSLayout);
    HSLayout->addSpacing(400);
    HSLayout->addStretch();
    
    /****/	
	m_ButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);

    connect(m_ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_ButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    mainLayout->addWidget(m_ButtonBox);

	setLayout(mainLayout);	
}

SaveBufferDlg::~SaveBufferDlg()
{
	saveConfig();
}

void SaveBufferDlg::selectFilename() {
	
	QFileDialog fileDialog(this,"Save Buffer as...",m_Directory,"Buffer (*.bin)");
	fileDialog.setViewMode(QFileDialog::Detail);
	fileDialog.setDefaultSuffix("bin");
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setConfirmOverwrite(true);
	
	if (fileDialog.exec()) {
		QStringList fileNames = fileDialog.selectedFiles();

		m_LineEdit->setText(fileNames.at(0));
	}	
	
	m_Directory = fileDialog.directory().absolutePath();
}

void SaveBufferDlg::closeEvent(QCloseEvent *event)
{
    if (true) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool SaveBufferDlg::saveRxBuffer() {
	return m_RxCBox->isChecked();
}

bool SaveBufferDlg::saveTxBuffer() {
	return m_TxCBox->isChecked();
}

QString SaveBufferDlg::Filename() {
	return m_LineEdit->text();
}

void SaveBufferDlg::saveConfig() {
	
	QSettings settings;
	
	settings.beginGroup("SaveBufferDialog");
	settings.setValue("pos",pos());
	settings.setValue("Directory",m_Directory);
	settings.setValue("Filename",m_LineEdit->text());
	settings.endGroup();
}

void SaveBufferDlg::loadConfig() {
	
	QSettings settings;
	
	settings.beginGroup("SaveBufferDialog");
	if (settings.contains("pos")) {
		QPoint pos = settings.value("pos").toPoint();
		move(pos);
	}
	m_Directory = settings.value("Directory",".").toString();
	m_Filename = settings.value("Filename","").toString();
	settings.endGroup();
}
