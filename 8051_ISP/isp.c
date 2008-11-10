#include <stdio.h>

#include "hexfiles.h"
#include "udelay.h"
#include "lpt.h"
#include "isp.h"

#define DELAY_ISP 2

/**
 * Verifica se o pino 1 da LPT esta conectado ao pino 12
 */
unsigned char checkISP() {
	out(0);
	if(in(BIT5) != 0) return 0;
	out(BIT0);
	if(in(BIT5) != 1) return 0;
	out(0);
	if(in(BIT5) != 0) return 0;
	
	return 1;
}

void outISP(int byte) {
	char cnt, outBit;
	for(cnt=7; cnt>=0; cnt--) {
		outBit = (byte & (1 << cnt)) ? MOSI : 0;
		out(outBit);
		udelay(DELAY_ISP);
		out(outBit + SCK);
		udelay(DELAY_ISP);
	}
	out(0);
}

unsigned char inISP() {
	char cnt, ret=0;
	for(cnt=7; cnt>=0; cnt--) {
		out(SCK);
		udelay(DELAY_ISP);
		if(in(MISO))
			ret += 1 << cnt; 
		out(0);
		udelay(DELAY_ISP);
	}
	return ret;
}

unsigned char progEnable() {
	unsigned char inByte;
	
	outISP(128+32+8+4); // 1010 1100
	outISP(64+16+2+1);  // 0101 0011
	inISP();            // xxxx xxxx
	
	// tem que sair 0110 1001
	inByte = inISP();
	 
	if(inByte != 64+32+8+1) return 0;
	return 1;
}

void chipErase() {
	outISP(128+32+8+4); // 1010 1100
	outISP(128);        // 100x xxxx
	inISP();            // xxxx xxxx
	inISP();            // xxxx xxxx
	udelay(500000);
}

unsigned char readMem(int addr) {
	// a1 = (addr & 0001 1111 0000 0000) = 
	unsigned char a1 = (addr >> 8) & (16+8+4+2+1);
	unsigned char a2 = (addr & 255);
	outISP(32);         // 0010   0000
	outISP(a1);         // xxxa12 a11 .. a8
	outISP(a2);         // a7 .. a0
	return inISP();     // d7 .. d0 (in)	
}

void writeMem(int addr, unsigned char data) {
	// a1 = (addr & 0001 1111 0000 0000) = 
	unsigned char a1 = (addr >> 8) & (16+8+4+2+1);
	unsigned char a2 = (addr & 255);
	outISP(64);         // 0100   0000
	outISP(a1);         // xxxa12 a11 .. a8
	outISP(a2);         // a7 .. a0
	outISP(data);       // d7 .. d0 (in)
	udelay(500);
}

void writeLock(int lock) {
	// lock = (128+64+32) + 0 .. 3  =>  128 .. 131
	outISP(128+32+8+4); // 1010 1100
	outISP(lock);       // 1110 00 b1 b2 
	inISP();            // xxxx xxxx
	inISP();            // xxxx xxxx
	udelay(500);
}

unsigned char readLock() {
	outISP(32+4);       // 0010 0100
	inISP();            // xxxx xxxx
	inISP();            // xxxx xxxx
	                    // xxx l3  l2 l1 xx
	return inISP() & (16+8+4);	
}

void readSig(unsigned char *sig) {
	int c;
	for(c=0; c<3; c++) {
		outISP(32+8);       // 0010   1000
		outISP(c);          // xxxa12 a11-a8
		outISP(0);          // a7xxx  xxx0
		sig[c] = inISP();
	}
}

void readPage(int paddr, unsigned char *buffer) {
	int cnt;
	outISP(32+16);      // 0011   0000
						// xxxa12 a11 .. a8
	outISP((unsigned char)paddr);         
	for(cnt=0; cnt<256; cnt++)
		buffer[cnt] = inISP();
}

int writePage(int paddr, unsigned char *data) {
	int cnt, grava=0;
	// Verifica se não é uma página em branco
	for(cnt=0; cnt<256; cnt++)
		if(data[cnt] != 0xFF) {
			grava=1;
			break;
		}
	
	if(grava) {
		outISP(64+16);      // 0101   0000
							// xxxa12 a11 .. a8
		outISP((unsigned char)paddr);
		for(cnt=0; cnt<256; cnt++) {
			outISP(data[cnt]);
			udelay(500);
		}
	}
	return grava;	
}
