#ifndef __RTC_DATE_H
#define   __RTC_DATE_H

#include "stm32f10x_lib.h"
#include "stdint.h"

/* RTC时间结构体 */
struct rtc_time {
	int tm_sec;			//秒
	int tm_min;			//分
	int tm_hour;			//时
	int tm_mday;			//日
	int tm_mon;			//月
	int tm_year;			//年
	int tm_wday;			//星期
};

void GregorianDay(struct rtc_time * tm);
uint32_t mktimev(struct rtc_time *tm);
void to_tm(uint32_t tim, struct rtc_time * tm);				//rtc寄存器值转换时间结构体值

#endif
