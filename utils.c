#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include <p18f4620.h>
#include "utils.h"
#include "Fan_Support.h"

//extern char found;
extern char Nec_code1;
extern short nec_ok;
//extern char array1[21];
extern char duty_cycle;

//char check_for_button_input()
//{       
//    if (nec_ok == 1)
//    {
//            nec_ok = 0;
//
//            printf ("NEC_Code = %x\r\n", Nec_code1);
//
//            INTCONbits.INT0IE = 1;          // Enable external interrupt
//            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge
//
//            found = 0xff;
//            for (int j=0; j< 21; j++)
//            {
//                if (Nec_code1 == array1[j]) 
//                {
//                    found = j;
//                    j = 21;
//                }
//            }
//            
//            if (found == 0xff) 
//            {
//                printf ("Cannot find button \r\n");
//                return (0);
//            }
//            else
//            {
//                return (1);
//            }
//    }
//}

char bcd_2_dec (char bcd)
{
    int dec;
    dec = ((bcd>> 4) * 10) + (bcd & 0x0f);
    return dec;
}

int dec_2_bcd (char dec)
{
    int bcd;
    bcd = ((dec / 10) << 4) + (dec % 10);
    return bcd;
}



//void Do_Beep_Good()
//{
//    Activate_Buzzer_2KHz();
//    Wait_One_Sec();
//    Deactivate_Buzzer();
//    Wait_One_Sec();
//    do_update_pwm(duty_cycle);
//}
//
void Do_Beep_Bad()
{
    Activate_Buzzer_500Hz();
    Wait_One_Second();
    Deactivate_Buzzer();
    Wait_One_Second();
    do_update_pwm(duty_cycle);
}

void Wait_One_Second() 
{                                                                                  // First, turn on the SEC LED 
    Wait_Half_Second();                                                                            // Wait for half second (or 500 msec)                                                                                  // then turn off the SEC LED 
    Wait_Half_Second();                                                                            // Wait for half second (or 500 msec) 
} 

void Wait_Half_Second() 
{ 
    T0CON = 0x02;                                                                                  // Timer 0, 16-bit mode, prescaler 1:8 
    TMR0L = 0xDB;                                                                                  // set the lower byte of TMR 
    TMR0H = 0x0B;                                                                                  // set the upper byte of TMR 
    INTCONbits.TMR0IF = 0;                                                                         // clear the Timer 0 flag 
    T0CONbits.TMR0ON = 1;                                                                          // Turn on the Timer 0 
    while (INTCONbits.TMR0IF == 0);                                                                // wait for the Timer Flag to be 1 for done 
    T0CONbits.TMR0ON = 0;                                                                          // turn off the Timer 0 
}

void Activate_Buzzer()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_500Hz()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000111 ;
    CCPR2L = 0b01111100 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_2KHz()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01111100 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_4KHz()
{
    PR2 = 0b01111100 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b00111110 ;
    CCP2CON = 0b00011100 ;
}

void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
	PORTCbits.RC1 = 0;
}

float read_volt()
{
	int nStep = get_full_ADC();
    float volt = nStep * 5 /1024.0;
	return (volt);
}

unsigned int get_full_ADC()
{
unsigned int result;
   ADCON0bits.GO=1;                     // Start Conversion
   while(ADCON0bits.DONE==1);           // wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;  // combine result of upper byte and
                                        // lower byte into result
   return result;                       // return the result.
}



