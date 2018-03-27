/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Date First Issued  : 05/21/2007
* Description        : Main Interrupt Service Routines.
*                      This file can be used to describe all the exceptions 
*                      subroutines that may occur within user application.
*                      When an interrupt happens, the software will branch 
*                      automatically to the corresponding routine.
*                      The following routines are all empty, user can write code 
*                      for exceptions handlers and peripherals IRQ interrupts.
********************************************************************************
* History:
* 05/21/2007: V0.3
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "NRF24L01.h"
#include "platform_config.h"
#include "stm32_config.h"
#include "a7108.h"
#include "a7108reg.h"
#include "bsp_usart.h"

#include "gatconfig.h"
#include "gatfunc.h"
#include "gatconnect.h"
#include "gatserial.h"
#include "gatupload.h"

#include "socketconfig.h"
#include "socketfunc.h"
#include "socketinstantia.h"
#include "socketinitialization.h"
#include "socketpark.h"

#include "calculationconfig.h"
#include "iooutputconfig.h"

#include "usrconfig.h"
#include "usrserial.h"


u8 startbytes[2],startbytes[2];
u8 u8params[RECV_MAX][ACK_LENGTH];
u8 u8checkData[CHECK_PKG_MAX][4];

u8 u8numx = 0;						//收到的数据包序号
u8 u8numy = 0;						//收到的数据包byte序号
u8 u8numx_now = 0;					// 当前处理到的位置
extern u8 rssi_value[10];			//读取RSSI值,范围0-255

void UARTx_IRQ(USART_TypeDef* USARTx);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : NMIException
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMIException(void)
{
}

