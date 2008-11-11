/* 
 * LamaProg v 0.2.0
 * opensource "universal" elevtronic chips programmer
 * Jan Parkman   (parkmaj@users.sourceforge.net)
 */


/* TODO -- Dynamic allocation strigns (port, file...)
 *   
 */

//if undef -- switch of localization
//Czech: Chcete-li cestinu odpoznamkujte nasledujici definici...
// #define HAVE_LOCALIZATION 1

#pragma warning(disable: 4996)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include "hexfile.h"
#include "com.h"

//localization
#ifdef HAVE_LOCALIZATION
	#include <locale.h>
	#define _(String) gettext (String)
	#define GETTEXT_DATA_ROOT "/usr/share/locale"
#else
#define _(String) (String)

#endif

//devices
#define	d24c01	1
#define	d24c02	2
#define	d24c04	3
#define	d24c08	4
#define	d24c16	5
#define	d24c32	6
#define	d24c64	7
#define	d24c128 8
#define	d24c256 9
#define	d24c512 10

#define	d89c1051 30
#define d89c2051 31
#define d89c4051 32

//actions
#define A_READ	1
#define A_WRITE 2 
#define A_VER	3
#define A_SIG	4

#define STR_LENGTH 1024

int	 device = d89c2051;	                    // Programmed device
char port[STR_LENGTH] = "COM1";	            // Communication port
char inputfile[STR_LENGTH] = "file.hex";	  // Input file
char outputfile[STR_LENGTH] = "o_file.hex"; // Output file
int	 action = 0;	                          // Action (0 -- no action, 1 -- write, 2 -- read) 


// Print help 
 void	help(void)
{
	printf(_("LamaProgWin v0.1\n\n"
		"Usage prog -[rw] file -d device\n\n"
		"Options:\n"
		"        -h: this help\n"
		"        -v: print programmer version\n"
		"        -r: read intel hex file from device\n"
		"        -w: programm device\n"
		"        -d: select programmed device (default 89c2051)\n"
		"        -p: communication port (default COM1)\n"
		"Devices:\n"
		"	24c01, 24c02, 24c04, 24c08, 24c16, 24c32, 24c64, 24c128, 24c256, 24c512\n"
		"	89c1051, 89c2051, 89c4051\n\n" 
	));
}


int select_device(char *dev)
{
	if (strstr(dev,"24c01")!=NULL) return d24c01;
	if (strstr(dev,"24c02")!=NULL) return d24c02;
	if (strstr(dev,"24c04")!=NULL) return d24c04;
	if (strstr(dev,"24c08")!=NULL) return d24c08;
	if (strstr(dev,"24c16")!=NULL) return d24c16;
	if (strstr(dev,"24c32")!=NULL) return d24c32;
	if (strstr(dev,"24c64")!=NULL) return d24c64;
	if (strstr(dev,"24c128")!=NULL) return d24c128;
	if (strstr(dev,"24c256")!=NULL) return d24c256;
	if (strstr(dev,"24c512")!=NULL) return d24c512;

 	if (strstr(dev,"89c1051")!=NULL) return d89c1051;
	if (strstr(dev,"89c2051")!=NULL) return d89c2051;
	if (strstr(dev,"8924051")!=NULL) return d89c4051;

	return -1;
}


// Read i2c eeproms
 int	i2c_ee_read(unsigned int size, int page_size)
{
	unsigned char *image;  // Content of processor memory
	char          mess[1024];	
	unsigned int	i;
	unsigned int  iChar;   // Hexadecimal character
	
	//programmer init
	com_write(":v ");
	com_read(mess);
	printf(_("Version of programmer: %s\n"),mess);
	if (size > 2048)
		com_write(":djl ");
	else
		com_write(":djs ");
	if(com_read(mess) != 1)
	{
		printf(_("Error...\n"));
		exit(-1);
	}
	com_write(":v ");
	com_read(mess);
	printf("                       %s\n",mess);

	image=malloc(sizeof(unsigned char) * size);
	//reading

	for(i=0;i<size;i+=page_size)
	{
		int	j;
		
		sprintf(mess,":r%02X%04X ",page_size,i);
		com_write(mess);
		if (com_read(mess)!=1)
		{
			printf(_("Reading error"));
			free(image);
			exit(-1);
		}

	//	printf("%04X-%s\n",i,mess);

		for(j=0;j<page_size;j++)
		{
			sscanf((mess+2*j), "%02X", &iChar);
			image[i+j] = LOBYTE(iChar);
		}

		printf(".");
		fflush(stdout);
	}
	printf("\n");
	
	if (savehex(outputfile, image, size)==-1)
	{
		printf(_("Saving failed...\n"));
		exit(-1);
	}

	//printhex(image, size);
	//end
	printf(_("\nReading complete at adress %d\n"),i);//???
//	printhex(image,size);
	free(image);
	return 0;
}


