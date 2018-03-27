#ifndef __SOCKET_INITIALIZATION_H
#define   __SOCKET_INITIALIZATION_H

#include "stm32f10x_lib.h"
#include "platform_config.h"

extern volatile u16 SOCKET_RX_STA;											//接收数据状态
extern volatile u8  SOCKET_RTC_CHECK;										//RTC对时校验标志

void SOCKET_Init(void);													//初始化Socket协议
void SOCKET_PortSerialInit(USART_TypeDef* USARTx, u32 BaudRate);					//初始化Socket串口
void SOCKET_UARTx_IRQ(USART_TypeDef* USARTx);								//Socket协议串口中断处理函数
void SOCKET_Implement(u16 sendtime);										//Socket协议处理

void SOCKET_USARTSendByte(USART_TypeDef* USARTx, u8 data);
u32  SOCKET_USARTSend(USART_TypeDef* USARTx, u8 *data_arr, u32 length);


#endif
