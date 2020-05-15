#include <msp430.h> 
#include "stdio.h"
#include "system_define.h"
#include "system_variable.h"
#include "function_prototype.h"
#include "main.h"

void main(void) {
	byte i, a = '0';
	WDTCTL = WDTPW | WDTHOLD;
	Init_System_Clock();
	Init_System();
	UART_init(3, 7, 1, 0, 0);

	for (i = 0; i < 10; i++)
		UART_sendbyte(i + a);
	UART_off();
	while(1);
}
