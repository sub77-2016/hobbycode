/* 
 * LamaProg v 0.1.1
 * Communicanion routines v 0.1.0
 * Jan Parkman   (parkmaj@users.sourceforge.net) 
 */

#pragma warning(disable: 4996)

#include "com.h"
#include <fcntl.h>
#include <stdio.h>
#include <windows.h>

#define STRICT 1 

static HANDLE hCOMHnd;          // Handle serioveho portu


// Initialize port
int com_init(char *s)
{
	DCB DCBData;
	char sPortName[512];

	sprintf(sPortName, "\\\\.\\%s", s);
	hCOMHnd = CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hCOMHnd==INVALID_HANDLE_VALUE)
	{
		#if VERBOSE
			printf("Init failed...\n");
			exit(-1);
		#endif
		return(-1);
	}

  // Initialize it: 9600 bauds, 8 bits of data, no parity and 1 stop bit
	ZeroMemory(&DCBData, sizeof(DCBData));
	DCBData.DCBlength = sizeof(DCBData);
	DCBData.fBinary      = 1;
	DCBData.fParity      = 1;
	DCBData.fOutxCtsFlow = 0;
	DCBData.fOutxDsrFlow = 0;
	DCBData.fDtrControl  = DTR_CONTROL_ENABLE;
	DCBData.fDsrSensitivity = 0;
	DCBData.fTXContinueOnXoff = 1;
	DCBData.fOutX           = 0;
	DCBData.fInX            = 0;
	DCBData.fErrorChar      = 0;
	DCBData.fNull           = 0;
	DCBData.fRtsControl     = RTS_CONTROL_DISABLE;
	DCBData.fAbortOnError   = 0;

	BuildCommDCB("9600,N,8,1", &DCBData);

	if ( !SetCommState(hCOMHnd, &DCBData) )
	{
		#if VERBOSE
			printf("Port parameters setting failed...\n");
			exit(-1);
		#endif
		return(-1);
	}

  return 0;
}


int com_write(char *data)
{
  BOOL bRes;
	DWORD dwWritten;
   
	bRes = WriteFile(hCOMHnd, data, (DWORD)(strlen(data)), &dwWritten, NULL);
		
	if ( !bRes )
	{
		#if VERBOSE
			printf("Put error...\n");
			exit(-1);
		#endif
		return(-1);
	}
	
	if ( dwWritten!=strlen(data) )
	{
		#if VERBOSE
			printf("Put error - buffer full...\n");
			exit(-1);
		#endif
		return(-1);
	}

  return 0;
}


int com_read(char *data)
{
	int i=0;
	int ch;

	while (((ch = com_get()) >= 0))
	{
		data[i++]=ch;
		if ( (ch == '+') || (ch == '-')) break;
	}
	data[i]='\0';
	
	if (ch == '+') return 1;
	if (ch == '-') return 0;
	return -1;
}


int com_put(char data)
{
	BOOL bRes;
	DWORD dwWritten;

	bRes = WriteFile(hCOMHnd, &data, 1, &dwWritten, NULL);

	if ( !bRes )
	{
		#if VERBOSE
		printf("Put error...\n");
		exit(-1);
		#endif
		return(-1);
	}

	if ( dwWritten!=1 )
	{
		#if VERBOSE
		printf("Put error - buffer full...\n");
		exit(-1);
		#endif
		return(-1);
	}

	return 0;
}


int com_get()
{
	BOOL  bRes;
  char  data;
	DWORD dwRead;

  bRes = ReadFile(hCOMHnd, &data, 1, &dwRead, NULL);
  if (!bRes)
	{
      #if VERBOSE
				printf("Get error...\n");
				exit(-1);
      #endif
      return(-1);
	}
	
	if (dwRead!=1)
	{
      #if VERBOSE
		    printf("Timeout...\n");
			  exit(-1);
      #endif
      return(-1);
	}

  return (int) data;
}


int com_exit()
{
  CloseHandle(hCOMHnd);
  return 0;
}