/*
 * Read atmel 8051
 */
int  mcu51_read(unsigned int size, int page_size)
{
	unsigned char *image;  // Content of processor memory
	char	        mess[1024];	
	unsigned int	i;
	unsigned int  iChar;   // Hexadecimal character
	
	//programmer init
	com_write(":v ");
	com_read(mess);
	printf(_("Version of programmer: %s\n"),mess);
	com_write(":da ");
	if(com_read(mess) != 1)
	{
		printf(_("Error...\n"));
		exit(-1);
	}
	com_write(":v ");
	com_read(mess);
	printf("                       %s\n",mess);

	com_write(":s ");
	com_read(mess);
	printf("Chip signature: %s\n",mess);

	com_write(":a0000 ");	//set address counter to 0	
	if (com_read(mess) != 1)
	{
		printf(_("Reading error"));
		exit(-1);
	}
	
	image=malloc(sizeof(unsigned char) * size);
	//reading

	for(i=0;i<size;i+=page_size) //???
	{
		int	j;
		
		sprintf(mess,":r%02X ",page_size);
		com_write(mess);
		if (com_read(mess)!=1)
		{
			printf(_("Reading error"));
			free(image);
			exit(-1);
		}

	//	printf("%04X-%s\n",i,mess);

		for(j=0;j<page_size;j++)
		{
			sscanf((mess+2*j), "%02X", &iChar);
			image[i+j] = LOBYTE(iChar);
		}

		printf(".");
		fflush(stdout);
	}
	printf("\n");
	
	if (savehex(outputfile, image, size)==-1)
	{
		printf(_("Saving failed...\n"));
		exit(-1);
	}

	//printhex(image, size);
	//end
	printf(_("\nReading complete at adress %d\n"),i); //???
//	printhex(image,size);
	free(image);
	return 0;
}


/* 
 * Read program from device and save it to output file
 */
int	prog_read()
{
	char mess[1024];
	
	//programmer init
	com_init(port);
	com_write(":q ");
	com_read(mess);
	com_write(":q ");
	com_read(mess);

	//select device
	switch (device)
	{
	case d24c01:
		i2c_ee_read(128,128);
		break;
	case d24c02:
		i2c_ee_read(256,128);
		break;
	case d24c04:
		i2c_ee_read(512,128);
		break;
	case d24c08:
		i2c_ee_read(1024,128);
		break;
	case d24c16:
		i2c_ee_read(2048,128);
		break;
	case d24c32:
		i2c_ee_read(4096,128); 
		break;
	case d24c64:
		i2c_ee_read(8192,128);
		break;
	case d24c128:
		i2c_ee_read(16384,128);
		break;
	case d24c256:
		i2c_ee_read(32768,128);
		break;
	case d24c512:
		i2c_ee_read(65536,128);
		break;

	case d89c1051:
		mcu51_read(1024,128);
		break;
	case d89c2051:
		mcu51_read(2048,128);
		break;
	case d89c4051:
		mcu51_read(4096,128);
		break;

	default:
		printf(_("Unknown device... \n"));
		break;
	}
	
	com_exit();
	return 0;
}


/*
 * 	Programm i2c eeproms
 */
