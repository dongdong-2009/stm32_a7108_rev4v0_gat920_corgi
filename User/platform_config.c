/**
  *********************************************************************************************************
  * @file    platform_config.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "stm32f10x_lib.h"
#include "platform_config.h"


PlatFormSocketTime			PlatformSockettime = SocketTime_DISABLE;			//Socket对时项
PlatFormSocket				PlatformSocket = Socket_DISABLE;					//Socket使能项
PlatFormGat920				PlatformGat920 = Gat920_DISABLE;					//GAT920使能项
PlatFormModBusUSART			PlatformModbusUsart = Modbus_USART2;				//Modbus连接串口项
PlatFormGat920USART			PlatformGat920Usart = Gat920_USART1;				//Gat920连接串口项
PlatFormGPRS				PlatformGPRS = GPRS_DISABLE;						//GPRS使能项
PlatFormRJ45				PlatformRJ45 = RJ45_DISABLE;						//RJ45使能项

/**********************************************************************************************************
 @Function			void PlatForm_SnToFunction(u32 Sn)
 @Description			根据SN码选择开启对应功能项
 @Input				SN
 @Return				void
**********************************************************************************************************/
void PlatForm_SnToFunction(u32 SN)
{
	u8 index = 0;
	
	if ((SN == 0xFFFFFFFF) || (SN == 0x12345678)) {							//SN未烧入
		PlatformSockettime = SocketTime_DISABLE;							//Socket对时关闭
		PlatformSocket = Socket_DISABLE;									//Socket关闭
		PlatformGat920 = Gat920_DISABLE;									//Gat920关闭
		PlatformModbusUsart = Modbus_USART2;								//Modbus连接串口2
		PlatformGat920Usart = Gat920_USART1;								//Gat920连接串口1
		PlatformGPRS = GPRS_DISABLE;										//GPRS不使用
		PlatformRJ45 = RJ45_DISABLE;										//RJ45不使用
		return;
	}
	
	index = (SN % 1000) / 100;
	
	switch (index)
	{
	case 0:
		/* Modbus --> USART2, Gat920 --> USART1, Socket --> DISABLE, SocketTime --> DISABLE, Gat920 --> DISABLE, GPRS --> DISABLE, RJ45 --> DISABLE */
		PlatformSockettime = SocketTime_DISABLE;							//Socket对时关闭
		PlatformSocket = Socket_DISABLE;									//Socket关闭
		PlatformGat920 = Gat920_DISABLE;									//Gat920关闭
		PlatformModbusUsart = Modbus_USART2;								//Modbus连接串口2
		PlatformGat920Usart = Gat920_USART1;								//Gat920连接串口1
		PlatformGPRS = GPRS_DISABLE;										//GPRS不使用
		PlatformRJ45 = RJ45_DISABLE;										//RJ45不使用
		break;
	
	case 1:
		/* Modbus --> USART1, Gat920 --> USART2, Socket --> DISABLE, SocketTime --> DISABLE, Gat920 --> DISABLE, GPRS --> DISABLE, RJ45 --> DISABLE */
		PlatformSockettime = SocketTime_DISABLE;							//Socket对时关闭
		PlatformSocket = Socket_DISABLE;									//Socket关闭
		PlatformGat920 = Gat920_DISABLE;									//Gat920关闭
		PlatformModbusUsart = Modbus_USART1;								//Modbus连接串口1
		PlatformGat920Usart = Gat920_USART2;								//Gat920连接串口2
		PlatformGPRS = GPRS_DISABLE;										//GPRS不使用
		PlatformRJ45 = RJ45_DISABLE;										//RJ45不使用
		break;
	
	case 2:
		/* Modbus --> USART2, Gat920 --> USART1, Socket --> ENABLE, SocketTime --> DISABLE, Gat920 --> DISABLE, GPRS --> DISABLE, RJ45 --> ENABLE */
		PlatformSockettime = SocketTime_DISABLE;							//Socket对时关闭
		PlatformSocket = Socket_ENABLE;									//Socket开启
		PlatformGat920 = Gat920_DISABLE;									//Gat920关闭
		PlatformModbusUsart = Modbus_USART2;								//Modbus连接串口2
		PlatformGat920Usart = Gat920_USART1;								//Gat920连接串口1
		PlatformGPRS = GPRS_DISABLE;										//GPRS不使用
		PlatformRJ45 = RJ45_ENABLE;										//RJ45使用
		break;
	
	case 3:
		/* Modbus --> USART2, Gat920 --> USART1, Socket --> ENABLE, SocketTime --> ENABLE, Gat920 --> DISABLE, GPRS --> DISABLE, RJ45 --> ENABLE */
		PlatformSockettime = SocketTime_ENABLE;								//Socket对时开启
		PlatformSocket = Socket_ENABLE;									//Socket开启
		PlatformGat920 = Gat920_DISABLE;									//Gat920关闭
		PlatformModbusUsart = Modbus_USART2;								//Modbus连接串口2
		PlatformGat920Usart = Gat920_USART1;								//Gat920连接串口1
		PlatformGPRS = GPRS_DISABLE;										//GPRS不使用
		PlatformRJ45 = RJ45_ENABLE;										//RJ45使用
		break;
	
	case 4:
		/* Modbus --> USART2, Gat920 --> USART1, Socket --> DISABLE, SocketTime --> DISABLE, Gat920 --> ENABLE, GPRS --> DISABLE, RJ45 --> DISABLE */
		PlatformSockettime = SocketTime_DISABLE;							//Socket对时关闭
		PlatformSocket = Socket_DISABLE;									//Socket关闭
		PlatformGat920 = Gat920_ENABLE;									//Gat920开启
		PlatformModbusUsart = Modbus_USART2;								//Modbus连接串口2
		PlatformGat920Usart = Gat920_USART1;								//Gat920连接串口1
		PlatformGPRS = GPRS_DISABLE;										//GPRS不使用
		PlatformRJ45 = RJ45_DISABLE;										//RJ45不使用
		break;
	
	case 5:
		/* Modbus --> USART1, Gat920 --> USART2, Socket --> DISABLE, SocketTime --> DISABLE, Gat920 --> ENABLE, GPRS --> DISABLE, RJ45 --> DISABLE */
		PlatformSockettime = SocketTime_DISABLE;							//Socket对时关闭
		PlatformSocket = Socket_DISABLE;									//Socket关闭
		PlatformGat920 = Gat920_ENABLE;									//Gat920开启
		PlatformModbusUsart = Modbus_USART1;								//Modbus连接串口1
		PlatformGat920Usart = Gat920_USART2;								//Gat920连接串口2
		PlatformGPRS = GPRS_DISABLE;										//GPRS不使用
		PlatformRJ45 = RJ45_DISABLE;										//RJ45不使用
		break;
	
	case 6:
		/* Modbus --> USART2, Gat920 --> USART1, Socket --> ENABLE, SocketTime --> DISABLE, Gat920 --> DISABLE, GPRS --> ENABLE, RJ45 --> DISABLE */
		PlatformSockettime = SocketTime_DISABLE;							//Socket对时关闭
		PlatformSocket = Socket_ENABLE;									//Socket开启
		PlatformGat920 = Gat920_DISABLE;									//Gat920关闭
		PlatformModbusUsart = Modbus_USART2;								//Modbus连接串口2
		PlatformGat920Usart = Gat920_USART1;								//Gat920连接串口1
		PlatformGPRS = GPRS_ENABLE;										//GPRS使用
		PlatformRJ45 = RJ45_DISABLE;										//RJ45不使用
		break;
	
	case 7:
		/* Modbus --> USART2, Gat920 --> USART1, Socket --> ENABLE, SocketTime --> ENABLE, Gat920 --> DISABLE, GPRS --> ENABLE, RJ45 --> DISABLE */
		PlatformSockettime = SocketTime_ENABLE;								//Socket对时开启
		PlatformSocket = Socket_ENABLE;									//Socket开启
		PlatformGat920 = Gat920_DISABLE;									//Gat920关闭
		PlatformModbusUsart = Modbus_USART2;								//Modbus连接串口2
		PlatformGat920Usart = Gat920_USART1;								//Gat920连接串口1
		PlatformGPRS = GPRS_ENABLE;										//GPRS使用
		PlatformRJ45 = RJ45_DISABLE;										//RJ45不使用
		break;
	
	default :
		PlatformSockettime = SocketTime_DISABLE;							//Socket对时关闭
		PlatformSocket = Socket_DISABLE;									//Socket关闭
		PlatformGat920 = Gat920_DISABLE;									//Gat920关闭
		PlatformModbusUsart = Modbus_USART2;								//Modbus连接串口2
		PlatformGat920Usart = Gat920_USART1;								//Gat920连接串口1
		PlatformGPRS = GPRS_DISABLE;										//GPRS不使用
		PlatformRJ45 = RJ45_DISABLE;										//RJ45不使用
		break;
	}
}

/********************************************** END OF FLEE **********************************************/
