#ifndef __BSP_USART_H
#define   __BSP_USART_H

#include "stm32f10x_lib.h"
#include "string.h"
#include "stdint.h"
#include <stdio.h>

/* 选择串口作为 printf 发送端口 */
//#define PRINTF_USART	USART1
#define PRINTF_USART	USART2
//#define PRINTF_USART	USART3

#define RS485_Mode_RX()		GPIO_ResetBits( GPIOA, GPIO_Pin_1 )
#define RS485_Mode_TX()		GPIO_SetBits( GPIOA, GPIO_Pin_1 )

void USART_Config(USART_TypeDef* USARTx, uint32_t BaudRate);					//串口配置
void USART_SendByte(USART_TypeDef* USARTx, uint8_t data);						//串口发送1Byte
u32  USART_Send(USART_TypeDef* USARTx, u8 *data_arr, u32 length);				//串口发送
void UARTx_IRQn_Interrupt(USART_TypeDef* USARTx);								//串口中断处理

#endif /* BSP_USART_H */