int	i2c_ee_write(int pagelen, unsigned int size)
{
	unsigned char *image;	//content of procesor memmory
	unsigned int	max_addr;
	char	mess[1024];	
	unsigned int	i;
	int	err;
	
	//load hex file
	image = malloc(sizeof(unsigned char) * size);
	if((err = loadhex(inputfile, size, image, &max_addr)) < 0)
	{	//loading failed 
		if ( err == -1)
			printf(_("Input file not found...\n"));
		else
			printf(_("Input file is corupt...\n"));
		free(image);
		exit(-1);
	}
	printf(_("Hex file loaded...\n"));

	//programmer init
	com_write(":v ");
	com_read(mess);
	printf(_("Version of programmer: %s\n"),mess);
	if (size > 2048)
		com_write(":djl ");
	else
		com_write(":djs ");
	if(com_read(mess) != 1)
	{
		printf(_("Error...\n"));
		exit(-1);
	}
	com_write(":v ");
	com_read(mess);
	printf("                       %s\n",mess);
	
	//programming
	for(i=0;i<max_addr;i+=pagelen)
	{
		int	j;
		char	s[8];

		sprintf(mess,":w%02X%04X",pagelen,i);
		for(j=0;j<pagelen;j++){
			sprintf(s,"%02X",image[i+j]);
			strcat(mess,s);
		}
		strcat(mess," ");
		//printf("%s\n",mess);
		com_write(mess);
		if(com_read(mess) != 1)
		{
			printf(_("Write error"));
			free(image); 
			return -1;
		}
		printf(".");
		fflush(stdout);
	}

	//end
	printf(_("\nProgramming complete...\n"));
	free(image);
	return 0;
}

/*
 * 	Programm atmel 8051
 */
int	mcu51_write(int pagelen, unsigned int size)
{
	unsigned char *image;	//content of procesor memmory
	unsigned int	max_addr;
	char	mess[1024];	
	unsigned int	i;
	int	err;
	
	//load hex file
	image = malloc(sizeof(unsigned char) * size);
	if((err = loadhex(inputfile, size, image, &max_addr)) < 0)
	{	//loading failed 
		if ( err == -1)
			printf(_("Input file not found...\n"));
		else
			printf(_("Input file is corupt...\n"));
		free(image);
		exit(-1);
	}
	printf(_("Hex file loaded...\n"));

	//programmer init
	com_write(":v ");
	com_read(mess);
	printf(_("Version of programmer: %s\n"),mess);

	com_write(":da ");
	if(com_read(mess) != 1)
	{
		printf(_("Error...\n"));
		exit(-1);
	}
	com_write(":v ");
	com_read(mess);
	printf("                       %s\n",mess);
	
	com_write(":s ");
	com_read(mess);
	printf("Chip signature: %s\n",mess);

	
	com_write(":e ");
	com_read(mess);
	printf(_("Ereasing complete...\n"));
	
	com_write(":a0000 "); //set "program counter"
	com_read(mess);
	
	//programming
	for(i=0;i<max_addr;i+=pagelen)
	{
		int	j;
		char	s[8];

		sprintf(mess,":w%02X",pagelen);
		for(j=0;j<pagelen;j++){
			sprintf(s,"%02X",image[i+j]);
			strcat(mess,s);
		}
		strcat(mess," ");
		//printf("%s\n",mess);
		com_write(mess);
		com_read(mess);
		if (mess[0] != '+') 
		{
			printf(_("Write error"));
			free(image); 
			return -1;
		}
		printf(".");
		fflush(stdout);
	}

	//end
	printf(_("\nProgramming complete...\n"));
	free(image);
	return 0;
}

/*
 * Write program to device (from inputfile)
 */
int	prog_write()
{
	char mess[1024];
	
	//programmer init
	com_init(port);
	com_write(":q ");
	com_read(mess);
	com_write(":q ");
	com_read(mess);

	//select device
	switch (device)
	{
	case d24c01:
		i2c_ee_write(8,128);
		break;
	case d24c02:
		i2c_ee_write(8,256);
		break;
	case d24c04:
		i2c_ee_write(16,512);	//TODO check page sizes...
		break;
	case d24c08:
		i2c_ee_write(16,1024);
		break;
	case d24c16:
		i2c_ee_write(16,2048);
		break;
	case d24c32 :
		i2c_ee_write(16,4096);
		break;
	case d24c64 :
		i2c_ee_write(16,8192);
		break;
	case d24c128 :
		i2c_ee_write(16,16384);
		break;
	case d24c256 :
		i2c_ee_write(32,32768);
		break;
	case d24c512 :
		i2c_ee_write(32,65536);
		break;

	case d89c1051 :
		mcu51_write(32,1024);
		break;
	case d89c2051 :
		mcu51_write(32,2048);
		break;
	case d89c4051 :
		mcu51_write(32,4096);
		break;
	
	default:
		printf(_("Unknown device... \n"));
		break;
}	
	com_exit();
	return 0;
}


