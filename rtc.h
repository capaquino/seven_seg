/*
 * rtc.h
 *
 * Created: 1/4/2020 8:30:52 PM
 *  Author: runne
 */ 


#ifndef RTC_H_
#define RTC_H_

#define DS3231_SLAVE_ADDRESS 0x68 // see datasheet
#define DS3231_SECONDS_REG_OFFSET 0x00
#define DS3231_MINUTES_REG_OFFSET 0x01
#define DS3231_HOURS_REG_OFFSET 0x02
#define DS3231_CONTROL_REG_OFFSET 0x0E

extern unsigned char rtc_read(unsigned char reg);
extern void rtc_write(unsigned char reg, unsigned char value);




#endif /* RTC_H_ */