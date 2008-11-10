#ifndef PROGRAMMER_H_
#define PROGRAMMER_H_

int idChip(void);
int writeChip(int, unsigned char *);
void readChip(int, unsigned char *);
int writeHex(char *, int);

#endif /*PROGRAMMER_H_*/
