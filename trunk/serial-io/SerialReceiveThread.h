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

#ifndef SERIALRECEIVETHREAD_H_
#define SERIALRECEIVETHREAD_H_

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <SerialStream.h>

using namespace LibSerial;


//! A thread for transmitting/receiving values 
class SerialReceiveThread : public QThread
{
	Q_OBJECT
	
public:
	SerialReceiveThread( QObject *parent);
	virtual ~SerialReceiveThread();
	
	void startThread(SerialStream *serialStr, unsigned ThreadSleep);
	void stopThread();
	bool sendData(QByteArray buffer);

signals:
    void receivedData(QByteArray buffer);
	
protected:
	void run();

private:	
	bool m_Abort;
	
	QMutex m_Mutex;
	
	SerialStream *m_serialStream;	
	
	QByteArray m_WriteBuffer;
	
	unsigned int m_ThreadSleep;
};

#endif /*SERIALRECEIVETHREAD_H_*/
