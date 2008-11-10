#include <sys/io.h>

#define LPT1 0x378 /* Endereço base da porta paralela*/

unsigned char initLPT()
{
	char status;
	
	// Libera acesso para os endereços: 0x378, 0x379, 0x37A
    if( ioperm(LPT1, 3, 1) )
		return 0;
	
	// Desliga todos os pinos do Registro de Dados
    outb(0,   LPT1);
    
    // Habilita a porta LPT bi-direcional
    status = inb(LPT1+2) & 32;
    outb(status, LPT1+2);
    return 1;
}


unsigned char in(char bit) {
	return (char)((inb(LPT1+1) & bit) != 0) ? 1 : 0;
}

void out(char byte) {
	outb(byte, LPT1);
}
