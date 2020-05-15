#include <msp430.h> 
#include "stdio.h"
#include "system_define.h"
#include "system_variable.h"
#include "function_prototype.h"
#include "main.h"


int wordToStr(char *str, unsigned num){
    int i = 0, j;
    do{
        str[i++] = '0' + num%10;
        num = num/10;
    } while (num > 0);
    str[i] = '\0';
    char t;
    for(j = 0; j < i/2; j++){
        t = str[j];
        str[j] = str[i-1-j];
        str[i-1-j] = t;
    }
    return i;
}

void main(void) {
    WDTCTL = WDTPW|WDTHOLD;
    Init_System_Clock();
    Init_System();
    _enable_interrupt();
    LCD_init();
    LCD_set_cursor(0);
    char message[32];// = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    const char om_str[] = "Îì";
    int pos;
    word value, last_value = 0;
    while(1){
        value = R22_get_resistance();
        //value = R22_get_resistance_TB();
        if (value != last_value){
            pos = wordToStr(message, value);
            LCD_clear();
            LCD_set_pos(0,0);
            LCD_message(message);
            LCD_set_pos(0,pos + 1);
            LCD_message(om_str);
            last_value = value;
        }
        wait_1ms(100);
    }
}
