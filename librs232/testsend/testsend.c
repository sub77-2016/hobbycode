/*
* This source code is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*       
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* File Name: testsend.c							
*
* Reference:
*
* Author: Li Feng                                                   
*
* Description:
*
* 	
* 
* History:
* 11/18/2005  Li Feng    Created
*  
*
*CodeReview Log:
* 
*/

#include <stdio.h>
#include "librs232.h"

int main(int argc, char** argv)
{
	HRS232 hRS232 = NULL;
	RS232_PARAM param;
	FILE *fp=NULL;
	char chrSend;

	hRS232 = RS232_Open  ("/dev/ttyS0");

	if(hRS232==NULL)
		return 0;

	param.BaudRate = BR_9600;
	param.DataBits = DB_8;
	param.Parity = PARITY_NONE;
	param.StopBits = SB_1;
	param.xonxoff = 0;
	param.rtscts = 0;

	RS232_SetParam(hRS232, &param);
	fp = fopen("send.txt", "rb");
	if(fp==NULL)
	{
		RS232_Close (hRS232);
		return 0;
	}
	while(1)
	{
		if(fread(&chrSend, 1,1, fp)!=1)
			break;
		RS232_Send (hRS232, &chrSend,1);
		printf("send %c\n", chrSend);
		usleep(1000);
	}
	RS232_Close (hRS232);

	fclose(fp);
	return 0;
}