/*******************************************************************************
* Function Name  : HardFaultException
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFaultException(void)
{
}

/*******************************************************************************
* Function Name  : MemManageException
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManageException(void)
{
}

/*******************************************************************************
* Function Name  : BusFaultException
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFaultException(void)
{
}

/*******************************************************************************
* Function Name  : UsageFaultException
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFaultException(void)
{
}

/*******************************************************************************
* Function Name  : DebugMonitor
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMonitor(void)
{
}

/*******************************************************************************
* Function Name  : SVCHandler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVCHandler(void)
{
}

/*******************************************************************************
* Function Name  : PendSVC
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSVC(void)
{
}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int SysTick_count;
void prvvTIMERExpiredISR( void );
void SysTickHandler(void)
{
	prvvTIMERExpiredISR();
	SysTick_count++;
}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern u16 rtc_minute_cnt, rtc_second_cnt, rtc_reset_time;
extern volatile u8  SOCKET_RTC_CHECK;
void RTC_IRQHandler(void)
{
	static u8 led_on = 0;												//运行灯
	static u8 Socket_RealTime = 1;										//Socket实时上传对时
	u32 rtctime = 0;
	
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_SEC);

		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		
		if (rtc_second_cnt >= 59) {										//1分钟
			rtc_minute_cnt++;
			rtc_second_cnt = 0;
		}
		else {
			rtc_second_cnt++;											//秒+1
			rtc_reset_time++;											//重启计时器+1
		}
		
		//1440分钟(24h)无修改，除了配置时间，其他全部配置为0，配置时间修改为默认的61分钟
		if ((rtc_minute_cnt == 1440) && (rtc_second_cnt == 0))
		{
			//配置时间过小
			if(param_wvd_cfg.config_item != 0)
			{
				param_wvd_cfg.config_item = 0;
				if(param_wvd_cfg.config_interval < 61)
				{
					param_wvd_cfg.config_item = 4;
					param_wvd_cfg.config_interval = 61;
				}
				param_save_to_flash();
			}
		}
		
		//1800分钟(30h)，无修改全部配置位都设置为0，不再配置
		if ((rtc_minute_cnt == 1880) && (rtc_second_cnt == 0))
		{
			//如果配置位有不为0的
			if (param_wvd_cfg.config_item != 0)
			{
				param_wvd_cfg.config_item = 0;
				param_save_to_flash();
			}
		}
		
		//RESETTIME秒，没收到数据，重启设备
		if (rtc_reset_time >= RESETTIME)
		{
			NVIC_SETFAULTMASK();
			NVIC_GenerateSystemReset();									//系统复位
		}
		
		//运行灯
		if (led_on) {
			led_on = 0;
			Led_OFF();
		}
		else {
			led_on = 1;
			Led_ON();
		}
		
#ifdef SOCKET_ENABLE
		if (USRInitialized == USR_INITCONFIGOVER) {
			if (PlatformSocket == Socket_ENABLE) {											//根据SN选择是否使能Socket
				if (INTERVALTIME != 0) {
					socket_dev.Implement(INTERVALTIME);									//Socket协议到达指定时间处理
				}
				else {
					if (PlatformSockettime == SocketTime_ENABLE) {
						if (Socket_RealTime == 1) {
							socket_dev.Implement(10);									//对时等待10秒
						}
						if (SOCKET_RTC_CHECK == 0) {										//对好时间
							Socket_RealTime = 0;
						}
						else {
							Socket_RealTime = 1;
						}
						rtctime = RTC_GetCounter();										//获取当前时间值
						if ((rtctime % 86400) == 0) {										//判断是否到达凌晨0点
							SOCKET_RTC_CHECK = PACKETTYPE_RTCCHECKINIT;						//开启RTC对时
							GPIO_SetBits(GPIOA, GPIO_Pin_4);
						}
					}
				}
			}
		}
#endif
		
#ifdef GAT920_ENABLE
		if (PlatformGat920 == Gat920_ENABLE) {												//根据SN选择是否使能GAT920
			gat920_dev.UploadOvertime();													//Gat920协议超时重发处理
			if (GATConnect == GAT_ONLINE) {
				gat920_dev.ImplementEnqueue(gatParamDetector.detector_config.statistical_period);	//到达统计时间将统计数据存入队列
			}
		}
#endif
	}
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void)
{
}

u8 IsRxPacketDifferent(u8* rxData)
{
	u8 i, j;
	
	if (param_recv.check_repeat_time)										//如果有设置检查重复时间
	{
		for(i = 0; i < CHECK_PKG_MAX; i++)
		{
			if ((u8checkData[i][0] == rxData[0])&&
			    (u8checkData[i][1] == rxData[1])&&
			    (u8checkData[i][2] == rxData[2]))
					return 0;				
		}
		//找无用的数据空间
		for(i = 0; i < CHECK_PKG_MAX; i++)
		{
			if(u8checkData[i][1] == 0)
			{
				j = i;		
				break;
			}
		}

		u8checkData[j][0] = rxData[0];
		u8checkData[j][1] = rxData[1];
		u8checkData[j][2] = rxData[2];
		u8checkData[j][3] = param_recv.check_repeat_time;						//最大等待时间
	}
	return 1;															//是一包新的数据
}


//无线接收中断
/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 ack_geted;
void EXTI0_IRQHandler(void)
{	
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {								//判断是否产生了EXTI0中断  
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0) {					//判断是否是PA0线变低
			GPIO_SetBits(GPIOB, GPIO_Pin_5);								//点亮LED9
			if (a7108_initialized) {
				rssi_value[u8numx] = A7108_ReadReg(ADC_REG) & 0xFF;			//读取RSSI值
				mvb_RxPacket(u8params[u8numx], fifo_length);					//接收length个长度的数据
				if (u8params[u8numx][1] != 0) {							//不是ID:00xx
					if (A7108_ReadReg(MODE_REG) & 0x0200) {					//如果接收到的数据长度不是fifo_length,那么也会表现为crc error.
					}
					else {			
						//检查是否有接收过
						if(IsRxPacketDifferent(u8params[u8numx]))
						{
							rtc_reset_time = 0;							//收到数据重启计时器清0
							
							//处理配置数据
							hand_config(u8params[u8numx]);
							
							//处理Volume获取
							calculation_dev.GetVolume(u8params[u8numx]);
							
							//处理AvgSpeed获取
							calculation_dev.GetAvgSpeed(u8params[u8numx]);
							
							//处理AvgOccupancy获取
							calculation_dev.GetAvgOccupancy(u8params[u8numx]);
							
							//处理AvgHeadTime获取
							calculation_dev.GetAvgHeadTime(u8params[u8numx]);

							//处理IO 输出
							hand_IOOutput(u8params[u8numx]);
							u8numx ++;
							u8numx %= RECV_MAX;
							
#ifdef SOCKET_ENABLE
							if (PlatformSocket == Socket_ENABLE) {			//根据SN选择是否使能Socket
								if (INTERVALTIME == 0) {
									if (SOCKET_RTC_CHECK == 0) {			//对好时间
										SOCKET_ParkImplementHeartbeat(u8params[u8numx]);
									}
								}
							}
#endif
							
							
							//处理实时数据
							if(param_recv.rt_channel)
								EXTI_GenerateSWInterrupt(EXTI_Line4);
						}
					}
				}
				if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)				//当前为standby模式,那么需要进入rx模式
					mvb_RX_Mode(ACK_LENGTH);								//估计在这里耗去了1ms太多时间
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line0);								//清除EXTI0上的中断标志
	}
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
	if (func_num == A7108_SLEEP_M) {
		mvb_powerdown_mode_int();
		EXTI_ClearITPendingBit(EXTI_Line0);			//清除EXTI0上的中断标志 
	}
	else if (func_num == RX_MODE_M) {
		mvb_RX_Mode_int(spi_data_num);
		EXTI_ClearITPendingBit(EXTI_Line0);			//清除EXTI0上的中断标志
	}
	else if (func_num == SPI_Write_Buf_M) {
	}
	else if (func_num == SPI_Read_Buf_M) {
	}
	else if (func_num == TX_MODE_M) {
		spi_status = mvb_TX_Mode_int(spi_data_p, spi_data_num);
		EXTI_ClearITPendingBit(EXTI_Line0);			//清除EXTI0上的中断标志 
	}
	func_num = 0;
	EXTI_ClearITPendingBit(EXTI_Line1);				//清除EXTI0上的中断标志 
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)			//判断是否产生了EXTI0中断
	{
		EXTI_ClearITPendingBit(EXTI_Line2);			//清除EXTI0上的中断标志    
	}
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{	
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)			//判断是否产生了EXTI3中断
	{
		EXTI_ClearITPendingBit(EXTI_Line3);			//清除EXTI3上的中断标志    
	}
}
//处理实时数据软中断，由中断0间接触发，有必要占用中断吗?直接函数即可
/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request 处理实时数据
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)			//判断是否产生了EXTI4中断
  	{
		while((u8numx_now != u8numx)){		
			hand_RTdata();
		}
		EXTI_ClearITPendingBit(EXTI_Line4);			//清除EXTI4上的中断标志
	}
}

/*******************************************************************************
* Function Name  : DMAChannel1_IRQHandler
* Description    : This function handles DMA Stream 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel2_IRQHandler
* Description    : This function handles DMA Stream 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel3_IRQHandler
* Description    : This function handles DMA Stream 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel4_IRQHandler
* Description    : This function handles DMA Stream 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel5_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel6_IRQHandler
* Description    : This function handles DMA Stream 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel6_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel7_IRQHandler
* Description    : This function handles DMA Stream 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel7_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_RX1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SCE_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line6) != RESET)
	{	
		/* Clear the EXTI line 6 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line6);
	}

	if (EXTI_GetITStatus(EXTI_Line7) != RESET)
	{
		/* Clear the EXTI line 7 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	
	if (EXTI_GetITStatus(EXTI_Line8) != RESET)
	{	
		/* Clear the EXTI line 8 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	
	if (EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
		/* Clear the EXTI line 9 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
}

//10ms定时器
/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)						//检测是否产生更新中断
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);							//清除更新中断标志位
		
#ifdef GAT920_ENABLE
		if (PlatformGat920 == Gat920_ENABLE) {								//根据SN选择是否使能GAT920
			GATReceiveLength |= 1<<15;									//标记接收完成
			TIM_Cmd(TIM1, DISABLE);										//关闭TIM1
		}
#endif
	}
}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and Commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
}

//10ms定时器,Socket协议串口接收数据超时完成处理
/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)						//检测是否产生更新中断
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);							//清除更新中断标志位
		
		if (USRInitialized == USR_INITCONFIGOVER) {							//USR配置完成
#ifdef SOCKET_ENABLE
			if (PlatformSocket == Socket_ENABLE) {							//根据SN选择是否使能Socket
				SOCKET_RX_STA |= 1<<15;									//标记接收完成
				TIM_Cmd(TIM2, DISABLE);									//关闭TIM2
			}
#endif
		}
		else {														//USR配置中
#ifdef USR_ENABLE
			if ((PlatformGPRS == GPRS_ENABLE) || (PlatformRJ45 == RJ45_ENABLE)) {
				USRReceiveLength |= 1<<15;								//标记接收完成
				TIM_Cmd(TIM2, DISABLE);									//关闭TIM2
			}
#endif
		}
	}
}

//1ms定时器, 车流量数据计算定时器、IO输出补充控制定时器(开启后不可关闭)
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
	u8 i = 0;
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)						//检测是否产生更新中断
	{
		/* Clear TIM3 update interrupt */
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);							//清除更新中断标志位
		
		for (i = 0; i < OUTPUT_MAX; i++) {
			if (CalculationDataPacket[i].Speed_CarinState == 1) {				//车检入平均速度时间累加器
				CalculationDataPacket[i].Speed_CarinUseTime += 1;
			}
			
			if (CalculationDataPacket[i].Speed_CaroutState == 1) {				//车检出平均速度时间累加器
				CalculationDataPacket[i].Speed_CaroutUseTime += 1;
			}
			
			if (CalculationDataPacket[i].Occupancy_CarState == 1) {			//占有时间累加器
				CalculationDataPacket[i].Occupancy_CarUseTime += 1;
			}
			
			if (CalculationDataPacket[i].Headtime_CarState == 1) {				//车头时距累加器
				CalculationDataPacket[i].Headtime_CarUseTime += 1;
			}
		}
		
		switch (param_recv.output_mode)
		{
		//输出方式0 : 跟随车辆输出
		case 0:
			if (param_recv.handle_lost == 1) {								//判断是否需要对丢包处理
				iooutput_dev.Mode0Supplying();							//IO输出丢包补发处理函数 跟随车辆输出
			}
			break;
		//输出方式1 : 车辆进入输出固定时长(记数)
		case 1:
			iooutput_dev.Mode1Supplying();								//IO输出丢包补发处理函数 车辆进入输出固定时长(记数)
			break;
		//输出方式2 : 车辆离开输出固定时长(记数)
		case 2:
			iooutput_dev.Mode2Supplying();								//IO输出丢包补发处理函数 车辆离开输出固定时长(记数)
			break;
		//输出方式3 : 车辆进入,离开时都输出固定时长(记数)
		case 3:
			iooutput_dev.Mode3Supplying();								//IO输出丢包补发处理函数 车辆进入,离开时都输出固定时长(记数)
			break;
		//输出方式4 : 车辆进入输出固定时长(不记数)
		case 4:
			iooutput_dev.Mode4Supplying();								//IO输出丢包补发处理函数 车辆进入输出固定时长(不记数)
			break;
		//输出方式5 : 车辆离开输出固定时长(不记数)
		case 5:
			iooutput_dev.Mode5Supplying();								//IO输出丢包补发处理函数 车辆离开输出固定时长(不记数)
			break;
		//输出方式6 : 车辆进入,离开时都输出固定时长(不记数)
		case 6:
			iooutput_dev.Mode6Supplying();								//IO输出丢包补发处理函数 车辆进入,离开时都输出固定时长(不记数)
			break;
		//默认 输出方式0 : 跟随车辆输出
		default :
			if (param_recv.handle_lost == 1) {								//判断是否需要对丢包处理
				iooutput_dev.Mode0Supplying();							//IO输出丢包补发处理函数 跟随车辆输出
			}
			break;
		}
	}
}

