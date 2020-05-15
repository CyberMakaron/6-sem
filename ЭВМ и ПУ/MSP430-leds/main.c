#include <msp430.h> 
#include "system_define.h"
#include "system_variable.h"
#include "function_prototype.h"
#include "main.h"

/*
����������� ���������, ����������� ������� ������ 1, 2 � 3 ��������� ����������
���������� ����������� 4, 5 � 6 ��������������. ����� �� ����� ������ ��������������
 ��� ������������� (�� �������, �� �������� �����) ������� ������ * � #. ������� �������� ��������� �� ����� SCL � 40 ���.
 */
//#pragma vector - ���������� ������� �� ������
// INT1 - P1.7
char last_key = 0;
char key_pressed = 0;
byte end_flag = 0;
char KEYS_i2c_addr = 0x73;

#pragma vector = PORT1_VECTOR
__interrupt void Key_int(){
    key_pressed = 1;
    P1IFG &= ~BIT7; // ����� ����� ����������
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    Init_System();
    Init_System_Clock();

    _enable_interrupt(); // ���������� ���������� ����������
    Init_I2C();
    I2C_WriteByte(0x03, 0x0F, KEYS_i2c_addr); //����������� ����� (0-�����)
    I2C_WriteByte(0x01, 0x00, KEYS_i2c_addr); //0 � ������� ���������� ��� ����������� ����������

    P1IFG &= ~BIT7; // ����� ����� ����������
    P1IE |= BIT7;
    P1IES |= BIT7; // ������ �����

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
            I2C_WriteByte(0x01, 0x00, KEYS_i2c_addr);//0 � ������� ���������� ��� ����������� ����������
            key_pressed = 0;
        }
}
