/* 
 * LamaProg v 0.1
 * Function for manipulation with files in intel hex files 
 * Jan Parkman   (parkmaj@users.sourceforge.net) 
 */

#define STR_LEN 255	//max strig length	

/*
 * Load hexfile to array
 * return -1 if input file not found 
 * 	  -2 if input file is corrupt
 *         0 if ok	
 */ 
int loadhex(char *filename,unsigned int size, unsigned char *image, unsigned int *max_addr);

/*
 * Save hex to hexfile
 */
int savehex(char *filename, unsigned char *image, unsigned int max_addr);

/*
 * Print hexfile to console...
 */
void printhex(unsigned char *image, unsigned int max_addr);