//100ms定时器,检查接收是否重复定时器(开启后不可关闭)
/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{
	u8 i;
	/* Clear TIM4 update interrupt */
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

	if(param_recv.check_repeat_time)//如果有设置检查重复时间
	{
		for(i = 0; i < CHECK_PKG_MAX; i++)
		{
			if(u8checkData[i][1] != 0)
			{
				u8checkData[i][3]--; //减1
				if(u8checkData[i][3] == 0)
					u8checkData[i][1] = 0; //缓存时间已到
			}		
		}
	}
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
}


/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
#ifdef MODBUS_ENABLE
#ifdef MB_SERIALPORT_USART1												//ModBus中断
	if (PlatformModbusUsart == Modbus_USART1) {								//根据SN选择Modbus连接串口1
		UARTx_IRQ(USART1);
	}
#endif
#endif

#ifdef SOCKET_ENABLE
#ifdef SOCKET_SERIALPORT_USART1											//SOCKET中断
	if (PlatformSocket == Socket_ENABLE) {									//根据SN选择是否使能Socket
		socket_dev.UARTx_IRQ(USART1);
	}
#endif
#endif
	
#ifdef GAT920_ENABLE													//GAT920中断
#ifdef GAT920_SERIALPORT_USART1
	if (PlatformGat920 == Gat920_ENABLE) {									//根据SN选择是否使能GAT920
		if (PlatformGat920Usart == Gat920_USART1) {							//根据SN选择Gat920连接串口1
			gat920_dev.UARTx_IRQ(USART1);
		}
	}
