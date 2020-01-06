/*
 * rtc.c
 *
 * Created: 1/4/2020 8:04:14 PM
 *  Author: runne
 */ 


#include "i2cmaster.h"
#include "rtc.h"
#include <stdbool.h>

unsigned char rtc_read(unsigned char reg)
{
	unsigned char data;
	i2c_start(DS3231_SLAVE_ADDRESS+I2C_WRITE);
	i2c_write(reg);
	i2c_stop();
		
	i2c_start(DS3231_SLAVE_ADDRESS+I2C_READ);
	data = i2c_readNak();
	i2c_stop();
	
	return data;// & 0x0F;
}

void rtc_write(unsigned char reg, unsigned char value)
{
	i2c_start(DS3231_SLAVE_ADDRESS+I2C_WRITE);
	i2c_write(reg);
	i2c_stop();
	
	i2c_start(DS3231_SLAVE_ADDRESS+I2C_WRITE);
	i2c_write(value);
	i2c_stop();
}