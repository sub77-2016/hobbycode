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

#ifndef DEVICEDIALOG_H_
#define DEVICEDIALOG_H_

#include <QDialog>
#include <QDialogButtonBox>

class QComboBox;
class QLineEdit;

//! A dialog for device options  
/*! 
  The class implements a dialog which lets the user choose the
  attributes for the serial connection.
*/
class DeviceDialog : public QDialog
{
	Q_OBJECT
	
public:
	//! The constructor.
	DeviceDialog(int BIndex, int FCIndex, int PIndex, int CSIndex, int SBIndex, QString DevName);
	//! The destructor.
	~DeviceDialog();
	
	//! The method returns the selected baudrate.
	int getBaudrate() const;
	//! The method returns the selected flow control option.
	int getFlowControl() const;
	//! The method returns the selected parity values.
	int getParity() const;
	//! The method returns the selected character size.
	int getCharSize() const;
	//! The method returns the selected number of stop bits.
	int getStopBits() const;
	//! The method returns the selected device.
	QString getDeviceName() const;

protected:
	//! The method is called if the dialog is closed.
    void closeEvent(QCloseEvent *event);

private slots:
	//! The method is called if the user changes the baudrate.
	void BaudrateChanged(int index);
	//! The method is called if the user changes the flow control value. 
	void FlowControlChanged(int index);
	//! The method is called if the user changes the parity. 
	void ParityChanged(int index);
	//! The method is called if the user changes the number of stop bits.
	void StopBitsChanged(int index);
	//! The method is called if the user changers the character size.
	void CharSizeChanged(int index);

private:
	//! The method save the dialog config, e.g. position
	void saveConfig();
	//! The method loads the dialog config, e.g. position
	void loadConfig();

	//! The variable holds the index for the baudrate list. 
	int BaudrateIndex;
	//! The variable holds the index for the flow control list.	
	int FlowControlIndex;
	//! The variable holds the index for the parity list.
	int ParityIndex; 
	//! The variable holds the index for character size list. 
	int CharSizeIndex;
	//! The variable holds the index for the stopbits list.
	int StopBitsIndex;

	//! A combobox for choosing the baudrate.
	QComboBox *BaudrateCBox;
	//! A combobox for choosing the flow control.
	QComboBox *FlowControlCBox;
	//! A combobox for choosing the parity.
	QComboBox *ParityCBox;
	//! A combobox for choosing the character size.
	QComboBox *CharSizeCBox;
	//! A combobox for choosing the number of stop bits.
	QComboBox *StopBitsCBox;
	//! A combobox for choosing the device
	QComboBox *DeviceCBox;
	//! A box for the cancel and ok button.
	QDialogButtonBox *buttonBox;
};

#endif /*DEVICEDIALOG_H_*/
