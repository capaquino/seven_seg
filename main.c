/*
 * nixie.c
 *
 * Created: 12/26/2019 8:33:28 PM
 * Author : Nathan
 */ 

// should redo this in  c++

#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>

#define F_CPU 8000000UL
#include <util/delay.h>

#define HC595_PORT PORTC
#define HC595_DDR DDRC
#define HC595_DATA PORTC2
#define HC595_CLOCK PORTC1
#define HC595_LATCH PORTC0

void hc595_clock_pulse(void)
{
	HC595_PORT |= 1<<HC595_CLOCK;
	HC595_PORT &= ~(1<<HC595_CLOCK);
}

void hc595_latch_pulse(void)
{
	HC595_PORT |= 1<<HC595_LATCH;
	//_delay_us(1);
	HC595_PORT &= ~(1<<HC595_LATCH);
	//_delay_us(1);
}

void shift_bytes_msb(uint8_t bytes[], unsigned int numberOfBytes)
{
	uint8_t data = 0;
	
	for (unsigned int b = 0; b < numberOfBytes; b++)
	{
		data = bytes[b];
		for (uint8_t i = 0; i < 8; i++)
		{
			if (data & 0x80)
			{
				HC595_PORT |= 1<<HC595_DATA;
			}
			else
			{
				HC595_PORT &= ~(1<<HC595_DATA);
			}
		
			hc595_clock_pulse();
		
			data<<=1;
		}
	}
	
	hc595_latch_pulse();
}

void shift_byte_msb(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (data & 0x80)
		{
			HC595_PORT |= 1<<HC595_DATA;
		}
		else
		{
			HC595_PORT &= ~(1<<HC595_DATA);
		}
			
		hc595_clock_pulse();
			
		data<<=1;
	}
	
	hc595_latch_pulse();
}

void shift_byte_lsb(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (data & 0x01)
		{
			HC595_PORT |= 1<<HC595_DATA;
		}
		else
		{
			HC595_PORT &= ~(1<<HC595_DATA);
		}
		
		hc595_clock_pulse();
		
		data>>=1;
	}
	
	hc595_latch_pulse();
}

#define ANODES_PORT PORTD
#define ANODES_DDR DDRD

#define ONES_OFFSET PORTD0
#define TENS_OFFSET PORTD1
#define HUNDREDS_OFFSET PORTD2
#define THOUSANDS_OFFSET PORTD3
#define NEGATIVE_SIGN 0xA
#define ERROR_SYMBOL_X 0xF

#define A 1
#define B 2
#define C 4
#define D 8
#define E 16
#define F 32
#define G 64
#define DP 128

typedef enum tag_Position
{
	ONES,
	TENS,
	HUNDREDS,
	THOUSANDS
} Position;

//typedef struct tag_SevenSegNumber
//{
	//Position 
	//
//} SevenSegNumber;
//
//typedef struct tag_SevenSegmentDisplay
//{
	//SevenSegNumber ones;
	//SevenSegNumber tens;
	//SevenSegNumber hundreds;
	//SevenSegNumber thousands;
	//Position decimal_point;
	//
//} SevenSegmentDisplay;

// position
void write_digit(Position position, uint8_t digit, bool decimalPoint)
{
	// dont need the position if you do some math.
	// if (digit <= 9999) good can fit in display
	// digit % 10 first digit (lsb)
	// divide by 10, LSB will drop cause computer math
	// repeat till digit is < 10
	// only works for integers! need to do some special stuff for float.
	// this way might be better if you want decimals, might be a std lib function that gets decimal of a float.
	
	// Common Cathode
		
	// Clear previous digit
	shift_byte_msb(0x00); // think there's a pin that'll do this instantly (_MR_)? Needs another pin though.
	
	// This must come after shift byte msb BECAUSE there are delays in shift byte msb (see latch function)
	// did reduce latch delays to microsecond range so this might be okay now.
	
	//ANODES_PORT |= 1<<ONES_OFFSET | 1<<TENS_OFFSET | 1<<HUNDREDS_OFFSET | 1<<THOUSANDS_OFFSET; // won't affect D4-7
	
	// if using npn transistors
	ANODES_PORT &= ~(1<<ONES_OFFSET | 1<<TENS_OFFSET | 1<<HUNDREDS_OFFSET | 1<<THOUSANDS_OFFSET); // won't affect D4-7
	
	// Set anode AKA digit position
	switch (position)
	{
		case ONES:
		//ANODES_PORT &= ~(1<<ONES_OFFSET);
		ANODES_PORT |= 1<<ONES_OFFSET;
		break;
		case TENS:
		//ANODES_PORT &= ~(1<<TENS_OFFSET);
		ANODES_PORT |= 1<<TENS_OFFSET;
		break;
		case HUNDREDS:
		//ANODES_PORT &= ~(1<<HUNDREDS_OFFSET);
		ANODES_PORT |= 1<<HUNDREDS_OFFSET;
		break;
		case THOUSANDS:
		//ANODES_PORT &= ~(1<<THOUSANDS_OFFSET);
		ANODES_PORT |= 1<<THOUSANDS_OFFSET;
		break;
	}
	
	uint8_t segmentData = 0;
	
	switch (digit)
	{
		case 0: segmentData = A|B|C|D|E|F;		break;
		case 1: segmentData = B|C;				break;
		case 2: segmentData = A|B|G|E|D;		break;
		case 3: segmentData = A|B|G|C|D;		break;
		case 4: segmentData = F|G|B|C;			break;
		case 5: segmentData = A|F|G|C|D;		break;
		case 6: segmentData = F|G|C|D|E;		break;
		case 7: segmentData = A|B|C;			break;
		case 8: segmentData = A|B|C|D|E|F|G;	break;
		case 9: segmentData = A|B|C|D|F|G;		break;
		case NEGATIVE_SIGN: segmentData = G;				break; // negative sign
		case ERROR_SYMBOL_X: segmentData = B|G|C|E|F;		break; // X or H symbol, denotes known error
		default:  segmentData = A|B|D|E;		break; // denotes unknown error
	}
	
	// Set cathodes AKA 7 segments
	if (decimalPoint)
	{
		segmentData |= DP;
	}
	
	shift_byte_msb(segmentData);
}


