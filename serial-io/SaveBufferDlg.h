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

#ifndef SAVEBUFFERDLG_H_
#define SAVEBUFFERDLG_H_

#include <QtGui>

//! A Dialog for saving the Rx/Tx buffer.
class SaveBufferDlg : public QDialog
{
	Q_OBJECT
	
public:
	SaveBufferDlg(bool saveTxData, bool saveRxData);
	virtual ~SaveBufferDlg();
	bool saveRxBuffer();
	bool saveTxBuffer();
	QString Filename();
	
public slots:
	void selectFilename();	
	
protected:
    void closeEvent(QCloseEvent *event);

private:	
	//! The method saves the dialog config, e.g. position
	void saveConfig();
	//! The method loads the dialog config, e.g. position
	void loadConfig();
	
	//! The variable stores the directory used in the file selection dialog.
	QString m_Directory;
	//! The variable stores the current filename of the buffer.
	QString m_Filename;

	QLineEdit *m_LineEdit;
	QPushButton *m_openButton;
	QGroupBox *m_SaveGroupBox;
	QCheckBox *m_RxCBox;
	QCheckBox *m_TxCBox;
	QDialogButtonBox *m_ButtonBox;
};

#endif /*SAVEBUFFERDLG_H_*/
