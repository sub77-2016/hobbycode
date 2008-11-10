// .HEX (OBJ) file functions.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readNum(FILE *fp, int tam) {
	char num[5];
	fread(num,tam,1,fp); num[tam] = 0;
	return strtol(num, NULL, 16);
}

int readHex(char *file, unsigned char *buffer, int tam)
{
  FILE *fp;
  unsigned char ch, done=0;
  int byteCount, addr, recType, byte, checkSum, cnt, lin=0, ret=0;
  
  if(buffer == NULL) {
    printf("Buffer de dados inválido.\n");
    return -3;
  }
  
  memset(buffer, 0xFF, tam);

  fp = fopen(file, "rb");
  if(!fp) {
    printf("Impossivel abrir %s.\n", file);
    return -1;
  }
  
  while(!done) {
  	lin++;
    ch = fgetc(fp);
    if(ch != ':') {
    	ch = fgetc(fp); // Hack para entender arquivos com quebra de linha padrão M$Win
    	if(ch != ':') {
    		printf("Arquivo %s com estrutura inválida! Erro linha %d. [%c]\n", file, lin, ch);
    		return -2;
    	}
    }
    byteCount = readNum(fp, 2);		 //printf("Byte Count: %d\n", byteCount);

    addr =    readNum(fp, 4);			 //printf("Start Address: %d\n", addr);
    recType = readNum(fp, 2);			 //printf("Record Type: %d\n", recType);
    if(recType == 1) done=1;

    for(cnt=0; cnt<byteCount; cnt++) {
        byte = readNum(fp, 2);			 //printf(" %d", byte);
        buffer[addr++] = byte;
        ret++;
    }
    checkSum = readNum(fp, 2);		 //printf("\nCheck Sum: %d\n\n", checkSum);
    fgetc(fp);						 // EOL
  }
  fclose(fp);
  
  return ret;
}