void display_error()
{
	Position p = ONES;
	for (uint8_t i = 0; i<4; i++)
	{
		write_digit(p,ERROR_SYMBOL_X,true);
		p++;
	}
}

void write_4dig_unsigned_number(unsigned int number)
{
	if (number <= 9999)
	{
		uint8_t lsb;
		Position p = ONES;
		bool clockDp; // testing for clock
		while (number >= 10)
		{		
			clockDp = false; // testing for clock
			if (p == HUNDREDS) clockDp = true; // testing for clock, >= is fine too since if there is thousands place it is done after loop always with DP off.
			lsb = number % 10;
			write_digit(p,lsb,clockDp);
			number /= 10;
			p++;				// set next digit position one to the left.
		}
		// msb needs to be done with one more at end. Better way to do this in-loop?
		if (number > 0) // number is msb
		{
			write_digit(p,number,false);
		}
		// for clock behaviour will need special cases or functions for minutes/hours/decimals.
		// like this. Like if less than 10, will want to display 01 02 03 04 05 06 etc, scroll
		else if (number == 0)
		{
			write_digit(ONES,0,false);
			write_digit(TENS,0,false);
		}
	}
	else
	{
		display_error();
	}
}

void write_3dig_signed_number(int number)
{
	bool negative = false;
	
	if (number < 0)
	{
		negative = true;
		number = -number;
	}
	
	if (number <= 999)
	{
		uint8_t lsb;
		Position p = ONES;
		while (number >= 10)
		{
			lsb = number % 10;
			write_digit(p,lsb,false);
			number /= 10;
			p++;				// set next digit position one to the left.
		}
		// msb needs to be done with one more at end. Better way to do this in-loop?
		if (number >= 0)
		{
			write_digit(p,number,false);
		}
		
		// negative place
		if (negative)
		{
			write_digit(THOUSANDS,NEGATIVE_SIGN,false);
		}
	}
	else // outside of range.
	{
		display_error();
	}
}

#include "i2cmaster.h"
#include "rtc.h"

int main(void)
{
		HC595_DDR = 1<<HC595_DATA | 1<<HC595_CLOCK | 1<<HC595_LATCH;
		ANODES_DDR =  1<<ONES_OFFSET | 1<<TENS_OFFSET | 1<<HUNDREDS_OFFSET | 1<<THOUSANDS_OFFSET;
		
		// Peter's lib
		i2c_init();
		
		rtc_write(DS3231_CONTROL_REG_OFFSET,0x00);
		rtc_write(DS3231_HOURS_REG_OFFSET,12); // this may or may not be 12
		rtc_write(DS3231_MINUTES_REG_OFFSET,toRegisterValue(29)); // 29 min
		rtc_write(DS3231_SECONDS_REG_OFFSET,toRegisterValue(45)); // 28s
		uint8_t rtc_data_sec = NEGATIVE_SIGN;
		uint8_t rtc_data_min = NEGATIVE_SIGN;
		
		unsigned int displayValue = NEGATIVE_SIGN;
		
    	while (1) 
		{
			// Peter's lib
			rtc_data_sec = rtc_read(DS3231_SECONDS_REG_OFFSET);
			rtc_data_min = rtc_read(DS3231_MINUTES_REG_OFFSET);
			
			//displayValue = toMinutes(rtc_data_min) * 100 + toSeconds(rtc_data_sec);
			//displayValue = bcd2dec(rtc_data_sec) + bcd2dec(rtc_data_min)*100; // works too
			displayValue = fromRegisterValue(rtc_data_min) * 100 + fromRegisterValue(rtc_data_sec);
			
			write_4dig_unsigned_number( displayValue );
			
		}
}

