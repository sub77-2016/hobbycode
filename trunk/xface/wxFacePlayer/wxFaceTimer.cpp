#include "wxFaceTimer.h"
#include <wx/utils.h>
#include <wx/timer.h>

wxFaceTimer::wxFaceTimer(void)
{
	start();
}

wxFaceTimer::~wxFaceTimer(void)
{
}

unsigned long wxFaceTimer::start()
{
	m_lastMark = m_startTime = 0;
	::wxStartTimer();
	return m_startTime;
}

void wxFaceTimer::wait(unsigned long sometime)
{
	::wxMilliSleep(sometime);
}

unsigned long wxFaceTimer::getElapsedTime(bool mark)
{
	unsigned long now = ::wxGetElapsedTime(false);
	unsigned long retVal = now - m_lastMark;
	if(mark)
		m_lastMark = now;
	return retVal;
}

unsigned long wxFaceTimer::getTotalElapsedTime(bool mark)
{
	unsigned long now = ::wxGetElapsedTime(false);
	if(mark)
		m_lastMark = now;

	return now - m_startTime;
}
