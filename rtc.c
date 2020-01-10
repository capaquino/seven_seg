/*
 * rtc.c
 *
 * Created: 1/4/2020 8:04:14 PM
 *  Author: runne
 */ 


#include "i2cmaster.h"
#include "rtc.h"
#include <stdbool.h>

uint8_t rtc_read(unsigned char reg)
{
	uint8_t data;
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

uint8_t toSeconds(uint8_t i2c_seconds_register_read_data)
{
	return (i2c_seconds_register_read_data&0x0F)			// 0b0000 1111
		+ (((i2c_seconds_register_read_data&0x70)>>4)*10);  // 0b0111 0000 >> 4 * 10
}

uint8_t toMinutes(uint8_t i2c_minutes_register_read_data)
{
	return (i2c_minutes_register_read_data&0x0F)			// 0b0000 1111
		+ (((i2c_minutes_register_read_data&0x70)>>4)*10);		// 0b0111 0000 >> 4 * 10
}