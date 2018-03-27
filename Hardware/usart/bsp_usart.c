/**
  ******************************************************************************
  * @file    usart.c
  * @author  movebroad
  * @version V1.0
  * @date    
  * @brief   USART Device
  ******************************************************************************
  * @attention
  *			 USART1 : RS485
  *			 USART2 : RS232
  *			 USART3 : TTL
  ******************************************************************************
  */

#include "bsp_usart.h"

extern void Delay(vu32 nCount);

/***************************************************************
 @Function			void USART_Config(USART_TypeDef* USARTx, uint32_t BaudRate)
 @Description			USART 配置
 @Input				USARTx	: 配置串口号
					BaudRate  : 配置波特率
 @Return				void
***************************************************************/
void USART_Config(USART_TypeDef* USARTx, uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_struct;
	USART_InitTypeDef USART_struct;
	
	/* 串口时钟GPIO配置 */
	if (USARTx == USART1) {
		/* Config USART1 and GPIOA Clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
		
		/* USART1 GPIO Config */
		/* Config USART1 TX ( PA.9 ) */
		GPIO_struct.GPIO_Pin = GPIO_Pin_9;
		GPIO_struct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_struct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_struct);
		/* Config USART1 RX ( PA.10 ) */
		GPIO_struct.GPIO_Pin = GPIO_Pin_10;
		GPIO_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_struct);
		/* Config USART1_RS485 DE ( PA.1 ) */
		GPIO_struct.GPIO_Pin = GPIO_Pin_1;
		GPIO_struct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_struct);
		RS485_Mode_RX();											//默认485为接收模式
	}
	else if (USARTx == USART2) {
		/* Config USART2 and GPIOA Clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
		/* USART2 GPIO Config */
		/* Config USART2 TX ( PA.2 ) */
		GPIO_struct.GPIO_Pin = GPIO_Pin_2;
		GPIO_struct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_struct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_struct);
		/* Config USART2 RX ( PA.3 ) */
		GPIO_struct.GPIO_Pin = GPIO_Pin_3;
		GPIO_struct.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_struct);
	}
	else if (USARTx == USART3) {
		/* Config USART3 and GPIOB Clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		
		/* USART3 GPIO Config */
		/* Config USART3 TX ( PB.10 ) */
		GPIO_struct.GPIO_Pin = GPIO_Pin_10;
		GPIO_struct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_struct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_struct);
		/* Config USART3 RX ( PB.11 ) */
		GPIO_struct.GPIO_Pin = GPIO_Pin_11;
		GPIO_struct.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOB, &GPIO_struct);
	}
	
	/* USART MODE Config */
	USART_struct.USART_BaudRate = BaudRate;
	USART_struct.USART_WordLength = USART_WordLength_8b;
	USART_struct.USART_StopBits = USART_StopBits_1;
	USART_struct.USART_Parity = USART_Parity_No;
	USART_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_struct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USARTx, &USART_struct);

	/* 串口中断开关配置 */
	if (USARTx == USART1) {
//		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
	else if (USARTx == USART2) {
//		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	}
	else if (USARTx == USART3) {
//		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}

	USART_Cmd(USARTx, ENABLE);
}

/***************************************************************
 @Function			void USART_SendByte(USART_TypeDef* USARTx, uint8_t data)
 @Description			USART 
 @Input				data : 8bit
 @Return				void
***************************************************************/
void USART_SendByte(USART_TypeDef* USARTx, uint8_t data)
{
	USART_SendData(USARTx, (uint8_t) data);
	
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

/***************************************************************
 @Function			uint8_t USART_Send(USART_TypeDef* USARTx, uint8_t *data_arr, uint8_t length)
 @Description			USART 发送数据
 @Input				串口号, 数据, 数据长度
 @Return				void
***************************************************************/
u32 USART_Send(USART_TypeDef* USARTx, u8 *data_arr, u32 length)
{
	u32 i = 0;
	
	if (USARTx == USART1) {
		RS485_Mode_TX();
		Delay(0x1fff);
		while (length > 0) {
			USART_SendByte(USARTx, *data_arr++);
			i++;
			length--;
		}
		Delay(0x1fff);
		RS485_Mode_RX();
	}
	else {
		while (length > 0) {
			USART_SendByte(USARTx, *data_arr++);
			i++;
			length--;
		}
	}
	
	return i;
}

/***************************************************************
 @Function			void UARTx_IRQn_Interrupt(USART_TypeDef* USARTx)
 @Description			串口中断处理函数
 @Input				USARTx	: 串口号
 @Return				void
***************************************************************/
void UARTx_IRQn_Interrupt(USART_TypeDef* USARTx)
{	
	/* 发送中断 */
	if (USART_GetITStatus(USARTx, USART_IT_TXE))
	{
		/* 串口1发送中断处理函数 */
		if (USARTx == USART1) {
			
		}
		/* 串口2发送中断处理函数 */
		else if (USARTx == USART2) {
			
		}
		/* 串口3发送中断处理函数 */
		else if (USARTx == USART3) {
			
		}
		
		USART_ClearITPendingBit(USARTx, USART_IT_TXE);				//清发送中断标志位
	}
	/* 接收中断 */
	else if (USART_GetITStatus(USARTx, USART_IT_RXNE))
	{
		/* 串口1接收中断处理函数 */
		if (USARTx == USART1) {

		}
		/* 串口2接收中断处理函数 */
		else if (USARTx == USART2) {
			
		}
		/* 串口3接收中断处理函数 */
		else if (USARTx == USART3) {
			
		}
		
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);					//清接收中断标志位
	}
}

#if 1
#pragma import(__use_no_semihosting)
struct __FILE
{
	int handle;
};
FILE __stdout;
void _sys_exit(int x)
{ 
	x = x;
}
/***************************************************************
 @Function			int fputc(int ch, FILE *f)
 @Description			重定向C库函数printf到USARTx
 @Input				void
 @Return				void
***************************************************************/
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到 USARTx */
	USART_SendData(PRINTF_USART, (uint8_t) ch);
	
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(PRINTF_USART, USART_FLAG_TXE) == RESET);
	
	return (ch);
}

/***************************************************************
 @Function			int fgetc(FILE *f)
 @Description			重定向C库函数Scanf到USARTx
 @Input				void
 @Return				void
***************************************************************/
int fgetc(FILE *f)
{
	/* 等待 USARTx 输入数据 */
	while (USART_GetFlagStatus(PRINTF_USART, USART_FLAG_RXNE) == RESET);
	
	return (int)USART_ReceiveData(PRINTF_USART);
}
#endif

/******************************** END OF FLEE **********************************/
