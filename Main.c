#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#include "I2C.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "Fan_Support.h"
#include "Main.h"
#include "ST7735_TFT.h"
#include "utils.h"

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF


// colors
#define RD1               1
#define GR1               2
#define YL1				 3
#define BL1               4
#define MA1               5
#define CY1				 6
#define WH1               7

void test_alarm();
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char found;
char tempSecond = 0xff; 
signed int DS1621_tempC, DS1621_tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
unsigned char fan_set_temp = 75;

float volt;
char INT0_flag, INT1_flag, INT2_flag;

short Nec_ok = 0;
char Nec_Button;
char FAN;
char duty_cycle;
int rps;
int rpm;
int ALARMEN;
int alarm_mode, MATCHED;


char buffer[31]     = " ECE3301L FA21_S#\0";
char *nbr;
char *txt;
char tempC[]        = "+25";
char tempF[]        = "+77";
char time[]         = "00:00:00";
char date[]         = "00/00/00";
char alarm_time[]   = "00:00:00";
char Alarm_SW_Txt[] = "OFF";
char Fan_Set_Temp_Txt[] = "075F";
char Fan_SW_Txt[]   = "OFF";                // text storage for Heater Mode


char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};								
int color[21]={RD1,RD1, RD1, BL1, BL1,GR1,MA1, MA1, MA1,WH1, WH1, WH1,WH1, WH1, WH1,WH1, WH1, WH1,WH1, WH1, WH1};					// add more value into this array
    
char DC_Txt[]       = "000";                // text storage for Duty Cycle value
char Volt_Txt[]     = "0.00V";
char RTC_ALARM_Txt[]= "0";                  //
char RPM_Txt[]      = "0000";               // text storage for RPM

char setup_time[]       	= "00:00:00";
char setup_date[]       	= "01/01/00";
char setup_alarm_time[] 	= "00:00:00"; 
char setup_fan_set_text[]   = "075F";


void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}



void Do_Init()                      // Initialize the ports 
{ 
    init_UART();                    // Initialize the uart
    //Init_ADC();
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    ADCON1=0x0E;
    TRISA = 0x11;
    TRISB = 0x07;
    TRISC = 0x01;
    TRISD = 0x00;
    TRISE = 0x00;
    PORTE = 0x00;

    FAN = 0;
    RBPU=0;

    TMR3L = 0x00;                   
    T3CON = 0x03;
    I2C_Init(100000); 

    DS1621_Init();
    init_Interrupt();
    Turn_Off_Fan();
    fan_set_temp = 75;
}


void main() 
{
    Do_Init();                                                  // Initialization  
    Initialize_Screen();  
    tempSecond = 0xff;
    while (1)
    {
        DS3231_Read_Time();

        if(tempSecond != second)
        {
            tempSecond = second;
            rpm = get_RPM();

            DS1621_tempC = DS1621_Read_Temp();
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;

            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);

            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);

            Update_Screen();
        }
		

		if (Nec_ok == 1)
		{
            Nec_ok = 0;


            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge
			printf ("Nec_Button = %x\r\n", Nec_Button);
			
			
            found = 0xff;
            for (int j=0; j< 21; j++)
            {
                if (Nec_Button == array1[j]) 
                {
                    found = j;
                    j = 21;

                }
            }
            
            if (found == 0xff) 
            {
                printf ("Cannot find button \r\n");
                return;
            }
            else
            {
                printf ("Key position %x\r\n", found);
				PORTD = (PORTD & 0xF1) | (color[found] << 1);// add code to output the color of the button to the RGB D1. The color will stay on until the next button is pressed
                PORTDbits.RD7=1; //Turn on KEY PRESSED
                Activate_Buzzer();			//Generate Beep Tone	
                Wait_One_Second();// wait 1 second
				Deactivate_Buzzer();// remove beep tone
				PORTDbits.RD7=0;// turn off KEY PRESSED
				
				switch (found)
				{
					case 6:        
						Decrease_Speed();// add code to decrease the duty cycle
						break;
					
					case 7:           
						Increase_Speed();// add code to increase the duty cycle
						break;
						
					case 5:
						Toggle_Fan();// add code to toggle fan on/off            
						break;
						
					case 8:
                        Initialize_Screen();
						Update_Screen();// add code to load default time;
						 break;           
			
					default:
						Do_Beep_Bad();
						break;
				}
			}    
			

    }
}
}






