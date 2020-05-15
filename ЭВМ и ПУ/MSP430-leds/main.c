#include <msp430.h> 
#include "system_define.h"
#include "system_variable.h"
#include "function_prototype.h"
#include "main.h"

/*
Разработать программу, фиксирующую нажатия клавиш 1, 2 и 3 матричной клавиатуры
включением светодиодов 4, 5 и 6 соответственно. Выход из цикла опроса осуществляется
 при одновременном (по очереди, по большому счёту) нажатии клавиш * и #. Частота тактовых импульсов на линии SCL – 40 кГц.
 */
//#pragma vector - размещение функции по адресу
// INT1 - P1.7
char last_key = 0;
char key_pressed = 0;
byte end_flag = 0;
char KEYS_i2c_addr = 0x73;

#pragma vector = PORT1_VECTOR
__interrupt void Key_int(){
    key_pressed = 1;
    P1IFG &= ~BIT7; // сброс флага прерывания
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    Init_System();
    Init_System_Clock();

    _enable_interrupt(); // глобальное разрешение прерываний
    Init_I2C();
    I2C_WriteByte(0x03, 0x0F, KEYS_i2c_addr); //направление ножек (0-вывод)
    I2C_WriteByte(0x01, 0x00, KEYS_i2c_addr); //0 в столбцы клавиатуры для обнаружения прерывания

    P1IFG &= ~BIT7; // сброс флага прерывания
    P1IE |= BIT7;
    P1IES |= BIT7; // задний фронт

    while(end_flag == 0)
        if (key_pressed == 1){
            char k;
            k = KEYS_scannow();
            switch (k){
                case '1': LED_change(1); break;
                case '2': LED_change(2); break;
                case '3': LED_change(3); break;
                case '*':
                    if (last_key == '#') end_flag = 1; break;
                case '#':
                    if (last_key == '*') end_flag = 1; break;
            }
            last_key = k;
            I2C_WriteByte(0x01, 0x00, KEYS_i2c_addr);//0 в столбцы клавиатуры для обнаружения прерывания
            key_pressed = 0;
        }
}
