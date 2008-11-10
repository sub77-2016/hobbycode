#include <stdio.h>

#include "hexfiles.h"
#include "isp.h"

int idChip(void) {
	unsigned char sig[3];
	
	readSig(sig);
	//printf("Sig: 0x%.2X 0x%.2X 0x%.2X.\n", sig[0], sig[1], sig[2]);
	return (sig[1] == 0x52) ? 8192 : 4096;
}

int writeChip(int totMem, unsigned char *buffer) {
	int paginas = totMem / 256, pag, totP=0;
	
	for(pag=0; pag<paginas; pag++)
		totP += writePage(pag, &buffer[pag * 256]);
		
	return totP;
}

void readChip(int totMem, unsigned char *buffer) {
	int paginas = totMem / 256, pag;
	
	for(pag=0; pag<paginas; pag++)
		readPage(pag, &buffer[pag * 256]);
}

int writeHex(char *nomeArq, int totMem) {
	int cnt, totP;
	unsigned char dados[totMem];
	unsigned char check[totMem];
	
	if(readHex(nomeArq, dados, totMem) < 0)
		return 10;
		
	chipErase();
	totP = writeChip(totMem, dados);
	
	readChip(totMem, check);
	for(cnt=0; cnt < totMem; cnt++)
		if(dados[cnt] != check[cnt]) {
			printf("Erro! Endereço %d leu 0x%.2X, mas era para ser 0x%.2X\n", cnt, check[cnt], dados[cnt]);
			return 0;
		}
	
	return totP;
}
