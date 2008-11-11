/* 
 * LamaProg v 0.1.1
 * Function for manipulation with files in intel hex files v 0.1.0
 * Jan Parkman   (parkmaj@users.sourceforge.net) 
 */

#pragma warning(disable: 4996)

#include "hexfile.h"
#include <stdio.h>
#include <string.h>


/*
 * Remove white spaces from string 
 */
void clearstr(char s[STR_LEN])
{
	char hs[STR_LEN];
	unsigned int i,j;


	j = 0;
	
	for(i=0; i <= strlen(s) ;i++)
	{
		if ( !((s[i] == ' ') || (s[i] == '\n') || (s[i] == 0x0D) || (s[i] == 0x0A)))
		 {
			 hs[j++]=s[i];
		//	 printf("c = %c, i = %d, j = %d\n",s[i],i,j);
		 }
	}
	strcpy(s,hs);	
}


/*
 * Return char calculated from hex dec(16*h + l)
 */
unsigned char hextochar(char h, char l)
{
	char s[2];
	int out;
	 
	s[0]=h;
	s[1]=l;
	sscanf(s,"%2X",&out);
	

	return out;
		
} 

/*
 * Load hexfile to array
 * return -1 on error or return number of read instruction
 */ 
int loadhex(char *filename, unsigned int size ,unsigned char *image, unsigned int *max_addr)
{
	/*implemented only data and end of file recordtype*/
	
	char s[STR_LEN];
	FILE *f;
	unsigned int addr,rec_type,crc;
	unsigned int length;	//record length
	unsigned int i;
	unsigned int index;
	unsigned int crc_temp;	//for calculating crc
//	long int max_addr;	

	*max_addr = 0;
	
	if ((f = fopen(filename, "r")) == NULL)
	{
		//printf("File '%s' not found...\n",filename);
		return(-1);
	};

	while (fgets(s, STR_LEN, f) != NULL)
	{
		//printf("--%s--\n",s);
		//remove white spaces
		clearstr(s);
		crc = hextochar(s[strlen(s)-2],s[strlen(s)-1]);

		//check crc
		crc_temp = crc;
		for(i=1; i < (strlen(s)-2); i=i+2)
		{
			crc_temp += hextochar(s[i],s[i+1]);
		}
		if ( (crc_temp % 256) != 0)
		{	
			printf("Bad CRC at line: '%s'\n", s);
			return -2;	
		} 
		
		sscanf(s, ":%02X%04X%02X", &length, &addr, &rec_type);

		//end file
		if (rec_type == 1)
		{
//			printf("\nFile end\n");
			break;
		}	

		//data procesing 
		//data are placed from column 9 to rew length - 2 
	        for(i=9; i<9+length*2; i=i+2)
		{
			 index = addr + (i-9)/2;
			 if (index >= size) return -2;
			 image[index]  = hextochar(s[i],s[i+1]);
			 if (index > *max_addr) {*max_addr = index;}	
		}
		
	}	

	return 0;
}


/*
 * Save hex to hexfile (filename)
 * return 0 if ok, -1 if nook :)
 */
int savehex(char *filename, unsigned char *image, unsigned int max_addr)
{
	unsigned int	j;
	unsigned int	i;
	unsigned int 	crc;
	FILE		*f;
	
	if ((f=fopen(filename,"w"))==NULL) return -1;

	for(i=0;i<(max_addr/16);i++) 
	{
		crc = 0x10 + ((i*16)%256) + ((i*16)/256);
		fprintf(f,":10%04X00", i*16);

		for(j=0;j<16;j++)
		{
			crc += image[16*i + j]; 
			fprintf(f,"%02X", image[16*i + j]);
		}
		
		fprintf(f,"%02X\n", (256 - (crc % 256)) % 256);
		
	}
	fprintf(f,":00000001FF\n");
	fclose(f);
	return 0;
}


/*
 * Print hexfile to console...
 */
void printhex(unsigned char *image, unsigned int max_addr)
{
	unsigned int	i;
	for(i=0;i<=max_addr;i++) 
	{	
		if ( (i % 16) == 0)
		{
			printf("\n%04X: ",i);
		}
		printf(" %02X ", image[i]);
	}
}

