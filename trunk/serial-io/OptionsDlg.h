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

#ifndef OPTIONSDLG_H_
#define OPTIONSDLG_H_

#include <QtGui>

//! Application configuration dialog
class OptionsDlg : public QDialog
{
	Q_OBJECT
	
public:
	OptionsDlg(unsigned int TxBSize, unsigned int RxBSize, 
			unsigned int Timing, unsigned int Columns, 
			unsigned int ValueR, bool Scroll, bool Select, QColor RxCol, 
			QColor TxCol, bool LastConfig);
	virtual ~OptionsDlg();
	
	unsigned int RxBufferSize();
	unsigned int TxBufferSize();
	unsigned int ThreadSleep();
	unsigned int ColumnCount();
	unsigned int ValueRepresentation();
	bool ScrollToBottom();
	bool SelectLastValues();
	QColor RxColor() const;
	QColor TxColor() const;
	bool LastConfig() const;
	
protected:
    void closeEvent(QCloseEvent *event);
    
private slots:
	void selectRxColor();
	void selectTxColor();

private:
	//! The method save the dialog config, e.g. position
	void saveConfig();
	//! The method loads the dialog config, e.g. position
	void loadConfig();

	QSpinBox *m_RxBufferSize;
	QSpinBox *m_TxBufferSize;
	QSpinBox *m_ThreadSleep;
	QSpinBox *m_ColumnCount;	
	QGroupBox *m_RepresentationGroupBox;
	QGroupBox *m_ColorGroupBox;
	QLabel *m_RxColorLabel;
	QLabel *m_TxColorLabel;
	QColor m_RxColor;
	QColor m_TxColor; 
	QPushButton *m_selectRxColor;
	QPushButton *m_selectTxColor;
	QRadioButton *m_DecRadio0;
	QRadioButton *m_HexRadio1;
	QButtonGroup *m_RepresentationButtonGroup;
	QCheckBox *m_ScrollToBottom;
	QCheckBox *m_SelectLastValues;
	QDialogButtonBox *m_ButtonBox;
	QCheckBox *m_LoadLastConfig;
};

#endif /*OPTIONSDLG_H_*/
