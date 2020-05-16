#include <msp430.h> 
#include "stdio.h"
#include "system_define.h"
#include "system_variable.h"
#include "function_prototype.h"
#include "main.h"

const float HIH_zero_offset = 0.958;    // �������� "��������� ��������" ������� ���������, �
const float HIH_slope = 0.03068;        // �������� "���� ������� �������", � / %RH
const float HIH_ion = 3.3;              // ������� ����������, �
const float HIH_divisor = 1.1;          // ����������� ������������ ��������

const float INA_RS = 0.21;              // ������������� �������������, ��
const float INA_RL = 30.1;              // ������������� ��������, ��

void ADC_init(){
    P6SEL |= BIT0;          // ����� ��� ����� P6.0 ������� ��� ADC0, � �������� ��������� ������ ���������
    P6SEL |= BIT1;          // ����� ��� ADC1, � �������� ��������� ������ ����

    ADC12CTL1 = SHP + CSTARTADD_0; // ������ ������� � ��������� ����� �������������� - ADC12MEM0
    ADC12CTL1 |= CONSEQ1 + CONSEQ0 + SHS0;   // ����� "������������� ������������������ �������" � ������������� �� ������ 1 ������� �(�������� ��� SHI)
    // ����� �������� ���������� - Vr+ = V�REF+ = 3.3�, Vr- = AVss = 0�
    //    � �������� ������ ADC0 ��� ������ ������ ADC12MEM0
    ADC12MCTL0 = SREF_3 + INCH_0;
    // ����� �������� ���������� - Vr+ = V�REF+ = 3.3�, Vr- = AVss = 0�
    //    � �������� ������ ADC1 ��� ������ ������ ADC12MEM1
    ADC12MCTL1 = SREF_3 + INCH_1 + EOS; // EOD - ����� ������������������

    ADC12IE = BIT1;// + BIT0;  // ��������� ����������
    ADC12CTL0 = ADC12ON;    // ��������� ���
    ADC12CTL0 |= ENC;       // �������������� ���������
}

void Timer_A_init(){
    TACCR0 = 328;   // T = 10 ��
    TACCR1 = 250;
    TACCTL1 = OUTMOD_3 + CCIE; //OUT1 � ����� ����������/����� � ���������� ���������� �� ���������
    TACTL = TASSEL0 + MC0; // 32768 �� � ����� �����
}

float rh, curr;
void main(void) {
    WDTCTL = WDTPW + WDTHOLD;
    __enable_interrupt();
    Init_System_Clock();
    Init_System();
    ADC_init();
    Timer_A_init();
    LCD_init();
    LCD_set_cursor(0);
    char message[32];
    while(1){
        LCD_set_pos(0, 0);
        sprintf(message, "���������: %g", rh);
        LCD_message(message);
        LCD_set_pos(1, 0);
        sprintf(message, "���: %g", curr);
        LCD_message(message);
        wait_1ms(100);
    }
}

/*
#pragma vector=TIMERA1_VECTOR
__interrupt void isrTIMERA(void){
  //if(TAIV == 0x02){
      rh = HIH_get_hum();
      curr = INA_get_curr();
      TACCTL1 &= ~CCIFG;
  //}
}
*/
#pragma vector = ADC12_VECTOR
__interrupt void ADC_interrupt(void){
    ADC12CTL0 &= ~ENC;
    //if (ADC12IFG & BIT0 == 1){
        rh = (((ADC12MEM0/4095.0) * HIH_ion * HIH_divisor) - HIH_zero_offset) / HIH_slope;
    //}
    //if (ADC12IFG & BIT1 == 1){
        curr = (ADC12MEM1*3.3) / (4095.0 * INA_RS * INA_RL);
    //}
    ADC12CTL0 |= ENC;       // �������������� ���������
}
