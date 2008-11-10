#ifndef ISP_H_
#define ISP_H_

#define MOSI BIT5   // LPT out
#define MISO BIT6   // LPT in
#define SCK  BIT4   // LPT out

unsigned char checkISP();
void outISP(int);
unsigned char inISP();
unsigned char progEnable();
void chipErase();
unsigned char readMem(int);
void writeMem(int, unsigned char);
void writeLock(int);
unsigned char readLock();
void readSig(unsigned char *);
void readPage(int, unsigned char *);
int writePage(int, unsigned char *);

#endif /*ISP_H_*/
