/* 
 * LamaProg v 0.1.1
 * Communicanion routines v 0.1.0
 * Jan Parkman   (parkmaj@users.sourceforge.net) 
 */

#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "com.h"

#define STRICT 1 

static int dev_fd ;          //descriptor zarizeni (serioveho portu)

int com_init(char *s)
{
  static struct termios termset;

  if((dev_fd = open(s, O_RDWR))== -1)
    {
	#if VERBOSE
        printf("Init failed...\n");
	exit(-1);
	#endif
      return(-1);
    }

  /* Initialize it: 9600 bauds, 8 bits of data, no parity and 1 stop bit */
  tcgetattr(dev_fd, &termset);
  cfmakeraw(&termset);
  termset.c_iflag=0;
  termset.c_oflag=0;
  termset.c_cflag=CS8|CLOCAL|CREAD;
  termset.c_lflag=0;
  
  cfsetispeed(&termset, B9600);
  cfsetospeed(&termset, B9600);
  termset.c_cc[VMIN] = 1;
  termset.c_cc[VTIME] = 1;

/*
  termset.c_cflag = CBAUD & B9600;
  termset.c_cflag &= ~(PARENB | CRTSCTS);
  termset.c_cflag |= (CSIZE & CS8) | CREAD | CLOCAL;
  termset.c_iflag = termset.c_oflag = termset.c_lflag = (ushort) 0 ;
  termset.c_oflag=0;
  */
  tcsetattr(dev_fd, TCSANOW, &termset);

  return 0;
}


int com_write(char *data)
{
  int err;
  int i;

   
  for(i=0;i < strlen(data);i++)
  {
	// err = com_put(data[i]);
	err=write(dev_fd, (void *)(&data[i]),1);
	 switch(err)
	   {
	   case -1: //error
	     #if VERBOSE
	     printf("Put error...\n");
	     exit(-1)
	     #endif
	     return(-1);
	     break;
	   case 0: // timeout
	     #if VERBOSE
	     printf("Timeout?\n");
	     exit(-1);
	     #endif
	     return(-1);
	     break;
	   }
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
  int err;

  err=write(dev_fd, (void *)(&data),1);// strlen(data));
  switch(err)
    {
    case -1: //error
      #if VERBOSE
      printf("Put error...\n");
      exit(-1);
      #endif
      return(-1);
      break;
    case 0: // timeout
      #if VERBOSE
      printf("Timeout?\n");
      exit(-1);
      #endif
      return(-1);
      break;
    }

  return 0;
}


int com_get()
{
  int err;
  char	data;

  err = read(dev_fd, &data, 1);
  switch(err)
    {
    case -1: //error
      #if VERBOSE
      printf("Get error...\n");
      exit(-1);
      #endif
      return(-1);
      break;
    case 0: // timeout
      #if VERBOSE
      printf("Timeout...\n");
      exit(-1);
      #endif
      return(-1);
      break;
    }

  return (int) data;
}


int com_exit()
{
  close(dev_fd);
  return 0;
}
