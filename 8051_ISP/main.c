#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/io.h>

#define TOT_MEM 8192

#include "programmer.h"
#include "udelay.h"
#include "lpt.h"
#include "isp.h"

int init() {
	if(!initLPT()) {
        printf(" Não foi possível obter os endereços: 0x378, 0x379, 0x37A.\n\n");
        exit(1);
    }
	if(!checkISP()) {
        printf(" Não foi possível detectar o cabo ISP. Verifique as conexões e tente novamente.\n\n");
        exit(2);
    }
    if(!progEnable()) {
        printf(" Não foi possível entrar em modo de programação serial no 8051.\n\n");
        exit(4);
    }
	return 1;
}

int main(int argc, char **argv)
{
	int aux, totP;
	
	if(argc < 2) {
		printf("8051 Programmer - G Soft Desenvolvimento\n\n");
		printf("USO: prog51 -opção  [arquivo]  - Opções:\n");
		printf("		-i -> Identifica o chip.\n");
		printf("		-g -> Grava arquivo em formato Intel HEX.\n");
		printf("		-w -> Grava arquivo em formato RAW.\n");
		printf("		-r -> Lê o conteúdo do Chip para arquivo.\n");
		return 1;
	}
	
	if(argc < 3 && (!strcmp(argv[1], "-g") || !strcmp(argv[1], "-w") || !strcmp(argv[1], "-r"))) {
		printf("8051 Programmer - G Soft Desenvolvimento\n\n");
		printf("USO: prog51 -opção  [arquivo]  - Opções:\n");
		printf("		As opções -g, -w e -r precisam do nome do arquivo no segundo parametro.\n");
		return 2;		
	}
	
	init();
	
	if(!strcmp(argv[1], "-i")) {
		aux = idChip();
		if(aux == 4096) printf("Chip: AT89S51\n");
		else            printf("Chip: AT89S52\n");
		return 0;
	}
	if(!strcmp(argv[1], "-g")) {
		aux = idChip();
		printf("Gravando arquivo %s no chip AT89S5%s:\n", argv[2], ((aux==4096)?"1":"2"));
		totP = writeHex(argv[2], aux);
		if(totP > 0) {
			if(totP == 1) printf("OK! Uma página (256 bytes) gravada.\n");
			else          printf("OK! %d páginas (256 bytes cada) gravadas.\n", totP);
		}
		return 0;
	}
	if(!strcmp(argv[1], "-w")) {
		printf("Ainda não!\n");
		return 0;
	}
	if(!strcmp(argv[1], "-r")) {
		printf("Ainda não!\n");
		return 0;
	}
	printf("Opção não reconhecida!\n");			
	return 0;
}
