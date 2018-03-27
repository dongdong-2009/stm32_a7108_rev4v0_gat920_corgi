#ifndef __RTC_H
#define   __RTC_H

#include "stm32f10x_lib.h"
#include "rtc_date.h"
#include "stdint.h"

extern struct rtc_time systemtime;

void RTC_Time_Init(struct rtc_time *tm);							//检查并配置RTC
void RTC_Config(void);											//配置RTC
void Time_Adjust(struct rtc_time *tm);								//时间调节

#endif /* RTC_H */
