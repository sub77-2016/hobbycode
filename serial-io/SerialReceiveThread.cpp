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

#include <iostream>
#include "SerialReceiveThread.h"

using namespace std;


SerialReceiveThread::SerialReceiveThread(QObject *parent) : QThread(parent)
{
	m_Abort = false;
	m_ThreadSleep = 100*1000;
}

SerialReceiveThread::~SerialReceiveThread(	// QString s = QString("0x%1").arg(value,2,16);
	
)
{
	m_Mutex.lock();
    m_Abort = true;
    m_Mutex.unlock();

    wait();
    
    m_serialStream->Close();
    delete m_serialStream;
}


void SerialReceiveThread::startThread(SerialStream *serialStr, unsigned int ThreadSleep)
{
	QMutexLocker locker(&m_Mutex);

	if (!isRunning()) {

		m_serialStream = serialStr;
		
		m_ThreadSleep = ThreadSleep*1000;

    	start(LowPriority);
	} 
}

void SerialReceiveThread::stopThread() 
{
	QMutexLocker locker(&m_Mutex);
	
	m_Abort = true;
}


void SerialReceiveThread::run()
{
	forever {

	    if (m_serialStream->rdbuf()->in_avail() > 0) {
	    	QByteArray sendBuffer;
	    	char ch;
	    	
	    	while (m_serialStream->rdbuf()->in_avail() > 0) {
	    		m_serialStream->get(ch);
	    		sendBuffer.append(ch);
	    	}
			emit receivedData(sendBuffer);
	    }

		usleep(m_ThreadSleep);
		
		m_Mutex.lock();
		if (m_WriteBuffer.size() > 0) {
 			m_serialStream->write(m_WriteBuffer.data(),m_WriteBuffer.size());
 			m_WriteBuffer.clear();
		} 		
		m_Mutex.unlock();
		
		if (m_Abort) {
			return;
		}
	}
}

bool SerialReceiveThread::sendData(QByteArray buffer) 
{
	QMutexLocker locker(&m_Mutex);
	
	m_WriteBuffer.append(buffer);

	return true;
}