int	prog_ver()
{
	char mess[1024];
	
	// Programmer init
	com_init(port);
	com_write(":q ");
	com_read(mess);
	com_write(":q ");
	com_read(mess);
	com_write(":v ");
	com_read(mess);
	printf("Programmer version:\n"
	       "		    software: LamaProg v 0.1.1\n"
	       "        hardware: %s\n\n", mess);
	com_exit();
	return 0;
}

// Parse option and its (optional) argument from command line arguments (argc, argv), starting with argument iArgNo
// Option and its argument are returned in pOpt and szOptArg
// Position of next option to parse is returned as return value
int getopt(int argc, char *argv[], int iArgNo, char *pOpt, char *szOptArg)
{
	char *pCurrArg;     // Pointer to current argument
	BOOL bOptAssigned;  // Option assigned

	*pOpt     = 0;      // Empty option
	*szOptArg = 0;      // Empty argument string

	if (iArgNo>=argc) return iArgNo;  // All arguments already processed
	
	bOptAssigned = FALSE;
	for (; iArgNo<argc; iArgNo++)
	{
		pCurrArg = argv[iArgNo];
		if (strlen(pCurrArg)!=2) continue;  // Option selector must have length 2
		if (*pCurrArg!='-') continue;       // Option selector is "-"

		*pOpt = tolower(pCurrArg[1]);

		// Now try to get option parameter
		iArgNo++;
		if (iArgNo>=argc) break;   // No further arguments -> break
		pCurrArg = argv[iArgNo];
		if (*pCurrArg!='-')        // Use as option argument only if it is not next option (starting with "-")
		{
			strcpy(szOptArg, pCurrArg);
			iArgNo++;
		}
		break;
	}

	return iArgNo;
}


int main(int argc, char *argv[])
{
	char opt;            // Option
	char optarg[1024];   // Option argument
	int  iCurrPos;       // Current command line argument index

	if (argc==1) help(); 

	// Processing input
	iCurrPos = 1;
	do
	{
		iCurrPos=getopt(argc, argv, iCurrPos, &opt, optarg);

		switch (opt)
		{
			case 'h': 
				help();
				action = 0;
				break;

			case 'd':
				if (strlen(optarg)==0) 
				{
					printf(_("Option -%c needs a value\n"), opt);
					break;
				}
				if ((device = select_device(optarg))==-1)
				{
					printf(_("Unknown device %s\n"), optarg);
					return -1;
				}
				else 
				{
					printf(_("Selected device: %s\n"), optarg);
				}
				break;

			case 'w':
				if (strlen(optarg)==0) 
				{
					printf(_("Option -%c needs a value\n"), opt);
					break;
				}
				printf(_("Programm device from %s\n"),optarg);
				strncpy(inputfile,optarg,STR_LENGTH);
				action = A_WRITE;
				break;

			case 'r':
				if (strlen(optarg)==0) 
				{
					printf(_("Option -%c needs a value\n"), opt);
					break;
				}
				printf(_("Read content of device to file %s\n"),optarg);
				strncpy(outputfile, optarg, STR_LENGTH);
				action = A_READ;
				break;

			case 'p':
				if (strlen(optarg)==0) 
				{
					printf(_("Option -%c needs a value\n"), opt);
					break;
				}
				printf(_("Communication port %s\n"),optarg);
				strncpy(port, optarg, STR_LENGTH);
				break;

			case 'v':
				action = A_VER;
				break;

			case '\x00':  // Last empty option
				break;

			default:
				printf(_("Unknow option: %c\n"), opt);	
				break;
		}
	 } while ( iCurrPos<argc );
	
	switch (action)
	{
		case 0: 
			break;
		case A_WRITE:
			prog_write();
			break;
		case A_READ:
			prog_read();
			break;	
		case A_VER:
			prog_ver();
			break;
		default:
			break;
	}

	return 0;
}
