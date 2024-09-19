#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

extern char FAN;
extern char duty_cycle;

#define OFF 0x00
#define RED 0x01
#define GREEN 0x02 
#define YELLOW 0x03
#define BLUE 0x04
#define PURPLE 0x05
#define CYAN 0x06
#define WHITE 0x07

char D2[7] = {RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE};

float get_RPM()
{
    TMR3L = 0;
    T3CONbits.TMR3ON = 1;
    Wait_One_Second();
    T3CONbits.TMR3ON = 0;
    float RPS = TMR3L;
    return (RPS * 60.00);
}

void Toggle_Fan()
{
    if  (FAN == 1) Turn_Off_Fan();
    else Turn_On_Fan();
}

void Turn_Off_Fan()
{
    FAN = 0;
    FAN_EN = 0;
    FAN_EN_LED = 0;
}

void Turn_On_Fan()
{
    FAN = 1;
    do_update_pwm(duty_cycle);
    FAN_EN = 1;
    FAN_EN_LED = 1;
}

void Do_Beep()
{
    Activate_Buzzer();
    Wait_Half_Second();
    Deactivate_Buzzer();
    Wait_Half_Second();
}

void Increase_Speed()
{
    if (duty_cycle >= 100)
    {
        Do_Beep();
        Do_Beep();
    }
    else 
    {
        duty_cycle += 5;
    }
    do_update_pwm(duty_cycle);
}

void Decrease_Speed()
{
    if (duty_cycle <= 0)
    {
        Do_Beep();
        Do_Beep();
    }
    else
    {
        duty_cycle -= 5;       
    }
    do_update_pwm(duty_cycle);
 
}

void do_update_pwm(char duty_cycle) 
{ 
    float dc_f;
    int dc_I;
    PR2 = 0b00000100;
    T2CON = 0b00000111;
    dc_f = ( 4.0 * duty_cycle / 20.0);
    dc_I = (int) dc_f;
    if (dc_I > duty_cycle) dc_I++;
    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
    CCPR1L = (dc_I) >> 2; 
}

//void Set_DC_RGB(int duty_cycle)
//{
//    int num = duty_cycle / 10;
//    if (num > 7) num = 7;
//    num = num << 3;
//    PORTD = PORTD & 0x07;
//    PORTD = PORTD | num;
//}

//void Set_DC_RGB(int duty_cycle)
//{
//    if (duty_cycle >= 70)
//        PORTD = ((PORTD & 0xF0) | 0x07) & 0xFF;
//    else
//        PORTD = ((PORTD & 0xF0) | duty_cycle/10) & 0xFF;
//}

void Set_DC_RGB(int duty_cycle)
{
    char color = (duty_cycle >= 70) ? 7 : duty_cycle / 10;
    PORTD = (PORTD & 0xF1) | (color << 1);
}

//void Set_RPM_RGB(int rpm)
//{
//    int num;
//    if (rpm == 0) num = 0;
//    else num = (rpm / 500) + 1;
//    if (num > 7) num = 7;
//    PORTE = num;
//}

void Set_RPM_RGB(int rpm)
{
    if (rpm < 1)
        PORTE = 0x00;
    else if (rpm >= 3000)
        PORTE = D2[6];
    else
        PORTE = D2[rpm / 500];
}