#include <msp430.h> 
#include "stdio.h"
#include "system_define.h"
#include "system_variable.h"
#include "function_prototype.h"
#include "main.h"


void main(void) {
    WDTCTL = WDTPW|WDTHOLD;
    Init_System_Clock();
    Init_System();
    LCD_init();
	LCD_set_cursor(0);
    const char *out_name = "Клесов МИ";
    const char *out_group = "ВТ-31";
    LCD_set_pos(0,0);
    LCD_message(out_name);
    LCD_set_pos(1,0);
    LCD_message(out_group);

    byte sym_table[8] = {
        0b00000000,
        0b00000000,
        0b00001110,
        0b00010101,
        0b00011111,
        0b00010101,
        0b00001110,
		0b00000000
    };
    char i;
    for (i = 0; i < 8; i++){	// формируем сивол в CGRAM (память знакогенератора)
        LCD_WriteCommand(0x40 + i);		// set CGRAM Address (0x40 + адресс)
        LCD_WriteData(sym_table[i]);
    }
    LCD_set_pos(1,8);
    LCD_WriteData(0);		// записываем в DDRAM (видеопамять) код запрограммированного символа
    while(1);
}