#endif
#endif
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
#ifdef MODBUS_ENABLE
#ifdef MB_SERIALPORT_USART2												//ModBus中断
	if (PlatformModbusUsart == Modbus_USART2) {								//根据SN选择Modbus连接串口2
		UARTx_IRQ(USART2);
	}
#endif
#endif

#ifdef SOCKET_ENABLE
#ifdef SOCKET_SERIALPORT_USART2											//SOCKET中断
	if (PlatformSocket == Socket_ENABLE) {									//根据SN选择是否使能Socket
		socket_dev.UARTx_IRQ(USART2);
	}
#endif
#endif
	
#ifdef GAT920_ENABLE													//GAT920中断
#ifdef GAT920_SERIALPORT_USART2
	if (PlatformGat920 == Gat920_ENABLE) {									//根据SN选择是否使能GAT920
		if (PlatformGat920Usart == Gat920_USART2) {							//根据SN选择Gat920连接串口2
			gat920_dev.UARTx_IRQ(USART2);
		}
	}
#endif
#endif
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
#ifdef MODBUS_ENABLE
#ifdef MB_SERIALPORT_USART3												//ModBus中断
	UARTx_IRQ(USART3);
#endif
#endif

	if (USRInitialized == USR_INITCONFIGOVER) {								//USR配置完成
#ifdef SOCKET_ENABLE
#ifdef SOCKET_SERIALPORT_USART3											//SOCKET中断
		if (PlatformSocket == Socket_ENABLE) {								//根据SN选择是否使能Socket
			socket_dev.UARTx_IRQ(USART3);
		}
#endif
#endif
	}
	else {															//USR配置中
#ifdef USR_ENABLE
		if ((PlatformGPRS == GPRS_ENABLE) || (PlatformRJ45 == RJ45_ENABLE)) {
			USR_UARTx_IRQ(USART3);										//USR中断
		}
#endif
	}
	
#ifdef GAT920_ENABLE													//GAT920中断
#ifdef GAT920_SERIALPORT_USART3
	if (PlatformGat920 == Gat920_ENABLE) {									//根据SN选择是否使能GAT920
		gat920_dev.UARTx_IRQ(USART3);
	}
#endif
#endif
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line10) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
	if(EXTI_GetITStatus(EXTI_Line14) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}

/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
	
}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
