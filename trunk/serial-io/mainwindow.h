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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <SerialStream.h>

#include <QDialog>
#include <QButtonGroup>
#include <QDomDocument>

#include "SerialReceiveThread.h"
#include "ValueBuffer.h"
#include "Event.h"
#include "Dialog.h"
#include "RxTxValue.h"

using namespace std;
using namespace LibSerial;

class QAction;
class QMenu;
class QTextEdit;
class QMenuBar;
class QToolBar;
class QStatusBar;
class QLineEdit;
class QGroupBox;
class QRadioButton;
class QAbstractButton;
class QTabWidget;
class QFrame;
class QHBoxLayout;
class QVBoxLayout;
class QTableWidget;
class QPushButton;
class QDomDocument;


//! The main window of the application.
/*! 
  The class show the pulldown menu, toolbar, options, transceiver window and a tabbar
  for the comandline, send buffer and events.
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
	//! The constructor.
    MainWindow();

protected:
	//! The close event method is called if the window is closed. 
    void closeEvent(QCloseEvent *event);

private slots:
	//! The method is called if a device should be opened.
    void openDevice();
    //! The method is called if the connection should be closed.
    bool closeCon();
    
    //! The method is called if the user pressed "return" in the command line tab.
    void returnPressed();

    //! The method is called if the user clicks the "send" button
    void sendClicked();

    //! The method is called if the user types a character
    void characterTyped(const QString & text);
    
    //! The method is called if changes "Append buffer" check box
    void appendBufferChanged(int state);
    
    //! The method shows an about dialog.
    void about();
    
    //! The method is called if the user triggers a recent file menu
    void loadrecentFile(QAction * action); 
    
    //! The method opens a user selected project. 
    void loadProject();
    //! The method saves the current buffers and events.
    void saveProject();
    
    //! The method opens a dialog to edit the application related options.
    void editOptions();
    
    //! The method opens a dialog for saving the transceiver buffer.
    void saveBuffer();
    
    //! The method clears the transceiver buffer.
    void clearBuffer();
    
    //! The method is called from serialThread to transmitt the received bytes. 
    void updateReceiveBuffer(QByteArray buffer);
    
    //! The method is called if another type is selected.
    void TypeButtonClicked(int id);
    //! The method is called if another width is selected.
    void WidthButtonClicked(int id);
    //! The method is called if signed or unsigned is selected.
    void SignButtonClicked(int id);
    //! The method is called if the byte order is changed.
    void OrderButtonClicked(int id);
    void CRCButtonClicked(int id);
    
    void addBuffer();
    void removeBuffer();
    void editBuffer();
    void sendBuffer();
	void BufferItemSelectionChanged();    

    void addEvent();
    void removeEvent();
    void editEvent();
    void EventItemSelectionChanged();
    
    void addDialog();
    void removeDialog();
    void editDialog();
    void openDialog();
    void DialogItemSelectionChanged();
    void DialogClosed(QString name);
    void sendDialogData(QByteArray &ba);
    
    void viewTools();
    void viewSendType();
    void viewWidth();
    void viewSign();
    void viewByteOrder();
    void viewCRC();

private:
	void loadLastConfig();
	
	void checkInput(QString s);
	bool checkNumber(QString s);
	
	void prepareDataAndSend();
	
	void setBufferAttributes(ValueBuffer *Buffer);
	
	void setSerialStreamAttributes(SerialStream &serialStr, int Baudrate, int FlowControl, int Parity, int CharSize, int NumOfStopBits);

	void appendBufferToTransceiverTable(QString Ts, QByteArray Buffer, e_ValueType Type);
	QString getValueRepresentation(unsigned int ival);
	QTableWidgetItem* newTableWidgetItem(unsigned int ival, e_ValueType Type);
	void TxBufferDeleteFirstBytes();
	void RxBufferDeleteFirstBytes();
	void highlightLastBytes(unsigned int count, QColor color);
	bool compareSequence(QByteArray rList, QByteArray eList); 
	QByteArray getReceivedBytes();

	void loadProjectFile(QString FileName);
	
	void BufferListToTable();
	void EventListToTable();
	void DialogListToTable();
	
	void createDocument(QDomDocument &doc);
	bool loadDocument(QDomDocument &doc);
	bool loadBuffersFromDoc(QDomNode buffers);
	bool loadEventsFromDoc(QDomNode buffers);
	bool loadDialogsFromDoc(QDomNode events);
	
	void removeAllEvents();
	void removeAllBuffers();
	void removeAllDialogs();

	bool sendData(ValueBuffer Buffer);
	void calcChecksum(QByteArray &Buffer, ValueBuffer::e_ChecksumType Type);
	bool prepareBuffer(ValueBuffer Buffer, QByteArray & ByteBuffer);
	bool checkUnsignedBounds(unsigned int upperBound, vector<unsigned int> &v);
	bool checkSignedBounds(signed int lowerBound, signed int upperBound, vector<signed int> &v);
	bool unsignedAnalyseInput(QStringList List, vector<unsigned int> &v);
	bool signedAnalyseInput(QStringList List, vector<signed int> &v);
			QStringList splitInput(QString S);
	void toNewRepresentation();
	void readdValues();
	
	void checkEvents();
    
	void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createTypeBox();
    void createWidthBox();
    void createSignBox();
    void createByteOrderBox();
    void createCRCBox();
    
    void addRecentFilesToMenu();
    void addRecentFile(QString FileName);

    void readSettings();
    void writeSettings();
    
	bool maybeSave();
	
	void rebuildBufferList();
	
	void addBufferRow(ValueBuffer BBuf);
    void updateBufferButtonStates();
    int getSelectedBufferRow();
    
    void addEventRow(Event Ev);
    void updateEventButtonStates();
    int getSelectedEventRow();
    
    QString getTimestamp();

    void addDialogRow(Dialog Dlg);
    void updateDialogButtonStates();
    int getSelectedDialogRow();
	

	// Gui stuff
	QMenuBar *menuBar;
	QStatusBar *statusBar;

    QMenu *fileMenu;
    QMenu *recentMenu;
    QMenu *conMenu;
    QMenu *bufferMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QToolBar *fileToolBar;
    
    QAction *openAct;
    QAction *closeAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *optionAct;
    QAction *loadAct;
    QAction *saveAct;
    QAction *saveBufferAct;
    QAction *clearBufferAct;
    QAction *viewToolsAct;
    QAction *viewSendTypeAct;
    QAction *viewWidthAct;
    QAction *viewSignAct;
    QAction *viewByteOrderAct;
    QAction *viewCRCAct;
     
	QTableWidget *TranceiverTable;

    QGroupBox *horizontalGroupBox;
    QGroupBox *horizontalGroupBox2;
    QGroupBox *horizontalGroupBox3;
    QGroupBox *horizontalGroupBox4;
    QGroupBox *horizontalGroupBox5;
     
    QButtonGroup *TypeButtonGroup;
    QButtonGroup *WidthButtonGroup;
    QButtonGroup *SignButtonGroup;
    QButtonGroup *OrderButtonGroup;
    QButtonGroup *CRCButtonGroup;
     
    QRadioButton *radio0;
    QRadioButton *radio1;
    
    QRadioButton *radioW0;
    QRadioButton *radioW1;
    QRadioButton *radioW2;

    QRadioButton *radioS0;
    QRadioButton *radioS1;

    QRadioButton *radioBo0;
    QRadioButton *radioBo1;
    
    QRadioButton *radioCRC0;
    QRadioButton *radioCRC1;
    QRadioButton *radioCRC2;
    
    QTabWidget *m_Tab;
    
    QFrame *m_LineEditFrame;
    QVBoxLayout *m_LineEditVLayout;
    QHBoxLayout *m_LineEditHLayout;
    QComboBox *m_HistoryCB;
    QLineEdit *m_LineEdit;
    QPushButton *m_SendButton;
    QCheckBox *m_AppendBufferCB;
    QComboBox *m_BufferCB;
    QLabel *m_ErrorLabel;
    
	QFrame *m_BufferFrame;
	QHBoxLayout *m_BufferHLayout;
	QTableWidget *m_BufferTable;
	QVBoxLayout *m_BufferVLayout;
	QPushButton *m_BufferAddButton;
	QPushButton *m_BufferRemoveButton;
	QPushButton *m_BufferEditButton;
	QPushButton *m_BufferSendButton;

	QFrame *m_EventFrame;
	QHBoxLayout *m_EventHLayout;
	QTableWidget *m_EventTable;
	QVBoxLayout *m_EventVLayout;
	QPushButton *m_EventAddButton;
	QPushButton *m_EventRemoveButton;
	QPushButton *m_EventEditButton;

	QFrame *m_DialogFrame;
	QHBoxLayout *m_DialogHLayout;
	QTableWidget *m_DialogTable;
	QVBoxLayout *m_DialogVLayout;
	QPushButton *m_DialogAddButton;
	QPushButton *m_DialogRemoveButton;
	QPushButton *m_DialogEditButton;
	QPushButton *m_DialogOpenButton;
	
	// GUI state variables
    int TypeButtonId;
    int WidthButtonId;
    int SignButtonId;
    int OrderButtonId;
    int CRCButtonId;

	// Thread
	bool StreamIsOpen;
    SerialReceiveThread *serialThread;
    
    // Device variables
    int BaudrateIndex;
    int FlowControlIndex;
	int ParityIndex; 
	int CharSizeIndex;
	int StopBitsIndex;
	QString DeviceName;
	
	// Lists
	QList<ValueBuffer> m_BufferList;
	QList<Event> m_EventList;
	QList<Dialog> m_DialogList;
	
	// Program state
	bool m_ProjectSaved;

	bool m_viewTools;
	bool m_viewSendType;
	bool m_viewWidth;
	bool m_viewSign;
	bool m_viewByteOrder;
	bool m_viewCRC;
	
	bool m_saveTxBuffer;
	bool m_saveRxBuffer;
	QString m_SaveBufferFilename;
	
	// Rx/Tx Value
	unsigned int m_TxBufferSize;
	unsigned int m_RxBufferSize;
	unsigned int m_ThreadSleep;
	unsigned int m_RxTxColumnCount;
	unsigned int m_RxTxValueRepresentation;
	bool m_ScrollToBottom;
	bool m_SelectLastValues;
	
    unsigned int SendCount;
    unsigned int ReceivedCount;
	vector<RxTxValue> RxTxValueBuffer;
	
	QString m_LastTimestamp;
	
	QColor m_RxColor;
	QColor m_TxColor;
	
	bool m_InputHasErrors;
	
	QRegExp m_NumberRegExp;
	
	//!
	QStringList m_recentFiles;
	
	bool m_LoadLastConfigOnStartup;
	
	QMap<QString, QDialog*> m_DialogMap;
};

#endif /*MAINWINDOW_H*/
