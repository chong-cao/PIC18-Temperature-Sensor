#include <stdio.h>

#include <p18f4620.h>
#include "I2C_Support.h"
#include "I2C.h"

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02
#define ACK     1
#define NAK     0


extern unsigned char second, minute, hour, dow, day, month, year;
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;
extern unsigned char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;


// Part A1
int DS1621_Read_Temp()
{
    char Device = 0x48;                 // The I2C address for this DS1621 device
    char Cmd = READ_TEMP;               // The 'Read_Temperature' command
    char Data_Ret;    
    I2C_Start();                      // Start I2C protocol
    I2C_Write((Device << 1) | 0);     // Device address
    I2C_Write(Cmd);                   // Send register address
    I2C_ReStart();                    // Restart I2C
    I2C_Write((Device << 1) | 1);     // Initialize data read
    Data_Ret = I2C_Read(NAK);         //
    I2C_Stop(); 
    return Data_Ret;
}

// Part A1
void DS1621_Init()
{
    char Device = 0x48;                 // The I2C address for this DS1621 device          
    I2C_Write_Cmd_Write_Data (Device, ACCESS_CFG, CONT_CONV);
    I2C_Write_Cmd_Only(Device, START_CONV);
}

// Part A2
void DS3231_Read_Time()
{
    char Device = 0x68;               // The I2C address for this DS3231 device
    char Address = 0x00;              // The value for the register 0x00 pointing to the register 'second'
    char Data_Ret;    
    I2C_Start();                      // Start I2C protocol
    I2C_Write((Device << 1) | 0);     // DS3231 address Write mode
    I2C_Write(Address);               // Send register address
    I2C_ReStart();                    // Restart I2C
    I2C_Write((Device << 1) | 1);     // Initialize data read
    
    second = I2C_Read(ACK);             // Allows the system to read the register 'second' from DS3231
    minute = I2C_Read(ACK);             // Allows the system to read the register 'minute' from DS3231
    hour = I2C_Read(ACK);             // Allows the system to read the register 'hour' from DS3231
    dow = I2C_Read(ACK);             // Allows the system to read the register 'dow' from DS3231
    day = I2C_Read(ACK);             // Allows the system to read the register 'day' from DS3231
    month = I2C_Read(ACK);             // Allows the system to read the register 'month' from DS3231
    year = I2C_Read(NAK);             // Allows the system to read the register 'year' from DS3231
    I2C_Stop();                                       
}

    // Part A3
void DS3231_Setup_Time()
{
    char Device = 0x68;               // The I2C address for this DS1621 device
    char Address = 0x00;              // The value for the register 0x00 pointing to the register 'second'
    
    second=0x00;                    //we picked the seconds to start at 00.
    minute=0x00;                    //we picked the minutes to start at 00.
    hour=0x04;                      //we picked the hour to start at 4.
    dow=0x05;                       //we picked the day of the week to start at 5.
    day=0x09;                       //we picked the day to start at 9.
    month=0x11;                     //we picked the month to start at 11.
    year=0x23;                      //we picked the year to start at 23, our current year.
    
    I2C_Start();                      // Start I2C protocol
    I2C_Write((Device << 1) | 0);     // Device address Write mode
    I2C_Write(Address);               // Send register address
    I2C_Write(second);                // Initialize data read by reading register second
    I2C_Write(minute);                // Read the register minute
    I2C_Write(hour);                  // Read the register hour
    I2C_Write(dow);                   // Read the register day of the week
    I2C_Write(day);                   // Read the register day
    I2C_Write(month);                 // Read the register month
    I2C_Write(year);                  // Read the register year
    I2C_Stop();                       // Stop the reading
}




