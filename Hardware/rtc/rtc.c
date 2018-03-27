/**
  ******************************************************************************
  * @file    rtc.c
  * @author  movebroad
  * @version V1.0
  * @date    
  * @brief   rtc 时间设置与获取
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include "rtc.h"

/* 时间结构体 */
struct rtc_time systemtime;

/***************************************************************
 @Function			void RTC_Time_Init(struct rtc_time *tm)
 @Description			检查并配置RTC
 @Input				用于读取RTC时间的结构体指针
 @Return				void
***************************************************************/
void RTC_Time_Init(struct rtc_time *tm)
{
	/* 在启动时检查备份寄存器BKP_DR1, 如果内容不是 0xA5A5,则需重新配置时间并询问用户调整时间 */
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		RTC_Config();												//配置RTC
		
		Time_Adjust(tm);											//时间调节
		
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);						//备份寄存器写入值
	}
	/* 启动无需设置新时钟 */
	else
	{
		/* 检查是否掉电重启 */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			
		}
		/* 检查是否Reset复位 */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			
		}
		
		/* 等待寄存器同步 */
		RTC_WaitForSynchro();
		
		/* 允许RTC秒中断 */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		
		/* 等待上次RTC寄存器写操作完成 */
		RTC_WaitForLastTask();
	}
	
	/* Clear reset flags */
	RCC_ClearFlag();
}

/***************************************************************
 @Function			void RTC_Config(void)
 @Description			配置RTC
 @Input				void
 @Return				void
***************************************************************/
void RTC_Config(void)
{
	/* Enable PWR and BKP Clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Enable RTC and BKP access */
	PWR_BackupAccessCmd(ENABLE);
	
	/* Reset Backup Domain */
	BKP_DeInit();
	
	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	
	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC registers synchronization 
	 * 因为RTC时钟是低速的，内环时钟是高速的，所以要同步
	 */
	RTC_WaitForSynchro();
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);								//使能RTC秒中断
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Set RTC prescaler: set RTC period to 1sec */
	/* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) = 1HZ */
	RTC_SetPrescaler(32767);										//RTC预分频值
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

/***************************************************************
 @Function			void Time_Adjust(struct rtc_time *tm)
 @Description			时间调节
 @Input				用于读取RTC时间的结构体指针
 @Return				void
***************************************************************/
void Time_Adjust(struct rtc_time *tm)
{
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Get time entred by the user on the hyperterminal */
	/* 设置默认时间 */
	tm->tm_year	=	2017;
	tm->tm_mon	=	7;
	tm->tm_mday	=	26;
	tm->tm_hour	=	14;
	tm->tm_min	=	30;
	tm->tm_sec	=	00;
	
	/* Get weekday */
	GregorianDay(tm);
	
	/* 修改当前RTC计数寄存器内容 */
	RTC_SetCounter(mktimev(tm));
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

/******************************** END OF FLEE **********************************/
